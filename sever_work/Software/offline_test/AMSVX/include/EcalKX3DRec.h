#ifndef __ECALKX3DREC__
#define __ECALKX3DREC__

////////////////////////////////////////////////////////////////////
//                  Head file of Ecal3D Rec                      //                         
//              Definition of EcalKX3DRec Class                    //
//     					@ 2014-02-22 										  //
///////////////////////////////////////////////////////////////////

#include <TString.h>
#include <TChain.h>
#include <TH2D.h>
#include <TH1D.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <TFile.h>
#include <TTree.h>
#include <TMath.h>
#include <TRandom.h>
#include <iostream>
#include <stdio.h>
#include <vector>
#include <math.h>
#include "TMVA/Tools.h"
#include "TMVA/Reader.h"

#include "EcalKX3DRecDB.h"
#include "EcalKXCalib.h"
//using namespace EcalKX3DRecDB;

class EcalKX3DRec //:public TObject
{
   public:
                static TString AMSDataDir;
                //=========== input ==============
		UInt_t   run, event, utime;
		Int_t    DataType; // See EcalKX3DRecDB.h
      Double_t CellDep[EcalKX3DRecDB::kNL][EcalKX3DRecDB::kNC]; // 4 + 72 + 4, 4 fake cells on each side
		Int_t    CellStat[EcalKX3DRecDB::kNL][EcalKX3DRecDB::kNC]; // cell status, from hardware: dead cell / wrong pedestal / sataration
      Short_t  CellFlag[EcalKX3DRecDB::kNL][EcalKX3DRecDB::kNC]; // flag, 1 means fake cell, 0 means real cell
      Int_t    CellMask[EcalKX3DRecDB::kNL][EcalKX3DRecDB::kNC]; // mask, 1 means bad cell for any reason, not used in fit and energy calculation
      // shower shape parameters as a funtion of t/T0
		Double_t Rc[3][30], // core radius
					Rt[3][30], // tail radius
					Fc[3][30]; // core fraction
		// shower fluctuation parameters
		Double_t P0[3][30], P1[3][30], P2[3][30];
		// cell-ratio
		Double_t fLrrPar0, fLrrPar1;
      //=========== intermediate for 3D fit =======
		//--> Current shower under fit
		Int_t    SeedCell[EcalKX3DRecDB::kNL];       // seed in each lay
		Int_t    SeedLayerX, SeedLayerY; // seed layer
		Int_t		XSeed, YSeed; 			// seed in x/y projection
		Double_t LayAmp[EcalKX3DRecDB::kNL];         // amplitubde in each layer
		Double_t LayCrd[EcalKX3DRecDB::kNL];         // coordinate in each layer

		Double_t CellDepS0[EcalKX3DRecDB::kNL][EcalKX3DRecDB::kNC], // energy after subtract identified showers
					CellDepS1[EcalKX3DRecDB::kNL][EcalKX3DRecDB::kNC]; // energy after subtract (identified showers + 2 sigma fluctuation)
		Double_t CellExp[EcalKX3DRecDB::kNL][EcalKX3DRecDB::kNC], // energy deposition with all effects applied(PMT, Att, ...)
					CellEtr[EcalKX3DRecDB::kNL][EcalKX3DRecDB::kNC], // ideal energy deposition
					CellExpNosat[EcalKX3DRecDB::kNL][EcalKX3DRecDB::kNC], // similar to CellExp, except no saturation corretion
					CellExpNoct[EcalKX3DRecDB::kNL][EcalKX3DRecDB::kNC], // similar to CellExp, except no crosstalk correction
					CellDepth[EcalKX3DRecDB::kNL][EcalKX3DRecDB::kNC], // energy weighted depth
					CellDist[EcalKX3DRecDB::kNL][EcalKX3DRecDB::kNC], // energy weighted distance to shower axis
					CellBeta[EcalKX3DRecDB::kNL][EcalKX3DRecDB::kNC]; // conversion factor E->N
      Double_t CellProb[EcalKX3DRecDB::kNL][EcalKX3DRecDB::kNC]; // cell probability
      Double_t CellProbNorm[EcalKX3DRecDB::kNL][EcalKX3DRecDB::kNC]; // subtract reference prob, e_meas = e_exp
		Double_t CellDepCor[EcalKX3DRecDB::kNL][EcalKX3DRecDB::kNC]; // after cell-level correction
		Int_t    Apex;    // apex, based on energy deposited in 3 adjacent cells
		// shower parameters
		Double_t E0, dE0, // energy
					B0, dB0, // beta  
					T0, dT0, // distance from shower apex to maximum
					A0, dA0, // z coordinate of shower apex
					Z0, dZ0, // z coordinate of shower maximum
					X0, dX0, // x coordinate of shower maximum
					Y0, dY0, // y coordinate of shower maximum
					KX, dKX, // kx
					KY, dKY; // ky 
		Double_t E0f, El2f;     // initial value defined by ShowerEstimate
		Double_t ShwrE0f[EcalKX3DRecDB::kNS], ShwrEl2f[EcalKX3DRecDB::kNS], ShwrX0f[EcalKX3DRecDB::kNS], ShwrY0f[EcalKX3DRecDB::kNS], ShwrZ0f[EcalKX3DRecDB::kNS], ShwrKXf[EcalKX3DRecDB::kNS], ShwrKYf[EcalKX3DRecDB::kNS]; // Initial value
		Int_t    fB0,     // flag to control if this varible is fixed in the fit
					fZ0;     // flag = 0, fixed, flag = 1, variable
		Double_t Alpha, GammaAlpha;

