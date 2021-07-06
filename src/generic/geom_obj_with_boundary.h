//LIC// ====================================================================
//LIC// This file forms part of oomph-lib, the object-oriented, 
//LIC// multi-physics finite-element library, available 
//LIC// at http://www.oomph-lib.org.
//LIC// 
//LIC//    Version 1.0; svn revision $LastChangedRevision: 1097 $
//LIC//
//LIC// $LastChangedDate: 2015-12-17 11:53:17 +0000 (Thu, 17 Dec 2015) $
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
#ifndef OOMPH_GEOM_OBJ_WITH_BOUNDARY_HEADER
#define OOMPH_GEOM_OBJ_WITH_BOUNDARY_HEADER


// Config header generated by autoconfig
#ifdef HAVE_CONFIG_H
  #include <oomph-lib-config.h>
#endif


//oomph-lib headers
#include "geom_objects.h"


namespace oomph
{


//===========================================================================
/// Base class for upgraded disk-like GeomObject (i.e. 2D surface in 3D space)
/// with specification of boundaries. The GeomObject's position(...)
/// function computes the 3D (Eulerian) position vector r as a function of the
/// 2D intrinsic (Lagrangian) coordinates, zeta, without reference to
/// any boundaries. This class specifies the boundaries by specifying
/// a mapping from a 1D intrinsic boundary coordinate, zeta_bound, 
/// to the 2D intrinsic (Lagrangian) coordinates, zeta.
///
/// The class is made functional by provision (in the derived class!) of:
/// - a pointer to a GeomObject<1,2> that parametrises the 2D intrinisic 
///   coordinates zeta along boundary b in terms of the 1D boundary 
///   coordinate, zeta_bound,
/// - the initial value of the 1D boundary coordinate zeta_bound on boundary b,
/// - the final value of the 1D boundary coordinate zeta_bound on boundary b
/// .
/// for each of these boundaries. All three containers for these
/// must be resized (consistently) and populated in the derived class.
/// Number of boundaries is inferred from their size. 
/// 
/// Class also provides broken virtual function to specify boundary triads,
/// and various output functions.
//===========================================================================
class DiskLikeGeomObjectWithBoundaries : public virtual GeomObject
{

public:

 /// Constructor
 DiskLikeGeomObjectWithBoundaries() : GeomObject(2,3)
  {}
 
 /// How many boundaries do we have?
 unsigned nboundary() const
  {
   return Boundary_parametrising_geom_object_pt.size();
  }
 
 /// \short Compute 3D vector of Eulerian coordinates at 1D boundary coordinate
 /// zeta_bound on boundary b:
 void position_on_boundary(const unsigned& b,
                           const double& zeta_bound,
                           Vector<double>& r) const
  {
   Vector<double> zeta(2);
   zeta_on_boundary(b,zeta_bound,zeta);
   position(zeta,r);
  }

 /// \short Compute 2D vector of intrinsic coordinates at 1D boundary coordinate
 /// zeta_bound on boundary b:
 void zeta_on_boundary(const unsigned& b,
                       const double& zeta_bound,
                       Vector<double>& zeta) const
  {
#ifdef PARANOID
   if (Boundary_parametrising_geom_object_pt[b]==0)
    {
     std::ostringstream error_message;
     error_message 
      << "You must create a <1,2> Geom Object that provides a\n"
      << "mapping from the 1D boundary coordinate to the 2D\n"
      << "intrinsic Lagrangian coordinate of disk-like object\n"
      << std::endl;
     throw OomphLibError(error_message.str(),
                         OOMPH_CURRENT_FUNCTION,
                         OOMPH_EXCEPTION_LOCATION);
    }
   if (Boundary_parametrising_geom_object_pt[b]->nlagrangian()!=1)
    {
     std::ostringstream error_message;
     error_message 
      << "Boundary_parametrising_geom_object_pt must point to\n"
      << "GeomObject with one Lagrangian coordinate. Yours has "
      << Boundary_parametrising_geom_object_pt[b]->nlagrangian() 
      << std::endl;
     throw OomphLibError(error_message.str(),
                         OOMPH_CURRENT_FUNCTION,
                         OOMPH_EXCEPTION_LOCATION);
    }
   if (Boundary_parametrising_geom_object_pt[b]->ndim()!=2)
    {
     std::ostringstream error_message;
     error_message 
      << "Boundary_parametrising_geom_object_pt must point to\n"
      << "GeomObject with two Eulerian coordinates. Yours has "
      << Boundary_parametrising_geom_object_pt[b]->ndim() 
      << std::endl;
     throw OomphLibError(error_message.str(),
                         OOMPH_CURRENT_FUNCTION,
                         OOMPH_EXCEPTION_LOCATION);
    }
#endif
   Vector<double> zeta_bound_vector(1,zeta_bound);
   Boundary_parametrising_geom_object_pt[b]->
    position(zeta_bound_vector,zeta);
  }
 
