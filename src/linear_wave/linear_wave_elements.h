//LIC// ====================================================================
//LIC// This file forms part of oomph-lib, the object-oriented, 
//LIC// multi-physics finite-element library, available 
//LIC// at http://www.oomph-lib.org.
//LIC// 
//LIC// Copyright (C) 2006-2021 Matthias Heil and Andrew Hazel
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
//Header file for LinearWave elements


#ifndef OOMPH_WAVE_ELEMENTS_HEADER
#define OOMPH_WAVE_ELEMENTS_HEADER

// Config header generated by autoconfig
#ifdef HAVE_CONFIG_H
#include <oomph-lib-config.h>
#endif

//OOMPH-LIB headers
#include "../generic/nodes.h"
#include "../generic/Qelements.h"
#include "../generic/oomph_utilities.h"

namespace oomph
{

//=============================================================
/// A class for all isoparametric elements that solve the 
/// LinearWave equations.
/// \f[ 
/// \frac{\partial^2 u}{\partial x_i^2}=
/// \frac{\partial^2 u}{\partial t^2}+f(t,x_j)
/// \f] 
/// This contains the generic maths. Shape functions, geometric
/// mapping etc. must get implemented in derived class.
//=============================================================
template <unsigned DIM>
class LinearWaveEquations : public virtual FiniteElement
{

  public:

 /// \short Function pointer to source function fct(x,f(x)) -- 
 /// x is a Vector! 
 typedef void (*LinearWaveSourceFctPt)(const double& time,
                                 const Vector<double>& x,
                                 double& u);

 /// Constructor (must initialise the Source_fct_pt to null)
 LinearWaveEquations() : Source_fct_pt(0) {}
 
 /// Broken copy constructor
 LinearWaveEquations(const LinearWaveEquations& dummy) 
  { 
   BrokenCopy::broken_copy("LinearWaveEquations");
  } 
 
 /// Broken assignment operator
 void operator=(const LinearWaveEquations&) 
  {
   BrokenCopy::broken_assign("LinearWaveEquations");
  }

 /// \short Return the index at which the unknown value
 /// is stored. The default value, 0, is appropriate for single-physics
 /// problems, when there is only one variable, the value that satisfies the
 /// linear wave equation. 
 /// In derived multi-physics elements, this function should be overloaded
 /// to reflect the chosen storage scheme. Note that these equations require
 /// that the unknown is always stored at the same index at each node.
 virtual inline unsigned u_index_lin_wave() const {return 0;}

 /// \short du/dt at local node n. 
 /// Uses suitably interpolated value for hanging nodes.
 double du_dt_lin_wave(const unsigned &n) const
  {
   // Get the data's timestepper
   TimeStepper* time_stepper_pt=node_pt(n)->time_stepper_pt();

   //Initialise d^2u/dt^2
   double dudt=0.0;
   //Loop over the timesteps, if there is a non steady timestepper
   if (!time_stepper_pt->is_steady())
    {
     // Find the index at which the linear wave unknown is stored
     const unsigned u_nodal_index = u_index_lin_wave();     

     // Number of timsteps (past & present)
     const unsigned n_time = time_stepper_pt->ntstorage();
     
     //Add the contributions to the derivative
     for(unsigned t=0;t<n_time;t++)
      {
       dudt+=time_stepper_pt->weight(1,t)*nodal_value(t,n,u_nodal_index);
      }
    }
   return dudt;
  }

 /// \short d^2u/dt^2 at local node n. 
 /// Uses suitably interpolated value for hanging nodes.
 double d2u_dt2_lin_wave(const unsigned &n) const
  {
   // Get the data's timestepper
   TimeStepper* time_stepper_pt=node_pt(n)->time_stepper_pt();

   //Initialise d^2u/dt^2
   double ddudt=0.0;
   //Loop over the timesteps, if there is a non steady timestepper
   if (!time_stepper_pt->is_steady())
    {
     // Find the index at which the linear wave unknown is stored
     const unsigned u_nodal_index = u_index_lin_wave();     

     // Number of timsteps (past & present)
     const unsigned n_time = time_stepper_pt->ntstorage();
     
     //Add the contributions to the derivative
     for(unsigned t=0;t<n_time;t++)
      {
       ddudt+=time_stepper_pt->weight(2,t)*nodal_value(t,n,u_nodal_index);
      }
    }
   return ddudt;
  }

 /// Output with default number of plot points
 void output(std::ostream &outfile) 
  {
   unsigned nplot=5;
   output(outfile,nplot);
  }

 /// \short Output FE representation of soln: x,y,u or x,y,z,u at 
 /// n_plot^DIM plot points
 void output(std::ostream &outfile, const unsigned &nplot);


