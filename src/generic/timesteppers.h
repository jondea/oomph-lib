//LIC// ====================================================================
//LIC// This file forms part of oomph-lib, the object-oriented, 
//LIC// multi-physics finite-element library, available 
//LIC// at http://www.oomph-lib.org.
//LIC// 
//LIC//           Version 0.90. August 3, 2009.
//LIC// 
//LIC// Copyright (C) 2006-2009 Matthias Heil and Andrew Hazel
//LIC// 
//LIC// This library is free software; you can redistribute it and/or
//LIC// modify it under the terms of the GNU Lesser General Public
//LIC// License as published by the Free Software Foundation; either
//LIC// version 2.1 of the License, or (at your option) any later version.
//LIC// 
//LIC// This library is distributed in the hope that it will be useful,
//LIC// but WITHOUT ANY WARRANTY; without even the implied warranty of
//LIC// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//LIC// Lesser General Public License for more details.
//LIC// 
//LIC// You should have received a copy of the GNU Lesser General Public
//LIC// License along with this library; if not, write to the Free Software
//LIC// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
//LIC// 02110-1301  USA.
//LIC// 
//LIC// The authors may be contacted at oomph-lib@maths.man.ac.uk.
//LIC// 
//LIC//====================================================================
//This header contains classes and function prototypes for the Time, 
//TimeStepper and derived objects

//Include guard to prevent multiple inclusions of the header
#ifndef OOMPH_TIME_STEPPERS_HEADER
#define OOMPH_TIME_STEPPERS_HEADER

// Config header generated by autoconfig
#ifdef HAVE_CONFIG_H
  #include <oomph-lib-config.h>
#endif

//oomph-lib headers
#include "Vector.h"
#include "nodes.h"
#include "matrices.h"

namespace oomph
{

//====================================================================
/// \short Class to keep track of discrete/continous time. It is essential
/// to have a single Time object when using multiple time-stepping schemes; 
/// e.g., in fluid-structure interaction problems, it is common to use 
/// different schemes for the fluid and solid domains.
/// Storage is allocated for the current value
/// of the (continuous) time and a limited history of previous timesteps.
/// The number of previous timesteps must be equal to the number required
/// by the "highest order" scheme.
//====================================================================
class Time
{
  private:
 
 /// Pointer to the value of the continuous time.
 double* Time_pt;

 /// Vector that stores the values of the current and previous timesteps.
 Vector<double> Dt;

  public:

 /// \short Constructor: Do not allocate any storage for previous timesteps,
 /// but set the initial value of the time to zero
 Time() {Time_pt = new double(0.0);}

 /// \short Constructor: Pass the number of timesteps to be stored
 /// and set the initial value of time to zero.
 Time(const unsigned &ndt)
  {
   //Allocate and initialise the value of the continuous time
   Time_pt=new double(0.0);
   // Allocate memory for the timestep storage and initialise values to one
   // to avoid division by zero.
   Dt.resize(ndt,1.0);
  }

 /// Broken copy constructor
 Time(const Time&) 
  { 
   BrokenCopy::broken_copy("Time");
  } 
 
 /// Broken assignment operator
 void operator=(const Time&) 
  {
   BrokenCopy::broken_assign("Time");
  }

 /// \short Resize the vector holding the number of previous timesteps
 /// and initialise the new values to zero.
 void resize(const unsigned &n_dt) {Dt.resize(n_dt,0.0);}

 /// \short Set all timesteps to the same value, dt.
 void initialise_dt(const double& dt_)
  {
   unsigned n_dt=Dt.size();
   for (unsigned i=0;i<n_dt;i++) {Dt[i]=dt_;}
  }

 /// \short Set the value of the timesteps to be equal to the values passed in 
 /// a vector 
 void initialise_dt(const Vector<double>& dt_)
  {
   //Assign the values from the vector
   //By using the size of the passed vector, we can handle the case when
   //the size of passed vector is smaller than that amount of storage allocated
   //in the object.
   unsigned n_dt=dt_.size();
   for (unsigned i=0;i<n_dt;i++) {Dt[i]=dt_[i];}
  }

 /// Destructor: Deallocate the memory allocated in the Object
 ~Time(){delete Time_pt;}

 /// Return the current value of the continuous time
 double& time(){return *Time_pt;}

 /// Return the number of timesteps stored
 unsigned ndt() {return Dt.size();}

 /// Return the value of the current timestep
 double& dt(){return Dt[0];}

 /// Return the value of the t-th stored timestep (t=0: present; t>0; previous)
 double& dt(const unsigned &t) {return Dt[t];}

 /// \short Return the value of the continuous time at the t-th previous 
 /// time level (t=0: current; t>0 previous)
 double time(const unsigned &t)
  {
   //Load the current value of the time
   double time_local = *Time_pt;
   //Loop over the t previous timesteps and subtract each dt
   for (unsigned i=0;i<t;i++) {time_local -= Dt[i];}
   return time_local;
  }

