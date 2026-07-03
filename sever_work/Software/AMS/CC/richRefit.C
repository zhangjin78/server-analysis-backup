#include "root.h"
#include "RichCharge.h"
#include "commons.h"

#ifdef _PGTRACK_
#ifndef __ROOTSHAREDLIBRARY__


int RichRingR::FillVector(double &fac,std::vector<int>&hits, double ns, int ver){ return 1; };


RichRingR *RichRingR::ReBuild(TrTrackR *tr){return this;}
RichRingR *RichRingR::ReBuild(){return this;}

RichRingR *RichRingR::Refit(double &fac, double ns, int ver){
  return this;
}
RichRingR *RichRingR::Refit(std::vector<int> &hits){return this;}

RichRingR *RichRingR::Build(TrTrackR *track){return 0;}

double RichRingR::ComputeNpExp(TrTrackR *track,double beta,double Z){return 0;}

int RichRingR::ClusterizeZ1(double dist,int min_size){return -1;}

#else

#include "richrecOff.h"
#include "richidOff.h"
#include "richradidOff.h"
#include "TEnv.h"
#include <iostream>
#include "timeid.h"
#include <algorithm>

using namespace std;

class RefitInitilizer{
public:
  RefitInitilizer(){
    AMSTimeID *tdvdb=0;
    bool isRealData=AMSEventR::Head()->nMCEventg()==0;
    time_t now=AMSEventR::Head()->UTime();
    tm begin;
    tm end;
    
    begin.tm_isdst=0;
    end.tm_isdst=0;
    begin.tm_sec  =0;
    begin.tm_min  =0;
    begin.tm_hour =0;
    begin.tm_mday =0;
    begin.tm_mon  =0;
    begin.tm_year =0;
    
    end.tm_sec=0;
    end.tm_min=0;
    end.tm_hour=0;
    end.tm_mday=0;
    end.tm_mon=0;
    end.tm_year=0;


    //////////////////////////// Initialize all the necessary stuff
    TString filename=Form("%sRichDefaultCalibration.dat",AMSDATADIR.amsdatadir);//the directory has been changed to v6.00
    RichOffline::RichPMTsManager::Init((char*)filename.Data());

    tdvdb=new AMSTimeID(AMSID("RichPMTChannelMask",isRealData),
			begin,end,
			RichOffline::RICmaxpmts*RichOffline::RICnwindows
			*sizeof(RichOffline::RichPMTsManager::_mask[0]),
			(void*)&RichOffline::RichPMTsManager::_mask[0],
			AMSTimeID::Standalone,1);
    if(tdvdb && tdvdb->validate(now)){
      // DO NOTHING, SHOULD BE OK
    }
    if(tdvdb) delete tdvdb;


    filename=Form("%sRichDefaultAGLTables.04.dat",AMSDATADIR.amsdatadir);
    RichOffline::RichRadiatorTileManager::Init((char*)filename.Data());

    //////////////////// AGL TDV /////////////////////////

    geant _optical_parameters[RichOffline::RICmaxtiles*4];

    tdvdb=new AMSTimeID(AMSID("RichRadTilesParameters",isRealData),
			begin,end,
			RichOffline::RICmaxtiles*4*sizeof(_optical_parameters[0]),
			(void*)&_optical_parameters[0],
			AMSTimeID::Standalone,1);
    if(tdvdb && tdvdb->validate(now)){
      using namespace RichOffline;
      const int n_=4;  // Number of stored parameters
      for(int i=0;i<RichRadiatorTileManager::_number_of_rad_tiles;i++){
	RichRadiatorTileManager::_tiles[i]->index=_optical_parameters[i*n_+0];
	RichRadiatorTileManager::_tiles[i]->clarity=_optical_parameters[i*n_+1];
	RichRadiatorTileManager::recompute_tables(i,RichRadiatorTileManager::_tiles[i]->index);
      }
    }

    if(tdvdb) delete tdvdb;

    tdvdb=0;

    /////////////////// MISSING CHANNELS MASKS /////////////////////



    ////////////////// ALIGNMENT TDV //////////////////////
    RichOffline::RichAlignment::Init(false);
    double _align_parameters[12]={0,0,0,0,0,0,0,0,0,0,0,0};

    tdvdb=new AMSTimeID(AMSID("RichAlignmentParameters",1),
				   begin,end,
				   12*sizeof(_align_parameters[0]),
				   (void*)&_align_parameters[0],
				   AMSTimeID::Standalone,1);


    if(tdvdb && tdvdb->validate(now)){
      RichOffline::RichAlignment::Set(_align_parameters[0],_align_parameters[1],_align_parameters[2],
				      _align_parameters[3],_align_parameters[4],_align_parameters[5]);
      
      RichOffline::RichAlignment::SetMirrorShift(_align_parameters[6],_align_parameters[7],_align_parameters[8]);
    }else{
      filename=Form("%sRichAlignmentCosmic.1207904521.2.dat",AMSDATADIR.amsdatadir);
      RichOffline::RichAlignment::LoadFile((char*)filename.Data());
    }
    if(tdvdb) delete tdvdb;
    tdvdb=0;
  }
};


