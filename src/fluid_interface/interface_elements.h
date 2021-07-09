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
//Header file for (one-dimensional) free surface elements
//Include guards, to prevent multiple includes
#ifndef OOMPH_INTERFACE_ELEMENTS_HEADER
#define OOMPH_INTERFACE_ELEMENTS_HEADER

// Config header generated by autoconfig
#ifdef HAVE_CONFIG_H
#include <oomph-lib-config.h>
#endif

#include "../generic/elements.h"
#include "../generic/spines.h"
#include "../generic/shape.h"
#include "../generic/hijacked_elements.h"

namespace oomph
{

//========================================================================
/// Base class for elements at the boundary of free surfaces or interfaces,
/// used typically to impose contact angle boundary conditions.
/// The elemental dimensions are one less than those of the
/// surface elements, or two less than those of the original bulk elements.
/// Thus in two-dimensional and axi-symmetric problems, are points,
/// but in three-dimensional problems, they are lines.
/// These boundaries may be in contact with a solid surface, in which case
/// the normal to that surface must be provided.
//=========================================================================
 class FluidInterfaceBoundingElement : public virtual FaceElement
 {
   private:
  
  /// \short Function pointer to a wall unit normal function. Returns the
  /// unit normal on the wall, at the specified Eulerian coordinate.
  typedef void (*WallUnitNormalFctPt)(const Vector<double> &x, 
                                      Vector<double> &unit_normal);
  
  /// \short Pointer to a wall normal function that returns
  /// the wall unit normal as a function of position in global
  /// Eulerian coordinates.
  WallUnitNormalFctPt Wall_unit_normal_fct_pt;
  
  /// Pointer to the desired value of the contact angle (if any)
  double *Contact_angle_pt;
  
  /// Pointer to the desired value of the capillary number
  double *Ca_pt;
  
   protected:
  
  /// \short Flag used to determine whether the contact angle is to be
  /// used (0 if not), and whether it will be applied weakly as a force term 
  /// in the momentum equations (1) or by hijacking the kinematic
  /// condition (2).
  unsigned Contact_angle_flag;
  
  /// \short Index at which the i-th velocity component is stored in the 
  /// element's nodes
  Vector<unsigned> U_index_interface_boundary;

  /// \short Function that is used to determine the local equation number of
  /// the kinematic equation associated with the nodes of the element
  /// This must be overloaded depending on the node update scheme
  virtual int kinematic_local_eqn(const unsigned &n)=0;
  
  /// \short Function that returns the unit normal of the bounding wall
  /// directed out of the fluid
  void wall_unit_normal(const Vector<double> &x, Vector<double> &normal)
  {
#ifdef PARANOID
   if(Wall_unit_normal_fct_pt)
    {
#endif
     (*Wall_unit_normal_fct_pt)(x,normal);
#ifdef PARANOID
    }
   else
    {
     throw 
      OomphLibError("Wall unit normal fct has not been set",
                    "FluidInterfaceBoundingElement::wall_unit_normal()",
                    OOMPH_EXCEPTION_LOCATION);
    }
#endif
  }
  
  ///\short The geometric data of the parent element is included as 
  ///external data and so a (bulk) node update must take place after
  ///the variation of any of this external data
  inline void update_in_external_fd(const unsigned &i) 
  {
   //Update the bulk element
   bulk_element_pt()->node_update();
  }
  
  ///\short The only external data are these geometric data so 
  ///We can omit the reset function (relying on the next update
  //function to take care of the remesh)
  inline void reset_in_external_fd(const unsigned &i) {}
  
  /// \short We require a final node update in the bulk element
  /// after all finite differencing
  inline void reset_after_external_fd() 
  {
   //Update the bulk element
   bulk_element_pt()->node_update();
  }
  
   public:
  
  ///Constructor
   FluidInterfaceBoundingElement() :
  Wall_unit_normal_fct_pt(0), Contact_angle_pt(0),
   Ca_pt(0), Contact_angle_flag(0) { } 
  
  /// Access function: Pointer to wall unit normal function
  WallUnitNormalFctPt &wall_unit_normal_fct_pt() 
   {return Wall_unit_normal_fct_pt;}
  
  /// Access function: Pointer to wall unit normal function. Const version
  WallUnitNormalFctPt wall_unit_normal_fct_pt() const 
  {return Wall_unit_normal_fct_pt;}
  
  ///Access for nodal index at which the velocity components are stored
  Vector<unsigned> &u_index_interface_boundary() 
   {
    return U_index_interface_boundary;
   }
  
