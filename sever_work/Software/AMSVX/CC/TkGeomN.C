//  $Id: TkGeomN.C,v 1.21 2022/04/14 09:57:22 qyan Exp $

//////////////////////////////////////////////////////////////////////////
///
///\file  TkGeomN Tracker software V6
///
/// author Q.Yan
///
///\date  2020/11/18 QY  First version
///
/////////////////////////////////////////////////////////////////////////u

#include "TkGeomN.h"
#include "TkDBcN.h"
#include <fstream>

//-------------------------TkSensorN-------------------------
TkSensorN::TkSensorN(): TkPlaneN(){
  dims_[0]=dims_[1]=dims_[2]=0;
  nchan[0]=nchan[1]=0;
  pladchanx=-1;
}


TkSensorN::TkSensorN(int senid,int senstype): TkPlaneN(senid,senstype){
  for(int ixyz=0;ixyz<3;ixyz++){
    dim_[ixyz]=trconstN::SensorDim[ixyz];//sensor total area
    dims_[ixyz]=trconstN::SensorDims[ixyz];//sensor strip area
  }
  //sensor silicon wafer pancake, Y at top and X at bottom
  if(type_==0){//K5
    nchan[0]=trconstN::SensorChannelXK5;
    nchan[1]=trconstN::SensorChannelY;
  }
  else {//K7
    nchan[0]=trconstN::SensorChannelXK7;
    nchan[1]=trconstN::SensorChannelY;
  }
  pladchanx=-1;
}


// K5: NReadoutChannels = 384, NReadoutPerSensor = 192
//     Sens #0       Sens #1         Sens #2       Sens #3
//     | 0, ..., 191 |   0, ..., 191 | 0, ..., 191 |   0, ..., 191 | ...
//     | 0, ..., 191 | 192, ..., 383 | 0, ..., 191 | 192, ..., 383 | ...
//     | 0, ...,            ..., 383 | 0, ...,            ..., 383 | ...
//     Mult #0                       Mult #1       
// K7: NReadoutChannels = 384, NReadoutPerSensor = 224
///     Sens #0       Sens#1                        Sens #2                                      Sens #3
//     | 0, ..., 223 |   0, ...,          ..., 223 |  0, ...,                          ..., 223 | ...
//     | 0, ..., 223 | 224, ..., 383,  0, ...,  63 | 64, ..., 287 | 288, ..., 383,  0, ..., 127 | ...
//     | 0, ...,                 383 | 0, ...,                           ..., 383 | 0, ...,       ...
//     Mult #0                       Mult #1                                      Mult #2
double TkSensorN::GetLocalCooX(int chanx){//chanx 0-191(223) in sensor readout
  if     (chanx==-10)return 0;//sensor center
  else if(chanx<0||chanx>=nchan[0])return -1;
  //------sensor center at 0
  double locx=-trconstN::SensorDims[0]/2.;
//|_._|_._|...|_._._|
//0   1   2  190  191
  if(type_==0){
    locx+=trconstN::PitchReadoutX*chanx;
    if(chanx==nchan[0]-1)locx+=trconstN::PitchReadoutX/2.;//190-191 1.5strip
  }
//|_._|_|_._|_|...|_._|_|_._|_._|...|_._|_._|_._|_|...|
//0   1 2   3 4  62  63 64  65  66 158 159 160 161 162 223
  else {
    int chanximp=0;
    if(chanx<64){//0-63
      chanximp=(chanx/2)*3+(chanx%2)*2;
    }
    else if(chanx<160){//64-160
      chanximp=96+(chanx-64)*2;
    }
    else{//160-223
      chanximp=(chanx/2)*3+(chanx%2)*2+48;
    }
    locx+=trconstN::PitchReadoutX*chanximp/2.;
  }
  return locx;
}


double TkSensorN::GetLocalCooY(int chany){
  if     (chany==-10)return 0;//sensor center
  else if(chany<0||chany>=nchan[1])return -1;
  double locy=-trconstN::SensorDims[1]/2.;
//|_._._._|_._|...|_._|_._._._|_
//0       1   2  637 638     639
  locy+=trconstN::PitchReadoutY*chany;
  if(chany!=0)         locy+=trconstN::PitchReadoutY;
  if(chany==nchan[1]-1)locy+=trconstN::PitchReadoutY;
  return locy;
}


double TkSensorN::GetLocalCooXY(int chan,int ixy){
  return (ixy==0)?GetLocalCooX(chan):GetLocalCooY(chan);
}


AMSPlaneM TkSensorN::GetLocalCoo(double chanx,double chany){//-0.5,+0.5 offsets
  double pxy[2]={-1,-1};
  int mstat=0;
  double merr[2]={trconstN::MeasErrXY[0],trconstN::MeasErrXY[1]};//default rough X/Y measurement error
  for(int ixy=0;ixy<2;ixy++){
    double chanp=(ixy==0)?chanx:chany;
    int chanl=-1,chanh=-1;
    if     (fabs(chanp+10)<1e-5){chanl=chanh=-10;merr[ixy]=0;}//use -10 as sensor center, assign unknown measurement error
    else if(chanp>=-0.5&&chanp<=nchan[ixy]-0.5){chanl=int(chanp);chanh=int(chanp+1.);}//[-0.5,+0.5] offsets
    double pl=GetLocalCooXY(chanl,ixy);
    double ph=GetLocalCooXY(chanh,ixy);
    double pv=-1;
    if     (pl!=-1&&ph!=-1){pv=pl+(ph-pl)*(chanp-chanl);}//both valid
    else if(ph!=-1)        {pv=ph;}//only ph (chanh) is valid
    else if(pl!=-1)        {pv=pl;}//only pl (chanl) is valid
    else                   {pv=-1;mstat+=((ixy==0)?-1:-10);merr[ixy]=0;}//measurement has problem, assign unknown measurement error
    pxy[ixy]=pv;
  }
  AMSPlaneM plm(TVector2(pxy[0],pxy[1]),getid(),mstat,this);
  for(int ixy=0;ixy<2;ixy++)plm.setMCov(ixy,ixy,merr[ixy]*merr[ixy]); 
  if(mstat>-11)plm.calMAlignDerGlobal(1,getid());//sensor alignment update to AMSPlaneM
  return plm;
}


bool TkSensorN::IsValidChan(double chan,int ixy){
  return (chan>=-0.5&&chan<=nchan[ixy]-0.5&&pladchanx>=0);
}


double TkSensorN::FindLocalChan(double &chan,int ixy,double coo){
  double dist=-1000;
  double coor[2]={GetLocalCooXY(0,ixy),GetLocalCooXY(nchan[ixy]-1,ixy)};
  if     (coo<=coor[0]){chan=0;           dist=coo-coor[0];}
  else if(coo>=coor[1]){chan=nchan[ixy]-1;dist=coo-coor[1];}
  else {
    double pitchc=(coor[1]-coor[0])/(nchan[ixy]-1);//averaged pitch size
    int chanp=int((coo-coor[0])/pitchc);//guess closest channel
    double coop=GetLocalCooXY(chanp,ixy);
    double cool=coop,cooh=coop;
    if(coop<=coo){
      for(;chanp<=nchan[ixy]-2;chanp++){
        cooh=GetLocalCooXY(chanp+1,ixy);
        if(cooh>=coo||chanp==nchan[ixy]-2)break;
        cool=cooh;
      }
    }
    else {
      if(chanp>0)chanp--;
      for(;chanp>=0;chanp--){
        cool=GetLocalCooXY(chanp,ixy);
        if(cool<=coo||chanp==0)break;
        cooh=cool;
      }
    }
    chan=chanp;
    if(cooh!=cool)chan+=(coo-cool)/(cooh-cool);
    dist=0;
  }
  return dist;
}


double TkSensorN::FindLocalChan(double &chanx,double &chany,TVector3 gcoo,TVector3 gdir){
  TVector2 loc=straightLineToPlane(gcoo,gdir);//minor linear interpolation to the sensor plan (get coordinate in sensor system)
  double disxy[2];
  for(int ixy=0;ixy<2;ixy++){
    disxy[ixy]=FindLocalChan((ixy==0)?chanx:chany,ixy,(ixy==0)?loc.X():loc.Y());
  }
  double dist=sqrt(disxy[0]*disxy[0]+disxy[1]*disxy[1]);
  return dist;
}