RefitInitilizer *__RefitInitilizer=0; // Initialize at startup

RichRingR *RichRingR::Build(TrTrackR *tr){
#pragma omp critical (ri)
{
  if(__RefitInitilizer==0) __RefitInitilizer=new RefitInitilizer;
}
   if(tr==NULL)return NULL;
  using namespace RichOffline;

  // Apply a simple patch to the obtained beta until I get the proper radiator indexes
  // and the alignment bias. This should be easy
  RichRingR *ring=new RichRingR;
  if(!ring->ReBuild(tr)){
    delete ring;
    return 0;
  }
  std::vector<int> empty;
  ring->Refit(empty);

  //  delete ptr;
  return ring;
}


RichRingR *RichRingR::ReBuild(){
  fBetaHit.clear();
  fRichHit.clear();
  for(int i=0;i<2;i++) fRawBeta[i].clear();
  fUsedBeta.clear();
  fHitBeta.clear();

  using namespace RichOffline;
  
  if(__RefitInitilizer==0) __RefitInitilizer=new RefitInitilizer;
  // We need to refit because it has not been filled


  RichRing *ptr=RichRing::build(AMSEventR::Head(),this);
  if(!ptr) {ptr=RichRing::build(AMSEventR::Head(),this,true);Status|=1;}

  if(!ptr){
    cout<<"RichRingR::ReBuild -- failed "<<endl;
    return 0;
  }

  Used  = ptr->_used;
  UsedM = ptr->_mused;
  Beta  = ptr->_beta;
  ErrorBeta = ptr->_errorbeta;
  BetaRefit = ptr->_wbeta;
  Status    = ptr->_status;
  NpCol= ptr->_collected_npe;
  NpColLkh= ptr->_collected_npe_lkh;
  NpExp     = ptr->_npexp;
  Prob    = ptr->_probkl;
  UDist = ptr->_unused_dist;
  Theta   =ptr->_theta;
  ErrorTheta  =ptr->_errortheta;

  for(int i=0;i<3;i++){
    TrRadPos[i]  = ptr->_radpos[i];
    TrPMTPos[i]  = ptr->_pmtpos[i];
  }
  for(int i=0;i<5;i++)
    AMSTrPars[i] = ptr->_crossingtrack[i];

  for(int i=0;i<10;i++){
    UsedWindow[i]=ptr->_collected_hits_window[i];
    NpColWindow[i]=ptr->_collected_pe_window[i];
  }

  ////////// Fill RAW BETA HIT
  for(int i=0;i<ptr->_hit_pointer.size();i++){
    // int used_beta=ptr->_hit_pointer[i]?ptr->_hit_used[i]:-1;
    fUsedBeta.push_back(ptr->_hit_used[i]);
    fHitBeta.push_back(ptr->_hit_pointer[i]);
    fRawBeta[0].push_back(ptr->_beta_direct[i]);
    fRawBeta[1].push_back(ptr->_beta_reflected[i]);
  }

  /////////////////////////////////////

  // Loop on all hits
  int which=0;
  double sum=0;
  double counter=0;
  int reflected=0;

  for(RichOffline::RichRawEvent *hit=new RichOffline::RichRawEvent(AMSEventR::Head());hit;hit=hit->next(),which++){
    
    // Search it in the pointer array (this should be done with a map)
    int found_at=-1;
    for(int j=0;j<ptr->_hit_pointer.size();j++){
      if(ptr->_hit_pointer[j]!=which) continue;
      found_at=j;
      break;
    }
    
    if(found_at==-1) continue;
    
    int used_beta=ptr->_hit_used[found_at];
    if(used_beta<0 || used_beta>1) continue;
    double beta=used_beta==0?ptr->_beta_direct[found_at]:ptr->_beta_reflected[found_at];
    
    fRichHit.push_back(ptr->_hit_pointer[found_at]);
    fBetaHit.push_back(beta);
    sum+=beta;
    counter++;
    if(used_beta==1) reflected++;
  }
  
  //  Used=fRichHit.size();
  //  UsedM=reflected;
  //  Beta=sum/counter;
  delete ptr;
  return this;
}



