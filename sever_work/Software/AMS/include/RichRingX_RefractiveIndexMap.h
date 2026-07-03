// $Id: RichRingX_RefractiveIndexMap.h,v 1.3 2025/05/23 16:04:27 jianan Exp $
//
//  RichRingX_RefractiveIndexMap.h
//  RichRingXR
//
//  Created by Jianan Xiao on 2025/04/28.
//
#pragma once

#include <string>
#include <vector>

namespace RichRingXModules {
	class RefractiveIndexMap {
	public:
		RefractiveIndexMap() = delete;
		RefractiveIndexMap(const RefractiveIndexMap&) = delete;
		RefractiveIndexMap& operator=(const RefractiveIndexMap&) = delete;
		static double interpolate(double x, double y, bool isNaF);
		static void initialize(const std::string& filename);
	public:
		static  int mapBins;
		static const double mapRange;
		static const double binLength;
		inline static std::vector<std::vector<double>> mapValues;

		inline static bool initialized = false;
	}; /* class RefractiveIndexMap */
} /* namespace RichRingXModules */
