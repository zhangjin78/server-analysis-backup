#include "EcalKXCalib.h"
#include "root.h"
#ifndef __ROOTSHAREDLIBRARY__
#include "commons.h"
#endif

#include "timeid.h"
#include "amsdbc.h"
#include "commonsi.h"

TString EcalKXCalib::RootFileDir = "";
TString EcalKXCalib::AMSDataDir = "/afs/cern.ch/ams/Offline/AMSDataDir/";
TString EcalKXCalib::AMSDataDir_Official = "/cvmfs/ams.cern.ch/Offline/AMSDataDir/";
bool EcalKXCalib::IsLocalDirectory = 0;
EcalKXCalib* EcalKXCalib::header=0;
float  EcalKXCalib::MIP2MeV =9.0;  // MIP energy in MeV

EcalKXCalib* EcalKXCalib::getHedaer(){
	if( header==0 ){
		header = new EcalKXCalib();
	}
	return header;
}

TString EcalKXCalib::getEnv( const TString & var ) {
	const char * val = getenv( var.Data() );
	if ( val == 0 ) {
		return "";
	}
	else {
		return TString(val);
	}
}

template<class T> void EcalKXCalib::fillDB(const char c_type[], T *_db, double time_start, double time_end){
	if( IsLocalDirectory ){
		setenv( "AMSDataDirRW", AMSDataDir.Data(), 1 );
		setenv( "AMSDataDir", AMSDataDir.Data(), 1 );
	}else{
		AMSDataDir = getEnv("AMSDataDir");
	}
	time_t starttime = time_t( time_start );
	time_t endtime   = time_t( time_end   );
	tm begin, end;
	localtime_r( &starttime, &begin );
	localtime_r( &endtime, &end );
	AMSTimeID *tid = new AMSTimeID( 
			AMSID( c_type, 1 ), 
			begin, end,
			sizeof(*_db),
			_db,
			AMSTimeID::Standalone,
			1 );
	tid->UpdateMe();
	tid->write( AMSDataDir + TString("/DataBase/") );
	delete tid;
}		  

template<class T> int EcalKXCalib::readDB(const char c_type[], T *_db, double asktime ){
	time_t time = (time_t) asktime;
	// this is due to special algorithm in timeid.C, AMSTimeID::_getDBRecord()
	time_t starttime = time+1; 
	time_t endtime = time;
	//    cout << "==> EcalKXCalib::readDB() " << " time=" << time << ", " << starttime<< ", " << endtime << endl;
	tm begin;
	tm end;
	localtime_r(&starttime,&begin);
	localtime_r(&endtime,&end);
	if(IsLocalDirectory) strcpy( AMSDBc::amsdatabase, (EcalKXCalib::AMSDataDir + "/DataBase/").Data() ); // local directory for EcalKXCalib

	AMSTimeID* tid= new AMSTimeID(
			AMSID( c_type, 1 ),
			begin, end,
			sizeof(*_db),
			_db,
			AMSTimeID::Standalone,
			1 );
	int read = tid->validate( time );
	if(IsLocalDirectory) strcpy( AMSDBc::amsdatabase, (EcalKXCalib::AMSDataDir_Official + "/DataBase/") ); // change back to offical for others
	delete tid;
	return read;
}

void EcalKXCalib::fillDB_EcalStaticCalibPar(EcalStaticCalibPar *_db, double time_start, double time_end){
	fillDB( "EcalStaticCalibPar", _db, time_start, time_end );
}

void EcalKXCalib::fillDB_EcalStaticCalibParMC(EcalStaticCalibPar *_db, double time_start, double time_end){
	fillDB( "EcalStaticCalibParMC", _db, time_start, time_end );
}

void EcalKXCalib::fillDB_EcalDynamicCalibPar(EcalDynamicCalibPar *_db, double time_start, double time_end){
	fillDB( "EcalDynamicCalibPar", _db, time_start, time_end );
}