		// to define IsHadronLike for preselection
		Double_t LkhdTail1, D0, D1, D2, EseedX, EseedY, sx, sy;
		Int_t    TotNhits, NhitTail1;
		Int_t    IsHadronLike, IsSkipCombineFit;

		Double_t E1;  // fitted energy of the most energetic shower

		Int_t    N_Shwr,  // count of showers
					I_Shwr,  // index of current shower
					I_ShwrTrk, // index of the shower matched with tracker
					N_Dep;   // count of neighboring showers: 10*x_dep + y_dep
		Int_t    NIter[EcalKX3DRecDB::kNS],
					NIterComb;
		Int_t    ShwrFlag[EcalKX3DRecDB::kNS], ShwrDep[EcalKX3DRecDB::kNS][2];  // x/y
		Int_t    ShwrFprt[EcalKX3DRecDB::kNS][EcalKX3DRecDB::kNL][2]; // min/max
		//--> Buffer to handle multiple showers, at most 3 showers will be reconstructed 
		Double_t ShwrExp[EcalKX3DRecDB::kNS][EcalKX3DRecDB::kNL][EcalKX3DRecDB::kNC],   // expected energy of given shower 
					ShwrEtr[EcalKX3DRecDB::kNS][EcalKX3DRecDB::kNL][EcalKX3DRecDB::kNC],   // ideal energy of given shower
					ShwrDepth[EcalKX3DRecDB::kNS][EcalKX3DRecDB::kNL][EcalKX3DRecDB::kNC], // energy weighted depth
					ShwrDist[EcalKX3DRecDB::kNS][EcalKX3DRecDB::kNL][EcalKX3DRecDB::kNC], // energy weighted distance to shower axis
					ShwrBeta[EcalKX3DRecDB::kNS][EcalKX3DRecDB::kNL][EcalKX3DRecDB::kNC];  // conversion factor E->N of given shower
		Int_t    ShwrCBit[EcalKX3DRecDB::kNS][EcalKX3DRecDB::kNL][EcalKX3DRecDB::kNC];  // 1->apex region, 2->core region, 3->tail regio
		Double_t ShwrPar[EcalKX3DRecDB::kNS][EcalKX3DRecDB::kNpar][2];   // shower parameters and errors
		Double_t ShwrLkhd[EcalKX3DRecDB::kNS];            // shower likelihood, boundary conditions
		Double_t ShwrLkhdI[EcalKX3DRecDB::kNS];           // shower likelihood, cells fluctuations
		Double_t ShwrEb[EcalKX3DRecDB::kNS][2];			  // buffering E0 and dE0 of identified showers from ShowerRec(), as boundary condition in ShowerCombineFit()
		Int_t    ShwrApexI[EcalKX3DRecDB::kNS];
		Double_t ShwrParI[EcalKX3DRecDB::kNS][EcalKX3DRecDB::kNpar][2];  // shower parameters from single shower fit
		Int_t    ShwrFlags[EcalKX3DRecDB::kNS][6]; // fX0, fX1, fEX, fY0, fY1, fEY
		Int_t    Flag_overlap;
		Float_t  sum_nsigma_x, sum_nsigma_y;

