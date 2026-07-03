//  $Id: EcalShowerDirectionFit.C,v 1.12 2016/06/10 09:11:37 choutko Exp $
// Extracted from ecalrec.C on 18.06.2015, by nzimmerm.

#include <float.h>
#include <fenv.h>
#include <cmath>
#include "amsdbc.h"
#include "EcalShowerDirectionFit.h"

#ifndef __ROOTSHAREDLIBRARY__
#include "ecalrec.h"
#else
#include "root.h"
#endif

#define DEBUG_ECAL_DIRECTION_FIT 0

extern "C" void e04ccf_(int &n, number x[], number &f, number &tol, int &iw, number w1[],number w2[], number w3[], number w4[], number w5[], number w6[],void * alfun, void * monit, int & maxcal, int &ifail, void * p);
extern "C" void d01amf_(void *alfun, number &bound, integer &inf, number &epsa, number &epsr, number &result, number &abserr, number w[], int &lw, int iw[], int &liw , int &ifail, void *p );

#ifndef __ROOTSHAREDLIBRARY__
EcalShowerDirectionFit::EcalShowerDirectionFit(AMSEcal2DCluster* pCl0, AMSEcal2DCluster* pCl1)
  : _Direction(0)
  , _OrphanedEnergy(0) {

  _pCl[0] = pCl0;
  _pCl[1] = pCl1;

  for (int i = 0; i < 2; ++i) {
    tot[i] = 0;
    chi2[i] = t0[i] = tantz[i] = 0.0;
    zcorr[i] = true;
  }

  for (int i = 0; i < 2*_ECSLMX; ++i) {
    _Zcorr[i] = 0.0;
  }
}
#else
EcalShowerDirectionFit::EcalShowerDirectionFit(EcalShowerR* shower, number orphanedEnergy)
  : _Direction(0)
  , _OrphanedEnergy(orphanedEnergy) {
   _Shower = shower;
   _pCl[0]=_pCl[1]=0;
   if(shower){
if(shower->NEcal2DCluster() >= 2){
  _pCl[0] = shower->pEcal2DCluster(0);
  _pCl[1] = shower->pEcal2DCluster(1);
}
else{
cerr<<"EcalShowerDirectionFit::EcalShowerDirectionFit-E-shower->NEcal2DCluster() <2 "<<shower->NEcal2DCluster()<<endl;
_pCl[0]=shower->NEcal2DCluster()?shower->pEcal2DCluster(0):0;
_pCl[1]=0;
 }
}
  for (int i = 0; i < 2; ++i) {
    tot[i] = 0;
    chi2[i] = t0[i] = tantz[i] = 0.0;
    zcorr[i] = true;
  }

  for (int i = 0; i < 2*_ECSLMX; ++i) {
    _Zcorr[i] = 0.0;
  }
}

const number sOrphanedEnergyFindingTresholdInMeV = 0.1; // Should absorb most rounding differences

#if DEBUG_ECAL_DIRECTION_FIT > 0
// Useful debugging helper, when looking for orphaned energies
void DumpAll2DClusters(EcalShowerR* shower) {

  fprintf(stderr, "-> START DUMPING ALL 2D CLUSTERS\n");
  for (int cluster2DIndex = 0; cluster2DIndex < shower->NEcal2DCluster(); ++cluster2DIndex) {
    Ecal2DClusterR* cluster2D = shower->pEcal2DCluster(cluster2DIndex);
    for (int cluster1DIndex = 0; cluster1DIndex < cluster2D->NEcalCluster(); ++cluster1DIndex) {
      EcalClusterR* p = cluster2D->pEcalCluster(cluster1DIndex);
      fprintf(stderr, "cluster2DIndex=%i, proj=%i, i=%i, plane=%i, w=%.4f, coo[0]=%.4f, coo[1]=%.4f, coo[2]=%.4f, left=%i, right=%i, stat=%i\n",
                       cluster2DIndex, p->Proj, cluster1DIndex, p->Plane, p->Edep, p->Coo[0], p->Coo[1], p->Coo[2], p->Left, p->Right, p->Status);
    }
    fprintf(stderr, "----------\n");
  }
  fprintf(stderr, "<- STOP DUMPING ALL 2D CLUSTERS\n");
}
#endif