 /// \short Update all stored values of dt by shifting each value along 
 /// the array. This function must be called before starting to solve at a 
 /// new time level.
  void shift_dt()
  {
   unsigned n_dt=Dt.size();
   //Return straight away if there are no stored timesteps
   if(n_dt == 0) {return;}
   //Start from the end of the array and shift every entry back by one
   for (unsigned i=(n_dt-1);i>0;i--) {Dt[i]=Dt[i-1];}
  }

};



/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////



//====================================================================
/// \short Base class for time-stepping schemes.
/// Timestepper provides an approximation of the temporal derivatives of 
/// Data such that the i-th derivative of the j-th value in Data is 
/// represented as
/// \code
/// unsigned n_value=data_pt->nvalue();
/// Vector<double> time_derivative(n_value);
/// for (unsigned j=0;j<n_value;j++)
///  {
///  time_derivative[j]=0.0;
///  for (unsigned t=0;t<ntstorage();t++)
///   {
///    time_derivative[j] += data_pt->value(t,j)*weight(i,t);
///   }
///  }
/// \endcode
/// where the time index \c t is such that
/// - \c t \c = \c 0 refers to the current value of the Data
/// - \c t \c > \c 0 refers to the `history' values, i.e. the doubles that 
///   are stored in Data to represent the value's time derivatives. For BDF
///   schemes these doubles are the values at previous times;
///   for other timestepping schemes they can represent quantities
///   such as previous first and second derivatives, etc.
/// 
/// TimeSteppers can evaluate all derivatives up to their \c order().
/// 
/// The first \c nprev_values() `history values' represent the
/// values at the previous timesteps. For BDF schemes,
/// \c nprev_values() \c = \c  ntstorage()-1 , i.e. all `history values'
/// represent previous values. For \c Newmark<NSTEPS>, only the first
/// NSTEPS `history values' represent previous values (NSTEPS=1
/// gives the normal Newmark scheme).
//====================================================================
class TimeStepper
{
 protected:

 /// Pointer to discrete time storage scheme
 Time* Time_pt;

 /// Storage for the weights associated with the timestepper
 DenseMatrix<double> Weight;

 /// \short String that indicates the type of the timestepper 
 ///(e.g. "BDF", "Newmark", etc.)
 std::string Type;

 /// \short Boolean variable to indicate whether the timestepping scheme can be
 /// adaptive
 bool Adaptive_Flag;

 /// \short Bool to indicate if the timestepper is steady, i.e. its 
 /// time-derivatives evaluate to zero. This status may be
 /// achieved temporarily by calling make_steady(). It's
 /// reset to the appropriate default by the pure virtual function
 /// undo_make_steady() implemented in all specific TimeSteppers
 bool Is_steady;

public:
 
 /// \short Constructor. Pass the amount of storage required by
 /// timestepper (present value + history values) and the 
 /// order of highest time-derivative.
 TimeStepper(const unsigned &tstorage, const unsigned &max_deriv) 
  : Time_pt(0), Adaptive_Flag(false), Is_steady(false)
  {
   //Resize Weights matrix and initialise each weight to zero 
   Weight.resize(max_deriv+1,tstorage,0.0);

/*    Weight = new double*[first idex]; */
/*    double* temp = new double[first_index*second_index]; */
/*    for (unsigned i=0;i<first_index;i++) */
/*     { */
/*      Weight[i]= temp; */
/*      temp+=second_index; */
/*     } */

   // in destructor
   // delete [] Weight[0];
   // delete [] Weight;

   // Set the weight for zero-th derivative, which is always 1.0
   Weight(0,0)=1.0;
  }

 /// Broken empty constructor
 TimeStepper()
  { 
   throw OomphLibError("Don't call default constructor for TimeStepper!",
                       "TimeStepper::TimeStepper()",
                       OOMPH_EXCEPTION_LOCATION);
  } 

 /// Broken copy constructor
 TimeStepper(const TimeStepper&) 
  { 
   BrokenCopy::broken_copy("TimeStepper");
  } 
 
 /// Broken assignment operator
 void operator=(const TimeStepper&) 
  {
   BrokenCopy::broken_assign("TimeStepper");
  } 

 /// Empty virtual destructor --- no memory is allocated in this class
 virtual ~TimeStepper(){}

 /// Highest order derivative that the scheme can compute
 unsigned highest_derivative() const
 {return Weight.nrow()-1;} 

 /// Actual order (accuracy) of the scheme
 virtual unsigned order() {return 0;}

 /// Return current value of continous time
 double& time(){return Time_pt->time();}

 /// Number of timestep increments that are required by the scheme
 virtual unsigned ndt()=0;

