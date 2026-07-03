//  $Id: AMSPlane.C,v 1.11 2022/04/08 00:20:02 qyan Exp $
//
//////////////////////////////////////////////////////////////////////////
///
///\file  AMSPlane.C
///\brief AMSPlane software for plane geometry and measurement
///
/// author Q.Yan
///
///\date  2020/11/18 QY  First version
///
//////////////////////////////////////////////////////////////////////////

#include "AMSPlane.h"

#include <cassert>
#include <cmath>
#include <TMath.h>
#include <TClass.h>
#include <TBuffer.h>
#include <iostream>

AMSPlane::AMSPlane()
{
  // default constructor
  o_.SetXYZ(0.,0.,0.);
  u_.SetXYZ(1.,0.,0.);
  v_.SetXYZ(0.,1.,0.);
  p_.SetXYZ(0.,0.,0.);
  d_[0].SetXYZ(0.,0.,1.);
  d_[1].SetXYZ(0.,0.,1.);
  // sane() not needed here
}

AMSPlane::AMSPlane(TVector3 o, TVector3 u, TVector3 v)
  :o_(o), u_(u), v_(v)
{
  sane();
  p_.SetXYZ(0.,0.,0.);
  d_[0].SetXYZ(0.,0.,1.);
  d_[1].SetXYZ(0.,0.,1.);
}

AMSPlane::AMSPlane(TVector3 o, TVector3 n)
  :o_(o)
{
  setNormal(n);
  p_.SetXYZ(0.,0.,0.);
  d_[0].SetXYZ(0.,0.,1.);
  d_[1].SetXYZ(0.,0.,1.);
}


AMSPlane::~AMSPlane(){
  ;
}


void AMSPlane::set(const TVector3& o,
                const TVector3& u,
                const TVector3& v)
{
  o_ = o;
  u_ = u;
  v_ = v;
  sane();
}


void AMSPlane::setO(const TVector3& o)
{
  o_ = o;
}

void AMSPlane::setO(double X,double Y,double Z)
{
  o_.SetXYZ(X,Y,Z);
}

void AMSPlane::setU(const TVector3& u)
{
  u_ = u;
  sane(); // sets v_ perpendicular to u_
}

void AMSPlane::setU(double X,double Y,double Z)
{
  u_.SetXYZ(X,Y,Z);
  sane(); // sets v_ perpendicular to u_
}

void AMSPlane::setV(const TVector3& v)
{
  v_ = v;
  u_ = getNormal().Cross(v_);
  u_ *= -1.;
  sane();
}

void AMSPlane::setV(double X,double Y,double Z)
{
  v_.SetXYZ(X,Y,Z);
  u_ = getNormal().Cross(v_);
  u_ *= -1.;
  sane();
}

void AMSPlane::setUV(const TVector3& u,const TVector3& v)
{
  u_ = u;
  v_ = v;
  sane();
}

void AMSPlane::setON(const TVector3& o,const TVector3& n){
  o_ = o;
  setNormal(n);
}


void AMSPlane::setP(const TVector3& p)
{
  p_ = p;
}

void AMSPlane::setP(double X,double Y,double Z)
{
  p_.SetXYZ(X,Y,Z);
}

void AMSPlane::setPLocal(const TVector3& ploc)
{
  p_=toLab3(ploc,0);
}

void AMSPlane::setD(const TVector3& d, int i)
{
  d_[i]=d;
  d_[i].SetMag(1.);
}

void AMSPlane::setD(double X,double Y, double Z,int i)
{
  d_[i].SetXYZ(X,Y,Z);
  d_[i].SetMag(1.);
}

void AMSPlane::setDLocal(const TVector3& dloc,int i)
{
  d_[i]=toLab3(dloc,1);
  d_[i].SetMag(1.);
}

TVector3 AMSPlane::getNormal() const
{
  return u_.Cross(v_);
}

void AMSPlane::setNormal(double X,double Y,double Z){
  setNormal( TVector3(X,Y,Z) );
}

void AMSPlane::setNormal(const TVector3& n){
  u_ = n.Orthogonal();
  v_ = n.Cross(u_);
  u_.SetMag(1.);
  v_.SetMag(1.);
}

void AMSPlane::setNormal(const double& theta, const double& phi){
  setNormal( TVector3(TMath::Sin(theta)*TMath::Cos(phi),TMath::Sin(theta)*TMath::Sin(phi),TMath::Cos(theta)) );
}


