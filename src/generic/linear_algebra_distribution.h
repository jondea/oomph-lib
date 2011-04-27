//LIC// ====================================================================
//LIC// This file forms part of oomph-lib, the object-oriented, 
//LIC// multi-physics finite-element library, available 
//LIC// at http://www.oomph-lib.org.
//LIC// 
//LIC//           Version 0.90. August 3, 2009.
//LIC// 
//LIC// Copyright (C) 2006-2009 Matthias Heil and Andrew Hazel
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
#ifndef OOMPH_LINEAR_ALGEBRA_DISTRIBUTION_CLASS_HEADER
#define OOMPH_LINEAR_ALGEBRA_DISTRIBUTION_CLASS_HEADER

// Config header generated by autoconfig
#ifdef HAVE_CONFIG_H
  #include <oomph-lib-config.h>
#endif

// MPI headers
#ifdef OOMPH_HAS_MPI
#include "mpi.h"
#endif

#include <ostream>

// General headers
#include "Vector.h"
#include "communicator.h"
#include "oomph_utilities.h"



namespace oomph{

//=============================================================================
/// \short Describes the distribution of a distributable linear algebra type 
/// object. Typically this is a container (such as a DoubleVector) or an 
/// operator (e.g Preconditioner or LinearSolver). \n
/// This object is used in both serial and parallel implementations. In the 
/// serial context (no MPI) this just contains an integer indicating
/// the number of rows. \n
/// In parallel either each processor holds a subset of the set of global rows.
/// (each processor contains only a single continuous block of rows - 
/// parametised with variables denoting the first row and the number of local 
/// rows) or, all rows are be duplicated across all processors. \n
/// In parallel this object also contains an OomphCommunicator object which 
/// primarily contains the MPI_Comm communicator associated with this object.
//=============================================================================
class LinearAlgebraDistribution
{

 public :

  /// \short Default Constructor - creates a Distribution that has not been
  /// setup
 LinearAlgebraDistribution() : Comm_pt(0) {}
  
  /// \short Constructor. Takes the first_row, nrow_local (both for this
  /// processor) and nrow as arguments. If nrow is not provided 
  /// or equal to 0 then it will be computed automatically
  LinearAlgebraDistribution(const OomphCommunicator &comm,
                            const unsigned &first_row_, 
                            const unsigned &n_row_local,
                            const unsigned &n_row = 0)
  : Comm_pt(0)
  {
    this->build(&comm,first_row_,n_row_local,n_row);
  };
 
 /// \short Constructor. Takes the number of global rows and uniformly 
 /// distributes them over the processors if distributed = true (default),
 /// if distributed = false every row is duplicated on every processor
 LinearAlgebraDistribution(const OomphCommunicator& comm, 
                           const unsigned &n_row,
                           const bool &distributed_ = true)
  : Comm_pt(0)
  {
    this->build(&comm,n_row,distributed_);
  };
   
  /// \short Constructor. Takes the first_row, nrow_local (both for this
  /// processor) and nrow as arguments. If nrow is not provided 
  /// or equal to 0 then it will be computed automatically
  LinearAlgebraDistribution(const OomphCommunicator* const comm_pt,
                            const unsigned &first_row_, 
                            const unsigned &n_row_local,
                            const unsigned &n_row = 0)
  : Comm_pt(0)
  {
    this->build(comm_pt,first_row_,n_row_local,n_row);
  };
 
 /// \short Constructor. Takes the number of global rows and uniformly 
 /// distributes them over the processors if distributed = true (default),
 /// if distributed = false every row is duplicated on every processor
 LinearAlgebraDistribution(const OomphCommunicator* const comm_pt, 
                           const unsigned &n_row,
                           const bool &distributed_ = true)
  : Comm_pt(0)
  {
    this->build(comm_pt,n_row,distributed_);
  };

