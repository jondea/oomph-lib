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
//Header file for classes that define refineable element objects

//Include guard to prevent multiple inclusions of the header
#ifndef OOMPH_REFINEABLE_ELEMENTS_HEADER
#define OOMPH_REFINEABLE_ELEMENTS_HEADER

// Config header generated by autoconfig
#ifdef HAVE_CONFIG_H
  #include <oomph-lib-config.h>
#endif

#include "elements.h"
#include "tree.h"

namespace oomph
{

class Mesh;

//=======================================================================
/// RefineableElements are FiniteElements that may be subdivided into 
/// children to provide a better local approximation to the solution. 
/// After non-uniform refinement adjacent elements need not necessarily have
/// nodes in common. A node that does not have a counterpart in its
/// neighbouring element is known as a hanging node and its position and
/// any data that it stores must be constrained to ensure inter-element
/// continuity. 
///
/// Generic data and function interfaces associated with refinement 
/// are defined in this class.
/// 
/// Additional data includes:
/// - a pointer to a general Tree object that is used to track the
///   refinement history,
/// - a refinement level (not necessarily the same as the level in the tree!),
/// - a flag indicating whether the element should be refined,
/// - a flag indicating whether the element should be de-refined,
/// - a global element number for plotting/validation  purposes,
/// - storage for local equation numbers associated with hanging nodes.
///
/// Additional functions perform the following generic tasks:
/// - provide access to  additional data,
/// - setup local equation numbering for data associated with hanging nodes,
/// - generic finite-difference calculation of contributions to the 
///   elemental jacobian from nodal data to include hanging nodes,
/// - split of the element into its sons,
/// - select and deselect the element for refinement,
/// - select and deselect the sons of the element for de-refinement (merging),
///
/// In addition, there are a number of interfaces that specify element-specific
/// tasks. These should be overloaded in RefineableElements of particular
/// geometric types and perform the following tasks:
/// - return a pointer to the root and father elements in the tree structure,
/// - define the number of sons into which the element is divided,
/// - build the element: construct nodes, assign their positions, 
///   values and any boundary conditions,
/// - recreate the element from its sons if they are merged,
/// - deactivate the element, perform any operations that are 
///   required when the element is still in the tree, but no longer active 
/// - set the number and provide access to the values interpolated
///   by the nodes,
/// - setup the hanging nodes
///  
/// In mixed element different sets of nodes are used to interpolate different
/// unknowns. Interfaces are provided for functions that can be used to find
/// the position of the nodes that interpolate the different unknowns. These
/// functions are used to setup hanging node information automatically in
/// particular elements, e.g. Taylor Hood Navier--Stokes. 
/// The default implementation assumes that the elements are isoparameteric.
///
//======================================================================
class RefineableElement : public virtual FiniteElement
{
  protected:

 /// A pointer to a general tree object
 Tree* Tree_pt;

 /// Refinement level
 unsigned Refine_level;

 /// Flag for refinement
 bool To_be_refined;
 
 /// Flag for unrefinement
 bool Sons_to_be_unrefined;

 /// Global element number -- for plotting/validation purposes
 long Number;
  
 /// \short Max. allowed discrepancy in element integrity check
 static double Max_integrity_tolerance;

 /// \short Static helper function that is used to check that the value_id
 /// is in range
 static void check_value_id(const int &n_continuously_interpolated_values,
                            const int &value_id);


  /// \short Assemble the jacobian matrix for the mapping from local
 /// to Eulerian coordinates, given the derivatives of the shape function
 /// w.r.t the local coordinates.
 /// Overload the standard version to use the hanging information for
 /// the Eulerian coordinates.
 void assemble_local_to_eulerian_jacobian(
  const DShape &dpsids, DenseMatrix<double> &jacobian) const;
 
 /// \short Assemble the the "jacobian" matrix of second derivatives of the
 /// mapping from local to Eulerian coordinates, given
 /// the second derivatives of the shape functions w.r.t. local coordinates.
 /// Overload the standard version to use the hanging information for
 /// the Eulerian coordinates.
 void assemble_local_to_eulerian_jacobian2(
  const DShape &d2psids, DenseMatrix<double> &jacobian2) const;
 
