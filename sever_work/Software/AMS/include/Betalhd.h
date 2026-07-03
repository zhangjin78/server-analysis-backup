#ifndef __Betalhd__
#define __Betalhd__

#include <sys/types.h>
#include "TrdKCluster.h"
#include "Rtypes.h"
#include "EcalChi2CY.h"
#include "TofTrack.h"
#include "Tofrec02_ihep.h"
#include "TrFit.h"
#include "TrTrackSelection.h"
#include "TrExtAlignDB.h"
#include "TrRecon.h"
#include "TVector3.h"
#include "TFile.h"
#include "TString.h"
#include <TrTrack.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include "TH3.h"
#include "Fit/FitConfig.h"
#include "TRandom3.h"
#include "Math/MinimizerOptions.h"
#include "Fit/Fitter.h"
#include "Fit/BinData.h"
#include "Fit/Chi2FCN.h"
#include "commonsi.h"
#include "TProfile2D.h"
#include "SpFold.h"
//#include "LxGeomHashes.h"
using namespace std;
////////////////////////////////////////////////////////////////////////
class Betalhd {
	/////////////////////////////////////////////////////
	//  This is a function to estimate beta of the particle measured by AMS-02
	//  
	//  Some functions are interesting for other studies:
	//  Caculate energy loss      ------>          CalculateEnergyLoss
	//  Bethe equation            ------>          BetheEquation
	//  Get elemental abundance   ------>          betalhd_GetElementAbundance
	//
	//  
	//  Some dedicated definitions for the moment in this class:
	//  1) the number of Tracker layers from top to buttom is from 0 to 8 (normally, it should be from 1 to 9).
	//  2) the number of TRD layers from top to buttom is from 0 to 19 (normally, it should be from 19 to 0).
	//
	//
	//
	//
	//
	/////////////////////////////////////////////////////
	public:
		Betalhd(){;}
		~Betalhd() {;}
		double operator() (const double *par) const{//for Minuite minimization
			double result;
			result = Betalhd::getchi2_offline(par);
			return result;
		};
		/// get element abundance in unit of mole/cm2 on the given trajectory between two points.
		static int betalhd_GetElementAbundance(const AMSPoint & pnt, const AMSDir & dir, double rigidity, double z1, double z2, double elem[9]);

		static double GetRadiationLength(const AMSPoint & pnt, const AMSDir & dir, double rigidity, double z1, double z2);
			
		/// Get beta likelihood using a genetic algorithm method.
		/// Option definitions:  0: L1-dEdx(down-going)  1:L9-dEdx(upgoing)  10:L1-TOF  20:L1-TOF+dEdx   100: using template wrt pl1 (regression method)      200: using template wrt pl1 TOF estimated (regression method)
		///These Option definitions are applied for the whole class.	
		static double GetBetaLikelihood(float & beta, AMSEventR * ev, TrTrackR * trk, int opt = 0, BetaHR *bth = NULL);	

		
		static double GetBetaLikelihood_TOFRICH(float & beta, float & beta_error, AMSPoint * trk_pnt, AMSPoint * tof_pnt, float * Time, float * TimeE, float * PathLength,bool IsNaF, float beta_rich_rec, TrProp prop, int opt = 0);
		//Get beta likelihood with TOF and RICH measurements (without energy loss in the inner Tracker layers)
		static double GetBetaLikelihood_TOFRICH(float & beta, float & beta_error, AMSPoint * trk_pnt, AMSPoint * tof_pnt, float * Time, float * TimeE, float * PathLength, float * PathLength_trk , int bthpattern ,bool IsNaF, float beta_rich_rec, TrProp prop, float beta_rich_err = -1, int opt = 0);
		//Get beta likelihood with TOF and RICH measurements (with energy loss in the inner Tracker layers)
		
		static double GetBetaLikelihood_TOFRICHhit(float & beta, float & beta_error, AMSPoint * trk_pnt, AMSPoint * tof_pnt, float * Time, float * TimeE, float * PathLength, float * PathLength_trk , int bthpattern ,AMSDir * tof_dir, bool IsNaF,vector<float> hit_beta_ini[2], int opt = 0, int charge = 1,  double m_p = 0.938272);
		//Get beta likelihood with TOF and RICH measurements (with energy loss in the inner Tracker layers)

		
		static double GetBetaLikelihood_TOF(float & beta, float & beta_error, AMSPoint * trk_pnt, AMSPoint * tof_pnt, float * Time, float * TimeE, float * PathLength, float * PathLength_trk = 0, int bthpattern = 4444, int opt = 0,  int charge = 1, double m_p = 0.938272);
		//Get beta likelihood with TOF measurement (without TRD hit positions)
		
