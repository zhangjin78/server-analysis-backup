#include "TrCluster.h"
#include "tkdcards.h"
#include "VCon.h"
#include "root.h"
#include "TkDBcN.h"

ClassImp(TrClusterR);


TrCalDB* TrClusterR::_trcaldb = NULL;
TrParDB* TrClusterR::_trpardb = NULL;


// asymmetry determined from Muons 2011 analysis
float TrClusterR::Asymmetry[2] = {0.040,0.005};

// the default corrections are used in reconstruction and are independent from calibration (overridden by datacard)
int   TrClusterR::DefaultBestResidualOpt  = -1;
int   TrClusterR::DefaultCorrOpt          = TrClusterR::kAsym|TrClusterR::kAngle; 
int   TrClusterR::DefaultLinearityCorrOpt = TrClusterR::kAsym|TrClusterR::kAngle|TrClusterR::kGain|TrClusterR::kPStrip;

int   TrClusterR::LinearUsedStrips=-1;

// measurement corrections (this settings are modified in root.C)
int   TrClusterR::DefaultChargeCorrOpt = TrClusterR::kAsym|TrClusterR::kGain|TrClusterR::kLoss|TrClusterR::kMIP|TrClusterR::kAngle|TrClusterR::kBeta|TrClusterR::kRigidity;
int   TrClusterR::DefaultEdepCorrOpt   = TrClusterR::kAsym|TrClusterR::kGain|TrClusterR::kLoss|TrClusterR::kMIP|TrClusterR::kMeV;

int   TrClusterR::DefaultUsedStrips = -1;     // -1: inclination dependent
float TrClusterR::TwoStripThresholdX = 0.70;  // tan(35deg)
float TrClusterR::TwoStripThresholdY = 0.36;  // tan(20deg)


TrClusterR::TrClusterR(void):mcZ(0) {
  clustercharge.Init();
  Clear();
  
}


TrClusterR::TrClusterR(const TrClusterR &orig):TrElem(orig) {
   mcZ=orig.mcZ;
  _tkid    = orig._tkid;
  _address = orig._address;
  _nelem   = orig._nelem;
  _seedind = orig._seedind;
  for (int i = 0; i<_nelem; i++) _signal.push_back(orig._signal.at(i));
  Status  = orig.Status;
  _mult    = orig._mult;
  _dxdz    = orig._dxdz;
  _dydz    = orig._dydz;
  _qtrk    = orig._qtrk;

  clustercharge=orig.clustercharge;
}


TrClusterR::TrClusterR(int tkid, int side, int add, int seedind, unsigned int status):mcZ(0) {
  clustercharge.Init();
  Clear();

  _tkid    = tkid;
  _address = add;
  _seedind = seedind;
  Status  = status;
}


TrClusterR::TrClusterR(int tkid, int side, int add, int nelem, int seedind, 
                           float* adc, unsigned int status):mcZ(0)  {
  clustercharge.Init();
  Clear();
  _tkid    =  tkid;
  _address =  add;
  if ( ((add+nelem-1)>=1024)&&(!TkDBc::Head->FindTkId(tkid)->IsK7()) ) {
    _nelem=1024-add;
    cerr<<"TrClusterR::TrClusterR-W You are tring to create a Cluster with address "<< add<<" and length "<<nelem<<endl;
    cerr<<"                         Cluster has been truncated to the physical limit"<<endl;
  }
  else
  _nelem   =  nelem;
  _seedind =  seedind;
  _signal.reserve(_nelem);
  for (int i = 0; i<_nelem; i++) _signal.push_back(adc[i]);
  Status  = status;
  _mult    = 0;
}


TrClusterR::~TrClusterR() {
  Clear();
}

TrRawClusterR *TrClusterR::pRawCluster(int idis){
  vector<TrRawClusterR *> raws;
  VCon *cont = GetVCon()->GetCont("AMSTrRawCluster");
  if(!cont)return 0;
  for(int i=0;i<cont->getnelem();i++){
    TrRawClusterR *raw=(TrRawClusterR *)cont->getelem(i);
    if(raw->GetTkId()!=GetTkId()||raw->GetSide()!=GetSide())continue;//same ladder and side
    raws.push_back(raw);
  }
  delete cont;
  std::sort(raws.begin(),raws.end(),TrRawClusterR::CompId);
//---match seed strip in the rawcluster
  int seedadd=GetSeedAddress();
  int ii=0;//seed pos
  for( ;ii<int(raws.size());ii++){
    TrRawClusterR *raw=raws.at(ii);
    if(seedadd>=raw->GetAddress()&&seedadd<=raw->GetAddress()+raw->GetNelem()-1)break;//found
  }
  if(ii>=int(raws.size())||std::abs(idis)>=raws.size())return 0;
//---search nearby rawcluster
  int ip=ii+idis;
  if(IsK7()){
    if(ip<0||ip>=int(raws.size()))return 0;
/*    if     (ip<0)ip+=raws.size();//special treatment for cyclicity
    else if(ip>=raws.size())ip-=raws.size();*/
  }
  else {
    if(ip<0||ip>=int(raws.size()))return 0;
  }
  return raws.at(ip);
}

int TrClusterR::GetMultiplicity()  {
  if (_mult==0) _mult = TkCoo::GetMaxMult(GetTkId(),GetAddress())+1; // maybe better to use GetSeedAddress();
  return _mult;
}

int TrClusterR::GetMultSeedIndex(){
  int multseed_index=GetSeedIndex();//imult: multiplicity-seed is defined by old cofg algorithm with tracker finding, can be changed
  return multseed_index; 
}

int TrClusterR::GetResolvedMultiplicity(int ii, int mult, int multseed_index){
  int imult=mult;
  if((GetSide()==1)||(!IsK7()))return imult;//Y or K5
  //-----X K7
  if(multseed_index<0)multseed_index=GetMultSeedIndex();
  int multseed_address=GetAddress(multseed_index);
  int maxmultseed=TkCoo::GetMaxMult(GetTkId(),multseed_address);//max mult for seed
  if(imult<0)          imult=0;
  if(imult>maxmultseed)imult=maxmultseed;//prevent go out
  int address=GetAddress(ii);
  if     (ii<multseed_index&&address>multseed_address)imult--;
  else if(ii>multseed_index&&address<multseed_address)imult++;
  //-----
  int maxmult=TkCoo::GetMaxMult(GetTkId(),address);
  if(imult<0)      imult=-2;//error mult underflow
  if(imult>maxmult)imult=-3;//error mult overflow
  return imult;
}


float TrClusterR::GetNoise(int ii) {
  if (_trcaldb==0) { printf("TrClusterR::GetNoise-E no _trcaldb specified.\n"); return -9999.; }
  int tkid = GetTkId();
  TrLadCal* ladcal = GetTrCalDB()->FindCal_TkId(tkid);
  if (!ladcal) { printf ("TrClusterR::GetNoise-W calibration not found.\n"); return -9999; }
  int address = GetAddress(ii); 
  return (float) ladcal->GetSigma(address);
}


short TrClusterR::GetStatus(int ii) {
  if (_trcaldb==0) {
    printf("TrClusterRs::GetStatus Error, no _trcaldb specified.\n");
    return -1; 
  }
  int tkid = GetTkId();
  TrLadCal* ladcal = GetTrCalDB()->FindCal_TkId(tkid);
  if (!ladcal) {printf ("TrClusterR::GetNoise, WARNING calibration not found!!\n"); return -9999;}
//  int address = _address+ii;
  int address = GetAddress(ii);//bug fixed by QY
  return (short) ladcal->GetStatus(address);
}


int TrClusterR::GetSensorAddress(int& sens, int ii, int mult, int multseed_index, int copt, int verbose) {

  if(copt>=2){//V6 version
    double senschan;
    TkSensorN *sensor=GetSensorChan(senschan,mult,ii,-1,multseed_index);
    sens=sensor?sensor->getid():-1;
    return senschan;
  }
  else {
    int tkid = GetTkId();
    bool isK7 = IsK7();
    int side = GetSide();
    if(multseed_index<0)multseed_index=GetMultSeedIndex();
    // it could happen that multiplicity exceeds by one the max (only in case of cluster on the last sensor of K7)
    int multseed_address = GetAddress(multseed_index);
    int max_mult = TkCoo::GetMaxMult(tkid,multseed_address);
    if ( isK7&&((mult-max_mult)==1) ) mult = max_mult;
    // mult, by XCofG method convention, is referred to multseed strip
    int multseedadd = GetAddress() + multseed_index; // use seed_address instead?!
    // here I convert multiplicity of seed to multiplicity of first strip
    if ( (side==0)&&isK7&&(multseedadd>1023) ) mult--; // if multseed > left margin the mult of first strip is mult-1
    if ( (side==0)&&isK7&&(multseedadd< 640) ) mult++; // if multseed < right margin the mult of first strip is mult+1 (this case is possible only if ii<0 & K7)
    // now take the address without regarding of cyclicity accounted in the following algorithm
    int address = GetAddress() + ii;
    // detect multiplicity jump in case of K7
    while ( (side==0)&&isK7&&(address>1023) ) { address -= 384; mult++; }
    while ( (side==0)&&isK7&&(address< 640) ) { address += 384; mult--; }
    if ( ( (side==0)&&( (address<640)||(address>1023) ) ) ||
       ( (side==1)&&( (address<  0)||(address> 639) ) ) ) {
      if (verbose>0) printf("TrClusterR::GetSensorAddress-W address out of bounds (%d) for side %1d. Return -5.\n",address,side);
      return -5;
    }
    return TkCoo::GetSensorAddress(tkid,address,mult,sens,verbose);
  }
}


void TrClusterR::Clear() {
  _tkid    =   0;
  _address =  -1;
  _nelem   =   0;
  _seedind =   0;
  _signal.clear();
  Status   =   0;
  _mult    =   0;
  _dxdz    =   0;  // vertical inclination by default 
  _dydz    =   0;  // vertical inclination by default 
  _qtrk    =   1;  // charge 1 as default 
}


void TrClusterR::push_back(float adc) {
  _signal.push_back(adc);
  _nelem = (int) _signal.size();
}


float TrClusterR::GetGCoord(int imult)  {
  float lcoo = GetXCofG(DefaultUsedStrips,imult);
  if (GetSide() == 0) return TkCoo::GetGlobalA(_tkid, lcoo, TkDBc::Head->_ssize_active[1]/2).x();
  else                return TkCoo::GetGlobalA(_tkid, TkDBc::Head->_ssize_active[0]/2, lcoo).y();
}


double TrClusterR::GetLadderChan(int imult, double ii, int multseed_index){
  if(GetSide()==0){
    if(imult<0)return imult;
    if(multseed_index<0)multseed_index=GetMultSeedIndex();//imult: multiplicity-seed is defined by old cofg algorithm with tracker finding
    double ladchanx=ii-multseed_index+GetAddress(multseed_index)-trconstN::LadderChannel[1]+imult*trconstN::LadderChannel[0];
    if(ladchanx<-0.5)ladchanx=-2;//underflow portection, avoid -10
    return ladchanx;
  }
  else {return ii+GetAddress();}
}


TkLadderN *TrClusterR::GetLadder(double &ladchan, int imult, int ii, float chrg, float dxdz, float dydz, int multseed_index){
  double ci=ii;
  if     (ii==-1)ci=GetXCofG(DefaultUsedStrips,imult,DefaultBestResidualOpt,chrg,dxdz,dydz,2);//strip index of CofG
  else if(ii==-2)ci=GetXCofG(DefaultUsedStrips,imult,DefaultBestResidualOpt,chrg,dxdz,dydz,3);//strip index of seed strip 
  ladchan=GetLadderChan(imult,ci,multseed_index);
  return GetLadder();
}


int TrClusterR::nSensor(int ii, double xaddress, int multseed_index){
  TkLadderN *ladder=GetLadder(); 
  if(GetSide()==0){
    if(ii==-1||ii==-2)ii=GetXCofG(DefaultUsedStrips,0,DefaultBestResidualOpt,0,-2,-2,3);//strip index of seed strip
    double daqchanx=GetAddress(ii);
    int mmult=ladder->GetMaxMult(daqchanx);
    if(multseed_index<0)multseed_index=GetMultSeedIndex();
    if     (multseed_index<ii&&GetAddress(multseed_index)>daqchanx)mmult--;//multiplicity is defined by multseed_index, multseed on left
    else if(multseed_index>ii&&GetAddress(multseed_index)<daqchanx)mmult++;//multiplicity is defined by multseed_index, multseed on right
    return mmult+1;
  }
  else if(xaddress>=0){//xaddress is only useful for YCluster
    int mmult=ladder->GetMaxMult(xaddress);
    return mmult+1;
  }
  else {
    return ladder->nSensors();
  }
}