 /// \short Assemble the covariant Eulerian base vectors, assuming that
 /// the derivatives of the shape functions with respect to the local 
 /// coordinates have already been constructed.
 /// Overload the standard version to account for hanging nodes.
 void assemble_eulerian_base_vectors(
  const DShape &dpsids, DenseMatrix<double> &interpolated_G) const;
 
 /// \short Calculate the mapping from local to Eulerian coordinates given 
 /// the derivatives of the shape functions w.r.t the local coordinates.
 /// assuming that the coordinates are aligned in the direction of the local 
 /// coordinates, i.e. there are no cross terms and the jacobian is diagonal.
 /// This funciton returns the determinant of the jacobian, the jacobian 
 /// and the inverse jacobian. Overload the standard version to take
 /// hanging info into account.
 double local_to_eulerian_mapping_diagonal(
  const DShape &dpsids,DenseMatrix<double> &jacobian,
  DenseMatrix<double> &inverse_jacobian) const;

  private:

 /// \short Storage for local equation numbers of hanging node variables
 /// (values stored at master nodes). It is
 /// essential that these are indexed by a Node pointer because the Node 
 /// may be internal or external to the element.
 /// local equation number = Local_hang_eqn(master_node_pt,ival)
 //MapMatrixMixed<Node*,int,int> Local_hang_eqn;
 std::map<Node*,int> *Local_hang_eqn;

  protected:
 
 /// \short Access function that returns the local equation number for the
 /// hanging node variables (values stored at master nodes). The local
 /// equation number corresponds to the i-th unknown stored at the node
 /// addressed by node_pt
 inline int local_hang_eqn(Node* const &node_pt, const unsigned &i)
  {
#ifdef RANGE_CHECKING
   if(i > ncont_interpolated_values())
    {
     std::ostringstream error_message;
     error_message << "Range Error: Value " << i
                   << " is not in the range (0,"
                   << ncont_interpolated_values()-1 << ")";
     throw OomphLibError(error_message.str(),
                         "RefineableElement::local_hang_eqn()",
                         OOMPH_EXCEPTION_LOCATION);
    }
#endif

   return Local_hang_eqn[i][node_pt];
  }


 /// \short Assign the local equation numbers for hanging node variables
 void assign_hanging_local_eqn_numbers();

 /// \short Calculate the contributions to the jacobian from the nodal
 /// degrees of freedom using finite differences.
 /// This version is overloaded to take hanging node information into
 /// account
 virtual void fill_in_jacobian_from_nodal_by_fd(Vector<double> &residuals,
                                            DenseMatrix<double> &jacobian);

  public:

 /// \short Constructor, calls the FiniteElement constructor and initialises
 /// the member data
 RefineableElement() : FiniteElement(), Tree_pt(0), Refine_level(0),
  To_be_refined(false), Sons_to_be_unrefined(false), Number(-1),
  Local_hang_eqn(0) {}


 /// Destructor, delete the allocated storage
 /// for the hanging equations
 virtual ~RefineableElement() {delete[] Local_hang_eqn;}

 /// Broken copy constructor
 RefineableElement(const RefineableElement&) 
  { 
   BrokenCopy::broken_copy("RefineableElement");
  } 
 
 /// Broken assignment operator
 void operator=(const RefineableElement&) 
  {
   BrokenCopy::broken_assign("RefineableElement");
  }

  /// Access function: Pointer to quadtree representation of this element
 Tree* tree_pt() {return Tree_pt;}

 /// Set pointer to quadtree representation of this element
 void set_tree_pt(Tree* my_tree_pt) {Tree_pt=my_tree_pt;}

 /// \short Set the number of sons that can be constructed by the element
 /// The default is none
 virtual unsigned required_nsons() const {return 0;}

