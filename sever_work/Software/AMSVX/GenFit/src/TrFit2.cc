//  $Id: TrFit2.cc,v 1.23 2022/06/16 10:30:30 qyan Exp $



//////////////////////////////////////////////////////////////////////////
///
///\file  TrFit.C
///\brief Source file of TrFit class
///
///\date  2007/12/12 SH  First import (SimpleFit)
///\date  2007/12/14 SH  First import (tkfitg)
///\date  2007/12/20 SH  First stable version after a refinement
///\date  2007/12/20 SH  All the parameters are defined in double
///\date  2007/12/21 SH  New methods (LINEAR, CIRCLE) added, not yet tested
///\date  2008/01/20 SH  Imported to tkdev (test version)
///\date  2008/11/25 SH  Splitted into TrProp and TrFit
///\date  2008/12/02 SH  Fits methods debugged and checked
///\date  2010/03/03 SH  ChikanianFit added
///\date  2017/12/13 QY  GEANE Kalman (DAF) fitter added
///\date  2020/09/24 QY  General broken lines (Gbl) fitter added
///$Date: 2022/06/16 10:30:30 $
///
///$Revision: 1.23 $
///
//////////////////////////////////////////////////////////////////////////

#include "TrFit.h"
#include "TkDBc.h"
#include "MagField.h"
#include <cmath>
#include <iostream>     // for min and max
#include "commonsi.h"
#ifdef USEGENFIT
#include "TGeoManager.h"
#include "AbsTrackRep.h"
#include "DetPlane.h"
#include "KalmanFittedStateOnPlane.h"
#include "MeasurementOnPlane.h"
#include "AbsKalmanFitter.h"
#include "KalmanFitter.h"
#include "KalmanFitterRefTrack.h"
#include "KalmanFitterInfo.h"
#include "KalmanFitStatus.h"
#include "DAF.h"
#include "PlanarMeasurement.h"
#include "Track.h"
#include "TrackPoint.h"
#include "TGeoMaterialInterface.h"
#include "MaterialEffects.h"
#include "RKTrackRep.h"
#include "GFGbl.h"
#include "GblFitter.h"
#include "GblFitStatus.h"
#include "GblFitterInfo.h"
#include "HMatrixU.h"
#include "StateOnPlane.h"
#endif


void TrProp::InitKalman(){
#ifdef USEGENFIT
  //----material effects
    if(!genfit::MaterialEffects::getInstance()->isInitialized()){
if(!gGeoManager){
#pragma omp critical  (initkalman)
{
 char fname[500];
#ifdef VERSION6
    sprintf(fname, "%sams02tgeom.root",AMSDATADIR.amsdatadir);
//    sprintf(fname, "%sams02tgeom_limit.root",AMSDATADIR.amsdatadir);
#else
    sprintf(fname, "%sams02tgeom5.root",AMSDATADIR.amsdatadir);
#endif
    TGeoManager::Import(fname);
}
}
      genfit::AbsMaterialInterface *materialInterface=new genfit::TGeoMaterialInterface();
//    materialInterface->setDebugLvl(1);
    AddNavigator();

    genfit::MaterialEffects::getInstance()->init(materialInterface);
 genfit::MaterialEffects::getInstance()->setAllEffects(true);
     }
#endif
}


double TrProp::Interpolate(AMSPlane &pl, double &rigidity, int eloss, int direction){//QY

  rigidity=0;
#ifndef USEGENFIT
  return -4000;
#else
  if(_chrg==0||_mass==0||_rigidity==0)return -3000;
  InitKalman();
  if(!eloss){
    genfit::MaterialEffects::getInstance()->setNoEffects(true);
  }
  else { 
    genfit::MaterialEffects::getInstance()->setAllEffects(true);
    genfit::MaterialEffects::getInstance()->setNoiseCoulomb(false);//No MS
  }

  //---build rep, using rigidity to judge charge
  if(_rigidity<0)_chrg=-fabs(_chrg);//p=r*z>0, negative charged particle
  else           _chrg=fabs(_chrg);//p=r*z>0, positive charged particle
  if(direction<0)_chrg=-_chrg;//direction=-1: upward going
  genfit::AbsTrackRep* rep=0;
  if(fabs(_chrg)==1 && fabs(_mass-Melectron)<1.E-6)rep = new genfit::RKTrackRep(-11*_chrg);//pdgcode positron/electron (Brems special treatment)
  else                                             rep = new genfit::RKTrackRep(_mass,_chrg);

  //---set state to rep
  genfit::StateOnPlane kfpre(rep); 
  TVector3 pos(_p0x,_p0y,_p0z);
  TVector3 mom(-_dxdz,-_dydz,-1.);//downwards going
  mom.SetMag(fabs(_chrg*_rigidity));//p=r*z should be >0
  if(direction<0)mom*=-1;//direction=-1: upward going p=r*z<0
  rep->setTime(kfpre,0);
  rep->setPosMom(kfpre,pos,mom);

  //---extrapolation
  genfit::StateOnPlane kfsop(kfpre);
  double tracklen=0;
  try {
    genfit::SharedPlanePtr planep(new genfit::DetPlane(pl.getO(),pl.getU(),pl.getV()));
    tracklen=rep->extrapolateToPlane(kfsop,planep);
  }
  catch(genfit::Exception& e){
    rigidity=0;
    delete rep;
    return -2000;
  }
  
  TVector3 kfpos,kfdir;
  kfsop.getPosDir(kfpos,kfdir);
  pl.setP(kfpos);//global coordinate
  pl.setD(kfdir);//global direction
  const TVectorD& statep=kfsop.getState();
  rigidity=1./statep[0];
  delete rep; 
  return tracklen;
#endif  
}


