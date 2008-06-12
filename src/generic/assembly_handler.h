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
//A class that is used to assemble the linear systems solved
//by oomph-lib.

//Include guards to prevent multiple inclusion of this header
#ifndef OOMPH_ASSEMBLY_HANDLER_CLASS_HEADER
#define OOMPH_ASSEMBLY_HANDLER_CLASS_HEADER

// Config header generated by autoconfig
#ifdef HAVE_CONFIG_H
  #include <oomph-lib-config.h>
#endif

//OOMPH-LIB headers
#include "matrices.h"
#include "linear_solver.h"

namespace oomph
{
//Forward class definition of the element
 class GeneralisedElement;

 //Forward class definition of the problem
 class Problem;
 
//=============================================================
/// A class that is used to define the functions used to
/// assemble the elemental contributions to the 
/// residuals vector and Jacobian matrix that define
/// the problem being solved. 
/// The main use
/// of this class is to assemble and solve the augmented systems
/// used in bifurcation detection and tracking. The default 
/// implementation merely calls the underlying elemental 
/// functions with no augmentation.
//===============================================================
class AssemblyHandler
{
  public:
 
 ///Empty constructor
 AssemblyHandler() {}

 ///Return the number of degrees of freedom in the element elem_pt
 virtual unsigned ndof(GeneralisedElement* const &elem_pt);
 
 /// \short Return the global equation number of the local unknown ieqn_local
 ///in elem_pt.
 virtual unsigned long eqn_number(GeneralisedElement* const &elem_pt,
                                  const unsigned &ieqn_local);
 
 ///Return the contribution to the residuals of the element elem_pt
 virtual void get_residuals(GeneralisedElement* const &elem_pt,
                            Vector<double> &residuals);
 
 /// \short Calculate the elemental Jacobian matrix "d equation 
 /// / d variable" for elem_pt.
 virtual void get_jacobian(GeneralisedElement* const &elem_pt,
                           Vector<double> &residuals, 
                           DenseMatrix<double> &jacobian);
 
 /// \short Calculate all desired vectors and matrices 
 /// provided by the element elem_pt.
 virtual void get_all_vectors_and_matrices(
  GeneralisedElement* const &elem_pt,
  Vector<Vector<double> >&vec, Vector<DenseMatrix<double> > &matrix);
 
 /// \short Empty virtual destructor
 virtual ~AssemblyHandler() {}
};


 
//=============================================================
/// A class that is used to define the functions used to
/// assemble the elemental contributions to the 
/// mass matrix and jacobian (stiffness) matrix that define
/// a generalised eigenproblem.
//===============================================================
class EigenProblemHandler : public AssemblyHandler
{
 /// Storage for the real shift
 double Sigma_real;

  public:

 ///Constructor, sets the value of the real shift
 EigenProblemHandler(const double &sigma_real) : Sigma_real(sigma_real) {} 

 ///Return the number of degrees of freedom in the element elem_pt
 unsigned ndof(GeneralisedElement* const &elem_pt);
 
 ///\short Return the global equation number of the local unknown ieqn_local
 ///in elem_pt.
 unsigned long eqn_number(GeneralisedElement* const &elem_pt,
                                  const unsigned &ieqn_local);
 
 ///\short Return the contribution to the residuals of the element elem_pt
 ///This is deliberately broken in our eigenproblem
 void get_residuals(GeneralisedElement* const &elem_pt,
                    Vector<double> &residuals);
 
 /// \short Calculate the elemental Jacobian matrix "d equation 
 /// / d variable" for elem_pt. Again deliberately broken in the eigenproblem
 void get_jacobian(GeneralisedElement* const &elem_pt,
                   Vector<double> &residuals, 
                   DenseMatrix<double> &jacobian);
 
 /// \short Calculate all desired vectors and matrices 
 /// provided by the element elem_pt.
 void get_all_vectors_and_matrices(
  GeneralisedElement* const &elem_pt,
  Vector<Vector<double> >&vec, Vector<DenseMatrix<double> > &matrix);
 
 /// \short Empty virtual destructor
 ~EigenProblemHandler() {}

};



//========================================================================
/// A custom linear solver class that is used to solve a block-factorised
/// version of the Fold bifurcation detection problem.
//========================================================================
class BlockFoldLinearSolver : public LinearSolver
{
 ///Pointer to the original linear solver
 LinearSolver* Linear_solver_pt;

 //Pointer to the problem, used in the resolve
 Problem* Problem_pt;
 
