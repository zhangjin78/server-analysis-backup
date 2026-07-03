// $Id: TrTrack.C,v 1.301 2023/01/10 11:15:27 qyan Exp $

//////////////////////////////////////////////////////////////////////////
///
///\file  TrTrack.C
///\brief Source file of AMSTrTrack class
///
///\date  2007/12/03 SH  First import (just a copy of trrec.C in Gbatch)
///\date  2007/12/20 SH  TkFit support
///\date  2007/12/28 SH  First stable vertion after a refinement
///\date  2008/01/21 SH  Imported to tkdev (test version)
///\date  2008/02/13 SH  Comments are updated
///\date  2008/03/01 AO  Preliminary new data format
///\date  2008/03/12 SH  Getting back to SH version
///\date  2008/07/01 PZ  New Tracker Lib Standalone but to be emebedded
///\date  2008/09/16 PZ  Rename as TrTrackR to be gbatch compatible
///\date  2008/10/23 PZ  Modifications for GBATCH compatibility
///\date  2008/11/05 PZ  New data format to be more compliant
///\date  2008/11/13 SH  Some updates for the new TrRecon
///\date  2008/11/20 SH  A new structure introduced
///\date  2022/02/07 QY  New V6 software
///$Date: 2023/01/10 11:15:27 $
///
///$Revision: 1.301 $
///
//////////////////////////////////////////////////////////////////////////

#include <iomanip>
#include "MagField.h"
#include "TkDBc.h"
#include "TFitResult.h"
#include "point.h"
#include "tkdcards.h"
#include "VCon.h"
#include "TrExtAlignDB.h"
#include "TrTrack.h"
#include "TrRecHit.h"
#include "TkSens.h"
#include "TrRecon.h"
#include <cmath>
#include <algorithm>


#include "TrCharge.h"
#include "TrLikeDB.h"
#include "TrMass.h"

#include "TrTrackChargeH.h"
#include "AMSPlane.h"
#include "root.h"


int my_int_pow(int base, int exp){
  if (exp<0) return -1;
  if (base==0) return 0;
  int ss=1;
  if (base<0 && exp%2==1)ss=-1;
  int bb=abs(base);
  int out=1;
  for(int ii=0;ii<exp;ii++)
    out*=bb;
  return out*ss;
}


ClassImp(TrTrackPar)


double TrTrackPar::RigPZ[DEF_NTKZRIG]={195,0,-70,-136};

void TrTrackPar::Print(int full) const {
  printf("Rigidity:  %6.3f Err(1/R):  %7.5f P0: %6.3f %6.3f %6.3f  Dir:  %6.4f %6.4f %6.4f\n",
	 Rigidity,ErrRinv,P0[0],P0[1],P0[2],Dir[0],Dir[1],Dir[2]);
  if(!full)return;
  printf("HitBits: %s, Chi2X/Ndf: %f/%d, Chi2Y/Ndf: %f/%d, Chi2: %f \n",
	 TrTrackR::HitBitsString(HitBits),ChisqX,NdofX,ChisqY,NdofY,Chisq);
}


void TrTrackR::PrintHits(int full) const {
  for (int ii=1;ii<10;ii++){
    TrRecHitR* hit=GetHitLJ(ii);
    if(hit) hit->Print(full);
  }
}

void  TrTrackPar::Print_stream(std::string &ostr,int full) const {
  ostr.append(Form("Rigidity:  %6.3f Err(1/R):  %7.5f P0: %6.3f %6.3f %6.3f  Dir:  %6.4f %6.4f %6.4f\n",
		   Rigidity,ErrRinv,P0[0],P0[1],P0[2],Dir[0],Dir[1],Dir[2]));
  if(!full)return;
  ostr.append(Form("HitBits: 0x%04x %s, Chi2X/Ndf: %f/%d, Chi2Y/Ndf: %f/%d, Chi2: %f \n",
		   HitBits,TrTrackR::HitBitsString(HitBits),ChisqX,NdofX,ChisqY,NdofY,Chisq));
  return ;
}




ClassImp(TrTrackR);


int   TrTrackR::NhitHalf      = 4;
int   TrTrackR::DefaultFitID  = TrTrackR::kChoutko | TrTrackR::kMultScat;
float TrTrackR::DefaultMass   = 0.938272297;
float TrTrackR::DefaultCharge = 1;

int TrTrackR::DefaultAdvancedFitFlags[DEF_ADVFIT_NUM]=
  { kChoutko, kChoutko|kMultScat, 
    kAlcaraz, kAlcaraz|kMultScat,
  };

int TrTrackR::DefaultAdvancedFitFlags2[DEF_ADVFIT_NUM2]=
  { kChoutko, kChoutko|kMultScat,
    kAlcaraz, kAlcaraz|kMultScat,
    kGEANE_Kalman|kMultScat|kEnergyLoss, kChikanianF|kMultScat|kEnergyLoss, 
    kGBLFITTERAMS|kMultScat,
  };

int TrTrackR::AdvancedFitBits = 0x5f;//(kChoutko,kChoutko|kMultScat,kAlcaraz,kAlcaraz|kMultScat,kGEANE_Kalman|kMultScat|kEnergyLoss,kGBLFITTERAMS|kMultScat) are on
int TrTrackR::AdvancedFitBitsHalf = 0x5a;//(kChoutko|kMultScat,kAlcaraz|kMultScat,kGEANE_Kalman|kMultScat|kEnergyLoss,kGBLFITTERAMS|kMultScat) are on
int TrTrackR::AdvancedFitBitsExtAll = 0x5a;//(kChoutko|kMultScat,kAlcaraz|kMultScat,kGEANE_Kalman|kMultScat|kEnergyLoss,kGBLFITTERAMS|kMultScat) are on

TrTrackR::TrTrackR(): _Pattern(-1), _Nhits(0)
{
  for (int i = 0; i < trconst::maxlay; i++) {
    _Hits [i] = 0;
    _iHits[i]= -1;
    //    _iMult[i] = -1;
    //    _BField[i]=AMSPoint(0,0,0);
  }
  trdefaultfit=0;
  _MagFieldOn=0;
  _bit_pattern=0;
  _bit_patternX=0;
  _PatternX=0;
  _PatternY=0;
  _PatternXY=0;
  _NhitsX=0;
  _NhitsY=0;
  _NhitsXY=0;
  DBase[0]=0;
  DBase[1]=0;
  Status=0;
  _ReconType=0; 
  advancedfitmass=advancedfitcharge=0;

  trkcharge.Init();
}

TrTrackR::TrTrackR(int pattern, int nhits, TrRecHitR *phit[],AMSPoint bfield[], int *imult,int fitmethod)
  :_Pattern(pattern), _Nhits(nhits), _NhitsX(0), _NhitsY(0), _NhitsXY(0)

{
  _ReconType=0; 
  _bit_pattern=0;
  _bit_patternX=0;
  _MagFieldOn=0;
  for (int i = 0; i < trconst::maxlay; i++) {
    _Hits [i] = (phit   && i < _Nhits) ? phit [i] :  0;
    //    _iMult[i] = (imult  && i < _Nhits) ? imult[i] : -1;

    if (imult  && i < _Nhits && phit[i] ) 
      phit[i]->SetResolvedMultiplicity(imult[i]);

    //    _BField[i]= (bfield && i < _Nhits) ? bfield[i]: AMSPoint(0,0,0);
    if(bfield && i < _Nhits && bfield[i].norm()!=0) _MagFieldOn=1;
    if (phit && i < _Nhits) {
      _bit_pattern|= 1<<(phit[i]->GetLayer()-1);
      if (phit[i]->GetXCluster()) {_NhitsX++; _bit_patternX|= 1<<(phit[i]->GetLayer()-1);}
      if (phit[i]->GetYCluster()) _NhitsY++;
      if (phit[i]->GetXCluster() && phit[i]->GetYCluster()) _NhitsXY++;
    }
  }

  // These patterns should be filled with SetPatterns()
  _PatternX = _PatternY = _PatternXY = 0;

  DBase[0] = DBase[1] = 0;
  Status=0;
  BuildHitsIndex();
  trdefaultfit=fitmethod;
  if(trdefaultfit==0) trdefaultfit=DefaultFitID;
  advancedfitmass=advancedfitcharge=0;

  trkcharge.Init();
}

double TrTrackR::ReFitHitStat(float chrg,int layJ, int id, bool forceV6){

  if(id==0)id=trdefaultfit;
  int bl=(layJ<0)?0:layJ-1;
  int el=(layJ<0)?9:layJ;
  double distm=0;//max distance for all hit
  for(int il=bl;il<el;il++){
    if( !TestHitLayerJ(il+1) ) continue;
    TrRecHitR *hit = GetHitLJ(il+1);
    if(!hit) continue;
    hit->SetUsed();
    if((id&kAlignV6)||forceV6){//V6 version
      TkLadderN *ladder=hit->GetLadder();
      AMSPlane pl=(*ladder);
      double rigidity;
      double length=Interpolate(pl,rigidity,id);//Interpolate to the ladder
      if(length<=-1000)continue;//should set some error
      int mult;
      double dist,xaddress;
      AMSPlaneM plm0=hit->FindMult(dist,mult,xaddress,pl.getP(),pl.getD(),-2);//position and direction in ladder, fast search
      length=Interpolate(plm0,rigidity,id);//Interpolate to sensor with more precise position and direction
      if(length<=-1000)continue;//should set some error
      AMSPlaneM plm=hit->FindMult(dist,mult,xaddress,plm0.getP(),plm0.getD(),-2);//position and direction in sensor, fast search
      hit->SetResolvedMultiplicity(mult);
      if(hit->OnlyY())hit->SetDummyX(xaddress-640);
      if(dist>distm)distm=dist;//distance between track and hit
    }
    else { 
      AMSPoint escc = InterpolateLayerJ (il+1,id);
      int imlt = -1;
      if(!hit->OnlyY()){
        hit->HitPointDist(escc,imlt);//hit postion between sensors resolved muliplicity by TkSens could be wrong, Hit distance always right
        hit->SetResolvedMultiplicity( imlt );
      }
      else {
//       TkSens tks(0, escc,0);
       TkSens tks(hit->GetTkId(),escc,0);//bug fixed by QY
       if (tks.LadFound() && tks.GetStripX() < 0) {
         float sx = tks.GetSensCoo().x();
         float sg = TkDBc::Head->FindTkId(tks.GetLadTkID())->GetRotMat().GetEl(0, 0);
         float sp = TkDBc::Head->FindTkId(tks.GetLadTkID())->GetPlane()->GetRotMat().GetEl(0, 0);
         if (sx < 0) escc[0] -= sg*sp*(sx-1e-3);//bug fixed by QY
         if (sx > TkDBc::Head->_ssize_active[0])
           escc[0] -= sg*sp*(sx-TkDBc::Head->_ssize_active[0]+1e-3);//bug fixed by QY
         tks.SetGlobalCoo(escc);
       }
       if (!tks.LadFound() || tks.GetLadTkID() != hit->GetTkId()) continue;
       int nsens = tks.GetMultIndex();
       int nmlt = hit->GetMultiplicity();
       if( nsens >= nmlt  )  	 imlt = nmlt-1;
       else if(  nsens < 0  )       imlt = 0;
       else imlt = nsens;
       hit->SetDummyX(tks.GetStripX());
       hit->SetResolvedMultiplicity( imlt );
      }
    }
  }

   //have to put inclination-angle/charge to clusters again. Because merge could lose these information
  if(layJ<0)RecalcHitCoordinates(id,0,0,chrg);
  return distm; 
  
}



TrTrackR::TrTrackR(number theta, number phi, AMSPoint point)
  : _Pattern(-1), _Nhits(0)
{
  trdefaultfit=kDummy;
  TrTrackPar &par = _TrackPar[trdefaultfit];
  AMSDir dir(theta,phi);
  AMSPoint p0;
  p0[0]=point[0]-dir[0]/dir[2]*point[2];
  p0[1]=point[1]-dir[1]/dir[2]*point[2];
  p0[2]=0;

  par.FitDone = true;
  par.P0      = p0;
  par.Dir     = AMSDir(theta,phi); 
  par.Rigidity=1e6;
  par.ErrRinv  = 1e7;
  for(int i = 0; i < trconst::maxlay; i++) {
    _Hits [i] = 0;
    _iHits[i] = -1; //_iMult[i] = -1;
    //    _BField[i]= AMSPoint(0,0,0);
    par.Residual[i][0] = 0;
    par.Residual[i][1] = 0;
  }
  Status=0;
 _bit_patternX= _bit_pattern=_PatternX = _PatternY = _PatternXY = _NhitsX = _NhitsY = _NhitsXY = 0;
  _MagFieldOn=0;
  DBase[0] = DBase[1] = 0;
  advancedfitmass=advancedfitcharge=0;  

  trkcharge.Init();
}

TrTrackR::TrTrackR(AMSDir dir, AMSPoint point, number rig, number errig)
  : _Pattern(-1), _Nhits(0)
{
  trdefaultfit=kDummy;
  TrTrackPar &par = _TrackPar[trdefaultfit];
  
  AMSPoint p0;
  p0[0]=point[0]-dir[0]/dir[2]*point[2];
  p0[1]=point[1]-dir[1]/dir[2]*point[2];
  p0[2]=0;

  par.FitDone  = true;
  par.Rigidity = rig;
  par.ErrRinv  = errig;
  par.P0       = p0;
  par.Dir      = dir;

  for(int i = 0; i < trconst::maxlay; i++) {
    _Hits [i] = 0;
    _iHits[i] = -1; //_iMult[i] = -1;
    //    _BField[i]= AMSPoint(0,0,0);
    par.Residual[i][0] = 0;
    par.Residual[i][1] = 0;
  }
  Status=0;
  _bit_patternX=_bit_pattern=_PatternX = _PatternY = _PatternXY = _NhitsX = _NhitsY = _NhitsXY = 0;
  _MagFieldOn=0;
  DBase[0] = DBase[1] = 0;
  advancedfitmass=advancedfitcharge=0;
  
  trkcharge.Init();
}

TrTrackR::TrTrackR(const TrTrackR& orig){
  for (int ii=0;ii<trconst::maxlay;ii++){
    _Hits[ii]=orig._Hits[ii] ;
    //    _BField[ii]=orig._BField[ii] ; 
    _iHits[ii]=orig._iHits[ii] ;
    //    _iMult[ii]=orig._iMult[ii] ;
  }
  _HitCoo=orig._HitCoo;
  _bit_pattern=orig._bit_pattern ;
  _bit_patternX=orig._bit_patternX ;
  _Pattern=orig._Pattern ;
  _Nhits=orig._Nhits ;
  _PatternX=orig._PatternX ;
  _PatternY=orig._PatternY ;
  _PatternXY=orig._PatternXY ;
  _NhitsX=orig._NhitsX ;
  _NhitsY=orig._NhitsY ;
  _NhitsXY=orig._NhitsXY ;
//  _TrFit=orig._TrFit ;  //!
  _TrackPar=orig._TrackPar;
  _MagFieldOn=orig._MagFieldOn ;
  for(int ii=0;ii<2;ii++)
    DBase[ii]=orig.DBase[ii] ;
  trdefaultfit=orig.trdefaultfit ;
  Status=orig.Status ;
   _ReconType = orig._ReconType; 
  advancedfitmass=orig.advancedfitmass;
  advancedfitcharge=orig.advancedfitcharge;

  trkcharge=orig.trkcharge;
}

TrTrackR::~TrTrackR()
{
#ifdef __ROOTSHAREDLIBRARY__
  for (int i = 0; i < trconst::maxlay; i++)
    if (_Hits[i] && _iHits[i] == -2 && _Hits[i]->Reblt()) {
      delete _Hits[i]->GetXCluster();
      delete _Hits[i]->GetYCluster();
      delete _Hits[i];
    }
#endif
}


const TrTrackPar &TrTrackR::GetPar(int id) const
{
  int id2 = (id == 0) ? trdefaultfit : id;
  if (_MagFieldOn == 0 && id2 != kDummy) id2 = kLinear;
  if (ParExists(id2) &&  _TrackPar.find(id2)!=_TrackPar.end() ) return _TrackPar.find(id2)->second;
  static int i=0;
   if(i++<100)
     cerr << "TrTrackR::GetPar-W-Parameter not exists (" << id << "): "
	  << GetFitNameFromID(id) << endl;
  static TrTrackPar parerr;
  return parerr;
}

