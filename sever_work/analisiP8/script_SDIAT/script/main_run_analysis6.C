//----------
//update from: run_analysis5.C
//update date: 2019.06.27
//----------
//----------
//////////

//#define ATLAS
//#define BAOBAB
//#define LXPLUS
#define SDIAT

//--
#define GBATCH
#define CORRIGGBATCH

//--
#include "readfile5.C"

#include "analysis6.C"

// #include "codebak/test_splinefit.C"

int run_analysis6(
////MC
			string inf="/home/ams/jzhang/AMS/Flux/runlist/MC/amsd68nMC_Ar36_l1/1",
////ISS
			// string inf="/home/ams/jzhang/AMS_analysis/FluxMeasurement/script/TestList/0",
			// string inf="/home/ams/jzhang/AMS/Flux/runlist/Data/amsd69n_13y5_20/1172",
			// #if defined CHARGECAL_NEWTRQ
			// string outdir=sWDirOnline+"/result/test/analysis6/ChargeCal",
			// #elif defined TRRES
			// string outdir=sWDirOnline+"/result/test/analysis6/TrRes",
			// #elif defined TRACKEFF || defined TRACKEFFN || defined TRIGEFF || defined TOFQEFF || defined L1EFF || defined L9EFF
			// //string outdir=sWDirOnline+"/result/test/analysis6/Efficiency",
			// string outdir="/afs/cern.ch/work/z/zhaominw/public/iron/result/analysis6/runInLocal_amsd64nMCFe56l19_B1130_EffAndAcce_MC_NEWTRIG/l1999999999",//2020-11-09 now run the MC in local -_-! -_-!!
			// #elif defined ESTCAL
			// string outdir=sWDirOnline+"/result/test/analysis6/RigEstiCal",
			// #elif defined EFFVAL
			// string outdir=sWDirOnline+"/result/test/analysis6/EffVal",
			// #elif defined TRDINTERCON
			// string outdir=sWDirOnline+"/result/test/analysis6/TrdInterCon",
			// #elif defined RIGRESO
			// string outdir=sWDirOnline+"/result/test/analysis6/RigReso",
			// #elif defined ACCEPTANCE
			// string outdir=sWDirOnline+"/result/test/analysis6/Acceptance",
			// #else
			// string outdir=sWDirOnline+"/result/test/analysis6",
			// #endif
			string outdir="/lustre02/user/jzhang/result/FluxMeasurement/test",
			int nevent=1000,
			// int nevent=-1,
			const int iGeoSafety=1
			)
{
	long int f_file=0, l_file=0;
	string outfilename;

	// cout << "Testing: ...." << endl;
	// test_splinefit();
	// return 0;
	
	TChain filechain;
	TChain filechain_zhen[2];
	//filechain.SetName("FeO/FeOTree");
	filechain.SetName("amstreea");
		
	readfile(inf.c_str(), filechain, f_file, l_file, 0);

	int last = inf.find_last_of('/');
	string number = inf.substr(last+1);
	cout << "runlist number: " << number << endl;
	cout << "entries in whole chain: " << filechain.GetEntries() << endl;

	//2020.12.22: read the dst version from runlist directory
	histn = inf.substr(inf.find("amsd")+4,inf.find_last_of('_')-(inf.find("amsd")+4));
	histn = histn.substr(0, histn.find_first_of('n'));
	//dstver = stoi(histn);
	//2021.01.19: change to non C++11 function
	std::istringstream  is(histn);
	is >> dstver;
	//2020.12.22: read the MC mass from the runlist directory
	histn = inf.substr(inf.find("nMC_") + 4, inf.find('_', inf.find("nMC_") + 4) - (inf.find("nMC_") + 4));
	
	outfilename = Form("%s_%ld_%ld", number.c_str(), f_file, l_file);
	//outfilename = Form("%s", number.c_str());
	if (nevent!=-1) outfilename = Form("%s_n%d", outfilename.c_str(), nevent);
	cout << "outfile name:" << outfilename.c_str() << endl;
	
	analysis(filechain, outdir, outfilename, nevent, iGeoSafety);
       // cout<<"test1~~~"<<endl;
	return 0;
}////

int main(int argc, char *argv[]){

  if (argc == 1) run_analysis6();
  else if (argc ==2) run_analysis6( argv[1] ); 
  else if (argc ==3) run_analysis6( argv[1], argv[2] ); 
  else if (argc ==4) run_analysis6( argv[1], argv[2] , atoi( argv[3] )); 
  else if (argc ==5) run_analysis6( argv[1], argv[2] , atoi( argv[3] ), atoi( argv[4] )); 
  else {
    cout<< "arg Nb Wrong!!"<<endl;
    return 0;
  }
  cout<<"Mission completed!!"<<endl;

  return 1;
}
