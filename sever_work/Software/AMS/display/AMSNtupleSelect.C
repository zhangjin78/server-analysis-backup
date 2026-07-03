
#include "AMSNtupleHelper.h"
#include "root_RVSP.h"
#include "TrRecon.h"
#include "TrReconQ.h"
static AMSNtupleHelper * fgHelper=0;

extern "C" AMSNtupleHelper * gethelper();

void* __dso_handle=0;

class AMSNtupleSelect: public AMSNtupleHelper{
public:
  AMSNtupleSelect(){};
  bool IsGolden(AMSEventR *ev){
    static set <unsigned int> fmap;
    static int a=0;
    static int dupl=0;
    static unsigned int oldev=0;
    //cout <<" q "<<ev<<endl;
  if(ev){
         cout <<" r "<<ev->NParticle() << " "<< ev->nTrTrack() <<" "<<ev->Event()<<endl;
     if(ev->NParticle()  && ev->nTrTrack() ){
TrTrackR *ptr=ev->Particle(0).pTrTrack();
if(ptr){
int ich=ptr->iTrTrackPar(2,3,111034);
if(ich>=0){
cout<<"  Event "<<ev->Event()<<" "<<ptr->GetNormChisqY(ich)<<" "<<ptr->GetRigidity()<<" "<<ptr->GetNormChisqX(ich)<<" "<<endl;
}
}
	     if(fabs(ev->Particle(0).Charge)>14)return true;
	      else return false;
       cout <<" q "<<endl;
        if(ev->Particle(0).iTrTrack()>=0  && fabs(ev->Particle(0).Charge==2 )  && fabs( ev->Particle(0).Momentum)>1){
        cout <<" event "<<ev->Event()<<" "<<TkDBc::Head->GetSetup()<<endl;
          BetaHR *betah=ev->Particle(0).pBetaH();
          if(betah){
             cout<<betah->GetBeta()<<endl;
             for(int il=0;il<4;il++){
               if(!betah->IsBetaUseHL(il)||!betah->TestExistHL(il))continue;
                 double time=betah->GetTime(il);
                 cout << il<<" "<<time<<endl;
             }
            TofBetaPar betapar;
//            betah->BetaReFit(betapar,111);
            betah->BetaReFit(betapar,111);
            cout<<" betah "<<betapar.Beta<<endl;
            betah->BetaReFit(betapar,1101);
            cout<<" betah "<<betapar.Beta<<endl;
            betah->BetaReFit(betapar,1011);
            cout<<" betah "<<betapar.Beta<<endl;
            betah->BetaReFit(betapar,1110);
            cout<<" betah "<<betapar.Beta<<endl;
}
          int i7o=-1;
          int i9o=-1;
          int i7n=7;
          int i9n=9;
          double chi2o=-1;
          double chi2n=-1;
again:           
          unsigned int pat3=0;
             unsigned int pat4=0;
          unsigned int pat6=0;
          unsigned int pat7=0;
         { // Event loop
  cout <<" old rec "<<endl;
//           TRCLFFKEY.UseZQFix =1;  // Activate the fix
//           TrRecon rec;
//           rec.Build(1111, 1, 0);
           TrTrackR *track = ev->pParticle(0)->pTrTrack();
               if(track){
//           cout <<track->Info()<<endl;
             for(int lm=1;lm<10;lm++){
                AMSPoint pntin,pntout,pntdist;
                 AMSDir dirin,dirout;
                int tkid;
                int ret=ev->IsInsideTracker(lm,track->GetP0(),track->GetDir(),track->GetRigidity(),0,tkid,pntout,pntdist);
                 cout <<" git "<<lm<<" "<<ret<<" "<<" "<<pntout<<" "<<pntdist<<endl;
              }
             cout<<" nh "<<track->GetNhitsY()<<" "<<track->GetRigidity()<<endl;
        //    track->ReFit(0, 2.8, fabs(ev->Particle(0).Charge), fabs(ev->Particle(0).Beta));
             i9o=track->GetNhitsY();
             i7o=i9o-track->HasExtLayers()%2-track->HasExtLayers()/2;
             chi2o=track->GetChisq();
             int nh=0;
             for(int l=8;l>1;l--){
              if(track->TestHitLayerJ(l)){
                if(nh<3){
                  pat3+=9*pow(10,l-1);
                }
                if(nh<4){
                  pat4+=9*pow(10,l-1);
                }
                nh++;
             }
            }              
            if(nh<6){
                 cout<<" too bad "<<ev->Event()<<endl;
                 //return false; 
            }
             nh=0;
             for(int l=2;l<9;l++){
              if(track->TestHitLayerJ(l)){
                if(nh<3 ){
                  pat6+=9*pow(10,l-1);
                }
                if(nh<4){
                  pat7+=9*pow(10,l-1);
                }
                nh++;
             }
            }              

            cout <<pat3<<" "<<pat4<<" "<<pat6<<" "<<pat7<<endl;
{
             int ir=track->iTrTrackPar(1,pat3,2,2.8,2,1);
             if(ir>=0)cout<<" refit "<<track->GetRigidity(ir)<<" "<<track->GetChisqY(ir)/track->GetNdofY(ir)<<" "<<pat3<<endl;
             else cout<<" no refit "<<pat3<<endl;
}
{
             int ir=track->iTrTrackPar(3,pat4,1,2.8,2,1);
             if(ir>=0)cout<<" refit 1 "<<track->GetRigidity(ir)<<" "<<track->GetChisqY(ir)/track->GetNdofY(ir)<<" "<<pat4<<endl;
}
{
             int ir=track->iTrTrackPar(2,pat6,2,2.8,2,1);
             if(ir>=0)cout<<" refit 2 "<<track->GetRigidity(ir)<<" "<<track->GetChisqY(ir)/track->GetNdofY(ir)<<" "<<pat6<<endl;
            else cout<<" no refit 2"<<pat6<<endl;
}
{
             int ir=track->iTrTrackPar(1,pat7,1,2.8,2,1);
             if(ir>=0)cout<<" refit 3  "<<track->GetRigidity(ir)<<" "<<track->GetChisqY(ir)/track->GetNdofY(ir)<<" "<<pat7<<endl;
            else cout<<" no refit 3"<<pat7<<endl;
}
{
             int ir=track->iTrTrackPar(2,0,3,3.7,2,1);
             if(ir>=0)cout<<" refit 4  "<<track->GetRigidity(ir)<<" "<<track->GetChisqY(ir)/track->GetNdofY(ir)<<endl;
}
{
             int ir=track->iTrTrackPar(3,0,2,3.7,2,1);
             if(ir>=0)cout<<" refit 5 "<<track->GetRigidity(ir)<<" "<<track->GetChisqY(ir)/track->GetNdofY(ir)<<endl;
}
{
             int ir=track->iTrTrackPar(1,0,2,2.8,2,1);
             if(ir>=0)cout<<" refit 6 "<<track->GetRigidity(ir)<<" "<<track->GetChisqY(ir)/track->GetNdofY(ir)<<endl;
             int ir1=track->iTrTrackPar(3,3,2,2.8,2,1);
             if(ir1>=0)cout<<" refit 7 "<<track->GetRigidity(ir1)<<" "<<track->GetChisqY(ir1)/track->GetNdofY(ir1)<<endl;
             int ir3=track->iTrTrackPar(1,3,2,2.8,2,1);
             if(ir3>=0)cout<<" refit inn "<<track->GetRigidity(ir3)<<" "<<track->GetChisqY(ir3)/track->GetNdofY(ir3)<<" "<<-track->GetErrRinv(ir3)*track->GetRigidity(ir3)<<endl;
}
               int ir1=track->iTrTrackPar(1,1,2,2.8,2,1);
             if(ir1>=0)cout<<" refit 1sth "<<track->GetRigidity(ir1)<<" "<<track->GetChisqY(ir1)/track->GetNdofY(ir1)<<endl;
 ir1=track->iTrTrackPar(1,2,2,2.8,2,1);
             if(ir1>=0)cout<<" refit 2ndh "<<track->GetRigidity(ir1)<<" "<<track->GetChisqY(ir1)/track->GetNdofY(ir1)<<endl;

double rata=100;
double rati=0;
for(int la=0;la<9;la++){
 float rat=ev->GetTrackerRawSignalRatio(la,10,track);
 if(rat && rat<rata)rata=rat;
 if(rat && rat>rati)rati=rat;
}
cout <<"  rata "<<rata<<endl;

           }
           else cout<<" no track"<<endl;
}
cout <<" i7n " << i7n<<" "<<i7o<<" "<<i9n <<" "<<i9o<<" "<<chi2n<<" "<<chi2o<<" "<<ev->Particle(0).Beta<<endl;

if(0 && (i7n> i7o || (i7o==i7n && i9n>i9o ) || (i7o==i7n && i9n==i9o && chi2n<chi2o )) ){ // Event loop
  cout <<" new rec "<<endl;
  // Then you can get TrTrack from the new track finding
  TrTrackR *track = ev->pParticle(0)->pTrTrack();

 if(track){
//     cout <<track->Info()<<endl;
             i9n=track->GetNhitsY();
             i7n=i9n-track->HasExtLayers()%2-track->HasExtLayers()/2;
             chi2n=track->GetChisq();
{
             int ir=track->iTrTrackPar(1,pat3,2,2.8,2,1);
             if(ir>=0)cout<<" refit "<<track->GetRigidity(ir)<<" "<<track->GetChisqY(ir)/track->GetNdofY(ir)<<" "<<pat3<<endl;
             else cout<<" no refit "<<pat3<<endl;
}
{
             int ir=track->iTrTrackPar(3,pat4,1,2.8,2,1);
             if(ir>=0)cout<<" refit 1 "<<track->GetRigidity(ir)<<" "<<track->GetChisqY(ir)/track->GetNdofY(ir)<<" "<<pat4<<endl;
}
{
             int ir=track->iTrTrackPar(2,pat6,2,2.8,2,1);
             if(ir>=0)cout<<" refit 2 "<<track->GetRigidity(ir)<<" "<<track->GetChisqY(ir)/track->GetNdofY(ir)<<" "<<pat6<<endl;
            else cout<<" no refit 2"<<pat6<<endl;
}
{
             int ir=track->iTrTrackPar(1,pat7,1,2.8,2,1);
             if(ir>=0)cout<<" refit 3  "<<track->GetRigidity(ir)<<" "<<track->GetChisqY(ir)/track->GetNdofY(ir)<<" "<<pat7<<endl;
            else cout<<" no refit 3"<<pat7<<endl;
}
{
             int ir=track->iTrTrackPar(1,99909000,2,2.8,2,1);
             //if(ir>=0)cout<<" refit 4  "<<track->GetRigidity(ir)<<" "<<track->GetChisqY(ir)/track->GetNdofY(ir)<<endl;
}
//             if(i7n<i7o || (i7n==i7o && i9n<i9o) || (i7n==i7o && i9n==i9o && chi2o<chi2n))goto again;
//            else cout<<" no track"<<endl;

}
}


     
       
return true;
}

else return false;

}
return false;
}
}
};
//
//  The code below should not be modified
//
#ifndef WIN32
extern "C" void fgSelect(){
  AMSNtupleHelper::fgHelper=new AMSNtupleSelect(); 
  cout <<"  Handle Loadedd "<<endl;
}

#else
#include <windows.h>
BOOL DllMain(HINSTANCE hinstDLL,  // DLL module handle
             DWORD fdwReason,              // reason called 
             LPVOID lpvReserved)           // reserved 
{ 
 
  switch (fdwReason) 
    { 
      // The DLL is loading due to process 
      // initialization or a call to LoadLibrary. 
 
    case DLL_PROCESS_ATTACH: 
      fgHelper=new AMSNtupleSelect();
      cout <<"  Handle Loadedd "<<fgHelper<<endl;
      break;

    case DLL_PROCESS_DETACH: 
      if(fgHelper){
        delete fgHelper;
        fgHelper=0;
      } 
 
      break; 
 
    default: 
      break; 
    } 
  return TRUE;
}
extern "C" AMSNtupleHelper * gethelper(){return fgHelper;}

#endif
