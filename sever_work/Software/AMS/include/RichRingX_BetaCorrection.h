// $Id: RichRingX_BetaCorrection.h,v 1.2 2025/05/23 16:04:27 jianan Exp $
//
//  RichRingX_BetaCorrection.h
//  RichRingXR
//
//  Created by Jianan Xiao on 2025/04/28.
//
#pragma once

// #undef EIGEN_HAS_STD_INVOKE_RESULT
// #define EIGEN_HAS_CXX11
#include <eigen3/Eigen/Dense>
#include <string>
#include <vector>

namespace RichRingXModules {

	class BetaCorrection {
	public:
		BetaCorrection() = delete;
		BetaCorrection(const BetaCorrection&) = delete;
		BetaCorrection& operator=(const BetaCorrection&) = delete;

		static double getThetaCorrection(double x, bool isNaF);
		static double getPhiCorrection(double x, bool isNaF);
		static double getTimeCorrection(double x, bool isNaF);

		static void initialize(const std::string& filePath);

	private:
		static const int nBins;
		static const double thetaLowerBound, thetaUpperBound;
		static const double phiLowerBound, phiUpperBound;
		static const double timeLowerBound, timeUpperBound;
		inline static std::vector<double> NaFThetaCorrectionXValues, NaFThetaCorrectionYValues;
		inline static std::vector<double> NaFPhiCorrectionXValues, NaFPhiCorrectionYValues;
		inline static std::vector<double> NaFTimeCorrectionXValues, NaFTimeCorrectionYValues;
		inline static std::vector<double> AglThetaCorrectionXValues, AglThetaCorrectionYValues;
		inline static std::vector<double> AglPhiCorrectionXValues, AglPhiCorrectionYValues;
		inline static std::vector<double> AglTimeCorrectionXValues, AglTimeCorrectionYValues;

		static void readCorrectionFromCSV(const std::string& filename, std::vector<double>& xValues, std::vector<double>& yValues);
		static double interpolateFunction(double x, const std::vector<double>& xValues, const std::vector<double>& yValues);

		inline static bool initialized = false;
	}; /* class BetaCorrection */
} /* namespace RichRingXModules */