TkSensorN *TrClusterR::GetSensorChan(double &senschan, int isen, double ii, double xaddress, int multseed_index){
  int imult=isen; 
  double ladchan=GetLadderChan(imult,ii,multseed_index);
  TkLadderN *ladder=GetLadder();
  if(GetSide()==0){//XCluster
    return ladder->GetSensor(ladchan,senschan);
  }
  else {//YCluster
    senschan=ladchan;//sensor channel is the same as ladchan for Y side
    if(xaddress>=0){
      double ladchanx=ladder->GetLadderChanX(xaddress,imult);//ladchan for seed strip
      double senschanx;
      return ladder->GetSensor(ladchanx,senschanx);
    }
    else {
      return ladder->pSensor(isen);//imult is not enough to distinguish the sensor for Y side: 1 imult~2 sensor
    }
  }
}


TkSensorN *TrClusterR::GetSensor(double &senschan, int isen, int ii, float chrg, float dxdz, float dydz, double xaddress, int multseed_index){
  double ci=ii;
  if     (ii==-1)ci=GetXCofG(DefaultUsedStrips,isen,DefaultBestResidualOpt,chrg,dxdz,dydz,2);//strip index of CofG
  else if(ii==-2)ci=GetXCofG(DefaultUsedStrips,isen,DefaultBestResidualOpt,chrg,dxdz,dydz,3);//strip index of seed strip 
  return GetSensorChan(senschan,isen,ci,xaddress,multseed_index);
}


AMSPlaneM TrClusterR::GetGCoordN(int isen, int ii, int ualign, bool dzshiftondxy, float chrg, float dxdz, float dydz, double xaddress, int multseed_index){
  int    ixy=GetSide();
  int    imult=isen;
  double ladchan[2]={-10,-10};//ladder center 
  TkLadderN *ladder=GetLadder(ladchan[ixy],imult,ii,chrg,dxdz,dydz,multseed_index);
  if(ixy==0&&imult>0&&ladchan[ixy]>ladder->nchans[0]-0.5){//try one more again with X overflow protection
    imult--;
    ladder=GetLadder(ladchan[ixy],imult,ii,chrg,dxdz,dydz,multseed_index);
  }
  else if(ixy==0&&imult==0&&ladchan[ixy]<-0.5){//try one more with X underflow protection
    imult++;
    ladder=GetLadder(ladchan[ixy],imult,ii,chrg,dxdz,dydz,multseed_index);
  }
  if(ixy==1){//Y cluster
    if(xaddress>=0)ladchan[0]=ladder->GetLadderChanX(xaddress,imult);
    else {
      TkSensorN *sensor=ladder->pSensor(isen);
      ladchan[0]=sensor?sensor->GetInLadderChanX(-10):-1;//sensor center strip at X or fatal error
    }
  }
  AMSPlaneM plm=GetTkTrack()->GetGlobalCoo(GetTkId(),ladchan[0],ladchan[1],ualign);
  plm.setMCov(1-ixy,1-ixy,0.);
  if((plm.getMCov())[ixy][ixy]<=0)plm.setMStat(-11);//set fatal error if no effective measurement
  if(plm.getMStat()<=-11)return plm;//fatal error
  // Apply MCtune without smearing
  if(TRMCFFKEY.MCtuneDmax>0){
    TVector2 ms=plm.getM(); 
    if(ii==-1&&TRMCFFKEY.MCtuneDs[ixy]!=0){//XY isolated
      AMSPlaneM pls=plm;
      MCtune(pls,GetTkId(),TRMCFFKEY.MCtuneDmax,TRMCFFKEY.MCtuneDs[ixy],ixy);//local X or Y
      ms+=(pls.getM()-plm.getM());
    }
    int jlayer=GetLayerJ();
    int il=(jlayer==1)?0:(jlayer==9?1:-1);
    if(il>=0&&(TRMCFFKEY.OuterSmearingN[il][0]!=0||TRMCFFKEY.OuterSmearingN[il][1]!=0)){//XY correlated
      AMSPlaneM pls=plm;//External layer misalignment
      MCSmearExtAlign(pls,jlayer-1,TRMCFFKEY.MCtuneDmax,2,TRMCFFKEY.OuterSmearingN[il],0);//opt=1 is more correct
      ms+=(pls.getM()-plm.getM());
    }
    plm.setM(ms);
  }
  // Apply Zshift correction
  if(ii==-1&&dzshiftondxy){
    double dzondxy[2]={0};
    dzondxy[ixy]=GetdZShiftondXY(chrg,dxdz,dydz,3);
    if(TRFITFFKEY.Zshift>=0)plm.moveM(dzondxy[0],dzondxy[1]);//XYlocal move
  }
  return plm;
}


AMSPlaneM TrClusterR::FindMult(double &dist, int &mult, double &xaddress, TVector3 gcoo, TVector3 gdir, int ii, float chrg, int multseed_index){
  dist=2.E100;
  mult=-2;
  xaddress=0;
  int ixy=GetSide();
  if(ii==-1||ii==-2)ii=GetXCofG(DefaultUsedStrips,0,DefaultBestResidualOpt,0,-2,-2,3);//strip index of seed strip
  double daqchanx=-2,daqchany=-2;
  if(ixy==0)daqchanx=GetAddress(ii);
  else      daqchany=GetAddress(ii);
  TkLadderN *ladder=GetLadder();
  TkSensorN *sensor=ladder->FindSensor(dist,gcoo,gdir,1,daqchanx,daqchany);//Find the closest sensor
  TVector2 ploc=sensor->straightLineToPlane(gcoo,gdir);//trajectory local coordinate in sensor
//-----
  double chanx=-1,chany=-1;
  if(ixy==0){//XCluster
    xaddress=daqchanx;
    chanx=sensor->GetSensorChanXFromDAQ(daqchanx,mult);//chanx measurement and the resolved multiplicity 
    double disty=sensor->FindLocalChan(chany,1,ploc.Y());//find closest chany in sensor
    if(multseed_index<0)multseed_index=GetMultSeedIndex();//multiplicity-seed is defined by old cofg algorithm with tracker finding
    if(multseed_index<ii&&GetAddress(multseed_index)>daqchanx){//multiplicity is defined by multseed_index, multseed on left
      if(mult==0){//deal with multiplicity underflow
        mult++;//manually increase multiplicity
        double ladchanx=ladder->GetLadderChanX(daqchanx,mult);//new ladchanx
        sensor=ladder->GetSensor(ladchanx,chanx);//new sensor and chanx measurement
        ploc=sensor->straightLineToPlane(gcoo,gdir);//new trajectory local coordinate in sensor
        disty=sensor->FindLocalChan(chany,1,ploc.Y());//try again to find closest chany
      }
      mult--;
    }
    else if(multseed_index>ii&&GetAddress(multseed_index)<daqchanx)mult++;//multiplicity is defined by multseed_index, multseed on right
    daqchany=chany; 
  }
  else {//YCluster
    chany=daqchany;//chany measurement
    double distx=sensor->FindLocalChan(chanx,0,ploc.X());//find closest chanx in sensor
    daqchanx=sensor->GetDAQChanX(chanx,mult);//daqchanx and the resolved multiplicity
    xaddress=daqchanx;
  }
  AMSPlaneM plm=sensor->GetLocalCoo(chanx,chany);//the closest possible measurement
  plm.setMCov(1-ixy,1-ixy,0.);
  plm.set(sensor->getO(),sensor->getU(),sensor->getV());
  plm.setD(gdir);
  if(ii==-1){
    TVector3 dloc=plm.getDLocal();
    float dxdz=(dloc[2]!=0)?dloc[0]/dloc[2]:0;
    float dydz=(dloc[2]!=0)?dloc[1]/dloc[2]:0;
    plm=GetGCoordN(mult,ii,111,1,chrg,dxdz,dydz,xaddress,multseed_index);
    plm.setD(gdir);
    ploc=plm.straightLineToPlane(gcoo,gdir);
  }
  plm.setPLocal(TVector3(ploc.X(),ploc.Y(),0.));//track trajectory position in sensor
  dist=plm.getMPdist(1).Mag();//distance of the residual
  return plm;
}


void TrClusterR::Print(int opt) { 
  cout << _PrepareOutput(opt);
}


std::string TrClusterR::_PrepareOutput(int opt){
  std::string sout;
  sout.append(Form("TkId: %5d  Side: %1d  Address: %4d  Nelem: %3d  Status: %3d  Signal(ADC): %10.3f  Edep(MeV): %8.3f  QStatus: %8X\n",
    GetTkId(),GetSide(),GetAddress(),GetNelem(),getstatus(),GetTotSignal(0),GetEdep(),GetQStatus()));
  if(!opt) return sout;
  if (opt>1) {
    int strip = -1;
    int address = GetAddress(strip);
    if ( (GetSide()==0)&&(address>1023) ) address = -1;
    if ( (GetSide()==1)&&(address> 639) ) address = -1;
    if (address>0) 
      sout.append(Form("Address: %4d                      Sigma: %10.5f                   Status: %3d\n",
        address,GetSigma(strip),GetStatus(strip)));                      
  }
  for (int ii=0; ii<GetNelem(); ii++) {
    sout.append(Form("Address: %4d  Signal: %10.5f  Sigma: %10.5f  S/N: %10.5f  Status: %3d  ",
      GetAddress(ii),GetSignal(ii,0),GetSigma(ii),GetSN(ii,0),GetStatus(ii)));
    if (ii==GetSeedIndex()) sout.append("<<< SEED\n");
    else sout.append(" \n");
  }
  if (opt>1) {
    int strip = GetNelem();
    int address = GetAddress(strip);
    if ( (GetSide()==0)&&(address>1023) ) address = -1;
    if ( (GetSide()==1)&&(address> 639) ) address = -1;
    if (address>0)
      sout.append(Form("Address: %4d                      Sigma: %10.5f                   Status: %3d\n",
        address,GetSigma(strip),GetStatus(strip))); 
  }
  return sout;
}


const char* TrClusterR::Info(int iRef){
  string aa;
  aa.append(Form("TrCluster #%d ",iRef));
  aa.append(_PrepareOutput(0));
  unsigned int len=MAXINFOSIZE;
  if(aa.size()<len) len=aa.size();
  strncpy(_Info,aa.c_str(),len+1);
  return _Info;
}


std::ostream &TrClusterR::putout(std::ostream &ostr) {
  return ostr << _PrepareOutput(1) << std::endl;
}


bool  TrClusterR::IsOverflowADC(int ii,int opt){
  if(GetSide()==1||(opt&kOverflow)==0)return 0;//p-strip or nooverflow
//-------n-strip
  int tkid=GetTkId();
  int jlayer = GetLayerJ();
  float radc=_signal.at(ii);
//-------
  bool isoverflow=0;
  if     (tkid==811)isoverflow=(radc>1400);//special-ladder
  else if(tkid==-310)isoverflow=(radc>2334);//special-ladder
  else if(jlayer==1)isoverflow=(radc>2667);//L1-ladder
  else              isoverflow=(radc>2840);//other-ladder
  return isoverflow;
}


float TrClusterR::GetSignal(int ii, int opt, int zgain) {
  double signal = _signal.at(ii);
  int tkid = GetTkId();
  int address = GetAddress(ii);
  int iva = address/64;

  bool isoverflow=IsOverflowADC(ii,opt);

  /////////////////////
  // asymmetry
  /////////////////////

  // p/n-side new asymmetry 2017 calibration(QY)
  if ( (kAsymEta&opt)&&!isoverflow ){//overflow-strip no asymmetry correction
    double lsignal=(ii-1>=0)?_signal.at(ii-1):0;
    double rsignal=(ii+1<GetNelem())?_signal.at(ii+1):0;
    if(GetSide()==0)signal=TrLinearXEtaDB::GetHead()->GetAsymCorADC(signal,lsignal,rsignal,tkid);
    else            signal=TrLinearEtaDB:: GetHead()->GetAsymCorADC(signal,lsignal,rsignal,tkid);
  }
  // multiplexing effect
  else if ( (kAsym&opt)&&(ii>0)&&!isoverflow  )    signal = signal - _signal.at(ii-1)*GetAsymmetry(GetSide());
  // asymmetry for MC 
  else if ( (kSimAsym&opt)&&(ii>0)&&!isoverflow ) signal = signal - _signal.at(ii-1)*TRMCFFKEY.Asymmetry[GetSide()];   

  /////////////////////
  // gain
  /////////////////////

  if (kGain&opt) signal = TrGainDB::GetHead()->GetGainCorrected(signal,tkid,iva); 
  // p-side ADC->Q2(+gain2 VA-equalization)
  if ( (kQ2Eta&opt)&&(GetSide()==1) ) signal = TrLinearEtaDB::GetHead()->GetLinearQ2(signal,tkid,address,zgain); 
  // p-side linearization 
  else if ( (kPStrip&opt)&&(GetSide()==1) ) signal = TrLinearDB::GetHead()->GetLinearityCorrected(signal,tkid,iva); 
   
  return signal;
}


int TrClusterR::GetAddress(int ii) {
  int address = GetAddress() + ii;
  int side    = GetSide();
  // cyclical definition, in order to avoid errors and manage the K7 clusters
  if (side == 1) {
    if (address >= 0) {
      if (address < 640) return address;       // address in range
      else               return address - 640; // address >= 640
    }
    else                 return address + 640; // address < 0
  }
  else {
    if (address >= 640) {
      if (address < 1024) return address;       // address in range
      else                return address - 384; // address >= 1024
    }
    else                  return address + 384; // address < 640
  }
  return 0;
}