bool TrTrackR::TestHitLayerJHasXY(int layJ) const
{
  // For backward compatibility (< B584)
  if (_bit_patternX == 0) {
    TrRecHitR *hit = GetHitLJ(layJ);
    return (hit && (!hit->OnlyY()) && (!hit->OnlyX()));
  }
  // For >= B584 (minimum I/O load)
  int lay=TkDBc::Head->GetLayerFromJ(layJ);
  if(_bit_patternX&(1<<(lay-1))) return true;
  return false;
}

long long TrTrackR::GetTrackPathID() const{
  long long ret=0;
  for (int ii=0;ii<GetNhits();ii++){
    TrRecHitR* hit=pTrRecHit(ii);
    if(hit){
      long long pp=hit->GetSlot();
      if(hit->GetSlotSide()==0)pp|=0x10;
      ret|=pp<<((hit->GetLayer()-1)*5);
    }
  }
  return ret;
}



TrTrackPar &TrTrackR::GetPar(int id)
{
  int id2 = (id == 0) ? trdefaultfit : id;
  if (ParExists(id2) && _TrackPar.find(id2)!=_TrackPar.end()) return _TrackPar[id2]; // Be careful of [] operator !
  static int i=0;
   if(i++<100)
     cerr << "TrTrackR::GetPar2-W-Parameter not exists (" << id << "): "
          << GetFitNameFromID(id) << endl;

  static TrTrackPar parerr;
  return parerr;
}

TrTrackPar &TrTrackR::AddPar(int id)
{
  return _TrackPar[id]; // Be careful of [] operator !
}

int TrTrackR::RemovePar(int id)
{
  int id2 = (id == 0) ? trdefaultfit : id;
  map<int, TrTrackPar>::iterator it=_TrackPar.find(id2); 
  if(it!=_TrackPar.end()){_TrackPar.erase(it); return 1;}
  else return 0;
}


double TrTrackR::GetRigidity (int id, int iz) const 
{
  double rigidity=GetPar(id).Rigidity;
  if(iz>=0){
    int id2 = (id == 0) ? trdefaultfit : id;
    int idf = id2&0xf;
    if(idf == kGEANE_DAF || idf == kGEANE_Kalman)rigidity=GetPar(id).PZRigidity[iz];
  }
  return rigidity;
}

double TrTrackR::GetNormChisqX(int id) const
{
  double enorm = 1;
  return (GetNdofX(id) > 0) ? GetChisqX(id)/GetNdofX(id)*enorm*enorm : 0;
}

double TrTrackR::GetNormChisqY(int id) const
{
  double enorm = 1;
  return (GetNdofY(id) > 0) ? GetChisqY(id)/GetNdofY(id)*enorm*enorm : 0;
}

#include "Math/ProbFuncMathCore.h"
double TrTrackR::GetPVal(int ixy, int id) const
{
  double chisq=0;
  int ndof=0;
  if     (ixy==0){ndof=GetNdofX(id);              chisq=GetChisqX(id);    }//X
  else if(ixy==1){ndof=GetNdofY(id);              chisq=GetChisqY(id);    }//Y
  else           {ndof=GetNdofX(id)+GetNdofY(id); chisq=GetChisq(id)*ndof;}//X+Y Combined 
  return (ndof<=0)? ndof: std::max(0.,ROOT::Math::chisquared_cdf_c(chisq, ndof));
}


AMSPoint TrTrackR::GetPlayerO(int ilay, int id) const
{
  TrRecHitR *hit = GetHitLO(ilay);
  if (hit) {
    AMSPoint pres = GetResidualO(ilay, id);
    AMSPoint coo  = hit->GetCoord();
    return coo-pres;
  }
  return InterpolateLayerO(ilay, id);
}


AMSPoint TrTrackR::GetPG_CIEMAT_diff(int layJ){
  if(layJ!=1 && layJ!=9) return AMSPoint(0,0,0);
  TrRecHitR* hit=GetHitLJ(layJ);
  if(!hit)return AMSPoint(0,0,0);
  return hit->GetCoord(-1,4);
}
  



AMSPoint TrTrackR::GetPlayerJ(int ilayJ, int id) const
{
  AMSPoint pres = GetResidualJ(ilayJ, id);

  for (int i = 0; i < GetNhits(); i++) {
    TrRecHitR *hit = GetHit(i);
    if (hit && hit->GetLayerJ() == ilayJ) {
      //      AMSPoint coo = (_iMult[i] >= 0) ? hit->GetCoord(_iMult[i])
      //                              : hit->GetCoord();
      AMSPoint coo = hit->GetCoord();
      return coo-pres;
    }
  }
  return pres;
}


void TrTrackR::AddHit(TrRecHitR *hit, int imult)
{
  int ihit = -1;
  for (int i = 0; i < _Nhits; i++){
    TrRecHitR *hh = GetHit(i);
    if (hh && hh->GetLayer() == hit->GetLayer()) ihit = i;
  }
  if (ihit < 0) {
    if (_Nhits >= trconst::maxlay) {
      cerr << "Error in TrTrack:AddHit the hit vector is already full"
	   << endl;
      return;
    }
    ihit = _Nhits;
    _Nhits++;
  }
  else {
    TrRecHitR *hh = GetHit(ihit);
    if (hh->GetXCluster()) _NhitsX--;
    if (hh->GetYCluster()) _NhitsY--;
    if (hh->GetXCluster() && hh->GetYCluster()) _NhitsXY--;
    _bit_pattern&= ~(1<<(hh->GetLayer()-1));//erase previous bit
    if(hh->GetXCluster())_bit_patternX&= ~(1<<(hh->GetLayer()-1));//X must have to erase bit
  }

  if (hit->GetXCluster()) _NhitsX++;
  if (hit->GetYCluster()) _NhitsY++;
  if (hit->GetXCluster() && hit->GetYCluster()) _NhitsXY++;
  _bit_pattern|= (1<<(hit->GetLayer()-1)); 
  if (hit->GetXCluster()) _bit_patternX|= (1<<(hit->GetLayer()-1));
  

  VCon* cont2=GetVCon()->GetCont("AMSTrRecHit");
  _Hits [ihit] = hit;
  _iHits[ihit] = cont2->getindex(hit);
  if(imult>-1) hit->SetResolvedMultiplicity(imult);
  int ll=hit->GetLayer();
  if(ll==8||ll==9){
    // TrExtAlignDB::SetAlKind(1);
    // int l1=-1;
    // int l9=-1;
    // if(ll==8)
    //   l1=!hit?-1:1+hit->GetSlotSide()*10+hit->lad()*100;
    // else
    //   l9=!hit?-1:9+hit->GetSlotSide()*10+hit->lad()*100;
    // int rret=UpdateExtLayer(1,l1,l9);  // CIEMAT
    // hit->BuildCoordinate();
    _HitCoo[ll+10]=hit->GetCoord(-1,2);
    _HitCoo[ll+20]=hit->GetCoord(-1,3);
    // TrExtAlignDB::SetAlKind(0);
    // hit->BuildCoordinate();
    _HitCoo[ll]=hit->GetCoord(-1,3);
  }else 
    _HitCoo[ll]=hit->GetCoord();

  //  _iMult[ihit] = (imult >= 0) ? imult : hit->GetResolvedMultiplicity();
  delete cont2;

  if (MagFieldOn()){
    _MagFieldOn = 1;
  }
  else {
    _MagFieldOn = 0;
  }

  // Update the bitted pattern

#ifdef __DARWIN__
  typedef unsigned short ushort;
#endif
  ushort _bit=0;
  ushort _bitX=0;
  ushort _bitY=0;
  ushort _bitXY=0;
  for (int jj=0;jj<_Nhits;jj++){
    _bit|=(1<<(9-pTrRecHit(jj)->GetLayer()-1));
    if( pTrRecHit(jj)->GetXCluster()) _bitX|=(1<<(9-pTrRecHit(jj)->GetLayer()-1));
    if( pTrRecHit(jj)->GetYCluster()) _bitY|=(1<<(9-pTrRecHit(jj)->GetLayer()-1));
    if( pTrRecHit(jj)->GetYCluster()&&pTrRecHit(jj)->GetYCluster())
      _bitXY|=(1<<(9-pTrRecHit(jj)->GetLayer()-1));
  }
  _bit   = _bit>>1;
  _bitX  = _bitX>>1;
  _bitY  = _bitY>>1;
  _bitXY = _bitXY>>1;
      if (!patt) {
       int nn = (TkDBc::Head->GetSetup()==3) ? 7 : 8;
       patt = new tkpatt(nn);
       patt->Init(nn);
     }
  _Pattern   = patt->GetHitPatternIndex(_bit);
  _PatternX  = patt->GetHitPatternIndex(_bitX);
  _PatternY  = patt->GetHitPatternIndex(_bitY);
  _PatternXY = patt->GetHitPatternIndex(_bitXY);
  return;
}


bool TrTrackR::RemoveHitOnLayer( int layer){
  if(layer <1 || layer >9) return false;
  int idx=-1;
  TrRecHitR* phit=0;
  for (int ii=0; ii< _Nhits; ii++){
    TrRecHitR* phit2=pTrRecHit(ii);
    if( phit2->GetLayer() == layer)
      {idx=ii; phit=phit2;}
  }
  if(idx==-1) return false;
  // Remove the entry in the arrays
  _Nhits--;
  for (int kk=idx;kk<_Nhits;kk++){
    _Hits   [kk] = _Hits   [kk+1];
    //    _BField [kk] = _BField [kk+1];
    _iHits  [kk] = _iHits  [kk+1];
  }
  map<int,AMSPoint>::iterator hh=_HitCoo.find(layer);
  if(hh!=_HitCoo.end()) _HitCoo.erase(hh);
  if(layer>7){
    hh=_HitCoo.find(layer+10);
    if(hh!=_HitCoo.end()) _HitCoo.erase(hh);
  }
  // Update the number of projection hits
  if (phit->GetXCluster()) _NhitsX--;
  if (phit->GetYCluster()) _NhitsY--;
  if (phit->GetXCluster() && phit->GetYCluster()) _NhitsXY--;
  //unmark the used status bit
  phit->ClearUsed();

  // Update the bitted pattern

#ifdef __DARWIN__
  typedef unsigned short ushort;
#endif
  ushort _bit=_bit_pattern ^ (1<<(phit->GetLayer()-1));  
  _bit_pattern=_bit;
  _bit=0;
  if(phit->GetXCluster()){
    _bit=_bit_patternX ^ (1<<(phit->GetLayer()-1));  
    _bit_patternX=_bit;
    _bit=0;
  }

  ushort _bitX=0;
  ushort _bitY=0;
  ushort _bitXY=0;
  for (int jj=0;jj<_Nhits;jj++){
    _bit|=(1<<(9-pTrRecHit(jj)->GetLayer()-1));
    if( pTrRecHit(jj)->GetXCluster()) _bitX|=(1<<(9-pTrRecHit(jj)->GetLayer()-1));
    if( pTrRecHit(jj)->GetYCluster()) _bitY|=(1<<(9-pTrRecHit(jj)->GetLayer()-1));
    if( pTrRecHit(jj)->GetYCluster()&&pTrRecHit(jj)->GetYCluster())
      _bitXY|=(1<<(9-pTrRecHit(jj)->GetLayer()-1));
  }
  _bit   = _bit>>1;
  _bitX  = _bitX>>1;
  _bitY  = _bitY>>1;
  _bitXY = _bitXY>>1;

  if (!patt) {
    int nn = (TkDBc::Head->GetSetup()==3) ? 7 : 8;
    patt = new tkpatt(nn);
    patt->Init(nn);
  }
  _Pattern   = patt->GetHitPatternIndex(_bit);
  _PatternX  = patt->GetHitPatternIndex(_bitX);
  _PatternY  = patt->GetHitPatternIndex(_bitY);
  _PatternXY = patt->GetHitPatternIndex(_bitXY);


  //---bug have to remove TrackPar
  
  return true;
}

void TrTrackR::BuildHitsIndex()
{
  VCon *cont2 = GetVCon()->GetCont("AMSTrRecHit");
  if (!cont2) return;

  for (int i = 0; i < _Nhits; i++)
    _iHits[i] = cont2->getindex(_Hits[i]);

  delete cont2;
}

void TrTrackR::SetHitsIndex(int *ihit)
{
  VCon *cont = GetVCon()->GetCont("AMSTrRecHit");
  if (!cont) return;

  for (int i = 0; i < _Nhits; i++) {
    _iHits[i] = ihit[i];
    _Hits[i] = (TrRecHitR *)cont->getelem(_iHits[i]);
  }

  delete cont;
}

void TrTrackR::GetMaxShift(int &left, int &right) const
{
  left = right = 99;
  for (int i = 0; i < _Nhits; i++) {
    TrRecHitR *hit = GetHit(i);
    if (!hit) continue;

    int tkid  = hit->GetTkId();
    int nmult = hit->GetMultiplicity();
    int imult =  hit->GetResolvedMultiplicity();
    //    int imult = _iMult[i];
    int ll = 0;
    int rr = 0;

    if (tkid >= 0) { ll = nmult-imult-1; rr = imult; }
    else           { rr = nmult-imult-1; ll = imult; }
    if (ll < left)  left  = ll;
    if (rr < right) right = rr;
  }
  left *= -1;
}

void TrTrackR::Move(int shift, int fit_flags)
{
  for (int i = 0; i < _Nhits; i++) {
    TrRecHitR *hit = GetHit(i);
    if (!hit) continue;
   
    int tkid = hit->GetTkId();
    //   if (!hit->OnlyY()) {
      int nmult = hit->GetMultiplicity();
      int imult = hit->GetResolvedMultiplicity();
      //int imult = _iMult[i];

      if (tkid >= 0) imult -= shift;
      else           imult += shift;

      if (imult >= 0 && imult < nmult) {
	//	_iMult[i] = imult;
	_Hits [i]->SetResolvedMultiplicity(imult);
      }
      else cerr
	<< "TrTrackR::Move-E- Problem moving the Track "
	<< hit->GetTkId() << " " << nmult << " " << imult << " " << shift
	<< endl;
      //  }
  }
  if (fit_flags != 0 && ParExists(fit_flags)) FitT(fit_flags);
  else ReFit();
}

void TrTrackR::FillExRes(int idsel)
{
  map<int, TrTrackPar>::iterator it = _TrackPar.begin();
  for(;it!=_TrackPar.end();it++) {
    int id = it->first;
    if (id & (kFitLayer8 | kFitLayer9)) continue;
    if (idsel > 0 && id != idsel) continue;

    for (int ily = 8; ily <= 9; ily++) {
      TrRecHitR *hit = GetHitLO(ily);
      if(!hit)continue;
      if(id&kAlignV6){
        AMSPlaneM plm=GetHitCooLJN(hit->GetLayerJ());
        double rigidity;
        double length=Interpolate(plm,rigidity,id);
        TVector3 res=(length<=-1000)?TVector3(999,999,999):plm.getMPdist(1);
        for(int ixy=0;ixy<2;ixy++)GetPar(id).Residual[ily-1][ixy]=res[ixy];
      }
      else {
        AMSPoint pint = InterpolateLayerO(ily, id);
        GetPar(id).Residual[ily-1][0] = (hit->GetCoord()-pint)[0];
        GetPar(id).Residual[ily-1][1] = (hit->GetCoord()-pint)[1];
      }
    }
  }
}

