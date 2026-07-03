#ifndef __ECALKXREC__
#define __ECALKXREC__
/*=======================================================================
 ************** The user interface to 3D shower reconstruction  *********
 * EcalKXFastRec: from raw ADC to MeV, after MIP attenuation correction  **
 * EcalKX3DRec: core part of 3D fit  **************************************
 *=======================================================================*/
#include "EcalKXCalib.h"
#include "EcalKXFastRec.h"
#include "EcalKX3DRec.h"

class EcalKXRec //:public TObject
{
	public:
		static EcalKXRec *GetHead();
		EcalKXRec(){}
		~EcalKXRec(){ 
			cout << "~EcalKXRec()" << endl; 
			if( pEcalKXFastRec ) delete pEcalKXFastRec;
			if( pEcalKX3DRec   ) delete pEcalKX3DRec;
		}

    static EcalKXFastRec *GetEcalKXFastRec();
    static EcalKX3DRec *GetEcalKX3DRec();

		void GetTotalEnergy(Float_t &ene, int opt=0){
			if(opt==0) ene = pEcalKX3DRec->EleEne;
			else if(opt==1) ene = pEcalKX3DRec->EneL2Cor;
		}
		void GetLikelihood(Float_t &lkhd1, Float_t &lkhd2){
			lkhd1 = pEcalKX3DRec->EmLkhd[0];
			lkhd2 = pEcalKX3DRec->EmLkhd[1];
		}
		void GetBDT(Float_t &bdt){ 
			bdt = pEcalKX3DRec->EmBDT; 
		}
		void GetNumberOfShowers(Int_t &nsh){ 
			nsh = N_Shwr; 
		}
		void GetNumberOfElectronicsSaturatedCells(Int_t &ncell){
			ncell = pEcalKX3DRec->NhitSat;
		}
		void GetEnergyLossFromElectronicsSaturation(Float_t &eloss){
			eloss = pEcalKX3DRec->ElossSat;
		}
		void GetEnergyLossFromFiberSaturation(Float_t &eloss){
			eloss = pEcalKX3DRec->Eloss;
		}
		void GetNumberOfLateralLeakCells(Int_t &nlatleak){
			nlatleak = pEcalKX3DRec->NhitLatLeak;
		}
		void GetLateralLeakFraction(Float_t &frac){
			frac = pEcalKX3DRec->FracLatLeak;
		}
		void GetRearLeakFraction(Float_t &frac){
			frac = pEcalKX3DRec->FracRearLeak;
		}
		void GetShowerEnergy(Float_t &_E0, Int_t I_Shwr=0){
			if( I_Shwr>=0 && I_Shwr<N_Shwr )
				_E0 = pEcalKX3DRec->ShwrPar[I_Shwr][0][0];
			else 
				_E0 = 0;
		}
		void GetShowerCenter(AMSPoint &position, Int_t I_Shwr=0){
			if( I_Shwr>=0 && I_Shwr<N_Shwr )
				position = AMSPoint( pEcalKX3DRec->ShwrPar[I_Shwr][3][0], pEcalKX3DRec->ShwrPar[I_Shwr][4][0], pEcalKX3DRec->ShwrPar[I_Shwr][5][0] );
			else
				position = AMSPoint(0,0,0);
		}
		void GetShowerDirection(AMSDir &direction, Int_t I_Shwr=0){
			if( I_Shwr>=0 && I_Shwr<N_Shwr )
				direction = AMSDir( pEcalKX3DRec->ShwrPar[I_Shwr][6][0], pEcalKX3DRec->ShwrPar[I_Shwr][7][0], 1 );
			else
				direction = AMSDir(0,0,1);
		}
		void GetShowerStartPoint(Float_t &_A0, Int_t I_Shwr=0){
			if( I_Shwr>=0 && I_Shwr<N_Shwr )
				_A0 = pEcalKX3DRec->ShwrPar[I_Shwr][2][0];
			else
				_A0 = 0;
		}
		void IsShowerContained(Int_t &_is_shower_contained, Int_t I_Shwr=0){
			_is_shower_contained=0;
			if( fabs(pEcalKX3DRec->ShwrPar[I_Shwr][3][0] )<31.5 ) _is_shower_contained |= 1; // X
			if( fabs(pEcalKX3DRec->ShwrPar[I_Shwr][4][0] )<31.5 ) _is_shower_contained |= 2; // Y
			if( pEcalKX3DRec->ShwrPar[I_Shwr][5][0] > EcalKX3DRecDB::EcalZ[16] ) _is_shower_contained |= 4; // Z
		}
		void GetHadronFlag(Int_t &hadflag){ hadflag = pEcalKX3DRec->IsHadronLike; }
		void ShowerDisplay(){ pEcalKX3DRec->ShowerDisplay(); }
		double GetMeasuredCellEnergy(int layer, int cell){
			if( layer<0 || layer>17 || cell<-4 || cell>75 ){ 
				std::cerr << "EcalKXRec::GetMeasuredCellEnergy() request out of range : layer=" << layer << ", cell=" << cell << std::endl;
				return 0.;
			}else{
				return pEcalKX3DRec->CellDep[layer][cell+4];
			}
		}
		double GetExpectedCellEnergy(int layer, int cell){
			if( layer<0 || layer>17 || cell<-4 || cell>75 ){ 
				std::cerr << "EcalKXRec::GetExpectedCellEnergy() request out of range : layer=" << layer << ", cell=" << cell << std::endl;
				return 0.;
			}else{
				return pEcalKX3DRec->CellExp[layer][cell+4];
			}
		}
		
		static void SetNumberOfCellsForFit(int a){ pEcalKX3DRec->SetNumberOfCellsForFit(a); }
		static void SetDEBUGLevel_EcalKX3DRec(int a){ pEcalKX3DRec->DEBUG = a; }

		static void SetLocalDatabaseDirectory(TString _dir){ 
			pEcalKXFastRec->pEcalKXCalib->IsLocalDirectory = 1; 
			pEcalKXFastRec->pEcalKXCalib->AMSDataDir = _dir; 
			pEcalKX3DRec->AMSDataDir = _dir;
		}
		static void ProcessAMSEvent(AMSEventR *_pev, Int_t isHadronicShower=0); 
		static void ProcessADCMap(UInt_t utime, Float_t ADCH[][72], Float_t ADCL[][72], Int_t Status[][72], Int_t dataType=EcalKX3DRecDB::_ISS_DATA, Int_t _is_hadronic_shower=0, Int_t _showershape_type=-1); 
	private:
		static Int_t N_Shwr;
		static void Do3DReconstruction(Int_t isHadronicShower=0, Int_t dataType=EcalKX3DRecDB::_ISS_DATA);
		//EcalKXRec();
		static EcalKXFastRec *pEcalKXFastRec;
		static EcalKX3DRec   *pEcalKX3DRec;
		static EcalKXRec    *Head;

//		ClassDef(EcalKXRec,2)
};

#endif