int TrClusterR::GetSeedSecondIndex(int &seed_index,int &second_index,int opt) {

//-------overflow treatment(max-number overflow strip-group search)
  int maxover=0;
  int pover=0;//cluster over
  int rover=-1,lover=-1;
  for(int ii=0; ii<=GetNelem(); ii++){//start
    bool isoverflow=(ii==GetNelem())?0:IsOverflowADC(ii,opt);
    if(isoverflow)pover++;//overflow
    else  {
      if(pover>maxover){maxover=pover;rover=ii-1;lover=rover+1-maxover;}//max-found
      pover=0;//initial setting
    }
  }


//-------
  second_index=-1;

  if (maxover==0) {//no overflow
    if(opt==kNoCorr)seed_index=_seedind;
    else {
      float maxadc  = -9999.;
      for (int ii=0; ii<GetNelem(); ii++) {
        float signal = GetSignal(ii, opt);
        if (signal > maxadc) {
          maxadc  = signal;
          seed_index = ii;
       }
      }
    }
    float lsignal=(seed_index-1<0)           ?0:GetSignal(seed_index-1, opt);
    float rsignal=(seed_index+1>GetNelem()-1)?0:GetSignal(seed_index+1, opt);
    if(lsignal<=0&&rsignal<=0)second_index=-1;
    else if  (rsignal>lsignal)second_index=seed_index+1;
    else                      second_index=seed_index-1;
  }
  else {//has overflow
    seed_index=(lover+rover)/2;
    float lsignal=(lover-1<0)?           0:GetSignal(lover-1, opt);
    float rsignal=(rover+1>GetNelem()-1)?0:GetSignal(rover+1, opt);//compare non-satureated rr/ll stirps
    if(lsignal<=0&&rsignal<=0&&rover!=lover){
      lsignal=GetSignal(lover, opt);
      rsignal=GetSignal(rover, opt);//compare satureated r/l stirps
    }
    if (lsignal<=0&&rsignal<=0)second_index=-1;
    else if((lover+rover)%2==0){//odd saturation
       if(rsignal>lsignal)second_index=seed_index+1;
       else               second_index=seed_index-1;
    }
    else {//even saturation
       second_index=seed_index+1;
       if(rsignal>lsignal){seed_index++;second_index--;}
    }
  }
 
  return maxover;
}

int TrClusterR::GetSeedIndex(int opt) {
  int seed_index,second_index;
  GetSeedSecondIndex(seed_index,second_index,opt);
  return seed_index;
}


int TrClusterR::GetSeedSecondPairType(int mult, int multseed_index, int opt, int copt){

//-----find seed/second index
  int seed_index,second_index;
  GetSeedSecondIndex(seed_index,second_index,opt);
  if(copt>=2){//V6 version
    if(GetSide()==0){
      double ladchan=GetLadderChan(mult,seed_index,multseed_index);
      if     (ladchan<-0.5)mult++;//underflow protection
      else if(ladchan>GetLadder()->nchans[0]-0.5)mult--;//overflow protection
    } 
  }
  else {
    if(GetResolvedMultiplicity(seed_index,mult,multseed_index)==-3)mult--;//QY K7 new seed address overflow
  }
  int seed_sensor,second_sensor=-1;
//------
  int seed_add=GetSensorAddress(seed_sensor,seed_index,mult,multseed_index,copt);
  int second_add=(second_index>=0)?GetSensorAddress(second_sensor,second_index,mult,multseed_index,copt):-1;

//-----type
  int ninters=0;//intermediate strips
  int type=0;
  if(GetSide()==1){//Y
    if(seed_add==0||second_add==0||seed_add==639||second_add==639)ninters=7;
    else ninters=3;
    type=1;
    if     (seed_add==0)    type+=1000; //seed-hit sensor edge0 
    else if(seed_add==639)  type+=2000; //seed-hit sensor edge1
    if     (second_add==0)  type+=10000;//second-hit sensor edge0 
    else if(second_add==639)type+=20000;//second-hit sensor edge1
  }
  else if((GetSide()==0)&&(!IsK7())){//X-K5
    if(seed_add==191||second_add==191)ninters=2;
    else                              ninters=1;
    type=2;
    if     (seed_add==0)    type+=1000;//seed-hit sensor edge0 
    else if(seed_add==191)  type+=2000;//seed-hit sensor edge1
    if     (second_add==0)  type+=10000;//second-hit sensor edge0 
    else if(second_add==191)type+=20000;//second-hit sensor edge1
  }
  else if((GetSide()==0)&&(IsK7())){//X-K7
    if(seed_add<=63||second_add<=63){
      if(second_add>=0&&(seed_add+1)/2==(second_add+1)/2)ninters=0;//the same  group(0 strip middle)
      else                                               ninters=1;//different group(1 strip middle)
      type=14;
    }
    else if(second_add<=159||second_add<=159){
      ninters=1;//1strip middle
      type=13;
    }
    else {//0-223
      if(second_add>=0&&(seed_add+1)/2==(second_add+1)/2)ninters=0;//the same  group(0 strip middle)
      else                                               ninters=1;//different group(1 strip middle)
      type=14;
    }
    if     (seed_add==0)    type+=1000; //seed-hit sensor edge0 
    else if(seed_add==223)  type+=2000; //seed-hit sensor edge1
    if     (second_add==0)  type+=10000;//second-hit sensor edge0 
    else if(second_add==223)type+=20000;//second-hit sensor edge1
  }
  type+=ninters*100;//intermediate strips number
  if     (second_add<0)              type+=100000;//no secondary-strip
  else if(second_sensor!=seed_sensor)type+=200000;//second-strip not the same sensor as the seed-strip(split)
  return type;
}


float TrClusterR::GetTotSignal(int opt, float beta, float rigidity, float mass_on_Z, int res_mult, float dxdz, float dydz) {
  float dxdzr=GetDxDz();//backup
  float dydzr=GetDyDz();//backup
  if(dxdz!=-2)SetDxDz(dxdz);
  if(dydz!=-2)SetDyDz(dydz);
  // switch to set charge calibration in data/MC 
  set_charge_calibration_tracker(this);
  // total signal with signal corrections
  float sum = 0.;
  for (int ii=0; ii<GetNelem(); ii++) {
    float signal = GetSignal(ii,opt);
    if (signal>0) sum += signal;
  }
  /* maybe better
  for (int ii=0; ii<GetNelem(); ii++) { 
    float signal = GetSignal(ii,TrClusterR::kAsym); 
    if (signal>0) sum += signal; 
  }     
  if (kGain&opt) sum = TrGainDB::GetHead()->GetGainCorrected(sum,GetTkId(),int(GetSeedAddress()/64));
  */
  // old correction scheme (no longer in use)
  if ( (kOld&opt)&&(!(kTotSign2017&opt)) ) {
    // if (kLoss&opt) sum *= GetTrParDB()->GetChargeLoss(GetSide(),GetCofG(DefaultUsedStrips,opt),GetImpactAngle()); // correction no longer in use
    if (kLoss&opt) sum = TrChargeLossDB::GetHead()->GetChargeLossCorrectedValue(GetSide(),GetCofG(DefaultUsedStrips,TrClusterR::kAsym|TrClusterR::kAngle),GetImpactAngle(),sum*GetCosTheta())/GetCosTheta();
    // if (kPN&opt) sum = ConvertToPSideScale(sum); // correction no longer in use
    if (kMIP&opt) sum = GetNumberOfMIPs(sum);
    if (kAngle&opt) sum *= GetCosTheta();
    float betagamma_corr = 1;
    if ((kBeta&opt)&&(kRigidity&opt)) betagamma_corr = BetaRigidityCorrection(beta,rigidity,mass_on_Z);
    else if (kBeta&opt) betagamma_corr = BetaCorrection(beta);
    else if (kRigidity&opt) betagamma_corr = RigidityCorrection(rigidity,mass_on_Z);
    if (betagamma_corr<=0.) betagamma_corr = 1;
    sum /= betagamma_corr;
    if (kMeV&opt) sum = sum*0.081; // 81 keV per MIP 
  }
  // current correction scheme 
  else if (!(kTotSign2017&opt)) {
    // correct first for angle
    sum *= GetCosTheta();   
    // charge loss correction
    if (kLoss&opt) {
      int interstrip = (res_mult>=0) ? GetNInterstrip(res_mult) : -1; 
      sum = TrChargeLossDB::GetHead()->GetChargeLossCorrectedValue(GetSide(),interstrip,GetCofG(DefaultUsedStrips,TrClusterR::kAsym|TrClusterR::kAngle),GetImpactAngle(),sum);
    } 
    // beta/rigidity correction
    if ( (kBeta&opt)&&(kRigidity&opt)&&(fabs(rigidity)>1e-6) ) 
      sum = pow(TrEDepDB::GetHead()->GetEDepCorrectedValue(GetLayerJ(),sqrt(sum),beta,rigidity,mass_on_Z,GetSide(),1),2.); 
    else if (kBeta&opt)
      sum = pow(TrEDepDB::GetHead()->GetEDepCorrectedValue(GetLayerJ(),sqrt(sum),beta,0,0,GetSide(),1),2.); 
    else if (kMIP&opt) // if no correction requested, but MIP scale wanted, apply relativistic particles normalization 
      sum = pow(TrEDepDB::GetHead()->GetEDepCorrectedValue(GetLayerJ(),sqrt(sum),1,0,0,GetSide(),1),2.);
    // MIP normalization
    if (kMIP&opt) {
      int iva = int(GetSeedAddress()/64);
      sum = pow(TrMipDB::GetHead()->GetMipCorrectedValue(sqrt(sum),GetTkId(),iva,3),2);
    }
    // go back with angle if not requested
    if (!(kAngle&opt)) sum /= GetCosTheta();
    if (kMeV&opt) sum = sum*0.081; // 81 keV per MIP 
  }
  // 2017 correction scheme (used only for MC with build>1106, for now) 
  else {
    // conversion ADC -> Z
    if (kSimSignal&opt) {
      int iva = int(GetSeedAddress()/64);
      sum = pow(TrSimSignalDB::GetHead()->GetValue(sqrt(sum),GetTkId(),iva,true),2);
    }
    // charge loss maps    
    if (kLoss&opt) {
      int interstrip = (res_mult>=0) ? GetNInterstrip(res_mult) : -1;
      sum *= GetCosTheta();
      sum = TrChargeLossDB::GetHead()->GetChargeLossCorrectedValue(GetSide(),interstrip,GetCofG(2,kSimAsym),GetImpactAngle(),sum);
      sum /= GetCosTheta();
    }
    // beta/rigidity correction 
    sum *= GetCosTheta();
    if ( (kBeta&opt)&&(kRigidity&opt)&&(fabs(beta)>0)&&(fabs(rigidity)>0) ) sum = pow(TrEDepDB::GetHead()->GetEDepCorrectedValue(GetLayerJ(),sqrt(sum),beta,rigidity,mass_on_Z,GetSide(),2),2.);
    else if ( (kBeta&opt)&&(fabs(beta)>0) )                                 sum = pow(TrEDepDB::GetHead()->GetEDepCorrectedValue(GetLayerJ(),sqrt(sum),beta,0,0,GetSide(),2),2.);
    else if ( (kRigidity&opt)&&(fabs(rigidity)>0) )                         sum = pow(TrEDepDB::GetHead()->GetEDepCorrectedValue(GetLayerJ(),sqrt(sum),0,rigidity,mass_on_Z,GetSide(),2),2.);
    sum /= GetCosTheta();
    // pathlenght 
    if (kAngle&opt) sum *= GetCosTheta();
    // 100 keV per MIP
    if (kMeV&opt) sum = sum*0.100; 
  }
  SetDxDz(dxdzr);//setbackup
  SetDyDz(dydzr);//setbackup
  return sum;
}

float TrClusterR::GetQH(float beta, float rigidity, float mass_on_Z, int res_mult, float dxdz, float dydz,float Z0, int qopt){
  if(!(clustercharge.cluster)) clustercharge.AssignPointer(this);
  return clustercharge.GetQ(res_mult,beta,rigidity,qopt,Z0,dxdz,dydz);
}
float TrClusterR::GetEdepH(int res_mult, float dxdz, float dydz,float Z0, int qopt){
  if(!(clustercharge.cluster)) clustercharge.AssignPointer(this);
  return 1000*clustercharge.GetEdep(res_mult,qopt,false,Z0,dxdz,dydz);
}

