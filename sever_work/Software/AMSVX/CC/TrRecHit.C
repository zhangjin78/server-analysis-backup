//////////////////////////////////////////////////////////////////////////
///
///\file  TrRecHit.C
///\brief Source file of AMSTrRecHit class
///
///\date  2007/12/03 SH  First import (just a copy of trrec.C in Gbatch)
///\date  2007/12/28 SH  First refinement (for TkTrack)
///\date  2008/01/14 SH  First stable vertion after a refinement
///\date  2008/01/20 SH  Imported to tkdev (test version)
///\date  2008/02/19 AO  New data format 
///\date  2008/02/22 AO  Temporary clusters reference  
///\date  2008/02/26 AO  Local and global coordinate (TkCoo.h)
///\data  2008/03/06 AO  Changing some data members and methods
///\data  2008/04/12 AO  From XEta to XCofG(3) (better for inclination)
///\date  2017/04/27 QY  New position reconstruction algorithm
///\date  2022/02/07 QY  New V6 software
///
//////////////////////////////////////////////////////////////////////////

#include "TrRecHit.h"
#include "TkDBc.h"
#include "TrTasCluster.h"
#include "MagField.h"
#include "TMath.h"
#include "TrExtAlignDB.h"
#include "root.h"
#include "tkdcards.h"

ClassImp(TrRecHitR);

#include "VCon.h"

float TrRecHitR::GGpars[6]={1428., 0.0000,
			    0.1444, 1645.,
			    0.0109, 0.0972};
float TrRecHitR::GGintegral=91765.;


TrRecHitR::TrRecHitR(void) {
  hitcharge.Init();
  Clear();
}


TrRecHitR::TrRecHitR(const TrRecHitR& orig) {
  _tkid     = orig._tkid;     
  _clusterX = orig._clusterX;   
  _clusterY = orig._clusterY;  

  _iclusterX = orig._iclusterX;   
  _iclusterY = orig._iclusterY;  
  //  _corr     = orig._corr;
  //  _prob     = orig._prob;     
  Status   = orig.Status;
  _mult     = orig._mult;
  _imult    = orig._imult;
  _coord    = orig._coord;
  //  _bfield    = orig._bfield;
  _dummyX   = orig._dummyX;

  hitcharge=orig.hitcharge;
}


TrRecHitR::TrRecHitR(int tkid, TrClusterR* clX, TrClusterR* clY,  int imult, int status) {
  _tkid     = tkid;   
  if((clX&&clX->GetTkId()!=_tkid)|| (clY&&clY->GetTkId()!=_tkid)){
    printf("TrRecHitR::TrRecHitR--> BIG problems you are building ans hit on Ladder %d  \n",_tkid);
    printf("                                        with a cluster X from Ladder %d and \n",clX->GetTkId());
    printf("                                        with a cluster Y from Ladder %d     \n",clY->GetTkId());
  }
  if(clX&&clX->GetSide()!=0)
    printf("TrRecHitR::TrRecHitR--> BIG problems The cluster you passed as X is on Y!!!!!  \n");
  if(clY&&clY->GetSide()!=1)
    printf("TrRecHitR::TrRecHitR--> BIG problems The cluster you passed as Y is on X!!!!!  \n");

  Status    = status;
  _clusterX = clX;   
  _clusterY = clY;   
  if (Reblt()) {
    _iclusterX = -1;
    _iclusterY = -1;
  }
  else {
    VCon* cont2=GetVCon()->GetCont("AMSTrCluster");
    _iclusterX = (_clusterX) ? cont2->getindex(_clusterX) : -1;
    _iclusterY = (_clusterY) ? cont2->getindex(_clusterY) : -1;
    delete cont2;
  }

  if (!clX) Status |= YONLY;
  if (!clY) Status |= XONLY;
  _dummyX   = 0;
  int xaddr =  640;
  if(clX!=0)xaddr =  clX->GetAddress(); // maybe better to use clX->GetSeedAddress();
	  
  _mult = (TasHit()) ? 1 : TkCoo::GetMaxMult(GetTkId(), xaddr)+1;

  // coordinate construction
  _imult    = imult; 
  _coord=GetGlobalCoordinate(_imult);	

  hitcharge.Init();
}


const AMSPoint TrRecHitR::HitPointDist(const AMSPoint& aa,int& mult){
  int mm=GetMultiplicity();
  float max=999999.;
  mult=-1;
  for (int ii=0;ii<mm;ii++){
    AMSPoint cc = aa - GetCoord(ii);
    if(fabs(cc[0])<max){
      mult=ii;
      max=fabs(cc[0]);
    }
  }
  
  if (mult>-1&& mult <mm)
    return aa-GetCoord(mult);
  else
    return aa-GetCoord(0); 
}