void EcalKXCalib::fillDB_EcalCellStatusPar(EcalCellStatusPar *_db, double time_start, double time_end){
	fillDB( "EcalCellStatusPar", _db, time_start, time_end );
}

int EcalKXCalib::readDB_EcalStaticCalibPar(double asktime){
	//	cout << "==> int readDB_EcalStaticCalibPar(): " << endl;
	//	cout << "before reading database: start_time=" << _staticPar.time_start << " end_time=" << _staticPar.time_end << endl;
	if( asktime > _staticPar.time_end || asktime < _staticPar.time_start ){
		// create a new instance
		_staticPar = EcalStaticCalibPar();
		readDB( "EcalStaticCalibPar", &_staticPar, asktime);
		if( _staticPar.time_end == 0 ) return 0; // empty
	}
	return 1; // good
}
int EcalKXCalib::readDB_EcalStaticCalibParMC(double asktime){
	if( asktime > _staticParMC.time_end || asktime < _staticParMC.time_start ){
		// create a new instance
		_staticParMC = EcalStaticCalibPar();
		readDB( "EcalStaticCalibParMC", &_staticParMC, asktime);
		if( _staticParMC.time_end == 0 ) return 0; // empty
	}
	return 1; // good
}

int EcalKXCalib::readDB_EcalDynamicCalibPar(double asktime){
	if( !isValidDynamicCalib(asktime) ){
		cout << "== EcalKXCalib::readDB_EcalDynamicCalibPar(" << asktime << "): Exceed validity time" << endl;
		return 0;
	}
	if( asktime > _dynamicPar.time_end || asktime < _dynamicPar.time_start ){
		_dynamicPar = EcalDynamicCalibPar(); 
		readDB( "EcalDynamicCalibPar", &_dynamicPar, asktime);
		if( _dynamicPar.time_end==0 ) return 0; // empty
	}
	return 1; // good
}

int EcalKXCalib::readDB_EcalCellStatusPar(double asktime){
	if( !isValidDynamicCalib(asktime) ){
		cout << "== EcalKXCalib::readDB_EcalCellStatusPar(" << asktime << "): Exceed validity time" << endl;
		return 0;
	}
	if( IsTTCSOff(asktime) ){
		if( asktime > _cellStatusPar_TTCSOFF.time_end || asktime < _cellStatusPar_TTCSOFF.time_start ){
			_cellStatusPar_TTCSOFF = EcalCellStatusPar();
			readDB( "EcalCellStatusPar", &_cellStatusPar_TTCSOFF, asktime);
			if( _cellStatusPar_TTCSOFF.time_end==0 ) return 0; // empty
		}
	}else{
		if( asktime > _cellStatusPar.time_end || asktime < _cellStatusPar.time_start ){
			_cellStatusPar = EcalCellStatusPar();
			readDB( "EcalCellStatusPar", &_cellStatusPar, asktime);
			if( _cellStatusPar.time_end==0 ) return 0; // empty
		}
	}
	return 1; // good
}

bool EcalKXCalib::IsTTCSOff(double time){
	if( time>=1411995797 && time<=1417187198 ) // 2014-09-29 to 2014-11-28
		return true;
	else if( time>=1532187857 && time<1533125144 )  // 2018-07-21 to 2018-08-01
		return true;
	else if( time>=1533908821 && time<1535703137 )  // 2018-08-10 to 2018-08-31
		return true;
	else if( time>=1538915261 && time<1541341495 )  // 2018-10-07 to 2018-11-04
		return true;
	else
		return false;
}

bool EcalKXCalib::IsBadCalibrationTime(double time){
	static const int nBadTime=2;
	static double BadTimeRange[nBadTime][2] = {
		{ 1378857600, 1378943999 }, // 2013-09-11, DOE review
		{ 1382400000, 1382572799 } // 2013-10-22 to 2013-10-24, TOF re-calibration
	};
	for(int i=0; i<nBadTime; i++){
		if( time >= BadTimeRange[i][0] && time <= BadTimeRange[i][1] ) return true;
	}
	return false;
}

