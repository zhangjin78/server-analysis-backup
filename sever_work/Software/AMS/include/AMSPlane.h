// $Id: AMSPlane.h,v 1.12 2022/04/08 00:07:45 qyan Exp $

#ifndef __AMSPlane__
#define __AMSPlane__


//////////////////////////////////////////////////////////////////////////
///
///
///\class AMSPlane software
///\brief AMSPlane software for plane geometry and measurement
///
///\date  2020/11/18 QY  First version
///
/// $Revision: 1.12 $
///
//////////////////////////////////////////////////////////////////////////

#include <TVector3.h>
#include <TRotation.h>
#include <TMatrixDSym.h>
#include <TMatrixD.h>
#include <memory>
#include <map>
#include <vector>

//! A General class for defining a plane and transformation between local and global coordinate system
/*!
 * \author qyan@cern.ch
 */
class AMSPlane {

 private:
  // Private Methods -----------------
  //! ensures orthonormal coordinates
  void sane();
  TVector3 o_;///<a point in the plane, normally defined to be the plane geometric center (or local original point)
  TVector3 u_;///<local X axis direction
  TVector3 v_;///<local Y axis direction
  TVector3 p_;///<track position in global coordinate system
  TVector3 d_[2];///<track direction in global coordinates(before/after plane)

 public:
  // Constructors/Destructors ---------
  AMSPlane();
  AMSPlane(TVector3 o, TVector3 u, TVector3 v);
  AMSPlane(TVector3 o, TVector3 n);

  virtual ~AMSPlane();

  // Accessors -----------------------
  int   moveOUV(TRotation Rta, TVector3 r0a);///<move plane with rotation matrix and shift 
  int   moveOLocal(double, double, double);///<move plane along local X,Y,Z axes
  const TVector3& getO() const    {return o_;}///<a point in the plane, normally defined to be the plane geometric center (or local original point)
  const TVector3& getU() const    {return u_;}///<local X axis direction
  const TVector3& getV() const    {return v_;}///<local Y axis direction
  const TVector3& getP() const    {return p_;}///<track position in global coordinate system
        TVector3  getPLocal()const{return LabToPlane3(p_);}///<track position in local coordinate system
  const TVector3& getD(int i=0) const    {return d_[i];}///<track direction in global coordinate system
        TVector3  getDLocal(int i=0)const{return project3(d_[i]);}///<track direction in local coordinate system
  // Modifiers -----------------------
  void set(const TVector3& o,
           const TVector3& u,
           const TVector3& v);
  void setO(const TVector3& o);
  void setO(double, double, double);
  void setU(const TVector3& u);
  void setU(double, double, double);
  void setV(const TVector3& v);
  void setV(double, double, double);
  void setP(const TVector3& p);
  void setP(double, double, double);
  void setPLocal(const TVector3& ploc);
  void setD(const TVector3& d,int i=0);
  void setD(double, double, double,int i=0);
  void setDLocal(const TVector3& dloc,int i=0);
  void setUV(const TVector3& u, const TVector3& v);
  void setON(const TVector3& o, const TVector3& n);

  // Operations ----------------------
  TVector3 getNormal() const;///<local Z axis direction
  void setNormal(const TVector3& n);
  void setNormal(double, double, double);
  void setNormal(const double& theta, const double& phi);

  //! projecting a direction onto the plane:
  TVector2 project(const TVector3& x) const;

  TVector3 project3(const TVector3& x) const;

  //! transform from Lab system into plane
  TVector2 LabToPlane(const TVector3& x) const;

  TVector3 LabToPlane3(const TVector3& x) const;
   
  //! transform from plane coordinates to lab system
  TVector3 toLab(const TVector2& x) const;

  //! transform from plane coordinates to lab system (isdir=0/position, isdir=1/direction)
  TVector3 toLab3(const TVector3& x,bool isdir=0) const;

  //! get vector from point to plane (normal)
  TVector3 dist(const TVector3& point) const;

  //! gives u,v coordinates of the intersection point of a straight line with plane
  TVector2 straightLineToPlane(const TVector3& point, const TVector3& dir) const;

  //! gives u,v coordinates of the intersection point of a straight line with plane
  void straightLineToPlane(const double& posX, const double& posY, const double& posZ,
                           const double& dirX, const double& dirY, const double& dirZ,
                           double& u, double& v) const;

  void Print() const;

  //! Checks equality of planes by comparing the 9 double values that define them.
  friend bool operator== (const AMSPlane& lhs, const AMSPlane& rhs);
  //! returns NOT ==
  friend bool operator!= (const AMSPlane& lhs, const AMSPlane& rhs);

  //! absolute distance from a point to the plane
  double distance(const TVector3& point) const;
  double distance(double, double, double) const;