float TrRecHitR::HitDist(TrRecHitR& B,int coo){
  if(coo==1 || coo==2){
    return (this->GetCoord()-B.GetCoord())[coo];
  }
  else if(coo==0){
    int MminA=0;
    int MminB=0;
    int MmaxA=this->GetMultiplicity();
    int MmaxB=B.GetMultiplicity();
    
    if(this->GetResolvedMultiplicity()>=0){
      MminA=this->GetResolvedMultiplicity();
      MmaxA=MminA+1;
    }
    
    if(B.GetResolvedMultiplicity()>=0){
      MminB=B.GetResolvedMultiplicity();
      MmaxB=MminB+1;
    }
    float dmin=1000;
    float mind=1000;
    for (int la=MminA;la <MmaxA;la++)
      for (int lb=MminB;lb <MmaxB;lb++){
	float dist=(this->GetCoord(la)-B.GetCoord(lb))[0];
	if(fabs(dist) < dmin) {dmin=fabs(dist) ; mind=dist;}
    }
    return mind;
  } else 
    return -1;
}


TrClusterR* TrRecHitR::GetXCluster() { 
  if(_clusterX==NULL&& !(Status&YONLY)){
    VCon* cont2=GetVCon()->GetCont("AMSTrCluster");
    _clusterX = (TrClusterR*)cont2->getelem(_iclusterX);
    delete cont2;
  }
  return _clusterX;
}


TrClusterR* TrRecHitR::GetYCluster() { 
  if(_clusterY==NULL&& !(Status&XONLY)){
    VCon* cont2=GetVCon()->GetCont("AMSTrCluster");
    _clusterY = (TrClusterR*)cont2->getelem(_iclusterY);
    delete cont2;
  }
  return _clusterY;
}

//void TrRecHitR::BuildCoordinates() {
//  // coordinate construction
//  TrClusterR* clX= GetXCluster();
//  int xaddr =  640;
//  if(clX!=0)
//    xaddr =  clX->GetAddress();
//  else if(_dummyX>=-0.5)
//    xaddr += _dummyX;
//  
//  _mult = (TasHit()) ? 1 : TkCoo::GetMaxMult(GetTkId(), xaddr)+1;
//  _coord.clear();
//  for (int imult=0; imult<_mult; imult++) _coord.push_back(GetGlobalCoordinate(imult));
//
////   for (int ii=0;ii<_coord.size();ii++){
////     float x[3],b[3];
////     x[0]=_coord[ii].x();
////     x[1]=_coord[ii].y();
////     x[2]=_coord[ii].z();
////       GUFLD(x,b);
////     _bfield.push_back(AMSPoint(b));
////   }
//
//
//  if (TasHit()) {
//    if (!GetXCluster() || !GetXCluster()->TasCls() || 
//        !GetYCluster() || !GetYCluster()->TasCls()) Status &= ~TASHIT;
//  }
//}

TrRecHitR::~TrRecHitR() {
  Clear();
}


void TrRecHitR::Clear() {
  _tkid     = 0; 
  _clusterX = 0;
  _clusterY = 0;
  _iclusterX = -1;
  _iclusterY = -1;
  Status   = 0;
  _mult     = 0;
  _imult    = -1; 
  _dummyX   = 0;
  _coord=AMSPoint(0,0,0);
  //  _bfield.clear();
}


void TrRecHitR::Print(int opt){
  cout << _PrepareOutput(opt);
}


std::string TrRecHitR::_PrepareOutput(int opt) { 
  std::string sout;
  if(_imult>0) 
    sout.append(Form("tkid: %+03d Right Coo %d (x,y,z)=(%10.4f,%10.4f,%10.4f)  AmpY: %8.2f  AmpX: %8.2f  Prob: %8.5f  Status: %4d  QStatus: %8X\n",
		     _tkid,_imult,GetCoord(_imult).x(),GetCoord(_imult).y(),GetCoord(_imult).z(),
		     (GetYCluster()) ? GetYCluster()->GetTotSignal(TrClusterR::DefaultCorrOpt) : 0,
		     (GetXCluster()) ? GetXCluster()->GetTotSignal(TrClusterR::DefaultCorrOpt) : 0,
		     GetCorrelationProb(),getstatus(),GetQStatus()));
  else 
    sout.append(Form("tkid: %+03d Base  Coo 0 (x,y,z)=(%10.4f,%10.4f,%10.4f)  AmpY: %8.2f  AmpX: %8.2f  Prob: %8.5f  Status: %4d  QStatus: %8X\n",
		     _tkid,GetCoord(0).x(),GetCoord(0).y(),GetCoord(0).z(),
                     (GetYCluster()) ? GetYCluster()->GetTotSignal(TrClusterR::DefaultCorrOpt) : 0,
                     (GetXCluster()) ? GetXCluster()->GetTotSignal(TrClusterR::DefaultCorrOpt) : 0,
		     GetCorrelationProb(),getstatus(),GetQStatus()));
  if(!opt) return sout;
  for(int ii=0;ii<_mult;ii++)
    sout.append(Form("mult %d (x,y,z)=(%10.4f,%10.4f,%10.4f)\n",
		     ii,GetCoord(ii).x(),GetCoord(ii).y(),GetCoord(ii).z()));
  return sout;
}