 /// \short Split the element into the  number of sons to be
 /// constructed and return a 
 /// vector of pointers to the sons. Elements are allocated, but they are
 /// not given any properties. The refinement level of the sons is one
 /// higher than that of the father elemern.
 template<class ELEMENT>
  void split(Vector<ELEMENT*>& son_pt) const
  {
   // Increase refinement level
   int son_refine_level=Refine_level+1;
   
   //How many sons are to be constructed
   unsigned n_sons = required_nsons();
   //Resize the son pointer
   son_pt.resize(n_sons);

   //Loop over the sons and construct
   for(unsigned i=0;i<n_sons;i++)
    {
     son_pt[i]=new ELEMENT;
     //Set the refinement level of the newly constructed son.
     son_pt[i]->set_refinement_level(son_refine_level);
    }
  }

 /// \Short Interface to function that builds the element: i.e. 
 /// construct the nodes, assign their positions, 
 //  apply boundary conditions, etc. The 
 /// required procedures depend on the geometrical type of the element
 /// and must be implemented in specific refineable elements. Any new
 /// nodes created during the build process are returned in the 
 /// vector new_node_pt.
 virtual void build(Mesh* &mesh_pt, Vector<Node*> &new_node_pt,
                    bool &was_already_built, std::ofstream &new_nodes_file)=0;

 /// Set the refinement level
 void set_refinement_level(const int& refine_level) 
  {Refine_level=refine_level;}
 
 /// Return the Refinement level
 unsigned refinement_level() const {return Refine_level;}
 
 /// Select the element for refinement
 void select_for_refinement() {To_be_refined=true;}

 /// Deselect the element for refinement
 void deselect_for_refinement() {To_be_refined=false;}
 
 /// Unrefinement will be performed by merging the four sons of this element
 void select_sons_for_unrefinement() {Sons_to_be_unrefined=true;}

 /// No unrefinement will be performed by merging the four sons of this element
 void deselect_sons_for_unrefinement() {Sons_to_be_unrefined=false;}

 /// Has the element been selected for refinement?
 bool to_be_refined() {return To_be_refined;}

 /// Has the element been selected for unrefinement?
 bool sons_to_be_unrefined() {return Sons_to_be_unrefined;}

 /// \short Rebuild the element, e.g. set internal values in line with
 /// those of the sons that have now merged
 virtual void rebuild_from_sons(Mesh* &mesh_pt)=0;

 /// \short Unbuild the element, i.e. mark the nodes that were created
 /// during its creation for possible deletion
 virtual void unbuild()
  {
   //Get pointer to father element
   RefineableElement* father_pt = father_element_pt();
   //If there is no father, nothing to do
   if(father_pt==0) {return;}

   //Loop over all the nodes
   unsigned n_node = this->nnode();
   for(unsigned n=0;n<n_node;n++)
    {
     //If any node in this element is in the father, it can't be deleted
     if(father_pt->get_node_number(this->node_pt(n)) >= 0)
      {
       node_pt(n)->set_non_obsolete();
      }
    }
  }

 /// \short Final operations that must be performed when the element is no
 /// longer active in the mesh, but still resident in the QuadTree.
 virtual void deactivate_element();
 
 /// Return true if all the nodes have been built, false if not
 virtual bool nodes_built() {return node_pt(0)!=0;}

 /// Element number (for debugging/plotting)
 long number() const {return Number;}
 
 /// Set element number (for debugging/plotting)
 void set_number(const long& mynumber) {Number=mynumber;}

 /// \short Number of continuously interpolated values. Note: We assume
 /// that they are located at the beginning of the value_pt Vector!
 /// (Used for interpolation to son elements, for integrity check
 /// and post-processing -- we can only expect
 /// the continously interpolated values to be continous across
 /// element boundaries).
 virtual unsigned ncont_interpolated_values() const=0;

 /// \short Get all continously interpolated function values in this 
 /// element as a Vector. Note: Vector sets is own size to ensure that
 /// that this function can be used in black-box fashion
 virtual void get_interpolated_values(const Vector<double>&s, 
                                      Vector<double>& values)=0;
 
