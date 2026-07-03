//  $Id: EcalShowerDirectionFit.h,v 1.1 2015/06/22 22:11:37 nzimmerm Exp $
#ifndef __EcalShowerDirectionFit_h__
#define __EcalShowerDirectionFit_h__

#include "point.h"
#include "typedefs.h"

#ifndef __ROOTSHAREDLIBRARY__
class AMSEcal2DCluster;
class Ecal1DCluster;
#else
class EcalShowerR;
class EcalClusterR;
class Ecal2DClusterR;
#endif

// Avoid including ecaldbc.h here, as that requires compiling ecaldbc.C also into libntuple, which is not an easy task.
class EcalShowerDirectionFit {
public:
  static const integer _ECSLMX=9; // same as ecalconst::ECLSMX
  static const uinteger _LayersNo = 9 * 2; // same as ECALDBc::GetLayersNo

  number _Direction;
  number  _Zcorr[2*_ECSLMX];
  AMSPoint _EntryPoint;
  AMSPoint _ExitPoint;
  AMSDir   _Dir;
  AMSDir   _EMDir;
  integer tot[2];
  number chi2[2],t0[2],tantz[2];
  bool zcorr[2];
  number _OrphanedEnergy;

  void DirectionFit();
  static void monit(number & a, number & b,number sim[], int & n, int & s, int & ncall) {}

#ifndef __ROOTSHAREDLIBRARY__
  AMSEcal2DCluster *_pCl[2];
  static bool StrLineFit(Ecal1DCluster *p1[],int maxr,int proj, bool reset,number *pcorrect,int & tot, number &chi2, number &t0, number &tantz);
#else
  EcalShowerR* _Shower;
  Ecal2DClusterR *_pCl[2];
  static bool StrLineFit(EcalClusterR *p1[],int maxr,int proj, bool reset,number *pcorrect,int & tot, number &chi2, number &t0, number &tantz);
  void FindOrphaned1DClustersFirstPass(std::vector<EcalClusterR*>& orphanedClusters);
  bool FindOrphaned1DClustersSecondPass(int cluster2DIndexStart, number& orphanedEnergySum, std::vector<EcalClusterR*>& orphanedClusters);
#endif

public:
#ifndef __ROOTSHAREDLIBRARY__
  EcalShowerDirectionFit(AMSEcal2DCluster* pCl0, AMSEcal2DCluster* pCl1);
#else
  EcalShowerDirectionFit(EcalShowerR*, number orphanedEnergy);
#endif

};

#endif