  /// \short Set a pointer to the desired contact angle. Optional boolean
  /// (defaults to true)
  /// chooses strong imposition via hijacking (true) or weak imposition
  /// via addition to momentum equation (false). The default strong imposition
  /// is appropriate for static contact angle problems.
  void set_contact_angle(double* const &angle_pt, 
                         const bool &strong=true);
  
  /// Access function to the pointer specifying the prescribed contact angle
  double*& contact_angle_pt() {return Contact_angle_pt;}
  
  /// Access function to the pointer specifying the capillary number
  double* &ca_pt() {return Ca_pt;}
  
  /// Return the value of the capillary number
  double ca() 
  {
#ifdef PARANOID
   if(Ca_pt!=0)
    {
#endif
     return *Ca_pt;
#ifdef PARANOID
    }
   else
    {
     throw 
      OomphLibError("Capillary number has not been set",
                    "FluidInterfaceBoundingElement::ca()",
                    OOMPH_EXCEPTION_LOCATION);
    }
#endif
  }
  

  /// Return value of the contact angle
  double &contact_angle()
  {
#ifdef PARANOID   
   if(Contact_angle_pt==0)
    {
     std::string error_message = "Contact angle not set\n";
     error_message += 
      "Please use FluidInterfaceBoundingElement::set_contact_angle()\n";
     throw OomphLibError(error_message,
                         OOMPH_CURRENT_FUNCTION,
                         OOMPH_EXCEPTION_LOCATION);
    }
#endif
   return *Contact_angle_pt;
  }
  
  /// Calculate the residuals
  void fill_in_contribution_to_residuals(Vector<double> &residuals)
  {
   //Add the residual contributions using a dummy matrix
   fill_in_generic_residual_contribution_interface_boundary(
    residuals,GeneralisedElement::Dummy_matrix,0);
  }
  
  /// Calculate the generic residuals contribution
  virtual void fill_in_generic_residual_contribution_interface_boundary(
   Vector<double> &residuals,  DenseMatrix<double> &jacobian, 
   unsigned flag)=0;


  /// \short Empty helper function to calculate the additional contributions
  /// arising from the node update strategy to the Jacobian within the 
  /// integration loop. This will be overloaded by elements that require 
  /// contributions to their underlying equations from boundary integrals.  
  /// The shape functions, their derivatives w.r.t. to the local coordinates, 
  /// the unit normal and integral weight are passed in so that they do not 
  /// have to be recalculated.
  virtual void add_additional_residual_contributions_interface_boundary(
   Vector<double> &residuals, 
   DenseMatrix<double> &jacobian,
   const unsigned &flag,
   const Shape &psif,
   const DShape &dpsifds,
   const Vector<double> &interpolated_n, 
   const double &W) {}
  
  ///Overload the output function
  void output(std::ostream &outfile) {FiniteElement::output(outfile);}
  
  ///Output function
  void output(std::ostream &outfile, const unsigned &n_plot)
  {FiniteElement::output(outfile,n_plot);}
  
  ///Overload the C-style output function
  void output(FILE* file_pt) {FiniteElement::output(file_pt);}
  
  ///C-style Output function
  void output(FILE* file_pt, const unsigned &n_plot)
  {FiniteElement::output(file_pt,n_plot);}
  
 }; 



//==========================================================================
///Specialisation of the interface boundary constraint to a point
//========================================================================== 
 class PointFluidInterfaceBoundingElement : 
  public FluidInterfaceBoundingElement
 {
   protected:
  
  /// \short Overload the helper function to calculate the residuals and 
  /// (if flag==1) the Jacobian -- this function only deals with
  /// the part of the Jacobian that can be handled generically. 
  /// Specific additional contributions may be provided in
  /// add_additional_residual_contributions_interface_boundary(...)
  void fill_in_generic_residual_contribution_interface_boundary(
   Vector<double> &residuals, 
   DenseMatrix<double> &jacobian, 
   unsigned flag);
  
   public:
  
  /// Constructor
   PointFluidInterfaceBoundingElement(): 
  FluidInterfaceBoundingElement() {}
  
 };


//==========================================================================
///Specialisation of the interface boundary constraint to a line
//========================================================================== 
 class LineFluidInterfaceBoundingElement : 
  public FluidInterfaceBoundingElement
 {
   protected:
  
  /// \short Overload the helper function to calculate the residuals and 
  /// (if flag==true) the Jacobian -- this function only deals with
  /// the part of the Jacobian that can be handled generically. 
  /// Specific additional contributions may be provided in
  /// add_additional_residual_contributions_interface_boundary()
  void fill_in_generic_residual_contribution_interface_boundary(
   Vector<double> &residuals, 
   DenseMatrix<double> &jacobian, 
   unsigned flag);
  
   public:
  
  /// Constructor
   LineFluidInterfaceBoundingElement(): 
  FluidInterfaceBoundingElement() {}
  
 };




//=======================================================================
/// Base class establishing common interfaces and functions for all 
/// Navier-Stokes-like fluid
/// interface elements. Namely, elements that represent either a free 
/// surface or an interface between two fluids that have distinct
/// momentum-like equation for each velocity component.
//======================================================================
 class FluidInterfaceElement : public virtual FaceElement
 {
  //Make the bounding element class a friend 
  friend class FluidInterfaceBoundingElement;

