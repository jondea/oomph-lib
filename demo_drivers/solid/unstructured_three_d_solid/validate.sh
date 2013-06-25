#! /bin/sh

#Set the number of tests to be checked
NUM_TESTS=1

# Setup validation directory
#---------------------------
rm -rf Validation
mkdir Validation

#######################################################################

cd Validation


# Validation for 3D unstructured solid
#-------------------------------------

# Get tetgen files
cp ../fsi_bifurcation_solid.* .

mkdir RESLT

echo "Running 3D unstructured solid "
../unstructured_three_d_solid la > OUTPUT

echo "done"
echo " " >> validation.log
echo "3D unstructured solid" >> validation.log
echo "---------------------" >> validation.log
echo " " >> validation.log
echo "Validation directory: " >> validation.log
echo " " >> validation.log
echo "  " `pwd` >> validation.log
echo " " >> validation.log
cat RESLT/solid_soln0.dat RESLT/solid_soln1.dat RESLT/solid_soln2.dat > result_three_d.dat

if test "$1" = "no_fpdiff"; then
  echo "dummy [OK] -- Can't run fpdiff.py because we don't have python or validata" >> validation.log
else
../../../../bin/fpdiff.py ../validata/result_three_d.dat.gz \
    result_three_d.dat  >> validation.log
fi



# Append output to global validation log file
#--------------------------------------------
cat validation.log >> ../../../../validation.log




# Append output to global validation log file
#--------------------------------------------
cat validation.log >> ../../../../validation.log


cd ..



#######################################################################


#Check that we get the correct number of OKs
# validate_ok_count will exit with status
# 0 if all tests has passed.
# 1 if some tests failed.
# 2 if there are more 'OK' than expected.
MAKEFLAGS=s
OOMPH_ROOT_DIR=$(make print-top_builddir)
. $OOMPH_ROOT_DIR/bin/validate_ok_count

# Never get here
exit 10
