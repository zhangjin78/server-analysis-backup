//  $Id: RichRingX.h,v 1.10 2025/05/23 16:04:27 jianan Exp $
//  
//  RichRingX.h
//  RichRingXR
//
//  Created by Jianan Xiao on 2025/04/28.
//
#pragma once

#include <eigen3/Eigen/Dense>
#include <cmath>
#include <iostream>
#include <limits>
#include <numeric>
#include <string>
#include <tuple>
#include <vector>
#include "link.h"
#include "Rtypes.h"
class AMSEventR;
class TrTrackR;
class RichRingX {
public:
	inline static bool ISS = false;
	inline static bool enableNCorrection = true; ///< Use calibrated refractive index map n(x, y) or default uniform value
	inline static bool enablePMTCorrection = true; ///< Use calibrated PMT response correction
	inline static bool enableBetaCorrection = true; ///< Use beta uniformity correction (corrects beta to 1 for high rigidity)

	RichRingX() {};
	RichRingX(int pTrack);
	virtual ~RichRingX() {};

	RichRingX(const RichRingX& ring);
	RichRingX& operator=(const RichRingX& ring);

	char* Info(int number = -1);

	static void initCorrection(const std::string& path, int ideal_mc, 
				   bool enableNCorrection = true, 
				   bool enablePMTCorrection = true, 
				   bool enableBetaCorrection = true);
	
	void buildEvent(AMSEventR *pev);

	int iTrTrack() const {return fTrTrack;}
	unsigned int iRichHit(unsigned int i) const {
		if (i >= m_validHitsIndex.size()) return std::numeric_limits<unsigned int>::max();
		return m_validHitsIndex[i];
	}

	double getBeta() const {return m_recBeta;}
	double getRefractiveIndex() const {return m_refractiveIndex;}
	double getCorrectionConstant() const {return m_correctionConstant;}
	double getTheta() const {return std::acos(m_recCosTheta);}
	double getThetaRefit() const {return std::acos(m_recCosTheta_npe);}
	double getLikelihood() const {return m_recLikelihood;}
	bool isGood() const {return (m_isGoodRing and (m_nHitsDirect + m_nHitsReflect) > 2);}
	bool isClean() const {return (m_isCleanRing and m_recLikelihood > 10);}
	bool isAboveThreshold() const {return m_isAboveThreshold;}
	bool isStrongSignal() const {return m_isStrongSignal;}
	bool isNaF() const {return m_isNaF;}
	unsigned int getNumHitsUsed() const {return m_nHitsDirect + m_nHitsReflect;}
	unsigned int getNumReflectedHitsUsed() const {return m_nHitsReflect;}
	double getPhotoElectrons() const {
		double sum = 0.0;
		for (size_t i = 0; i < m_nHits; ++i) sum += m_npe[i];
		return sum;
	}
	double getCollectedPhotoElectrons() const {
		double sum = 0.0;
		for (auto i : m_validHitsIndex) sum += m_npe[i];
		return sum;
	}
	std::vector<float> getTrackEmissionPoint() const {return { static_cast<float>(m_particleTrack[0]),  static_cast<float>(m_particleTrack[1]),  static_cast<float>(m_particleTrack[2])};};
	std::vector<float> getPMTCrossPoint() const {return std::vector<float> (m_coneCenter.begin(), m_coneCenter.end());}
	double getTrackTheta() const {return m_particleTrack[3];}
	double getTrackPhi() const {return m_particleTrack[4];}

protected:
	int fTrTrack = -1; ///< index of TrTrackR in collection
	std::vector<int> fRichHit; ///< indices of RichHitR in collection
	// ClassDef(RichRingX, 1)

private:

	static char _Info[4096];
	unsigned int m_time = 0;

	std::vector<double> m_particleTrack = {};
	bool m_isNaF = false;

	double m_recBeta = -1;
	double m_recCosTheta = -1;
	double m_recCosTheta_npe = -1;
	double m_recLikelihood = -1;
	double m_refractiveIndex = -1;
	double m_correctionConstant = -1;

	unsigned int m_nHitsDirect = 0;
	unsigned int m_nHitsReflect = 0;
	unsigned int m_nHitsNoise = 0;

	int m_nHits = -1;
	std::vector<std::vector<double>> m_coneAngles = {};
	std::vector<std::vector<double>> m_emitAnglesTheta = {};
	std::vector<std::vector<double>> m_emitAnglesPhi = {};
	std::vector<int> m_hitType = {};
	std::vector<double> m_npe = {};
	std::vector<int> m_hits = {}; // Indices of RichHitR in collection
	std::vector<int> m_validHitsIndex = {};

	std::vector<double> m_coneCenter = {};

	bool m_isGoodRing = false;
	bool m_isCleanRing = false;
	bool m_isStrongSignal = false;
	bool m_isAboveThreshold = false;
	double m_signalStrength = -1;
	double m_rich_beta_new = -1;

	void initParams(AMSEventR *pev);
	std::tuple<Eigen::ArrayX3d, Eigen::VectorXd> collectValidHits(AMSEventR *pev);

	void updateRefractiveIndex(const Eigen::Vector3d& emissionPoint, bool isNaF);
	void updateBetaCorrection(const Eigen::Vector2d& trackDirection, bool isNaF, unsigned int time);
	void updateQualityIndex(const Eigen::VectorXd& selectedHitsWeight, const Eigen::VectorXd& npeWeight, AMSEventR *pev);

	friend class AMSEventR;
	friend class RichRingXR;
}; /* class RichRingX */