void EcalShowerDirectionFit::FindOrphaned1DClustersFirstPass(std::vector<EcalClusterR*>& orphanedClusters) {

  for (int cluster2DIndex = 0; cluster2DIndex < _Shower->NEcal2DCluster(); ++cluster2DIndex) {
    Ecal2DClusterR* cluster2D = _Shower->pEcal2DCluster(cluster2DIndex);

    int lastPlane = -1;
    for (int cluster1DIndex = 0; cluster1DIndex < cluster2D->NEcalCluster(); ++cluster1DIndex) {
      EcalClusterR* cluster1D = cluster2D->pEcalCluster(cluster1DIndex);
      if (cluster1D->Plane <= lastPlane) {
        for (int orphanedCluster1DIndex = cluster1DIndex; orphanedCluster1DIndex < cluster2D->NEcalCluster(); ++orphanedCluster1DIndex) {
          EcalClusterR* orphanedCluster = cluster2D->pEcalCluster(orphanedCluster1DIndex);
          orphanedClusters.push_back(orphanedCluster);
          _OrphanedEnergy -= orphanedCluster->Edep;
        }
        break;
      }

      lastPlane = cluster1D->Plane;
    }
  }

  if (std::abs(_OrphanedEnergy) < sOrphanedEnergyFindingTresholdInMeV)
    _OrphanedEnergy = 0;
}

bool EcalShowerDirectionFit::FindOrphaned1DClustersSecondPass(int cluster2DIndexStart, number& orphanedEnergySum, std::vector<EcalClusterR*>& orphanedClusters) {

  if (cluster2DIndexStart == _Shower->NEcal2DCluster())
    return false;
  if (orphanedEnergySum > _OrphanedEnergy)
    return false;

  Ecal2DClusterR* cluster2D = _Shower->pEcal2DCluster(cluster2DIndexStart);
  for (int cluster1DIndex = cluster2D->NEcalCluster() - 1; cluster1DIndex > 0; --cluster1DIndex) {
    EcalClusterR* cluster1D = cluster2D->pEcalCluster(cluster1DIndex);
    if (std::find(orphanedClusters.begin(), orphanedClusters.end(), cluster1D) != orphanedClusters.end())
      continue;
    number orphanedEnergySumBefore = orphanedEnergySum;
    size_t orphanedClustersBeforeSize = orphanedClusters.size();
    if (FindOrphaned1DClustersSecondPass(cluster2DIndexStart + 1, orphanedEnergySum, orphanedClusters))
      return true;
    orphanedEnergySum = orphanedEnergySumBefore;
    orphanedClusters.resize(orphanedClustersBeforeSize);

    orphanedEnergySum += cluster1D->Edep;
    orphanedClusters.push_back(cluster1D);

    if (std::abs(orphanedEnergySum - _OrphanedEnergy) < sOrphanedEnergyFindingTresholdInMeV) {
      _OrphanedEnergy = 0;
      return true;
    }
  }

  return false;
}
#endif