void EcalKXCalib::setValidityTimeRangeISS(){
	global_begin_time_ISS = 1305849600; // 00:00:00 of 2011-05-20
	// global_end_time_ISS   = 1464307200; // 00:00:00 of 2016-05-27
	// global_end_time_ISS   = 1480201200; // 00:00:00 of 2016-11-27
	// global_end_time_ISS   = 1494633600; // 00:00:00 of 2017-05-13
	// global_end_time_ISS   = 1510531200; // 00:00:00 of 2017-11-13
	// global_end_time_ISS   = 1527552000; // 00:00:00 of 2018-05-29
	// global_end_time_ISS   = 1546473600; // 00:00:00 of 2019-01-03
	// global_end_time_ISS   = 1580169600; // 00:00:00 of 2020-01-28
	// global_end_time_ISS   = 1591488000; // 00:00:00 of 2020-06-07
	//global_end_time_ISS   = 1620086400; // 00:00:00 of 2021-05-04
	//global_end_time_ISS   = 1635897600; // 00:00:00 of 2021-11-03
	//global_end_time_ISS   = 1668211200; // 00:00:00 of 2022-11-12
	//global_end_time_ISS   = 1699660800; // 00:00:00 of 2023-11-11
	global_end_time_ISS   = 1731801600; // 00:00:00 of 2023-11-17
	std::cout << "ValidityTimeRange: " << global_begin_time_ISS << " - " << global_end_time_ISS << std::endl;
}

EcalStaticCalibPar *EcalKXCalib::getStaticCalibPar(long time){
	readDB_EcalStaticCalibPar( double(time) );
	return &_staticPar;
}

EcalStaticCalibPar *EcalKXCalib::getStaticCalibParMC(long time){
	readDB_EcalStaticCalibParMC( double(time) );
	return &_staticParMC;
}


EcalDynamicCalibPar *EcalKXCalib::getDynamicCalibPar(long time){
	if( //IsTTCSOff(time) || 
			IsBadCalibrationTime(time) || !readDB_EcalDynamicCalibPar( double(time) ) ){
		cout << "--EcalKXCalib--Default value for EcalDynamicCalibPar will be used" << endl;
		_dynamicPar.SetDefaultValue();
	}
	return &_dynamicPar;
}

EcalCellStatusParUnit *EcalKXCalib::getCellStatusParUnit(long time){
	if( //IsTTCSOff(time) ||
			IsBadCalibrationTime(time) || !readDB_EcalCellStatusPar( double(time) ) ){
		cout << "--EcalKXCalib--Default value for EcalCellStatusPar will be used" << endl;
		if( IsTTCSOff(time) )
			_cellStatusPar_TTCSOFF.SetDefaultValue();
		else
			_cellStatusPar.SetDefaultValue();
	}
	if( IsTTCSOff(time) ){
		int index = _cellStatusPar_TTCSOFF.GetIndex(time);
		static int nwarning=0;
		if( index<0 || index>EcalCellStatusPar::nRunsMax-1 ){
			if( nwarning++<20 ){
				cout << "found index " << index << ", Bad Index, Default value will be used " << nwarning << ". maximum 20 warning will be shown " << endl;
			}
			_cellStatusPar_TTCSOFF.SetDefaultValue();
			index = 0;
		}
		return &(_cellStatusPar_TTCSOFF.cellStatusParUnit[index]);
	}else{
		int index = _cellStatusPar.GetIndex(time);
		static int nwarning=0;
		if( index<0 || index>EcalCellStatusPar::nRunsMax-1 ){
			if( nwarning++<20 ){
				cout << "found index " << index << ", Bad Index, Default value will be used " << nwarning << ". maximum 20 warning will be shown " << endl;
			}
			_cellStatusPar.SetDefaultValue();
			index = 0;
		}
		return &(_cellStatusPar.cellStatusParUnit[index]);
	}
}