void TrTrackR::EstimateDummyX(int fitid)
{
  if(fitid==0)fitid=trdefaultfit;
  for (int i = 0; i < _Nhits; i++) {
    TrRecHitR *hit = GetHit(i);
    if (!hit || !hit->OnlyY()) continue;
    if(fitid&kAlignV6){//V6 version
      TkLadderN *ladder=hit->GetLadder();
      AMSPlane pl=(*ladder);
      double rigidity;
      double length=Interpolate(pl,rigidity,fitid);//Interpolate to the ladder
      if(length<=-1000)continue;//should set some error
      int mult;
      double dist,xaddress;
      AMSPlaneM plm0=hit->FindMult(dist,mult,xaddress,pl.getP(),pl.getD(),-2);//position and direction in ladder, fast search
      length=Interpolate(plm0,rigidity,fitid);//Interpolate to sensor with more precise position and direction
      if(length<=-1000)continue;//should set some error
      AMSPlaneM plm=hit->FindMult(dist,mult,xaddress,plm0.getP(),plm0.getD(),-2);//position and direction in sensor, fast search
      hit->SetResolvedMultiplicity(mult);
      hit->SetDummyX(xaddress-640);
    }
    else {
      int tkid = hit->GetTkId();
      int ily  = hit->GetLayer();
      AMSPoint gcoo = InterpolateLayerO(ily, fitid);
      TkSens tks(tkid, gcoo, 0);

      if (tks.LadFound()) {
       float dy = TkDBc::Head->_ssize_active[1]/2;
       float ly = TkCoo::GetLadderCenterY(tkid);

       if (tks.GetStripX() < 0) {
 	float sx = tks.GetSensCoo().x();
	float sg = TkDBc::Head->FindTkId(tks.GetLadTkID())->GetRotMat().GetEl(0, 0);
        float sp = TkDBc::Head->FindTkId(tks.GetLadTkID())->GetPlane()->GetRotMat().GetEl(0, 0); 
	if (sx < 0) gcoo[0] -= sg*sp*(sx-1e-3);//bug fixed by QY
	if (sx > TkDBc::Head->_ssize_active[0])
	  gcoo[0] -= sg*sp*(sx-TkDBc::Head->_ssize_active[0]+1e-3);//bug fixed by QY
       }
       if (fabs(gcoo.y()-ly) > dy) gcoo[1] = ly;
       tks.SetGlobalCoo(gcoo);
      }

      if (!tks.LadFound() || tks.GetLadTkID() != tkid) {
       static int nerr = 0;
       if (nerr++ < 0) {//100) {
 	float dy = TkDBc::Head->_ssize_active[1]/2;
	float dx = TkCoo::GetLadderLength (tkid)/2;
	float lx = TkCoo::GetLadderCenterX(tkid);
	float ly = TkCoo::GetLadderCenterY(tkid);
	cerr << "TrTrackR::EstimateDummyX-W-Problem in X-coo estimation: "
	     << "tkid= " << tkid << " " 
	     << tks.GetLadTkID() << " " << tks.LadFound() << " "
	     << "g=" << gcoo.x() << " " << gcoo.y() << " "
	     << "dx=" << gcoo.x()-lx << " "
	     << "dy=" << gcoo.y()-ly  << " "
	     << "d=" << dx << " " << dy << endl;
      }

      //      _iMult[i] = 0;
       hit->SetDummyX(0);
     }
     else {
       hit->SetDummyX(tks.GetStripX());
       hit->SetResolvedMultiplicity(tks.GetMultIndex());
     }
    //    hit->BuildCoordinates();
   }
  }
}

void TrTrackR::ReFit( const float *err,
		      float mass, float chrg, float beta){
  map<int, TrTrackPar>::iterator it=_TrackPar.begin();
  for(;it!=_TrackPar.end();it++)
    FitT(it->first,-1,1,err,mass,chrg,beta);
  return;
}


double TrTrackR::GetCorrectedRigidity (int id, int version, int iz) const
{
  double rigidity=GetRigidity(id,iz);
  if(rigidity==0)return rigidity;
//  int pgcm=2;//PG+CIEMAT
  int pgcm=0;//inner (current PG external alignment follows better to the inner rigidity-scale time-dependent change)
  int id2 = (id == 0) ? trdefaultfit : id;
  if(id2&kAlignV6)return rigidity;//V6 version, no additional correction is needed
  bool useextnal=(id2 & (kFitLayer8|kFitLayer9|kExternal));
  if(useextnal){
    if     (id2&kExtAverage)pgcm=2;//PG+CIEMAT
    else if(id2&kAltExtAl)  pgcm=1;//CIEMAT
    else                    pgcm=0;//PG
  }
  return GetCorrectedRigidityG(rigidity,version,pgcm);
}

AMSPoint TrTrackR::GetPentry(int id) const
{
  int    ilay = 7;
  double zmax = -999;
  for (int i = 0; i < _Nhits; i++) {
    TrRecHitR *hit = GetHit(i);
    if (hit && TestHitBits(hit->GetLayer(), id) && 
	hit->GetCoord().z() > zmax) {
      zmax = hit->GetCoord().z();
      ilay = hit->GetLayer()-1;
    }
  }

  return InterpolateLayerO(ilay+1, id);
}

AMSDir TrTrackR::GetPdir(int id) const
{
  int    ilay = 7;
  double zmax = -999;
  for (int i = 0; i < _Nhits; i++) {
    TrRecHitR *hit = GetHit(i);
    if (hit && TestHitBits(hit->GetLayer(), id) && 
	hit->GetCoord().z() > zmax) {
      zmax = hit->GetCoord().z();
      ilay = hit->GetLayer()-1;
    }
  }

  AMSPoint pnt;
  AMSDir   dir;
  InterpolateLayerO(ilay+1, pnt, dir, id);
  if (dir.z() < 0) dir = dir*(-1);
  return dir;
}

TrRecHitR *TrTrackR::GetHit(int i) 
{
  if (i < 0 || trconst::maxlay <= i) return 0;
  if (_Hits[i] == 0 && _iHits[i] >= 0) {
    VCon* cont2 = GetVCon()->GetCont("AMSTrRecHit");
    _Hits[i] = (TrRecHitR*)cont2->getelem(_iHits[i]);
    delete cont2;
  }

  return _Hits[i];
}

TrRecHitR *TrTrackR::GetHit(int i) const
{
  if (i < 0 || trconst::maxlay <= i) return 0;
  TrRecHitR *hh = _Hits[i];
  
  if (!hh && _iHits[i] >= 0) {
    VCon* cont2 = GetVCon()->GetCont("AMSTrRecHit");
    hh = (TrRecHitR*)cont2->getelem(_iHits[i]);
    delete cont2;
  }

  return hh;
}


TrRecHitR *TrTrackR::GetHitLJ(int ilayJ) const
{
  for (int i = 0; i < GetNhits(); i++) {
    TrRecHitR *hit = GetHit(i);
    if (hit && hit->GetLayerJ() == ilayJ) return hit;
  }
  return 0;
}



TrRecHitR *TrTrackR::GetHitLO(int ilay) const
{
  for (int i = 0; i < GetNhits(); i++) {
    TrRecHitR *hit = GetHit(i);
    if (hit && hit->GetLayer() == ilay) return hit;
  }
  return 0;
}

TrRecHitR & TrTrackR::TrRecHit(int i)  
{
  if (i < 0 || trconst::maxlay <= i) return *_Hits[0];
  if (_Hits[i] == 0 && _iHits[i] >= 0) {
    VCon* cont2 = GetVCon()->GetCont("AMSTrRecHit");
    _Hits[i] = (TrRecHitR*)cont2->getelem(_iHits[i]);
    delete cont2;
  }

  return (*_Hits[i]);
}

AMSPoint TrTrackR::FitCoo[trconst::maxlay];


AMSPlaneM TrTrackR::GetHitCooLJN(int layJ, int ualign, bool dzshiftondxy, float chrg, float dxdzg, float dydzg, bool updatecls) const{
  TrRecHitR *hit=GetHitLJ(layJ);
  if(!hit){return AMSPlaneM(TVector2(-1,-1),0,-11);}//error
  float dxdz=-2,dydz=-2;
  if(dxdzg!=-2&&dydzg!=-2){
    AMSPlaneM plm0=hit->GetGCoordN(-1,ualign,0,chrg);//no dzshift correction
    plm0.setD(dxdzg,dydzg,1);
    TVector3 dloc=plm0.getDLocal();
    dxdz=(dloc[2]!=0)?dloc[0]/dloc[2]:0;
    dydz=(dloc[2]!=0)?dloc[1]/dloc[2]:0;
  }
  if(updatecls){
    for(int ixy=0;ixy<2;ixy++){
      TrClusterR *cls=(ixy==0)?hit->GetXCluster():hit->GetYCluster();
      if(!cls)continue;
      if(chrg>0)  cls->SetQtrk(chrg);
      if(dxdz!=-2)cls->SetDxDz(dxdz);
      if(dydz!=-2)cls->SetDyDz(dydz);
    }
  }
  return hit->GetGCoordN(-1,ualign,dzshiftondxy,chrg,dxdz,dydz);//update coordinate+dzshift correction with new dxdz and dydz
}