TrFit::TrFit(void) : TrProp()
{
#ifdef USEGENFIT
  fitTrack=0;
  kalmanfitter=0;
  kalmanmethod=-1;
  gblfitter=0;
  gblmethod=-1; 
#endif
  Clear();
}

TrFit::~TrFit()
{
#ifdef USEGENFIT
  if(fitTrack)    {delete fitTrack;fitTrack=0;}
  if(kalmanfitter){delete kalmanfitter;kalmanfitter=0;}
  if(gblfitter)   {delete gblfitter;gblfitter=0;}
#endif
}

void TrFit::Clear()
{
  TrProp::Clear();
  _nhit = _nhitx = _nhity = _nhitxy = 0;
  for (int i = 0; i < PMAX; i++) _param[i] = 0;
  for (int i = 0; i < LMAX; i++){
    _xh[i] = _yh[i] = _zh[i] = 0;
    _xs[i] = _ys[i] = _zs[i] = 0;
    _xr[i] = _yr[i] = _zr[i] = 0;
    _bx[i] = _by[i] = _bz[i] = 0;
    _xchi2[i]=_ychi2[i]=_xkchi2[i]=_ykchi2[i]=_chiL[i]=0;  
  }
  _chisqx = _chisqy = _chisq = -1;
  _ndofx  = _ndofy  = _ndof  = 0;
  _errrinv = 0;
  _mscat = 0;
  _eloss = 0;
  _hits.clear();
#ifdef USEGENFIT
   if(fitTrack)    {delete fitTrack;fitTrack=0;}
   if(kalmanfitter){delete kalmanfitter;kalmanfitter=0;}
   kalmanmethod=-1;
   if(gblfitter)   {delete gblfitter;gblfitter=0;}
   gblmethod=-1;
#endif
}


int TrFit::BuildfitTrack(double mass,double charge,int seedfit,float fixrig){
#ifndef USEGENFIT
  return -4;
#else

  if(fitTrack){delete fitTrack; fitTrack=0;}

  //---inner to first measurement layer to get initial values
  TVector3 pos,mom; 
  if(seedfit>0){
    //----sort hits and initial seed by simplefit with inner-track first if successful otherwise with max-span
    double reti=SimpleFit(2);
    if(reti<0)return -20+(int)reti;
    AMSPoint pnt(0, 0, _zh[0]);
    AMSDir   dir(0, 0,  1);
    Interpolate(pnt, dir);
    if(dir[2]>0)dir=dir*(-1.);//downward particle
    if(fixrig!=0)_rigidity=fixrig;
    if(_rigidity<0)charge=-fabs(charge);//keep initial values symmetric for neg and pos,p=r*z>0
    else           charge=fabs(charge);//keep p=r*z>0
    double momv=fabs(_rigidity*charge);//p=r*z
//    cout<<" p0[3]="<<pnt[0]<<","<<pnt[1]<<","<<pnt[2]<<" dir="<<dir[0]<<","<<dir[1]<<","<<dir[2]<<" rig="<<_rigidity<<endl;
    pos.SetXYZ(pnt[0], pnt[1], pnt[2]);
    mom.SetXYZ(momv*dir[0],momv*dir[1],momv*dir[2]);
  }

  //----build track 
  genfit::AbsTrackRep* rep=0;
  if(fabs(charge)==1 && fabs(mass-Melectron)<1.E-6)rep = new genfit::RKTrackRep(-11*charge);//pdgcode positron/electron (Brems special treatment)
  else                                             rep = new genfit::RKTrackRep(mass,charge);
  if(seedfit>0){fitTrack=new genfit::Track(rep, pos, mom);}
  else         {fitTrack=new genfit::Track();fitTrack->addTrackRep(rep);}
 
  //----add Measurements
  int detId(0); // detector ID
  int planeId(0); // detector plane ID
  int hitId(0); // hit ID
  for(unsigned int i = 0; i <_hits.size(); i++){
    AMSPlaneM&   plm=_hits[i];
    const TVector2& m=plm.getM();
    const TMatrixDSym& mcov=plm.getMCov();
    int ixy=-1,nd=0;
    if     (mcov[0][0]>0&&mcov[1][1]>0){ixy=2; nd=2;}//2d
    else if(mcov[1][1]>0)              {ixy=1; nd=1;}//1dy
    else if(mcov[0][0]>0)              {ixy=0; nd=1;}//1dx
    else   {cerr<<"error hit noxy"<<endl;continue;}
    TVectorD hitCoords(nd);
    TMatrixDSym hitCov(nd);
    hitCov.UnitMatrix();
    if(ixy==2){hitCoords[0]=m.X();hitCoords[1]=m.Y();hitCov=mcov;}
    else      {hitCoords[0]=(ixy==0)?m.X():m.Y();hitCov[0][0]=mcov[ixy][ixy];}
    detId=plm.getid();
    genfit::PlanarMeasurement* measurement = new genfit::PlanarMeasurement(hitCoords, hitCov, detId, ++hitId, nullptr);
    if(ixy==1)measurement->setStripV();//set direction
    measurement->setPlane(genfit::SharedPlanePtr(new genfit::DetPlane(plm.getO(),plm.getU(),plm.getV())), ++planeId);
    measurement->setglobalMAlignDers(plm.getMAlignDerGlobal());//adding alignment module
    fitTrack->insertPoint(new genfit::TrackPoint(measurement, fitTrack));
  }

  return 0;
#endif
}