double TkSensorN::GetDistance(TVector3 gcoo,TVector3 gdir,double chanx,double chany,bool striparea,int uxy){
  double lcoor[2][2]={{0}};
  double chan[2]={chanx,chany};
  for(int ixy=0;ixy<2;ixy++){//rectangle edge
    if(ixy==0||ixy==1){if(ixy!=uxy)continue;}
    lcoor[ixy][0]=striparea?GetLocalCooXY(0,ixy):          -getdim(ixy)/2.;
    lcoor[ixy][1]=striparea?GetLocalCooXY(nchan[ixy]-1,ixy):getdim(ixy)/2.;
    if(chan[ixy]!=-2){
      if(!IsValidChan(chan[ixy],ixy))return 2.E100;//error
      else lcoor[ixy][0]=lcoor[ixy][1]=(ixy==0)?GetLocalCoo(chan[ixy],-10).getM().X():GetLocalCoo(-10,chan[ixy]).getM().Y();//fixed X,Y local
    }
  }
  return getLocalDistToEdge(lcoor,gcoo,gdir,uxy);
}


AMSPlaneM TkSensorN::GetInLadderCoo(double chanx,double chany,int ualign){
  AMSPlaneM plm=GetLocalCoo(chanx,chany);
  if(plm.getMStat()<=-11)return plm;//invalid sensor channel
  int ualignsen=(ualign%10);//sensor alignment
  TRotation Rta; TVector3 r0a;
  getRtar0a(Rta,r0a,(ualignsen==0)?0:-1);//without/with sensor alignment,correction:dR,dq:rotation,movement to the ladder
  plm.moveOUV(Rta,r0a);
  return plm;
}


double TkSensorN::GetInLadderChanX(double chanx){
  if(fabs(chanx+10)<1e-5)chanx=double(nchan[0]-1)/2.;
  return IsValidChan(chanx,0)?(chanx+pladchanx):-1;
}


double TkSensorN::GetDAQChanX(double chanx,int &mult){
  double ladchanx=GetInLadderChanX(chanx);//ladder channel
  if(ladchanx<=-1){mult=-2; return ladchanx;}
  double daqchanx=TkLadderN::GetDAQChanX(ladchanx,mult);//daqchanx and multiplicity
  return daqchanx; 
}


double TkSensorN::GetSensorChanX(double ladchanx){
  double chanx=ladchanx-pladchanx;
  return IsValidChan(chanx,0)?chanx:-1;
}


double TkSensorN::GetSensorChanXFromDAQ(double daqchanx, int &mult){
  mult=-2;
  double daqchanx0=TkLadderN::GetDAQChanX(pladchanx,mult);//DAQ channel and mult for the first strip
  double ladchanx=TkLadderN::GetLadderChanX(daqchanx,mult); 
  if(ladchanx<=-1){mult=-2; return ladchanx;}//fatal error
  else {
    if(ladchanx<pladchanx-0.5){mult++; ladchanx+=trconstN::LadderChannel[0];}//underflow mult++
    if(ladchanx>pladchanx+nchan[0]-0.5){mult=-2; return -1;}//overflow outside the sensor
    else {
      double chanx=GetSensorChanX(ladchanx);
      if(chanx<=-1){mult=-2; return -2;}//fatal error
      else         {return chanx;}
    } 
  }
}

//-------------------------TkLadderN-------------------------
TkLadderN::TkLadderN(): TkPlaneN(){
  nchans[0]=nchans[1]=0;
}


TkLadderN::TkLadderN(int tkid,int ladtype): TkPlaneN(tkid,ladtype){
  nchans[0]=nchans[1]=0;
}

 
int TkLadderN::AddSensors(int nsensors){
  nchans[0]=nchans[1]=0;
  dim_[0]=dim_[1]=0;
  sensors_.clear();
  for(int i=0;i<nsensors;i++){
    int senid=(id_>0)?id_*100+i:id_*100-i;
    AddSensor(TkSensorN(senid,type_));
  }
  //------ladder center at 0
  double ladposx=-dim_[0]/2;
  for(size_t i=0;i<sensors_.size();i++){
    ladposx+=sensors_[i].getdim(0)/2.;
    sensors_[i].r0[0]=ladposx;
    ladposx+=sensors_[i].getdim(0)/2.+trconstN::SensorGap;
  }
  return sensors_.size();
}


TkSensorN *TkLadderN::AddSensor(TkSensorN sensor){
   sensor.mother=this;
   sensor.pladchanx=nchans[0];//sensor starting ladder channel
   if(sensors_.size()>=1)dim_[0]+=trconstN::SensorGap;//+gap
   dim_[0]+=sensor.getdim(0);//+senordim
   if(sensor.getdim(1)>dim_[1])dim_[1]=sensor.getdim(1);//use max Y
   nchans[0]+=sensor.nchan[0];//use adding X
   if(sensor.nchan[1]>nchans[1])nchans[1]=sensor.nchan[1];//use max Y
   sensors_.push_back(sensor);
   return &(sensors_.back());
}


double TkLadderN::GetDAQChanX(double ladchanx,int &mult){
  mult=-2;
  double daqchanx=-2;
  if(ladchanx>-0.5){
    int ladchanxi=int(ladchanx);
    if(ladchanx>ladchanxi+0.5)ladchanxi++;
    mult=ladchanxi/trconstN::LadderChannel[0];
    daqchanx=ladchanx-mult*trconstN::LadderChannel[0];
    daqchanx+=trconstN::LadderChannel[1];
  }
  return daqchanx;
}


//mult*trconstN::LadderChannel[0]+(chanx-trconstN::LadderChannel[1])=isen*SensorChannelX+senschanx
int TkLadderN::GetMaxMult(double daqchanx){//0<=imul<=maxmult
  daqchanx-=trconstN::LadderChannel[1];
  if(daqchanx<-0.5||daqchanx>trconstN::LadderChannel[0]-0.5||nchans[0]<=0)return -1;
  int chanxi=int(daqchanx);
  if(daqchanx>chanxi+0.5)chanxi++;
  return (nchans[0]-1-chanxi)/trconstN::LadderChannel[0];
}


double TkLadderN::GetLadderChanX(double daqchanx,int mult){
  double ladchanx=-1;
  if(fabs(daqchanx+10)<1e-5){//use -10 as ladder center
    ladchanx=daqchanx;
  }
  else {
    daqchanx-=trconstN::LadderChannel[1];//639.5
    if(daqchanx<-0.5||daqchanx>trconstN::LadderChannel[0]-0.5||mult<0)ladchanx=-2;//Error out of [-0.5,+0.5] channel offset
    else {
      ladchanx=daqchanx+mult*trconstN::LadderChannel[0];
    }
  }
  return ladchanx;
}

//mult*trconstN::LadderChannel[0]+(chanx-trconstN::LadderChannel[1])=isen*SensorChannelX+senschanx
TkSensorN *TkLadderN::GetSensor(double ladchanx,double &senschanx){
  senschanx=-1;
  if(fabs(ladchanx+10)<1e-5){//use -10 as ladder center
    ladchanx=double(nchans[0]-1)/2.;
  }
  if(ladchanx<-0.5||ladchanx>nchans[0]-0.5)return 0;//Error ladder out of [-0.5,+0.5] channel offset
  int chanr[2]={0,0};
  for(size_t i=0;i<sensors_.size();i++){
    chanr[1]=chanr[0]+sensors_[i].nchan[0];
    if(ladchanx>=chanr[0]-0.5&&ladchanx<=chanr[1]-0.5){senschanx=ladchanx-chanr[0];return &(sensors_[i]);}
    chanr[0]=chanr[1];
  }
  return 0;
}