float TrTrackR::FitT(TrFit &_TrFit, int id2, int layer, bool update, const float *err, 
		     float mass, float chrg, float beta, float fixrig, int ualign)
{
  int id=id2;
  if (id2==0) id=trdefaultfit;

  if (id < 0) return -1;

  int idf = id&0xf;
  if (idf == kDummy) {
    cerr << "Warning in TrTrack::Fit Dummy fit is ignored " << endl;
    return -2;
  }

  // Force to use Linear fit if magnet is off
  if (_MagFieldOn==0) id = (id&0xfff0)+kLinear;
  idf = id&0xf;

  // Select fitting method
  int method = TrFit::CHOUTKO;
  if      (idf == kAlcaraz)    method = TrFit::ALCARAZ;
  else if (idf == kChikanian)  method = TrFit::CHIKANIANC;
  else if (idf == kChikanianF) method = TrFit::CHIKANIANF;
  else if (idf ==kGEANE_DAF)   method = TrFit::GEANE_DAF;
  else if (idf ==kGEANE_Kalman)method = TrFit::GEANE_Kalman;
  else if (idf ==kGBLFITTERAMS)method = TrFit::GBLFITTERAMS;
  else if (idf ==kGBLFITTER)   method = TrFit::GBLFITTER;
  else if (idf == kLinear)     method = TrFit::LINEAR;
  else if (idf == kCircle)     method = TrFit::CIRCLE;
  else if (idf == kSimple)     method = TrFit::SIMPLE;


  int mscat=0,eloss=0;
  // Set multiple scattering option 
  if ((id & kMultScat) || idf == kChikanian || idf == kChikanianF) mscat = 1;
  // Set energy loss option
  if ((idf==kGEANE_DAF||idf==kGEANE_Kalman||idf==kChikanianF) && (id&kEnergyLoss)) eloss = 1;


  // One drop fitting
  if (id & kOneDrop) {
    double csymin = 0;
    int    ihmin  = -1;
    for (int i = 0; i < _Nhits; i++) {
      TrRecHitR *hit = GetHit(i);
      if (hit && FitT(_TrFit, idf, hit->GetLayer(), false,err,mass,chrg,beta,0.,ualign) > 0 && 
	  _TrFit.GetChisqY() > 0) {
	if (ihmin < 0 || _TrFit.GetChisqY() < csymin) {
	  ihmin  = i;
	  csymin = _TrFit.GetChisqY();
	}
      }
    }
    if (ihmin < 0) return -4;
    layer = GetHit(ihmin)->GetLayer();
  }

  // Noise drop fitting
  if (id & kNoiseDrop) {
    double snmin = 0;
    int    ihmin = -1;
    for (int i = 0; i < _Nhits; i++) {
      TrRecHitR *hit = GetHit(i);
      if (hit && hit->GetYCluster()) {
	if (ihmin < 0 || hit->GetYCluster()->GetSeedSN() < snmin) {
	  ihmin = i;
	  snmin = hit->GetYCluster()->GetSeedSN();
	}
      }
    }
    if (ihmin < 0) return -5;
    layer = GetHit(ihmin)->GetLayer();
  }

  

  // Sort hits in the ascending order of the layer number
  int idx[trconst::maxlay], nhit = 0;
  int bhit[2] = { 0, 0 };

  for (int i = 0; i < _Nhits && nhit < trconst::maxlay; i++) {
    TrRecHitR *hit = GetHit(i);
    if (!hit || hit->GetLayer() == layer) continue;

    // Inner halves
    if ((id & kUpperHalf) && hit->GetLayer() >= 6) continue;
    if ((id & kLowerHalf) && hit->GetLayer() == 1) continue;

    // For AMS02P
    if (TkDBc::Head->GetSetup() == 3) {
      if (hit->GetLayer() == 8) { 
	bhit[0] = 1;
	if (!(id & kFitLayer8)) continue; 
      }
      if (hit->GetLayer() == 9) {
	bhit[1] = 1;
	if (!(id & kFitLayer9)) continue; 
      }
      int lyr = hit->GetLayer();
      if (lyr == 8) lyr = 0;
      idx[nhit++] = lyr*10+i;
    }
    // For AMS02P

    // For AMS02-Ass1/PreInt with S.C. magnet
    else
      idx[nhit++] = hit->GetLayer()*10+i;
  }

  // AMS02P
  if (TkDBc::Head->GetSetup() == 3) {
    if ((id & kFitLayer8) && !bhit[0]) return -6;
    if ((id & kFitLayer9) && !bhit[1]) return -7;
    if ((id & kExternal)  && !bhit[0] && !bhit[1]) return -17;
  }
  std::sort(idx, &idx[nhit]);

  // External fit
  if (TkDBc::Head->GetSetup() == 3 && (id & kExternal) ){
    int idx2[4];
    if(!bhit[0] || !bhit[1]||nhit<4) return -8;
    idx2[0]=idx[0];
    idx2[1]=idx[1];
    idx2[2]=idx[nhit-2];
    idx2[3]=idx[nhit-1];
    nhit=4;
    for(int kk=0;kk<4;kk++)idx[kk]=idx2[kk];
  }  

  // Selected Pattern fit
  if(id & kPattern){
     int idx2[9];
     int nhit2=0;
     for (int kk=0;kk<nhit;kk++)
       if(CheckLayFit(id,idx[kk]/10)) idx2[nhit2++]=idx[kk];
     for(int kk=0;kk<nhit2;kk++)idx[kk]=idx2[kk];
     nhit=nhit2;
  }


  //  Update External DB alignment
  int cookind=0;
  if( (id & kAltExtAl) ){
    // Set TkPlaneExt to CIEMAT
    // TrExtAlignDB::SetAlKind(1);
    //  TrRecHitR *hit1=GetHitLJ(1);
    //  TrRecHitR *hit9=GetHitLJ(9);
    //  int l1=!hit1?-1:1+hit1->GetSlotSide()*10+hit1->lad()*100;
    //  int l9=!hit9?-1:9+hit9->GetSlotSide()*10+hit9->lad()*100;
    //  rret=UpdateExtLayer(1,l1,l9);  // CIEMAT
    //  UsedCiemat=1;
     //     UpdateCoo=1;
    cookind=2;
  }else if(id & kExtAverage){
    cookind=3;
  }else  if(id & kDisExtAlCorr){
    cookind=5;
  }else  if(id & kMC){
    cookind=6;
  }else  if(id & kAlignV6){//V6 version
    cookind=7;
  }else{
    cookind=0;
  }


  // Set fitting errors
  double errx = (err) ? err[0] : TRFITFFKEY.ErrX;
  double erry = (err) ? err[1] : TRFITFFKEY.ErrY;
  int hitbits = 0;

  if (chrg > 1.5) {
    errx *= TRFITFFKEY.ErcHeX;  // 0.65
    if(TRFITFFKEY.ErcHeY){
     erry *= TRFITFFKEY.ErcHeY;  // 0.50
    }
    else{
     // Realistic Error charge based
     int ic=chrg-0.5;
     if(ic<0)ic=0;
     if(ic>=sizeof(TRFITFFKEY.ErcZY)/sizeof(TRFITFFKEY.ErcZY[0]))ic=sizeof(TRFITFFKEY.ErcZY)/sizeof(TRFITFFKEY.ErcZY[0])-1;
      erry=TRFITFFKEY.ErcZY[ic];
    }
  }

  // Fill hit points
  map<int, AMSPlaneM> plms;
  for(int i = 0; i < _Nhits; i++) {
    TrRecHitR *hit = GetHit(i);
    if (!hit) continue;
    AMSPoint coo;
    AMSPlaneM *plm=0;
    if(cookind==7){//V6 version
      plms[i]=GetHitCooLJN(hit->GetLayerJ(),ualign,1);
      plm=&(plms[i]);
    }
    else {
      coo =  GetHitCooLJ(hit->GetLayerJ()); 
      if((hit->GetLayerJ()==1||hit->GetLayerJ()==9)){
        if(cookind==0) 
          coo = GetHitCooLJ(hit->GetLayerJ(),0);
        else if (cookind==2)
          coo= GetHitCooLJ(hit->GetLayerJ(),1);
        else if (cookind==3)
          coo= (GetHitCooLJ(hit->GetLayerJ(),0)+GetHitCooLJ(hit->GetLayerJ(),1))/2.;
        else if (cookind==5)
	  coo= hit->GetCoord(-1,cookind);
        else if (cookind==6){
	  coo= hit->GetCoord(-1,cookind);
        }
      }
      // printf("cookind %d  Layer %d",cookind,hit->GetLayerJ()); coo.Print();

      // Small Z-correction (charge dependent)
      if(TRFITFFKEY.Zshift >= 0 && TkDBc::Head){
        double dxdz=0,dydz=0;
        int uid=-1;
        if      (ParExists(id)) uid=id;
        else if (trdefaultfit!=0 && ParExists(trdefaultfit&(~kAlignV6))) uid=(trdefaultfit&(~kAlignV6));//V5
        else if (trdefaultfit!=0 && ParExists(trdefaultfit)) uid=trdefaultfit;
        if(uid>0){dxdz = GetThetaXZ(uid); dydz = GetThetaYZ(uid);}
        TkLadder *lad = TkDBc::Head->FindTkId(hit->GetTkId());
        double dxdzl=(lad) ? lad->GetRotMat().GetEl(2, 2)*lad->GetPlane()->GetRotMat().GetEl(2, 2)*dxdz : 0;
        double dydzl=(lad) ? lad->GetRotMat().GetEl(2, 2)*lad->GetPlane()->GetRotMat().GetEl(2, 2)*dydz : 0;
        for(int ixy=0;ixy<2;ixy++)coo[ixy]+=TrClusterR::GetdZShiftondXYG(ixy,int(chrg+0.5),dxdzl,dydzl,2);
      }

      // 2014.05.23 SH
      // Workaround to retune the MC resolution (not activated by default)
      if(TRMCFFKEY.MCtuneDmax > 0 && (TRMCFFKEY.MCtuneDs[0] != 0 ||  TRMCFFKEY.MCtuneDs[1]!=0)){
        AMSPoint cna = hit->GetCoord(-1,5); // No Ext-alignment (nor smearing)
        AMSPoint cdif = coo-cna;

        // Remove ext-smearing
        if(hit->GetLayerJ()==1||hit->GetLayerJ()==9) coo = cna;

        // Apply MCtune without smearing to MC
        AMSPlaneM plmr(TVector2(coo[0],coo[1]),hit->GetTkId());
        plmr.setO(0,0,coo[2]);
        for(int ixy=0;ixy<2;ixy++){
          MCtune(plmr, hit->GetTkId(), TRMCFFKEY.MCtuneDmax, TRMCFFKEY.MCtuneDs[ixy], ixy);//X or Y
          coo[ixy]=(ixy==0)?plmr.getM().X():plmr.getM().Y();
        }

        // Apply ext-smearing here
        if(hit->GetLayerJ()==1||hit->GetLayerJ()==9) coo = coo+cdif;
      }
    }


    float ferx = (hit->OnlyY()) ? 0 : 1;
    float fery = (hit->OnlyX()) ? 0 : 1;

    if (TRCLFFKEY.AllowYonlyTracks) ferx = 100;

    if (((id & kUpperHalf) || 
	 (id & kLowerHalf)) && ferx == 0) ferx = 10;

    float ery = erry;
    float erx = errx;
    if (hit->GetLayerJ() == 1){
      ery = std::sqrt(erry*erry+TRFITFFKEY.ErrYL1*TRFITFFKEY.ErrYL1);
      if( idf == kGEANE_Kalman || idf == kGEANE_DAF || idf == kGBLFITTERAMS || idf == kGBLFITTER) erx= std::sqrt(errx*errx+TRFITFFKEY.ErrXL1*TRFITFFKEY.ErrXL1); 
    }
    if (hit->GetLayerJ() == 9){
      ery = std::sqrt(erry*erry+TRFITFFKEY.ErrYL9*TRFITFFKEY.ErrYL9);
      if( idf == kGEANE_Kalman || idf == kGEANE_DAF || idf == kGBLFITTERAMS || idf == kGBLFITTER) erx= std::sqrt(errx*errx+TRFITFFKEY.ErrXL9*TRFITFFKEY.ErrXL9);
    }
    AMSPoint ferr(ferx*erx,fery*ery,0);
    if(!plm){//creat new plane measurement for o-alignment
      plms[i]=AMSPlaneM(TVector3(0.,0.,coo[2]),TVector3(1,0,0), TVector3(0,1,0),hit->GetLayerJ()-1);//o,u,v,id
      plm=&(plms[i]);
      plm->setM(coo[0],coo[1]);
      plm->calMAlignDerGlobal(1,hit->GetLayerJ()-1);
    }
    for(int ixy=0;ixy<2;ixy++)plm->setMCov(ixy,ixy,ferr[ixy]*ferr[ixy]);//only X and Y err used
  }

  //-----add fitting hits 
  _TrFit.Clear();
  int i1 = 0, i2 = nhit;
  for (int i = i1; i < i2 && i < _Nhits; i++) {
    int j = idx[i]%10;
    TrRecHitR *hit = GetHit(j);
    if (!hit) continue;
    if(plms[j].getMStat()<=-11){//Hit has fatal error
      cerr<<"TrTrackR::iTrTrackPar::errorHitStat="<<plms[j].getMStat()<<" layj="<<hit->GetLayerJ()<<endl;
      return -200;
    }
    _TrFit.Add(plms[j]);
    TVector3 m=plms[j].getMGlobal();
    FitCoo[hit->GetLayerJ()-1].setp(m[0],m[1],m[2]);
    hitbits |= (1 << (hit->GetLayer()-1));
  }


#ifdef __ROOTSHAREDLIBRARY__
  // Use measured BetaHR::GetBeta (if available)
  if (beta > 1 || beta < -1) beta = (mass > 0) ? 0 : GetBeta();
#else
  if (beta > 1 || beta < -1) beta = (mass > 0) ? 0 : 1;
#endif

  // Perform fitting
  float fdone = _TrFit.DoFit(method,mscat,eloss,chrg,mass,beta,fixrig);

  bool done = (fdone >= 0 && _TrFit.GetChisqX() >= 0 && 
	                     _TrFit.GetChisqY() >= 0);


  /// Check if the fit was successful
  if (done && method != TrFit::LINEAR && _TrFit.GetRigidity() == 0)
    done = false;
  float retbad=0;
  if(fdone>1E4) fdone=1E4;
  if (!done) retbad=-90000+fdone;

  // Return if fitting values are not to be over written
  if (!update) retbad= _TrFit.GetChisq();


  if (!update || !done){
  //   /// Restore deafult PG alignment if CIEMAT one was used
  //   if (UpdateCoo) {
  //     // Set TkPlaneExt to PG
  //     TrExtAlignDB::SetAlKind(0);
  //     rret=UpdateExtLayer(0); //PG
  //     for (int ii=0;ii<getnhits () ;ii++)
  // 	if(pTrRecHit(ii)->GetLayer()>7)pTrRecHit(ii)->BuildCoordinate();
  //   }
  return retbad;
   }


  // Interpolate to Z=0
  AMSPoint pnt(0, 0, 0);
  AMSDir   dir(0, 0, 1);
  double pz=0,prig,pleng=0;
  if     (idf == kGEANE_Kalman || idf == kGEANE_DAF)pleng=_TrFit.InterpolateKalman(pz, pnt, dir, prig, 0);
  else if(idf == kGBLFITTERAMS || idf == kGBLFITTER)pleng=_TrFit.InterpolateGbl(pz, pnt, dir);
  else                                              _TrFit.Interpolate(pnt, dir);
  if(pleng<=-1000)return -100;//<=-1000 means interpolate failed


  // Fill fittng parameters
  TrTrackPar &par = _TrackPar[id];
  par.P0  = pnt;
  par.Dir = dir;

  // Fill Fit Status
  par.FitDone  = true;
  par.HitBits  = hitbits;
  par.ChisqX   = _TrFit.GetChisqX();
  par.ChisqY   = _TrFit.GetChisqY();
  par.NdofX    = _TrFit.GetNdofX();
  par.NdofY    = _TrFit.GetNdofY();
  par.Chisq    = _TrFit.GetChisq();
  par.Rigidity = _TrFit.GetRigidity();
  par.ErrRinv  = _TrFit.GetErrRinv();

  // filter the Rigidity to PZ
  if(idf == kGEANE_Kalman || idf == kGEANE_DAF){
    for(int iz = 0; iz < DEF_NTKZRIG; iz++){
      _TrFit.InterpolateKalman(par.RigPZ[iz], pnt, dir, prig, 1);
      par.PZRigidity[iz]=prig;
    }
  }

  for (int i = 0; i < trconst::maxlay; i++){
    par.Residual[i][0] = par.Residual[i][1] = 0;
    par.weight  [i][0] = par.weight  [i][1] = 0;
  }
  // Fill residuals
  for (int i = 0; i < _TrFit.GetNhit(); i++) {
    int j = idx[i]%10;
    TrRecHitR *hit = GetHit(j);

    int il = hit->GetLayer()-1;
    int ires=_TrFit.iHit(il+1);//DoFit will sort hit from z highest to lowest
    if(ires<0)continue;
    par.Residual[il][0] = _TrFit.GetXr(ires);
    par.Residual[il][1] = _TrFit.GetYr(ires);
    par.weight  [il][0] = _TrFit.GetXs(ires);
    par.weight  [il][1] = _TrFit.GetYs(ires);
  }

  //--fill residual for non-used hits
  for (map<int, AMSPlaneM>::iterator it = plms.begin(); it!=plms.end(); ++it){
    TrRecHitR *hit = GetHit(it->first);
    int il = hit->GetLayer()-1;
    if(par.Residual[il][0]!=0||par.Residual[il][1]!=0)continue;
    double length=0;
    if     (idf==kGEANE_Kalman||idf==kGEANE_DAF)length=_TrFit.InterpolateKalman(it->second,prig,0);//Kalman
    else if(idf==kGBLFITTERAMS||idf==kGBLFITTER)length=_TrFit.InterpolateGbl(it->second);//Gbl
    else					length=_TrFit.Interpolate(it->second,prig);//Others
    TVector3 res=(length<=-1000)?TVector3(999,999,999):(it->second).getMPdist(1);//residual in local (999 means interpolate failed)
    for(int ixy=0;ixy<2;ixy++)par.Residual[il][ixy]=res[ixy];
  }


  par.Bcorr=1;
  par.BcorrFlag=0;
  MagField * mag=MagField::GetPtr();
  if (mag){
    par.Bcorr=mag->GetScale();
    par.BcorrFlag=mag->GetScaleFlag();
  }
  // /// Restore deafult PG alignment if CIEMAT one was used
  // if(UsedCiemat){
  //   // Set TkPlaneExt to PG
  //   TrExtAlignDB::SetAlKind(0);
  //   //rret=UpdateExtLayer(0); //PG
  //   for (int ii=0;ii<getnhits () ;ii++)
  //     if(pTrRecHit(ii)->GetLayer()>7)pTrRecHit(ii)->BuildCoordinate();
  // }

  return GetChisq(id);
}


float TrTrackR::FitT(int id2, int layer, bool update, const float *err,
                     float mass, float chrg, float beta, float fixrig, int ualign)
{
  TrFit trfit;
  return FitT(trfit, id2, layer, update, err, mass, chrg, beta, fixrig, ualign);
}


void TrTrackR::Print(int opt) {
  cout << _PrepareOutput(opt);
}

bool TrTrackR::CheckLayFit(int fittype,int lay) const{
  if ( lay==0) return ((fittype & kFitLayer8)>0) ;
  if ( lay==1) return ((fittype & kFitLayer1)>0) ; 
  if ( lay==2) return ((fittype & kFitLayer2)>0) ;
  if ( lay==3) return ((fittype & kFitLayer3)>0) ;
  if ( lay==4) return ((fittype & kFitLayer4)>0) ;
  if ( lay==5) return ((fittype & kFitLayer5)>0) ;
  if ( lay==6) return ((fittype & kFitLayer6)>0) ;
  if ( lay==7) return ((fittype & kFitLayer7)>0) ;
  if ( lay==8) return ((fittype & kFitLayer8)>0) ;
  if ( lay==9) return ((fittype & kFitLayer9)>0) ;
  return false;
}

const char *  TrTrackR::Info(int iRef) {
  string aa;
  aa.append(Form("TrTrack #%d ",iRef));
  aa.append(_PrepareOutput(0));
  unsigned int len=MAXINFOSIZE;
  if(aa.size()<len) len=aa.size();
  strncpy(_Info,aa.c_str(),len+1);
  return _Info;
}
std::ostream &TrTrackR::putout(std::ostream &ostr) {
  return ostr << _PrepareOutput(1) << std::endl;
    
}

std::string TrTrackR::_PrepareOutput(int full )
{
  std::string sout;
  
  sout.append(Form("NHits %d (x:%d,y:%d,xy:%d)Pattern: %d  %s,   DefFit: %d, Chi2 %6.3f Pirig %6.3f ",
		   GetNhits(),GetNhitsX(),GetNhitsY(),GetNhitsXY(),GetPattern(),HitBitsString(GetBitPatternJ()),
		   trdefaultfit,GetChisq(trdefaultfit),GetRigidity(trdefaultfit)));
  const TrTrackPar &bb=GetPar();
  bb.Print_stream(sout,full);
  if(!full) return sout;
  map<int, TrTrackPar>::const_iterator it=_TrackPar.begin();
  for(;it!=_TrackPar.end();it++){
    sout.append(Form("\nFit mode 0x%06x ",it->first));
    it->second.Print_stream(sout,full);
  }
  return sout;
}
  



double TrTrackR::Interpolate(const double zpl, AMSPoint &pnt, 
                               AMSDir &dir, int id2) const
{
  int id=id2;
  if (id2==0) id=trdefaultfit;

  if (!FitDone(id)) return -1;

  TrProp tprop(GetP0(id), GetDir(id), GetRigidity(id));
  if (id == kDummy) tprop.SetChrg(0);

  dir.setp(0, 0,   1);
  pnt.setp(0, 0, zpl);
  return tprop.Interpolate(pnt, dir);
}


double TrTrackR::Interpolate(AMSPlane &pl, double &rigidity, int id, float mass, float chrg, int eloss, int direction) const{
  if (id==0) id=trdefaultfit;

  if (!FitDone(id)) return -1000;

  TrProp tprop(GetP0(id), GetDir(id), GetRigidity(id));
  tprop.SetMassChrg(mass,chrg);
  return tprop.Interpolate(pl,rigidity,eloss,direction); 
}


AMSPlaneM TrTrackR::InterpolateLayerJN(double &length, double &rigidity, int layJ, int id, float mass, float chrg, int eloss, int direction) const{
  AMSPlaneM plm=GetHitCooLJN(layJ);
  if(plm.getMStat()>-11){//has valid measurment
    length=Interpolate(plm,rigidity,id,mass,chrg,eloss,direction);
  }
  else {
    TkLayerN *layer=GetLayer(layJ);
    if(layer==0){length=-5000; rigidity=0; return plm;}
    plm.set(layer->getO(),layer->getU(),layer->getV());
    length=Interpolate(plm,rigidity,id,mass,chrg,eloss,direction);//interpolate to layer
    if(length<=-1000)return plm;
    double dist;
    TkSensorN *sensor=layer->FindSensor(dist,plm.getP(),plm.getD(),1);//the closest sensor
    plm.set(sensor->getO(),sensor->getU(),sensor->getV());
    length=Interpolate(plm,rigidity,id,mass,chrg,eloss,direction);//interpolate to sensor
    if(length<=-1000)return plm;
    TkSensorN *sensor1=layer->FindSensor(dist,plm.getP(),plm.getD(),1);//find again the sensor
    if(sensor1!=sensor){//check again the sensor
      plm.set(sensor1->getO(),sensor1->getU(),sensor1->getV());
      length=Interpolate(plm,rigidity,id,mass,chrg,eloss,direction);//interpolate again to sensor
      if(length<=-1000)return plm;
    }
    plm.setid(sensor1->getid());
    plm.setmodule(sensor1);
  }
  return plm;
}


AMSPoint TrTrackR::InterpolateLayerO(int ily, int id) const
{
  AMSPoint pnt;
  AMSDir   dir;
  InterpolateLayerO(ily, pnt, dir, id);
  return pnt;
}