		// Construct electron-proton estimator
		Double_t R0, R1; // de-correlated energy in L0 and L1
		Double_t Chi2E, Nsigma; // of the core
		Int_t    Npos, Nneg; // number of cells with dep>exp and dep<exp respectively
		Double_t LkhdAxis, // likelihood along shower axis
					LkhdCore, // likelihood in core region
					LkhdTail, // likelihood in tail region
					LkhdBump, // neutron energy in tail region, 3 sigma
					LkhdBump2, // neutron energy in tail region, 2 sigma
					LkhdBump4, // neutron energy in tail region, 1 sigma
					LkhdLong, // likeklihood from 1D longitudinal fit
					SizeX,    // characteristic size of X projection
					SizeY;    // characteristic size of Y projection
		Double_t DepAxis,  // total energy deposition along shower axis
					ExpAxis,  // expected energy along shower axis
					DepCore,  // total energy deposition in shower core
					ExpCore,  // expected energy in shower core
					DepTail,  // total energy deposition in shower tail
					ExpTail,  // expected energy in shower tail
					DepBump, ExpBump, // total and expected energy of "neutron" bumps, 3 sigma 
					DepBump2, ExpBump2, // total and expected energy of "neutron" bumps, 2 sigma 
					DepBump4, ExpBump4; // total and expected energy of "neutron" bumps, 1 sigma 
		Double_t LayLkhd[EcalKX3DRecDB::kNL], // likelihood of each layer
					LayLkhdA[EcalKX3DRecDB::kNL], // axis lkhd of each layer
					LayLkhdReNorm[EcalKX3DRecDB::kNL], // reweighted layer likelihood
					LayDep[EcalKX3DRecDB::kNL],  // real energy deposition of each layer
					LayExp[EcalKX3DRecDB::kNL],  // expected energy of each layer
					LayAxisDepExp[EcalKX3DRecDB::kNL],
					LayCoreDepExp[EcalKX3DRecDB::kNL],
					LaySize[EcalKX3DRecDB::kNL]; // characteristic size of each layer
		Int_t    LayNhit[EcalKX3DRecDB::kNL]; // number of good cells in each layer
		Int_t    NhitAxis, // number of cells in apex region
					NhitCore, // number of cells in core region
					NhitTail, // number of cells in tail region
					NhitBump, // number of "neutron"s, 3 sigma
					NhitBump2, // number of "neutron"s, 2 sigma
					NhitBump4; // number of "neutron"s, 1 sigma
		Double_t S3x, // +-1 cells in x
					S3y, // +-1 cells in y
					Sx,  // all cells in x
					Sy;  // all cells in y
		Double_t Eseed;
		Int_t    NhitAll, NhitOut;
		Double_t EdepAll, EdepOut, LkhdOut;
		Int_t    Lmax, Cmax;
		Double_t Emax, Dmax, Emaxc, Emaxt, Eloss, dPMT, Bmax; // nonlinearity study
		Double_t Emax3, Dmax3, Emaxc3; // energy deposited in 3 maximum cells
		Double_t emx, emx2, emy, emy2, dPMTx, dPMTy; // distance to PMT, of the maximum depistion cell in X/Y projection
		Int_t    LayCmax[18];
		Double_t LayEmax[18], LayDmax[18];

		// Normalized EM variables
		Double_t EmVar[23];
		Double_t EmLkhd[4]; // final EM likelihood:0->integral value, 1->layer value combined, 2->Combine tracker information, 3->Variables to be improved
		Double_t EmBDT;
		Double_t TotEne; // total energy in ECAL, corrected by lateral leak and PMT efficiency
		Double_t TotDep; // raw energy in ECAL, no corrections
		Double_t TotExp; // expected raw energy, to match with Dep
		Double_t TotEmask; // total expected energy of the masked cells
		Double_t AxisEmask; // total expected energy of the masked cells on the shower axis
		Double_t EleEne; // reconstructed energy, with electron hypothesis
		Double_t L2Ene, L2Dep, L2Exp; 
		Double_t EneL2Cor; // using L2R correction
		Double_t EneSat; // total energy of saturated cells
		Int_t    NhitSat; // number of saturated cells
		Double_t ElossSat;// energy loss due to electronics saturation
		Int_t    NhitMask; // number of saturated cells
		Int_t    NhitMiss; // number of cells cannot be recovered by mirror algorithm
		Int_t    NhitLatLeak; // number of imaginary cells used in energy reconstruction
		Float_t  FracLatLeak; // fraction of energy "deposited" in imaginary cells w.r.t. the total energy
		Float_t  FracRearLeak; // fraction of rear leakage
		Double_t E0M, // energy deposition around shower axis, layer 0
					E1M, // energy deposition around shower axis, layer 1 
					E2M; // energy deposition around shower axis, layer 2
		Double_t D0bs, // energy depostion of cells not on the shower axis, layer 0
					D1bs, // energy depostion of cells not on the shower axis, layer 1
					E0bs, // energy expectation of cells not on the shower axis, layer 0
					E1bs;// energy expectation of cells not on the shower axis, layer 1
		// 
		Double_t TrkX0, TrkY0, TrkZ0, TrkKX, TrkKY; // Tracker information
		bool IsTrackerUsed;
		void SetTrackerPath(float trk_p0[], float trk_dir[]); //
		void MatchTrackerAndShower();

