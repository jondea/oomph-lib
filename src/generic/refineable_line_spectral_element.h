#ifndef OOMPH_REFINEABLE_LINE_SPECTRAL_ELEMENT_HEADER
#define OOMPH_REFINEABLE_LINE_SPECTRAL_ELEMENT_HEADER


// Config header generated by autoconfig
#ifdef HAVE_CONFIG_H
  #include <oomph-lib-config.h>
#endif


// oomph-lib headers
#include "refineable_line_element.h"

namespace oomph
{
 
 //==========================================================================
 /// Refineable version of LineElements that add functionality for spectral
 /// Elements.
 //==========================================================================
 template<>
  class RefineableQSpectralElement<1> : public virtual RefineableQElement<1> 
  {
   
  public:
   
   /// Constructor
   RefineableQSpectralElement() : RefineableElement()
    {
#ifdef LEAK_CHECK
     LeakCheckNames::RefineableQSpectralElement<1>_build+=1;
#endif
    } 
   
   /// Broken copy constructor
   RefineableQSpectralElement(const RefineableQSpectralElement<1>& dummy) 
    { 
     BrokenCopy::broken_copy("RefineableQSpectralElement<1>");
    } 
   
   /// Broken assignment operator
   void operator=(const RefineableQSpectralElement<1>&) 
    {
     BrokenCopy::broken_assign("RefineableQSpecralElement<1>");
    }

   /// Destructor
   virtual ~RefineableQSpectralElement()
    {
#ifdef LEAK_CHECK
     LeakCheckNames::RefineableQSpectralElement<1>_build-=1;
#endif
    } 
   
