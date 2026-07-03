#ifndef RIGCALIB_H
#define RIGCALIB_H
////////////////////////
//-----------------
//updated from: ReadRigCalib.h
//updated date: 2023.03.24
//-----------------
//updated method on getting rigidity estimation
//1) use TObjArray to deal with different estimation function, read all files in the directory
//2) enable to store only given list of charge according to vector
//3) enable to deal with different rigidity algorithm
//-----------------
//-----------------

#include "drawvariable.h"
#include "dirent.h" //read the files in the directory
#include "parameter.h"

////////////////////////
/*const int nQ=30;
TF1* f1Cutoff[nQ][6][4]; //[charge][QVer][version]
TF1* f1Edep[nQ][6][4]; //[charge][QVer][version]*/

TObjArray *oaRigEstCF = NULL;
TObjArray *oaRigEstECal = NULL;

void BuildRigEst(const string sCalibDir, vector<int> viQ)
{
	oaRigEstCF = new TObjArray();
	oaRigEstCF->SetOwner(kTRUE);
	oaRigEstECal = new TObjArray();
	oaRigEstECal->SetOwner(kTRUE);
	
	cout << "***Reading Rigidity estimation calibration from " << sCalibDir << "***" << endl;
	ifstream infile;
	string sLine;
	
	//get the list of files in the calibration directory //is it possible in c++?
	DIR *dir = opendir(sCalibDir.c_str());
	if (dir == NULL)
	{
		cout << " !!! BuildRigEst: directory not accessable: " << sCalibDir << endl;
		return;
	}
	struct dirent *entry;
	while ((entry = readdir(dir)) != NULL)
	{
		string sFile = entry->d_name;
		if (sFile=="." || sFile=="..") continue;
		if (sFile.find_first_of("_")==string::npos || sFile.find_first_of("q")==string::npos)
		{
			//cout << "not calibration file: " << sFile << endl;
			continue;
		}
		
		int iRigM=0;
		if (sFile.find("cutoff")!=string::npos) iRigM=1;
		else if (sFile.find("Edep")!=string::npos) iRigM=2;
		else
		{
			continue;
		}
		string sRigM = (iRigM==1)?"cutoff":"Edep";
		double dRigEstU = (iRigM==1)?32:850;
		
		string sQ = sFile.substr(1, sFile.find_first_of("_")-1);
		int charge = stoi(sQ);
		if (viQ.size()>0 && find(viQ.begin(), viQ.end(), charge) == viQ.end())
		{
			continue;
		}
		/*else
		{
			cout << "Charge=" << charge << " in the list: " << sFile << endl;
		}*/
		
		//--read calibration file
		infile.open(Form("%s/%s", sCalibDir.c_str(), sFile.c_str()));
		int iEstVer=0;
		while(infile.good())
		{
			getline(infile,sLine,'\n');
			if (sLine=="") continue;
			if (sLine.find(sRigM.c_str())!=string::npos) //read the version
			{
				string sEstVer = sLine.substr(sRigM.size(), sLine.size());
				iEstVer = stoi(sEstVer);
			}
			else
			{
				histn = Form("%s_EstV%d", sFile.c_str(), iEstVer);
				if (iRigM==1) oaRigEstCF->Add(new TF1(histn.c_str(), sLine.c_str(), 0, dRigEstU));
				else if (iRigM==2) oaRigEstECal->Add(new TF1(histn.c_str(), sLine.c_str(), 0, dRigEstU));
			}
		}
		if (iEstVer>0) cout << "***Finish reading calibration for q" << charge << ": " << sFile << endl;
		infile.close();
	}
	
	closedir(dir);
}

void BuildRigEst(const string sCalibDir)
{
	vector<int> viQ; //empty vector
	BuildRigEst(sCalibDir, viQ);
}

double getcutoffrig(const double cutoff, const int tkz, const int QVer, const int irig, const int version)
{
	double cutoffrig=cutoff;
	//if (tkz<=0 || tkz>nQ) return cutoffrig;
	if (tkz<=0) return cutoffrig; //2023.03.26
	//if (f1Cutoff[tkz-1][QVer][version]==NULL) return cutoffrig;
	//double dCalVal=f1Cutoff[tkz-1][QVer][version]->Eval(cutoff);
	//2023.03.24
	if (oaRigEstCF==NULL)
	{
		cout << "Cutoff Rig estimation not read" << endl;
		return cutoffrig;
	}
	histn = Form("q%d_v%d_cutoff%s_EstV%d", tkz, QVer, sRigName[irig].c_str(), version);
	TF1 *f1RigEst = static_cast<TF1*>(oaRigEstCF->FindObject(histn.c_str()));
	if (f1RigEst==NULL)
	{
		// cout << "Cutoff Rig estimation not found for Z=" << tkz << " QVer=" << QVer << " irig=" << irig << " ver=" << version << endl;
		return cutoffrig;
	}
	double dCalVal=f1RigEst->Eval(cutoff);
	
	if (version<2)
	{
		if (dCalVal<=0) cutoffrig=cutoff;
		else cutoffrig = dCalVal;
	}
	else
	{
		if (dCalVal<=0) dCalVal=1;
		cutoffrig = cutoff/dCalVal;
	}
	return cutoffrig;
} //getcutoffrig

double getedeprig(const double showedep, const int tkz, const int QVer, const int irig, const int version)
{
	double showedeprig=showedep;
	//if (tkz<=0 || tkz>nQ) return showedeprig;
	if (tkz<=0) return showedeprig; //2023.03.26
	//if (f1Edep[tkz-1][QVer][version]==NULL) return showedeprig;
	//double dCalVal=f1Edep[tkz-1][QVer][version]->Eval(showedep);
	//2023.03.24
	if (oaRigEstECal==NULL)
	{
		cout << "ECal Rig estimation not read" << endl;
		return showedeprig;
	}
	histn = Form("q%d_v%d_Edep%s_EstV%d", tkz, QVer, sRigName[irig].c_str(), version);
	TF1 *f1RigEst = static_cast<TF1*>(oaRigEstECal->FindObject(histn.c_str()));
	if (f1RigEst==NULL)
	{
		// cout << "ECal Rig estimation not found for Z=" << tkz << " QVer=" << QVer << " irig=" << irig << " ver=" << version << endl;
		return showedeprig;
	}
	double dCalVal=f1RigEst->Eval(showedeprig);
	
	if (version<2)
	{
		if (dCalVal<=0) showedeprig=showedep;
		else showedeprig = dCalVal;
	}
	else
	{
		if (dCalVal<=0) dCalVal=1;
		showedeprig = showedep/dCalVal;
	}
	return showedeprig;
} //getedeprig

void ClearRigEst()
{
	delete oaRigEstCF;
	oaRigEstCF = NULL;
	delete oaRigEstECal;
	oaRigEstECal = NULL;
}

//---Cutoff->Rigidity, EnergyD/1000->Rigidity
//--use r1 for cutoff (more statistic) and r2 for Edep (more accurate geometry)
//--For Edep require the interpolation of Tracker track to pass all Ecal layer

#endif //RIGCALIB_H
