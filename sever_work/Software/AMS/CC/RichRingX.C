//  $Id: RichRingX.C,v 1.17 2025/05/29 14:36:01 choutko Exp $
//
//  RichRingX.C
//  RichRingX
//
//  Created by Jianan Xiao on 2025/04/28.
//


#include "RichRingX.h"
#include "RichRingXModules.h"
#include "root.h"


#include <algorithm>
#include <cmath>
#include <cstring>
#include <eigen3/Eigen/Dense>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>
#include "TrChargeYJ.h"

char RichRingX::_Info[4096];
RichRingX::RichRingX(int iTrTrack) {
	fTrTrack = iTrTrack;
}

RichRingX::RichRingX(const RichRingX& ring) {
	m_time = ring.m_time;
	m_particleTrack = ring.m_particleTrack;
	m_isNaF = ring.m_isNaF;
	m_recBeta = ring.m_recBeta;
	m_recCosTheta = ring.m_recCosTheta;
	m_recCosTheta_npe = ring.m_recCosTheta_npe;
	m_recLikelihood = ring.m_recLikelihood;
	m_refractiveIndex = ring.m_refractiveIndex;
	m_correctionConstant = ring.m_correctionConstant;
	m_nHitsDirect = ring.m_nHitsDirect;
	m_nHitsReflect = ring.m_nHitsReflect;
	m_nHitsNoise = ring.m_nHitsNoise;
	m_nHits = ring.m_nHits;
	m_coneAngles = ring.m_coneAngles;
	m_emitAnglesTheta = ring.m_emitAnglesTheta;
	m_emitAnglesPhi = ring.m_emitAnglesPhi;
	m_hitType = ring.m_hitType;
	m_npe = ring.m_npe;
	m_hits = ring.m_hits;
	m_validHitsIndex = ring.m_validHitsIndex;
	m_isGoodRing = ring.m_isGoodRing;
	m_isCleanRing = ring.m_isCleanRing;
	m_isStrongSignal = ring.m_isStrongSignal;
	m_isAboveThreshold = ring.m_isAboveThreshold;
	m_signalStrength = ring.m_signalStrength;
	m_rich_beta_new = ring.m_rich_beta_new;
	fTrTrack = ring.fTrTrack;
	fRichHit = ring.fRichHit;
	m_coneCenter = ring.m_coneCenter;
}

RichRingX& RichRingX::operator=(const RichRingX& ring) {
	if (this != &ring) {
		m_time = ring.m_time;
		m_particleTrack = ring.m_particleTrack;
		m_isNaF = ring.m_isNaF;
		m_recBeta = ring.m_recBeta;
		m_recCosTheta = ring.m_recCosTheta;
		m_recCosTheta_npe = ring.m_recCosTheta_npe;
		m_recLikelihood = ring.m_recLikelihood;
		m_refractiveIndex = ring.m_refractiveIndex;
		m_correctionConstant = ring.m_correctionConstant;
		m_nHitsDirect = ring.m_nHitsDirect;
		m_nHitsReflect = ring.m_nHitsReflect;
		m_nHitsNoise = ring.m_nHitsNoise;
		m_nHits = ring.m_nHits;
		m_coneAngles = ring.m_coneAngles;
		m_emitAnglesTheta = ring.m_emitAnglesTheta;
		m_emitAnglesPhi = ring.m_emitAnglesPhi;
		m_hitType = ring.m_hitType;
		m_npe = ring.m_npe;
		m_hits = ring.m_hits;
		m_validHitsIndex = ring.m_validHitsIndex;
		m_isGoodRing = ring.m_isGoodRing;
		m_isCleanRing = ring.m_isCleanRing;
		m_isStrongSignal = ring.m_isStrongSignal;
		m_isAboveThreshold = ring.m_isAboveThreshold;
		m_signalStrength = ring.m_signalStrength;
		m_rich_beta_new = ring.m_rich_beta_new;
		fTrTrack = ring.fTrTrack;
		fRichHit = ring.fRichHit;
		m_coneCenter = ring.m_coneCenter;
	}
	return *this;
}