 /// \short Copy Constructor.
 LinearAlgebraDistribution(const LinearAlgebraDistribution& old_dist) 
  : Comm_pt(0)
  {
   this->build(old_dist);
  }
 
 /// \short pointer based copy constructor
 LinearAlgebraDistribution(const LinearAlgebraDistribution* old_dist_pt)
  : Comm_pt(0)
  {
   this->build(old_dist_pt);
  }

 /// \short Destructor 
 ~LinearAlgebraDistribution() 
  {
   delete Comm_pt;
  }
 
 /// Assignment Operator
 void operator=(const LinearAlgebraDistribution& old_dist)  
  {    
   this->build(old_dist);                                
  }
 
 /// \short Sets the distribution. Takes first_row, nrow_local and 
 /// nrow as arguments. If nrow is not provided or equal to
 /// 0 then it is computed automatically
 void build(const OomphCommunicator& comm,
	    const unsigned& first_row, 
	    const unsigned& nrow_local,
	    const unsigned& nrow = 0);
 
 /// \short Sets the distribution. Takes first_row, nrow_local and 
 /// nrow as arguments. If nrow is not provided or equal to
 /// 0 then it is computed automatically
 void build(const OomphCommunicator* const comm_pt,
	    const unsigned& first_row, 
	    const unsigned& nrow_local,
	    const unsigned& nrow = 0);
 
 /// \short Build the LinearAlgebraDistribution. if distributed = true 
 /// (default) then uniformly distribute nrow over all processors where 
 /// processors 0 holds approximately the first nrow/n_proc, processor 
 /// 1 holds the next nrow/n_proc and so on... or if distributed = false
 /// the every row is held on every processor
 void build(const OomphCommunicator& comm_pt,
	    const unsigned& nrow,
	    const bool& distributed = true);
 
 /// \short Build the LinearAlgebraDistribution. if distributed = true 
 /// (default) then uniformly distribute nrow over all processors where 
 /// processors 0 holds approximately the first nrow/n_proc, processor 
 /// 1 holds the next nrow/n_proc and so on... or if distributed = false
 /// the every row is held on every processor
 void build(const OomphCommunicator* const comm_pt,
	    const unsigned& nrow,
	    const bool& distributed = true);
 
 /// \short Copy the argument distribution.\n
 /// Also a helper method for the =assignment operator and copy constructor
 void build(const LinearAlgebraDistribution& new_dist);

 /// \short Copy the argument distribution.\n
 /// Also a helper method for the =assignment operator and copy constructor
 void build(const LinearAlgebraDistribution* new_dist_pt)
  {
    this->build(*new_dist_pt);
  }
 
 /// \short clears the distribution
 void clear()
  {
   // delete the communicator
   delete Comm_pt;
   Comm_pt = 0;

   // delete first_row and nrow_local
   First_row.clear();
   Nrow_local.clear();

   // zero Nrow
   Nrow = 0;
  }
 
 /// \short access function to the number of global rows. 
 unsigned nrow() const
  {
   return Nrow;
  }

 /// \short access function for the num of local rows on this processor. If 
 /// no MPI the nrow is returned
 unsigned nrow_local() const
  {
#ifdef PARANOID
   if (Comm_pt == 0)
    { 
     throw OomphLibError(
      "LinearAlgebraDistribution has not been built : Comm_pt == 0.",
      "LinearAlgebraDistribution::nrow_local()",     
      OOMPH_EXCEPTION_LOCATION);
    }                 
#endif

   // return the nrow_local
#ifdef OOMPH_HAS_MPI
   if (Distributed)
    {
     return Nrow_local[Comm_pt->my_rank()];
    }
   else
    {
     return Nrow;
    }
#else
   return Nrow;
#endif
  }