 /// Number of previous values available: 0 for static, 1 for BDF<1>,...
 virtual unsigned nprev_values()=0;

 /// \short Function to set the weights for present timestep (don't
 /// need to pass present timestep or previous timesteps as they 
 /// are available via Time_pt)
 virtual void set_weights()=0;

 /// \short Function to make the time stepper temporarily steady. This
 /// is trivially achieved by setting all the weights to zero
 void make_steady()
  {
   //Find out dimensions of the weights matrix
   unsigned n_rows = Weight.nrow();
   unsigned n_tstorage = ntstorage();
   //Loop over all the non-zeroth order entries and zero the weights
   for(unsigned i=1;i<n_rows;i++)
    {
     for(unsigned j=0;j<n_tstorage;j++) {Weight(i,j) = 0.0;}
    }
   // Update flag
   Is_steady=true;
  }

 /// \short Flag to indicate if a timestepper has been made steady (possibly
 /// temporarily to switch off time-dependence)
 bool is_steady()
  {
   return Is_steady;
  }
 
 /// \short Pure virtual function to reset the is_steady status
 /// of a specific TimeStepper to its default and to re-assign
 /// the weights.
 virtual void undo_make_steady()=0;

 /// \short Return string that indicates the type of the timestepper 
 /// (e.g. "BDF", "Newmark", etc.)
 std::string type() {return Type;}

 /// \short Evaluate i-th derivative of all values in Data and return in 
 /// Vector deriv[]
 void time_derivative(const unsigned &i, 
                      Data* const &data_pt, Vector<double>& deriv)
  {
   // Number of values stored in the Data object
   unsigned nvalue=data_pt->nvalue();
   unsigned n_tstorage = ntstorage();
   // Loop over all values
   for (unsigned j=0;j<nvalue;j++)
    {
     double aux_deriv=0.0;
     // Loop over all history data and add the approriate contribution
     // to the derivative
     for (unsigned t=0;t<n_tstorage;t++)
      {
       aux_deriv+=Weight(i,t)*data_pt->value(t,j);
      }
     deriv[j]=aux_deriv;
    }
  }

 /// \short Evaluate i-th derivative of j-th value in Data
 double time_derivative(const unsigned &i, Data* const &data_pt, 
                        const unsigned& j)
  {
   double deriv=0.0;
   unsigned n_tstorage = ntstorage();
   // Loop over all history data and add the appropriate contribution
   // to the derivative
   for (unsigned t=0;t<n_tstorage;t++)
    {
     deriv+=Weight(i,t)*data_pt->value(t,j);
    }
   return deriv;
  }

 /// \short Evaluate i-th derivative of all values in Node and return in 
 /// Vector deriv[]. Note the use of the nodal_value() function so that
 /// hanging nodes are taken into account
 void time_derivative(const unsigned &i, 
                      Node* const &node_pt, Vector<double>& deriv)
  {
   // Number of values stored in the Data object
   unsigned nvalue = node_pt->nvalue();
   unsigned n_tstorage = ntstorage();
   // Loop over all values
   for (unsigned j=0;j<nvalue;j++)
    {
     double aux_deriv=0.0;
     // Loop over all history data and add the approriate contribution
     // to the derivative
     for (unsigned t=0;t<n_tstorage;t++)
      {
       aux_deriv+=weight(i,t)*node_pt->value(t,j);
      }
     deriv[j]=aux_deriv;
    }
  }

 /// \short Evaluate i-th derivative of j-th value in Node. Note the
 /// explicit use of nodal_value() so that hanging nodes are properly
 /// evaluated
 double time_derivative(const unsigned &i, Node* const &node_pt, 
                        const unsigned& j)
  {
   double deriv=0.0;
   unsigned n_tstorage = ntstorage();
   // Loop over all history data and add the appropriate contribution
   // to the derivative
   for (unsigned t=0;t<n_tstorage;t++)
    {
     deriv+=weight(i,t)*node_pt->value(t,j);
    }
   return deriv;
  }


 ///Access function for the pointer to time (const version)
 Time* const &time_pt() const {return Time_pt;}

 ///Access function for the pointer to time
 Time*& time_pt() {return Time_pt;}

 /// Access function for j-th weight for the i-th derivative
 virtual double weight(const unsigned &i, const unsigned &j) const
  {return Weight(i,j);}

 /// \short Return the number of doubles required to represent history
 /// (one for steady)
 unsigned ntstorage() const 
  {return (Weight.ncol());}

 /// \short Initialise the time-history for the Data values
 /// corresponding to an impulsive start.
 virtual void assign_initial_values_impulsive(Data* const &data_pt)=0;
 
 /// \short Initialiset the positions for the node corresponding to 
 /// an impulsive start
 virtual void assign_initial_positions_impulsive(Node* const &node_pt)=0;

