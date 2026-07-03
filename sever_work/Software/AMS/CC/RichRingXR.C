//  $Id: RichRingXR.C,v 1.26 2025/05/29 14:36:01 choutko Exp $
//
//  RichRingXR.C
//  RichRingXR
//
//  Created by Jianan Xiao on 2025/04/28.
#include "RichRingXR.h"
#include "RichRingX_RichConst.h"
#include "RichRingX.h"
#include "root.h"
ClassImp(RichRingXR)

bool RichRingXR::enableNCorrection = true;
bool RichRingXR::enablePMTCorrection = true;
bool RichRingXR::enableBetaCorrection = true;

int & RichRingXR::sdphi(int d){
static int dphil=0;
#pragma omp threadprivate (dphil)
if(d)dphil=d;
return dphil?dphil:dphi;
}

int & RichRingXR::sdz(int d){
static int dzl=0;
#pragma omp threadprivate (dzl)
if(d)dzl=d;
return dzl?dzl:dz;
}

int RichRingXR::create = 0;
int RichRingXR::mthr=1;
int RichRingXR::ideal_mc=0;
int RichRingXR::dz=500;
int RichRingXR::dphi=400;
bool RichRingXR::initialized = false;

int RichRingXR::getHitType(int i) {
	if(i<0)return -4;
	if(!pRing)return -3;
	if(i>=pRing->m_hitType.size() )return -2;
	if(pRing->m_hitType[i]==static_cast<int>(HitTypes::Direct))return 0;
	else if (pRing->m_hitType[i]==static_cast<int>(HitTypes::Reflect))return 1;
	else if (pRing->m_hitType[i]==static_cast<int>(HitTypes::Noise)) return 2;
	else return -1;
}

char * RichRingXR::Info(int number) {
	sprintf(_Info,"RichRingX No %d Track=%d %s N_{Hits}=%d N_{MirrHits}=%d  #beta=%8.4g  Prob_{Kl.}=%7.3g  Expected_{PhotoEl}=%5.2f Collected_{PhotoEl}=%5.2f",number,fTrTrack,isNaF()?"NaF":"",Used,UsedM,getBeta(),Prob,NpExp,NpCol);
	return _Info;
}

RichRingXR::RichRingXR(int pTrack) {
	this->pRing = new RichRingX(pTrack);
	fTrTrack=pTrack;
	Prob=0;
	PMTs=0;
	NpExp=0;
	Beta=0;
	Status=0;
	Used=0;
	UsedM=0;
	NpCol=0;
	NpTot=0;
	Theta=0;
	Phi=0;
	// P0[0]=P0[1]=P0[2]=0;
}

RichRingXR::RichRingXR(RichRingX* pRing) {
        Status=0;
	this->pRing = pRing;
	fTrTrack = pRing->fTrTrack;
	Beta=pRing->getBeta();
	if(pRing->isGood())Status|=1;
	if(pRing->isNaF())Status|=(1<<1);
	if(pRing->isAboveThreshold())Status|=(1<<2);
	if(pRing->isStrongSignal())Status|=(1<<3);
	if(pRing->isClean())Status|=(1<<4);
	RI=pRing->getRefractiveIndex();
	Used=pRing->getNumHitsUsed();
	UsedM=pRing->getNumReflectedHitsUsed();
	NpCol=pRing->getCollectedPhotoElectrons();
	NpTot=pRing->getPhotoElectrons();
	Theta=pRing->getTrackTheta();
	Phi=pRing->getTrackPhi();

	CorrectionConstant = pRing->getCorrectionConstant();
	ThetaC = pRing->getTheta();
	ThetaC_npe = pRing->getThetaRefit();
	Likelihood = pRing->getLikelihood();
	EmissionPoint = pRing->getTrackEmissionPoint();
	PMTCrossPoint = pRing->getPMTCrossPoint();
	// for(int k=0;k<3;k++)P0[k]=EmissionPoint[k]; 

	for(int i=0;i<pRing->m_validHitsIndex.size();i++)fRichHit.push_back(pRing->m_validHitsIndex[i]);
	Prob=0;
	PMTs=0;
	NpExp=0; 
}

