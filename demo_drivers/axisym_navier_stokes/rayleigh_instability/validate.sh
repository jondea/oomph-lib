#! /bin/sh


#Set the number of tests to be checked
NUM_TESTS=1


# Setup validation directory
#---------------------------
touch Validation
rm -r -f Validation
mkdir Validation

# Validation for demo axisym advection diffusion with flux bc
#------------------------------------------------------------
cd Validation

echo "Running Plateau-Rayleigh instability validation "
mkdir RESLT
../rayleigh_instability arg1 > OUTPUT
echo "done"
echo " "                                        >> validation.log
echo "Plateau-Rayleigh instability validation " >> validation.log
echo "----------------------------------------" >> validation.log
echo " "                                        >> validation.log
echo "Validation directory: "                   >> validation.log
echo " "                                        >> validation.log
echo "  " `pwd`                                 >> validation.log
echo " "                                        >> validation.log
cat RESLT/trace.dat RESLT/soln2.dat > results.dat

if test "$1" = "no_fpdiff"; then
  echo "dummy [OK] -- Can't run fpdiff.py because we don't have python or validata" >> validation.log
else
../../../../bin/fpdiff.py ../validata/results.dat.gz \
    results.dat 0.1 2.0e-9 >> validation.log
fi

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