const char *  TrRecHitR::Info(int iRef){
  string aa;
  aa.append(Form("TrRecHit #%d ",iRef));
  aa.append(_PrepareOutput(0));
  unsigned int len=MAXINFOSIZE;
  if(aa.size()<len) len=aa.size();
  strncpy(_Info,aa.c_str(),len+1);
  return _Info;
}


std::ostream &TrRecHitR::putout(std::ostream &ostr)  {
  return ostr << _PrepareOutput(1) << std::endl; 
}


float TrRecHitR::GetXloc(int imult, int nstrips, int opt) {
  TrClusterR *cls = GetXCluster();
  int idummyX=(_dummyX>=383.5)?383:int(_dummyX+0.5);//V6 protection due to limited precision of float
  if (!cls) return TkCoo::GetLocalCoo(_tkid,idummyX+640,imult);
  if (TasHit()) return ((TrTasClusterR *)cls)->GetXCofGTas();
  return cls->GetXCofG(nstrips, imult, opt);
}


float TrRecHitR::GetYloc(int nstrips, int opt) { 
  TrClusterR *cls = GetYCluster();
  if (!cls) return -1000;
  if (TasHit()) return ((TrTasClusterR *)cls)->GetXCofGTas();
  return cls->GetXCofG(nstrips, 0, opt);
}


void TrRecHitR::SetTkCand() {
  setstatus(TKCAND);
  TrClusterR *clx=GetXCluster();
  TrClusterR *cly=GetYCluster();
  if(clx)clx->SetTkCand();
  if(cly)cly->SetTkCand();
}


void TrRecHitR::ClearTkCand() {
  clearstatus(TKCAND);
  TrClusterR *clx=GetXCluster();
  TrClusterR *cly=GetYCluster();
  if(clx)clx->ClearTkCand();
  if(cly)cly->ClearTkCand();
}


void TrRecHitR::SetUsed() {
  setstatus(AMSDBc::USED);
  TrClusterR *clx=GetXCluster();
  TrClusterR *cly=GetYCluster();
  if(clx)clx->SetUsed();
  if(cly)cly->SetUsed();
}


void TrRecHitR::ClearUsed(int opt) {
  clearstatus(AMSDBc::USED);
  TrClusterR *clx=GetXCluster();
  TrClusterR *cly=GetYCluster();
  if(clx)clx->ClearUsed(opt);
  if(cly)cly->ClearUsed(opt);
}

// #include "root.h"
// #include "DynAlignment.h"
// void     TrRecHitR::BuildCoordinateDynExt(){
//   if(GetLayerJ()!=1 && GetLayerJ()!=9) {BuildCoordinate();return;}  // Apply the patch only for layer 1 and 9
//   if(_imult<0) return;
  
//   double newcoor[3]={0,0,0};
//   if(DynAlManager::FindAlignment(*AMSEventR::Head(),*this,newcoor[0],newcoor[1],newcoor[2])){
//     for(int i=0;i<3;i++) _coord[i]=newcoor[i];
//   }else{
//     cout<<"TrRecHitR::BuildCoordinateDynExt-W-DynAlManager::FindAlignment did not find an alignment for run "<<AMSEventR::Head()->fHeader.Run<<" hit at layer "<<this->GetLayerJ()<<endl;
//   }
// }


