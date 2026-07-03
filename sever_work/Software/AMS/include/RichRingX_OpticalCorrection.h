// $Id: RichRingX_OpticalCorrection.h,v 1.2 2025/05/23 16:04:27 jianan Exp $
//
//  RichRingX_OpticalCorrection.h
//  RichRingXR
//
//  Created by Jianan Xiao on 2025/04/28.
//
#pragma once

// #undef EIGEN_HAS_STD_INVOKE_RESULT
// #define EIGEN_HAS_CXX11
#include <eigen3/Eigen/Dense>
#include <tuple>
#include <vector>

namespace RichRingXModules {
	class OpticalCorrection {
	public:
		OpticalCorrection() = delete;
		OpticalCorrection(const OpticalCorrection&) = delete;
		OpticalCorrection& operator=(const OpticalCorrection&) = delete;

		// Optical Correction Functions
		static double findEmissionAngle(double h1, double h2, double h3, double n1, double n2, double d);
		static void traceToPMT(const Eigen::Vector3d& emissionPoint, const Eigen::Vector3d& reflectPoint, Eigen::Vector2d& result);
		static bool isLocalMinimum(const Eigen::ArrayXXd& arr, int i, int j);
		static std::tuple<std::vector<std::vector<double>>, std::vector<std::vector<double>>, std::vector<std::vector<double>>> computeAngles(const Eigen::Array<double, Eigen::Dynamic, 3>& hits, const Eigen::Vector3d& emissionPoint, const Eigen::Vector3d& coneCenter, bool isNaF, double refractiveIndex = 0);

	}; /* class OpticalCorrection */
} /* namespace RichRingXModules */