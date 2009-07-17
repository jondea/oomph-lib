//LIC// ====================================================================
//LIC// This file forms part of oomph-lib, the object-oriented, 
//LIC// multi-physics finite-element library, available 
//LIC// at http://www.oomph-lib.org.
//LIC// 
//LIC//           Version 0.85. June 9, 2008.
//LIC// 
//LIC// Copyright (C) 2006-2008 Matthias Heil and Andrew Hazel
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
//Header file for the class
//ElementWithExternalElement which stores pointers to external elements

//Include guards to prevent multiple inclusion of the header
#ifndef OOMPH_ELEMENT_WITH_EXTERNAL_ELEMENT_HEADER
#define OOMPH_ELEMENT_WITH_EXTERNAL_ELEMENT_HEADER

// Config header generated by autoconfig
#ifdef HAVE_CONFIG_H
#include <oomph-lib-config.h>
#endif

//Oomph-lib headers
#include "elements.h"

namespace oomph
{
//========================================================================
/// This is a base class for all elements that require external sources
/// (e.g. FSI, multi-domain problems such as Helmholtz, multi-mesh
///  Boussinesq convection, etc.).  It provides storage for the source
/// element and corresponding local coordinate at each integration point,
/// and allows use of locate_zeta to obtain such source elements. In 
/// addition separate storage is allocated for all field data in the 
/// external elements and all geometric data that can affect the field
/// data in the external elements. Generic finite difference routines 
/// are provided to calculate entries in the Jacobian from the data 
/// of the external elements.
//========================================================================
class ElementWithExternalElement : public virtual FiniteElement
  {
    public:

   /// Constructor. Initialise member data and pointers to data associated
   /// with the external elements to zero.
   ElementWithExternalElement() : FiniteElement(),
    Add_external_interaction_data(true),  
    Add_external_geometric_data(true),  
    Ninteraction(0),
    Nintpt(0),
    Nexternal_element_storage(0),
    Nexternal_interaction_field_data(0),
    Nexternal_interaction_geometric_data(0),
    External_element_pt(0),
    External_element_local_coord(0),
    External_interaction_field_data_pt(0),
    External_interaction_field_data_index(0),
    External_interaction_field_data_local_eqn(0),
    External_interaction_geometric_data_pt(0),
    External_interaction_geometric_data_index(0),
    External_interaction_geometric_data_local_eqn(0)
    { }
   
   /// \short The destructor, clean up any allocated memory 
   virtual ~ElementWithExternalElement();
   
   /// Broken copy constructor
   ElementWithExternalElement(const ElementWithExternalElement&) 
    { 
     BrokenCopy::broken_copy("ElementWithExternalElement");
    } 
 
   /// Broken assignment operator
   void operator=(const ElementWithExternalElement&) 
    {
     BrokenCopy::broken_assign("ElementWithExternalElement");
    }
   
   /// \short Access function to source element for specified interaction
   /// index at specified integration point
   FiniteElement*& external_element_pt(const unsigned &interaction_index,
                                       const unsigned &ipt)
    {
#ifdef PARANOID
     check_storage_allocated();
#endif
#ifdef RANGE_CHECKING
     range_check(interaction_index,ipt);
#endif     
     //Return the appropriate entry in the storage array
     return External_element_pt[Nintpt*interaction_index + ipt];
    }
   
   /// \short Access function to source element, const version
   FiniteElement* const &external_element_pt(const unsigned &interaction_index,
                                             const unsigned &ipt) const
    {
#ifdef PARANOID
     check_storage_allocated();
#endif
#ifdef RANGE_CHECKING
     range_check(interaction_index,ipt);
#endif     
     //Return the appropriate entry in the storage array
     return External_element_pt[Nintpt*interaction_index + ipt];
    }

   /// \short Access function to get source element's local coords for
   /// specified interaction index at specified integration point
   Vector<double>& external_element_local_coord(
    const unsigned &interaction_index,
    const unsigned &ipt)
    {
#ifdef PARANOID
     check_storage_allocated();
#endif
#ifdef RANGE_CHECKING
     range_check(interaction_index,ipt);
#endif     
     return External_element_local_coord[Nintpt*interaction_index + ipt];
    }

   /// \short Access function to get source element's coords, const version
   Vector<double> const &external_element_local_coord
    (const unsigned &interaction_index, const unsigned &ipt) const
    {
#ifdef PARANOID
     check_storage_allocated();
#endif
#ifdef RANGE_CHECKING
     range_check(interaction_index,ipt);
#endif     
     return External_element_local_coord[Nintpt*interaction_index + ipt];
    }


   /// \short Initialise storage for pointers to external elements and their
   /// local coordinates. This must be called before any of
   /// the access functions are used.
   void initialise_external_element_storage();