void EcalShowerDirectionFit::DirectionFit(){

  // correct tan(theta) /shower profile dependence  
  // by linear extrapolation
  // (later by profilefit?)

  const integer Maxrow=_ECSLMX*2;

#ifndef __ROOTSHAREDLIBRARY__
#if DEBUG_ECAL_DIRECTION_FIT > 0
  if (!AMSEcalShower::s_executingFirstReconstructionPass)
    fprintf(stderr, "BEGIN DIRECTION FIT!\n");
#endif

  Ecal1DCluster *p1c[Maxrow+1] = { 0 };
  for (int proj=0;proj<2;proj++){

#if DEBUG_ECAL_DIRECTION_FIT > 0
    if (!AMSEcalShower::s_executingFirstReconstructionPass)
      fprintf(stderr, "2D proj=%i, Edep=%.4f, coo=%.4f, stat=%i\n", _pCl[proj]->getproj(), _pCl[proj]->_Energy,  _pCl[proj]->getcoo(),  _pCl[proj]->getstatus());
#endif

    for (int i=0;i<_pCl[proj]->getNClustKernel();i++){
      Ecal1DCluster *p=_pCl[proj]->getpClust(i);
      p1c[p->getplane()]=p;

#if DEBUG_ECAL_DIRECTION_FIT > 0
      if (!AMSEcalShower::s_executingFirstReconstructionPass) {
        fprintf(stderr, "proj=%i, i=%i, plane=%i, w=%.4f, coo[0]=%.4f, coo[1]=%.4f, coo[2]=%.4f, left=%i, right=%i, stat=%i\n",
                        p->getproj(), i, p->getplane(), p->getweight(), p->getcoo()[0], p->getcoo()[1], p->getcoo()[2], p->getleft(), p->getright(), p->getstatus());
      }
#endif
    }
  }

#if DEBUG_ECAL_DIRECTION_FIT > 0
  if (!AMSEcalShower::s_executingFirstReconstructionPass)
    fprintf(stderr, "END DIRECTION FIT PREPARATIONS!\n");
#endif
#else
#if DEBUG_ECAL_DIRECTION_FIT > 0
  fprintf(stderr, "BEGIN DIRECTION FIT! orphanedEnergy=%.4f\n", _OrphanedEnergy);
  DumpAll2DClusters(_Shower);
#endif

  static std::vector<EcalClusterR*> orphanedClusters;
#ifndef __PPC64
#pragma omp threadprivate (orphanedClusters)
#else
  //FIXME
#ifndef __ROOTSHAREDLIBRARY__
  cerr<<"iEcalShowerDirectionFit--FPPC64 NOT SUPPORTED "<<endl;
  exit(1);
#endif
#endif
  orphanedClusters.reserve(100);

  // First pass to identify orphaned clusters.
  // The list of 1D clusters attached to a 2D cluster is ascending in plane. If we encounter a cluster
  // whose plane is less than the one before, it marks the beginning of the orphaned clusters.
  if (_OrphanedEnergy > 0) {
    FindOrphaned1DClustersFirstPass(orphanedClusters);
#if DEBUG_ECAL_DIRECTION_FIT > 0
    fprintf(stderr, " -> ORPHANED ENERGY after first pass: orphanedEnergy=%.4f\n", _OrphanedEnergy);
#endif
  }

  if (_OrphanedEnergy > 0) {
    std::vector<EcalClusterR*> firstPassOrphanedClusters = orphanedClusters;
    number orphanedEnergySum = 0;
    if (!FindOrphaned1DClustersSecondPass(0, orphanedEnergySum, orphanedClusters)) {
      bool realProblem = true;
      for (int cluster2DIndex = 0; cluster2DIndex < _Shower->NEcal2DCluster(); ++cluster2DIndex) {
        Ecal2DClusterR* cluster2D = _Shower->pEcal2DCluster(cluster2DIndex);
        if (cluster2D->NEcalCluster() == 2*Maxrow) {
          realProblem = false;
          break;
        }
      }
      static int errcount=0;
      if(errcount++<5){
        if (!realProblem)
          std::cerr << "EcalShowerDirectionFit::DirectionFit-I-Can't find out where the orphaned energy (" << Form("%.2f MeV", _OrphanedEnergy) << ") is gone. Results might be slightly wrong, as not all 1D clusters were stored for this event! The maximum of " << 2 * Maxrow << " 1D clusters per 2D cluster was exceeded during initial ECAL reconstruction!" << " (run=" << AMSEventR::Head()->Run() << " event=" << AMSEventR::Head()->Event() << ")" << std::endl;
        else
          std::cerr << "EcalShowerDirectionFit::DirectionFit-E-Can't find out where the orphaned energy (" << Form("%.2f MeV", _OrphanedEnergy) << ") is gone. Results will be wrong! (run=" << AMSEventR::Head()->Run() << " event=" << AMSEventR::Head()->Event() << ")" << std::endl;
       }
       orphanedClusters = firstPassOrphanedClusters;
     }
  }

  EcalClusterR *p1c[Maxrow+1] = { 0 };
  for (int proj=0;proj<2;proj++){
#if DEBUG_ECAL_DIRECTION_FIT > 0
    fprintf(stderr, "2D proj=%i, Edep=%.4f, coo=%.4f, stat=%i\n", _pCl[proj]->Proj, _pCl[proj]->Edep,  _pCl[proj]->Coo, _pCl[proj]->Status);
#endif

    for (int i=0;i< _pCl[proj]->NEcalCluster();i++){
      EcalClusterR *p=_pCl[proj]->pEcalCluster(i);
      if (std::find(orphanedClusters.begin(), orphanedClusters.end(), p) == orphanedClusters.end()) {
        p1c[p->Plane]=p;
#if DEBUG_ECAL_DIRECTION_FIT > 0
        fprintf(stderr, "proj=%i, i=%i, plane=%i, w=%.4f, coo[0]=%.4f, coo[1]=%.4f, coo[2]=%.4f, left=%i, right=%i, stat=%i\n",
                        p->Proj, i, p->Plane, p->Edep, p->Coo[0], p->Coo[1], p->Coo[2], p->Left, p->Right, p->Status);
#endif
      } else {
#if DEBUG_ECAL_DIRECTION_FIT > 0
          fprintf(stderr, "proj=%i, i=%i, plane=%i, w=%.4f, coo[0]=%.4f, coo[1]=%.4f, coo[2]=%.4f, left=%i, right=%i, stat=%i #### ORPHANED!!!!\n",
                            p->Proj, i, p->Plane, p->Edep, p->Coo[0], p->Coo[1], p->Coo[2], p->Left, p->Right, p->Status);
#endif
      }
    }
  }
  orphanedClusters.clear();
#if DEBUG_ECAL_DIRECTION_FIT > 0
  fprintf(stderr, "END DIRECTION FIT PREPARATIONS!\n");
#endif
#endif

  for(int proj=0;proj<2;proj++){
    EcalShowerDirectionFit::StrLineFit(p1c,_LayersNo,proj,true,_Zcorr,tot[proj],chi2[proj],t0[proj],tantz[proj]);
  }

#ifndef __ROOTSHAREDLIBRARY__
  integer pr,pl,ce;
  number cl,ct;
#endif

  const number ZECALLower = -158.914995;
  const number ZECALUpper = -143.194995;
  if(_Direction==0){
#ifdef __ROOTSHAREDLIBRARY__
    _EntryPoint[2] = ZECALUpper;
    _ExitPoint[2] = ZECALLower;
#else
    ECALDBc::getscinfoa(0,0,0,pr,pl,ce,ct,cl,_EntryPoint[2]);
    ECALDBc::getscinfoa(ECALDBc::slstruc(3)-1,0,2,pr,pl,ce,ct,cl,_ExitPoint[2]);
    if(std::abs(_EntryPoint[2] - ZECALUpper) >= 1e-5){
     cerr<<"  EcalShowerDirectionFit::DirectionFit-E-std::abs(_EntryPoint[2] - ZECALUpper) >= 1e-5 "<<_EntryPoint[2]<<" "<<ZECALUpper<<" "<<std::abs(_EntryPoint[2] - ZECALUpper)<<endl;
    }
    if(std::abs(_ExitPoint[2] - ZECALLower) >= 1e-5){
     cerr<<"  EcalShowerDirectionFit::DirectionFit-E-std::abs(_ExitPoint[2] - ZECALLower) >= 1e-5 "<<_ExitPoint[2]<<" "<<ZECALLower<<" "<<std::abs(_ExitPoint[2] - ZECALLower)<<endl;
    }
#endif
  }
  else{
#ifdef __ROOTSHAREDLIBRARY__
    _EntryPoint[2] = ZECALLower;
    _ExitPoint[2] = ZECALUpper;
#else
    ECALDBc::getscinfoa(0,0,0,pr,pl,ce,ct,cl,_ExitPoint[2]);
    ECALDBc::getscinfoa(ECALDBc::slstruc(3)-1,0,2,pr,pl,ce,ct,cl,_EntryPoint[2]);
//    assert(std::abs(_EntryPoint[2] - ZECALLower) < 1e-6);
//    assert(std::abs(_ExitPoint[2] - ZECALUpper) < 1e-6);
#endif
  }

   number ECREFFKEY_Chi2Change2D=0.33;    // pass6 setting
   number ECREFFKEY_EMDirCorrection=1.03; // pass6 setting
#ifndef __ROOTSHAREDLIBRARY__
  ECREFFKEY_Chi2Change2D = ECREFFKEY.Chi2Change2D;
  ECREFFKEY_EMDirCorrection = ECREFFKEY.EMDirCorrection;
#endif

  for (int proj=0;proj<2;proj++){
    // Renonce if chi2 is bad;
#ifndef __ROOTSHAREDLIBRARY__
    if(chi2[proj]>1 && chi2[proj]*ECREFFKEY_Chi2Change2D>_pCl[proj]->_Chi2 ){
      t0[proj]=_pCl[proj]->_Coo;
      tantz[proj]=_pCl[proj]->_Tan;
      chi2[proj]=_pCl[proj]->_Chi2;
      zcorr[proj]=false;
    }
#else
    if(chi2[proj]>1 && chi2[proj]*ECREFFKEY_Chi2Change2D>_pCl[proj]->Chi2 ){
      t0[proj]=_pCl[proj]->Coo;
      tantz[proj]=_pCl[proj]->Tan;
      chi2[proj]=_pCl[proj]->Chi2;
      zcorr[proj]=false;
    }
#endif

    _EntryPoint[proj]=t0[proj]+tantz[proj]*_EntryPoint[2];
    _ExitPoint[proj]=t0[proj]+tantz[proj]*_ExitPoint[2];
  }
  _Dir=_ExitPoint-_EntryPoint;

  //    Get corrected EM dir
  AMSPoint Entry(0,0,_EntryPoint[2]);      
  AMSPoint Exit(0,0,_ExitPoint[2]);      
  for(int i=0;i<2;i++){
#ifndef __ROOTSHAREDLIBRARY__
    Entry[i]=_pCl[i]->getcoo()+_pCl[i]->gettan()*ECREFFKEY_EMDirCorrection*Entry[2];
    Exit[i]=_pCl[i]->getcoo()+_pCl[i]->gettan()*ECREFFKEY_EMDirCorrection*Exit[2];
#else
    Entry[i]=_pCl[i]->Coo+_pCl[i]->Tan*ECREFFKEY_EMDirCorrection*Entry[2];
    Exit[i]=_pCl[i]->Coo+_pCl[i]->Tan*ECREFFKEY_EMDirCorrection*Exit[2];
#endif
  }
  _EMDir=Exit-Entry;
}