 ///Pointer to the storage for the vector alpha
 Vector<double> *Alpha_pt;

 ///Pointer to the storage for the vector e
 Vector<double> *E_pt;

public:
 
 ///Constructor, inherits the original linear solver
 BlockFoldLinearSolver(LinearSolver* const linear_solver_pt)
  : Linear_solver_pt(linear_solver_pt), Problem_pt(0), Alpha_pt(0), E_pt(0) {}
 
 ///Destructor: clean up the allocated memory
 ~BlockFoldLinearSolver();

 /// The solve function uses the block factorisation
 void solve(Problem* const &problem_pt, Vector<double> &result);

 /// The resolve function also uses the block factorisation
 void resolve(const Vector<double> &rhs, Vector<double> &result);

 /// Access function to the original linear solver
 LinearSolver* linear_solver_pt() const {return Linear_solver_pt;}

};


//===================================================================
/// A class that is used to assemble the augmented system that defines
/// a fold (saddle-node) or limit point. The "standard" problem must
/// be a function of a global paramter \f$\lambda\f$, and a  
/// solution is \f$R(u,\lambda) = 0 \f$, where \f$u\f$ are the unknowns
/// in the problem. A limit point is formally specified by the augmented
/// system of size \f$2N+1\f$ 
/// \f[ R(u,\lambda) = 0, \f]
/// \f[ Jy = 0, \f]
/// \f[\phi\cdot y = 1. \f]
/// In the above \f$J\f$ is the usual Jacobian matrix, \f$dR/du\f$, and
/// \f$\phi\f$ is a constant vector that is chosen to 
/// ensure that the null vector, \f$y\f$, is not trivial. 
//====================================================================== 
class FoldHandler : public AssemblyHandler
 {
  friend class BlockFoldLinearSolver;

  /// \short Boolean flag to indicate whether we are solving a block-factorised
  /// system or not
  bool Solve_block_system;

  ///Pointer to the problem
  Problem *Problem_pt;

  /// \short Store the number of degrees of freedom in the non-augmented
  ///problem
  unsigned Ndof;

  /// \short A constant vector used to ensure that the null vector
  /// is not trivial
  Vector<double> Phi;

  /// \short Storage for the null vector
  Vector<double> Y;

  /// \short A vector that is used to determine how many elements
  /// contribute to a particular equation. It is used to ensure
  /// that the global system is correctly formulated. 
  Vector<int> Count;

 public:

  ///\short Constructor: 
  /// initialise the fold handler, by setting initial guesses
  /// for Y, Phi and calculating count. If the system changes, a new
  /// fold handler must be constructed
  FoldHandler(Problem* const &problem_pt, double* const &parmater_pt);
  
  /// \short Destructor, return the problem to its original state
  /// before the augmented system was added
  ~FoldHandler();

  ///Get the number of elemental degrees of freedom
  unsigned ndof(GeneralisedElement* const &elem_pt);

  ///Get the global equation number of the local unknown
  unsigned long eqn_number(GeneralisedElement* const &elem_pt,
                           const unsigned &ieqn_local);

   ///Get the residuals
  void get_residuals(GeneralisedElement* const &elem_pt,
                     Vector<double> &residuals);
  
  /// \short Calculate the elemental Jacobian matrix "d equation 
  /// / d variable".
  void get_jacobian(GeneralisedElement* const &elem_pt,
                    Vector<double> &residuals, 
                    DenseMatrix<double> &jacobian);

  /// \short Set to solve the block system
  void solve_block_system();

  /// \short Solve non-block system
  void solve_full_system();
  
 };


//========================================================================
/// A custom linear solver class that is used to solve a block-factorised
/// version of the PitchFork bifurcation detection problem.
//========================================================================
class BlockPitchForkLinearSolver : public LinearSolver
{
 ///Pointer to the original linear solver
 LinearSolver* Linear_solver_pt;

 ///Pointer to the problem, used in the resolve
 Problem* Problem_pt;

 ///Pointer to the storage for the vector alpha
 Vector<double> *Alpha_pt;

 ///Pointer to the storage for the vector e
 Vector<double> *E_pt;

public:
 
 ///Constructor, inherits the original linear solver
 BlockPitchForkLinearSolver(LinearSolver* const linear_solver_pt)
  : Linear_solver_pt(linear_solver_pt), Problem_pt(0),
   Alpha_pt(0), E_pt(0) {}
 
 ///Destructor: clean up the allocated memory
 ~BlockPitchForkLinearSolver();