int AMSPlane::moveOUV(TRotation Rta, TVector3 r0a){
  u_=Rta*u_;//Rt*dR=Rta
  v_=Rta*v_;//Rt*dR=Rta
  o_=Rta*o_+r0a;//Rt*dR*(o_+dq)+r0=Ra*o_+(Ra*dq+r0)
  return 0;
}

int AMSPlane::moveOLocal(double U, double V, double W){ 
  TVector3 w = getNormal();
  o_+=U*u_;
  o_+=V*v_;
  o_+=W*w;
  return 0;
}


TVector2 AMSPlane::project(const TVector3& x)const
{
  return TVector2(u_*x, v_*x);
}


TVector3 AMSPlane::project3(const TVector3& x)const
{
  TVector3 w = getNormal();
  return TVector3(u_*x, v_*x, w*x);
}


TVector2 AMSPlane::LabToPlane(const TVector3& x)const
{
  return project(x-o_);
}


TVector3 AMSPlane::LabToPlane3(const TVector3& x)const
{
  return project3(x-o_);
}



TVector3 AMSPlane::toLab(const TVector2& x)const
{
  TVector3 d(o_);
  d += x.X()*u_;
  d += x.Y()*v_;
  return d;
}

TVector3 AMSPlane::toLab3(const TVector3& x,bool isdir)const
{
  TVector3 d(o_);
  if(isdir)d.SetXYZ(0.,0.,0.);//direction has no offset
  TVector3 w = getNormal();
  d += x.X()*u_;
  d += x.Y()*v_;
  d += x.Z()*w;
  return d;
}


TVector3 AMSPlane::dist(const TVector3& x)const
{
  return toLab(LabToPlane(x)) - x;
}


void AMSPlane::sane(){
  assert(u_!=v_);

  // ensure unit vectors
  u_.SetMag(1.);
  v_.SetMag(1.);

  // check if already orthogonal
  if (u_.Dot(v_) < 1.E-5) return;

  // ensure orthogonal system
  v_ = getNormal().Cross(u_);
}


void AMSPlane::Print() const
{
 std::cout<<"AMSPlane: "
     <<"O("<<o_.X()<<", "<<o_.Y()<<", "<<o_.Z()<<") "
     <<"u("<<u_.X()<<", "<<u_.Y()<<", "<<u_.Z()<<") "
     <<"v("<<v_.X()<<", "<<v_.Y()<<", "<<v_.Z()<<") "
     <<"n("<<getNormal().X()<<", "<<getNormal().Y()<<", "<<getNormal().Z()<<") "
       <<std::endl;
}


/*
  I could write pages of comments about correct equality checking for
  floating point numbers, but: When two planes are as close as 10E-5 cm
  in all nine numbers that define the plane, this will be enough for all
  practical purposes
 */
bool operator== (const AMSPlane& lhs, const AMSPlane& rhs){
  if (&lhs == &rhs)
    return true;
  static const double detplaneEpsilon = 1.E-5;
  if(
     fabs( (lhs.o_.X()-rhs.o_.X()) ) > detplaneEpsilon  ||
     fabs( (lhs.o_.Y()-rhs.o_.Y()) ) > detplaneEpsilon  ||
     fabs( (lhs.o_.Z()-rhs.o_.Z()) ) > detplaneEpsilon
     ) return false;
  else if(
    fabs( (lhs.u_.X()-rhs.u_.X()) ) > detplaneEpsilon  ||
    fabs( (lhs.u_.Y()-rhs.u_.Y()) ) > detplaneEpsilon  ||
    fabs( (lhs.u_.Z()-rhs.u_.Z()) ) > detplaneEpsilon
    ) return false;
  else if(
    fabs( (lhs.v_.X()-rhs.v_.X()) ) > detplaneEpsilon  ||
    fabs( (lhs.v_.Y()-rhs.v_.Y()) ) > detplaneEpsilon  ||
    fabs( (lhs.v_.Z()-rhs.v_.Z()) ) > detplaneEpsilon
    ) return false;
  return true;
}

bool operator!= (const AMSPlane& lhs, const AMSPlane& rhs){
  return !(lhs==rhs);
}