 /// \short This function advances the Data's time history so that
 /// we can move on to the next timestep
 virtual void shift_time_values(Data* const &data_pt)=0;

 ///\short This function advances the time history of the positions
 ///at a node. The default should be OK, but would need to be overloaded
 virtual void shift_time_positions(Node* const &node_pt)=0;
  
 /// Function to indicate whether the scheme is adaptive (false by default)
 bool adaptive_flag() {return Adaptive_Flag;}

 /// \short Set the weights for the predictor 
 /// previous timestep (currently empty -- overwrite for specific scheme)
 virtual void set_predictor_weights() {}
 
 /// \short Do the predictor step for data stored in a Data object
 /// (currently empty -- overwrite for specific scheme)
 virtual void calculate_predicted_values(Data* const &data_pt) {}
 
 ///\short Do the predictor step for the positions at a node
 /// (currently empty --- overwrite for a specific scheme)
 virtual void calculate_predicted_positions(Node* const &node_pt) {}

 /// \short Set the weights for the error computation, 
 /// (currently empty -- overwrite for specific scheme)
 virtual void set_error_weights() {}

 /// Compute the error in the position i at a node 
 /// zero here -- overwrite for specific scheme.
 virtual double temporal_error_in_position(Node* const &node_pt, 
                                           const unsigned &i)
  {return 0.0;}

 /// Compute the error in the value i in a Data structure
 /// zero here -- overwrite for specific scheme.
 virtual double temporal_error_in_value(Data* const &data_pt, 
                                        const unsigned &i) 
  {return 0.0;}
};


/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////



//====================================================================
/// Faux time-stepper for steady problems. Allows storage
/// for NSTEPS previous values.
//====================================================================
template<unsigned NSTEPS>
class Steady : virtual public TimeStepper
{
 
public:
 
 /// \short Constructor: Creates storage for NSTEPS previous timesteps
 /// and can evaluate up to 2nd derivatives (though it doesn't
 /// actually do anything -- all time-derivatives
 /// evaluate to zero)
 Steady() : TimeStepper(NSTEPS+1,2)
  {
   Type="Steady";

   //Overwrite default assignment in base constructor -- this TimeStepper
   //actually is steady all the time.
   Is_steady=true;
  }
 

 /// Broken copy constructor
 Steady(const Steady&) 
  { 
   BrokenCopy::broken_copy("Steady");
  } 
 
 /// Broken assignment operator
 void operator=(const Steady&) 
  {
   BrokenCopy::broken_assign("Steady");
  }

 /// \short Return the actual order of the scheme. Returning zero here --
 /// doesn't make much sense, though
 unsigned order() {return 0;}
   
 /// \short Reset the is_steady status to its default (true) and
 /// re-compute the weights
 void undo_make_steady() 
  {
   Is_steady=true;
   set_weights();
  }

 /// \short  Initialise the time-history for the Data values,
 /// corresponding to an impulsive start.
 void assign_initial_values_impulsive(Data* const &data_pt)
  {
   //Find number of values stored
   unsigned n_value = data_pt->nvalue();
   //Loop over values
   for(unsigned j=0;j<n_value;j++)
    {
     //Set previous values to the initial value, if not a copy
     if(data_pt->is_a_copy(j) == false)
      {
       for(unsigned t=1;t<=NSTEPS;t++)
        {
         data_pt->set_value(t,j,data_pt->value(j));
        }
      }
    }
  }

 /// \short  Initialise the time-history for the nodal positions
 /// corresponding to an impulsive start.
 void assign_initial_positions_impulsive(Node* const &node_pt)
  {
   //Find the number of coordinates
   unsigned n_dim = node_pt->ndim();
   //Find the number of positoin types
   unsigned n_position_type = node_pt->nposition_type();

   //Loop over values
   for(unsigned i=0;i<n_dim;i++)
    {
     //Set previous values to the initial value, if not a copy
     if(node_pt->position_is_a_copy(i) == false)
      {
       for(unsigned k=0;k<n_position_type;k++)
        {
         for(unsigned t=1;t<=NSTEPS;t++)
          {
           node_pt->x_gen(t,k,i) = node_pt->x_gen(k,i);
          }
        }
      }
    }
  }


 /// \short Typedef for function that returns the (scalar) initial
 /// value at a given value of the continuous time t.
 typedef double (*InitialConditionFctPt)(const double& t);

 /// \short  Initialise the time-history for the Data values,
 /// corresponding to given time history, specified by
 /// Vector of function pointers.
 void assign_initial_data_values(Data* const &data_pt, 
                                 Vector<InitialConditionFctPt> 
                                 initial_value_fct)
  {
   // The time history stores the previous function values
   unsigned n_time_value = ntstorage();

   //Find number of values stored
   unsigned n_value = data_pt->nvalue();

   //Loop over current and stored timesteps
   for(unsigned t=0;t<n_time_value;t++)
    {
     // Get corresponding continous time
     double time_local=Time_pt->time(t);

     //Loop over values
     for(unsigned j=0;j<n_value;j++)
      {
       data_pt->set_value(t,j,initial_value_fct[j](time_local));
      }
    }
  }
 