 /// Output with default number of plot points
 void output(FILE* file_pt) 
  {
   unsigned nplot=5;
   output(file_pt,nplot);
  }

 /// \short Output FE representation of soln: x,y,u or x,y,z,u at 
 /// n_plot^DIM plot points
 void output(FILE* file_pt, const unsigned &nplot);

 /// Output exact soln: x,y,u_exact or x,y,z,u_exact at nplot^DIM plot points
 void output_fct(std::ostream &outfile, const unsigned &nplot, 
                 FiniteElement::SteadyExactSolutionFctPt 
                 exact_soln_pt);

 /// \short Output exact soln: x,y,u_exact or x,y,z,u_exact at 
 /// nplot^DIM plot points (time-dependent version)
 virtual 
  void output_fct(std::ostream &outfile, const unsigned &nplot,
                  const double& time, 
                  FiniteElement::UnsteadyExactSolutionFctPt 
                  exact_soln_pt);

 /// Get error against and norm of exact solution
 void compute_error(std::ostream &outfile, 
                    FiniteElement::SteadyExactSolutionFctPt 
                    exact_soln_pt,
                    double& error, double& norm);

 /// Get error against and norm of exact solution
 void compute_error(std::ostream &outfile, 
                    FiniteElement::UnsteadyExactSolutionFctPt 
                    exact_soln_pt,
                    const double& time, double& error, double& norm);

 /// Access function: Pointer to source function
 LinearWaveSourceFctPt& source_fct_pt() {return Source_fct_pt;}

 /// Access function: Pointer to source function. Const version
 LinearWaveSourceFctPt source_fct_pt() const {return Source_fct_pt;}


 /// \short Get source term at continous time t and (Eulerian) position x.
 /// Virtual so it can be overloaded in derived multiphysics elements. 
 inline void get_source_lin_wave(const double& t, 
                                 const unsigned& ipt,
                                 const Vector<double>& x,
                                 double& source) const
  {
   //If no source function has been set, return zero
   if(Source_fct_pt==0) {source = 0.0;}
   else
    {
     // Get source strength
     (*Source_fct_pt)(t,x,source);
    }
  }


 /// Get flux: flux[i] = du/dx_i
 void get_flux(const Vector<double>& s, Vector<double>& flux) const
  {
   //Find out how many nodes there are in the element
   unsigned n_node = nnode();

   // Find the index at which the linear wave unknown is stored
   unsigned u_nodal_index = u_index_lin_wave();

   //Set up memory for the shape and test functions
   Shape psi(n_node);
   DShape dpsidx(n_node,DIM);
 
   //Call the derivatives of the shape and test functions
   dshape_eulerian(s,psi,dpsidx);
     
   //Initialise to zero
   for(unsigned j=0;j<DIM;j++) {flux[j] = 0.0;}
   
   // Loop over nodes
   for(unsigned l=0;l<n_node;l++) 
    {
     //Loop over derivative directions
     for(unsigned j=0;j<DIM;j++)
      {                               
       flux[j] += nodal_value(l,u_nodal_index)*dpsidx(l,j);
      }
    }
  }


 /// Compute element residual Vector (wrapper)
 void fill_in_contribution_to_residuals(Vector<double> &residuals)
  {
   //Call the generic residuals function with flag set to 0
   //using the dummy matrix argument
   fill_in_generic_residual_contribution_lin_wave(
    residuals,GeneralisedElement::Dummy_matrix,0);
  }


 /// Compute element residual Vector and element Jacobian matrix (wrapper)
 virtual void fill_in_contribution_to_jacobian(Vector<double> &residuals,
                                           DenseMatrix<double> &jacobian)
  {
   //Call the generic routine with the flag set to 1
   fill_in_generic_residual_contribution_lin_wave(residuals,jacobian,1);
  }
 

 /// Return FE representation of function value u(s) at local coordinate s
 inline double interpolated_u_lin_wave(const Vector<double> &s) const
  {
   //Find number of nodes
   unsigned n_node = nnode();

   // Find the index at which the linear wave unknown is stored
   unsigned u_nodal_index = u_index_lin_wave();

   //Local shape function
   Shape psi(n_node);

   //Find values of shape function
   shape(s,psi);

   //Initialise value of u
   double interpolated_u = 0.0;

   //Loop over the local nodes and sum
   for(unsigned l=0;l<n_node;l++) 
    {
     interpolated_u += nodal_value(l,u_nodal_index)*psi[l];
    }

   return(interpolated_u);
  }


