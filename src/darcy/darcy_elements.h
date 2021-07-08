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
#ifndef OOMPH_RAVIART_THOMAS_DARCY_HEADER
#define OOMPH_RAVIART_THOMAS_DARCY_HEADER

// Config header generated by autoconfig
#ifdef HAVE_CONFIG_H
  #include <oomph-lib-config.h>
#endif

#include "../generic/elements.h"
#include "../generic/shape.h"
#include "../generic/error_estimator.h"
#include "../generic/projection.h"


namespace oomph
{

 //===========================================================================
 /// \short Class implementing the generic maths of the Darcy equations using
 /// Raviart-Thomas elements with both edge and internal degrees of freedom
 //===========================================================================
 template <unsigned DIM>
  class DarcyEquations : public virtual FiniteElement,
  public virtual ElementWithZ2ErrorEstimator
 {
 public:

  /// Source function pointer typedef
  typedef void (*SourceFctPt)(const Vector<double>& x,
                              Vector<double>& f);

  /// Mass source function pointer typedef
  typedef void (*MassSourceFctPt)(const Vector<double>& x,
                                   double& f);

  /// Constructor
  DarcyEquations() : Source_fct_pt(0), Mass_source_fct_pt(0)
  {
  }

  /// Access function: Pointer to body force function
  SourceFctPt& source_fct_pt() {return Source_fct_pt;}

  /// Access function: Pointer to body force function (const version)
  SourceFctPt source_fct_pt() const {return Source_fct_pt;}

  /// Access function: Pointer to mass source function
  MassSourceFctPt& mass_source_fct_pt() {return Mass_source_fct_pt;}

  /// Access function: Pointer to mass source function (const version)
  MassSourceFctPt mass_source_fct_pt() const {return Mass_source_fct_pt;}

  /// \short Indirect access to the source function - returns 0 if no source
  /// function has been set
  void source(const Vector<double>& x,
              Vector<double>& b) const
   {
    // If no function has been set, return zero vector
    if(Source_fct_pt==0)
     {
      // Get spatial dimension of element
      unsigned n=dim();
      for(unsigned i=0;i<n;i++)
       {
        b[i] = 0.0;
       }
     }
    else
     {
      // Get body force
      (*Source_fct_pt)(x,b);
     }
   }

  /// \short Indirect access to the mass source function - returns 0 if no
  /// mass source function has been set
  void mass_source(const Vector<double>& x,
                   double& b) const
   {
    // If no function has been set, return zero vector
    if(Mass_source_fct_pt==0)
     {
      b = 0.0;
     }
    else
     {
      // Get body force
      (*Mass_source_fct_pt)(x,b);
     }
   }

  /// Number of values required at node n
  virtual unsigned required_nvalue(const unsigned &n) const = 0;

  /// Return the equation number of the n-th edge (flux) degree of freedom
  virtual int q_edge_local_eqn(const unsigned &n) const = 0;

  /// Return the equation number of the n-th internal degree of freedom
  virtual int q_internal_local_eqn(const unsigned &n) const = 0;

  /// \short Return vector of pointers to the Data objects that store the 
  /// edge flux values
  virtual Vector<Data*> q_edge_data_pt() const=0;

  /// Return pointer to the Data object that stores the internal flux values
  virtual Data* q_internal_data_pt() const=0;

  /// Return the nodal index at which the nth edge unknown is stored
  virtual unsigned q_edge_index(const unsigned &n) const = 0;

  /// \short Return the index of the internal data where the q_internal degrees
  /// of freedom are stored
  virtual unsigned q_internal_index() const = 0;

  /// Return the number of the node where the nth edge unknown is stored
  virtual unsigned q_edge_node_number(const unsigned &n) const = 0;

  /// Return the values of the n-th edge (flux) degree of freedom
  virtual double q_edge(const unsigned &n) const = 0;

  /// Return the face index associated with edge flux degree of freedom
  virtual unsigned face_index_of_q_edge_basis_fct(const unsigned& j) const=0;

  /// Return the face index associated with specified edge 
  virtual unsigned face_index_of_edge(const unsigned& j) const=0;

  /// \short Compute the face element coordinates of the nth flux interpolation 
  /// point along an edge
  virtual void face_local_coordinate_of_flux_interpolation_point(
   const unsigned &edge,
   const unsigned &n,
   Vector<double> &s) const=0; 
  
  /// Return the values of the internal degree of freedom
  virtual double q_internal(const unsigned &n) const = 0;

  /// Set the values of the edge (flux) degree of freedom
  virtual void set_q_edge(const unsigned &n, const double& value)=0;

  /// Set the values of the internal degree of freedom
  virtual void set_q_internal(const unsigned &n, const double& value)=0;

  /// Return the total number of computational basis functions for q
  unsigned nq_basis() const
  {
   return nq_basis_edge()+nq_basis_internal();
  }

  /// Return the number of edge basis functions for q
  virtual unsigned nq_basis_edge() const = 0;

  /// Return the number of internal basis functions for q
  virtual unsigned nq_basis_internal() const = 0;

  /// Returns the local form of the q basis at local coordinate s
  virtual void get_q_basis_local(const Vector<double> &s,
                                 Shape &q_basis) const = 0;

  /// Returns the local form of the q basis and dbasis/ds at local coordinate s
  virtual void get_div_q_basis_local(const Vector<double> &s,
                                     Shape &div_q_basis_ds) const = 0;

  /// Returns the transformed basis at local coordinate s
  void get_q_basis(const Vector<double> &s,
                   Shape &q_basis) const
   {
    const unsigned n_node = this->nnode();
    Shape psi(n_node,DIM);
    const unsigned n_q_basis = this->nq_basis();
    Shape q_basis_local(n_q_basis,DIM);
    this->get_q_basis_local(s,q_basis_local);
    (void)this->transform_basis(s,q_basis_local,psi,q_basis);
   }

  /// \short Returns the number of flux interpolation points along each
  /// edge of the element
  virtual unsigned nedge_flux_interpolation_point() const = 0;

  /// \short Compute the global coordinates of the flux_interpolation
  /// point associated with the j-th edge-based q basis fct
  virtual void edge_flux_interpolation_point_global(const unsigned &j, 
                                       Vector<double> &x) const=0;
 

  /// Returns the local coordinate of nth flux interpolation point along an edge
  virtual Vector<double> edge_flux_interpolation_point(const unsigned &edge,
                                                       const unsigned &n)
   const=0;
  
  /// \short Returns the global coordinates of the nth flux 
  /// interpolation point along an edge
  virtual void edge_flux_interpolation_point_global(const unsigned &edge,
                                                    const unsigned &n,
                                                    Vector<double> &x) const=0;
  
  /// Pin the nth internal q value
  virtual void pin_q_internal_value(const unsigned &n) = 0;

  /// Return the equation number of the n-th pressure degree of freedom
  virtual int p_local_eqn(const unsigned &n) const = 0;

  /// Return the nth pressure value
  virtual double p_value(const unsigned &n) const = 0;

  /// Return the total number of pressure basis functions
  virtual unsigned np_basis() const = 0;

  /// Return the pressure basis
  virtual void get_p_basis(const Vector<double> &s,
                           Shape &p_basis) const = 0;

  /// Pin the nth pressure value
  virtual void pin_p_value(const unsigned &n) = 0;

  /// Set the nth pressure value
  virtual void set_p_value(const unsigned &n, const double& value)=0;

  /// Return pointer to the Data object that stores the pressure values
  virtual Data* p_data_pt() const=0;

  /// Scale the edge basis to allow arbitrary edge mappings
  virtual void scale_basis(Shape& basis) const = 0;

  /// \short Performs a div-conserving transformation of the vector basis
  /// functions from the reference element to the actual element
  double transform_basis(const Vector<double> &s,
                         const Shape &q_basis_local,
                         Shape &psi,
                         Shape &q_basis) const;

  /// Fill in contribution to residuals for the Darcy equations
  void fill_in_contribution_to_residuals(Vector<double> &residuals)
   {
    this->fill_in_generic_residual_contribution(
      residuals,GeneralisedElement::Dummy_matrix,0);
   }

  //mjr do finite differences for now
  //void fill_in_contribution_to_jacobian(Vector<double> &residuals,
  //                                      DenseMatrix<double> &jacobian)
  // {
  //  this->fill_in_generic_residual_contribution(residuals,jacobian,1);
  // }

  /// Calculate the FE representation of q
  void interpolated_q(const Vector<double> &s,
                      Vector<double> &q) const
   {
    unsigned n_q_basis = nq_basis();
    unsigned n_q_basis_edge = nq_basis_edge();

    Shape q_basis(n_q_basis,DIM);

    get_q_basis(s,q_basis);
    for(unsigned i=0;i<DIM;i++)
     {
      q[i]=0.0;
      for(unsigned l=0;l<n_q_basis_edge;l++)
       {
        q[i] += q_edge(l)*q_basis(l,i);
       }
      for(unsigned l=n_q_basis_edge;l<n_q_basis;l++)
       {
        q[i] += q_internal(l-n_q_basis_edge)*q_basis(l,i);
       }
     }
   }

  /// Calculate the FE representation of the i-th component of q
  double interpolated_q(const Vector<double> &s,
                        const unsigned i) const
   {
    unsigned n_q_basis = nq_basis();
    unsigned n_q_basis_edge = nq_basis_edge();

    Shape q_basis(n_q_basis,DIM);

    get_q_basis(s,q_basis);
    double q_i=0.0;
    for(unsigned l=0;l<n_q_basis_edge;l++)
     {
      q_i += q_edge(l)*q_basis(l,i);
     }
    for(unsigned l=n_q_basis_edge;l<n_q_basis;l++)
     {
      q_i += q_internal(l-n_q_basis_edge)*q_basis(l,i);
     }

    return q_i;
   }

  /// Calculate the FE representation of div q
  void interpolated_div_q(const Vector<double> &s, double &div_q) const
   {
    // Zero the divergence
    div_q=0;

    // Get the number of nodes, q basis function, and q edge basis functions
    unsigned n_node=nnode();
    const unsigned n_q_basis = nq_basis();
    const unsigned n_q_basis_edge = nq_basis_edge();

    // Storage for the divergence basis
    Shape div_q_basis_ds(n_q_basis);

    // Storage for the geometric basis and it's derivatives
    Shape psi(n_node);
    DShape dpsi(n_node,DIM);

    // Call the geometric shape functions and their derivatives
    this->dshape_local(s,psi,dpsi);

    // Storage for the inverse of the geometric jacobian (just so we can call
    // the local to eulerian mapping)
    DenseMatrix<double> inverse_jacobian(DIM);

    // Get the determinant of the geometric mapping
    double det = local_to_eulerian_mapping(dpsi,inverse_jacobian);

    // Get the divergence basis (wrt local coords) at local coords s
    get_div_q_basis_local(s,div_q_basis_ds);

    // Add the contribution to the divergence from the edge basis functions
    for(unsigned l=0;l<n_q_basis_edge;l++)
     {
      div_q+=1.0/det*div_q_basis_ds(l)*q_edge(l);
     }

    // Add the contribution to the divergence from the internal basis functions
    for(unsigned l=n_q_basis_edge;l<n_q_basis;l++)
     {
      div_q+=1.0/det*div_q_basis_ds(l)*q_internal(l-n_q_basis_edge);
     }
   }

  /// Calculate the FE representation of div q and return it
  double interpolated_div_q(const Vector<double> &s)
   {
    // Temporary storage for div q
    double div_q=0;

    // Get the intepolated divergence
    interpolated_div_q(s,div_q);

    // Return it
    return div_q;
   }

  /// Calculate the FE representation of p
  void interpolated_p(const Vector<double> &s,
                      double &p) const
   {
    // Get the number of p basis functions
    unsigned n_p_basis = np_basis();

    // Storage for the p basis
    Shape p_basis(n_p_basis);

    // Call the p basis
    get_p_basis(s,p_basis);

    // Zero the pressure
    p=0;

    // Add the contribution to the pressure from each basis function
    for(unsigned l=0;l<n_p_basis;l++)
     {
      p+=p_value(l)*p_basis(l);
     }
   }

  /// Calculate the FE representation of p and return it
  double interpolated_p(const Vector<double> &s) const
   {
    // Temporary storage for p
    double p=0;

    // Get the interpolated pressure
    interpolated_p(s,p);

    // Return it
    return p;
   }


  /// \short Helper function to pin superfluous dofs (empty; can be overloaded
  /// in projectable elements where we introduce at least one
  /// dof per node to allow projection during unstructured refinement)
  virtual void pin_superfluous_darcy_dofs(){}


  /// Self test -- empty for now.
  unsigned self_test()
   {
    return 0;
   }

  /// Output with default number of plot points
  void output(std::ostream &outfile)
   {
    unsigned nplot=5;
    output(outfile,nplot);
   }

  /// \short Output FE representation of soln: x,y,q1,q2,div_q,p at
  /// Nplot^DIM plot points
  void output(std::ostream &outfile, const unsigned &nplot);



  /// \short Output incl. projection of fluxes into direction of
  /// the specified unit vector
  void output_with_projected_flux(std::ostream &outfile, const unsigned &nplot, 
                                  const Vector<double> unit_normal);


  /// \short Output FE representation of exact soln: x,y,q1,q2,div_q,p at
  /// Nplot^DIM plot points
  void output_fct(std::ostream &outfile,
                  const unsigned &nplot,
                  FiniteElement::SteadyExactSolutionFctPt exact_soln_pt);

  /// \short Compute the error between the FE solution and the exact solution
  /// using the H(div) norm for q and L^2 norm for p
  void compute_error(std::ostream &outfile,
                     FiniteElement::SteadyExactSolutionFctPt exact_soln_pt,
                     Vector<double>& error,
                     Vector<double>& norm);


  // Z2 stuff:


  /// Number off flux terms for Z2 error estimator (use actual flux)
  unsigned num_Z2_flux_terms()
  {
   return DIM;
  }

  /// Z2 flux (use actual flux)
  void get_Z2_flux(const Vector<double>& s, Vector<double>& flux)
  {
   interpolated_q(s,flux);
  }


 protected:

  /// \short Returns the geometric basis, and the q, p and divergence basis
  /// functions and test functions at local coordinate s
  virtual double shape_basis_test_local(const Vector<double> &s,
                                        Shape &psi,
                                        Shape &q_basis,
                                        Shape &q_test,
                                        Shape &p_basis,
                                        Shape &p_test,
                                        Shape &div_q_basis_ds,
                                        Shape &div_q_test_ds) const = 0;

  /// \short Returns the geometric basis, and the q, p and divergence basis
  /// functions and test functions at integration point ipt
  virtual double shape_basis_test_local_at_knot(const unsigned &ipt,
                                                Shape &psi,
                                                Shape &q_basis,
                                                Shape &q_test,
                                                Shape &p_basis,
                                                Shape &p_test,
                                                Shape &div_q_basis_ds,
                                                Shape &div_q_test_ds) const = 0;

  // fill in residuals and, if flag==true, jacobian
  virtual void fill_in_generic_residual_contribution(
    Vector<double> &residuals,
    DenseMatrix<double> &jacobian,
    bool flag);

 private:

  /// Pointer to body force function
  SourceFctPt Source_fct_pt;

  /// Pointer to the mass source function
  MassSourceFctPt Mass_source_fct_pt;
 };



////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////


//==========================================================
/// Darcy upgraded to become projectable
//==========================================================
 template<class DARCY_ELEMENT>
 class ProjectableDarcyElement : 
  public virtual ProjectableElement<DARCY_ELEMENT>,
  public virtual ProjectableElementBase 
 {

 public:

  /// \short Constructor [this was only required explicitly
  /// from gcc 4.5.2 onwards...]
  ProjectableDarcyElement(){}

  /// \short Specify the values associated with field fld. 
  /// The information is returned in a vector of pairs which comprise 
  /// the Data object and the value within it, that correspond to field fld. 
  Vector<std::pair<Data*,unsigned> > data_values_of_field(const unsigned& fld)
   { 

#ifdef PARANOID
    if (fld>1)
     {
      std::stringstream error_stream;
      error_stream 
       << "Darcy elements only store 2 fields so fld = "
       << fld << " is illegal \n";
      throw OomphLibError(
       error_stream.str(),
       OOMPH_CURRENT_FUNCTION,
       OOMPH_EXCEPTION_LOCATION);
     }
#endif
  
    // Create the vector
    Vector<std::pair<Data*,unsigned> > data_values;
   
    // Pressure
    if (fld==0)
     {
      Data* dat_pt=this->p_data_pt();
      unsigned nvalue=dat_pt->nvalue();
      for (unsigned i=0;i<nvalue;i++)
       {
        data_values.push_back(std::make_pair(dat_pt,i));
       }
     }
    else
     {
      Vector<Data*> edge_dat_pt=this->q_edge_data_pt();
      unsigned n=edge_dat_pt.size();
      for (unsigned j=0;j<n;j++)
       {
        unsigned nvalue=edge_dat_pt[j]->nvalue();
        for (unsigned i=0;i<nvalue;i++)
         {
          data_values.push_back(std::make_pair(edge_dat_pt[j],i));
         }
       }
      if (this->nq_basis_internal()>0)
       {
        Data* int_dat_pt=this->q_internal_data_pt();
        unsigned nvalue=int_dat_pt->nvalue();
        for (unsigned i=0;i<nvalue;i++)
         {
          data_values.push_back(std::make_pair(int_dat_pt,i));
         }
       }
     }
   
    // Return the vector
    return data_values;
   }

  /// \short Number of fields to be projected: 2 (pressure and flux)
  unsigned nfields_for_projection()
   {
    return 2;
   }
 
  /// \short Number of history values to be stored for fld-th field. 
  /// (Note: count includes current value!)
  unsigned nhistory_values_for_projection(const unsigned &fld)
  {
#ifdef PARANOID
    if (fld>1)
     {
      std::stringstream error_stream;
      error_stream 
       << "Darcy elements only store two fields so fld = "
       << fld << " is illegal\n";
      throw OomphLibError(
       error_stream.str(),
       OOMPH_CURRENT_FUNCTION,
       OOMPH_EXCEPTION_LOCATION);
     }
#endif
   return this->node_pt(0)->ntstorage();   
  }
  
  ///\short Number of positional history values
  /// (Note: count includes current value!)
  unsigned nhistory_values_for_coordinate_projection()
   {
    return this->node_pt(0)->position_time_stepper_pt()->ntstorage();
   }
  
  /// \short Return Jacobian of mapping and shape functions of field fld
  /// at local coordinate s
  double jacobian_and_shape_of_field(const unsigned &fld, 
                                     const Vector<double> &s, 
                                     Shape &psi)
   {
#ifdef PARANOID
    if (fld>1)
     {
      std::stringstream error_stream;
      error_stream 
       << "Darcy elements only store two fields so fld = "
       << fld << " is illegal.\n";
      throw OomphLibError(
       error_stream.str(),
       OOMPH_CURRENT_FUNCTION,
       OOMPH_EXCEPTION_LOCATION);
     }
#endif

    
    // Get the number of geometric nodes, total number of basis functions,
    // and number of edges basis functions
    const unsigned n_dim=this->dim();
    const unsigned n_node = this->nnode();
    const unsigned n_q_basis = this->nq_basis();
    const unsigned n_p_basis = this->np_basis();
    
    // Storage for the geometric and computational bases and the test functions
    Shape psi_geom(n_node), q_basis(n_q_basis,n_dim), q_test(n_q_basis,n_dim),
     p_basis(n_p_basis), p_test(n_p_basis),
     div_q_basis_ds(n_q_basis), div_q_test_ds(n_q_basis);
    double J= this->shape_basis_test_local(s,
                                           psi_geom,
                                           q_basis,
                                           q_test,
                                           p_basis,
                                           p_test,
                                           div_q_basis_ds,
                                           div_q_test_ds);
    // Pressure basis functions
    if (fld==0)
     {
      unsigned n=p_basis.nindex1();
      for (unsigned i=0;i<n;i++)
       {
        psi[i]=p_basis[i];
       }
     }
    // Flux basis functions
    else
     {
      unsigned n=q_basis.nindex1();
      unsigned m=q_basis.nindex2();
      for (unsigned i=0;i<n;i++)
       {
        for (unsigned j=0;j<m;j++)
         {
          psi(i,j)=q_basis(i,j);
         }
       }
     }

    return J;
   }



  /// \short Return interpolated field fld at local coordinate s, at time level
  /// t (t=0: present; t>0: history values)
  double get_field(const unsigned &t, 
                   const unsigned &fld,
                   const Vector<double>& s)
   {
#ifdef PARANOID
    if (fld>1)
     {
      std::stringstream error_stream;
      error_stream 
       << "Darcy elements only store two fields so fld = "
       << fld << " is illegal\n";
      throw OomphLibError(
       error_stream.str(),
       OOMPH_CURRENT_FUNCTION,
       OOMPH_EXCEPTION_LOCATION);
     }
#endif

    double return_value=0.0;

    // Pressure
    if (fld==0)
     {
      this->interpolated_p(s,return_value);
     }
    else
     {
      throw OomphLibError(
       "Don't call this function for Darcy!",
       OOMPH_CURRENT_FUNCTION,
       OOMPH_EXCEPTION_LOCATION);
     }
    
    return return_value;
   }


  ///Return number of values in field fld
  unsigned nvalue_of_field(const unsigned &fld)
   {
#ifdef PARANOID
    if (fld>1)
     {
      std::stringstream error_stream;
      error_stream 
       << "Darcy elements only store two fields so fld = "
       << fld << " is illegal\n";
      throw OomphLibError(
       error_stream.str(),
       OOMPH_CURRENT_FUNCTION,
       OOMPH_EXCEPTION_LOCATION);
     }
#endif

    unsigned return_value=0;
    if (fld==0)
     {
      return_value=this->np_basis();
     }
    else
     {
      return_value=this->nq_basis();
     }

    return return_value;
   }

 
  ///Return local equation number of value j in field fld.
  int local_equation(const unsigned &fld,
                     const unsigned &j)
   {
#ifdef PARANOID
    if (fld>1)
     {
      std::stringstream error_stream;
      error_stream 
       << "Darcy elements only store two fields so fld = "
       << fld << " is illegal\n";
      throw OomphLibError(
       error_stream.str(),
       OOMPH_CURRENT_FUNCTION,
       OOMPH_EXCEPTION_LOCATION);
     }
#endif

    int return_value=0;
    // Pressure
    if (fld==0)
     {
      return_value=this->p_local_eqn(j);
     }
    else
     {
      unsigned nedge=this->nq_basis_edge();
      if (j<nedge)
       {
        return_value=this->q_edge_local_eqn(j);
       }
      else
       {
        return_value=this->q_internal_local_eqn(j-nedge);
       }
     }

    return return_value;
   }
  
  /// \short Output FE representation of soln as in underlying element
  void output(std::ostream &outfile, const unsigned &nplot)
  {
   DARCY_ELEMENT::output(outfile,nplot);
  }
   
  /// Overload required_nvalue to return at least one value
  unsigned required_nvalue(const unsigned &n) const
  {
   return std::max(this->Initial_Nvalue[n],unsigned(1));
  }
    

  /// \short Helper function to pin superfluous dofs; required because
  /// we introduce at least one dof per node to allow projection 
  /// during unstructured refinement)
  void pin_superfluous_darcy_dofs()
   {
    // Pin dofs at vertex nodes (because they're only used for projection)
    for(unsigned j=0;j<3;j++)
     {
      this->node_pt(j)->pin(0);
     }
   }

  /// \short Residual for the projection step. Flag indicates if we
  /// want the Jacobian (1) or not (0). Virtual so it can be 
  /// overloaded if necessary
  void residual_for_projection(Vector<double> &residuals, 
                               DenseMatrix<double> &jacobian, 
                               const unsigned& flag)
   {
    //Am I a solid element
    SolidFiniteElement* solid_el_pt = 
     dynamic_cast<SolidFiniteElement*>(this);
    
   unsigned n_dim=this->dim();
   
   //Allocate storage for local coordinates
   Vector<double> s(n_dim);

   //Current field
   unsigned fld=this->Projected_field;
    
   //Number of nodes
   const unsigned n_node = this->nnode();
   //Number of positional dofs
   const unsigned n_position_type = this->nnodal_position_type();
   
   //Number of dof for current field
   const unsigned n_value=nvalue_of_field(fld);  

   //Set the value of n_intpt
   const unsigned n_intpt = this->integral_pt()->nweight();
   
   //Loop over the integration points
   for(unsigned ipt=0;ipt<n_intpt;ipt++)
    {
     // Get the local coordinates of Gauss point
     for(unsigned i=0;i<n_dim;i++) s[i] = this->integral_pt()->knot(ipt,i);

     //Get the integral weight
     double w = this->integral_pt()->weight(ipt);

     // Find same point in base mesh using external storage
     FiniteElement* other_el_pt=0;
     other_el_pt=this->external_element_pt(0,ipt);
     Vector<double> other_s(n_dim);
     other_s=this->external_element_local_coord(0,ipt);

     ProjectableElement<DARCY_ELEMENT>* cast_el_pt = 
      dynamic_cast<ProjectableElement<DARCY_ELEMENT>*>(other_el_pt);

     //Storage for the local equation and local unknown
     int local_eqn=0, local_unknown=0;
     
     //Now set up the three different projection problems
     switch(Projection_type)
      {
      case Lagrangian:
      {
       //If we don't have a solid element there is a problem
       if(solid_el_pt==0)
        {
         throw OomphLibError(
          "Trying to project Lagrangian coordinates in non-SolidElement\n",
          OOMPH_CURRENT_FUNCTION,
          OOMPH_EXCEPTION_LOCATION);
        }

       //Find the position shape functions
       Shape psi(n_node,n_position_type);
       //Get the position shape functions
       this->shape(s,psi);
       //Get the jacobian
       double J = this->J_eulerian(s);

       //Premultiply the weights and the Jacobian
       double W = w*J;

       //Get the value of the current position of the 0th coordinate
       //in the current element
       //at the current time level (which is the unkown)
       double interpolated_xi_proj = this->interpolated_x(s,0);
       
       //Get the Lagrangian position in the other element
       double interpolated_xi_bar=
        dynamic_cast<SolidFiniteElement*>(cast_el_pt)
        ->interpolated_xi(other_s,Projected_lagrangian);
       
       //Now loop over the nodes and position dofs
       for(unsigned l=0;l<n_node;++l)
        {
         //Loop over position unknowns
         for(unsigned k=0;k<n_position_type;++k)
          {   
           //The local equation is going to be the positional local equation
           local_eqn = solid_el_pt->position_local_eqn(l,k,0);
           
           //Now assemble residuals
           if(local_eqn >= 0)
            {
             //calculate residuals
             residuals[local_eqn] += 
              (interpolated_xi_proj - interpolated_xi_bar)*psi(l,k)*W;
             
             //Calculate the jacobian
             if(flag==1)
              {
               for(unsigned l2=0;l2<n_node;l2++)
                {
                 //Loop over position dofs
                 for(unsigned k2=0;k2<n_position_type;k2++)
                  {
                   local_unknown = 
                    solid_el_pt->position_local_eqn(l2,k2,0);
                    if(local_unknown >= 0)
                     {
                      jacobian(local_eqn,local_unknown) 
                      += psi(l2,k2)*psi(l,k)*W;  
                     }
                  }
                }
              } //end of jacobian
            }
          }
        }
      } //End of Lagrangian coordinate case
      
      break;

      //Now the coordinate history case
      case Coordinate:
      {
       //Find the position shape functions
       Shape psi(n_node,n_position_type);
       //Get the position shape functions
       this->shape(s,psi);
       //Get the jacobian
       double J = this->J_eulerian(s);

       //Premultiply the weights and the Jacobian
       double W = w*J;

       //Get the value of the current position in the current element
       //at the current time level (which is the unkown)
       double interpolated_x_proj = 0.0;
       //If we are a solid element read it out directly from the data
       if(solid_el_pt!=0)
        {
         interpolated_x_proj = this->interpolated_x(s,Projected_coordinate);
        }
       //Otherwise it's the field value at the current time
       else
        {
         interpolated_x_proj = this->get_field(0,fld,s);
        }

       //Get the position in the other element
       double interpolated_x_bar=
        cast_el_pt->interpolated_x(Time_level_for_projection,
                                   other_s,Projected_coordinate);

       //Now loop over the nodes and position dofs
       for(unsigned l=0;l<n_node;++l)
        {
         //Loop over position unknowns
         for(unsigned k=0;k<n_position_type;++k)
          {   
           //If I'm a solid element
           if(solid_el_pt!=0)
            {
             //The local equation is going to be the positional local equation
             local_eqn = 
              solid_el_pt->position_local_eqn(l,k,Projected_coordinate);
            }
           //Otherwise just pick the local unknown of a field to
           //project into
           else
            {
             //Complain if using generalised position types
             //but this is not a solid element, because the storage
             //is then not clear
             if(n_position_type!=1)
              {
               throw OomphLibError(
                "Trying to project generalised positions not in SolidElement\n",
                OOMPH_CURRENT_FUNCTION,
                OOMPH_EXCEPTION_LOCATION);
              }
             local_eqn = local_equation(fld,l);
            }
           
           //Now assemble residuals
           if(local_eqn >= 0)
            {
             //calculate residuals
             residuals[local_eqn] += 
              (interpolated_x_proj - interpolated_x_bar)*psi(l,k)*W;
             
             //Calculate the jacobian
             if(flag==1)
              {
               for(unsigned l2=0;l2<n_node;l2++)
                {
                 //Loop over position dofs
                 for(unsigned k2=0;k2<n_position_type;k2++)
                  {
                   //If I'm a solid element
                   if(solid_el_pt!=0)
                    {
                     local_unknown = solid_el_pt
                      ->position_local_eqn(l2,k2,Projected_coordinate);
                    }
                   else
                   {
                    local_unknown = local_equation(fld,l2);
                   }
                   
                   if(local_unknown >= 0)
                    {
                     jacobian(local_eqn,local_unknown) 
                      += psi(l2,k2)*psi(l,k)*W;  
                    }
                  }
                }
              } //end of jacobian
            }
          }
        }
      } //End of coordinate case
      break;

      //Now the value case
      case Value:
      {
       
       // Pressure -- "normal" procedure
       if (fld==0)
        {
         //Field shape function
         Shape psi(n_value);
         
         //Calculate jacobian and shape functions for that field
         double J=jacobian_and_shape_of_field(fld,s,psi);
         
         //Premultiply the weights and the Jacobian
         double W = w*J;
         
         //Value of field in current element at current time level 
         //(the unknown)
         unsigned now=0;
         double interpolated_value_proj = this->get_field(now,fld,s);
         
         //Value of the interpolation of element located in base mesh
         double interpolated_value_bar = 
          cast_el_pt->get_field(Time_level_for_projection,fld,other_s);
         
         //Loop over dofs of field fld
         for(unsigned l=0;l<n_value;l++)
          {
           local_eqn = local_equation(fld,l);      
           if(local_eqn >= 0)
            {
             //calculate residuals
             residuals[local_eqn] += 
              (interpolated_value_proj - interpolated_value_bar)*psi[l]*W;
             
             //Calculate the jacobian
             if(flag==1)
              {
               for(unsigned l2=0;l2<n_value;l2++)
                {
                 local_unknown = local_equation(fld,l2);
                 if(local_unknown >= 0)
                  {
                   jacobian(local_eqn,local_unknown) 
                    += psi[l2]*psi[l]*W;  
                  }
                }
              } //end of jacobian
            }
          }
        }
       // Flux -- need inner product
       else if (fld==1)
        {

         //Field shape function
         Shape psi(n_value,n_dim);
         
         //Calculate jacobian and shape functions for that field
         double J=jacobian_and_shape_of_field(fld,s,psi);
         
         //Premultiply the weights and the Jacobian
         double W = w*J;
         
         //Value of flux in current element at current time level 
         //(the unknown)
         Vector<double> q_proj(n_dim);
         this->interpolated_q(s,q_proj);
         
         //Value of the interpolation of element located in base mesh
         Vector<double> q_bar(n_dim);
         cast_el_pt->interpolated_q(other_s,q_bar);
         
#ifdef PARANOID
         if (Time_level_for_projection!=0)
          {
           std::stringstream error_stream;
           error_stream 
            << "Darcy elements are steady!\n";
           throw OomphLibError(
            error_stream.str(),
            OOMPH_CURRENT_FUNCTION,
            OOMPH_EXCEPTION_LOCATION);
          }
#endif

         //Loop over dofs of field fld
         for(unsigned l=0;l<n_value;l++)
          {
           local_eqn = local_equation(fld,l);      
           if(local_eqn >= 0)
            {
             // Loop over spatial dimension for dot product
             for (unsigned i=0;i<n_dim;i++)
              {
               // Add to residuals
               residuals[local_eqn] += 
                (q_proj[i] - q_bar[i])*psi(l,i)*W;
             
               //Calculate the jacobian
               if(flag==1)
                {
                 for(unsigned l2=0;l2<n_value;l2++)
                  {
                   local_unknown = local_equation(fld,l2);
                   if(local_unknown >= 0)
                    {
                     jacobian(local_eqn,local_unknown) 
                      += psi(l2,i)*psi(l,i)*W;  
                    }
                  }
                } //end of jacobian
              }
            }
          }
        }
       else
        {
         throw OomphLibError(
          "Wrong field specified. This should never happen\n",
          OOMPH_CURRENT_FUNCTION,
          OOMPH_EXCEPTION_LOCATION);
        }
       
       
       break;
       
      default:
       throw OomphLibError(
        "Wrong type specificied in Projection_type. This should never happen\n",
        OOMPH_CURRENT_FUNCTION,
        OOMPH_EXCEPTION_LOCATION);
      }
      } //End of the switch statement
     
    }//End of loop over ipt
   
   }//End of residual_for_projection function
   
 };


//=======================================================================
/// Face geometry for element is the same as that for the underlying
/// wrapped element
//=======================================================================
 template<class ELEMENT>
 class FaceGeometry<ProjectableDarcyElement<ELEMENT> > 
  : public virtual FaceGeometry<ELEMENT>
 {
 public:
  FaceGeometry() : FaceGeometry<ELEMENT>() {}
 };



////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////


} // end of oomph-lib namespace

#endif

