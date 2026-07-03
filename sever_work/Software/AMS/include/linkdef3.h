//  $Id: linkdef3.h,v 1.2 2016/06/10 09:11:38 choutko Exp $
#ifdef __CINT__
#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

////////////// Ext. alignment
#pragma link C++ class DynAlContinuity;
#pragma link C++ class DynAlFit;
#pragma link C++ class DynAlHistory+;
#pragma link C++ class DynAlEvent+;
#pragma link C++ class DynAlFitParameters+;
#pragma link C++ class DynAlFitContainer+;
#pragma link C++ class DynAlManager;
/////////////////////// Rich Tools
#ifndef VERSION66
#pragma link C++ class GeomHash;
#endif
#pragma link C++ class GeomHashEnsemble;
//////////////////////////////////////
#ifdef __ROOTSHAREDLIBRARY__
#pragma link C++ class RichPMTCalib;
#endif
#pragma link C++ class RichConfigManager;
#pragma link C++ class RichConfigContainer+;
/////////////////////////////////////// Unfolding tools
#ifdef __ROOTSHAREDLIBRARY__
#pragma link C++ class BayesianUnfolder;
#pragma link C++ class DEMinimizer;
#pragma link C++ class MCSampler;
#pragma link C++ class StochasticUnfolding;
#pragma link C++ class FUnfolding;
#pragma link C++ class SamplingUnfolding;
#pragma link C++ class BayesianConsistent;
#endif
#pragma link C++ class SplFit+;
#pragma link C++ class SpReso+;
#pragma link C++ class SpFold+;
///////////////////////////////////////////////////////
#pragma link C++ class TrPdf+;
#pragma link C++ class VCon;
#pragma link C++ class TrElem+;
#ifdef __ROOTSHAREDLIBRARY__
#pragma link C++ class VCon_root;
#endif
#pragma link C++ class EcalAxis+;
#pragma link C++ class EcalCR+;
#pragma link C++ class EcalFastRec+;
#pragma link C++ class Ecal3DRec+;
#pragma link C++ class EcalKRec+;
#pragma link C++ class GeoMagField+;
#pragma link C++ class GeoMagTrace+;
#pragma link C++ class GeoMagCoord+;
#pragma link C++ class LxMCcutoff+;
#pragma link C++ class QBincutoff+;
#endif