int EcalKXCalib::LoadAttenuationPar(float *out, TString fin){
	TChain *ch = new TChain("tattpar");
	ch->Add(fin);
	Int_t _tlay, _tcell;
	Float_t _tattpar[4];
	ch->SetBranchAddress( "lay", &_tlay );
	ch->SetBranchAddress( "cell", &_tcell );
	ch->SetBranchAddress( "attpar", _tattpar );

	for(int _entry=0; _entry<ch->GetEntries(); _entry++){
		ch->GetEntry( _entry );
		for(int _ii=0; _ii<nattpar; _ii++){
			out[(_tlay*72 + _tcell)*nattpar + _ii] = _tattpar[_ii];
		}
	}
	if( ch ) delete ch;
	return 0;
}

int EcalKXCalib::LoadIntercalibPar(float *out, TString fin){
	TChain *ch = new TChain("tavgpar");
	ch->Add(fin);
	Int_t _tlay, _tcell;
	Float_t _gain;
	ch->SetBranchAddress( "lay", &_tlay );
	ch->SetBranchAddress( "cell", &_tcell );
	ch->SetBranchAddress( "gain", &_gain );

	for(int _entry=0; _entry<ch->GetEntries(); _entry++){
		ch->GetEntry( _entry );
		out[_tlay*72 + _tcell] = _gain;
	}
	if( ch ) delete ch;
	return 0;
}

int EcalKXCalib::LoadHighLowGainRatio(float *out_hlr, float *out_hdr, TString fin){
	TChain *ch = new TChain("gr");
	ch->Add(fin);
	Int_t _tlay, _tcell;
	Float_t _hlr, _hdr;
	ch->SetBranchAddress( "lay", &_tlay );
	ch->SetBranchAddress( "cell", &_tcell );
	ch->SetBranchAddress( "hlr", &_hlr );
	ch->SetBranchAddress( "hdr", &_hdr );

	for(int _entry=0; _entry<ch->GetEntries(); _entry++){
		ch->GetEntry( _entry );
		out_hlr[_tlay*72 + _tcell] = _hlr;
		out_hdr[_tlay*72 + _tcell] = _hdr;
	}
	if( ch ) delete ch;
	return 0;
}

//
int EcalKXCalib::WriteStaticCalibPar(){
	long time_start = 1262304000; // 2010-01-01
  	long time_end = 1861916400;
	//	cout << "time stamp: "  << time_start << " " << time_end << endl;;
	const int nCells = 18*72;
	float _attpar[nCells*nattpar],
			_gain[nCells],
			_hlr[nCells],
			_hdr[nCells];
	memset( _attpar, 0, sizeof( _attpar ) );
	memset( _gain, 0, sizeof( _gain ) );
	memset( _hlr, 0, sizeof( _hlr ) );
	memset( _hdr, 0, sizeof( _hdr ) );
	LoadAttenuationPar( _attpar, RootFileDir + "attpar_z2_20GV_langaus.root" );
	LoadIntercalibPar( _gain, RootFileDir + "avgpar_He_80GV_langaus_attcor3z2langaus.root" );
	LoadHighLowGainRatio( _hlr, _hdr, RootFileDir + "gainRatio_ISS.root" );

	// build instance
	EcalStaticCalibPar _db(time_start, time_end, _attpar, _gain,  _hlr, _hdr);	
//	_db.Print();
	fillDB_EcalStaticCalibPar( &_db, double(time_start), double(time_end) );
	return 1; // success
}