RichRingR *RichRingR::ReBuild(TrTrackR *tr){
  fBetaHit.clear();
  fRichHit.clear();
  for(int i=0;i<2;i++) fRawBeta[i].clear();
  fUsedBeta.clear();
  fHitBeta.clear();

  using namespace RichOffline;
  
  if(__RefitInitilizer==0) __RefitInitilizer=new RefitInitilizer;
  // We need to refit because it has not been filled
  

  if(tr==0){
    cout<<"RichRingR::Refit -- No track available "<<endl;
    return 0;
  }

  if(AMSEventR::Head()->nRichHit()<=0) return 0;
  
  RichRing *ptr=RichRing::build(AMSEventR::Head(),tr);

  if(!ptr){
    cout<<"RichRingR::Refit -- failed "<<endl;
    return 0;
  }

  Used  = ptr->_used;
  UsedM = ptr->_mused;
  Beta  = ptr->_beta;
  ErrorBeta = ptr->_errorbeta;
  BetaRefit = ptr->_wbeta;
  Status    = ptr->_status;
  NpCol= ptr->_collected_npe;
  NpColLkh= ptr->_collected_npe_lkh;
  NpExp     = ptr->_npexp;
  Prob    = ptr->_probkl;
  UDist = ptr->_unused_dist;
  Theta   =ptr->_theta;
  ErrorTheta  =ptr->_errortheta;

  for(int i=0;i<3;i++){
    TrRadPos[i]  = ptr->_radpos[i];
    TrPMTPos[i]  = ptr->_pmtpos[i];
  }
  for(int i=0;i<5;i++)
    AMSTrPars[i] = ptr->_crossingtrack[i];

  for(int i=0;i<10;i++){
    UsedWindow[i]=ptr->_collected_hits_window[i];
    NpColWindow[i]=ptr->_collected_pe_window[i];
  }

  ////////// Fill RAW BETA HIT
  for(int i=0;i<ptr->_hit_pointer.size();i++){
    fUsedBeta.push_back(ptr->_hit_used[i]);
    fHitBeta.push_back(ptr->_hit_pointer[i]);
    fRawBeta[0].push_back(ptr->_beta_direct[i]);
    fRawBeta[1].push_back(ptr->_beta_reflected[i]);
  }

  //////////////////////////////////////

  // Loop on all hits
  int which=0;
  double sum=0;
  double counter=0;
  int reflected=0;

  for(RichOffline::RichRawEvent *hit=new RichOffline::RichRawEvent(AMSEventR::Head());hit;hit=hit->next(),which++){
    
    // Search it in the pointer array (this should be done with a map)
    int found_at=-1;
    for(int j=0;j<ptr->_hit_pointer.size();j++){
      if(ptr->_hit_pointer[j]!=which) continue;
      found_at=j;
      break;
    }
    
    if(found_at==-1) continue;
    
    int used_beta=ptr->_hit_used[found_at];
    if(used_beta<0 || used_beta>1) continue;
    double beta=used_beta==0?ptr->_beta_direct[found_at]:ptr->_beta_reflected[found_at];
    
    fRichHit.push_back(ptr->_hit_pointer[found_at]);
    fBetaHit.push_back(beta);
    sum+=beta;
    counter++;
    if(used_beta==1) reflected++;
  }
  
  //  Used=fRichHit.size();
  //  UsedM=reflected;
  //  Beta=sum/counter;
  delete ptr;
  return this;
}


