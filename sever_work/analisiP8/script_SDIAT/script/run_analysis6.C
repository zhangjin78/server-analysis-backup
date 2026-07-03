//----------
//update from: run_analysis5.C
//update date: 2019.06.27
//----------
//2019.10.13
//	1) update the directory to be run in Baobab: "/atlas/users/chenya" to "/dpnc/beegfs/users/chenya/atlas_user"
//
//2020.03.09
//	1) add new parameter, int iGeoSafety, to set the safety factor to use for geomagnetic cutoff
//
//2020.06.02
//	1) update the directory to lxplus
//
//2020.09.01
//	1) add flag for AMS software and corrected rigidity
//
//2020.12.22
//	1) read the MC mass from the runlist directory
//----------
//////////

//2020.06.05
//#define ATLAS
//#define BAOBAB
#define LXPLUS
#define MLXPLUS

//-- //2020.09.01
//#define GBATCH
#define CORRIGGBATCH

//--
#include "readfile5.C"

#include "analysis6.C"

int run_analysis6(
			//string inf=sWDirOnline+"/runlist/ISS/B1130/amsd68n_20/1",
			string inf="/afs/cern.ch/work/j/jzhang2/AMS/runlist/Data/amsd69nMIT_13y5_100/518",
			// string inf="/afs/cern.ch/work/j/jzhang2/AMS/runlist/MC/B1130/amsd68nMCHe4l1_100/80",
			// string inf="/afs/cern.ch/work/j/jzhang2/AMS/runlist/MC/B1130/amsd68nMCFe56l1_100/10",
			// string inf="/afs/cern.ch/work/j/jzhang2/AMS/runlist/MC/B1130/amsd68nMCSi28l1_100/0",
			// string inf="/afs/cern.ch/work/j/jzhang2/AMS/runlist/MC/B1130/amsd68nMCNe20l19_100/10",
			// string inf="/afs/cern.ch/work/j/jzhang2/AMS/runlist/MC/B1130/amsd68nMCNe20l1_100/0",
			// string inf="/afs/cern.ch/work/j/jzhang2/AMS/runlist/MC/B1130/amsd68nMCNe20l1_ECalCheck_100/0",
			// string inf="/afs/cern.ch/work/j/jzhang2/AMS/runlist/Data/amsd69nChenYa_NewTrig_ECalCheck/0",
			string outdir="/afs/cern.ch/work/j/jzhang2/AMS/analisiP8/testoutput",
			// string outdir="/eos/ams/user/j/jzhang2/AMS/result/testout",
			int nevent=1000,
			// int nevent=-1,
			const int iGeoSafety=1 //2020.03.09
			)
{
	//--
	time_t start0 = time(NULL);
	cout << " ****** starting time: " << ctime(&start0) << endl;
	
	//--
	long int f_file=0, l_file=0;
	string outfilename;
	
	TChain filechain;
	TChain filechain_zhen[2];
	filechain.SetName("amstreea");
		
	readfile(inf.c_str(), filechain, f_file, l_file, 0);

	int last = inf.find_last_of('/');
	string number = inf.substr(last+1);
	int nentries = filechain.GetEntries();
	cout << "runlist number: " << number << endl;
	cout << "entries in whole chain: " << nentries << endl;
	
	if (nentries<=0)
	{
		cout << "Error in reading files, nentries=" << nentries << endl;
		cout << "!!! Stop running" << endl;
		return 0;
	}
	
	outfilename = Form("%s_%ld_%ld", number.c_str(), f_file, l_file);
	if (nevent!=-1) outfilename = Form("%s_n%d", outfilename.c_str(), nevent);
	cout << "outfile name:" << outfilename.c_str() << endl;
	
	//2020.12.22: read the dst version from runlist directory
	histn = inf.substr(inf.find("amsd")+4,inf.find_last_of('_')-(inf.find("amsd")+4));
	histn = histn.substr(0, histn.find_first_of('n'));
	//dstver = stoi(histn);
	//2021.01.19: change to non C++11 function
	std::istringstream  is(histn);
	is >> dstver;
	//2020.12.22: read the MC mass from the runlist directory
	histn = inf.substr(inf.find("nMC")+3,inf.find_last_of('l')-(inf.find("nMC")+3));
	
	//analysis(filechain, outdir, outfilename, nevent);
	int status = analysis(filechain, outdir, outfilename, nevent, iGeoSafety); //2020.03.09
	cout << "Running status: " << status << endl;
	return status;
}////
