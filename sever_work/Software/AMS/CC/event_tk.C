//  $Id: event_tk.C,v 1.68 2025/02/27 16:03:32 nbelyaev Exp $
#include "TrRecon.h"
#include "TrSim.h"
#include "TkSens.h"
#include "TrExtAlignDB.h"
#include "TrInnerDzDB.h"
#include "TkGeomN.h"

void AMSEvent::_sitkinitevent(){
  AMSEvent::gethead()->add(new AMSContainer(AMSID("AMSContainer:AMSTrMCCluster",0),0));
}
 
void AMSEvent::_retkinitevent(){
    AMSEvent::gethead()->add(new AMSContainer(AMSID("AMSContainer:AMSTrRawCluster",0),0));
  AMSEvent::gethead()->add(new AMSContainer(AMSID("AMSContainer:AMSTrCluster",0),0));
  AMSEvent::gethead()->add(new AMSContainer(AMSID("AMSContainer:AMSTrRecHit",0),0));
  AMSEvent::gethead()->add(new AMSContainer(AMSID("AMSContainer:AMSTrTrack",0),0));
  AMSEvent::gethead()->add(new AMSContainer(AMSID("AMSContainer:AMSVtx",0),0));
}
 

void AMSEvent::_catkinitevent(){
}

void AMSEvent::_catkevent(){
}

void AMSEvent::_sitkinitrun(){
}

void AMSEvent::_retkinitrun(){
}

void AMSEvent::_retkevent(integer refit){
    
  AMSgObj::BookTimer.start("RETKEVENT");
  TrRecon rec;
  bool usezqrec=(TRCLFFKEY.recflag/1000000%10>=1);
  try{
    TkTrackN::GetHead()->CopyAlignTDVModule(TkTrackN::GetHeadG(),gettime());//V6 version
//-----
    if (TrExtAlignDB::GetHead()->GetSize(8) > 0 &&
	TrExtAlignDB::GetHead()->GetSize(9) > 0 && AMSJob::gethead()->isRealData() )
    {
      if (AMSEventR::MaskV5DB!=3) {
        TrExtAlignDB::GetHead()->UpdateTkDBc(gettime());
        TrExtAlignDB::GetHead()->UpdateTkDBcDyn(getrun(),gettime(),3);
      }
    }
    if(AMSJob::gethead()->isRealData())
      TrInnerDzDB::GetHead()->UpdateTkDBc(gettime());
    // Set TkPlaneExt to PG
    TrExtAlignDB::SetAlKind(0);

    int flag = TRCLFFKEY.recflag;
    if (flag/10000 > 0) flag -= ((flag/10000)%10)*10000;
    trstat = rec.Build(flag, 0, usezqrec? 0: TRCLFFKEY.statflag);
  }
  catch (AMSTrTrackError e){
    cerr<<"AMSEvent::_retkevent-E-"<<e.getmessage()<<endl;
    seterror();
    setmoreerror(3);
  }
 if(usezqrec)  {
    try{
      AMSgObj::BookTimer.start("ZquTrackBuild");
      TrReconQ rq;
      trstat =  rq.BuildTracks(trstat,TRCLFFKEY.statflag);
    }
    catch(std::bad_alloc a){
      cerr<<" AMSEvent::_reaxevent-E-BadALLOC in "<<getrun()<<" "<<getid()<<" vtxph"<<endl;
      throw;
    }
    AMSgObj::BookTimer.stop("ZquTrackBuild");
  }
  
  
  AMSgObj::BookTimer.stop("RETKEVENT");


}

void AMSEvent:: _sitkevent(){
  TrSim::sitkdigi();
}
