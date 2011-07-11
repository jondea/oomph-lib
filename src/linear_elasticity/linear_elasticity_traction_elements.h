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
//Header file for elements that are used to apply surface loads to 
//the equations of linear elasticity

#ifndef OOMPH_LINEAR_ELASTICITY_TRACTION_ELEMENTS_HEADER
#define OOMPH_LINEAR_ELASTICITY_TRACTION_ELEMENTS_HEADER

// Config header generated by autoconfig
#ifdef HAVE_CONFIG_H
  #include <oomph-lib-config.h>
#endif


//OOMPH-LIB headers
#include "../generic/Qelements.h"

namespace oomph
{



//=======================================================================
/// Namespace containing the zero traction function for linear elasticity 
/// traction elements
//=======================================================================
namespace LinearElasticityTractionElementHelper
 {

  //=======================================================================
  /// Default load function (zero traction)
  //=======================================================================
  void Zero_traction_fct(const Vector<double> &x,
                         const Vector<double>& N,
                         Vector<double>& load)
   {
    unsigned n_dim=load.size();
    for (unsigned i=0;i<n_dim;i++) {load[i]=0.0;}
   }
 
 }


//======================================================================
/// A class for elements that allow the imposition of an applied traction
/// in the equations of linear elasticity.
/// The geometrical information can be read from the FaceGeometry<ELEMENT> 
/// class and and thus, we can be generic enough without the need to have
/// a separate equations class.
//======================================================================
template <class ELEMENT>
class LinearElasticityTractionElement : public virtual FaceGeometry<ELEMENT>, 
public virtual FaceElement
{
  protected:
 
 /// Index at which the i-th displacement component is stored
 Vector<unsigned> U_index_linear_elasticity_traction;
 
 /// \short Pointer to an imposed traction function. Arguments:
 /// Eulerian coordinate; outer unit normal;
 /// applied traction. (Not all of the input arguments will be
 /// required for all specific load functions but the list should
 /// cover all cases)
 void (*Traction_fct_pt)(const Vector<double> &x, 
                         const Vector<double> &n,
                         Vector<double> &result);
 
 
 /// \short Get the traction vector: Pass number of integration point (dummy), 
 /// Eulerlian coordinate and normal vector and return the load vector
 /// (not all of the input arguments will be
 /// required for all specific load functions but the list should
 /// cover all cases). This function is virtual so it can be 
 /// overloaded for FSI.
 virtual void get_traction(const unsigned& intpt,
                           const Vector<double>& x,
                           const Vector<double>& n,
                           Vector<double>& traction)
 {
  Traction_fct_pt(x,n,traction);
 }
 
 
 /// \short Helper function that actually calculates the residuals
 // This small level of indirection is required to avoid calling
 // fill_in_contribution_to_residuals in fill_in_contribution_to_jacobian
 // which causes all kinds of pain if overloading later on
 void fill_in_contribution_to_residuals_linear_elasticity_traction(
  Vector<double> &residuals);
 
 
  public:
 
 /// \short Constructor, which takes a "bulk" element and the 
 /// value of the index and its limit
 LinearElasticityTractionElement(FiniteElement* const &element_pt, 
                                 const int &face_index) : 
 FaceGeometry<ELEMENT>(), FaceElement()
  { 
#ifdef PARANOID
   {
    //Check that the element is not a refineable 3d element
    ELEMENT* elem_pt = new ELEMENT;
    //If it's three-d
    if(elem_pt->dim()==3)
     {
      //Is it refineable
      if(dynamic_cast<RefineableElement*>(elem_pt))
       {
        //Issue a warning
        OomphLibWarning(
         "This flux element will not work correctly if nodes are hanging\n",
         "LinearElasticityTractionElement::Constructor",
         OOMPH_EXCEPTION_LOCATION);
       }
     }
   }
#endif
   
   //Attach the geometrical information to the element. N.B. This function
   //also assigns nbulk_value from the required_nvalue of the bulk element
   element_pt->build_face_element(face_index,this);
   
   //Find the dimension of the problem
   unsigned n_dim = element_pt->nodal_dimension();
   
   //Find the index at which the displacemenet unknowns are stored
   ELEMENT* cast_element_pt = dynamic_cast<ELEMENT*>(element_pt);
   this->U_index_linear_elasticity_traction.resize(n_dim);
   for(unsigned i=0;i<n_dim;i++)
    {
     this->U_index_linear_elasticity_traction[i] = 
      cast_element_pt->u_index_linear_elasticity(i);
    }
   
   // Zero traction
   Traction_fct_pt=&LinearElasticityTractionElementHelper::Zero_traction_fct;
  }
 
 
 /// Reference to the traction function pointer
 void (* &traction_fct_pt())(const Vector<double>& x,
                             const Vector<double>& n,
                             Vector<double>& traction)
  {return Traction_fct_pt;}
 
 
 /// Return the residuals
 void fill_in_contribution_to_residuals(Vector<double> &residuals)
 {
  fill_in_contribution_to_residuals_linear_elasticity_traction(residuals);
 }
 
 
 
 /// Fill in contribution from Jacobian
 void fill_in_contribution_to_jacobian(Vector<double> &residuals,
                                       DenseMatrix<double> &jacobian)
 {
  //Call the residuals
  fill_in_contribution_to_residuals_linear_elasticity_traction(residuals);
 }
 
 /// Specify the value of nodal zeta from the face geometry
 /// \short The "global" intrinsic coordinate of the element when
 /// viewed as part of a geometric object should be given by
 /// the FaceElement representation, by default (needed to break
 /// indeterminacy if bulk element is SolidElement)
 double zeta_nodal(const unsigned &n, const unsigned &k,           
                          const unsigned &i) const 
  {return FaceElement::zeta_nodal(n,k,i);}     

 /// \short Output function
 void output(std::ostream &outfile)
 {FiniteElement::output(outfile);}
 
 /// \short Output function
 void output(std::ostream &outfile, const unsigned &n_plot)
 {FiniteElement::output(outfile,n_plot);}
 
 /// \short C_style output function
 void output(FILE* file_pt)
 {FiniteElement::output(file_pt);}
 
 /// \short C-style output function
 void output(FILE* file_pt, const unsigned &n_plot)
 {FiniteElement::output(file_pt,n_plot);}
 
 
 /// \short Compute traction vector at specified local coordinate
 /// Should only be used for post-processing; ignores dependence
 /// on integration point!
 void traction(const Vector<double>& s, 
               Vector<double>& traction);
 
}; 

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

//=====================================================================
/// Compute traction vector at specified local coordinate
/// Should only be used for post-processing; ignores dependence
/// on integration point!
//=====================================================================
template<class ELEMENT>
void LinearElasticityTractionElement<ELEMENT>::traction(
 const Vector<double>& s, Vector<double>& traction)
 {
  unsigned n_dim = this->nodal_dimension();
  
  // Position vector
  Vector<double> x(n_dim);
  interpolated_x(s,x);
  
  // Outer unit normal
  Vector<double> unit_normal(n_dim);
  outer_unit_normal(s,unit_normal);
  
  // Dummy
  unsigned ipt=0;
  
  // Traction vector
  get_traction(ipt,x,unit_normal,traction);
  
 }


//=====================================================================
/// Return the residuals for the LinearElasticityTractionElement equations
//=====================================================================
template<class ELEMENT>
 void LinearElasticityTractionElement<ELEMENT>::
 fill_in_contribution_to_residuals_linear_elasticity_traction(
  Vector<double> &residuals)
 {

  //Find out how many nodes there are
  unsigned n_node = nnode();
    
 #ifdef PARANOID
  //Find out how many positional dofs there are
  unsigned n_position_type = this->nnodal_position_type();  
  if(n_position_type != 1)
   {
    throw OomphLibError(
     "LinearElasticity is not yet implemented for more than one position type",
     "LinearElasticityEquationsBase<DIM>:: fill_in_contribution_to_residuals_linear_elasticity_traction()",
     OOMPH_EXCEPTION_LOCATION);
   }
#endif
  
  //Find out the dimension of the node
  unsigned n_dim = this->nodal_dimension();
  
  //Cache the nodal indices at which the displacement components are stored
  unsigned u_nodal_index[n_dim];
  for(unsigned i=0;i<n_dim;i++)
   {
    u_nodal_index[i] = this->U_index_linear_elasticity_traction[i];
   }
  
  //Integer to hold the local equation number
  int local_eqn=0;
  
  //Set up memory for the shape functions
  //Note that in this case, the number of lagrangian coordinates is always
  //equal to the dimension of the nodes
  Shape psi(n_node);
  DShape dpsids(n_node,n_dim-1); 
  
  //Set the value of n_intpt
  unsigned n_intpt = integral_pt()->nweight();
  
  //Loop over the integration points
  for(unsigned ipt=0;ipt<n_intpt;ipt++)
   {
    //Get the integral weight
    double w = integral_pt()->weight(ipt);
    
    //Only need to call the local derivatives
    dshape_local_at_knot(ipt,psi,dpsids);
    
    //Calculate the Eulerian and Lagrangian coordinates 
    Vector<double> interpolated_x(n_dim,0.0);
    
    //Also calculate the surface Vectors (derivatives wrt local coordinates)
    DenseMatrix<double> interpolated_A(n_dim-1,n_dim,0.0);   
    
    //Calculate displacements and derivatives
    for(unsigned l=0;l<n_node;l++) 
     {
      //Loop over directions
      for(unsigned i=0;i<n_dim;i++)
       {        
        //Calculate the Eulerian coords
        const double x_local = nodal_position(l,i);
        interpolated_x[i] += x_local*psi(l);
        
        //Loop over LOCAL derivative directions, to calculate the tangent(s)
        for(unsigned j=0;j<n_dim-1;j++)
         {
          interpolated_A(j,i) += x_local*dpsids(l,j);
         }
       }
     }
    
    //Now find the local metric tensor from the tangent Vectors
    DenseMatrix<double> A(n_dim-1);
    for(unsigned i=0;i<n_dim-1;i++)
     {
      for(unsigned j=0;j<n_dim-1;j++)
       {
        //Initialise surface metric tensor to zero
        A(i,j) = 0.0;
        
        //Take the dot product
        for(unsigned k=0;k<n_dim;k++)
         { 
          A(i,j) += interpolated_A(i,k)*interpolated_A(j,k);
         }
       }
     }
    
    //Get the outer unit normal
    Vector<double> interpolated_normal(n_dim);
    outer_unit_normal(ipt,interpolated_normal);
    
    //Find the determinant of the metric tensor
    double Adet =0.0;
    switch(n_dim)
     {
     case 2:
      Adet = A(0,0);
      break;
     case 3:
      Adet = A(0,0)*A(1,1) - A(0,1)*A(1,0);
      break;
     default:
      throw 
       OomphLibError(
        "Wrong dimension in LinearElasticityTractionElement",
        "LinearElasticityTractionElement::fill_in_contribution_to_residuals()",
        OOMPH_EXCEPTION_LOCATION);
     }
    
    //Premultiply the weights and the square-root of the determinant of 
    //the metric tensor
    double W = w*sqrt(Adet);
    
    //Now calculate the load
    Vector<double> traction(n_dim);
    get_traction(ipt,
                 interpolated_x,
                 interpolated_normal,
                 traction);
       
    //Loop over the test functions, nodes of the element
    for(unsigned l=0;l<n_node;l++)
     {
      //Loop over the displacement components
      for(unsigned i=0;i<n_dim;i++)
       {
        local_eqn = this->nodal_local_eqn(l,u_nodal_index[i]);
        /*IF it's not a boundary condition*/
        if(local_eqn >= 0)
         {
          //Add the loading terms to the residuals
          residuals[local_eqn] -= traction[i]*psi(l)*W;
         }
       } //End of if not boundary condition
     } //End of loop over shape functions
   } //End of loop over integration points
  
 }





}

#endif
