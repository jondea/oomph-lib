/**

\mainpage A two-dimensional Biharmonic problem with the C1-curved triangular finite element

In <A HREF="../../../c1_element/bell_element/html/index.html">
the previous tutorial</A>, the triangular Bell element has been successfully used to solve the Biharmonic equation which is a fourth-order problem in two dimensions. The domains of interest considered in the particular problem contained only straight boundaries so that this representation was exact. However, in many engineering applications, the geometric boundary of a problem is not straight. Solving such a problem with the straight-sided \f$ C^1 \f$-finite elements limits the convergence rate and accuracy as presented in <A HREF="http://onlinelibrary.wiley.com/doi/10.1002/pamm.201010272/pdf"> a publication of 
P.Fischer(2010)</A> .

In order to deal with a fourth-order problem, which requires \f$ C^1 \f$-continuity, on a curvilinear domain, the \f$ C^1 \f$-curved triangular element is introduced in this document. Numerical implementations of the Biharmonic equation on a circular domain will be presented. This is to compare the results obtained between the Bell and the \f$ C^1 \f$-curved triangular elements. Both rate of convergence and the obtained accuracy will be determined together with the computational time. This is to show that representing a curved boundary by a series of straight-sided elements exhibits a limitation in convergence rate and accuracy. Also, we would like to illustrate that the \f$ C^1 \f$-curved triangular element can retain the rate of convergence and accuracy when a curvilinear boundary is concerned.

The reader is referred to <A HREF="../../../c1_element/bell_element/html/index.html">
a two-dimensional Biharmonic problem with the Bell triangular finite element tutorial</A> for more detailed descriptions of the variational principle of the Biharmonic equation and the Bell element. 

In addition, in this document we demonstrate

- general descriptions of the \c C1CurvedElement which is our defined \f$ C^1 \f$-triangular finite element dealing with a 2D curvilinear domain
.
- numerical results of solving the Biharmonic equation with the \c C1CurvedElement
.
and
- how to implement the \c C1CurvedElement in \c oomph-lib using the Biharmonic equation as a case study.
.

<HR>   
<HR>

\section overview Overview of the C1CurvedElement 

The constructed \c C1CurvedElement provides a discretisation of a problem with two-dimensional, subparametric triangular finite elements. With this element, the discretisation of the domain of interest in this document will be the union of two sets of triangles. The first set will constitute of straight-sided triangles which is the triangulation of interior elements. The other will constitute of curve-sided triangles which are elements at a curved boundary. Note that each of two distinct triangles of the triangulations is either disjoint, or have a common vertex or a common edge. Consequently, we have to define mappings associated the reference and the physical triangles for each straight and curved triangles.

Regarding the approximation of variables on the straight boundary domain, the Bell shape functions will be employed. On the curved boundary domain, the shape functions will be re-constructed compatible with the Bell shape functions. These new functions will be defined over the approximated domain associated with the mappings approximating the curved boundary. Finally, all of these shape functions will be used to define interpolations of variables on the typical elements.

Specifically, we shall provide 
- The mappings associated the reference triangle and the physical triangles
- The shape functions defined on the \c C1CurvedElement 
- The interface of accessible functions to the shape functions
.

\subsection mapping The mappings associated the reference triangle with the physical triangles

In order to approximate the geometry in the \c C1CurvedElement, the subparametric mapping which associates the reference and the physical triangles is considered in this study.  Also, we will define mapping by using polynomials to approximate boundaries. 

Since there are two sets of triangular elements in the mesh, different mappings have to be considered for typical elements. Dealing with a straight-sided triangle in a physical domain, an affine mapping will be taken into account. Nonetheless, a nonlinear mapping has to be considered in order to deal with a curved-edge triangle.

Regarding a straight-sided triangle, we define the affine mapping
\f$F_{K_{I}}\f$ which is parametrised by the reference coordinates \f$
\hat{x}_{\alpha}, \alpha = 1,2, \f$ defined on the reference triangle \f$ \hat{K} = \{(\hat{x}_1, \hat{x}_2) | 0 \leq \hat{x}_1, \hat{x}_2 \leq 1, \hat{x}_1 + \hat{x}_2 = 1 \} \f$ and can be constructed as follow

\f[
x_{\alpha} = F_{K_{I\alpha}}(\hat{x}_{1},\hat{x}_{2}) = x_{\alpha 3} +
(x_{\alpha 1} - x_{\alpha 3})\hat{x}_{1} + (x_{\alpha 2} - x_{\alpha
3})\hat{x}_{2}, \ \ \ \ \ \ (1)
\f]
where \f$ x_{\alpha i}\f$ denote the global coordinates \f$ \alpha =
1,2 \f$ of the vertices \f$\mathbf{a}_{i}, i = 1,2,3,\f$ of the physical triangle \f$ K_I \f$ as depicted in the following figure.  Also, \f$ F_{K_{I \alpha}} \f$ is a \f$ \alpha^{th} \f$ component of the mapping \f$ F_{K_{I}} \f$. This mapping will help us to associate a position \f$(\hat{x}_1,\hat{x}_2)\f$ on the reference triangle with a position \f$(x_{1},x_{2})\f$ on the physical triangle whose sides are straight.

\image html mapping.gif "A graphical description of the mappings associated between the reference coordinates and the physical coordinates. " 
\image latex mapping.eps "A graphical description of the mappings associated between the reference coordinates and the physical coordinates. " width=0.35\textwidth

Regarding the curved boundary, the mapping \f$ F_{K_{C}} \f$ is defined to associate between the reference and the physical curved triangles. These curved triangles are considered to have two straight sides and one curved side approximating an arc of the boundary. In this case, the mapping \f$ F_{K_{C}} \f$ is nonlinear and can be constructed as in the following descriptions.

Let \f$\Omega\f$ be a given bounded domain on the plane whose boundary is curve in two-dimensional space. We assume that the curved boundary \f$\Gamma\f$ can be subdivided into a finite number of arcs and each of them can be described as

\f[ x_{1} = \chi_{1}(s),  \ \ \  x_{2} = \chi_{2}(s), \ \ \ s_{m} \leq s \leq
s_{M}, \f]

where \f$ x_{\alpha}, \alpha=1,2, \f$ denote coordinates in two-dimensional space defined on the curved boundary \f$ \Gamma \f$ and approximated by the polynomials \f$\chi_{1}(s), \chi_{2}(s)\f$ which are continuous on the arc \f$ [s_{m},s_{M}]. \f$

Before we will consider the nonlinear mapping \f$ F_{K_{C}} \f$, we will first determine a mapping \f$ \varphi_{\alpha} \f$ which associates the side \f$ \bf{\hat{a}}_1\bf{\hat{a}}_2 \f$ on the reference triangle, \f$ \hat{K} \f$, with the curved boundary \f$\Gamma\f$. This mapping is defined to be parametrised by the reference coordinate, say \f$ \hat{x}_{2}, \f$ and satisfies

\f[ \varphi_{\alpha}(\hat{x}_2 = 0) = \chi_{\alpha}(s_{m}) = x_{\alpha 1} = \mathbf{a}_{1}, \f]
\f[ \varphi_{\alpha}(\hat{x}_2 = 1) = \chi_{\alpha}(s_{M}) = x_{\alpha 2} = \mathbf{a}_{2}, \ \ \ \ \ \ (2) \f]

where \f$ \varphi_{\alpha}(\hat{x_{2}}) \equiv \chi_{\alpha}(s_{m} + (s_{M}-s_{m})\hat{x}_{2}) \f$. Note that either the reference coordinate \f$ \hat{x}_{1} \f$ or \f$ \hat{x}_{2} \f$ can be used to parametrise the arc but re-calculation will be required to defined the mapping.

Here, the Hermite-type polynomial of degree 3 is our choice to approximate the curved boundary. The reason for this choice is that cubic is the minimum degree of polynomial to obtain the \f$C^1\f$-continuity ( see <A HREF="http://www.sciencedirect.com/science?_ob=MiamiImageURL&_cid=271868&_user=6991156&_pii=004578259390111A&_check=y&_origin=article&_zone=toolbar&_coverDate=31-Jan-1993&view=c&originContentFamily=serial&wchp=dGLzVlV-zSkzk&md5=d038a1aec18c25806e39c26187c2a200&pid=1-s2.0-004578259390111A-main.pdf"> M.Bernadou (1993)</A> ). Hence, the following four conditions have to be specified 

\f[	x_{\alpha 1} = \varphi_{\alpha}(0) = \chi_{\alpha}(s_{m}), \mbox{   } x_{\alpha 2} = \varphi_{\alpha}(1) = \chi_{\alpha}(s_{M}), \f]
\f[   \varphi^{\prime}_{\alpha}(0) = (s_{M}-s_{m})\chi^{\prime}_{\alpha}(s_{m}), 	 \mbox{   } \varphi^{\prime}_{\alpha}(1) = (s_{M}-s_{m})\chi^{\prime}_{\alpha}(s_{M}).  \ \ \ \ (3)\f]

These conditions are constraints in order to satisfy (2) for the coordinate values \f$ x_{\alpha} \f$ in order to obtain the continuity.

By considering the cubic polynomial, \f$ \varphi_{\alpha}(\hat{x}_2)  \f$, in the form of (4) with the conditions for derivatives in (3), the mapping  \f$ \varphi_{\alpha}(\hat{x}_2) \f$ can be obtained.

\f[ \varphi_{\alpha}(\hat{x}_2) = x_{\alpha 1} + (x_{\alpha 2} - x_{\alpha 1})\hat{x}_{2} + \hat{x}_{2}(1-\hat{x}_{2})[m_{\alpha}\hat{x}_{2}+c_{\alpha}]. \ \ \ \ (4) \f]


Regarding the nonlinear mapping \f$ F_{K_{C}}, \f$ it is defined to  approximate the coordinates \f$ x_{\alpha}, \alpha = 1,2, \f$ so that the side \f$ \bf{\hat{a}}_1\bf{\hat{a}}_2 \f$ on the reference triangle, \f$ \hat{K}, \f$ associates with the curved side of the curved physical triangle. Also, the mapping \f$ F_{K_{C}} \f$ has to be an affine mapping along the side \f$ \mathbf{a}_{3}\mathbf{a}_{\alpha}, \alpha = 1,2 \f$. Therefore, the nonlinear mapping \f$ F_{K_{C}} \f$ that associates the reference and the curved elements with the cubic polynomial approximating a curved boundary is obtained as follow

\f[ x_{\alpha} = 
F_{K_{C\alpha}}(\hat{x}_{1},\hat{x}_{2}) =  x_{\alpha 3} + ( x_{\alpha 1}
- x_{\alpha 3} )\hat{x}_{1} + (x_{\alpha 2} - x_{\alpha 3})\hat{x}_{2}  \f]
\f[ + \frac{1}{2}\hat{x}_{1}\hat{x}_{2}\left\{\left[2(x_{\alpha 2}-x_{\alpha 1})-(s_{M}-s_{m})(\chi^{\prime}_{\alpha}(s_{m}) +\chi^{\prime}_{\alpha}(s_{M}))\right](\hat{x}_{2}-\hat{x}_{1} )  + (s_{M}-s_{m})\left[\chi^{\prime}_{\alpha}(s_{m})
-\chi^{\prime}_{\alpha}(s_{M})\right]\right\},  \ \ \ (5)\f]
where \f$ F_{K_{C\alpha}} \f$ denotes an \f$\alpha^{th} \f$ component of the mapping \f$ F_{K_{C}} \f$. Note that the nonlinear mapping \f$ F_{K_{C\alpha}}(1-\hat{x}_{2},\hat{x}_{2}) = \varphi_{\alpha}(\hat{x}_{2}) \f$ when comes to the side  \f$ \mathbf{a}_{1}\mathbf{a}_{2} \f$, as decided. 


The derivation of the nonlinear mapping \f$ F_{K_{C}} \f$ can be found in <A HREF="http://www.sciencedirect.com/science?_ob=MiamiImageURL&_cid=271868&_user=6991156&_pii=004578259390111A&_check=y&_origin=article&_zone=toolbar&_coverDate=31-Jan-1993&view=c&originContentFamily=serial&wchp=dGLzVlV-zSkzk&md5=d038a1aec18c25806e39c26187c2a200&pid=1-s2.0-004578259390111A-main.pdf"> a publication of 
M.Bernadou (1993)</A> .

<HR>

\subsection c1_shape The shape functions defined on the C1CurvedElement 

In this section, we will elaborate the shape functions defined on the \c C1CurvedElement. Since there are two types of elements defined on the \c C1CurvedElement, two sets of shape functions will be considered.

On the straight-sided triangular element, we inherit the \c BellElement to approximate values defined on the element. Therefore, the Bell shape functions will be employed and their details can be found in <A HREF="../../../c1_element/bell_element/html/index.html">
the Bell triangular finite element tutorial</A>. Note that the Bell shape functions are defined with respect to the global coordinates so that the transformation of derivatives between the local and the global coordinates is no longer need in an implementation.

Next, we will define shape functions of the \f$ C^1 \f$-curved finite element in order to interpolate any function defined over the curve-sided triangular element. Unlike the interpolation functions defined on the Bell triangular element, the interpolation functions of the \f$ C^1 \f$-curved triangle are chosen to defined over the reference triangle \f$ \hat{K}. \f$ Therefore, for any polynomial \f$p \in P_{K}\f$ defined over the curved triangle \f$ K_{C}, \f$ we employ the mapping defined in (5) to associate this polynomial \f$ p \f$ with the polynomial defined on the reference triangle as
\f[ \hat{p} = p \circ F_{K_C}. \f] 

Note that defining a polynomial on the reference triangle, \f$\hat{K}\f$, is a desirable condition which is convenient for the study of the approximation error and to take into account the numerical integration.

Subsequently, using a polynomial of degree 3 to approximate a curved boundary in our study, polynomials of degree 7 have to be defined as shape functions over the reference triangle in order to interpolate unknowns on a curved triangle. This is in order to achieve the required convergence and to satisfy the \f$ C^1 \f$-compatibility with the Bell finite elements. The reader may refer to <A HREF="http://www.sciencedirect.com/science?_ob=MiamiImageURL&_cid=271868&_user=6991156&_pii=004578259390111A&_check=y&_origin=article&_zone=toolbar&_coverDate=31-Jan-1993&view=c&originContentFamily=serial&wchp=dGLzVlV-zSkzk&md5=d038a1aec18c25806e39c26187c2a200&pid=1-s2.0-004578259390111A-main.pdf"> a publication of M.Bernadou (1993)</A> for more details. 

The desired \f$P_{7}\f$-\f$C^1\f$ reference element will constitute of \f$\hat{K}\f$ which is a unit right-angled triangle, the set of degrees of freedom \f$ \hat{\Sigma}(\hat{w}),\f$  and \f$\hat{P}\f$ which is a space of complete polynomials of degree 7 with its dimension equals to 36. The set of \f$ \hat{\Sigma}(\hat{w}) \f$ composes of values and their derivatives defined on vertices, \f$ \mathbf{\hat{a}}_i, i=1,2,3, \f$ and along edges, \f$ \mathbf{\hat{b}}_i, i=1,2,3 \f$, \f$ \mathbf{\hat{d}}_i, i = 1,...,6, \f$ and the internal nodes, \f$ \mathbf{\hat{e}}_i, i=1,2,3, \f$ of the reference triangle illustrated in the following figure and are defined as

\f[ \hat{\Sigma}(\hat{w}) = \left\{\hat{w}(\mathbf{\hat{a}}_{i}),
\frac{\partial \hat{w}}{\partial \hat{x}_{1}}(\mathbf{\hat{a}}_{i}),
\frac{\partial \hat{w}}{\partial \hat{x}_{2}}(\mathbf{\hat{a}}_{i}),
\frac{\partial^2 \hat{w}}{\partial \hat{x}^2_{1}}(\mathbf{\hat{a}_{i}}),
\frac{\partial^2 \hat{w}}{\partial
\hat{x}_{1}\hat{x}_{2}}(\mathbf{\hat{a}}_{i}), \frac{\partial^2
\hat{w}}{\partial \hat{x}^2_{2}}(\mathbf{\hat{a}}_{i}), i=1,2,3 \right\} \f] 
\f[ \cup \left\{ -\frac{\partial \hat{w}}{\partial \hat{x}_{1}}(\mathbf{\hat{b}}_{1}); -\frac{\partial \hat{w}}{\partial \hat{x}_{2}}(\mathbf{\hat{b}}_{2}); \frac{\sqrt{2}}{2}\left(\frac{\partial \hat{w}}{\partial \hat{x}_{1}}+\frac{\partial \hat{w}}{\partial \hat{x}_{2}}\right)(\mathbf{\hat{b}}_{3}) \right\} \cup \f]
\f[ 
\left\{ \hat{w}(\mathbf{\hat{d}}_{i}),i=1,...,6; 
 -\frac{\partial \hat{w}}{\partial \hat{x}_{1}}(\mathbf{\hat{d}}_{i}), i=1,2; -\frac{\partial \hat{w}}{\partial \hat{x}_{2}}(\mathbf{\hat{d}}_{i}), i=3,4; 
 \frac{\sqrt{2}}{2}\left(\frac{\partial \hat{w}}{\partial \hat{x}_{1}}
 + \frac{\partial \hat{w}}{\partial \hat{x}_{2}}\right)(\mathbf{\hat{d}}_{i}),
i=5,6 \right\} \f]
\f[ \cup \left\{
\hat{w}(\mathbf{\hat{e}}_{i}),i=1,2,3 \right\}, \f]

where \f$\hat{w}\f$ denotes a function defined over the reference element \f$\hat{K}.\f$ 

\image html Triple_of_FE.gif "The reference element of the C1-curved finite element compatible with the Bell triangle where the degree of polynomial approximating curved boundaries is cubic. " 
\image latex Triple_of_FE.eps "The reference element of the C1-curved finite element compatible with the Bell triangle where the degree of polynomial approximating curved boundaries is cubic. " width=0.35\textwidth

Since the \f$ P_7-C^1 \f$-shape functions of the \f$ C^1 \f$-curved finite element are defined on the reference triangle \f$ \hat{K} \f$, its 36 degrees of freedom have to be associated with those defined on the curved physical triangle. To consider the degrees of freedom defined on the curved physical triangle, they have to ensure the connection of class \f$ C^1 \f$-compatible with the Bell triangles as the Bell triangles are its adjacent elements.

To define such a connection, the following conditions have to be satisfied to assure a compatibility between a curved finite element and the Bell triangle.

- One-variable polynomials of any function \f$ p \f$ defined over the curved triangle and their normal derivatives \f$ \frac{\partial p}{\partial n} \f$ along the connected sides have to coincide with those of the Bell elements. That is the degrees of the polynomial and its normal derivatives along the connecting sides have to be of degree 5 and 3, respectively.

- the degrees of freedom of the curved finite element relative to the connecting sides have to be identical to those of the Bell elements and have to be entirely determined on those sides. 

Therefore, we have that our \f$ C^1\f$-curved triangular finite element has to define so that the set of degrees of freedom on the vertices of the curved physical triangle is the same as those of the Bell element. 

Furthermore, there will be three additional degrees of freedom defined inside the curved triangle \f$ K_C\f$ as seen in Fig. 1.3. These nodes come from three internal nodes defined in order to ensure the \f$C^1\f$-continuity in determining the polynomial of degree 7 on the reference element. Consequently, there are 21 degrees of freedom in total defined over the curved triangular element. 

The set \f$\Sigma_{K}(v)\f$ of values of degrees of freedom of \f$v\f$ defined on the curved physical triangle is then given by 

\f[ \Sigma_{K}(v) = \left\{ (D^{\alpha}v(\mathbf{a}_{i}), \alpha = 0,1,2), i=1,2,3; v(\mathbf{e}_{i}), i = 1,2,3 \right\}. \ \ \ \ \ \ (6)\f]

\image html Set_of_dof_curved_element.gif "The set of degrees of freedom for the C1-curved finite element compatible with the Bell element. " 
\image latex Set_of_dof_curved_element.eps "The set of degrees of freedom for the C1-curved finite element compatible with the Bell element. " width=0.35\textwidth

In order to interpolate a function \f$ v \f$ defined on the curved physical triangle, the \f$P_7-C^1\f$-shape functions and the 36 associated values of degrees of freedom have to be taken into account. Since the 36 nodal degrees of freedom depend on 21 nodal degrees of freedom defined on \f$K_C\f$ illustrated in (6), the association between those values of degrees of freedom have to be defined.

Since the number of degrees of freedom defined on the curved and the reference triangles are different, the derivation of the associations is not straightforward. Detailed descriptions of the derivation of the association \f$ \tilde{M} \f$ can be found in <A HREF="http://www.sciencedirect.com/science?_ob=MiamiImageURL&_cid=271868&_user=6991156&_pii=004578259390111A&_check=y&_origin=article&_zone=toolbar&_coverDate=31-Jan-1993&view=c&originContentFamily=serial&wchp=dGLzVlV-zSkzk&md5=d038a1aec18c25806e39c26187c2a200&pid=1-s2.0-004578259390111A-main.pdf"> a publication of 
M.Bernadou (1993)</A>.

<HR>

\subsection intp The interpolation of a function, v, defined on the domain

The interpolation of a function \f$ v \f$ defined on the physical triangle can be determined associated with the straight and the curved element. 

If we want to interpolate a function \f$ v \f$ defined on the straight physical triangle, the Bell shape functions have to be taken into account. Therefore, the interpolation can be defined as

\f[
u(x_1,x_2) =
\sum_{l=1}^{3}\sum_{k=1}^{6} u_{lk}\psi^{B}_{lk}(x_{1},x_{2}), \ \ \ \ \ \ (7) \f]

where \f$ u_{lk} \f$ and \f$ \psi^{B}_{lk}, l=1,2,3, k=1,...,6, \f$ are the nodal values and the Bell shape functions defined on the physical element \f$ K_I \f$, respectively. 

Otherwise, the interpolation of a function \f$ v \f$ defined on the curved physical triangle can be determined by quantities defined on the reference triangle as 

\f[ u(x_1(\mathbf{\xi}),x_2(\mathbf{\xi})) =
\sum_{j=1}^{36}{\hat{u}_{j}\hat{\psi}_{j}}(\xi_{1},\xi_{2}), \ \ \ \ \ \ (8)\f]

where \f$ \hat{u}_{j} \f$ are the nodal values defined at node \f$j\f$ on the reference triangle \f$ \hat{K} \f$. Also, \f$ \hat{\psi}_{j}, j=1,...,36, \f$ are the \f$ C^1 \f$-shape functions defined by complete polynomials of degree 7 on the reference element \f$ \hat{K} \f$.

Since all these nodal values \f$ \hat{u}_{j} \f$ are defined to depend only on 21 nodal values, \f$ u_{k} \f$, defined on the curved element \f$ K_{C} \f$, the set of nodal values defined on the reference element can be associated with those defined on the curved element through the association \f$ \tilde{M} \f$ by
\f[ 
\hat{u}_{j} = \sum_{k=1}^{21}{u_{k}\tilde{M}_{kj}},
\forall j=1,...,36, \ \ \ \ \ \ (9)
\f]

Substituting (9) into (8), we have that the approximation of the unknown \f$ u\f$ can be expressed as the linear combination between the shape functions defined on the reference triangle \f$ \hat{K} \f$ and the values defined on the physical curved triangle as follow

\f[
u(x_1(\mathbf{\xi}),x_2(\mathbf{\xi})) = \sum_{j=1}^{36}{\sum_{k=1}^{21}{u_{k}\tilde{M}_{kj}}\hat{\psi}_{j}}(\xi_{1},\xi_{2}).
\f]

Note that the shape functions of the \f$ C^1 \f$-curved element can be determined from the \f$ \delta_{ij} \f$ property of the shape function. Also, they are defined with respect to the local coordinates. Hence, the transformation of derivatives between the local and the global coordinates is needed in an implementation in order to ensure \f$ C^1 \f$-continuity.

<HR>

\subsection interface The interface of accessible functions to the shape functions of the C1CurvedElement

Regarding the curvilinear boundary domain study in this tutorial, its triangulation constitutes of both straight-sided and curve-sided triangle. Different number of nodes and shape functions have to be considers in \c C1CurvedElement in order to associate with both the straight and curved triangular elements in the triangulation. 

Now, we will consider the face-element of the \c C1CurvedElement. According to section 1.1.2, we have that 21 degrees of freedom defined on the \f$ C^1 \f$-curved triangular element are associated with three vertices and three interior node of the curved physical triangle. Unlike the \f$ C^1 \f$-curved triangular elements, all 18 degrees of freedom of the Bell element are defined on only three vertices. 

Therefore, the face element of the \c C1CurvedElement have to define generally in order to associate with both straight-sided and curve-sided elements. Consequencely, there are six nodes in total in a \c C1CurvedElement with \c NNODE_1D equals to 2. The following figure illustrates the number of nodes defined in both the Bell and the \f$ C^1 \f$-curved triangular elements.

\image html Graphic_curved_element.gif "The graphical description of the face-element defined in the \c C1CurvedElement. " 
\image latex Graphic_curved_element.eps "The graphical description of the face-element defined in the \c C1CurvedElement. " width=0.35\textwidth

Apparently, our constructed \c C1CurvedElement are only available for three interior nodes for all cases of \c NNODE_1D. The fixed number of interior nodes is a consequence of using degree 3 of polynomial to approximate a curvilinear boundary as mentioned in subsection 1.1.1. 

In order to approximate the geometry, the linear mapping defined in (1) is employed to associate with the straight-sided triangles while the nonlinear mapping defined in (5) is employed to associate with the curved triangles. Those mapping will be inherited from GeometricCurvedTElementShape<2> and can be accessible in \c C1CurvedElemet<> by

<CENTER>
<TABLE>
<TR>
<TD>
<CENTER>
 \c C1CurvedElement<>::shape(s,x),
</CENTER>
</TD>
</TR>
</TABLE>
</CENTER>

Note that, unlike other elements in \c oomph-lib, the geometric shape functions in \c C1CurvedElement rather computed the global position of the local coordinate \f$ s. \f$

Regarding shape functions defined on the \c C1CurvedElement, different definitions of shape functions are defined in order to associate with both straight and curved triangular elements. On the set of straight-sided triangles, the Bell shape functions will be employed to approximate the unknowns. When the curve-sided triangles are concerned, the \f$ C^1 \f$-curved triangular shape functions defined in subsection 1.1.2 will be used to approximate the unknowns. Hence, the \c C1CurvedElement is a \f$ C^1 \f$-triangular element carries two families of \f$ C^1 \f$-shape functions defined on both straight and curved triangles.

The shape functions of both Bell and \f$ C^1 \f$-curved triangular elements can be accessible via \c BellElementShape::Bshape(...) and \c C1CurvedElementShape::Cshape(...), respectively. These functions will compute the shape functions at local coordinate \f$ s. \f$

As the global coordinates are required in the derivation of the shape functions of the \c BellElementShape<>, the physical coordinates of vertices have to be passed as an argument when shape functions are overloaded. The interface of the Bell shape functions obtained from \c BellElementShape<> at local coordinate \f$ s \f$ is
\c BellElementShape<>::Bshape(s,psi,position). Similarly, the first and the second-order derivatives can be obtained at local coordinate \f$ s \f$ as
\c BellElementShape<>::dBshape(s,psi,dpsi,position) and \c BellElementShape<>::d2Bshape(s,psi,dpsi,d2psi,position), respectively. The vector \c position is the collection of coordinates on vertices of straight-edged triangles. 

Now, to obtained the basis functions that employed to approximate variables on the straight-sided triangles in \c C1CurvedElement, they can be accessible via 

<CENTER>
<TABLE>
<TR>
<TD>
<CENTER>
 \c C1CurvedElement<>::basis_straight(s,psi),
</CENTER>
</TD>
</TR>
</TABLE>
</CENTER>

Furthermore, the first- and second-order derivatives of the Bell shape functions in \c C1CurvedElement<> can be accessible via

<CENTER>
<TABLE>
<TR>
<TD>
<CENTER>
 \c C1CurvedElement<>::dbasis_straight(s,psi,dpsi),
</CENTER>
</TD>
</TR>
</TABLE>
</CENTER>

and

<CENTER>
<TABLE>
<TR>
<TD>
<CENTER>
 \c C1CurvedElement<>::d2basis_straight(s,psi,dpsi,d2psi),
</CENTER>
</TD>
</TR>
</TABLE>
</CENTER>
respectively.

Unlike the Bell shape functions, the \f$ C^1 \f$-curved shape functions has a usual interface as no global coordinate is required. They can be overloaded from \c C1CurvedElementShape<>::Cshape(s,psi) while their first- and second-order derivatives can be obtained from \c C1CurvedElementShape<>::dCshape(s,psi,dpsi) and \c C1CurvedElementShape<>::d2Cshape(s,psi,dpsi,d2psi), respectively.

Now, to obtained the basis functions that employed to approximate variables on the curved triangles in \c C1CurvedElement, they can be accessible via 

<CENTER>
<TABLE>
<TR>
<TD>
<CENTER>
 \c C1CurvedElement<>::basis_curve(s,psi),
</CENTER>
</TD>
</TR>
</TABLE>
</CENTER>

Furthermore, the first- and second-order derivatives
of the \f$ C^1 \f$-curved shape functions in \c C1CurvedElement can be accessible via 

<CENTER>
<TABLE>
<TR>
<TD>
<CENTER>
 \c C1CurvedElement<>::dbasis_curve(s,psi,dpsi),
</CENTER>
</TD>
</TR>
</TABLE>
</CENTER>

and

<CENTER>
<TABLE>
<TR>
<TD>
<CENTER>
 \c C1CurvedElement<>::d2basis_curve(s,psi,dpsi,d2psi),
</CENTER>
</TD>
</TR>
</TABLE>
</CENTER>
respectively.

Regarding the association, which associated the values of degrees of freedom between the reference and physical curved triangles, mentioned in (9), it can be inherited from the functions \c C1CurvedElementShape<2>::set_of_value(D,B,node_position,bd_element,bd_node_position,x) and the following are required as its arguments 

- coordinates of three vertices of a curved triangle passed as \c node_position,
- coordinates of the vertices of a curved triangle that situate on a curved boundary passed as \c bd_node_position,
- coordinates of the vertices of a curved triangle that is off-boundary passed as \c x.

Note that the matrices \f$ D, B \f$ denote the submatrices of value transformation described in <A HREF="http://www.sciencedirect.com/science?_ob=MiamiImageURL&_cid=271868&_user=6991156&_pii=004578259390111A&_check=y&_origin=article&_zone=toolbar&_coverDate=31-Jan-1993&view=c&originContentFamily=serial&wchp=dGLzVlV-zSkzk&md5=d038a1aec18c25806e39c26187c2a200&pid=1-s2.0-004578259390111A-main.pdf"> a publication of 
M.Bernadou (1993)</A> .

<HR>
<HR>

\section reslt Solving the Biharmonic equation with the C1CurvedElement

The sections below provide an example of solving the fourth-order
problem using \c C1CurvedElement. The fourth-order problem consider here is the same Biharmonic equation described in <A HREF="../../../c1_element/bell_element/html/index.html">
another tutorial</A> which is expressed as follow:

\f[ \frac{\partial^4 u}{\partial x^4} + 2\frac{\partial^4 u}{\partial
x^2\partial y^2} + \frac{\partial^4 u}{\partial y^4} = 0, \ \ \ (10) \f] 

with the exact solution \f$ u(x,y) = \cos(x)e^y.\f$ However, in this document, the problem is implemented in the quarter of the unit circular domain \f$ \Omega = \{(x,y) |
x,y \in [0,1] , x^2+y^2 = 1\} \f$ which is a curvilinear boundary domain rather than a straight-sided boundary domain. Similarly, we also apply Dirichlet boundary conditions such that the exact solution is satisfied. Also, the conditions that have to be specified on the boundaries have to correspond with the degrees of freedom defined for the \c C1CurvedElement in the previous section. 

Since 36 values of degrees of freedom defined on the reference triangle depend on the 21 values of degrees of freedom defined on the physical curved triangle, those 21 degrees of freedom on the physical curved triangle have to be imposed. These degrees of freedom constitute of 6 degrees of freedom at 3 vertex nodes and the values of degrees of freedom at 3 internal nodes. The 6 degrees of freedom defined on vertices incorporate the value of unknown field, the first derivatives with respect to the first and second coordinates, and the second derivatives with respect to the first, second, and mix derivatives have to be determined at the boundaries. The boundary specifications can be determined the same as described in <A HREF="../../../c1_element/bell_element/html/index.html">
a two-dimensional Biharmonic problem with the Bell triangular finite element tutorial</A>. 

Here are two tables comparing the performance with the associated computational time between the \c BellElement and the \c C1CurvedElement. The accuracy of the solutions will base on the \f$ L^2 \f$-norm error which is mathematically described as 

\f[
|u_{exact}-u_{FE}| = \left( \int_{\Omega} | u_{exact} - u_{FE}|^2 d\Omega \right)^{1/2},
\f]
where \f$ u_{exact},u_{FE} \f$ denote the exact and the finite element solutions, respectively. 

Table 1: \f$L^2\f$-norm error of the solution obtained from the Biharmonic implementations using the BellElement with various numbers of elements.

<CENTER>
<TABLE>
<TR>
<TD>
Number of elements
</TD>
<TD>
Number of dofs
</TD>
<TD>
Error
</TD>
<TD>
Time (sec)
</TD>
</TR>
<TR>
<TD>
29
</TD>
<TD>
48
</TD>
<TD>
\f$ 5.67891 \times 10^{-4} \f$
</TD>
<TD>
0.23
</TD>
</TR>
<TR>
<TD>
84
</TD>
<TD>
198
</TD>
<TD>
\f$ 1.77260 \times 10^{-4} \f$
</TD>
<TD>
0.66
</TD>
</TR>
<TR>
<TD>
250
</TD>
<TD>
636
</TD>
<TD>
\f$ 9.42737 \times 10^{-5} \f$
</TD>
<TD>
2.02
</TD>
</TR>
<TR>
<TD>
2034
</TD>
<TD>
5838
</TD>
<TD>
\f$ 3.49306 \times 10^{-5} \f$
</TD>
<TD>
17.48
</TD>
</TR>
<TR>
<TD>
4833
</TD>
<TD>
14022
</TD>
<TD>
\f$ 5.12128 \times 10^{-6} \f$
</TD>
<TD>
43.94
</TD>
</TR>
</TD>
</TR>
</TABLE>  
</CENTER>

Table 2: \f$L^2\f$-norm error of the solution obtained from the Biharmonic implementations using the C1CurvedElement with various numbers of elements.

<CENTER>
<TABLE>
<TR>
<TD>
Number of elements
</TD>
<TD>
Number of dofs
</TD>
<TD>
Error
</TD>
<TD>
Time (sec)
</TD>
</TR>
<TR>
<TD>
29
</TD>
<TD>
63
</TD>
<TD>
\f$ 1.41802 \times 10^{-4} \f$
</TD>
<TD>
5.73
</TD>
</TR>
<TR>
<TD>
84
</TD>
<TD>
228
</TD>
<TD>
\f$ 3.70896 \times 10^{-5} \f$
</TD>
<TD>
16.07
</TD>
</TR>
<TR>
<TD>
250
</TD>
<TD>
696
</TD>
<TD>
\f$ 9.52325 \times 10^{-6} \f$
</TD>
<TD>
54.92
</TD>
</TR>
<TR>
<TD>
2034
</TD>
<TD>
5958
</TD>
<TD>
\f$ 1.0478 \times 10^{-8} \f$
</TD>
<TD>
436.17
</TD>
</TR>
<TR>
<TD>
4833
</TD>
<TD>
14262
</TD>
<TD>
\f$ 3.6392 \times 10^{-10} \f$
</TD>
<TD>
1749.63
</TD>
</TR>
</TD>
</TR>
</TABLE>  
</CENTER>

Tables 1 and 2 show that the computational time for the \c C1CurvedElement is obviously very expensive compared to that of the \c BellElement when the same number of element is concerned. However, for the same number of elements, accuracies obtained from the curved elements are superior to those obtained from the straight elements. 

Furthermore, if the same error is considered, say \f$O(10^{-5})\f$, it can be seen from Tables 1 and 2 that the number of the \c BellElement has to be 2034 to obtain such an accuracy. On the other hands, the number of the \c C1CurvedElement needed to obtain such accuracy is 84 which is a lot less. Also, the computational time to obtained the accuracy of order \f$O(10^{-5})\f$ for the \c BellElement is 17.48 and for the \c C1CurvedElement is 16.07 which is smaller.

In order to compare the convergence rate between the \c BellElement and the \c C1CurvedElement, Fig. 1.5 illustrates the comparison between the \f$L^2\f$-norm error and the element size obtained from those elements. It can be seen that the \c C1CurvedElement converges faster and, also, gives smaller error than the \c BellElement. 

When performing the log-plot between the \f$L^2\f$-norm error and the element size, Fig. 1.6 depicts that the convergence rate of the \c C1CurvedElement are greater than the \c BellElement as its slope is greater. The obtained rate of convergence for the \c C1CurvedElement and \c the BellElement is quintic and quadratic, respectively.

It is noteworthy that using the \c BellElement to solve the \f$ C^1 \f$-problem with a curved boundary domain decreases its rate of convergence from its potential when solving an exact representation domain (see <A HREF="../../../c1_element/bell_element/html/index.html">
another tutorial</A>). This limitation of convergence rate in the \c BellElement is due to the representation of the curved boundaries with straight-edge elements. 

\image html Rate_of_convergence_compare.gif "The comparison of the convergence rate between the Bell and the C1-curved triangular elements. " 
\image latex Rate_of_convergence_compare.eps "The comparison of the convergence rate between the Bell and the C1-curved triangular elements. " width=0.65\textwidth

\image html Rate_of_convergence_compare_logplot.gif "The comparison of the convergence rate between the Bell and the C1-curved triangular elements. " 
\image latex Rate_of_convergence_compare_logplot.eps "The comparison of the convergence rate between the Bell and the C1-curved triangular elements. " width=0.65\textwidth

<HR>
<HR>

\section impl Implementation in oomph-lib

In order to solve the Biharmonic equation with \c oomph-lib, the Global parameters and functions can be defined the same as in <A HREF="../../../c1_element/bell_element/html/index.html">
another Biharmonic tutorial</A>. However, we use the \c BiharmonicCurvedElement<2,2> to solve the Biharmonic equation when the curvilinear boundary is concerned. This can be modified in the driver code.

In sequels, we will discuss only functions that defined differently from solving the Biharmonic equation with the \c BellElement.

<HR>

\subsection problem The problem class

The problem class has five member functions, illustrated as follows:

- The problem constructor
- \c action_before_newton_solve() : Update the problem specifications before solve. Boundary conditions maybe set here.
- \c action_after_newton_solve() : Update the problem specifications after solve.
- \c doc_solution() : Pass the number of the case considered, so that output files can be distinguished.
- \c nodal_permutation() : Perform a nodal permutation in a curved element.
.

\dontinclude unstructured_2d_biharmonic_curvedelement.cc
\skipline start_of_problem_class
\until end of problem class

Only function \c Problem::nodal_permutation() is newly defined to associate with a curvilinear boundary. This function is employed to swap nodes so that they are correspond with the defined shape functions when using the \c C1CurvedElement.

<HR>

\subsection permute The nodal_permutation() function

Regarding the mapping associated between the reference coordinates and the physical coordinates defined on the curved triangle, it can be seen that we defined the side \f$ \hat{\bf{a}}_1\hat{\bf{a}}_2\f$ of the reference to associate with the curved boundary. Also, in the definition defined in (2), the vertex node \f$ \bf{a}_1 \f$ was defined to be correspond with the position where \f$ s \f$ is minimum on each arc of the curved boundary \f$ \Gamma \f$ and the node \f$ \bf{a}_2 \f$ was correspond with the position where the maximum \f$ s \f$  is obtained. Furthermore, the node \f$ \bf{a}_3 \f$ was defined to be the off-boundary node of a curved triangle. 

However, a discretisation of the curvilinear domain by triangles can give a triangulation where the node ordering in a triangular element incompatible with the described definitions. As illustrated in the following figure, three cases of node ordering can happen.

\image html node_ordering.gif "The graphical description of node ordering. " 
\image latex node_ordering.eps "The graphical description of node ordering. " width=0.45\textwidth

It can be seen from the definition in (2) and the figure that \f$ F_{K}(\bf{\hat{a}}_{i}) = \bf{p}_{i}\f$ rather than \f$ \bf{a}_i. \f$ Therefore, in order to derive our \f$ C^1\f$-curved triangular shape functions, a re-position of nodes has to be performed. The function \c nodal_permutation(...) will provide a nodal permutation where the nodal pointers will be swapped so that they are associated with the shape functions defined at that node. The nodal pointers will be permuted to correspond with the conditions required in (2).

Here are codes for a nodal permutation.

\dontinclude unstructured_2d_biharmonic_curvedelement.cc
\skipline start_of_nodal_permutation
\until end of permutation

<HR>

\subsection constructor The Problem constructor


The problem constructor starts by overloading the function \c
Problem::mesh_pt() and set to the specific mesh used in this
problem. In this tutorial, we implement the problem with a 2D
unstructured mesh which is externally created by \c Triangle.
The generated output will be used to build the \c oomph-lib mesh. The reader may refer to <A HREF="../../../meshes/mesh_from_inline_triangle/html/index.html"> another tutorial</A> to create an unstructured triangular mesh internally.

\dontinclude unstructured_2d_biharmonic_curvedelement.cc
\skipline start_of_constructor
\until TriangleMesh

Next, the boundary conditions of the problem will be taken care. Since
the Dirichlet boundary conditions are applied in this problem, we pin
the nodal values and their first-order derivatives on all
boundaries. In this study, we also set degrees of freedom associated
with the second-order derivatives to be pinned in order to reduce the number of degrees of freedom in the problem. 

Note that there are three boundaries on the quarter of the unit circular domain. The boundary identities are labelled anticlockwise where the first boundary is associated with the side \f$ (x,y) = (0,y). \f$

\dontinclude unstructured_2d_biharmonic_curvedelement.cc
\skipline start_of_boundary_conditions
\until end of boundary conditions

According to the \c C1CurvedElement, there are two different types of triangular elements which contain different numbers of nodes in an element. The straight-sided triangular elements constitute of three vertex nodes with out any interior node while the curved elements constitute of three vertices and three interior nodes. Since there are overall six nodes in \c C1CurvedElement, all three interior nodes have to be pinned when the straight-sided elements are concerned. 

\dontinclude unstructured_2d_biharmonic_curvedelement.cc
\skipline pinned the unnecessary nodes
\until end of pinning the unnecessary nodes

We then loop over the elements and set the pointers to the physical
parameters, the function pointer to the source function which is the
function on the right-hand side of the Biharmonic equation, and the pointer
to the exact solutions.

\dontinclude unstructured_2d_biharmonic_curvedelement.cc
\skipline Loop over elements
\until end of pointers set up

Before we can implement a problem in a curved element, a nodal permutation is needed in order to satisfy (2). That is the coordinates of the first and the second nodes have to be associated with the lower and upper position on a boundary, respectively. Furthermore, the third node should be off-boundary.

\dontinclude unstructured_2d_biharmonic_curvedelement.cc
\skipline Perform a nodal permutation
\until nodal_permutation()

We finish the constructor by assigning the equation numbering scheme.
\dontinclude unstructured_2d_biharmonic_curvedelement.cc
\skipline Setup equation numbering scheme
\until end of constructor


<HR>
<HR>


\section sources Source files for this tutorial
- The source files for this tutorial are located in the directory:\n\n
<CENTER>
<A HREF="../../../../demo_drivers/biharmonic/unstructured_2d_biharmonic/">
demo_drivers/biharmonic/unstructured_2d_biharmonic/
</A>
</CENTER>\n
- The driver code for the 2D Biharmonic problem with the \c
BellElement is: \n\n
<CENTER>
<A HREF="
../../../../
demo_drivers/biharmonic/unstructured_2d_biharmonic/unstructured_2d_biharmonic_bellelement.cc
">
demo_drivers/biharmonic/unstructured_2d_biharmonic/unstructured_2d_biharmonic_bellelement.cc
</A>
</CENTER>

- The driver code for the 2D Biharmonic problem with the \c
C1CurvedElement is: \n\n
<CENTER>
<A HREF="
../../../../
demo_drivers/biharmonic/unstructured_2d_biharmonic/unstructured_2d_biharmonic_curvedelement.cc
">
demo_drivers/biharmonic/unstructured_2d_biharmonic/unstructured_2d_biharmonic_curvedelement.cc
</A>
</CENTER>
.

<hr>
<hr>
\section pdf PDF file
A <a href="../latex/refman.pdf">pdf version</a> of this document is available.
**/