   /// \short Flush the storage for external elements
   void flush_all_external_element_storage();

   /// \short Set the number of interactions in the element
   /// This function is usually called in the specific element's constructor
   inline void set_ninteraction(const unsigned &n_interaction) 
    {Ninteraction = n_interaction;}
   
   /// \short Return the number of interactions in the element
   unsigned ninteraction() const {return Ninteraction;}

   /// \short Function that must return all the data involved
   /// in the desired interactions from the external element 
   virtual void identify_all_field_data_for_external_interaction(
    Vector<std::set<FiniteElement*> >  const &external_elements_pt,
    std::set<std::pair<Data*,unsigned> > &paired_interaction_data); 

   /// \short Function that must return all geometric data involved 
   /// in the desired interactions from the external element
   virtual void identify_all_geometric_data_for_external_interaction(
    Vector<std::set<FiniteElement*> >  const &external_elements_pt,
    std::set<Data*> &external_geometric_data_pt);
   
   /// \short Return the number of Data items that affect the 
   /// external interactions in this element.
   /// This includes e.g. fluid velocities
   /// and pressures in adjacent fluid elements in an FSI problem
   unsigned nexternal_interaction_field_data() const 
    {return Nexternal_interaction_field_data;}
   
   /// \short Return vector of pointers to the field Data objects that 
   /// affect the interactions on the element. 
   Vector<Data*> external_interaction_field_data_pt() const 
    {
     //Find out how many external field data there are
     const unsigned n_external_interaction_field_data =
      nexternal_interaction_field_data();
     //Create and populate  a temporary vector
     Vector<Data*> temp_data(n_external_interaction_field_data);
     for(unsigned i=0;i<n_external_interaction_field_data;i++)
      {temp_data[i] = External_interaction_field_data_pt[i];}
     //Return the temporary data
     return temp_data;
    }
       
   
   /// \short Return the number of geometric Data items that affect the 
   /// external interactions in this element: i.e. any geometric Data
   /// in the problem that affects the nodal positions in the
   /// external elements. 
   unsigned nexternal_interaction_geometric_data() const 
    {return Nexternal_interaction_geometric_data;}
   
   /// \short Return vector of pointers to the geometric Data objects that 
   /// affect the interactions on the element. 
   Vector<Data*> external_interaction_geometric_data_pt() const 
    {
     //Find out how many external field data there are
     const unsigned n_external_interaction_geometric_data =
      nexternal_interaction_geometric_data();
     //Create and populate  a temporary vector
     Vector<Data*> temp_data(n_external_interaction_geometric_data);
     for(unsigned i=0;i<n_external_interaction_geometric_data;i++)
      {temp_data[i] = External_interaction_geometric_data_pt[i];}
     //Return the temporary data
     return temp_data;
    }

   /// Do not include any external geometric data. For efficiency, 
   /// this function should be
   /// called if the external element does not move.
   void ignore_external_geometric_data() {Add_external_geometric_data = false;}
     
   /// Do include external geometric data. This function should be set
   /// to re-enable inclusion of external geometric data
   void include_external_geometric_data() {Add_external_geometric_data = true;}

   /// \short Is the external geometric data taken into account when forming
   /// the Jacobian?
   bool external_geometric_data_is_included() const
   {return Add_external_geometric_data;}
   
    protected:

   /// \short Overload the assign internal and external local equation
   /// number scheme so that the interaction data is taken into account
   void assign_internal_and_external_local_eqn_numbers()
    {
     //Call the external stuff first so that it is near the front of the 
     //list for fast searches when using indexing by global dof for 
     //analytic calculation of interaction blocks.
     this->assign_external_interaction_data_local_eqn_numbers();
     //Now call the underlying equation numbering
     GeneralisedElement::assign_internal_and_external_local_eqn_numbers();
    }
   
   /// \short Assign the local equation numbers for those 
   /// Data values involved in the external interactions that 
   /// affect the residuals of the element
   void assign_external_interaction_data_local_eqn_numbers();


   /// \short Calculate the contributions to the jacobian from all external
   /// interaction degrees of freedom (geometric and field data) in
   /// the external element using finite differences.
   /// This version of the function assumes that the residuals vector has 
   /// already been calculated. 
   void fill_in_jacobian_from_external_interaction_by_fd(
    Vector<double> &residuals, DenseMatrix<double> &jacobian)
    {
     //Get the contribution from the external field data
     fill_in_jacobian_from_external_interaction_field_by_fd(residuals,
                                                            jacobian);
     //Get the contribution from the external geometric data
     fill_in_jacobian_from_external_interaction_geometric_by_fd(residuals,
                                                                jacobian);
    }

