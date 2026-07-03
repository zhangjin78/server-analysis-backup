# set up all AMSsoft environment variables using specific configuration

# icc or gcc
export UCC=icc64
#export UCC=gcc64

# root5 or root6
export UROOT=5
#export UROOT=6

# please change AMSWD to absolute directory of your AMSsoft
# export AMSWD 
echo "$AMSWD/install/amsvar_all.sh"
source $AMSWD/install/amsvar_all.sh