double TrFit::KalmanFit(double mass,double charge,int mscat,int eloss,int method){//QY
#ifndef USEGENFIT
  return -4;
#else

  if (_nhitx < 2 || _nhity < 3) return -1;

  //----fitter
  const int nIter = 20; // max number of iterations
  const double dPVal = 1.E-3; // convergence criterion
//  const double dFBWdPval = 1; // convergence criterion2 (not cut)
  const double dFBWdPval = 1.E-3; // convergence criterion2
//  const double dRigv = 1; // convergence criterion2 (not cut)
  const double dRigv = 1.E-2; //convergence criterion3
  bool  isDAF  = false;
  const genfit::eMultipleMeasurementHandling mmHandling = genfit::unweightedClosestToPredictionWire;
  if(kalmanfitter && method!=kalmanmethod){delete kalmanfitter; kalmanfitter=0; }//exist but not the same method
  if(!kalmanfitter){
//    kalmanfitter = new genfit::KalmanFitter(nIter, dPVal);//SimpleKalman
    if(method==GEANE_Kalman){
       kalmanfitter = new genfit::KalmanFitterRefTrack(nIter, dPVal);//RefKalman
       static_cast<genfit::KalmanFitterRefTrack*>(kalmanfitter)->setRefitAll(true);
    }
    else         {
      kalmanfitter = new genfit::DAF();//DafRef
      isDAF =  true;
    }
//    kalmanfitter = new genfit::DAF(false);//DafSimple
    kalmanfitter->setMultipleMeasurementHandling(mmHandling);
    kalmanfitter->setMaxIterations(nIter);
    kalmanfitter->setForwardBackwardDeltaPval(dFBWdPval);//convergence criterion 2: forward and backward fitting matching<dFBWdPval
    kalmanfitter->setDeltaRigv(dRigv);//d(1/R)/(1/R)<deltaRigv && d(e1/R)/e1/R<deltaRigv
//    kalmanfitter->setDebugLvl(2);
    kalmanmethod=method;
  }
  InitKalman(); 
  genfit::MaterialEffects::getInstance()->setAllEffects(true);
  if(!mscat&&!eloss)genfit::MaterialEffects::getInstance()->setNoEffects(true);//NoEffect
  else if(!mscat)   genfit::MaterialEffects::getInstance()->setNoiseCoulomb(false);//No MS
  else if(!eloss)  {
    genfit::MaterialEffects::getInstance()->setEnergyLossBetheBloch(false);//No dE/dx Loss
    genfit::MaterialEffects::getInstance()->setEnergyLossBrems(false);//No Brems Loss(e+/e-)
  }

  //-----build track
  int reti=BuildfitTrack(mass,charge,1);//sort hits and seed with simplefit
  if(reti<0)return reti;

  //-----fit track
  fitTrack->checkConsistency();
  kalmanfitter->processTrack(fitTrack);  
  fitTrack->checkConsistency();
//  fitTrack->getFittedState().Print();//print fit result

  //---Not converge
  genfit::KalmanFitStatus* status = fitTrack->getKalmanFitStatus();
  if(! status->isFitConverged()){delete fitTrack; fitTrack=0; _rigidity=0; return -2;}

  _chisq = (status->getBackwardNdf()>0) ? status->getBackwardChi2()/status->getBackwardNdf() : 0;//backward chi2/ndof
  _ndof = status->getBackwardNdf();

  //---chisqx+chisqy+measurements
  _chisqx =  0; _chisqy =  0;
  _ndofx  = -2; _ndofy  = -3;
  bool hasstat=0;
  for (size_t ii = 0; ii < fitTrack->getNumPointsWithMeasurement(); ++ii) {
    genfit::TrackPoint* tp = fitTrack->getPointWithMeasurementAndFitterInfo(ii);
    if(!tp->getFitterInfo())continue;
    genfit::KalmanFitterInfo* kfi = static_cast<genfit::KalmanFitterInfo*>(tp->getFitterInfo());
    genfit::KalmanFittedStateOnPlane *kfstat = kfi->getBackwardUpdate();//backward
    const TVectorD&  state0 = kfstat->getState();
    const TMatrixDSym& cov0 = kfstat->getCov();
    TVector3 kfpos,kfdir;
    kfstat->getPosDir(kfpos,kfdir);
    if(!hasstat){//treat the first measurment as inital state
      _rigidity=1./state0[0];//first Track-Plane Parameter Fitting 1/R
      _errrinv=sqrt(cov0[0][0]);//Fitting Error of 1/R
      _p0x=kfpos[0]; _p0y=kfpos[1];
      _p0z=kfpos[2];
      _dxdz=kfdir[0]/kfdir[2];
      _dydz=kfdir[1]/kfdir[2];
      hasstat=1;  
    }
    AMSPlaneM& plm=_hits[ii];
    plm.setP(kfpos);//global coordinate
    plm.setD(kfdir);//global direction
    _xchi2[ii]=kfstat->getChiSquareIncrementuv(0);//position+angle
    _ychi2[ii]=kfstat->getChiSquareIncrementuv(1);//position+angle
    _chisqx+=_xchi2[ii]; _ndofx+=kfstat->getNdfuv(0);
    _chisqy+=_ychi2[ii]; _ndofy+=kfstat->getNdfuv(1);
    //residuals in position
    genfit::MeasuredStateOnPlane respos;
    try {
      respos=kfi->getResidual(0,true,true);
    }catch(genfit::Exception& e){
      cerr<<"<<------Error Kalman backward and forward combined"<<endl;
      delete fitTrack; fitTrack=0; _rigidity=0; return -3; 
    }
    const TVectorD&  residual = respos.getState();
    const TMatrixDSym& rescov = respos.getCov();
    if(residual.GetNoElements()<2) {
      if (tp->getRawMeasurement(0)->constructHMatrix(kfi->getRep())->isEqual(genfit::HMatrixU())){
        _xr[ii]=residual(0);
      }
      else {
        _yr[ii]=residual(0);
      }
    }
    else {
      _xr[ii]=residual(0);
      _yr[ii]=residual(1);
    }
  }
  if(isDAF ){
    for(size_t ii = 0; ii < fitTrack->getNumPointsWithMeasurement(); ii++){
      _chiL[ii] = 0.5 - kalmanfitter->_chiL[ii] > 0 ?0:1   ; 
    }
  }
  

  return _chisq;
#endif

}