   /// \short Calculate the contributions to the jacobian from all enternal
   /// interaction degrees of freedom (geometric and field data) in 
   /// the external element using finite differences. This version computes
   /// the residuals vector before calculating the jacobian terms.
   void fill_in_jacobian_from_external_interaction_by_fd(
    DenseMatrix<double> &jacobian)
    {
     const unsigned n_dof = this->ndof();
     //Allocate storage for the residuals
     Vector<double> residuals(n_dof,0.0);
     //Get the residuals for the entire element
     get_residuals(residuals);
     //Call the jacobian calculation
     fill_in_jacobian_from_external_interaction_by_fd(residuals,
                                                      jacobian);
    }
   
   
   /// \short Calculate the contributions to the jacobian from the external
   /// interaction degrees of freedom associated with fields interpolated by 
   /// the external element using finite differences.
   /// This version of the function assumes that the residuals vector has 
   /// already been calculated. 
   void fill_in_jacobian_from_external_interaction_field_by_fd(
    Vector<double> &residuals, DenseMatrix<double> &jacobian);
   
   /// \short Calculate the contributions to the jacobian from the enternal
   /// interaction degrees of freedom associated with fields interpolated by 
   /// the external element using finite differences. This version computes
   /// the residuals vector before calculating the jacobian terms.
   void fill_in_jacobian_from_external_interaction_field_by_fd(
    DenseMatrix<double> &jacobian)
    {
     const unsigned n_dof = this->ndof();
     //Allocate storage for the residuals
     Vector<double> residuals(n_dof,0.0);
     //Get the residuals for the entire element
     get_residuals(residuals);
     //Call the jacobian calculation
     fill_in_jacobian_from_external_interaction_field_by_fd(residuals,
                                                            jacobian);
    }
  
   /// \short Calculate the contributions to the jacobian from the external
   /// interaction degrees of freedom associated with the geometry of
   /// the external elements using finite differences. 
   /// This version of the function assumes that the residuals vector has 
   /// already been calculated. 
   void fill_in_jacobian_from_external_interaction_geometric_by_fd(
    Vector<double> &residuals, DenseMatrix<double> &jacobian);
   
   /// \short Calculate the contributions to the jacobian from the 
   /// external
   /// interaction degrees of freedom associated with the geometry of
   /// the external elements using finite differences. This version computes
   /// the residuals vector before calculating the jacobian terms.
   void fill_in_jacobian_from_external_interaction_geometric_by_fd(
    DenseMatrix<double> &jacobian)
    {
     const unsigned n_dof = this->ndof();
     //Allocate storage for the residuals
     Vector<double> residuals(n_dof,0.0);
     //Get the residuals for the entire element
     get_residuals(residuals);
     //Call the jacobian calculation
     fill_in_jacobian_from_external_interaction_geometric_by_fd(residuals,
                                                                jacobian);
    }

   /// Fill in the element's contribution to the Jacobian matrix
   /// and the residual vector: Done by finite differencing the
   /// residual vector w.r.t. all nodal, internal, external and load Data.
   void fill_in_contribution_to_jacobian(Vector<double> &residuals,
                                         DenseMatrix<double> &jacobian)
    {
     //Add the contribution to the residuals
     fill_in_contribution_to_residuals(residuals);
     //Allocate storage for the full residuals  (residuals of entire element)
     const unsigned n_dof = this->ndof();
     Vector<double> full_residuals(n_dof);
     //Get the residuals for the entire element
     get_residuals(full_residuals);
     //Add the internal and external by finite differences
     fill_in_jacobian_from_internal_by_fd(full_residuals,jacobian);
     fill_in_jacobian_from_external_by_fd(full_residuals,jacobian);
     fill_in_jacobian_from_nodal_by_fd(full_residuals,jacobian);
     fill_in_jacobian_from_external_interaction_by_fd(full_residuals,jacobian);
    }


   
   /// \short Function that is called before the finite differencing of
   /// any external interaction data associated with external fields. 
   /// This may be overloaded to update any slaved
   /// data before finite differencing takes place.
   virtual inline void update_before_external_interaction_field_fd() { }
   
   /// \short Function that is call after the finite differencing of
   /// the external interaction data associated with external fields
   /// This may be overloaded to reset any slaved
   /// variables that may have changed during the finite differencing.
   virtual inline void reset_after_external_interaction_field_fd() { }
   
   /// \short Function called within the finite difference loop for 
   /// external interaction data after a change in any values in the i-th 
   /// external interaction data object associated with external fields.
   virtual inline 
    void update_in_external_interaction_field_fd(const unsigned &i) { }
   
   /// \short Function called within the finite difference loop for
   /// external interaction data after the values in the 
   /// i-th external interaction data object associated with external fields
   /// are reset. The default behaviour is to call the update function.
   virtual inline 
    void reset_in_external_interaction_field_fd(const unsigned &i)
    {update_in_external_interaction_field_fd(i);}
   