int EcalKXCalib::WriteStaticCalibParMC(){ // use version as the id, instead of time
	long time_start = 1000; // B1042
  	long time_end = 1999;
	//	cout << "time stamp: "  << time_start << " " << time_end << endl;;
	const int nCells = 18*72;
	float _attpar[nCells*nattpar],
			_gain[nCells],
			_hlr[nCells],
			_hdr[nCells];
	memset( _attpar, 0, sizeof( _attpar ) );
	memset( _gain, 0, sizeof( _gain ) );
	memset( _hlr, 0, sizeof( _hlr ) );
	memset( _hdr, 0, sizeof( _hdr ) );
	LoadAttenuationPar( _attpar, RootFileDir + "mc_attpar_z1_40GV_langaus.root" );
	LoadIntercalibPar( _gain, RootFileDir + "mc_avgpar_z1_80GV_langaus.root" );
	LoadHighLowGainRatio( _hlr, _hdr, RootFileDir + "mc_gainRatio.root" );
	// build instance
	EcalStaticCalibPar _db(time_start, time_end, _attpar, _gain,  _hlr, _hdr);	
//	_db.Print();
	fillDB_EcalStaticCalibParMC( &_db, double(time_start), double(time_end) );
	return 1; // success
}

int EcalKXCalib::WriteDynamicCalibPar(TString fin, long _time_begin, long _time_end){
	// load TGraph from fname, get discrete shift value by interpolation
	TChain *ch = new TChain("tk");
	ch->Add( RootFileDir + fin );
	long t_begin, t_end;
	float hgain[18*72];
	ch->SetBranchAddress( "t_begin", &t_begin );
	ch->SetBranchAddress( "t_end", &t_end );
	ch->SetBranchAddress( "hgaink", &hgain );
	
	for(int entry=0; entry<ch->GetEntries(); entry++)
	{
		ch->GetEntry(entry);
		if( _time_begin==0 || (t_begin >= _time_begin && t_begin <_time_end) ){
			EcalDynamicCalibPar _db(t_begin, t_end, hgain);
			fillDB_EcalDynamicCalibPar( &_db, double(t_begin), double(t_end) );
		}
	}

	if(ch) delete ch;
	return 1; // sucess
}

int EcalKXCalib::WriteCellStatusPar(TString fin, long _time_begin, long _time_end){
	// load TGraph from fname, get discrete shift value by interpolation
	TChain *ch = new TChain("tstat");
	ch->Add( RootFileDir + fin );
	long t_begin, t_end;
	int  _cellstat[18][72];
	ch->SetBranchAddress( "t_begin", &t_begin );
	ch->SetBranchAddress( "t_end",   &t_end );
	ch->SetBranchAddress( "status",  &_cellstat );
	int nruns=0;
	EcalCellStatusPar *_db=0;
	for(int entry=0; entry<ch->GetEntries(); entry++)
	{
		ch->GetEntry(entry);
		if( _time_begin==0 || (t_begin >= _time_begin && t_begin <_time_end) ){
			if( nruns==0 || _db==0 ){
				_db = new EcalCellStatusPar();
			}
			_db->Insert(t_begin, t_end, _cellstat);
			nruns++;
			if( nruns==EcalCellStatusPar::nRunsMax || entry==ch->GetEntries()-1 ){
//				_db->Print();
				fillDB_EcalCellStatusPar( _db, _db->time_start, _db->time_end );
				delete _db;
				_db = 0;
				nruns = 0;
			}
		}
	}

	if(ch) delete ch;
	return 1; // sucess
}