double TrTrackR::InterpolateLayerO(int ily, AMSPoint &pnt, 
                                  AMSDir &dir, int id2) const
{
  int id=id2;
  if (id2==0) id=trdefaultfit;
  if (!FitDone(id)) return -1;

  if(id&kAlignV6){//V6 version
    double rigidity,length;
    AMSPlaneM plm=InterpolateLayerJN(length,rigidity,TkPlaneN::convertlayid(ily-1)+1,id);
    const TVector3& pgob=plm.getP();
    const TVector3& dgob=plm.getD();
    pnt.setp(pgob[0],pgob[1],pgob[2]);
    dir.setd(dgob[0],dgob[1],dgob[2]);
    return length;
  }
  else {
    TrProp tprop(GetP0(id), GetDir(id), GetRigidity(id));
    if (id == kDummy) tprop.SetChrg(0);

    dir.setp(0, 0, 1);

    TrRecHitR *hit = GetHitLO(ily);
    if(hit)
      pnt.setp(0,0,hit->GetCoord()[2]);
    else {
     dir.setp(0, 0, 1);
     pnt.setp(0, 0, TkDBc::Head->GetZlayerA(ily));

     tprop.Interpolate(pnt, dir);
     TkSens tks(pnt,0);
     AMSPoint dd=tks.FindCloseSensorCenter();
     dir.setp(0, 0, 1);
     pnt.setp(0, 0, dd[2]);
    }
    return tprop.Interpolate(pnt, dir);
//  }
  // if (hit) {
//     tkid = hit->GetTkId();

//     AMSPoint pnt = hit->GetLocalCoordinate(hit->GetResolvedMultiplicity());
//     double   ax  = (TkDBc::Head->_ssize_inactive[0]-
// 		    TkDBc::Head->_ssize_active  [0])/2;
//     sens = (int)(abs(pnt.x()+ax)/TkDBc::Head->_SensorPitchK);
//   }
//   else {
//     dir.setp(0, 0, 1);
//     pnt.setp(0, 0, TkDBc::Head->GetZlayer(ily));

//     double ret = tprop.Interpolate(pnt, dir);

//     TkSens tks(pnt,0);
//     if (!tks.LadFound()) return ret;
//     tkid = tks.GetLadTkID();
//     sens = tks.GetSensor();
//   } 

//   TkLadder *lad = TkDBc::Head->FindTkId(tkid);
//   if (!lad) return -1;

//   TkPlane  *pla = lad->GetPlane();
//   AMSRotMat lrm0 = lad->GetRotMatA();
//   AMSRotMat lrm = lrm0*lad->GetRotMat();

//   AMSRotMat prm0 = pla->GetRotMatA();
//   AMSRotMat prm = prm0*pla->GetRotMat();

//   pnt = prm*(lad->GetPos()+lad->GetPosA())+pla->GetPosA()+pla->GetPos();
//   dir = prm*lrm*dir;

//   if (TRCLFFKEY.UseSensorAlign == 1 && 
//       0 <= sens && sens < trconst::maxsen) {
//     pnt[0] -= lad->_sensx[sens];
//     pnt[1] -= lad->_sensy[sens];
//     pnt[2] -= lad->_sensz[sens];
//   }

  // return tprop.Interpolate(pnt, dir);
  }
}

void TrTrackR::Interpolate(const int nz, const double *zpl, 
                             AMSPoint *pvec, AMSDir *dvec, double *lvec,
                             int id2) const
{
  int id=id2;
  if (id2==0) id=trdefaultfit;
  if (!FitDone(id)) return;

  TrProp tprop(GetP0(id), GetDir(id), GetRigidity(id));
  if (id == kDummy) tprop.SetChrg(0);
  tprop.Interpolate(nz, zpl, pvec, dvec, lvec);
}

void TrTrackR::interpolate(AMSPoint pnt, AMSDir dir, AMSPoint &P1, 
			   number &theta, number &phi, number &length, 
			   int id2) const
{
  int id=id2;
  if (id2==0) id=trdefaultfit;
  if (!FitDone(id)) return;
  if(trdefaultfit==kDummy){
    AMSPoint pp=GetP0();
    AMSDir  dd=GetDir();

    P1[0]=pp[0]+dd[0]/dd[2]*pnt[2];
    P1[1]=pp[1]+dd[1]/dd[2]*pnt[2];
    P1[2]=pnt[2];
    theta=dd.gettheta();
    phi=dd.getphi();
    AMSPoint P2=P1-pp;
    length =P2.norm();
    if(pnt[2]>0)length*=-1;
 //    printf(" Z %f P0  %f %f %f \n",pnt[2],pp[0],pp[1],pp[2]);
//     printf("       P1  %f %f %f \n",pnt[2],P1[0],P1[1],P1[2]);
//     printf(" dist %f P2  %f %f %f \n",length,P2[0],P2[1],P2[2]);
    return;
  }

  TrProp tprop(GetP0(id), GetDir(id), GetRigidity(id));
  if (id == kDummy) tprop.SetChrg(0);

  length = tprop.Interpolate(pnt, dir);
  //PZ bugfix lenght must be signed for beta calculation.
  if(pnt[2]>0)length*=-1;
  P1     = pnt;
  theta  = dir.gettheta();
  phi    = dir.getphi();
}

bool TrTrackR::interpolateCyl(AMSPoint pnt, AMSDir dir, number rad, 
                                number idir, AMSPoint &P1, number &theta, 
                                number &phi, number &length, int id2) const
{
  int id=id2;
  if (id2==0) id=trdefaultfit;

  if (!FitDone(id)) return false;

  int sdir = (idir < 0) ? -1 : 1;

  TrProp tprop(GetP0(id), GetDir(id), GetRigidity(id));
  if (id == kDummy) tprop.SetChrg(0);

  if ((length = tprop.InterpolateCyl(pnt, dir, rad, sdir)) < 0) return false;
  P1     = pnt;
  theta  = dir.gettheta();
  phi    = dir.getphi();

  return true;
}

int TrTrackR::intercept(AMSPoint &pnt, int layer, 
			number &theta, number &phi, number &local, int id) const
{

  if(id==0)id=trdefaultfit;
  if(id&kAlignV6){
    pnt.setp(0,0,0);
    theta=phi=0;
    local=999;//outside the sensor
    double length,rigidity;
    AMSPlaneM plm=InterpolateLayerJN(length,rigidity,TkPlaneN::convertlayid(layer)+1);//from layerO to V6
    TkSensorN *sensor=(TkSensorN *)plm.getmodule();
    if(length<=-1000||!sensor)return -2;//interpolation error
    
    double dist=sensor->GetDistance(plm.getP(),plm.getD(),-2,-2,1);
    const TVector3& pgob=plm.getP();
    const TVector3& dgob=plm.getD();
    pnt.setp(pgob[0],pgob[1],pgob[2]);
    AMSDir dir(dgob[0],dgob[1],dgob[2]);
    theta=dir.gettheta();
    phi  =dir.getphi();
    local=dist;//new defination: dist<=0 (>0) means inside (outside) sensor volume with the closest distance=fabs(dist) to the sensor edge
    return (dist<=0)?1:-1;//inside(1) or outside(-1) the sensor volume
  }
  else {
    AMSDir dir;
    Interpolate(TkDBc::Head->GetZlayer(layer+1), pnt, dir, id);
    theta = dir.gettheta();
    phi   = dir.getphi();
    local = 0;

    TkSens tks(pnt,0);
    if (tks.LadFound()) {
      AMSPoint ps(TkDBc::Head->_ssize_active[0], 
		TkDBc::Head->_ssize_active[1], TkDBc::Head->_silicon_z);
      AMSPoint xloc = tks.GetSensCoo();
      xloc = (xloc/ps).abs();
      if (xloc[0] > xloc[1]) local = 1-xloc[0];
      else                   local = 1-xloc[1];
    }
    else return -1;

    return 1;
  }
}

void TrTrackR::getParFastFit(number& Chi2,  number& Rig, number& Err, 
		   number& Theta, number& Phi, AMSPoint& X0) const {
  int id=trdefaultfit;

  if(_MagFieldOn==0) id=kLinear;
  if(trdefaultfit==kDummy) id=kDummy;
  Chi2 = GetChisq(id); Rig = GetRigidity(id); Err = GetErrRinv(id); 
  Theta = GetTheta(id); Phi = GetPhi(id); X0 = GetP0(id);
}

int TrTrackR::DoAdvancedFit(int add_flag, float mass, float chrg, bool primary)
{
 if (!_MagFieldOn) return (int)FitT(kLinear|add_flag);
 
 int kmax=1;
 if (add_flag & (kFitLayer8 | kFitLayer9)){
   kmax=(add_flag&kAlignV6)? 1: TRFITFFKEY.MultipleAlign;
 }

 float DefaultMass_save   = DefaultMass;
 float DefaultCharge_save = DefaultCharge;


 if (mass!=0){
   DefaultMass   = mass;
   DefaultCharge = chrg; 
 }
 else if (DefaultMass > 0.1) {
   DefaultMass   = TrFit::Mproton;
   DefaultCharge = 1;
   if((TRCLFFKEY.ClusterCofGOpt%10)>=2){
     float qtrk = GetInnerQ();
     int ztrk=int(qtrk+0.5);
     if (ztrk>=2) {DefaultMass = TrFit::Mhelium/2*ztrk; DefaultCharge = ztrk;}
   }
   else if (GetQ() > 1.5) {
     DefaultMass   = TrFit::Mhelium;
     DefaultCharge = 2;
   }
 }
 else if(DefaultMass == 0){
#ifdef __ROOTSHAREDLIBRARY__
   // Assign the best estimated charge from measured BetaHR::GetBeta()
    DefaultCharge = GetInnerQ_all(GetBeta()).Mean;
#endif
 }

 advancedfitmass   = DefaultMass;
 advancedfitcharge = DefaultCharge;

 int add_flagplus;
 for (int kk=0;kk<kmax;kk++){
   if     (kk==0)add_flagplus=add_flag;
   else if(kk==1)add_flagplus=add_flag|kAltExtAl;
   else          add_flagplus=add_flag|kExtAverage;
  
   int nfittype  = (mass==0)? DEF_ADVFIT_NUM: DEF_ADVFIT_NUM2;  
   int *fittypes = (mass==0)? DefaultAdvancedFitFlags: DefaultAdvancedFitFlags2;
   for(int ii=0;ii<nfittype;ii++) {
     int fittype=fittypes[ii];

     int fitbit=(kk==0&&primary)? AdvancedFitBits: AdvancedFitBitsExtAll;
     if((fitbit&(1<<ii)) && fittype>0){
       FitT(fittype | add_flagplus);
       if (kk==0 && (AdvancedFitBitsHalf&(1<<ii)) && (add_flagplus==0||add_flagplus==kAlignV6)) {
	 FitT(fittype | add_flagplus | kUpperHalf);
	 FitT(fittype | add_flagplus | kLowerHalf);
       }
     }

   }
 }
 DefaultMass   = DefaultMass_save;
 DefaultCharge = DefaultCharge_save;

 return AdvancedFitDone(add_flag,primary);
}

int TrTrackR::AdvancedFitDone(int add_flag, bool primary) const
{ 
  if (!_MagFieldOn) return FitDone(kLinear|add_flag);
  bool done = true;
  for(int ii=0;ii<DEF_ADVFIT_NUM;ii++) {

    int fitbit=primary? AdvancedFitBits: AdvancedFitBitsExtAll;
    if ((fitbit&(1<<ii)) && DefaultAdvancedFitFlags[ii]>0) {
      done &=FitDone(DefaultAdvancedFitFlags[ii]| add_flag);

/*     if (add_flag == 0) {
       done &= FitDone(DefaultAdvancedFitFlags[ii]| kUpperHalf);
       done &= FitDone(DefaultAdvancedFitFlags[ii]| kLowerHalf);
     }
     if (add_flag == (kFitLayer8 | kFitLayer9)) 
       done &= FitDone(DefaultAdvancedFitFlags[ii]| add_flag| kExternal);*/
    }
  }
  return done;
}

void TrTrackR::Resettrdefaultfit()
{
  int kv6=(trdefaultfit&kAlignV6);
  int algo[4] = { kAlcaraz, kChoutko, kAlcaraz|kMultScat, kChoutko|kMultScat };
  int span[4] = { 0, kFitLayer8, kFitLayer9, kFitLayer8 | kFitLayer9 };
  for (int i = 0; i < 4; i++)
    for (int j = 0; j < 4; j++)
      if (ParExists(algo[j]|span[i]|kv6)) trdefaultfit = algo[j]|span[i]|kv6;
}

char * TrTrackR::GetFitNameFromID(int fitnum){
  static char out[1000];
  out[0]='\0';
  int basefit= fitnum & 0x000F;
  if     (basefit == kDummy       ) strcat(out,"kDummy");
  else if(basefit == kChoutko     ) strcat(out,"kChoutko");
  else if(basefit == kGEANE_DAF   ) strcat(out,"kGEANE_DAF");
  else if(basefit == kGEANE_Kalman) strcat(out,"kGEANE_Kalman");
  else if(basefit == kAlcaraz     ) strcat(out,"kAlcaraz");
  else if(basefit == kChikanian   ) strcat(out,"kChikanian");
  else if(basefit == kChikanianF  ) strcat(out,"kChikanianF");
  else if(basefit == kGBLFITTERAMS) strcat(out,"kGBLFITTERAMS");
  else if(basefit == kGBLFITTER   ) strcat(out,"kGBLFITTER");
  else if(basefit == kVertex      ) strcat(out,"kVertex");
  else if(basefit == kLinear      ) strcat(out,"kLinear");
  else if(basefit == kCircle      ) strcat(out,"kCircle");
  else if(basefit == kSimple      ) strcat(out,"kSimple");
  if(fitnum   & kMultScat    ) strcat(out," | kMultScat");
  if(fitnum   & kUpperHalf   ) strcat(out," | kUpperHalf");
  if(fitnum   & kLowerHalf   ) strcat(out," | kLowerHalf");
  if(fitnum   & kExternal    ) strcat(out," | kExternal");
  if(fitnum   & kOneDrop     ) strcat(out," | kOneDrop");
  if(fitnum   & kNoiseDrop   ) strcat(out," | kNoiseDrop");
  if(fitnum   & kFitLayer8   ) strcat(out," | kFitLayer8");
  if(fitnum   & kFitLayer9   ) strcat(out," | kFitLayer9");
  if(fitnum   & kPattern     ) strcat(out," | kPattern");
  if(fitnum   & kSameWeight  ) strcat(out," | kSameWeight");
  if(fitnum   & kAltExtAl    ) strcat(out," | kAltExtAl");
  if(fitnum   & kExtAverage  ) strcat(out," | kExtAverage");
  if(fitnum   & kDisExtAlCorr) strcat(out," | kDisExtAlCorr");
  if(fitnum   & kMC          ) strcat(out," | kMC");
  if(fitnum   & kEnergyLoss  ) strcat(out," | kEnergyLoss");
  if(fitnum   & kAlignV6     ) strcat(out," | kAlignV6");
  return out;
}

int TrTrackR::GetFitID(int pos){
  if(pos >= int(_TrackPar.size())) return 0;
  int count=0;
  map<int, TrTrackPar>::iterator it;
  
  for(it=_TrackPar.begin();it!=_TrackPar.end();it++)
    if(count++==pos) return it->first;
    
  return 0;

}
void TrTrackR::PrintFitNames() const { 
  map<int, TrTrackPar>::const_iterator it;
  for(it=_TrackPar.begin();it!=_TrackPar.end();it++)
    printf("[0x%04x] %s\n", it->first, GetFitNameFromID(it->first));
}

const TrTrackPar &  TrTrackR::gTrTrackPar(int fitcode) {
   if(ParExists(fitcode)) return  _TrackPar[fitcode];
    //  throw an exception
    char tbt[255];
    sprintf(tbt,"TrTrackPar-E-NotFound %d", fitcode);
    throw string(tbt); 


}