double AMSPlane::distance(const TVector3& point) const {
  // |(point - o_)*(u_ x v_)|
  return fabs( (point.X()-o_.X()) * (u_.Y()*v_.Z() - u_.Z()*v_.Y()) +
               (point.Y()-o_.Y()) * (u_.Z()*v_.X() - u_.X()*v_.Z()) +
               (point.Z()-o_.Z()) * (u_.X()*v_.Y() - u_.Y()*v_.X()));
}

double AMSPlane::distance(double x, double y, double z) const {
  // |(point - o_)*(u_ x v_)|
  return fabs( (x-o_.X()) * (u_.Y()*v_.Z() - u_.Z()*v_.Y()) +
               (y-o_.Y()) * (u_.Z()*v_.X() - u_.X()*v_.Z()) +
               (z-o_.Z()) * (u_.X()*v_.Y() - u_.Y()*v_.X()));
}


TVector2 AMSPlane::straightLineToPlane (const TVector3& point, const TVector3& dir) const {
  TVector3 dirNorm(dir.Unit());
  TVector3 normal = getNormal();
  double dirTimesN = dirNorm*normal;
  if(fabs(dirTimesN)<1.E-6){//straight line is parallel to plane, so return infinity
    return TVector2(1.E100,1.E100);
  }
  double t = 1./dirTimesN * ((o_-point)*normal);
  return project(point - o_ + t * dirNorm);
}


//! gives u,v coordinates of the intersection point of a straight line with plane
void AMSPlane::straightLineToPlane(const double& posX, const double& posY, const double& posZ,
                                   const double& dirX, const double& dirY, const double& dirZ,
                                   double& u, double& v) const {

  TVector3 W = getNormal();
  double dirTimesN = dirX*W.X() + dirY*W.Y() + dirZ*W.Z();
  if(fabs(dirTimesN)<1.E-6){//straight line is parallel to plane, so return infinity
    u = 1.E100;
    v = 1.E100;
    return;
  }
  double t = 1./dirTimesN * ((o_.X()-posX)*W.X() +
                             (o_.Y()-posY)*W.Y() +
                             (o_.Z()-posZ)*W.Z());

  double posOnPlaneX = posX-o_.X() + t*dirX;
  double posOnPlaneY = posY-o_.Y() + t*dirY;
  double posOnPlaneZ = posZ-o_.Z() + t*dirZ;

  u = u_.X()*posOnPlaneX + u_.Y()*posOnPlaneY + u_.Z()*posOnPlaneZ;
  v = v_.X()*posOnPlaneX + v_.Y()*posOnPlaneY + v_.Z()*posOnPlaneZ;
}


void AMSPlane::rotate(double angle) {
  TVector3 normal = getNormal();
  u_.Rotate(angle, normal);
  v_.Rotate(angle, normal);

  sane();
}


void AMSPlane::reset() {
  o_.SetXYZ(0.,0.,0.);
  u_.SetXYZ(1.,0.,0.);
  v_.SetXYZ(0.,1.,0.);
}

//---------------------------------
AMSPlaneM::AMSPlaneM(int id, TkPlaneN *module): AMSPlane(),id_(id),module_(module){
  mstat_=0;
  m_.Set(0.,0.);
  mcov_.ResizeTo(TMatrixDSym(2));
  mcov_.Zero();
  malignder_.clear();
}

AMSPlaneM::AMSPlaneM(TVector3 o, TVector3 u, TVector3 v, int id, TkPlaneN *module): AMSPlane(o,u,v),id_(id),module_(module){
  mstat_=0;
  m_.Set(0.,0.);
  mcov_.ResizeTo(TMatrixDSym(2));
  mcov_.Zero();
  malignder_.clear();
}

AMSPlaneM::AMSPlaneM(TVector2 m, int id, int mstat, TkPlaneN *module): AMSPlane(),id_(id),module_(module),mstat_(mstat),m_(m){
  mcov_.ResizeTo(TMatrixDSym(2));
  mcov_.Zero();
  malignder_.clear();
}

AMSPlaneM::AMSPlaneM(TVector2 m, TMatrixDSym mcov, int id, int mstat, TkPlaneN *module): AMSPlane(),id_(id),module_(module),mstat_(mstat),m_(m),mcov_(mcov){
  malignder_.clear();
}