		static double GetBetaLikelihood_TOF(float & beta, float & beta_error, AMSPoint * trk_pnt,  AMSPoint * trd_pnt, AMSPoint * tof_pnt, float * Time, float * TimeE, float * PathLength, float * PathLength_trk = 0, int bthpattern = 4444, int opt = 0,  int charge = 1, double m_p = 0.938272);
		//Get beta likelihood with TOF measurement (with TRD hit positions)

		static double GetBetaLikelihood_TOF(float & beta, float & beta_error);
		//Get beta likelihood with TOF measurement (with TRD hit positions)

		static double GetBetaLikelihood_TOFdEdx(
				float & beta, float & beta_error, AMSPoint * trk_pnt,  AMSPoint * trd_pnt, AMSPoint * tof_pnt, 
				float * Time, float * TimeE, float * PathLength, float * PathLength_trk, 	
				float trk_dEdx[2][9], float trd_dEdx[20], float tof_dEdx[4],
				int bthpattern = 4444, int opt = 0, int charge = 1, double m_p = 0.938272
				);
		//Get beta likelihood with TOF measurement + dEdx measurement (with TRD hit positions)
	
		static double GetBetaLikelihood_dEdx(
				float & beta, float & beta_error, AMSPoint * trk_pnt,  AMSPoint * trd_pnt, AMSPoint * tof_pnt, 	
				float trk_dEdx[2][9], float trd_dEdx[20], float tof_dEdx[4],
				int opt = 0,
				float m_p = 0.938272,
				int charge = 1,
				int subopt = 111
				);
		//subopt=100, tracker; subopt=010, TRD; subopt=001, TOF.
		//Get beta likelihood with dEdx measurement (with TRD hit positions)
		static double GetBetaLikelihood_dEdx(
				float & beta, float & beta_error, 	
				int opt = Betalhd::betalhd_opt,
				float m_p = 0.938272,
				int charge = 1,
				int subopt = 111
				);
		//subopt=100, tracker; subopt=010, TRD; subopt=001, TOF.
		//Get beta likelihood with dEdx measurement (with TRD hit positions)




		static double GetBetaLikelihood_RICH(bool IsNaF, float & beta_rich_rec, float & beta_err, TrProp prop, float beta_rich_err = -1, int opt = 0);
		//Get beta likelihood with RICH measurement
		
		static double GetBetaLikelihood_RICHhit(float & beta, float & beta_error, AMSPoint * trk_pnt, AMSPoint * tof_pnt, AMSDir * tof_dir, bool IsNaF,vector<float> hit_beta_ini[2], int opt = 0, int charge = 1,  double m_p = 0.938272);
		//Get beta likelihood with TOF and RICH measurements (with energy loss in the inner Tracker layers)

		static double GetBetaLikelihood(float & beta, float & beta_error, int charge = 1, float m_p = 0.938272);
		//Get beta likelihood with TOF measurement + dEdx measurement + RICH measurement
	
		static double GetBetaLikelihood(float & beta, float & beta_error, AMSPoint * trk_pnt,  AMSPoint * trd_pnt, AMSPoint * tof_pnt, float * Time, float * TimeE, float * PathLength, float * PathLength_trk,float trk_dEdx[2][9], float trd_dEdx[20], float tof_dEdx[4],	bool IsNaF = 0, float beta_rich = 0, float beta_rich_err = -1,AMSDir * tof_dir = NULL, int bthpattern = 4444, int opt = 0, int charge = 1, float m_p = 0.938272, bool FitInnerRigidity = false 
				);
		//Get beta likelihood with TOF measurement + dEdx measurement + RICH measurement
	