int RichRingR::FillVector(double &fac,std::vector<int>&hits, double ns, int verbose){
double chi=0;
hits.clear();
 if(ns<=1){
  if(verbose)cerr<<"RichRingR::FillVector-E-nsTooSmallMin1 "<<ns<<endl;
    return 2;
  }
const int ntry=11;
  bool conv=false;
  double mea=100000;
  double rms=mea;
  double s3o=0;
for(int l=0;l<ntry;l++){
  double s=0;
  double s2=0;
  double s3=0;
  for(int j=0;j<fRichHit.size();j++){
    RichHitR &hit=AMSEventR::Head()->RichHit(iRichHit(j));
    double counts=hit.Npe;
     if(verbose>3)cout<<"RichRingR::FillVector-I-"<<j<<" "<<counts<<endl;
    if(fabs(counts-mea)<ns*rms){
    s+=counts;
    s2+=counts*counts;
    s3++;
   }
   }
  
  if(s3>0){
   s/=s3;
   s2/=s3;
   s2=sqrt(s2-s*s);
   mea=s;
   rms=s2;
  }
  else{
   if(verbose)cerr<<"RichRingR::FillVector-E-nohitsFound "<<endl;
   return 1;
  }  
  fac=mea*s3;
  if(l==0)chi=fac;
  fac/=chi;
     if(verbose>2)cout<<"RichRingR::FillVector-I-"<<" itry mean rms "<<l<<"  "<<mea<<" "<<rms<<" "<<s3<<endl;
     if(s3o==s3){
       conv=true;
       break;
     }
     s3o=s3;
}
if(verbose && !conv)cerr<<"RichRingR::FillVector-E-NotConverged"<<endl;
int acc=0;
int rej=0;
  for(int j=0;j<fRichHit.size();j++){
    RichHitR &hit=AMSEventR::Head()->RichHit(iRichHit(j));
    double counts=hit.Npe;
    if(fabs(counts-mea)>=ns*rms){
     hits.push_back(iRichHit(j));
     if(verbose>1)cout<<"RichRingR::FillVector-I-"<<" RejectedHit "<<iRichHit(j)<<" counts "<<counts<<" "<<mea<<" "<<rms<<endl;
     rej++;
   }
   else acc++;
  }
 if(verbose)cout<<"RichRingR::FillVector-I-"<<" Rejected "<<rej<<" out of "<<acc+rej<<" Charge "<<sqrt(fac*getCharge2Estimate())<<endl;
 return conv?0:3;
}

RichRingR *RichRingR::Refit(double &fac,double ns, int ver){
vector <int>s;
FillVector(fac,s,ns,ver);
return Refit(s);
}

RichRingR *RichRingR::Refit(std::vector<int> &hits){
  if(AMSEventR::Head()==0){
    cout<<"RichRingR::Refit -- Could not find the AMSEventR: singleton "<<endl;
    return 0;
  }

  if(__RefitInitilizer==0) __RefitInitilizer=new RefitInitilizer;

  if(fBetaHit.size()==0)   ReBuild();

  // Select the right hits
  double mean=0;
  double mean2=0;
  double meanw=0;
  double mean2w=0;
  int counter=0;
  double weight=0;
  
  for(int j=0;j<fRichHit.size();j++){

    bool skip=false;
    for(int i=0;i<hits.size();i++){
      if(hits[i]==fRichHit[j]){
        skip=true;
        break;
      }
    }

    if(skip) continue;

    mean+=fabs(fBetaHit[j]);
    mean2+=fBetaHit[j]*fBetaHit[j];
    counter++;

    RichHitR &hit=AMSEventR::Head()->RichHit(iRichHit(j));
      
    double w=hit.Npe;
    meanw+=fabs(fBetaHit[j])*w;
    mean2w+=fBetaHit[j]*fBetaHit[j]*w;
    weight+=w;
  }

  Beta=mean/counter;
  BetaRefit=meanw/weight;
  ErrorBeta=sqrt(mean2/counter-Beta*Beta)/sqrt(counter);
  
  //// TO BE DONE
  // Probkl
  // NpCol
  // Theta
  // ...

  return this;
}



