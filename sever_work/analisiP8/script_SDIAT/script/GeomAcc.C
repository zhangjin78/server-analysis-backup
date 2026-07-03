//------------------
//updated from: analysis5.C
//updated date: 2019.03.15
//------------------
//toy MC to calculate Geometrical Acceptance
//------------------
//------------------
#include "HistoMan.C"
#include "SelEvent13.h"

string histn, histn1, histn2;

int GeomAcc(long int nevent=1e7)
{
	string odir = "/atlas/users/chenya/result/toyMC";
	//string ofile = Form("GeomAcc_n%d", nevent);
	string ofile = Form("GeomAcc_n%.0e", (double)nevent);
	time_t start = 0, end = 0;
	start = time(NULL);
	cout << "Start at " << ctime(&start) << endl;
	
	//------parameter
	const float PI = 3.14159265357;
	float dXGenRange=195., dYGenRange=195.;
	float dZGen=195.;
	
	TF1 *f1Uniform = new TF1("uniform", "1", -200, 200);
	//---output
	string commond = Form("mkdir -pv %s", odir.c_str());
	cout << "creating outdir:" << system(commond.c_str()) << endl;
	
	string outf_name = Form("%s/%s.root", odir.c_str(), ofile.c_str());
	cout << outf_name.c_str() << endl;
	
	TFile outfile(outf_name.c_str(), "RECREATE");
	
	TObjArray hman1;
	TH1::SetDefaultSumw2();
	
	//--input
	SelEvent *ev = new SelEvent();
	
	//--booking histogram
	string sDir = "Geometrical_Acceptance";
	
	outfile.mkdir(sDir.c_str());
	outfile.cd(sDir.c_str());
	//--Generated distribution
	//------x,y distribution
	histn = Form("GenCoo_Gen");
	hman1.Add(new TH2D(histn.c_str(), histn.c_str(), 400, -200, 200, 400, -200, 200)); 
	GetHist(hman1, histn.c_str())->GetXaxis()->SetTitle("Gen-X");
	GetHist(hman1, histn.c_str())->GetYaxis()->SetTitle("Gen-Y");
	
	//------theta distribution
	histn = Form("Theta_Gen");
	hman1.Add(new TH1D(histn.c_str(), histn.c_str(), 900, 0, 90)); 
	GetHist(hman1, histn.c_str())->GetYaxis()->SetTitle("Nevent");
	
	//------phi distribution
	histn = Form("Phi_Gen");
	hman1.Add(new TH1D(histn.c_str(), histn.c_str(), 720, 0, 360)); 
	GetHist(hman1, histn.c_str())->GetYaxis()->SetTitle("Nevent");
	
	//--events passed fiducial volume
	for (int ir=0; ir<3; ir++) {
		outfile.mkdir(Form("%s/%s", sDir.c_str(), sSpan[ir].c_str()));
		outfile.cd(Form("%s/%s", sDir.c_str(), sSpan[ir].c_str()));
		
		histn = Form("GeomAcc_r%d", ir);
		hman1.Add(new TH1D(histn.c_str(), histn.c_str(), 2, 0, 2)); 
		GetHist(hman1, histn.c_str())->GetYaxis()->SetTitle("Nevent");
		
		//------x,y distribution
		histn = Form("GenCoo_r%d", ir);
		hman1.Add(new TH2D(histn.c_str(), histn.c_str(), 400, -200, 200, 400, -200, 200)); 
		GetHist(hman1, histn.c_str())->GetXaxis()->SetTitle("Gen-X");
		GetHist(hman1, histn.c_str())->GetYaxis()->SetTitle("Gen-Y");
		
		//------theta distribution
		histn = Form("Theta_r%d", ir);
		hman1.Add(new TH1D(histn.c_str(), histn.c_str(), 900, 0, 90)); 
		GetHist(hman1, histn.c_str())->GetYaxis()->SetTitle("Nevent");
		
		//------phi distribution
		histn = Form("Phi_r%d", ir);
		hman1.Add(new TH1D(histn.c_str(), histn.c_str(), 720, 0, 360)); 
		GetHist(hman1, histn.c_str())->GetYaxis()->SetTitle("Nevent");
	} //ir
	
	cout << "Finish booking histogram for Geometrical Acceptance." << endl;
	
	//////
	for (int ievent=0; ievent<nevent; ievent++)
	{
		if (ievent%100000==0) cout << " " << ievent << " " << ievent/(double)nevent*100 << "%" << endl;
		//------Generating initial values
		//uniform X & Y
		float dXGen = f1Uniform->GetRandom(-dXGenRange, dXGenRange);
		float dYGen = f1Uniform->GetRandom(-dYGenRange, dYGenRange);
		histn = Form("GenCoo_Gen");
		GetHist(hman1, histn.c_str())->Fill(dXGen, dYGen);
	
		//uniform cos(theta)^2
		float dCosSqrTheta = f1Uniform->GetRandom(0, 1);
		float dTheta = acos(sqrt(dCosSqrTheta));
		histn = Form("Theta_Gen");
		GetHist(hman1, histn.c_str())->Fill(dTheta*180/PI);
	
		//uniform phi
		float dPhi = f1Uniform->GetRandom(0, 2*PI);
		histn = Form("Phi_Gen");
		GetHist(hman1, histn.c_str())->Fill(dPhi*180/PI);
		
		//Generated coordinate & direction
		float dCooGen[3]={dXGen, dYGen, dZGen};
		float dDirGen[3]={sin(dTheta)*cos(dPhi), sin(dTheta)*sin(dPhi), cos(dTheta)};
		
		//------checking fiducial volume
		bool mlh[9];
		int mnhitf=0;
		for(int il=0;il<9;il++){
			mlh[il]=ev->PassTkL(il, dCooGen, dDirGen);
		} //il
	
		for(int il=0+1;il<9-1;il++){
			if(mlh[il]){mnhitf++;}
		} //il
		bool bTrackerFidu = ( (mnhitf>=5) && (mlh[1]) && (mlh[2] || mlh[3]) && (mlh[4] || mlh[5]) && (mlh[6] || mlh[7]) );
		for (int ir=1; ir<3; ir++)
		{
			histn = Form("GeomAcc_r%d", ir);
			GetHist(hman1, histn.c_str())->Fill(0);
			
			if (ir==1) bTrackerFidu = bTrackerFidu && mlh[0];
			else if (ir==2) bTrackerFidu = bTrackerFidu & mlh[8];
			if (bTrackerFidu)
			{
				histn = Form("GeomAcc_r%d", ir);
				GetHist(hman1, histn.c_str())->Fill(1);
				
				histn = Form("GenCoo_r%d", ir);
				GetHist(hman1, histn.c_str())->Fill(dXGen, dYGen);
	
				histn = Form("Theta_r%d", ir);
				GetHist(hman1, histn.c_str())->Fill(dTheta*180/PI);
	
				histn = Form("Phi_r%d", ir);
				GetHist(hman1, histn.c_str())->Fill(dPhi*180/PI);
			}
		} //ir
	} //ievent
	
	//------result of Geometrical Acceptance
	for (int ir=1; ir<3; ir++)
	{
		double dNGen=0., dNSel=0.;
		histn = Form("GeomAcc_r%d", ir);
		dNGen = GetHist(hman1, histn.c_str())->GetBinContent(1);
		dNSel = GetHist(hman1, histn.c_str())->GetBinContent(2);
		cout << "r" << ir << " NGen=" << dNGen << " " << "NSel=" << dNSel << " GeomAcc=" << 3.9*3.9*PI*dNSel/dNGen << endl;
	}
	//////--saving
	outfile.Write(); //save all histograms no matter filled or not
	outfile.Close();
	
	cout << "Outfile: " << outf_name.c_str() << endl;
	
	end = time(NULL);
	cout << "Finish at " << ctime(&end) << endl;
	cout << "total processing time:" << end - start << endl;
	
	return 0;
} //analysis

