/**

\mainpage (Not-So-)Quick Users Guide
The purpose of this document is to provide a "quick" introduction to the
fundamental objects in \c oomph-lib. Assuming that you 
- have a basic understanding of the finite element method
.
and
- are reasonably familiar with object-oriented programming in C++,
.
the Quick Guide should enable you to solve basic PDE 
problems using \c oomph-lib. You should also consult
the extensive <A HREF="../../example_code_list/html/index.html">
list of example codes</A> distributed with the library. 

\section main_how_to How to solve simple PDEs with oomph-lib

  To solve a given PDE with \c oomph-lib, its discretisation 
needs to be specified in a \c Problem object. 
This usually involves the specification of the \c Mesh and 
the element types, followed by the application of the boundary conditions. 
If these steps are performed in the \c Problem constructor,
the driver code itself can be as simple as:
\dontinclude one_d_poisson_generic_only.cc
\skipline main(
\until } 

The amount of work required to represent a given PDE as
a \c Problem object is problem-dependent:
- If \c oomph-lib's existing \c Mesh objects and element types
  are sufficient to discretise the problem, the generation of
  a fully-functional \c Problem object is straightforward. We shall discuss
  this case in the section \ref problem. 
- If \c oomph-lib provides the required element type but none of the
  available meshes are suitable, 
  you will have to write your own \c Mesh object. We shall discuss this in 
  the section \ref mesh. It is also possible to create
  \c oomph-lib meshes based on the output from third-party (commercial)
  mesh generators. This is illustrated in 
  <A HREF="../../meshes/third_party_meshes/html/index.html">
  another example.</A> 
- Finally, the problem might involve a PDE for which \c oomph-lib
  does not provide any suitable elements. In this case you 
  will need to write your own element. When implementing elements
  in \c oomph-lib the element formulation is generally sub-divided into 
  (at least) two levels. Geometric elements implement the element geometry
  (e.g. 2D quad elements, 3D brick elements, etc); these geometric elements
  act as base classes for specific \c FiniteElements that implement
  the discretisation of particular PDEs. We discuss the
  implementation of the two types of elements in two separate sections:
  - In the section \ref FEequations we describe how to
    discretise a new system of equations, using existing
    geometric elements.
  - In the section \ref FEgeom we discuss how to formulate new geometric
    elements.
.
  
In all cases, we start with a brief description of
the minimum functionality that must be implemented for the
various objects. The general principles are then illustrated
by considering the discretisation of the model problem

<CENTER>
<TABLE class="oomph-table">
<TR>
<TD class="panel panel-default">
<CENTER class="panel-heading">
<B>One-dimensional model Poisson problem</B>
</CENTER>
<div class="panel-body">
Solve
\f[
\frac{\mbox{d}^2u}{\mbox{d}x^2} = \pm 30 \sin(\sqrt{30} x),
 \ \ \ \ \ \ \ \ \ \ (1)
\f]
in the one-dimensional domain \f$x\in[0,1]\f$, subject to 
\f[
u(0)=0 \ \mbox{\ \ \ and \ \ \ } \ \ u(1)=\mp 1, 
\ \ \ \ \ \ \ \ \ \ (2)
\f]
</div>
</TD>
</TR>
</TABLE>
</CENTER>

by isoparametric two-node elements. (Please consult the
<A HREF="../../intro/html/index.html">Introduction</A> for a review of
the relevant finite element theory.) The source code for this example
(<A HREF="../../../demo_drivers/poisson/one_d_poisson_generic_only/one_d_poisson_generic_only.cc">one_d_poisson_generic_only.cc</A>)
is  self-contained and uses only objects from the "generic" part of
the \c oomph-lib library. [\c oomph-lib does, of course, provide 
1D meshes and Poisson elements but we deliberately ignore these 
because we wish to illustrate how to build such objects from scratch.
The alternative example code 
<A HREF="../../../demo_drivers/poisson/one_d_poisson/one_d_poisson.cc">
one_d_poisson.cc</A>,
discussed in <A HREF="../../poisson/one_d_poisson/html/index.html">
another example</A> illustrates the implementation of the same problem using 
existing library objects.]

The main purpose of this document is to 
provide a \e "quick" introduction to \c oomph-lib's fundamental objects,
so the example classes presented below are fully-functional but very
basic. We provide exercises at the end of each section
to allow the reader to explore various straightforward improvements,
most of which are implemented in the corresponding objects
in the library. Finally, the section \ref comments provides a 
discussion of some general design principles that should
be respected when creating new classes for use with the library. 

<HR>
<HR>

\section problem How to build a Problem

All specific \c Problems should be implemented as 
objects that inherit from the generic \c Problem class. These
specific \c Problem objects will vary considerably depending
on the exact details of the problem being solved. 

Only a few member functions \e must be implemented for each specific
\c Problem class:
- The problem constructor \code SomeSpecificProblem(...) \endcode
- and the member functions
  \code Problem::actions_before_newton_solve() \endcode
  \code Problem::actions_after_newton_solve() \endcode
.
\subsection spec The constructor: SomeSpecificProblem(...)
The Problem constructor  \c SomeSpecificProblem(...) 
usually contains the following steps:
- Construction of the timestepper (if required), e.g. 
  \code Problem::add_time_stepper_pt(new SomeTimeStepper()); \endcode   

- Construction of the \c Mesh (which may involve passing a particular
  element type as a template parameter), e.g.
  \code Problem::mesh_pt() = new SomeMesh<SomeElement>(...); \endcode

- Specification of the boundary conditions. By default, all nodal
  values are assumed to be free and their values are initialised to zero. 
  Therefore, we only need to "pin" the values that are prescribed
  by boundary conditions and assign any nonzero boundary values.
  For instance, the following instruction "pins" the (single) nodal 
  value at node "0":
  \code Problem::mesh_pt()->node_pt(0)->pin(0); \endcode
  A non-zero boundary value can be set by
  \code Problem::mesh_pt()->node_pt(0)->set_value(0,1.0); \endcode
  This statement sets the first (= zero-th in C++'s zero-based indexing)
  nodal value to 1.0.

- Completion of the build of the elements: Elements are typically
  constructed during the mesh generation process. To allow a 
  generic implementation of this process, mesh constructors 
  use an argument-free constructor to
  create the elements. Thus, element constructors must not
  have any arguments. (See the section `\ref new_mesh', below, for an 
  illustration of this process.) If an element requires any global parameters
  (such as pointers to physical parameters, function pointers 
  to source functions, etc.), these parameters should be passed to the
  element via suitable access functions, \e after the element has been 
  created by the mesh constructor.

- Assignment of the global and local equation numbers 
  \code Problem::assign_eqn_numbers(); \endcode


\subsection actbef The function Problem::actions_before_newton_solve()
\c oomph-lib treats all problems as non-linear problems and 
employs Newton's method to solve the system of nonlinear algebraic
equations that results from its spatial (and, in time-dependent
problems, temporal) discretisation. Within this framework, linear
problems are special cases for which Newton's method converges
in one step.
 
The (pure virtual) member function \c Problem::actions_before_newton_solve() 
should contain everything that \e must be done before a nonlinear 
solve to complete the
specification of the problem. For example, the function may contain a call to
update the boundary conditions.
\code void actions_before_newton_solve() {update_boundary_conditions();}
\endcode

If desired, finer granularity may be obtained by overloading the 
empty virtual 
functions \c Problem::actions_before_newton_convergence_check(),
\c Problem::actions_before_newton_step() and/or 
\c Problem::actions_before_implicit_timestep(), which are executed 
before each calculation of the maximum value of the residuals,
each Newton step or each timestep, respectively. We refer to a 
<A HREF="../../order_of_action_functions/html/index.html">separate
document</A> for a more detailed discussion of the various
"action functions" executed by \c oomph-lib's Newton solver. 

\subsection actafter The function Problem::actions_after_newton_solve()
The (pure virtual) member function \c Problem::actions_after_newton_solve() 
should contain
everything that is to take place after each nonlinear solve. For
example, the function might
contain commands to update the values of any "dependent" variables, or 
post-processing commands.
\code 
void actions_after_newton_solve()
 {
  //Update value of the slave variable in namespace GlobalVariables
  GlobalVariables::slave = node_pt(0)->value(0) + node_pt(0)->value(1);

  //Call the output function
  ofstream output_file("result.dat");
  output(output_file); 
 }
 \endcode

Again, the finer-grained member functions 
\c Problem::actions_after_newton_step() and  
\c Problem::actions_after_implicit_timestep() are provided.


<HR>

\subsection full An example of a "complete" specific Problem
Here is the full specification for the \c DemoPoissonProblem class
(taken from <A HREF="../../../demo_drivers/poisson/one_d_poisson_generic_only/one_d_poisson_generic_only.cc">one_d_poisson_generic_only.cc</A>)
which implements the discretisation of the 1D Poisson problem
described above, using a \c Mesh of type  \c OneDimMesh, with 
elements of type \c TwoNodePoissonElements.

\dontinclude one_d_poisson_generic_only.cc
\skipline DemoPoissonProblem
\until End of problem 


<HR>
\subsection problem_exercise Exercises:
- Compile and run the example code 
  <A HREF="../../../demo_drivers/poisson/one_d_poisson_generic_only/one_d_poisson_generic_only.cc">one_d_poisson_generic_only.cc</A> and compare the numerical 
  results against the (fish-shaped) exact solution
  \f[
  u_{fish}(x) =  
  \pm \left[ \left(\sin(\sqrt{30})-1\right) x - \sin(\sqrt{30} x)\right].
  \f]
  (The sign of the source function is an optional argument to the
  problem constructor. If no argument is specified, the + sign
  is used.)  Modify the \c actions_after_newton_solve() function so that it
  writes the exact solution into another file, "exact_solution.dat", say.
- Vary the number of elements and observe how the numerical results
  converge to the exact solution.
- Adjust the boundary conditions:
  - Change the boundary condition at the left end of the domain 
    to \f$ u(0)=1\f$.
  - What happens when you don't apply a boundary condition 
    at \f$ x=1  \f$? (Hint: What are the natural boundary conditions
    for the Poisson equation?) 
  - What happens when no boundary conditions are applied?
  .
- Suppress the assignment of the equation numbers in the problem
  constructor and observe what happens. Check the error messages
  produced by \c Problem::self_test().
.

<HR>
<HR>

\section mesh How to build a Mesh

\c oomph-lib provides a large number of fully-functional \c Mesh objects. 
Many of these meshes can easily be adapted to discretise other
domains, provided the domain has the same topology as the
original \c Mesh. We shall illustrate this in section \ref distorted_mesh 
below.


\subsection new_mesh How to build a completely new Mesh


If the domain is too different from any of the existing meshes, a new,
customised \c Mesh object, \c SpecificMesh, say, must be created. 
The specific mesh should be created as an object that inherits from the 
generic \c Mesh class. To maximise the potential for code-reuse, we
recommended making the element type a template parameter
of the \c SpecificMesh class.

\code
template<class ELEMENT>
class SpecificMesh : public Mesh
\endcode

This allows the \c SpecificMesh to be used
with any \c FiniteElement that is based on the same geometric
element type (see the section \ref FEgeom for a more detailed discussion of 
geometric elements).

The minimum requirements for a specific mesh object are that it must:
- Construct the elements and the \c Nodes,
- Store pointers to the elements in the \c Mesh::Element_pt vector,
- Store pointers to the \c Nodes in the \c Mesh::Node_pt vector,
- Set the elements' pointers to their local \c Nodes
- Set the positions of the \c Nodes.
.

We suggest that all specific \c Mesh constructors also set up 
an auxiliary lookup scheme that stores information about the mesh
boundaries. While this is not strictly required, it greatly facilitates
the application of boundary conditions in complex domains. Firstly,
the function \c Mesh::set_nboundary(...) must be called to specify the
number of mesh boundaries. In addition, the generic \c Mesh class 
provides a function \c Mesh::add_boundary_node(i,node_pt), 
which adds the \c Node pointed to by \c node_pt to the \c Mesh's i-th 
boundary and constructs the required
look-up schemes, including a reverse lookup scheme that informs all 
\c Nodes which mesh boundaries (if any) they are located on. 
(In our simple 1D example there are only
two boundaries, each consisting of a single Node.) 
Although this step is optional, it \e is required 
during mesh adaptation -- if the reverse lookup scheme has not been 
set up, a warning is issued and the code execution stops when any mesh 
adaptation is attempted. \b Implementation \b detail: \c Nodes that are
located on mesh boundaries must be defined as \c BoundaryNodes.  
 
The majority of the work required to build a specific \c Mesh
takes place in its constructor. Typically, the constructor creates
the elements (of the type specified by the template parameter \c
ELEMENT)  and uses the elements' member function \c
FiniteElement::construct_node(...) to create the new \c Nodes. The
equivalent member function \c
FiniteElement::construct_boundary_node(...) 
is used to create new \c BoundaryNodes. 

Here is the complete class definition for a simple, one-dimensional 
(line) mesh which discretises the 1D domain \f$x\in [0,1]\f$ using 
a specified number of equally-spaced elements.
\dontinclude one_d_poisson_generic_only.cc
\skipline template<
\until };


To build a \c OneDimMesh with ten elements of type \c SomeElement, say, the 
\c Problem constructor would contain the following
\code
Problem::mesh_pt() = new OneDimMesh<SomeElement>(10);
\endcode

\subsection distorted_mesh How to adapt an existing Mesh to a different domain shape

Given that mesh generation tends to be a fairly tedious process, one should
always check whether it is possible to deform an existing \c Mesh
into a shape that matches the required domain. Provided that the 
new domain has the same topology as the domain represented 
by the original \c Mesh, this can always be done by 
re-positioning the Nodes. Such "deformed" \c Meshes should be implemented
via inheritance, by deriving the new \c Mesh from an existing one.

Here is an example that illustrates the procedure. Assume we wish to 
solve an equation in the 2D annular domain bounded (in polar
coordinates) by \f$ r \in [r_{min}, r_{max}] \f$ and  \f$ \varphi \in 
[\varphi_{min}, \varphi_{max}] \f$. Inspection of the 
<A HREF="../../meshes/mesh_list/html/index.html"> list of available
meshes </A> shows that \c oomph-lib does not provide a mesh for 
this geometry. However, there is a \c Mesh object,
 \c SimpleRectangularMesh, which provides a uniform 
discretisation of a 2D rectangular domain \f$ x\in [0,L_x] \f$
and \f$ y \in [0,L_y] \f$ with \f$ N_x \times N_y \f$
quadrilateral elements. Since the topology of the two domains
is identical, the annular mesh can be implemented in a few 
lines of code:

\code
//====================================================================
/// AnnularQuadMesh, derived from SimpleRectangularQuadMesh.
//====================================================================
template<class ELEMENT> 
class AnnularQuadMesh : public SimpleRectangularQuadMesh<ELEMENT>
{
 
  public:

 /// \short Constructor for angular mesh with n_r x n_phi 
 /// 2D quad elements. Calls constructor for the underlying 
 /// SimpleRectangularQuadMesh; then deforms the mesh so that it fits 
 /// into the annular region bounded by the radii r_min and r_max
 /// and angles (in degree) of phi_min and phi_max.
 AnnularQuadMesh(const unsigned& n_r, const unsigned& n_phi,
                 const double& r_min, const double& r_max,
                 const double& phi_min, const double& phi_max) :
  SimpleRectangularQuadMesh<ELEMENT>(n_r,n_phi,1.0,1.0)
  {

   // The constructor for the  SimpleRectangularQuadMesh has
   // built the mesh with n_x x n_y = n_r x n_phi elements in the unit
   // square. Let's reposition the nodal points so that the mesh
   // gets mapped into the required annular region:

   // Find out how many nodes there are
   unsigned n_node=nnode();

   // Calculate the value of pi
   const double pi = 4.0*atan(1.0);

   // Loop over all nodes
   for (unsigned n=0;n<n_node;n++)
    {
     // Pointer to node:
     Node* nod_pt=node_pt(n);

     // Get the x/y coordinates
     double x_old=nod_pt->x(0);
     double y_old=nod_pt->x(1);

     // Map from the old x/y to the new r/phi:
     double r=r_min+(r_max-r_min)*x_old;
     double phi=(phi_min+(phi_max-phi_min)*y_old)*pi/180.0;

     // Set new nodal coordinates
     nod_pt->x(0)=r*cos(phi);
     nod_pt->x(1)=r*sin(phi);
    }
  }

};
\endcode



<HR>

\subsection mes_exercise Exercise:
Modify the \c OneDimMesh object so that it provides a piecewise 
uniform discretisation of the domain
\f$ x \in [0,1]\f$: \f$ N_0 \f$ equally spaced elements 
are to be placed in the the region \f$ x \in [0,\hat{x}]\f$, while
\f$ N_1 \f$ elements are to be placed in the the region \f$ x \in
[\hat{x},1]\f$. Pass the parameters \f$ N_0, N_1  \f$
and \f$\hat{x} \f$ to the mesh constructor.
Construct the modified mesh via inheritance from the basic \c OneDimMesh
and include an error check to confirm that \f$ 0 < \hat{x} < 1 \f$. 


<HR>
<HR>
 
\section finiteelement How to build a FiniteElement

\c oomph-lib provides fully-functional \c FiniteElements for 
the discretisation of a wide range of PDEs. Most of the existing
elements are constructed in a
three-level hierarchy, the base being the generic 
\c FiniteElement class. The next level in the hierarchy contains
geometric elements (e.g. 1D line elements, 2D quad or triangular
elements, 3D brick elements, etc.). These geometric
classes form the bases for elements that implement
the discretisation of particular PDEs. This hierarchy maximises
the potential for code-reuse, because it allows many different
specific elements to be derived from the same geometric element.
 
We shall discuss the implementation of new element types in
two sections:
- \ref FEequations
- \ref FEgeom

<HR>

\subsection FEequations How to implement a new system of equations as a specific FiniteElement

Assume that we wish to design a new element type, \c SpecificElement,
say, that implements the discretisation of a system of PDEs on an existing
geometric element. The \c SpecificElement class needs to implement
the following functions:
- The function that specifies the number of nodal values
  required at each of the element's nodes:
  \code FiniteElement::required_nvalue(n) \endcode 
- The function that computes the element's residual vector:
  \code FiniteElement::get_residuals(residuals) \endcode
- The function that computes the element's Jacobian matrix and
  its residual vector:
  \code FiniteElement::get_jacobian(residuals,jacobian) \endcode 
- The output function:
  \code FiniteElement::output(ostream) \endcode
.


Most specific finite element classes will contain further
member functions and member data to provide 
additional, problem-specific functionality.
As a concrete example, we consider the \c TwoNodePoissonElement,
a specific \c FiniteElement that provides an isoparametric
discretisation of the 1D Poisson equation (1), based on 
the geometric element \c TwoNodeGeometricElement, to be discussed below:
\dontinclude one_d_poisson_generic_only.cc
\skipline TwoNodePoisson

In addition to the \c FiniteElement member functions discussed above, 
this element provides a function that defines the source function
\f$ f(x) \f$,
\dontinclude one_d_poisson_generic_only.cc
\skipline f(
\until }

where the sign of the source function is stored as private member data
\dontinclude one_d_poisson_generic_only.cc
\skipline int Sign

and can be set by the access function
\dontinclude one_d_poisson_generic_only.cc
\skipline sign(

We provide a member function that returns the (single) 
nodal value stored at a specified local node in the element,
\dontinclude one_d_poisson_generic_only.cc
\skipline u(

Finally, it is good practice to implement a self-test function 
that provides a sanity check of all data before
the problem is solved. \c oomph-lib already provides self-test functions
for all fundamental objects. Additional tests can be added
by overloading these. For instance, in our Poisson element, the
\c Sign variable should only take the values \f$ \pm 1 \f$.
This can be tested with the following function,
which also executes the \c self_test() function of the underlying
\c FiniteElement:
\dontinclude one_d_poisson_generic_only.cc
\skip Self test function
\until End of self test

We will now discuss the implementation of the generic \c FiniteElement 
member functions for the specific \c TwoNodePoissonElement:

\subsubsection Fereq_nvalue The function FiniteElement::required_nvalue(n)
The function returns the number of values that are stored at the 
element's \c n-th
local node. In our scalar Poisson problem, each node stores one value:

\dontinclude one_d_poisson_generic_only.cc
\skip For the Poisson 
\until required_nvalue(

The function is used by \c FiniteElement::construct_node(...) to
determine the amount of storage to be allocated at
each of the element's \c Nodes.

\subsubsection residuals The function FiniteElement::get_residuals(residuals)
This function calculates the element's residual vector whose entries
are labelled by the \e local equation numbers. (Consult the 
<A HREF="../../intro/html/index.html">Introduction</A> for
a detailed discussion of the weak form of Poisson's equation and its finite
element discretisation which produces the discrete residual
implemented here.) The spatial integration over the element is 
performed by the (default) integration scheme specified in the 
underlying geometric element. Note that it should (must) not be
assumed that the residuals vector has been set to zero before the
function call.
\skipline get_residuals(
\until End of function

\subsubsection jac The function FiniteElement::get_jacobian(residuals,jacobian)
This function calculates the element's Jacobian matrix and its 
residual vector, performing the spatial integration with the
(default) integration scheme defined in the underlying geometric element. 
The entries in the Jacobian matrix and the residual vector are labelled 
by the \e local equation numbers. Note that neither the residuals
vector nor the Jacobian matrix should be assumed to have been initialised to
zero before the function call.
\skipline get_jacobian(
\until End of function

\b Note: There is a large amount of code duplication between 
the \c get_residuals()
and \c get_jacobian() functions. To avoid this, we usually 
implement the computation of the residual vector and the 
Jacobian matrix in a single
function containing the loop over the integration points. We then
use a boolean flag as an additional argument to determine whether the
Jacobian matrix should be assembled, e.g.
\code
void get_generic_residual_contribution(Vector<double>& residuals,
                                       DenseMatrix<double>& jacobian,
                                       bool flag)         
\endcode


\subsubsection FEout The function FiniteElement::output(out_stream)
The output function is used to print the nodal values over the element
\skipline output(
\until End of function

<HR>
\subsection FE_exercise Exercises:
- Implement the function \c get_generic_residual_contribution(...) as a private
  member function of the \c TwoNodePoissonElement. Use the function to avoid
  the large amount of code duplication between \c get_residuals(...) and
  \c get_jacobian(...) by re-writing these functions as follows:
  \code
  /// \short Calculate the elemental contributions to the residuals for 
  /// the weak form of the Poisson equation
  void get_residuals(Vector<double> &residuals)
   {
    // Set flag for not computing the Jacobian
    bool flag=false;

    // Dummy Jacobian
    DenseMatrix<double> jacobian;

    // Compute the residuals only:
    get_generic_residual_contribution(residuals,jacobian,flag);
   }
  \endcode
  and 
  \code
  /// \short Calculate the elemental contribution to the Jacobian 
  /// matrix dR_{i}/du_{j} used in the Newton method
  void get_jacobian(Vector<double> &residuals, DenseMatrix<double> &jacobian)
   {
    // Set flag for computing the Jacobian matrix
    bool flag=true;

    // Compute the residuals and the Jacobian matrix:
    get_generic_residual_contribution(residuals,jacobian,flag);
   }
  \endcode

- The implementation of the specific source function  
  \f$ f(x) \f$ in \c TwoNodePoissonElement::f(...) 
  makes it impossible to use the \c TwoNodePoissonElement to solve
  the Poisson equation with any other source functions, even though
  the discretisation of the ODE would otherwise be completely identical. 
  A better implementation would allow the "user" to specify a different source
  function without having to change the implementation of the 
  element class itself. 
  \n \n
  Here is a suggestion for an improved implementation, which illustrates the
  procedure employed in the existing elements in \c oomph-lib:
  - Introduce a public typedef into the element class to define
    the required format of the function pointer:
    \code
    /// \short Function pointer to source function: 
    /// The source function returns the value of the 
    /// source function at the global coordinate x.
    typedef double (*PoissonSourceFctPt)(const double& x);
    \endcode
  - Add a function pointer to the private data of the \c
    TwoNodePoissonElement class and initialise it to NULL
    in the constructor:
    \code
    /// \short Function pointer to source function (initialised to
    /// NULL in the constructor)
    PoissonSourceFctPt Source_fct_pt;
    \endcode
  - Provide an access function for the source function pointer:
    \code
    /// \short Access function to pointer to source function
    PoissonSourceFctPt& source_fct_pt() {return Source_fct_pt;}
    \endcode
  - Re-write the \c TwoNodePoissonElement::f(...) function,
    so that it evaluates the (global) function pointed to by
    the source function pointer.  If possible, provide a default value 
    for the case when the function pointer has not been set:
    \code
    /// Evaluate source function at Eulerian position x
    double f(const double& x) const
     { 
      //If no source function has been set, return zero
      //so that the Poisson equation defaults to a Laplace equation.
      double source=0.0;
      if(Source_fct_pt!=0)
       {
        // Evaluate source function 
        source = (*Source_fct_pt)(x);
       }
      return source;
     }
    \endcode
  .
  The pointer to the source function (typically defined in a suitable
  namespace in the "user's" driver code) can now be set in the 
  Problem constructor.
- Generalise the \c TwoNodePoissonElement to a \c
  TwoNodeSelfAdjointElement that implements the isoparametric
  discretisation of the self-adjoint ODE
  \f[
  \frac{\mbox{d}}{\mbox{d}x} 
  \left( a(x) \frac{\mbox{d}u}{\mbox{d}x} \right) + b(x) u(x)
  = f(x). \ \ \ \ \ \ \ \ \  (3)
  \f]
  Use function pointers to allow the "user" to specify 
  the coefficient functions \f$ a(x), \ b(x) \f$ and \f$ f(x) \f$.
.


<HR>

\subsection FEgeom How to build a new geometric element

Geometric elements define the geometry of a \c FiniteElement. They are
usually implemented as distinct classes that can then be used to create
a number of \c FiniteElements each discretising a specific PDE, but 
with the same underlying geometrical representation. 
For this purpose, each geometric \c FiniteElement must 
implement the following functions:
- The constructor
  \code SomeGeometricFiniteElement::SomeGeometricFiniteElement() \endcode 
  must perform the following actions:
  - Set the number of Nodes by calling the protected member function
    \code FiniteElement::set_n_node(n_node). \endcode
  - Set the dimension of the element by calling the protected member
  function \code FiniteElement::set_dimension(dim)\endcode 
  Here the dimension of the element is defined as the number of 
  local coordinates required to parametrise its shape.
  (Note that this is not necessarily the same as the spatial dimension
  of the problem -- we can have one-dimensional \c FiniteElements 
  in 3D space, say.) 
  - \b Optional: 
  Set the spatial integration scheme by calling the protected member
  function \code
  FiniteElement::set_integration_scheme(&integration_scheme) \endcode
  - \b Optional: 
  If the spatial dimension (= the number of Eulerian coordinates)
  required at all Nodes in the element is different from the spatial
  dimension of the element then set the "nodal dimension" 
  by calling the protected 
  member function \code FiniteElement::set_nodal_dimension(dim)\endcode
  For example in shell elements, a two-dimensional surface is embedded
  in a three-dimensional space, so the elemental dimension is two, but
  the nodal dimension is three.
  - \b Optional: If the Eulerian coordinate (position) is interpolated using
  generalised coordinates, e.g. vector-valued shape functions, or
  Hermite-type interpolation, set the number of generalised
  coordinates or "position types"  by using the protected member function
  \code FiniteElement::set_n_nodal_position_type(n_position_type)\endcode

- The function that computes the element's (geometric) shape functions
  at specified values of the local coordinates. These shape
  functions are primarily used to implement the mapping between the
  element's local coordinates and the global (Eulerian) coordinates.  
  The geometric shape functions \e can be (and often are) also used in 
  specific \c FiniteElements to interpolate the unknown 
  function(s) between the nodal values. 
  \code FiniteElement::shape(s,psi) \endcode

It is usually necessary to implement the following additional functions:
- The function that computes the derivatives of the element's shape
  functions with respect to the local coordinates,
  \code FiniteElement::dshape_local(s,psi,dpsids) \endcode
- The function that specifies the number of Nodes along the
  element's 1D "edges"  
  \code FiniteElement::nnode_1d() \endcode 
.
Further member functions and member data may be defined as and when required.
For instance, all specific \c FiniteElements must store a pointer to an
(instantiated) spatial integration scheme. It is good practice to
provide a default integration scheme for each geometric element and 
to ensure its instantiation by making it
a static data member of the geometric element class. This allows the 
constructor of the geometric element to set the pointer to the default 
integration scheme. If the default is not appropriate for a 
specific derived \c FiniteElement, the default assignment
can be over-written in the constructor of the derived class.

As a concrete example, we consider the implementation of the
one-dimensional, two-node geometric element, \c TwoNodeGeometricElement, 
that uses linear shape functions to define the mapping between
the element's local and global coordinates. The element is derived
from the \c FiniteElement base class,
\dontinclude one_d_poisson_generic_only.cc
\skipline TwoNodeGeometric
 
and it uses a one-dimensional, two-point Gauss rule
as the default spatial integration scheme 
\skipline Integration
\until static 

Here is the implementation of the generic \c FiniteElement member
functions for our specific geometric \c FiniteElement:

\subsubsection Geomconst The constructor: TwoNodeGeometricElement()
The constructor sets the number of local nodes in the element (2),
sets the dimension of the element (1) and 
sets the pointer to the spatial integration scheme 
to be the default that is defined and instantiated as a static data
member of the class: 
\skipline TwoNodeGeometric
\until }

\subsubsection shape The function FiniteElement::shape(s,psi)
This function returns the shape functions at the local
element coordinate, s. The shape functions are used
to interpolate the position, and also, any data values stored at 
nodes in derived, isoparametric elements.
\skipline shape(
\until }

\subsubsection dshape The function FiniteElement::dshape_local(s,psi,dpsids)
This function returns the shape functions and their derivatives with
respect to the local coordinates
\skipline dshape_
\until }


\subsubsection nnode_1d The function FiniteElement::nnode_1d()
Return the number of nodes along the element's 1d "edge" -- for
a one-dimensional element, this is obviously the 
same as the number of nodes:
\skipline nnode_1d()

<HR>
\subsection geom_element_exercise Exercises:
- Change the \c TwoNodeGeometricElement to a \c
  ThreeNodeGeometricElement in which quadratic interpolation
  is used to interpolate the Eulerian coordinates
  between the nodal points. Use this element as a basis
  for a \c ThreeNodePoissonElement and convince yourself that
  changing the \c TwoNodePoissonElement 
  to a  \c ThreeNodePoissonElement only requires the change of
  a single line of code!
- Use templating to generalise the geometric elements to 
  an arbitrary number of nodes so that the class
  \code
  template <unsigned NNODE>
  class GeometricLineElement : public FiniteElement
  \endcode
  represents 1D line elements with \c NNODE nodes. Consider carefully
  which member functions you can implement in generality and
  which member functions require specialised implementations.
  Provide the specialised member functions for elements
  with two, three and four nodes.
- Use the templated  \c GeometricLineElements to implement an equivalent
  generalisation of the \c TwoNodePoissonElement and 
  \c ThreeNodePoissonElement classes to the general class
  \code
  template <unsigned NNODE>
  class PoissonLineElement : public GeometricLineElement<NNODE>
  \endcode 
- Validate all elements against the exact solution and
  confirm the theoretical error estimate which predicts that 
  if the domain \f$ x \in [0,1]\f$ is discretised with \f$ N \f$
  equally spaced \f$n\f$-node line elements, we have
  \f[
  e = \sqrt{ \int_0^1 \left( u_{FE}(x) - u_{exact}(x)  \right) ^2 dx }
  \sim h^{(n-1)} \mbox{ \ \ \ \ as $N \to \infty$}
  \f]       
  where \f$ h = 1/N \f$.
.
<HR>
<HR>

\section comments Further comments
We reiterate that the main purpose of this document is to 
provide a \e quick introduction to \c oomph-lib's fundamental objects.
The exercises have already highlighted several undesirable features
of the simple example classes which could easily be improved to 
facilitate the (re-)use of the classes in different problems. 
Here we shall briefly discuss some further modifications that 
we regard as good practice, and which tend to be implemented 
in the existing classes in \c oomph-lib:
- \ref dim_independent
- \ref pre_compute_psi
- \ref equation_classes
- \ref paranoia 
- \ref magic
- \ref multi_physics

<HR>

\subsection dim_independent Dimension-independent implementation

The exercises in the section \ref FEgeom suggest the
use of templating to implement families of objects. This idea
also permits the implementation of (geometric and specific)
\c FiniteElements in arbitrary spatial dimensions. For instance,
\c oomph-lib's \c QElement class represents the family of 
(line/quadrilateral/brick-shaped) geometric elements
with an arbitrary number of nodes along the elements' 1D edges.
Thus, a \c QElement<1,3> is a three-node 1D line element,
a  \c QElement<3,2> is an eight-node 3D brick element, etc.

These geometric elements naturally form the basis for 
corresponding specific elements, such as the \c QPoissonElement family
which provides an isoparametric discretisation of the Poisson
equation (in an arbitrary spatial dimension), based on the
\c QElements with the same template parameters.

<HR>

\subsection pre_compute_psi Using pre-computed shape functions and the StorableShapeElement<ELEMENT> class

 The computation of the element's residual vector and the Jacobian
matrix requires the evaluation of the element's shape functions,
\f$ \psi_j \f$, and their derivatives with respect to the global
coordinate, \f$ \mbox{d}\psi_j/\mbox{d}x \f$, at the 
element's integration points. In our simple example class, 
\c TwoNodePoissonElement, we (re-)compute these functions
"on the fly", using the function
\code
FiniteElement::dshape_eulerian(s,psi,dpsidx)
\endcode
where the local coordinate of the integration point is passed
in the (one-dimensional) vector \c s.

  The re-computation is wasteful because:
- The values of the shape functions and their derivatives, evaluated
  at the integration point, are independent of the value of the 
  \c Sign variable. Their re-computation during the second
  solve could therefore be avoided if we stored their values
  in a suitable container.
- The values of the shape functions at the integration points
  are identical for all elements. Therefore we only need to provide
  storage for the shape function values in a single, representative
  element. The other elements can access the values via pointers.
.
To circumvent these potential inefficiencies, \c oomph-lib 
provides alternative interfaces for various functions that compute
shape functions and their derivatives:
- Calls to the function 
  \code
  FiniteElement::shape(s,psi)
  \endcode
  which computes the shape function at a given local coordinate, \c s,
  can be replaced by
  \code
  FiniteElement::shape_at_knot(int_point,psi)
  \endcode
  where the \c unsigned argument \c int_point identifies
  the integration point (as specified by the
  element's spatial integration scheme).
 In "standard" \c FiniteElements the function 
 \c FiniteElement::shape_at_knot(...) simply determines the position
 of the integration point and calls \c FiniteElement::shape(...) and
 so the shape functions are still computed "on the fly". 
 The templated class \c StorableShapeElement<ELEMENT>, however,
  may be used to
 upgrade any class derived from \c FiniteElement into a class that \b
 can store the values of the shape functions and their derivatives at
 the integration points. The function \c shape_at_knot(...) is
 overloaded so that when called for the first
 time, it computes the values of the shape function
 at all integration points and stores them for future reference.
 In subsequent calls, the function returns the stored values, rather 
 than re-computing them. 
- There are equivalent alternatives for the functions that
  compute the shape functions and their derivatives with respect
  to the local coordinates:
  \code
  FiniteElement::dshape(s,psi,dpsids)
  \endcode
  can be replaced by
  \code
  FiniteElement::dshape_at_knot(int_point,psi,dpsids)
  \endcode
  while
  \code
  FiniteElement::d2shape(s,psi,dpsids,d2psids)
  \endcode
  can be replaced by
  \code
  FiniteElement::d2shape_at_knot(int_point,psi,dpsids,d2psids)
  \endcode
.
These functions are all overloaded in the \c
StorableShapeElement<ELEMENT> class and by default the 
overloaded functions store the pre-computed values of the
shape functions and their derivatives locally within each element. 
This implementation ensures data locality and should increase 
the speed of access to the stored values. However, it
can also create significant storage overheads. \c oomph-lib 
therefore provides the function 
\code 
StorableShapeElement<ELEMENT>::set_shape_local_stored_from_element(...)
\endcode
which frees up the storage in the element and replaces it by 
pointer-based access to the values stored in another element.
Typically, all elements in a mesh are of the same type and use
the same spatial integration scheme and so the values of the shape
functions need only be stored in one element.

Derivatives of the element's shape functions with respect to 
the global Eulerian coordinates are generally
computed by
\code
FiniteElement::dshape_eulerian(s,psi,dpsidx)
\endcode
and 
\code
FiniteElement::d2shape_eulerian(s,psi,dpsidx,d2psidx)
\endcode
These functions also return the Jacobian of the mapping between
the local and global coordinates -- the Jacobian can also be
computed independently from 
\code
FiniteElement::J_eulerian(s)
\endcode
The derivatives of the shape functions with respect the
global coordinates depend on the nodal positions, and it is not safe
to assume that they will remain constant. For instance
in moving boundary problems, the position of the nodes is
determined as part of the solution. By default, the functions 
\code
StorableShapeElement<ELEMENT>::dshape_eulerian_at_knot(int_point,psi,dpsidx)
\endcode
\code
StorableShapeElement<ELEMENT>::d2shape_eulerian_at_knot(int_point,psi,dpsidx,d2psidx)
\endcode
and
\code
StorableShapeElement<ELEMENT>::J_eulerian_at_knot(int_point)
\endcode
re-compute the derivatives at the specified integration 
point "on the fly". If the "user" is confident
that in his/her problem, the nodal positions will not change, the values
can be pre-computed by calling 
\code
StorableShapeElement<ELEMENT>::pre_compute_dshape_eulerian_at_knots()
\endcode
\code
StorableShapeElement<ELEMENT>::pre_compute_d2shape_eulerian_at_knots()
\endcode
\code
StorableShapeElement<ELEMENT>::pre_compute_J_eulerian_at_knots()
\endcode
Once these functions have been called, any subsequent calls to 
the \c *_eulerian_at_knot(...) functions return the stored values.
To revert to the case in which the derivatives are re-computed
"on the fly", the storage for the derivatives must be deleted by  
calling 
\code 
StorableShapeElement<ELEMENT>::delete_dshape_eulerian_stored()
\endcode

\b Notes: 
- In the (unlikely!) case that an element's spatial integration
  scheme is changed during the code execution, all stored
  values are automatically re-computed.
- The class 
  \c StorableShapeSolidElement<ELEMENT> provides equivalent overloaded
  functions for derivatives
  with respect to the element's Lagrangian coordinates that are used in solid
  mechanics problems. 


<HR>

\subsection equation_classes Further sub-division of specific element classes -- equation classes.

We suggested that any newly developed elements should be constructed
in a three-level hierarchy, \c FiniteElement -- Geometric Element --
Specific \c FiniteElement. Most finite elements in \c oomph-lib
incorporate an additional intermediate "equation class" which implements the
computation of the element residual vector and the element Jacobian
matrix in terms of abstract shape and test functions, defined
as pure virtual functions in the "equation class". This makes it
easy to change the specific element formulation, without having to
re-implement the weak form of the governing equation. 

Note that different element types may store the same physical variable at
different locations. For example, the pressure in the Navier--Stokes
equations may be stored as internal \c Data (discontinuous) or nodal
\c Data
(continuous). Particular equation classes may require 
internal numbering schemes that store the appropriate 
local equation numbers for each physical variable. 
These schemes must be assembled for each specific element in
the function 
\c GeneralisedElement::assign_additional_local_eqn_numbers(), which is called
from within \c Problem::assign_eqn_numbers(). 

  As an example, consider the weak form of the 2D advection diffusion
equation
\f[
\int \left(\mbox{Pe} \ \sum_{i=1}^2 w_i \frac{\partial u}{\partial x_i} \psi^{(test)}_l  + 
\sum_{i=1}^2 \frac{\partial u}{\partial x_i}
             \frac{\partial \psi^{(test)}_l}{\partial x_i} \right) dA
             = 0,
\f]
where the Peclet number, \f$ Pe \f$ , and the "wind" \f$w_i \ (i=1,2)\f$
are given. We expand the unknown function \f$u(x_1,x_2)\f$ in terms of 
the (global) basis functions  \f$ \psi^{(basis)}_k(x_1,x_2), \ (k=1,...,N)\f$,
\f[
u(x_1,x_2) = \sum_{k=1}^N U_k \ \psi^{(basis)}_k(x_1,x_2),
\f]
where \f$N\f$ is the total number of nodes in the mesh. 
The mapping between the element's local and global coordinates
is represented in terms of the local shape functions
\f$\psi^{(shape)}_j(s_1,s_2)\f$ as
\f[
x_i = \sum_{j=1}^n X_{ij} \ \psi^{(shape)}_j(s_1,s_2)
\f]
where \f$ n \f$ is the number of nodes in the element. 

 The following sketch illustrates how  this discretisation
is implemented in \c oomph-lib's \c QAdvectionDiffusionElement --
an isoparametric, quadrilateral element, based on the Galerkin
discretisation of the weak form with
\f$ \psi^{(shape)}_j =   \psi^{(test)}_j =   \psi^{(basis)}_j \f$

\image html advection_diffusion_element_inheritance.gif "Typical inheritance diagram for oomph-lib elements " 
\image latex advection_diffusion_element_inheritance.eps "Typical inheritance diagram for oomph-lib elements " width=0.75\textwidth

At large Peclet number, the Galerkin discretisation of the advection
diffusion equation is well-known to  produce spurious "wiggles" in the 
solution. These can be suppressed by SUPG stabilisation which 
employs test functions,  \f$ \psi_j^{(test)}
\f$, that differ from the basis function, \f$ \psi_j^{(basis)} \f$,
\f[
\psi_j^{(test)} = \psi_j^{(basis)} + 
\tau^{(SUPG)} \sum_{i=1}^2 w_i \frac{\partial
\psi_j^{(basis)}}{\partial x_i},
\f]
where \f$ \tau^{(SUPG)} \f$ is a stabilisation parameter.
This can be implemented with a trivial change to the
\c QAdvectionDiffusionElement class -- the 
\c QSUPGAdvectionDiffusionElement simply provides a different
implementation of the test functions.


<HR>

\subsection paranoia Implement extensive (but optional) self tests in all classes

We have already illustrated the use of \c oomph-lib's generic \c self_test()
functions. The top-level \c Problem::self_test() function 
performs a systematic test of all fundamental objects
involved in a specific problem and can be used to diagnose
any problems. It is good practise to implement 
further \c self_test() functions in any newly developed classes.
The generic \c self_test() functions are defined to be virtual
functions in \c oomph-lib's fundamental objects and can
be overloaded. Obviously, the \c self_test() function in a
specific derived object should still call the \c self_test()
function of the underlying fundamental object. The 
\c TwoNodePoissonElement::self_test() function, listed in
in the section \ref FEequations illustrates the procedure. 

  While frequent sanity checks are helpful during code-development,
they can introduce significant overheads into the
code execution. \c oomph-lib therefore provides a compiler flag PARANOID,
which allows the execution of sanity checks to be switched on
or off. When developing new classes, sanity checks should
be implemented to catch any potential problems, but the relevant
code should be surrounded by <CODE>ifdef/endif</CODE> statements to allow
the tests to be disabled. Here is an example:
\code

 #include <typeinfo>

  [...]

  // Recast to a different pointer type
  SomeOtherObject* other_pt=dynamic_cast<SomeOtherObject*>(some_pt);

 #ifdef PARANOID
  if (other_pt==0)
   {
   std::ostringstream error_stream;
     error_stream << "Failed to cast some_pt to SomeOtherObject* "  
                  << std::endl;
     error_stream << "The pointer some_pt points to an object of type: " <<
                  << typeid(some_pt).name() << std::endl; 
     throw OomphLibError(error_stream.str(),
                         OOMPH_CURRENT_FUNCTION,
                         OOMPH_EXCEPTION_LOCATION);
    }
 #endif 

[...]

\endcode 
Note that we throw a specific \c OomphLibError object after catching 
the error -- this allows the provision of more explicit (and hopefully more
meaningful) error messages.

   Many access functions that provide indexed access to
a private container, do, in fact, access a private STL vector.
Explicit range checking for these (frequent!) cases can be avoided
by changing the container to \c oomph-lib's \c Vector class. The 
\c Vector class
performs automatic range checking, if \c oomph-lib's \c generic library is compiled
with the \c RANGE_CHECKING flag set (i.e. if
\c -DRANGE_CHECKING is specified as a compilation flag for the
C++ compiler). For access functions that do not use the \c Vector
class you should implement your own range checks using the
RANGE_CHECKING compiler flag.

<HR>

\subsection magic The use of "magic numbers"

 In general, we set the equation number to -1 to indicate 
that a nodal value is 
pinned, i.e. prescribed by boundary conditions. This convention is used
throughout \c oomph-lib, but the direct use of -1 as a (bad!) "magic number"
is generally avoided. Instead we refer to the static data member of 
the \c Data class
\code static long Data::Is_pinned \endcode
which is (of course) set to -1. Similarly, when nodes are created,
the equation numbers of their value(s) are initialised to a value of -10,
represented by the static data member 
\code static int Data::Is_unclassified \endcode
This allows the function \c Problem::self_test() to check if
any values have not been classified as pinned or free.


<HR>

\subsection multi_physics Facilitating the use of specific elements in multi-physics problems

As the name of the library suggests, \c oomph-lib's existing
"single-physics" elements are (and any newly designed ones should be) 
designed so that they can easily be used in multi-physics problems. 
We anticipate two types of multi-physics interactions:
- The solution of one system of equations affects the geometry of 
  the domain in which another equation is solved -- this 
  occurs, for instance, in fluid-structure interaction problems.
- Two physical phenomena occur in the same domain and
  interact with each other, so that forcing terms in one
  equation depend on the solution of another -- this occurs, for
  instance, in thermo-elasticity problems where the temperature
  distribution (determined by the solution of an unsteady heat
  equation, say) affects the "growth term" in the equations of
  elasticity. 
.

 Multi-physics elements for such problems are usually implemented by combining
two (or more) single-physics elements via multiple inheritance. The combined, 
multi-physics element will obviously have a larger number of degrees
of freedom than the constituent single-physics elements. The combined element
typically calls the \c get_residuals(...) function of the two 
constituent elements and concatenates their residual vectors. 
When computing the Jacobian matrix, the "single physics" elements 
provide the diagonal blocks for the Jacobian matrix of the
multi-physics element, while the off-diagonal interaction blocks must 
be computed separately. The details of the implementation vary
from problem to problem. However, any single-physics element 
must satisfy the following requirements if it is to be used
as a base class for derived multi-physics elements: 
- \ref dont_wipe 
- \ref virtual 
- \ref index_fcts 
- \ref mesh_movement 
.


<HR>

\subsubsection dont_wipe Rule 1: Elements must only initialise their own entries in the element residual vector and the element Jacobian matrix

The computation of the element residual vector and
the element Jacobian matrix typically involves a loop over the
element's integration points, in which each integration point adds
a further contribution to the entries. In multi-physics problems
it is crucial that the initialisation of the residual vector
and the Jacobian matrix only sets  the element's "own" entries to zero. 


To illustrate this point, consider what would happen if we used 
the \c TwoNodePoissonElement::get_jacobian(...) function,
discussed in the section \ref jac, in a derived multi-physics element,
which combines the  \c TwoNodePoissonElement with another
element,  \c TwoNodeSomeOtherEquationElement, say. 
Assume that we implement the function \c get_jacobian(...) of the 
combined element so that it first calls the function \c
TwoNodeSomeOtherEquationElement::get_jacobian(...) to determine
the first diagonal block in the combined Jacobian matrix.
When we call  \c TwoNodePoissonElement::get_jacobian(...)
to compute the entries in the second diagonal block,
the initialisation loop

\code
//Find the number of degrees of freedom (unpinned values) in the element
 unsigned n_dof = ndof();
//Initialise all entries of the Jacobian matrix to zero
for(unsigned i=0;i<n_dof;i++) 
 {
  for(unsigned j=0;j<n_dof;j++) {jacobian(i,j) = 0.0;}
 }
\endcode 

would initialise the \e entire Jacobian matrix, thus wiping out
the entries that were already computed by  
\c TwoNodeSomeOtherEquationElement::get_jacobian(...).

 The strategy used in \c oomph-lib to permit the easy combination of
 elements is to use the two protected member functions of the
 \c GeneralisedElement class:
\code
virtual void GeneralisedElement::fill_in_contribution_to_residuals(Vector<double>&residuals);
\endcode

and

\code
virtual void GeneralisedElement::fill_in_contribution_to_jacobian(Vector<double> &residuals,
                                                                  DenseMatrix<double> &jacobian); 
\endcode

 These functions DO NOT initialise the entries of the residuals vector
 or the Jacobian matrix. Instead, the functions merely add the 
 appropriate contributions to the vector and the matrix entries. The
 default version of
 the \c get_residuals() and \c get_jacobian() functions, defined in
 \c GeneralisedElement, are simple
 wrappers that initialise the residuals and Jacobian to zero and then
 call the appropriate \c fill_in_contribution... function.
\code
  virtual void GeneralisedElement::get_residuals(Vector<double> &residuals) 
  {
   //Zero the residuals vector
   residuals.initialise(0.0);

   //Add the elemental contribution to the residuals vector
   fill_in_contribution_to_residuals(residuals);
  }

  virtual void GeneralisedElement::get_jacobian(Vector<double> &residuals, 
                                                DenseMatrix<double> &jacobian) 
  {
   //Zero the residuals vector
   residuals.initialise(0.0);

   //Zero the Jacobian matrix
   jacobian.initialise(0.0);

   //Add the elemental contribution to the residuals vector and Jacobian
   fill_in_contribution_to_jacobian(residuals,jacobian);
  } 
\endcode

 The \c get_residuals function, for example, can thus be overloaded 
 in a multi-physics element, as follows:
\code 

 /// Multi-physics element, created by multiple inheritance
 class SomeMultiPhysicsElement : public virtual TwoNodePoissonElement,
                                 public virtual TwoNodeSomeOtherEquationElement
 {

  [...]

  /// Residual vector of the combined element is made from the entries
  /// of the constituent single-physics elements
  virtual void get_residuals(Vector<double> &residuals) 
   {
    //Zero the residuals vector
    residuals.initialise(0.0);

   //Add the first elemental contribution to the residuals vector
   TwoNodePoissonElement::fill_in_contribution_to_residuals(residuals);

    //Add the second elemental contribution to the residuals vector
    TwoNodeSomeOtherEquationElement::fill_in_contribution_to_residuals(residuals);
   }


   [...]

  };
 
\endcode

 It is, therefore, recommended that authors of "single-physics"
 elements, overload \c fill_in_contribution_to_residuals(...) and 
 \c fill_in_contribution_to_jacobian(...), rather than \c
 get_residuals() and \c get_jacobian(...), respectively. A further
 advantage of the implementation is that the author need not
 worry about initialisation of the residuals vector or the Jacobian 
 matrix when using the \c "fill_in_" rather than the \c "get_" functions.

\subsubsection virtual Rule 2: Forcing functions, etc. should be implemented as  uniquely-named virtual functions

To allow for an interaction between multiple equations, any
forcing functions (such as the source function in the Poisson
equation) should be implemented as virtual functions. This allows
them to be overloaded in derived, multi-physics elements where the
forcing function in one equation might depend on the unknowns in
another one.

Furthermore, to avoid clashes of function names that may occur when two
single-physics elements are combined, member functions that can 
be expected to have counterparts in the context of other equations should 
be given suitable modifiers. For instance, 
\code
AdvectionDiffusionEquation::source_fct_adv_diff(...)
\endcode
is a better name for a member function that returns the source function
in the advection-diffusion equations than
\code
AdvectionDiffusionEquation::source_fct(...)
\endcode
since many other equations are likely to have source functions, too. 
It is obviously impossible to completely avoid such clashes but
this strategy makes them less likely to occur. We are fairly confident
that the relevant member functions of all existing elements 
in \c oomph-lib have been given suitable modifiers  to avoid
clashes of this type. Therefore, you should be able to combine any
element with any other element in the library. If you find a counter-example 
 <A HREF="../../contact/html/index.html">let us know</A>, and
we will rectify this in the next release. In the meantime
exploit the fact that \c oomph-lib is an open source library; you can
change anything you want!

\subsubsection index_fcts Rule 3: Nodal values should only be accessed indirectly via index functions that can be overloaded in derived multi-physics elements

When implementing the single-physics \c TwoNodePoissonElement,
discussed above, we provided an access function \c u(n) 
that returns the (single) nodal value, stored at the element's
\c n -th \c Node. The function was implemented as a simple wrapper 
that followed the pointer to the element's \c n -th \c Node and 
returned the zero-th nodal value stored at that \c Node:
\code
double TwoNodePoissonElement::u(const unsigned &n) 
 {
  return node_pt(n)->value(0);
 }
\endcode

In a single-physics context, this implementation is perfectly
acceptable since we know \e a \e priori that in a scalar problem
each \c Node only stores a single value. 

The same logic suggests that a \c TwoNodeAdvectionDiffusionElement
would have a member function 
\code
double TwoNodeAdvectionDiffusionElement::u(const unsigned &n) 
 {
  return node_pt(n)->value(0);
 }
\endcode

However, merging these two elements via multiple inheritance
creates two problems. First we have clash of names which could
have been avoided by following rule 2 above and calling the two functions
\c TwoNodePoissonElement::u_poisson(...) and 
\c TwoNodeAdvectionDiffusionElement::u_adv_diff(...), say.
More serious is that both elements regard the zero-th nodal value
as "their own". This can (and should!) be avoided
by the following re-implementation:

\code
double TwoNodePoissonElement::u(const unsigned &n) 
 {
  return node_pt(n)->value(u_index_poisson());
 }
\endcode

where the virtual function \c u_index_poisson() provides the 
default index at which the Poisson nodal values are stored in a 
single-physics context:

\code
virtual unsigned TwoNodePoissonElement::u_index_poisson()
 {
  // By default (i.e. in a single-physics context) the Poisson 
  // value is stored at the zero-th nodal value:
  return 0;
 }
\endcode

The advection-diffusion element can be modified in the same way:

\code
double TwoNodeAdvectionDiffusionElement::u(const unsigned &n) 
 {
  return node_pt(n)->value(u_index_adv_diff());
 }
\endcode

with

\code
virtual unsigned TwoNodeAdvectionDiffusionElement::u_index_adv_diff()
 {
  // By default (i.e. in a single-physics context) the advection-diffusion
  // value is stored at the zero-th nodal value:
  return 0;
 }
\endcode

In a combined multi-physics problem, we can now merge the two elements by
multiple inheritance,

\code
class TwoNodeAdvDiffAndPoissonElement : 
       public virtual TwoNodePoissonElement,
       public virtual TwoNodeAdvectionDiffusionElement
\endcode

Name clashes are already avoided, so we only have to  overwrite the two index 
functions to indicate which (scalar) value is stored as which 
nodal value:

\code
unsigned TwoNodeAdvDiffAndPoissonElement::u_index_adv_diff()
 {
  // In the combined multi-physics element we continue to store the 
  // advection-diffusion value at the zero-th nodal value
  // [as a result it is not actually necessary to re-implement this
  // function!]
  return 0;
 }
\endcode

and 

\code
unsigned TwoNodeAdvDiffAndPoissonElement::u_index_poisson()
 {
  // In the combined multi-physics element we store the Poisson
  // value at the first nodal value:
  return 1;
 }
\endcode

Specific examples that illustrate the creation of (non-trivial) 
multi-physics elements by multiple inheritance are provided
in the <A HREF="../../example_code_list/html/index.html#multi">
list of example codes.</A>



\subsubsection mesh_movement Rule 4: Time derivatives should be implemented in the ALE formulation to allow for moving meshes. 

In moving mesh problems, the time-derivative of a nodal value 
(approximated by the Node's \c TimeStepper) is not the same as
the Eulerian time-derivative \f$ \partial/ \partial t \f$
that "usually" occurs in the governing equation.
The former represents the rate of change when following the 
moving node; the latter is the rate of change evaluated at a fixed 
Eulerian position.  The two time-derivatives are related to each other
via the ALE relation
\f[
\left. \frac{\partial (.)}{\partial t} \right|_{Eulerian} 
= \left. \frac{\partial (.) }{\partial t} \right|_{at \ node} - \ 
\sum_{i=1}^D v_i^{(Mesh)} \  \frac{\partial (.) }{ \partial x_i} 
\f]
where \f$ D \f$ is the spatial dimension of the problem and
\f$ v_i^{(Mesh)}  \ (i=1,...,D) \f$ is the velocity of the node. 
Any Eulerian time-derivatives in the governing equation should
be implemented in the above form to ensure that the
element remains functional in moving mesh problems.
See the <A HREF="../../unsteady_heat/two_d_unsteady_heat_ALE/html/index.html">
example describing the solution of the unsteady heat equation in a
moving domain</A> for further details.






<hr>
<hr>
\section pdf PDF file
A <a href="../latex/refman.pdf">pdf version</a> of this document is available.
**/