double TrFit::InterpolateKalman(AMSPlane &pl, double &rigidity, int ustate, bool ulength){

  rigidity=0;
#ifndef USEGENFIT
  return -4000; 
#else
  if(!fitTrack)return -1000;

  double tracklen=0;
  double zpl=pl.getO()[2];
  try {
    int direction=1;//1 forward(downward)/-1 backward(upward)
    double zp[2]={zpl,0};
    if(zp[0]<zp[1]){double temp=zp[0];zp[0]=zp[1];zp[1]=temp;}//zp[0]>zp[1]
    if(direction<0){double temp=zp[0];zp[0]=zp[1];zp[1]=temp;}//zp[0]<zp[1]
    int beid[2]={-1,-1};
    genfit::KalmanFitterInfo* kfipre=0;
    for (size_t ii = 0; ii <= fitTrack->getNumPointsWithMeasurement(); ++ii) {
      bool isend=(ii==fitTrack->getNumPointsWithMeasurement());
      int ui=isend? ii-1: ii;
      if(ui<0)break;
      genfit::TrackPoint* tp = fitTrack->getPointWithMeasurementAndFitterInfo(direction>0?ui:-ui-1);
      if(!tp->getFitterInfo())continue;
      genfit::KalmanFitterInfo* kfi = static_cast<genfit::KalmanFitterInfo*>(tp->getFitterInfo());
      const genfit::MeasuredStateOnPlane &smoothedState=kfi->getFittedState();//forward backward combined (smoothed)
      double zn=smoothedState.getPos()[2];
      for(int iz=0;iz<2;iz++){//begin and end
        if(beid[iz]>=0||(iz>=1&&beid[iz-1]<0))continue;
        int directionp=(zn>zp[iz])?1:-1;
        if(zn==zp[iz])directionp=direction;//special treatment
        if(isend || directionp!=direction){//start to set begin/end
          beid[iz]=ii;
          genfit::SharedPlanePtr planep(new genfit::DetPlane(pl.getO(),pl.getU(),pl.getV()));
          genfit::MeasuredStateOnPlane kfsop((directionp!=direction && kfipre)? kfipre->getFittedState(): smoothedState);//previous/now
          bool iszpl=(zp[iz]==zpl);//destination point required
          if(iszpl||ulength){
            double segmentlen=fitTrack->getCardinalRep()->extrapolateToPlane(kfsop, planep);
            if(iszpl){//point status, backward and forward average
              const TVectorD&  statep = kfsop.getState();
              TVector3 kfpos,kfdir;
              kfsop.getPosDir(kfpos,kfdir);
              pl.setP(kfpos);//global coordinate
              pl.setD(kfdir);//global direction
              rigidity=1./statep[0];
            }
            if(ulength){
              if(iz==0)tracklen-=segmentlen;//P0->M0
              else     tracklen+=segmentlen;//M1->P1
            }
          }
        }
      }
      if(beid[1]>=0)break;//find end
      if(beid[0]>=0&&kfipre){//middle
        genfit::MeasuredStateOnPlane kfsop(kfipre->getFittedState());//previous
        if(ulength){
          double segmentlen=fitTrack->getCardinalRep()->extrapolateToPlane(kfsop, kfi->getPlane());
          tracklen+=segmentlen;//M0->M1
        }
      }
      kfipre=kfi;
    }
    if(zpl>0)      tracklen*=-1;
    if(direction<0)tracklen*=-1;
    //---interpolate rigidity by last backward/forward state
    if(ustate==1||ustate==-1){
      genfit::TrackPoint* tp = fitTrack->getPointWithMeasurementAndFitterInfo(ustate>0? 0: -1);//backward or forward last
      genfit::KalmanFitterInfo* kfi = static_cast<genfit::KalmanFitterInfo*>(tp->getFitterInfo());
      genfit::KalmanFittedStateOnPlane kfsop(ustate>0? *kfi->getBackwardUpdate(): *kfi->getForwardUpdate());//backward or forward state
      genfit::SharedPlanePtr planep(new genfit::DetPlane(pl.getO(),pl.getU(),pl.getV())); 
      fitTrack->getCardinalRep()->extrapolateToPlane(kfsop, planep);//state extrapolate to plane
      const TVectorD&  statep = kfsop.getState();
      rigidity=1./statep[0];
    }
  }
  catch(genfit::Exception& e){
//    cerr << e.what();
//    cerr<<"<<------Error TrFit::InterpolateKalman track could not reach z="<<zpl;
//    cerr<<" rigidity="<<_rigidity<<endl;
    rigidity=0;
    return -2000;
  }

  return tracklen;
#endif

}