void TrClusterR::GetBoundsSymm(int &leftindex, int &rightindex, int nstrips, int opt, int seedIndex) {
  /*               _        
   *              | |_      
   *           _ _| | |     
   *     _ _ _| | | | |_    
   *  __|_|_|_|_|_|_|_|_|__                   
   *     X X 6 4 3 1 2 5
   *     X X 6 5 2 1 3 4 (kFlip)          
   *         
   * Trying to contruct symmetrycally the bounds given the number of strips
   * - n = min(int((nstrips-1)/2),min(nleft,nright)) to be used automatically on left an right (symmetry)
   * - add one more strip if nstrips is odd or if the nleft and nright are dramatically different 
   */
  if (checkstatus(kFlip)!=0) opt|=kFlip;
  int cstrip     = seedIndex == -1 ? GetSeedIndex(opt) : seedIndex;
  int nleft      = GetLeftLength(opt, cstrip);
  int nright     = GetRightLength(opt, cstrip);
  int n          = min(int((nstrips-1)/2),min(nleft,nright));
  int nleftused  = min(n,nleft);
  int nrightused = min(n,nright);
  if ((nstrips-nleftused-nrightused-1)>0) { // add one more strip
    if      ( (nleft==nleftused)&&(nright==nrightused) ) {}
    else if (nleft  == nleftused)  nrightused++;
    else if (nright == nrightused) nleftused++;
    else {
      double leftSignal = GetSignal(cstrip-nleftused-1,opt);
      double rightSignal = GetSignal(cstrip+nrightused+1,opt);
      ( ( (leftSignal>rightSignal)&&(!(opt&kFlip)) ) || ( (leftSignal<rightSignal)&&(  opt&kFlip ) ) ) ? nleftused++ : nrightused++;
    }
  }
  leftindex  = cstrip - nleftused;
  rightindex = cstrip + nrightused;
}


void TrClusterR::GetBoundsAsym(int &leftindex, int &rightindex, int nstrips, int opt) {
  /*               _        
   *              | |_      
   *           _ _| | |     
   *     _ _ _| | | | |_ _   
   *  __|_|_|_|_|_|_|_|_|_|__                   
   *     8 7 5 4 3 1 2 5 6                      
   *
   * Trying to construct asymmetrycally the bounds given the number of strips
   * - add strips in hierarchical order
   */
  int cstrip     = GetSeedIndex(opt);
  int nleft      = GetLeftLength(opt, cstrip);
  int nright     = GetRightLength(opt, cstrip);
  int nleftused  = 0; 
  int nrightused = 0; 
  for (int ii=0; ii<nstrips-1; ii++) {
    if      ( (nleft==nleftused)&&(nright==nrightused) ) break;  
    else if (nleft  == nleftused)  nrightused++; 
    else if (nright == nrightused) nleftused++; 
    else    ( GetSignal(cstrip-nleftused-1,opt)>GetSignal(cstrip+nrightused+1,opt) ) ? nleftused++ : nrightused++; 
  }
  leftindex  = cstrip - nleftused;
  rightindex = cstrip + nrightused;
}


double TrClusterR::GetXCofG(int nstrips, int imult, int opt, float chrg, float dxdz, float dydz, int copt) {
  int z=0;
  if(chrg>0){z=(chrg>1)?int(chrg+0.5):1;}
  if ((TRCLFFKEY.ClusterCofGOpt%10)>=2){//With Eta Uniformity Calibration
     if(GetSide() == 0)return GetLinearXCofG(-1,     imult,opt,z,dxdz,dydz,copt);//best number of strips used
     else              return GetLinearYCofG(nstrips,imult,opt,z,dxdz,dydz,copt);
  }
  return GetSimpleXCofG(nstrips,imult,opt,dxdz,dydz,copt);
}

double TrClusterR::GetSimpleXCofG(int nstrips, int imult, int opt, float dxdz, float dydz, int copt){
  if (nstrips==-1) {
    nstrips = 2;
    if(dxdz==-2)dxdz=GetDxDz();
    if(dydz==-2)dydz=GetDyDz();
    if (GetSide() == 0 && fabs(dxdz) > TwoStripThresholdX) nstrips = 3;
    if (GetSide() == 1 && fabs(dydz) > TwoStripThresholdY) nstrips = 3;
  } 
  if (nstrips==1) return 0.;
  if (opt==-1) {
    opt = DefaultCorrOpt;
    if ( (GetSide()==1)&&(((TRCLFFKEY.UseNonLinearity%10)==1)||( ((TRCLFFKEY.UseNonLinearity%10)==2)&&(GetQtrk()>2.5)&&(GetQtrk()<12.5))) ) opt = DefaultLinearityCorrOpt;
  }
  int leftindex;  
  int rightindex;
  double snumerator  = 0.;//position in strip unit
  double numerator   = 0.;
  double denominator = 0.;
  GetBoundsSymm(leftindex,rightindex,nstrips,opt);
  // GetBoundsAsym(leftindex,rightindex,nstrips,opt);
  int seed_index = GetSeedIndex(opt);
  //----fast return seed strip ladchanx or ladchany
  if(copt==3)return seed_index;
  double seedposition = (copt>=2)? seed_index:GetX(seed_index,imult);//V6 version
  int seed_sensor=-1,sensor=-1,multseed_index=-1;
  if(copt>=2&&GetSide()==0){//V6 version
    multseed_index=GetMultSeedIndex();
    GetSensorAddress(seed_sensor,seed_index,imult,multseed_index,copt);
  }
  for (int index=leftindex; index<=rightindex; index++) {
    // signal weight
    double weight = GetSignal(index,opt);
    if (opt&kCoupl) { // trying to eliminate coupling effect
      if (index-1>0)          weight -= 0.04*GetSignal(index-1,opt);
      if (index+1<GetNelem()) weight -= 0.04*GetSignal(index+1,opt);
    }
    // position
    double position = (copt>=2)? (index-seed_index):GetX(index,imult);//V6 version
    double sposition=(index-seed_index);//position in strip unit
    int   mask = 1;
    if(copt>=2){//V6 version
      if(GetSide()==0){
        GetSensorAddress(sensor,index,imult,multseed_index,copt);//split clusters by sensor
        if(sensor!=seed_sensor)mask=0;//must be the same sensor for coordinate measurement
      }
    }
    else if (IsK7()!=0) { 
      // special computation for K7 (cyclic clusters)
      // - take seed strip as the strip with a fixed multiplicity
       int address = GetAddress(index); 
       int maxmult = TkCoo::GetMaxMult(GetTkId(),address);
       if ( (index<seed_index)&&(position>seedposition) ) {
         position = (imult>0) ? GetX(index,imult-1) : 0; 
         mask     = (imult>0) ? 1 : 0;
       }
       else if ( (index>seed_index)&&(position<seedposition) ) {  
         position = (imult<(maxmult-1)) ? GetX(index,imult+1) : 0;
         mask     = (imult<(maxmult-1)) ? 1 : 0;
       }
    }
    // weighted average
    if(((TRCLFFKEY.ClusterCofGOpt%10)>=1)&&(weight<0))weight=0;//QY
    snumerator  += weight*sposition*mask;
    numerator   += weight*position*mask;
    denominator += weight*mask;
    // cout << "numerator: " << numerator << "  denominator: " << denominator << endl;
  } 
  double xcoo=0;
  if(copt==0||copt==2){
    xcoo = (denominator==0)?0:snumerator/denominator;
    if(copt==2){
      if     (xcoo<-0.5)xcoo=-0.5;
      else if(xcoo> 0.5)xcoo= 0.5;
      xcoo+=seed_index;//index for the cluster CofG
    }
  }
  else { 
    xcoo = (denominator==0)?seedposition:numerator/denominator;
  }
  return xcoo; 
} 


double TrClusterR::GetLinearXCofG(int nstrips, int imult, int opt, int z, float dxdz, float dydz, int copt) {//With Eta Uniformity Calibration(by QY)
  if (GetSide() == 1) return -2;// error X Side
  if (!TrLinearXEtaDB::GetHead()->IsValid()){//error TDV not valid
//     static int nerr=0;
//     if(nerr++<10)cerr<<"TrClusterR::GetLinearXCofG TrLinearXEtaDB-NotValidate"<<endl;
     return GetSimpleXCofG(nstrips,imult,opt,dxdz,dydz,copt);
  }
  //----Z
  if(z<=0){
    z=int(GetQtrk()+0.5);
    if(z<1)z=1;
  }
  if(dxdz==-2)dxdz=GetDxDz();

  opt=(TrClusterR::kAsymEta|TrClusterR::kOverflow);

  //----strips
  int seed_index,second_index;
  int maxover=GetSeedSecondIndex(seed_index,second_index,opt);
  //---------------
  int multseed_index=GetMultSeedIndex();//multiplicity-seed is defined by old cofg algorithm with tracker finding
  //----fast return seed strip ladchanx
  if(copt==3)return seed_index;;

  if(copt<=1){//no need for V6
    int cmult=GetResolvedMultiplicity(seed_index,imult,multseed_index);
    if(cmult==-3){imult--;cmult=GetResolvedMultiplicity(seed_index,imult,multseed_index);}//QY K7 new seed address overflow
  }

  int striptype=GetSeedSecondPairType(imult,multseed_index,opt,copt);
  //----sensor
  int seed_sensor,sensor;
  GetSensorAddress(seed_sensor,seed_index,imult,multseed_index,copt);  

  //---adc/coo 
  int leftindex;
  int rightindex;
  const int mstrips = 5;//try to get 5strips
  GetBoundsSymm(leftindex,rightindex,mstrips,opt);
  int ic=(mstrips-1)/2;
  double adc[mstrips]={0},pos[mstrips]={0};
  bool   ado[mstrips]={0};
  for (int index=leftindex; index<=rightindex; index++) {
    int ip=index-seed_index+ic;
    adc[ip]=GetSignal(index,opt);
    ado[ip]=IsOverflowADC(index,opt);
    if(copt<=1)pos[ip]=GetX(index,imult);//no need for V6,copt>=2
//-----check+protection the same sensor as the seed-sensor
    GetSensorAddress(sensor,index,imult,multseed_index,copt);//split clusters by sensor
    if((sensor!=seed_sensor)&&(copt>=1)){adc[ip]=pos[ip]=0;continue;}//must be the same sensor for coordinate measurement
//-----
    if(IsK7()&&(copt==1)){//multiplicity is defined by old cofg algorithm seed, no need for V6 copt>=2
      int umult=GetResolvedMultiplicity(index,imult,multseed_index);
      if(umult>=0)pos[ip]=GetX(index,umult);//update with the new-mult
      else {
        if(index!=seed_index)adc[ip]=pos[ip]=0;//non-seed strip exclude from coordinate calculation
      }
    }
//-----
  }

  //---linearized xcoo
  int patt=LinearUsedStrips;//2/3/4/5
  if(LinearUsedStrips<0){//overflow dynamic adjustment
    if     (z<=16)patt=12;//>=2 strips-0 over
    else if(z<=24)patt=13;//>=3 strips-1 over
    else if(z<=30)patt=14;//>=4 strips-2 over
    else          patt=15;//>=5 strips-3 over
  }
  if(nstrips>=2)patt=nstrips;//force to use defined nstrips/overflow dynamic adjustment >=10
  if(patt>=10) {//overflow dynamic adjustment
    patt=patt%10;
    while (maxover>patt-2&&patt<5){patt++;}
  }
  //---linear cut for high z
  int upatt;
  double xcoo=0;
  if(copt==0||copt==2){//gravity in strip unit
     xcoo=TrLinearXEtaDB::GetHead()->GetLinearCofG(adc,patt,upatt,dxdz,z,GetTkId(),striptype);
     if(copt==2){
       if     (xcoo<-0.5)xcoo=-0.5;
       else if(xcoo> 0.5)xcoo= 0.5;
       xcoo+=seed_index;//index for the cluster CofG
     }
     return xcoo;
  }
  xcoo=TrLinearXEtaDB::GetHead()->GetLinearX(adc,pos,patt,upatt,dxdz,z,GetTkId(),striptype);//absolute position
  //---alignment compensation
  bool   isedge=((striptype/1000)%10>=1);//seed hit sensor-edge
  double pitch= (!IsK7())?TkDBc::Head->_PitchK5:(63*1.+320*2.)/383*TkDBc::Head->_ImplantPitchK;//K5 or K7 pitch_size
  double offset=TrLinearXEtaDB::GetHead()->GetAsymAlign(GetTkId())*pitch; 
//  cout<<"xcoo="<<xcoo<<" IsK7="<<IsK7()<<" tkid="<<GetTkId()<<" offset="<<offset<<endl;
  if(isedge)offset=0;//prevent go out of boudary
  return xcoo-offset;
}