TMatrixDSym AMSPlaneM::getMCovGlobal() const{
  TMatrixDSym mcovg=mcov_;
  TMatrixD jac(3,2);
  const TVector3 &u=getU();
  const TVector3 &v=getV();
  jac(0,0) = u.X();
  jac(1,0) = u.Y();
  jac(2,0) = u.Z();
  jac(0,1) = v.X();
  jac(1,1) = v.Y();
  jac(2,1) = v.Z();
  mcovg.Similarity(jac);//mcovg=jac*mcov_*jac^T,32*22*23
  return mcovg;
}


TVector3 AMSPlaneM::getMPdist(bool isloc) const{
  TVector3 res=-getPLocal();
  res[0]+=getM().X(); 
  res[1]+=getM().Y(); 
  if(!isloc)toLab3(res,1);
  return res; 
}


TMatrixD AMSPlaneM::calMAlignDerGlobal(bool update,int id)
{ 
  TMatrixD rtos(3,3);
  rtos.Zero();
  const TVector3 &u=getU();
  const TVector3 &v=getV();
  TVector3 w=getNormal();
  for(int i=0;i<3;i++){//(e1,e2,e3)^{T}
    rtos(0,i)=u[i];
    rtos(1,i)=v[i];
    rtos(2,i)=w[i];
  }

  TMatrixD dmdg(3,6);
  dmdg.Zero();
  TVector3 m=getMGlobal();//measurement project in this rigidity body(du,dv,dw,alpha,beta,gamma)
  dmdg(0, 0)=1.; dmdg(0, 4)=-m[2]; dmdg(0, 5)= m[1];
  dmdg(1, 1)=1.; dmdg(1, 3)= m[2]; dmdg(1, 5)=-m[0];
  dmdg(2, 2)=1.; dmdg(2, 3)=-m[1]; dmdg(2, 4)= m[0];
  
  TMatrixD dmdgs(3,6);
  dmdgs=rtos*dmdg;//dmdg project back to the sensor
 
  if(update){
    std::vector<std::pair<int,TMatrixD> >::iterator it=malignder_.begin();
    for(;it!=malignder_.end();++it){if(it->first==id)break;}
    if(it==malignder_.end())malignder_.push_back(std::make_pair(id,dmdgs));
    else                   (*it)=std::make_pair(id,dmdgs);//id match using replacement
  }
  return dmdgs;
}


std::pair<std::vector<int>,TMatrixD> AMSPlaneM::getMAlignDerGlobal(){
  int nrow=0,ncol=0;
  for(std::vector<std::pair<int,TMatrixD> >::iterator it=malignder_.begin();it!=malignder_.end();++it){
    nrow =(it->second).GetNrows(); 
    ncol+=(it->second).GetNcols();
  }
  std::vector<int> glabels;
  if(nrow<=0||ncol<=0){return std::make_pair(glabels,TMatrixD());}

  TMatrixD galignders(nrow,ncol);
  galignders.Zero();
  int ips=0;
  for(std::vector<std::pair<int,TMatrixD> >::iterator it=malignder_.begin();it!=malignder_.end();++it){
    int detid=it->first;
    for(int ip=0;ip<it->second.GetNcols();ip++){
      int label=(detid>=0)?detid*10+(ip+1):detid*10-(ip+1);//du,dv,dw,alpha,beta,gamma
      if(label<0)label=10000000+abs(label);//negative ladder
      glabels.push_back(label);
      for(int im=0;im<it->second.GetNrows();im++)galignders(im,ips)=(it->second)(im,ip);
      ips++;
    }
  }
  return std::make_pair(glabels,galignders); 
}


std::pair<std::vector<int>,TMatrixD> AMSPlaneM::getResAlignDerGlobal(TVector3& dloc)
{
  std::pair<std::vector<int>,TMatrixD> malignders=getMAlignDerGlobal();
  if(malignders.first.size()==0)return malignders;
  TMatrixD Pv(2,3);
  Pv.Zero();
  Pv(0,0)=-1;Pv(0,2)=dloc[0]/dloc[2];//dudw
  Pv(1,1)=-1;Pv(1,2)=dloc[1]/dloc[2];//dvdw

  TMatrixD ralignders(2,malignders.second.GetNcols());
  ralignders=Pv*malignders.second;
  return std::make_pair(malignders.first,ralignders);
}


void AMSPlaneM::setMStat(int mstat)
{
  mstat_=mstat;
}


void AMSPlaneM::setM(const TVector2& m)
{
  m_ = m;
}

void AMSPlaneM::setM(double X,double Y)
{
  m_.Set(X,Y);
}

