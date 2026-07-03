//  $Id: linkdef2.h,v 1.2 2016/06/06 05:03:17 choutko Exp $
#ifdef __CINT__
#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;
#ifdef _PGTRACK_
#pragma link C++ class MagField;
#pragma link C++ class BCorr;
#pragma link C++ class TrFit+;
#pragma link C++ class TrProp+;
#pragma link C++ class GausBF+;
//#pragma link C++ class TrAlignFit;
#pragma link C++ class TrTrackPar+;
#pragma link C++ class TrTasClusterR+;
#pragma link C++ class TrTasPar+;
#pragma link C++ class TrTasDB+;
#pragma link C++ class TrTrackSelection+;
#pragma link C++ class TrInnerDzDB+;
#pragma link C++ class TrInnerDzDB::DzElem+;
#pragma link C++ class TrExtAlignDB+;
#pragma link C++ class TrExtAlignPar+;
#pragma link C++ class TrExtAlignFit+;
#pragma link C++ class TrHistoManHeader;
#pragma link C++ class TrHistoMan;
#pragma link C++ class TrOnlineMonitor;
#pragma link C++ class TrkLH;
#pragma link C++ class TrkLHVar;

#pragma link C++ class TrkCC;
#pragma link C++ class TrkCCVar;
#pragma link C++ class TrkCCAlgorithm;
#pragma link C++ class TrkCCNozzoli;
#pragma link C++ class TrkCCPizzolotto;
#pragma link C++ class TMVAxmlReader;
#pragma link C++ class TrkCCLoadDatacardX;

#pragma link C++ class TKGEOMFFKEY_DEF+;
#pragma link C++ class TRMCFFKEY_DEF+;
#pragma link C++ class TRCALIB_DEF+;
#pragma link C++ class TRALIG_DEF+;
#pragma link C++ class TRCLFFKEY_DEF+;
#pragma link C++ class TRFITFFKEY_DEF+;
#pragma link C++ class TRCHAFFKEY_DEF+;

#pragma link C++ class threp+;
#pragma link C++ class HistoProofMan+;
#pragma link C++ class hashtable<threp>;
#pragma link C++ class hashtable<TH1*>;

#pragma link C++ class HistoMan;
#pragma link C++ class TRDMCFFKEY_DEF+;
#else
#pragma link C++ class TrTrackFitR+;
#pragma link C++ class TrTrackFitR::TrSCooR+;
#endif
#endif