int  TrTrackR::iTrTrackPar(TrFit &_TrFit, int algo, int pattern, int refit, float mass, float  chrg, float beta, float fixrig){
  int type=algo%10;//algo<10 all correction
  bool nomscat=((algo/10)==1);//no MS but has energy-Loss
  bool noloss=((algo/10)==3);//no energy-Loss but has MS
  bool nomat =((algo/10)==4);//no material effect (no energy-Loss and no MS)
  if(nomat)noloss=nomscat=1;
  int fittype=0;

  int DisableFlag=(refit/100)%10;
  if((refit/10)%10<=2)refit=refit%100;

  if ((refit >13&&refit<20 && refit!=16)|| (refit>23&&refit<30) || (((refit/10)%10)>=4) ||refit <0||(refit>6&&refit<10)) return -1;

  // Load alignment in case of explicit refit
//   if (refit==4) { ret1 = UpdateExtLayer(0); refit =  3; }
//   if (refit==5) { ret1 = UpdateExtLayer(1); refit = 13; }
//   if (ret1 !=0) return -5;
  if (refit==4) {  refit =  3; }
  if (refit==5) {  refit = 13; }
  if (refit==6 || refit==16) {  fittype|=kMC; }

  //-------
  int ualign=(refit>=30&&refit<=33)?111:(refit/1000);//ualign=111(30-33) ualign=0(34) ualign=11(11034)
  int AlignMethod=(refit/10)%10;
  int refitc=refit%10;
  bool primary= AlignMethod!=0;

  switch (type){
  case 0 :
    fittype|=(trdefaultfit&0xf);
    break;
  case 1 :
    fittype|=kChoutko;
    break;
  case 2 :
    fittype|=kAlcaraz;
    break;
  case 3 :
    fittype|=kChikanianF;
    break;
  case 4 :
    fittype|=kChikanian;
    break;
  case 5 :
    fittype|=kGEANE_DAF;
    break;
  case 6:
    fittype|=kGEANE_Kalman;
    break;
  case 7:
    fittype|=kGBLFITTERAMS;
    break;
  case 8:
    fittype|=kGBLFITTER;
    break;
    default :
      fittype|=kChoutko;
  }

  
  if(algo==0){
    fittype|=(trdefaultfit&(kMultScat|kEnergyLoss)); 
  }
  else {
    int idf = fittype&0xf;
    if (idf == kChikanian || idf == kChikanianF || !nomscat) fittype|=kMultScat;
    if ((idf==kGEANE_DAF||idf==kGEANE_Kalman||idf==kChikanianF) && !noloss) fittype|=kEnergyLoss;//DAF/Kalman/ChikanianF
  }


  int fflayer[9]={
      kFitLayer1, kFitLayer2,   kFitLayer3, kFitLayer4, kFitLayer5,
      kFitLayer6, kFitLayer7, kFitLayer8, kFitLayer9 };
  int ebpat = _bit_pattern & 0x180;
  int basetype=fittype;
  if(pattern==0){
    // Has1N 
    if ((_bit_pattern & 0x80)>0) fittype|= kFitLayer8;
    // Has9 
    if ((_bit_pattern & 0x100)>0) fittype|= kFitLayer9;
  }
  else if(pattern==1) fittype|= kUpperHalf;
  else if(pattern==2) fittype|= kLowerHalf;
  else if(pattern==3) fittype=basetype;
  else if(pattern==4){ 
    if (ebpat == 0x180) fittype|= kExternal;
    else return -1;
  }
  else if(pattern==5){ 
    if (ebpat  & 0x080) fittype|= kFitLayer8;
      else return -1;
  }
  else if(pattern==6){
    if (ebpat  & 0x100) fittype|= kFitLayer9;
    else return -1;
  }
  else if(pattern==7){ 
    if (ebpat == 0x180) fittype|= kFitLayer8|kFitLayer9;
      else return -1;
  }
  else if(pattern>9){ //it is a base10 hit pattern	
    fittype|=kPattern;
    for(int kk=0;kk<9;kk++){
      if(((pattern/my_int_pow(10,kk))%10)==1){  //OLD scheme
	if(_bit_pattern & (1<<kk))  fittype|=fflayer[kk];
	else
	  return -1;
      }
      else if(((pattern/my_int_pow(10,kk))%10)==9){ // J-Schemw
	int lll=TkDBc::Head->GetLayerFromJ(kk+1)-1;
	if(_bit_pattern & (1<<lll))  fittype|=fflayer[lll];
	else
	  return -1;
      }
    }
    
  }else if (pattern ==9) {
    for (int hh=0;hh<_Nhits;hh++){
      fittype|=kPattern;
      if((_bit_pattern & (1<<hh)) && !(pTrRecHit(_iHits[hh])->OnlyY()) )  fittype|=fflayer[hh];
    }
  }else
    return -1;


  // Disable AlignMethod to avoid mis-use CIEMAT without external layers
  bool useextnal=(fittype & (kFitLayer8|kFitLayer9|kExternal));//use external
  if(useextnal){//use external alignment
    if(AlignMethod<=2 && DisableFlag)fittype|=kDisExtAlCorr;
  }
  else {
    if((AlignMethod ==1||AlignMethod ==2)) AlignMethod = 0;//go back to PG if not use external
  }
  //------
  if( AlignMethod==1) fittype|=kAltExtAl;
  else if( AlignMethod==2) fittype|=kExtAverage;
  else if( AlignMethod==3) fittype|=kAlignV6;//V6 version


  bool FitExists=ParExists(fittype);

  if(refitc>=2 || (!FitExists && refitc==1)) { 
    //    if (refit >= 3 || AlignMethod){
    if(AlignMethod==3){//V6 version
      RecalcHitCoordinates(fittype,refitc==6,primary,chrg,ualign);
    }
    else if (refitc >= 3 ){//V5
      if(AlignMethod>=1){
	TrRecHitR *hit1=GetHitLJ(1);
	TrRecHitR *hit9=GetHitLJ(9);
	int l1=!hit1?-1:1+hit1->GetSlotSide()*10+hit1->lad()*100;
	int l9=!hit9?-1:9+hit9->GetSlotSide()*10+hit9->lad()*100;
	UpdateExtLayer(1,l1,l9);
	if(AlignMethod>1)UpdateExtLayer(0);
      }
      else 
	UpdateExtLayer(0);
	  UpdateInnerDz();
      for (int ii=0;ii<getnhits () ;ii++)
	pTrRecHit(ii)->BuildCoordinate();
      RecalcHitCoordinates(trdefaultfit&(~kAlignV6),refitc==6,primary,chrg,ualign);
    }
    float ret=FitT(_TrFit, fittype,-1,true,0,mass,chrg,beta,fixrig,ualign);
    if (ret>=0) 
      return fittype; 
    else 
      return -3;
  }
  FitExists=ParExists(fittype);
  if(!FitExists && refitc==0) return -2;
  else if(FitExists) return fittype;
  else
    return -4;
}

int  TrTrackR::iTrTrackPar(int algo, int pattern, int refit, float mass, float  chrg, float beta, float fixrig){
  TrFit trfit;
  return iTrTrackPar(trfit, algo, pattern, refit, mass, chrg, beta, fixrig);
}


int TrTrackR::GetResidualKindJ(int ilay, AMSPoint& pnt,int kind, int id){
  pnt.setp(0,0,0);
  //sanity check
  if(!GetHitLJ(ilay)) return -1;
  TrTrackPar aa=GetPar(id);
  if(aa.FitDone==0) return -2;
 

  if(!aa.TestHitLayerJ(ilay)){
    if(kind==2 ||kind==1){
      pnt=GetResidualJ(ilay,id);
      return 1;
    }	
    else
      return -3;
  }
  if(kind==0){
    pnt=GetResidualJ(ilay,id);
    return 0;
  }
  return -4;
    
}



int TrTrackR::Pattern(int input) const {
  int pat=0;
  int p=1;
  for(int k=0;k<maxlay;k++){
    if((input/p)%10)pat+=((_bit_pattern>>k)&1)*p;
    p*=10;
  }
  return pat;
}

int TrTrackR::FixAndUpdate()
{
  // Fix 1 : trdefaultfit to be the longest span
  int kv6=(trdefaultfit&kAlignV6);
  int spans[4] = { kFitLayer8 | kFitLayer9, kFitLayer9, kFitLayer8, 0 };
  int algos[2] = { kChoutko | kMultScat, kAlcaraz | kMultScat };

  int dfitsave = trdefaultfit;
  trdefaultfit = 0;
  for (int i = 0; !trdefaultfit && i < 4; i++)
    for (int j = 0; !trdefaultfit && j < 2; j++) {
      int id = spans[i] | algos[j] | kv6;
      if (ParExists(id)) trdefaultfit = id;
    }

  static int nerr = 0;
  static int nwar = 0;

  if (dfitsave > 0 && trdefaultfit == 0 && nerr++ < 20) 
    cout << "TrTrackR::FixAndUpdate-E-No default fit found: " << dfitsave
	 << endl;
  if (dfitsave != trdefaultfit && nwar++ < 20)
    cout << "TrTrackR::FixAndUpdate-I-Default fit changed from "
	 << dfitsave << " to " << trdefaultfit << endl;

  if (trdefaultfit == 0) return -1;

  // For the backward compatibility
  if (_bit_patternX == 0) {
    UpdateBitPattern();
    return 1;
  }

  // Fix 2 : set _bit_pattern and _bit_patternX with defaultfit
  const TrTrackPar &par = GetPar(trdefaultfit);

  int bpsave  = _bit_pattern;
  int bpsaveX = _bit_patternX;

  _bit_pattern = _bit_patternX = 0;
  for (int i = 0; i < trconst::maxlay; i++) {
    if (par.HitBits & (1<<i)) {
      _bit_pattern  |= 1<<i;
      if (par.weight[i][1] == 0 && nerr++ < 20)
	cout << "TrTrackR::FixAndUpdate-E-HitBits and weight are not "
	     << "consistent: HitBits= " << par.HitBits
	     << " weight[" << i << "][1]= " << par.weight[i][1] << endl;

      if (0 < par.weight[i][0] && par.weight[i][0] < 1) _bit_patternX |= 1<<i;
    }
  }

  if (bpsave != _bit_pattern && nwar++ < 20)
    cout << "TrTrackR::FixAndUpdate-I-Bit pattern changed from "
	 << bpsave << " to " << _bit_pattern << endl;
  if (bpsaveX != _bit_patternX && nwar++ < 20)
    cout << "TrTrackR::FixAndUpdate-I-Bit pattern(X) changed from "
	 << bpsaveX << " to " << _bit_patternX << endl;

  return 0;
}

#ifdef __ROOTSHAREDLIBRARY__
#include "TSpline.h"

int TrTrackR::Enhance(int opt, float qmax, float rmin)
{
  int pat = 0;

  static TSpline3 *spl1 = 0;
  static TSpline3 *spl2 = 0;
  if (!spl1) {
    double x[5] = {  1,  2,  3,  4,  6 };
    double y[5] = { 20, 16, 25, 30, 40 };
    spl1 = new TSpline3("spl1", x, y, 5, "e2");
    cout << "TrTrackR::Enhance-I-TSpline-1 created" << endl;
  }
  if (!spl2) {
    double x[8] = { 0.05, 0.1,  0.2, 0.3, 0.7, 0.8, 0.9, 0.95 };
    double y[8] = { 1.84, 5.01, 0.730, -0.341, -0.307, -1.29, -5.58, -2.58 };
    for (int i = 0; i < 8; i++) y[i] *= 0.01;
    spl2 = new TSpline3("spl2", x, y, 8, "");
    cout << "TrTrackR::Enhance-I-TSpline-2 created" << endl;
  }

  if ((opt&0x300) == 0x300) opt -= 0x300;

  for (int i = 0; i < GetNhits(); i++) {
    TrRecHitR *hit = GetHit(i);
    if (!hit) continue;

    int lay = hit->GetLayer();
    if ((opt&0x100) && lay >= 8) continue;
    if ((opt&0x200) && lay <  8) continue;

    TrClusterR *cl = hit->GetYCluster();
    if (!cl) continue;

    if (opt&1) {
      int is = cl->GetSeedIndex();
      if (cl->GetStatus(is-1) ||
	  cl->GetStatus(is)   ||
	  cl->GetStatus(is+1)) { pat |= (1<<lay); continue; }
    }

    int    nelm = cl->GetNelem();
    double stot = cl->GetTotSignal();
    double eta  = cl->GetCofG(2); if (eta < 0) eta += 1;

    if ((opt&2) && stot > qmax)             { pat |= (1<<lay); continue; }
    if ((opt&4) && stot < spl1->Eval(nelm)) { pat |= (1<<lay); continue; }

    map<int,AMSPoint>::iterator it = _HitCoo.find(lay);
    if (it == _HitCoo.end()) continue;

    TkLadder *lad = TkDBc::Head->FindTkId(cl->GetTkId());
    double     dy = TkDBc::Head->_PitchS*lad->GetRotMat().GetEl(1, 1);
    AMSPoint &coo = it->second;

    if ((opt&8)  && 0.05 < eta && eta < 0.95) coo[1] -= spl2->Eval(eta)*dy;
    if ((opt&16) &&  0.1 < eta && eta < 0.9 && lay < 8) {
      int ita = iTrTrackPar(2, 3, 0);
      int lyj = hit->GetLayerJ();
      TrFit trfit;
      if (FitT(trfit, ita, lyj, false) > 0) {
//	TrFit  *fit = GetTrFit(); fit->Propagate(coo.z());
//        double ares = (GetHitCooLJ(lyj).y()-fit->GetP0y())/dy;
        trfit.Propagate(coo.z());
        double ares = (GetHitCooLJ(lyj).y()-trfit.GetP0y())/dy;
	double rthd = rmin;
	if ((eta < 0.5 && ares > eta*2-0.35) ||
	    (eta > 0.5 && ares < eta*2-1.65)) {
	  if (lyj == 2) rthd *= 1.5;
	  if (lyj == 7 ||
	      lyj == 8) rthd *= 1.3;
	  if (eta < 0.5 && ares >  rthd) coo[1] -= 2*dy*   eta;
	  if (eta < 0.9 && ares < -rthd) coo[1] += 2*dy*(1-eta);
	}
      }
    }
  }

  int nrm = 0;
  for (int i = 0; i < 9; i++)
    if (pat&(1<<(i+1))) { RemoveHitOnLayer(i+1); nrm++; }

  return nrm;
}

#else // __ROOTSHAREDLIBRARY__
int TrTrackR::Enhance(int, float, float) { return 0; }
#endif

int TrTrackR::UpdateBitPattern()
{
  _bit_pattern = _bit_patternX = 0;
  for (int i = 0; i < trconst::maxlay; i++) {
    TrRecHitR *hit = GetHitLO(i+1);
    if (hit)                  _bit_pattern  |= 1<<i;
    if (hit && !hit->OnlyY()) _bit_patternX |= 1<<i;
  }
  return 0;
}