#ifndef __ROOTSHAREDLIBRARY__
bool EcalShowerDirectionFit::StrLineFit(Ecal1DCluster *p1c[],int Maxrow,int proj,bool reset, number *pcorrect, int &tot, number &chi2, number &t0, number &tantz){
  if(reset){
    for(int i=0;i<Maxrow;i++){
      if(p1c[i] && p1c[i]->getproj()==proj)p1c[i]->clearstatus(AMSDBc::DELETED);
    }
  }
  bool again=false;
  bool restore=false;
  number chi2old=0;
  integer ipmaxold=-1;
 AGAIN:
  number z=0;
  number z2=0;
  number t=0;
  number tz=0;
  number e=0;
  tot=0;
  for(int ipl=0;ipl<Maxrow;ipl++){
    if(p1c[ipl]&& p1c[ipl]->getproj()==proj && !p1c[ipl]->checkstatus(AMSDBc::DELETED)){
      number w=p1c[ipl]->getweight();
      AMSPoint coo=p1c[ipl]->getcoo();

#if DEBUG_ECAL_DIRECTION_FIT > 0
      if (!AMSEcalShower::s_executingFirstReconstructionPass)
        fprintf(stderr, "proj=%i, ipl=%i, w=%.4f, coo[0]=%.4f, coo[1]=%.4f, coo[2]=%.4f, left=%i, right=%i, stat=%i\n",
                        proj, ipl, w, coo[0], coo[1], coo[2], p1c[ipl]->getleft(), p1c[ipl]->getright(), p1c[ipl]->getstatus());
#endif

      number zc=0;
      if(pcorrect){ 
        zc=pcorrect[ipl];
      }
      z+=(coo[2]+zc)*w;
      z2+=(coo[2]+zc)*(coo[2]+zc)*w;
      t+=coo[p1c[ipl]->getproj()]*w;
      tz+=coo[p1c[ipl]->getproj()]*coo[2]*w;
      e+=w;
      tot++;
    }
  }
  if(tot>ECREFFKEY.Length2DMin && e>0){
    z/=e;
    z2/=e;
    t/=e;
    tz/=e;
    tantz=(tz-t*z)/(z2-z*z);
    t0=t-z*tantz;
    chi2=0;
    number chi2max=0;
    int ipmax=-1;
    for (int ipl=0;ipl<Maxrow;ipl++){
      if(p1c[ipl] && p1c[ipl]->getproj()==proj && !p1c[ipl]->checkstatus(AMSDBc::DELETED)){
    number w=1./p1c[ipl]->PosError();
    AMSPoint coo=p1c[ipl]->getcoo();
    number zc=0;
    if(pcorrect){ 
      zc=pcorrect[ipl];
    }
    number dx=(coo[p1c[ipl]->getproj()]-tantz*(coo[2]+zc)-t0);
    number delta=(dx*w)*(dx*w);
    if(delta>chi2max){
      chi2max=delta;
      ipmax=ipl;
    }    
    chi2+=delta;
      }
    }
    chi2=chi2/(tot-1);
    if(again && chi2>chi2old){
      restore=true;
      again=false;
      p1c[ipmaxold]->clearstatus(AMSDBc::DELETED);
#if DEBUG_ECAL_DIRECTION_FIT > 0
      fprintf(stderr, " --> AGAIN1!\n");
#endif
      goto AGAIN;
    }
    number chi2proj=(chi2*(tot-1)-chi2max)/(tot-2);
    if(chi2>0 && !restore && tot>ECREFFKEY.Length2DMin+1 && ((chi2>ECREFFKEY.Chi22DMax/10. && chi2proj/chi2<2*ECREFFKEY.Chi2Change2D) || chi2proj/chi2<ECREFFKEY.Chi2Change2D)){
      again=true;
      chi2old=chi2;
      p1c[ipmax]->setstatus(AMSDBc::DELETED);
      ipmaxold=ipmax;
#if DEBUG_ECAL_DIRECTION_FIT > 0
      fprintf(stderr, " --> AGAIN2!\n");
#endif
      goto AGAIN;
    }
    return true;
  }
  return false;
}
#else
void setstatus(uinteger& _status, uinteger status){_status=_status | status;}
void clearstatus(uinteger& _status, uinteger status){_status=_status & ~status;}
uinteger checkstatus(uinteger _status, integer checker){return _status & checker;}

