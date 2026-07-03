#ifndef __ECALKXCALIB__
#define __ECALKXCALIB__

#include "TTree.h"
#include "TFile.h"
#include "TString.h"
#include "TMath.h"
#include "signal.h"
#include <iostream>
#include <fstream>

// AMS/include
#include <time.h>
#include <id.h>
#include <amsdbc.h>
//#include <commonsi.h> // AMSDATADIR

//#include "ECALXConst.h"

class EcalKXCalib;

// time-independent parameters, used for reading DB
class EcalStaticCalibPar { // 
	static const int nattpar = 3;
	static const int nLayer = 18;
	static const int nCell = 72;
	public:
		EcalStaticCalibPar(long _time_start, long _time_end, float *_attpar, float *_gain, float *_hlr, float *_hdr): time_start(_time_start), time_end(_time_end) {
			for(int lay=0; lay<nLayer; lay++){
				for(int cell=0; cell<nCell; cell++){
					int index = lay*nCell + cell;
					hlr[lay][cell] = _hlr[index];
					hdr[lay][cell] = _hdr[index];
					for(int ipar=0; ipar<nattpar; ipar++){
						attpar[lay][cell][ipar] = _attpar[index*nattpar + ipar];
					}
					gain[lay][cell] = _gain[index];
					// protection
					if( attpar[lay][cell][0] <= 0 ){
						attpar[lay][cell][0] = 0.2;
						attpar[lay][cell][1] = 10.;
						attpar[lay][cell][2] = 200.;
					}
					// 
					if( hlr[lay][cell]<= 0 ) hlr[lay][cell] = 33.;
					if( hdr[lay][cell]<= 0 ) hdr[lay][cell] = 30.;
					if( gain[lay][cell]<=0 ) gain[lay][cell] = 20.;
				}
			}
		}
		EcalStaticCalibPar(){
			time_start = 0;
			time_end = 0;
			memset(attpar, 0, sizeof(attpar));
			memset(gain, 0, sizeof(gain));
			memset(hlr, 0, sizeof(hlr));
			memset(hdr, 0, sizeof(hdr));
		}
		~EcalStaticCalibPar(){};
		void Print(){
			std::cout << "========= EcalStaticCalibPar ============= " << std::endl;
			for(int lay=0; lay<nLayer; lay++){
				std::cout << "--- layer " << lay << std::endl;
				for(int cell=0; cell<nCell; cell++){
					std::cout << Form("Lay %02d, Cell %02d, Gain %.2f, Attpar[]=( %.3f %.1f, %.1f ), H/L=%.1f, H/D=%.1f", lay, cell, gain[lay][cell], attpar[lay][cell][0], attpar[lay][cell][1], attpar[lay][cell][2], hlr[lay][cell], hdr[lay][cell] ) << std::endl;
				}
				std::cout << std::endl;
			}
		}
		void Write2RootFile(TString fname);

		long time_start;
		long time_end;
		float attpar[nLayer][nCell][nattpar]; // attenation parameters, 0->fast att. frac.; 1->fast att. len., 2->slow att. len.; 
		float gain[nLayer][nCell]; // absolute gain in ADC
		float hlr[nLayer][nCell]; // High/Low gain ratio
		float hdr[nLayer][nCell]; // High/Dynode gain ratio
};

// time-dependent part
class EcalDynamicCalibPar {
	static const int nLayer = 18;
	static const int nCell = 72;
	public:
		EcalDynamicCalibPar(int _time_start, int _time_end, float *_dyngain){
			time_start = _time_start;
			time_end = _time_end;
			for(int lay=0; lay<nLayer; lay++){
				for(int cell=0; cell<nCell; cell++){
					int index = lay*nCell + cell;
					dyngain[lay][cell] = _dyngain[index];
				}
			}
		};
		EcalDynamicCalibPar(){
			time_start = 0;
			time_end = 0;
			SetDefaultValue();
//			memset(dyngain, 0, sizeof(dyngain));
		}
		~EcalDynamicCalibPar(){};
		void Print(){
			std::cout << "========= EcalDynamicCalibPar ============= " << std::endl;
		}
		void Write2RootFile(TString fname, double asktime);
		void SetDefaultValue(){
			for(int lay=0; lay<nLayer; lay++){ for(int cell=0; cell<nCell; cell++){ dyngain[lay][cell] = 1.; } }
		}


