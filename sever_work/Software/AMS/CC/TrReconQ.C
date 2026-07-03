// $Id: TrReconQ.C,v 1.85 2025/01/31 11:34:21 nbelyaev Exp $

//////////////////////////////////////////////////////////////////////////
///
///\file  TrReconQ.C
///\brief Source file of TrReconQ class
///
///\date  2018/05/12 QY  Pass7 track reconstruction algorithm
///\date  2022/06/04 QY  Pass8 track reconstruction algorithm
///
/// $Date: 2025/01/31 11:34:21 $
///
/// $Revision: 1.85 $
///
///////////////////////////////////////////////////////////////////////////

#include "TSystem.h"
#include "TObjString.h"
#include "TrReconQ.h"
#include "tkdcards.h"
#include "MagField.h"
#include "bcorr.h"
#include "TkDBc.h"
#include "TrFit.h"
#include "TkSens.h"
#include "TkCoo.h"
#include "Vertex.h"
#include "VCon.h"
#include "HistoMan.h"
#include "TrCharge.h"
#include "patt.h"

#ifndef __ROOTSHAREDLIBRARY__
#include "trrec.h"
#include "event.h"
#include "trdrec.h"
#else
#include "root.h"
#endif



int TrReconQ::BdVtx = 0;
int TrReconQ::BdNclei = 0;
int TrReconQ::TrTreeVersion = 3;
unsigned int TrReconQ::hisrun = 0;
TString TrReconQ::hisname="";
unsigned int TrReconQ::flsrun = 0;
TChain *TrReconQ::hisTree = 0;
int TrReconQ::_rrun =  0 ;
int TrReconQ::_revt =  0 ;
int TrReconQ::_rentry= 0 ;




TrReconQ::TrReconQ(AMSEventR *ttevt)
{
  Clear();
  if (!ttevt) ttevt = AMSEventR::Head();
  evt = ttevt;
  trType = 0;
}

int TrReconQ::AddRebuildTracks( int rtype ){
  int bdsult = 1;
  if(  flsrun == evt->Run()  ){
      return -1;
  }

  if(!( hisTree && hisrun == evt->Run() && hisname == evt->Tree()->GetCurrentFile()->GetName() )){
    bdsult = SetBuildAddress();
    if( bdsult < 0  ) {// could not find the file on EOS
      flsrun = evt->Run();
      return -1;
    }
  }

  if(bdsult > 0) {
    int isearch = SearchEventFast();
    if(  isearch< 0 ){
      return -1;
    }
    else{
      AddTrackFromIndex( isearch, rtype );
    }
 }
 else
 {
   return -1;
  }

 return 0;
}

int TrReconQ::AddTrackFromIndex( int isearch, int rtype  )
{
  if( rtype !=0 && rtype !=1 && rtype!=2  )   return -1;
  static int nmessage = 0;
  if (TrTreeVersion == 3 && rtype != 2) {
    if (nmessage < 100) {
      std::cout << "TrReconQ::AddTrackFromIndex-W-WrongRecType For TrTreeVersion == " << TrTreeVersion << " rtype must be 2 but is " << rtype << ". Can't get results." << std::endl;
      ++nmessage;
    }
    return -1;
  }

  const Int_t kmax = 500;
  Int_t nhit[3][kmax];
  Int_t idx[3][kmax][9];
  Int_t ntrack[3] ;
  Float_t rgt[3][kmax];
  Float_t pos[3][kmax][3];
  Float_t dir[3][kmax][3];

  UChar_t zquntrack;
  UChar_t zqunhit[kmax];
  UShort_t zquidx[kmax][9];
  Float_t zqurgt[kmax];
  Float_t zqupos[kmax][3];
  Float_t zqudir[kmax][3];

  if (TrTreeVersion < 3) {
    hisTree->SetBranchAddress("fixnhit",nhit[0]   );
    hisTree->SetBranchAddress("fixidx", idx[0]   );
    hisTree->SetBranchAddress("rebnhit",nhit[1]    );
    hisTree->SetBranchAddress("rebidx", idx[1]   );
    hisTree->SetBranchAddress("zqunhit",nhit[2]    );
    hisTree->SetBranchAddress("zquidx" ,idx[2]    );

    hisTree->SetBranchAddress("fixntrack",&ntrack[0] );
    hisTree->SetBranchAddress("rebntrack",&ntrack[1] );
    hisTree->SetBranchAddress("zquntrack",&ntrack[2] );

    hisTree->SetBranchAddress("fixrgt",rgt[0]    );
    hisTree->SetBranchAddress("fixpos",pos[0]    );
    hisTree->SetBranchAddress("fixdir",dir[0]   );

    hisTree->SetBranchAddress("rebrgt",rgt[1]    );
    hisTree->SetBranchAddress("rebpos",pos[1]    );
    hisTree->SetBranchAddress("rebdir",dir[1]   );

    hisTree->SetBranchAddress("zqurgt",rgt[2]    );
    hisTree->SetBranchAddress("zqupos",pos[2]    );
    hisTree->SetBranchAddress("zqudir",dir[2]    );
  }
  else {
    hisTree->SetBranchAddress("zquntrack", &zquntrack);
    hisTree->SetBranchAddress("zqunhit",    zqunhit);
    hisTree->SetBranchAddress("zquidx",     zquidx);
    hisTree->SetBranchAddress("zqurgt",     zqurgt);
    hisTree->SetBranchAddress("zqupos",     zqupos);
    hisTree->SetBranchAddress("zqudir",     zqudir);
  }

  hisTree->GetEntry(isearch);

  if (TrTreeVersion == 3) {
    ntrack[2] = zquntrack;
    for (unsigned int iTrack = 0; iTrack < zquntrack; ++iTrack) {
      nhit[2][iTrack] = zqunhit[iTrack];
      for (unsigned int iHit = 0; iHit < 9; ++iHit)
        idx[2][iTrack][iHit] = zquidx[iTrack][iHit];
      rgt[2][iTrack] = zqurgt[iTrack];
      for (unsigned int iCoord = 0; iCoord < 3; ++iCoord) {
        pos[2][iTrack][iCoord] = zqupos[iTrack][iCoord];
        dir[2][iTrack][iCoord] = zqudir[iTrack][iCoord];
      }
    }
  }

  const double lz[9] = {159.065, 53.044, 29.225,25.239 , 1.708, -2.294, -25.254, -29.255, -136.042};

  vector<TrTrackR> &vec = evt->TrTrack();
  if( TRCLFFKEY.MergeDiffRecon != 11 )   vec.clear();
  if( ntrack[rtype] >0    )  {
    for(int itk=0;itk< ntrack[rtype]; itk++ ){
      if( TRCLFFKEY.MergeDiffRecon ==11 ){
        bool isoverlap = false;
        for(int ihit = 0;ihit<nhit[rtype][itk]; ihit++  )  {
          int tdx = idx[rtype][itk][ihit];
          if(tdx<0) continue;
          TrRecHitR *hit1 = evt->pTrRecHit(tdx);
          if(!hit1) continue;
          int ilayer1  = hit1->GetLayerJ();
          for(unsigned int iex=0;iex< evt->NTrTrack();iex++) {
            TrTrackR *track = evt->pTrTrack(iex);
            if(!track) continue;
            TrRecHitR *hit2 = track->GetHitLJ( ilayer1 );
            if( !hit2  ) continue;
            if( hit2->GetYClusterIndex ()  ==  hit1->GetYClusterIndex ()   ) {
              isoverlap = true; track->SetRecType(3);  break; }
            if(  (hit1->OnlyY () || hit2->OnlyY() == false ) &&
                ( hit2->GetXClusterIndex ()  ==  hit1->GetXClusterIndex()) ){
              isoverlap = true;  track->SetRecType(3);  break; }
          }
          if( isoverlap ) { break;}//overlap track
        }
        if( isoverlap  ) continue; // itk++
      }
      AMSPoint pcc; AMSDir pdd;
      pcc = AMSPoint( pos[rtype][itk][0], pos[rtype][itk][1],pos[rtype][itk][2] );
      pdd = AMSDir( dir[rtype][itk][0],  dir[rtype][itk][1], dir[rtype][itk][2] );
      float rig = rgt[rtype][itk];
      if(rig == 0) continue;
      TrProp tprop(pcc, pdd, rig);
      AMSPoint tkpcc[9];
      for(int il=0;il<9;il++){
        tprop.Propagate( lz[il] );
        tkpcc[il] =  tprop.GetP0();
      }
      TrTrackR *track = new TrTrackR(0);
      if( TRCLFFKEY.MergeDiffRecon ==11 )  track->SetRecType(10);
      //      int nhit = 0;
      int nadd = 0;
      for(int ihit = 0;ihit<nhit[rtype][itk]; ihit++ )  {
        int tdx = idx[rtype][itk][ihit];
        if(tdx<0) continue;
        TrRecHitR* hit = evt->pTrRecHit(tdx );
        if(!hit) continue;
        int ilay = hit->GetLayerJ();
        TkSens tks = EstimateXCoord( tkpcc[ilay-1]  );
        if (!tks.LadFound() || tks.GetLadTkID() != hit->GetTkId()) continue;
        int nsens = tks.GetMultIndex();
        int nmlt = hit->GetMultiplicity();
        int imlt = -1;
        if( nsens >= nmlt  )  	 imlt = nmlt-1;
        else if(  nsens < 0  )       imlt = 0;
        else imlt = nsens;
        if( hit->OnlyY()  )  hit->SetDummyX(tks.GetStripX());
        hit->SetResolvedMultiplicity( imlt );
        track->AddHit(hit);
        hit->SetUsed();
        nadd++;
      }
      if( nadd >=3  ){
        if (!patt) {
          int nn = (TkDBc::Head->GetSetup()==3||TkDBc::Head->GetSetup()==4) ? 7 : 8;
          patt = new tkpatt(nn);
          patt->Init(nn);
        }
        int masky = 0x7f, maskc = 0x7f;
        for(int i=0;i<track->GetNhits();i++ ){
          TrRecHitR *hit = track->GetHit(i);
          unsigned int bit = 1 << (patt->GetSCANLAY()-hit->GetLayer());
          bit = ~bit;
          if (!hit->OnlyX()) masky &= ~(1 << (patt->GetSCANLAY()-hit->GetLayer()));
          if (!hit->OnlyY()) maskc &= ~(1 << (patt->GetSCANLAY()-hit->GetLayer()));
        }

        track->SetPatterns(patt->GetHitPatternIndex(maskc),
            patt->GetHitPatternIndex(masky),
            patt->GetHitPatternIndex(maskc),
            patt->GetHitPatternIndex(masky));
        track->UpdateBitPattern();
        int mfit2 = TrTrackR::DefaultFitID;
        track->FitT(mfit2);
        if (track->FitDone(mfit2)) track->Settrdefaultfit(mfit2);
        track->RecalcHitCoordinates(mfit2);
        track->DoAdvancedFit();
      }
      if ( nadd >=3&&  track) { vec.push_back(*track); }
      else delete track;

    }// track;
    evt->UpdateTrRecon();
    for (unsigned int i = 0; i < evt->NParticle(); i++)
      evt->pParticle(i)->UpdateTrTrack(5, 5);

  }
  return vec.size();

  return -1;
}


int TrReconQ::SetBuildAddress()
{
  unsigned int trun = evt->Run();
  string trdir = "";
  TString evPath = evt->Tree()->GetCurrentFile()->GetName();
  TObjArray *tp = evPath.Tokenize("/");
  Int_t ns = tp->GetEntries();
  TString tname = ((TObjString *)(tp->At(ns-1)))->String();
  if ( gSystem->Getenv("TrTreeList") ){
      trdir =  Form("%s/hitidx_%s", gSystem->Getenv("TrTreeList"), tname.Data() ) ;
  }
  else{
    if (TrTreeVersion == 3)
      trdir = Form("root://eosams.cern.ch//eos/ams/Data/pass6.trreon/hit_comb_v3.1/hitidx_%s",tname.Data());
    else if (TrTreeVersion == 2)
      trdir = Form("root://eosams.cern.ch//eos/ams/Data/pass6.trreon/hit_comb_v2.1/hitidx_%s",tname.Data());
    else if (TrTreeVersion == 1)
      trdir = Form("root://eosams.cern.ch//eos/ams/Data/pass6.trreon/hit_comb_v1.1/hitidx_%s",tname.Data());
    else {
      static int nmessage = 0;
      if (nmessage < 100) {
        std::cout << "TrReconQ::SetBuildAddress-W-InvalidTrTreeVersion TrTreeVersion must be between 1 and 3 but is " << TrTreeVersion << "." << std::endl;
        ++nmessage;
      }
      return -1;
    }
  }
  TChain *tree = new TChain("ReCombo");
  if( !tree  )  {
    return -1;
  }
  if( !tree->Add(trdir.c_str() ) )  {
    delete tree;
    return -1;
  }
  if(  tree->GetEntries()<=1 ) {
    delete tree;
    return -1;
  }

  if(  !hisTree ||  hisrun != trun || hisname != evPath ) {
    //set address here
    delete hisTree;
    hisTree = tree;
    hisname = evPath;
    return 1;
  }
  // Just continue.
  //  use his tree as it is.
  delete tree;
  return 1;
}

int TrReconQ::SearchEventFast(){

  hisrun = evt->Run();
  hisname = evt->Tree()->GetCurrentFile()->GetName();
  int tevt = evt->Event();
  int tentry = evt->Entry();
  int nentry = hisTree->GetEntries();
  bool isfound = false;
  int uplim = nentry -1;
  //  if( uplim >= nentry -1)  uplim = nentry - 1;
  int lwlim = 0;
  hisTree->SetBranchAddress( "run",  &_rrun   );
  hisTree->SetBranchAddress( "event", &_revt  );
  hisTree->SetBranchAddress( "entry", &_rentry);
  int ibecon = tentry > uplim ? uplim : tentry;  // this evt entry;
  hisTree->GetEntry(ibecon);
  int nloop = 0;
  while( !isfound )//dynamic search, should be very fast.
  {
    if( nloop++ >100) { isfound = true; return -1;}
    hisTree->GetEntry(ibecon);
    if( _rrun != hisrun  ) return -1;
    if(_revt == tevt) {
      return ibecon;
    }

    int idis = _rentry - tentry;
    if( idis > 0 ){
      uplim = _rentry;
      ibecon =   ( ibecon - idis < lwlim ) ?( ibecon - idis ) : lwlim;
    }
    else if( idis <0 ){
      lwlim = _rentry;
      ibecon = ( ibecon + idis >  uplim ) ? ( ibecon + idis  ):  uplim;
    }
    else{
      //we found that the entry is the same, but the event is not,
      // so, we encounter the double file run.
      return -2;
    }
  }
  return -1;
}


int TrReconQ::BuildTracks(int trstat, int hist){

//------
  Clear();
  InitBuild();

//------
/*  _tkStat = -1; _cutccList=0;
  int nbuild = BuildAll();//require 4 planes for zseed
  if(GetNTrTrack()==0){//loose angle selection for zseed
    _tkStat = 0; _cutccList=0;
    nbuild += BuildAll();
  }*/
  _tkStat = 0; _cutccList=0;//zseed rec
  int nbuild = BuildAll();
  _tkStat = 1; _cutccList=0;//2nd rec, increase buffer
  nbuild += BuildAll();
  if(GetNTrTrack()==0&&_cutccList>0){//3rd rec, increase buffer again
    _tkStat = 2; _cutccList=0;
    nbuild += BuildAll();
  }
  if((GetNTrTrack()==0&&TRCLFFKEY.MaxZQAngleF>0)||TRCLFFKEY.MaxZQAngleF<0){//4th rec, increase angle+x windows
    _tkStat = 3; _cutccList=0;
    nbuild += BuildAll();
  }

//------
  MergeClean();

//------
  ResetHitStat();

//------Final
  if (hist > 0) {
    bool usev6=(TRCLFFKEY.recflag/10000000%10>=1);
    int refit=usev6?30:0;
    trstat = CountTracks(trstat,refit);
    trstat = FillHistos (trstat,refit);
  }

  return trstat;
}


