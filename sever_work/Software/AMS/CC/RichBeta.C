#include "RichBeta.h"
#include "RichTools.h"
#include "TFile.h"
#include "root.h"
#include "commons.h"

RichBetaUniformityCorrection* RichBetaUniformityCorrection::_head=0;

bool RichBetaUniformityCorrection::Init(TString file){
  if(_head) delete _head; _head=0;
  if(file==""){
    // Get default
    file=Form("%sRichBetaUniformityCorrection.root",AMSDATADIR.amsdatadir);//the directory has been changed to v6.00
  }

  bool fail=false;
#pragma omp critical  
  if(!_head){
    _head=new RichBetaUniformityCorrection;
    if(_head){
      TFile *currentFile=0;
      if(gDirectory) currentFile=gDirectory->GetFile();
      TFile f(file);
      _head->_agl=(GeomHashEnsemble*)f.Get("BetaAgl");
      _head->_naf=(GeomHashEnsemble*)f.Get("BetaNaF");
      f.Close();
      if(currentFile) currentFile->cd();
      
      if(!_head->_agl || !_head->_naf) fail=true;
    }else fail=true;
  }

  if(fail){if(_head) delete _head;_head=0;return false;}

  return true;
}

float RichBetaUniformityCorrection::getCorrection(float x,float y,float vx,float vy){
  // Determine if it is AGL or NaF
  GeomHashEnsemble *corr=_agl;
  if(RichRingR::getTileIndex(x,y)==121) corr=_naf;
  if(_latest[0]==x && _latest[1]==y && _latest[2]==vx && _latest[3]==vy) return 1.0/corr->MeanPeak;
  _latest[0]=x;_latest[1]=y;_latest[2]=vx;_latest[3]=vy;
  corr->Eval(x,y,vx,vy);
  return 1.0/corr->MeanPeak;
}

float RichBetaUniformityCorrection::getCorrection(float *x){
  return getCorrection(x[0],x[1],x[2],x[3]);
}

float RichBetaUniformityCorrection::getCorrection(RichRingR *ring){
  GeomHashEnsemble *corr=_agl;
  if(ring->IsNaF()) corr=_naf;

  float x=ring->AMSTrPars[0];
  float y=ring->AMSTrPars[1];
  float theta=ring->AMSTrPars[3];
  float phi=ring->AMSTrPars[4];

  float vx=sin(theta)*cos(phi);
  float vy=sin(theta)*sin(phi);
  if(cos(theta)>0) {vx*=-1;vy*=-1;} // Keep a coherent definition

  if(_latest[0]==x && _latest[1]==y && _latest[2]==vx && _latest[3]==vy) return 1.0/corr->MeanPeak;
  _latest[0]=x;_latest[1]=y;_latest[2]=vx;_latest[3]=vy;
  corr->Eval(x,y,vx,vy);
  return 1.0/corr->MeanPeak;
}


///////////////////////////////////////

float RichBetaUniformityCorrection::getRms(float x,float y,float vx,float vy){
  // Determine if it is AGL or NaF
  GeomHashEnsemble *corr=_agl;
  if(RichRingR::getTileIndex(x,y)==121) corr=_naf;
  if(_latest[0]==x && _latest[1]==y && _latest[2]==vx && _latest[3]==vy) return corr->MeanRms;
  _latest[0]=x;_latest[1]=y;_latest[2]=vx;_latest[3]=vy;
  corr->Eval(x,y,vx,vy);
  return corr->MeanRms;
}

float RichBetaUniformityCorrection::getRms(float *x){
  return getRms(x[0],x[1],x[2],x[3]);
}

float RichBetaUniformityCorrection::getRms(RichRingR *ring){

  GeomHashEnsemble *corr=_agl;
  if(ring->IsNaF()) corr=_naf;

  float x=ring->AMSTrPars[0];
  float y=ring->AMSTrPars[1];
  float theta=ring->AMSTrPars[3];
  float phi=ring->AMSTrPars[4];

  float vx=sin(theta)*cos(phi);
  float vy=sin(theta)*sin(phi);
  if(cos(theta)>0) {vx*=-1;vy*=-1;} // Keep a coherent definition

  if(_latest[0]==x && _latest[1]==y && _latest[2]==vx && _latest[3]==vy) return corr->MeanRms;
  _latest[0]=x;_latest[1]=y;_latest[2]=vx;_latest[3]=vy;
  corr->Eval(x,y,vx,vy);
  return corr->MeanRms;
}