AMSPoint TrRecHitR::GetGlobalCoordinate(int imult, const char* options,
					int nstripsx, int nstripsy, int optx, int opty) {
  // parsing options
  bool ApplyAlignement = false;
  bool ApplyCiemat = false;
  bool NoExtAlignment = false;
  char character = ' ';
  int  cc = 0;
  while (character!='\0') {
    character = *(options+cc);
    if ( (character=='Z')||(character=='z') ) NoExtAlignment= true;
    if ( (character=='A')||(character=='a') ) ApplyAlignement = true;
    if ( (character=='M')||(character=='m') ) ApplyCiemat = true;
    cc++;
  }
  // calculation
  AMSPoint loc = GetLocalCoordinate(imult, nstripsx, nstripsy, optx, opty);
  if(ApplyCiemat){
    if (GetLayer()==8)
      UpdateExtLayer(1,GetSlotSide()*10+lad()*100,-1);
    else if(GetLayer()==9)
      UpdateExtLayer(1,-1,GetSlotSide()*10+lad()*100);
    TrExtAlignDB::SetAlKind(1);
  }else if(NoExtAlignment){
    TrExtAlignDB::SetAlKind(2);
  }else
    TrExtAlignDB::SetAlKind(0);

  AMSPoint glo;
  if (!ApplyAlignement) {
    glo = TkCoo::GetGlobalN(GetTkId(),loc);
  }
  else {
    if( (GetLayer()==8)|| (GetLayer()==9))UpdateExtLayer(0);
    glo = TkCoo::GetGlobalA(GetTkId(),loc);
    if (TasHit()) glo = glo-TrTasClusterR::Align(GetXCluster(), GetYCluster());
  }
  TrExtAlignDB::SetAlKind(0);
  return glo;
}


TkLadderN *TrRecHitR::GetLadder(double ladchan[2], int isen, float chrg, float dxdz, float dydz, double xaddress){
  int imult=isen;
  if(isen==-1){imult=GetResolvedMultiplicity();if(xaddress<0)xaddress=double(_dummyX)+640;}
  TrClusterR *clxy[2]={GetXCluster(),GetYCluster()};
  ladchan[0]=ladchan[1]=-10;
  TkLadderN *ladder=GetLadder();
  for(int ixy=0;ixy<2;ixy++){
    if(clxy[ixy])clxy[ixy]->GetLadder(ladchan[ixy],imult,-1,chrg,dxdz,dydz);//get ladchan for  X or Y Cluster
    else if(ixy==0){
      if(xaddress>=0)ladchan[ixy]=ladder->GetLadderChanX(xaddress,imult);//sensor channel
      else {
        TkSensorN *sensor=ladder->pSensor(isen);
        ladchan[ixy]=sensor?sensor->GetInLadderChanX(-10):-1;//sensor center strip at X or fatal error
      }
    }
  }
  return ladder;
}


int TrRecHitR::nSensor(double xaddress){
  TrClusterR *clx=GetXCluster();
  if(clx)return clx->nSensor(-1,xaddress);//xaddress is useless for XCluster
  TrClusterR *cly=GetYCluster();
  return cly->nSensor(-1,xaddress);
}


TkSensorN *TrRecHitR::GetSensor(double senschan[2], int isen, float chrg, float dxdz, float dydz, double xaddress){
  int imult=isen;
  if(isen==-1){imult=GetResolvedMultiplicity();if(xaddress<0)xaddress=double(_dummyX)+640;}
  TrClusterR *clxy[2]={GetXCluster(),GetYCluster()};
  senschan[0]=senschan[1]=-10;
  TkSensorN *sensor[2]={0};
  for(int ixy=0;ixy<2;ixy++){
    if(clxy[ixy])sensor[ixy]=clxy[ixy]->GetSensor(senschan[ixy],imult,-1,chrg,dxdz,dydz,xaddress);//get ladchan for  X or Y Cluster
    else if(ixy==0&&xaddress>=0){
      TkLadderN *ladder=GetLadder();
      double ladchanx=ladder->GetLadderChanX(xaddress,imult);//ladchan for seed strip
      ladder->GetSensor(ladchanx,senschan[ixy]);//sensor channel
    } 
  }
  return clxy[0]?sensor[0]:sensor[1];
}


