#include <stdio.h>
#include "CLHEP/Random/RandFlat.h"
#ifdef G4MULTITHREADED
#include "G4MTHepRandom.hh"
#endif

#if USEDPMUS == 2
# define dt_rndm   dt_rndm__
#elif USEDPMUS == 3

#else
# define dt_rndm   dt_rndm_
#endif
extern "C" {
  double dt_rndm(int &) 
  {
    double ret;
#ifdef G4MULTITHREADED
    ret=G4MTHepRandom::getTheEngine()->flat();
#else
    ret=CLHEP::RandFlat::shoot();
#endif
    return ret;
  }
}