double TrClusterR::GetLinearYCofG(int nstrips, int imult, int opt, int z, float dxdz, float dydz, int copt) {//With Eta Uniformity Calibration(by QY)
  if (GetSide() == 0) return -2;//error X Side
  if (!TrLinearEtaDB::GetHead()->IsValid()){//error TDV not valid
//     static int nerr=0;
//     if(nerr++<10)cerr<<"TrClusterR::GetLinearYCofG TrLinearEtaDB-NotValidate"<<endl;
     return GetSimpleXCofG(nstrips,imult,opt,dxdz,dydz,copt);
  }
  //----Z
  if(z<=0){
    z=int(GetQtrk()+0.5);
    if(z<1)z=1;
  }
  if(dydz==-2)dydz=GetDyDz();

  //----Yopt
  int optr=opt;
  if(z<=2)opt=TrClusterR::kAsym;
  else    opt=(TrClusterR::kAsym|TrClusterR::kGain);
  //----initial new gain
  if((optr&TrClusterR::kAsymEta)||(TRCLFFKEY.ClusterCofGOpt%10)>=3)opt|=TrClusterR::kAsymEta;
  if((optr&TrClusterR::kQ2Eta)  ||(TRCLFFKEY.ClusterCofGOpt/10)>=1)opt|=TrClusterR::kQ2Eta;
  //----strips
  int seed_index = GetSeedIndex(opt);
  //----fast return seed strip ladchany
  if(copt==3)return seed_index;
  int leftindex;
  int rightindex;
  nstrips = 3;//try to get 3strips
  GetBoundsSymm(leftindex,rightindex,nstrips,opt,seed_index);
  //---sort by adc
  double cadc=0,sadc=0,tadc=0;
  double cpos=0,spos=0,tpos=0;
  cpos=(copt==0||copt>=2)? seed_index : GetX(seed_index,imult);//gravity in strip unit
  for (int index=leftindex; index<=rightindex; index++) {
    double nadc=GetSignal(index,opt,z);
    double npos=(copt==0||copt>=2)? index : GetX(index,imult);//gravity in strip unit
    if(nadc>cadc){
      tadc=sadc;tpos=spos;
      sadc=cadc;spos=cpos;
      cadc=nadc;cpos=npos;
    }
    else if(nadc>sadc){
      tadc=sadc;tpos=spos;
      sadc=nadc;spos=npos;
    }
    else if(nadc>tadc){
      tadc=nadc;tpos=npos;
    }
  }
  //---linearized xcoo
  double xcoo=0;
  if(copt==0||copt==2){//gravity in strip unit
     if(sadc>0){
       xcoo=TrLinearEtaDB::GetHead()->GetLinearEta(cadc,sadc,tadc,dydz,z,GetTkId());
       if(spos<cpos)xcoo*=-1;
     }
     if(copt==2){
       if     (xcoo<-0.5)xcoo=-0.5;
       else if(xcoo> 0.5)xcoo= 0.5;
       xcoo+=seed_index;//index for the cluster CofG
     }
     return xcoo;
  }
  //---alignment compensation
  double offset=0;
  int cadd=GetAddress(seed_index);
  if((kAsymEta&opt)&&cadd>=1&&cadd<=638){//prevent go out of boudary
    offset=TrLinearEtaDB::GetHead()->GetAsymAlign(GetTkId())*TkDBc::Head->_PitchS;
  }
  if(sadc<=0)return cpos-offset;
  xcoo=TrLinearEtaDB::GetHead()->GetLinearX(cpos,spos,cadc,sadc,tadc,dydz,z,GetTkId());
  return xcoo-offset;
}

float TrClusterR::GetCofG(int nstrips, int opt) {
  if (nstrips==-1) {
    nstrips = 2;
    if (GetSide() == 0 && fabs(GetDxDz()) > TwoStripThresholdX) nstrips = 3;
    if (GetSide() == 1 && fabs(GetDyDz()) > TwoStripThresholdY) nstrips = 3;
  }
  if (nstrips==1) return 0.;
  if (opt==-1) {
    opt = DefaultCorrOpt;
    if ( (GetSide()==1)&&(((TRCLFFKEY.UseNonLinearity%10)==1)||( ((TRCLFFKEY.UseNonLinearity%10)==2)&&(GetQtrk()>2.5)&&(GetQtrk()<12.5))) ) opt = DefaultLinearityCorrOpt;
  }
  int leftindex; 
  int rightindex;
  float numerator   = 0.;
  float denominator = 0.;  
  // GetBoundsAsym(leftindex,rightindex,nstrips,opt);
  GetBoundsSymm(leftindex,rightindex,nstrips,opt);
  int cstrip = GetSeedIndex(opt);
  for (int index=leftindex; index<=rightindex; index++) {
    float weight = GetSignal(index,opt);
    if(((TRCLFFKEY.ClusterCofGOpt%10)>=1)&&(weight<0))weight=0;//QY
    numerator   += weight*(index-cstrip);
    denominator += weight;
  }
  float CofG = (denominator==0)?0:numerator/denominator;
  return CofG;
}

float TrClusterR::GetLinearCofG(int nstrips, int imult, int opt, int z, float dxdz, float dydz) {
  if (GetSide() == 0)return GetLinearXCofG(nstrips,imult,opt,z,dxdz,dydz,0);
  else               return GetLinearYCofG(nstrips,imult,opt,z,dxdz,dydz,0);
}

float TrClusterR::GetEta(int opt) {
    
  if (opt==-1) {
    opt = DefaultCorrOpt;
    if ( (GetSide()==1)&&(((TRCLFFKEY.UseNonLinearity%10)==1)||( ((TRCLFFKEY.UseNonLinearity%10)==2)&&(GetQtrk()>2.5)&&(GetQtrk()<12.5))) ) opt = DefaultLinearityCorrOpt;
  }
  /*! Eta = center of gravity with the two higher strips = Q_{R} / ( Q_{L} + Q_{R} )
   *      _                                    _ 
   *    l|c|r          c*0 + r*1    r        l|c|r            l*0 + c*1    c
   *     | |_    eta = --------- = ---       _| |       eta = --------- = ---
   *    _| | |           c + r     c+r      | | |_              l + c     l+c
   * __|_|_|_|__                          __|_|_|_|__
   *      0 1                                0 1 
   *  Eta is 1 for particle near to the right strip,
   *  while is approx 0 when is near to the left strip (old definition) */
  if (GetNelem()<1) return -1;
  int cstrip = GetSeedIndex(opt);
  int nleft  = GetLeftLength(opt);
  int nright = GetRightLength(opt);
  // 1 strip
  if ( (nleft==0)&&(nright==0) ) return 0.; 
  // 2 strips
  if (nleft==0)  return GetSignal(cstrip+1,opt)/(GetSignal(cstrip+1,opt) + GetSignal(cstrip,opt));
  if (nright==0) return GetSignal(cstrip,  opt)/(GetSignal(cstrip-1,opt) + GetSignal(cstrip,opt));
  // >2 strips
  if ( GetSignal(cstrip+1,opt)>GetSignal(cstrip-1,opt) ) {
    return GetSignal(cstrip+1,opt)/(GetSignal(cstrip+1,opt) + GetSignal(cstrip,opt));
  }
  else {                                           
    return GetSignal(cstrip  ,opt)/(GetSignal(cstrip-1,opt) + GetSignal(cstrip,opt));
  } 
  return -1.;
}


double TrClusterR::GetdZShiftondXYG(int ixy, int z, float dxdz, float dydz, int vers){
  double zdxc = 0;
  double zdyc = 0;

  if(vers>=3){//New V6 Version, new alignment 2022
    zdxc=zdyc=0;
    if(z==2){//z=2
      zdxc= 4.8;//um
      zdyc=-2.9;//um (L1InnerG, -0.3<=dy/dz<=0.3)
//      zdyc=-2.3;//um (InnerG, -0.45<=dy/dz<=0.45)
    }
    else if(z>=3&&z<=8){//3<=z<=8
      zdxc=-27.55+14.8244*z-0.531946*z*z;//um  57.0um for Z=8
      zdyc= 11.69-7.00571*z+0.243039*z*z;//um -28.8um for Z=8 (L1InnerG, -0.3<=dy/dz<=0.3)
//      zdyc= 11.07-6.23457*z+0.189751*z*z;//um -26.7um for Z=8 (InnerG, -0.45<=dy/dz<=0.45)
    }
    else if(z>=9){//z>=8 all OK
      zdxc= 61.5822/(1.+9.98107*exp(-z/4.4821))+33.98;//um  93.8um for Z=26
      zdyc=-32.5672/(1.+464.198*exp(-z/1.2073))-8.64; //um -41.2um for Z=26 (L1InnerG, -0.3<=dy/dz<=0.3)
//      zdyc=-51.3670/(1.+196.399*exp(-z/1.28646))+10.26;//um -41.1um for Z=26 (InnerG, -0.45<=dy/dz<=0.45)
    }
    zdxc*=1e-4;//um->cm
    zdyc*=1e-4;//um->cm
  }
  else if(vers>=0){//vers=-1 means no correction
    zdxc = 3e-4;
    zdyc = 0;
    if (z >= 2) {//Z correction for z=2
      zdxc = (1-TMath::Abs(dxdz))*12e-4;
      zdyc = -4e-4;
    }
    else if (z >= 3){//Z correction for 3<=z<=8
      if (z >= 3) { zdyc = -11.0; }
      if (z >= 4) { zdyc = -18.5; }
      if (z >= 5) { zdyc = -23.5; }
      if (z >= 6) { zdyc = -24.5; }
      if (z >= 7) { zdyc = -25.5; }
      if (z >= 8) { zdyc = -30.5; }
      //-----QY dzshift parameterization for heavy ion Y-side
      if (z>=9) zdyc = -(77.771/(1.+92.461*exp(-z/1.27526))-65.3307)-30.5;
      //-----QY dzshift parameterization for heavy ion X-side
      zdxc  = (1-TMath::Abs(dxdz))*12;//keep the same as helium
      if (z>=9) zdxc+=  74.6093/(1.+3.94242*exp(-z/5.28066))+19.3189;//Z>=9
      else      zdxc+= -37.5394+18.9544*z-0.856072*z*z;//Z=3-8
      //-----
      zdyc *= 1e-4;
      zdxc *= 1e-4;
    }
  }
  double dxy=0;
  if     (ixy==0)dxy=zdxc*dxdz;//z shift on X
  else if(ixy==1)dxy=zdyc*dydz;//z shift on Y
  return dxy;
}


double TrClusterR::GetdZShiftondXY(float chrg, float dxdz, float dydz, int vers){
  int z=0;
  if(chrg>0){z=(chrg>1)?int(chrg+0.5):1;}
  //----Z
  if(z<=0){
    z=int(GetQtrk()+0.5);
    if(z<1)z=1;
  }
  if(dxdz==-2)dxdz=GetDxDz();
  if(dydz==-2)dydz=GetDyDz();
 
  return GetdZShiftondXYG(GetSide(),z,dxdz,dydz,vers);
}

int TrClusterR::GetQStatusYJ(int nstrip_from_seed, int mult) {
  int seed_index = GetSeedIndex();
  int qstatusyj = (GetDeadChannelStatusYJ(nstrip_from_seed,mult,seed_index)>0);
  return (GetQStatus(nstrip_from_seed,mult) | qstatusyj);
}

int TrClusterR::GetDeadChannelStatusYJ(int nstrip_from_seed, int mult, int seed_index){
  int second_index=-1;
  if (seed_index < 0){GetSeedSecondIndex(seed_index,second_index);}
  return TrChargeYJ::GetHead()->GetQChaStatus(GetTkId(),GetAddress(seed_index));
}

TrQYJCluster TrClusterR::GetQYJ_allZGain(float beta, float rigidity, int res_mult, float dxdz, float dydz, float xcof, int zgain, int qopt){

  if(res_mult<0)res_mult=0;
  if(dxdz==-2)dxdz=GetDxDz();
  if(dydz==-2)dydz=GetDyDz();
  if(zgain<=1)zgain=1;

  //-------
  int opt=(TrChargeYJDB::GetHead()->isreal)? (TrClusterR::kAsymEta|TrClusterR::kOverflow): TrClusterR::kAsym;
  int side = GetSide();
  float tkcof[2]={0};
  tkcof[0]=(side==0)?GetLinearXCofG(-1,res_mult,opt,zgain,dxdz,dydz,0):xcof; 
  if(side==1)tkcof[1]=GetLinearYCofG(-1,res_mult,opt,zgain,dxdz,dydz,0);
  int cstrip,sstrip;
  int multseed_index=GetMultSeedIndex();//multiplicity-seed is defined by old cofg algorithm with tracker finding
  int noverflow=GetSeedSecondIndex(cstrip,sstrip,opt);
  int striptype=GetSeedSecondPairType(res_mult,multseed_index,opt)%1000;
  int ik=0;
  if     (striptype==102||striptype==202)ik=1;//niters=1(2)/K5/type=2
  else if(striptype==113)                ik=2;//niters=1   /K7/type=3(sensor-address: 64-159)
  else if(striptype==114)                ik=3;//niters=1   /K7/type=4(sensor-address: 0-63,160-223)
  else if(striptype== 14)                ik=4;//niters=0   /K7/type=4(sensor-address: 0-63,160-223)
  int stripx=ik+noverflow*1000000; 
  const int mstrips=7;//try to get 7strips
  int cpos=(mstrips-1)/2;//left/right
  float radc[mstrips]={0};
  float aadc=0;
  float adc_half[2]={0};
  for(int ii=0;ii<GetNelem();ii++){
    float nadc=GetSignal(ii,opt);
    if(nadc<=0)continue;
    if(abs(ii-cstrip)<=cpos)radc[ii-cstrip+cpos]=nadc;
    bool seedside=(tkcof[side]<0)?(ii>=cstrip):(ii<=cstrip);
    if(seedside)adc_half[0]+=nadc;
    else        adc_half[1]+=nadc;
    aadc+=nadc;
  }
  float adc[7]={0};
  adc[0]=radc[cpos];
  if(tkcof[side]<0){
    adc[1]=radc[cpos-1];
    adc[2]=radc[cpos+1];
    adc[3]=radc[cpos-2];
    adc[4]=radc[cpos+2];
    adc[5]=radc[cpos-3];
    adc[6]=radc[cpos+3];
  }
  else {
    adc[1]=radc[cpos+1];
    adc[2]=radc[cpos-1];
    adc[3]=radc[cpos+2];
    adc[4]=radc[cpos-2];
    adc[5]=radc[cpos+3];
    adc[6]=radc[cpos-3];
  }
  if(side==0&&noverflow==0){adc[5]=adc_half[0];adc[6]=adc_half[1];}
  if(side==1)adc[4]=aadc;//total adc for y side
  float dadz[2]={dxdz,dydz};
  return TrQYJCluster(GetTkId(),GetAddress(cstrip),adc,tkcof,dadz,stripx,beta,rigidity,qopt); 
}