 /// \short Pointer to GeomObject<1,2> that parametrises intrinisc coordinates
 /// along boundary b
 GeomObject* boundary_parametrising_geom_object_pt(const unsigned& b) const
 {
  return Boundary_parametrising_geom_object_pt[b];
 }

 /// \short Initial value of 1D boundary coordinate
 /// zeta_bound on boundary b:
 double zeta_boundary_start(const unsigned& b) const
  {
   return Zeta_boundary_start[b];
  }

 /// \short Final value of 1D boundary coordinate
 /// zeta_bound on boundary b:
 double zeta_boundary_end(const unsigned& b) const
  {
   return Zeta_boundary_end[b];
  }
 


 /// \short Boundary triad on boundary b at boundary coordinate zeta_bound.
 /// Broken virtual.
 virtual void boundary_triad(const unsigned& b,
                             const double& zeta_bound,
                             Vector<double>& r,
                             Vector<double>& tangent,
                             Vector<double>& normal,
                             Vector<double>& binormal)
  {
   std::ostringstream error_message;
   error_message 
    << "Broken virtual function; please implement for your\n"
    << "derived version of this class!"
    << std::endl;
   throw OomphLibError(error_message.str(),
                       OOMPH_CURRENT_FUNCTION,
                       OOMPH_EXCEPTION_LOCATION);
  }

 /// \short Output boundaries at nplot plot points. Streams:
 /// - two_d_boundaries_file: zeta_0, zeta_1, zeta_bound
 /// - three_d_boundaries_file : x, y, z, zeta_0, zeta_1, zeta_bound
 void output_boundaries(const unsigned& nplot,
                        std::ofstream& two_d_boundaries_file,
                        std::ofstream& three_d_boundaries_file)
  {
   std::ofstream boundaries_tangent_file;
   std::ofstream boundaries_normal_file;
   std::ofstream boundaries_binormal_file;
   output_boundaries_and_triads(nplot,
                                two_d_boundaries_file,
                                three_d_boundaries_file,
                                boundaries_tangent_file,
                                boundaries_normal_file,
                                boundaries_binormal_file);
  }
    

