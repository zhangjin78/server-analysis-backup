// $Id: RichRingX_OpticalCorrection.C,v 1.11 2025/05/29 14:36:01 choutko Exp $
//
//  RichRingX_OpticalCorrection.C
//  RichRingXR
//
//  Created by Jianan Xiao on 2025/04/28.
//

#include "RichRingX_OpticalCorrection.h"
#include "RichRingX_Geometry.h"
#include "RichRingX_RichConst.h"
#include "RichRingXR.h"
#include <algorithm>
#include <cmath>
// #undef EIGEN_HAS_STD_INVOKE_RESULT
// #define EIGEN_HAS_CXX11
#include <eigen3/Eigen/Dense>
#include <limits>
#include <tuple>
#include <vector>

namespace RichRingXModules {
	double OpticalCorrection::findEmissionAngle(double h1, double h2, double h3, double n1, double n2, double d) {
		double theta1_low = 0.0;
		double theta1_high = M_PI / 2.0;

		const double tolerance = 1e-6;
		const double n1_over_n2 = n1 / n2;
		while (theta1_high - theta1_low > tolerance) {
			double theta1 = (theta1_low + theta1_high) / 2.0;
			
			double sin_theta1 = std::sin(theta1);
			double sin_theta2 = n1_over_n2 * sin_theta1;
			double sin_theta3 = n1 * sin_theta1;
			if (sin_theta3 > 1.0) {
				// in case of total reflection
				theta1_high = theta1;
				continue;
			}
			// Compute cosines using identity cos(theta) = sqrt(1 - sin^2(theta))
			double cos_theta1 = std::sqrt(1.0 - sin_theta1 * sin_theta1);
			double cos_theta2 = std::sqrt(1.0 - sin_theta2 * sin_theta2);
			double cos_theta3 = std::sqrt(1.0 - sin_theta3 * sin_theta3);

			double horizontalOffset1 = h1 * sin_theta1 / cos_theta1;
			double horizontalOffset2 = h2 * sin_theta2 / cos_theta2;
			double horizontalOffset3 = h3 * sin_theta3 / cos_theta3;

			double totalHorizontalOffset = horizontalOffset1 + horizontalOffset2 + horizontalOffset3;

			if (totalHorizontalOffset < d) {
				theta1_low = theta1;
			} else {
				theta1_high = theta1;
			}
		}

		return (theta1_low + theta1_high) / 2.0;
	}

	void OpticalCorrection::traceToPMT(const Eigen::Vector3d& emissionPoint, const Eigen::Vector3d& reflectPoint, Eigen::Vector2d& result) {
		// Mirror surface normal vector at the reflection point
		static const double mirSlope = -(RichConst::mirBottomRadius - RichConst::mirTopRadius) / RichConst::mirHeight;
		static const double theta = M_PI / 2.0 + std::atan(-mirSlope);
		const double phi = std::atan2(-reflectPoint(1), -reflectPoint(0));
		Eigen::Vector3d normal {std::cos(phi) * std::sin(theta), std::sin(phi) * std::sin(theta), std::cos(theta)};

		// Incident direction (from emission point to reflection point)
		Eigen::Vector3d incidentRay = reflectPoint - emissionPoint;

		// Compute the reflected direction using reflection formula
		Eigen::Vector3d reflectedRay = incidentRay - 2 * (incidentRay.dot(normal)) * normal;

		// Now we need to find the point where the reflected ray intersects the PMT plane (z = constant)
		const double zPMT = 0.0;  // z-coordinate of the PMT plane

		// Find intersection point with PMT plane
		// Equation: point = reflectPoint + t * reflectedRay, we solve for t when z = zPMT
		double t = (zPMT - reflectPoint(2)) / reflectedRay(2);
		Eigen::Vector3d pmtPoint = reflectPoint + t * reflectedRay;

		// Return the x and y coordinates of the intersection point on the PMT plane
		result(0) = pmtPoint(0);
		result(1) = pmtPoint(1);
	}