		static double GetBetaLikelihood(float & beta, float & beta_error, AMSPoint * trk_pnt,  AMSPoint * trd_pnt, AMSPoint * tof_pnt, float * Time, float * TimeE, float * PathLength, float * PathLength_trk,float trk_dEdx[2][9], float trd_dEdx[20], float tof_dEdx[4],	int IsNaF = 0, float beta_rich = 0, float beta_rich_err = -1,AMSDir * tof_dir = NULL, int bthpattern = 4444, int opt = 0, int charge = 1, float m_p = 0.938272, bool FitInnerRigidity = false
				);
		//Get beta likelihood with TOF measurement + dEdx measurement + RICH measurement

		static double GetBetaLikelihood(float & beta, float & beta_error, AMSPoint * trk_pnt,  AMSPoint * trd_pnt, AMSPoint * tof_pnt, float * Time, float * TimeE, float * PathLength, float * PathLength_trk,float trk_dEdx[2][9], float trd_dEdx[20], float tof_dEdx[4],	bool IsNaF, vector<float> hit_beta_ini[2], AMSDir * tof_dir = NULL, int bthpattern = 4444, int opt = 0, int charge = 1, float m_p = 0.938272, bool FitInnerRigidity = false
				);
		//Get beta likelihood with TOF measurement + dEdx measurement + RICH hit measurement

		static double GetBetaLikelihood(float & beta, float & beta_error, AMSPoint * trk_pnt,  AMSPoint * trd_pnt, AMSPoint * tof_pnt, float * Time, float * TimeE, float * PathLength, float * PathLength_trk,float trk_dEdx[2][9], float trd_dEdx[20], float tof_dEdx[4],	int IsNaF, vector<float> hit_beta_ini[2], AMSDir * tof_dir = NULL, int bthpattern = 4444, int opt = 0, int charge = 1, float m_p = 0.938272, bool FitInnerRigidity = false
				);
		//Get beta likelihood with TOF measurement + dEdx measurement + RICH hit measurement

		static double GetBetaLikelihood_TOFRICH(float & beta, float & beta_error, AMSPoint * trk_pnt,  AMSPoint * trd_pnt, AMSPoint * tof_pnt, float * Time, float * TimeE, float * PathLength, float * PathLength_trk,
				bool IsNaF = 0, float beta_rich = 0, float beta_rich_err = -1,AMSDir * tof_dir = NULL, int bthpattern = 4444, int opt = 0, int charge = 1, float m_p = 0.938272
				);
		//Get beta likelihood with TOF measurement + RICH measurement

		static double GetBetaLikelihood_TOFRICH(float & beta, float & beta_error, AMSPoint * trk_pnt,  AMSPoint * trd_pnt, AMSPoint * tof_pnt, float * Time, float * TimeE, float * PathLength, float * PathLength_trk,
				int IsNaF = 0, float beta_rich = 0, float beta_rich_err = -1,AMSDir * tof_dir = NULL, int bthpattern = 4444, int opt = 0, int charge = 1, float m_p = 0.938272
				);
		//Get beta likelihood with TOF measurement + RICH measurement




		static bool GetMCTrueEnergy(AMSEventR * ev, double *trkmcE, double *trdmcE, double *tofmcE, int opt = 0);
		//Get MC True Energy on each hit positions.

		static double CalculateBetaLikelihood( double *trk_dEdx, double * trd_dEdx, double *tof_dEdx, double * trk_beta_tmp, double *trd_beta_tmp, double *tof_beta_tmp);// Step 2: calculate likelihood
		//Calculate beta likelihood based on dE/dx method with templates of MC true energy on the hits.

		static double CalculateBetaLikelihood_cal( double *trk_dEdx, double * trd_dEdx, double *tof_dEdx, double * trk_beta_tmp, double *trd_beta_tmp, double *tof_beta_tmp);// Step 2: calculate likelihood
		//Calculate beta likelihood based on dE/dx method with templates of TOF fitted energy on the hits.

		static double CalculateBetaLikelihood_pl1( double *trk_dEdx, double * trd_dEdx, double *tof_dEdx, double beta);// Step 2: calculate likelihood, beta is the one at pl1
		//Calculate beta likelihood based on dE/dx method with templates of MC true energy on layer 1 (regression method).

		static double CalculateBetaLikelihood_cal_pl1( double *trk_dEdx, double * trd_dEdx, double *tof_dEdx, double beta);// Step 2: calculate likelihood, beta is the one at pl1
		//Calculate beta likelihood based on dE/dx method with templates of TOF fitted energy on layer 1 (regression method).