 /// \short Get all continously interpolated function values at previous 
 /// timestep in this element as a Vector. (t=0: present; t>0: prev. timestep)
 /// Note: Vector sets is own size to ensure that
 /// that this function can be used in black-box fashion
 virtual void get_interpolated_values(const unsigned& t,
                                      const Vector<double>&s, 
                                      Vector<double>& values)=0;

 /// \short In mixed elements, different sets of nodes are used to interpolate
 /// different unknowns. This function returns the n-th node that interpolates
 /// the value_id-th unknown. Default implementation is that all
 /// variables use the positional nodes, i.e. isoparametric elements. Note
 /// that any overloaded versions of this function MUST provide a set
 /// of nodes for the position, which always has the value_id -1.
 virtual Node* interpolating_node_pt(const unsigned &n,
                                     const int &value_id)
  
  {return node_pt(n);}

 /// \short Return the local one dimensional fraction of the n1d-th node
 /// in the direction of the local coordinate s[i] that is used to interpolate
 /// the value_id-th continuously interpolated unknown. Default assumes 
 /// isoparametric interpolation for all unknowns
 virtual double local_one_d_fraction_of_interpolating_node(
  const unsigned &n1d, const unsigned &i, const int &value_id)
  {return local_one_d_fraction_of_node(n1d,i);}

 /// \short Return a pointer to the node that interpolates the value-id-th
 /// unknown at local coordinate s. If there is not a node at that position,
 /// then return 0.
 virtual Node* 
  get_interpolating_node_at_local_coordinate(const Vector<double> &s,
                                             const int &value_id)
                                             
  {return get_node_at_local_coordinate(s);}


 /// \short Return the number of nodes that are used to interpolate the
 /// value_id-th unknown. Default is to assume isoparametric elements.
 virtual unsigned ninterpolating_node(const int &value_id) {return nnode();}

 /// \short Return the number of nodes in a one_d direction that are 
 /// used to interpolate the value_id-th unknown. Default is to assume
 /// an isoparametric mapping.
 virtual unsigned ninterpolating_node_1d(const int &value_id) 
  {return nnode_1d();}

 /// \short Return the basis functions that are used to interpolate
 /// the value_id-th unknown. By default assume isoparameteric interpolation
 virtual void interpolating_basis(const Vector<double> &s,
                                  Shape &psi,
                                  const int &value_id) const
  {shape(s,psi);}

 /// \short Check the integrity of the element: Continuity of positions
 /// values, etc. Essentially, check that the approximation of the functions
 /// is consistent when viewed from both sides of the element boundaries
 /// Must be overloaded for each different geometric element
 virtual void check_integrity(double &max_error)=0;


  /// \short Max. allowed discrepancy in element integrity check
 static double max_integrity_tolerance()
  { return Max_integrity_tolerance;}

 /// Setup the local equation numbering schemes:
 virtual inline void assign_all_generic_local_eqn_numbers()
  {
   //Call the standard FiniteElement local numbering scheme
   FiniteElement::assign_all_generic_local_eqn_numbers();
   //Set up the generic hanging-node-based look-up schemes
   assign_hanging_local_eqn_numbers();
  }

 /// \short Pointer to the root element in refinement hierarchy (must be 
 /// implemented in specific elements that do refinement via
 /// tree-like refinement structure. Here we provide a default
 /// implementation that is appropriate for cases where tree-like
 /// refinement doesn't exist or if the element doesn't have 
 /// root in that tree (i.e. if it's a root itself): We return
 /// "this". 
 virtual RefineableElement* root_element_pt()
  {
   //If there is no tree -- the element is its own root
   if(Tree_pt==0) {return this;}
   //Otherwise it's the tree's root object
   else {return Tree_pt->root_pt()->object_pt();}
  }

 /// Return a pointer to the father element.
 virtual RefineableElement* father_element_pt() const 
  {
   //If we have no tree, we have no father
   if(Tree_pt==0) {return 0;}
   else
    {
     //Otherwise get the father of the tree
     Tree* father_pt = Tree_pt->father_pt();
     //If the tree has no father then return null, no father
     if(father_pt==0) {return 0;}
     else {return father_pt->object_pt();}
    }
  }

 /// \short Further build: e.g. deal with interpolation of internal values
 virtual void further_build() {}
 
