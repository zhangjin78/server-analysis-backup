#ifndef __ECALKX3DRECDB__
#define __ECALKX3DRECDB__
#include <iostream>

namespace EcalKX3DRecDB{
	enum EnumCellStatType{
		//-- online calibration related
		HighGainBadCalibRun=1, // 1
		HighGainBadPedestal=2, // 2
		HighGainNoisyChannel=4, // 3
		HighGainLowEfficiencyChannel=8, //4
 
		LowGainBadCalibRun=16, //5
		LowGainBadPedestal=32, //6
		LowGainNoisyChannel=64, //7
		LowGainLowEfficiencyChannel=128,//8

		DynodeBadCalibRun=256, //9
		DynodeBadPedestal=512, //10
		DynodeNoisyChannel=1024, //11
		DynodeLowEfficiencyChannel=2048, //12

		//-- saturation related
		HighGainSaturated = 2048*2, //13
		LowGainSaturated  = 2048*4, //14
		DynodeSaturated   = 2048*8, //15

		//-- Shower clustering related
		HighGainUsed      = 2048*16, //16
		LowGainUsed       = 2048*32, //17
		DynodeUsed        = 2048*64, //18
		DeadCell          = 2048*128, //19
		Dummy             = 2048*256, //20

		CellUsed          = 2048*512, //21
		OverlapHit        = 2048*1024, //22
		OrphanHit         = 2048*2048 //23
	};

	enum EnumCellMaskType{
		ImaginaryCell=1,
		BadCell=2,
		SaturatedCell=4,
		HighAmplitudeCell=8
	};

	enum emDataType{
		_ISS_DATA=1,
		_TB_DATA=2,
		_MC_DATA=3,
		_ToyMC_DATA=4
	};

	const UInt_t ADCLowGainSatu = 3830;
	const UInt_t ADCHighGainSatu = 3730; // -100 ADC of real saturation ADC
	const UInt_t ADCLowGainSatuMC = 3940; // ADC(after pedstal subtraction) saturation in MC is ~3945
	const UInt_t ADCHighGainSatuMC = 3000; // ADC(after pedstal subtraction) saturation in MC is defined as ~3050

	// fit quality control
	const Int_t MaxIteration = 10; // global in ShowerRec
	const Int_t MaxIteration_Element = 5; // for single parameter
	const Double_t MinDeltaLkhd = 0.5;
	const Double_t DeltaLkhd_LowEnergy = 0; // loose limit at low energy
	const Double_t DeltaLkhd_TransEnergy = 10; // 10 GeV

	const Int_t nLayer = 18,
			nFibLayer = 90,
			nFibPerLay = 535,
			nCell = 72,
			nSL = 9,
			nCol = 36,
			nSL_X = 4, // fiber along Y
			nSL_Y = 5; // fiber along X

	// offset == boundary between fiber 242 and 243
	const Double_t F_Offset[nFibLayer] = { 
		0.000,  0.067, -0.002,  0.065, -0.004,
		0.064, -0.005,  0.062, -0.007,  0.060,

		0.019,  0.087,  0.019,  0.087,  0.019,
		0.087,  0.019,  0.087,  0.019,  0.087,

		-0.028,  0.048, -0.012,  0.064,  0.004,
		-0.054,  0.021, -0.038,  0.037, -0.022,

		0.022, -0.038,  0.038, -0.022,  0.054,
		-0.008,  0.068,  0.008,  0.084,  0.024,

		0.008,  0.071, -0.002,  0.061, -0.012,
		0.051, -0.022,  0.041, -0.032,  0.031,

		0.026,  0.088,  0.014,  0.076,  0.002,
		0.066, -0.008,  0.054, -0.018,  0.042,

		-0.042,  0.024, -0.044,  0.022, -0.046,
		0.020, -0.048,  0.018, -0.050,  0.016,

		-0.005,  0.053, -0.025,  0.033, -0.045,
		0.011, -0.067, -0.009, -0.087, -0.029,

		0.000,  0.068, -0.005,  0.059, -0.013,
		0.049, -0.023,  0.041, -0.031,  0.033
	};

