//  $Id: linkdef5.h,v 1.1 2016/06/01 12:44:19 choutko Exp $
#ifdef __CINT__
#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ class RichHitR+;
#pragma link C++ class RichRingR+;
#pragma link C++ class RichRingTables+;
#pragma link C++ class RichRingBSegmentR+;
#pragma link C++ class RichRingBR+;
#pragma link C++ class TrdRawHitR+;
#pragma link C++ class TrdClusterR+;
#pragma link C++ class TrdSegmentR+;
#pragma link C++ class TrdTrackR+;
#pragma link C++ class TrdHSegmentR+;
#pragma link C++ class TrdHTrackR+;
#pragma link C++ class TrdHReconR+;
#pragma link C++ class TrdHChargeR+;
#pragma link C++ class TrdHCalibR+;

#ifdef _PGTRACK_
#pragma link C++ class TofTrack+;
#pragma link C++ class TofCandidate+;
#pragma link C++ class TofRecon+;
#pragma link C++ class TofTimeCalibration+;
#pragma link C++ class TofGeometry+;
#endif

#ifdef _PGTRACK_
//#pragma link C++ class AC_TrdHits+;
//#pragma link C++ class TrdSCalibR+;
//#pragma link C++ class TrdSChi2Fit+;
#pragma link C++ class TrdKCalib+;
#pragma link C++ class TrdKToyMC+;
#pragma link C++ class TrdKPDF+;
#pragma link C++ class TrdKCluster+;
#pragma link C++ class TrdKHit+;
#pragma link C++ class TrdMTrack++;
#pragma link C++ class GammaFit++;
#pragma link C++ class GammaPDF++;
#endif
#endif