TkSensorN *TkLadderN::FindSensor(double &dist,TVector3 gcoo,TVector3 gdir,bool striparea,double daqchanx,double daqchany){
  dist=2.E100;
  TkSensorN *msensor=0;
  for(size_t i=0;i<sensors_.size();i++){
    TkSensorN *psensor=&sensors_[i];
    double chanx=-2,chany=daqchany;
    if(daqchanx!=-2){
      int mult;
      chanx=psensor->GetSensorChanXFromDAQ(daqchanx,mult);
      if(chanx<=-1)continue;
    }
    double ndist=psensor->GetDistance(gcoo,gdir,chanx,chany,striparea);
    if(msensor&&ndist>=dist){break;}///find minimal using previous one
    else                    {dist=ndist;msensor=psensor;}//this is the closest
  }
  return msensor;
}


double TkLadderN::GetDistToEdgeRough(TVector3 gcoo,TVector3 gdir){
  double lcoor[2][2]={{0}};
  for(int ixy=0;ixy<2;ixy++){//rectangle edge
    lcoor[ixy][0]=-getdim(ixy)/2.;
    lcoor[ixy][1]= getdim(ixy)/2.;
  }
  return getLocalDistToEdge(lcoor,gcoo,gdir);
}


AMSPlaneM TkLadderN::GetLocalCoo(double ladchanx,double ladchany,int ualign){
  double senschanx=-1;
  double senschany=ladchany;
  TkSensorN *psensor=GetSensor(ladchanx,senschanx);
  if(!psensor){return AMSPlaneM(TVector2(-1,-1),getid(),-22);}//invalid sensor
  else {
    AMSPlaneM plm=psensor->GetInLadderCoo(senschanx,senschany,ualign);
    if(fabs(ladchanx+10)<1e-5)plm.setMCov(0,0,0.);//use -10 as ladder center, assign unknown measurement error
    plm.calMAlignDerGlobal(1,getid());//ladder alignment update to AMSPlaneM
    return plm;
  }
}


AMSPlaneM TkLadderN::GetInLayerCoo(double ladchanx,double ladchany,int ualign){
  int ualignsen=(ualign%10);//sensor alignment
  int ualignlad=((ualign/10)%10);//ladder alignment
  AMSPlaneM plm=GetLocalCoo(ladchanx,ladchany,ualignsen);//sensor alignment
  if(plm.getMStat()<=-11)return plm;//invalid
  TRotation Rta; TVector3 r0a;
  getRtar0a(Rta,r0a,(ualignlad==0)?0:-1);//without/with ladder alignment, correction:dR,dq:rotation,movement to the layer
  plm.moveOUV(Rta,r0a);
  return plm;
}


//-------------------------TkLayerN-------------------------
TkLayerN::TkLayerN(): TkPlaneN(){
}


TkLayerN::TkLayerN(int layid,int laytype): TkPlaneN(layid,laytype){
}


TkLadderN *TkLayerN::AddLadder(TkLadderN lad){
  lad.mother=this;
  ladders_[lad.getid()]=lad;
  return &ladders_[lad.getid()];
}


TkLadderN *TkLayerN::GetLadder(int tkid){
  map<int, TkLadderN>::iterator it=ladders_.find(tkid);
  return (it==ladders_.end())? 0: &(it->second);
}


TkSensorN *TkLayerN::FindSensor(double &dist,TVector3 gcoo,TVector3 gdir,bool striparea){
  dist=2.E100;
  TkLadderN *mladder[2]={0,0};
  for(map<int, TkLadderN>::iterator it1=ladders_.begin();it1!=ladders_.end();it1++){//ladder
    TkLadderN *pladder=&(it1->second);
    double ndist=pladder->GetDistToEdgeRough(gcoo,gdir);//rough comparison
    if(!mladder[0]||ndist<dist){
      mladder[1]=mladder[0];//second closest ladder
      mladder[0]=pladder;   //closest ladder
      dist=ndist;
    }
  }
  //----more detailed comparison
  TkSensorN *psensor=0;
  if(mladder[0]){
    psensor=mladder[0]->FindSensor(dist,gcoo,gdir,striparea);//precisely choose between the best 2 candidates
    if(mladder[1]){
      double ndist;
      TkSensorN *nsensor=mladder[1]->FindSensor(ndist,gcoo,gdir,striparea);
      if(ndist<dist){dist=ndist;psensor=nsensor;}
    }
  }
  return psensor; 
}


AMSPlaneM TkLayerN::GetLocalCoo(int tkid,double ladchanx,double ladchany,int ualign){
  TkLadderN *pladder=GetLadder(tkid);
  if(!pladder){return AMSPlaneM(TVector2(-1,-1),getid(),-33);}//invalid ladder
  else {
    AMSPlaneM plm=pladder->GetInLayerCoo(ladchanx,ladchany,ualign);
    plm.calMAlignDerGlobal(1,getid());//layer alignment update to AMSPlaneM
    return plm;
  }
}


AMSPlaneM TkLayerN::GetGlobalCoo(int tkid,double ladchanx,double ladchany,int ualign){
  int ualignladsen=(ualign%100);//ladder+sensor alignment
  int ualignlay=((ualign/100)%10);//ladder alignment
  AMSPlaneM plm=GetLocalCoo(tkid,ladchanx,ladchany,ualignladsen);//sensor alignment
  if(plm.getMStat()<=-11)return plm;//invalid
  TRotation Rta; TVector3 r0a;
  getRtar0a(Rta,r0a,(ualignlay==0)?0:-1);//without/with layer alignment, correction:dR,dq:rotation,movement to the global
  plm.moveOUV(Rta,r0a);
  return plm;
}



//-------------------------TkAlignParN-------------------------
TkAlignParN::TkAlignParN(){
  Clear();
}


int TkAlignParN::Clear(){
  atime[0]=atime[1]=0;
  apar.clear();
  apae.clear();
  return 0;
}


int TkAlignParN::ConvertId(int uid,int opt){
  int label=uid;
  int detid=uid;
  if(opt==0){//opt=0 label->detid
    if(label>=10000000){detid=label%10000000;detid=-detid;}//negative tkid
    return detid;
  }
  else {//detid->label
    if(detid<0)label=10000000+abs(detid);
    return label;
  }
}


//-------------------------TkAlignParS-------------------------
TkAlignParS::TkAlignParS(){
  spaf=0;
  Clear();
}


int TkAlignParS::Clear(){
  label=0;
  atime[0]=atime[1]=0;
  spar.clear();
  if(spaf){delete spaf; spaf=0;}
  return 0;
}


TkAlignParS& TkAlignParS::operator=(const TkAlignParS &other){
  if(this!=&other){
    label=other.label;
    atime[0]=other.atime[0];
    atime[1]=other.atime[1];
    spar=other.spar;
    if(spaf){delete spaf; spaf=0;}
    if(other.spaf)BuildFun();
  }
  return *this;
}


int TkAlignParS::BuildFun(){
  if(spaf){delete spaf; spaf=0;}
  if(spar.size()>=2&&!spaf){
    int nno=spar.size();
    Double_t *xno=new Double_t[nno];
    Double_t *yno=new Double_t[nno];
    int ino=0;
    for(map<unsigned int, double>::iterator it=spar.begin();it!=spar.end();it++){
      xno[ino]=it->first;
      yno[ino]=it->second;
      ino++;
    }
    spaf=new TSpline3("tkalignsp",xno,yno,nno,"b1e1",0,0);
    delete [] xno;
    delete [] yno;
  }
  return 0;
}


double TkAlignParS::GetAlignPar(double ptime){
  static int cn=0;
  bool outtime=0;
  if     (ptime<double(atime[0])-1)     {if(ptime<double(atime[0])-100)outtime=1; ptime=double(atime[0])-1;}
  else if(atime[1]!=0&&ptime>atime[1]+1){if(ptime>atime[1]+100)outtime=1; ptime=atime[1]+1;}
  if(outtime&&cn++<10)cerr<<"Error TkAlignParS::GetAlignPar No accurate alignment for time="<<ptime<<" atime="<<atime[0]<<","<<atime[1]<<endl;
  if(spar.size()>=2){
    if(!spaf)BuildFun();
    return spaf->Eval(ptime-1305800000);//1305800000 is the offset
  }
  else {
    if(spaf){delete spaf; spaf=0;}
    if(spar.size()==1){return spar.begin()->second;}//1305800000 is the offset
    else return 0; 
  }
}