TrQYJCluster TrClusterR::GetQYJ_all(float beta, float rigidity, int res_mult, float dxdz, float dydz, float xcof, float qgain, int qopt){
  int zgain=int(qgain);
  if(zgain<0){
    zgain=1;//start from Z=1
    while(1){
      double q10=GetQYJ_allZGain(beta,rigidity,res_mult,dxdz,dydz,xcof,zgain,qopt).Q;
      double q20=GetQYJ_allZGain(beta,rigidity,res_mult,dxdz,dydz,xcof,zgain+1,qopt).Q;
      if(q10<1||q20<1){zgain=0;break;}
      else if((q10>=zgain&&q20<=zgain+1)||(q10<=zgain&&q20>=zgain+1)){
        break;
      }
      else if(q10>=zgain+1||q20>=zgain+1){zgain=(q10>q20)?int(q10):int(q20);}
      else if(q10<=zgain||q20<=zgain)    {zgain--;}
      else {cerr<<"TrQYJCluster::could not find z0"<<endl;break;}
    }
  }
  int z1=zgain;
  int z2=z1+1;
  if(z1<=1)z1=1;
  if(z2<=1)z2=1;
  TrQYJCluster yjcl1=GetQYJ_allZGain(beta,rigidity,res_mult,dxdz,dydz,xcof,z1,qopt);
  TrQYJCluster yjcl2=GetQYJ_allZGain(beta,rigidity,res_mult,dxdz,dydz,xcof,z2,qopt);
//--------
  double q1=yjcl1.Q;//smoothed with weighted q
  double q2=yjcl2.Q;
  double ww1=fabs(q2*q2-z2*z2); 
  double ww2=fabs(q1*q1-z1*z1);
  if(qgain>=0){
    ww1=fabs(qgain*qgain-z2*z2);
    ww2=fabs(qgain*qgain-z1*z1);
  }
  double wpow = 1.0;
  if(TrChargeYJDB::GetHead()->isreal&&qopt&TrChargeYJ::kPow)wpow=1.5;//use optimized eta interpolation parameter for innerQ
  ww1=pow(ww1,wpow);ww2=pow(ww2,wpow);
  TrQYJCluster yjcla=yjcl1.Add(yjcl1,ww1,yjcl2,ww2);//linear smoothed with weighted q
  return yjcla;
}

float TrClusterR::GetQYJ(float beta, float rigidity, int res_mult, float dxdz, float dydz, float xcof, float qgain, int qopt){
   return GetQYJ_all(beta,rigidity,res_mult,dxdz,dydz,xcof,qgain,qopt).Q;
}

float TrClusterR::GetClusterSN(int opt) {
  if (GetNelem()<1) return -1;
  float sum = 0;
  float sigma = 0;
  for (int ii=0; ii<GetNelem(); ii++) {
    sum += GetSignal(ii,opt);
    // if problems this sum will blow-up (default sigma is -9999)
    sigma += pow(GetSigma(ii),2); 
  }
  return sum/sqrt(sigma);
}


//////////////////////////////////////////////////////////////////////////////
// More functions for cluster quality
//////////////////////////////////////////////////////////////////////////////


bool TrClusterR::Check(int verbosity) {
  if (IsK7()) return true; 
  for (int istrip=0; istrip<GetNelem(); istrip++) {
    int address = GetAddress() + istrip;
    // no cluster out-of-bounds S (outside or between X and Y)
    if ( (GetSide()==1)&&((address<0)||(address>639))) {
      if (verbosity>0) printf("TrClusterR::Check-E cluster on S-side out of bounds (tkid=%+4d, strip=%4d, address=%4d, nelem=%2d)\n",GetTkId(),address,GetAddress(),GetNelem());
      return false;
    }
    // no cluster out-of-bounds K5 (outside or between X and Y)
    if ( (!IsK7())&&(GetSide()==0)&&((address>1023)||(address<640))) { 
      if (verbosity>0) printf("TrClusterR::Check-E cluster on K-side out of bounds (tkid=%+4d, strip=%4d, address=%4d, nelem=%2d)\n",GetTkId(),address,GetAddress(),GetNelem());
      return false;
    }
    if (istrip==GetNelem()-1) continue;
    // no cluster between sensors (for K5)
    if ( (!IsK7())&&(GetSide()==0)&&(int((address-640)/192)!=int((address-640+1)/192)) ) {
      if (verbosity>0) printf("TrClusterR::Check-E cluster on K-side across sensors (tkid=%+4d, strip=%4d, address=%4d, nelem=%2d)\n",GetTkId(),address,GetAddress(),GetNelem());
      return false;
    }
  }
  return true;
}


bool TrClusterR::CheckK7(int mult, int seed_index, int multseed_index, int verbosity) {
  if (!IsK7()) return true;
  // ok if multiplicity is not defined
  if (mult<0) return true;
  // check that seed and all the strips are in the same sensor
  if (seed_index < 0) seed_index = GetSeedIndex();
  if (multseed_index<0)multseed_index=GetMultSeedIndex();//multiplicity-seed is defined by old cofg algorithm with tracker finding
  int seed_sensor;
  GetSensorAddress(seed_sensor,seed_index,mult,multseed_index);
  for (int stri_index=0; stri_index<GetNelem(); stri_index++) {
    int stri_sensor;
    int stri_addsens = GetSensorAddress(stri_sensor,stri_index,mult,multseed_index,1,verbosity);
    // is wrong?
    if (stri_addsens<0) {
      // no error message 
      return false;
    }
    // is the strip on the same sensor of the seed? 
    if (stri_sensor!=seed_sensor) {
      if (verbosity>0) printf("TrClusterR::CheckK7-W cluster on K7-side across sensors (tkid=%+4d, strip=%4d, sens1=%4d, sens2=%2d)\n",
        GetTkId(),stri_addsens,stri_sensor,seed_sensor);
      return false;
    }
  }
  return true;
}


int TrClusterR::GetNStripWithCalibrationStatus(int nstrip_from_seed, int mask, int mult, int seed_index, int multseed_index) {
  int nstatus = 0;
  if (mult==-1) mult = 0; //mult=-2 don't judge the sensor
  int second_index=-1;
  if (seed_index < 0){GetSeedSecondIndex(seed_index,second_index);}
  if (multseed_index<0)multseed_index=GetMultSeedIndex();
  int seed_sensor=0;
  if(mult>=0)GetSensorAddress(seed_sensor,seed_index,mult,multseed_index);
  TrLadCal* ladcal = TrCalDB::Head->FindCal_TkId(GetTkId()); 
  int leftindex =seed_index-nstrip_from_seed;
  int rightindex=seed_index+nstrip_from_seed;
  if(nstrip_from_seed==-1){//nstrip_from_seed=-1 used for two exist strips
    if     (second_index<0)         {leftindex=rightindex=seed_index;}
    else if(seed_index<second_index){leftindex=seed_index;rightindex=second_index;}
    else                            {leftindex=second_index;rightindex=seed_index;}
  }
  for (int stri_index=leftindex; stri_index<=rightindex; stri_index++) {
    // has the strip a valid position (address,mult) ?  
    int stri_sensor=0;
    // no error produced if wrong, just skip (error can produced if requested strip is outside from sensor)
    int stri_addsens =(mult>=0)?  GetSensorAddress(stri_sensor,stri_index,mult,multseed_index,1,0): 0;
    if (stri_addsens<0) continue; 
    // is the strip on the same sensor of the seed? 
    if (stri_sensor!=seed_sensor) continue;
    // condition 
    int stri_address = GetAddress(stri_index);
    short status = ladcal->GetStatus(stri_address);
    if (status&mask) nstatus++;
  }
  return nstatus;
}


int TrClusterR::GetNStripWithOccupancyStatus(int nstrip_from_seed, int mask, int mult, int seed_index, int multseed_index) {
  if (mult<0) mult = 0; 
  int nstatus = 0;
  if (seed_index < 0) seed_index = GetSeedIndex();
  if (multseed_index<0)multseed_index=GetMultSeedIndex();//multiplicity-seed is defined by old cofg algorithm with tracker finding
  int seed_sensor;
  GetSensorAddress(seed_sensor,seed_index,mult,multseed_index);
  for (int stri_index=seed_index-nstrip_from_seed; stri_index<=seed_index+nstrip_from_seed; stri_index++) {
    // has the strip a valid position (address,mult) ?  
    int stri_sensor;
    int stri_addsens = GetSensorAddress(stri_sensor,stri_index,mult,multseed_index,1,0);
    // no error produced if wrong, just skip (error can produced if requested strip is outside from sensor)
    if (stri_addsens<0) continue;
    // is the strip on the same sensor of the seed? 
    if (stri_sensor!=seed_sensor) continue;
    // condition 
    int stri_address = GetAddress(stri_index);
    int tkid = GetTkId();
    TrLadOcc* ladocc = TrOccDB::GetHead()->FindOccTkId(tkid);
    short status = ladocc->GetStatus(stri_address);
    if (status&mask) nstatus++;
  }
  return nstatus;
}


int TrClusterR::GetNStripWithGainStatus(int nstrip_from_seed, int mask, int mult, int seed_index, int multseed_index) {
  if (mult<0) mult = 0; 
  int nstatus = 0;
  if (seed_index < 0) seed_index = GetSeedIndex();
  if (multseed_index<0)multseed_index=GetMultSeedIndex();//multiplicity-seed is defined by old cofg algorithm with tracker finding
  int seed_sensor;
  GetSensorAddress(seed_sensor,seed_index,mult,multseed_index);
  for (int stri_index=seed_index-nstrip_from_seed; stri_index<=seed_index+nstrip_from_seed; stri_index++) {
    // has the strip a valid position (address,mult) ?  
    int stri_sensor;
    // no error produced if wrong, just skip (error can produced if requested strip is outside from sensor)
    int stri_addsens = GetSensorAddress(stri_sensor,stri_index,mult,multseed_index,1,0);
    if (stri_addsens<0) continue;
    // is the strip on the same sensor of the seed? 
    if (stri_sensor!=seed_sensor) continue;
    // condition 
    int stri_address = GetAddress(stri_index);
    int tkid = GetTkId();
    int iva = int(stri_address/64);
    int status = TrGainDB::GetHead()->FindGainTkId(tkid)->GetStatus(iva);
    if (status&mask) nstatus++;
  }
  return nstatus;
}


int TrClusterR::GetNStripOnTheEdgeOfSensor(int nstrip_from_seed, int mult, int seed_index, int multseed_index) {
  // no judgment if K7 and multiplicity is not defined
  if ( (mult<0)&&(IsK7()) ) return 0;
  if (mult<0) mult = 0; // doesn't really matter in this method for K5 and S
  // evaluation
  int nstrip = 0;
  int first = 0;
  int last = 639;
  if (GetSide()==0) last = (IsK7()) ? 223 : 191;
  if (GetSide()==1) mult = 0;
  if (seed_index < 0) seed_index = GetSeedIndex();
  if (multseed_index<0)multseed_index=GetMultSeedIndex();//multiplicity-seed is defined by old cofg algorithm with tracker finding
  int seed_sensor;
  GetSensorAddress(seed_sensor,seed_index,mult,multseed_index);
  for (int stri_index=seed_index-nstrip_from_seed; stri_index<=seed_index+nstrip_from_seed; stri_index++) {
    // has the strip a valid position (address,mult) ?  
    int stri_sensor;
    // no error produced if wrong, just skip (error can produced if requested strip is outside from sensor)
    int stri_addsens = GetSensorAddress(stri_sensor,stri_index,mult,multseed_index,1,0);
    if (stri_addsens<0) continue;
    // is the strip on the same sensor of the seed? 
    if (stri_sensor!=seed_sensor) continue;
    // condition 
    if ( (stri_addsens==first)||(stri_addsens==last) ) nstrip++;
  }
  return nstrip;
}