 /// \short access function for the num of local rows on this processor. If 
 /// no MPI the nrow is returned
 unsigned nrow_local(const unsigned& p) const
  {
#ifdef PARANOID
   if (Comm_pt == 0)
    { 
     throw OomphLibError(
      "LinearAlgebraDistribution has not been built : Comm_pt == 0.",
      "LinearAlgebraDistribution::nrow_local()",     
      OOMPH_EXCEPTION_LOCATION);
    }                 
   if (p >= unsigned(Comm_pt->nproc()))
    {
     std::ostringstream error_message;
     error_message
      << "Requested nrow_local(" << p << "), but this distribution is defined "
      << "on " << Comm_pt->nproc() << "processors.";  
     throw OomphLibError(error_message.str(),
                         "LinearAlgebraDistribution::nrow_local()",     
                         OOMPH_EXCEPTION_LOCATION);
    }
#endif

   // return the nrow_local
#ifdef OOMPH_HAS_MPI
   if (Distributed)
    {
     return Nrow_local[p];
    }
   else
    {
     return Nrow;
    }
#else
   return Nrow;
#endif
  }

 /// \short access function for the first row on this processor
 unsigned first_row() const
  {
#ifdef PARANOID
   if (Comm_pt == 0)
    { 
     throw OomphLibError
      ("LinearAlgebraDistribution has not been built : Comm_pt == 0.",
       "LinearAlgebraDistribution::first_row()",     
       OOMPH_EXCEPTION_LOCATION);
    }                                             
#endif 

      // return the first row
#ifdef OOMPH_HAS_MPI
   if (Distributed)
    {
     return First_row[Comm_pt->my_rank()];
    }
   else
    {
     return 0;
    }
#else
   return 0;
#endif
  }

 /// \short access function for the first row on this processor
 unsigned first_row(const unsigned& p) const
  {
#ifdef PARANOID
   if (Comm_pt == 0)
    { 
     throw OomphLibError
      ("LinearAlgebraDistribution has not been built : Comm_pt == 0.",
       "LinearAlgebraDistribution::first_row()",     
       OOMPH_EXCEPTION_LOCATION);
    }                  
   if (p >= unsigned(Comm_pt->nproc()))
    {
     std::ostringstream error_message;
     error_message
      << "Requested first_row(" << p << "), but this distribution is defined "
      << "on " << Comm_pt->nproc() << "processors.";  
     throw OomphLibError(error_message.str(),
                         "LinearAlgebraDistribution::first_row()",     
                         OOMPH_EXCEPTION_LOCATION);
    }
                                         
#endif 

      // return the first row
#ifdef OOMPH_HAS_MPI
   if (Distributed)
    {
     return First_row[p];
    }
   else
    {
     return 0;
    }
#else
   return 0;
#endif
  }

 /// \short access function to the distributed - indicates whether the 
 /// distribution is serial or distributed
 bool distributed() const
  {
   return Distributed;
  }

 /// const access to the communicator pointer
 OomphCommunicator* communicator_pt() const
  {
   return Comm_pt;
  }

 /// if the communicator_pt is null then the distribution is not setup then
 /// false is returned, otherwise return true
 bool built() const 
  { 
   if (Comm_pt == 0)
    {
     return false;
    }
   return true;
  }

 /// \short == Operator
 bool operator==(const LinearAlgebraDistribution& other_dist) const;

 /// \short != operator
 bool operator!=(const LinearAlgebraDistribution& other_dist) const
  {
   return !(*this == other_dist);
  }

 /// \short << operator
 friend std::ostream& operator<<(std::ostream& stream, 
                                 LinearAlgebraDistribution dist);
 
 /// \short return the local index corresponding to the global index 
 unsigned global_to_local_row_map(const unsigned& global_i) const
  {
#ifdef PARANOID
   if (global_i >= Nrow)
    {
     throw OomphLibError
      ("Requested global row outside the number of global rows",
       "LinearAlgebraDistribution::global_to_local_row_map()",     
       OOMPH_EXCEPTION_LOCATION);      
    }
#endif
   int local_i = static_cast<int>(global_i);
   int p = 0;
   while ((int)(local_i - (int)nrow_local(p)) >= 0)
    {
     local_i -= (int)nrow_local(p);
     p++;
    }
   return (unsigned)local_i;
  }