   private:

  /// Pointer to the Capillary number 
  double *Ca_pt;
 
  /// Pointer to the Strouhal number
  double *St_pt;

  /// Default value for physical constants 
  static double Default_Physical_Constant_Value;

 
   protected:

  /// Nodal index at which the i-th velocity component is stored.
  Vector<unsigned> U_index_interface;

  /// \short The Data that contains the external  pressure is stored
  /// as external Data for the element. Which external Data item is it?
  /// (int so it can be initialised to -1, indicating that external
  /// pressure hasn't been set).
  int External_data_number_of_external_pressure;
 
  /// \short Pointer to the Data item that stores the external pressure
  Data* Pext_data_pt;

  /// \short Which of the values in Pext_data_pt stores the external pressure
  unsigned Index_of_external_pressure_value;
 
  /// \short Access function that returns the local equation number
  /// for the (scalar) kinematic equation associated with the j-th local
  /// node. This must be overloaded by specific interface elements
  /// and depends on the method for handing the free-surface deformation.
  virtual int kinematic_local_eqn(const unsigned &n)=0;
  
  /// \short Access function for the local equation number that
  /// corresponds to the external pressure.
  int pext_local_eqn() 
  {
#ifdef PARANOID
   if (External_data_number_of_external_pressure<0)
    {
     throw OomphLibError("No external pressure has been set\n",
                         OOMPH_CURRENT_FUNCTION,
                         OOMPH_EXCEPTION_LOCATION);
    }
#endif
   return external_local_eqn(External_data_number_of_external_pressure,
                             Index_of_external_pressure_value);
  }

  /// \short Helper function to calculate the residuals and 
  /// (if flag==1) the Jacobian of the equations.
  /// This is implemented generically using the surface
  /// divergence information that is overloaded in each element
  /// i.e. axisymmetric, two- or three-dimensional.
  virtual void fill_in_generic_residual_contribution_interface(
   Vector<double> &residuals, 
   DenseMatrix<double> &jacobian, 
   unsigned flag);

  /// \short Compute the surface gradient and surface divergence
  /// operators given the shape functions, derivatives,
  /// tangent vectors and position. All derivatives and
  /// tangent vectors should be formed 
  /// with respect to the local coordinates.
  /// 
  /// Return the jacobian of the surface, as well
  /// as the dpsidS, and dpsidS_div objects. 
  /// 
  /// This is the only
  /// function that needs to be overloaded to specify
  /// different geometries.
  ///
  /// In order to compute the surface gradient of a scalar
  /// function one needs only compute the sum over the nodes
  /// of dpsidS(l,i) * nodal_value(l,scalar_index)
  /// To compute the surface divergence of a vector quantity
  /// one computes a sum over nodes and coordinate directions
  /// dpsidS_div(l,i) * nodal_value(l,vector_index[i])
  /// In Cartesian cordinates the two surface derivatives are the
  /// same, but in Axisymmetric coordinates they are not!
  virtual double compute_surface_derivatives(const Shape &psi, const DShape &dpsids,
                                             const DenseMatrix<double> &interpolated_t,
                                             const Vector<double> &interpolated_x,
                                             DShape &dpsidS,
                                             DShape &dpsidS_div)=0;

