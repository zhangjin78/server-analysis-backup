//readfile: TChain in the input runlist
//----------
//update from: readfile4.C
//update date: 2018.10.17
//----------
//2018.10.17
//	1) determind the name of the tree we want to add from the name of the TChain
//////////
#include <iostream>
#include <fstream>
#include <string>
#include <dirent.h>
#include <vector>
#include "TString.h"
#include "TChain.h"
#include <cstdlib> //2020.09.01: header for atol
using namespace std;

int readfile(const string ifile, TChain &ch, long &firstfile, long &lastfile, const int dispaly=2, const int span=1) //dispaly=0:not dispaly; =1:dispaly; =2:ask whether dispaly
{
	int dis=0, error=0;
	if (dispaly==2) {
		cout << "dispaly? (1:yes)" << endl;
		cin >> dis;
	}
	else dis = dispaly;
	
	string ifname;
	vector<long> filenameS;
	string per_fname;
	int nper_fname;
//	char rootfile[1000];
//	string rootfile;
	int first=0, last=0;
	if (ifile.find(".root") != string::npos)
	{
		ch.AddFile(ifile.c_str());
		//get root file name
//		first=ifile.find_last_of('/');
//		ifname = ifile.substr(first+1);
		ifname = ifile.substr(ifile.find_last_of('/')+1);
		//get start and end file name
		first = ifname.find_first_of('_');
		last =ifname.find_last_of('_');
//		firstfile = stol(ifname.substr(first, 10));
//		lastfile = stol(ifname.substr(last, 10));
		firstfile = atol((ifname.substr(first+1, 10)).c_str());
		lastfile = atol((ifname.substr(last+1, 10)).c_str());
	}
	else
	{
		ifstream infile;
		infile.open(ifile.c_str());
		if(!infile)
		{
			cerr<<"infile open err!"<<" "<<ifile.c_str()<<endl;
			exit(1);
		}
//----
		int count=0;
		while(infile.good())
		{
//			infile.getline(rootfile,1000,'\n');
//			if(strcmp(rootfile,"")==0)continue; //stccmp:compare two string; if rootfile=="", skip
//			ifname=(string)rootfile;
			getline(infile, ifname);
			if (ifname=="") continue;
			
			first=ifname.find_last_of('/', ifname.size());
			last =ifname.find_last_of('_', ifname.size());
			if ((ifname.find("/zhen/") != std::string::npos || ifname.find("/TrdDst") != std::string::npos))
			{
				first=ifname.find_last_of('_', ifname.size());
				last =ifname.find_last_of('.', ifname.size());
			}
			per_fname = ifname.substr(first+1, last-first-1);
			nper_fname = atoi(per_fname.c_str());
			filenameS.push_back(atoi(per_fname.c_str()));
			
			size_t fond=ifname.find(".root");
			if(fond==string::npos)
			{ //skip if no ".root", i.e. not a root file
				error++;
				cout << "has no root file:" << ifname << endl;
				continue; 
			}
//			fond=ifname.find("castor");
//			if(fond!=string::npos){
//				if(castype==0)ifname="root://castorpublic.cern.ch//"+ifname;
//				else          ifname="rfio:"+ifname;
//			}
			fond=ifname.find("eos");
			if(fond!=string::npos)
			{
				//ifname="root://eosams/"+ifname; //2020.06.02: not working adding these prefix
			}
			
			ifname+="/";
			ifname+=ch.GetName();
			ch.AddFile(ifname.c_str());
			
			if (dis == 1) cout<<"count="<<count<<" "<<ifname<<endl;
			count++;
		}
		infile.close();
		firstfile = filenameS.front();
		lastfile = filenameS.back();
	}
	cout << "first file:" << firstfile << endl;
	cout << "last file:" << lastfile << endl;
	return 0;

}