      // Correction for inclination, attenuation, lateral leakage
		// from simplified model to real 3d model
		static Bool_t  is_cf_es2ed_loaded;
		static Float_t cf_es2ed[3628800];
		// recover continuous attenuation and finite fiber length
		static Bool_t  is_cf_ed2ei_loaded;
		static Float_t cf_ed2ei[3628800];
		// 
		static Bool_t is_cf_ky_es2ed_loaded;
		static Float_t cf_ky_es2ed[25200];

      // Shower quality 
		Double_t EX,		// total energy in x-projection footprint for fit
					EY,		// total energy in y-projection footprint for fit
					dEX,		// maximum cell energy in x-projection
					dEY,		// maximum cell energy in y-projection
					X1,		// center of gravity in X 
					Y1;		// center of gravity in Y
		Int_t    fEX,		// number of X cells associated during shower footprint recognition, using subtrated energy map
					fEY;		// number of Y cells associated during shower footprint recognition, using subtracted energy map
		Int_t    fEX0,		// number of X cells associated during shower footprint recognition, using original energy map
					fEY0;		// number of Y cells associated during shower footprint recognition, using original energy map
		Int_t		fX0,		// number of X layers with good local seed, based on subtracted deposition
					fY0;  	// number of Y layers with good local seed, based on subtracted deposition
		Int_t		fX1,		// number of X layers with good local seed, based on original deposition
					fY1;  	// number of Y layers with good local seed, based on original deposition
      // Footprint of current shower
      Int_t    Cell_Min[18],
				  	Cell_Max[18],
					Cell_Med[18]; 
		Int_t    Shwr_Med[EcalKX3DRecDB::kNS][18]; // axis cell of each shower on each plane
		Int_t    nfbs[18]; // number of shared fibers
		Int_t    nfbd[18]; // number of unvisible fibers

		Int_t    Index; // for shower shape scanning

		// calibration
		static Int_t DEBUG;
		double GetDistance2PMT(int l, int c);
		void   SetBit(int &a, int b);
		void   ClearBit(int &a, int b);

		static  Int_t NumberOfCellsForFit; // fit footprint size
		static void   SetNumberOfCellsForFit(int a){ NumberOfCellsForFit=a; }
 
   public: 
      EcalKX3DRec();
      ~EcalKX3DRec(){ 
			cout << "~EcalKX3DRec()" << endl; 
		}

		// input
      void 		SetCluster(Float_t ene_cell[][72], Int_t cellstat[][72], bool IsGeV=1);
		void     SetRunEvent(UInt_t _run, UInt_t _event, UInt_t _utime){ run = _run; event = _event; utime = _utime; } 
		void		SetCellStat();
		void     SetDataType(Int_t _dt){ DataType = _dt; }
		void     SetHadronicFlag(){ IsHadronLike = 1; }
		void     MaskCellsAbove(double energy_threshold=0);
		void     MaskCellsInLast2Layer();
		void     MaskCellsInLast4Layer();
      void 		Reset();
		// MC smearing
		void     SmearMC(Float_t ene_cell[][72], int imethod=0);

		// shower shape parameters
      void 		SetLatPar(Int_t ish, Double_t ene); // Energy dependence of lateral shower shape
		// fluctuation parameters
      void     SetPdfPar(Int_t ish, Double_t ene); // Energy dependence of shower fluctuation

		// Cell-Ratio 
		void     CellRatioParToyMC(double ene, double &par0, double &par1);
		void     CellRatioParMC(double ene, double &par0, double &par1);
		void     CellRatioParISS(double ene, double &par0, double &par1);

      // Fast reconstruction
      Int_t		ShowerEstimate();
		Int_t    ShowerClustering();
		Int_t		CalcCoG(Int_t flag=0);
		Int_t    ParabolaZ0();
		Int_t    ShowerAxis();
		Int_t    DefineFootprint();
		void     CalcShowerSize();