 /// \short This function updates the Data's time history so that
 /// we can advance to the next timestep. As for BDF schemes,
 /// we simply push the values backwards...
 void shift_time_values(Data* const &data_pt)
  {
   //Find number of values stored
   unsigned n_value = data_pt->nvalue();
   
   //Loop over the values
   for(unsigned j=0;j<n_value;j++)
    {
     //Set previous values to the previous value, if not a copy
     if(data_pt->is_a_copy(j) == false)
      {
       //Loop over times, in reverse order
       for(unsigned t=NSTEPS;t>0;t--)
        {
         data_pt->set_value(t,j,data_pt->value(t-1,j));
        }
      } 
    }
  }
 
 ///\short This function advances the time history of the positions
 ///at a node. 
 void shift_time_positions(Node* const &node_pt)
  {
   //Find the number of coordinates
   unsigned n_dim = node_pt->ndim();
   //Find the number of position types
   unsigned n_position_type = node_pt->nposition_type();
      
   //Loop over the positions
   for(unsigned i=0;i<n_dim;i++)
    {
     //If the position is not a copy
     if(node_pt->position_is_a_copy(i) == false)
      {
       for(unsigned k=0;k<n_position_type;k++)
        {
         //Loop over stored times, and set values to previous values
         for(unsigned t=NSTEPS;t>0;t--)
          {
           node_pt->x_gen(t,k,i) = node_pt->x_gen(t-1,k,i);
          }
        }
      }
    }
  }
 
 ///Set weights 
 void set_weights()
  {
   // Loop over higher derivatives
   unsigned max_deriv=highest_derivative();
   for (unsigned i=0;i<max_deriv;i++)
    {
     for (unsigned j=0;j<NSTEPS;j++)
      {
       Weight(i,j) = 0.0;
      }
    }
   
   // Zeroth derivative must return the value itself:
   Weight(0,0)=1.0;
  }

 /// Number of previous values available.
 unsigned nprev_values(){return NSTEPS;}

 /// Number of timestep increments that need to be stored by the scheme
 unsigned ndt() {return NSTEPS;}

/*  /// Dummy: Access function for i-th weight (for the highest derivative) */
/*  double weight(const unsigned &i) {return Zero;} */

 /// Dummy: Access function for j-th weight for the i-th derivative
 double weight(const unsigned &i, const unsigned &j) const
  {
   if ((i==0)&&(j==0))
    {
     return One;
    }
   else 
    {
     return Zero;
    }
  }

private:
 
 /// Static variable to hold the value 1.0
 static double One;
 
 /// Static variable to hold the value 0.0
 static double Zero;
 
 // Default Time object
 static Time* Default_time_pt;
 
};




/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////



//====================================================================
/// \short Newmark scheme for second time deriv. Stored data represents
/// - t=0: value at at present time, Time_pt->time()
/// - t=1: value at previous time, Time_pt->time()-dt
/// -  ...
/// - t=NSTEPS:   value at previous time, Time_pt->time()-NSTEPS*dt
/// - t=NSTEPS+1: 1st time deriv (= "velocity") at previous time,
///               Time_pt->time()-dt
/// - t=NSTEPS+2: 2nd time deriv (= "acceleration") at previous time, 
///                Time_pt->time()-dt
///
/// NSTEPS=1 gives normal Newmark.
//====================================================================
template<unsigned NSTEPS>
class Newmark : public TimeStepper
{
 
  public:
 
 /// \short Constructor: Pass pointer to global time. We set up a 
 /// timestepping scheme with NSTEPS+2 doubles to represent the history and
 /// the highest deriv is 2.
 Newmark() : TimeStepper(NSTEPS+3,2)
  {
   Type="Newmark";

   // Standard Newmark parameters
   Beta1=0.5;
   Beta2=0.5;
  }

 /// Broken copy constructor
 Newmark(const Newmark&) 
  { 
   BrokenCopy::broken_copy("Newmark");
  } 
 
 /// Broken assignment operator
 void operator=(const Newmark&) 
  {
   BrokenCopy::broken_assign("Newmark");
  }


 ///The actual order (accuracy of the scheme)
 unsigned order() 
  {
   std::string error_message =
    "Can't remember the order of the Newmark scheme";
   error_message += " -- I think it's 2nd order...\n";

   OomphLibWarning(error_message,"Newmark::order()",
                   OOMPH_EXCEPTION_LOCATION);
   return 2;
  }


