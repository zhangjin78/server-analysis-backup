// $Id: RichRingX_HitsSelector.C,v 1.2 2025/05/23 16:03:57 jianan Exp $
//
//  RichRingX_HitsSelector.C
//  RichRingXR
//
//  Created by Jianan Xiao on 2025/04/28.
//

#include "RichRingX_HitsSelector.h"
#include "RichRingX_RichConst.h"

#include "TFile.h"
#include "TGraph.h"

#include <cmath>
// #undef EIGEN_HAS_STD_INVOKE_RESULT
// #define EIGEN_HAS_CXX11
#include <eigen3/Eigen/Dense>
#include <iostream>
#include <limits>
#include <tuple>
#include <vector>

namespace RichRingXModules {

	double HitsSelector::getPMTCorrectionFactor(int channel) {
		return gCell->Eval(static_cast<double>(channel));
	}

	void HitsSelector::filterErrorSolution(std::vector<std::vector<double>>& coneAngles, std::vector<std::vector<double>>& emitAnglesTheta, std::vector<int>& hitType, bool isNaF) {
		for (int i = coneAngles.size() - 1; i >= 0; --i) {
			for (int j = coneAngles[i].size() - 1; j >= 1; --j) {
				if (emitAnglesTheta[i][j] < (isNaF ? 0.81 : 1.32)) continue;
				coneAngles[i].erase(coneAngles[i].begin() + j);
				emitAnglesTheta[i].erase(emitAnglesTheta[i].begin() + j);
			}

			if (emitAnglesTheta[i][0] > (isNaF ? 0.81 : 1.32)) {
				if (coneAngles[i].size() == 1)  {
					hitType[i] = static_cast<int>(HitTypes::Noise);
				} else {
					hitType[i] = static_cast<int>(HitTypes::Reflect);
				}
			}
		}
		return;
	}

	void HitsSelector::filterErrorBeta(std::vector<std::vector<double>>& coneAngles, std::vector<std::vector<double>>& emitAnglesTheta, std::vector<int>& hitType, double refractiveIndex, bool isNaF) {
		// const double maxBeta = 1.038;
		// const double minBeta = (1.0 + RichConst::betaThreshold * (refractiveIndex - 1.0)) / refractiveIndex; // Defined by Casaus in richrec.C

		double minBeta, maxBeta;

		maxBeta = isNaF ? 1.02 : 1.01;
		minBeta = isNaF ? 0.7621835 : 0.94;
		refractiveIndex = isNaF ? RichConst::NaFIndex : RichConst::aglIndex;

		for (int i = coneAngles.size() - 1; i >= 0; --i) {
			if (hitType[i] == static_cast<int>(HitTypes::Noise)) continue;
			// Always keep the Direct solution, and discard Reflect solution that has its value too large
			for (int j = coneAngles[i].size() - 1; j >= 1; --j) {
				const double beta_j = 1.0 / refractiveIndex / std::cos(coneAngles[i][j]);
				if (beta_j > minBeta and beta_j < maxBeta) continue;
				coneAngles[i].erase(coneAngles[i].begin() + j);
				emitAnglesTheta[i].erase(emitAnglesTheta[i].begin() + j);
			}

			const double beta_0 = 1.0 / refractiveIndex / std::cos(coneAngles[i][0]);
			if (beta_0 < minBeta or beta_0 > maxBeta) {
				if (coneAngles[i].size() == 1) {
					hitType[i] = static_cast<int>(HitTypes::Noise);
				} else {
					hitType[i] = static_cast<int>(HitTypes::Reflect);
				}
			}
		}
		return;
	}

	std::pair<int, double> HitsSelector::selectBestSolution(const std::vector<double>& hitSolutions, int hitType, double theta_i) {
		// return the solution col index and its deviation value from a given theta_i
		double minDeviation = std::numeric_limits<double>::infinity();
		int colIndex = 0;
		const int nCols = hitSolutions.size();
		for (int i = (hitType == static_cast<int>(HitTypes::Direct) ? 0 : 1); i < nCols; ++i) {
			const double deviation = hitSolutions[i] - theta_i;
			if (std::fabs(minDeviation) > std::fabs(deviation)) {
				minDeviation = deviation;
				colIndex = i;
			}
		}
		return {colIndex, minDeviation};
	}

	void HitsSelector::selectHits(Eigen::VectorXd& selectedHits, Eigen::VectorXd& selectedHitsWeight, std::vector<int>& hitType, const std::vector<std::vector<double>>& coneAngles, double recCosTheta) {
		// Assign hits to different types
		const int nRows = coneAngles.size();
		for (int iRow = 0; iRow < nRows; ++iRow) {
			selectedHits[iRow] = 0;
			if (hitType[iRow] == static_cast<int>(HitTypes::Noise)) {
				selectedHitsWeight[iRow] = 0;
				continue;
			}

			double minDeviation = std::numeric_limits<double>::infinity();
			int colIndex = selectBestSolution(coneAngles[iRow], hitType[iRow], std::acos(recCosTheta)).first;
			selectedHits[iRow] = coneAngles[iRow][colIndex];
			if (colIndex != 0) hitType[iRow] = static_cast<int>(HitTypes::Reflect);
			if (std::fabs(selectedHits[iRow] - std::acos(recCosTheta)) > 0.011) {
				selectedHitsWeight[iRow] = 0;
				hitType[iRow] = static_cast<int>(HitTypes::Noise);
			}
			// If the hit has been reflected, apply mirror reflectivity correction
			if (hitType[iRow] == static_cast<int>(HitTypes::Reflect)) selectedHitsWeight[iRow] /= RichConst::mirReflectivity;
		}
	}

	void HitsSelector::initialize(const std::string& filename) {
		if (initialized) {
			// std::cerr << "[Warning] RichRingXModules::HitsSelector::initialize() called more than once. Ignoring...\n";
			return;
		}
		TFile* pmtCorrectionFile = new TFile(filename.c_str(), "READ");
		gCell = (TGraph*) pmtCorrectionFile->Get("g_cell");

		initialized = true;
		return;
	}
} /* namespace RichRingXModules */