char* RichRingX::Info(int number) {
	std::ostringstream os;
	os << "RichRingX No " << number << ", Track = " << fTrTrack << "\n"
	   << "N_{hits} = " << m_nHitsDirect + m_nHitsReflect << ", #beta = " << m_recBeta << "\n"
	   << "#theta_C = " << (m_isNaF ? std::acos(m_recCosTheta) : std::acos(m_recCosTheta_npe)) << "\n";

	std::string outputInfo = os.str();
	std::strncpy(_Info, outputInfo.c_str(), sizeof(_Info));
	_Info[sizeof(_Info) - 1] = '\0';
	return _Info;
}

void RichRingX::initCorrection(const std::string& path, int ideal_mc, bool enableNCorrection, bool enablePMTCorrection, bool enableBetaCorrection) {
	RichRingX::enableNCorrection = enableNCorrection;
	RichRingX::enablePMTCorrection = enablePMTCorrection;
	RichRingX::enableBetaCorrection = enableBetaCorrection;
	if (!AMSEventR::Head()->nMCEventgC()) {
		RichConst::aglIndex=1.048;
		RichConst::xShift = 0.1;
		RichConst::yShift = -0.07;
		RichRingX::ISS=true;
		RichRingXModules::RefractiveIndexMap::initialize(path + "/indexMap_ISS.csv");
		RichRingXModules::BetaCorrection::initialize(path + "/");
		std::cout<<"RichRingX::initCorrection-I-ISS Initialized"<<std::endl;
	} else {
		RichConst::aglIndex=ideal_mc?1.0529:1.050;
		RichConst::xShift = 0;
		RichConst::yShift = 0;
		RichRingX::ISS=false;
		if (!ideal_mc) RichRingXModules::RefractiveIndexMap::initialize(path + "/indexMap_MC.csv");
		else RichRingXModules::RefractiveIndexMap::mapBins=0;
		RichRingXModules::BetaCorrection::initialize(path + "/MC_");
		std::cout<<"RichRingX::initCorrection-I-MC Initialized"<<std::endl;
	}
	RichRingXModules::HitsSelector::initialize(path + "/NpeCorr.root");
}