	bool OpticalCorrection::isLocalMinimum(const Eigen::ArrayXXd& arr, int i, int j) {
		// if the current value is a local minima of an array, return true
		const double threshold = 1.5;
		double val = arr(i, j);
		if (val > threshold) return false;

		int rows = arr.rows() - 1;
		int cols = arr.cols() - 1;
		for (int x = std::max(0, i - 1); x <= std::min(i + 1, rows); ++x) {
			for (int y = std::max(0, j - 1); y <= std::min(j + 1, cols); ++y) {
				if ((x != i or y != j) and arr(x, y) <= val) {
					return false;
				}
			}
		}

		// while return the result we can change the neighbors of this arr(i, j) to infinity to speed up future uses of isLocalMinimum.
		return true;
	}

	std::tuple<std::vector<std::vector<double>>, std::vector<std::vector<double>>, std::vector<std::vector<double>>> 
	OpticalCorrection::computeAngles(const Eigen::Array<double, Eigen::Dynamic, 3>& hits, const Eigen::Vector3d& emissionPoint, const Eigen::Vector3d& coneCenter, bool isNaF, double refractiveIndex) {
		const double h1 = emissionPoint(2) - (RichConst::foilHeight + RichConst::radMirGap + RichConst::mirHeight + RichConst::lgMirGap);
		const double h2 = RichConst::foilHeight;
		const double n1 = refractiveIndex == 0 ? (isNaF? RichConst::NaFIndex : RichConst::aglIndex) : refractiveIndex;
		const double n2 = RichConst::foilIndex;
#ifdef __AROOTSHAREDLIBRARY__
		const int arrSizeZ = RichRingXR::dz; // 500 It's better to keep dz and dphi the same (by adjusting arrSize)
		const int arrSizePhi = RichRingXR::dphi; //4000
#else
                const int arrSizeZ = RichRingXR::sdz(); // 500 It's better to keep dz and dphi the same (by adjusting arrSize)
                const int arrSizePhi = RichRingXR::sdphi(); //4000
#endif
		const double deadZone = 0.0; // range of Z starting from below the radiator that we don't want to consider ray back-tracing
		// Step size of z and phi on the mirror surface
		const double dz = (RichConst::mirHeight + RichConst::lgMirGap - deadZone) / arrSizeZ;
		const double dphi = 2.0 * M_PI / arrSizePhi;

		Eigen::ArrayXXd pmtX(arrSizeZ, arrSizePhi); // Store the unreflected position on PMT plane from mirror
		Eigen::ArrayXXd pmtY(arrSizeZ, arrSizePhi);
		Eigen::ArrayXXd emitTheta(arrSizeZ, arrSizePhi); // Store the emission direction
		Eigen::ArrayXXd emitPhi(arrSizeZ, arrSizePhi);

		const Eigen::Vector3d coneAxis = coneCenter - emissionPoint;
		const double coneAxisLength = coneAxis.norm();
		
		// Generate mesh on mirror with interested values
#ifdef __ROOTSHAREDLIBRARY__
#ifndef __clang__ 
		#pragma omp parallel for
#else
#pragma omp parallel for collapse(2)
#endif
#endif
		for (int i = 0; i < arrSizeZ; ++i) {
			const double z = i * dz;
			const double h3 = RichConst::radMirGap + RichConst::mirHeight + RichConst::lgMirGap - z;

			for (int j = 0; j < arrSizePhi; ++j) {
				const double phi = j * dphi;

				Eigen::Vector3d mirPoint;
				RichRingXModules::Geometry::getMirrorPoint(phi, z, mirPoint);
				
				// Consider refraction effect when crossing the radiator
				const double d_rad2Mir = (emissionPoint.head<2>() - mirPoint.head<2>()).norm();
				const double iEmitPhi = std::atan2(mirPoint(1) - emissionPoint(1), mirPoint(0) - emissionPoint(0));
				const double iEmitTheta = findEmissionAngle(h1, h2, h3, n1, n2, d_rad2Mir);

				const double d_rad2FoilBottom = h1 * std::tan(iEmitTheta) + h2 * std::tan(std::asin(n1 / n2 * std::sin(iEmitTheta)));
				const double dx_rad2FoilBottom = d_rad2FoilBottom * std::cos(iEmitPhi);
				const double dy_rad2FoilBottom = d_rad2FoilBottom * std::sin(iEmitPhi);
				// find refraction point
				const Eigen::Vector3d refractionPoint {emissionPoint(0) + dx_rad2FoilBottom, emissionPoint(1) + dy_rad2FoilBottom, RichConst::vacuumHeight};
				
				Eigen::Vector2d pmtPoint;
				traceToPMT(refractionPoint, mirPoint, pmtPoint);
				
				if (pmtPoint.squaredNorm() < 4900.0) {
					pmtX(i, j) = pmtPoint(0);
					pmtY(i, j) = pmtPoint(1);
				} else {
					pmtX(i, j) = std::numeric_limits<double>::infinity();
					pmtY(i, j) = std::numeric_limits<double>::infinity();
				}
				emitTheta(i, j) = iEmitTheta;
				emitPhi(i, j) = iEmitPhi;
			}
		}
		
		std::vector<std::vector<double>> emitAnglesTheta(hits.rows()); // Emission direction of each photon
		std::vector<std::vector<double>> emitAnglesPhi(hits.rows()); // Emission dirction w.r.t cone center of each photon
		std::vector<std::vector<double>> coneAngles(hits.rows()); // Emission dirction w.r.t cone center of each photon
		const double h3 = RichConst::richHeight - RichConst::radHeight - RichConst::foilHeight;
#ifdef __ROOTSHAREDLIBRARY__
		#pragma omp parallel for
#endif
		for (int index = 0; index < hits.rows(); ++index) {
			const Eigen::Vector3d iHit = (hits.row(index)).transpose();
			
			// Cherenkov angle of Direct hit
			const double dDirect = (emissionPoint.head<2>() - iHit.head<2>()).norm();
			const double thetaDirect = findEmissionAngle(h1, h2, h3, n1, n2, dDirect);
			const double phiDirect = std::atan2(iHit(1) - emissionPoint(1), iHit(0) - emissionPoint(0));
			const Eigen::Vector3d rayDirectionDirect = {std::sin(thetaDirect) * std::cos(phiDirect), std::sin(thetaDirect) * std::sin(phiDirect), -std::cos(thetaDirect)};
			const double localCosineAngleDirect = rayDirectionDirect.dot(coneAxis) / coneAxisLength;

			emitAnglesTheta[index].push_back(thetaDirect);
			emitAnglesPhi[index].push_back(phiDirect);
			coneAngles[index].push_back(std::acos(localCosineAngleDirect));

			// Distance of each detected photon to generated points
			Eigen::ArrayXXd distanceArr(arrSizeZ, arrSizePhi);
			distanceArr << (pmtX - iHit(0)).square() + (pmtY - iHit(1)).square();
			// local minima of distanceArr, so as to find the real reflection point

			for (int i = 0; i < arrSizeZ; ++i) {
				for (int j = 0; j < arrSizePhi; ++j) {
					if (isLocalMinimum(distanceArr, i, j)) {
						const double localEmitTheta = emitTheta(i, j);
						const double localEmitPhi = emitPhi(i, j);
						const Eigen::Vector3d rayDirection = {std::sin(localEmitTheta) * std::cos(localEmitPhi), std::sin(localEmitTheta) * std::sin(localEmitPhi), -std::cos(localEmitTheta)};
						const double localCosineAngle = rayDirection.dot(coneAxis) / coneAxisLength;

						emitAnglesTheta[index].push_back(localEmitTheta);
						emitAnglesPhi[index].push_back(localEmitPhi);
						coneAngles[index].push_back(std::acos(localCosineAngle));
					}
				}
			}
		}
		
		return std::make_tuple(emitAnglesTheta, emitAnglesPhi, coneAngles);
	}
} /* namespace RichRingXModules */
