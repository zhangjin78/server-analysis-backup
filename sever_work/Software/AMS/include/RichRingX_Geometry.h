// $Id: RichRingX_Geometry.h,v 1.2 2025/05/23 16:04:27 jianan Exp $
//
//  RichRingX_Geometry.cpp
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
	class Geometry {
	public:
		Geometry() = delete;
		Geometry(const Geometry&) = delete;
		Geometry& operator=(const Geometry&) = delete;
		// RICH Geometry
		static double getMirrorRadius(double z);
		static void getMirrorPoint (double phi, double z, Eigen::Vector3d& mirPoint);
		static void updateAlignment(std::vector<double>& particleTrack);
		static std::tuple<Eigen::Vector3d, Eigen::Vector3d, Eigen::Vector2d> getConeGeometry(const std::vector<double>& particleTrack, bool isNaF);
		static bool updateRadiatorType(const std::vector<double>& trackAgl, const std::vector<double>& trackNaF);

	private:
		static const double K_getMirrorRadius1;
		static const double K_getMirrorRadius2;
	}; // class Geometry
} /* namespace RichRingXModules */