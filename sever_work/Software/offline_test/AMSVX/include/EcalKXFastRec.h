#ifndef _ECALKXFASTREC_
#define _ECALKXFASTREC_

/* =========================================================
 * ECAL fast reconstruction
 * The main purpose is to do MIP attenuation correction
 * Including the following procedures:
 * 1) Interface to handle different input data format
 * 2) Use cell-level calibration 
 * 3) Shower finding and clustering 
 * 4) Attenuation correction
 *
 * The output will be used as input of 3D fit
 *========================================================== */
#include "TString.h"
#include "TMath.h"
#include <iostream>
//#include "root_RVSP.h"
#include "root.h"
#include "amschain.h"
#include "EcalKXCalib.h"
#include "EcalKX3DRecDB.h"
//using namespace EcalKX3DRecDB;

class EcalKXFastRec // :public TObject
{
	/*
	 *  Fast shower reconstruction
	 *  Apply gain correction and MIP attenuation correction
	 *  Input:  ADCH and ADCL
	 *  Output: EdepAttC
	 * */
	public:
		EcalKXFastRec(){  Reset();  }
		~EcalKXFastRec(){ std::cout<<"~EcalKXFastRec()"<<std::endl; }

		void Reset();

		// Read ADC map 
		void ReadADCMapFromAMSEvent( AMSEventR *pev );
		void ReadADCMap(Double_t _time, Float_t _adcH[][EcalKX3DRecDB::nCell], Float_t _adcL[][EcalKX3DRecDB::nCell], Int_t _stat[][EcalKX3DRecDB::nCell], Int_t _dataType=EcalKX3DRecDB::_ISS_DATA);
		void ReadADCMapFromToyEvent( Double_t _time, Float_t _edep[][EcalKX3DRecDB::nCell], Int_t _stat[][EcalKX3DRecDB::nCell] );

		// Calibration database 
		int LoadCalibrationDataBase();

		// CellRatio
		void CellRatioParToyMC(double ene, double &par0, double &par1);
		void CellRatioParMC(double ene, double &par0, double &par1);
		void CellRatioParISS(double ene, double &par0, double &par1);

		// Fast axis reconstruction
		void DefineCellStatus();   // Mask cell status
		void GainCalibration();    // Gain correction from Zuhao
		void ShowerSeedFinding(Int_t iter=0);  // Shower finding
		void ShowerClustering(Int_t iter=0);   // Shower clustering 
		void CalculateLayerCOG(Int_t iter=0); // Axis reconstruction, 0-> COG, 1->Cell ratio
		void ParabolaZ0Fit(); // Fit shower maximum Z position
		void LinearFit(Int_t n, vector<Double_t> vx, vector<Double_t> vy, vector<Double_t> vw, Double_t &kx, Double_t &x0); // Linear fit
		void ShowerAxisFit();

		// Attenuation correction
		void AttenuationCorrection(); // Attenuation correction for clustered hits
		double GetAttCorFactor(int l, int c, float posfiber);
		void OrphanHitsAttCor(); // Attenuation correction for orphan hits

		// Cell Status
		void ClearCellStatus(Int_t l, Int_t c, Int_t stat_ask);
		int  CheckCellStatus(Int_t l, Int_t c, Int_t stat_ask);
		void ClearAllCellStatus(Int_t stat_ask);
		void CellEnergyMeV2GeV(){ // convert MeV to GeV
			for(int l=0; l<EcalKX3DRecDB::nLayer; l++) for(int c=0; c<EcalKX3DRecDB::nCell; c++) Edep[l][c] *= 1e-3;
		}

		void DoCalibration(); // gain calibration / channel selection / cell status
		void DoFastReconstruction();  // Main interface
		void EstimateEnergy();
		void EnergyCorrection();

		Float_t GetEdepAttC(Int_t lay, Int_t cell){ return EdepAttC[lay][cell]; }
		Float_t GetRawADC(Int_t lay, Int_t cell, Int_t channel=0){ 
			if( channel==0 ) return ADCH[lay][cell]; 
			else if( channel==1 ) return ADCL[lay][cell];
			else return 0;
		}
		Float_t GetEdep(Int_t lay, Int_t cell){ return Edep[lay][cell]; }
		Float_t GetAttenuationFactor(Int_t lay, Int_t cell){ return Edep[lay][cell]>0?EdepAttC[lay][cell]/Edep[lay][cell]:1.; }
		Int_t   GetCellStatus(Int_t lay, Int_t cell){ return Status[lay][cell]; }

