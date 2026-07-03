// $Id: RichRingX_AngleEstimator.h,v 1.2 2025/05/23 16:04:27 jianan Exp $
//
//  RichRingX_AngleEstimator.h
//  RichRingXR
//
//  Created by Jianan Xiao on 2025/04/28.
//
#pragma once

// #undef EIGEN_HAS_STD_INVOKE_RESULT
// #define EIGEN_HAS_CXX11
#include <eigen3/Eigen/Dense>
#include <utility>
#include <vector>

namespace RichRingXModules {
	class AngleEstimator {
	public:
		AngleEstimator() = delete;
		AngleEstimator(const AngleEstimator&) = delete;
		AngleEstimator& operator=(const AngleEstimator&) = delete;

		// Probability density function for likelihood calculation
		static double thetaPDF(double x, bool isNaF);

		// AngleEstimator Algorithm
		static double evaluateLikelihood(const std::vector<std::vector<double>>& coneAngles, 
						 const std::vector<int>& hitType, 
						 const Eigen::VectorXd& npeWeight,
						 double theta_i, 
						 bool isNaF);
		static std::pair<std::vector<double>, std::vector<double>> 
		scanLikelihood(const std::vector<std::vector<double>>& coneAngles, 
			       const std::vector<int>& hitType, 
			       const Eigen::VectorXd& npeWeight, 
			       bool isNaF, 
			       double minTheta, 
			       double maxTheta);
		static std::vector<int> findPeaks(const std::vector<double>& y);
		static std::vector<double> estimateTheta(const std::vector<std::vector<double>>& coneAngles, 
							 const std::vector<int>& hitType, 
							 const Eigen::VectorXd& npeWeight, 
							 bool isNaF, 
							 double refitTheta = 0);
	private:
		static double gaussPlusConst(double x, double gaussBase, double sigma); // get value from a gaus + base distribution
		static double thetaPDF_NaF(double x); // theta residual distribution for NaF
		static double thetaPDF_Agl(double x); // theta residual distribution for Agl

	}; // class AngleEstimator
} /* namespace RichRingXModules */