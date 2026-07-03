// $Id: RichRingX_RefractiveIndexMap.C,v 1.3 2025/05/23 16:03:58 jianan Exp $
//
//  RichRingX_RefractiveIndexMap.C
//  RichRingXR
//
//  Created by Jianan Xiao on 2025/04/28.
//

#include "RichRingX_RefractiveIndexMap.h"
#include "RichRingX_RichConst.h"

#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace RichRingXModules {
	int RefractiveIndexMap::mapBins = 800;
	const double RefractiveIndexMap::mapRange = 60.0;
	const double RefractiveIndexMap::binLength = 2.0 * RefractiveIndexMap::mapRange / RefractiveIndexMap::mapBins;

	double RefractiveIndexMap::interpolate(double x, double y, bool isNaF) {

		double i_f = (x + mapRange) / binLength - 0.5;
		double j_f = (y + mapRange) / binLength - 0.5;

		int i0 = static_cast<int>(std::floor(i_f));
		int j0 = static_cast<int>(std::floor(j_f));
		int i1 = i0 + 1;
		int j1 = j0 + 1;

		if (i0 < 0 || i1 >= mapBins || j0 < 0 || j1 >= mapBins) {
			// std::cerr << "RefractiveIndexMap :: Error! Index(x, y) out of range: x=" << x << ", y=" << y << std::endl;
			return isNaF ? RichConst::NaFIndex : RichConst::aglIndex;
		}

		// The value of four neighbours
		double v[4] = {
			mapValues[i0][j0], // v00
			mapValues[i0][j1], // v01
			mapValues[i1][j0], // v10
			mapValues[i1][j1]  // v11
		};

		// Calculate weight
		double dx = i_f - i0;
		double dy = j_f - j0;

		double w[4] = {
			(1 - dx) * (1 - dy),
			(1 - dx) * dy,
			dx * (1 - dy),
			dx * dy
		};

		double interpolatedValue = 0.0;
		double sumWeights = 0.0;

		const double maxn = isNaF ? 1.35 : 1.06;
		const double minn = isNaF ? 1.30 : 1.03;
		for (int k = 0; k < 4; ++k) {
			if (v[k] > minn and v[k] < maxn) {
				interpolatedValue += w[k] * v[k];
				sumWeights += w[k];
			}
		}

		if (sumWeights == 0) {
			// std::cerr << "RefractiveIndexMap :: Error! No available points for interpolation" << std::endl;
			return isNaF ? RichConst::NaFIndex : RichConst::aglIndex;
		}

		interpolatedValue /= sumWeights;

		if (interpolatedValue < 1.0 or interpolatedValue > 1.6) 
			return isNaF ? RichConst::NaFIndex : RichConst::aglIndex;
		return interpolatedValue;
	}

	void RefractiveIndexMap::initialize(const std::string& filename) {
		if (initialized) {
			// std::cerr << "[Warning] RichRingXModules::RefractiveIndexMap::initialize() called more than once. Ignoring...\n";
			return;
		}

		mapValues.resize(mapBins, std::vector<double>(mapBins, 0));
		std::ifstream csvFile(filename);
		if (!csvFile.is_open()) {
			std::cerr << "[Warning] RichRingXModules::RefractiveIndexMap Error! Cannot Open File " << filename << std::endl;
			return;
		}

		std::string line;
		std::getline(csvFile, line); // Skip header row

		while (std::getline(csvFile, line)) {
			std::stringstream ss(line);
			std::string xStr, yStr, valueStr;
			std::getline(ss, xStr, ',');
			std::getline(ss, yStr, ',');
			std::getline(ss, valueStr, ',');

			double x = std::stod(xStr);
			double y = std::stod(yStr);
			double value = std::stod(valueStr);

			int i = static_cast<int>((x + mapRange) / binLength);
			int j = static_cast<int>((y + mapRange) / binLength);

			if (i >= 0 && i < mapBins && j >= 0 && j < mapBins) {
				mapValues[i][j] = value;
			} else {
				std::cerr << "[Warning] RichRingXModules::RefractiveIndexMap Error! Index out of range when initializing. x=" << x << ", y=" << y << std::endl;
			}
		}
		csvFile.close();
		initialized = true;
		return;
	}
} /* namespace RichRingXModules */