		// output of fast reconstruction
		Float_t EdepAttC[EcalKX3DRecDB::nLayer][EcalKX3DRecDB::nCell]; // Energy after MIP attenuation correction
		Int_t   Status[EcalKX3DRecDB::nLayer][EcalKX3DRecDB::nCell]; // Cell status
//	private:
		Double_t Time;
		UInt_t   MCVersion;
		UInt_t   Run, Event;
		Float_t ADCH[EcalKX3DRecDB::nLayer][EcalKX3DRecDB::nCell], // High gain ADC
				  ADCL[EcalKX3DRecDB::nLayer][EcalKX3DRecDB::nCell], // Low gain ADC
				  PedH[EcalKX3DRecDB::nLayer][EcalKX3DRecDB::nCell], // High gain pedestal
				  PedL[EcalKX3DRecDB::nLayer][EcalKX3DRecDB::nCell]; // Low gain pedestal
		Float_t Edep[EcalKX3DRecDB::nLayer][EcalKX3DRecDB::nCell];   // Energy after gain calibration
		Float_t EdepBuffer[EcalKX3DRecDB::nLayer][EcalKX3DRecDB::nCell]; // Buffer used for shower clustering

		Double_t fLrrPar0, fLrrPar1; // Cell ratio parameters
		Double_t LayCrd[EcalKX3DRecDB::nLayer], // Coordinate of each layer
					LayLRR[EcalKX3DRecDB::nLayer], // Left/Right cell energy ratio
					LayAmp[EcalKX3DRecDB::nLayer], // Energy deposition in central 3 cells of each layer
					LayPos[EcalKX3DRecDB::nLayer], // shower axis extrapolation
					LayEdep[EcalKX3DRecDB::nLayer]; // Energy deposition in the shower footprint of each layer
		Double_t LayCrd0[EcalKX3DRecDB::nLayer], // Coordinate of each layer
					LayLRR0[EcalKX3DRecDB::nLayer], // Left/Right cell energy ratio
					LayPos0[EcalKX3DRecDB::nLayer], // Left/Right cell energy ratio
					LayEdep0[EcalKX3DRecDB::nLayer]; // Energy deposition in the shower footprint of each layer
		Int_t   	SeedCell0[EcalKX3DRecDB::nLayer]; // Seed Cell
		Int_t    Cell_Min[EcalKX3DRecDB::nLayer],
				   Cell_Max[EcalKX3DRecDB::nLayer]; // current footprint
		Double_t Z0, X0, Y0; // Shower maximum position
		Double_t dZ0, dX0, dY0; // error
		Double_t KX, KY;     // Shower axis direction
		Double_t dKX, dKY;   // error
		Int_t    N_Shwr;
		Int_t    DataType;

		// shower quality
		Double_t Ed0,     // shower energy after attenuation correction
					Ed1,     // shower energy after PMT efficiency correction
					E0,      // shower energy after rear leakage correction
					EX,		// total energy in x-projection footprint for fit
					EY,		// total energy in y-projection footprint for fit
					dEX,		// maximum cell energy in x-projection
					dEY,		// maximum cell energy in y-projection
					X1,		// center of gravity in X 
					Y1;		// center of gravity in Y
		Int_t    fEX,		// number of X cells associated during shower footprint recognition
					fEY;		// number of Y cells associated during shower footprint recognition
		Int_t		fX0,		// number of X layers with good local seed, based on subtracted deposition
					fY0;  	// number of Y layers with good local seed, based on subtracted deposition
		Int_t		fX1,		// number of X layers with good local seed, based on original deposition
					fY1;  	// number of Y layers with good local seed, based on original deposition
		Int_t    SeedCell[EcalKX3DRecDB::nLayer]; // Seed cell id in each layer
		Int_t    Apex;
		Int_t    Nhits;
		Int_t    NhitsBad;
		// Buffer of shower parameters
		Double_t ShwrZ0[EcalKX3DRecDB::kNSFast], ShwrX0[EcalKX3DRecDB::kNSFast], ShwrY0[EcalKX3DRecDB::kNSFast],
					ShwrKX[EcalKX3DRecDB::kNSFast], ShwrKY[EcalKX3DRecDB::kNSFast];
	  	Double_t ShwrE0[EcalKX3DRecDB::kNSFast], ShwrEd0[EcalKX3DRecDB::kNSFast], ShwrEd1[EcalKX3DRecDB::kNSFast];
		Int_t    ShwrCellMin[EcalKX3DRecDB::kNSFast][EcalKX3DRecDB::nLayer], ShwrCellMax[EcalKX3DRecDB::kNSFast][EcalKX3DRecDB::nLayer];
		Int_t    ShwrNhits[EcalKX3DRecDB::kNSFast];
		// Calibration 
		static EcalKXCalib *pEcalKXCalib;
		static EcalStaticCalibPar *pEcalStaticCalibPar;
		static EcalDynamicCalibPar *pEcalDynamicCalibPar;
		static EcalCellStatusParUnit *pEcalCellStatusParUnit;
		// skip calibration option
		static bool IsSkipCalibration;
		static bool IsSkipAttenuationCorrection;

//		ClassDef(EcalKXFastRec,1)
};
#endif //_ECALKXFASTREC_
