# set up all AMSsoft environment variables using specific configuration

# icc or gcc
setenv UCC icc64
#setenv UCC gcc64

# root5 or root6
setenv UROOT 5
#setenv UROOT 6

# please change AMSWD to absolute directory of your AMSsoft
# setenv AMSWD 
source $AMSWD/install/amsvar_all.csh
