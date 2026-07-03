//  $Id: RichRingXR.h,v 1.22 2025/05/29 14:36:06 choutko Exp $
//  
//  RichRingXR.h
//  RichRingXR
//
//  Created by Jianan Xiao on 2025/04/28.
//

#ifndef __RICHRINGXR__
#define __RICHRINGXR__
#pragma once

#include <vector>
#include "Rtypes.h"
class RichRingX;
class AMSEventR;

//!  RichRingXR Structure
/*!

   \author jianan@cern.ch
*/

class RichRingXR{
public:
	static bool enableNCorrection; ///< Use calibrated refractive index map n(x, y) or default uniform value
	static bool enablePMTCorrection; ///< Use calibrated PMT response correction
	static bool enableBetaCorrection; ///< Use beta uniformity correction (corrects beta to 1 for high rigidity)
	static int create;     ///< Create rinch ring for minimum charge > create/100 or don't create (0)
	static int mthr; ///< Max Thread number (0 == proc_number)
	static int ideal_mc; ///< =1 if ideal_mc (Default)
	static int dphi;
	static int dz;
        static int &sdphi(int d=0);
        static int &sdz(int d=0);
	RichRingXR() {};
	RichRingXR(int iTrTrack);
	virtual ~RichRingXR() {};

	RichRingXR(RichRingX* pRing);
	RichRingXR(const RichRingXR& handle);
	RichRingXR& operator=(const RichRingXR& handle);
        int  getPMTs(){return PMTs;}
        int getdphi(){return (Status>>6)%4096;}
	void setdphi(){Status|= (sdphi()%4096)<<6;}
	char* Info(int n = -1);

	static void init(const char* path,int ideal_mc,int thr=-1);
        void ReconRingNpexp(unsigned int ir,AMSEventR*pev);
	void buildEvent(AMSEventR* pev);
	int getHitType(int i);
	int iTrTrack() const {return fTrTrack;}
	int nRichHit() const{ return fRichHit.size();}
	int iRichHit(unsigned int i) const {return i >= fRichHit.size() ? -1 : fRichHit[i];}
	bool checkstatus(int checker) const {return (Status & checker) != 0;}
	double getBeta() const {return Beta;}
	double getRefractiveIndex()const {return RI;}
	bool isGood() const {return checkstatus(1);}
	bool isClean() const {return checkstatus(16);}
	bool isAboveThreshold()const {return checkstatus(4);}
	bool isStrongSignal()const {return checkstatus(8);}
	bool isNaF() const {return checkstatus(2);}
	bool isUsed()const {return checkstatus(32);}
	unsigned int getNumHitsUsed() const {return Used;}
	unsigned int getNumReflectedHitsUsed() const {return UsedM;}
	double getPhotoElectrons() const {return NpTot;}
	double getCollectedPhotoElectrons() const {return NpCol;}
	double getTrackTheta() const {return Theta;}
	double getTrackPhi() const {return Phi;}

	double getCorrectionConstant() const {return CorrectionConstant;}
	double getTheta() const {return ThetaC;}
	double getThetaRefit() const {return ThetaC_npe;}
	double getLikelihood() const {return Likelihood;}
	std::vector<float> getTrackEmissionPoint() const {return EmissionPoint;}
	std::vector<float> getTrackPMTCrossPoint() const {return PMTCrossPoint;}

protected:
	int fTrTrack; ///< index of TrTrackR in collection
	std::vector<int> fRichHit; ///< indices of RichHitR in collection
	ClassDef(RichRingXR, 5);
private:
	static bool initialized;
	static char _Info[4096];
	RichRingX* pRing; //!
public:
	double Beta;  ///< Beta
	unsigned int Status;     ///< status word
	/*!<
	Ring status bits (counting from 1 to 32)
	1 - isGood() 
	2 - isNaF() Ring reconstructed using the NaF radiator in the double radiator configuration
	3 - isAboveThreshold()
	4 - is StrongSignal()
	5 - isClean()
	6 - Associated to a particle
	7-18 - ::dphi used to create a ring
	*/

	int Used; ///< Nb. of RICH hits in the ring cluste
	int UsedM; ///< Nb. of RICH reflected hits in the ring cluster
	float NpCol; ///< number of photoelectron in the ring getCollectedPhotoElectrons()
	float NpTot; ///< total number of photoelectrons getPhotoElectrons()
	float Theta; ///< track theta getTrackTheta();
	float Phi; ///< track phi getTrackPhi
	float RI; ///< Refraction index
	float Prob; ///< Kolmogorov test probability to be a good ring based on azimuthal distribution
	float NpExp;      ///< Number of expected photoelectrons for Z=1 charge
	int PMTs; ///< number of pmt's in the ring

	double CorrectionConstant; ///< Beta uniformity correction constant
	double ThetaC; ///< Reconstructed Cherenkov angle
	double ThetaC_npe; ///< Reconstructed Cherenkov angle with refit using number of photons
	double Likelihood; ///< Likelihood from Maximum Likelihood Estimation of reconstructed Cherenkov angle
	std::vector<float> EmissionPoint; ///< Emission point of incident particle inside radiator
	std::vector<float> PMTCrossPoint; ///< Pmt Cross Point
	friend class AMSEventR;
	
#ifdef VERSION6
#else
#pragma omp threadprivate(fgIsA)
#endif
};

#if !defined(__CLING__)          &&  \
	!defined(__ROOTCLING__)  &&  \
	!defined(__CINT__)       &&  \
	!defined(__MAKECINT__)
 // #include "RichRingX.h"
#endif 
#endif /* __RICHRINGXR__*/
