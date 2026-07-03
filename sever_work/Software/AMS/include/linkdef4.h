//  $Id: linkdef4.h,v 1.1 2016/06/01 12:44:19 choutko Exp $
#ifdef __CINT__
#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ class TrMCClusterR+;
#pragma link C++ class TrRawClusterR+;
#pragma link C++ class TrClusterR+;
#pragma link C++ class TrRecHitR+;
#pragma link C++ class TrTrackR+;
//////////////////////////////////////
#ifdef __ROOTSHAREDLIBRARY__
#pragma link C++ class TofRecH++;
#endif
#pragma link C++ class TofChargePar++;
#pragma link C++ class TofLikelihoodPar++;
#pragma link C++ class TofChargeHR++;
#ifdef __ROOTSHAREDLIBRARY__
#pragma link C++ class TofPDFH++;
#endif
//#pragma link C++ class TofTDVTool++;
//#pragma link C++ class TofTAlignPar++;
//////////////////////////////////////

#ifdef __ROOTSHAREDLIBRARY__
#pragma link C++ class AMSChain+;
#pragma link C++ class AMSEventList+;
#ifndef _PGTRACK_
#pragma link C++ class TrHitA+;
#pragma link C++ class TrTrackA+;
#endif
#endif

#pragma link C++ class MagnetVarp++;
#pragma link C++ class MagnetVarp::MagnetTemperature++;
#pragma link C++ class ScalerMon+;
#pragma link C++ class Lvl1TrigConfig+;
#pragma link C++ class AMSPoint+;
#pragma link C++ class AMSDir+;
#pragma link C++ class AMSRotMat+;
#pragma link C++ class AMSEnergyLoss+;
#pragma link C++ class HeaderR+;
#pragma link C++ class AMSSetupR::SlowControlR+;
#pragma link C++ class AMSSetupR::SlowControlR::Element+;
#pragma link C++ class AMSSetupR::Header+;
#pragma link C++ class AMSSetupR::GPS+;
#pragma link C++ class AMSSetupR::Tsy05Cutoff+;
#pragma link C++ class AMSSetupR::RTI+;
#pragma link C++ class AMSSetupR::RunI+;
#pragma link C++ class EcalHitR+;
#pragma link C++ class EcalClusterR+;
#pragma link C++ class Ecal2DClusterR+;
#pragma link C++ class EcalShowerR+;
#pragma link C++ class TofRawClusterR+;
#pragma link C++ class TofRawSideR+;
#pragma link C++ class TofClusterR+;
#pragma link C++ class TofClusterHR+;
#pragma link C++ class AntiRawSideR+;
#pragma link C++ class AntiClusterR+;
#ifdef __USEANTICLUSTERPG__
#pragma link C++ class AntiRecoPG;
#endif
#endif