int TrReconQ::MergeClean(){

  VCon *cont=GetVCon()->GetCont("AMSTrTrack");
  if(cont)cont->eraseC();
  for(unsigned int ik=0;ik<_atracks.size();ik++){
    if(cont)cont->addnext(_atracks[ik]);
    else {delete _atracks[ik];}
  }
  int ntk=_atracks.size();
  if(cont)delete cont;
  else    ntk*=-1;
  _atracks.clear();
  return ntk;
}


void TrReconQ::ResetHitStat(){
    VCon *conth = GetVCon()->GetCont("AMSTrRecHit");
    int nhit = (conth) ? conth->getnelem() : 0;
    for(int ihit=0;ihit<nhit;ihit++){
      TrRecHitR *hit = (TrRecHitR *) conth->getelem(ihit);
      hit->ClearUsed ();
    }
    if(conth)delete conth;

/*    VCon *contt = GetVCon()->GetCont("AMSTrTrack");
    if (!contt) return;
    int ntk = contt->getnelem();
    for(int ik=0;ik<ntk;ik++){
      TrTrackR *track = (TrTrackR *)contt->getelem(ik);
      if( !track  ) continue;
      track->ReFitHitStat();
    }
    delete contt;*/
    ResetHitUsed();
}


void TrReconQ::ResetHitUsed(){
    VCon *cont = GetVCon()->GetCont("AMSTrTrack");
    if (!cont) return;
    int ntk = cont->getnelem();
    for(int ik=0;ik<ntk;ik++){
      TrTrackR *track = (TrTrackR *)cont->getelem(ik);
      for (int i = 0; i < track->GetNhits(); i++) {
         TrRecHitR  *hit = track->GetHit(i);
         if(!hit)continue;
         hit->SetUsed();
      } 
    }
    delete cont;
}


int TrReconQ::BuildTracksVertex(){
  BuildTracks();
  trType = 1;
  if( BdVtx >0 ) {
    BuildVertex();
  }
  return  0;
}


void TrReconQ::Clear()
{
    for(unsigned int ik=0;ik<_atracks.size();ik++)delete _atracks[ik];
    _atracks.clear();
    _rhits.clear();
    trType  = 0;
    _nsvtx  = 0;
    _reorderHits = 0;
    _isNuclei = false;
    _qNuclei = 0;
    _tkStat=_cutccList=0;

}


int TrReconQ::InitBuild(int select_tag){

//----Get Hits
    bool usevtxhit=(((TRCLFFKEY.recflag/10)%10)>=2);
    _rhits.clear();
    VCon *conth = GetVCon()->GetCont("AMSTrRecHit");
    int nhit = (conth) ? conth->getnelem() : 0;
    for(int ihit=0;ihit<nhit;ihit++){
      int opt=1;//QY add (clear angle+charge)
      TrRecHitR *hit = (TrRecHitR *) conth->getelem(ihit);
      if(select_tag!=0){if(!hit->checkstatus(select_tag))continue;}//partial
      else if(!usevtxhit){
        if(hit->checkstatus(TrRecHitR::VERTEX))continue;
      }
      hit->ClearUsed (opt);
      _rhits.push_back(hit);
    }
    if(conth)delete conth;

//----Reorder Hits and Eval Nuclei
  _reorderHits =0;
  _isNuclei = false;
  _qNuclei = 0;
  if(  TRCLFFKEY.TrParticleType == 1 ) {
    _reorderHits = 1;
    double dnuclei = evaNuclei( _reorderHits);//bug fixed by QY: sort with YQ+XYMatch first, X*muplicity is limited to be <70
    if( dnuclei > 0  ) {
      _isNuclei = true;
      _qNuclei = dnuclei;
    }
   }
   return 0;
}


int TrReconQ::BuildccList(vector<ccL> &ccList, vector<ccL> &ccList19, int select_tag, int nmaxcclistscale)
{
  ccList.clear();
  ccList19.clear();
  int nhit=_rhits.size();
  if(nhit < 3  ) return 0;

  bool usev6=(TRCLFFKEY.recflag/10000000%10>=1);

  //----nmaxccList
  int nmaxzqcclist=TRCLFFKEY.MaxZQccList;//1st and 2nd reconsruction
  if(nmaxcclistscale>0)nmaxzqcclist*=nmaxcclistscale; 

  if (!patt) {
    int nn = (TkDBc::Head->GetSetup()==3||TkDBc::Head->GetSetup()==4) ? 7 : 8;
    patt = new tkpatt(nn);
    patt->Init(nn);
  }

  // Debug control
  bool DEBUG=false;

  if(DEBUG) cout << "BuildTracks select_tag= " << select_tag << endl;
  ///////////////////////////////////////////////////////////
  /////////fill the hits coord into the buffer          /////
  for (int ihit=0; ihit<nhit; ihit++) {
    TrRecHitR* rhit = _rhits[ihit];
    if (!rhit) continue;
    if( (select_tag!=0) && !rhit->checkstatus(select_tag)  ) continue;
    if(rhit->OnlyX())continue;//onlyX is not used, require at least Y, should never happen
    if(rhit->Used()||rhit->GetYCluster()->Used()) continue;//shared X could happen in different tkStat
    int nLJ  = rhit->GetLayerJ();
    int iCly = rhit->GetYClusterIndex();
    int iClx = rhit->GetXClusterIndex();
    if(_reorderHits==1 && int(ccList.size())>=nmaxzqcclist){
      ccList.pop_back();//remove last incomplete ccList
      _cutccList=1;
      break;
    }
//----
    vector<ccL> &ccListn=(nLJ>8||nLJ<2)?ccList19:ccList;
    ccL *pccL=0;
    for(unsigned int ic=0;ic<ccListn.size();ic++){
      if(ccListn[ic].icly==iCly){pccL=&(ccListn[ic]);break;}
    }
    if(pccL==0){//add a new one
      ccListn.push_back(ccL());
      pccL=&(ccListn.back());
    }
//----
    TrClusterR* clx=(TrClusterR*)rhit->GetXCluster();
    TrClusterR* cly=(TrClusterR*)rhit->GetYCluster(); 
    double tqx=clx?clx->GetQ():0;//Has X
    double tqy=cly->GetQ();
    double com[3]={0};//averaged coordinate
    int nsensor=usev6?rhit->nSensor():rhit->GetMultiplicity();
    for(int isen=0;isen<nsensor;isen++){
      TVector3 coo;
      if(usev6){
        AMSPlaneM plm=rhit->GetGCoordN(isen);
        coo=plm.getMGlobal();
      }
      else {
        AMSPoint poo=rhit->GetCoord(isen);
        for(int ii=0;ii<3;ii++)coo[ii]=poo[ii];
      }
      if(clx)pccL->AddX(coo[0],coo[1],coo[2],ihit,isen,nLJ,iCly,iClx,tqx,tqy);//HasX
      for(int ii=0;ii<3;ii++)com[ii]+=coo[ii];
    }
    for(int ii=0;ii<3;ii++)com[ii]/=nsensor;
    if(!clx){
      double xlh[2]={0};
      if(usev6){//v6 version
        xlh[0]=TkTrackN::GetHead()->GetGlobalCoo(rhit->GetTkId(),0.,-10.).getMGlobal()[0];//xlow edge in ladder
        xlh[1]=TkTrackN::GetHead()->GetGlobalCoo(rhit->GetTkId(),rhit->GetLadder()->nchans[0]-1.,-10.).getMGlobal()[0];//xhigh edge in ladder
        if(xlh[0]>xlh[1]){double temp=xlh[0];xlh[0]=xlh[1];xlh[1]=temp;}
      }
      pccL->SetOnlyY(com[1],com[2],ihit,nLJ,iCly,tqy,xlh[0],xlh[1]);//YOnly assign averaged
    }
  }

  //<=  end of filling coordinate to buffer.
 
  //SORT VIA Z DECREASE CHARGE
  int ncl=ccList.size(); 
  if(ncl >= nmaxzqcclist) {
    ccL tmp;
    for(int i=0;i<ncl-1;i++){
      for(int j=0;j<ncl-i-1;j++){
        if(ccList[j].qy < ccList[j+1].qy){
          tmp = ccList[j];
          ccList[j] = ccList[j+1];
          ccList[j+1] = tmp;
        }
      }
    }
    while (int(ccList.size()) >= nmaxzqcclist){
       ccList.pop_back();
    }
    _cutccList=1;
  }
  ncl = ccList.size();

  //again SORT VIA Z Position DECREASE ORDRER
  ccL tmp;
  for(int i=0;i<ncl-1;i++){
    for(int j=0;j<ncl-i-1;j++){
      if(ccList[j].cz < ccList[j+1].cz){
        tmp = ccList[j];
        ccList[j] = ccList[j+1];
        ccList[j+1] = tmp;
      }
   }}


  return ccList.size();
}


int TrReconQ::BuildsemiIdx(vector<semiIndex> &semiIdx, vector<ccL> &ccList, int select_tag, float kiklimtscale){


  semiIdx.clear();
  int ncl = ccList.size();
  if(ncl < 3 )return 0;

// rechits
  int nRechit = _rhits.size();

// memory leak control
  bool status=true;

// EVET by event debug
  bool heavyMode = false;
  int  heavyIdx   = -1;
  if(nRechit>280) heavyMode = true;

// NUCLEI check
  double kiklimt = 0.0021;
  if(kiklimtscale>0)kiklimt*=kiklimtscale;
/*  if(  heavyMode)  {
    if(  ! _isNuclei ) {
      double weightR = 1. + 19./pow( 380./(1.0*nRechit -120),3  );
      if(kiklimtscale<0.99) kiklimt /= weightR;
      else                  kiklimt /= (weightR/3);
    }
  }*/
  //Rth=0.3*(l1+l2)*B/2/sqrt(2*kik)=0.016*(l1+l2)/sqrt(k): l1+l2=0.28(0.51)
  double v_cos[5] = {-1.0+kiklimt, -1.0+1.2*kiklimt, -1.0+1.2*kiklimt, -1.0+1.1*kiklimt, -1.0 + kiklimt};

  vcos headcos;
  vcos headcosXZ;


  int nNeuron=0;
  vector<vector<int>  > sNeu;
  sNeu.resize(ncl);
  for(int i=0;i<ncl;i++)
    sNeu[i].resize(ncl);
  //int sNfl[ncl][ncl][ncl];
  for(int i=0;i<ncl;i++) {
    for(int j=0;j<ncl;j++){
      sNeu[i][j]=-9999;
      if(ccList[i].lay < ccList[j].lay)      {
        sNeu[i][j]=1;
        nNeuron++;
      }
    } }

  if(nNeuron <=0 ) return 0;
  ///////////////////////////////////////////////////////////
  /////////  start the CATS iteration process   ////////////

  for(int isel=0;isel<6;isel++){
    for(int i=0;i<ncl-2;i++) {
      for(int j =i+1;j<ncl-1;j++) {
        if(ccList[i].lay >= ccList[j].lay) continue;
        int flgS=1;
        int lmax=1;
        for(int k=j+1;k<ncl;k++)    {
          if(ccList[j].lay >= ccList[k].lay) continue;
          if(sNeu[i][j]!=sNeu[j][k]) continue;
          if(headcos.getcosyz(ccList[i],ccList[j],ccList[k])>v_cos[ccList[j].lay-3] ) continue;
          lmax=sNeu[j][k];
          flgS=0;
        }
        if(flgS==0){
          sNeu[i][j]=lmax+1;
        } }
    }
  }// 7 layer, only 6 neuron, 6 times is necessary

  // add them to a single array;
  vector<vector<int> > tmin;
  tmin.resize( nNeuron );
  for(int i=0;i<nNeuron;i++ )
    tmin[i].resize( 3  );
  int ntmin=0;
  int nS[6] = {0,0,0,0,0,0};
  for (int i=0;i<nNeuron;i++) {
    for(int j=i+1;j<3;j++) {
      tmin[i][j] = -9999;
    }}
  for (int i=0;i<ncl-1;i++) {
    for(int j=i+1;j<ncl;j++) {
      if(sNeu[i][j]==-9999) continue;
      tmin[ntmin][0] = sNeu[i][j];
      tmin[ntmin][1] = i;
      tmin[ntmin][2] = j;
      nS[6-sNeu[i][j]]++;
      ntmin++;
    }
  }

  int tmp_tmin[3];
  for(int i=0;i<ntmin-1;i++)
  {
    for(int j=0;j<ntmin-i-1;j++)
    {
      if(tmin[j][0]  <  tmin[j+1][0]){
        tmp_tmin[0] = tmin[j][0];
        tmp_tmin[1] = tmin[j][1];
        tmp_tmin[2] = tmin[j][2];

        tmin[j][0] =tmin[j+1][0];
        tmin[j][1] =tmin[j+1][1];
        tmin[j][2] =tmin[j+1][2];

        tmin[j+1][0] = tmp_tmin[0];
        tmin[j+1][1] = tmp_tmin[1];
        tmin[j+1][2] = tmp_tmin[2];
      }
    }
  }
  if(tmin[0][0]<2)
  {
    return 0;
  }

  if(heavyMode){
/*    heavyIdx = tmin[0][0];//require longest track
    if((select_tag!=0) && _isNuclei) heavyIdx = tmin[0][0]-1;//shorter also allow for nuclei*/
    heavyIdx = tmin[0][0]-1;//longest track-1
  }
  semiIndex tmpIdx;
  int iTrack=0;
  int inue[6]={0,0,0,0,0,0};
  int flg[6]={1,1,1,1,1,1};
  int adderS[6] = {0,0,0,0,0,0};
  for(int i=0;i<6;i++)
    for(int j=0;j<6;j++)
      adderS[i] += ( i<j ? 0 : nS[j] );
  //<=  end of iteration of xz track process.


  const int NCandi = 1000;
  /////     merge all subtracks to buffer   //////
  ////           start to merge              /////
  for(inue[0]=0;inue[0]<ntmin;inue[0]++){
    if(tmin[inue[0]][0] <heavyIdx|| tmin[inue[0]][0]<2) continue;
    flg[1]=1;
    int id0 = 6 - tmin[inue[0]][0];
    if( id0+1<6 )
      for(inue[1]=adderS[id0]; inue[1]<ntmin;inue[1]++){
        if(tmin[inue[0]][2]!=tmin[inue[1]][1]) continue;
        if(tmin[inue[1]][0]<1) continue;
        if(tmin[inue[0]][0]-tmin[inue[1]][0] < 1 ) continue;
        if(headcos.getcosyz(ccList[tmin[inue[0]][1]],
              ccList[tmin[inue[0]][2]],ccList[tmin[inue[1]][2]])>
            v_cos[ccList[tmin[inue[0]][2]].lay-3]  ) continue;
        flg[2]=1;
        if( id0+2<6 )
          for(inue[2]=adderS[id0+1];inue[2]<ntmin ;inue[2]++){
            if(tmin[inue[1]][2]!=tmin[inue[2]][1]) continue;
            if(tmin[inue[2]][0] < 1 ) continue;
            if(tmin[inue[1]][0]-tmin[inue[2]][0] < 1) continue;
            if(headcos.getcosyz(ccList[tmin[inue[1]][1]],
                  ccList[tmin[inue[1]][2]],ccList[tmin[inue[2]][2]])>
                v_cos[ ccList[tmin[inue[1]][2]].lay-3]   ) continue;
            flg[3]=1;
            if( id0+3<6 )
              for(inue[3]=adderS[id0+2];inue[3]<ntmin;inue[3]++){
                if(tmin[inue[2]][2]!=tmin[inue[3]][1])continue;
                if(tmin[inue[3]][0]<1) continue;
                if(tmin[inue[2]][0]-tmin[inue[3]][0] < 1) continue;
                if(headcos.getcosyz(ccList[tmin[inue[2]][1]],
                      ccList[tmin[inue[2]][2]],ccList[tmin[inue[3]][2]]) >
                    v_cos[ ccList[tmin[inue[2]][2]].lay-3 ]   ) continue;
                flg[4]=1;
                if( id0+4<6 )
                  for(inue[4]=adderS[id0+3];inue[4]<ntmin;inue[4]++ ){
                    if(tmin[inue[3]][2]!=tmin[inue[4]][1]) continue;
                    if(tmin[inue[4]][0]<1) continue;
                    if(tmin[inue[3]][0]-tmin[inue[4]][0] < 1) continue;
                    if(headcos.getcosyz(ccList[tmin[inue[3]][1]],
                          ccList[tmin[inue[3]][2]],ccList[tmin[inue[4]][2]]) >
                        v_cos[ ccList[tmin[inue[3]][2]].lay-3 ]   ) continue;
                    flg[5]=1;
                    if( id0+5<6 )
                      for(inue[5]=adderS[id0+4];inue[5]<ntmin ;inue[5]++ ){
                        if(tmin[inue[4]][2]!=tmin[inue[5]][1]) continue;
                        if(tmin[inue[5]][0]<1) continue;
                        if(tmin[inue[4]][0]-tmin[inue[5]][0] < 1) continue;
                        if(headcos.getcosyz(ccList[tmin[inue[4]][1]],
                              ccList[tmin[inue[4]][2]],ccList[tmin[inue[5]][2]]) >
                            v_cos[ ccList[tmin[inue[4]][2]].lay-3 ]   ) continue;
                        tmpIdx.clear();
                        tmpIdx.AddHit(tmin[inue[5]][2]);
                        tmpIdx.AddHit(tmin[inue[4]][2]);
                        tmpIdx.AddHit(tmin[inue[3]][2]);
                        tmpIdx.AddHit(tmin[inue[2]][2]);
                        tmpIdx.AddHit(tmin[inue[1]][2]);
                        tmpIdx.AddHit(tmin[inue[0]][2]);
                        tmpIdx.AddHit(tmin[inue[0]][1]);
                        if(iTrack>=NCandi){ status=false; break;}
                        semiIdx.push_back(tmpIdx);
                        iTrack++;
                        flg[5]=0;
                      }
                    if(flg[5]==1){
                      tmpIdx.clear();
                      tmpIdx.AddHit(tmin[inue[4]][2]);
                      tmpIdx.AddHit(tmin[inue[3]][2]);
                      tmpIdx.AddHit(tmin[inue[2]][2]);
                      tmpIdx.AddHit(tmin[inue[1]][2]);
                      tmpIdx.AddHit(tmin[inue[0]][2]);
                      tmpIdx.AddHit(tmin[inue[0]][1]);
                      if(iTrack>=NCandi){ status=false; break;}
                      semiIdx.push_back(tmpIdx);
                      iTrack++;
                      flg[4]=0;
                    }}
                if(flg[4]==1){
                  tmpIdx.clear();
                  tmpIdx.AddHit(tmin[inue[3]][2]);
                  tmpIdx.AddHit(tmin[inue[2]][2]);
                  tmpIdx.AddHit(tmin[inue[1]][2]);
                  tmpIdx.AddHit(tmin[inue[0]][2]);
                  tmpIdx.AddHit(tmin[inue[0]][1]);
                  if(iTrack>=NCandi)  {		status=false;	break;	  }
                  semiIdx.push_back(tmpIdx);
                  iTrack++;
                  flg[3]=0;}}
            if(flg[3]==1)
            {
              tmpIdx.clear();
              tmpIdx.AddHit(tmin[inue[2]][2]);
              tmpIdx.AddHit(tmin[inue[1]][2]);
              tmpIdx.AddHit(tmin[inue[0]][2]);
              tmpIdx.AddHit(tmin[inue[0]][1]);
              if(iTrack>=NCandi){status=false;break;}
              semiIdx.push_back(tmpIdx);
              iTrack++;
              flg[2]=0;}}
        if(flg[2]==1)
        {
          tmpIdx.clear();
          tmpIdx.AddHit(tmin[inue[1]][2]);
          tmpIdx.AddHit(tmin[inue[0]][2]);
          tmpIdx.AddHit(tmin[inue[0]][1]);
          if(iTrack>=NCandi){status=false;break;}
          semiIdx.push_back(tmpIdx);
          iTrack++;
          flg[1]=0;
        }}
  }


  if(iTrack == 0)
  {
    semiIdx.clear();
    return 0;
  }
  if(semiIdx.size()  > NCandi) {cerr<<"TrReconQ::semiIdx size overflow error"<<endl;}


  ///////////////////////////////////////////////
  ///     pre-selection of y-x track         ///
  //test the linear of the semitrack
  for(unsigned int i=0; i<semiIdx.size();i++){
    int nx=0;
    for(int j=0;j<semiIdx[i].nLength;j++){
      if(ccList[semiIdx[i].iccL[j]].hasX) nx++;
      semiIdx[i].Setlay(j,ccList[semiIdx[i].iccL[j]].lay);
    }
    fastFitQu trfit1;
    TrFit trfitk;
    for(int j=0;j<semiIdx[i].nLength;j++ )
    {
      trfit1.addYZ(ccList[semiIdx[i].iccL[j]].cy,
          ccList[semiIdx[i].iccL[j]].cz);
      trfitk.Add(0.5, ccList[semiIdx[i].iccL[j]].cy,ccList[semiIdx[i].iccL[j]].cz,
          -1, 25e-4,300e-4	);
    }

    trfitk.SimpleFitTop();
//    double rig = fabs( trfitk.GetRigidity() ) ;
    double lm  = fabs(trfit1.getMean());
//    double lc  = fabs( trfit1.getCsq() )  ;
    double ly = trfitk.LinearFit(2);
    double lmth=0.19;//YZ kik angle cut for mean namely signed curvature
    if(kiklimtscale>1)lmth*=sqrt(kiklimtscale);
    if(ly < 0 || lm > lmth)semiIdx[i].isBad=true;//Rth=0.3*B*(l1+l2)/2/lmth=0.0225*(l1+l2)/lmth=0.033(0.06)GV: lm=sin(a),l1+l2=0.28(0.51)
    if(semiIdx[i].isBad)continue;//isBad do not have any quality
    double quality = 0;
    quality +=  2.3 * semiIdx[i].nLength;
/*    int nplan=semiIdx[i].GetNPlane();
    if     (nplan==4)quality+=10;
    else if(nplan==3)quality+=5;*/
    semiIdx[i].hfqql = quality;
  }

  //reorder the search by is bad and length.
  std::sort(semiIdx.begin(),semiIdx.end());

 return semiIdx.size();
}