 /// \short Output boundaries and triad at nplot plot points. Streams:
 /// - two_d_boundaries_file: zeta_0, zeta_1, zeta_bound
 /// - three_d_boundaries_file : x, y, z, zeta_0, zeta_1, zeta_bound
 /// - boundaries_tangent_file : x, y, z, t_x, t_y, t_z
 /// - boundaries_normal_file  : x, y, z, n_x, n_y, n_z
 /// - boundaries_binormal_file: x, y, z, N_x, N_y, N_z
 void output_boundaries_and_triads(const unsigned& nplot,
                                   std::ofstream& two_d_boundaries_file,
                                   std::ofstream& three_d_boundaries_file,
                                   std::ofstream& boundaries_tangent_file,
                                   std::ofstream& boundaries_normal_file,
                                   std::ofstream& boundaries_binormal_file)
  {
   Vector<double> r(3);
   Vector<double> zeta(2);
   double zeta_bound=0.0;
   Vector<double> tangent(3);
   Vector<double> normal(3);
   Vector<double> binormal(3);
   unsigned nb=nboundary();
   for (unsigned b=0;b<nb;b++)
    {
     two_d_boundaries_file << "ZONE" << std::endl;
     three_d_boundaries_file << "ZONE" << std::endl;
     boundaries_tangent_file << "ZONE" << std::endl;
     boundaries_normal_file << "ZONE" << std::endl;
     boundaries_binormal_file << "ZONE" << std::endl;

     double zeta_min=zeta_boundary_start(b);
     double zeta_max=zeta_boundary_end(b);
     unsigned n=100;
     for (unsigned i=0;i<n;i++)
      {
       zeta_bound=zeta_min+
        (zeta_max-zeta_min)*double(i)/double(n-1);
       position_on_boundary(b,zeta_bound,r);  
       zeta_on_boundary(b,zeta_bound,zeta);     
       boundary_triad(b,
                      zeta_bound,
                      r,
                      tangent,
                      normal,
                      binormal);

       two_d_boundaries_file << zeta[0] <<" " 
                             << zeta[1] <<" " 
                             << zeta_bound << " " 
                             << std::endl;

       three_d_boundaries_file << r[0] << " " 
                               << r[1] << " " 
                               << r[2] << " " 
                               << zeta[0] <<" " 
                               << zeta[1] <<" " 
                               << zeta_bound << " " 
                               << std::endl;

       boundaries_tangent_file << r[0] << " " 
                               << r[1] << " " 
                               << r[2] << " "        
                               << tangent[0] << " "
                               << tangent[1] << " "
                               << tangent[2] << " "
                               << std::endl;

       boundaries_normal_file << r[0] << " " 
                              << r[1] << " " 
                              << r[2] << " " 
                              << normal[0] << " "
                              << normal[1] << " "
                              << normal[2] << " "
                              << std::endl;

       boundaries_binormal_file << r[0] << " " 
                                << r[1] << " " 
                                << r[2] << " "  
                                << binormal[0] << " "
                                << binormal[1] << " "
                                << binormal[2] << " " 
                                << std::endl;
      }
    }
  }

 
 /// \short Specify intrinsic coordinates of a point within a specified
 /// region  -- region ID, r, should be positive.
 void add_region_coordinates(const unsigned &r,
                             Vector<double> &zeta_in_region)
 {
  // Verify if not using the default region number (zero)
  if (r == 0) 
   {
    std::ostringstream error_message;
    error_message << "Please use another region id different from zero.\n"
                  << "It is internally used as the default region number.\n";
    throw OomphLibError(error_message.str(),
                        OOMPH_CURRENT_FUNCTION,
                        OOMPH_EXCEPTION_LOCATION);
   }
  // Need two coordinates
  unsigned n=zeta_in_region.size();
  if (n!=2) 
   {
    std::ostringstream error_message;
    error_message << "Vector specifying zeta coordinates of point in\n"
                  << "region has be length 2; yours has length "
                  << n << std::endl;
    throw OomphLibError(error_message.str(),
                        OOMPH_CURRENT_FUNCTION,
                        OOMPH_EXCEPTION_LOCATION);
   }
  
  // First check if the region with the specified id does not already exist
  std::map<unsigned, Vector<double> >::iterator it;
  it = Zeta_in_region.find(r);
  
  // If it is already a region defined with that id throw an error
  if (it != Zeta_in_region.end())
   {
    std::ostringstream error_message;
    error_message << "The region id (" << r << ") that you are using for"
                  << "defining\n"
                  << "your region is already in use. Use another\n"
                  << "region id and verify that you are not re-using\n"
                  <<" previously defined regions ids.\n"<<std::endl;
    OomphLibWarning(error_message.str(),
                    OOMPH_CURRENT_FUNCTION,
                    OOMPH_EXCEPTION_LOCATION);
   }
  
  // If it does not exist then create the map
  Zeta_in_region[r] = zeta_in_region;
  
 }
 
 /// Return map that stores zeta coordinates of points that identify regions
 std::map<unsigned, Vector<double> > zeta_in_region() const
  {
   return Zeta_in_region;
  }

protected:

 /// \short Storage for initial value of 1D boundary coordinate
 /// on boundary b:
 Vector<double> Zeta_boundary_start;

 /// \short Storage for final value of 1D boundary coordinate
 /// on boundary b:
 Vector<double> Zeta_boundary_end;
 
 /// \short Pointer to GeomObject<1,2> that parametrises intrinisc coordinates
 /// along boundary b; essentially provides a wrapper to zeta_on_boundary(...)
 Vector<GeomObject*> Boundary_parametrising_geom_object_pt;

 /// Map to store zeta coordinates of points that identify regions
 std::map<unsigned, Vector<double> > Zeta_in_region;

};



/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////


//=========================================================================
/// \short Warped disk in 3d: zeta[0]=x; zeta[1]=y (so it doesn't have 
/// coordinate singularities), with specification of two boundaries (b=0,1)
/// that turn the whole thing into a circular disk. 
//=========================================================================
class WarpedCircularDisk : public DiskLikeGeomObjectWithBoundaries
{

public:

 /// Constructor. Pass amplitude and azimuthal wavenumber of
 /// warping as arguments. Can specify vertical offset as final, optional
 /// argument.
 WarpedCircularDisk(const double& epsilon, const unsigned& n,
                    const double& z_offset=0.0) :
  Epsilon(epsilon),  N(n), Z_offset(z_offset)
  {
   // How many boundaries do we have?
   unsigned nb=2;
   Boundary_parametrising_geom_object_pt.resize(nb);
   Zeta_boundary_start.resize(nb,0.0);
   Zeta_boundary_end.resize(nb,0.0);

   // GeomObject<1,2> representing the first boundary
   Boundary_parametrising_geom_object_pt[0]=new Ellipse(1.0,1.0);
   Zeta_boundary_start[0]=0.0;
   Zeta_boundary_end[0]=MathematicalConstants::Pi;

   // GeomObject<1,2> representing the second boundary
   Boundary_parametrising_geom_object_pt[1]=new Ellipse(1.0,1.0);
   Zeta_boundary_start[1]=MathematicalConstants::Pi;
   Zeta_boundary_end[1]=2.0*MathematicalConstants::Pi;

  }

  /// Empty default constructor.
  WarpedCircularDisk()
   {
    throw OomphLibError("Don't call default constructor!",
                        OOMPH_CURRENT_FUNCTION,
                        OOMPH_EXCEPTION_LOCATION);
   }


 /// Broken copy constructor
 WarpedCircularDisk(const WarpedCircularDisk& dummy) 
  { 
   BrokenCopy::broken_copy("WarpedCircularDisk");
  } 
 
 /// Broken assignment operator
 void operator=(const WarpedCircularDisk&) 
  {
   BrokenCopy::broken_assign("WarpedCircularDisk");
  }

 /// Destructor
 virtual ~WarpedCircularDisk()
  {
   unsigned n=nboundary();
   for (unsigned b=0;b<n;b++)
    {
     delete Boundary_parametrising_geom_object_pt[b];
     Boundary_parametrising_geom_object_pt[b]=0;
    }
  }

 /// Access fct to amplitude of disk warping
 double& epsilon()
  {
   return Epsilon;
  }

 /// \short Position Vector at Lagrangian coordinate zeta 
 void position(const Vector<double>& zeta, Vector<double>& r) const
  {
   // Position Vector
   r[0] = zeta[0];
   r[1] = zeta[1];
   double radius=sqrt(r[0]*r[0]+r[1]*r[1]);
   double phi=atan2(r[1],r[0]);
   r[2]=Z_offset+w(radius,phi);
  }


 /// \short Parametrised position on object: r(zeta). Evaluated at
 /// previous timestep. t=0: current time; t>0: previous
 /// timestep. Object is steady so calls time-independent version
 void position(const unsigned& t, const Vector<double>& zeta, 
               Vector<double>& r) const
  {
   position(zeta,r);
  }


 /// Boundary triad on boundary b at boundary coordinate zeta_bound
 void boundary_triad(const unsigned& b,
                     const double& zeta_bound,
                     Vector<double>& r,
                     Vector<double>& tangent,
                     Vector<double>& normal,
                     Vector<double>& binormal)
  {
   double phi=zeta_bound;

   // Position Vector
   r[0] = cos(phi);
   r[1] = sin(phi);
   r[2]=Z_offset+w(1.0,phi);
   
   Vector<double> dr_dr(3);
   dr_dr[0]=cos(phi);
   dr_dr[1]=sin(phi);
   dr_dr[2]=dwdr(1.0,phi);
   double inv_norm=1.0/sqrt(dr_dr[0]*dr_dr[0]+
                            dr_dr[1]*dr_dr[1]+
                            dr_dr[2]*dr_dr[2]);

   normal[0]=dr_dr[0]*inv_norm;
   normal[1]=dr_dr[1]*inv_norm;
   normal[2]=dr_dr[2]*inv_norm;

   Vector<double> dr_dphi(3);
   dr_dphi[0]=-sin(phi);
   dr_dphi[1]=cos(phi);
   dr_dphi[2]=dwdphi(1.0,phi);

   inv_norm=1.0/sqrt(dr_dphi[0]*dr_dphi[0]+
                     dr_dphi[1]*dr_dphi[1]+
                     dr_dphi[2]*dr_dphi[2]);
   
   tangent[0]=dr_dphi[0]*inv_norm;
   tangent[1]=dr_dphi[1]*inv_norm;
   tangent[2]=dr_dphi[2]*inv_norm;

   binormal[0]=tangent[1]*normal[2]-tangent[2]*normal[1];
   binormal[1]=tangent[2]*normal[0]-tangent[0]*normal[2];
   binormal[2]=tangent[0]*normal[1]-tangent[1]*normal[0];

  }
                    
private:

 /// Vertical deflection
 double w(const double& r, const double& phi) const
  {
   return Epsilon*cos(double(N)*phi)*pow(r,2);
  }

 /// Deriv of vertical deflection w.r.t. radius
 double dwdr(const double& r, const double& phi) const
  {
   return Epsilon*cos(double(N)*phi)*2.0*r;
  }

 /// Deriv of vertical deflection w.r.t. angle
 double dwdphi(const double& r, const double& phi) const
  {
   return -Epsilon*double(N)*sin(double(N)*phi)*pow(r,2);
  }

 /// Amplitude of non-axisymmetric deformation
 double Epsilon;

 /// Wavenumber of non-axisymmetric deformation
 unsigned N;

 /// Vertical offset
 double Z_offset;

};

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////



//=========================================================================
/// \short Warped disk in 3d: zeta[0]=x; zeta[1]=y (so it doesn't have 
/// coordinate singularities), with specification of two boundaries (b=0,1)
/// that turn the whole thing into a circular disk. In addition
/// has two internal boundaries (b=2,3), a distance h_annulus from 
/// the outer edge. Annual (outer) region is region 1.
//=========================================================================
class WarpedCircularDiskWithAnnularInternalBoundary : 
public virtual WarpedCircularDisk
{

public:

 /// Constructor. Pass amplitude and azimuthal wavenumber of
 /// warping as arguments. Can specify vertical offset as final, optional
 /// argument.
  WarpedCircularDiskWithAnnularInternalBoundary
   (const double& h_annulus,
    const double& epsilon, 
    const unsigned& n,
    const double& z_offset=0.0) :
 WarpedCircularDisk(epsilon,n,z_offset), H_annulus(h_annulus)
  {  
   // We have two more boundaries!
   Boundary_parametrising_geom_object_pt.resize(4);
   Zeta_boundary_start.resize(4);
   Zeta_boundary_end.resize(4);

   // Radius of the internal annular boundary
   double r_annulus=1.0-h_annulus;

   // GeomObject<1,2> representing the third boundary
   Boundary_parametrising_geom_object_pt[2]=new Ellipse(r_annulus,r_annulus);
   Zeta_boundary_start[2]=0.0;
   Zeta_boundary_end[2]=MathematicalConstants::Pi;

   // GeomObject<1,2> representing the fourth boundary
   Boundary_parametrising_geom_object_pt[3]=new Ellipse(r_annulus,r_annulus);
   Zeta_boundary_start[3]=MathematicalConstants::Pi;
   Zeta_boundary_end[3]=2.0*MathematicalConstants::Pi;

   // Region 1 is the annular region; identify it by a point in 
   // this region. 
   unsigned r=1;
   Vector<double> zeta_in_region(2);
   zeta_in_region[0]=0.0;
   zeta_in_region[0]=1.0-0.5*h_annulus;
   add_region_coordinates(r,zeta_in_region);
  }

 /// Broken copy constructor
 WarpedCircularDiskWithAnnularInternalBoundary
  (const WarpedCircularDiskWithAnnularInternalBoundary& dummy) 
  { 
   BrokenCopy::broken_copy("WarpedCircularDiskWithAnnularInternalBoundary");
  } 
 
 /// Broken assignment operator
 void operator=(const WarpedCircularDiskWithAnnularInternalBoundary&) 
  {
   BrokenCopy::broken_assign("WarpedCircularDiskWithAnnularInternalBoundary");
  }

 /// Destructor (empty; cleanup happens in base class)
 virtual ~WarpedCircularDiskWithAnnularInternalBoundary()
  {}


 /// \short Thickness of annular region (distance of internal boundary
 /// from outer edge of unit circle)
 double h_annulus() const
 {
  return H_annulus;
 }

  protected:

 /// \short Thickness of annular region (distance of internal boundary
 /// from outer edge of unit circle)
 double H_annulus;


};


}

#endif