 /// The solve function uses the block factorisation
 void solve(Problem* const &problem_pt, Vector<double> &result);

 /// The resolve function also uses the block factorisation
 void resolve(const Vector<double> &rhs, Vector<double> &result);

 /// Access function to the original linear solver
 LinearSolver* linear_solver_pt() const {return Linear_solver_pt;}

};

//========================================================================
/// A class that is used to assemble the augmented system that defines
/// a pitchfork (symmetry-breaking) bifurcation. The "standard" problem
/// must be a function of a global parameter \f$\lambda\f$ and a solution
/// is \f$R(u,\lambda) = 0\f$, where \f$u\f$ are the unknowns in the 
/// problem. A pitchfork bifurcation may be specified by the augmented
/// system of size \f$2N+2\f$.
/// \f[ R(u,\lambda) + \sigma \psi = 0,\f]
/// \f[ Jy = 0,\f]
/// \f[ \langle u, \phi \rangle = 0, \f]
/// \f[ \phi \cdot y = 1.\f]
/// In the abovem \f$J\f$ is the  usual Jacobian matrix, \f$dR/du\f$ 
/// and \f$\phi\f$ is a constant vector that is chosen to ensure that
/// the null vector, \f$y\f$, is not trivial.  
/// Here \f$\sigma \f$ is a slack variable that is used to enforce the 
/// constraint \f$ \langle u, \phi \rangle = 0 \f$ --- the inner product
/// of the solution with the chosen symmetry vector is zero. At the 
/// bifurcation \f$ \sigma \f$ should be very close to zero. Note that 
/// this formulation means that any odd symmetry in the problem must
/// be about zero. Moreover, we use the dot product of two vectors to
/// calculate the inner product, rather than an integral over the 
/// domain and so the mesh must be symmetric in the appropriate directions.
//======================================================================== 
 class PitchForkHandler : public AssemblyHandler
  {
   friend class BlockPitchForkLinearSolver;

   /// \short Boolean flag to indicate whether we are solving 
   /// a block-factorised system or not
   bool Solve_block_system;

   ///Pointer to the problem
   Problem *Problem_pt;

   /// \short Store the number of degrees of freedom in the non-augmented
   ///problem
   unsigned Ndof;

   /// \short A slack variable used to specify the amount of antisymmetry
   /// in the solution. 
  double Sigma;

  /// \short Storage for the null vector
  Vector<double> Y;

  /// \short A constant vector that is specifies the symmetry being broken
  Vector<double> Psi;

  /// \short A constant vector used to ensure that the null vector
  /// is not trivial
  Vector<double> C;

  /// \short A vector that is used to determine how many elements
  /// contribute to a particular equation. It is used to ensure
  /// that the global system is correctly formulated. 
  Vector<int> Count;

 public:

  ///Constructor, initialise the systems
  PitchForkHandler(Problem* const &problem_pt, 
                   double* const &parameter_pt,
                   const Vector<double> &symmetry_vector);
  
  /// Destructor, return the problem to its original state,
  /// before the augmented system was added
  ~PitchForkHandler();

  //Has this been called

  ///Get the number of elemental degrees of freedom
  unsigned ndof(GeneralisedElement* const &elem_pt);
  
  ///Get the global equation number of the local unknown
  unsigned long eqn_number(GeneralisedElement* const &elem_pt,
                           const unsigned &ieqn_local);
   
  ///Get the residuals
  void get_residuals(GeneralisedElement* const &elem_pt,
                     Vector<double> &residuals);
  
  /// \short Calculate the elemental Jacobian matrix "d equation 
  /// / d variable".
  void get_jacobian(GeneralisedElement* const &elem_pt,
                    Vector<double> &residuals, 
                    DenseMatrix<double> &jacobian);

  /// \short Set to solve the block system
  void solve_block_system();

  /// \short Solve non-block system
  void solve_full_system();
  
  };

//========================================================================
/// A custom linear solver class that is used to solve a block-factorised
/// version of the Hopf bifurcation detection problem.
//========================================================================
class BlockHopfLinearSolver : public LinearSolver
{
 ///Pointer to the original linear solver
 LinearSolver* Linear_solver_pt;

 ///Pointer to the problem, used in the resolve
 Problem* Problem_pt;

 ///Pointer to the storage for the vector a
 Vector<double> *A_pt;

 ///Pointer to the storage for the vector e
 Vector<double> *E_pt;

 ///Pointer to the storage for the vector g
 Vector<double> *G_pt;

public:
 