double RichRingR::ComputeNpExp(TrTrackR *tr,double beta,double Z){
  if(__RefitInitilizer==0) __RefitInitilizer=new RefitInitilizer;

  using namespace RichOffline;
  // Extrapolate the track to the radiator
  AMSPoint pnt;
  AMSDir dir;
  tr->Interpolate(RichOffline::RICHDB::RICradpos(),pnt,dir);

  // Fill all the necessary variables
  TrTrack track(pnt,dir);
  RichRadiatorTileManager crossed_tile(&track);
  if(crossed_tile.getkind()==empty_kind) return 0;
  
  RichRing::_index=crossed_tile.getindex();
  RichRing::_height=crossed_tile.getheight();
  RichRing::_entrance_p=crossed_tile.getentrancepoint();
  RichRing::_entrance_d=crossed_tile.getentrancedir();
  RichRing::_clarity=crossed_tile.getclarity();
  RichRing::_abs_len=crossed_tile.getabstable();
  RichRing::_index_tbl=crossed_tile.getindextable();
  RichRing::_kind_of_tile=crossed_tile.getkind();
  RichRing::_tile_index=crossed_tile.getcurrenttile();
  RichRing::_distance2border=crossed_tile.getdistance();

  AMSPoint dirp=crossed_tile.getemissionpoint();
  AMSDir   dird=crossed_tile.getemissiondir();
  AMSPoint refp=crossed_tile.getemissionpoint(1);
  AMSDir   refd=crossed_tile.getemissiondir(1);
  
  RichRing::_emission_p=dirp;
  RichRing::_emission_d=dird;

  RichRing *ptr=new RichRing(&track,
			     0,0,
			     beta,
			     0,
			     beta,
			     beta,
			     0,0,
			     0,
			     0,
			     0,
			     true);

  if(ptr==0){
    cout<<"FAILED BUILDING FAKE RING "<<endl;
    return 0;
  }

  if(ptr->_npexp!=ptr->_npexp) ptr->_npexp=0;
  if(ptr->_npexp==0){
    delete ptr;
    return 0;
  }


  ////// APPLY CORRECTIONS
  // If it is data, check that the required tools are OK
  RichPMTCalib::currentDir=correctionsDir;
  RichPMTCalib::useRichRunTag = useRichRunTag;
  RichPMTCalib::usePmtStat = usePmtStat;
  RichPMTCalib::useSignalMean = useSignalMean;
  RichPMTCalib::useGainCorrections = useGainCorrections;
  RichPMTCalib::useEfficiencyCorrections = useEfficiencyCorrections;
  RichPMTCalib::useBiasCorrections = useBiasCorrections;
  RichPMTCalib::useTemperatureCorrections = useTemperatureCorrections;
  RichPMTCalib::useEffectiveTemperatureCorrection = useEffectiveTemperatureCorrection;

  RichConfigManager::useExternalFiles = useExternalFiles;
  RichConfigManager::reloadTemperatures = reloadTemperatures;
  RichConfigManager::reloadRunTag = reloadRunTag;
  RichConfigManager::reloadPMTs = reloadPMTs;

  RichPMTCalib *corr=RichPMTCalib::Update();

  if(corr){
    double sum=0;
    for(int i=0;i<680;i++){
      if(ptr->NpExpPMT[i]<=0) continue;
      if(find(corr->BadPMTs.begin(), corr->BadPMTs.end(),i)!=corr->BadPMTs.end()) continue;
      sum+=ptr->NpExpPMT[i]*
	corr->EfficiencyCorrection(i) * corr->EfficiencyTemperatureCorrection(i)
	* corr->EffectiveTemperatureCorrection();
    }
    ptr->_npexp=sum;
  }

  if(RichChargeUniformityCorrection::getHead()) ptr->_npexp*=RichChargeUniformityCorrection::getHead()->getCorrection(beta,RichRing::_emission_p[0],RichRing::_emission_p[1],RichRing::_emission_d[0],RichRing::_emission_d[1]);

  double result=ptr->_npexp*Z*Z;

  delete ptr;
  return result;
}