///////////////////////////////////////

float RichBetaUniformityCorrection::getWidth(float x,float y,float vx,float vy){
  // Determine if it is AGL or NaF
  GeomHashEnsemble *corr=_agl;
  if(RichRingR::getTileIndex(x,y)==121) corr=_naf;
  if(_latest[0]==x && _latest[1]==y && _latest[2]==vx && _latest[3]==vy) return corr->MeanPeakWidth;
  _latest[0]=x;_latest[1]=y;_latest[2]=vx;_latest[3]=vy;
  corr->Eval(x,y,vx,vy);
  return corr->MeanPeakWidth;
}

float RichBetaUniformityCorrection::getWidth(float *x){
  return getWidth(x[0],x[1],x[2],x[3]);
}

float RichBetaUniformityCorrection::getWidth(RichRingR *ring){

  GeomHashEnsemble *corr=_agl;
  if(ring->IsNaF()) corr=_naf;

  float x=ring->AMSTrPars[0];
  float y=ring->AMSTrPars[1];
  float theta=ring->AMSTrPars[3];
  float phi=ring->AMSTrPars[4];

  float vx=sin(theta)*cos(phi);
  float vy=sin(theta)*sin(phi);
  if(cos(theta)>0) {vx*=-1;vy*=-1;} // Keep a coherent definition

  if(_latest[0]==x && _latest[1]==y && _latest[2]==vx && _latest[3]==vy) return corr->MeanPeakWidth;
  _latest[0]=x;_latest[1]=y;_latest[2]=vx;_latest[3]=vy;
  corr->Eval(x,y,vx,vy);
  return corr->MeanPeakWidth;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////// V2 replicates the same interface, but there is a single ensemble ///////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RichBetaUniformityCorrectionV2* RichBetaUniformityCorrectionV2::_head=0;

bool RichBetaUniformityCorrectionV2::Init(TString file,bool isData){
  if(_head) delete _head; _head=0;

  if(file==""){
    if(isData) file=Form("%sRichBetaUniformityCorrectionV2Data.root",AMSDATADIR.amsdatadir);//the directory has been changed to v6.00
    if(!isData) file=Form("%sRichBetaUniformityCorrectionMCV2.root",AMSDATADIR.amsdatadir);//the directory has been changed to v6.00
  }

  cout<<"RichBetaUniformityCorrectionV2::Init -- Loading "<<file<<endl;
  
  bool fail=false;
#pragma omp critical  
  if(!_head){
    _head=new RichBetaUniformityCorrectionV2;
    if(_head){
      TFile *currentFile=0;
      if(gDirectory) currentFile=gDirectory->GetFile();
      TFile f(file);
      _head->_data=(GeomHashEnsemble*)f.Get("GeomHashEnsemble");
      f.Close();
      if(currentFile) currentFile->cd();
      
      if(!_head->_data) fail=true;
    }else fail=true;
  }

  if(fail){if(_head) delete _head;_head=0;return false;}

  return true;
}

float RichBetaUniformityCorrectionV2::getCorrection(float x,float y,float vx,float vy){
  // Determine if it is AGL or NaF
  GeomHashEnsemble *corr=_data;
  if(_latest[0]==x && _latest[1]==y && _latest[2]==vx && _latest[3]==vy) return 1.0/corr->MeanPeak;
  _latest[0]=x;_latest[1]=y;_latest[2]=vx;_latest[3]=vy;
  corr->Eval(x,y,vx,vy);
  return 1.0/corr->MeanPeak;
}

float RichBetaUniformityCorrectionV2::getCorrection(float *x){
  return getCorrection(x[0],x[1],x[2],x[3]);
}

float RichBetaUniformityCorrectionV2::getCorrection(RichRingR *ring){
  GeomHashEnsemble *corr=_data;

  float x=ring->AMSTrPars[0];
  float y=ring->AMSTrPars[1];
  float theta=ring->AMSTrPars[3];
  float phi=ring->AMSTrPars[4];

  float vx=sin(theta)*cos(phi);
  float vy=sin(theta)*sin(phi);
  if(cos(theta)>0) {vx*=-1;vy*=-1;} // Keep a coherent definition

  if(_latest[0]==x && _latest[1]==y && _latest[2]==vx && _latest[3]==vy) return 1.0/corr->MeanPeak;
  _latest[0]=x;_latest[1]=y;_latest[2]=vx;_latest[3]=vy;
  corr->Eval(x,y,vx,vy);
  return 1.0/corr->MeanPeak;
}


///////////////////////////////////////

float RichBetaUniformityCorrectionV2::getRms(float x,float y,float vx,float vy){
  // Determine if it is AGL or NaF
  GeomHashEnsemble *corr=_data;
  if(_latest[0]==x && _latest[1]==y && _latest[2]==vx && _latest[3]==vy) return corr->MeanRms;
  _latest[0]=x;_latest[1]=y;_latest[2]=vx;_latest[3]=vy;
  corr->Eval(x,y,vx,vy);
  return corr->MeanRms;
}

float RichBetaUniformityCorrectionV2::getRms(float *x){
  return getRms(x[0],x[1],x[2],x[3]);
}

float RichBetaUniformityCorrectionV2::getRms(RichRingR *ring){
  GeomHashEnsemble *corr=_data;

  float x=ring->AMSTrPars[0];
  float y=ring->AMSTrPars[1];
  float theta=ring->AMSTrPars[3];
  float phi=ring->AMSTrPars[4];

  float vx=sin(theta)*cos(phi);
  float vy=sin(theta)*sin(phi);
  if(cos(theta)>0) {vx*=-1;vy*=-1;} // Keep a coherent definition

  if(_latest[0]==x && _latest[1]==y && _latest[2]==vx && _latest[3]==vy) return corr->MeanRms;
  _latest[0]=x;_latest[1]=y;_latest[2]=vx;_latest[3]=vy;
  corr->Eval(x,y,vx,vy);
  return corr->MeanRms;
}

///////////////////////////////////////

float RichBetaUniformityCorrectionV2::getWidth(float x,float y,float vx,float vy){
  // Determine if it is AGL or NaF
  GeomHashEnsemble *corr=_data;
  if(_latest[0]==x && _latest[1]==y && _latest[2]==vx && _latest[3]==vy) return corr->MeanPeakWidth;
  _latest[0]=x;_latest[1]=y;_latest[2]=vx;_latest[3]=vy;
  corr->Eval(x,y,vx,vy);
  return corr->MeanPeakWidth;
}

float RichBetaUniformityCorrectionV2::getWidth(float *x){
  return getWidth(x[0],x[1],x[2],x[3]);
}

float RichBetaUniformityCorrectionV2::getWidth(RichRingR *ring){

  GeomHashEnsemble *corr=_data;

  float x=ring->AMSTrPars[0];
  float y=ring->AMSTrPars[1];
  float theta=ring->AMSTrPars[3];
  float phi=ring->AMSTrPars[4];

  float vx=sin(theta)*cos(phi);
  float vy=sin(theta)*sin(phi);
  if(cos(theta)>0) {vx*=-1;vy*=-1;} // Keep a coherent definition

  if(_latest[0]==x && _latest[1]==y && _latest[2]==vx && _latest[3]==vy) return corr->MeanPeakWidth;
  _latest[0]=x;_latest[1]=y;_latest[2]=vx;_latest[3]=vy;
  corr->Eval(x,y,vx,vy);
  return corr->MeanPeakWidth;
}


////////////////////////////////////////////////////////

GAMModel RichBetaAdditionalCorrections::model[2][2];
RichBetaAdditionalCorrections *RichBetaAdditionalCorrections::head=nullptr;

RichBetaAdditionalCorrections* RichBetaAdditionalCorrections::Init(TString filename_data,TString filename_mc){

  if(head!=0){
    std::cerr<<"RichBetaAdditionalCorrections::init -- already called."<<std::endl;
    return head;
  }


  //////////////// DATA CORRECTION /////////////////
  {
    TFile fin(filename_data);
    GAMModel *m=(GAMModel*)fin.Get("model_z_agl");
    if(!m){
      std::cerr<<"RichBetaAdditionalCorrections::init -- File "<<filename_data<<" does not contains model_z_agl"<<std::endl;
      return nullptr;
    }
    model[0][0]=*m;
    
    m=(GAMModel*)fin.Get("model_z_naf");
    if(!m){
      std::cerr<<"RichBetaAdditionalCorrections::init -- File "<<filename_data<<" does not contains model_z_naf"<<std::endl;
      return nullptr;
    }
    model[0][1]=*m;
    
  }


  ///////////////// MC CORRECTION /////////////////
  {
    TFile fin(filename_mc);
    GAMModel *m=(GAMModel*)fin.Get("model_z_agl");
    if(!m){
      std::cerr<<"RichBetaAdditionalCorrections::init -- File "<<filename_mc<<" does not contains model_z_agl"<<std::endl;
      return nullptr;
    }
    model[1][0]=*m;
    
    m=(GAMModel*)fin.Get("model_z_naf");
    if(!m){
      std::cerr<<"RichBetaAdditionalCorrections::init -- File "<<filename_mc<<" does not contains model_z_naf"<<std::endl;
      return nullptr;
    }
    model[1][1]=*m;
  }
  

    head=new RichBetaAdditionalCorrections;
    return head;
}


  
float RichBetaAdditionalCorrections::corrected_beta(float beta,
						RichBetaAdditionalCorrections::Rad radiator, // 0= AGL
						float run,
						float charge, 
						float x_rad, 
						float y_rad,
						float theta_rad,
						float phi_rad,
						float nreflected,
						float nhits,
						int mc      
						){


  float vx=sin(theta_rad)*cos(phi_rad);
  float vy=sin(theta_rad)*sin(phi_rad);

  // The arguments for GAM
  float args[]={
    nreflected/nhits,                       // Fraction of reflected
    static_cast<float>(run-1.30845e+09),    // Run number (for the time corretion)
    charge                                  // The charge
  };

  // SELECTION: THE CORRECTION DOES NOT APPLY FOR THESE
  if(min(fabs(x_rad),fabs(y_rad))>=40.5) return 0;
  if(x_rad*x_rad+y_rad*y_rad>58.5*58.5) return 0;
  if(max(fabs(x_rad),fabs(y_rad))>28.5 && max(fabs(x_rad),fabs(y_rad))<29.5) return 0;

  if(mc==1) args[1]=0;
  double k=model[mc][radiator].eval(args);
  
  if(k==0){static bool first=true; if(first) cout<<"RichBetaAdditionalCorrections::corrected_beta model returned no value for "<<" "<<args[0]<<" "<<args[1]<<" "<<args[2]<<endl; first=false;}
  if(!k) return 0;

  return beta*model[mc][radiator].get_index_correction(x_rad,y_rad)/k;
}

float RichBetaAdditionalCorrections::getCorrection(RichRingR *ring,float run,int isMc,float z=0){

  float x=ring->AMSTrPars[0];
  float y=ring->AMSTrPars[1];
  float theta=ring->AMSTrPars[3];
  float phi=ring->AMSTrPars[4];
  
  float vx=sin(theta)*cos(phi);
  float vy=sin(theta)*sin(phi);
  if(cos(theta)>0) {vx*=-1;vy*=-1;} // Keep a coherent definition

  //  float z=sqrt(ring->getCharge2Estimate()); 
  //  if(ring->pTrTrack() && ring->pTrTrack()->GetQ_all().Mean>0) z=ring->pTrTrack()->GetQ_all().Mean;
  float rms;
  int inner_patt;
  if(z==0 && ring->pTrTrack()) z=ring->pTrTrack()->GetInnerQYJ(rms,inner_patt);
  if(z==0) z=sqrt(ring->getCharge2Estimate()); 
  z=int(z+0.5);
  
  // Do not over correct
  if(z>9) z=9;

  
  return RichBetaAdditionalCorrections::corrected_beta(1,
						       ring->IsNaF()?RichBetaAdditionalCorrections::NAF:RichBetaAdditionalCorrections::AGL,
						       run,
						       z, 
						       x,y,
						       theta,phi,
						       ring->getReflectedHits(),
						       ring->getHits(),
						       isMc
						       );
}