 ///Constructor, inherits the original linear solver
 BlockHopfLinearSolver(LinearSolver* const linear_solver_pt)
  : Linear_solver_pt(linear_solver_pt), Problem_pt(0),
   A_pt(0), E_pt(0), G_pt(0) {}
 
 ///Destructor: clean up the allocated memory
 ~BlockHopfLinearSolver();

 /// Solve for two right hand sides
 void solve_for_two_rhs(Problem* const &problem_pt,
                        Vector<double> &result,
                        const Vector<double> &rhs2,
                        Vector<double> &result2);

 /// The solve function uses the block factorisation
 void solve(Problem* const &problem_pt, Vector<double> &result);

 /// The resolve function also uses the block factorisation
 void resolve(const Vector<double> &rhs, Vector<double> &result);

 /// Access function to the original linear solver
 LinearSolver* linear_solver_pt() const {return Linear_solver_pt;}

};


//===============================================================
/// A class that is used to assemble the augmented system that defines
/// a Hopf bifurcation. The "standard" problem
/// must be a function of a global parameter \f$\lambda\f$ and a solution
/// is \f$R(u,\lambda) = 0\f$, where \f$u\f$ are the unknowns in the 
/// problem. A Hopf bifurcation may be specified by the augmented
/// system of size \f$3N+2\f$.
/// \f[ R(u,\lambda) = 0,\f]
/// \f[ J\phi + \omega M \psi = 0,\f]
/// \f[ J\psi - \omega M \phi = 0,\f]
/// \f[ c \cdot \phi  = 1.\f]
/// \f[ c \cdot \psi  = 0.\f]
/// In the abovem \f$J\f$ is the  usual Jacobian matrix, \f$dR/du\f$ 
/// and \f$M\f$ is the mass matrix that multiplies the time derivative terms.
/// \f$\phi + i\psi\f$ is the (complex) null vector of the complex matrix
/// \f$ J - i\omega M \f$, where $\f \omega \f$ is the critical frequency.
/// \f$ c \f$ is a constant vector that is used to ensure that the null vector
/// is non-trivial.
//===========================================================================
class HopfHandler : public AssemblyHandler
 {
  friend class BlockHopfLinearSolver;

  ///\short Integer flag to indicate which system should be assembled.
  /// There are three possibilities. The full augmented system (0), 
  /// the non-augmented jacobian system (1), and complex
  /// system (2), where the matrix is a combination of the jacobian 
  /// and mass matrices.
  unsigned Solve_which_system;

  ///Pointer to the problem
  Problem *Problem_pt;

  ///Pointer to the parameter
  double *Parameter_pt;

  /// \short Store the number of degrees of freedom in the non-augmented
  ///problem
  unsigned Ndof;

  /// \short The critical frequency of the bifurcation
  double Omega;

  /// \short The real part of the null vector
  Vector<double> Phi;

  /// \short The imaginary part of the null vector
  Vector<double> Psi;

  /// \short A constant vector used to ensure that the null vector is
  /// not trivial
  Vector<double> C;

  /// \short A vector that is used to determine how many elements
  /// contribute to a particular equation. It is used to ensure
  /// that the global system is correctly formulated. 
  Vector<int> Count;

 public:

  /// Constructor
  HopfHandler(Problem* const &problem_pt, double* const &parameter_pt);
  
  /// Constructor with initial guesses for the frequency and null
  /// vectors, such as might be provided by an eigensolver
  HopfHandler(Problem* const &problem_pt, double* const &paramter_pt,
              const double &omega, const Vector<double> &phi,
              const Vector<double> &psi);

  /// Destructor, return the problem to its original state,
  /// before the augmented system was added
  ~HopfHandler();

  ///Get the number of elemental degrees of freedom
  unsigned ndof(GeneralisedElement* const &elem_pt);

  ///Get the global equation number of the local unknown
  unsigned long eqn_number(GeneralisedElement* const &elem_pt,
                           const unsigned &ieqn_local);

  ///Get the residuals
  void get_residuals(GeneralisedElement* const &elem_pt,
                     Vector<double> &residuals);
  
  /// \short Calculate the elemental Jacobian matrix "d equation 
  /// / d variable".
  void get_jacobian(GeneralisedElement* const &elem_pt,
                    Vector<double> &residuals, 
                    DenseMatrix<double> &jacobian);
  
  /// \short Set to solve the standard system
  void solve_standard_system();

  /// \short Set to solve the complex system
  void solve_complex_system();

  /// \short Solve non-block system
  void solve_full_system();

 };



 
}
#endif