void AMSPlaneM::moveM(double X,double Y)
{
  X+=m_.X();
  Y+=m_.Y();
  m_.Set(X,Y);
}

void AMSPlaneM::setMCov(const TMatrixDSym& mcov)
{
  mcov_=mcov;
}

void AMSPlaneM::setMCov(int rown, int coln, double covn)
{
  mcov_[rown][coln]=covn;
}

//---------------------------------
TkPlaneN::TkPlaneN(): AMSPlane(){
  id_=type_=0;
  dim_[0]=dim_[1]=dim_[2]=0;
  mother=0;
}

TkPlaneN::TkPlaneN(int id,int type): AMSPlane(),id_(id),type_(type){
  dim_[0]=dim_[1]=dim_[2]=0;
  mother=0;
}

int TkPlaneN::getlayid(int tkid){
  int layid=abs(tkid/100);
  if     (layid==8)layid=1;
  else if(layid<8) layid=layid+1;
  layid--;
  return layid;
}

int TkPlaneN::convertlayid(int lay,int opt){
  int layid=-2;
  if(opt==0){//from layO to V6
    if     (lay==7)layid=0;//L1J 
    else if(lay>=0&&lay<=6)layid=lay+1;//L2-L8J
    else if(lay==8)layid=8;//L9J
  }
  else {//from V6 to layO
    if     (lay==0)layid=7;//L1J
    else if(lay>=1&&lay<=7)layid=lay-1;//L2-L8J
    else if(lay==8)layid=8;//L9J
  }
  return layid;
}

double TkPlaneN::getLocalDistToEdge(double lcoor[2][2],TVector3 gcoo,TVector3 gdir,int uxy){
  TVector2 loc=straightLineToPlane(gcoo,gdir);//minor linear interpolation to the local plane (local coordinate)
  double disxy[2];
  for(int ixy=0;ixy<2;ixy++){//rectangle
    double lcoo=(ixy==0)?loc.X():loc.Y();
    double disl=lcoo-lcoor[ixy][0];
    double dish=lcoo-lcoor[ixy][1];
    disxy[ixy]=(fabs(disl)<=fabs(dish))?fabs(disl):fabs(dish);//minimal absolute distance
    if((disl>0&&dish<0)||(disl<0&&dish>0))disxy[ixy]*=-1.;//inside volume, distance*=-1
  }
  if(uxy==0||uxy==1)return disxy[uxy];
  double dist=2.E100;
  if     (disxy[0]>=0&&disxy[1]>=0)dist=sqrt(disxy[0]*disxy[0]+disxy[1]*disxy[1]);//XY outside
  else if(disxy[0]>0 &&disxy[1]<=0)dist=disxy[0];//X outside 
  else if(disxy[0]<=0&&disxy[1]>0 )dist=disxy[1];//Y outside
  else                             dist=(disxy[0]>=disxy[1])?disxy[0]:disxy[1];//XY inside
  return dist;
}

void TkPlaneN::setr0(const TVector3& r0n)
{
  r0 = r0n;
}

void TkPlaneN::setRt(const TRotation& Rtn)
{
  Rt = Rtn;
}


void TkPlaneN::getRtar0a(TRotation &Rta,TVector3 &r0a,int ua){
  TVector3 dqa(0.,0.,0.);
  Rta=Rt;
  if(ua<0||(ua%10)>=1)   {dqa+=dq[0];Rta*=dR[0];}//<0||%10>=1 with alignment
  if(ua<0||(ua/10)%10>=1){dqa+=dq[1];Rta*=dR[1];}//<0||/10%10>=1 with additional alignment
  r0a=Rta*dqa+r0;
}

void TkPlaneN::cleardRdq(int ia) {
  dq[ia].SetXYZ(0.,0.,0.);dR[ia].SetToIdentity();
}


int TkPlaneN::updateOUVFromMother(int level){
  reset();
  int iu=0;
  TkPlaneN *pdet=this;
  while(pdet->mother){
    int up=2;
    if(level==0)break;
    if(level>0){
      if     ((level%10)==0){return -1;}//find 0 in middel
      else if((level%10)==1){up=1;}//without using alignment, level should be ..22 to apply full
      level=(level/10);
    }
    TRotation Rta; TVector3 r0a;
    pdet->getRtar0a(Rta,r0a,(up==1)?0:-1);
    moveOUV(Rta,r0a);
    iu++;
    pdet=pdet->mother;
  }
  return iu;
}