		static double CalculateBetaLikelihood_TOF(TrTrackR *trk, BetaHR* bth, AMSPoint * trk_pnt, AMSPoint * tof_pnt, double * trk_beta_tmp, double * tof_beta_tmp, int opt = 0);
		//Calculate beta likelihood based on TOF time measurements

		static double CalculateBetaLikelihood_offline_dEdx( double trk_dEdx[2][9], double trd_dEdx[20], double tof_dEdx[4], double * trk_beta_tmp, double *trd_beta_tmp, double *tof_beta_tmp);// Step 2: calculate likelihood
		//Calculate beta likelihood based on dE/dx method with templates of MC true energy on the hits.

		static double CalculateBetaLikelihood_offline_TOF(float * time, float * timeE, float * pathlength, double * tof_beta_tmp, int bthpattern=4444, int opt = 0);//Step 1: calculate expected beta from incoming beta
		//Calculate beta likelihood based on TOF time measurements (without energy loss in the inner Tracker layers)

		static double CalculateBetaLikelihood_offline_TOF(float *time, float *timeE, float *pathlength, double *tof_beta_tmp, float *pathlength_trk, double *trk_beta_tmp, int bthpattern=4444, int opt = 0);
		//Calculate beta likelihood based on TOF time measurements (with energy loss in the inner Tracker layers)

		static double CalculateBetaLikelihood_offline_RICH(double beta_rich, int opt = 0);
		//Calculate beta likelihood based on RICH measurement 

		static double CalculateBetaLikelihood_offline_RICH_hit(double beta_rich);
		//Calculate beta likelihood based on RICH measurement 

		static double CalculateBetaLikelihood_rigidity( int & ndf_trk, AMSPoint * trk_pnt, AMSDir trk_dir, double trk_dEdx[2][9], double trk_beta[9], double m_p, int charge, int opt = 0);
		//Calculate beta likelihood based on Tracker coordinate measurement
		//opt=0: downgoing; opt=1: upgoing

		static double CalculateBetaLikelihood_TOF_pl1(TrTrackR *trk, BetaHR* bth, AMSPoint * tof_pnt, double beta);//Step 1: calculate expected beta from incoming beta
		//Calculate beta likelihood based on TOF time measurements. 
		//Templates are generated wrt to layer 1 MC true energy (regression method)

		static double CalculateBetaLikelihood_TOF_cal_pl1(TrTrackR *trk, BetaHR* bth, AMSPoint * tof_pnt, double beta);//Step 1: calculate expected beta from incoming beta
		//Calculate beta likelihood based on TOF time measurements
		//Templates are generated wrt to layer 1 TOF fitted energy (regression method)
		
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//Maximum Likelihood
		static double CalculateMaximumBetaLikelihood_RICH_hit();
		static double CalculateMaximumBetaLikelihood_TOF();
		static int GetNDF();
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		
		static int InitialBetaLikelihood(AMSEventR * ev, TrdKCluster trdkcluster, TrTrackR * trk = NULL, BetaHR *bth = NULL,  int correction = 1);
		static int InitialBetaLikelihood(AMSEventR * ev,  int correction = 1);

		static double TOF4Layersfcn(Double_t *x, Double_t *par);
		static double CalculateEnergyLoss(const AMSPoint  pnt, const AMSDir  dir, double rigidity, double z1, double z2, double m_p = 0.938272, double charge = 1, int opt = 0);//Calculate particle energy loss in GeV
		static double BetheEquation(double beta, double ZA, double I, int z = 1, double m_p = 938.272);//energy deposition calculation by Bethe equation, in MeV
		static double GetLikelihood(double beta, double dEdx, int detector = 0);// 0:Tracker; 1:TRD; 2:TOF
		static double GetLikelihood_cal(double beta, double dEdx, int detector = 0);// 0:Tracker; 1:TRD; 2:TOF
		static double GetLikelihood_pl1(double beta, double dEdx, int detector = 0, int ilayer = 0);// 0:Tracker; 1:TRD; 2:TOF
		static double GetLikelihood_cal_pl1(double beta, double dEdx, int detector = 0, int ilayer = 0);// 0:Tracker; 1:TRD; 2:TOF
		static double EvalPDF(TH2D* h, double betagamma, double dEdx);
		static double InterpolateGrid(double x, double x_0, double x_1, double y_0, double y_1);

