#ifndef RIGCALIB_H
#define RIGCALIB_H
////////////////////////
//-----------------
//updated from: amsd57n_Cal.h
//updated date: 2019.05.07
//-----------------
//2019.05.07
//	1) create function BuildRigEst to create rigidity estimation function for different charge & different method from calibration 
//	   files created when plotting rigidity estimation
//
//2019.12.11
//	1) include also the charge version 3-5
//-----------------
////////////////////////
/*TF1* f1Cutoff[30][4]; //[charge][version]
TF1* f1Edep[30][4]; //[charge][version]*/
//2019.05.07: different calibration for different charge calibration
const int nQ=30;
/*TF1* f1Cutoff[nQ][3][4]; //[charge][QVer][version]
TF1* f1Edep[nQ][3][4]; //[charge][QVer][version]*/
//2019.12.11: include also charge version 4-6
TF1* f1Cutoff[nQ][6][4]; //[charge][QVer][version]
TF1* f1Edep[nQ][6][4]; //[charge][QVer][version]

void BuildRigEstv2(const string sCalibDir)
{
	cout << "***Reading Rigidity estimation calibration from " << sCalibDir << "***" << endl;
	ifstream infile;
	string ifname;
	int iEstVer=0;
	for (int ich=1; ich<=30; ich++)
	{
		for (int iQVer=0; iQVer<3; iQVer++)
		{
			for (int iver=0; iver<4; iver++)
			{
				f1Cutoff[ich-1][iQVer][iver]=NULL;
				f1Edep[ich-1][iQVer][iver]=NULL;
			} //iver
		} //iQVer
		
		//cout << "***q" << ich << "***" << endl;
		//for (int iQVer=0; iQVer<3; iQVer++)
		for (int iQVer=0; iQVer<6; iQVer++) //2019.12.11
		{
			//--cutoff
			infile.open(Form("%s/q%d_v%d_cutoff", sCalibDir.c_str(), ich, iQVer));
			iEstVer=0;
			while(infile.good())
			{
				getline(infile,ifname,'\n');
				if (ifname=="") continue;
				//if (ifname.find("cutoff")!=string::npos) cout << ifname << endl; //2019.12.11: take out to reduce print out
				//else
				if (ifname.find("cutoff")==string::npos)
				{
					//cout << "function setting " << iEstVer << ": " << ifname;
					f1Cutoff[ich-1][iQVer][iEstVer] = new TF1(Form("q%d_cutoff_v%d", ich, iEstVer), ifname.c_str(), 0, 32);
					iEstVer++;
				}
			}
			infile.close();
			if (iEstVer>0) cout << "***Finish reading calibration for q" << ich << " cutoff" << endl;
		
			//--Edep
			infile.open(Form("%s/q%d_v%d_Edep", sCalibDir.c_str(), ich, iQVer));
			iEstVer=0;
			while(infile.good())
			{
				getline(infile,ifname,'\n');
				if (ifname=="") continue;
				//if (ifname.find("Edep")!=string::npos) cout << ifname << endl; //2019.12.11: take out to reduce print out
				//else
				if (ifname.find("Edep")==string::npos)
				{
					//cout << "function setting " << iEstVer << ": " << ifname << endl;
					f1Edep[ich-1][iQVer][iEstVer] = new TF1(Form("q%d_Edep_v%d", ich, iEstVer), ifname.c_str(), 0, 850);
					iEstVer++;
				}
			}
			infile.close();
			if (iEstVer>0) cout << "***Finish reading calibration for q" << ich << " Edep" << endl;
		} //iQVer
	}
}

double getcutoffrig5(const double cutoff, const int tkz, const int QVer, const int version)
{
	double cutoffrig=cutoff;
	if (tkz<=0 || tkz>nQ) return cutoffrig;
	if (f1Cutoff[tkz-1][QVer][version]==NULL) return cutoffrig;
	double dCalVal=f1Cutoff[tkz-1][QVer][version]->Eval(cutoff);
	if (version<2) {
		if (dCalVal<=0) cutoffrig=cutoff;
		else cutoffrig = dCalVal;
	}
	else {
		if (dCalVal<=0) dCalVal=1;
		cutoffrig = cutoff/dCalVal;
	}
	return cutoffrig;
} //getcutoffrig5

double getedeprig5(const double showedep, const int tkz, const int QVer, const int version)
{
	double showedeprig=showedep;
	if (tkz<=0 || tkz>nQ) return showedeprig;
	if (f1Edep[tkz-1][QVer][version]==NULL) return showedeprig;
	double dCalVal=f1Edep[tkz-1][QVer][version]->Eval(showedep);
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
} //getedeprig5

//---Cutoff->Rigidity, EnergyD/1000->Rigidity
//--use r1 for cutoff (more statistic) and r2 for Edep (more accurate geometry)
//--For Edep require the interpolation of Tracker track to pass all Ecal layer

#endif //RIGCALIB_H
