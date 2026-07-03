// $Id: RichRingX_BetaCorrection.C,v 1.3 2025/05/23 16:03:57 jianan Exp $
//
//  RichRingX_BetaCorrection.C
//  RichRingXR
//
//  Created by Jianan Xiao on 2025/04/28.
//

#include "RichRingX_BetaCorrection.h"

#include <algorithm>
// #undef EIGEN_HAS_STD_INVOKE_RESULT
// #define EIGEN_HAS_CXX11
#include <eigen3/Eigen/Dense>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace RichRingXModules {

	const int    BetaCorrection::nBins = 60;
	const double BetaCorrection::thetaLowerBound = 0.0;
	const double BetaCorrection::thetaUpperBound = 1.0;
	const double BetaCorrection::phiLowerBound = -3.142;
	const double BetaCorrection::phiUpperBound = 3.142;
	const double BetaCorrection::timeLowerBound = 1305853512;
	const double BetaCorrection::timeUpperBound = 1700000000;

	double BetaCorrection::getThetaCorrection(double x, bool isNaF) {
		double correctionConstant;
		if (isNaF) {
			correctionConstant = interpolateFunction(x, NaFThetaCorrectionXValues, NaFThetaCorrectionYValues);
		} else {
			correctionConstant = interpolateFunction(x, AglThetaCorrectionXValues, AglThetaCorrectionYValues);
		}
		return correctionConstant;
	}

	double BetaCorrection::getPhiCorrection(double x, bool isNaF) {
		double correctionConstant;
		if (isNaF) {
			correctionConstant = interpolateFunction(x, NaFPhiCorrectionXValues, NaFPhiCorrectionYValues);
		} else {
			correctionConstant = interpolateFunction(x, AglPhiCorrectionXValues, AglPhiCorrectionYValues);
		}
		return correctionConstant;
	}

	double BetaCorrection::getTimeCorrection(double x, bool isNaF) {
		double correctionConstant;
		if (isNaF) {
			correctionConstant = interpolateFunction(x, NaFTimeCorrectionXValues, NaFTimeCorrectionYValues);
		} else {
			correctionConstant = interpolateFunction(x, AglTimeCorrectionXValues, AglTimeCorrectionYValues);
		}
		return correctionConstant;
	}

	void BetaCorrection::initialize(const std::string& filePath) {
		if (initialized) {
			// std::cerr << "[Warning] RichRingXModules::BetaCorrection::initialize() called more than once. Ignoring...\n";
			return;
		}
		readCorrectionFromCSV(filePath + "NaFThetaCorrection.csv", NaFThetaCorrectionXValues, NaFThetaCorrectionYValues);
		readCorrectionFromCSV(filePath + "NaFPhiCorrection.csv", NaFPhiCorrectionXValues, NaFPhiCorrectionYValues);
		readCorrectionFromCSV(filePath + "NaFTimeCorrection.csv", NaFTimeCorrectionXValues, NaFTimeCorrectionYValues);

		readCorrectionFromCSV(filePath + "AglThetaCorrection.csv", AglThetaCorrectionXValues, AglThetaCorrectionYValues);
		readCorrectionFromCSV(filePath + "AglPhiCorrection.csv", AglPhiCorrectionXValues, AglPhiCorrectionYValues);
		readCorrectionFromCSV(filePath + "AglTimeCorrection.csv", AglTimeCorrectionXValues, AglTimeCorrectionYValues);

		initialized = true;
		return;
	}

	void BetaCorrection::readCorrectionFromCSV(const std::string& filename,
						   std::vector<double>& xValues,
						   std::vector<double>& yValues) {
		std::ifstream csvFile(filename);
		if (!csvFile.is_open()) {
			std::cerr << "BetaCorrection :: Error! Cannot open CSV file " << filename << std::endl;
			return;
		}

		std::string line;
		std::getline(csvFile, line);

		while (std::getline(csvFile, line)) {
			std::stringstream ss(line);
			std::string xStr, yStr;
			std::getline(ss, xStr, ',');
			std::getline(ss, yStr, ',');

			double x = std::stod(xStr);
			double y = std::stod(yStr);

			xValues.push_back(x);
			yValues.push_back(y);
		}
		csvFile.close();
	}

	double BetaCorrection::interpolateFunction(double x,
						   const std::vector<double>& xValues,
						   const std::vector<double>& yValues) {
		if (xValues.empty()) {
			// std::cerr << "BetaCorrection :: Error! Empty vector. Cannot interpolate." << std::endl;
			return 1.0;
		}

		if (x <= xValues.front()) {
			return yValues.front();
		} else if (x >= xValues.back()) {
			return yValues.back();
		}

		auto it = std::lower_bound(xValues.begin(), xValues.end(), x);
		size_t idx = std::distance(xValues.begin(), it);

		// Linear interpolate
		double x0 = xValues[idx - 1];
		double x1 = xValues[idx];
		double y0 = yValues[idx - 1];
		double y1 = yValues[idx];

		double t = (x - x0) / (x1 - x0);
		return y0 + t * (y1 - y0);
	}
} /* namespace RichRingXModules */