RichRingXR::RichRingXR(const RichRingXR& handle) {
	pRing = handle.pRing;
	fTrTrack = handle.fTrTrack;
	fRichHit = handle.fRichHit;
	Beta=handle.Beta;
	Prob=handle.Prob;
	PMTs=handle.PMTs;
	NpExp=handle.NpExp;
	Status=handle.Status;
	RI=handle.RI;
	Used=handle.Used;
	UsedM=handle.UsedM;
	NpCol=handle.NpCol;
	NpTot=handle.NpTot;
	Theta=handle.Theta;
	Phi=handle.Phi;
	// for(int k=0;k<3;k++)P0[k]=handle.P0[k];

	CorrectionConstant = handle.CorrectionConstant;
	ThetaC = handle.ThetaC;
	ThetaC_npe = handle.ThetaC_npe;
	Likelihood = handle.Likelihood;
	EmissionPoint = handle.EmissionPoint;
	PMTCrossPoint = handle.PMTCrossPoint;
}

RichRingXR& RichRingXR::operator=(const RichRingXR& handle) {
	if (this != &handle) {
		pRing = handle.pRing;
		fTrTrack = handle.fTrTrack;
		fRichHit = handle.fRichHit;
		Beta=handle.Beta;
		Status=handle.Status;
		RI=handle.RI;
		Used=handle.Used;
		UsedM=handle.UsedM;
		NpCol=handle.NpCol;
		NpTot=handle.NpTot;
		Theta=handle.Theta;
		Phi=handle.Phi;
		// for(int k=0;k<3;k++)P0[k]=handle.P0[k];
		Prob=handle.Prob;
		PMTs=handle.PMTs;
		NpExp=handle.NpExp;
		
		CorrectionConstant = handle.CorrectionConstant;
		ThetaC = handle.ThetaC;
		ThetaC_npe = handle.ThetaC_npe;
		Likelihood = handle.Likelihood;
		EmissionPoint = handle.EmissionPoint;
		PMTCrossPoint = handle.PMTCrossPoint;
	}
	return *this;
}


void RichRingXR::init(const char* path,int ideal_mc,int thr) {
#ifdef __ROOTSHAREDLIBRARY__
#ifdef _OPENMP
	if(thr>0 && thr<omp_get_num_procs())
		omp_set_num_threads(thr);
	else
		omp_set_num_threads(omp_get_num_procs());
	 cout<<" RichringXR::init-I-threads "<<omp_get_num_threads()<<" "<<thr<<" "<<omp_get_num_procs()<<endl;

	// omp_set_dynamic(1);
	// kmp_set_blocktime(200);
#endif
#endif
	if (initialized) {
		//std::cerr << "[Warning] RichRingXR::Init() called more than once. Ignoring...\n";
		return;
	}
	#pragma omp critical (richringxinit)
	{
		RichRingX::initCorrection(path, ideal_mc, RichRingXR::enableNCorrection, RichRingXR::enablePMTCorrection, RichRingXR::enableBetaCorrection);
		initialized = true;
	}
}

void RichRingXR::buildEvent(AMSEventR *pev) {
	if (!initialized) std::cerr << "[Warning] RichRingXR::buildEvent() running without Init()\n";
	pRing->buildEvent(pev);
	RichRingXR a=RichRingXR(pRing);
	*this=a;
}


#include "richrecOff.h"
#include "point.h"
void RichRingXR::ReconRingNpexp(unsigned int ir,AMSEventR *_event){
RichOffline::RichRing::_index=RI;
RichOffline::RichRing::_event=_event;
RichOffline::RichRing r(_event->TrTrack(fTrTrack));
RichRingR::Build(NULL);
r._beta=Beta;
r.ReconRingNpexp(3,0,ir+10);
Prob=r._probkl;
NpExp=r._npexp;
 for(int i=0;i<3;i++)EmissionPoint[i]=r._emission_p[i];
Theta=r._emission_d.gettheta();
Phi=r._emission_d.getphi();

 AMSPoint pnt=r.calcpmtpos();
// delete pointer (ownership problem)
  delete r.gettrack();
 
  PMTCrossPoint.resize(3);
  for(int i=0;i<3;i++)PMTCrossPoint[i]=pnt[i];



     bool counted[680]; 
     for(int i=0;i<680;counted[i++]=false); 
       PMTs=0; 
      for(int i=0;i<nRichHit();i++){
		 RichHitR &hit=_event->RichHit(iRichHit(i));
		  if(hit.IsCrossed()) continue;
		  int pmt=hit.Channel/16;
		  if(counted[pmt]) continue;
		  counted[pmt]=true;
		  PMTs++;
      }

}
