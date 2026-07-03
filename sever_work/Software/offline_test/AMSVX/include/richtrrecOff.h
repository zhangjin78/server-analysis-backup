#ifndef __RICHTRTRACKOFF__
#define __RICHTRTRACKOFF__

#include "point.h"
#include "root.h"

namespace RichOffline{

  // This is a simple wrapper to be removed in the future

  class TrTrack{
  public:
    AMSPoint _r;
    AMSDir   _d;
  public:
    TrTrack(AMSPoint r,AMSDir p):_r(r),_d(p){};
    TrTrack(){};
    TrTrack(TrTrackR *tr);
    void interpolate(AMSPoint pnt,AMSDir dir,AMSPoint &point,
		     double &theta,double &phi,double &length);
  };
}
#endif
