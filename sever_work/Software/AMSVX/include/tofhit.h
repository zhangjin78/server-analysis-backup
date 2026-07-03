//  $Id: tofhit.h,v 1.8 2018/03/04 11:43:34 qyan Exp $

//Author Qi Yan 2012/Jul/19 10:03 qyan@cern.ch
#ifndef __AMSTOFHIT__
#define __AMSTOFHIT__
#include "root.h"
#include "link.h"
#include "Tofdbc.h"
#include "charge.h"
#include "trdrec.h"
#include "Tofrec02_ihep.h"
//////////////////////////////////////////////////////////////////////////
class TOF2RawSide;
class AMSCharge;
class AMSTrTrack;
class AMSTRDTrack;
class AMSEcalShower;
//////////////////////////////////////////////////////////////////////////
class AMSTOFClusterH: public AMSlink,public TofClusterHR {

protected:
    integer  _idsoft;
    TOF2RawSide *_tfraws[2];
public:
    AMSTOFClusterH(){};
    AMSTOFClusterH(uinteger sstatus[2],uinteger status,integer pattern,integer idsoft,number adca[],number adcd[][TOF2GC::PMTSMX],
                   number sdtm[],number timers[],number timer,number etimer,AMSPoint coo,AMSPoint ecoo,
                   number q2pa[],number q2pd[][TOF2GC::PMTSMX],number edepa,number edepd,TOF2RawSide *tfraws[2],TofRawSideR *raws[2]): AMSlink(status),
                   TofClusterHR(sstatus,status,pattern,idsoft,adca,adcd,sdtm,timers,timer,etimer,coo,ecoo,q2pa,q2pd,edepa,edepd,raws)
      {
          for(int i=0;i<2;i++)_tfraws[i]=tfraws[i]; _idsoft=idsoft;
       }
     ~AMSTOFClusterH(){};
     AMSTOFClusterH * next(){ return (AMSTOFClusterH *)_next;}

public:
   void _printEl(ostream &stream){stream <<"AMSTOFClusterH "<<_idsoft<<" "<<_status<<endl;}
   void _writeEl();
   void _copyEl();
   static integer Out(integer);
   integer operator < (AMSlink & o)const{
      return _idsoft < ((AMSTOFClusterH*)(&o)) ->_idsoft;
  }

#ifdef __WRITEROOT__
 friend class TofClusterHR;
 friend class AMSBetaH;
 friend class TofRecH;
#endif
};
/////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
class AMSBetaH: public AMSlink,public BetaHR {

public: 
   AMSTOFClusterH * _phith[4];
   AMSTrTrack *     _ptrack;//trdtrack delete not exis/should be careful
   AMSTRDTrack *    _ptrdtrack;
   AMSEcalShower *  _pecalshower;
   AMSCharge  *     _pcharge;
   number           _sumq;
//---virtual to be the same as BetaHR
#ifndef _PGTRACK_
   TrTrackR         _track;
#endif
   TrdTrackR        _trdtrack;
   EcalShowerR      _ecalshower;
public:
   AMSBetaH(){};
   AMSBetaH(TofClusterHR *phith[4],AMSTrTrack *ptrack,AMSTRDTrack *trdtrack,AMSEcalShower *show,TofBetaPar betapar):
      AMSlink(), BetaHR(), _ptrack(ptrack),_ptrdtrack(trdtrack), _pecalshower(show),_pcharge(0),_sumq(0){
        if((betapar.Status&TOFDBcN::TRDTRACK)==TOFDBcN::TRDTRACK)_ptrack=0;//aleady delete next
        if((betapar.Status&TOFDBcN::ECALTRACK)==TOFDBcN::ECALTRACK)_ptrack=0;
        for(int ilay=0;ilay<4;ilay++){
           _phith[ilay]=dynamic_cast<AMSTOFClusterH *>(phith[ilay]);
          if(_phith[ilay]){
            _phith[ilay]->Status|=TOFDBcN::USED;    _phith[ilay]->Pattern+=1000;
            number lq=_phith[ilay]->GetQSignal(-1); if(lq>0)_sumq+=lq;;
            fTofClusterH.push_back(ilay);           fLayer[ilay]=0;
          }
          else            {fLayer[ilay]=-1;}
        }
        fTrTrack=fTrdTrack=fEcalShower=-1;
        if(_ptrack){
#ifndef _PGTRACK_
         _track =  TrTrackR(_ptrack);
#endif
          fTrTrack=0;
        }
        if(_ptrdtrack)  {_trdtrack=TrdTrackR(_ptrdtrack);       fTrdTrack=0;}
        if(_pecalshower){_ecalshower=EcalShowerR(_pecalshower); fEcalShower=0;}
        BetaPar=betapar;
    }
    ~AMSBetaH(){};
    AMSBetaH *        next(){ return (AMSBetaH *)_next;}
    AMSTrTrack *      gettrack(){return _ptrack;}
    AMSTRDTrack*      gettrdtrack(){return _ptrdtrack;}
    AMSEcalShower*    getecalshower(){return _pecalshower;}
    number            getsumq(){return _sumq;}
    void              settrdtrack(AMSTRDTrack* ptrdtrack){_ptrdtrack=ptrdtrack; _trdtrack=TrdTrackR(_ptrdtrack); fTrdTrack=0;}
    void              setcharge(AMSCharge *amscharge){_pcharge=amscharge;}
    const TofBetaPar& getbetapar(){return BetaPar;}
//---virtual to be the same as BetaHR
    TofClusterHR*     pTofClusterH(unsigned int i){return dynamic_cast<TofClusterHR *>(_phith[fTofClusterH[i]]);}
    TofClusterHR*     GetClusterHL(int ilay)      {return dynamic_cast<TofClusterHR *>(_phith[ilay]);}
#ifdef _PGTRACK_
    TrTrackR*         pTrTrack()                  {return dynamic_cast<TrTrackR *>(_ptrack);}
#else
    TrTrackR*         pTrTrack()                  {return (_ptrack ? &_track: 0);}
#endif
    TrdTrackR*        pTrdTrack()                 {return (_ptrdtrack? &_trdtrack: 0);}
    EcalShowerR*      pEcalShower()               {return (_pecalshower? &_ecalshower: 0);}
public:
   void  MassRec(number rig=0,number charge=0,number evrig=0,int isubetac=0){TofRecH::GetHead()->MassRec(BetaPar,rig,charge,evrig,isubetac);}
   void _printEl(ostream &stream){stream <<"BetaH "<<endl;}
   void _writeEl();
   void _copyEl();
   static integer Out(integer);
   integer operator < (AMSlink & o)const{
     AMSBetaH * bo= (AMSBetaH*)(&o);
     if     ( _ptrack &&  bo->_ptrack)return ( (*_ptrack)< (*(bo->_ptrack)) );
     else if( _ptrack && !bo->_ptrack)return 1;
     else if(!_ptrack &&  bo->_ptrack)return 0;
     else                             return (_sumq > bo->_sumq);//largest sum charge
   }
   friend class TofRecH;
};

/////////////////////////////////////////////////////////////////////////

#endif