  /// \short Helper function to calculate the additional contributions
  /// to the resisuals and Jacobian that arise from specific node update 
  /// strategies. This is called within the integration loop over the
  /// element (for efficiency) and therefore requires a fairly large
  /// number of input parameters:
  /// - the velocity shape functions and their derivatives w.r.t.
  ///   the local coordinates
  /// - the surface gradient and divergence of the velocity shape
  ///   functions
  /// - The local and Eulerian coordinates,
  /// - the outer unit normal,
  /// - the integration weight from the integration scheme 
  /// - the Jacobian of the mapping between the local and global coordinates
  ///   along the element. (Note that in the axisymmmetric case this
  ///   includes the r term)!
  virtual void add_additional_residual_contributions_interface(
   Vector<double> &residuals, 
   DenseMatrix<double> &jacobian,
   const unsigned &flag,
   const Shape &psif,
   const DShape &dpsifds,
   const DShape &dpsifdS,
   const DShape &dpsifdS_div,
   const Vector<double> &s,
   const Vector<double> &interpolated_x, 
   const Vector<double> &interpolated_n, 
   const double &W, 
   const double &J) {}

   public:

 
  /// Constructor, set the default values of the booleans and pointers (null)
   FluidInterfaceElement():   Pext_data_pt(0) 
   {
    // Initialise pointer to capillary number
    Ca_pt = 0;
   
    //Set the Strouhal number to the default value
    St_pt = &Default_Physical_Constant_Value;
   }
  
  /// \short Virtual function that specifies the non-dimensional 
  ///  surface tension as a function of local position within the element.
  /// The default behaviour is a constant surface tension of value 1.0
  /// This function can be overloaded in more specialised elements to
  /// incorporate variations in surface tension.
  virtual double sigma(const Vector<double> &s_local) { return 1.0; }
  
  /// Calculate the residuals by calling the generic residual contribution.
  void fill_in_contribution_to_residuals(Vector<double> &residuals)
  {
   //Add the residual contributions
   fill_in_generic_residual_contribution_interface(
    residuals,GeneralisedElement::Dummy_matrix,0);
  }
  


  /// The value of the Capillary number
  const double& ca() const {
#ifdef PARANOID
   if(Ca_pt!=0)
    {
#endif
     return *Ca_pt;
#ifdef PARANOID
    }
   else
    {
     throw 
      OomphLibError("Capillary number has not been set",
                    "FluidInterfaceElement::ca()",
                    OOMPH_EXCEPTION_LOCATION);
    }
#endif
  }
  
  /// Pointer to the Capillary number
  double*& ca_pt() {return Ca_pt;}
  
  /// The value of the Strouhal number
  const double &st() const {return *St_pt;}
  
  /// The pointer to the Strouhal number
  double* &st_pt() {return St_pt;}
 
  /// \short Return the i-th velocity component at local node j.
  double u(const unsigned &j, const unsigned &i)
  {return node_pt(j)->value(U_index_interface[i]);}
 
  /// \short Calculate the i-th velocity component at the local coordinate s.
  double interpolated_u(const Vector<double> &s, const unsigned &i); 
 
  /// Return the value of the external pressure
  double pext() const 
  {
   //If the external pressure has not been set, then return a 
   //default value of zero.
   if(Pext_data_pt==0)
    {
     return 0.0;
    }
   //Otherwise return the appropriate value
   else
    {
     return Pext_data_pt->value(Index_of_external_pressure_value);
    }
  }
 
  /// \short Set the Data that contains the single pressure value
  /// that specifies the "external pressure" for the 
  /// interface/free-surface. Setting this only makes sense
  /// if the interface is, in fact, a free surface (well,
  /// an interface to another inviscid fluid if you want to be picky). 
  void set_external_pressure_data(Data* external_pressure_data_pt)
  {
#ifdef PARANOID
   if (external_pressure_data_pt->nvalue()!=1)
    {
     std::ostringstream error_message;
     error_message
      << "External pressure Data must only contain a single value!\n"
      << "This one contains "
      << external_pressure_data_pt->nvalue() << std::endl;

     throw OomphLibError(error_message.str(),
                         OOMPH_CURRENT_FUNCTION,
                         OOMPH_EXCEPTION_LOCATION);
    }
#endif
 
   // Store pointer explicitly
   Pext_data_pt=external_pressure_data_pt;
 
   // Add the external pressure to the element's external Data?
   // But do not finite-difference with respect to it
   this->add_external_data(Pext_data_pt,false);

   // The external pressure has just been promoted to become
   // external Data of this element -- what is its number?
   External_data_number_of_external_pressure=this->nexternal_data()-1;

   // Index of pressure value in Data object 
   Index_of_external_pressure_value=0;
  }
 
