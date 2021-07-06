//LIC// ====================================================================
//LIC// This file forms part of oomph-lib, the object-oriented, 
//LIC// multi-physics finite-element library, available 
//LIC// at http://www.oomph-lib.org.
//LIC// 
//LIC//    Version 1.0; svn revision $LastChangedRevision$
//LIC//
//LIC// $LastChangedDate$
//LIC// 
//LIC// Copyright (C) 2006-2016 Matthias Heil and Andrew Hazel
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
//Header file for THelmholtz elements
#ifndef OOMPH_THELMHOLTZ_ELEMENTS_HEADER
#define OOMPH_THELMHOLTZ_ELEMENTS_HEADER


// Config header generated by autoconfig
#ifdef HAVE_CONFIG_H
  #include <oomph-lib-config.h>
#endif


//OOMPH-LIB headers
#include "../generic/nodes.h"
#include "../generic/oomph_utilities.h"
#include "../generic/Telements.h"
#include "../generic/error_estimator.h"
#include "helmholtz_elements.h"

namespace oomph
{

/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
// THelmholtzElement
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////



//======================================================================
/// THelmholtzElement<DIM,NNODE_1D> elements are isoparametric triangular 
/// DIM-dimensional Helmholtz elements with  NNODE_1D nodal points along each
/// element edge. Inherits from TElement and HelmholtzEquations
//======================================================================
template <unsigned DIM, unsigned NNODE_1D>
class THelmholtzElement : public TElement<DIM,NNODE_1D>, 
 public HelmholtzEquations<DIM>,
 public virtual ElementWithZ2ErrorEstimator
{
 
 public:
 
 ///\short  Constructor: Call constructors for TElement and 
 /// Helmholtz equations
 THelmholtzElement() : TElement<DIM,NNODE_1D>(), HelmholtzEquations<DIM>()
  { }


 /// Broken copy constructor
 THelmholtzElement(const THelmholtzElement<DIM,NNODE_1D>& dummy) 
  { 
   BrokenCopy::broken_copy("THelmholtzElement");
  } 
 
 /// Broken assignment operator
//Commented out broken assignment operator because this can lead to a conflict warning
//when used in the virtual inheritence hierarchy. Essentially the compiler doesn't
//realise that two separate implementations of the broken function are the same and so,
//quite rightly, it shouts.
 /*void operator=(const THelmholtzElement<DIM,NNODE_1D>&) 
  {
   BrokenCopy::broken_assign("THelmholtzElement");
   }*/

 /// \short  Access function for Nvalue: # of `values' (pinned or dofs) 
 /// at node n (always returns the same value at every node, 1)
 inline unsigned required_nvalue(const unsigned &n) const 
  {return Initial_Nvalue;}

 /// \short Output function:  
 ///  x,y,u   or    x,y,z,u
 void output(std::ostream &outfile)
  {
   HelmholtzEquations<DIM>::output(outfile);
  }

 ///  \short Output function:  
 ///   x,y,u   or    x,y,z,u at n_plot^DIM plot points
 void output(std::ostream &outfile, const unsigned &n_plot)
  {
   HelmholtzEquations<DIM>::output(outfile,n_plot);
  }


 /// \short C-style output function:  
 ///  x,y,u   or    x,y,z,u
 void output(FILE* file_pt)
  {
   HelmholtzEquations<DIM>::output(file_pt);
  }


 ///  \short C-style output function:  
 ///   x,y,u   or    x,y,z,u at n_plot^DIM plot points
 void output(FILE* file_pt, const unsigned &n_plot)
  {
   HelmholtzEquations<DIM>::output(file_pt,n_plot);
  }


 /// \short Output function for an exact solution:
 ///  x,y,u_exact
 void output_fct(std::ostream &outfile, const unsigned &n_plot,
                 FiniteElement::SteadyExactSolutionFctPt exact_soln_pt)
 {
  HelmholtzEquations<DIM>::output_fct(outfile,n_plot,exact_soln_pt);
 }

 
 /// \short Output function for a time-dependent exact solution.
 ///  x,y,u_exact (calls the steady version)
 void output_fct(std::ostream &outfile, const unsigned &n_plot,
                 const double& time,
                 FiniteElement::UnsteadyExactSolutionFctPt exact_soln_pt)
  {
   HelmholtzEquations<DIM>::output_fct(outfile,n_plot,time,exact_soln_pt);
  }

protected:

/// Shape, test functions & derivs. w.r.t. to global coords. Return Jacobian.
 inline double dshape_and_dtest_eulerian_helmholtz(const Vector<double> &s, 
                                                 Shape &psi, 
                                                 DShape &dpsidx, 
                                                 Shape &test, 
                                                 DShape &dtestdx) const;
 

/// Shape, test functions & derivs. w.r.t. to global coords. Return Jacobian.
 inline double dshape_and_dtest_eulerian_at_knot_helmholtz(const unsigned &ipt,
                                                         Shape &psi, 
                                                         DShape &dpsidx, 
                                                         Shape &test,
                                                         DShape &dtestdx) 
  const;


 /// \short Order of recovery shape functions for Z2 error estimation:
 /// Same order as shape functions.
 unsigned nrecovery_order() {return (NNODE_1D-1);}
 
 /// Number of 'flux' terms for Z2 error estimation 
 unsigned num_Z2_flux_terms() {return 2*DIM;}
 
 /// \short Get 'flux' for Z2 error recovery:  Standard flux from 
 /// UnsteadyHeat equations
 void get_Z2_flux(const Vector<double>& s, Vector<double>& flux)
 {
  Vector<std::complex <double> > complex_flux(DIM);
  this->get_flux(s,complex_flux);
  unsigned count=0;
  for (unsigned i=0;i<DIM;i++)
   {
    flux[count++]=complex_flux[i].real();
    flux[count++]=complex_flux[i].imag();
   }
 }
 
 /// \short Number of vertex nodes in the element
 unsigned nvertex_node() const
 {return TElement<DIM,NNODE_1D>::nvertex_node();}
 
 /// \short Pointer to the j-th vertex node in the element
 Node* vertex_node_pt(const unsigned& j) const
 {return TElement<DIM,NNODE_1D>::vertex_node_pt(j);}
 
  private:
 
 /// Static unsigned that holds the (same) number of variables at every node
 static const unsigned Initial_Nvalue;
 

};




//Inline functions:


//======================================================================
/// Define the shape functions and test functions and derivatives
/// w.r.t. global coordinates and return Jacobian of mapping.
///
/// Galerkin: Test functions = shape functions
//======================================================================
template<unsigned DIM, unsigned NNODE_1D>
double THelmholtzElement<DIM,NNODE_1D>::dshape_and_dtest_eulerian_helmholtz(
 const Vector<double> &s, 
 Shape &psi, 
 DShape &dpsidx,
 Shape &test, 
 DShape &dtestdx) const
{
 unsigned n_node = this->nnode();

 //Call the geometrical shape functions and derivatives  
 double J = this->dshape_eulerian(s,psi,dpsidx);

 //Loop over the test functions and derivatives and set them equal to the
 //shape functions
 for(unsigned i=0;i<n_node;i++)
  {
   test[i] = psi[i]; 
   dtestdx(i,0) = dpsidx(i,0);
   dtestdx(i,1) = dpsidx(i,1);
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
double THelmholtzElement<DIM,NNODE_1D>::
dshape_and_dtest_eulerian_at_knot_helmholtz(
 const unsigned &ipt,
 Shape &psi, 
 DShape &dpsidx,
 Shape &test, 
 DShape &dtestdx) const
{

 //Call the geometrical shape functions and derivatives  
 double J = this->dshape_eulerian_at_knot(ipt,psi,dpsidx);

 //Set the pointers of the test functions
 test = psi;
 dtestdx = dpsidx;

 //Return the jacobian
 return J;

}



//=======================================================================
/// Face geometry for the THelmholtzElement elements: The spatial 
/// dimension of the face elements is one lower than that of the
/// bulk element but they have the same number of points
/// along their 1D edges.
//=======================================================================
template<unsigned DIM, unsigned NNODE_1D>
class FaceGeometry<THelmholtzElement<DIM,NNODE_1D> >: 
 public virtual TElement<DIM-1,NNODE_1D>
{

  public:
 
 /// \short Constructor: Call the constructor for the
 /// appropriate lower-dimensional TElement
 FaceGeometry() : TElement<DIM-1,NNODE_1D>() {}

};

//=======================================================================
/// Face geometry for the 1D THelmholtzElement elements: Point elements
//=======================================================================
template<unsigned NNODE_1D>
class FaceGeometry<THelmholtzElement<1,NNODE_1D> >: 
 public virtual PointElement
{

  public:
 
 /// \short Constructor: Call the constructor for the
 /// appropriate lower-dimensional TElement
 FaceGeometry() : PointElement() {}

};


}

#endif