double TrFit::InterpolateKalman(const double zpl, AMSPoint &pnt, AMSDir &dir, double &rigidity, int ustate, bool ulength){
  AMSPlane pl(TVector3(0,0,zpl), TVector3(1,0,0), TVector3(0,1,0));//o,u,v
  double tracklen=InterpolateKalman(pl,rigidity,ustate,ulength);
  pnt.setp(pl.getP()[0],pl.getP()[1],pl.getP()[2]);
  dir.setd(pl.getD()[0],pl.getD()[1],pl.getD()[2]);
  if(dir[2]>0)dir=dir*(-1);//downwards
  return tracklen;
}


double TrFit::GblFit(double mass,double charge,int mscat,int eloss,int method,float fixrig,int uiter){//QY
#ifndef USEGENFIT
  return -4;
#else
  if (_nhitx < 2 || _nhity < 3) return -1;
 
  if(gblfitter){delete gblfitter; gblfitter=0; }//clean
  if(!gblfitter){
    if(method==GFGBL){
       gblfitter = new genfit::GFGbl();//
       if(!mscat)dynamic_cast<genfit::GFGbl*>(gblfitter)->setGBLOptions("THC",false,false);//No MS
    }
    else         {
       gblfitter = new genfit::GblFitter();//
       if(method==GBLFITTERAMS)dynamic_cast<genfit::GblFitter*>(gblfitter)->useAMSTune=1;//using AMS tuned by Q.Yan
       if(!mscat)dynamic_cast<genfit::GblFitter*>(gblfitter)->setMSOptions(false,false);//No MS
    }
    gblmethod=method;
  }
  InitKalman();//first use real geometry, has to remove
  if(method==GBLFITTERAMS)genfit::MaterialEffects::getInstance()->setNoEffects(true);
  else {
    genfit::MaterialEffects::getInstance()->setAllEffects(true);
    genfit::MaterialEffects::getInstance()->setEnergyLossBetheBloch(false);//No dE/dx Loss
    genfit::MaterialEffects::getInstance()->setEnergyLossBrems(false);//No Brems Loss(e+/e-)
  }

  //-----build track
  double prig=0;
  float fixrigms=0,fixrigmag=0;
  if(mscat==2){fixrigms=fixrig;}//fix MS weight in track fitting
  else        {if(fabs(fixrig)>1.e-10)fixrigmag=fixrig;}//fix rigidity fitting
  gblfitter->setFixMSRig(fixrigms);
  gblfitter->setRigFixFit(fixrigmag!=0?1:0);
  if(uiter<=0){
    int reti=BuildfitTrack(mass,charge,1,fixrigmag);//sort hits and seed with simplefit
    if(reti<0)return reti;
    prig=_rigidity;
  }
  else {//use result (seed) from previous fitting iteration
    if(_rigidity==0)return -3;//previous fit failed
    if(fixrigmag!=0)_rigidity=fixrigmag;//fix rigidity fitting 
    if(_rigidity<0)charge=-fabs(charge);//keep p=r*z>0
    else           charge=fabs(charge);//keep p=r*z>0
    int reti=BuildfitTrack(mass,charge,0);//without simplefit
    if(reti<0)return reti;
    TVector3 pos(_p0x,_p0y,_p0z);
    double momv=fabs(_rigidity*charge);//p=r*z, seed with previous GblFit
    AMSDir seeddir(_dxdz,_dydz,1);
    if(seeddir[2]>0)seeddir=seeddir*(-1);//downward going
//    cout<<"uiter="<<uiter<<","<<"p[3]="<<_p0x<<","<<_p0y<<","<<_p0z<<" dir="<<seeddir[0]<<","<<seeddir[1]<<","<<seeddir[2]<<" rig="<<_rigidity<<endl;
    TVector3 mom(momv*seeddir[0],momv*seeddir[1],momv*seeddir[2]);
    fitTrack->setStateSeed(pos,mom);
  }

  fitTrack->checkConsistency();
  gblfitter->processTrackWithRep(fitTrack,fitTrack->getTrackRep(0),0);
  fitTrack->checkConsistency();

  //-----Result
  genfit::GblFitStatus* status = dynamic_cast<genfit::GblFitStatus*>(fitTrack->getFitStatus());  
  if(!status||!status->isFitConverged()){delete fitTrack; fitTrack=0; _rigidity=0; return status?(-10-status->getNFailedPoints()):-10;}

  _chisq = (status->getNdf()>0) ? status->getChi2()/status->getNdf() : 0;//backward chi2/ndof
  _ndof = status->getNdf();

  //---Measurment on first plane
  _chisqx =  0; _chisqy =  0;
  _ndofx  = -2; _ndofy  = -3;
   bool hasstat=0; int ipm=0;
   for (unsigned int ip = 0; ip < fitTrack->getNumPoints(); ip++) {
     genfit::TrackPoint* tp=fitTrack->getPoint(ip);
     genfit::GblFitterInfo *gblfi= dynamic_cast<genfit::GblFitterInfo*>(tp->getFitterInfo());
     if(!gblfi)continue;
     TVector3 kfpos,kfdir[2];
     double pz=0;
     for(int iba=0;iba<2;iba++){
       const genfit::MeasuredStateOnPlane &kfsop=gblfi->getFittedState((iba==0)?false:true);//before kink and after kink
       const TVectorD&  state0 = kfsop.getState();
       const TMatrixDSym& cov0 = kfsop.getCov();
       kfsop.getPosDir(kfpos,kfdir[iba]);
       pz=kfpos[2];
       if(!hasstat){//the first layer&&before kink
          _rigidity=1./state0[0];//first Track-Plane Parameter Fitting 1/R
          _errrinv=sqrt(cov0[0][0]);//Fitting Error of 1/R
         _p0x=kfpos[0]; _p0y=kfpos[1];
         _p0z=kfpos[2];
         _dxdz=kfdir[iba][0]/kfdir[iba][2];
         _dydz=kfdir[iba][1]/kfdir[iba][2];
//          cout<<"ip2="<<ip<<" iba="<<iba<<" x,y,z="<<kfpos[0]<<","<<kfpos[1]<<","<<kfpos[2]<<" rig="<<_rigidity<<"+-"<<(_errrinv*_rigidity*_rigidity)<<" dx(dy)/dz="<<(kfdir[0]/kfdir[2])<<","<<(kfdir[1]/kfdir[2])<<endl;
         hasstat=1;
       }
     }
//---residuals in position
     if(tp->hasRawMeasurements()){
       AMSPlaneM& plm=_hits[ipm];
       plm.setP(kfpos);//global coordinate
       for(int iba=0;iba<2;iba++)plm.setD(kfdir[iba],iba);//global direction
       genfit::MeasuredStateOnPlane respos=gblfi->getResidual();
       const TVectorD&  residual = respos.getState();
       const TMatrixDSym& rescov = respos.getCov();
       if(residual.GetNoElements()<2) {
          if (tp->getRawMeasurement(0)->constructHMatrix(gblfi->getRep())->isEqual(genfit::HMatrixU())){
          _xr[ipm]=residual(0);
          _xchi2[ipm]=residual(0)*residual(0)/rescov[0][0];
          _chisqx+=_xchi2[ipm]; _ndofx++;
//          cout<<"ip="<<ip<<" pz="<<pz<<" chisx="<<(residual(0)*residual(0)/rescov[0][0])<<endl;
        }
        else {
          _yr[ipm]=residual(0);
          _ychi2[ipm]=residual(0)*residual(0)/rescov[0][0];
          _chisqy+=_ychi2[ipm]; _ndofy++;
//          cout<<"ip="<<ip<<" pz="<<pz<<" chisy="<<(residual(0)*residual(0)/rescov[0][0])<<endl;
         }
       }
       else {
        _xr[ipm]=residual(0);
        _yr[ipm]=residual(1);
        _xchi2[ipm]=residual(0)*residual(0)/rescov[0][0];
        _ychi2[ipm]=residual(1)*residual(1)/rescov[1][1];
        _chisqx+=_xchi2[ipm]; _ndofx++;
        _chisqy+=_ychi2[ipm]; _ndofy++;
//        cout<<"ip="<<ip<<" pz="<<pz<<" chisx="<<(residual(0)*residual(0)/rescov[0][0])<<endl;
//        cout<<"ip="<<ip<<" pz="<<pz<<" chisy="<<(residual(1)*residual(1)/rescov[1][1])<<endl; 
       }
       ipm++;//measurement point++
     }
//---rediduals in angle (kink): ndof do not need to be added, nparx=nscat, nanglex=nscat-2, nmeasx=NhitX, nmeasx+anglex-nparx=NhitX-2
     if(tp->hasThinScatterer()){//npary=nscat+1, nangley=nscat-2, nmeasy=NhitY, nmeasy+angley-npary=NhitY-3
       genfit::SharedPlanePtr planek(new genfit::DetPlane(TVector3(0,0,pz), TVector3(1,0,0), TVector3(0,1,0)));
       TMatrixDSym kinkCov=gblfi->getCovariance(tp->getMaterialInfo()->getMaterial().density,kfdir[0],planek);//new XYZ plane
       double kink[2]={0};
       for(int ixy=0;ixy<2;ixy++){kink[ixy]=atan(kfdir[1][ixy]/kfdir[1][2])-atan(kfdir[0][ixy]/kfdir[0][2]);}
       if(kinkCov[0][0]>0){
         double kchi2x=kink[0]*kink[0]/kinkCov[0][0];
         _xkchi2[ipm]+=kchi2x;
         _chisqx+=kchi2x;
       }
       if(kinkCov[1][1]>0){
         double kchi2y=kink[1]*kink[1]/kinkCov[1][1];
         _ykchi2[ipm]+=kchi2y;
         _chisqy+=kchi2y;
       }
//       cout<<"ip="<<ip<<" pz="<<pz<<" errx="<<sqrt(kinkCov[0][0])<<" kinkx="<<kink[0]<<" kchisx="<<(kink[0]*kink[0]/kinkCov[0][0])<<endl;
//       cout<<"ip="<<ip<<" pz="<<pz<<" erry="<<sqrt(kinkCov[1][1])<<" kinky="<<kink[1]<<" kchisy="<<(kink[1]*kink[1]/kinkCov[1][1])<<endl;
     }
  }

  //---do interations
  if(uiter<=0){
    int niter=(uiter==0)?20:abs(uiter);
    bool isrigconv=0;
    for(int iter=1;iter<niter+1;iter++){
//      cout<<"uiter="<<uiter<<" iter="<<iter<<" prig="<<prig<<" nrig="<<_rigidity<<" drig="<<(fabs(1-(prig/_rigidity)))<<endl;
      if(prig!=0&&(fabs(1-(prig/_rigidity))<1.e-2||fabs(1./prig-1./_rigidity)<1.e-5)){isrigconv=1; if(uiter==0)break;}//converged in rigidity,dR/R<1e-2||d|1/R|<1/100 TV-1
      if(iter==niter)break;
      prig=_rigidity;
      double chisq=GblFit(mass,charge,mscat,eloss,method,fixrig,iter+1);
      if(chisq<0){return chisq;};//do iterations/fittings using previous result as a seed
    }
    if(uiter==0&&!isrigconv){return -20;}//for uiter=0(default), require converge in rigidity
  }

  //-----
  return _chisq;
#endif
}