   /// The only thing to add is rebuild from sons
   void rebuild_from_sons(Mesh* &mesh_pt)
    {
     // The timestepper should be the same for all nodes and node 0 should
     // never be deleted.
     if(this->node_pt(0)==0)
      {
       throw OomphLibError("The vertex node (0) does not exist",
                           "RefineableQSpectralElement::rebuild_from_sons()",
                           OOMPH_EXCEPTION_LOCATION);
      }
     
     TimeStepper* time_stepper_pt = this->node_pt(0)->time_stepper_pt();
     
     // Determine number of history values stored
     const unsigned ntstorage = time_stepper_pt->ntstorage();
     
     // Allocate storage for local coordinates
     Vector<double> s_fraction(1), s(1);
     
     // Determine the number of nodes in the element
     const unsigned n_node = this->nnode_1d();
     
     // Loop over the nodes in the element
     for(unsigned n=0;n<n_node;n++)
      {
       // Get the fractional position of the node in the direction of s[0]
       s_fraction[0] = this->local_one_d_fraction_of_node(n,0);
       
       // Determine the local coordinate in the father element
       s[0] = -1.0 + 2.0*s_fraction[0];
       
       // If the node has not been built
       if(this->node_pt(n)==0)
        {
         // Has the node been created by one of its neighbours?
         bool is_periodic = false;
         Node* created_node_pt = 
          this->node_created_by_neighbour(s_fraction,is_periodic);
         
         // If it has, set the pointer
         if(created_node_pt!=0)
          {
           // If the node is periodic
           if(is_periodic)
            {
             throw OomphLibError(
              "Cannot handle periodic nodes in refineable spectral elements",
              "RefineableQuadSpectralElement<1>::rebuild_from_sons()",
              OOMPH_EXCEPTION_LOCATION);
            }
           // Non-periodic case, just set the pointer
           else
            {
             this->node_pt(n) = created_node_pt;
            }
          }
         // Otherwise, we need to build it
         else
          {
           // First, find the son element in which the node should live
           
           // Find coordinate in the son
           Vector<double> s_in_son(1);
           using namespace BinaryTreeNames;
           int son=-10;
           // If s_fraction is between 0 and 0.5, we are in the left son
           if(s_fraction[0] < 0.5)
            {
             son = L;
             s_in_son[0] =  -1.0 + 4.0*s_fraction[0];
            }
           // Otherwise we are in the right son
           else
            {
             son = R;
             s_in_son[0] =  -1.0 + 4.0*(s_fraction[0]-0.5);
            }
           
           // Get the pointer to the son element in which the new node
           // would live
           RefineableQSpectralElement<1>* son_el_pt = 
            dynamic_cast<RefineableQSpectralElement<1>*>(
             this->tree_pt()->son_pt(son)->object_pt());
           
           // In 1D we should never be rebuilding an element's vertex nodes
           // (since they will never be deleted), so throw an error if we
           // appear to be doing so
#ifdef PARANOID
           if(n==0 || n==n_node-1)
            {           
             std::string error_message =
              "I am trying to rebuild one of the (two) vertex nodes in\n";
             error_message +=
              "this 1D element. It should not have been possible to delete\n";
             error_message +=
              "either of these!\n";
             
             throw OomphLibError(
              error_message,
              "RefineableQSpectralElement<1>::rebuild_from_sons()",
              OOMPH_EXCEPTION_LOCATION);
            }
#endif
           
           // With this in mind we will always be creating normal "bulk" nodes
           this->node_pt(n) = this->construct_node(n,time_stepper_pt);
           
           // Now we set the position and values at the newly created node
           
           // In the first instance use macro element or FE representation
           // to create past and present nodal positions.
           // (THIS STEP SHOULD NOT BE SKIPPED FOR ALGEBRAIC ELEMENTS AS NOT
           // ALL OF THEM NECESSARILY IMPLEMENT NONTRIVIAL NODE UPDATE
           // FUNCTIONS. CALLING THE NODE UPDATE FOR SUCH ELEMENTS/NODES WILL
           // LEAVE THEIR NODAL POSITIONS WHERE THEY WERE (THIS IS APPROPRIATE
           // ONCE THEY HAVE BEEN GIVEN POSITIONS) BUT WILL NOT ASSIGN SENSIBLE
           // INITIAL POSITIONS!)
           
           // Loop over history values
           for(unsigned t=0;t<ntstorage;t++)
            {
             // Allocate storage for the previous position of the node
             Vector<double> x_prev(1);
           
             // Get position from son element -- this uses the macro element
             // representation if appropriate
             son_el_pt->get_x(t,s_in_son,x_prev);
             
             // Set the previous position of the new node
             this->node_pt(n)->x(t,0) = x_prev[0];
             
             // Allocate storage for the previous values at the node
             // NOTE: the size of this vector is equal to the number of values
             // (e.g. 3 velocity components and 1 pressure, say)
             // associated with each node and NOT the number of history values
             // which the node stores!
             Vector<double> prev_values;         
             
             // Get values from son element
             // Note: get_interpolated_values() sets Vector size itself.
             son_el_pt->get_interpolated_values(t,s_in_son,prev_values);
             
             // Determine the number of values at the new node
             const unsigned n_value = this->node_pt(n)->nvalue();
           
             // Loop over all values and set the previous values
             for(unsigned v=0;v<n_value;v++)
              {
               this->node_pt(n)->set_value(t,v,prev_values[v]);
              }
            } // End of loop over history values
           
           // Add new node to mesh
           mesh_pt->add_node_pt(this->node_pt(n));
           
          } // End of case where we build the node ourselves
         
         // Check if the element is an algebraic element
         AlgebraicElementBase* alg_el_pt =
          dynamic_cast<AlgebraicElementBase*>(this);
         
         // If so, throw error
         if(alg_el_pt!=0)
          {
           std::string error_message =
            "Have not implemented rebuilding from sons for";
           error_message +=
            "Algebraic Spectral elements yet\n";
           
           throw 
            OomphLibError(error_message,
                          "RefineableQSpectralElement<1>::rebuild_from_sons()",
                          OOMPH_EXCEPTION_LOCATION);
          }
         
        } // End of if this node has not been built
      } // End of loop over nodes in element
    }
   
   
   /// Overload the nodes built function
   virtual bool nodes_built()
    {
     const unsigned n_node = this->nnode();
     for(unsigned n=0;n<n_node;n++)
      {
       if(node_pt(n)==0) { return false; }
      }
     // If we get to here, OK
     return true;
    }
   
  };
 
} // End of namespace

#endif