 /// \short return the processor rank of the global row number i 
 unsigned rank_of_global_row(const unsigned i) const
  {
   unsigned p = 0;
   while (i < first_row(p) || i >= first_row(p)+nrow_local(p))
    {
     p++;
    }
   return p;
  }
 
  private:
 
 /// the number of global rows
 unsigned Nrow;
 
 /// the number of local rows on the processor
 Vector<unsigned> Nrow_local;
 
 /// the first row on this processor
 Vector<unsigned> First_row;
  
 /// flag to indicate whether this distribution describes an object that is
 /// distributed over the processors of Comm_pt (true) or duplicated over the
 /// processors of Comm_pt (false)
 bool Distributed;

 /// the pointer to the MPI communicator object in this distribution
 OomphCommunicator* Comm_pt;
}; //end of LinearAlgebraDistribution


//=============================================================================
/// \short Base class for any linear algebra object that is distributable.
/// Just contains storage for the LinearAlgebraDistribution object and 
/// access functions
//=============================================================================
 class DistributableLinearAlgebraObject
  {
   
   public :
    
    /// Default constructor - just null the distribution pt
    DistributableLinearAlgebraObject() 
    {
     Distribution_pt = new LinearAlgebraDistribution;
    }
   
   /// Broken copy constructor
   DistributableLinearAlgebraObject
    (const DistributableLinearAlgebraObject& matrix) 
    {
     BrokenCopy::broken_copy("DistributableLinearAlgebraObject");
    } 
   
   /// Broken assignment operator
   void operator=(const DistributableLinearAlgebraObject&) 
    {
     BrokenCopy::broken_assign("DistributableLinearAlgebraObject");
    }
   
   /// Destructor
   virtual ~DistributableLinearAlgebraObject()
    {
     delete Distribution_pt;
    }
   
   /// access to the LinearAlgebraDistribution
   LinearAlgebraDistribution* distribution_pt()  const
    {
     return Distribution_pt;
    }
  
   /// \short access function to the number of global rows. 
   unsigned nrow() const
    {
     return Distribution_pt->nrow();
    }
   
   /// \short access function for the num of local rows on this processor.
   unsigned nrow_local() const
    {
     return Distribution_pt->nrow_local();
    }
   
   /// \short access function for the num of local rows on this processor.
   unsigned nrow_local(const unsigned& p) const
    {
     return Distribution_pt->nrow_local(p);
    }
   
   /// \short access function for the first row on this processor
   unsigned first_row() const
    {
     return Distribution_pt->first_row();
    }
   
   /// \short access function for the first row on this processor
   unsigned first_row(const unsigned& p) const
    {
     return Distribution_pt->first_row(p);
    }

   /// distribution is serial or distributed
   bool distributed() const
    {
     return Distribution_pt->distributed();
    }

   /// if the communicator_pt is null then the distribution is not setup then
   /// false is returned, otherwise return true
   bool distribution_built() const 
    { 
     return Distribution_pt->built();
    }
   
    protected:
   
   /// \short setup the distribution of this distributable linear algebra
   /// object
   void build_distribution(const LinearAlgebraDistribution* const dist_pt)
    {
     Distribution_pt->build(dist_pt);
    }

   /// \short setup the distribution of this distributable linear algebra
   /// object
   void build_distribution(const LinearAlgebraDistribution& dist)
    {
     Distribution_pt->build(dist);
    }

   /// \short clear the distribution of this distributable linear algebra 
   /// object
   void clear_distribution()
    {
     Distribution_pt->clear();
    }

  private:
   
   /// the LinearAlgebraDistribution object
   LinearAlgebraDistribution* Distribution_pt;
  }; // end of DistributableLinearAlgebraObject
} // end of oomph namespace
#endif