		static bool CalculateExpectedBetaMC(double *trk_beta_tmp, double * trd_beta_tmp, double *tof_beta_tmp, float & beta, double * trkmcE, double *trdmcE, double *tofmcE, int opt = 0);
		//calculate true beta on each hit from MC

		static bool CalculateExpectedBeta(double *trk_beta_tmp, double * trd_beta_tmp, double *tof_beta_tmp, float & beta, AMSPoint *trk_pnt, AMSPoint * trd_pnt, AMSPoint * tof_pnt, const double m_p, const int charge = 1, int opt = 0);
		//calculate expected beta on each hit from incoming beta

		static bool CalculateExpectedBeta_TOF(double * trk_beta_tmp, double *tof_beta_tmp, float & beta, AMSPoint * trk_pnt, AMSPoint * tof_pnt, const double m_p, const int charge = 1, int opt = 0);
		//calculate expected beta on each hit from incoming beta without TRD positions

		static bool CalculateExpectedBeta_z(double & beta_z, double & beta, double z_aim, TrProp prop, const double m_p, const int charge = 1, int opt = 0);
		//calculate expected beta for a given z-position from incoming beta

		static bool CalculateExpectedBeta_z(double & beta_z, float & beta, double z_aim, TrProp prop, const double m_p, const int charge = 1, int opt = 0);
		//calculate expected beta for a given z-position from incoming beta

		static bool CalculateExpectedBeta_z2z(double & beta_z, double & beta, double z_aim, double z_ini, TrProp prop, const double m_p, const int charge = 1, int opt = 0, double step = 1);
		//calculate expected beta for a given z-position from z_ini beta

		static bool CalculateExpectedBeta_z2z(double & beta_z, float & beta, double z_aim, double z_ini, TrProp prop, const double m_p, const int charge = 1, int opt = 0, double step = 1);
		//calculate expected beta for a given z-position from z_ini beta

		static TrProp CopyTrProp(TrProp trprop, int opt = -1);
		static bool TrPropagation(TrProp trprop_o, double z1, double J[5][5]);
		static double BetaToGamma(double beta);
		static double GammaToBeta(double gamma);



		/////////////////////////////////////////////////////////////////////
		//The following functions are for getting the variable used in the beta estimator from AMS data.
		static int GetVariable(AMSEventR * ev, 
				float trk_dEdx[2][9], float trd_dEdx[20], float tof_dEdx[4], 
				float trk_coo[10][3], float trd_coo[20][3], float tof_coo[4][3],
				float trk_d[10][2], float trd_d[20][2], float tof_d[4][2],
				float * Time, float * TimeE, float * PathLength, float * PathLength_trk,
				int & bthpattern,
				TrTrackR * trk = NULL, BetaHR *bth = NULL, TrdTrackR * trd = NULL, int opt = 0);
		static int GetVariable(AMSEventR * ev, 
				float trk_dEdx[2][9], float trd_dEdx[20], float tof_dEdx[4], 
				float trk_coo[10][3], float trd_coo[20][3], float tof_coo[4][3],
				float trk_d[10][2], float trd_d[20][2], float tof_d[4][2],
				float * Time, float * TimeE, float * PathLength, float * PathLength_trk,
				int & bthpattern,
				TrdKCluster trdkcluster,
				TrTrackR * trk = NULL, BetaHR *bth = NULL,  int opt = 0);
		static int GetVariable(AMSEventR * ev, 
				float trk_dEdx[2][9], float trd_dEdx[20], float tof_dEdx[4], 
				float trk_coo[10][3], float trd_coo[20][3], float tof_coo[4][3],
				float trk_d[10][2], float trd_d[20][2], float tof_d[4][2],
				float * Time, float * TimeE, float * PathLength, float * PathLength_trk,
				TrTrackR * trk = NULL, BetaHR *bth = NULL, TrdTrackR * trd = NULL, int opt = 0);
		static int GetVariable(AMSEventR * ev, 
				float trk_dEdx[2][9], float trd_dEdx[20], float tof_dEdx[4], 
				float trk_coo[10][3], float trd_coo[20][3], float tof_coo[4][3],
				float trk_d[10][2], float trd_d[20][2], float tof_d[4][2],
				float * Time, float * TimeE, float * PathLength, float * PathLength_trk,
				TrdKCluster trdkcluster,
				TrTrackR * trk = NULL, BetaHR *bth = NULL,  int opt = 0);
		static int GetVariable_RICH(bool & IsNaF, float & rich_beta, float & rich_beta_err, AMSEventR * ev, RichRingR *richring = NULL, int opt = 0); 