AMSPlaneM TrRecHitR::GetGCoordN(int isen, int ualign, bool dzshiftondxy, float chrg, float dxdz, float dydz, double xaddress) {
  int imult=isen;
  if(isen==-1){imult=GetResolvedMultiplicity();if(xaddress<0)xaddress=double(_dummyX)+640;}
  TrClusterR *clxy[2]={GetXCluster(),GetYCluster()};
  double ladchan[2]={-10,-10};//ladder center
  TkLadderN *ladder=GetLadder(ladchan,imult,chrg,dxdz,dydz,xaddress);
  if(imult>0&&ladchan[0]>ladder->nchans[0]-0.5){//try one more again with X side ladder channel overflow protection
    imult--;
    ladder=GetLadder(ladchan,imult,chrg,dxdz,dydz,xaddress);
  }
  else if(imult==0&&ladchan[0]<-0.5){//try one more again with X side ladder channel underfow protection
    imult++;
    ladder=GetLadder(ladchan,imult,chrg,dxdz,dydz,xaddress);
  }
  //-----
  AMSPlaneM plm=GetTkTrack()->GetGlobalCoo(GetTkId(),ladchan[0],ladchan[1],ualign);
  int nm=0;
  for(int ixy=0;ixy<2;ixy++){if(!clxy[ixy])plm.setMCov(ixy,ixy,0.);if((plm.getMCov())[ixy][ixy]>0)nm++;}
  if(nm==0)plm.setMStat(-11);//set fatal error if neither X nor Y measurement
  if(plm.getMStat()<=-11)return plm;//fatal error
  // Apply Smearing or Misalignment for MC
  if(TRMCFFKEY.MCtuneDmax>0){
    TVector2 ms=plm.getM();
    if(TRMCFFKEY.MCtuneDs[0]!=0||TRMCFFKEY.MCtuneDs[1]!=0){
      AMSPlaneM pls=plm;//Intrinsic spatial resolution tunning
      for(int ixy=0;ixy<2;ixy++){
        if((plm.getMCov())[ixy][ixy]>0)MCtune(pls,GetTkId(),TRMCFFKEY.MCtuneDmax,TRMCFFKEY.MCtuneDs[ixy],ixy);//local X or Y
      }
      ms+=(pls.getM()-plm.getM());
    }
    int jlayer=GetLayerJ();
    int il=(jlayer==1)?0:(jlayer==9?1:-1);
    if(il>=0&&(TRMCFFKEY.OuterSmearingN[il][0]!=0||TRMCFFKEY.OuterSmearingN[il][1]!=0)){
      AMSPlaneM pls=plm;//External layer misalignment
      MCSmearExtAlign(pls,jlayer-1,TRMCFFKEY.MCtuneDmax,2,TRMCFFKEY.OuterSmearingN[il],0);//opt=1 is more correct
      ms+=(pls.getM()-plm.getM());
    }
    plm.setM(ms);
  }
  // Apply Zshift correction
  if(dzshiftondxy){
    double dzondxy[2]={0};
    for(int ixy=0;ixy<2;ixy++){
      if((plm.getMCov())[ixy][ixy]>0&&clxy[ixy])dzondxy[ixy]=clxy[ixy]->GetdZShiftondXY(chrg,dxdz,dydz,3);
    } 
    if(TRFITFFKEY.Zshift>=0)plm.moveM(dzondxy[0],dzondxy[1]);//XYlocal move
  }
  return plm;
}


AMSPlaneM TrRecHitR::FindMult(double &dist, int &mult, double &xaddress, TVector3 gcoo, TVector3 gdir, int ii, float chrg){
  TrClusterR *clxy[2]={GetXCluster(),GetYCluster()};
  if(clxy[0]&&clxy[1]){
    int iixy[2]={ii,ii};
    double daqchan[2]={-2,-2};
    for(int ixy=0;ixy<2;ixy++){
      if(ii==-1||ii==-2)iixy[ixy]=clxy[ixy]->GetXCofG(TrClusterR::DefaultUsedStrips,0,TrClusterR::DefaultBestResidualOpt,0,-2,-2,3);//strip index of seed strip
      daqchan[ixy]=clxy[ixy]->GetAddress(iixy[ixy]);
    }
    TkLadderN *ladder=GetLadder();
    TkSensorN *sensor=ladder->FindSensor(dist,gcoo,gdir,1,daqchan[0],daqchan[1]);//Find the closest sensor
    TVector2 ploc=sensor->straightLineToPlane(gcoo,gdir);//trajectory local coordinate in sensor
    xaddress=daqchan[0];
    double chan[2]={-1,daqchan[1]};
    chan[0]=sensor->GetSensorChanXFromDAQ(daqchan[0],mult);//chanx measurement and the resolved multiplicity
    int multseed_index=clxy[0]->GetMultSeedIndex();//multiplicity-seed is defined by old cofg algorithm with tracker finding
    if(multseed_index<iixy[0]&&clxy[0]->GetAddress(multseed_index)>daqchan[0]){//mult is defined by multseed_index, multseed on left
      if(mult==0){//deal with multiplicity underflow
        mult++;//manually increase multiplicity
        double ladchanx=ladder->GetLadderChanX(daqchan[0],mult);//new ladchanx
        sensor=ladder->GetSensor(ladchanx,chan[0]);//new sensor and chanx measurement
        ploc=sensor->straightLineToPlane(gcoo,gdir);//new trajectory local coordinate in sensor
      }
      mult--;
    }
    else if(multseed_index>iixy[0]&&clxy[0]->GetAddress(multseed_index)<daqchan[0])mult++;//mult is defined by multseed_index, multseed on right
    AMSPlaneM plm=sensor->GetLocalCoo(chan[0],chan[1]);//the closest possible measurement
    plm.set(sensor->getO(),sensor->getU(),sensor->getV());
    plm.setD(gdir);
    if(ii==-1){
      TVector3 dloc=plm.getDLocal();
      float dxdz=(dloc[2]!=0)?dloc[0]/dloc[2]:0;
      float dydz=(dloc[2]!=0)?dloc[1]/dloc[2]:0;
      plm=GetGCoordN(mult,111,1,chrg,dxdz,dydz,xaddress);
      plm.setD(gdir);
      ploc=plm.straightLineToPlane(gcoo,gdir);
    }
    plm.setPLocal(TVector3(ploc.X(),ploc.Y(),0.));//track trajectory position in sensor
    dist=plm.getMPdist(1).Mag();//distance of the residual
    return plm;
  }
  else if(clxy[0]){//should never happen
    return clxy[0]->FindMult(dist,mult,xaddress,gcoo,gdir,ii,chrg);
  }
  else {
    return clxy[1]->FindMult(dist,mult,xaddress,gcoo,gdir,ii,chrg);
  }
}