 /// \short Reset the is_steady status to its default (false) and
 /// re-compute the weights
 void undo_make_steady() 
  {
   Is_steady=false;
   set_weights();
  }


 /// \short Initialise the time-history for the values,
 /// corresponding to an impulsive start.
 void assign_initial_values_impulsive(Data* const &data_pt);

 /// \short Initialise the time-history for the values,
 /// corresponding to an impulsive start.
 void assign_initial_positions_impulsive(Node* const &node_pt);

 /// \short Typedef for function that returns the (scalar) initial
 /// value at a given value of the continuous time t.
 typedef double (*InitialConditionFctPt)(const double& t);

 /// \short  Initialise the time-history for the Data values,
 /// so that the Newmark representations for current veloc and
 /// acceleration are exact.
 void assign_initial_data_values(Data* const & data_pt, 
                                 Vector<InitialConditionFctPt>
                                 initial_value_fct,
                                 Vector<InitialConditionFctPt>
                                 initial_veloc_fct,
                                 Vector<InitialConditionFctPt>
                                 initial_accel_fct);


 /// \short Typedef for function that returns the (scalar) initial
 /// value at a given value of the continuous time t and the spatial
 /// coordinate -- appropriate for assignement of initial conditions for
 /// nodes
 typedef double (*NodeInitialConditionFctPt)(const double& t, 
                                             const Vector<double>& x);

 /// \short  Initialise the time-history for the nodal values,
 /// so that the Newmark representations for current veloc and
 /// acceleration are exact.
 void assign_initial_data_values(Node* const & node_pt, 
                                 Vector<NodeInitialConditionFctPt>
                                 initial_value_fct,
                                 Vector<NodeInitialConditionFctPt>
                                 initial_veloc_fct,
                                 Vector<NodeInitialConditionFctPt>
                                 initial_accel_fct);

 /// \short  First step in a two-stage procedure to assign
 /// the history values for the Newmark scheme so 
 /// that the veloc and accel that are computed by the scheme
 /// are correct at the current time.
 /// 
 /// Call this function for t_deriv=0,1,2,3.
 /// When calling with
 /// - t_deriv=0 :  data_pt(0,*) should contain the values at the
 ///                previous timestep
 /// - t_deriv=1 :  data_pt(0,*) should contain the current values;
 ///                they get stored (temporarily) in data_pt(1,*).
 /// - t_deriv=2 :  data_pt(0,*) should contain the current velocities
 ///                (first time derivs); they get stored (temporarily) 
 ///                in data_pt(NSTEPS+1,*).
 /// - t_deriv=3 :  data_pt(0,*) should contain the current accelerations
 ///                (second time derivs); they get stored (temporarily) 
 ///                in data_pt(NSTEPS+2,*).
 /// .
 /// Follow this by calls to 
 /// \code
 /// assign_initial_data_values_stage2(...)
 /// \endcode
 void assign_initial_data_values_stage1(const unsigned t_deriv, 
                                        Data* const &data_pt);

 /// \short Second step in a two-stage procedure to assign
 /// the history values for the Newmark scheme so 
 /// that the veloc and accel that are computed by the scheme
 /// are correct at the current time.
 /// 
 /// This assigns appropriate values for the "previous 
 /// velocities and accelerations" so that their current
 /// values, which were defined in assign_initial_data_values_stage1(...),
 /// are represented exactly by the Newmark scheme. 
 void assign_initial_data_values_stage2(Data* const &data_pt);


 /// \short This function updates the Data's time history so that
 /// we can advance to the next timestep.
 void shift_time_values(Data* const &data_pt);

 /// \short This function updates a nodal time history so that
 /// we can advance to the next timestep.
 void shift_time_positions(Node* const &node_pt);

 ///Set weights 
 void set_weights();

 /// Number of previous values available.
 unsigned nprev_values(){return NSTEPS;}

 /// Number of timestep increments that need to be stored by the scheme
 unsigned ndt() {return NSTEPS;}

 
  protected:
 
 /// First Newmark parameter (usually 0.5)
 double Beta1;
 
 /// Second Newmark parameter (usually 0.5)
 double Beta2;

};


/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////



//====================================================================
/// \short Newmark scheme for second time deriv with first derivatives
/// calculated using BDF. . Stored data represents
/// - t=0: value at at present time, Time_pt->time()
/// - t=1: value at previous time, Time_pt->time()-dt
/// -  ...
/// - t=NSTEPS:   value at previous time, Time_pt->time()-NSTEPS*dt
/// - t=NSTEPS+1: 1st time deriv (= "velocity") at previous time,
///               Time_pt->time()-dt
/// - t=NSTEPS+2: 2nd time deriv (= "acceleration") at previous time, 
///                Time_pt->time()-dt
///
/// NSTEPS=1 gives normal Newmark.
//====================================================================
template<unsigned NSTEPS>
class NewmarkBDF : public Newmark<NSTEPS>
{
  public:
 
