// $Id: RichRingX_RichConst.h,v 1.5 2025/05/24 23:38:22 jianan Exp $
//
//  RichRingX_RichConst.h
//  RichRingXR
//
//  Created by Jianan Xiao on 2025/04/28.
//

#pragma once
#include <cstdint>
#include <array>

#define kMAXIMUM 5999

enum class HitTypes {
	Direct, Reflect, Noise
};

namespace RichConst {
	constexpr int RICnrot 					= 30001;	// Rot. matrix no.

	constexpr double RIClgthk_top 				= 0.02;		// LG    top gap
	constexpr double RIClgthk_bot 				= 0.07;		// LG bottom gap


	constexpr double RICotherthk 				= 0.08;		// PMT window thickness
	constexpr double RICcatolength 				= 1.81;		// cathode window length
	constexpr double RICcatogap 				= 0.03;		// Gap btwn PMT pixels
	constexpr double RICeleclength 				= 2.75;		// electronics length below PMT
	constexpr double RICshiheight 				= 6.5;		// This for the new LG with 3.4
	constexpr double RICpmtshield 				= 0.1;		// PMT shield thickness
	constexpr double RICepsln 				= 0.002;	// Epsilon
	constexpr double RICpmtsupportheight 			= 11.1 - 2.0;	// support structure height
	constexpr double PMT_electronics 			= 3.0;		// PMT side size
	constexpr double cato_inner_pixel 			= 0.42;		// Innerr pixel side size in the photocathode
	constexpr double cathode_length 			= RICcatolength + RICcatogap;
	constexpr double eff_rad_clarity 			= 0.0055;	// clarity used in charge recosntruction
	constexpr int    RICmaxentries 				= 44;


	constexpr double aeThk 					= 0.1;		// Radiator support thickness (gap btwn radiator tiles).
	constexpr double aglHeight 				= 2.5;		// Radiator agl thickness
	constexpr double aglSide 				= 11.5;		// AGL Radiator tile side length
//	#ifdef ISISS
//	double aglIndex				 	= 1.048; //1.048
//	#else
//	double aglIndex				 	= 1.050; //1.048
//	#endif

	constexpr double elecLength 				= 2.75;		// Electronics length below PMT.
	constexpr double foilHeight 				= 0.1;		// Foil thickness.
										// the foil is the supportive structure under the RICH Radiator Plane.
	constexpr double foilIndex 				= 1.46;
	
	constexpr double lgHeight 				= 3.0;		// Light guide height without the fixing foil.
	constexpr double lgTopLength 				= 3.4;		// Side length of light guide top (Called lg_length in the standalone version)
	constexpr double lgBottomLength 			= 1.77;		// Side length on the bottom
	constexpr double lgMirGap 				= 0.5;		// Gap length between Mirror and Light Guide.

	constexpr double mirHeight 				= 46.32;	// Mirror height.
	constexpr double mirThk 				= 0.2;		// Mirror thickness.
	constexpr double mirTopRadius 				= 60.10;	// Is this 57.0cm??
	constexpr double mirBottomRadius 			= 67.00;
	constexpr double mirReflectivity			= 0.85;		// Mirror reflectivity at sector 2 and 3

	constexpr double NaFHeight 				= 0.5;		// NaF radiator thickness
	constexpr double NaFSideGap 				= 0.4;
	constexpr double NaFSide 				= 8.52;		// NAF Radiator tile side length
	constexpr double NaFIndex				= 1.3330;

	constexpr double pmtFoilThk 				= 0.1;		// Thickness of the foil over the Light Guide.
	constexpr double pmtLength 				= 2.0;		// Phototube length including PMT window.
	constexpr double pmtSupportThk 				= 0.6;		// PMT support structure thickness.
	constexpr double pmtRadius 				= 67.0;		// sure.
	
	constexpr double radHeight 				= 1.43; // new calibration. original was 3; could be radPos calibration as well		// Radiator support structure height.
	constexpr double radMirGap 				= 0.1;		// Gap length between Radiator and Mirror.
	constexpr double radPos 				= -71.87;	// RICH Radiator position with respect to AMS02.
	constexpr double radRadius 				= 60.0;		// RICH Radiator Radius.
	// #ifdef ISISS
	// constexpr double refractiveIndexDefault 		= 1.048; //1.048
	// #else
	constexpr double refractiveIndexDefault 		= 1.050; //1.048
	// #endif