void EcalStaticCalibPar::Write2RootFile(TString fname){
	TFile *fout = TFile::Open( fname, "recreate" );
	TTree *tree = new TTree("t", "static parameters from TDV");
	float tmpattpar[3], tmphlr, tmphdr, tmpgain;
	int lay, cell;
	tree->Branch( "time_start", &time_start, "time_start/L" );
	tree->Branch( "time_end", &time_end, "time_end/L" );
	tree->Branch( "lay", &lay, "lay/I" );
	tree->Branch( "cell", &cell, "cell/I" );
	tree->Branch( "attpar", tmpattpar, "attpar[3]/F" );
	tree->Branch( "gain", &tmpgain, "gain/F" );
	tree->Branch( "hlr", &tmphlr, "hlr/F" );
	tree->Branch( "hdr", &tmphdr, "hdr/F" );
	for(lay=0; lay<18; lay++){
		for(cell=0; cell<72; cell++){
			for(int ipar=0; ipar<3; ipar++) tmpattpar[ipar] = attpar[lay][cell][ipar];
			tmphlr = hlr[lay][cell];
			tmphdr = hdr[lay][cell];
			tmpgain = gain[lay][cell];
			tree->Fill();
		}
	}
	fout->cd();
	tree->Write();
	fout->Close();
}

void EcalDynamicCalibPar::Write2RootFile(TString fname, double asktime){
	TFile *fout = TFile::Open( fname, "update" );
	TTree *tree;
	double t_start, t_end;
	float tmpgain;
	int lay, cell, amsday;
	static int amsday0 = 1305849600;
	tree = dynamic_cast<TTree*>( fout->Get("t") );
	if( tree==NULL ){
		tree = new TTree("t", "static parameters from TDV");
		tree->Branch( "t_start", &t_start, "t_start/D" );
		tree->Branch( "t_end", &t_end, "t_end/D" );
		tree->Branch( "asktime", &asktime, "asktime/D" );
		tree->Branch( "amsday", &amsday, "amsday/I" );
		tree->Branch( "lay", &lay, "lay/I" );
		tree->Branch( "cell", &cell, "cell/I" );
		tree->Branch( "gain", &tmpgain, "gain/F" );
	}else{
		tree->SetBranchAddress( "t_start", &t_start );
		tree->SetBranchAddress( "asktime", &asktime );
		tree->SetBranchAddress( "t_end", &t_end );
		tree->SetBranchAddress( "amsday", &amsday );
		tree->SetBranchAddress( "lay", &lay );
		tree->SetBranchAddress( "cell", &cell );
		tree->SetBranchAddress( "gain", &tmpgain );
	}
	t_start = time_start;
	t_end   = time_end;
	for(lay=0; lay<18; lay++){
		for(cell=0; cell<72; cell++){
			tmpgain = dyngain[lay][cell];
			amsday = int( asktime - amsday0 )/86400;
			tree->Fill();
		}
	}
	fout->cd();
	tree->Write("", TObject::kOverwrite);
	fout->Close();
}

void EcalCellStatusParUnit::Write2RootFile(TString fname, double asktime){
	TFile *fout = TFile::Open( fname, "update" );
	TTree *tree;
//	double t_start, t_end;
	UInt_t tmpstat;
	int lay, cell;
	tree = dynamic_cast<TTree*>( fout->Get("t") );
	if( tree==NULL ){
		tree = new TTree("t", "static parameters from TDV");
//		tree->Branch( "t_start", &t_start, "t_start/D" );
//		tree->Branch( "t_end", &t_end, "t_end/D" );
		tree->Branch( "asktime", &asktime, "asktime/D" );
		tree->Branch( "lay", &lay, "lay/I" );
		tree->Branch( "cell", &cell, "cell/I" );
		tree->Branch( "stat", &tmpstat, "stat/i" );
	}else{
//		tree->SetBranchAddress( "t_start", &t_start );
//		tree->SetBranchAddress( "t_end", &t_end );
		tree->SetBranchAddress( "asktime", &asktime );
		tree->SetBranchAddress( "lay", &lay );
		tree->SetBranchAddress( "cell", &cell );
		tree->SetBranchAddress( "stat", &tmpstat );
	}
//	t_start = time_start;
//	t_end   = time_end;
	for(lay=0; lay<18; lay++){
		for(cell=0; cell<72; cell++){
			tmpstat = cellstat[lay][cell];
			tree->Fill();
		}
	}
	fout->cd();
	tree->Write("", TObject::kOverwrite);
	fout->Close();
}