 /// Return FE representation of function value u(s) at local coordinate s
 inline double interpolated_du_dt_lin_wave(const Vector<double> &s) const
  {
   //Find number of nodes
   unsigned n_node = nnode();

   //Local shape function
   Shape psi(n_node);

   //Find values of shape function
   shape(s,psi);

   //Initialise value of u
   double interpolated_du_dt = 0.0;

   //Loop over the local nodes and sum
   for(unsigned l=0;l<n_node;l++) 
    {
     interpolated_du_dt += du_dt_lin_wave(l)*psi[l];
    }

   return(interpolated_du_dt);
  }

 /// Return FE representation of function value u(s) at local coordinate s
 inline double interpolated_d2u_dt2_lin_wave(const Vector<double> &s) const
  {
   //Find number of nodes
   unsigned n_node = nnode();

   //Local shape function
   Shape psi(n_node);

   //Find values of shape function
   shape(s,psi);

   //Initialise value of u
   double interpolated_d2u_dt2 = 0.0;

   //Loop over the local nodes and sum
   for(unsigned l=0;l<n_node;l++) 
    {
     interpolated_d2u_dt2 += d2u_dt2_lin_wave(l)*psi[l];
    }

   return(interpolated_d2u_dt2);
  }



 /// \short Self-test: Return 0 for OK
 unsigned self_test();


  protected:

 /// \short Shape/test functions and derivs w.r.t. to global coords at 
 /// local coord. s; return  Jacobian of mapping
 virtual double dshape_and_dtest_eulerian_lin_wave(const Vector<double> &s, 
                                                   Shape &psi, 
                                                   DShape &dpsidx, 
                                                   Shape &test, 
                                                   DShape &dtestdx) const=0;

 /// \short Shape/test functions and derivs w.r.t. to global coords at 
 /// integration point ipt; return  Jacobian of mapping
 virtual double dshape_and_dtest_eulerian_at_knot_lin_wave(
  const unsigned &ipt, 
  Shape &psi, 
  DShape &dpsidx,
  Shape &test, 
  DShape &dtestdx) 
  const=0;

 /// \short Compute element residual Vector only (if flag=and/or element 
 /// Jacobian matrix 
 virtual void fill_in_generic_residual_contribution_lin_wave(
  Vector<double> &residuals, DenseMatrix<double> &jacobian, 
  unsigned flag); 