  //! rotate u and v around normal. Angle is in rad. More for debugging than for actual use.
  void rotate(double angle);

  //! set O, U, V to default values
  void reset();

};


//! A General class for defining a measurement in plane
/*!
 * \author qyan@cern.ch
 */
class TkPlaneN;
class AMSPlaneM: public AMSPlane {
 protected:
  int id_;///<detector id
  TkPlaneN *module_;///<detector module
  int mstat_;///<measurement status (should==0, at least>=-10)
  TVector2 m_;///<measurement point in plane (local)
  TMatrixDSym mcov_;///<measurement variance V=err^2
  std::vector<std::pair<int,TMatrixD> > malignder_;
 
 public:
  AMSPlaneM(int id=0, TkPlaneN *module=0);
  AMSPlaneM(TVector3 o, TVector3 u, TVector3 v, int id=0, TkPlaneN *module=0);
  AMSPlaneM(TVector2 m, int id=0, int mstat=0, TkPlaneN *module=0);
  AMSPlaneM(TVector2 m, TMatrixDSym mcov, int id=0, int mstat=0, TkPlaneN *module=0);
  int                getid()          {return id_;}///<detector id
  TkPlaneN          *getmodule()      {return module_;}///<get detector module
  int                getMStat()       {return mstat_;}///<measurement status for checkng error (mstat<0 measurement has error, mstat<-10 measurement has fatal error)
  const TVector2&    getM()const      {return m_;}///<measurement point in local coordinate system
  TVector3           getMGlobal()const{return toLab(m_);}///<measurement point in global coordinate system
  const TMatrixDSym& getMCov()        {return mcov_;}///<measurement variance in local coordinate system
  TMatrixDSym        getMCovGlobal() const;///<measurement variance in global coordinate system
  //! measurement-track position (residual), isloc=1(0) will return residual in local(global) coordinate system 
  TVector3           getMPdist(bool isloc=1)const;
  TMatrixD           calMAlignDerGlobal(bool update=0, int id=0);///<calculate derivative matrix used for alignment
  std::vector<std::pair<int,TMatrixD> >* getmalignder() {return &malignder_;} 
  std::pair<std::vector<int>,TMatrixD>   getMAlignDerGlobal();
  std::pair<std::vector<int>,TMatrixD>   getResAlignDerGlobal(TVector3& dloc);
  void setid(int id) {id_=id;}///<set detector id
  void setmodule(TkPlaneN *module) {module_=module;}///<set detector module
  void setMStat(int);
  void setM(const TVector2& p);
  void setM(double, double);
  void moveM(double, double);//in local
  void setMCov(const TMatrixDSym& mcov);
  void setMCov(int, int, double);
};


//! A General class for defining a detector plane
/*!
 * \author qyan@cern.ch
 */
class TkPlaneN: public AMSPlane{

 protected:
  int id_;///<detector id
  int type_;///<detector type
  double dim_[3];///<detector dimension

 public:
  ///r_{g}=Rt*dR[0]*dR[1]*(q_{l}+dq[0]+dq[1])+r0
  TRotation Rt;///<nominal rotation matrix with respect to mother volume
  TRotation dR[2];///<alignment correction
  TVector3  dq[2];///<alignment correction
  TVector3  r0;///<detector nominal position in mother volume(shoud be center of detector gravity)
  TkPlaneN *mother;///<pointer of the mother volume

 public:
  TkPlaneN();
  TkPlaneN(int id,int type);
  int        getid() {return id_;}///<detector id
  static int getlayid(int tkid);///<get tracker layer id by using ladder tkid
  static int convertlayid(int lay,int opt=0);///<convert layid between layO scheme and V6 scheme (0-8), opt=0 from layO to V6, opt=1 from V6 to layO
  double     getdim(int ixyz) {return dim_[ixyz];}///<detector dimension
  //! pointer of the mother volume, its derived class TkSensorN's mother is TkLadderN, TkLadderN's mother is TkLayerN, and TkLayerN's mother is TkTrackN
  TkPlaneN  *getmother() {return mother;} 
  double     getLocalDistToEdge(double lcoor[2][2],TVector3 gcoo,TVector3 gdir,int uxy=2);
  void       getRtar0a(TRotation &Rta,TVector3 &r0a,int ua=-1);///<get total rotation matrix and position with respect to mother volume
  void       setid(int id) {id_=id;}///<set detector id
  void       setr0(const TVector3& r0n);///<set detector nominal position in mother volume
  void       setRt(const TRotation& Rtn);///<set nominal rotation matrix with respect to mother volume
  //! set alignment correction to zero, dR=E and dq=0
  void       cleardRdq(int ia);
  int        updateOUVFromMother(int level=-1);///<222 with alignment, 111 without alignment
};

#endif // __AMSPlane__
