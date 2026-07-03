/* Copyright 2008-2010, Technische Universitaet Muenchen,
   Authors: Christian Hoeppner & Sebastian Neubert & Johannes Rauch

   This file is part of GENFIT.

   GENFIT is free software: you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License as published
   by the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   GENFIT is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with GENFIT.  If not, see <http://www.gnu.org/licenses/>.
*/
/** @addtogroup genfit
 * @{
 */

#ifndef genfit_KalmanFittedStateOnPlane_h
#define genfit_KalmanFittedStateOnPlane_h

#include "MeasuredStateOnPlane.h"


namespace genfit {


/**
 *  @brief #MeasuredStateOnPlane with additional info produced by a Kalman filter or DAF.
 */
class KalmanFittedStateOnPlane : public MeasuredStateOnPlane {

 public:

  KalmanFittedStateOnPlane();
  KalmanFittedStateOnPlane(const TVectorD& state, const TMatrixDSym& cov, const SharedPlanePtr& plane, const AbsTrackRep* rep, double chiSquareIncrement, double ndf);
  KalmanFittedStateOnPlane(const TVectorD& state, const TMatrixDSym& cov, const SharedPlanePtr& plane, const AbsTrackRep* rep, const TVectorD& auxInfo, double chiSquareIncrement, double ndf);
  KalmanFittedStateOnPlane(const MeasuredStateOnPlane& state, double chiSquareIncrement, double ndf);

  KalmanFittedStateOnPlane& operator=(KalmanFittedStateOnPlane other);
  void swap(KalmanFittedStateOnPlane& other); // nothrow

  virtual ~KalmanFittedStateOnPlane() {}

  double getChiSquareIncrement() const {return chiSquareIncrement_;}
  double getNdf() const {return ndf_;}

  void setChiSquareIncrement(double chiSquareIncrement) {chiSquareIncrement_ = chiSquareIncrement;}
  void setNdf(double ndf) {ndf_ = ndf;}

  void setChiSquareIncrementuv(double chiSquareIncrementuv[2]) {for(int iuv=0;iuv<2;iuv++)chiSquareIncrementuv_[iuv] = chiSquareIncrementuv[iuv];}
  void setNdfuv(double ndfuv[2]) {for(int iuv=0;iuv<2;iuv++)ndfuv_[iuv] = ndfuv[iuv];}

  double getChiSquareIncrementuv(int iuv) const {return chiSquareIncrementuv_[iuv];}
  double getNdfuv(int iuv) const {return ndfuv_[iuv];}

 protected:

  double chiSquareIncrement_;
  
  //! Degrees of freedom. Needs to be a double because of DAF.
  double ndf_;

  double chiSquareIncrementuv_[2];
  
  double ndfuv_[2];

 public:
#ifdef VERSION6
  ClassDef(KalmanFittedStateOnPlane,1)
#endif
};


inline KalmanFittedStateOnPlane::KalmanFittedStateOnPlane() :
  MeasuredStateOnPlane(), chiSquareIncrement_(0), ndf_(0)
{
  for(int iuv=0;iuv<2;iuv++)chiSquareIncrementuv_[iuv]=ndfuv_[iuv]=0;
}

inline KalmanFittedStateOnPlane::KalmanFittedStateOnPlane(const TVectorD& state, const TMatrixDSym& cov, const SharedPlanePtr& plane, const AbsTrackRep* rep, double chiSquareIncrement, double ndf) :
  MeasuredStateOnPlane(state, cov, plane, rep), chiSquareIncrement_(chiSquareIncrement), ndf_(ndf)
{
  for(int iuv=0;iuv<2;iuv++)chiSquareIncrementuv_[iuv]=ndfuv_[iuv]=0;
}

inline KalmanFittedStateOnPlane::KalmanFittedStateOnPlane(const TVectorD& state, const TMatrixDSym& cov, const SharedPlanePtr& plane, const AbsTrackRep* rep, const TVectorD& auxInfo, double chiSquareIncrement, double ndf) :
  MeasuredStateOnPlane(state, cov, plane, rep, auxInfo), chiSquareIncrement_(chiSquareIncrement), ndf_(ndf)
{
  for(int iuv=0;iuv<2;iuv++)chiSquareIncrementuv_[iuv]=ndfuv_[iuv]=0;
}

inline KalmanFittedStateOnPlane::KalmanFittedStateOnPlane(const MeasuredStateOnPlane& state, double chiSquareIncrement, double ndf) :
  MeasuredStateOnPlane(state), chiSquareIncrement_(chiSquareIncrement), ndf_(ndf)
{
  for(int iuv=0;iuv<2;iuv++)chiSquareIncrementuv_[iuv]=ndfuv_[iuv]=0;
}

inline KalmanFittedStateOnPlane& KalmanFittedStateOnPlane::operator=(KalmanFittedStateOnPlane other) {
  swap(other);
  return *this;
}

inline void KalmanFittedStateOnPlane::swap(KalmanFittedStateOnPlane& other) {
  MeasuredStateOnPlane::swap(other);
  std::swap(this->chiSquareIncrement_, other.chiSquareIncrement_);
  std::swap(this->ndf_, other.ndf_);
  for(int iuv=0;iuv<2;iuv++){
    std::swap(this->chiSquareIncrementuv_[iuv], other.chiSquareIncrementuv_[iuv]);
    std::swap(this->ndfuv_[iuv], other.ndfuv_[iuv]);
  }
}

} /* End of namespace genfit */
/** @} */

#endif // genfit_KalmanFittedStateOnPlane_h