int TrClusterR::GetNStripOnTheEdgeOfVA(int nstrip_from_seed, int mult, int seed_index, int multseed_index) {
  // no judgment if K7 and multiplicity is not defined
  if ( (mult<0)&&(IsK7()) ) return 0;
  if (mult<0) mult = 0; // doesn't really matter in this method for K5 and S
  // evaluation
  int nstrip = 0;
  if (GetSide()==1) mult = 0;
  if (seed_index < 0) seed_index = GetSeedIndex();
  if (multseed_index<0)multseed_index=GetMultSeedIndex();//multiplicity-seed is defined by old cofg algorithm with tracker finding
  int seed_sensor;
  GetSensorAddress(seed_sensor,seed_index,mult,multseed_index);
  for (int stri_index=seed_index-nstrip_from_seed; stri_index<=seed_index+nstrip_from_seed; stri_index++) {
    // has the strip a valid position (address,mult) ?  
    int stri_sensor;
    // no error produced if wrong, just skip (error can produced if requested strip is outside from sensor)
    int stri_addsens = GetSensorAddress(stri_sensor,stri_index,mult,multseed_index,1,0);
    if (stri_addsens<0) continue;
    // is the strip on the same sensor of the seed? 
    if (stri_sensor!=seed_sensor) continue;
    // condition 
    int stri_address = GetAddress(stri_index);
    int stri_addrva  = stri_address%64;
    if (stri_addrva==63) nstrip++;
  }
  return nstrip;
}


bool TrClusterR::IsMonotonic(int nstrip_from_seed) { 
  bool return_value = true;
  // sub-cluster indexes
  int seedadd = GetSeedIndex();
  int leftadd = seedadd-nstrip_from_seed; 
  if (leftadd<=0) leftadd = 0;
  int righadd = seedadd+nstrip_from_seed;
  if (righadd>=GetNelem()) righadd = GetNelem()-1;
  // monotonic on the left
  for (int istrip=seedadd; istrip>=leftadd+1; istrip--) 
    if (GetSignal(istrip)<GetSignal(istrip-1)) return_value = false;
  // monotonic on the right
  for (int istrip=seedadd; istrip<=righadd-1; istrip++) 
    if (GetSignal(istrip)<GetSignal(istrip+1)) return_value = false; 
  return return_value;
}


bool TrClusterR::IsMonotonicWithThreshold(float threshold) {
  bool return_value = true;
  int seedadd = GetSeedIndex();
  for (int istrip=seedadd; istrip>0; istrip--) {
    if (GetSN(istrip)<threshold) break;
    if (GetSignal(istrip)<GetSignal(istrip-1)) return_value = false;
  }
  for (int istrip=seedadd; istrip<GetNelem()-1; istrip++) {
    if (GetSN(istrip)<threshold) break;
    if (GetSignal(istrip)<GetSignal(istrip+1)) return_value = false;
  }
  return return_value;
}


bool TrClusterR::IsOverThreshold(int nstrip_from_seed, float threshold) {
  bool return_value = true;
  // sub-cluster indexes
  int seedadd = GetSeedIndex();
  int leftadd = seedadd-nstrip_from_seed;
  if (leftadd<=0) leftadd = 0;
  int righadd = seedadd+nstrip_from_seed;
  if (righadd>=GetNelem()) righadd = GetNelem()-1;
  // is everything over threshold?
  for (int istrip=leftadd; istrip<=righadd; istrip++) 
    if (GetSN(istrip)<threshold) return_value = false;
  return return_value;
}

bool TrClusterR::IsGoodVA(int nstrip_from_seed){
  if (GetSide() == 0)return true;
  if (!TrLinearEtaDB::GetHead()->IsValid()){
//     static int nerr=0;
//     if(nerr++<10)cerr<<"TrClusterR::IsGoodVA TrLinearEtaDB-NotValidate"<<endl;
     return true;
  }
  bool return_value = true;
  // sub-cluster indexes
  int seedadd = GetSeedIndex();
  int leftadd = seedadd-nstrip_from_seed;
  if (nstrip_from_seed<0||leftadd<=0) leftadd = 0;
  int righadd = seedadd+nstrip_from_seed;
  if (nstrip_from_seed<0||righadd>=GetNelem()) righadd = GetNelem()-1;
   // is good va (p-strip)
  int tkid=GetTkId();
  for (int istrip=leftadd; istrip<=righadd; istrip++)
    if(!TrLinearEtaDB::GetHead()->IsGoodVA(tkid,GetAddress(istrip))){return_value=false;break;}
  return return_value;
}

float TrClusterR::GetSignalToSignalRatio(int nstrip_from_seed) {
  // sub-cluster indexes
  int seedadd = GetSeedIndex();
  int leftadd = seedadd-nstrip_from_seed;
  if (leftadd<=0) leftadd = 0;
  int righadd = seedadd+nstrip_from_seed;
  if (righadd>=GetNelem()) righadd = GetNelem()-1;
  // sum everything (GetTotSignal without corretions)
  float sum_all = 0;
  for (int istrip=0; istrip<GetNelem(); istrip++)
    sum_all += GetSignal(istrip);
  // sum sub-cluster     
  float sum_sub = 0;
  for (int istrip=leftadd; istrip<=righadd; istrip++) 
    sum_sub += GetSignal(istrip);
  if (sum_all>0) return sum_sub/sum_all;
  return -1;
}


int TrClusterR::GetQStatus(int nstrip_from_seed, int mult) {
  int seed_index = GetSeedIndex();
  int multseed_index=GetMultSeedIndex(); 
  return
    ((GetNStripWithCalibrationStatus(nstrip_from_seed,0x1|0x4,mult,seed_index,multseed_index)>0)*0x1) +
    ((GetNStripWithCalibrationStatus(nstrip_from_seed,0x2|0x8,mult,seed_index,multseed_index)>0)*0x2) +
    ((GetNStripWithCalibrationStatus(nstrip_from_seed,0x200,mult,seed_index,multseed_index)>0)*0x4) +
    ((GetNStripWithCalibrationStatus(nstrip_from_seed,0x8000,mult,seed_index,multseed_index)>0)*0x8) +
    ((GetNStripWithOccupancyStatus(nstrip_from_seed,0x1,mult,seed_index,multseed_index)>0)*0x10) +
    ((GetNStripWithOccupancyStatus(nstrip_from_seed,0x2,mult,seed_index,multseed_index)>0)*0x20) +
    ((GetNStripWithGainStatus(nstrip_from_seed,0x1F,mult,seed_index,multseed_index)>0)*0x40) +
    ((GetNStripWithGainStatus(nstrip_from_seed,0x01,mult,seed_index,multseed_index)>0)*0x80) +
    ((GetNStripOnTheEdgeOfSensor(nstrip_from_seed,mult,seed_index,multseed_index)>0)*0x100) +
    ((GetNStripOnTheEdgeOfVA(nstrip_from_seed,mult,seed_index,multseed_index)>0)*0x200) +
    ((!Check(mult))*0x400) +
    ((!CheckK7(mult,seed_index,multseed_index))*0x800);
}


int TrClusterR::GetMorfologyStatus() {
  return
    ((!IsMonotonic(2))*0x1) +
    ((!IsMonotonic(3))*0x2) +
    ((!IsMonotonicWithThreshold(0.0))*0x4) +
    ((!IsMonotonicWithThreshold(1.5))*0x8) +
    ((!IsMonotonicWithThreshold(3.0))*0x10) +
    ((!IsMonotonicWithThreshold(4.5))*0x20);
}


int TrClusterR::GetNInterstrip(int mult) {
  int nelem   = GetNelem();
  if (nelem==1) return -1;
  int seedadd = GetSeedIndex();
  int leftadd = seedadd-1;
  int righadd = seedadd+1;
  int add1    = seedadd;
  int add2    = seedadd;
  if ( (leftadd< 0)&&(righadd< nelem) ) add2 = righadd;
  if ( (leftadd>=0)&&(righadd>=nelem) ) add2 = leftadd;
  if ( (leftadd>=0)&&(righadd< nelem) ) 
    add2 = (GetSignal(leftadd)>GetSignal(righadd)) ? leftadd : righadd;
  int index = (add2<add1) ? add2 : add1;
  // sensor address 
  int sens;
  int add = GetSensorAddress(sens,index,mult); 
  // S side
  if (GetSide()==1) {
    //  implantation 0000 0001 0002 0003 0004 0005 0006 0007 0008 0009 0010 0011 0012 ... 2555 2556 2557 2558 2559 2560 2561 2562 2563 2564 2565 2566 2567
    //  readout       000                 xxx                 001                 002           638                 xxx                 639
    if ( (add==0)||(add==638) ) return 7;
    else                        return 3;
  }
  // K5 side
  if ( (GetSide()==0)&&(!IsK7()) ) {
    //  implantation  000 001 002 003 004 005 006 ... 378 379 380 381 382 383
    //  readout         0       1       2       3     189     190         191
    if (add==190) return 2;
    else          return 1; 
  }
  // K7 side
  if ( (GetSide()==0)&&(IsK7()) ) {
    //  implantation  000 001 002 003 004 005 006 ... 092 093 094 095 | 096 097 098 ... 286 | 287 288 289 290 291 ... 380 381 382 383
    //  readout         0       1   2       3   4      61  62      63 |  64      65     159 |     160     161 162     221 222     223
    if      (add<= 63) return ((add%2)==0) ? 11 : 10;
    else if (add<=159) return 21; 
    else               return ((add%2)==0) ? 31 : 30;
  }
  return -1;
}


//////////////////////////////////////////////////////////////////////////////
// Old stuff part (mostly deprecated/old methods left for backward compatibility)
//////////////////////////////////////////////////////////////////////////////


// function derived from truncated mean X/Y comparison of ISS data B538 (Oct 2011) 
static float n_to_p_pars[7] = {0.99925,0.00125804,-0.000345359,4.77114e-06,-2.04879e-08,-6.26803e-12,1.71962e-13};
float TrClusterR::ConvertToPSideScale(float adc) {
  if (GetSide()==1) return adc; // already p-side
  double x = sqrt(adc);
  double corr = 0;
  for (int ipar=0; ipar<7; ipar++) corr += pow(x,ipar)*n_to_p_pars[ipar];
  double sqrt_p = corr*x;
  return sqrt_p*sqrt_p;
}


// function derived from truncated mean X/Y comparison of ISS data B538 (Oct 2011) 
static float p_to_n_pars[10] = {2.00371,-0.36054,0.0536171,-0.00417238,0.000186663,-4.94765e-06,7.89718e-08,-7.46039e-10,3.84518e-12,-8.33832e-15};
float TrClusterR::ConvertToNSideScale(float adc) {
  if (GetSide()==0) return adc; // already n-side
  double x = sqrt(adc);
  double corr = 0;
  for (int ipar=0; ipar<10; ipar++) corr += pow(x,ipar)*p_to_n_pars[ipar];
  double sqrt_n = corr*x;
  return sqrt_n*sqrt_n;
}


/* 
  Beta Correction Parameters
  - KSC 2010, muons + proton signal   
  - Plane-by-plane vs beta TOF normalized
  - Three values for 1st plane, anyone of the inner, 9th plane
  - Physical folding
*/
static float A_BetaCorr[3]  = { 1.14505, 0.73906, -0.39480};
static float B_BetaCorr[3]  = { 0.67118, 0.05288, -1.47569};
static float b0_BetaCorr[3] = { 0.85387, 0.89147,  0.93449};
float TrClusterR::BetaCorrection_Muons_2010(float beta) {
  /*
     - Maximum Probability Energy Loss:
       MPEL(300 um of Si) = 53.6614 eV / beta^2 * { 12.1489 - 2 log(beta) - beta^2 - 0.1492 * 
                            * [max(0,2.8716-log(beta gamma)/log(10))]^3.2546}, beta > 0.20
     - Simplified fitting function:
       f(beta) = beta<beta0, A/beta^2 + B*log(beta)/beta^2 + C
                 beta>beta0, k (the TOF beta "saturates")     
       Continuity imposed on beta0 (no derivative continuity)
       C = k - A/beta0^2 - B*log(beta0)/beta0^2
     - Beta Correction:
       g(beta) = beta<beta0, A/beta^2 + B*log(beta)/beta^2 + 1 - A/beta0^2 - B*log(beta0)/beta0^2
                 beta>beta0, 1
     - Physical folding:
       use abs(beta) instead of beta 
       use the upper plane coefficient for the lower one and viceversa in case of negative beta
  */
  int jlayer = GetLayerJ();
  int index  = 0;        // 0 for the layer over TRD
  if (jlayer>1) index++; // 1 for any inner tracker layer 
  if (jlayer>8) index++; // 2 for the layer over ECAL
  if ( (jlayer<1)||(jlayer>9) ) {
    printf("TrClusterR::BetaCorrection-W invalid layer index number (%d), returning 1.\n",index);
    return 1.;
  }
  if (beta<0) index = 2 - index; // physical folding
  beta = fabs(beta);
  if (beta>=b0_BetaCorr[index]) return 1.; // beta "saturation" region
  return A_BetaCorr[index]/pow(beta,2) +
         B_BetaCorr[index]*log(beta)/pow(beta,2) +
         1 -
         A_BetaCorr[index]/pow(b0_BetaCorr[index],2) -
         B_BetaCorr[index]*log(b0_BetaCorr[index])/pow(b0_BetaCorr[index],2);
}