void TrTrackR::RecalcHitCoordinates(int id,bool mc, bool primary, float chrg, int ualign) {
  // check for invalid pointers
  if (!ValidTrRecHitsPointers()) {
    printf("TrTrackR::RecalcHitCoordinates-W invalid pointers on track.\n");
    return;
  }
  // check for valid id
  if (id<0) {
    printf("TrTrackR::RecalcHitCoordinates-W negative fit id requested.\n");
    return;
  }
  if(id==0)id=trdefaultfit;
  if(id&kAlignV6){//V6 version, put cluster with angle and charge
    int uid=-1;
    const int nid=5;//possible fitting algo to get interpolation for the direction
    int fid[nid]={id, (id&(0xf|kMultScat|kEnergyLoss)), trdefaultfit, (kChoutko|kMultScat), kChoutko};//id, Inner-Algo,Inner-MSChoutko, Inner-Choutko
    for(int iv=0;iv<2;iv++){//iv=0 is with kAlignV6, iv=1 is without kAlignV6
      for(int ii=0;ii<nid;ii++){
        if(fid[ii]==0)continue;
        int pid=(iv==0)?(fid[ii]|kAlignV6):(fid[ii]&(~kAlignV6));//with or without V6
        if(ParExists(pid)){uid=pid;break;}
      }
      if(uid>0)break;//found
    }
     
    double length[trconst::maxlay]={0}; 
    AMSPlane pl[trconst::maxlay];
    for(int i=0;i<GetNhits();i++){
      TrRecHitR *hit=GetHit(i);
      TkLadderN *ladder=hit->GetLadder();//precision in ladder is enough
      pl[i]=(*ladder);
      double rigidity;
      length[i]=Interpolate(pl[i],rigidity,uid);//direction in ladder
      TVector3 dloc=pl[i].getDLocal();//get direction in local ladder for possible charge and raw-coo calculation
      if(length[i]<=-1000)dloc.SetXYZ(0.,0.,-1.);//interpolation failed
      for(int ixy=0;ixy<2;ixy++){
        TrClusterR *cls=(ixy==0)?hit->GetXCluster():hit->GetYCluster();
        if(!cls)continue;
        cls->SetDxDz(dloc[0]/dloc[2]);
        cls->SetDyDz(dloc[1]/dloc[2]);
      }
    }

    float qtrk=chrg;//using the direction in local ladder to calculate charge
    if(qtrk<=0)qtrk=GetInnerQ();
    //----set more precise direction in local sensor
    for(int i=0;i<GetNhits();i++){
      TrRecHitR *hit=GetHit(i);
      AMSPlaneM plm=hit->GetGCoordN(-1,ualign,0,qtrk);//sensor plane with no dzshift correction
      if(plm.getMStat()<=-11){//Hit has fatal error
        cerr<<"TrTrackR::RecalcHitCoordinates::errorHitStat="<<plm.getMStat()<<" layj="<<hit->GetLayerJ()<<endl;
      }
      const TVector3& dgob=pl[i].getD();
      plm.setD(dgob[0],dgob[1],dgob[2]);
      TVector3 dloc=plm.getDLocal();//more accurate direction in local sensor
      if(length[i]<=-1000)dloc.SetXYZ(0.,0.,-1.);
      for(int ixy=0;ixy<2;ixy++){
        TrClusterR *cls=(ixy==0)?hit->GetXCluster():hit->GetYCluster();
        if(!cls)continue;
        cls->SetQtrk(qtrk);
        cls->SetDxDz(dloc[0]/dloc[2]);
        cls->SetDyDz(dloc[1]/dloc[2]);
      }
    }
  }
  else {//V5
    // Get interpolated positions/dirs
    double   zhit[trconst::maxlay];
    AMSPoint dpoi[trconst::maxlay];
    AMSDir   dtrk[trconst::maxlay];
    // computation
    for (int i = 0; i < GetNhits(); i++)
      zhit[i] = GetHit(i)->GetCoord().z();
    int uid=trdefaultfit;
    if (ParExists(id)) uid=id;
    else if (trdefaultfit!=0 && ParExists(trdefaultfit&(~kAlignV6))) uid=(trdefaultfit&(~kAlignV6));//V5
    Interpolate(GetNhits(), zhit, dpoi, dtrk, 0, uid);
    // set cluster angle for corrections 
    for (int i = 0; i < GetNhits(); i++) {
      TrRecHitR  *hit = (TrRecHitR*) GetHit(i);
//      if (hit->Shared()) continue; 
      TrClusterR *xcls = (TrClusterR*) hit->GetXCluster();
      TrClusterR *ycls = (TrClusterR*) hit->GetYCluster();
      TkSens sens(hit->GetTkId(),dpoi[i],dtrk[i],0);
      float dxdz = (fabs(sens.GetSensDir().z())>0) ? sens.GetSensDir().x()/sens.GetSensDir().z() : 0;
      float dydz = (fabs(sens.GetSensDir().z())>0) ? sens.GetSensDir().y()/sens.GetSensDir().z() : 0;
      if (xcls) { xcls->SetDxDz(dxdz); xcls->SetDyDz(dydz); }
      if (ycls) { ycls->SetDxDz(dxdz); ycls->SetDyDz(dydz); }
    }
    // store charge in the hits only if linearity correction requests charge
    if (TRCLFFKEY.UseNonLinearity>1||(TRCLFFKEY.ClusterCofGOpt%10)>=2) { 
      // no need to refit many times since charge calculation is independent from linearity correction 
      float qtrk =0; 
      if((TRCLFFKEY.ClusterCofGOpt%10)>=2)qtrk=chrg;//use chrg
      if(qtrk<=0)qtrk=GetInnerQ();
      // set charge into the hits 
      for (int i = 0; i < GetNhits(); i++) {
        TrRecHitR  *hit = (TrRecHitR*) GetHit(i);
//        if (hit->Shared()) continue;
        TrClusterR *xcls = (TrClusterR*) hit->GetXCluster();
        TrClusterR *ycls = (TrClusterR*) hit->GetYCluster();
        if (xcls) { xcls->SetQtrk(qtrk); }
        if (ycls) { ycls->SetQtrk(qtrk); }
      }
    }
    // re-build coordinates
    for (int i = 0; i < GetNhits(); i++) {
      TrRecHitR  *hit = (TrRecHitR*) GetHit(i);
      int ll = hit->GetLayer();
      if (ll == 8 || ll == 9) {
      // TrExtAlignDB::SetAlKind(1);
      // hit->BuildCoordinate();
      // int l1=-1;
      // int l9=-1;
      // if(ll==8)
      // 	l1=!hit?-1:1+hit->GetSlotSide()*10+hit->lad()*100;
      // else
      // 	l9=!hit?-1:9+hit->GetSlotSide()*10+hit->lad()*100;
      // int rret=UpdateExtLayer(1,l1,l9);  // CIEMAT
        _HitCoo[ll+10] = hit->GetCoord(-1,2);
        _HitCoo[ll+20] = hit->GetCoord(-1,3);
      }
      hit->BuildCoordinate();
      _HitCoo[ll] = hit->GetCoord(-1,mc?(primary?-6:6):0);
    }
  }
}


int TrTrackR::RebuildHits(void)
{
#ifdef __ROOTSHAREDLIBRARY__
  int n = 0;
  for (int i = 0; i < trconst::maxlay; i++) if (_Hits[i]) n++;
  if (n > 0) return n;

  typedef map<int,AMSPoint>::const_iterator mapIT;

  _Nhits = 0;

  int id0 = iTrTrackPar(0);
  if (id0 < 0) return -1;

  static int nwar = 0;

  for (int i = 0; i < trconst::maxlay; i++) {
    int layr = i+1;
    int layj = TkDBc::Head->GetJFromLayer(layr);

    mapIT it = _HitCoo.find(layr);
    if (it == _HitCoo.end()) continue;

    AMSPoint hcoo = it->second;

    int tkid = 0;
    for (int slot = -15; slot <= 15; slot++) {
      int   tkidchk = TMath::Sign(layr*100+TMath::Abs(slot), slot);
      AMSPoint diff = hcoo-TkCoo::GetLadderCenter(tkidchk);
      if (fabs(diff[0]) < TkCoo::GetLadderLength(tkidchk)/2 &&
	  fabs(diff[1]) < TkDBc::Head->_ladder_Ypitch/2) {
	tkid = tkidchk;
	break;
      }
    }
    if (tkid == 0) {
      if (nwar++ < 20)
	cout << "TrTrackR::RebuildHits-W-Ladder not found(1): "
	     << layr << " " << hcoo << endl;
      continue;
    }

    TkSens tks(tkid, hcoo, 0);
    if (!tks.LadFound()) {
      if (nwar++ < 20)
	cout << "TrTrackR::RebuildHits-W-Ladder not found(2): "
	     << layr << " " << hcoo << endl;
      continue;
    }

    int mult = tks.GetMultIndex();
    float wx = GetFitWeightXLayerJ(layj, id0);
    // float wy = GetFitWeightYLayerJ(layj, id0);

    int    stx = tks.GetStripX()+640;
    int    sty = tks.GetStripY();
    float  ipx = tks.GetImpactPointX();
    float  ipy = tks.GetImpactPointY();
    int  seedx = (ipx > 0) ? 0 : 1;
    int  seedy = (ipy > 0) ? 0 : 1;
    if (ipx < 0) stx--;
    if (ipy < 0) sty--;

    float sig = 20;
    float adcx[2] = { 0, 0 };
    float adcy[2] = { 0, 0 };
    adcx[0] = (ipx > 0) ? sig*(1-ipx) : -sig*ipx;
    adcx[1] = (ipx < 0) ? sig*(1+ipx) :  sig*ipx;
    adcy[0] = (ipy > 0) ? sig*(1-ipy) : -sig*ipy;
    adcy[1] = (ipy < 0) ? sig*(1+ipy) :  sig*ipy;

    if (stx <   640) stx =  640;
    if (stx >= 1024) stx = 1023;
    if (sty <     0) sty =    0;
    if (sty >=  640) sty =  639;

    TrClusterR  *clx = (wx < 1) ? 
                       new TrClusterR(tkid, 0, stx, 2, seedx, adcx, 0) : 0;
    TrClusterR  *cly = new TrClusterR(tkid, 0, sty, 2, seedy, adcy, 0);
    _iHits[_Nhits]   = -2;
     _Hits[_Nhits++] = new TrRecHitR (tkid, clx, cly, mult, TrRecHitR::REBLT);
  }
  EstimateDummyX(id0);

  TrClusterR::DefaultUsedStrips = 2;
  TrClusterR::DefaultCorrOpt    = 0;
#endif

  return _Nhits;
}

bool TrTrackR::ValidTrRecHitsPointers() {
  bool invalid_pointers = false;
  for (int i = 0; i < GetNhits(); i++) {
    TrRecHitR  *hit = (TrRecHitR*) GetHit(i);
    if (!hit) invalid_pointers = true; 
  }
  return (!invalid_pointers);
}

int TrTrackR::MergeHits(int layer, float dmax, float qmin,
			           float qmax, float beta, int opt)


{
  if (!(opt&2) && TestHitLayerJ(layer)) return 0;

  VCon *cont = 0;

  if (opt&32) cont = GetVCon()->GetCont("AMSTrCluster");
  else        cont = GetVCon()->GetCont("AMSTrRecHit");
  if (!cont) return -1;

  int        mfit = (opt&8) ? DefaultFitID : trdefaultfit;
  AMSPoint   ptrk = InterpolateLayerJ(layer, mfit);
  AMSDir     pdir;  Interpolate(ptrk.z(), ptrk, pdir, mfit);
  TrRecHitR *hmin =  0;
  float      dmin = -1;
  int        mmin = -1;
  float      amin[2] = { 0, 0 };

  float rgt = GetRigidity(mfit);
  int  qopt = TrClusterR::kAsym | TrClusterR::kGain  | TrClusterR::kLoss |
              TrClusterR::kMIP  | TrClusterR::kAngle | TrClusterR::kBeta |
              TrClusterR::kRigidity;

  int nhit = 0;
  TrRecHitR *htmp = 0;
  TrRecHitR *htmn = 0;

  for (int i = 0; i < cont->getnelem(); i++) {
    TrRecHitR *hit = 0;

#ifdef __ROOTSHAREDLIBRARY__
    if (opt&32) {
      TrClusterR *cls = (TrClusterR *)cont->getelem(i);
      if (cls->GetLayerJ() != layer || cls->GetSide() != 1) continue;

      double      pmax = 0;
      TrClusterR *cmax = 0;
      for (int j = 0; j < cont->getnelem(); j++) {
	TrClusterR *cl = (TrClusterR *)cont->getelem(j);
	if (cl->GetTkId() == cls->GetTkId() && cl->GetSide() == 0) {
	  TrRecHitR hh(cls->GetTkId(), cl, cls);
	  double p = hh.GetCorrelationProb();

	  if (p > pmax) {
	    pmax = p;
	    cmax = cl;
	  }
	}
      }
      if (htmp) delete htmp;
      hit = htmp = new TrRecHitR(cls->GetTkId(), cmax, cls);
    }
    else
#endif
      hit = (TrRecHitR *)cont->getelem(i);

    if (!hit || hit->GetLayerJ() != layer) continue;
    if (!(opt&1) && hit->OnlyY()) continue;

    TrClusterR *xcls = hit->GetXCluster();
    TrClusterR *ycls = hit->GetYCluster();

    TkSens sens(hit->GetTkId(), ptrk, pdir, 0);
    AMSDir sdir = sens.GetSensDir();
    float  dxdz = (sdir.z() != 0) ? sdir.x()/sdir.z() : 0;
    float  dydz = (sdir.z() != 0) ? sdir.y()/sdir.z() : 0;

    // Save the angles in clusters
    float dds[4] = { (xcls) ? xcls->GetDxDz() : 0,
		     (xcls) ? xcls->GetDyDz() : 0,
		     (ycls) ? ycls->GetDxDz() : 0,
		     (ycls) ? ycls->GetDyDz() : 0 };

    if (xcls && xcls->GetDxDz() == 0) xcls->SetDxDz(dxdz);
    if (xcls && xcls->GetDyDz() == 0) xcls->SetDyDz(dydz);
    if (ycls && ycls->GetDxDz() == 0) ycls->SetDxDz(dxdz);
    if (ycls && ycls->GetDyDz() == 0) ycls->SetDyDz(dydz);

    float qhit = std::sqrt(hit->GetSignalCombination(2, qopt, beta, rgt));

    // Restore the angles in clusters
    if (xcls) xcls->SetDxDz(dds[0]);
    if (xcls) xcls->SetDyDz(dds[1]);
    if (ycls) ycls->SetDxDz(dds[2]);
    if (ycls) ycls->SetDyDz(dds[3]);

    if (qmin > 0 && qhit < qmin) continue;
    if (qmax > 0 && qhit > qmax) continue;

    int    mult = -1;
    AMSPoint dd = hit->HitPointDist(ptrk, mult);
    float  dist = std::sqrt(dd.x()*dd.x()+dd.y()*dd.y());
    if (dist > dmax) continue;

    nhit++;
    if (opt&4) {
      TrTrackR tfit = *this;
      tfit.AddHit(hit, mult);
      dist = tfit.FitT(0, -1, false);
    }

    if (hit->OnlyY()) mult = -1;
    if (mmin > 0 && mult < 0) continue;

    if (!hmin || (mmin < 0 && mult > 0) || dist < dmin) {
      if (htmp) {
	if (htmn) delete htmn;
	htmn = htmp;
	htmp = 0;
      }
      hmin = hit;
      dmin = dist;
      mmin = mult;
      amin[0] = dxdz;
      amin[1] = dydz;
    }
  }
  if (htmp) delete htmp;

  if (hmin && hmin == htmn) {
#ifdef __ROOTSHAREDLIBRARY__
    VCon *chit = GetVCon()->GetCont("AMSTrRecHit");
    int ih = chit->getnelem();

    TrRecHitR *hit = new TrRecHitR(*htmn);
    chit->addnext(hit);
    hmin = (TrRecHitR *)chit->getelem(ih);
    delete chit;
#else
    hmin = 0;
#endif
    delete htmn;
  }

  if (hmin) {
    AddHit(hmin, mmin);

    // Update the angles
    TrClusterR *xcls = hmin->GetXCluster();
    TrClusterR *ycls = hmin->GetYCluster();
    if (xcls) xcls->SetDxDz(amin[0]);
    if (xcls) xcls->SetDyDz(amin[1]);
    if (ycls) ycls->SetDxDz(amin[0]);
    if (ycls) ycls->SetDyDz(amin[1]);
  }

  delete cont;
  return nhit;
}

int TrTrackR::MergeExtHitsAndRefit(float dmax, const map<int, float> &qmin,
		                               const map<int, float> &qmax,
				   float beta, int opt)
{
  vector<like_t> like;
  int ztrk = GetInnerZ(like, beta);

  map<int, float>::const_iterator imin = qmin.find(ztrk);
  map<int, float>::const_iterator imax = qmax.find(ztrk);

  float qmn = (imin != qmin.end()) ? imin->second : -1;
  float qmx = (imax != qmax.end()) ? imax->second : -1;

  int hext = HasExtLayers();
  int nm1  = MergeHits(1, dmax, qmn, qmx, beta, opt);
  int nm9  = MergeHits(9, dmax, qmn, qmx, beta, opt);

  int mfit1 = kChoutko | TrTrackR::kMultScat;
  int mfit2 = kAlcaraz | TrTrackR::kMultScat;
  if (nm1 > 0) {
    DoAdvancedFit(kFitLayer8);
    if      (FitDone(mfit1|kFitLayer8)) Settrdefaultfit(mfit1|kFitLayer8);
    else if (FitDone(mfit2|kFitLayer8)) Settrdefaultfit(mfit2|kFitLayer8);
  }
  if (nm9 > 0) {
    DoAdvancedFit(kFitLayer9);
    if      (FitDone(mfit1|kFitLayer9)) Settrdefaultfit(mfit1|kFitLayer9);
    else if (FitDone(mfit2|kFitLayer9)) Settrdefaultfit(mfit2|kFitLayer9);
  }
  if ((nm1 > 0 && nm9 > 0) || (nm1 > 0 && (hext&2))
                           || (nm9 > 0 && (hext&1))) {
    int k89 = kFitLayer8|kFitLayer9;
    DoAdvancedFit(k89);
    if      (FitDone(mfit1|k89)) Settrdefaultfit(mfit1|k89);
    else if (FitDone(mfit2|k89)) Settrdefaultfit(mfit2|k89);
  }

  return nm1+nm9;
}