		static double GetTOFAmpCorrection(int layer, int ut);
		static void GetMCTrackerAmpCorrection(float trk_dEdx[2][9]);
		static void GetMCTrackerAmpCorrection(double trk_dEdx[2][9]);
		//////////////////////////////////////////////////////////////////
		//The following functions are used for the genetic algorithm
		static double variation1D(double x, double sigma);
		static int    GetChromoRoulette(double *chi2, const unsigned np);
		static double GA_Mode(double*p, const unsigned np);
		static void   CreatUnitVectors(double *AnyVector_1D, const unsigned np);
		static double GA(double *p, double *sigma_p, const unsigned np, double *p_limitleft, double *p_limitright, AMSEventR *ev, double * trk_dEdx, double *trd_dEdx, double *tof_dEdx, AMSPoint * trk_pnt, AMSPoint * trd_pnt, AMSPoint * tof_pnt, const double m_p = 0.938272, const int charge = 1, const int opt = 0, BetaHR *bth = NULL, TrTrackR * trk = NULL);
		static double getchi2( double *p, int np, AMSEventR *ev, double * trk_dEdx, double *trd_dEdx, double *tof_dEdx, AMSPoint * trk_pnt, AMSPoint * trd_pnt, AMSPoint * tof_pnt, const double m_p = 0.938272, const int charge = 1, const int opt = 0, BetaHR * bth = NULL, TrTrackR * trk = NULL);
		///////////////////////////////////////////////////////////////////
		//The following functions are chi2 definition for Minuit method
		static double getchi2_offline( const double *par );
		static double getchi2_offline_TOF( const double *par );
		static double getchi2_offline_RICH( const double *par );
		static double getchi2_offline_TOFRICH( const double *par );
		///////////////////////////////////////////////////////////////////
		static void SetDeBug(bool debug, int version = 0);
		static bool DeBug;
		static bool DeBug_1;
		static bool IsMC;
		static bool z2zPropagation;
		static int IsTOFRICH;//0: TOF fit; 1 : RICH fit;
		static int IsTOF;
		static int IsdEdx;
		static int IsRICH;
		static int IsRICHhit;
		static bool WithTrd;//If use trd coordinates.
		static int ini_TOF( AMSPoint * trk_pnt, AMSPoint * tof_pnt, float * Time, float * TimeE, float * PathLength, float * PathLength_trk,  
				int bthpattern = 4444, int opt = 0, double m_p = 0.938272, int charge = 1);
		static int ini_TOF( AMSPoint * trk_pnt, AMSPoint * trd_pnt, AMSPoint * tof_pnt, float * Time, float * TimeE, float * PathLength, float * PathLength_trk,  
				int bthpattern = 4444, int opt = 0, double m_p = 0.938272, int charge = 1);
		static int ini_dEdx( AMSPoint * trk_pnt, AMSPoint * trd_pnt, AMSPoint * tof_pnt, 
				float trk_dEdx[2][9], float trd_dEdx[20], float tof_dEdx[4],
				int opt = 0, double m_p = 0.938272, int charge = 1, int subopt = 101);
		static int ini_dEdx( AMSPoint * trk_pnt, AMSPoint * trd_pnt, AMSPoint * tof_pnt, 
				double trk_dEdx[2][9], double trd_dEdx[20], double tof_dEdx[4],
				int opt = 0, double m_p = 0.938272, int charge = 1, int subopt = 101);
		static float betalhd_Time[4], betalhd_TimeE[4], betalhd_PathLength[4],  betalhd_PathLength_trk[7];
		static double betalhd_trk_dEdx[2][9], betalhd_trd_dEdx[20], betalhd_tof_dEdx[4];
		static AMSPoint  betalhd_trk_pnt[10];
		static AMSPoint  betalhd_tof_pnt[4];
		static AMSPoint  betalhd_trd_pnt[20];
		static AMSDir  betalhd_trk_dir[10];
		static AMSDir  betalhd_tof_dir[4];
		static AMSDir  betalhd_trd_dir[20];
		static int betalhd_opt;
		static int dEdx_opt;
		static int betalhd_bthpattern;
		static double betalhd_m_p;
		static int betalhd_charge;
		static int betalhd_ndf;
		static int ini_RICH( bool IsNaF, float beta_rich_rec, float beta_rich_err, TrProp prop, int opt = 0, double m_p = 0.938272, int charge = 1);
		static int ini_RICHhit( bool IsNaF, vector<float> hit_beta_ini[2], int opt = 0, double m_p = 0.938272, int charge = 1);
		static bool betalhd_IsNaF;
		static double betalhd_beta_rich_rec;
		static double betalhd_beta_rich_err;
		static TrProp betalhd_prop;
		static bool betalhd_FitInnerRigidity;
		////////////////////////RICH BDT///////////////////////////////////////////
		static TMVA::Reader * RICHBDTreader[2];
		static float RICHBDTvar[21];
		static double GetClassifierValue(float p[21], int IsNaF, TString method = "BDTD");
		////////////////////////Tracker BDT///////////////////////////////////////////
		static TMVA::Reader * TrackerBDTreader[2];
		//static float TrackerBDTvar[23];//zwengntuple's variables
		//tatic double GetTrackerClassifierValue(float p[23], int IsNaF, TString method = "BDTD");
		static float TrackerBDTvar[31];
		static double GetTrackerClassifierValue(float p[31], int IsNaF, TString method = "BDTD");
		////////////////////////RICH reconstruction///////////////////////////////////
		static vector<float> hit_beta[2];
		static float sigma_naf[2];
		static float sigma_agl[2];
		static int rich_usedhits_direct;
		static int rich_usedhits_mirror;
		static bool update_variable;
		static bool init_rich_correction(TString file);
		static TFile * rich_xy_correction_file;
		static TProfile2D * rich_xy_agl;
		static TProfile2D * rich_xy_naf;
		static TProfile2D * rich_vxvy_agl;
		static TProfile2D * rich_vxvy_naf;
		static double get_rich_xy_correction(double x, double y, bool IsNaF);
		static double get_rich_vxvy_correction(double x, double y, bool IsNaF);
		static double get_time_corrected_beta(float beta, int ut, bool IsNaF);
		static double get_rich_correction(double x, double y, double vx, double vy, bool IsNaF);
		static TF1 * tf_time_corr;
		static TF1 * tf_time_corr_naf;
		////////////////////////TOF reconstruction///////////////////////////////////////
		static int GetTOFResidual(float ibeta[4], float ibeta_err[4], float residual[4], float residual_err[4], float Time[4], float TimeE[4], float PathLength[4]);//Linear fit residual
		static int GetTOFResidual(double ibeta[4], double ibeta_err[4], double residual[4], double residual_err[4], double Time[4], double TimeE[4], double PathLength[4]);//Linear fit residual
		static double GetTOFBeta_linear(float & ibeta, float & ibeta_err, float Time[4], float TimeE[4], float PathLength[4]);//Beta linear fit 
		static double GetTOFTimeCorrection_UT(int ilayer, int UT);//TOF Time dependent Time correction. ilayer: 0 ~3
		static TF1 * spline_TOF_time[4];
		//static GeomHashBetaEnsemble * TOFTimeHash[4];
		//static double GetTOFTimeCorrection_coo(int ilayer, float x, float y, float vx, float vy);//TOF coo dependent Time correction. ilayer: 0 ~ 3
		static double GetTOFTimeCorrection_coo(int ilayer, float x, float y);//TOF coo dependent Time correction. ilayer: 0 ~ 3
		static TFile * file_TOFTime;
		static TFile * file_TOFCoo;
		//static TProfile2D * h_xy_corr[4];
		static TH2D * h_xy_corr[4]; //v6.01 above
		static void MCtuneTime(float & time, int ilayer, int RUN = 0, int EVENT =0);
		static double MCtunePar_width; //MC tuning parameter, in ps
		static double MCtunePar_shift; //MC tuning parameter, in ps
		static bool extrapolation;

		/////////////////////////Configuration E//////////////////////////////////////
		/// To see if the track is cross the ladders in configuration E.
		static bool IsTrackerOffConf(TrTrackR * track, int & patt_i, int & patt, int iconf = 1); 

};

#endif
