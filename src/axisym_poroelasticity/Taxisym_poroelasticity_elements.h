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
#ifndef OOMPH_TAXISYM_POROELASTICITY_ELEMENTS_HEADER
#define OOMPH_TAXISYM_POROELASTICITY_ELEMENTS_HEADER

// Config header generated by autoconfig
#ifdef HAVE_CONFIG_H
  #include <oomph-lib-config.h>
#endif

#include "axisym_poroelasticity_elements.h"
#include "../generic/Telements.h"

namespace oomph
{

 ///=================================================================
 /// Element which solves the Darcy/linear elasticity equations 
 /// using TElements
 /// Geometrically the element is always a six noded triangle.
 /// We use the mid-side nodes to store edge-based flux degrees of
 /// freedom and internal data for the discontinuous pressure 
 /// and internal flux dofs.
 ///=================================================================
 template<unsigned ORDER>
 class TAxisymmetricPoroelasticityElement :
  public TElement<2,3>,
  public AxisymmetricPoroelasticityEquations
 {
   private:

  /// The number of values stored at each node
  static const unsigned Initial_Nvalue[];

  ///  Face index associated with edge flux degree of freedom
  static const unsigned Face_index_of_edge_flux[];

  /// \short Conversion scheme from an edge degree of freedom to the node 
  /// it's stored at
  static const unsigned Q_edge_conv[];

  /// The points along each edge where the fluxes are taken to be
  static const double Flux_interpolation_point[];

  /// The internal data index where the internal q degrees of freedom are stored
  unsigned Q_internal_data_index;

  /// The internal data index where the p degrees of freedom are stored
  unsigned P_internal_data_index;

  /// \short Unit normal signs associated with each edge to ensure inter-element
  /// continuity of the flux
  std::vector<short> Sign_edge;

 public:

  /// Constructor
  TAxisymmetricPoroelasticityElement();

  /// Destructor
  ~TAxisymmetricPoroelasticityElement();

  /// Number of values required at node n
  unsigned required_nvalue(const unsigned &n) const
   {
    return Initial_Nvalue[n];
   }
  
  /// Return the face index associated with specified edge 
  unsigned face_index_of_edge(const unsigned& j) const
  {
   return (j+2)%3;
  }

  /// \short Compute the face element coordinates of the nth flux interpolation 
  /// point along specified edge
  void face_local_coordinate_of_flux_interpolation_point(const unsigned &edge,
                                                         const unsigned &n,
                                                         Vector<double> &s) 
   const
   {
    // Get the location of the n-th flux interpolation point along 
    // the edge in terms of the distance along the edge itself
    Vector<double> flux_interpolation_point=
     edge_flux_interpolation_point(edge,n);

    // Convert the edge number to the number of the mid-edge node along that
    // edge
    unsigned node_number=Q_edge_conv[edge];

    // The edge basis functions are defined in a clockwise manner, so we have
    // to effectively "flip" some coordinates
    switch(node_number)
     {
      case 3:
       s[0]=flux_interpolation_point[0];
       break;
      case 4:
       s[0]=1.0-flux_interpolation_point[0];
       break;
      case 5:
       s[0]=flux_interpolation_point[0];
       break;
     }
   }

  /// Return the face index associated with j-th edge flux degree of freedom
  unsigned face_index_of_q_edge_basis_fct(const unsigned& j) const
  {
   return Face_index_of_edge_flux[j];
  }
  
  /// \short Return the nodal index of the j-th solid displacement unknown
  /// [0: r; 1: z]
  unsigned u_index_axisym_poroelasticity(const unsigned &j) const
   {
#ifdef RANGE_CHECKING
    if(j >= 2)
     {
      std::ostringstream error_message;
      error_message << "Range Error: j " << j
                    << " is not in the range (0,1)";
      throw OomphLibError(error_message.str(),
                          OOMPH_CURRENT_FUNCTION,
                          OOMPH_EXCEPTION_LOCATION);
     }
#endif
    return j;
   }

  
  /// Return the equation number of the j-th edge (flux) degree of freedom
  int q_edge_local_eqn(const unsigned &j) const
   {
#ifdef RANGE_CHECKING
    if(j >= nq_basis_edge())
     {
      std::ostringstream error_message;
      error_message << "Range Error: j " << j
                    << " is not in the range (0,"
                    << nq_basis_edge()-1 <<")";
      throw OomphLibError(error_message.str(),
                          OOMPH_CURRENT_FUNCTION,
                          OOMPH_EXCEPTION_LOCATION);
     }
#endif
    return this->nodal_local_eqn(q_edge_node_number(j),q_edge_index(j));
   }

  /// Return the equation number of the j-th internal degree of freedom
  int q_internal_local_eqn(const unsigned &j) const
   {
#ifdef RANGE_CHECKING
    if(j >= nq_basis_internal())
     {
      std::ostringstream error_message;
      error_message << "Range Error: j " << j
                    << " is not in the range (0,"
                    << nq_basis_internal()-1 <<")";
      throw OomphLibError(error_message.str(),
                          OOMPH_CURRENT_FUNCTION,
                          OOMPH_EXCEPTION_LOCATION);
     }
#endif
    return internal_local_eqn(q_internal_index(),j);
   }
  
  /// \short Return vector of pointers to the Data objects that store the 
  /// edge flux values
  Vector<Data*> q_edge_data_pt() const
   {
    // It's the mid-side nodes:
    Vector<Data*> data_pt(3);
    data_pt[0]=node_pt(3);
    data_pt[1]=node_pt(4);
    data_pt[2]=node_pt(5);
    return data_pt;
   }

  /// Return pointer to the Data object that stores the internal flux values
  Data* q_internal_data_pt() const
  {
   return this->internal_data_pt(Q_internal_data_index);
  }

  /// \short Return the index of the internal data where the q_internal degrees
  /// of freedom are stored
  unsigned q_internal_index() const
   {
    return Q_internal_data_index;
   }

  /// Return the nodal index at which the jth edge unknown is stored
  unsigned q_edge_index(const unsigned &j) const
   {
#ifdef RANGE_CHECKING
    if(j >= (nq_basis_edge()))
     {
      std::ostringstream error_message;
      error_message << "Range Error: j " << j
                    << " is not in the range (0,"
                    << nq_basis_edge()-1 <<")";
      throw OomphLibError(error_message.str(),
                          OOMPH_CURRENT_FUNCTION,
                          OOMPH_EXCEPTION_LOCATION);
     }
#endif
    return j%(ORDER+1)+2;
   }

  /// Return the number of the node where the jth edge unknown is stored
  unsigned q_edge_node_number(const unsigned &j) const
   {
#ifdef RANGE_CHECKING
    if(j >= (nq_basis_edge()))
     {
      std::ostringstream error_message;
      error_message << "Range Error: j " << j
                    << " is not in the range (0,"
                    << nq_basis_edge()-1 <<")";
      throw OomphLibError(error_message.str(),
                          OOMPH_CURRENT_FUNCTION,
                          OOMPH_EXCEPTION_LOCATION);
     }
#endif
    return Q_edge_conv[j/(ORDER+1)];
   }


  /// Get pointer to node that stores the edge flux dofs for specified edge
  Node* edge_flux_node_pt(const unsigned& edge)
  {
   return node_pt(Q_edge_conv[edge]);
  }

  /// Return the values of the j-th edge (flux) degree of freedom
  double q_edge(const unsigned &j) const
   {
#ifdef RANGE_CHECKING
    if(j >= (nq_basis_edge()))
     {
      std::ostringstream error_message;
      error_message << "Range Error: j " << j
                    << " is not in the range (0,"
                    << nq_basis_edge()-1 <<")";
      throw OomphLibError(error_message.str(),
                          OOMPH_CURRENT_FUNCTION,
                          OOMPH_EXCEPTION_LOCATION);
     }
#endif
    return nodal_value(q_edge_node_number(j),q_edge_index(j));
   }

  /// \short Return the values of the j-th edge (flux) degree of 
  /// freedom at time history level t
  double q_edge(const unsigned &t, const unsigned &j) const
   {
#ifdef RANGE_CHECKING
    if(j >= (nq_basis_edge()))
     {
      std::ostringstream error_message;
      error_message << "Range Error: j " << j
                    << " is not in the range (0,"
                    << nq_basis_edge()-1 <<")";
      throw OomphLibError(error_message.str(),
                          OOMPH_CURRENT_FUNCTION,
                          OOMPH_EXCEPTION_LOCATION);
     }
#endif
    return nodal_value(t,q_edge_node_number(j),q_edge_index(j));
   }

  /// Return the values of the internal degree of freedom
  double q_internal(const unsigned &j) const
   {
#ifdef RANGE_CHECKING
    if(j >= nq_basis_internal())
     {
      std::ostringstream error_message;
      error_message << "Range Error: j " << j
                    << " is not in the range (0,"
                    << nq_basis_internal()-1 <<")";
      throw OomphLibError(error_message.str(),
                          OOMPH_CURRENT_FUNCTION,
                          OOMPH_EXCEPTION_LOCATION);
     }
#endif
    return this->internal_data_pt(q_internal_index())->value(j);
   }
  
  /// \short Return the value of the j-th internal degree of freedom at
  /// time history level t
  double q_internal(const unsigned &t,const unsigned &j) const
   {
#ifdef RANGE_CHECKING
    if(j >= nq_basis_internal())
     {
      std::ostringstream error_message;
      error_message << "Range Error: j " << j
                    << " is not in the range (0,"
                    << nq_basis_internal()-1 <<")";
      throw OomphLibError(error_message.str(),
                          OOMPH_CURRENT_FUNCTION,
                          OOMPH_EXCEPTION_LOCATION);
     }
#endif
    return this->internal_data_pt(q_internal_index())->value(t,j);
   }

  /// Pin the j-th edge (flux) degree of freedom and set it to specified value
  void pin_q_edge_value(const unsigned &j, const double& value)
  {
   node_pt(q_edge_node_number(j))->pin(q_edge_index(j));
   node_pt(q_edge_node_number(j))->set_value(q_edge_index(j),value);
  }

  /// Set the values of the j-th edge (flux) degree of freedom
  void set_q_edge(const unsigned &j, const double& value)
  {
    node_pt(q_edge_node_number(j))->set_value(q_edge_index(j),value);
  }
  
  /// \short Set the values of the j-th edge (flux) degree of freedom at 
  /// time history level t
  void set_q_edge(const unsigned &t, const unsigned &j, const double& value)
   {
    node_pt(q_edge_node_number(j))->set_value(t,q_edge_index(j),value);
   }


  /// \short Set the values of the j-th internal degree of freedom
  void set_q_internal(const unsigned &j, const double& value)
   {
    this->internal_data_pt(q_internal_index())->set_value(j,value);
   }

  /// \short Set the values of the j-th internal degree of freedom at 
  /// time history level t
  void set_q_internal(const unsigned &t, const unsigned &j, const double& value)
   {
    this->internal_data_pt(q_internal_index())->set_value(t,j,value);
   }

  /// Return the number of edge basis functions for flux q
  unsigned nq_basis_edge() const;

  /// Return the number of internal basis functions for flux q
  unsigned nq_basis_internal() const;

  /// Returns the local form of the q basis at local coordinate s
  void get_q_basis_local(const Vector<double> &s,
                         Shape &q_basis) const;

  /// Returns the local form of the q basis and dbasis/ds at local coordinate s
  void get_div_q_basis_local(const Vector<double> &s,
                             Shape &div_q_basis_ds) const;

  /// \short Returns the number of flux_interpolation points along each 
  /// edge of the element
  unsigned nedge_flux_interpolation_point() const;

  /// \short Returns the local coordinate of the jth flux_interpolation point 
  /// along specified edge
  Vector<double> edge_flux_interpolation_point(const unsigned &edge,
                                               const unsigned &j) const
   {
#ifdef RANGE_CHECKING
    if(edge >= 3)
     {
      std::ostringstream error_message;
      error_message << "Range Error: edge " << edge
                    << " is not in the range (0,2)";
      throw OomphLibError(error_message.str(),
                          OOMPH_CURRENT_FUNCTION,
                          OOMPH_EXCEPTION_LOCATION);
     }
    if(j >= nedge_flux_interpolation_point())
     {
      std::ostringstream error_message;
      error_message << "Range Error: j " << j
                    << " is not in the range (0,"
                    << nedge_flux_interpolation_point()-1 <<")";
      throw OomphLibError(error_message.str(),
                          OOMPH_CURRENT_FUNCTION,
                          OOMPH_EXCEPTION_LOCATION);
     }
#endif
    Vector<double> coord(1);
    coord[0]=(1.0-sign_edge(edge))/2.0+sign_edge(edge)*
     Flux_interpolation_point[j];
    return coord;
   }

  /// \short Compute the global coordinates of the jth flux_interpolation 
  /// point along specified edge
  void edge_flux_interpolation_point_global(const unsigned &edge,
                                            const unsigned &j,
                                            Vector<double> &x) const
  {
#ifdef RANGE_CHECKING
   if(edge >= 3)
    {
     std::ostringstream error_message;
     error_message << "Range Error: edge " << edge
                   << " is not in the range (0,2)";
     throw OomphLibError(error_message.str(),
                         OOMPH_CURRENT_FUNCTION,
                         OOMPH_EXCEPTION_LOCATION);
    }
   if(j >= nedge_flux_interpolation_point())
    {
     std::ostringstream error_message;
     error_message << "Range Error: j " << j
                   << " is not in the range (0,"
                   << nedge_flux_interpolation_point()-1 <<")";
     throw OomphLibError(error_message.str(),
                         OOMPH_CURRENT_FUNCTION,
                         OOMPH_EXCEPTION_LOCATION);
    }
#endif
   
   // Get the location of the n-th flux_interpolation point along the 
   // edge in terms of the distance along the edge itself
   Vector<double> flux_interpolation_point=
    edge_flux_interpolation_point(edge,j);
   
    // Convert the edge number to the number of the mid-edge node along that
    // edge
    unsigned node_number=Q_edge_conv[edge];

    // Storage for the local coords of the flux_interpolation point
    Vector<double> s_flux_interpolation(2,0);

    // The edge basis functions are defined in a clockwise manner, so we have
    // to effectively "flip" the coordinates along edges 0 and 1 to match this
    switch(node_number)
     {
      case 3:
       s_flux_interpolation[0]=1.0-flux_interpolation_point[0];
       s_flux_interpolation[1]=flux_interpolation_point[0];
       break;
      case 4:
       s_flux_interpolation[0]=0.0;
       s_flux_interpolation[1]=1.0-flux_interpolation_point[0];
       break;
      case 5:
       s_flux_interpolation[0]=flux_interpolation_point[0];
       s_flux_interpolation[1]=0.0;
       break;
     }

    // Calculate the global coordinates from the local ones
    interpolated_x(s_flux_interpolation,x);
   }

  /// Pin the jth internal q value and set it to specified value
  void pin_q_internal_value(const unsigned &j, const double& q)
   {
#ifdef RANGE_CHECKING
    if(j >= nq_basis_internal())
     {
      std::ostringstream error_message;
      error_message << "Range Error: j " << j
                    << " is not in the range (0,"
                    << nq_basis_internal()-1 <<")";
      throw OomphLibError(error_message.str(),
                          OOMPH_CURRENT_FUNCTION,
                          OOMPH_EXCEPTION_LOCATION);
     }
#endif
    this->internal_data_pt(q_internal_index())->pin(j);
    this->internal_data_pt(q_internal_index())->set_value(j,q);
   }

  /// Return the equation number of the j-th pressure degree of freedom
  int p_local_eqn(const unsigned &j) const
   {
#ifdef RANGE_CHECKING
    if(j >= np_basis())
     {
      std::ostringstream error_message;
      error_message << "Range Error: j " << j
                    << " is not in the range (0,"
                    << np_basis()-1 <<")";
      throw OomphLibError(error_message.str(),
                          OOMPH_CURRENT_FUNCTION,
                          OOMPH_EXCEPTION_LOCATION);
     }
#endif
    return this->internal_local_eqn(P_internal_data_index,j);
   }

  /// Return the jth pressure value
  double p_value(const unsigned &j) const
   {
#ifdef RANGE_CHECKING
    if(j >= np_basis())
     {
      std::ostringstream error_message;
      error_message << "Range Error: j " << j
                    << " is not in the range (0,"
                    << np_basis()-1 <<")";
      throw OomphLibError(error_message.str(),
                          OOMPH_CURRENT_FUNCTION,
                          OOMPH_EXCEPTION_LOCATION);
     }
#endif
    return this->internal_data_pt(P_internal_data_index)->value(j);
   }

  /// Return the total number of pressure basis functions
  unsigned np_basis() const;

  /// Return the pressure basis
  void get_p_basis(const Vector<double> &s,
                   Shape &p_basis) const;

  /// Pin the jth pressure value and set to specified value
  void pin_p_value(const unsigned &j, const double &p)
   {
#ifdef RANGE_CHECKING
    if(j >= np_basis())
     {
      std::ostringstream error_message;
      error_message << "Range Error: j " << j
                    << " is not in the range (0,"
                    << np_basis()-1 <<")";
      throw OomphLibError(error_message.str(),
                          OOMPH_CURRENT_FUNCTION,
                          OOMPH_EXCEPTION_LOCATION);
     }
#endif
    this->internal_data_pt(P_internal_data_index)->pin(j);
    this->internal_data_pt(P_internal_data_index)->set_value(j,p);
   }

  /// Return pointer to the Data object that stores the pressure values
  Data* p_data_pt() const
  {
   return this->internal_data_pt(P_internal_data_index);
  }

  /// Set the jth pressure value
  void set_p_value(const unsigned &j, const double& value)
   {
    this->internal_data_pt(P_internal_data_index)->set_value(j,value);
   }

  /// Scale the edge basis to allow arbitrary edge mappings
  void scale_basis(Shape& basis) const
   {
    // Storage for the lengths of the edges of the element
    Vector<double> length(3,0.0);

    // Temporary storage for the vertex positions
    double x0,y0,x1,y1;

    // loop over the edges of the element and calculate their lengths (in x-y
    // space)
    for(unsigned i=0;i<3;i++)
     {
      x0=this->node_pt(i)->x(0);
      y0=this->node_pt(i)->x(1);
      x1=this->node_pt((i+1)%3)->x(0);
      y1=this->node_pt((i+1)%3)->x(1);

      length[i]=std::sqrt(std::pow(y1-y0,2)+std::pow(x1-x0,2));
     }

    // lengths of the sides of the reference element (in the same order as the
    // basis functions)
    const double ref_length[3]={std::sqrt(2.0),1,1};

    // get the number of basis functions associated with the edges
    unsigned n_q_basis_edge=nq_basis_edge();

    // rescale the edge basis functions to allow arbitrary edge mappings from
    // element to ref. element
    const unsigned n_index2 = basis.nindex2();
    for(unsigned i=0;i<n_index2;i++)
     {
      for(unsigned l=0;l<n_q_basis_edge;l++)
       {
        basis(l,i)*=(length[l/(ORDER+1)]/ref_length[l/(ORDER+1)]);
       }
     }
   }

  /// Accessor for the unit normal sign of edge n (const version)
  const short &sign_edge(const unsigned &n) const
   {
    return Sign_edge[n];
   }

  /// Accessor for the unit normal sign of edge n
  short &sign_edge(const unsigned &n)
   {
    return Sign_edge[n];
   }

  /// Output with default number of plot points
  void output(std::ostream &outfile)
   {
    AxisymmetricPoroelasticityEquations::output(outfile);
   }

  /// \short Output FE representation of soln: x,y,u1,u2,div_q,p at
  /// Nplot^DIM plot points
  void output(std::ostream &outfile, const unsigned &Nplot)
   {
    AxisymmetricPoroelasticityEquations::output(outfile,Nplot);
   }


 /// \short Number of vertex nodes in the element
 unsigned nvertex_node() const
 {return TElement<2,3>::nvertex_node();}
 
 /// \short Pointer to the j-th vertex node in the element
 Node* vertex_node_pt(const unsigned& j) const
 {return TElement<2,3>::vertex_node_pt(j);}
 
/// Recovery order for Z2 error estimator
 unsigned nrecovery_order()
 {
  return 2; // need to experiment with this...
 }

 protected:

  /// \short Returns the geometric basis, and the u, p and divergence basis
  /// functions and test functions at local coordinate s
  double shape_basis_test_local(const Vector<double> &s,
                                Shape &psi,
                                DShape &dpsi,
                                Shape &u_basis,
                                Shape &u_test,
                                DShape &du_basis_dx,
                                DShape &du_test_dx,
                                Shape &q_basis,
                                Shape &q_test,
                                Shape &p_basis,
                                Shape &p_test,
                                Shape &div_q_basis_ds,
                                Shape &div_q_test_ds) const
   {
    const unsigned n_q_basis = this->nq_basis();

    Shape q_basis_local(n_q_basis,2);
    this->get_q_basis_local(s,q_basis_local);
    this->get_p_basis(s,p_basis);
    this->get_div_q_basis_local(s,div_q_basis_ds);

    double J = this->transform_basis(s,q_basis_local,psi,dpsi,q_basis);

    // u_basis consists of the normal Lagrangian shape functions
    u_basis=psi;
    du_basis_dx=dpsi;

    u_test=psi;
    du_test_dx=dpsi;

    q_test=q_basis;
    p_test=p_basis;
    div_q_test_ds=div_q_basis_ds;

    return J;
   }

  /// \short Returns the geometric basis, and the u, p and divergence basis
  /// functions and test functions at integration point ipt
  double shape_basis_test_local_at_knot(const unsigned &ipt,
                                        Shape &psi,
                                        DShape &dpsi,
                                        Shape &u_basis,
                                        Shape &u_test,
                                        DShape &du_basis_dx,
                                        DShape &du_test_dx,
                                        Shape &q_basis,
                                        Shape &q_test,
                                        Shape &p_basis,
                                        Shape &p_test,
                                        Shape &div_q_basis_ds,
                                        Shape &div_q_test_ds) const
   {
    Vector<double> s(2);
    for(unsigned i=0;i<2;i++) { s[i] = this->integral_pt()->knot(ipt,i); }

    return shape_basis_test_local(
      s,psi,dpsi,
      u_basis,u_test,
      du_basis_dx,du_test_dx,
      q_basis,q_test,
      p_basis,p_test,
      div_q_basis_ds,div_q_test_ds);
   }
 };



 //================================================================
 /// Face geometry for TAxisymmetricPoroelasticityElement<0>
 //================================================================
 template<>
 class FaceGeometry<TAxisymmetricPoroelasticityElement<0> > :
  public virtual TElement<1,3>
 {
   public:

   /// Constructor: Call constructor of base
   FaceGeometry() : TElement<1,3>() {}
 };


 //================================================================
 /// Face geometry for TAxisymmetricPoroelasticityElement<1>
 //================================================================
 template<>
 class FaceGeometry<TAxisymmetricPoroelasticityElement<1> > :
  public virtual TElement<1,3>
 {
   public:

   /// Constructor: Call constructor of base class
   FaceGeometry() : TElement<1,3>() {}
 };



} // End of oomph namespace

#endif