	constexpr double betaThreshold				= 0.05;		// Threshold, in % of the total beta range, to avoid the reconstruction of secondaries.

	const std::uint8_t nBadAglBlocks = 9;
	constexpr std::array<float, nBadAglBlocks> badAglPosX {-2, 20, -29, 30, 16, 30, -30, -10, 12};
	constexpr std::array<float, nBadAglBlocks> badAglPosY {-60, -42, -18, 16, 22, 38, 40, 56, 56};
	constexpr std::array<float, nBadAglBlocks> badAglBlockWidthX {2, 2, 1, 2, 2, 2, 2, 2, 2};
	constexpr std::array<float, nBadAglBlocks> badAglBlockWidthY {2, 2, 2, 2, 2, 1.5, 2, 1.5, 2};
	
	// RICH height from top of the radiator plane to top of the PMT plane.
	constexpr double richHeight = radHeight + foilHeight + radMirGap + mirHeight + lgMirGap;

	// Mean transmission distance of photon radiated from NaF.
	// extern double NaFTransmissionHeight;
	constexpr double NaFTransmissionHeight = 0.5 * NaFHeight + foilHeight + radMirGap + mirHeight + lgMirGap;

	// Mean transmission distance of photon radiated from Agl.
	// extern double aglTransmissionHeight;
	constexpr double aglTransmissionHeight = 0.5 * aglHeight + foilHeight + radMirGap + mirHeight + lgMirGap;

	constexpr double pmtTotalHeight = pmtLength + elecLength + lgHeight + pmtFoilThk;

	constexpr double pmtPos = radHeight + aglHeight + foilHeight + radMirGap + mirHeight + lgMirGap;

	constexpr double vacuumHeight = radMirGap + mirHeight + lgMirGap;

	// Geometric alignment
	extern double heightCorrection;
	extern double xShift;
	extern double yShift;
	extern double aglIndex;
} /* namespace RichConst */

namespace TrackerConst {
	constexpr int layerCount = 9;
	namespace Algorithm {
		constexpr std::uint8_t Choutko = 0;
		constexpr std::uint8_t GBL = 1;
		constexpr std::uint8_t Kalman = 2;
		constexpr std::uint8_t Default = GBL;
	}; /* namespace Algorithm */

	namespace Alignment {
		constexpr std::uint8_t inner_PG = 0;
		constexpr std::uint8_t external_CIEMAT = 1;
		constexpr std::uint8_t v6_all = 2;
		constexpr std::uint8_t Default = v6_all;
	}; /* namespace Alignment */

	namespace Span {
		constexpr std::uint8_t fullSpan = 0;
		constexpr std::uint8_t inner = 1;
		constexpr std::uint8_t innerL1 = 2;
		constexpr std::uint8_t innerL9 = 3;
		constexpr std::uint8_t forceFullSpan = 4;
		constexpr std::uint8_t innerUp = 5;
		constexpr std::uint8_t innerLow = 6;
		constexpr std::uint8_t Default = inner;
	}; /* namespace Span */

	namespace ChargeAlgorithm {
		constexpr std::uint8_t qYJ = 0;
		constexpr std::uint8_t qh = 1;
		constexpr std::uint8_t Default = qYJ;
	}; /* namespace ChargeAlgorithm */

	namespace Direction {
		constexpr std::uint8_t x = 0;
		constexpr std::uint8_t y = 1;
		constexpr std::uint8_t xy = 2;
		constexpr std::uint8_t Default = xy;
	}; /* namespace Direction */

	namespace FiducialCuts {
		struct Position {
			double yPos;
			double rPos;
		};

		constexpr std::array<Position, layerCount> positions{
			Position{47.0, 62.0}, Position{40.0, 62.0}, Position{44.0, 46.0},
			Position{44.0, 46.0}, Position{36.0, 46.0}, Position{36.0, 46.0},
			Position{44.0, 46.0}, Position{44.0, 46.0}, Position{29.0, 43.0}
		};
	}; /* namespace FiducialCuts */
}; /* namespace TrackerConst */

namespace TofConst {
	constexpr int layerCount = 4;
	constexpr int upperLayers = 2;
	constexpr int lowerLayers = 2;
	constexpr std::array<int, 2> edgeIndex {0, 7}; // normal layer edge bar index
	constexpr std::array<int, 2> trapezoidEdgeIndex {0, 9}; // Trapezoid layer edge bar index
}; /* namespace TofConst */