 /// \short Constructor: Pass pointer to global time. We set up a 
 /// timestepping scheme with NSTEPS+2 doubles to represent the history and
 /// the highest deriv is 2.
 NewmarkBDF()
  {
   this->Type="NewmarkBDF";
  }

 /// Broken copy constructor
 NewmarkBDF(const NewmarkBDF&) 
  { 
   BrokenCopy::broken_copy("NewmarkBDF");
  } 
 
 /// Broken assignment operator
 void operator=(const NewmarkBDF&) 
  {
   BrokenCopy::broken_assign("NewmarkBDF");
  }

 ///Set weights 
 void set_weights();

};



/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////



//====================================================================
/// \short Templated class for BDF-type time-steppers with fixed or 
/// variable timestep. 
/// 1st time derivative recovered directly from the previous function
/// values. Template parameter represents the number of previous timesteps
/// stored, so that BDF<1> is the classical first order
/// backward Euler scheme. 
/// Need to reset weights after every change in timestep.
//====================================================================
template<unsigned NSTEPS>
class BDF : public TimeStepper
{
  private:
 
 ///Private data for the predictor weights
 Vector<double> Predictor_weight;
 
 /// Private data for the error weight 
 double Error_weight;
 
  public:

 ///Constructor for the case when we allow adaptive timestepping
 BDF(bool adaptive=false) : TimeStepper(NSTEPS+1,1)
  {
   Type="BDF";

   //If it's adaptive, we need to allocate additional space to 
   //carry along a prediction and an acceleration
   if(adaptive)
    {
     //Set the adaptive flag to be true
     Adaptive_Flag = true;
     
     //Set the size of the Predictor_Weights Vector
     //N.B. The size is correct for BDF<2>, but may be wrong for others
     Predictor_weight.resize(NSTEPS+2);
     
     //The order of the scheme is 1, i.e. Weights has two entries

     //Resize the weights to the appropriate size
     Weight.resize(2,NSTEPS+3);

     // Initialise
     for(unsigned i=0;i<2;i++)
      {
       //Initialise each weight to zero
       for(unsigned j=0;j<NSTEPS+3;j++)
        {
         Weight(i,j) = 0.0;
        }
      }
     //Set the weight for the zero-th derivative
     Weight(0,0) = 1.0;
    }
  }


 /// Broken copy constructor
 BDF(const BDF&) 
  { 
   BrokenCopy::broken_copy("BDF");
  } 
 
 /// Broken assignment operator
 void operator=(const BDF&) 
  {
   BrokenCopy::broken_assign("BDF");
  }

 ///Return the actual order of the scheme
 unsigned order() {return NSTEPS;}
   

 /// \short Reset the is_steady status to its default (false) and
 /// re-compute the weights
 void undo_make_steady() 
  {
   Is_steady=false;
   set_weights();
  }

 /// \short  Initialise the time-history for the Data values,
 /// corresponding to an impulsive start.
 void assign_initial_values_impulsive(Data* const &data_pt)
  {
   //Find number of values stored
   unsigned n_value = data_pt->nvalue();
   //Loop over values
   for(unsigned j=0;j<n_value;j++)
    {
     //Set previous values to the initial value, if not a copy
     if(data_pt->is_a_copy(j) == false)
      {
       for(unsigned t=1;t<=NSTEPS;t++)
        {
         data_pt->set_value(t,j,data_pt->value(j));
        }
       
       //If it's adaptive 
       if(adaptive_flag())
        {
         //Initial velocity is zero
         data_pt->set_value(NSTEPS+1,j,0.0);
         //Initial prediction is the value
         data_pt->set_value(NSTEPS+2,j,data_pt->value(j));
        }
      }
    }
  }

 /// \short  Initialise the time-history for the nodal positions
 /// corresponding to an impulsive start.
 void assign_initial_positions_impulsive(Node* const &node_pt)
  {
   //Find the dimension of the node
   unsigned n_dim = node_pt->ndim();
   //Find the number of position types at the node
   unsigned n_position_type = node_pt->nposition_type();

   //Loop over the position variables
   for(unsigned i=0;i<n_dim;i++)
    {
     //If the position is not copied 
     //We copy entire coordinates at once
     if(node_pt->position_is_a_copy(i) == false)
      {
       //Loop over the position types
       for(unsigned k=0;k<n_position_type;k++)
        {
         //Set previous values to the initial value, if not a copy
         for(unsigned t=1;t<=NSTEPS;t++)
          {
           node_pt->x_gen(t,k,i) = node_pt->x_gen(k,i);
          }
         
         //If it's adaptive 
         if(adaptive_flag())
          {
           //Initial mesh velocity is zero
           node_pt->x_gen(NSTEPS+1,k,i) = 0.0;
           //Initial prediction is the value
           node_pt->x_gen(NSTEPS+2,k,i) = node_pt->x_gen(k,i);
          }
        }
      }
    }
  }