bool EcalShowerDirectionFit::StrLineFit(EcalClusterR *p1c[],int Maxrow,int proj,bool reset, number *pcorrect, int &tot, number &chi2, number &t0, number &tantz){
  if(reset){
    for(int i=0;i<Maxrow;i++){
      if(p1c[i] && p1c[i]->Proj==proj)clearstatus(p1c[i]->Status, AMSDBc::DELETED);
    }
  }
  bool again=false;
  bool restore=false;
  number chi2old=0;
  integer ipmaxold=-1;
 AGAIN:
  number z=0;
  number z2=0;
  number t=0;
  number tz=0;
  number e=0;
  tot=0;
  for(int ipl=0;ipl<Maxrow;ipl++){
    if(p1c[ipl]&& p1c[ipl]->Proj==proj && !checkstatus(p1c[ipl]->Status, AMSDBc::DELETED)){

      number w=0;
      const integer ECREFFKEY_Cl1DCoreSize=2; //pass6 setting

      integer ileft=p1c[ipl]->Center-ECREFFKEY_Cl1DCoreSize;

      if( ileft<p1c[ipl]->Left)ileft=p1c[ipl]->Left;
      integer iright=p1c[ipl]->Center+ECREFFKEY_Cl1DCoreSize;
      if(iright>p1c[ipl]->Right)iright=p1c[ipl]->Right;
      integer ilr=min(iright-p1c[ipl]->Center,p1c[ipl]->Center-ileft);
      ileft=p1c[ipl]->Center-ilr;
      iright=p1c[ipl]->Center+ilr;
      for(int k=ileft;k<=iright;k++){
        number e;
        number adc = 0;
        for (int hitIndex=0; hitIndex < p1c[ipl]->NEcalHit();++hitIndex) {
          EcalHitR* hit = p1c[ipl]->pEcalHit(hitIndex);
//          assert(hit->Proj == proj);
//          assert(hit->Plane == ipl);
          if (hit->Cell == k) {
            adc = hit->Edep;
            break;
          }
        }

        if(k==p1c[ipl]->Center+1 && p1c[ipl]->Status & AMSDBc::WIDE){
           e=adc/2;
        }
        else {
          e=adc;
        }
        w+= e;
      }

      AMSPoint coo=p1c[ipl]->Coo;

#if DEBUG_ECAL_DIRECTION_FIT > 0
      fprintf(stderr, "proj=%i, ipl=%i, w=%.4f, coo[0]=%.4f, coo[1]=%.4f, coo[2]=%.4f, left=%i, right=%i, stat=%i\n",
                      proj, ipl, w, coo[0], coo[1], coo[2], p1c[ipl]->Left, p1c[ipl]->Right, p1c[ipl]->Status);
#endif

      number zc=0;
      if(pcorrect){ 
        zc=pcorrect[ipl];
      }
      z+=(coo[2]+zc)*w;
      z2+=(coo[2]+zc)*(coo[2]+zc)*w;
      t+=coo[p1c[ipl]->Proj]*w;
      tz+=coo[p1c[ipl]->Proj]*coo[2]*w;
      e+=w;
      tot++;
    }
  }
  const number ECREFFKEY_Length2DMin=3;     // pass6 setting
  const  number ECREFFKEY_Chi2Change2D=0.33; // pass6 setting
  const  number ECREFFKEY_Chi22DMax=1000;   // pass6 setting
  const  number ECREFFKEY_PosError1D=0.1;   // pass6 setting
  if(tot>ECREFFKEY_Length2DMin && e>0){
    z/=e;
    z2/=e;
    t/=e;
    tz/=e;
    tantz=(tz-t*z)/(z2-z*z);
    t0=t-z*tantz;
    chi2=0;
    number chi2max=0;
    int ipmax=-1;
    for (int ipl=0;ipl<Maxrow;ipl++){
      if(p1c[ipl] && p1c[ipl]->Proj==proj && !checkstatus(p1c[ipl]->Status, AMSDBc::DELETED)){
    number w=1./ECREFFKEY_PosError1D;
    AMSPoint coo=p1c[ipl]->Coo;
    number zc=0;
    if(pcorrect){ 
      zc=pcorrect[ipl];
    }
    number dx=(coo[p1c[ipl]->Proj]-tantz*(coo[2]+zc)-t0);
    number delta=(dx*w)*(dx*w);
    if(delta>chi2max){
      chi2max=delta;
      ipmax=ipl;
    }    
    chi2+=delta;
      }
    }
    chi2=chi2/(tot-1);
    if(again && chi2>chi2old){
      restore=true;
      again=false;
      clearstatus(p1c[ipmaxold]->Status, AMSDBc::DELETED);
#if DEBUG_ECAL_DIRECTION_FIT > 0
      fprintf(stderr, " --> AGAIN1!\n");
#endif
      goto AGAIN;
    }
    number chi2proj=(chi2*(tot-1)-chi2max)/(tot-2);
    if(chi2>0 && !restore && tot>ECREFFKEY_Length2DMin+1 && ((chi2>ECREFFKEY_Chi22DMax/10. && chi2proj/chi2<2*ECREFFKEY_Chi2Change2D) || chi2proj/chi2<ECREFFKEY_Chi2Change2D)){
      again=true;
      chi2old=chi2;
      setstatus(p1c[ipmax]->Status, AMSDBc::DELETED);
      ipmaxold=ipmax;
#if DEBUG_ECAL_DIRECTION_FIT > 0
      fprintf(stderr, " --> AGAIN2!\n");
#endif
      goto AGAIN;
    }
    return true;
  }
  return false;
}
#endif