void TrRecHitR::SetiTrCluster(int iclsx, int iclsy)
{
  VCon *cont = GetVCon()->GetCont("AMSTrCluster");

  _iclusterX = iclsx;
  _iclusterY = iclsy;
  _clusterX  = (!OnlyY()) ? (TrClusterR *)cont->getelem(iclsx) : 0;
  _clusterY  = (!OnlyX()) ? (TrClusterR *)cont->getelem(iclsy) : 0;

  delete cont;
}


float TrRecHitR::GetCorrelation()   {
  if (!GetXCluster()) return -1.;
  if (!GetYCluster()) return 1.;
  float n = GetXCluster()->GetTotSignal();
  float p = GetYCluster()->GetTotSignal();
  return (p - n)/(p + n);
}


float TrRecHitR::GetProb()   {
  float correlation = GetCorrelation();
  return ( GGpars[0]*TMath::Gaus(correlation,GGpars[1],GGpars[2],kFALSE) +
           GGpars[3]*TMath::Gaus(correlation,GGpars[4],GGpars[5],kFALSE) ) / GGintegral;
}


float TrRecHitR::GetSignalCombination(int iside, int opt, float beta, float rigidity, float mass_on_Z, int res_mult, float dxdz, float dydz) {
  int mult = GetResolvedMultiplicity();
  if(res_mult!=-2)mult=res_mult;
  TrClusterR* clx = GetXCluster();
  TrClusterR* cly = GetYCluster();
  if (iside == 0 || (iside == 2 && clx)) {
    return (clx!=0) ? clx->GetTotSignal(opt,beta,rigidity,mass_on_Z,mult,dxdz,dydz) : 0;
  }
  else if (iside == 1 || (iside == 2 && !clx)) {
    return (cly!=0) ? cly->GetTotSignal(opt,beta,rigidity,mass_on_Z,mult,dxdz,dydz) : 0;
  }
  else if (iside == 3) {
    float sig_x = (clx!=0) ? clx->GetTotSignal(opt,beta,rigidity,mass_on_Z,mult,dxdz,dydz) : 0;
    float sig_y = (cly!=0) ? cly->GetTotSignal(opt,beta,rigidity,mass_on_Z,mult,dxdz,dydz) : 0;
    float wei_x = 1; // to be tuned
    float wei_y = 1; // to be tuned
    return (sig_x*wei_x + sig_y*wei_y)/(wei_x + wei_y);
  }
  else if (iside == 4) {
    float sig_x = (clx!=0) ? clx->GetTotSignal(opt,beta,rigidity,mass_on_Z,mult,dxdz,dydz) : 0;
    float sig_y = (cly!=0) ? cly->GetTotSignal(opt,beta,rigidity,mass_on_Z,mult,dxdz,dydz) : 0;
    return (sig_x + sig_y)/2.;
  }
  return 0.;
}