		long time_start;
		long time_end;
		float dyngain[nLayer][nCell];
};

class EcalCellStatusParUnit {
	public:
		static const int nLayer = 18;
		static const int nCell = 72;
		int cellstat[nLayer][nCell];
		EcalCellStatusParUnit(){
			SetDefaultValue();
		}
		void Set(int _stat[][72]){
			for(int ilay=0; ilay<nLayer; ilay++){
				for(int icell=0; icell<nCell; icell++){
					cellstat[ilay][icell] = _stat[ilay][icell];
				}
			}
		}
		void Print(){
			for(int ilay=0; ilay<nLayer; ilay++){
				for(int icell=0; icell<nCell; icell++){
					if( cellstat[ilay][icell]>0 ){
						std::cout << "lay " << ilay << ", cell " << icell << ": status " << cellstat[ilay][icell] << std::endl;
					}
//					std::cout << cellstat[ilay][icell];					if( icell<nCell-1 ) std::cout << ", ";					else std::cout << std::endl;
				}
			}
		}
		void SetDefaultValue(){
			memset( cellstat, 0, sizeof( cellstat ) );
		}
		
		void Write2RootFile(TString fname, double asktime);
};

class EcalCellStatusPar { // 
	public:
		static const int nRunsMax = 100;
		long time_start;
		long time_end;
		long array_time_start[nRunsMax];
		EcalCellStatusParUnit cellStatusParUnit[nRunsMax];
		int size;

		EcalCellStatusPar(){
			time_start = 0;
			time_end   = 0;
			size = 0;
			memset( array_time_start, 0, sizeof(array_time_start) );
		}
		int Insert(int _time_start, int _time_end, int _stat[][72]){
			if( size<0 || size>nRunsMax-1 ) return -1;
			if( time_start==0 || time_start>_time_start) time_start = _time_start;
			if( time_end==0 || time_end<_time_end)       time_end   = _time_end;
			array_time_start[size] = _time_start;
			cellStatusParUnit[size].Set( _stat );
			size++;
			return size;
		}
		void Print(){
			std::cout << "==EcalCellStatusPar - " << time_start << " - " << time_end << std::endl;
			for(int i=0; i<size; i++){
				std::cout << "---- unit " << i << std::endl;
			  	cellStatusParUnit[i].Print();
				std::cout << "--------------" << std::endl;
			}
			std::cout << "===========" << std::endl;
		}
		int GetIndex(long time){
			int ret = -1;
			if( time < time_start || time > time_end ) 
				ret = -1;
			else 
				ret = TMath::BinarySearch(size, array_time_start, time);
//			if(ret<0)
//				std::cout << "time=" << time << ", ret=" << ret << ", array_time_start[0]=" << array_time_start[0] << ", array_time_start[nMax]=" << array_time_start[nRunsMax-1] << std::endl;
			return ret;
		}
		void SetDefaultValue(){
			time_start = time_end = 0;
			for(int i=0; i<nRunsMax; i++){
				cellStatusParUnit[i].SetDefaultValue();
			}
		}
};

class EcalKXCalib {
	public:
		static const int nattpar = 3;
		static const int nLayer = 18;
		static const int nCell = 72;
		static  float MIP2MeV ;  // MIP energy in MeV

		static EcalKXCalib* header;
		static EcalKXCalib* getHedaer();
		EcalKXCalib(){ _staticPar_Loaded=0; _staticParMC_Loaded=0; setValidityTimeRangeTB2010(); setValidityTimeRangeISS(); };
		~EcalKXCalib(){};

		int dynCalib_time_interval; // 1 day
		// keep currect parameters
		bool _staticPar_Loaded;
		EcalStaticCalibPar _staticPar;
		EcalDynamicCalibPar _dynamicPar; 
		EcalCellStatusPar _cellStatusPar;
		EcalCellStatusPar _cellStatusPar_TTCSOFF;
		// MC
		bool _staticParMC_Loaded;
		EcalStaticCalibPar _staticParMC;