int TrFit::GblScanMat(double mass,double charge,double seedrig,double seedpos[3], double seeddir[3],double rad[20],double fds[20][2],double ftheta[20][2],double fscatmd[20][2]){//QY
#ifndef USEGENFIT
  return -4;
#else
  if (_nhitx < 2 || _nhity < 3) return -1;

  genfit::GblFitter *gblp=new genfit::GblFitter(); 
  InitKalman();//first use real geometry, has to remove
  genfit::MaterialEffects::getInstance()->setEnergyLossBetheBloch(false);//No dE/dx Loss
  genfit::MaterialEffects::getInstance()->setEnergyLossBrems(false);//No Brems Loss(e+/e-)

  //-----build track
  int reti=BuildfitTrack(mass,charge,0);//&&sort hits
  if(reti<0)return reti;

  //---seed track
  double momv=fabs(seedrig*charge);//p=r*z
  TVector3 pos(seedpos[0],seedpos[1],seedpos[2]);
  if(seeddir[2]>0){for(int id=0;id<3;id++)seeddir[id]*=-1;}//downward particle
  TVector3 mom(momv*seeddir[0],momv*seeddir[1],momv*seeddir[2]);
  fitTrack->setStateSeed(pos,mom);
  
  //----
  fitTrack->checkConsistency();
  gblp->scanMat(fitTrack,fitTrack->getTrackRep(0),rad,fds,ftheta,fscatmd);
  delete gblp;
  return 0;
#endif
}