TrQYJHit TrRecHitR::GetQYJ_all(int iside, float beta, float rigidity, int res_mult, float dxdz, float dydz, float qgain, int qopt){
  int mult = GetResolvedMultiplicity();
  if(res_mult>=0)mult=res_mult;
  TrClusterR* clx = GetXCluster();
  TrClusterR* cly = GetYCluster();

  float beta0=1.0;
  float rigidity0=0.0;
  float xcof=0.0;
  TrQYJCluster yjclx0=(clx==0)? TrQYJCluster(): clx->GetQYJ_all(beta0,rigidity0,mult,dxdz,dydz,xcof,qgain,qopt);
  xcof=yjclx0._tkcof[0];
  if(qgain<0&&((iside==1)||(iside==2&&!clx))){//Ycharge||XYcharge without XCluster (Y is not independent from X)
    if(yjclx0.Q>0)qgain=yjclx0.Q;//with XCluster: use X to linearize Y
    else {//without XCluster: use Y_qs[4] to linearize Y
      TrQYJCluster yjcly0=(cly==0)? TrQYJCluster(): cly->GetQYJ_all(beta0,rigidity0,mult,dxdz,dydz,qgain,qopt);
      qgain=yjcly0._qs[4];
    }
  }

  TrQYJHit yjhit;
  int nc=(qgain>=0)?1:3;
  for(int ic=0;ic<nc;ic++){//few iteration
    float beta0=(ic<nc-1)?1.0:beta;
    float rigidity0=(ic<nc-1)?0.0:rigidity;
    TrQYJCluster yjclx=(clx==0||iside==1)? TrQYJCluster(): clx->GetQYJ_all(beta0,rigidity0,mult,dxdz,dydz,xcof,qgain,qopt);
    TrQYJCluster yjcly=(cly==0||iside==0)? TrQYJCluster(): cly->GetQYJ_all(beta0,rigidity0,mult,dxdz,dydz,xcof,qgain,qopt);
    yjhit.SetCluster(yjclx,yjcly);
    if(yjclx.Q<=0&&yjcly.Q<=0)break;
    qgain=(ic<nc-1&&yjclx.Q>0)?yjclx.Q:yjhit.Q;
  }
  return yjhit; 
}


float TrRecHitR::GetQYJ(int iside, float beta, float rigidity, int res_mult, float dxdz, float dydz, float qgain, int qopt){
  return GetQYJ_all(iside,beta,rigidity,res_mult,dxdz,dydz,qgain,qopt).Q;
}

int TrRecHitR::GetQStatusYJ(int nstrip_from_seed) {
  int Sx = (GetXCluster()) ? GetXCluster()->GetQStatusYJ(nstrip_from_seed,GetResolvedMultiplicity()) : 0;
  int Sy = (GetYCluster()) ? GetYCluster()->GetQStatusYJ(nstrip_from_seed,GetResolvedMultiplicity()) : 0;
  return (Sy<<12)|Sx;
}

int TrRecHitR::GetQStatus(int nstrip_from_seed) { 
  int Sx = (GetXCluster()) ? GetXCluster()->GetQStatus(nstrip_from_seed,GetResolvedMultiplicity()) : 0;
  int Sy = (GetYCluster()) ? GetYCluster()->GetQStatus(nstrip_from_seed,GetResolvedMultiplicity()) : 0; 
   return (Sy<<12)|Sx;
}

float TrRecHitR::GetSignalDifference() {
  TrClusterR* clx = GetXCluster();
  TrClusterR* cly = GetYCluster();
  if ( (clx!=0)&&(cly!=0) ) {
    float sig_x = clx->GetTotSignal(TrClusterR::kAsym|TrClusterR::kGain|TrClusterR::kLoss|TrClusterR::kPN);
    float sig_y = cly->GetTotSignal(TrClusterR::kAsym|TrClusterR::kGain|TrClusterR::kLoss|TrClusterR::kPN);
    return sig_x - sig_y;
  }
  else if (clx!=0) return -10000;
  else if (cly!=0) return  10000;
  return -1000000;
}

float TrRecHitR::GetQH(int iside, float beta, float rigidity, int res_mult, float dxdz, float dydz, float Z0,int qopt){
   if(res_mult<0) res_mult=GetResolvedMultiplicity();
   if(!(hitcharge.phit)){
      hitcharge.AssignPointer(this);
   }
   return hitcharge.GetQ(beta,rigidity,qopt,Z0,iside,res_mult,dxdz,dydz);
}
float TrRecHitR::GetEdepH(int iside, float dxdz, float dydz, float Z0,int qopt){
   int res_mult=GetResolvedMultiplicity();
   if(!(hitcharge.phit)){
      hitcharge.AssignPointer(this);
   }
   return 1000*hitcharge.GetEdep(qopt,false,Z0,iside,res_mult,dxdz,dydz);
}