		// begin and end time of current available database
		// only used when reading dynamic alignment database
		int global_begin_time_TB;
		int global_end_time_TB;
		int global_begin_time_ISS;
		int global_end_time_ISS;
		void setValidityTimeRangeTB2010(){
			global_begin_time_TB = 1281312000; // 2010-08-09
			global_end_time_TB = 1282435200; // 2010-08-21
			std::cout << "ValidityTimeRange: " << global_begin_time_TB << " - " << global_end_time_TB << std::endl;
		}
		void setValidityTimeRangeISS();
		/* // move this function to EcalKXCalib.C
		void setValidityTimeRangeISS(){
			global_begin_time_ISS = 1305849600; // 00:00:00 of 2011-05-20
		//	global_end_time_ISS   = 1464307200; // 00:00:00 of 2016-05-27
		//	global_end_time_ISS   = 1480201200; // 00:00:00 of 2016-11-27
		//	global_end_time_ISS   = 1494633600; // 00:00:00 of 2017-05-13
		//	global_end_time_ISS   = 1510531200; // 00:00:00 of 2017-11-13
		//	global_end_time_ISS   = 1527552000; // 00:00:00 of 2018-05-29
		//	global_end_time_ISS   = 1546473600; // 00:00:00 of 2019-01-03
		//	global_end_time_ISS   = 1580169600; // 00:00:00 of 2020-01-28
		//	global_end_time_ISS   = 1591488000; // 00:00:00 of 2020-06-07
			global_end_time_ISS   = 1620086400; // 00:00:00 of 2021-05-04	
			std::cout << "ValidityTimeRange: " << global_begin_time_ISS << " - " << global_end_time_ISS << std::endl;
		}
		*/
		bool IsTTCSOff(double time);
		bool IsBadCalibrationTime(double time);
		bool isValidDynamicCalib(double ask_time){
			bool goodTB = ask_time > global_begin_time_TB && ask_time < global_end_time_TB;
			bool goodISS = ask_time > global_begin_time_ISS && ask_time < global_end_time_ISS;
			return goodTB || goodISS;
		}

		// interface to deal with TDV database
		template<class T> void fillDB(const char c_type[], T *_db, double time_start, double time_end);
		template<class T> int readDB(const char c_type[], T *_db, double ask_time);
		void fillDB_EcalStaticCalibPar(EcalStaticCalibPar *_db, double time_start, double time_end);
		void fillDB_EcalStaticCalibParMC(EcalStaticCalibPar *_db, double time_start, double time_end);
		void fillDB_EcalDynamicCalibPar(EcalDynamicCalibPar *_db, double time_start, double time_end);
		void fillDB_EcalCellStatusPar(EcalCellStatusPar *_db, double time_start, double time_end);

		static TString RootFileDir,
							AMSDataDir,
							AMSDataDir_Official;
		int LoadAttenuationPar(float *out, TString);
		int LoadIntercalibPar(float *out, TString);
		int LoadHighLowGainRatio(float *outhlr, float *outhdr, TString);
		int WriteStaticCalibPar(); // write static parameters database
		int WriteStaticCalibParMC(); // write static parameters database
		int WriteDynamicCalibPar(TString, long _time_begin=0, long _time_end=0); // write database from _begin to _end
		int WriteCellStatusPar(TString, long _time_begin=0, long _time_end=0); // write database from _begin to _end

		// read TDV database
		int readDB_EcalStaticCalibPar(double ask_time);
		int readDB_EcalStaticCalibParMC(double ask_time);
		int readDB_EcalDynamicCalibPar(double ask_time);
		int readDB_EcalCellStatusPar(double ask_time);

		// accessors to alignment parameters
		EcalStaticCalibPar *getStaticCalibPar(long time);
		EcalStaticCalibPar *getStaticCalibParMC(long time);
		EcalDynamicCalibPar *getDynamicCalibPar(long time);
		EcalCellStatusParUnit *getCellStatusParUnit(long time);
		EcalDynamicCalibPar   *getDefaultDynamicCalibPar(){ return &_dynamicPar; };
		EcalCellStatusParUnit *getDefaultCellStatusParUnit(){ _cellStatusPar.SetDefaultValue(); return &(_cellStatusPar.cellStatusParUnit[0]); }

		//=========> implementation of alignment methods <===============
		static bool IsLocalDirectory;
		TString getEnv( const TString & );
};

#endif