#include "timeid.h"
#include "commonsi.h"
//-------------------------TkAlignParDB-------------------------
TkAlignParDB::TkAlignParDB(){
  tdv=0;
  Clear(12);
  InitTDV("TkAlign6",-2,40000);//maxsize
  memset(TDVBlock,0,GetTDVLength());
}


TkAlignParDB::~TkAlignParDB(){
  if(tdv){delete tdv;}
}


int TkAlignParDB::Clear(int opt){
  for(int i=0;i<2;i++){ppara[i].first.first=0; ppara[i].first.second=-9999; ppara[i].second.Clear();}
  ppart.Clear();
  pparn=0;
  ppars.clear();
  if(opt==0||(opt%10)>=2){aparns.clear();}
  if(opt==1||(opt%10)>=2){aparss.clear();}
  if((opt/10)%10>=1){aparbias.Clear(); ainvbias=0; abiasvers=-1;}
  return 0;
}


int TkAlignParDB::InitTDV(const char *tname,int tvers,int tsize){
  if(tname){
    strcpy(TDVName[0],tname);
    TDVVers=tvers;
    sprintf(TDVName[1],"%sV%d",TDVName[0],TDVVers);
  }
  if(tsize>0)TDVSize=tsize;
  TDVTime[0]=TDVTime[1]=0;
  if(tdv){delete tdv; tdv=0;}
  return 0;
}


TkAlignParDB& TkAlignParDB::operator=(const TkAlignParDB &other){
  if(this!=&other){
    aparns=other.aparns;
    aparss=other.aparss;
    aparbias=other.aparbias;
    ainvbias=other.ainvbias;
    pparn=0;//clean pointer=0
    ppars.clear();//clean pointer
    for(int i=0;i<2;i++){ppara[i]=other.ppara[i];}
    if(ppara[0].first.first!=-2)ppara[0].first.first=0;
    ppart=other.ppart;
    TDVVers=other.TDVVers;
    TDVSize=other.TDVSize;
    memcpy(TDVBlock,other.TDVBlock,sizeof(TDVBlock));
    memcpy(TDVName,other.TDVName,sizeof(TDVName));
    for(int i=0;i<2;i++)TDVTime[i]=other.TDVTime[i];
    if(tdv){delete tdv; tdv=0;}
  }
  return *this;
}


double TkAlignParDB::GetRigTimeCor(TkAlignParN &pcor,double rig,double ptime,int opt,double scalebias){
  unsigned int rtime=(unsigned int)ptime;
  pcor.Clear();
  pcor.atime[0]=pcor.atime[1]=rtime;
  double invcor=0;//TV-1
  if(ptime>1305000000){//V6 time-dependent scale correction
    double x=ptime-1305853511;
//    double fpar[3]={2.40117,0.0588201,2.12624e-08};
    double fpar[4]={0.224544,4.46777,3.84535e-08,0.03375+0.0126};//evp=-0.0126(will soften flux)->+0.0126
    invcor=fpar[0]/(1.+fpar[1]*exp(-fpar[2]*x))-fpar[0]+fpar[3];//time->infinite, invcor=0
  }
  invcor+=scalebias;
  if(invcor!=0){
//  double shifts[trconstN::nLayer]={-2.84867,-0.565055,-0.18503,-0.139079,-0.000638471,-0.00115095,-0.137036,-0.181856,-2.28694};//inav=+0.17283506
    double shifts[trconstN::nLayer]={-2.67583,-0.392220,-0.01219, 0.033756, 0.17219659,  0.17168411, 0.035799,-0.009021,-2.11410};//shift for 1/10TV-1
    for(int il=0;il<trconstN::nLayer;il++){//Yshift for each layer
      double pshift=invcor/(0.1)*shifts[il]*1e-4;//um->cm
      pcor.apar[il]=make_pair(TVector3(0.,pshift,0.),TVector3(0.,0.,0.));//id=il
    }
  }
//------
  double nrig=rig;
  if(nrig!=0){//GV
    if(opt==0)nrig=1./(1./rig+invcor*1e-3);//raw-rigidity to corrected-rigidity (default)
    else      nrig=1./(1./rig-invcor*1e-3);//corrected-rigidity to raw-rigidity 
  }
  return nrig;
}


int TkAlignParDB::UpdateAlignPar(double ptime,int force,int um){
  if(ppara[0].first.first==-2)return ppara[0].first.first;//load file error appearing
  if(force==0){
    if(ptime==0){cerr<<"Error TkAlignParDB::UpdateAlignPar ptime=0"<<endl;}
    if(ppara[0].first.second==um){
      if(ptime==ppara[0].first.first)return 0;
      if(ppara[0].first.first==-1)return 0;//stop updating as this is static block
    }
  }
  ppara[0].first.first=ptime;
  ppara[0].first.second=um;
  ppara[0].second.Clear();
  unsigned int rtime=(unsigned int)ptime;

//------
  pparn=0;
  if(aparns.size()==1){
    pparn=&((aparns.begin())->second); 
  }
  else if(aparns.size()>=2){
    map<unsigned int, TkAlignParN>::iterator it0=aparns.upper_bound(rtime);//index>rtime
    if(it0!=aparns.begin())it0--;
    pparn=&(it0->second);
  }
  if(pparn&&(um<0||(um%10)>=1))ppara[0].second=(*pparn);

//----Dynamic alignment
  ppars.clear();
  for(map<int, map<unsigned int, TkAlignParS> >::iterator it=aparss.begin();it!=aparss.end();it++){
    int label=it->first;
    if     ((it->second).size()==0){cout<<"Error TkAlignParDB::UpdateAlignPar zero size"<<endl;continue;}
    else if((it->second).size()==1){ppars[label]=&(((it->second).begin())->second);}
    else {
      map<unsigned int, TkAlignParS>::iterator it1=(it->second).upper_bound(rtime);//index>rtime
      if(it1!=(it->second).begin())it1--;
      ppars[label]=&(it1->second);
    }
  }
  for(map<int, TkAlignParS*>::iterator it2=ppars.begin();it2!=ppars.end();it2++){
    map<int,pair<TVector3,TVector3> > *dp=&(ppara[0].second.apar);
    int label=it2->first;
    int id=TkAlignParN::ConvertId(label,0);//label->detid
    int ipar=abs(id%10)-1;//ipar
    id/=10;
    double var=it2->second->GetAlignPar(ptime);
    if(um<0||(um/10)%10>=1){
      if((*dp).find(id)==(*dp).end()){
        (*dp)[id]=make_pair(TVector3(0.,0.,0.),TVector3(0.,0.,0.));
      }
      if(ipar>=0&&ipar<3){
        ((*dp)[id].first)[ipar]+=var;
      }
      else {
        ((*dp)[id].second)[ipar-3]+=var;
      }
    }
  }


//----Additional offset bias added
  if(um<=-1||(um/100)%10>=1){
    for(map<int, pair<TVector3,TVector3> >::iterator it=aparbias.apar.begin();it!=aparbias.apar.end();it++){
      int id=it->first;
      map<int,pair<TVector3,TVector3> > *dp=&(ppara[0].second.apar);
      if((*dp).find(id)==(*dp).end()){
        (*dp)[id]=it->second;
      }
      else {(*dp)[id].first+=(it->second).first;(*dp)[id].second+=(it->second).second;}
    }
  }
  

//----Inner Time dependence correction
  if(um<=-2||(um/1000)%10>=1){
    GetRigTimeCor(ppart,0,ptime,0,ainvbias);
    map<int,pair<TVector3,TVector3> > *tcor=&(ppart.apar);
    for(map<int,pair<TVector3,TVector3> >::iterator it3=tcor->begin();it3!=tcor->end();it3++){
      int id=it3->first;
      map<int,pair<TVector3,TVector3> > *dp=&(ppara[0].second.apar);
      if((*dp).find(id)==(*dp).end()){
        (*dp)[id]=it3->second;
      }
      else {(*dp)[id].first+=(it3->second).first;(*dp)[id].second+=(it3->second).second;} 
    }
  }
  else if(ppart.atime[0]!=0)ppart.Clear(); 

    
  ppara[0].second.atime[0]=ppara[0].second.atime[1]=rtime;
//-----
//  if(aparns.size()<=1&&aparss.size()==0&&aparbias.atime[0]==0&&ppart.atime[0]==0)ppara[0].first.first=-1;//stop updating as this is static block
  if(aparns.size()<=1&&aparss.size()==0&&ppart.atime[0]==0)ppara[0].first.first=-1;//stop updating as this is static block

  return 1;
}