		// 1D longitudinal fit
		void     Parabolic1DFit(Double_t&, Double_t&, Double_t&, Double_t&);
		Int_t    Shower1DFit_AK();
		Double_t ShowerLongProfile();
		void 		LinearFit(Int_t n, vector<Double_t> vx, vector<Double_t> vy, vector<Double_t> vw, Double_t &kx, Double_t &x0);

		// used for fast calculation, internal parameters
		static Int_t    GammaFast_FirstCall;
      static Double_t GammaFast(Double_t depth, Double_t _T0);
		static Int_t    GammaFast_par2_FirstCall;
      static Double_t GammaFast_par2(Double_t depth, Double_t _T0);
      static Double_t getCorrEs2Ed(Double_t depth, Double_t dist, Double_t x, Double_t kx, Double_t beta, Int_t flag=0);
      static Double_t getCorrEd2Ei(Double_t depth, Double_t dist, Double_t x, Double_t kx, Double_t beta, Int_t flag=0);
		static Double_t getKyCorrEs2Ed(Double_t depth, Double_t dist, Double_t kx, Double_t ky, Int_t flag=0);
		double GetPMTEfficiency(Int_t l_fiber, Int_t k_fiber, Int_t l_ecal, Int_t cell);
		void   GetPMTEfficiencyFromTable(int l_fb, int k_fb, int &cm, int &cp, double &em, double &ep);

		// shower model, calculate energy deposition in fibers and summed up to cells
		Int_t		ShowerModel(Int_t index=0, Int_t flag=0); // calculate energy in each cell
      Double_t	ShowerShape(Int_t slay, Double_t depth, Double_t dist, Double_t &fsat);
      Double_t	ShowerShapeIdeal(Double_t depth, Double_t dist, Double_t rc, Double_t rt, Double_t fc, Double_t &fsat);

		// single cell resolution limit
		void   SetBetaLimit(); 

		// saturation correction
		void   NonlinearityCorrection(); // non-linearity in the scintillation fiber
		double NonlinearityCorrectionBias;
		void   SetNonlinearityCorrectionBias(double nsigma=0){ NonlinearityCorrectionBias = nsigma; }

		void   CalculateDmax3(); // 
		void   CalculateD8NF();

		// Shower fluctuation and probability
      Int_t 	ShowerProbability(Int_t flag=0);
      Double_t	GetLkhd(Double_t &L1, Double_t &L2); // L1:likelihood from cell fluctuation, L2: from boundary conditions

		Int_t		EcalShwrDef(Int_t k, Int_t flag=0); // calculate each shower
 
		// 3D fit
      Int_t		ShowerRec(Int_t n=0); 
		void     DefineCommonFootprint();
      Int_t		ShowerCombineFit(); // combined fit in case of overlapped showers 
      Int_t		TransFit(Double_t &par, Double_t &par_err, Double_t &L0, Double_t &L1, Double_t &L2); 
      Int_t		TransFitKx(Double_t &L0, Double_t &L1, Double_t &L2); // called by ShowerCombineFit()
      Int_t		TransFitKy(Double_t &L0, Double_t &L1, Double_t &L2); // called by ShowerCombineFit()
		Int_t		ParabolaMin(Int_t n1, Int_t n2, Double_t param[], Double_t lkhd[], Double_t &l1, Double_t &par, Double_t &err);
		Int_t    ShowerFootprint(bool IsExtend=0); // extend the footprint from the one used in fit
		Int_t    GetFootPrintSize(Double_t energy);

		// print or display
		Int_t    ShowerPrint(Int_t k);
		void     ShowerDisplay(); // make plots
		void     PrintShwrPar();

		// overlap shower search
		Int_t    OverlapShowerSearch(int &flag_x, int &flag_y);
		Int_t    OverlapShowerRec();

		// Energy reconstruction
		Double_t ShowerEnergy(Int_t pid=0);
		Double_t ElectronEnergy();
		Double_t ProtonEnergy();
		Double_t PhotonEnergy();
		Double_t GetRearLeakage(Double_t a0, Double_t z0, Double_t kx, Double_t ky);
		Double_t GetRearLeakageL2R(Double_t einit, Double_t l2r);

		// identification estimator
		void     EMEstimatorVars(); // calculate discrimination variables
		void     EMEstimatorLkhd(); // combined in likelihood, easier to control
		void     EMEstimatorBDT();  // combined by BDT, better rejection performance
		void     EMEnergy();        // reconstruct EM shower energy
		void     SortShowers();     // sorted by energy

      // summary output variables
      void     Summary(); // place to calculate output variables related to likelihood
		
//		ClassDef(EcalKX3DRec, 1);
};

#endif