 /// \short Mark up any hanging nodes that arise as a result of non-uniform
 /// refinement. Any hanging nodes will be documented in files addressed by
 /// the streams in the vector output_stream, if the streams are open.
 virtual void setup_hanging_nodes(Vector<std::ofstream*> &output_stream) { }

 /// \short Perform additional hanging node procedures for variables
 /// that are not interpolated by all nodes (e.g. lower order interpolations
 /// for the pressure in Taylor Hood). 
 virtual void further_setup_hanging_nodes() { }
};
 

//=======================================================================
/// RefineableSolidElements are SolidFiniteElements that may 
/// be subdivided into children to provide a better local approximation 
/// to the solution. The distinction is required to keep a clean 
/// separation between problems that alter nodal positions and others.
/// A number of procedures are generic and are included in this class.
//======================================================================
class RefineableSolidElement : public virtual RefineableElement,
                               public virtual SolidFiniteElement
{
 protected:

 /// \short Access the local equation number of of hanging node variables
 /// associated with nodal positions. The function returns a dense
 /// matrix that contains all the local equation numbers corresponding to
 /// the positional degrees of freedom.
 DenseMatrix<int> &local_position_hang_eqn(Node* const &node_pt)
  {return Local_position_hang_eqn[node_pt];}
 
 /// \short Assemble the jacobian matrix for the mapping from local
 /// to lagrangian coordinates, given the derivatives of the shape function
 /// Overload the standard version to use the hanging information for
 /// the lagrangian coordinates.
void assemble_local_to_lagrangian_jacobian(
  const DShape &dpsids, DenseMatrix<double> &jacobian) const;

 /// \short Assemble the the "jacobian" matrix of second derivatives, given
 /// the second derivatives of the shape functions w.r.t. local coordinates
 /// Overload the standard version to use the hanging information for
 /// the lagrangian coordinates.
 void assemble_local_to_lagrangian_jacobian2(
  const DShape &d2psids, DenseMatrix<double> &jacobian2) const;
 
 /// \short Calculate the mapping from local to Lagrangian coordinates given 
 /// the derivatives of the shape functions w.r.t the local coorindates.
 /// assuming that the coordinates are aligned in the direction of the local 
 /// coordinates, i.e. there are no cross terms and the jacobian is diagonal.
 /// This function returns the determinant of the jacobian, the jacobian 
 /// and the inverse jacobian.
 double local_to_lagrangian_mapping_diagonal(
  const DShape &dpsids,DenseMatrix<double> &jacobian,
  DenseMatrix<double> &inverse_jacobian) const;

  private:
 /// \short Storage for local equation numbers of 
 /// hanging node variables associated with nodal positions.
 /// local position equation number = 
 /// Local_position_hang_eqn(master_node_pt,ival)
 std::map<Node*,DenseMatrix<int> > Local_position_hang_eqn; 

  protected:
 /// \short Assign local equation numbers to the hanging values associated
 /// with positions or additional solid values.
 void assign_solid_hanging_local_eqn_numbers();
  
  public:
 /// Constructor
 RefineableSolidElement() : RefineableElement(), SolidFiniteElement() {}

 /// Virtual Destructor, delete any allocated storage
 virtual ~RefineableSolidElement() { }
 

 /// \short Assign local equations numbers for the solid positions.
 /// Overload the standard version to include hanging node information
 virtual inline void assign_all_generic_local_eqn_numbers()
  {
   //Call the non-solid finite element equations, including the hanging
   //schemes
   RefineableElement::assign_all_generic_local_eqn_numbers();
   //Call the solid values
   assign_solid_local_eqn_numbers();
   //Set generic solid hanging equation numbers
   assign_solid_hanging_local_eqn_numbers();
  }

 /// \short Compute element residual Vector and element Jacobian matrix
 /// corresponding to the solid positions. Overloaded version to take
 /// the hanging nodes into account
 void fill_in_jacobian_from_solid_position_by_fd(Vector<double> & residuals,
                                             DenseMatrix<double> &jacobian);

};

}

#endif
