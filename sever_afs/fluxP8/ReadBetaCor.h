#ifndef READBETACOR_H
#define READBETACOR_H
////////////////////////
//-----------------
//create date: 2021.09.06
//-----------------
//-----------------
////////////////////////
//2019.05.07: different calibration for different charge calibration
const int iBetaCorNQ=30;
TF1* f1BetaCor[iBetaCorNQ][2][3]; //[charge][ievcut][Beta]

bool InitialBetaCor()
{
	for (int ich=1; ich<=30; ich++)
	{
		for (int ibeta=0; ibeta<3; ibeta++)
		{
			for (int ievcut=0; ievcut<2; ievcut++)
			{
				f1BetaCor[ich-1][ievcut][ibeta]=NULL;
			} //ievcut
		} //ibeta
	} //ich
	
	return true;
}

bool ClearBetaCor()
{
	for (int ich=1; ich<=30; ich++)
	{
		for (int ibeta=0; ibeta<3; ibeta++)
		{
			for (int ievcut=0; ievcut<2; ievcut++)
			{
				delete f1BetaCor[ich-1][ievcut][ibeta];
				f1BetaCor[ich-1][ievcut][ibeta]=NULL;
			} //ievcut
		} //ibeta
	} //ich
	
	return true;
}

bool ReadBetaCor(const string sCalibDir)
{
	cout << "***Reading Beta Correction from " << sCalibDir << "***" << endl;
	InitialBetaCor();
	
	ifstream infile;
	string sPar;
	
	int iEstVer=0;
	for (int ich=1; ich<=30; ich++)
	{
		for (int ibeta=0; ibeta<3; ibeta++)
		{
			for (int ievcut=0; ievcut<2; ievcut++)
			{
				vector<double> vdMean, vdSigma;
				
				//--par=1: mean, par=2: sigma
				for (int ipar=1; ipar<3; ipar++)
				{
					infile.open(Form("%s/BetaResoCor_q%d/Beta%d_q%d_evcut%d_par%d", sCalibDir.c_str(), ich, ibeta, ich, ievcut, ipar));
					while(infile.good())
					{
						getline(infile, sPar, '\n');
						if (sPar=="") continue;
						istringstream issStream(sPar);
						
						double dPar;
						issStream >> dPar;
						
						if (ipar==1) vdMean.push_back(dPar);
						else vdSigma.push_back(dPar);
					}
					infile.close();
				} //ipar
				if (vdMean.size()!=2 || vdSigma.size()!=2) continue;
				
				string sName = Form("BetaCor%d_q%d_evcut%d", ibeta, ich, ievcut);
				f1BetaCor[ich-1][ievcut][ibeta] = new TF1(sName.c_str(), "(x-[0])/[1]*[2]+[3]", -10, 10);
				f1BetaCor[ich-1][ievcut][ibeta]->SetParameter(0, vdMean.at(1));
				f1BetaCor[ich-1][ievcut][ibeta]->SetParameter(1, vdSigma.at(1));
				f1BetaCor[ich-1][ievcut][ibeta]->SetParameter(2, vdSigma.at(0));
				f1BetaCor[ich-1][ievcut][ibeta]->SetParameter(3, vdMean.at(0));
				
				cout << "Finish reading Beta correction for q" << ich << " ievcut=" << ievcut << " ibeta=" << ibeta << ": mu_data=" << vdMean.at(0) << ", mu_MC=" << vdMean.at(1) << ", sigma_data=" << vdSigma.at(0) << ", sigma_MC=" << vdSigma.at(1) << endl;
			} //ievcut
		} //ibeta
	}
	return true;
} //ReadBetaCor

double GetCorBeta(double dBetaGen, double dBetaRec, int ich, int ievcut, int ibeta, const string option="rBeta")
{
	double dBetaRecCor=0.;
	
	double dDBeta=0.;
	if (option=="rBeta") dDBeta = 1./dBetaGen - 1./dBetaRec;
	else if (option=="Beta") dDBeta = dBetaGen - dBetaRec;
	else
	{
		cout << "GetCorBeta: wrong option given, option=" << option << ", can only take option=rBeta or option=Beta. return -1." << endl;
		return -1;
	}
	if (f1BetaCor[ich-1][ievcut][ibeta]==NULL)
	{
		//cout << "GetCorBeta: the Beta correction function has not been set for ich=" << ich << " ievcut=" << ievcut << " ibeta=" << ibeta << endl;
		return -1;
	}
	
	double dDBetaCor = f1BetaCor[ich-1][ievcut][ibeta]->Eval(dDBeta);
	
	if (option=="rBeta") dBetaRecCor = 1./(1./dBetaGen - dDBetaCor);
	else if (option=="Beta") dBetaRecCor = dBetaGen - dDBetaCor;
	
	return dBetaRecCor;
}
#endif //READBETACOR_H