	const Double_t F_Zcoord[nFibLayer] = {
		-142.847+0.009, -143.031+0.007, -143.215+0.005, -143.399+0.003, -143.583+0.001,
		-143.767-0.001, -143.951-0.003, -144.135-0.005, -144.319-0.007, -144.503-0.009,
		-144.697+0.009, -144.881+0.007, -145.065+0.005, -145.249+0.003, -145.433+0.001,
		-145.617-0.001, -145.801-0.003, -145.985-0.005, -146.169-0.007, -146.353-0.009,
		-146.547+0.009, -146.731+0.007, -146.915+0.005, -147.099+0.003, -147.283+0.001,
		-147.467-0.001, -147.651-0.003, -147.835-0.005, -148.019-0.007, -148.203-0.009,
		-148.397+0.009, -148.581+0.007, -148.765+0.005, -148.949+0.003, -149.133+0.001,
		-149.317-0.001, -149.501-0.003, -149.685-0.005, -149.869-0.007, -150.053-0.009,
		-150.247+0.009, -150.431+0.007, -150.615+0.005, -150.799+0.003, -150.983+0.001,
		-151.167-0.001, -151.351-0.003, -151.535-0.005, -151.719-0.007, -151.903-0.009,
		-152.097+0.009, -152.281+0.007, -152.465+0.005, -152.649+0.003, -152.833+0.001,
		-153.017-0.001, -153.201-0.003, -153.385-0.005, -153.569-0.007, -153.753-0.009,
		-153.947+0.009, -154.131+0.007, -154.315+0.005, -154.499+0.003, -154.683+0.001,
		-154.867-0.001, -155.051-0.003, -155.235-0.005, -155.419-0.007, -155.603-0.009,
		-155.797+0.009, -155.981+0.007, -156.165+0.005, -156.349+0.003, -156.533+0.001,
		-156.717-0.001, -156.901-0.003, -157.085-0.005, -157.269-0.007, -157.453-0.009,
		-157.647+0.009, -157.831+0.007, -158.015+0.005, -158.199+0.003, -158.383+0.001,
		-158.567-0.001, -158.751-0.003, -158.935-0.005, -159.119-0.007, -159.303-0.009
	};


	const Double_t F_Rotation[nLayer] = {
//		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
		-0.0005, -0.0005, -0.0008, -0.0008, -0.0001, -0.0001, -0.0003, -0.0003, 0.0010, 0.0010,
		0.0000,  0.0000,  0.0000,  0.0000, -0.0003, -0.0003,  0.0016,  0.0016
	};
	const Double_t EcalZ[nLayer]  = {
		-143.215-0.005, -144.135+0.005, -145.065-0.005, -145.985+0.005, -146.915-0.005, -147.835+0.005,
		-148.765-0.005, -149.685+0.005, -150.615-0.005, -151.535+0.005, -152.465-0.005, -153.385+0.005,
		-154.315-0.005, -155.235+0.005, -156.165-0.005, -157.085+0.005, -158.015-0.005, -158.935+0.005
	};

	const Double_t Cell_Size = 0.9; // cm
	const Double_t F_Diameter = 0.135; // cm
	const Double_t Fiber_Pitch = 0.135; // cm
	const Double_t L_Thickness = 0.925; // cm
	const Double_t Fiber_Radius = 0.050; // cm

	// PMT efficiency model, need more tuning
	// M10: PW=0.3, MinEff=0.4
	const Double_t L_Eff_PW = 0.25; // Plateau width
	const Double_t L_Eff_MinEff = 0.60; // Minimum efficiency
	const Double_t L_Eff_Par = (1-L_Eff_MinEff)/(0.5-L_Eff_PW); // Slope
	const Double_t L_Eff[5] = { 0.8, 1.0, 1.0, 1.0, 0.8 }; // five fiber layers of each cell

	/*
	//MC
	const Double_t L_Eff_PW = 0.3; // Plateau width
	const Double_t L_Eff_MinEff = 0.60; // Minimum efficiency
	const Double_t L_Eff_Par = (1-L_Eff_MinEff)/(0.5-L_Eff_PW); // Slope
	const Double_t L_Eff[5] = { 1, 1.0, 1.0, 1.0, 1 }; // five fiber layers of each cell
	*/
	// Cross-talk parameters
	// const Double_t CT_PW = 0.3; 
	// const Double_t CT_AM = 0.05; // maximum leak when the fiber is at the cell border

	// Shower clustering parameters
	const Double_t Cutoff_MIP = 0.005; // 5 MeV
	const Double_t Cutoff_SEED = 0.020; // 20 MeV
	const Double_t Cutoff_ETOT = 0.100; // 100 MeV
	const Int_t    Cutoff_NNB = 8;  // number of neighboring cells around the layer seed, in each projection(X/Y)
	const Int_t    Cutoff_NPL = 4; // number of matching layers around the shower seed, in each projection(X/Y)
	const Double_t NSigma_Subtraction = 2; // number of matching layers around the shower seed, in each projection(X/Y)

	
	//constant used for calibration
	const float ADC2MEV = 0.4704;  //ADC to Mev
	const float CellWidth = 0.9;   //Cell Width, cm
	
	//Fiber Sampling Fraction, depending on PMT efficiency model
	//// sampling fraction and integration over fiber diameter
	//const Double_t fNorm = 0.005495*0.790*1.135;  // PW=0.25, MinEff=0.40
	const Double_t fNorm = 0.005495*0.790*1.084;  // PW=0.25, MinEff=0.60


	const Int_t kNSFast = 4,	// maximum number of showers in fast reconsturction
			kNS = 3, // maxinum number of fit showers
			kNT = 30,      // shower depth granurity
			kNL = 18, 		// number ECAL layers
			kNC = 80; 		// +-4 cells to recover lateral leakage
	const Int_t kNpar=8; // number of shower parameters
}
#endif
