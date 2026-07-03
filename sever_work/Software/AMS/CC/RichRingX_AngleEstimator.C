// $Id: RichRingX_AngleEstimator.C,v 1.4 2025/05/23 16:03:57 jianan Exp $
//
//  RichRingX_AngleEstimator.C
//  RichRingXR
//
//  Created by Jianan Xiao on 2025/04/28.
//

#include "RichRingX_AngleEstimator.h"
#include "RichRingX_RichConst.h"
#include "RichRingX_HitsSelector.h"

#include <cmath>
#include <cstddef>
// #undef EIGEN_HAS_STD_INVOKE_RESULT
// #define EIGEN_HAS_CXX11
#include <eigen3/Eigen/Dense>
#include <limits>
#include <random>
#include <tuple>
#include <utility>
#include <vector>


namespace RichRingXModules {
	double AngleEstimator::thetaPDF(double x, bool isNaF) {
		return isNaF ? thetaPDF_NaF(x) : thetaPDF_Agl(x);
	}

	double AngleEstimator::evaluateLikelihood(const std::vector<std::vector<double>>& coneAngles, const std::vector<int>& hitType, const Eigen::VectorXd& npeWeight, double theta_i, bool isNaF) {
		double score = 0;
		const int nRows = coneAngles.size();
#ifdef __AROOTSHAREDLIBRARY__
#pragma omp parallel for reduction(+:score)
#endif
		for (size_t iRow = 0; iRow < nRows; ++iRow) {
			if (hitType[iRow] == static_cast<int>(HitTypes::Noise)) continue;
			double f1 = HitsSelector::selectBestSolution(coneAngles[iRow], hitType[iRow], theta_i).second;
			score += npeWeight[iRow] * log(thetaPDF(f1, isNaF));
		}

		return score;
	}

	std::pair<std::vector<double>, std::vector<double>> AngleEstimator::scanLikelihood(const std::vector<std::vector<double>>& coneAngles, const std::vector<int>& hitType, const Eigen::VectorXd& npeWeight, bool isNaF, double minTheta, double maxTheta) {
		const int nSteps = 1000;
		const double stepSize = (maxTheta - minTheta) / nSteps;

		std::vector<double> likelihoodX(nSteps);
		std::vector<double> likelihoodY(nSteps);

		const int nRows = coneAngles.size();
#ifdef __AROOTSHAREDLIBRARY__         
#pragma omp parallel for
#endif
		for (size_t i = 0; i < nSteps; ++i) {
			likelihoodX[i] = minTheta + i * stepSize;
			likelihoodY[i] = evaluateLikelihood(coneAngles, hitType, npeWeight, likelihoodX[i], isNaF);
		}

		return {likelihoodX, likelihoodY};
	}

	std::vector<int> AngleEstimator::findPeaks(const std::vector<double>& y) {
	    std::vector<int> peakIndices;
	    int n = y.size();
	    
	    for (size_t i = 1; i < n - 1; ++i) {
	        if (y[i] > y[i-1] && y[i] > y[i+1]) {
	            peakIndices.push_back(i);
	        }
	    }
	    
	    return peakIndices;
	}

	std::vector<double> AngleEstimator::estimateTheta(const std::vector<std::vector<double>>& coneAngles, const std::vector<int>& hitType, const Eigen::VectorXd& npeWeight, bool isNaF, double refitTheta) {
		double minTheta = refitTheta == 0 ? 0 : refitTheta - 0.02;
		double maxTheta = refitTheta == 0 ? (isNaF ? 0.8 : 0.55) : refitTheta + 0.02;
		auto [likelihoodX, likelihoodY] = scanLikelihood(coneAngles, hitType, npeWeight, isNaF, minTheta, maxTheta);
		std::vector<int> peakIndices = findPeaks(likelihoodY);

		double bestY = -std::numeric_limits<double>::infinity();
		double bestX = 0;
		for (auto iPeak : peakIndices) {
			if (likelihoodY[iPeak] > bestY) {
				bestY = likelihoodY[iPeak];
				bestX = likelihoodX[iPeak];
			}
		}

		// Random walking to refine the result
		std::random_device rd;
		std::mt19937 gen(rd());

		size_t isMinimaCount = 0;
		std::normal_distribution<> normalDist(0.0, 5e-3);
		for (size_t iWalk = 0; iWalk < 20000 and isMinimaCount < 1000; ++iWalk) {
			if (iWalk == 2501) normalDist = std::normal_distribution<>(0.0, 2e-3);
			double theta_i = bestX + normalDist(gen);
			double score = evaluateLikelihood(coneAngles, hitType, npeWeight, theta_i, isNaF);

			if (score > bestY) {
				bestY = score;
				bestX = theta_i;
				isMinimaCount = 0;
			} else {
				++isMinimaCount;
			}
		}
	    
		// In case refit fails (which is likely to happen)
		if(refitTheta != 0 and std::abs(refitTheta - bestX) > 0.015) bestX = refitTheta;
		return {std::cos(bestX), bestY};
	}

	double AngleEstimator::gaussPlusConst(double x, double gaussBase, double sigma) {
		// Make sure that the integral in (-0.1, 0.1) is 1
		if (std::fabs(x) > 0.1) x = 0.1;
		const double ratio = 0.2 * gaussBase;
		const double scale = (1.0 - ratio) / sqrt(2.0 * M_PI) / sigma;
		const double mean = 0.0;
		return scale * std::exp(-0.5 * pow((x - mean) / sigma, 2)) + gaussBase;
	}

	double AngleEstimator::thetaPDF_NaF(double x) {
		double gaussBase = 2.0;
		double sigma = 0.00700451;
		return gaussPlusConst(x, gaussBase, sigma);
	}

	double AngleEstimator::thetaPDF_Agl(double x) {
		double gaussBase = 2.0;
		double sigma = 0.00700451;
		return gaussPlusConst(x, gaussBase, sigma);
	}
} /* namespace RichRingXModules */