int TkAlignParDB::CopyToTDVBlock(int wopt){
  int ib=2;
  memset(TDVBlock,0,GetTDVLength());
  if(aparns.size()>=1&&(wopt/1000)%10!=2){
    for(map<unsigned int, TkAlignParN>::iterator it1=aparns.begin();it1!=aparns.end();it1++){
      TkAlignParN *ppar=&(it1->second);
      if(it1==aparns.begin())((unsigned int *)TDVBlock)[0]=ppar->atime[0];//the first 
      ((unsigned int *)TDVBlock)[1]=ppar->atime[1];//the last
      int haserr=((ppar->apae).size()>=1)?1:0;
      if((wopt/100)%10==1)haserr=0;//force no err in writing
      if(aparns.size()>=2||ppar->atime[1]!=0||haserr){
        ((int *)TDVBlock)[ib++]=-1;//new format with int
        TDVBlock[ib++]=(haserr)?-2:-1;
        ((unsigned int *)TDVBlock)[ib++]=ppar->atime[0];
        ((unsigned int *)TDVBlock)[ib++]=ppar->atime[1];
      }
      TkAlignParN *tpar=ppar;
      if(aparss.size()>=1&&(wopt/1000)%10==3){//has spline && add spline
        double ptime=(ppar->atime[1]==0)?double(ppar->atime[0])+0.5:(double(ppar->atime[0])+double(ppar->atime[1])+1.)/2.;
        UpdateAlignPar(ptime,0);
        tpar=&(ppara[0].second);
      }
      for(map<int,pair<TVector3,TVector3> >::iterator it=(tpar->apar).begin();it!=(tpar->apar).end();it++){
        int label=ppar->ConvertId(it->first*10,1);//detid->label
        for(int ipar=0;ipar<6;ipar++){
          ((int *)TDVBlock)[ib++]=label+(ipar+1);
          if(ipar>=0&&ipar<3){TDVBlock[ib++]=(it->second).first[ipar];   if(haserr)TDVBlock[ib++]=(ppar->apae)[it->first].first[ipar];}
          else               {TDVBlock[ib++]=(it->second).second[ipar-3];if(haserr)TDVBlock[ib++]=(ppar->apae)[it->first].second[ipar-3];}
        }
      }
    }
  }
  else {
    for(map<int, map<unsigned int, TkAlignParS> >::iterator it1=aparss.begin();it1!=aparss.end();it1++){//det
      int label=it1->first;
      for(map<unsigned int, TkAlignParS>::iterator it=(it1->second).begin();it!=(it1->second).end();it++){//time
        TkAlignParS *ppas=&(it->second);
        if(it1==aparss.begin()){
          if(it==(it1->second).begin())((unsigned int *)TDVBlock)[0]=ppas->atime[0];//the first 
          ((unsigned int *)TDVBlock)[1]=ppas->atime[1];//the last
        }
        ((int *)TDVBlock)[ib++]=-1;//new format with int
        TDVBlock[ib++]=-5;
        ((int *)TDVBlock)[ib++]=label;//detid
        ((unsigned int *)TDVBlock)[ib++]=it->first;//stime
        ((unsigned int *)TDVBlock)[ib++]=ppas->atime[0];
        ((unsigned int *)TDVBlock)[ib++]=ppas->atime[1];
//        cout<<"streamtdv="<<label<<","<<it->first<<","<<ppas->atime[0]<<","<<ppas->atime[1]<<" size="<<(it->second).spar.size()<<endl;
        for(map<unsigned int, double>::iterator it0=(it->second).spar.begin();it0!=(it->second).spar.end();it0++){//node
          ((int *)TDVBlock)[ib++]=it0->first;//time
          TDVBlock[ib++]=it0->second;//value
//          cout<<"varn="<<it0->first<<","<<it0->second<<endl;
        }
      }
    }
  }
  return 0;
}


int TkAlignParDB::GetSize(int im){
  int dsize=0;
  if(im==0){
    dsize=2;
    for(map<unsigned int, TkAlignParN>::iterator it1=aparns.begin();it1!=aparns.end();it1++)dsize+=(2+(it1->second).GetSize());
  }
  else {
    dsize=2;
    for(map<int, map<unsigned int, TkAlignParS> >::iterator it1=aparss.begin();it1!=aparss.end();it1++){//detid
      for(map<unsigned int, TkAlignParS>::iterator it=(it1->second).begin();it!=(it1->second).end();it++)dsize+=(3+(it->second).GetSize());//time
    }
  }
  return dsize;
}