 /// Pointer to source function:
 LinearWaveSourceFctPt Source_fct_pt;
  
};






///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////



//======================================================================
/// QLinearWaveElement elements are linear/quadrilateral/brick-shaped 
/// LinearWave elements with isoparametric interpolation for the function.
///
/// Empty, just establishes the template parameters
///
///
//======================================================================
template <unsigned DIM, unsigned NNODE_1D>
 class QLinearWaveElement : public virtual QElement<DIM,NNODE_1D>,
 public virtual LinearWaveEquations<DIM>
{

  private:

 /// \short Static array of ints to hold number of variables at 
 /// nodes: Initial_Nvalue[n]
 static const unsigned Initial_Nvalue[];

 
  public:

 ///\short  Constructor: Call constructors for QElement and 
 /// LinearWave equations
 QLinearWaveElement() : QElement<DIM,NNODE_1D>(), LinearWaveEquations<DIM>()
  { }

 /// Broken copy constructor
 QLinearWaveElement(const QLinearWaveElement<DIM,NNODE_1D>& dummy) 
  { 
   BrokenCopy::broken_copy("QLinearWaveElement");
  } 

 
 /// Broken assignment operator
 void operator=(const QLinearWaveElement<DIM,NNODE_1D>&) 
  {
   BrokenCopy::broken_assign("QLinearWaveElement");
  }

 /// \short  Required  # of `values' (pinned or dofs) 
 /// at node n
 inline unsigned required_nvalue(const unsigned &n) const 
  {return Initial_Nvalue[n];}

 /// \short Output function:  
 ///  x,y,u   or    x,y,z,u
 void output(std::ostream &outfile)
  {LinearWaveEquations<DIM>::output(outfile);}

 ///  \short Output function:  
 ///   x,y,u   or    x,y,z,u at n_plot^DIM plot points
 void output(std::ostream &outfile, const unsigned &n_plot)
  {LinearWaveEquations<DIM>::output(outfile,n_plot);}

 /// \short Output function:  
 ///  x,y,u   or    x,y,z,u
 void output(FILE* file_pt)
  {LinearWaveEquations<DIM>::output(file_pt);}

 ///  \short Output function:  
 ///   x,y,u   or    x,y,z,u at n_plot^DIM plot points
 void output(FILE* file_pt, const unsigned &n_plot)
  {LinearWaveEquations<DIM>::output(file_pt,n_plot);}


 /// \short Output function for an exact solution:
 ///  x,y,u_exact   or    x,y,z,u_exact at n_plot^DIM plot points
 void output_fct(std::ostream &outfile, const unsigned &n_plot,
                 FiniteElement::SteadyExactSolutionFctPt 
                 exact_soln_pt)
  {LinearWaveEquations<DIM>::output_fct(outfile,n_plot,exact_soln_pt);}



 /// \short Output function for a time-dependent exact solution.
 ///  x,y,u_exact   or    x,y,z,u_exact at n_plot^DIM plot points
 /// (Calls the steady version)
 void output_fct(std::ostream &outfile, const unsigned &n_plot,
                 const double& time,
                 FiniteElement::UnsteadyExactSolutionFctPt
                 exact_soln_pt)
  {LinearWaveEquations<DIM>::output_fct(outfile,n_plot,time,exact_soln_pt);}


  protected:

/// Shape, test functions & derivs. w.r.t. to global coords. Return Jacobian.
 inline double dshape_and_dtest_eulerian_lin_wave(const Vector<double> &s, 
                                                  Shape &psi, 
                                                  DShape &dpsidx, 
                                                  Shape &test, 
                                                  DShape &dtestdx) const;
 

 /// \short Shape, test functions & derivs. w.r.t. to global coords. at
 /// integration point ipt. Return Jacobian.
 inline double dshape_and_dtest_eulerian_at_knot_lin_wave(const unsigned& ipt,
                                                          Shape &psi, 
                                                          DShape &dpsidx, 
                                                          Shape &test,
                                                          DShape &dtestdx) 
  const;

};

//Inline functions:


//======================================================================
/// Define the shape functions and test functions and derivatives
/// w.r.t. global coordinates and return Jacobian of mapping.
///
/// Galerkin: Test functions = shape functions
//======================================================================
template<unsigned DIM,unsigned NNODE_1D>
double QLinearWaveElement<DIM,NNODE_1D>::
 dshape_and_dtest_eulerian_lin_wave(const Vector<double> &s,
                                    Shape &psi, 
                                    DShape &dpsidx,
                                    Shape &test, 
                                    DShape &dtestdx) const
{
 //Call the geometrical shape functions and derivatives  
 double J = this->dshape_eulerian(s,psi,dpsidx);
 
 //Loop over the test functions and derivatives and set them equal to the
 //shape functions
 for(unsigned i=0;i<NNODE_1D;i++)
  {
   test[i] = psi[i]; 
   for(unsigned j=0;j<DIM;j++)
    {
     dtestdx(i,j) = dpsidx(i,j);
    }
  }
 
 //Return the jacobian
 return J;
}

//======================================================================
/// Define the shape functions and test functions and derivatives
/// w.r.t. global coordinates and return Jacobian of mapping.
///
/// Galerkin: Test functions = shape functions
//======================================================================
template<unsigned DIM, unsigned NNODE_1D>
 double QLinearWaveElement<DIM,NNODE_1D>::
 dshape_and_dtest_eulerian_at_knot_lin_wave(
 const unsigned &ipt,
 Shape &psi, 
 DShape &dpsidx,
 Shape &test, 
 DShape &dtestdx) const
{
 //Call the geometrical shape functions and derivatives  
 double J = this->dshape_eulerian_at_knot(ipt,psi,dpsidx);

 //Set the test functions equal to the shape functions
 //(sets internal pointers)
 test = psi;
 dtestdx = dpsidx;

 //Return the jacobian
 return J;
}


////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////



//=======================================================================
/// Face geometry for the QLinearWaveElement elements: The spatial 
/// dimension of the face elements is one lower than that of the
/// bulk element but they have the same number of points
/// along their 1D edges.
//=======================================================================
template<unsigned DIM, unsigned NNODE_1D>
class FaceGeometry<QLinearWaveElement<DIM,NNODE_1D> > : public virtual QElement<DIM-1,NNODE_1D>
{

  public:
 
 /// \short Constructor: Call the constructor for the
 /// appropriate lower-dimensional QElement
 FaceGeometry() : QElement<DIM-1,NNODE_1D>() {}

};

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////


//=======================================================================
/// Face geometry for the 1D QLinearWaveElement elements: Point elements
//=======================================================================
template<unsigned NNODE_1D>
class FaceGeometry<QLinearWaveElement<1,NNODE_1D> >: 
 public virtual PointElement
{

  public:
 
 /// \short Constructor: Call the constructor for the
 /// appropriate lower-dimensional QElement
 FaceGeometry() : PointElement() {}

};

}

#endif