/* 
  Hit Correlation (updated on Sep 2012): 
  - Hypothesys: for a fixed x the y distribution is gaussian
  - Plot sqrt(signal y) VS sqrt(signal x) with only basic correction (to be independent from charge reconstruction)
  - Gaussian slice fit 
  - Polynomial fit of the mean (through 0,0) and sigma behaviour
*/
/* Old parameters (2011)
static double HitCorrelation_XMax = 134; // after this value I give always 1 as probability
static double HitCorrelation_MeanPar[7] = {0,1.15735,-1.36923e-02,7.01365e-05,0,0,0};
static double HitCorrelation_SigmPar[7] = {0.943443,-0.0388644,0.00313217,-5.64444e-06,-1.0751e-06,1.35689e-08,-4.64308e-11};
static double HitCorrelation_SigmMin = 0.8; // evaluated by hand for very low x
*/
// options used for probability calculation
static int    HitCorrelation_Opt = TrClusterR::kAsym;
// mean position
static double HitCorrelation_MeanPar[7] = {0, 1.10018e+00,-6.99183e-03,-1.92904e-04,3.55675e-06,-1.44755e-08,0};
// monotonic function (explodes after 123 ADC counts)!
static double HitCorrelation_XMax = 160;
static double HitCorrelation_SigmPar[7] = {1,-5.50738e-02, 1.24566e-02,-4.60968e-04,7.55153e-06,-5.75738e-08,1.67042e-10}; 
// list of low gain on p (added on nov. 2013)
static int HitCorrelation_TkId_Gain15[15] = {-803, 307, 305, 802, 813, -813, 811, -610, 611, 110, 703, 503, 603, -706, -903};
static int HitCorrelation_TkId_Gain20[ 2] = {213, -904};
float TrRecHitR::GetCorrelationProb() {
  TrClusterR* clx = GetXCluster();
  TrClusterR* cly = GetYCluster();
  if ( (clx==0)||(cly==0) ) return -1; // no definition, default value!
  // cluster signal
  double sig_x = clx->GetTotSignal(HitCorrelation_Opt); 
  double x = sqrt(sig_x);
  if (x>HitCorrelation_XMax) return 1; // good if out of range
  double sig_y = cly->GetTotSignal(HitCorrelation_Opt);
  if ( (HitCorrelation_TkId_Gain15+15)!=find(HitCorrelation_TkId_Gain15,HitCorrelation_TkId_Gain15+15,GetTkId())) sig_y *= 1.5;
  if ( (HitCorrelation_TkId_Gain20+ 2)!=find(HitCorrelation_TkId_Gain20,HitCorrelation_TkId_Gain20+ 2,GetTkId())) sig_y *= 2.0;
  double y = sqrt(sig_y);
  // parameters for the test
  double mean = 0.;
  double sigma = 0.;
  for (int ipar=0; ipar<7; ipar++) {
    double tmp = pow(x,ipar);
    mean  += HitCorrelation_MeanPar[ipar]*tmp;
    sigma += HitCorrelation_SigmPar[ipar]*tmp;
  }
  // gaussian p-value
  float n = fabs(y - mean)/sigma;
  return 1-TMath::Erf(n/sqrt(2));
}
AMSPoint TrRecHitR::MCCoo(bool primary){
if(AMSEventR::Head() && AMSEventR::Head()->nMCEventgC()){
int p=-1;
double rmin=10000;
bool useprimary=primary;
again:
for(unsigned int l=0; l<AMSEventR::Head()->NTrMCCluster();l++){
TrMCClusterR &mc=AMSEventR::Head()->TrMCCluster(l);
if(fabs(mc.GetXgl()[2]-_coord[2])<0.5  && (!useprimary || mc.IsPrimary())){
 double r=(mc.GetXgl()[0]-_coord[0])*(mc.GetXgl()[0]-_coord[0])+(mc.GetXgl()[1]-_coord[1])*(mc.GetXgl()[1]-_coord[1]);
r=sqrt(r);
if(r<rmin){
p=l;
rmin=r;
}
}
}
if(p>=0)return AMSEventR::Head()->TrMCCluster(p).GetXgl();
else if(!useprimary)return  AMSPoint(0,0,0);
else {
useprimary=false;
goto again;
}
}
else return AMSPoint(0,0,0);
}