int TkAlignParDB::LoadAlignPar(const char *falign,bool usetdv,unsigned int rtime,int force,unsigned int etimeb,int wopt){
  int lstat=1;//need to update time
  int label=0,label2=0;
  double var=0,vad[2]={0},vae=0;
  unsigned int ptime[2]={0},stime=0;
  TkAlignParN *ppar=0;
  TkAlignParS *ppas=0;
  if(falign){
    ifstream fbin(falign);
    cout<<"TkAlignParDB::LoadAlignPar falign="<<falign<<endl;
    if(!fbin){cerr<<"Error TkAlignParDB::LoadAlignPar: can not open "<<falign<<endl;ppara[0].first.first=-2;return -1;}//set error bit
    string ss;
    ppara[0].first.first=ppara[1].first.first=0;
    ppara[0].first.second=ppara[1].first.second=-9999;
    int haserr=0,hasclean=0;
    while(getline(fbin,ss)){
      istringstream iss(ss);
      if(!(iss>>label>>var))continue;
      if(label==-1&&(var==-1||var==-2)){//bin format: next would be atime[2]
        if(!(iss>>ptime[0]>>ptime[1]))continue;
        if(force>=0){if(hasclean==0){Clear(force>=2?force:0);hasclean=1;}}//clean all
        ppar=&(aparns[ptime[0]]);
        if(force>=-1)ppar->Clear();//force>=-1 means clean local
        ppar->atime[0]=ptime[0];
        ppar->atime[1]=ptime[1];
        haserr=(var==-2)?1:0;
        ppas=0;
        continue;
      }
      else if(label==-1&&(var==-5)){//spline format
        if(!(iss>>label2>>stime>>ptime[0]>>ptime[1]))continue; //label2 btime etime
        if(force>=0){if(hasclean==0){Clear(force>=2?force:1);hasclean=1;}}//clean all
        ppas=&(aparss[label2][stime]);
        ppas->Clear();
        ppas->label=label2;
        ppas->atime[0]=ptime[0];
        ppas->atime[1]=ptime[1];
        haserr=0;
        ppar=0;
        continue;
      }
      if(ppar==0&&ppas==0){//default bin format
        if(force>=0){if(hasclean==0){Clear(force>=2?force:0);hasclean=1;}}//clean all
        ppar=&(aparns[rtime]);
        if(force>=-1)ppar->Clear();//force>=-1 means clean local
        ppar->atime[0]=rtime;
        ppar->atime[1]=etimeb;
        haserr=0;
      }
      if     ((wopt/100)%10==1)haserr=0;//format1/no err
      else if((wopt/100)%10==2)haserr=1;//format2/err
      else if((wopt/100)%10==3)haserr=2;//format3/other+err
      if     (haserr==2){if(!(iss>>vad[0]>>vad[1]>>vae))continue;}
      else if(haserr==1){if(!(iss>>vae))continue;}
      if(ppas){
        (ppas->spar)[label]=var;//label is time
      }
      else {
        map<int,pair<TVector3,TVector3> > *dp=&(ppar->apar);
        int id=ppar->ConvertId(label,0);//label->detid
        int ipar=abs(id%10)-1;//ipar
        id=id/10;
        if((*dp).find(id)==(*dp).end()){
          (*dp)[id]=make_pair(TVector3(0.,0.,0.),TVector3(0.,0.,0.));
          if(haserr)(ppar->apae)[id]=make_pair(TVector3(0.,0.,0.),TVector3(0.,0.,0.));
        }
        if(ipar>=0&&ipar<3){
          if(force>=-1)((*dp)[id].first)[ipar]=var;
          else         ((*dp)[id].first)[ipar]+=var;//force<=-2 means addup
          if(haserr)((ppar->apae)[id].first)[ipar]=vae;//err
        }
        else {
          if(force>=-1)((*dp)[id].second)[ipar-3]=var;
          else         ((*dp)[id].second)[ipar-3]+=var;//force<=-2 means addup
          if(haserr)((ppar->apae)[id].second)[ipar-3]=vae;//err
        }
      }
    }
    fbin.close();
    CopyToTDVBlock(wopt);//copy alignpars to TDVBlock
    lstat=2;
  }
  else if(usetdv){
    unsigned int btime=((unsigned int *)TDVBlock)[0];
    unsigned int etime=((unsigned int *)TDVBlock)[1];
    if(force==0){
      if(btime==0&&etime==0){cerr<<"Error TkAlignParDB::LoadAlignPar TDVBlock time=0"<<endl;ppara[0].first.first=-2;return -2;}//set error bit
      if(btime==TDVTime[0]&&etime==TDVTime[1]){return (aparns.size()>=2||aparss.size()>=1)?1:0;}//tdv not update,fast return
    }
    TDVTime[0]=btime;
    TDVTime[1]=etime;
    Clear(2);//ppara[0].first.first=ppara[1].first.first=0;
    int haserr=0;;
    for(int ib=2;ib<TDVSize;){//starting from 2
      label=((int *)TDVBlock)[ib++];//new format with int
      if(label==0)break;//when label=0, stop reading the block
      var=TDVBlock[ib++];
      if(label==-1&&(var==-1||var==-2)){//next would be atime[2]
        ptime[0]=((unsigned int *)TDVBlock)[ib++];
        ptime[1]=((unsigned int *)TDVBlock)[ib++];
        ppar=&(aparns[ptime[0]]);
        ppar->Clear();//force>=-1 means clean local
        ppar->atime[0]=ptime[0];
        ppar->atime[1]=ptime[1];
        haserr=(var==-2)?1:0;
        ppas=0;
        continue;
      }
      else if(label==-1&&(var==-5)){//spline format
        label2=((int *)TDVBlock)[ib++];
        stime=((unsigned int *)TDVBlock)[ib++];
        ptime[0]=((unsigned int *)TDVBlock)[ib++];
        ptime[1]=((unsigned int *)TDVBlock)[ib++];
        ppas=&(aparss[label2][stime]);
        ppas->Clear();
        ppas->label=label2;
        ppas->atime[0]=ptime[0];
        ppas->atime[1]=ptime[1];
        haserr=0;
        ppar=0;
        continue;
      }
      if(ppar==0&&ppas==0){
        ppar=&(aparns[btime]);
        ppar->Clear();//force>=-1 means clean local
        ppar->atime[0]=btime;
        ppar->atime[1]=etime;
        haserr=0;
      }
      if(haserr&&(wopt/100)%10==1)haserr*=-1;//force no err
      vae=haserr?TDVBlock[ib++]:0;
      if(ppas){
        (ppas->spar)[label]=var;//label is time
      }
      else {
        map<int,pair<TVector3,TVector3> > *dp=&(ppar->apar);
        int id=ppar->ConvertId(label,0);//label->detid
        int ipar=abs(id%10)-1;//ipar
        id=id/10;
        if((*dp).find(id)==(*dp).end()){
          (*dp)[id]=make_pair(TVector3(0.,0.,0.),TVector3(0.,0.,0.));
          if(haserr>0)(ppar->apae)[id]=make_pair(TVector3(0.,0.,0.),TVector3(0.,0.,0.));
        }
        if(ipar>=0&&ipar<3){
          ((*dp)[id].first)[ipar]=var;
          if(haserr>0)((ppar->apae)[id].first)[ipar]=vae;
        }
        else {
          ((*dp)[id].second)[ipar-3]=var;
          if(haserr>0)((ppar->apae)[id].second)[ipar-3]=vae;
        }
      }
    }
    lstat=2;
  }
  return lstat;
}


int TkAlignParDB::StreamAlignTDV(const char *ofile,int wopt){
  cout<<"TkAlignParDB::StreamAlignTDV";
  ofstream *fbin=0;
  if(ofile){
    if(wopt%10==1)fbin=new ofstream(ofile,ios::out|ios::app);
    else          fbin=new ofstream(ofile);
    cout<<" ofile="<<ofile;
  }
  cout<<endl;
  cout<<" TDVTime="<<((unsigned int *)TDVBlock)[0]<<","<<((unsigned int *)TDVBlock)[1]<<endl;
  if((wopt/10)%10==1){if(fbin)(*fbin)<<"-1 -1. "<<((unsigned int *)TDVBlock)[0]<<" "<<((unsigned int *)TDVBlock)[1]<<endl;}
  unsigned int ptime[2]={0},stime=0;;
  int npar=0;
  int haserr=0;
  int ib=2;
  for(;ib<TDVSize;){//start from 2
    int label=((int *)TDVBlock)[ib++];//new format with int
    if(label==0)break;//when label=0, stop reading the block
    double var=TDVBlock[ib++];
//    cout<<"var="<<var<<" ib="<<(ib-1)<<endl;
    if(label==-1&&(var==-1||var==-2)){
      ptime[0]=((unsigned int *)TDVBlock)[ib++];
      ptime[1]=((unsigned int *)TDVBlock)[ib++];
      haserr=(var==-2)?1:0;
      if(haserr&&(wopt/100)%10==1){//force no err in writing
        var=-1;
        haserr=-1;
      }
      if(fbin)(*fbin)<<label<<" "<<var<<" "<<ptime[0]<<" "<<ptime[1]<<endl;
      else       cout<<label<<" "<<var<<" "<<ptime[0]<<" "<<ptime[1]<<endl;
    }
    else if(label==-1&&(var==-5)){
      int label2=((int *)TDVBlock)[ib++];
      stime=((unsigned int *)TDVBlock)[ib++];
      ptime[0]=((unsigned int *)TDVBlock)[ib++];
      ptime[1]=((unsigned int *)TDVBlock)[ib++];
      haserr=0;
      if(fbin)(*fbin)<<label<<" "<<var<<" "<<label2<<" "<<stime<<" "<<ptime[0]<<" "<<ptime[1]<<endl;
      else       cout<<label<<" "<<var<<" "<<label2<<" "<<stime<<" "<<ptime[0]<<" "<<ptime[1]<<endl;
    }
    else {
      double vae=haserr?TDVBlock[ib++]:0;
      if(fbin){(*fbin)<<label<<" "<<var;if(haserr>0)(*fbin)<<" "<<vae;(*fbin)<<endl;}
      else    {   cout<<label<<" "<<var;if(haserr>0)   cout<<" "<<vae;   cout<<endl;}
      npar++;
    }
  }
  if(fbin){fbin->close();delete fbin;}
  cout<<"npar="<<npar<<" need tdvsize>="<<(ib-1)<<endl;
  return npar;
}


int TkAlignParDB::LoadAlignTDV(long int rtime,int isreal,int force){
  if(force>=1&&tdv){delete tdv; tdv=0;}
  if(TDVVers<0)return -100;//TDVVers<0 means no alignment
  if(!tdv){
    time_t statime = 1;
    time_t endtime = 0;
    tm beg,end;
    localtime_r(&statime, &beg);
    localtime_r(&endtime, &end);
    memset(TDVBlock,0,GetTDVLength());
    TDVTime[0]=TDVTime[1]=0;
    tdv=new AMSTimeID(AMSID(TDVName[1],isreal),beg,end,GetTDVLength(),TDVBlock,AMSTimeID::Standalone,1);
  }
  time_t tt = time_t(rtime);
  int ret=tdv->validate(tt);//ret=0 should assign error!
  int fstat=LoadAlignPar(0,1,rtime,0);
  return fstat;
}