void RichRingX::buildEvent(AMSEventR *pev) {
	if(fTrTrack<0 || fTrTrack>=pev->nTrTrack()){
	std::cerr << "[Error] RichRingX::buildEvent() failed to assign TrTrackR pointer to the event. Aborting...\n";
	return;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - // Standard Correction and Hits Preselection
	initParams(pev);
	const auto [emissionPoint, coneCenter, trackDirection] = RichRingXModules::Geometry::getConeGeometry(m_particleTrack, m_isNaF);
	m_coneCenter.resize(3);
	for (int i = 0; i < 2; ++i) m_coneCenter[i] = coneCenter(i);
	m_coneCenter[2] = RichConst::radPos - RichConst::pmtPos;

	updateRefractiveIndex(emissionPoint, m_isNaF);
	updateBetaCorrection(trackDirection, m_isNaF, m_time);
	const auto [hits, npeWeight] = collectValidHits(pev);
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - // Ray Back-Tracing
	std::tie(m_emitAnglesTheta, m_emitAnglesPhi, m_coneAngles) = RichRingXModules::OpticalCorrection::computeAngles(hits, emissionPoint, coneCenter, m_isNaF, m_refractiveIndex);
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - // Discard Error Solution
	// Some solutions in coneAngles are resulted from large emit angles (basically close to total reflection).
	// We need to discard them in order to keep data clean.
	m_hitType.assign(hits.rows(), 0);
	RichRingXModules::HitsSelector::filterErrorSolution(m_coneAngles, m_emitAnglesTheta, m_hitType, m_isNaF);
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - // Denoise
	// Filter unwanted hits assigned to beta values that're out of range
	RichRingXModules::HitsSelector::filterErrorBeta(m_coneAngles, m_emitAnglesTheta, m_hitType, m_refractiveIndex, m_isNaF);
	int validHits = std::count(m_hitType.begin(), m_hitType.end(), static_cast<int>(HitTypes::Direct)) + std::count(m_hitType.begin(), m_hitType.end(), static_cast<int>(HitTypes::Reflect));
	if (validHits <  3) {
		// std::cout << "[Warning] RichRingX::buildEvent() no enough hits for reconstruction after discarding error beta\n";
		m_recCosTheta = std::numeric_limits<double>::infinity();
		m_recCosTheta_npe = std::numeric_limits<double>::infinity();
		m_recLikelihood = -std::numeric_limits<double>::infinity();
		m_recBeta = -1;
		return;
	}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - // Beta Reconstruction
	std::vector<double> recResult_default = RichRingXModules::AngleEstimator::estimateTheta(m_coneAngles, m_hitType, Eigen::VectorXd::Constant(m_hitType.size(), 1.0), m_isNaF);
	m_recCosTheta = recResult_default[0];
	m_recLikelihood = recResult_default[1];

	// Refit using npe signal
	std::vector<double> recResult_npe = RichRingXModules::AngleEstimator::estimateTheta(m_coneAngles, m_hitType, npeWeight, m_isNaF, std::acos(m_recCosTheta));
	m_recCosTheta_npe = recResult_npe[0];

	Eigen::VectorXd selectedHits(m_coneAngles.size());
	Eigen::VectorXd selectedHitsWeight = npeWeight;
	RichRingXModules::HitsSelector::selectHits(selectedHits, selectedHitsWeight, m_hitType, m_coneAngles, m_isNaF ? m_recCosTheta : m_recCosTheta_npe);

	m_nHitsDirect = std::count(m_hitType.begin(), m_hitType.end(), static_cast<int>(HitTypes::Direct));
	m_nHitsReflect = std::count(m_hitType.begin(), m_hitType.end(), static_cast<int>(HitTypes::Reflect));
	m_nHitsNoise = std::count(m_hitType.begin(), m_hitType.end(), static_cast<int>(HitTypes::Noise));
	for (size_t i = 0; i < m_nHits; ++i) {
		if (m_hitType[i] != static_cast<int>(HitTypes::Noise)) {
			m_validHitsIndex.push_back(i);
			fRichHit.push_back(m_hits[i]);
		}
	}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - // Fill Tree
	m_recBeta = 1.0 / m_refractiveIndex / (m_isNaF ? m_recCosTheta : m_recCosTheta_npe);
	if (RichRingX::enableBetaCorrection == true and RichRingX::ISS) m_recBeta /= m_correctionConstant;
	
	if (std::acos(m_recCosTheta) == 0) {
		// std::cout << "[Warning] RichRingX::buildEvent() reconstructed Cherenkov Angle at 0\n";
		m_recCosTheta = std::numeric_limits<double>::infinity();
		m_recCosTheta_npe = std::numeric_limits<double>::infinity();
		m_recLikelihood = -std::numeric_limits<double>::infinity();
		m_recBeta = -1;
		return;
	}
	updateQualityIndex(selectedHitsWeight, npeWeight, pev);
	return;
}

void RichRingX::initParams(AMSEventR *pev) {
TrTrackR& pTrack = pev->TrTrack(fTrTrack);
	m_time = pev->fHeader.Time[0];
	double z;
	AMSDir richdir;
	AMSPoint richp;

	std::vector<double> trackAgl(5);
	z = RichConst::radPos - RichConst::radHeight - RichConst::aglHeight / 2.0; // Agl center
	pTrack.Interpolate(z, richp, richdir);
	trackAgl[0] = richp.x();
	trackAgl[1] = richp.y();
	trackAgl[2] = richp.z();
	trackAgl[3] = richdir.gettheta();
	trackAgl[4] = richdir.getphi();
	RichRingXModules::Geometry::updateAlignment(trackAgl);

	std::vector<double> trackNaF(5);
	z = RichConst::radPos - RichConst::radHeight - RichConst::aglHeight + RichConst::NaFHeight / 2.0; // NaF center
	pTrack.Interpolate(z, richp, richdir);
	trackNaF[0] = richp.x();
	trackNaF[1] = richp.y();
	trackNaF[2] = richp.z();
	trackNaF[3] = richdir.gettheta();
	trackNaF[4] = richdir.getphi();
	RichRingXModules::Geometry::updateAlignment(trackNaF);

	m_isNaF = RichRingXModules::Geometry::updateRadiatorType(trackAgl, trackNaF);
	m_particleTrack = m_isNaF ? trackNaF : trackAgl;

	return;
}

std::tuple<Eigen::ArrayX3d, Eigen::VectorXd> RichRingX::collectValidHits(AMSEventR *head) {

	const unsigned int nHitsTot = std::min(head->nRichHit(), kMAXIMUM);

	Eigen::ArrayX3d hits(nHitsTot, 3);
	Eigen::VectorXd npeWeight(nHitsTot);

	unsigned int nUsed = 0;
	for (size_t i = 0; i < nHitsTot; ++i) {
		RichHitR* hit = head->pRichHit(i);
		if (hit->IsCrossed()) continue;

		const float* coo = hit->Coo;
		hits.row(nUsed) << coo[0], coo[1], 0.0;

		double w = hit->Npe;
		if (RichRingX::enablePMTCorrection and RichRingX::ISS) w /= RichRingXModules::HitsSelector::getPMTCorrectionFactor(hit->Channel);
		npeWeight(nUsed) = w;
		m_hits.push_back(i);
		m_npe.push_back(w);
		++nUsed;
	}

	m_nHits = nUsed;

	hits.conservativeResize(nUsed, Eigen::NoChange);
	npeWeight.conservativeResize(nUsed, Eigen::NoChange);
	return std::make_tuple(hits, npeWeight);
}


void RichRingX::updateRefractiveIndex(const Eigen::Vector3d& emissionPoint, bool isNaF) {
	if (RichRingX::enableNCorrection) {
		m_refractiveIndex = RichRingXModules::RefractiveIndexMap::interpolate(emissionPoint(0), emissionPoint(1), isNaF);
	} else {
		m_refractiveIndex = isNaF ? RichConst::NaFIndex : RichConst::aglIndex;
	}
}

void RichRingX::updateBetaCorrection(const Eigen::Vector2d& trackDirection, bool isNaF, unsigned int time) {
	m_correctionConstant = RichRingXModules::BetaCorrection::getThetaCorrection(trackDirection(0), isNaF)  * RichRingXModules::BetaCorrection::getPhiCorrection(trackDirection(1), isNaF) * RichRingXModules::BetaCorrection::getTimeCorrection(time, isNaF);
}

void RichRingX::updateQualityIndex(const Eigen::VectorXd& selectedHitsWeight, const Eigen::VectorXd& npeWeight,  AMSEventR *pev) {
	TrTrackR& pTrack = pev->TrTrack(fTrTrack);
	m_signalStrength = selectedHitsWeight.sum() / npeWeight.sum();
	if (!(pev->nMCEventgC())) {
		m_isGoodRing = m_isNaF ? true : (std::acos(m_recCosTheta_npe) > 0.06 and std::acos(m_recCosTheta_npe) < 0.312); // if cherenkov angle size is acceptable
		m_isStrongSignal = m_signalStrength > 0.2;
	} else {
		m_isGoodRing = m_isNaF ? true : (std::acos(m_recCosTheta_npe) > 0.02 and std::acos(m_recCosTheta_npe) < 0.35); // if cherenkov angle size is acceptable
		m_isStrongSignal = m_signalStrength > 0.4;
	}

	double tofBeta = 1;
	m_isAboveThreshold = true;
	double trRigidity = pTrack.GetRigidity(0, -70); // RICH rigidity
	size_t iTofBeta = -1;
	for (size_t iBeta = 0; iBeta <pev->nBeta(); ++iBeta) {
		if (pev->pBeta(iBeta)->iTrTrack() == fTrTrack) {
			iTofBeta = iBeta;
			break;
		}
	}

	if (iTofBeta >= 0 && iTofBeta<pev->nBeta()) {
		BetaR &pBeta = pev->Beta(iTofBeta);
		tofBeta = pBeta.Beta;
		m_isAboveThreshold = !(tofBeta * m_refractiveIndex < 1.01 and trRigidity < 7.0);
	}

	double trInnerQ=0;
	if(TrChargeYJTimeDB::GetHead()->pelem){
		TrQYJTrack trQYJ = pTrack.GetQYJ_all(2, tofBeta, 0);
		trInnerQ = trQYJ.InnerQ;
	} else trInnerQ = pTrack.GetInnerQ(tofBeta);

	float noiseCutOff = m_isNaF ? 0.2 : std::max(0.12 - trInnerQ * 0.00625, 0.02); // To avoid discarding reconstructed highZ events (which have a very large Noise level)
	m_isCleanRing = ((m_nHitsDirect + m_nHitsReflect) / static_cast<float>(m_nHits) > noiseCutOff); // if Noise level is acceptable
	return;
}
