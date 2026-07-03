// $Id: RichRingX_HitsSelector.h,v 1.2 2025/05/23 16:04:27 jianan Exp $
//
//  RichRingX_HitsSelector.h
//  RichRingXR
//
//  Created by Jianan Xiao on 2025/04/28.
//
#pragma once

#include "TGraph.h"

// #undef EIGEN_HAS_STD_INVOKE_RESULT
// #define EIGEN_HAS_CXX11
#include <eigen3/Eigen/Dense>
#include <string>
#include <utility>
#include <vector>

namespace RichRingXModules {
	class HitsSelector {
	public:
		HitsSelector() = delete;
		HitsSelector(const HitsSelector&) = delete;
		HitsSelector& operator=(const HitsSelector&) = delete;
		
		// Hits Selection
		static double getPMTCorrectionFactor(int channel);
		static void filterErrorSolution(std::vector<std::vector<double>>& coneAngles, std::vector<std::vector<double>>& emitAnglesTheta, std::vector<int>& hitType, bool isNaF);
		static void filterErrorBeta(std::vector<std::vector<double>>& coneAngles, std::vector<std::vector<double>>& emitAnglesTheta, std::vector<int>& hitType, double refractiveIndex, bool isNaF);
		static std::pair<int, double> selectBestSolution(const std::vector<double>& hitSolutions, int hitType, double theta_i);
		static void selectHits(Eigen::VectorXd& selectedHits, 
				       Eigen::VectorXd& selectedHitsWeight, 
				       std::vector<int>& hitType, 
				       const std::vector<std::vector<double>>& coneAngles, 
				       double recCosTheta);

		static void initialize(const std::string& filename);
	private:
		inline static TGraph* gCell = nullptr;
		inline static bool initialized = false;
	}; // class HitsSelector
} /* namespace RichRingXModules */