int TkAlignParDB::CopyAlignTDV(const TkAlignParDB &other){
  if(this==&other)return 0;
  if(TDVVers==other.TDVVers&&TDVSize==other.TDVSize&&TDVBlock[0]==other.TDVBlock[0]&&TDVBlock[1]==other.TDVBlock[1])return 0;//no check TDVName to speed up
  TDVVers=other.TDVVers;
  TDVSize=other.TDVSize;
  memcpy(TDVBlock,other.TDVBlock,sizeof(TDVBlock));
  memcpy(TDVName,other.TDVName,sizeof(TDVName));
  for(int i=0;i<2;i++)TDVTime[i]=other.TDVTime[i];
  if(TDVVers<0)Clear(12);//clean
  else         LoadAlignPar(0,1,0,1);//force to load, will copy TDVTime as well
  return 1;
}


int TkAlignParDB::WriteAlignTDV(long int btime,long int etime,int vers,int isreal){
  time_t statime = time_t(btime);
  time_t endtime = time_t(etime);
  tm beg, end;
  localtime_r(&statime,&beg);
  localtime_r(&endtime,&end);
  cout << "Begin: " <<(int)statime<<"  " <<asctime(&beg)<<endl;
  cout << "End  : " <<(int)endtime<<"  " <<asctime(&end)<<endl;
  ((unsigned int *)TDVBlock)[0]=btime;
  ((unsigned int *)TDVBlock)[1]=etime;
  char tname[200];
  sprintf(tname,"%sV%d",TDVName[0],vers);
  cout<<"Write TDVName="<<tname<<endl;
  AMSTimeID* tdvw=new AMSTimeID(AMSID(tname,isreal),beg,end,GetTDVLength(),TDVBlock,AMSTimeID::Standalone,1);
  tdvw->UpdateMe();
  int ret=tdvw->write(AMSDATADIR.amsdatabase);
  delete tdvw;
  return ret;
}


//-------------------------TkTrackN-------------------------
TkTrackN::TkTrackN(): TkPlaneN(){
  UseAlignVersion(0,-1);//set Alignment Version=DefaultTDVVers
}


TkTrackN* TkTrackN::fHead = 0;
TkTrackN* TkTrackN::fHeadG = 0;

TkTrackN* TkTrackN::GetHead() {
  if(fHead ==0){
    cout<<"TkTrackN::GetHead()->ConstructTracker() singleton initialization"<<endl;
    fHead = new TkTrackN();
    fHead->ConstructTracker();
  }
  return fHead;
}

TkTrackN* TkTrackN::GetHeadG(){//only used for storing TDV, even have no real geometry
  if(fHeadG==0){
#pragma omp critical (trtrackheadtdv)
   {
     cout<<"TkTrackN::GetHeadG()->ConstructTracker() singleton initialization"<<endl;
     fHeadG = new TkTrackN();
     fHeadG->ConstructTracker();
   }
  }
  return fHeadG;
}


TkLayerN *TkTrackN::AddLayer(TkLayerN layer){
  layer.mother=this;
  layers_[layer.getid()]=layer;
  return &layers_[layer.getid()];
}


int TkTrackN::ConstructTracker(){//norm geometry without alignment
  for(int il=0;il<trconstN::nLayer;il++){
    TkLayerN *layer=AddLayer(TkLayerN(il,0));
    layer->setr0(TVector3(0,0,trconstN::LayerZ[il]));//position in tracker
    for(int is=0;is<trconstN::nSide;is++){
      for(int ilad=0;ilad<trconstN::mLadder;ilad++){
        int ladid=trconstN::LadderId[il][is][ilad];
        int nsen=trconstN::nSensor[il][is][ilad];
        if(ladid==0||nsen<=0)continue;
        int tkid=ladid%1000;
        int ladtype=abs(ladid/1000);
        TkLadderN *ladder=layer->AddLadder(TkLadderN(tkid,ladtype));
        ladder->setr0(TVector3(trconstN::LadderXY[il][is][ilad][0],trconstN::LadderXY[il][is][ilad][1],0));//position in layer
        TRotation ladrt;
        ladrt.RotateX(-trconstN::LadderRtAngle[il][is][ilad][0]);//alpha
        ladrt.RotateY(-trconstN::LadderRtAngle[il][is][ilad][1]);//beta
        ladrt.RotateZ(-trconstN::LadderRtAngle[il][is][ilad][2]);//gamma
        ladder->setRt(ladrt);
        ladder->AddSensors(nsen);
      }
    }
  }
  UpdateModule();
//  LoadAlignPar();
  UpdateAllDetOUV(-1);//update all ouv
  return 0;
}


TkPlaneN *TkTrackN::GetModule(int detid){
  if(modules_.size()==0)UpdateModule();
  map<int, TkPlaneN*>::iterator it=modules_.find(detid);
  return (it==modules_.end())? 0: it->second;
}


TkLayerN *TkTrackN::GetLayer(int layid){
  map<int, TkLayerN>::iterator it=layers_.find(layid);
  return (it==layers_.end())? 0: &(it->second);
}


TkLadderN *TkTrackN::GetLadder(int tkid){
  TkLayerN *layer=GetLayer(getlayid(tkid));
  if(!layer)return 0;
  return layer->GetLadder(tkid);
}


TkSensorN *TkTrackN::GetSensor(int tkid,double ladchanx){
  TkLadderN *ladder=GetLadder(tkid);
  if(!ladder)return 0;
  double senschanx=-1;
  return ladder->GetSensor(ladchanx,senschanx);
}


AMSPlaneM TkTrackN::GetGlobalCoo(int tkid,double ladchanx,double ladchany,int ualign){
  TkLayerN *layer=GetLayer(getlayid(tkid));
  if(!layer){return AMSPlaneM(TVector2(-1,-1),getid(),-44);}//invalid layer
  return layer->GetGlobalCoo(tkid,ladchanx,ladchany,ualign); 
}


AMSPlaneM TkTrackN::GetGlobalCoo(int tkid,double daqchanx,int mult,double daqchany,int ualign){
  double ladchanx=TkLadderN::GetLadderChanX(daqchanx,mult);
  double ladchany=daqchany;
  return GetGlobalCoo(tkid,ladchanx,ladchany,ualign);
}


int TkTrackN::UpdateModule(){
  modules_.clear();
  int nm=0;
  for(map<int, TkLayerN>::iterator it=layers_.begin();it!=layers_.end();it++){//layer
    TkLayerN &layer=(it->second);
    layer.mother=this;
    modules_[layer.getid()]=&layer;
    nm++;
    for(map<int, TkLadderN>::iterator it1=layer.ladders_.begin();it1!=layer.ladders_.end();it1++){//ladder
      TkLadderN &ladder=(it1->second);
      ladder.mother=&layer;
      modules_[ladder.getid()]=&ladder;
      nm++;
      for(size_t i=0;i<ladder.sensors_.size();i++){//sensor
        TkSensorN &sensor=ladder.sensors_[i];
        sensor.mother=&ladder;
        modules_[sensor.getid()]=&sensor;
        nm++;
      }
    }
  }
  for(int ia=0;ia<nalig;ia++)modulea[ia].clear();
  return nm;
}


int TkTrackN::UpdateAllDetOUV(int ualign){//mother change: all daughters should change
  int iu=0;
  int level=-1;
  for(map<int, TkLayerN>::iterator it=layers_.begin();it!=layers_.end();it++){//layer
    TkLayerN &layer=(it->second);
    if(ualign>=0){level=1+ualign/100;}//without or with layer align
    layer.mother=this;
    layer.updateOUVFromMother(level);
    iu++;
    for(map<int, TkLadderN>::iterator it1=layer.ladders_.begin();it1!=layer.ladders_.end();it1++){//ladder
      TkLadderN &ladder=(it1->second);
      if(ualign>=0){level=11+ualign/10;}//without or with layer/ladder align
      ladder.mother=&layer;
      ladder.updateOUVFromMother(level);
      iu++;
      for(size_t i=0;i<ladder.sensors_.size();i++){//sensor
        TkSensorN &sensor=ladder.sensors_[i];
        if(ualign>=0){level=111+ualign;}//without or with layer/ladder/sensor align
        sensor.mother=&ladder;
        sensor.updateOUVFromMother(level);
        iu++;
      }
    }
  }
  return iu;
}