int TrReconQ::BuildtrkCd(vector<candi> &trkCd, vector<semiIndex> &semiIdx, vector<ccL> &ccList, vector<ccL> &ccList19, float xwscale){
  
  trkCd.clear();
  if(semiIdx.size()<=0)return 0;

//-------
  map<unsigned int,vector<int> >maxqL19[2];
  if(_isNuclei&&_qNuclei >0) {
    for(unsigned int ic=0;ic<ccList19.size();ic++) {
      ccL &pcL=ccList19[ic];
      int iex=(pcL.lay==1)?0:(pcL.lay==9?1:-1);
      if(iex<0)continue;
      for(int ix=0;ix<pcL.ncx;ix++){
        if(pcL.qx[ix]>0 && fabs(pcL.qx[ix]-_qNuclei)<=0.3*_qNuclei) maxqL19[iex][ic].push_back(ix);
      }
    }
  }


//----------
  bool trdxs = false;
  bool tofxs = false;

  vector<AMSPoint > tofcc;
  vector<int > tofly;
  vector<AMSPoint > tofecc;
  vector<float > tofrq;//raw charge
  vector<float > tofbw;
  VCon* tcont = GetVCon()->GetCont("AMSTOFClusterH");
  int ntofhit = (tcont)? tcont->getnelem(): 0;
  for(int ii=0;ii<ntofhit;ii++){
    TofClusterHR* tofhit = (TofClusterHR*)tcont->getelem(ii);
    if(!(tofhit->IsGoodTime())) continue;
    AMSPoint ecoo(tofhit->ECoo);
    AMSPoint coo(tofhit->Coo);
    tofcc.push_back(coo);
    tofecc.push_back(ecoo);
    int il = tofhit->Layer;
    tofly.push_back( il );
    tofrq.push_back(tofhit->GetQSignal(-1));
    tofbw.push_back(tofhit->GetBarWidth());
  }
  if(tcont)delete tcont;
  if( tofcc.size() >1   ) tofxs = true;

//-----
  AMSPoint ptrd;
  AMSDir   dtrd;
  vector<AMSPoint > trdc;
  vector<AMSPoint > trdd;
#ifndef __ROOTSHAREDLIBRARY__
  AMSTRDTrack *trd = (AMSTRDTrack*)AMSEvent::gethead()
    ->getheadC("AMSTRDTrack", 0, 1);
  for (; trd; trd = trd->next()) {
    ptrd = trd->getCooStr();
    dtrd = trd->getCooDirStr();
#else
    int ntrd = evt->nTrdTrack();
    for(int itrd=0;itrd<ntrd;itrd++)       {
      TrdTrackR *trd=evt->pTrdTrack(itrd);
      ptrd = AMSPoint(trd->Coo[0], trd->Coo[1], trd->Coo[2]);
      dtrd = AMSDir  (trd->Theta,  trd->Phi);// get the trd direction and  coordinate
#endif
      if( ptrd.z()==0 || dtrd[2] == 0  )  continue;
      if( ptrd.z()==0 || dtrd[2] == 0  )  continue;
      trdc.push_back(ptrd);
      trdd.push_back(dtrd);
      trdxs = true;
    }

 
//---------
  const int NCandiX = 1000;
  map<int,int> hitptt;
  int nraw=0;
  for(unsigned int iter6 = 0; iter6<semiIdx.size(); iter6++)
  {
    if(semiIdx[iter6].isBad) continue;
    if(trkCd.size() > NCandiX)break;
    int nL = semiIdx[iter6].nLength;
    int nxL = 0;
    for(int iz=0;iz<nL; iz++  )
    {
      if( ccList[semiIdx[iter6].iccL[iz]].hasX==false  ) continue;
      nxL++;
    }
    if(nxL<2) continue;//require X>=2

    //length<max-2  semiIdx are excluded
    bool isInc = false;
    for(int iz=0;iz<nL;iz++){
      int iccL=semiIdx[iter6].iccL[iz];
      map<int,int>::iterator it=hitptt.find(iccL);
      if(it!=hitptt.end()&&nL<=it->second-2){isInc = true; break; }
    }
    if(isInc) continue;

    vector<double> cx;
    vector<double> cz;
    vector<int> cID;
    vector<int> nixL;
    vector<int> xicx;
    nix_L ccixL;
    for(int icd = 0;icd < semiIdx[iter6].nLength; icd++)
    {
      int icdx = semiIdx[iter6].iccL[icd];
      if( ccList[icdx].hasX == false  ) continue;
      nixL.push_back( ccList[icdx].ncx );
      ccixL.Add( ccList[icdx].ncx,icd );
      for(int id0=0;id0<ccList[icdx].ncx;id0++)
      {
        cx.push_back( ccList[icdx].cx[id0] );
        xicx.push_back(id0);
        cz.push_back( ccList[icdx].cz1[id0]  );
        cID.push_back(icdx);
      }
    }
    int nlevel = ccixL.nlevel;
    vector<vector<int>  > cxzCandi;
    vector<int> bff;
    vector<int> badhits;
    int nfind=0;
    int limtop[2] = { 0,                   ccixL.mixL[0]       };
    int limbtm[2] = { ccixL.mixL[nlevel-2],ccixL.mixL[nlevel-1]};
    if(nlevel>=3){limtop[1]=ccixL.mixL[1]; limbtm[0]=ccixL.mixL[nlevel-3];}
    unsigned int maxlx = 0;
    tryallagin:
    for(int itop = limtop[0]; itop<limtop[1]; itop++) {
      for(int ibtm = limbtm[1]-1; ibtm>=limbtm[0]; ibtm--) {
      //building track road here.
        int ilvtop = ccixL.GetLevel(itop);
        int ilvbtm = ccixL.GetLevel(ibtm);
        if( ilvtop<0|| ilvbtm<0   ) continue;
        if( ilvtop>=ilvbtm  ) continue;
        //calculate in fiducial volume for all hits (v6 version)
        int badhit=0;
        int nxmatch=0; 
        for(int icd = 0;icd < semiIdx[iter6].nLength; icd++){
          int icdx = semiIdx[iter6].iccL[icd];
          ccL *pccL=&(ccList[icdx]);
          if(pccL->rx2[0]>=pccL->rx2[1])continue;// check X within all layers range
          double xn=Intpol1(cz[itop],cz[ibtm],cx[itop],cx[ibtm],pccL->cz);
          if(xn>=pccL->rx2[0]-1.&&xn<=pccL->rx2[1]+1.)nxmatch++;//1 cm match, more than enough
          else badhit|=(1<<icd);
        }
        //-----
        bff.clear();
        bff.push_back(itop);
        for(int ix  =  0; ix< ccixL.mixL[nlevel-1]  ;ix++){
           int ttlv = ccixL.GetLevel(ix);
           if( ttlv <= ilvtop || ttlv >= ilvbtm  ) continue;
           double ttc = Intpol1( cz[itop],cz[ibtm] ,cx[itop], cx[ibtm] , cz[ix]  );
           double tmpx = 0.20; int tmpi = -1;
           for(int it=ccixL.mixL[ttlv-1]  ; it< ccixL.mixL[ttlv];it++  )
           {
              if( fabs(ttc - cx[it]) < fabs(tmpx)  ) {
                tmpx = fabs(ttc - cx[it]);
                tmpi = it;
              }
           }
           double xwindow=0.05;//500um
           if(xwscale>0)xwindow*=xwscale;
           if(fabs(tmpx) > xwindow|| tmpi < 0) continue;//500 um windows
           bff.push_back(tmpi);
           ix = ccixL.mixL[ttlv]-1;//ix have to add one afterwards
        }
        bff.push_back(ibtm);
        if(bff.size()<2 ) {bff.clear();continue;}
        //or we get an ideal combinaiton.
        fastFitQu trfit1;
        for(unsigned int i=0;i<bff.size();i++)trfit1.addYZ( cx[bff[i]], cz[bff[i]]);
        double axzth[2]={0.06, 0.03};//XZ kik angle cut for mean namely signed curvature and rms 
        if(xwscale>1){axzth[0]*=xwscale; axzth[1]*=xwscale;}
        if(  bff.size()<4 && fabs(  trfit1.getMean()) > axzth[0] ) continue;//M*l1*l2/(l1+l2)= M*(12e4,3.4e4),7200-2000um
        if( bff.size()>= 4 &&  trfit1.getCsq() > axzth[1] ) continue;//C*l1*l2/(l1+l2)= C*(12e4,3.4e4),3600-1000um
        if( nfind>=1 && bff.size()<maxlx ) continue;//try to keep max
        if( maxlx<bff.size() ) maxlx = bff.size();
        cxzCandi.push_back( bff );
        badhits.push_back(badhit);
      }
    }
    if(nfind==0&&nlevel>=5&&int(maxlx)<nlevel-2){cxzCandi.clear(); limtop[1]=ccixL.mixL[nlevel-2]; limbtm[0]=ccixL.mixL[0]; nfind++; goto tryallagin;}
//-------

    unsigned int imax = 0;
    for(unsigned int i=0;i< cxzCandi.size();i++)
      if( cxzCandi[i].size() > imax ) imax = cxzCandi[i].size();
    int nchosen=0;
    vector<candi> tmpCds;
    imaxminus:
    for(unsigned int iChosen =0; iChosen < cxzCandi.size();iChosen++){
      if( cxzCandi[iChosen].size() != imax  ) continue;//max X was selected

      candi tmpCandi;
//----1D Fit
      TrFit trfit2;
      fastFitQu trfit4;
      fastFitQu trfit5;
      for(unsigned int j=0;j<cxzCandi[iChosen].size();j++){
        int ttidx = cxzCandi[iChosen][j];
        int iccL=cID[ttidx];
        int icx=xicx[ttidx];
        ccL &pcL=ccList[iccL];
        tmpCandi.AddHit(pcL.ihit[icx],pcL.iclx[icx],pcL.icly,pcL.imlt[icx],pcL.lay,iccL,icx,pcL.cx[icx],pcL.cy1[icx],pcL.cz1[icx],pcL.qx[icx],pcL.qy);
        trfit2.Add(pcL.cx[icx],pcL.cy1[icx],pcL.cz1[icx],80e-4,25e-4,300e-4);
        trfit4.addYZ(pcL.cy1[icx],pcL.cz1[icx]);
        trfit5.addYZ(pcL.cx[icx], pcL.cz1[icx]);
      }
      double lx=trfit2.LinearFit(1);//X linear Fit
      if(tmpCandi.nLengthX >3  ){//ChisX Cut
        double xchisth=TRCLFFKEY.ZQXChisqCut;
        if(xwscale>1)xchisth*=(xwscale*xwscale);
        if( lx > xchisth || lx<= 0) continue;
      }
      int nbadhits=0;
      semiIndex semib=semiIdx[iter6];
      for(int j=0;j<semiIdx[iter6].nLength;j++){
        int iccL=semiIdx[iter6].iccL[j];
        ccL &pcL=ccList[iccL];
        if((badhits[iChosen]&(1<<j))||(tmpCandi.ilay[pcL.lay]<0&&pcL.ihity<0)){//skip this plane
          semib.remove(semiIdx[iter6].lay[j]);
          nbadhits++;
          continue;
        }
        if(pcL.ihity<0) continue;
        if(tmpCandi.AddSemi(pcL.ihity,pcL.icly,pcL.lay,iccL,pcL.cy,pcL.cz,pcL.qy)<0)continue;//try Yonly, already added (XY)
        trfit2.Add(0,pcL.cy, pcL.cz,-1,25e-4,300e-4);
        trfit4.addYZ(pcL.cy, pcL.cz);
      }
      int nplan=tmpCandi.GetNPlane();//Y
//      int nplanx=tmpCandi.GetNPlane(0);
//      if(nplan<=2||nplanx<=1)continue;//require >=3 inner planes,>=2X planes
      if(nplan<=2||tmpCandi.nLengthX<=1)continue;//require >=3 inner planes,>=2X hits
      if(nbadhits>=1){
        if(semib.nLength<=2)continue;
        bool bfound=0;
        unsigned int k=iter6+1;
        for(;k<semiIdx.size();k++){
          semiIndex &ksem=semiIdx[k];//next
          if(ksem.hfqql<semib.hfqql-0.01)break;//not found and lower quality, no need to continue;
          if(ksem.same(semib)){bfound=1;break;}//found the same
        }
        if(!bfound)semiIdx.insert(semiIdx.begin()+k,semib);//create a new semiIdx
        continue;
      }
//------
      double ly=trfit2.LinearFit(2);
      if(ly<=0)  continue;
      double quality[3]={0};
      if(nplan>=4){
        quality[0] += 190;
        tmpCandi.qbit|=(1<<1);
      }
      else if(nplan==3){//3plane
        quality[0] += 50;
        tmpCandi.qbit|=(1<<0);
      }
      if(imax <= 2 ) {
        quality[0] += tmpCandi.nLength * 23;
        quality[0] += imax * 1.3;
        quality[1] +=  0.019/(ly/1e2 + 0.25);
      }
      else{
        quality[0] += tmpCandi.nLength * 23;
        quality[0] += (tmpCandi.nLengthX)*1.3;
        quality[1] +=   0.25/(ly/1e2 + 0.25);
        quality[1] +=  0.019/(lx/1e2 + 0.25);
      }

      bool isInnerNuclei = (_isNuclei && _qNuclei>0 && tmpCandi.GetinnerQ()>1.5 && fabs(tmpCandi.GetinnerQ()-_qNuclei) < 0.3*_qNuclei);
      if( isInnerNuclei  ) {
          int nNucleix = 0, nNucleiy=0;
          for(int j=0;j<tmpCandi.nLength;j++){
            if(tmpCandi.qx[j]>0 && fabs(tmpCandi.qx[j]- _qNuclei ) <= 0.3*_qNuclei){nNucleix++; tmpCandi.qhitbit[0]|=(1<<tmpCandi.lay[j]);}
            if(tmpCandi.qy[j]>0 && fabs(tmpCandi.qy[j]- _qNuclei ) <= 0.3*_qNuclei){nNucleiy++; tmpCandi.qhitbit[1]|=(1<<tmpCandi.lay[j]);}
          }
          quality[0] += 0.35*nNucleix; //x
          quality[0] += 5.9*nNucleiy;//y
//          quality[0] += 3*nNucleiy;//y
          tmpCandi.qbit|=(1<<2);
       }
       int matchx=0;
       if(isInnerNuclei){//L1 or L9 Hits
         for(int iex=0;iex<2;iex++){
           double mindis = 9999;
           for(map<unsigned int,vector<int> >::iterator it=maxqL19[iex].begin();it!=maxqL19[iex].end();it++){
             ccL &pcL=ccList19[it->first];
             vector<int> &ixs=it->second;
             for(unsigned int ii=0;ii<ixs.size();ii++){
               int ix=ixs[ii];
               double px=trfit5.guessY(pcL.cz1[ix]);
               if(fabs(px-pcL.cx[ix])<mindis){
                 mindis=fabs(px-pcL.cx[ix]);
               }
             } 
           }
           if(mindis<0.2) {quality[2]+=0.73; matchx+=3; tmpCandi.qbit|=(1<<(3+iex)); }// /(  mindis + 0.08   )
        }
      }
      else {//for proton mutliplicity (only for both has L1 and L9 Hits)
        double mindis19[2] = {9999, 9999};
        for(unsigned int ic=0;ic<ccList19.size();ic++) {
          ccL &pcL=ccList19[ic];
          int iex=(pcL.lay==1)?0:(pcL.lay==9?1:-1);
          if(iex<0)continue;
          for(int ix=0;ix<pcL.ncx;ix++){
            if(pcL.qx[ix]<=0 || fabs(pcL.qx[ix]-tmpCandi.GetinnerQ())>0.3*tmpCandi.GetinnerQ())continue;//charge match
            double px=trfit5.guessY(pcL.cz1[ix]);
            if(fabs(px-pcL.cx[ix])<mindis19[iex]){
              mindis19[iex]=fabs(px-pcL.cx[ix]);
            }
          }
        }
        if(mindis19[0]<0.2 && mindis19[1]<0.2) {quality[2]+=0.73;  matchx+=3; tmpCandi.qbit|=(1<<3); }// /(  mindis19 + 0.08   );
      }
      for(int ii=0;ii<3;ii++)tmpCandi.quality[ii]=quality[ii];    

      //further 2D Fit
      TrFit kkfit;
      for(int j=0; j<tmpCandi.nLength; j++){
        int ix=tmpCandi.ix[j];
        double tx = (ix>=0)? tmpCandi.cx[j] : 0;
        double ty = tmpCandi.cy[j];
        double tz = tmpCandi.cz[j];
        kkfit.Add( tx,ty,tz, (ix>=0?25e-4:-1), 13e-4,300e-4 );
      }
      if(kkfit.SimpleFit(0)<0)continue;
      if(kkfit.GetNdofX()>0)tmpCandi.chis[0][0]=kkfit.GetChisqX()/kkfit.GetNdofX();
      if(kkfit.GetNdofY()>0)tmpCandi.chis[0][1]=kkfit.GetChisqY()/kkfit.GetNdofY();
      if(kkfit.GetNdof()>0) tmpCandi.chis[0][2]=kkfit.GetChisq();//Chis
      tmpCandi.rig[0]=kkfit.GetRigidity();
      int tofmatchx[2]={0};
      int tofmatch[2]={0};
      if(tofxs) {
        double toftx[2]={9999,9999};
        double tofsx[2]={9999,9999};
        int tofbit[2]={0};
        int tofbitx[2]={0};
        for(unsigned int itof=0;itof < tofcc.size();itof++) {
          if(tofrq[itof]<tmpCandi.GetinnerQ()/2.)continue;//amplitude match
          kkfit.Propagate(  tofcc[itof].z() );
          AMSPoint tp = kkfit.GetP0();
          AMSDir   td = kkfit.GetDir();
          int il = tofly[itof];
          double dd[2] = { fabs( tp.x()-tofcc[itof].x()), fabs( tp.y()-tofcc[itof].y()) };
          int txy=TOFGeom::Proj[il];//time measurement projection
          if(dd[1-txy]<tofbw[itof]/2.+0.5){
            tofbit[0]|=(1<<il);//1d match
            if(dd[txy]<4.){tofbit[1]|=(1<<il);}//2d match. 4cm is minimal to resolve multiplicity
            if(txy==1){//time Y measurement
              tofbitx[0]|=(1<<il);//shape X match
            }
            else{//time X measurement
              if(dd[txy]<4.)tofbitx[1]|=(1<<il);//shape Y && time X match
              if(dd[0]<toftx[il/2])toftx[il/2]=dd[0];
            }
          }
          if(txy==1&&dd[0]<tofsx[il/2]){tofsx[il/2]=dd[0];}//shape X measurement
        }
        if((tofbitx[0]&0x3)>0)tofmatchx[0]=1;//1d match upper
        if((tofbitx[0]&0xc)>0)tofmatchx[1]=1;;//1d match lower
        if((tofbit[0]&0x3)==0x3||(tofbit[1]&0x3)>0)tofmatch[0]=1;//2d match upper
        if((tofbit[0]&0xc)==0xc||(tofbit[1]&0xc)>0)tofmatch[1]=1;;//2d match lower
        for(int im=0;im<2;im++){
          if(tofbit[im]==0xf){///4 layers shape/time TOF
            tmpCandi.quality[2]+=(im==0)?0.8:0.4; 
            if(im==0){tmpCandi.ntofsl=40; tmpCandi.qbit|=(1<<5);}
            else     tmpCandi.qbit|=(1<<7);
          }
          else if(tofbit[im]==0x7||tofbit[im]==0xb||tofbit[im]==0xd||tofbit[im]==0xe){//3 layers shape/time TOF
            tmpCandi.quality[2]+=(im==0)?0.6:0.31; 
            if(im==0){tmpCandi.ntofsl=30; tmpCandi.qbit|=(1<<6);}
            else      tmpCandi.qbit|=(1<<7);
          }
          else if(tofbit[im]==0x5||tofbit[im]==0x6||tofbit[im]==0x9||tofbit[im]==0xa){//2 layers shape/time TOF-UD
            tmpCandi.quality[2]+=(im==0)?0.4:0.22;
            if(im==0)tmpCandi.ntofsl=20;
            else     tmpCandi.qbit|=(1<<7);
          }
          else {//<=2 Layer others pattern shape/time
            for(int il=0;il<4;il++){
              if(tofbit[im]&(1<<il)){
                tmpCandi.quality[2]+=(im==0)?0.1:0.06;//1d TOF
                if(im==0)tmpCandi.ntofsl++;
              }
            }
          }
        }
        for(int iud=0;iud<2;iud++){
          tmpCandi.toftx[iud]=toftx[iud];
          tmpCandi.tofsx[iud]=tofsx[iud];
        }
      }
      int trdmatch=0;
      if(trdxs) {
       double qualitytrdm=0;
       for(unsigned int itrd=0;itrd < trdc.size();itrd++) {
         kkfit.Propagate(  trdc[itrd].z() );
         AMSPoint tp = kkfit.GetP0();
         AMSDir   td = kkfit.GetDir();
         ptrd=trdc[itrd]; dtrd=trdd[itrd];
          double qualitytrd=0;
         if( fabs(tp.x()-ptrd.x())<4 && fabs(tp.y()-ptrd.y())<6 ) {
           qualitytrd += 0.08;
           trdmatch=1;
           if( fabs( td.gettheta () - dtrd.gettheta () ) <0.2 && fabs( td.getphi() - dtrd.getphi() ) <0.2   )
             qualitytrd += 0.08;
             trdmatch=2;
             tmpCandi.qbit|=(1<<8);
          }
          if(qualitytrd>qualitytrdm){qualitytrdm=qualitytrd;}
        }
        tmpCandi.quality[2]+=qualitytrdm;
      }
//-----
      if(tmpCandi.nLength<=3||tmpCandi.nLengthX<=2) {if(tofmatch[0]+tofmatch[1]<=1&&trdmatch<=1)continue;}//bad pre-selection
      if     (nchosen > NCandiX/4)break;//too much (250 allowned )
      else if(nchosen > NCandiX/5 && (tofmatch[0]+tofmatch[1]<=1&&trdmatch<=1))continue;//2d match
      else if(nchosen > NCandiX/7 && (tofmatchx[0]+tofmatchx[1]<=1))continue;//1d match
      tmpCandi.sort();//ordering the hits
      tmpCds.push_back(tmpCandi);
      nchosen++;
      //in cmppitable check;
    } // from many chosen
    if(nchosen==0&&imax>=3){imax--; goto imaxminus;}//try to remove one X

    //merge multiplicity and X
    candi *pc[2]={0};
    for(int i=0; i<int(tmpCds.size())-1;i++){
      pc[0]=&(tmpCds[i]);
      if(pc[0]->isBad)continue;
      for(unsigned int j=i+1; j<tmpCds.size();j++){
        pc[1]=&(tmpCds[j]);
        if(pc[1]->isBad)continue;
        if(!pc[1]->samehit(*pc[0],1))continue;//different Y, should not happen
        double quality0=pc[0]->quality[0]+pc[0]->quality[2];
        double quality1=pc[1]->quality[0]+pc[1]->quality[2];
        int k=(quality0>=quality1)?1:0;
        if(quality0==quality1){
          if(!pc[1]->samehit(*pc[0],0)){//different X
            continue;//not able to judge
          }
          else {//same X
            double dist[2]={0};
            int ntx[2]={0};
            for(int ic=0;ic<2;ic++){
              for(int iud=0;iud<2;iud++){
                if(pc[ic]->toftx[iud]<6){dist[ic]+=pc[ic]->toftx[iud];ntx[ic]++;}
                else {dist[ic]+=pc[ic]->tofsx[iud];}
              }
            }
            if(ntx[0]==ntx[1])k=(dist[0]<=dist[1])?1:0;
            else k=(ntx[0]>ntx[1])?1:0;
            if((pc[k]->qbit&(1<<5))&&(pc[k]->qbit&(1<<7))&&(ntx[k]>=ntx[1-k]-1)){//4 Layer TOF golden
              pc[1-k]->addimlto(*pc[k]);
/*              cout<<"toftx1="<<pc[0]->toftx[0]<<","<<pc[1]->toftx[0]<<" tofsx="<<pc[0]->tofsx[0]<<","<<pc[1]->tofsx[0]<<" nlengthx="<<pc[0]->nLengthX<<","<<pc[1]->nLengthX<<" k="<<k<<" ntx="<<ntx[0]<<","<<ntx[1]<<" dist="<<dist[0]<<","<<dist[1]<<endl;
              cout<<"toftx2="<<pc[0]->toftx[1]<<","<<pc[1]->toftx[1]<<" tofsx="<<pc[0]->tofsx[1]<<","<<pc[1]->tofsx[1]<<" quality="<<pc[0]->quality[2]<<","<<pc[1]->quality[2]<<endl;
              for(int m=0;m<pc[0]->nLength;m++){
                if(pc[0]->ix[m]>=0)cout<<"m="<<m<<" x="<<pc[0]->cx[m]<<","<<pc[1]->cx[m]<<" mult="<<pc[0]->imlt[m]<<","<<pc[1]->imlt[m]<<" z="<<pc[0]->cz[m]<<","<<pc[1]->cz[m]<<endl;
              }*/
            }
          }
        }
        pc[k]->isBad=2;
        if(k==0)break;
      }
    }
    nraw+=tmpCds.size();
    for(unsigned int i=0;i<tmpCds.size();i++){
      candi &pcd=tmpCds[i];
      if(pcd.isBad)continue;
       //incompatible check
      int nlength=pcd.nLength;
      for(int i=0;i<nlength;i++){
        int iccL=pcd.iccL[i];
        map<int,int>::iterator it=hitptt.find(iccL);
        if     (it==hitptt.end()){hitptt.insert(make_pair(iccL,nlength));}
        else if(nlength>it->second){it->second=nlength;}
      }
      trkCd.push_back(pcd);
    }
  }//semi track candidates.

//  cout<<"tkStat="<<_tkStat<<" tempCds="<<trkCd.size()<<" nraw="<<nraw<<" nhit="<<_rhits.size()<<endl;

   
//---compare XY
  if(trkCd.size() > 1){
    //only the decrease can get the highest NO of track set
    std::sort(trkCd.begin(),trkCd.end());


    //remove duplicate and subtrack candidates
    int nrm[2]={0};
    candi *pc[2]={0};
    for(int i=0; i<int(trkCd.size())-1;i++){
      pc[0]=&(trkCd[i]);
      if(pc[0]->isBad)continue;
      for(unsigned int j=i+1; j<trkCd.size();j++){
        pc[1]=&(trkCd[j]);
        if(pc[1]->isBad)continue;
        double quality0=pc[0]->quality[0]+pc[0]->quality[2];
        double quality1=pc[1]->quality[0]+pc[1]->quality[2];
        if(quality0==quality1){
          if(*(pc[1])==*(pc[0])){pc[1]->isBad=10; nrm[0]++;}//all the same including multiplicity
          else continue;//keep the same quality, should use chis to judge
        }
        else {
          //only deal with heavy overlap both in Y and X
          int nsamey=pc[1]->getnsamehit(*pc[0],1);
          if(pc[1]->nLength-nsamey>=2)continue;
          int nsamex=pc[1]->getnsamehit(*pc[0],0);
          if(pc[1]->nLengthX-nsamex>=2)continue;
          int flag=0;
          bool presel=(pc[1]->nLength>=4 && pc[1]->nLength>=pc[0]->nLength-1 && pc[1]->nLengthX>=pc[0]->nLengthX-1 && pc[1]->ntofsl>=pc[0]->ntofsl);//>=N-1
          if(presel){//exceptional >=N-1 would be kept
            if     (pc[1]->ntofsl>pc[0]->ntofsl)flag=1;
            else if(pc[1]->nLength>=pc[0]->nLength && pc[1]->nLengthX>=pc[0]->nLengthX)flag=2;
            else if(pc[1]->nLength-nsamey==1 && pc[1]->nLength>=pc[0]->nLength)flag=3;//one different in Y
          } 
          if(flag>0){
/*            cout<<"flag="<<flag<<" length="<<pc[1]->nLength<<","<<pc[0]->nLength<<" same="<<nsamey<<" lengthx="<<pc[1]->nLengthX<<","<<pc[0]->nLengthX<<" samex="<<nsamex<<" plane0="<<pc[1]->GetNPlane()<<","<<pc[0]->GetNPlane()<<" rig="<<pc[1]->rig[0]<<","<<pc[0]->rig[0]<<" chisy="<<pc[1]->chis[0][1]<<","<<pc[0]->chis[0][1]<<" tofs="<<pc[1]->ntofsl<<","<<pc[0]->ntofsl<<" quality2="<<pc[1]->quality[2]<<","<<pc[0]->quality[2]<<" quality0="<<pc[1]->quality[0]<<","<<pc[0]->quality[0]<<" innerqx="<<pc[1]->GetinnerQ(0)<<","<<pc[0]->GetinnerQ(0)<<" innerqy="<<pc[1]->GetinnerQ(1)<<","<<pc[0]->GetinnerQ(1)<<endl;*/
            continue;
          }
          //longest will will eat shorter candi
          nrm[1]++;
          pc[1]->isBad=1;//backup
        }
        if(pc[0]->isBad)break;
      }
    }
//    cout<<"rmsame="<<nrm[0]<<" rmY="<<nrm[1]<<endl;

//---time consuming use Choutko Fit
    int nrefit = 0;
    const int mchoutfit=1000;
//    const int mchoutfit=30;
    for(unsigned int i=0; i< trkCd.size() ;i++ )
    {
      if(trkCd[i].isBad) continue;
      TrFit kkfit;
      for(int j=0; j<trkCd[i].nLength; j++){
        int ix=trkCd[i].ix[j];
        double tx = (ix>=0)? trkCd[i].cx[j] : 0;
        double ty = trkCd[i].cy[j];
        double tz = trkCd[i].cz[j];
        kkfit.Add( tx,ty,tz, (ix>=0?25e-4:-1), 13e-4,300e-4 );
      }
//-----      
      int nplan=trkCd[i].GetNPlane();
      if(trkCd[i].nLength>=4 && ((trkCd[i].nLengthX>=3&&nplan>=3)||(trkCd[i].nLengthX>=2&&nplan>=4)) && nrefit++<=mchoutfit){
        if(kkfit.ChoutkoFit()>=0){//VC without MS
          if(kkfit.GetNdofX()>0)trkCd[i].chis[1][0]=kkfit.GetChisqX()/kkfit.GetNdofX();
          if(kkfit.GetNdofY()>0)trkCd[i].chis[1][1]=kkfit.GetChisqY()/kkfit.GetNdofY();
          if(kkfit.GetNdof()>0) trkCd[i].chis[1][2]=kkfit.GetChisq();
          if(trkCd[i].chis[1][0]<20) {trkCd[i].quality[1]+=0.2;trkCd[i].qbit|=(1<<9);}
          if(trkCd[i].chis[1][1]<20) {trkCd[i].quality[1]+=5;  trkCd[i].qbit|=(1<<10);}
          trkCd[i].rig[1]=kkfit.GetRigidity();
        }
      }
    }

    //---sort again
    std::sort(trkCd.begin(),trkCd.end());
//    cout<<"nrefit="<<nrefit<<endl;
  }


  //////////////////////////////////////////////
  /////   incompatible check of xzy candidate //
  if(trkCd.size() > 1){
    //   for now, we do not allow the track sharing hits. ////
    candi *pc[2]={0}; 
    for(int i=0; i<int(trkCd.size())-1;i++){
      pc[0]=&(trkCd[i]);
      if(pc[0]->isBad)continue;
      for(unsigned int j=i+1; j<trkCd.size();j++){
        if(pc[0]->isBad)break;
        pc[1]=&(trkCd[j]);
        if(pc[1]->isBad)continue;
        bool tmpflg = false;
        int noverlaph=0,boverlaph=0;
        int noverlapx=0,boverlapx=0;
        int noverlapy=0,boverlapy=0;
        for(int in=0; in<pc[0]->nLength; in++){
          for(int jn=0; jn<pc[1]->nLength; jn++){
            if(pc[0]->ihit[in]==-1 || pc[1]->ihit[jn]==-1 ) continue;
            bool hsame=(pc[0]->ihit[in]==pc[1]->ihit[jn]);
            bool xsame=(pc[0]->iclx[in]==pc[1]->iclx[jn]&&pc[0]->iclx[in]>=0);
            bool ysame=(pc[0]->icly[in]==pc[1]->icly[jn]);
            if(hsame) {noverlaph++; boverlaph|=(1<<pc[0]->lay[in]);}
            if(xsame) {noverlapx++; boverlapx|=(1<<pc[0]->lay[in]);}
            if(ysame) {noverlapy++; boverlapy|=(1<<pc[0]->lay[in]);}
            if(hsame || xsame || ysame ){
              tmpflg = true; break;
            }
          }
        }
        if(!tmpflg)continue;
        int k=(pc[0]->Getquality()>=pc[1]->Getquality())?1:0;//the killed candi
        //comparable weight, compare Chi2
        if((pc[k]->quality[0]+pc[k]->quality[2])>=(pc[1-k]->quality[0]+pc[1-k]->quality[2])){
          int ixy=(noverlapy!=pc[k]->nLength)?1:0;//different in Y or X
          if(noverlaph==pc[k]->nLength)ixy=2;//resolve multiplicity if all other are the same
          int ifit=(pc[0]->chis[1][ixy]!=pc[1]->chis[1][ixy])?1:0;
          if(pc[0]->chis[ifit][ixy]==pc[1]->chis[ifit][ixy]){
            if(ixy==0){
              if(pc[0]->GetNPlane(0)!=pc[1]->GetNPlane(0))k=(pc[0]->GetNPlane(0)>pc[1]->GetNPlane(0))?1:0;//not able to distinish chi2, max-Xspan
              else {//with best XY charge match
                k=(fabs(pc[0]->GetinnerQ(0)-pc[0]->GetinnerQ(1))<=fabs(pc[1]->GetinnerQ(0)-pc[1]->GetinnerQ(1)))?1:0;
              }
            }
            else  k=(fabs(pc[0]->rig[ifit])>=fabs(pc[1]->rig[ifit]))?1:0;//the same chi2,choose the largest rig
          }
          else {
            k=(pc[0]->chis[ifit][ixy]<pc[1]->chis[ifit][ixy])?1:0;//differ chis
            if(ixy==0&&pc[k]->nLengthX>=3&&pc[k]->nLength>=4&&pc[k]->GetNPlane(0)>pc[1-k]->GetNPlane(0)){
              if(pc[k]->chis[ifit][ixy]<pc[k]->chis[ifit][1-ixy]*10||pc[k]->chis[ifit][ixy]<pc[1-k]->chis[ifit][ixy]*10)k=1-k;//XY X^2 compatible, max-Xspan
            }
          }
        }
/*        //heavy overlap
        else if(pc[k]->nLength>=4 && pc[k]->nLength>=pc[1-k]->nLength-1 && pc[k]->nLength-noverlapy<=1 &&pc[1-k]->nLength-noverlapy>=1 && pc[k]->nLengthX>=pc[1-k]->nLengthX && pc[1-k]->nLengthX-noverlapx<=1 && pc[k]->GetNPlane()==pc[1-k]->GetNPlane() && pc[k]->quality[2]>=pc[1-k]->quality[2]){
          int flag=0;
          if(pc[k]->nLengthX>pc[1-k]->nLengthX)flag=1;
          else if(pc[k]->ntofsl>pc[1-k]->ntofsl&&pc[k]->ntofsl==40)flag=2;
          else if(pc[k]->nLength-noverlapy==1&&pc[k]->chis[1][1]<pc[1-k]->chis[1][1]/5.)flag=3;
          if(flag>=1)cout<<"flag="<<flag<<" length="<<pc[k]->nLength<<","<<pc[1-k]->nLength<<" same="<<noverlapy<<" lengthx="<<pc[k]->nLengthX<<","<<pc[1-k]->nLengthX<<" samex="<<noverlapx<<" plane0="<<pc[k]->GetNPlane()<<" rig="<<pc[k]->rig[0]<<","<<pc[1-k]->rig[0]<<" chisy="<<pc[k]->chis[0][1]<<","<<pc[1-k]->chis[0][1]<<" chisy2="<<pc[k]->chis[1][1]<<","<<pc[1-k]->chis[1][1]<<" tofs="<<pc[k]->ntofsl<<","<<pc[1-k]->ntofsl<<" quality2="<<pc[k]->quality[2]<<","<<pc[1-k]->quality[2]<<" quality0="<<pc[k]->quality[0]<<","<<pc[1-k]->quality[0]<<" quality1="<<pc[k]->quality[1]<<","<<pc[1-k]->quality[1]<<" innerqx="<<pc[k]->GetinnerQ(0)<<","<<pc[1-k]->GetinnerQ(0)<<" qnuclei="<<_qNuclei<<","<<pc[k]->GetinnerQ()<<","<<pc[1-k]->GetinnerQ()<<" innerqy="<<pc[k]->GetinnerQ(1)<<","<<pc[1-k]->GetinnerQ(1)<<" qhitbit="<<pc[k]->qhitbit[0]<<","<<pc[1-k]->qhitbit[0]<<" qhitbit2="<<pc[k]->qhitbit[1]<<","<<pc[1-k]->qhitbit[1]<<" rm="<<pc[k]->nrm[1]<<","<<pc[1-k]->nrm[1]<<" rmx="<<pc[k]->nrm[1]<<","<<pc[1-k]->nrm[1]<<endl;
        }*/
        //have one common Y cluster
        if(noverlapy==1 && pc[k]->GetNPlane()>=4 && pc[k]->nLengthX>=3 && pc[k]->quality[2]>=pc[1-k]->quality[2] && pc[k]->chis[1][1]<pc[1-k]->chis[1][1]){
          double qtrk[2]={0};
          for(int ixy=0;ixy<2;ixy++)qtrk[ixy]=pc[k]->GetinnerQ(ixy);
          bool qmatch[2]={1,1};
          qmatch[0]=fabs(qtrk[0]-qtrk[1])<0.3*qtrk[1];//X and Y track qmatch
          for(int lay=2;lay<=8;lay++){
            if((boverlapy&(1<<lay))==0)continue;
            int ip=pc[k]->ilay[lay];
            for(int ixy=0;ixy<2;ixy++){
              double ql=(ixy==0)?pc[k]->qx[ip]:pc[k]->qy[ip];
              if(ql<=0)continue;
              if(fabs(ql-qtrk[ixy])>0.3*qtrk[ixy])qmatch[ixy]=0;//cluster and track match
            }
          }
          if(qmatch[1]&&pc[k]->chis[1][1]<pc[1-k]->chis[1][1]/5.){//huge chis suppression, revise candidate selection
            k=1-k;
          }
        }
        //remove overlap: update or insert a new one for the remnant
        candi bcad=(*pc[k]);
        for(int lay=2;lay<=8;lay++){
          if     ((boverlaph&(1<<lay))||(boverlapy&(1<<lay)))bcad.removeHit(lay,2);//remove both X and Y
          else if( boverlapx&(1<<lay))bcad.removeHit(lay,0);//remove X
        }
        if(bcad.GetNPlane()>=3&&bcad.GetNPlane(0)>=2){
          bool bfound=0;
          bcad.iparent=(k==1)?j:i;
          unsigned int l=bcad.iparent+1;
          unsigned int lp=trkCd.size(); 
          for(;l<trkCd.size();l++){
            candi &pcad=trkCd[l];//next
            if(!(pcad.isBad==0||pcad.isBad==1))continue;
            if(bcad.samehit(pcad,2)){bfound=1;break;}
            else if(bcad.Getquality()>=pcad.Getquality()&&l<lp)lp=l;
          }
          if(bfound){//updat existing one
            candi &pcad=trkCd[l];
            if(pcad.isBad==1)pcad.isBad=0;
            if(pcad.iparent==-1||bcad.iparent<pcad.iparent){//only accept highest priority
              pcad.iparent=bcad.iparent;
              for(int im=0;im<pcad.nLength;im++){
                pcad.imlt[im]=bcad.imlt[im];
              }
              pcad.imlto=bcad.imlto;
            }
          }
          else {//insert new one
            trkCd.insert(trkCd.begin()+lp,bcad);//add new candi
            pc[0]=&(trkCd[i]);
            pc[1]=&(trkCd[j]);
          }
        }
//-------
        pc[k]->isBad=10;
      }
    }
  }

//---remove Bad/NonTrack candidates
  for(int i=int(trkCd.size())-1;i>=0;i--){
    if(trkCd[i].nLength<3) trkCd[i].isBad=10;
    if(trkCd[i].nLengthX<2)trkCd[i].isBad=10;
    if(trkCd[i].isBad)trkCd.erase(trkCd.begin()+i);
  }

  return trkCd.size() ;
}


#ifndef __ROOTSHAREDLIBRARY__
AMSTrTrack *TrReconQ::BuildTrack(candi &pcad, int select_tag)
#else
TrTrackR   *TrReconQ::BuildTrack(candi &pcad, int select_tag)
#endif
{
  //////////////////// Create a new TrTrack ////////////////////
#ifndef __ROOTSHAREDLIBRARY__
  AMSTrTrack *track = new AMSTrTrack(0);
  track->_quality=pcad.Getquality();
#else
  TrTrackR   *track = new TrTrackR(0);
#endif

  bool usev6=(TRCLFFKEY.recflag/10000000%10>=1);

//-------
  TrFit trfit;
  for (int j=0; j<pcad.nLength; j++) {
    int ix=pcad.ix[j];
    double tx = (ix>=0)? pcad.cx[j] : 0;
    double ty = pcad.cy[j];
    double tz = pcad.cz[j];
    trfit.Add( tx,ty,tz, (ix>=0?25e-4:-1), 13e-4,300e-4 );
  }
  double fstat=trfit.SimpleFit(0);//without MS
  double qtrk=pcad.GetinnerQ();
  
  //    cout<<"before adding to tracks "<<endl;
  for (int j=0; j<pcad.nLength; j++) {
    TrRecHitR* hit = _rhits[pcad.ihit[j]];
    if (!hit) continue;
    int imlt = pcad.imlt[j];
    if(hit->OnlyY())imlt=hit->GetResolvedMultiplicity();
    if(usev6&&fstat>=0){//v6 version
      TkLadderN *ladder=hit->GetLadder();
      AMSPlane pl=(*ladder);
      double rigidity;
      double length=trfit.Interpolate(pl,rigidity);//Interpolate to the ladder
      TVector3 dloc=pl.getDLocal();//get direction in local ladder
      for(int ixy=0;ixy<2;ixy++){
        TrClusterR *cls=(ixy==0)?hit->GetXCluster():hit->GetYCluster();
        if(!cls)continue;
        cls->SetQtrk(qtrk);//set rough charge and direction
        if(length<=-1000)continue;
        cls->SetDxDz((dloc[2]!=0)?dloc[0]/dloc[2]:0);//set rough direction
        cls->SetDyDz((dloc[2]!=0)?dloc[1]/dloc[2]:0);
      }
      if(hit->OnlyY()&&length>-1000){
        double dist,xaddress;
        AMSPlaneM plm0=hit->FindMult(dist,imlt,xaddress,pl.getP(),pl.getD(),-2);//position and direction in ladder
        length=trfit.Interpolate(plm0,rigidity);//Interpolate to sensor with more precise position and direction
        if(length>-1000){//should set some error
          AMSPlaneM plm=hit->FindMult(dist,imlt,xaddress,plm0.getP(),plm0.getD(),-2);//position and direction in sensor
          hit->SetDummyX(xaddress-640);
        }
      }
    }
    track->AddHit(hit, imlt);//Add hit and set multiplicity
  }
  //try to remove bad hits here.
  TryDropYHits(track,-1);

  int masky = 0x7f, maskc = 0x7f;
  int nhit = 0;
  for(int i=0;i<track->GetNhits();i++){
    TrRecHitR *hit=track->GetHit(i);
    unsigned int bit = 1 << (patt->GetSCANLAY()-hit->GetLayer());
    bit = ~bit;
    if (!hit->OnlyX()) masky &= ~(1 << (patt->GetSCANLAY()-hit->GetLayer()));
    if (!hit->OnlyY()) maskc &= ~(1 << (patt->GetSCANLAY()-hit->GetLayer()));
    nhit++;
  }

  track->SetPatterns(patt->GetHitPatternIndex(maskc),
      patt->GetHitPatternIndex(masky),
      patt->GetHitPatternIndex(maskc),
      patt->GetHitPatternIndex(masky));

  int result=0;
  if(nhit<3||(nhit==3&&TRCLFFKEY.AllowYonlyTracks<=0))result=-1;//3hits is not allowed in norminal condition, but only in TTCSoff period
  else result=ProcessTrackInner(track, select_tag);
  if(result <= 0){
    if(track){
      for(int i=0;i<track->GetNhits();i++){
        TrRecHitR *hit=track->GetHit(i);
        hit->ClearUsed();
      }
      delete track;
    }
    return 0;
  }

  int mfit2 = track->Gettrdefaultfit();
  ProcessTrackExt(track, mfit2, select_tag);
 
 return track;
}


int TrReconQ::BuildTrack(vector<candi> & trkCd, int select_tag, int nmaxtracks){

  if(trkCd.size()<=0   ) return 0;

  ///first tag all track candidates hits to avoid sharing the same Y between different tracks
  for(unsigned int i=0;i<trkCd.size();i++) {
    candi &pcad=trkCd[i];  
    for (int j=0; j<pcad.nLength; j++) {
      TrRecHitR* hit = _rhits[pcad.ihit[j]];
      if(hit)hit->SetTkCand();
    }
  }
  

//////////////////// Create new TrTracks ////////////////////
  int ntrack = 0;
  for(unsigned int i=0;i<trkCd.size();i++) {
    candi &pcad=trkCd[i];
    for (int j=0; j<pcad.nLength; j++) {
      TrRecHitR* hit = _rhits[pcad.ihit[j]];
      if(hit)hit->ClearTkCand();
    }
    //---already too many tracks
    if(nmaxtracks>=0 && GetNTrTrack()>=nmaxtracks)continue; 
#ifndef __ROOTSHAREDLIBRARY__
    AMSTrTrack *track=0, *trackn=0;
#else
    TrTrackR   *track=0, *trackn=0;
#endif
    track=BuildTrack(pcad,select_tag);
    if(track==0)continue;
    //try with different multiplicity to attach L1/L9
    if(pcad.imlto.size()>=1 && !track->TestHitLayerJHasXY(1) && !track->TestHitLayerJHasXY(9)){//has no external layer
//      cout<<"imltosize="<<pcad.imlto.size()<<endl;
      if(track->TestHitLayerJ(1))track->GetHitLJ(1)->ClearUsed();
      if(track->TestHitLayerJ(9))track->GetHitLJ(9)->ClearUsed();
      for(unsigned int im=0;im<pcad.imlto.size();im++){
        map<int,int> imlts=pcad.imlto[im];
        for(map<int,int>::iterator it=imlts.begin();it!=imlts.end();it++){
          int ip=pcad.ilay[it->first];
          if(ip<0)continue;
          if(pcad.ix[ip]>=0)pcad.imlt[ip]=it->second;
        }
        trackn=BuildTrack(pcad,select_tag);
        if(!trackn)continue;
        if(trackn->TestHitLayerJHasXY(1)||trackn->TestHitLayerJHasXY(9))break;//find L1 XY or L9 XY
        else {delete trackn; trackn=0;} 
      }
      if(trackn){delete track; track=trackn;/*cout<<"recover track"<<endl;*/}
      else       track->ReFitHitStat();//put back used,dummyx,multiplicity,dxdz,dydz,charge,coordinate
    }
    //go to final
    int genid=GetNTrTrack(); 
    if(genid>99)genid=99;//Track generate id 1-98 would be correct
    track->SetRecType(10*(_tkStat+1)+100*genid);//10000 reserved for primary
    _atracks.push_back(track);
    ntrack++;
  }

  return ntrack;
}


int TrReconQ::BuildAll(){

  int select_tag     = (_tkStat <= 0)? TrRecHitR::ZSEED: 0;
  int nmaxtracks     = (_tkStat <= 0)? -1: TRCLFFKEY.MaxZQ2ndTrack;
  //already too many tracks after zseed reconstruction
  if(nmaxtracks>=0 && GetNTrTrack()>=nmaxtracks)return 0;
 
  //----ccList
  vector<ccL> ccList; 
  vector<ccL> ccList19;
  int nmaxcclistscale = (_tkStat == 2)? TRCLFFKEY.MaxZQccListF: 1;
  BuildccList(ccList,ccList19,select_tag,nmaxcclistscale);

  //----angle 
  vector<semiIndex> semiIdx;
  float kiklimtscale=1;
//  if     (_tkStat==-1)kiklimtscale=1./6;
  if     (_tkStat==0)kiklimtscale=1./6;
  else if(_tkStat==3)kiklimtscale=fabs(TRCLFFKEY.MaxZQAngleF);
  BuildsemiIdx(semiIdx,ccList,select_tag,kiklimtscale);

  //-----candi
  vector<candi> trkCd; 
  float xwscale=TRCLFFKEY.MaxZQXWindow[0];
  if(_tkStat==3)xwscale=TRCLFFKEY.MaxZQXWindow[1];
  BuildtrkCd(trkCd,semiIdx,ccList,ccList19,xwscale); 
//  if(_tkStat==-1&&trkCd.size()>=1&&trkCd[0].GetNPlane()<=3)trkCd.clear();//_tkStat==-1 require 4 planes

  //-----track
  int ntrack=BuildTrack(trkCd,select_tag,nmaxtracks);
  return ntrack;
}


int TrReconQ::TryDumpHits(TrTrackR *track, int idx[3], int ixy){

  for(int i=0;i<3;i++)idx[i]=-1;
  if(ixy==0){
    if(track->GetNhits()<=3 || track->GetNhitsX()<=3) return 0;
  }
  else {
    if(track->GetNhits()<=4) return 0;
  }

  bool usev6=(TRCLFFKEY.recflag/10000000%10>=1);

  vector<int> lays;
  int hitbit[2]={0};
  vector<AMSPlaneM> plms;
  for(int i=0;i<track->GetNhits();i++){
    TrRecHitR *hit=track->GetHit(i);
    if(usev6){
      plms.push_back(hit->GetGCoordN(hit->GetResolvedMultiplicity()>=0?-1:0)); 
    }
    else {
      AMSPoint aa = hit->GetCoord();
      AMSPlaneM plm(TVector2(aa[0],aa[1]),hit->GetTkId());
      plm.setO(0,0,aa[2]);
      plms.push_back(plm);
    }
    lays.push_back(hit->GetLayer());
    AMSPlaneM *plm=&(plms.back());
    double ferr[2]={25e-4,13e-4};
    if(hit->OnlyY())ferr[0]=0;
    if(hit->OnlyX())ferr[1]=0;
    for(int ixy=0;ixy<2;ixy++){
      if(ferr[ixy]>0)hitbit[ixy]|=(1<<lays.back());
      plm->setMCov(ixy,ixy,ferr[ixy]*ferr[ixy]);
    }
  }

  int nm=plms.size();
  TrFit trfittb;
  for(int i=0;i<nm;i++)trfittb.Add(plms[i]);
  double fdone=trfittb.DoFit();//with MS
  double tcsq[2]={0};
  if(trfittb.GetNdofX()>0)tcsq[0]=trfittb.GetChisqX()/trfittb.GetNdofX();
  if(trfittb.GetNdofY()>0)tcsq[1]=trfittb.GetChisqY()/trfittb.GetNdofY();
  map<int,double> csqs;//lay,chis
  if(fdone<0||tcsq[1]<0||tcsq[ixy]<0)csqs[-1]=1e7;//failed
  else if(tcsq[1]==0||tcsq[ixy]==0)csqs[-1]=0;//No-dof
  else {
    csqs[-1]=(ixy==0)?tcsq[0]/tcsq[1]:tcsq[1];//ChisX/ChisY for X, ChisY for Y
  }
  if(ixy==0){if(csqs[-1]<TRFITFFKEY.RthdDropX[0]||tcsq[0]<10)return 0;}//only ChisX/ChisY>=100
  else      {if(csqs[-1]<TRCLFFKEY.ZQDropYChisq)return 0;}//only chisq>1000 would go below
  //compare Chi2 
  for(int im=0;im<nm;im++){//with MS
//  for(int im=-1;im<nm;im++){without MS 
    if(im>=0){if((hitbit[ixy]&(1<<lays[im]))==0)continue;}//no measurement in this side
    TrFit trfittk;
    for(int i=0;i<nm;i++){
      if(ixy==0){
        AMSPlaneM pln=plms[i];
        if(i==im)pln.setMCov(0,0,0);//mask not use this layer X
        trfittk.Add(pln);
      }
      else {
        if(i==im)continue;//not use ths layer in both X and Y
        trfittk.Add(plms[i]);
      }
    }
    double fdone0=trfittk.DoFit();//with MS
//    double fdone0=trfittk.DoFit(TrFit::CHOUTKO,0);//without MS
    double ncsq[2]={0};
    if(trfittk.GetNdofX()>0)ncsq[0]=trfittk.GetChisqX()/trfittk.GetNdofX();
    if(trfittk.GetNdofY()>0)ncsq[1]=trfittk.GetChisqY()/trfittk.GetNdofY();
    int lay=(im>=0)?lays[im]:-1;
    if(fdone0<0||ncsq[1]<=0||ncsq[ixy]<=0)csqs[lay]=2e7;//failed
    else csqs[lay]=(ixy==0)?ncsq[0]/ncsq[1]:ncsq[1]; 
  }
  int rlay[2]={-1,-1};//double layer min, all lay min
  for(map<int,double>::iterator it=csqs.begin();it!=csqs.end();it++){
    if(it->first<0)continue;
    int ipl=it->first/2;//layer id->plane id
    for(int ib=0;ib<2;ib++){
      if(ib==0){if(((hitbit[ixy]>>(ipl*2))&0x3)!=0x3)continue;}//double layers measured plane
      if(it->second<csqs[rlay[ib]]){
        rlay[ib]=it->first;//layer with minimal chis
      }
    }
  }
  int ndump=0;
  if(ixy==0){
    for(int ib=0;ib<2;ib++){//double layers first, otherwise all layers
      if(rlay[ib]>=0&&csqs[rlay[ib]]<csqs[-1]/TRFITFFKEY.RthdDropX[1]){//huge chis suppression,~f*N=10*4
        idx[0]=rlay[ib];
        ndump++;
        break;
      }
    }
  }
  else {//for Y, only double layers measured plane can be erased
    if(rlay[0]>=0&&csqs[rlay[0]]<csqs[-1]/TRCLFFKEY.ZQDropYRth){//huge chis suppression,~f*N=4*4
      idx[0]=rlay[0];
      ndump++;
    }
  }
  return ndump;
}


int TrReconQ::TryDropYHits(TrTrackR *track, int mfit){

  int ndump=0;
  for(int idp=0;idp<2;idp++){//upto 2 hits can be removed
    int rlay[3] = {-1,-1,-1};
    TryDumpHits(track,rlay,1);
    if(rlay[0]>0){track->RemoveHitOnLayer(rlay[0]);ndump++;}//remove hits.
    else break;
  }
  if(ndump>0&&mfit>0){
    if(track->FitT(mfit)>=0){
      track->EstimateDummyX(mfit);//renew dummyx and multiplicity
      track->RecalcHitCoordinates(mfit);//renew dxdz,dydx,charge,coordinate
    }
  }
  return ndump;  
}


int TrReconQ::TryDropXHits(TrTrackR *track, int mfit){

  if(track->GetNdofX(mfit)<=1||track->GetNdofY(mfit)==0)return 0;
  double tcsq[2]={0};
  tcsq[0]=track->GetChisqX(mfit)/track->GetNdofX(mfit);
  tcsq[1]=track->GetChisqY(mfit)/track->GetNdofY(mfit);
  if(tcsq[0]/tcsq[1]<TRFITFFKEY.RthdDropX[0])return 0;//fast return

  int idup[3] = {-1,-1,-1};
  int ndump=TryDumpHits(track,idup,0);//xside
  if(idup[0]>0){
    TrRecHitR *hit0=track->GetHitLO(idup[0]);
    VCon *cont = GetVCon()->GetCont("AMSTrRecHit");
    for(int ii=0; ii< cont->getnelem(); ii++){
      TrRecHitR *hit = (TrRecHitR*) cont->getelem(ii);
      if(!hit||!hit->OnlyY()||hit==hit0) continue;
      if(hit->GetYClusterIndex()==hit0->GetYClusterIndex()){//replaced by YONLY Hit
        hit0->setstatus(TrRecHitR::YONLY);//not use current Hit X to get fit
        track->FitT(mfit);//update fitting to resolve dummyx and multiplicity as new hit have no any X information
        hit0->clearstatus(TrRecHitR::YONLY);
        hit0->ClearUsed();
        track->AddHit(hit);//This would replace current Hit
        hit->SetUsed();
        break;
      }
    }
    delete cont;
    track->EstimateDummyX(mfit);//renew dummyx and multiplicity, must be performed for each hit remove
    track->RecalcHitCoordinates(mfit);//renew dxdz,dydx,charge,coordinate 
  }
  return ndump;
}


int TrReconQ::ProcessTrackInner(TrTrackR *track, int select_tag)
{

  //Clear all Fit par
  track->ClearPar();

  // 1st. step Fit
  bool usev6=(TRCLFFKEY.recflag/10000000%10>=1);
  int add_flags=usev6?TrTrackR::kAlignV6:0;
  int mfit1 = (MagFieldOn()) ? TrTrackR::kChoutko|add_flags: TrTrackR::kLinear|add_flags;//no MS

  float ret = track->FitT(mfit1);
  int ndofy = (ret > 0) ? track->GetNdofY(mfit1) : 0;
  if (TRCLFFKEY.AllowYonlyTracks && ndofy == 0) ndofy = 1;
  if (ret < 0 ||
      track->GetChisqX(mfit1) < 0 || track->GetChisqY(mfit1) <= 0 ||
      track->GetNdofX (mfit1) < 0 || ndofy <= 0) {
      return 0;
  }

  // Mark the hit as USED
  for (int i = 0; i < track->GetNhits(); i++) {
    TrRecHitR  *hit = track->GetHit(i);
    hit->SetUsed();
  }

  // recalc hit coordinates using the 1st fit information
  track->Settrdefaultfit(mfit1);
  if(track->GetChisqX(mfit1)>1e6)TryDropXHits(track,mfit1);//drop X and renew dummyx,multiplicity,dxdz,dydx,charge,coordinate
  else {
    track->EstimateDummyX(mfit1);//renew dummyx and multiplicity without MS
    track->RecalcHitCoordinates(mfit1);//renew dxdz,dydx,charge,coordinate without MS
  }

  // 2nd. step Fit
  int mfit2 = (MagFieldOn())? TrTrackR::DefaultFitID|add_flags : TrTrackR::kLinear|add_flags;
  int mfit3=mfit2;
  track->FitT(mfit2);
  if (track->FitDone(mfit2))track->Settrdefaultfit(mfit2);
  else if(MagFieldOn()){
    if((mfit2&0xf)!=TrTrackR::kChoutko)
      mfit3=TrTrackR::kChoutko|TrTrackR::kMultScat|add_flags;
    else if((mfit2&0xf)!=TrTrackR::kAlcaraz)
      mfit3=TrTrackR::kAlcaraz|TrTrackR::kMultScat|add_flags;
    track->FitT(mfit3);
    if(track->FitDone(mfit3))track->Settrdefaultfit(mfit3);
  }
  int mfit4=track->Gettrdefaultfit();
  if(TryDropXHits(track,mfit4)==0){///drop X and renew dummyx,multiplicity,dxdz,dydx,charge,coordinate (V6 consider to move this ahead without MS)
    track->EstimateDummyX(mfit4);//renew dummyx and multiplicity with MS
    track->RecalcHitCoordinates(mfit4);//renew dxdz,dydx,charge,coordinate with MS
  }
  // refind missing hits
  if(TRCLFFKEY.MergeLim[0]>0||TRCLFFKEY.MergeLim[1]>0){
    int iadd[10]={0};
    int nadd=TryMergeHits(track,mfit4,select_tag,iadd,-2);//-2: inner layers
    if(nadd>0){
      TryDropYHits(track,-1);
      if(track->FitT(mfit4)<0)return 0;//fatal error
      track->EstimateDummyX(mfit4);
      track->RecalcHitCoordinates(mfit4);
      TryDropXHits(track,mfit4);
    }
  }
  track->UpdateBitPattern();//Update bit patterns


  // 3rd. step Fit
  if( TRCLFFKEY.UseZQRebuild %10 < 2 ){
    track->DoAdvancedFit(add_flags);
    if(track->FitDone(mfit2))track->Settrdefaultfit(mfit2);
    int mfit5=track->Gettrdefaultfit();
    track->EstimateDummyX(mfit5);//with MS
    track->RecalcHitCoordinates(mfit5);
  }

  if (track->GetRigidity() == 0 || track->GetChisq() <= 0) {
//    delete track;
    return 0;
  }
  // Add the track to the collection

  return 1;
}


int TrReconQ::GetNTrTrack()
{
  return  _atracks.size();
}

TrTrackR* TrReconQ::pTrTrack(int itk)
{
  return (itk>=0&&itk<=int(_atracks.size())-1)? dynamic_cast<TrTrackR* >(_atracks[itk]): 0;
}

int TrReconQ::TryMergeHits(TrTrackR *track, int mfit, int select_tag, int iadd[10], int mlay)
{

  const int nlm=10;//maxL+1
  for(int i=0;i<nlm;i++)iadd[i]=0;

  float rig= std::fabs(track->GetRigidity(mfit));
  if (rig == 0) return -2;

  class ctest {
    public:
      int    ihmin;
      int    icmin;
      int    ncmin;
      int    mlmin;
      AMSPoint diff;
      double rxmin;
      double rymin;
      double xaddr;
      double dadz[2];
      ctest(){
        ihmin = -1;
        icmin =  0;
        ncmin =  0;
        mlmin =  0;
        rxmin = 10;
        rymin = 10;
        xaddr = -1;
        dadz[0]=dadz[1]=0;
      }
  };
  ctest DXY[nlm];
  ctest DY[nlm];

  //define which layer to search
  int hitbit[2]={0};
  TrRecHitR* tkhits[nlm]={0};
  for(int i=0;i<track->GetNhits();i++){
    TrRecHitR *hit=track->GetHit(i);
    tkhits[hit->GetLayer()]=hit;
    for(int ixy=0;ixy<2;ixy++){
      if((ixy==0&&!hit->OnlyY())||(ixy==1&&!hit->OnlyX()))hitbit[ixy]|=(1<<hit->GetLayer());
    }
  }
  int llr[2]={0};
  if     (mlay==-2){llr[0]=1; llr[1]=7;}//merge all inner layers
  else if(mlay==-1){llr[0]=8; llr[1]=9;}//merge all external layers
  else             {llr[0]=llr[1]=mlay;}//one layer
  map<int,int> mlays;//lay,pattern
  for(int il=llr[0];il<=llr[1];il++){
    if(il>=1&&il<=7)DXY[il].rymin=DY[il].rymin=2;//2cm, hard cut
    if     ((hitbit[1]&(1<<il))==0)mlays[il]=1;//mising Y, find Y or XY
    else if((hitbit[0]&(1<<il))==0)mlays[il]=0;//has Y but mising X, find XY
  }
  if(mlays.size()==0)return 0;//no need to refind, fast return

  
  //interpolation to missing layers
  bool usev6=(TRCLFFKEY.recflag/10000000%10>=1);
  AMSPoint ptrk[nlm];
  double distv6[nlm];
  TVector3 ptrkv6[nlm],dtrkv6[nlm];//find position and direction in the closest sensor
  for(int i=0;i<nlm;i++){distv6[i]=2.E100; dtrkv6[i].SetXYZ(0,0,1);}
  for(map<int,int>::iterator it=mlays.begin();it!=mlays.end();it++){
    int il=it->first;
    if(usev6){
      double length,rigidity;
      AMSPlaneM plm=track->InterpolateLayerJN(length,rigidity,TkPlaneN::convertlayid(il-1)+1,mfit);
      TkSensorN *sensor=(TkSensorN *)plm.getmodule();
      if(length<=-1000||!sensor)continue;
      distv6[il]=sensor->GetDistance(plm.getP(),plm.getD(),-2,-2,1);//distance to the closest sensor edge
      ptrkv6[il]=plm.getP();
      dtrkv6[il]=plm.getD();
    }
    else {
      ptrk[il] = track->InterpolateLayerO(il, mfit);
    }
  }

  // 1. Search the XY and Y-only hits closest to the track extrapolation on Y
  double qtrk=track->GetInnerQ();
  int mult;
  int nhit = _rhits.size();
  for (int i = 0; i < nhit; i++) {
    TrRecHitR *hit = (TrRecHitR*) _rhits[i]  ;
    if (!hit) continue;
    if (hit->OnlyX()) continue;
    if(hit->Used()||hit->GetYCluster()->Used()) continue;
    if( (select_tag!=0) && !hit->checkstatus(select_tag)  ) continue;
    //check if missing hit
    int il=hit->GetLayer();
    if(mlays.find(il)==mlays.end())continue;
    if(mlays[il]==0){//has YCluster but missing X
      if(hit->OnlyY()||hit->GetYClusterIndex()!=tkhits[il]->GetYClusterIndex())continue;//must use the same Y
    }
    else {//missing Y
      if(hit->GetYCluster()->TkCandi())continue;//possible to share the same XCluster, forbidden in Y
    }
   
    AMSPoint DD;
    double dist,xaddress=-1,dadz[2]={0};
    if(usev6){//v6 version
      if(distv6[il]>10)continue;//track 10cm out of L1/L9 volume
      AMSPlaneM plm=hit->FindMult(dist,mult,xaddress,ptrkv6[il],dtrkv6[il],-1,qtrk);
      TVector3 dloc=plm.getDLocal();
      for(int ixy=0;ixy<2;ixy++)dadz[ixy]=(dloc[2]!=0)?dloc[ixy]/dloc[2]:0;
      TVector3 res=plm.getMPdist(1);//residual in X,Y,Z
      DD.setp(res[0],res[1],res[2]);
    }
    else {
      DD = hit->HitPointDist(ptrk[il],mult);
      double dxy=sqrt(DD[0]*DD[0]+DD[1]*DD[1]);
      double dy=fabs(DD[1]);
      dist=(hit->OnlyY())?dy:dxy;
    }

    ctest *pct=(hit->OnlyY())?&(DY[il]):&(DXY[il]);
    if(dist<pct->rymin){
      pct->ihmin = i;
      pct->icmin = hit->GetYClusterIndex();
      pct->rymin = dist;
      pct->ncmin = 1;
      pct->mlmin = mult;
      pct->diff  = DD;
      pct->xaddr = xaddress;
      for(int ixy=0;ixy<2;ixy++)pct->dadz[ixy]=dadz[ixy];
    }

  }

  // here calculate the limx and limy: 2. XY or Y
  double diff_max[nlm][2]={0};
  for(map<int,int>::iterator it=mlays.begin();it!=mlays.end();it++){
    int il=it->first;//missing layer
    for(int ixy=0;ixy<2;ixy++){
      if(il>=1&&il<=7){//inner layers
        int ipl=il/2;//layer id->plane id
        int iud[2]={-1,-1};
        for(int i=il-1;i>=1;i--){if(hitbit[ixy]&(1<<i)){iud[0]=i;break;} }//upper hit layer
        for(int i=il+1;i<=7;i++){if(hitbit[ixy]&(1<<i)){iud[1]=i;break;} }//lower hit layer
        double sl=0;//length
        if(iud[0]<0||iud[1]<0){//extrapolation
          int tl=(iud[0]>=0)?iud[0]:iud[1];
          if(tl<0)continue;//not possbile
          int tpl=tl/2;//layer id->plane id
          int dpl=abs(ipl-tpl);
          if(dpl==0)sl+=5.*5.;//same plane: l=5cm, l better changed to be angle dependent
          else {//diff plane
            for(int j=1;j<=dpl;j++){sl+=(26.*26.*j*j);}//l=26cm*j
          }
        }
        else {//interpolation
          int dpl[2]={0};//plane number distance
          for(int k=0;k<2;k++)dpl[k]=abs(ipl-iud[k]/2);
          if(dpl[0]==0||dpl[1]==0)sl+=5.*5.;//same plane: l=5cm
          else sl+=pow(26.*(dpl[0]+dpl[1]-1)/2.,2);//diff plane: l=26cm*(dpl[0]+dpl[1]-1)/2
        }
        double dms=0.0136/rig*sqrt(1.5e-2/fabs(dtrkv6[il][2]))*sqrt(sl);//multiple-scattering: X=1.5e-2X0
        double sp0=40.e-4;//spatial-resolution: 40um
        double rcor=sqrt(sp0*sp0+dms*dms);
        diff_max[il][ixy] = 5.*rcor*TRCLFFKEY.MergeLim[ixy]; 
      }
      else {//external layer
        double lim = (ixy==0)? TRFITFFKEY.MergeExtLimX: TRFITFFKEY.MergeExtLimY;
        double sp0  = 0.02*TRCLFFKEY.MergeExtLimH[ixy];
        double sp1  = 1;
        double rcor = 4.*std::sqrt(sp0*sp0+sp1*sp1/rig/rig);
        diff_max[il][ixy] = lim*rcor;//for lim=0.5: 2e4um at 1GV, 2000um at 10GV, 400um at 100GV, MS(7000um, 700um, 70um)
        if (diff_max[il][ixy]> lim*10) diff_max[il][ixy] = lim*10;
      }
    }
  }

  int nadd=0;
  for(map<int,int>::iterator it=mlays.begin();it!=mlays.end();it++){
    int il=it->first; 
    TrRecHitR *hit = 0;
    if(usev6){//v6 version
      ctest *pct=0;
      if(DXY[il].ihmin>=0&&fabs(DXY[il].diff.y())<diff_max[il][1]&&fabs(DXY[il].diff.x())<diff_max[il][0]){
        pct=&(DXY[il]);
      }
      else if(DY[il].ihmin>=0&&DY[il].rymin<diff_max[il][1]){
        pct=&(DY[il]);
      }
      if(!pct)continue;
      mult=pct->mlmin;      
      hit=(TrRecHitR*)_rhits[pct->ihmin];
      if(!hit)continue;
      for(int ixy=0;ixy<2;ixy++){
        TrClusterR *cls=(ixy==0)?hit->GetXCluster():hit->GetYCluster();
        if(!cls)continue;
        cls->SetQtrk(qtrk);//set rough charge and direction
        cls->SetDxDz(pct->dadz[0]);//set rough direction
        cls->SetDyDz(pct->dadz[1]);
      }
      if(hit->OnlyY())hit->SetDummyX(pct->xaddr-640);
    }
    else {//this part has bug, low efficiency for Y Cluster
      if (DY[il].ihmin < 0) continue;//Has Y only Hit

      if (DXY[il].ihmin>=0 && fabs(DXY[il].diff.y()) > diff_max[il][1]) continue;//XY Hit within 10cm, Y out of range
      if (DXY[il].ihmin< 0 && fabs(DY [il].diff.y()) > diff_max[il][1]) continue;//No XY Hit, Y out of range

      if (DXY[il].ihmin>=0 &&
          fabs(DXY[il].diff.x()) < diff_max[il][0])
        hit=(TrRecHitR*)_rhits[DXY[il].ihmin];
      else
        hit=(TrRecHitR*)_rhits[DY[il].ihmin];
     
      if (!hit)continue;

      if (hit->OnlyY()) {
        TkSens tks = EstimateXCoord(ptrk[il]);
        if (!tks.LadFound() || tks.GetLadTkID() != hit->GetTkId()) continue;
        DY[il].mlmin = tks.GetMultIndex();
        int nmlt = hit->GetMultiplicity();
        if (DY[il].mlmin >= nmlt) DY[il].mlmin = nmlt-1;
        if (DY[il].mlmin <     0) DY[il].mlmin = 0;
        hit->SetDummyX(tks.GetStripX());
        mult=DY[il].mlmin;
      }
      else mult=DXY[il].mlmin;
    }
    if(tkhits[il])tkhits[il]->ClearUsed();
    track->AddHit(hit,mult);
    hit->SetUsed();
    iadd[il]=hit->OnlyY()?1:2;//1d or 2d hit
    nadd++;
  }
  return nadd;
}


int TrReconQ::ProcessTrackExt(TrTrackR *track, int mfit, int select_tag){
 
 
  int iadd[10]={0}; 
  int nadd=TryMergeHits(track,mfit,select_tag,iadd,-1);//-1 external layers
  if(nadd<=0)return nadd;
  
  //Fill external layer residuals for inner patterns
  track->FillExRes();

  //new added pattern
  vector<int> mext;
  if(iadd[8]>=1&&iadd[9]>=1){
    mext.push_back(TrTrackR::kFitLayer8|TrTrackR::kFitLayer9);//L1+L9 higest priority
    if(iadd[8]>=iadd[9]){mext.push_back(TrTrackR::kFitLayer8); mext.push_back(TrTrackR::kFitLayer9);}//L1 higher priority
    else                {mext.push_back(TrTrackR::kFitLayer9); mext.push_back(TrTrackR::kFitLayer8);}//L9 higher priority
  }
  else if(iadd[8]>=1)mext.push_back(TrTrackR::kFitLayer8);
  else if(iadd[9]>=1)mext.push_back(TrTrackR::kFitLayer9);

  //extend current default pattern
  int fitid=track->Gettrdefaultfit();
  int flag=0;
  for(unsigned int iex=0;iex<mext.size();iex++){//highest priority first
    int mfit1=fitid|mext[iex]; 
    if(track->FitT(mfit1)>0){
      if(flag++==0){//1nd. step Fit
        track->Settrdefaultfit(mfit1);
        track->EstimateDummyX(mfit1);
        track->RecalcHitCoordinates(mfit1);
      }
    }
  }

  //lowest priority first
  std::reverse(mext.begin(),mext.end());

  bool usev6=(TRCLFFKEY.recflag/10000000%10>=1);
  int add_flags=usev6?TrTrackR::kAlignV6:0;

  int mfit2 = (MagFieldOn())? TrTrackR::DefaultFitID|add_flags : TrTrackR::kLinear|add_flags;
  int mfit3=mfit2;
  if(MagFieldOn()){
    if((mfit2&0xf)!=TrTrackR::kChoutko)
      mfit3=TrTrackR::kChoutko|TrTrackR::kMultScat|add_flags;
    else if((mfit2&0xf)!=TrTrackR::kAlcaraz)
      mfit3=TrTrackR::kAlcaraz|TrTrackR::kMultScat|add_flags;
  }

  for(unsigned int iex=0;iex<mext.size();iex++){//lowest priority first
    track->DoAdvancedFit(add_flags|mext[iex]);//2nd. step Fit
    if     (track->FitDone(mfit2|mext[iex]))track->Settrdefaultfit(mfit2|mext[iex]);
    else if(track->FitDone(mfit3|mext[iex]))track->Settrdefaultfit(mfit3|mext[iex]);
  }
  int mfit4=track->Gettrdefaultfit();
  track->EstimateDummyX(mfit4);
  track->RecalcHitCoordinates(mfit4);

  return nadd;
}

double TrReconQ::evaNuclei(int reorderhits)
{
 //bug fixed by QY: sort with YQ+XYMatch first. This must have to be done. Because  X*muplicity is limited to be <70. This means only ~5X could match YCluster
  vector<pair<double,TrRecHitR*> > rhitsorder;
  double maxqx[9] = {0,0,0,0,0,0,0,0,0 };//X should matched with Y
  double maxqy[9] = {0,0,0,0,0,0,0,0,0 };
  for(unsigned int ihit=0;ihit< _rhits.size(); ihit++  )  {
    TrRecHitR *rhit = _rhits[ihit];
    if( rhit->OnlyX() ) continue;
    int il = rhit->GetLayerJ();
    if( il<1 || il>9 ) continue;
    TrClusterR *ycls = rhit->GetYCluster();
    if(!ycls) continue;
    double tqy = ycls->GetQ();

    TrClusterR *xcls = rhit->GetXCluster();
    double tqx= (xcls) ? xcls->GetQ() : 0;
    //----exclude noisey cluster and find highest cluster
    bool xygood[2]={1,1};
    for(int ixy=0;ixy<2;ixy++){
      TrClusterR *cls=(ixy==0)? xcls: ycls;
      if(!cls)continue;
      int dstrip=-1,mult=-2,seed_index=-1;//judge 2 strips
      xygood[ixy]=(cls->GetNStripWithCalibrationStatus(dstrip,0x8,mult,seed_index)==0);
    }
    if( (xygood[1] && tqy > 1.2 * maxqy[il-1]) || ( xygood[0] && xygood[1] && tqx > 1.05*maxqx[il-1] && tqy > 0.99*maxqy[il-1]) ){//Y is the seed
      maxqy[il-1] = tqy;
      maxqx[il-1] = tqx;
    }
    //----set first linearization
    if((TRCLFFKEY.ClusterCofGOpt%10)>=2){//QY add
      if(xcls)xcls->SetQtrk(tqx);
      ycls->SetQtrk(tqy);
    }
    double qratio=(tqy<tqx)? tqy/tqx: tqx/tqy;
    if(tqx==tqy)qratio=1;//avoid 0
    double sig=tqy*1000+qratio;//<1/1000
    if(ycls->checkstatus(TrClusterR::ZSEED))sig=sig*2;//set higher priority for ZSEED
    rhitsorder.push_back(make_pair(sig,rhit));
  }

  //sort with YQ+XYMatch
  if(reorderhits>=1){
    std::sort(rhitsorder.begin(),rhitsorder.end(),CompSignal<TrRecHitR>); 
    _rhits.clear();
    for(unsigned int ii=0; ii<rhitsorder.size(); ii++){
      _rhits.push_back(rhitsorder.at(ii).second);
    }
  }

///-----check ion charge (Z>=2)
  vector<double> qs[2];
  for(int il=0;il<9;il++) {
    if( il<2 || il>8 ) continue;//QY change to use inner
    if(maxqx[il]>1.6 )qs[0].push_back(maxqx[il]);
    if(maxqy[il]>1.6 )qs[1].push_back(maxqy[il]);
  }
  int noversp[2] ={0,0};
  double totqsp[2]={0,0};
  for(int ixy=0;ixy<2;ixy++){
    if(qs[ixy].size()==0)continue;
    std::sort(qs[ixy].begin(),qs[ixy].end());
    std::reverse(qs[ixy].begin(),qs[ixy].end());//highest to lowest
    while(qs[ixy].size()>4){//eliminate noise by 4th highest cluster
      double th=qs[ixy].at(3)*TRCLFFKEY.ZSEEDClQThr;//4th highest cluster q as threshould
      if(qs[ixy].back()<th)qs[ixy].pop_back();
      else break;
    }
    for(unsigned int ii=0;ii<qs[ixy].size();ii++){
      double qn=qs[ixy].at(ii);
//      cout<<"ixy="<<ixy<<" ii="<<ii<<" qn="<<qn<<endl;
      noversp[ixy]++; totqsp[ixy] += qn; 
    }
  }
  if( noversp[0] <3 || noversp[1] <4 ) return 0;
//  cout<<"totq="<< (totqsp[0]+totqsp[1])/( noversp[0] +noversp[1] )<<","<<(totqsp[0]+totqsp[1]-qs[0][0]-qs[1][0])/( noversp[0] +noversp[1]-2)<<endl;//truncation mean
//-----QY add
  if((TRCLFFKEY.ClusterCofGOpt%10)>=2){//set second linearized coordinate
    totqsp[0]=(totqsp[0]-qs[0][0])/(noversp[0]-1);//X
    totqsp[1]=(totqsp[1]-qs[1][0])/(noversp[1]-1);//Y
    for(unsigned int ihit=0;ihit< _rhits.size(); ihit++  )  {
      TrRecHitR *rhit = _rhits[ihit];
      for(int ixy=0;ixy<2;ixy++){
        TrClusterR *cls =(ixy==0)?rhit->GetXCluster():rhit->GetYCluster();
        if(!cls)continue;
        double tqn=cls->GetQtrk(); 
        if( fabs(tqn-totqsp[ixy]) <= 0.3*totqsp[ixy])cls->SetQtrk(totqsp[1-ixy]);//use other-side average to linearize(inside windows)
      }
      rhit->BuildCoordinate();
    }
  }
  return (totqsp[0]+totqsp[1]-qs[0][0]-qs[1][0])/( noversp[0] +noversp[1]-2);//truncation
  
}


int TrReconQ::AddTracks(int clear, int update)
{
  AMSEventR *ev = evt;
  if (!ev) ev = AMSEventR::Head();
  if (!ev) return -1;

  ev->UpdateTrRecon();

  if (update) {
    for (unsigned int i = 0; i < ev->NParticle(); i++)
      ev->pParticle(i)->UpdateTrTrack(5, 5);
  }

  return 0;
}


double TrReconQ::TkTkDist( TrTrackR* tr1, TrTrackR* tr2 , AMSPoint &vertex, AMSPoint &cdist )
{
  AMSPoint pEntry[2],pEnd[2];
  if( tr1->GetNormChisqY() > 100 || tr2->GetNormChisqY() > 100  ) return -1;
  if( tr1->GetNormChisqY() <= 0 || tr2->GetNormChisqY() <= 0  ) return -1;
  //check the overlap of track, if exist, just skip this event.
  bool isoverlap = false;
  for(int i=0;i<9;i++)
  {
    if( !tr1->TestHitLayerJ( i+1 ) ) continue;
    if( !tr2->TestHitLayerJ( i+1 ) ) continue;
    int icly1= tr1->GetHitLJ(i+1)->GetYClusterIndex ();
    int icly2= tr2->GetHitLJ(i+1)->GetYClusterIndex ();
    if(icly1 == icly2 )
    {
      isoverlap = true;
      break;
    }
    if(  tr1-> TestHitLayerJHasXY(i+1)&& tr2->TestHitLayerJHasXY(i+1)  )
    {
       int iclx1= tr1->GetHitLJ(i+1)->GetXClusterIndex ();
       int iclx2= tr2->GetHitLJ(i+1)->GetXClusterIndex ();
       if(iclx1 == iclx2 )    {
         isoverlap = true;
         break;
       }
    }
  }
  if( isoverlap ) return -1;

  for(int i=0;i<9;i++)
  {
    if(  tr1->TestHitLayerJ( i+1 ) )  pEnd[0] = tr1->GetHitLJ(i+1) ->GetCoord();
    if(  tr2->TestHitLayerJ( i+1 ) )  pEnd[1] = tr2->GetHitLJ(i+1) ->GetCoord();
    if(  tr1->TestHitLayerJ( 9-i))  pEntry[0] = tr1->GetHitLJ(9-i) ->GetCoord();
    if(  tr2->TestHitLayerJ( 9-i))  pEntry[1] = tr2->GetHitLJ(9-i) ->GetCoord();
  }

// float rge[2] = {  pEntry[0].z() < pEntry[1].z() ? pEntry[0].z(): pEntry[1].z(),
//                pEnd[0].z() > pEnd[1].z() ? pEnd[0].z():pEnd[1].z()
//  };

  double rge1[2] = {-200,200 };
  double step = 4; double dist = 9999;
  double idealz = 9999;
  double ideald = 10;
  AMSPoint ctx;
  for(int iclus = 0; iclus<50; iclus++){
    if( fabs(rge1[0] - rge1[1]) < 1e-5  ) break;
    for( int iter = 0; iter<100;iter++  )
    {
      double fz =  rge1[0] + iter * step;
      AMSPoint pp[2];
      AMSDir   dd[2];
      tr1->Interpolate(fz,pp[0],dd[0]);
      tr2->Interpolate(fz,pp[1],dd[1]);
      dist =   pow( ( pp[0][1] - pp[1][1] )*( pp[0][1] - pp[1][1] ) + ( pp[0][0] - pp[1][0]  )* ( pp[0][0] - pp[1][0]  ) ,0.5 ) ;
      if(dist < ideald ) {
        cdist.setp(  ( pp[0][0] - pp[1][0]  ), ( pp[0][1] - pp[1][1] ), 0);
        ideald = dist;
        idealz = fz;
        ctx = (pp[0] + pp[1])/2;
      }
    }
    if(ideald >= 10 ) break;
    rge1[0] = ( (idealz - 5* step) < ( rge1[0] ) ?  ( rge1[0] ) : (idealz - 5* step))  ;
    rge1[1] = ( (idealz + 5* step) > ( rge1[1] ) ?  rge1[1]:  (idealz + 5* step) )  ;
    step /= 10.0;
  }
  if(ctx.z()==0 || ideald >= 10 || idealz == 9999 ) return -1;
 //if( ctx.z() >  rge[1] - 300e-4   &&  ctx.z() <  rge[0] + 300e-4  ) return -1;

  vertex = ctx;
  return  ideald ;
}

int TrReconQ::PriTrkSelt(vector<TrTrackR*> &trks, vector<int> &idx)
{
  if( trks.size() <= 0) return -1;
  vector<double> qualities;
  for(unsigned int itrk=0;itrk<trks.size();itrk++)
  {
    double quality = 0;
    TrTrackR  *trk = trks[itrk];
    Int_t itp = trk->iTrTrackPar(1, 0, 1);
    if (trk->GetRigidity  (itp) == 0)
    {
      qualities.push_back(0);
      continue;
    }
    if(trk->TestHitLayerJ(1)) quality+=2;
    if(trk->TestHitLayerJ(9)) quality+=2;

    for(int i=1;i<8;i++)
    {
      if(trk->TestHitLayerJ(i+1)) quality += 0.8;
    }

    double ly = trk->GetNormChisqY(itp);
//    double lx = trk->GetNormChisqX(itp);
//    quality += 1.2/( lx*lx*lx + 2000 );
//    if( ly > 1) quality -= 2;
    quality += 2.5/( ly*ly*ly + 2000 );
    qualities.push_back(quality);
  }
  int tmpidx = -1;
  vector<int> index;
  for(unsigned int i=0;i<qualities.size();i++)
    index.push_back(i);
  for(unsigned int i=0;i<qualities.size();i++)
    idx.push_back(-1);
  double tmpquality = -1;
  if(qualities.size()==1)
  {
    index[0] = 0;
  }
  else
  {
    for(int i=0;i<int(qualities.size())-1;i++)
    {
      for( int j=0; j<int(qualities.size())-i-1;j++  )
        if(qualities[j] < qualities[j+1] )
        {
          tmpquality = qualities[j];
          qualities[j] = qualities[j+1];
          qualities[j+1] = tmpquality;
          int itmp;
          itmp = index[j];
          index[j] = index[j+1];
          index[j+1] = itmp;
        }
    }
  }

  for(unsigned int i=0;i<index.size();i++)
  {
    if( i+1> index.size()) break;
    idx[i] = index[i];
  }
  tmpidx = idx[0];
  return tmpidx;
}


struct TrTrVertex
{
  int trIndex[2];
  double dist;
  AMSPoint vertex;
  AMSPoint cdist;
  TrTrVertex()
  {
    trIndex[0] = -1;
    trIndex[1] = -1;
    dist = -1;
    vertex.setp(0,0,0);
  };
  void setp(int tr1, int tr2,AMSPoint vv, double dd ,AMSPoint dst ) {
    trIndex[0] = tr1;
    trIndex[1] = tr2;
    vertex = vv;
    dist = dd;
    cdist = dst;
  };
};

double TrReconQ::BuildVertex(  )
{
  vector<TrTrackR*> trks;
  int prTkIdx = -1;
  VCon *cont = GetVCon()->GetCont("AMSTrTrack");
  int ntk = (cont)? cont->getnelem() : 0;
  for(int ik=0;ik<ntk;ik++){
    TrTrackR *track = (TrTrackR *)cont->getelem(ik);
    trks.push_back(track);
  }
  if(cont)delete cont;
  if(trType == 0 ) {//
    vector<int> tkIdx ;
    for(int i=0;i<ntk;i++) {
      tkIdx.push_back( -1 );
    }
    if( ntk >1  )
      PriTrkSelt(trks, tkIdx);
    else
      return -1;
    prTkIdx = tkIdx[0];
  }
  else
  {
    if(ntk >1)
    {
      vector<int> tkIdx ;
      PriTrkSelt(trks, tkIdx);
      prTkIdx = tkIdx[0];
    }
    else
      return -1;
  }
  if(prTkIdx <0 ) return -1;
  if(ntk <= 1) return -1;
  ///merging track track vertex.
  vector<TrTrVertex> attex;
  double dd =0;
  TrTrackR *tr1 = trks[prTkIdx];
  for(int i=0;i<ntk;i++   ){
    TrTrackR *tr2 = trks[i];
    AMSPoint vertex;
    AMSPoint tktkdist;
    dd =  TkTkDist(tr1,tr2,vertex, tktkdist );
    if (dd > 10) continue;
    if( dd <= 0) continue;
    TrTrVertex v1;
    v1.setp(prTkIdx, i,vertex, dd ,tktkdist  );
    attex.push_back(v1);
  }


  // reorder all track combination, by increasing of dd
  TrTrVertex tvtex;
  if(attex.size() <1  ) return -1;
  else if(attex.size()>1){
    for(int i=0;i<int(attex.size()) -1;i++) {
      for(int j=0;j<int(attex.size())-i-1;j++) {
        if( attex[j].dist >  attex[j+1].dist) {
          tvtex = attex[j];
          attex[j] = attex[j+1];
          attex[j+1] = tvtex;
        }
      }
    }
  }

  _svtx = attex[0].vertex;
  _sdst = attex[0].cdist;
  _sddst = attex[0].dist;
  _nsvtx = 1;
  _vtrIdx[0] = attex[0]. trIndex[0];
  _vtrIdx[1] = attex[0]. trIndex[1];

  return  attex[0].dist;
}


int TrReconQ::GetVertex(AMSPoint &vtex , AMSPoint &dist , int idx[2])
{
  if( _nsvtx <1 ) return 0;
  vtex = _svtx; dist = _sdst;
  idx[0] = _vtrIdx[0];
  idx[1] = _vtrIdx[1];
  return _nsvtx;
}