  /// \short Set the Data that contains the pressure value
  /// that specifies the "external pressure" for the 
  /// interface/free-surface. Setting this only makes sense
  /// if the interface is, in fact, a free surface (well,
  /// an interface to another inviscid fluid if you want to be picky). 
  /// Second argument specifies the index of the pressure
  /// value within the Data object.
  void set_external_pressure_data(Data* external_pressure_data_pt,
                                  const unsigned& 
                                  index_of_external_pressure_value)
  {
   // Index of pressure value in Data object 
   Index_of_external_pressure_value=index_of_external_pressure_value;

#ifdef PARANOID
   if (index_of_external_pressure_value>=external_pressure_data_pt->nvalue())
    {
     std::ostringstream error_message;
     error_message
      << "External pressure Data only contains " 
      << external_pressure_data_pt->nvalue() <<  " values\n"
      << "You have declared value " << index_of_external_pressure_value
      << " to be the value representing the pressure\n" << std::endl;
     throw OomphLibError(error_message.str(),
                         OOMPH_CURRENT_FUNCTION,
                         OOMPH_EXCEPTION_LOCATION);
    }
#endif
 
   // Store pointer explicitly
   Pext_data_pt=external_pressure_data_pt;
 
   // Add the external pressure to the element's external Data?
   // But do not finite-difference with respect to it
   this->add_external_data(Pext_data_pt,false);

   // The external pressure has just been promoted to become
   // external Data of this element -- what is its number?
   External_data_number_of_external_pressure=this->nexternal_data()-1;
  }
 

  /// \short Create a bounding element e.g. to apply a contact angle boundary
  /// condition
  virtual FluidInterfaceBoundingElement* make_bounding_element(
   const int &face_index)
  {
   throw OomphLibError("Virtual function not yet implemented",
                       OOMPH_CURRENT_FUNCTION,
                       OOMPH_EXCEPTION_LOCATION);
   return 0;
  }


  /// \short Hijack the kinematic condition at the node numbers passed in
  /// the vector. The node numbers correspond to the local numbers of
  /// nodes in the associated bulk element.
  /// This is required so that contact-angle conditions can be applied
  /// by the FluidInterfaceBoundingElements.
  virtual void hijack_kinematic_conditions(const Vector<unsigned> 
                                           &bulk_node_number)=0;

  ///Overload the output function
  void output(std::ostream &outfile) {FiniteElement::output(outfile);}
  
  ///Output function
  void output(std::ostream &outfile, const unsigned &n_plot);
  
  ///Overload the C-style output function
  void output(FILE* file_pt) {FiniteElement::output(file_pt);}
  
  ///C-style Output function
  void output(FILE* file_pt, const unsigned &n_plot);
 
 };


//=============================================================
/// Class that establishes the surface derivative functions for
/// LineElements. These are defined in a separate class so that
/// they can be used by other interface equation-type classes.
//=============================================================
 class LineDerivatives
 {
   public:
  //Empty Constructor
  LineDerivatives() {}

   protected:
 
 
  ///Fill in the specific surface derivative calculations
  double compute_surface_derivatives(const Shape &psi, const DShape &dpsids,
                                     const DenseMatrix<double> &interpolated_t,
                                     const Vector<double> &interpolated_x,
                                     DShape &surface_gradient,
                                     DShape &surface_divergence);

 };


//=============================================================
/// Class that establishes the surface derivative functions for
/// AxisymmetricInterfaceElements.
/// These are defined in a separate class so that
/// they can be used by other interface equation-type classes.
//=============================================================
 class AxisymmetricDerivatives
 {
   public:
  //Empty Constructor
  AxisymmetricDerivatives() {}

   protected:
 
 
  ///Fill in the specific surface derivative calculations
  double compute_surface_derivatives(const Shape &psi, const DShape &dpsids,
                                     const DenseMatrix<double> &interpolated_t,
                                     const Vector<double> &interpolated_x,
                                     DShape &surface_gradient,
                                     DShape &surface_divergence);

 };


//=============================================================
/// Class that establishes the surface derivative functions for
/// SurfaceInterfaceElements (2D surfaces in 3D space)
/// These are defined in a separate class so that
/// they can be used by other interface equation-type classes.
//=============================================================
 class SurfaceDerivatives
 {
   public:
  //Empty Constructor
  SurfaceDerivatives() {}

   protected:
 
 
  ///Fill in the specific surface derivative calculations
  double compute_surface_derivatives(const Shape &psi, const DShape &dpsids,
                                     const DenseMatrix<double> &interpolated_t,
                                     const Vector<double> &interpolated_x,
                                     DShape &surface_gradient,
                                     DShape &surface_divergence);

 };


}

#endif






