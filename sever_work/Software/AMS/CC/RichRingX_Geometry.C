// $Id: RichRingX_Geometry.C,v 1.2 2025/05/23 16:03:57 jianan Exp $
//
//  RichRingX_Geometry.C
//  RichRingXR
//
//  Created by Jianan Xiao on 2025/04/28.
//

#include "RichRingX_Geometry.h"
#include "RichRingX_RichConst.h"

#include <cmath>
// #undef EIGEN_HAS_STD_INVOKE_RESULT
// #define EIGEN_HAS_CXX11
#include <eigen3/Eigen/Dense>
#include <tuple>
#include <vector>

namespace RichRingXModules {
	const double Geometry::K_getMirrorRadius1 = -(RichConst::mirBottomRadius - RichConst::mirTopRadius) / RichConst::mirHeight;
	const double Geometry::K_getMirrorRadius2 = RichConst::mirBottomRadius - Geometry::K_getMirrorRadius1 * RichConst::lgMirGap;

	double Geometry::getMirrorRadius(double z) {
		return K_getMirrorRadius1 * z + K_getMirrorRadius2;
	}

	void Geometry::getMirrorPoint (double phi, double z, Eigen::Vector3d& mirPoint) {
		double r = getMirrorRadius(z);
		mirPoint(0) = r * std::cos(phi);
	    mirPoint(1) = r * std::sin(phi);
	    mirPoint(2) = z;
	}

	void Geometry::updateAlignment(std::vector<double>& particleTrack) {
		// Traker extrapolate z error correction
		particleTrack[0] += -RichConst::xShift + RichConst::heightCorrection * std::tan(particleTrack[3]) * std::cos(particleTrack[4] + M_PI);
		particleTrack[1] += -RichConst::yShift + RichConst::heightCorrection * std::tan(particleTrack[3]) * std::sin(particleTrack[4] + M_PI);
		return;
	}

	std::tuple<Eigen::Vector3d, Eigen::Vector3d, Eigen::Vector2d> Geometry::getConeGeometry(const std::vector<double>& particleTrack, bool isNaF) {
		// Čerenkov radiation emission point
		const auto [emitX, emitY, emitPhi, emitTheta] = std::make_tuple(particleTrack[0], particleTrack[1], particleTrack[4] + M_PI, particleTrack[3]);
		const double emitZ = isNaF ? RichConst::NaFTransmissionHeight : RichConst::aglTransmissionHeight;

		const Eigen::Vector3d emissionPoint {emitX, emitY, emitZ};
		const Eigen::Vector2d trackDirection {emitTheta, emitPhi};

		// Čerenkov radiation cone center extrapolated from emission center to PMT plane.
		const double coneCenterX = emitX + emitZ * std::tan(emitTheta) * std::cos(emitPhi);
		const double coneCenterY = emitY + emitZ * std::tan(emitTheta) * std::sin(emitPhi);
		const Eigen::Vector3d coneCenter {coneCenterX, coneCenterY, 0};

		return std::make_tuple(emissionPoint, coneCenter, trackDirection);
	}

	bool Geometry::updateRadiatorType(const std::vector<double>& trackAgl, const std::vector<double>& trackNaF) {
		bool isNaF = std::fabs(trackNaF[0]) < 18 and std::fabs(trackNaF[1]) < 18;
		bool isAgl = std::fabs(trackAgl[0]) > 18 or std::fabs(trackAgl[1]) > 18;
		isNaF = (isNaF and !isAgl);
		return isNaF;
	}
} /* namespace RichRingXModules */