double TrFit::InterpolateGbl(AMSPlane &pl, bool ulength){

#ifndef USEGENFIT
  return -4000;
#else
  if(!fitTrack)return -1000;
 
  double tracklen=0; 
  double zpl=pl.getO()[2];
  double zp[2]={zpl,0};
  if(zp[0]<zp[1]){double temp=zp[0];zp[0]=zp[1];zp[1]=temp;}//forward(downward)
  int beid[2]={-1,-1};
  genfit::GblFitterInfo* gblfipre=0;
  try {
    for (size_t ii = 0; ii <= fitTrack->getNumPoints(); ++ii) {
      bool isend=(ii==fitTrack->getNumPoints());
      int ui=isend? ii-1: ii;
      if(ui<0)break;
      genfit::GblFitterInfo * gblfi = dynamic_cast<genfit::GblFitterInfo*>(fitTrack->getPoint(ui)->getFitterInfo());
      if (!gblfi)continue;
      const genfit::MeasuredStateOnPlane &kfsop=gblfi->getFittedState(false);//before kink
      double zn=kfsop.getPos()[2];//current plane
      for(int iz=0;iz<2;iz++){//begin and end
        if(beid[iz]>=0||(iz>=1&&beid[iz-1]<0))continue;
        int directionp=(zn>zp[iz])?1:-1;
        if(isend||(directionp<0)){//start to set begin/end()
          beid[iz]=ii;
          genfit::SharedPlanePtr planep(new genfit::DetPlane(pl.getO(),pl.getU(),pl.getV()));
          genfit::MeasuredStateOnPlane kfsopc((directionp<0 && gblfipre)?gblfipre->getFittedState(true):gblfi->getFittedState(directionp<0?false:true));//before or after kink
          bool iszpl=(zp[iz]==zpl);//destination point required
          if(iszpl||ulength){
            double segmentlen=fitTrack->getCardinalRep()->extrapolateToPlane(kfsopc, planep);//top neighbour to point
            if(iszpl){
              TVector3 kfpos,kfdir;
              kfsopc.getPosDir(kfpos,kfdir);
              pl.setP(kfpos);//global coordinate
              pl.setD(kfdir);//global direction
            }
            if(ulength){
              if(iz==0)tracklen-=segmentlen;//P0->M0
              else     tracklen+=segmentlen;//M1->P1
            }
          }
        }
      }
      if(beid[1]>=0)break;//find end
      if(beid[0]>=0&&gblfipre){//middle
        genfit::MeasuredStateOnPlane kfsopre(gblfipre->getFittedState(true));//previous
        if(ulength){
          double segmentlen=fitTrack->getCardinalRep()->extrapolateToPlane(kfsopre, kfsop.getPlane());
          tracklen+=segmentlen;//M0->M1
        }
      }
      gblfipre=gblfi;
    }
    if(zpl>0)tracklen*=-1;
  }
  catch(genfit::Exception& e){
//    cerr << e.what();
//    cerr<<"<<------Error TrFit::InterpolateGbl track could not reach z="<<zpl;
//    cerr<<" rigidity="<<_rigidity<<endl;
    return -2000;
  }
  
  return tracklen;
#endif

}

double TrFit::InterpolateGbl(const double zpl, AMSPoint &pnt, AMSDir &dir,bool ulength){
  AMSPlane pl(TVector3(0,0,zpl), TVector3(1,0,0), TVector3(0,1,0));//o,u,v,
  double tracklen=InterpolateGbl(pl,ulength); 
  pnt.setp(pl.getP()[0],pl.getP()[1],pl.getP()[2]);
  dir.setd(pl.getD()[0],pl.getD()[1],pl.getD()[2]); 
  if(dir[2]>0)dir=dir*(-1);//downwards
  return tracklen;
}