int RichRingR::ClusterizeZ1(double dist,int min_size){
  ClusterMean.clear();
  ClusterRms.clear();
  ClusterBetas.clear();

  // Given a ring, use DBSCAN to estimate the number of clusters
  if(fUsedBeta.size()==0) if(!ReBuild() || fUsedBeta.size()==0) return 0; // Rebuild myself

  vector<double> all_betas;  // All betas used as seed

  for(int i=0;i<2;i++) for(uint j=0;j<fRawBeta[i].size();j++) if(fRawBeta[i][j]>0) all_betas.push_back(fRawBeta[i][j]);
  
  vector<int> kind; // association for each beta and hit
  const int noise=-1;
  const int undef=-2;
  kind.assign(all_betas.size(),undef);
  vector<int> nb;nb.reserve(all_betas.size());
  vector<int> seed;seed.reserve(all_betas.size());

  // Use DBSCAN algorithm
  int cluster=0;
  for(uint b=0;b<all_betas.size();b++){
    
    if(kind[b]!=undef) continue; // Already processed

    nb.clear();
    // Associate hits

    for(uint h=0;h<all_betas.size();h++) if(fabs(all_betas[h]-all_betas[b])<dist) nb.push_back(h); 

    if(nb.size()<min_size){kind[b]=noise;continue;} // Not enough neighbours

    // Found a core
    cluster++;
    kind[b]=cluster-1;
    
    // Expand it
    for(uint i=0;i<nb.size();i++) if(nb[i]!=b) seed.push_back(nb[i]); 

    for(uint i=0;i<seed.size();i++){
      if(kind[seed[i]]==noise) kind[seed[i]]=cluster-1; 
      if(kind[seed[i]]!=undef) continue;
      kind[seed[i]]=cluster-1; 
      nb.clear();
      for(uint h=0;h<all_betas.size();h++) if(fabs(all_betas[h]-all_betas[seed[i]])<dist) nb.push_back(h);
      if(nb.size()>=min_size) for(uint h=0;h<nb.size();h++) seed.push_back(nb[h]);
    }
  }
  

  vector<int> sizes; sizes.assign(cluster,0);
  vector<double> sum; sum.assign(cluster,0);
  vector<double> sum2; sum2.assign(cluster,0);
  for(int i=0;i<kind.size();i++){
    if(kind[i]==noise || kind[i]==undef) continue;
    sizes[kind[i]]++;
    sum[kind[i]]+=all_betas[i];
    sum2[kind[i]]+=all_betas[i]*all_betas[i];
  }


  // Sort from more to less hits
  vector<int> pointers; pointers.reserve(cluster);
  /*
  for(int i=0;i<cluster;i++) pointers.push_back(i);

  struct _COMP{
    vector<int> *_sizes;
    bool operator()(int i,int j){return _sizes->at(i)>=_sizes->at(j);}
    _COMP(vector<int> *p):_sizes(p){};
  } _comp(&sizes);
  
  sort (pointers.begin(), pointers.end(), _comp); 
  */

  // Do the sorting by hand
  vector<bool> done;done.assign(sizes.size(),false);
  for(;;){
    int max=-1;
    int which=-1;
    for(uint i=0;i<sizes.size();i++){
      if(done[i]) continue;
      if(sizes[i]>max){max=sizes[i];which=i;}
    }
    if(which<0) break;
    pointers.push_back(which);
    done[which]=true;
  }

  

  // Fill the clusters
  ClusterBetas.assign(cluster,0);
  ClusterMean.assign(cluster,0);
  ClusterRms.assign(cluster,0);


  for(int ii=0;ii<pointers.size();ii++){
    int i=pointers[ii];
    ClusterBetas[ii]=sizes[i];
    ClusterMean[ii]=sum[i]/sizes[i];
    ClusterRms[ii]=sqrt(sum2[i]/sizes[i]-sum[i]*sum[i]/sizes[i]/sizes[i]);
  }

  return pointers.size();
}



#endif
#endif
