#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ nestedclass;
#pragma link C++ nestedtypedef;

#pragma link C++ namespace gbl;
#pragma link C++ class gbl::MilleEvent+;
#pragma link C++ namespace genfit;
#pragma link C++ class genfit::AbsFinitePlane+;
#pragma link C++ class genfit::AbsHMatrix+;
#pragma link C++ class genfit::RectangularFinitePlane+;
#pragma link C++ class genfit::FitStatus+;
#pragma link C++ class genfit::Material+;
#pragma link C++ class genfit::PruneFlags+;
#pragma link C++ class genfit::TrackCand+;
#pragma link C++ class genfit::TrackCandHit+;

#pragma link C++ class genfit::AbsTrackRep+;
#pragma link C++ class genfit::MeasuredStateOnPlane+;
#pragma link C++ class genfit::AbsMeasurement+; // trackPoint_
#pragma link C++ class genfit::AbsFitterInfo-; // trackPoint_, rep_, sharedPlanePtr
#pragma link C++ class genfit::DetPlane-;  // scoped_ptr<> finitePlane_
#pragma link C++ class genfit::MeasurementOnPlane-; // scoped_ptr<> hMatrix_
#pragma link C++ class genfit::StateOnPlane-;  // rep_, sharedPlanePtr
#pragma link C++ class genfit::ThinScatterer-; // sharedPlanePtr
#pragma link C++ class genfit::Track-;
#pragma link C++ class genfit::TrackPoint-; // track_, fixup the map
#pragma link C++ class vector<genfit::TrackPoint*>-;
#pragma link C++ class genfit::RKTrackRep-;

#pragma link C++ class genfit::HMatrixU+;
#pragma link C++ class genfit::HMatrixUnit+;
#pragma link C++ class genfit::HMatrixV+;
#pragma link C++ class genfit::HMatrixUV+;
#pragma link C++ class genfit::ProlateSpacepointMeasurement+;
#pragma link C++ class genfit::WireMeasurement+;
#pragma link C++ class genfit::WireMeasurementNew+;
#pragma link C++ class genfit::WirePointMeasurement+;
#pragma link C++ class genfit::HMatrixPhi-;
#pragma link C++ class genfit::FullMeasurement-;
#pragma link C++ class genfit::PlanarMeasurement-;
#pragma link C++ class genfit::SpacepointMeasurement-;
#pragma link C++ class genfit::WireTrackCandHit+;

#pragma link C++ class genfit::AbsKalmanFitter+;
#pragma link C++ class genfit::KalmanFitStatus;
#pragma link C++ class genfit::KalmanFitterRefTrack+;
#pragma link C++ class genfit::KalmanFittedStateOnPlane+;
#pragma link C++ class genfit::ReferenceStateOnPlane+;
#pragma link C++ class genfit::KalmanFitter-;
#pragma link C++ class genfit::KalmanFitterInfo-;
#pragma link C++ class genfit::DAF-;

#pragma link C++ class genfit::GFGbl+;
#pragma link C++ class genfit::GblFitter+;
#pragma link C++ class genfit::ICalibrationParametersDerivatives+;
#pragma link C++ class genfit::GblFitStatus+;
#pragma link C++ class genfit::GblFitterInfo+;
#pragma link C++ class genfit::GblTrackSegmentController+;
#pragma link C++ class gbl::GblData+;
#pragma link C++ class vector<gbl::GblData>+;