int TkTrackN::ClearAlignPar(int ia){//should be followed by UpdateAllDetOUV(-1)
  if(modules_.size()==0)UpdateModule();
  int iu=0;
  map<int, TkPlaneN*> *modules=(modulea[ia].size()>=1)?&(modulea[ia]):&modules_;
  for(map<int, TkPlaneN*>::iterator itm=modules->begin();itm!=modules->end();itm++){
    itm->second->cleardRdq(ia);
    iu++;
  } 
//  UpdateAllDetOUV(-1);//update all ouv
  modulea[ia].clear();
  return iu;
}


int TkTrackN::DefaultTDVVers[nalig]={7,2};//default tdv version

int TkTrackN::UseAlignVersion(int vers,int ua){
  int ca=0;
  for(int ia=0;ia<nalig;ia++){
    if(ua>=0){if(((ua/int(pow(10.,ia)))%10)==0)continue;}
    int uvers=vers;
    if(vers==0)uvers=DefaultTDVVers[ia];
    if(uvers==aligndb[ia].TDVVers)continue;//the same version,no need to initial
    if(ia==0)aligndb[ia].InitTDV("TkAlignN6",uvers,2*3000*6);//label/parmeter, should be <40000
    else     aligndb[ia].InitTDV("TkAlignExtN6",uvers,2+(2*3*6+4)*999+38);//L1/L9 dynamic label/parmeter (999*2min~1.3 days), should be <40000
    if(uvers==-1){//use no alignment (vers=-1 will set no alignment)
      aligndb[ia].Clear(12);
      ClearAlignPar(ia);
      UpdateAllDetOUV(-1);
      cout<<"TkTrackN::UseAlignVersion(New V6 alignment 2022) Use No Alignment for ia="<<ia<<endl;
    }
    else {
      ca++;
      cout<<"TkTrackN::UseAlignVersion(New V6 alignment 2022)="<<uvers<<" for ia="<<ia<<endl;
    }
  }
  return ca;
}


int TkTrackN::LoadAlignExtbiasCor(int ia,int force){
  if(force==0){
    if(aligndb[ia].TDVVers==aligndb[ia].abiasvers)return 0;
  }
  aligndb[ia].abiasvers=aligndb[ia].TDVVers;
  cout<<"TkTrackN::LoadAlignExtbiasCor Version="<<aligndb[ia].abiasvers<<" for ia="<<ia;
  aligndb[ia].aparbias.Clear();
//-----additional global bias correction only for external layer dynamic alignment ia=1
  if(ia==1){
    map<int, pair<TVector3,TVector3> > &abias=aligndb[ia].aparbias.apar;
    if(aligndb[ia].abiasvers==2){//L1/L9 dynamic alignment based on "sa2441qnobias1g5"
      /*(abias[0].first)[1]=0.8e-4;//l1,l1i(1.054e-4)
      (abias[8].first)[1]=2.89e-4;//l9,il9(3.56e-4)*/
      (abias[0].first)[0]=3.76e-4;  (abias[0].first)[1]=0.61e-4;   (abias[0].first)[2]=-9.16e-4;  //L1-shift[cm]
      (abias[0].second)[0]=1.1e-5;  (abias[0].second)[1]=1.9e-5;   (abias[0].second)[2]=-0.208e-5;//L1-rotation[rad]
      (abias[8].first)[0]=1.27e-4;  (abias[8].first)[1]=1.97e-4;   (abias[8].first)[2]=-19.78e-4; //L9-shift[cm]
      (abias[8].second)[0]=-8.7e-5; (abias[8].second)[1]=0.913e-5; (abias[8].second)[2]=0.459e-5; //L9-rotation[rad]
      cout<<" aparbias="<<(abias[0].first)[0]<<","<<(abias[0].first)[1]<<"...";
    }
    else if(aligndb[ia].abiasvers==3){//L1/L9 dynamic alignment based on "sa2843qnobias1g5b"
    }
  }
//-----
  cout<<endl;
  return 1;
}


int TkTrackN::UpdateAlignModule(unsigned int rtime,int ia,int force,int um){
  double ptime=double(rtime)+0.5;
  int pm=um;
  if(ia!=1&&pm<=-2)pm=-1;//additional time-dependent correction only for dynamic alignment ia=1
  int nu=aligndb[ia].UpdateAlignPar(ptime,force,pm);//update every second to the latest
  if((nu==0)&&(aligndb[ia].ppara[1].first!=aligndb[ia].ppara[0].first))nu=1;
  if((nu>0 )&&(aligndb[ia].ppara[1].second.apar==aligndb[ia].ppara[0].second.apar)){aligndb[ia].ppara[1].first=aligndb[ia].ppara[0].first;nu=0;}//check pameters futher
  if(nu<=0)return nu;//err or no update
  ClearAlignPar(ia);
  int nm=0;
  for(map<int,pair<TVector3,TVector3> >::iterator it=(aligndb[ia].ppara[0].second.apar).begin();it!=(aligndb[ia].ppara[0].second.apar).end();it++){
    int id=it->first;
    TkPlaneN *pmodule=GetModule(id);
    if(!pmodule){cerr<<"Error TkTrackN::LoadAlignModule module id="<<id<<" not exist"<<endl;nm=-2;continue;}
    TRotation dr;
    dr.RotateX(-((it->second).second)[0]);//alpha
    dr.RotateY(-((it->second).second)[1]);//beta
    dr.RotateZ(-((it->second).second)[2]);//gamma
    pmodule->dq[ia]=(it->second).first;
    pmodule->dR[ia]=dr;
    modulea[ia][id]=pmodule;//insert to modulea
    if(nm>=0)nm++;
  }
  aligndb[ia].ppara[1]=aligndb[ia].ppara[0];
  UpdateAllDetOUV(-1);//ouv with all alignment: time consuming
  return nm;
}


int TkTrackN::LoadAlignModulePar(const char *falign,unsigned int rtime,int ia,int force,unsigned int etimeb,int wopt,int um,bool biascor){
  int fstat=aligndb[ia].LoadAlignPar(falign,0,rtime,force,etimeb,wopt);//>=1 will require UpdateAlignModule
  if(fstat<0)return fstat;
  if(biascor)LoadAlignExtbiasCor(ia);
  return UpdateAlignModule(rtime,ia,0,um);
}


int TkTrackN::LoadAlignModuleTDV(unsigned int rtime,int ua,int isreal,int force,int um,bool biascor){
  int cstat=0;
  for(int ia=0;ia<nalig;ia++){
    if(ua>=0){if(((ua/int(pow(10.,ia)))%10)==0)continue;}
    int fstat=aligndb[ia].LoadAlignTDV(rtime,isreal,force);//>=1 will require UpdateAlignModule
    if(fstat<0){cstat=fstat;continue;}
    if(biascor)LoadAlignExtbiasCor(ia);
    int mstat=UpdateAlignModule(rtime,ia,0,um);
    if     (mstat<0) cstat=mstat;
    else if(cstat>=0)cstat+=mstat;
  }
  return cstat;
}


int TkTrackN::CopyAlignTDVModule(const TkTrackN *other,unsigned int rtime,int um,bool biascor){
  if(other==0||this==other){return -10;}
  int cstat=0;
  for(int ia=0;ia<nalig;ia++){
    int nc=aligndb[ia].CopyAlignTDV(other->aligndb[ia]);
    int mstat=0;
    if(aligndb[ia].TDVVers<0){
      if(nc>=1){//has update
        mstat=ClearAlignPar(ia);
        UpdateAllDetOUV(-1);
      }
    }
    else {
      if(biascor)LoadAlignExtbiasCor(ia);
      mstat=UpdateAlignModule(rtime,ia,0,um);
    }
    if     (mstat<0) cstat=mstat;
    else if(cstat>=0)cstat+=mstat;
  }
  return cstat;
}