 /// \short Typedef for function that returns the (scalar) initial
 /// value at a given value of the continuous time t.
 typedef double (*InitialConditionFctPt)(const double& t);

 /// \short  Initialise the time-history for the Data values,
 /// corresponding to given time history, specified by
 /// Vector of function pointers.
 void assign_initial_data_values(Data* const &data_pt, 
                                 Vector<InitialConditionFctPt> 
                                 initial_value_fct)
  {
   // The time history stores the previous function values
   unsigned n_time_value = ntstorage();

   //Find number of values stored
   unsigned n_value = data_pt->nvalue();

   //Loop over current and stored timesteps
   for(unsigned t=0;t<n_time_value;t++)
    {

     // Get corresponding continous time
     double time_local=Time_pt->time(t);

     //Loop over values
     for(unsigned j=0;j<n_value;j++)
      {
       data_pt->set_value(t,j,initial_value_fct[j](time_local));
      }
    }
  }

 /// \short This function updates the Data's time history so that
 /// we can advance to the next timestep. For BDF schemes,
 /// we simply push the values backwards...
 void shift_time_values(Data* const &data_pt)
  {
   //Find number of values stored
   unsigned n_value = data_pt->nvalue();
   //Storage for velocity need to be here to be in scope
   Vector<double> velocity(n_value);

   //If adaptive, find the velocities
   if(adaptive_flag()) {time_derivative(1,data_pt,velocity);}
   
   //Loop over the values
   for(unsigned j=0;j<n_value;j++)
    {
     //Set previous values to the previous value, if not a copy
     if(data_pt->is_a_copy(j) == false)
      {
       //Loop over times, in reverse order
       for(unsigned t=NSTEPS;t>0;t--)
        {
         data_pt->set_value(t,j,data_pt->value(t-1,j));
        }
       
       //If we are using the adaptive scheme
       if(adaptive_flag())
        {
         //Set the velocity
         data_pt->set_value(NSTEPS+1,j,velocity[j]);
        }
      } 
    }
  }

 ///\short This function advances the time history of the positions
 ///at a node. 
 void shift_time_positions(Node* const &node_pt)
  {
   //Find the number of coordinates
   unsigned n_dim = node_pt->ndim();
   //Find the number of position types
   unsigned n_position_type = node_pt->nposition_type();

   //Find number of stored timesteps
   unsigned n_tstorage = ntstorage();

   //Storage for the velocity
   double velocity[n_position_type][n_dim];
     
   //If adaptive, find the velocities
   if(adaptive_flag())
    {
     //Loop over the variables
     for(unsigned i=0;i<n_dim;i++)
      {
       for(unsigned k=0;k<n_position_type;k++)
        {
         //Initialise velocity to zero
         velocity[k][i] =0.0;
         //Loop over all history data
         for(unsigned t=0;t<n_tstorage;t++)
          {
           velocity[k][i] += Weight(1,t)*node_pt->x_gen(t,k,i);
          }
        }
      }
    }
   
   //Loop over the positions
   for(unsigned i=0;i<n_dim;i++)
    {
     //If the position is not a copy
     if(node_pt->position_is_a_copy(i) == false)
      {
       //Loop over the position types
       for(unsigned k=0;k<n_position_type;k++)
        {
         //Loop over stored times, and set values to previous values
         for(unsigned t=NSTEPS;t>0;t--)
          {
           node_pt->x_gen(t,k,i) = node_pt->x_gen(t-1,k,i);
          }
       
         //If we are using the adaptive scheme, set the velocity
         if(adaptive_flag())
          {
           node_pt->x_gen(NSTEPS+1,k,i) = velocity[k][i];
          }
        }
      }
    }
  }

 /// Set the weights 
 void set_weights();

 /// Number of previous values available.
 unsigned nprev_values(){return NSTEPS;}

 /// Number of timestep increments that need to be stored by the scheme
 unsigned ndt() {return NSTEPS;}

 /// Function to set the predictor weights
 void set_predictor_weights();

 ///Function to calculate predicted positions at a node
 void calculate_predicted_positions(Node* const &node_pt);

 ///Function to calculate predicted data values in a Data object
 void calculate_predicted_values(Data* const &data_pt);

 ///Function to set the error weights
 void set_error_weights();
 
 /// Compute the error in the position i at a node 
 double temporal_error_in_position(Node* const &node_pt, 
                                   const unsigned &i);

 /// Compute the error in the value i in a Data structure
 double temporal_error_in_value(Data* const &data_pt, 
                                const unsigned &i);

};

}

#endif
