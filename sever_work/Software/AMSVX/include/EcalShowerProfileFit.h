//  $Id: EcalShowerProfileFit.h,v 1.1 2015/06/22 22:11:38 nzimmerm Exp $
#ifndef __EcalShowerProfileFit_h__
#define __EcalShowerProfileFit_h__

#include "typedefs.h"

// Avoid including ecaldbc.h here, as that requires compiling ecaldbc.C also into libntuple, which is not an easy task.
class EcalShowerProfileFit {
private:
  static const integer _ECSLMX=9; // same as ecalconst::ECLSMX
  static const uinteger _LayersNo = 9 * 2; // same as ECALDBc::GetLayersNo

public:
  number  _ProfilePar[10];
  number  _Edep[2*_ECSLMX];
  number  _Ez[2*_ECSLMX];
  number  _Zcorr[2*_ECSLMX];
  integer _Direction;
  integer _iflag;
  number  _Dz;
  number _Et;
  integer _ShowerMax;
  void ProfileFit();
  static void monit(number & a, number & b,number sim[], int & n, int & s, int & ncall) {}
  static void gamfun(integer & n, number xc[], number & fc, EcalShowerProfileFit * ptr);
  static void gamfunr(number& xc, number & fc, EcalShowerProfileFit * ptr);

public:
  EcalShowerProfileFit();
};

#endif