BetaHR *TrTrackR::pBetaH()
{
  return FindBetaH(this);
}

float TrTrackR::GetBeta()
{
  return ::GetBeta(pBetaH());
}

int TrTrackR::DropExtHits(void)
{
  // Workaround to retune the MC scatterng
  int ip = ParExists(DefaultFitID) ? DefaultFitID :
          (ParExists(kChoutko)     ? kChoutko : kAlcaraz);
  if (!ParExists(ip)) return 0;

  int ndrop = 0;

  for (int i = 0; i < 2; i++) {
    TrRecHitR *hit = (i == 0) ? GetHitLJ(1) : GetHitLJ(9);
    if (!hit) continue;

    AMSPoint pnt = InterpolateLayerJ(hit->GetLayerJ(), ip);
    double   rig = GetRigidity(ip);
    double   dy  = hit->GetCoord().y()-pnt.y();

    float limy = TRFITFFKEY.MergeExtLimY;
    float sp0  = 0.02*TRCLFFKEY.MergeExtLimH[1];
    float sp1  = 1;
    float rcor = std::sqrt(sp0*sp0+sp1*sp1/rig/rig);
    float sigy = limy*rcor;
    float dmax = sigy*4;

    if (fabs(dy) > dmax) {
      RemoveHitOnLayer(hit->GetLayer());
      ndrop++;
    }
  }
  return ndrop;
}

int TrTrackR::DefaultChargeVersion = 2;

mean_t TrTrackR::GetQ_all(float beta, int fit_id, float mass_on_z, int version) {
  // ver0: X side only, no kLoss
  if (version==0) 
    return TrCharge::GetMean(
      TrCharge::kAll|TrCharge::kTruncMean|TrCharge::kSqrt,this,TrCharge::kX,beta,-1,
      TrClusterR::kAsym|TrClusterR::kGain|TrClusterR::kMIP|TrClusterR::kAngle|TrClusterR::kBeta|TrClusterR::kOld);
  // ver1: X/Y truncated mean combination (old corrections) 
  else if (version==1)
    return TrCharge::GetCombinedMean(TrCharge::kAll|TrCharge::kTruncMean|TrCharge::kSqrt,this,beta,-1,
      TrClusterR::kAsym|TrClusterR::kGain|TrClusterR::kLoss|TrClusterR::kMIP|TrClusterR::kAngle|TrClusterR::kBeta|TrClusterR::kOld);
  // ver2: X/Y truncated mean combination (new corrections, 2013)   
  else if (version==2) 
    return TrCharge::GetCombinedMean(TrCharge::kAll|TrCharge::kTruncMean|TrCharge::kSqrt,this,beta,-1,
      TrClusterR::kAsym|TrClusterR::kGain|TrClusterR::kAngle|TrClusterR::kLoss|TrClusterR::kBeta|TrClusterR::kRigidity|TrClusterR::kMIP,fit_id,mass_on_z);  
  // ver3: X/Y truncated mean combination (2017 correction scheme for MC)   
  return TrCharge::GetCombinedMean(TrCharge::kAll|TrCharge::kTruncMean|TrCharge::kSqrt,this,beta,-1,
    TrClusterR::kTotSign2017|TrClusterR::kSimAsym|TrClusterR::kSimSignal|TrClusterR::kLoss|TrClusterR::kAngle|TrClusterR::kBeta|TrClusterR::kRigidity,fit_id,mass_on_z);
}


float TrTrackR::GetQ(float beta, int fit_id, float mass_on_z, int version) {
  return GetQ_all(beta, fit_id, mass_on_z, version).Mean;
}


int TrTrackR::GetQ_NPoints(float beta, int fit_id, float mass_on_z, int version) {
  return GetQ_all(beta, fit_id, mass_on_z, version).NPoints;
}


float TrTrackR::GetQ_RMS(float beta, int fit_id, float mass_on_z, int version) {
  return GetQ_all(beta, fit_id, mass_on_z, version).RMS;
}

 
mean_t TrTrackR::GetInnerQ_all(float beta, int fit_id, float mass_on_z, int version) {
  // ver0: X side only, no kLoss
  if (version==0)
    return TrCharge::GetMean(
      TrCharge::kInner|TrCharge::kTruncMean|TrCharge::kSqrt,this,TrCharge::kX,beta,-1,
      TrClusterR::kAsym|TrClusterR::kGain|TrClusterR::kMIP|TrClusterR::kAngle|TrClusterR::kBeta|TrClusterR::kOld);
  // ver1: X/Y truncated mean combination (old corrections) 
  else if (version==1)
    return TrCharge::GetCombinedMean(TrCharge::kInner|TrCharge::kTruncMean|TrCharge::kSqrt,this,beta,-1,
      TrClusterR::kAsym|TrClusterR::kGain|TrClusterR::kLoss|TrClusterR::kMIP|TrClusterR::kAngle|TrClusterR::kBeta|TrClusterR::kOld);
  // ver2: X/Y truncated mean combination (new corrections, 2013) 
  else if (version==2)
    return TrCharge::GetCombinedMean(TrCharge::kInner|TrCharge::kTruncMean|TrCharge::kSqrt,this,beta,-1,
      TrClusterR::kAsym|TrClusterR::kGain|TrClusterR::kAngle|TrClusterR::kLoss|TrClusterR::kBeta|TrClusterR::kRigidity|TrClusterR::kMIP,fit_id,mass_on_z);
  // ver3: X/Y truncated mean combination (2017 correction scheme for MC)   
  return TrCharge::GetCombinedMean(TrCharge::kInner|TrCharge::kTruncMean|TrCharge::kSqrt,this,beta,-1,
    TrClusterR::kTotSign2017|TrClusterR::kSimAsym|TrClusterR::kSimSignal|TrClusterR::kLoss|TrClusterR::kAngle|TrClusterR::kBeta|TrClusterR::kRigidity,fit_id,mass_on_z);
}


float TrTrackR::GetInnerQ(float beta, int fit_id, float mass_on_z, int version) {
  return GetInnerQ_all(beta, fit_id, mass_on_z, version).Mean;
}


int TrTrackR::GetInnerQ_NPoints(float beta, int fit_id, float mass_on_z, int version) {
  return GetInnerQ_all(beta, fit_id, mass_on_z, version).NPoints;
}

  
float TrTrackR::GetInnerQ_RMS(float beta, int fit_id, float mass_on_z, int version) {
  return GetInnerQ_all(beta, fit_id, mass_on_z, version).RMS;
}


TrQYJTrack TrTrackR::GetQYJ_all(int iside, float beta, int fit_id, int qopt){
  float rigidity=(fit_id>=0)?GetRigidity(fit_id):0;
  int mult=-2;
  float dxdz=-2,dydz=-2,qgain=-1;
  TrQYJTrack yjtrack;
  int nc=3;
  for(int ic=0;ic<nc;ic++){//few iteration
    float beta0=(ic<nc-1)?1.0:beta;
    float rigidity0=(ic<nc-1)?0.0:rigidity;
    vector<TrQYJHit> yjhits;
    float qy=0;
    int   qyn=0;//for y side alone
    for(int i=0; i<GetNhits(); i++){
      TrRecHitR* hit=GetHit(i);
      if(!hit)continue;
      int hside=(iside<=2)?iside:2;//X/Y/XY cluster
      TrQYJHit yjhit=hit->GetQYJ_all(hside,beta0,rigidity0,mult,dxdz,dydz,qgain,qopt);
      yjhits.push_back(yjhit);
      if(iside==1){qy+=yjhit.GetCluster(1)->_qs[4];qyn++;}//use totalQ for y
    }
    yjtrack.SetHits(yjhits,0);
    qgain=(ic<nc-1&&yjtrack.InnerQ2[0]>0)?yjtrack.InnerQ2[0]:yjtrack.InnerQ;
    if(iside==1){qy/=qyn;qgain=qy;}
  }
  return yjtrack;
}

float TrTrackR::GetInnerQYJ(float &innerq_rms, int &innerq_patt, int iside, float beta, int fit_id, int qopt){
  TrQYJTrack yjtrack=GetQYJ_all(iside,beta,fit_id,qopt);
  innerq_rms=yjtrack.InnerQRMS;
  innerq_patt=yjtrack.InnerQPatt;
  return yjtrack.InnerQ;
}

TrQYJHit TrTrackR::GetLayerQYJ_all(int jlayer, int iside, float beta, int fit_id, int qopt){
  TrRecHitR* hit=GetHitLJ(jlayer);
  if(!hit) return TrQYJHit();
  float rigidity=(fit_id>=0)?GetRigidity(fit_id):0;
  int mult=-2;
  float dxdz=-2,dydz=-2,qgain=-1;
  return hit->GetQYJ_all(iside,beta,rigidity,mult,dxdz,dydz,qgain,qopt); 
}

float TrTrackR::GetLayerQYJ(int jlayer, int iside, float beta, int fit_id, int qopt){
  return GetLayerQYJ_all(jlayer,iside,beta,fit_id,qopt).Q;
}


float TrTrackR::GetLayerJQ(int jlayer, float beta, int fit_id, float mass_on_z, int version) {
  TrRecHitR* hit = (TrRecHitR*) GetHitLJ(jlayer);
  if (hit==0) return 0;
  // ver0: X side only, no kLoss
  if (version==0) 
    return sqrt(hit->GetSignalCombination(2,TrClusterR::kAsym|TrClusterR::kGain|TrClusterR::kLoss|TrClusterR::kMIP|TrClusterR::kAngle|TrClusterR::kBeta|TrClusterR::kOld,beta)); 
  // ver1: X side, but if no X side available Y side
  else if (version==1) 
    return sqrt(hit->GetSignalCombination(2,TrClusterR::kAsym|TrClusterR::kGain|TrClusterR::kLoss|TrClusterR::kMIP|TrClusterR::kAngle|TrClusterR::kBeta|TrClusterR::kOld,beta));
  // ver2: X side, but if no X side available Y side (new corrections, 2013) 
  else if (version==2) {
    float rigidity = (fit_id>=0) ? GetRigidity(fit_id) : 0; 
    return sqrt(hit->GetSignalCombination(2,TrClusterR::kAsym|TrClusterR::kGain|TrClusterR::kLoss|TrClusterR::kMIP|TrClusterR::kAngle|TrClusterR::kBeta|TrClusterR::kRigidity,beta,rigidity,mass_on_z));
  }
  // ver3: X/Y truncated mean combination (2017 correction scheme for MC)   
  float rigidity = (fit_id>=0) ? GetRigidity(fit_id) : 0;
  return sqrt(hit->GetSignalCombination(2,TrClusterR::kTotSign2017|TrClusterR::kSimAsym|TrClusterR::kSimSignal|TrClusterR::kLoss|TrClusterR::kAngle|TrClusterR::kBeta|TrClusterR::kRigidity,beta,rigidity,mass_on_z));
}


int TrTrackR::GetLayerJQStatus(int jlayer) {
  TrRecHitR* hit = (TrRecHitR*) GetHitLJ(jlayer);
  if (hit==0) return 0;
  return hit->GetQStatus();
}


int TrTrackR::GetZ(vector<like_t>& like, float beta, int fit_id, float mass_on_z) {
  TrCharge trcharge;
  double rigidity = (fit_id>0) ? GetRigidity(fit_id) : 0;
  return trcharge.GetZ(like,this,TrCharge::kAll,2,beta,rigidity,mass_on_z);
}


int TrTrackR::GetInnerZ(vector<like_t>& like, float beta, int fit_id, float mass_on_z) {
  TrCharge trcharge;
  double rigidity = (fit_id>0) ? GetRigidity(fit_id) : 0;
  return trcharge.GetZ(like,this,TrCharge::kInner,2,beta,rigidity,mass_on_z);
}


int TrTrackR::GetLayerJZ(vector<like_t>& like, int jlayer, float beta, int fit_id, float mass_on_z) {
  TrCharge trcharge;
  double rigidity = (fit_id>0) ? GetRigidity(fit_id) : 0;
  return trcharge.GetZ(like,this,100+jlayer,2,beta,rigidity,mass_on_z);
}

mean_t  TrTrackR::GetQH_all(int iside, float beta, int fit_id, int qopt, float Z0){
   if(iside<0||iside>2) return mean_t();
   if(!(trkcharge.ptrk)){
      trkcharge.AssignPointer(this);
   }
   return trkcharge.GetCombQ(iside,beta,fit_id,false,qopt,Z0);
}

mean_t  TrTrackR::GetInnerQH_all(int iside, float beta, int fit_id, int qopt, float Z0){
   if(iside<0||iside>2) return mean_t();
   if(!(trkcharge.ptrk)){
      trkcharge.AssignPointer(this);
   }
   return trkcharge.GetCombQ(iside,beta,fit_id,true,qopt,Z0);
}

float TrTrackR::GetLayerJQH(int jlayer, int iside, float beta, int fit_id, int qopt, float Z0, int res_mult, float dxdz, float dydz){
   if(iside<0||iside>2) return 0;
   if(jlayer<1||jlayer>9) return 0;
   if(!(trkcharge.ptrk)){
      trkcharge.AssignPointer(this);
   }
   if(qopt<-1){
      TrRecHitR* phit=this->GetHitLJ(jlayer);
      if(!phit) return 0;
      double rigidity=(fit_id>=0)?this->GetRigidity(fit_id):0;
      qopt=(TrTrackChargeH::ConvertChargeOpt(2,qopt)|TrClusterChargeLightH::kUnbias);
      return phit->GetQH(iside,beta,rigidity,res_mult,dxdz,dydz,Z0,qopt);
   }
   else{
      return trkcharge.GetQ(jlayer,iside,beta,fit_id,qopt,Z0);
   }
}

float TrTrackR::GetMassLikelihood(int z, int a, float beta)
{
  return TrMass::GetLL(z, a, beta, this);
}

 
float TrTrackR::SimpleChi2X(){
  float err=0.0030;
  int fcode2=iTrTrackPar(1,3,2);
  if(fcode2<0) {
    fcode2=iTrTrackPar(2,3,2);
    if(fcode2<0) {
      printf("Problem\n");
      return 1E10;
    }
  }
  const TrTrackPar& trf=gTrTrackPar(fcode2);
  if(!trf.FitDone){
    printf("Problem2\n");
    return 1E11;
  }
  float x[7],y[7],ex[7],ey[7];

  int NN=0;
  TrRecHitR* thit[7];
  for (int ii=2;ii<9;ii++){
    if(!trf.TestHitLayerJ(ii)) continue;
    TrRecHitR* hh=GetHitLJ(ii);
    if(!hh) continue;
    if(hh->OnlyY()) continue;
    thit[NN]=hh;
    x[NN]=hh->GetCoord()[2];
    y[NN]=hh->GetCoord()[0];
    ex[NN]=0;
    ey[NN]=err;
    NN++;
  }
  if(NN<2) return 1E12;
  TGraphErrors gr(NN,x,y,ex,ey);
  TFitResultPtr pp=gr.Fit("pol1","QS");
  double qx=(*pp).Parameter(0);
  double mx=(*pp).Parameter(1);
  double chi2=0;
  for (int ii=0;ii<NN;ii++){
    double x2=(mx*thit[ii]->GetCoord()[2]+qx);
    double res=thit[ii]->GetCoord()[0]-x2;
    chi2+=pow(res/err,2);
  }

 
  return chi2/NN;

}



int TrTrackR::SetResolvedMultiplicityLJ(int jlay,int mult){

  TrRecHitR* hit=GetHitLJ(jlay);
  if( ! hit) return -1;
  int maxmult=hit->GetMultiplicity();
  if(mult<0 || mult>=maxmult) return -2;
  hit->SetResolvedMultiplicity(mult);

  int ll=hit->GetLayer();
  _HitCoo[ll]=hit->GetCoord();

  return 0;
}



int TrTrackR::UpdateHitCoo(){

  for (int ii=0;ii<getnhits();ii++){
    TrRecHitR* hit=GetHit(ii);
    int ll=hit->GetLayer();
    if(ll==8||ll==9){
      _HitCoo[ll+10]=hit->GetCoord(-1,2);
      _HitCoo[ll+20]=hit->GetCoord(-1,3);
      _HitCoo[ll]=hit->GetCoord(-1,3);
    }else 
      _HitCoo[ll]=hit->GetCoord();
  }
  return 0;
}