float TrClusterR::BetaCorrection_ISS_2011(float beta) {
  int jlayer = GetLayerJ();
  return AMSEnergyLoss::GetTrackerLayerLogBetaGammaCorrectionFromBeta(jlayer,beta);
}


float TrClusterR::RigidityCorrection(float rigidity, float mass_on_Z) {
  int jlayer = GetLayerJ();
  return AMSEnergyLoss::GetTrackerLayerLogBetaGammaCorrectionFromRigidity(jlayer,rigidity,mass_on_Z); 
}


float TrClusterR::BetaRigidityCorrection(float beta, float rigidity, float mass_on_Z) {
  int jlayer = GetLayerJ();
  return AMSEnergyLoss::GetTrackerLayerLogBetaGammaCorrection(jlayer,beta,rigidity,mass_on_Z);  
}


/* OLD MIP Correction Parameters (DEPRECATED)
  - Extracted by preliminar charge reconstruction ISS data (15/03/2012)
  - No p-strip correction
  - No charge loss
  - Old gain
  - Forced to pass through 0,0
  - A point at charge 50 is added extrapolating linerly from the two last points (for around Iron stuff)

static Int_t    npoints_x_iss11 = 14;
static Double_t sqrtmip_x_iss11[14] = {
   0.000000,  1.000000,  2.000000,  3.000000,  4.000000,  5.000000,  6.000000,
   7.000000,  8.000000, 10.000000, 12.000000, 14.000000, 26.000000, 50.000000
};
static Double_t sqrtadc_x_iss11[14] = {
   0.000000,  5.531720, 11.296013, 18.371866, 25.171396, 31.897547, 38.464909,
   44.658779, 50.470642, 60.331924, 68.210243, 74.606316,103.365150,160.882820
};
static Int_t    npoints_y_iss11 = 14;
static Double_t sqrtmip_y_iss11[14] = {
   0.000000,  1.000000,  2.000000,  3.000000,  4.000000,  5.000000,  6.000000,
   7.000000,  8.000000, 10.000000, 12.000000, 14.000000, 26.000000, 50.000000
};
static Double_t sqrtadc_y_iss11[14] = {
   0.000000,  5.567749, 11.480528, 17.141417, 21.615240, 25.100355, 27.365740,
   29.365822, 30.827951, 33.765102, 37.085659, 40.455532, 64.482658,112.536911
};
*/

/* 
  MIP correction parameters from Pierre (as result of the charge loss procedure)
  - New charge reconstruction (Sep. 2012): full range charge loss corrections, new gain tables 
  - Forced to pass through (0,0). Charge 50 is added extrapolating linerly/
*/
// Original kMIP correction from Pierre
static Int_t     npoints_x_iss12 = 16;
static Double_t  sqrtmip_x_iss12[16] = {0,1,2,3,4,5,6,7,8,9,10,11,12,14,26,50};
static Double_t  sqrtadc_x_iss12[16] = {0,5.36449,11.2985,17.8391,24.4941,31.3103,37.1813,43.1753,49.2113,54.31,59.4016,63.6657,67.0189,73.84,102.47,159.7298};
static Int_t     npoints_y_iss12 = 16;
static Double_t  sqrtmip_y_iss12[16] = {0,1,2,3,4,5,6,7,8,9,10,11,12,14,26,50};
static Double_t  sqrtadc_y_iss12[16] = {0,5.42537,11.1375,16.7609,21.2022,24.4082,26.6097,28.3442,29.8655,31.0935,33.0569,35.5054,36.8489,40.0563,63.498,111.2657};
TSpline3* TrClusterR::sqrtadc_to_sqrtmip_spline[2] = {0,0};
/* 
   MIP correction refinement done after Pierre correction (more effective since wer are in a quasi-linear approximation) 
   - Not really needed layer-by-layer. 
   - Layer J1 shows higher "gain" in both x and y.
   - Few month statistics not enough for low abundance charges (to be completed with high statistics).
   - p-side middle charges (CNO group) have very bad pdf. Most probably is due to saturation itself. 
*/
static Int_t    npoints_iss12 = 16;
static Double_t sqrtmip_iss12[16] = {0,1,2,3,4,5,6,7,8,9,10,11,12,14,26,50};
static Double_t sqrtref_x_iss12[9][16] = {
  {0,  1.0322,  2.0489,  3.0693,  4.0631,  5.0415,  6.2071,  7.2126,  8.3645,  9.4535, 10.3910, 11.6805, 12.8976, 15.3946, 30.1871, 59.7721},
  {0,  1.0350,  2.0461,  3.0605,  4.0527,  5.0613,  6.1905,  7.1984,  8.3238,  9.3426, 10.4076, 11.5819, 12.8762, 15.4510, 29.8118, 58.5334},
  {0,  1.0449,  2.0514,  3.0610,  4.0550,  5.0624,  6.1793,  7.1752,  8.2715,  9.2997, 10.2858, 11.3916, 12.5588, 14.9659, 27.4918, 52.5436},
  {0,  1.0484,  2.0568,  3.0659,  4.0604,  5.0713,  6.1914,  7.1918,  8.2965,  9.3083, 10.2897, 11.3468, 12.5040, 14.8803, 27.2409, 51.9622},
  {0,  1.0406,  2.0491,  3.0608,  4.0575,  5.0692,  6.1887,  7.1962,  8.3110,  9.3488, 10.3997, 11.5029, 12.7061, 15.0752, 28.2011, 54.4528},
  {0,  1.0386,  2.0519,  3.0670,  4.0563,  5.0564,  6.1734,  7.1651,  8.2539,  9.2869, 10.2801, 11.3809, 12.5284, 14.9131, 27.1859, 51.7317},
  {0,  1.0516,  2.0599,  3.0681,  4.0510,  5.0542,  6.1663,  7.1692,  8.2687,  9.2966, 10.2959, 11.3675, 12.5747, 14.8496, 27.6746, 53.3247},
  {0,  1.0479,  2.0572,  3.0672,  4.0629,  5.0740,  6.1913,  7.1944,  8.3050,  9.3272, 10.3588, 11.4431, 12.6560, 15.0087, 27.8669, 53.5833},
  {0,  1.0462,  2.0650,  3.0788,  4.0871,  5.0459,  6.1650,  7.1653,  8.2535,  9.3581, 10.2502, 11.2678, 12.4975, 15.0953, 26.0000, 47.8094}
};
static Double_t sqrtref_y_iss12[9][16] = {
  {0,  1.0291,  2.0627,  3.1274,  4.2725,  5.6714,  7.1794,  8.6908,  9.4906, 10.2831, 11.5284, 12.9723, 13.8364, 16.1600, 28.5842, 53.4327},
  {0,  1.0561,  2.0759,  3.1309,  4.2854,  5.6802,  7.2532,  8.3584,  9.1859,  9.7849, 10.7260, 11.7720, 12.9885, 14.8882, 26.5852, 49.9791},
  {0,  1.0536,  2.0739,  3.1070,  4.1946,  5.4368,  6.7961,  7.8707,  8.8287,  9.6105, 10.2936, 11.6422, 12.8606, 14.8479, 26.7053, 50.4202},
  {0,  1.0354,  2.0656,  3.0870,  4.1484,  5.3347,  6.6247,  7.6550,  8.5860,  9.4774, 10.0505, 11.2958, 12.6397, 14.6636, 26.3616, 49.7576},
  {0,  1.0619,  2.0856,  3.1200,  4.2038,  5.4659,  6.8993,  8.0564,  9.0063,  9.6648, 10.3244, 11.7713, 13.0439, 15.0478, 27.1355, 51.3109},
  {0,  1.0504,  2.0718,  3.1062,  4.2029,  5.4722,  6.8946,  8.0276,  8.9763,  9.6493, 10.3395, 11.8026, 13.0911, 15.1686, 27.2059, 51.2806},
  {0,  1.0102,  2.0447,  3.0617,  4.1086,  5.2329,  6.4542,  7.5464,  8.3948,  9.3438,  9.9125, 11.0561, 12.5202, 14.5890, 27.1602, 52.3027},
  {0,  1.0414,  2.0630,  3.0905,  4.1502,  5.3162,  6.5415,  7.6901,  8.5096,  9.4463,  9.9231, 11.1030, 12.4809, 14.4799, 26.0203, 49.1012},
  {0,  1.0319,  2.0616,  3.1091,  4.2062,  5.3760,  6.5881,  7.7158,  8.5756,  9.7031, 10.2494, 11.8778, 13.2512, 15.2821, 28.0574, 53.6081}
};
TSpline3* TrClusterR::sqrtref_to_sqrtmip_spline[2][9] = { {0,0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0} };
float TrClusterR::GetNumberOfMIPs_ISS(float adc) {
  // initialize if needed
  if (!sqrtadc_to_sqrtmip_spline[0]) sqrtadc_to_sqrtmip_spline[0] = new TSpline3("sqrtadc_to_sqrtmip_x",sqrtadc_x_iss12,sqrtmip_x_iss12,npoints_x_iss12);
  if (!sqrtadc_to_sqrtmip_spline[1]) sqrtadc_to_sqrtmip_spline[1] = new TSpline3("sqrtadc_to_sqrtmip_y",sqrtadc_y_iss12,sqrtmip_y_iss12,npoints_y_iss12);
  for (int ilay=0; ilay<9; ilay++) {
    if (!sqrtref_to_sqrtmip_spline[0][ilay])
      sqrtref_to_sqrtmip_spline[0][ilay] = new TSpline3(Form("sqrtref_to_sqrtmip_x_%02d",ilay),&sqrtref_x_iss12[ilay][0],sqrtmip_iss12,npoints_iss12);
    if (!sqrtref_to_sqrtmip_spline[1][ilay])
      sqrtref_to_sqrtmip_spline[1][ilay] = new TSpline3(Form("sqrtref_to_sqrtmip_y_%02d",ilay),&sqrtref_y_iss12[ilay][0],sqrtmip_iss12,npoints_iss12);
  }
  // calculation 
  int   iside = GetSide();
  int   ilayer = GetLayerJ()-1;
  float sqrtadc = (adc>0) ? sqrt(adc) : 0;
  float sqrtref = sqrtadc_to_sqrtmip_spline[iside]->Eval(sqrtadc);
  float sqrtmip = sqrtref_to_sqrtmip_spline[iside][ilayer]->Eval(sqrtref);
  return pow(sqrtmip,2);
}


/* 
  MIP Correction Parameters (DEPRECATED)
  - These parameters are extracted from TB2003.
  - Straight tracks 
  - On readout strip 
  - No p-strip correction
*/
static float adc_vs_z_tb03[2][12] = {
  {  40.50, 167.92, 387.77, 713.61,1124.73,1615.55,2166.01,2734.12,3257.81,3671.87,4021.40,4290.52}, // n-side
  {  31.50, 106.58, 214.47, 304.94, 368.64, 413.37, 472.73, 575.47, 702.39, 854.23,1039.92,1233.86}  // p-side
  // {  31.50, 106.58, 209.81, 335.58, 525.61, 747.78, 977.83,1299.27,1609.36,1919.63,2220.36,2533.95}  // p-side corr
};

float TrClusterR::GetNumberOfMIPs_TB_2003(float adc) {
  /*
    These parameters are extracted from TB2003.
    - straight tracks 
    - readout strip 
    - no p-strip correction
  */
  int iside = GetSide();
  double x = adc;
  // lower extrapolation 
  if (adc<adc_vs_z_tb03[iside][0]) {
    double x0 = adc_vs_z_tb03[iside][0];
    double x1 = adc_vs_z_tb03[iside][1];
    double y0 = 1*1;
    double y1 = 2*2;
    return ( (x-x0)*y1 + (x1-x)*y0 ) / (x1-x0);
  }
  // upper extrapolation
  if (adc>=adc_vs_z_tb03[iside][11]) {
    double x0 = adc_vs_z_tb03[iside][10];
    double x1 = adc_vs_z_tb03[iside][11];
    double y0 = 11*11;
    double y1 = 12*12;
    return ( (x-x0)*y1 + (x1-x)*y0 ) / (x1-x0);
  }
  // inside range
  for (int i=0; i<11; i++) {
    if ((adc>=adc_vs_z_tb03[iside][i])&&(adc<adc_vs_z_tb03[iside][i+1])) {
      double x0 = adc_vs_z_tb03[iside][i];
      double x1 = adc_vs_z_tb03[iside][i+1];
      double y0 = (i+1)*(i+1);
      double y1 = (i+2)*(i+2);
      return ( (x-x0)*y1 + (x1-x)*y0 ) / (x1-x0);
    }
  }
  return -100;
}