    /// \short Function that is called before the finite differencing of
   /// any external interaction data associated with external geometry. 
   /// This may be overloaded to update any slaved
   /// data before finite differencing takes place.
   virtual inline void update_before_external_interaction_geometric_fd() { }
   
   /// \short Function that is call after the finite differencing of
   /// the external interaction data associated with external geometry.
   /// This may be overloaded to reset any slaved
   /// variables that may have changed during the finite differencing.
   virtual inline void reset_after_external_interaction_geometric_fd() { }
   
   /// \short Function called within the finite difference loop for 
   /// external interaction data after a change in any values in the i-th 
   /// external interaction data object associated with external geometry.
   virtual inline 
    void update_in_external_interaction_geometric_fd(const unsigned &i) { }
   
   /// \short Function called within the finite difference loop for
   /// external interaction data after the values in the 
   /// i-th external interaction data object associated with external geometry
   /// are reset. The default behaviour is to call the update function.
   virtual inline 
    void reset_in_external_interaction_geometric_fd(const unsigned &i)
    {update_in_external_interaction_geometric_fd(i);}

   ///Boolean flag to indicate whether to include the external data
   bool Add_external_interaction_data;

   ///Boolean flag to indicate whether to include the external geometric data
   bool Add_external_geometric_data;
   
    private:

   /// \short Helper function to check that storage has actually been allocated
   void check_storage_allocated() const
    {
     //If either of the storage arrays is zero, then storage has not
     //yet been allocated. Both arrays are allocated at once, so 
     //if one is zero both will (should) be
     if((External_element_pt==0) || (External_element_local_coord==0))
        {
         std::ostringstream error_stream;
         error_stream
          << "Storage for the external elements has not been allocated.\n"
          << "initialise_external_element_storage() must be called\n"
          << "followed by a function that calls set_external_storage()\n";
         
         throw OomphLibError(
          error_stream.str(),
          "ElementWithExternalElement::check_storage_allocated()",
          OOMPH_EXCEPTION_LOCATION);
        }
    }

   /// Helper function for range checking in the access functions
   void range_check(const unsigned& interaction_index, const unsigned& ipt)
    const
    {
     //Boolean flag used to indicate range error
     bool range_error = false;
     //String for the error message
     std::ostringstream error_message;
     //If there is a range error in the interaction index
     if(interaction_index >= Ninteraction)  
      {
       error_message << "Range Error: Interaction " << interaction_index
                     << " is not in the range (0,"
                     << Ninteraction-1 << ")";
       //There has been a range error
       range_error=true;
      }

     //If there is a range error in the integration point
     if(ipt >= Nintpt) 
      {
       error_message << "Range Error: Integration point " << ipt
                     << " is not in the range (0,"
                     << Nintpt-1 << ")";
       range_error=true;
      }


     //If there has been a range error report it
     if(range_error)
      {
       throw OomphLibError(error_message.str(),
                           "ElementWithExternalElement::range_check()",
                           OOMPH_EXCEPTION_LOCATION);
      }
    }

   /// Number of interactions
   unsigned Ninteraction;

   /// Number of intergation point in the element
   unsigned Nintpt;
   
   /// \short Number of entries in the external element storage schemes
   /// (Nintergation_pt * Ninteraction)
   unsigned Nexternal_element_storage;

   /// Number of external interaction field data
   unsigned Nexternal_interaction_field_data;

   /// Number of external interaction geometric data
   unsigned Nexternal_interaction_geometric_data;

 
   /// \short Storage for pointers to elements that provide contributions
   /// to the residuals of the current element. Potentially a different 
   /// element contributes to each integration point.
   FiniteElement **External_element_pt;

   /// \short  Storage for vectors of local coordinates in
   /// external elements that correspond to the appropriate integration 
   //// point.
   Vector<double> *External_element_local_coord;

   //// Storage for pointers to external field Data that affect the 
   /// interactions in the elemenet
   Data** External_interaction_field_data_pt;
    
   /// \short Storage for the index of the values in the external field data
   /// that affect the interactions in the element
   unsigned* External_interaction_field_data_index;
   
   /// \short Storage for the local equation number associated with the 
   /// external field data the affect the interactions in the element
   int* External_interaction_field_data_local_eqn;
 
   //// Storage for pointers to external geometric Data that affect the 
   /// interactions in the elemenet
   Data** External_interaction_geometric_data_pt;
    
   /// \short Storage for the index of the values in the external 
   /// geometric data
   /// that affect the interactions in the element
   unsigned* External_interaction_geometric_data_index;
   
   /// \short Storage for the local equation number associated with the 
   /// external geometric data the affect the interactions in the element
   int* External_interaction_geometric_data_local_eqn;

  };
}

#endif
