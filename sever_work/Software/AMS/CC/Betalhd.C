#include "Betalhd.h"
#include "commonsi.h"

using namespace std;
int Betalhd::InitialBetaLikelihood(AMSEventR * ev, int correction){
	ParticleR *p = ev->pParticle(0);
	TrTrackR *  trk = (p) ? p->pTrTrack () : 0;
	BetaHR *  bth = (p) ? p->pBetaH()    : 0;
	TrdTrackR * trd = (p) ? p->pTrdTrack() : 0;
	TrdKCluster trdkcluster;
	if (trd!=0) {
		trdkcluster = TrdKCluster(ev, trd, trk->GetRigidity());
	}else if(trk){
		int fitcode =trk->iTrTrackPar(2,0,21);
		trdkcluster = TrdKCluster(ev, trk, fitcode);
	}

	return Betalhd::InitialBetaLikelihood(ev, trdkcluster, trk, bth, correction);
}
int Betalhd::InitialBetaLikelihood(AMSEventR * ev, TrdKCluster trdkcluster, TrTrackR * trk, BetaHR *bth, int correction){
	if (DeBug){
	cout << "Initializing BetaLikelihood variables." << endl;
	}
	ParticleR *p = ev->pParticle(0);
	if (!trk)  trk = (p) ? p->pTrTrack () : 0;
	if (!bth)  bth = (p) ? p->pBetaH()    : 0;
	if (ev->nMCEventg()) {
		bth->DoMCtune();
		TofRecH::ReBuild(1);
		Betalhd::IsMC = true;
	}
	else Betalhd::IsMC = false;
	float betah_tof = bth -> GetBeta();
	betalhd_bthpattern = bth -> GetBetaPattern();
	if (betah_tof > 0) Betalhd::betalhd_opt = 0;
	else Betalhd::betalhd_opt = 1;
	float trk_coo[10][3], trd_coo[20][3], tof_coo[4][3], trk_d[10][2], trd_d[20][2], tof_d[4][2];
	float trk_dEdx[2][9], trd_dEdx[20], tof_dEdx[4];
	Betalhd::GetVariable(ev, trk_dEdx, trd_dEdx, tof_dEdx, trk_coo, trd_coo, tof_coo, trk_d, trd_d, tof_d,betalhd_Time, betalhd_TimeE, betalhd_PathLength, betalhd_PathLength_trk, trdkcluster, trk, bth, betalhd_opt);

	for (int i = 0; i < 10 ;i ++) {
		if (i < 9) {
			betalhd_trk_dEdx[0][i] = trk_dEdx[0][i];
			betalhd_trk_dEdx[1][i] = trk_dEdx[1][i];
		}
		betalhd_trk_pnt[i].setp(trk_coo[i][0],trk_coo[i][1],trk_coo[i][2]);
		betalhd_trk_dir[i].SetTheta(trk_d[i][0]);
		betalhd_trk_dir[i].SetPhi(trk_d[i][1]);
	}
	for (int i = 0; i < 4 ;i ++) {
		betalhd_tof_dEdx[i] = tof_dEdx[i];
		betalhd_tof_pnt[i].setp(tof_coo[i][0],tof_coo[i][1],tof_coo[i][2]);
		betalhd_tof_dir[i].SetTheta(tof_d[i][0]);
		betalhd_tof_dir[i].SetPhi(tof_d[i][1]);
	}
	for (int i = 0; i < 20 ;i ++) {
		betalhd_trd_dEdx[i] = trd_dEdx[i];
		betalhd_trd_pnt[i].setp(trd_coo[i][0],trd_coo[i][1],trd_coo[i][2]);
		betalhd_trd_dir[i].SetTheta(trd_d[i][0]);
		betalhd_trd_dir[i].SetPhi(trd_d[i][1]);
	}

	if (correction){
		if((ev->nMCEventg() > 0) && (ev->Head()->Version()>=1106)){
			GetMCTrackerAmpCorrection(betalhd_trk_dEdx);
		}
		if (ev->nMCEventg()==0){
			int utime = (int) ev->UTime();
			for (int i = 0; i < 4 ; i++) betalhd_tof_dEdx[i] /= GetTOFAmpCorrection(i, utime);
		}
	}

	betalhd_beta_rich_rec = 0;
	betalhd_beta_rich_err = -1;
	if (ev->nRichRing()>0) {
		int iring = (p) ? p->iRichRing() : -1;
		if (iring >= 0 ){
			RichRingR * richring = p-> pRichRing();
			if (richring && richring->IsGood() && richring->IsClean()) {			
				betalhd_beta_rich_rec = richring -> getBeta();
				betalhd_beta_rich_err = richring -> getBetaError();
			}
		}
	}

	return 0;
}

bool Betalhd::DeBug;
bool Betalhd::DeBug_1;
void Betalhd::SetDeBug(bool debug, int version){
	if (version==0) Betalhd::DeBug = debug;
	if (version==1) Betalhd::DeBug_1 = debug;
}
double Betalhd::GetBetaLikelihood_TOFRICH(float & beta, float & beta_error ,AMSPoint * trk_pnt, AMSPoint * tof_pnt, float * Time, float * TimeE, float * PathLength, bool IsNaF, float beta_rich_rec, TrProp prop, int opt){
	int bthpattern = 4444;
	float PathLength_trk[7] = {0};//inner tracker
	float beta_rich_err = -1;
	return GetBetaLikelihood_TOFRICH(beta, beta_error, trk_pnt, tof_pnt, Time, TimeE,  PathLength, 0, bthpattern, IsNaF, beta_rich_rec, prop, beta_rich_err, opt);
}
int Betalhd::IsRICHhit = 0;
double Betalhd::GetBetaLikelihood_RICHhit(float & beta, float & beta_error ,AMSPoint * trk_pnt, AMSPoint * tof_pnt, AMSDir * tof_dir, bool IsNaF, vector<float> hit_beta_ini[2], int opt, int charge , double m_p ){
	ROOT::Math::MinimizerOptions::SetDefaultMinimizer("Minuit");
	ROOT::Fit::Fitter fitter;
	const int Npar = 1;
	//double par0[Npar] = {beta * (1 + exp(1.71 - 7.53*beta))};
	double par0[Npar] = {beta};
	Betalhd::ini_RICHhit(  IsNaF,  hit_beta_ini, opt, m_p, charge);
	for (int i = 0; i < 4; i++) Betalhd::betalhd_tof_dir[i] = tof_dir[i];
	Betalhd::IsdEdx = 0;
	float limit_1 = 0.95;
	float limit_2 = 1.02;
	if (IsNaF) {
		limit_1 = 0.75;
		limit_2 = 1.05;
	}
	if (par0[0] < limit_1 || par0[0] > limit_2) par0[0] = (limit_1 + limit_2)/2;
	fitter.Config().SetParamsSettings(Npar, par0);
	fitter.Config().ParSettings(0).SetLimits(limit_1, limit_2);
	fitter.Config().MinimizerOptions().SetPrintLevel(0);
	fitter.Config().SetMinimizer("Minuit2", "Scan");
	Betalhd betalhd;
	fitter.FitFCN(Npar, betalhd);
	ROOT::Fit::FitResult result = fitter.Result();
	beta = result.GetParams()[0];
	if (IsNaF) fitter.Config().ParSettings(0).SetLimits(beta-0.007,beta+0.007);
	else fitter.Config().ParSettings(0).SetLimits(beta-0.0035,beta+0.0035);
	fitter.Config().SetMinimizer("Minuit2", "Simplex");
	fitter.FitFCN(Npar, betalhd);
	ROOT::Fit::FitResult result2 = fitter.Result();
	beta = result2.GetParams()[0];
	beta_error = result2.GetErrors()[0];
	double minval = result2.MinFcnValue();
	update_variable = true;
	par0[0] = beta;
	getchi2_offline(par0);
	update_variable = false;

	return exp(-.5*minval);
}
double Betalhd::GetBetaLikelihood_TOFRICHhit(float & beta, float & beta_error ,AMSPoint * trk_pnt, AMSPoint * tof_pnt, float * Time, float * TimeE, float * PathLength, float * PathLength_trk, int bthpattern, AMSDir * tof_dir, bool IsNaF, vector<float> hit_beta_ini[2], int opt, int charge , double m_p ){
	int err_level = gErrorIgnoreLevel;
	gErrorIgnoreLevel = 5000;
	ROOT::Math::MinimizerOptions::SetDefaultMinimizer("Minuit");
	ROOT::Fit::Fitter fitter;
	const int Npar = 1;
	//double par0[Npar] = {beta * (1 + exp(1.71 - 7.53*beta))};
	double par0[Npar] = {beta};
	Betalhd::ini_TOF(  trk_pnt, tof_pnt, Time,  TimeE, PathLength, PathLength_trk, bthpattern, opt, m_p, charge);
	Betalhd::ini_RICHhit(  IsNaF,  hit_beta_ini, opt, m_p, charge);
	for (int i = 0; i < 4; i++) Betalhd::betalhd_tof_dir[i] = tof_dir[i];
	Betalhd::IsdEdx = 0;
	float limit_1 = 0.95;
	float limit_2 = 1.02;
	if (IsNaF) {
		limit_1 = 0.75;
		limit_2 = 1.05;
	}
	if (par0[0] < limit_1 || par0[0] > limit_2) par0[0] = (limit_1 + limit_2)/2;
	fitter.Config().SetParamsSettings(Npar, par0);
	fitter.Config().ParSettings(0).SetLimits(limit_1, limit_2);
	fitter.Config().MinimizerOptions().SetPrintLevel(0);
	fitter.Config().SetMinimizer("Minuit2", "Scan");
	Betalhd betalhd;
	fitter.FitFCN(Npar, betalhd);
	ROOT::Fit::FitResult result = fitter.Result();
	beta = result.GetParams()[0];
	if (IsNaF) fitter.Config().ParSettings(0).SetLimits(beta-0.007,beta+0.007);
	else fitter.Config().ParSettings(0).SetLimits(beta-0.0015,beta+0.0015);
	//fitter.Config().SetMinimizer("Minuit2", "Simplex");// result not continue
	fitter.Config().SetMinimizer("Minuit2", "Migrad");// result continue
	fitter.FitFCN(Npar, betalhd);
	ROOT::Fit::FitResult result2 = fitter.Result();
	beta = result2.GetParams()[0];
	beta_error = result2.GetErrors()[0];
	double minval = result.MinFcnValue();
	update_variable = true;
	par0[0] = beta;
	getchi2_offline(par0);
	update_variable = false;
	if (DeBug){
		for (int i = 0; i < hit_beta[0].size(); i++){
			cout << "RICH hit[" << i << "] = " << hit_beta[0][i] << " , " << hit_beta[1][i] << endl;
		}
		cout << "best fit = " << IsNaF << ", "<< beta << ", " << minval << endl;
	}
	gErrorIgnoreLevel = err_level;
	return exp(-.5*minval);
}

double Betalhd::GetBetaLikelihood_TOFRICH(float & beta, float & beta_error ,AMSPoint * trk_pnt, AMSPoint * tof_pnt, float * Time, float * TimeE, float * PathLength, float * PathLength_trk, int bthpattern,bool IsNaF, float beta_rich_rec, TrProp prop, float beta_rich_err, int opt){
	int err_level = gErrorIgnoreLevel;
	gErrorIgnoreLevel = 5000;
	ROOT::Math::MinimizerOptions::SetDefaultMinimizer("Minuit");
	ROOT::Fit::Fitter fitter;
	const int Npar = 1;
	double par0[Npar] = {beta * (1 + exp(1.71 - 7.53*beta))};
	Betalhd::ini_TOF(  trk_pnt, tof_pnt, Time,  TimeE, PathLength, PathLength_trk, bthpattern, opt);
	if (beta_rich_rec > 0.7) {
		Betalhd::ini_RICH(  IsNaF, beta_rich_rec, beta_rich_err, prop, opt);
		par0[0] = beta_rich_rec;
		Betalhd::IsTOFRICH = 2;
	}else {
		Betalhd::IsTOFRICH = 0;
	}
	Betalhd::IsdEdx = 0;
	fitter.Config().SetParamsSettings(Npar, par0);
	fitter.Config().MinimizerOptions().SetPrintLevel(0);
	fitter.Config().SetMinimizer("Minuit2", "Simplex");
	//fitter.Config().SetMinimizer("Minuit2", "Scan");
	//fitter.Config().SetMinimizer("Minuit2", "Migrad");
	Betalhd betalhd;
	fitter.FitFCN(Npar, betalhd);
	//fitter.CalculateHessErrors();
	//fitter.CalculateMinosErrors();
	ROOT::Fit::FitResult result = fitter.Result();
	int status = result.Status();
	/*	if ( status != 0 ){
		for (int ii=0; ii<100; ii++) {
		fitter.FitFCN(Npar, betalhd);
	//fitter.CalculateMinosErrors();
	ROOT::Fit::FitResult result = fitter.Result();
	status = result.Status();
	if (status==0) break;	
	}
	}*/
	beta = result.GetParams()[0];
	//beta_error = -result.LowerError(0);
	beta_error = result.GetErrors()[0];
	double minval = result.MinFcnValue();
	//cout << beta << "+/-" << beta_error << endl;
	gErrorIgnoreLevel = err_level;
	return exp(-.5*minval);
}
double Betalhd::GetBetaLikelihood_TOF(float & beta, float & beta_error ,AMSPoint * trk_pnt, AMSPoint * trd_pnt, AMSPoint * tof_pnt, float * Time, float * TimeE, float * PathLength, float * PathLength_trk, int bthpattern, int opt, int charge, double m_p){
	ROOT::Math::MinimizerOptions::SetDefaultMinimizer("Minuit");
	Betalhd::ini_TOF(  trk_pnt, trd_pnt, tof_pnt, Time,  TimeE, PathLength, PathLength_trk, bthpattern, opt, m_p, charge);
	Betalhd::IsRICH = 0;
	Betalhd::IsdEdx = 0;
	ROOT::Fit::Fitter fitter;
	const int Npar = 1;
	double par0[Npar];
	if (beta < 1) {
		par0[0]= (beta * (1 + exp(1.71 - 7.53*beta)));
		if (opt%2) par0[0] = beta * (1 + exp(1.82 - 6.54*beta));
	}
	else par0[0] = beta;
	fitter.Config().SetParamsSettings(Npar, par0);
	fitter.Config().MinimizerOptions().SetPrintLevel(0);
	//fitter.Config().SetMinimizer("Minuit2", "Simplex");
	//fitter.Config().SetMinimizer("Minuit2", "Scan");
	fitter.Config().SetMinimizer("Minuit2", "Migrad");
	Betalhd betalhd;
	fitter.FitFCN(Npar, betalhd);
	//fitter.CalculateMinosErrors();
	/*ROOT::Fit::FitResult result = fitter.Result();
	  beta = result.GetParams()[0];
	  fitter.Config().ParSettings(0).SetLimits(beta-0.016,beta+0.016);
	  fitter.FitFCN(Npar, betalhd);
	  */
	ROOT::Fit::FitResult result2 = fitter.Result();
	beta = result2.GetParams()[0];
	beta_error = result2.GetErrors()[0];
	double minval = result2.MinFcnValue();
	return exp(-.5*minval);
}
double Betalhd::GetBetaLikelihood_TOF(float & beta, float & beta_error ){
	return Betalhd::GetBetaLikelihood_TOF(beta, beta_error, betalhd_trk_pnt, betalhd_trd_pnt, betalhd_tof_pnt, betalhd_Time, betalhd_TimeE, betalhd_PathLength, betalhd_PathLength_trk, betalhd_bthpattern, betalhd_opt);
}
double Betalhd::GetBetaLikelihood_TOF(float & beta, float & beta_error ,AMSPoint * trk_pnt, AMSPoint * tof_pnt, float * Time, float * TimeE, float * PathLength, float * PathLength_trk, int bthpattern, int opt,  int charge, double m_p){
	ROOT::Math::MinimizerOptions::SetDefaultMinimizer("Minuit");
	Betalhd::ini_TOF(  trk_pnt, tof_pnt, Time,  TimeE, PathLength, PathLength_trk, bthpattern, opt, m_p, charge);
	Betalhd::IsRICH = 0;
	Betalhd::IsdEdx = 0;
	ROOT::Fit::Fitter fitter;
	const int Npar = 1;
	double par0[Npar];
	if (beta < 1) {
		par0[0]= (beta * (1 + exp(1.71 - 7.53*beta)));
		if (opt%2) par0[0] = beta * (1 + exp(1.82 - 6.54*beta));
	}
	else par0[0] = beta;
	fitter.Config().SetParamsSettings(Npar, par0);
	fitter.Config().MinimizerOptions().SetPrintLevel(0);
	//fitter.Config().SetMinimizer("Minuit2", "Scan");
	fitter.Config().SetMinimizer("Minuit2", "Migrad");
	Betalhd betalhd;
	fitter.FitFCN(Npar, betalhd);
	//fitter.CalculateMinosErrors();
	ROOT::Fit::FitResult result = fitter.Result();
	int status = result.Status();
	beta = result.GetParams()[0];
	beta_error = result.GetErrors()[0];
	//fitter.Config().ParSettings(0).SetLimits(beta-0.12,beta+0.12);
	//fitter.Config().SetMinimizer("Minuit2", "Migrad");
	//fitter.FitFCN(Npar, betalhd);
	//ROOT::Fit::FitResult result2 = fitter.Result();
	//beta = result2.GetParams()[0];

	//	fitter.CalculateHessErrors();
	//	result = fitter.Result();
	//result.Print(std::cout);
	//	fitter.CalculateMinosErrors();
	//	result = fitter.Result();
	//result.Print(std::cout);
	double minval = result.MinFcnValue();
	update_variable = true;
	par0[0] = beta;
	getchi2_offline(par0);
	update_variable = false;

	return exp(-.5*minval);
}

double Betalhd::GetBetaLikelihood_TOFdEdx(
		float & beta, float & beta_error ,
		AMSPoint * trk_pnt, AMSPoint * trd_pnt, AMSPoint * tof_pnt, 
		float * Time, float * TimeE, float * PathLength, float * PathLength_trk, 
		float trk_dEdx[2][9], float trd_dEdx[20], float tof_dEdx[4],
		int bthpattern, int opt,
		int charge, double m_p){
	ROOT::Math::MinimizerOptions::SetDefaultMinimizer("Minuit");
	Betalhd::ini_TOF(  trk_pnt, trd_pnt, tof_pnt, Time,  TimeE, PathLength, PathLength_trk, bthpattern, opt, m_p, charge);
	Betalhd::ini_dEdx(  trk_pnt, trd_pnt, tof_pnt, trk_dEdx, trd_dEdx, tof_dEdx, opt);
	Betalhd::IsRICH = 0;
	ROOT::Fit::Fitter fitter;
	const int Npar = 1;
	double par0[Npar];
	if (beta < 1) {
		par0[0]= beta * (1 + exp(1.71 - 7.53*beta));
		if (opt%2) par0[0] = beta * (1 + exp(1.82 - 6.54*beta));
	}
	else par0[0] = beta;
	fitter.Config().SetParamsSettings(Npar, par0);
	fitter.Config().MinimizerOptions().SetPrintLevel(0);
	fitter.Config().SetMinimizer("Minuit2", "Migrad");
	Betalhd betalhd;
	fitter.FitFCN(Npar, betalhd);
	//fitter.CalculateMinosErrors();
	ROOT::Fit::FitResult result = fitter.Result();
	int status = result.Status();
	beta = result.GetParams()[0];
	beta_error = result.GetErrors()[0];
	double minval = result.MinFcnValue();
	return exp(-.5*minval);
}

double Betalhd::GetBetaLikelihood_dEdx(
		float & beta, float & beta_error ,
		int opt,
		float m_p,
		int charge, 
		int subopt
		){
	float trk_dEdx[2][9], trd_dEdx[20], tof_dEdx[4];
	for (int i = 0; i < 9; i++) {
		trk_dEdx[0][i] = betalhd_trk_dEdx[0][i];
		trk_dEdx[1][i] = betalhd_trk_dEdx[1][i];
	}
	for (int i = 0; i < 20; i++) {
		trd_dEdx[i] = betalhd_trd_dEdx[i];
	}
	for (int i = 0; i < 4; i++) {
		tof_dEdx[i] = betalhd_tof_dEdx[i];
	}
	return Betalhd::GetBetaLikelihood_dEdx( beta, beta_error , betalhd_trk_pnt, betalhd_trd_pnt, betalhd_tof_pnt, 
			trk_dEdx, trd_dEdx, tof_dEdx,
			opt,
			m_p,
			charge, 
			subopt
			);

}
double Betalhd::GetBetaLikelihood_dEdx(
		float & beta, float & beta_error ,
		AMSPoint * trk_pnt, AMSPoint * trd_pnt, AMSPoint * tof_pnt, 
		float trk_dEdx[2][9], float trd_dEdx[20], float tof_dEdx[4],
		int opt,
		float m_p,
		int charge, 
		int subopt
		){
	ROOT::Math::MinimizerOptions::SetDefaultMinimizer("Minuit");
	Betalhd::ini_dEdx(  trk_pnt, trd_pnt, tof_pnt, trk_dEdx, trd_dEdx, tof_dEdx, opt, m_p, charge,subopt);
	Betalhd::IsTOF = 0;
	Betalhd::IsRICH = 0;
	ROOT::Fit::Fitter fitter;
	const int Npar = 1;
	double par0[Npar];
	if (beta < 1) {
		par0[0]= beta * (1 + exp(1.71 - 7.53*beta));
		if (opt%2) par0[0] = beta * (1 + exp(1.82 - 6.54*beta));
	}
	else par0[0] = 1;
	//cout << "par0 = " << par0[0] << endl;
	double vstep[Npar];
	fitter.Config().SetParamsSettings(Npar, par0);
	fitter.Config().ParSettings(0).SetLimits(0.2, 1);
	fitter.Config().SetMinimizer("Minuit2", "Scan");
	fitter.Config().MinimizerOptions().SetPrintLevel(0);
	Betalhd betalhd;
	fitter.FitFCN(Npar, betalhd);
	//fitter.CalculateMinosErrors();
	ROOT::Fit::FitResult result = fitter.Result();
	int status = result.Status();
	beta = result.GetParams()[0];
	fitter.Config().ParSettings(0).SetLimits(beta-0.016,beta+0.016);
	fitter.FitFCN(Npar, betalhd);
	ROOT::Fit::FitResult result2 = fitter.Result();

	beta = result2.GetParams()[0];
	beta_error = result2.GetErrors()[0];
	double minval = result.MinFcnValue();
	//cout << "beta_dEdx = " <<beta << endl;
	return exp(-.5*minval);
}

double Betalhd::GetBetaLikelihood_RICH(bool IsNaF, float & beta_rich_rec, float & beta_err, TrProp prop, float beta_rich_err, int opt){
	if (beta_rich_rec >= 1) return -2;
	if (beta_rich_rec < 0.7) return -1;
	ROOT::Math::MinimizerOptions::SetDefaultMinimizer("Minuit");
	Betalhd::ini_RICH(  IsNaF, beta_rich_rec, beta_rich_err, prop, opt);
	Betalhd::IsTOF = 0;
	ROOT::Fit::Fitter fitter;
	const int Npar = 1;
	double par0[Npar] = {beta_rich_rec+0.00001};
	if (par0[0] > 1) par0[0] = 0.99999;
	fitter.Config().SetParamsSettings(Npar, par0);
	//fitter.Config().ParSettings(0).SetLimits(par0[0]-0.00001,1);
	fitter.Config().MinimizerOptions().SetPrintLevel(0);
	fitter.Config().SetMinimizer("Minuit2", "Migrad");
	Betalhd betalhd;
	fitter.FitFCN(Npar, betalhd);
	fitter.CalculateMinosErrors();
	ROOT::Fit::FitResult result = fitter.Result();
	beta_rich_rec = result.GetParams()[0];
	//beta_err = result.GetErrors()[0];
	beta_err = result.UpperError(0);
	double minval = result.MinFcnValue();
	//cout << "RICH: "<< beta_rich_rec << "+" << beta_err <<  result.LowerError(0) << ", beta_rich_err = " << beta_rich_err << ", error_ratio=" << beta_err/beta_rich_err <<  endl;
	return exp(-0.5*minval);
}
double Betalhd::GetBetaLikelihood_TOFRICH(
		float & beta, float & beta_error ,
		AMSPoint * trk_pnt, AMSPoint * trd_pnt, AMSPoint * tof_pnt, 
		float * Time, float * TimeE, float * PathLength, float * PathLength_trk, 
		int IsNaF, float beta_rich, float beta_rich_err, AMSDir * tof_dir,
		int bthpattern, int opt, 
		int charge, float m_p ){
	return Betalhd::GetBetaLikelihood_TOFRICH(
			beta, beta_error ,
			trk_pnt, trd_pnt, tof_pnt, 
			Time, TimeE, PathLength, PathLength_trk, 
			(bool)IsNaF, beta_rich, beta_rich_err, tof_dir,
			bthpattern, opt, 
			charge, m_p );
}

double Betalhd::GetBetaLikelihood_TOFRICH(
		float & beta, float & beta_error ,
		AMSPoint * trk_pnt, AMSPoint * trd_pnt, AMSPoint * tof_pnt, 
		float * Time, float * TimeE, float * PathLength, float * PathLength_trk, 
		bool IsNaF, float beta_rich, float beta_rich_err, AMSDir * tof_dir,
		int bthpattern, int opt, 
		int charge, float m_p ){
	float trk_dEdx_tmp[2][9];
	float trd_dEdx_tmp[20];
	float tof_dEdx_tmp[4];
	memset(trk_dEdx_tmp, 0, sizeof(trk_dEdx_tmp));
	memset(trd_dEdx_tmp, 0, sizeof(trd_dEdx_tmp));
	memset(tof_dEdx_tmp, 0, sizeof(tof_dEdx_tmp));
	return Betalhd::GetBetaLikelihood(
			beta, beta_error ,
			trk_pnt, trd_pnt, tof_pnt, 
			Time, TimeE, PathLength, PathLength_trk, 
			trk_dEdx_tmp, trd_dEdx_tmp, tof_dEdx_tmp,
			IsNaF, beta_rich, beta_rich_err, tof_dir,
			bthpattern, opt, 
			charge, m_p );
}
double Betalhd::GetBetaLikelihood(
		float & beta, float & beta_error ,
		AMSPoint * trk_pnt, AMSPoint * trd_pnt, AMSPoint * tof_pnt, 
		float * Time, float * TimeE, float * PathLength, float * PathLength_trk, 
		float trk_dEdx[2][9], float trd_dEdx[20], float tof_dEdx[4],
		int IsNaF, float beta_rich, float beta_rich_err, AMSDir * tof_dir,
		int bthpattern, int opt, 
		int charge, float m_p , bool FitInnerRigidity){
	return Betalhd::GetBetaLikelihood(
			beta, beta_error ,
			trk_pnt, trd_pnt, tof_pnt, 
			Time, TimeE, PathLength, PathLength_trk, 
			trk_dEdx, trd_dEdx, tof_dEdx,
			(bool) IsNaF, beta_rich, beta_rich_err, tof_dir,
			bthpattern, opt, 
			charge, m_p , FitInnerRigidity);
}
double Betalhd::GetBetaLikelihood(
		float & beta, float & beta_error ,
		AMSPoint * trk_pnt, AMSPoint * trd_pnt, AMSPoint * tof_pnt, 
		float * Time, float * TimeE, float * PathLength, float * PathLength_trk, 
		float trk_dEdx[2][9], float trd_dEdx[20], float tof_dEdx[4],
		int IsNaF, vector<float> hit_beta_ini[2], AMSDir * tof_dir,
		int bthpattern, int opt, 
		int charge, float m_p , bool FitInnerRigidity){
	return Betalhd::GetBetaLikelihood(
			beta, beta_error ,
			trk_pnt, trd_pnt, tof_pnt, 
			Time, TimeE, PathLength, PathLength_trk, 
			trk_dEdx, trd_dEdx, tof_dEdx,
			(bool) IsNaF, hit_beta_ini, tof_dir,
			bthpattern, opt, 
			charge, m_p , FitInnerRigidity);
}
bool Betalhd::betalhd_FitInnerRigidity = false;
double Betalhd::GetBetaLikelihood(float & beta, float & beta_error, int charge, float m_p) {
	float trk_dEdx[2][9], trd_dEdx[20], tof_dEdx[4];
	for (int i =0 ; i < 9 ;i ++){
		trk_dEdx[0][i] = (float) betalhd_trk_dEdx[0][i]; 
		trk_dEdx[1][i] = (float) betalhd_trk_dEdx[1][i]; 
	}
	for (int i =0 ; i < 20 ;i ++){
		trd_dEdx[i] = (float) betalhd_trd_dEdx[i]; 
	}
	for (int i =0 ; i < 4 ;i ++){
		tof_dEdx[i] = (float) betalhd_tof_dEdx[i]; 
	}
	float beta_rich_rec = (float) betalhd_beta_rich_rec;
	float beta_rich_err = (float) betalhd_beta_rich_err;
	return Betalhd::GetBetaLikelihood(
			beta, beta_error ,
			betalhd_trk_pnt, betalhd_trd_pnt, betalhd_tof_pnt, 
			betalhd_Time, betalhd_TimeE, betalhd_PathLength, betalhd_PathLength_trk, 
			trk_dEdx, trd_dEdx, tof_dEdx,
			betalhd_IsNaF, betalhd_beta_rich_rec, betalhd_beta_rich_err, betalhd_tof_dir,
			betalhd_bthpattern, betalhd_opt, 
			charge, m_p);

}
double Betalhd::GetBetaLikelihood(
		float & beta, float & beta_error ,
		AMSPoint * trk_pnt, AMSPoint * trd_pnt, AMSPoint * tof_pnt, 
		float * Time, float * TimeE, float * PathLength, float * PathLength_trk, 
		float trk_dEdx[2][9], float trd_dEdx[20], float tof_dEdx[4],
		bool IsNaF, float beta_rich, float beta_rich_err, AMSDir * tof_dir,
		int bthpattern, int opt, 
		int charge, float m_p, bool FitInnerRigidity ){
	int err_level = gErrorIgnoreLevel;
	gErrorIgnoreLevel = 5000;
	ROOT::Math::MinimizerOptions::SetDefaultMinimizer("Minuit2");
	Betalhd::IsdEdx = 0;
	Betalhd::IsTOF = 0;
	Betalhd::IsRICH = 0;
	Betalhd::ini_TOF(  trk_pnt, trd_pnt, tof_pnt, Time,  TimeE, PathLength, PathLength_trk, bthpattern, opt, m_p, charge);
	Betalhd::ini_dEdx(  trk_pnt, trd_pnt, tof_pnt, trk_dEdx, trd_dEdx, tof_dEdx, opt, m_p, charge);
	if (beta_rich > 0.7) {
		for (int i = 0; i < 4; i++) Betalhd::betalhd_tof_dir[i] = tof_dir[i];
		double rigidity = m_p*Betalhd::BetaToGamma(beta_rich)*beta_rich/charge;
		TrProp prop(tof_pnt[3], tof_dir[3], rigidity);
		Betalhd::ini_RICH(  IsNaF, beta_rich, beta_rich_err, prop, opt, m_p, charge);
	}
	ROOT::Fit::Fitter fitter;
	const int Npar = 1;
	double par0[Npar];
	double vpar[Npar];
	if (beta_rich > 0.7 && beta_rich < 1) {
		if (IsNaF){
			par0[0] = beta_rich*(1+exp(5.36058 - 11.9998*beta_rich)) ;
			vpar[0] = 0.00001;
		}else {
			par0[0] = beta_rich*(1+exp(29.5665 - 38.2354*beta_rich)) ;
			vpar[0] = 0.000001;
		}
		//cout  << "Prior RICH: " << beta_rich << " -> "<< par0[0] << ", " << IsdEdx << IsTOF<< IsRICH<<endl;
	}else if (beta_rich > 0.7) {
		par0[0] = beta_rich;
		vpar[0] = 0.000001;
	}else if (beta < 1) {
		par0[0]= beta * (1 + exp(1.71 - 7.53*beta));
		if (opt%2) par0[0] = beta * (1 + exp(1.82 - 6.54*beta));
		vpar[0] = 0.0001;
	}else {
		par0[0] = beta;
		vpar[0] = 0.0001;
	}
	if (beta_rich > 0.7){
		if ( IsNaF && !(par0[0]>0.75 && par0[0] < 1.1) ) {
			par0[0] = 0.999;
		}else if (!IsNaF && !(par0[0]>0.94 && par0[0] < 1.02)){
			par0[0] = 0.9999;
		}
	}
	if (par0[0] > 2 || par0[0] <0) {
		par0[0] = 1.;
	}
	if (DeBug) cout  << "Prior: " << par0[0] << ", " << IsdEdx << IsTOF<< IsRICH<< endl;
	fitter.Config().SetParamsSettings(Npar, par0);
	fitter.Config().ParSettings(0).SetLimits(0,2);
	if (beta_rich > 0.7){
		if (IsNaF) fitter.Config().ParSettings(0).SetLimits(0.75,1.1);
		else fitter.Config().ParSettings(0).SetLimits(0.94,1.02);
	}
	fitter.Config().MinimizerOptions().SetPrintLevel(0);
	//fitter.Config().SetMinimizer("Minuit2", "Simplex");
	fitter.Config().SetMinimizer("Minuit2", "Scan");
	//fitter.Config().SetMinimizer("Minuit2", "Migrad");
	Betalhd betalhd;
	fitter.FitFCN(Npar, betalhd);
	ROOT::Fit::FitResult result = fitter.Result();
	beta = result.GetParams()[0];
	fitter.Config().ParSettings(0).SetLimits(beta-0.015,beta+0.015);
	if (beta_rich > 0.7){
		if (IsNaF) fitter.Config().ParSettings(0).SetLimits(beta-0.007,beta+0.007);
		else fitter.Config().ParSettings(0).SetLimits(beta-0.0015,beta+0.0015);
	}

	fitter.Config().SetMinimizer("Minuit2", "Scan");
	fitter.FitFCN(Npar, betalhd);
	//fitter.CalculateMinosErrors();
	ROOT::Fit::FitResult result2 = fitter.Result();
	int status = result2.Status();
	//cout << "Edm = " << result2.Edm() << endl;
	beta = result2.GetParams()[0];
	beta_error = result2.GetErrors()[0];
	double minval = result2.MinFcnValue();
	if (DeBug) {
		cout << "best fit = " << beta << ", " << minval<< endl;
		cout << "##################################################"<< endl;
	}
	gErrorIgnoreLevel = err_level;
	return exp(-.5*minval);
}
double Betalhd::GetBetaLikelihood(
		float & beta, float & beta_error ,
		AMSPoint * trk_pnt, AMSPoint * trd_pnt, AMSPoint * tof_pnt, 
		float * Time, float * TimeE, float * PathLength, float * PathLength_trk, 
		float trk_dEdx[2][9], float trd_dEdx[20], float tof_dEdx[4],
		bool IsNaF, vector<float> hit_beta_ini[2], AMSDir * tof_dir,
		int bthpattern, int opt, 
		int charge, float m_p, bool FitInnerRigidity ){
	int err_level = gErrorIgnoreLevel;
	gErrorIgnoreLevel = 5000;
	ROOT::Math::MinimizerOptions::SetDefaultMinimizer("Minuit2");
	Betalhd::IsdEdx = 0;
	Betalhd::IsTOF = 0;
	Betalhd::IsRICH = 0;
	Betalhd::IsRICHhit = 0;
	Betalhd::ini_TOF(  trk_pnt, trd_pnt, tof_pnt, Time,  TimeE, PathLength, PathLength_trk, bthpattern, opt, m_p, charge);
	Betalhd::ini_dEdx(  trk_pnt, trd_pnt, tof_pnt, trk_dEdx, trd_dEdx, tof_dEdx, opt, m_p, charge);
	Betalhd::ini_RICHhit(  IsNaF,  hit_beta_ini, opt, m_p, charge);

	ROOT::Fit::Fitter fitter;
	const int Npar = 1;
	double par0[Npar];
	double vpar[Npar];
	if (IsNaF){
		par0[0] = beta*(1+exp(5.36058 - 11.9998*beta)) ;
		vpar[0] = 0.00001;
	}else {
		par0[0] = beta*(1+exp(29.5665 - 38.2354*beta)) ;
		vpar[0] = 0.000001;
	}
	if ( IsNaF && !(par0[0]>0.75 && par0[0] < 1.1) ) {
		par0[0] = 0.999;
	}else if (!IsNaF && !(par0[0]>0.94 && par0[0] < 1.02)){
		par0[0] = 0.9999;
	}
	if (par0[0] > 2 || par0[0] <0) {
		par0[0] = 1.;
	}
	if (DeBug) cout  << "Prior: " << par0[0] << ", " << IsdEdx << IsTOF<< IsRICHhit<< endl;
	fitter.Config().SetParamsSettings(Npar, par0);
	fitter.Config().ParSettings(0).SetLimits(0,2);
	if (IsNaF) fitter.Config().ParSettings(0).SetLimits(0.75,1.1);
	else fitter.Config().ParSettings(0).SetLimits(0.94,1.02);
	fitter.Config().MinimizerOptions().SetPrintLevel(0);
	//fitter.Config().SetMinimizer("Minuit2", "Simplex");
	fitter.Config().SetMinimizer("Minuit2", "Scan");
	//fitter.Config().SetMinimizer("Minuit2", "Migrad");
	Betalhd betalhd;
	fitter.FitFCN(Npar, betalhd);
	ROOT::Fit::FitResult result = fitter.Result();
	beta = result.GetParams()[0];
	fitter.Config().ParSettings(0).SetLimits(beta-0.015,beta+0.015);
	if (IsNaF) fitter.Config().ParSettings(0).SetLimits(beta-0.007,beta+0.007);
	else fitter.Config().ParSettings(0).SetLimits(beta-0.0015,beta+0.0015);

	fitter.Config().SetMinimizer("Minuit2", "Simplex");
	fitter.FitFCN(Npar, betalhd);
	//fitter.CalculateMinosErrors();
	ROOT::Fit::FitResult result2 = fitter.Result();
	int status = result2.Status();
	//cout << "Edm = " << result2.Edm() << endl;
	beta = result2.GetParams()[0];
	beta_error = result2.GetErrors()[0];
	double minval = result2.MinFcnValue();
	if (DeBug) {
		cout << "best fit = " << beta << ", " << minval<< endl;
		cout << "##################################################"<< endl;
	}
	gErrorIgnoreLevel = err_level;
	return exp(-.5*minval);
}

int Betalhd::ini_TOF( AMSPoint * trk_pnt, AMSPoint * tof_pnt, float * Time, float * TimeE, float * PathLength, float * PathLength_trk, int bthpattern, int opt, double m_p, int charge){
	for (int i = 0 ; i < 10 ; i++) Betalhd::betalhd_trk_pnt[i] = trk_pnt[i];
	if(PathLength_trk==0) memset(Betalhd::betalhd_PathLength_trk,0,sizeof(Betalhd::betalhd_PathLength_trk));
	else for (int i = 0 ; i < 7 ; i++) {
		Betalhd::betalhd_PathLength_trk[i] = PathLength_trk[i];
	}
	for (int i = 0 ; i < 4 ; i++) {
		Betalhd::betalhd_tof_pnt[i] = tof_pnt[i];
		Betalhd::betalhd_Time[i] = Time[i];
		Betalhd::betalhd_TimeE[i] = TimeE[i];
		Betalhd::betalhd_PathLength[i] = PathLength[i];
	}
	Betalhd::betalhd_bthpattern = bthpattern;
	Betalhd::betalhd_opt = opt;
	Betalhd::betalhd_m_p = m_p;
	Betalhd::betalhd_charge = charge;
	Betalhd::IsTOFRICH = 0;
	Betalhd::IsTOF = 1;
	Betalhd::WithTrd = 0;
	Betalhd::betalhd_ndf = 0;
	return 0;
}
int Betalhd::ini_TOF( AMSPoint * trk_pnt, AMSPoint * trd_pnt, AMSPoint * tof_pnt, float * Time, float * TimeE, float * PathLength, float * PathLength_trk, int bthpattern, int opt, double m_p, int charge){
	for (int i = 0 ; i < 20 ; i++) Betalhd::betalhd_trd_pnt[i] = trd_pnt[i];
	for (int i = 0 ; i < 10 ; i++) Betalhd::betalhd_trk_pnt[i] = trk_pnt[i];
	if(PathLength_trk==0) memset(Betalhd::betalhd_PathLength_trk,0,sizeof(Betalhd::betalhd_PathLength_trk));
	else for (int i = 0 ; i < 7 ; i++) {
		Betalhd::betalhd_PathLength_trk[i] = PathLength_trk[i];
	}
	for (int i = 0 ; i < 4 ; i++) {
		Betalhd::betalhd_tof_pnt[i] = tof_pnt[i];
		Betalhd::betalhd_Time[i] = Time[i];
		Betalhd::betalhd_TimeE[i] = TimeE[i];
		Betalhd::betalhd_PathLength[i] = PathLength[i];
	}
	Betalhd::betalhd_bthpattern = bthpattern;
	Betalhd::betalhd_opt = opt;
	Betalhd::betalhd_m_p = m_p;
	Betalhd::betalhd_charge = charge;
	Betalhd::IsTOFRICH = 0;
	Betalhd::IsTOF = 1;
	Betalhd::WithTrd = 1;
	return 0;
}

int Betalhd::ini_dEdx( AMSPoint * trk_pnt, AMSPoint * trd_pnt, AMSPoint * tof_pnt, 
		float trk_dEdx[2][9], float trd_dEdx[20], float tof_dEdx[4],	
		int opt, double m_p, int charge, int subopt){
	double trk_dEdx_d[2][9], trd_dEdx_d[20], tof_dEdx_d[4];
	for (int i = 0 ; i < 20; i++){
		trd_dEdx_d[i] = trd_dEdx[i];
	}
	for (int i = 0 ; i < 4; i++){
		tof_dEdx_d[i] = tof_dEdx[i];
	}
	for (int i = 0 ; i < 9; i++){
		for (int j = 0; j < 2; j++) trk_dEdx_d[j][i] = trk_dEdx[j][i];
	}
	return Betalhd::ini_dEdx( trk_pnt,  trd_pnt,  tof_pnt, 
			trk_dEdx_d, trd_dEdx_d, tof_dEdx_d,	
			opt, m_p, charge, subopt);

}
int Betalhd::ini_dEdx( AMSPoint * trk_pnt, AMSPoint * trd_pnt, AMSPoint * tof_pnt, 
		double trk_dEdx[2][9], double trd_dEdx[20], double tof_dEdx[4],	
		int opt, double m_p, int charge, int subopt){
	for (int i = 0 ; i < 20 ; i++) {
		Betalhd::betalhd_trd_pnt[i] = trd_pnt[i];
		Betalhd::betalhd_trd_dEdx[i] = trd_dEdx[i];
	}
	for (int i = 0 ; i < 10 ; i++) {
		Betalhd::betalhd_trk_pnt[i] = trk_pnt[i];
	}
	for (int i = 0 ; i < 9 ; i++) {
		for (int j = 0; j < 2; j++)Betalhd::betalhd_trk_dEdx[j][i] = trk_dEdx[j][i];
	}
	for (int i = 0 ; i < 4 ; i++) {
		Betalhd::betalhd_tof_pnt[i] = tof_pnt[i];
		Betalhd::betalhd_tof_dEdx[i] = tof_dEdx[i];
	}
	Betalhd::betalhd_opt = opt;
	Betalhd::betalhd_m_p = m_p;
	Betalhd::betalhd_charge = charge;
	Betalhd::IsTOFRICH = 0;
	Betalhd::IsdEdx = 1;
	Betalhd::dEdx_opt = subopt;
	Betalhd::WithTrd = 1;
	Betalhd::betalhd_ndf = 0;
	return 0;
}

AMSPoint Betalhd::betalhd_trd_pnt[20];
AMSPoint Betalhd::betalhd_tof_pnt[4];
AMSPoint Betalhd::betalhd_trk_pnt[10];
AMSDir Betalhd::betalhd_trd_dir[20];
AMSDir Betalhd::betalhd_tof_dir[4];
AMSDir Betalhd::betalhd_trk_dir[10];
float 	Betalhd::betalhd_Time[4];
float	Betalhd::betalhd_TimeE[4];
float Betalhd::betalhd_PathLength[4];
float Betalhd::betalhd_PathLength_trk[7];
int	Betalhd::betalhd_bthpattern;
int Betalhd::betalhd_opt;
int Betalhd::dEdx_opt;
double 	Betalhd::betalhd_trk_dEdx[2][9];
double 	Betalhd::betalhd_trd_dEdx[20];
double 	Betalhd::betalhd_tof_dEdx[4];

double Betalhd::betalhd_m_p;
int Betalhd::betalhd_charge;
bool Betalhd::WithTrd;
bool Betalhd::z2zPropagation;
int Betalhd::ini_RICH( bool IsNaF, float beta_rich_rec, float beta_rich_err, TrProp prop, int opt, double m_p, int charge){
	Betalhd::betalhd_IsNaF = IsNaF;
	Betalhd::betalhd_beta_rich_rec = beta_rich_rec;
	Betalhd::betalhd_beta_rich_err = beta_rich_err;
	Betalhd::betalhd_prop = prop;
	Betalhd::betalhd_opt = opt;
	Betalhd::betalhd_m_p = m_p;
	Betalhd::betalhd_charge = charge;
	Betalhd::IsTOFRICH = 1;
	Betalhd::IsRICH = 1;
	Betalhd::betalhd_ndf = 0;
	return 0;
}
int Betalhd::ini_RICHhit( bool IsNaF, vector<float> hit_beta_ini[2], int opt, double m_p, int charge){
	Betalhd::betalhd_IsNaF = IsNaF;
	hit_beta[0].clear();
	hit_beta[1].clear();
	hit_beta[0] = hit_beta_ini[0];
	hit_beta[1] = hit_beta_ini[1];
	Betalhd::betalhd_opt = opt;
	Betalhd::betalhd_m_p = m_p;
	Betalhd::betalhd_charge = charge;
	Betalhd::IsRICHhit = 1;
	Betalhd::betalhd_ndf = 0;
	Betalhd::rich_usedhits_mirror = 0;
	Betalhd::rich_usedhits_direct = 0;
	return 0;
}
int Betalhd::IsTOFRICH;//0: TOF fit; 1 : RICH fit;
int Betalhd::IsTOF;
int Betalhd::IsdEdx;
int Betalhd::IsRICH;
bool Betalhd::betalhd_IsNaF;
double Betalhd::betalhd_beta_rich_rec;
double Betalhd::betalhd_beta_rich_err;
TrProp Betalhd::betalhd_prop;



double Betalhd::GetBetaLikelihood(float & beta, AMSEventR * ev, TrTrackR * trk, int opt, BetaHR *bth) {
	//0: L1-dEdx  1:L9-dEdx  10:L1-TOF  20:L1-TOF+dEdx
	double m_p = 0.938272;
	double m_d = m_p*2;
	int charge = 1;
	ParticleR *p = ev->pParticle(0);
	if (!trk)  trk = (p) ? p->pTrTrack () : 0;
	TrdTrackR *trd = (p) ? p->pTrdTrack() : 0;
	if (!bth)  bth = (p) ? p->pBetaH()    : 0;

	double trk_dEdx[9];
	double trd_dEdx[20];
	int trd_count[20];
	double tof_dEdx[4];
	double trk_coo[10][3];
	double trd_coo[20][3];
	double tof_coo[4][3];
	double beta_tof = 0;

	for(int i = 0; i< 20; i++){
		trd_dEdx[i] = 0;
		trd_count[i] = 0;
		for (int j = 0; j < 3; j++){
			trd_coo[i][j] = -500;
		}
		if (i < 4) {
			tof_dEdx[i] = 0;
			for (int j = 0; j < 3; j++){
				tof_coo[i][j] = -500;
			}
		}
		if (i < 10) {
			if (i<9) trk_dEdx[i] = 0;
			for (int j = 0; j < 3; j++){
				trk_coo[i][j] = -500;
			}
		}
	}

	AMSPoint trk_pnt[10];
	AMSDir trk_dir[10];
	AMSPoint trd_pnt[20];
	AMSDir trd_dir[20];
	AMSPoint tof_pnt[4];
	AMSDir tof_dir[4];
	double mip_correction[9]= {1.23004, 1.20906, 1.22526, 1.22467, 1.23119, 1.22666, 1.21529, 1.21956, 1.22406};
	if (ev->nMCEventg() == 0)for (int i = 0; i<9; i++) mip_correction[i] = 1;
	int fitcode = trk->iTrTrackPar(2,0,23);
	if (trk){
		for (int ihit = 0; ihit < 9; ihit++) {
			if (trk->TestHitLayerJHasXY(ihit+1) ) {
				trk_dEdx[ihit] = pow(trk->GetLayerJQ(ihit+1), 2)/mip_correction[ihit];
			}
			trk -> InterpolateLayerJ(ihit+1, trk_pnt[ihit], trk_dir[ihit], fitcode);
			trk_pnt[ihit].getp(trk_coo[ihit]);
		}
	}
	if (opt%2==0){//down going	
		trk -> Interpolate(180, trk_pnt[9], trk_dir[9], fitcode);
	}else{
		trk -> Interpolate(-180, trk_pnt[9], trk_dir[9], fitcode);
	}
	trk_pnt[9].getp(trk_coo[9]);
	if (bth && bth->IsTkTofMatch()){
		double tof_layer_z[4] = {65, 62, -62.5, -65.5};
		if (ev->nMCEventgC()) {bth->DoMCtune(); TofRecH::ReBuild(1);}
		beta_tof=bth->GetBeta();
		for (int ilayer=0;ilayer<4;ilayer++) {
			TofClusterHR * tofcluster = bth -> GetClusterHL(ilayer);
			if (tofcluster!=NULL){
				tof_layer_z[ilayer] = tofcluster->Coo[2];
				tof_dEdx[ilayer] = bth->GetQL(ilayer,2,TofClusterHR::DefaultQOptIonW,111111,1,25);
				tof_dEdx[ilayer] *= tof_dEdx[ilayer];
			}
			trk -> Interpolate(tof_layer_z[ilayer], tof_pnt[ilayer], tof_dir[ilayer], fitcode);
			tof_pnt[ilayer].getp(tof_coo[ilayer]);
		}
	}
	if (trk){
		TrdKCluster tk;
		AMSPoint p0;
		AMSDir  dir;
		if (trd!=0) {
			tk = TrdKCluster(ev, trd, trk->GetRigidity());
			p0 = AMSPoint(trd->Coo);
			dir = AMSDir(trd->Theta, trd->Phi);
		}else {
			tk = TrdKCluster(ev, trk, fitcode);
			trk -> Interpolate(115, p0, dir, fitcode);
		}
		for (int j = 0; j < tk.NHits(); j++) {
			TrdKHit *hit = tk.GetHit(j);
			if (!hit) continue;

			double len = hit->Tube_Track_3DLength(&p0, &dir);
			double amp = hit->TRDHit_Amp;
			int itrdlay = hit->TRDHit_Layer;
			if (len > 0.3) {
				if (!trd_count[itrdlay]) {
					trd_dEdx[itrdlay] = amp/len;
					trd_coo[itrdlay][0] = hit->TRDHit_x;
					trd_coo[itrdlay][1] = hit->TRDHit_y;
					trd_coo[itrdlay][2] = hit->TRDHit_z;
					trd_count[itrdlay]++;
				}else{
					trd_dEdx[itrdlay] += amp/len; 
					trd_coo[itrdlay][0] += hit->TRDHit_x;
					trd_coo[itrdlay][1] += hit->TRDHit_y;
					trd_coo[itrdlay][2] += hit->TRDHit_z;
					trd_count[itrdlay]++;
				} 
			}
		}
		double trd_coo_tmp[20][3];
		double trd_dEdx_tmp[20];
		memset(trd_dEdx_tmp, 0, sizeof(trd_dEdx_tmp));
		for (int i = 0; i < 20; i++) {
			trd_dEdx[i] /= trd_count[i];
			trd_dEdx_tmp[19-i] = trd_dEdx[i];
			for (int j = 0; j < 3; j++) {
				trd_coo[i][j] /= trd_count[i];
				trd_coo_tmp[19-i][j] = trd_coo[i][j];
			}
		}
		double trd_coo_tmp2[20][3];
		double coo_z[20]= {
			140.328,
			137.369,
			134.558,
			131.669,
			130.067,
			127.236,
			124.321,
			121.536,
			118.53,
			115.589,
			112.708,
			108.298,
			105.428,
			102.552,
			99.5794,
			96.8924,
			95.4669,
			92.3616,
			89.4792,
			86.5694,
		};
		for (int i = 0; i < 20; i++) {
			trd_dEdx[i] = trd_dEdx_tmp[i];
			for (int j = 0; j < 3; j++) {
				trd_coo[i][j] = trd_coo_tmp[i][j];
			}
			for (int j = 0; j < 2; j++){
				if (trd_coo[i][j] == 0 || fabs(trd_coo[i][j]) > 200) {
					trk -> Interpolate(trd_coo[i][2], p0, dir, fitcode);
					p0.getp(trd_coo_tmp2[i]);
					trd_coo[i][j] = trd_coo_tmp2[i][j];
				}
			}

			trd_pnt[i].setp(trd_coo[i]);
			if (!(trd_coo[i][2]<150 && trd_coo[i][2]>80)) {
				trd_coo[i][2] = coo_z[i];
				trk -> Interpolate(trd_coo[i][2], trd_pnt[i], dir, fitcode);
			}

		}
	}
	if (beta_tof <0) beta_tof *= -1;
	if (!(beta_tof < 1.)) beta_tof = 0.999999999999999;
	double prior = beta_tof * (1 + exp(1.71 - 7.53*beta_tof));
	if (opt%2) prior = beta_tof * (1 + exp(1.82 - 6.54*beta_tof));
	//double prior = beta_tof; 
	double pri[1] = {prior};
	//double sigma_p[1] = {beta_tof*exp(1.71 - 7.53*beta_tof)};
	double sigma_p[1] = {0.02/beta_tof/beta_tof};
	double p_limitleft[1] = {0.1};
	double p_limitright[1] = {0.9999999999999999999};
	double minloglhd = GA(pri, sigma_p, 1, p_limitleft, p_limitright, ev, trk_dEdx, trd_dEdx, tof_dEdx, trk_pnt, trd_pnt, tof_pnt, m_p, 1, opt, bth, trk);
	beta = pri[0];
	return minloglhd;

}
/////////////////////////////////////////////////////////////////////////////////////////////////
int Betalhd::GetVariable_RICH(bool & IsNaF, float & rich_beta, float & rich_beta_err, AMSEventR * ev, RichRingR * richring, int opt){
	if (opt!=0) {
		return 0;
	}
	if (!richring)richring= ev->pParticle(0)->pRichRing();
	rich_beta=richring->getBeta();
	rich_beta_err=richring->getBetaError();
	IsNaF = richring->IsNaF();
	return 0;
}
int Betalhd::GetVariable(AMSEventR * ev, 
		float trk_dEdx[2][9], float trd_dEdx[20], float tof_dEdx[4],
		float trk_coo[10][3], float trd_coo[20][3], float tof_coo[4][3],
		float trk_d[10][2], float trd_d[20][2], float tof_d[4][2],
		float * Time, float * TimeE, float * PathLength, float * PathLength_trk,
		TrTrackR * trk, BetaHR *bth, TrdTrackR * trd ,int opt){
	ParticleR *p = ev->pParticle(0);
	if (!trk)  trk = (p) ? p->pTrTrack () : 0;
	if (!bth)  bth = (p) ? p->pBetaH()    : 0;
	trd = (p) ? p->pTrdTrack() : 0;
	TrdKCluster trdkcluster;
	if (trd!=0) {
		trdkcluster = TrdKCluster(ev, trd, trk->GetRigidity());
	}else if(trk){
		int fitcode =trk->iTrTrackPar(2,0,21);
		trdkcluster = TrdKCluster(ev, trk, fitcode);
	}

	return  Betalhd::GetVariable( ev, 
			trk_dEdx,  trd_dEdx, tof_dEdx,
			trk_coo,  trd_coo,  tof_coo,
			trk_d, trd_d, tof_d,
			Time, TimeE, PathLength, PathLength_trk,
			trdkcluster, 
			trk, bth, opt);
}
int Betalhd::GetVariable(AMSEventR * ev, 
		float trk_dEdx[2][9], float trd_dEdx[20], float tof_dEdx[4],
		float trk_coo[10][3], float trd_coo[20][3], float tof_coo[4][3],
		float trk_d[10][2], float trd_d[20][2], float tof_d[4][2],
		float * Time, float * TimeE, float * PathLength, float * PathLength_trk,
		int & bthpattern,
		TrTrackR * trk, BetaHR *bth, TrdTrackR * trd ,int opt){
	ParticleR *p = ev->pParticle(0);
	if (!trk)  trk = (p) ? p->pTrTrack () : 0;
	if (!bth)  bth = (p) ? p->pBetaH()    : 0;
	if (bth) bthpattern = bth->GetBetaPattern();
	trd = (p) ? p->pTrdTrack() : 0;
	TrdKCluster trdkcluster;
	if (trd!=0) {
		trdkcluster = TrdKCluster(ev, trd, trk->GetRigidity());
	}else if(trk){
		int fitcode =trk->iTrTrackPar(2,0,21);
		trdkcluster = TrdKCluster(ev, trk, fitcode);
	}

	return  Betalhd::GetVariable( ev, 
			trk_dEdx,  trd_dEdx, tof_dEdx,
			trk_coo,  trd_coo,  tof_coo,
			trk_d, trd_d, tof_d,
			Time, TimeE, PathLength, PathLength_trk,
			trdkcluster, 
			trk, bth, opt);
}
int Betalhd::GetVariable(AMSEventR * ev, 
		float trk_dEdx[2][9], float trd_dEdx[20], float tof_dEdx[4],
		float trk_coo[10][3], float trd_coo[20][3], float tof_coo[4][3],
		float trk_d[10][2], float trd_d[20][2], float tof_d[4][2],
		float * Time, float * TimeE, float * PathLength, float * PathLength_trk,
		int & bthpattern,
		TrdKCluster trdkcluster ,
		TrTrackR * trk, BetaHR *bth, int opt){
	ParticleR *p = ev->pParticle(0);
	if (!bth)  bth = (p) ? p->pBetaH()    : 0;
	if (bth) bthpattern = bth->GetBetaPattern();

	return  Betalhd::GetVariable( ev, 
			trk_dEdx,  trd_dEdx, tof_dEdx,
			trk_coo,  trd_coo,  tof_coo,
			trk_d, trd_d, tof_d,
			Time, TimeE, PathLength, PathLength_trk,
			trdkcluster, 
			trk, bth, opt);
}
int Betalhd::GetVariable(AMSEventR * ev, 
		float trk_dEdx[2][9], float trd_dEdx[20], float tof_dEdx[4],
		float trk_coo[10][3], float trd_coo[20][3], float tof_coo[4][3],
		float trk_d[10][2], float trd_d[20][2], float tof_d[4][2],
		float * Time, float * TimeE, float * PathLength, float * PathLength_trk,
		TrdKCluster trdkcluster ,
		TrTrackR * trk, BetaHR *bth, int opt){
	ParticleR *p = ev->pParticle(0);
	if (!trk)  trk = (p) ? p->pTrTrack () : 0;
	if (!bth)  bth = (p) ? p->pBetaH()    : 0;
	AMSPoint p0;
	AMSDir  dir;
	bool hastrd = 1;
	if (trdkcluster.GetValidity()!=-1) trdkcluster.GetTrTrackExtrapolation(p0, dir);
	else if ( trdkcluster.GetTRDRefittedTrack(p0, dir) == 0) {
		TrdTrackR * trd = (p) ? p->pTrdTrack() : 0;
		if (trd!=0) {
			trdkcluster = TrdKCluster(ev, trd, trk->GetRigidity());
			p0 = AMSPoint(trd->Coo);
			dir = AMSDir(trd->Theta, trd->Phi);
		}else if (trk){
			int fitcode =trk->iTrTrackPar(2,0,21);
			trdkcluster = TrdKCluster(ev, trk, fitcode);
			trk -> Interpolate(115, p0, dir, fitcode);
		}else {
			hastrd = 0;
		}
	}

	int trd_count[20];
	double beta_tof = 0;

	for(int i = 0; i< 20; i++){
		trd_dEdx[i] = 0;
		trd_count[i] = 0;
		for (int j = 0; j < 3; j++){
			trd_coo[i][j] = -500;
		}
		if (i < 4) {
			tof_dEdx[i] = 0;
			for (int j = 0; j < 3; j++){
				tof_coo[i][j] = -500;
			}
		}
		if (i < 10) {
			if (i<9) {
				trk_dEdx[0][i] = 0;
				trk_dEdx[1][i] = 0;
			}
			for (int j = 0; j < 3; j++){
				trk_coo[i][j] = -500;
			}
		}
	}

	AMSPoint trk_pnt[10];
	AMSDir trk_dir[10];
	AMSPoint trd_pnt[20];
	AMSDir trd_dir[20];
	AMSPoint tof_pnt[4];
	AMSDir tof_dir[4];
	//double mip_correction[9]= {1.23004, 1.20906, 1.22526, 1.22467, 1.23119, 1.22666, 1.21529, 1.21956, 1.22406};
	double PathLength_trk_tmp;
	//if (ev->nMCEventg() == 0)for (int i = 0; i<9; i++) mip_correction[i] = 1;
	int fitcode =trk->iTrTrackPar(2,0,21);
	if (trk){
		for (int ihit = 0; ihit < 9; ihit++) {
			//if (trk->TestHitLayerJHasXY(ihit+1) ) {
			//	trk_dEdx[ihit] = pow(trk->GetLayerJQ(ihit+1), 2)/mip_correction[ihit];
			//}
			TrRecHitR* hit = trk->GetHitLJ(ihit+1);
			if (hit ==0) {
				trk_dEdx[0][ihit] = 0;
				trk_dEdx[1][ihit] = 0;
			}else{
				if (ev->nMCEventg() && (ev->Head()->Version()>=1106)) {
					trk_dEdx[0][ihit] = hit->GetSignalCombination(0,TrClusterR::kTotSign2017|TrClusterR::kSimAsym|TrClusterR::kSimSignal|TrClusterR::kLoss|TrClusterR::kAngle, 1, 3);
					trk_dEdx[1][ihit] = hit->GetSignalCombination(1,TrClusterR::kTotSign2017|TrClusterR::kSimAsym|TrClusterR::kSimSignal|TrClusterR::kLoss|TrClusterR::kAngle, 1, 3);
				}else {
					trk_dEdx[0][ihit] = hit->GetSignalCombination(0,TrClusterR::kAsym|TrClusterR::kGain|TrClusterR::kLoss|TrClusterR::kMIP|TrClusterR::kAngle, 1, 3);
					trk_dEdx[1][ihit] = hit->GetSignalCombination(1,TrClusterR::kAsym|TrClusterR::kGain|TrClusterR::kLoss|TrClusterR::kMIP|TrClusterR::kAngle, 1, 3);
				}
			}
			//trk_dEdx[ihit] /= mip_correction[ihit];

			PathLength_trk_tmp = trk -> InterpolateLayerJ(ihit+1, trk_pnt[ihit], trk_dir[ihit], fitcode);
			if (ihit>0 and ihit <8){
				PathLength_trk[ihit-1] = PathLength_trk_tmp;
				if (trk_pnt[ihit].z()<0) PathLength_trk[ihit-1] *= -1;
			}
		}
	}
	if (opt%2==0){//down going	
		trk -> Interpolate(180, trk_pnt[9], trk_dir[9], fitcode);
	}else{
		trk -> Interpolate(-180, trk_pnt[9], trk_dir[9], fitcode);
	}
	for (int ihit = 0; ihit < 10 ; ihit++){
		trk_pnt[ihit].getp(trk_coo[ihit]);
		trk_d[ihit][0] = trk_dir[ihit].gettheta();
		trk_d[ihit][1] = trk_dir[ihit].getphi();
	}
	if (bth && bth->IsTkTofMatch()){
		double tof_layer_z[4] = {65, 62, -62.5, -65.5};
		if (ev->nMCEventgC()) {bth->DoMCtune(); TofRecH::ReBuild(1);}
		beta_tof=bth->GetBeta();
		for (int ilayer=0;ilayer<4;ilayer++) {
			TofClusterHR * tofcluster = bth -> GetClusterHL(ilayer);
			if (tofcluster!=NULL){
				tof_layer_z[ilayer] = tofcluster->Coo[2];
				//tof_dEdx[ilayer] = bth->GetQL(ilayer,2,TofClusterHR::DefaultQOptIonW,111111,1,25);
				//tof_dEdx[ilayer] *= tof_dEdx[ilayer];
				tof_dEdx[ilayer] = bth->GetQL(ilayer,2,TofRecH::kThetaCor|TofRecH::kBirkCor|              
						TofRecH::kReAttCor|TofRecH::kDAWeight,
						111111,1,25);

			}
			PathLength[ilayer] = trk -> Interpolate(tof_layer_z[ilayer], tof_pnt[ilayer], tof_dir[ilayer], fitcode);
			if (tof_layer_z[ilayer] < 0) PathLength[ilayer] *= -1;
			tof_pnt[ilayer].getp(tof_coo[ilayer]);
			tof_d[ilayer][0] = tof_dir[ilayer].gettheta();
			tof_d[ilayer][1] = tof_dir[ilayer].getphi();
			Time[ilayer] = bth->GetTime(ilayer);
			TimeE[ilayer] = bth->GetETime(ilayer);

		}
	}
	if (hastrd){
		for (int j = 0; j < trdkcluster.NHits(); j++) {
			TrdKHit *hit = trdkcluster.GetHit(j);
			if (!hit) continue;

			double len = hit->Tube_Track_3DLength(&p0, &dir);
			double amp = hit->TRDHit_Amp;
			int itrdlay = hit->TRDHit_Layer;
			if (len > 0.3) {
				if (!trd_count[itrdlay]) {
					trd_dEdx[itrdlay] = amp/len;
					trd_coo[itrdlay][0] = hit->TRDHit_x;
					trd_coo[itrdlay][1] = hit->TRDHit_y;
					trd_coo[itrdlay][2] = hit->TRDHit_z;
					trd_count[itrdlay]++;
				}else{
					trd_dEdx[itrdlay] += amp/len; 
					trd_coo[itrdlay][0] += hit->TRDHit_x;
					trd_coo[itrdlay][1] += hit->TRDHit_y;
					trd_coo[itrdlay][2] += hit->TRDHit_z;
					trd_count[itrdlay]++;
				} 
			}
		}
		double trd_coo_tmp[20][3];
		double trd_dEdx_tmp[20];
		memset(trd_dEdx_tmp, 0, sizeof(trd_dEdx_tmp));
		for (int i = 0; i < 20; i++) {
			trd_dEdx[i] /= trd_count[i];
			trd_dEdx_tmp[19-i] = trd_dEdx[i];
			for (int j = 0; j < 3; j++) {
				trd_coo[i][j] /= trd_count[i];
				trd_coo_tmp[19-i][j] = trd_coo[i][j];
			}
		}
		double trd_coo_tmp2[20][3];
		double coo_z[20]= {
			140.328,
			137.369,
			134.558,
			131.669,
			130.067,
			127.236,
			124.321,
			121.536,
			118.53,
			115.589,
			112.708,
			108.298,
			105.428,
			102.552,
			99.5794,
			96.8924,
			95.4669,
			92.3616,
			89.4792,
			86.5694,
		};
		for (int i = 0; i < 20; i++) {
			trd_dEdx[i] = trd_dEdx_tmp[i];
			for (int j = 0; j < 3; j++) {
				trd_coo[i][j] = trd_coo_tmp[i][j];
			}
			for (int j = 0; j < 2; j++){
				if (trd_coo[i][j] == 0 || fabs(trd_coo[i][j]) > 200) {
					trk -> Interpolate(trd_coo[i][2], p0, dir, fitcode);
					p0.getp(trd_coo_tmp2[i]);
					trd_coo[i][j] = trd_coo_tmp2[i][j];
					trd_d[i][0] = dir.gettheta();
					trd_d[i][1] = dir.getphi();
				}
			}

			trd_pnt[i].setp(trd_coo[i]);
			if (!(trd_coo[i][2]<150 && trd_coo[i][2]>80)) {
				trd_coo[i][2] = coo_z[i];
				trk -> Interpolate(trd_coo[i][2], trd_pnt[i], dir, fitcode);
				trd_pnt[i].getp(trd_coo[i]);
				trd_d[i][0] = dir.gettheta();
				trd_d[i][1] = dir.getphi();
			}

		}
	}
	if (DeBug){
		//cout << "++++++++++++++++++++++++++++++++++" << endl;
		//cout << "inside GetVariable:" << endl;
		//for (int i = 0; i < 20 ; i++){
		//	cout << "trd_dir[" << i << "]: " << trk_d[i][0] << ", " << trk_d[i][1] << endl;
		//}
		//cout << "++++++++++++++++++++++++++++++++++" << endl;
	}
	return 0;
}
TFile * betalhd_tof_file;
TGraphErrors * gr_betalhd_tof[4];
double Betalhd::GetTOFAmpCorrection(int layer, int ut){
	if (!gr_betalhd_tof[0]){
		TDirectory *dsave = gDirectory;
		TString sfn = AMSDATADIR.amsdatadir; sfn += "LAPP/dEdxPDF/tof_time_correction_v27.root";//the directory has been changed to v6.00
		//betalhd_tof_file = TFile::Open("root://eosams.cern.ch//eos/ams/group/mitep/Ntuple/ForAntiD/dEdxPDF/tof_time_correction_v27.root");
		betalhd_tof_file = TFile::Open(sfn);
		if (dsave) dsave->cd();
		for (int i = 0; i < 4; i++) gr_betalhd_tof[i] = (TGraphErrors*) betalhd_tof_file->Get(Form("gr_tof_time_%d",i));
		//cout << "Opening files: " <<gr_betalhd_tof[0]->Eval(ut) << endl;
	}
	return gr_betalhd_tof[layer]->Eval(ut);
}
void Betalhd::GetMCTrackerAmpCorrection(float trk_dEdx[2][9]){
	double mip_correction[9] = {1.23004, 1.20906, 1.22526, 1.22467, 1.23119, 1.22666,  1.21529, 1.21956, 1.22406};
	double mip_correction_y[9] = {1.20570, 1.18702, 1.15357, 1.18047, 1.15407, 1.15772,1.17397, 1.17258, 1.20287};
	for (int i = 0; i < 9; i++) {
		trk_dEdx[0][i] = trk_dEdx[0][i]/mip_correction[i];
		trk_dEdx[1][i] = trk_dEdx[1][i]/mip_correction_y[i];
	}
}
void Betalhd::GetMCTrackerAmpCorrection(double trk_dEdx[2][9]){
	double mip_correction[9] = {1.23004, 1.20906, 1.22526, 1.22467, 1.23119, 1.22666,  1.21529, 1.21956, 1.22406};
	double mip_correction_y[9] = {1.20570, 1.18702, 1.15357, 1.18047, 1.15407, 1.15772,1.17397, 1.17258, 1.20287};
	for (int i = 0; i < 9; i++) {
		trk_dEdx[0][i] = trk_dEdx[0][i]/mip_correction[i];
		trk_dEdx[1][i] = trk_dEdx[1][i]/mip_correction_y[i];
	}
}

///
double Betalhd::variation1D(double x, double sigma){
	TRandom3 random(0);
	double c=random.Uniform(-sigma,sigma);
	x=x+c;
	return x;
}

int Betalhd::GetChromoRoulette(double *chi2, const unsigned np){
	double Fitness[np];double TotalFitness=0;int TheChosenOne = 0;
	for(int i=0;i<np;i++){
		Fitness[i]=exp(-chi2[i]);
		TotalFitness=TotalFitness+Fitness[i];
	}
	TRandom3 random(0);
	double Slice=random.Uniform(0,TotalFitness);
	double FitnessSoFar = 0;
	for(int i=0;i<np;i++){
		FitnessSoFar += Fitness[i];
		if (FitnessSoFar >= Slice){
			TheChosenOne=i;
			break;
		}
	}
	return  TheChosenOne;
}

double Betalhd::GA_Mode(double*p, const unsigned np){
	double sum = 0;
	for (int i=0; i<np; i++) sum += p[i]*p[i];
	double mode = sqrt(sum);
	return mode;
}
void Betalhd::CreatUnitVectors(double *AnyVector_1D, const unsigned np){
	TRandom3 random(0);
	double eta[np][np];
	for (int i=0; i<np; i++) {
		for (int j=0; j<np; j++){
			eta[i][j] = AnyVector_1D[i*np+j];
		}
	}
	double v[np][np];
	double mode_v[np];
	double v_dot_v[np];
	bool isParallel = 0;
	bool isParallel_tmp = 0;
	for (int i=0; i<np; i++) {
		isParallel = 1;
		while (isParallel){
			for (int j=0; j<np; j++){
				v[i][j] = random.Uniform();
			}
			mode_v[i] = GA_Mode(v[i], np);
			//check if the vector is parallel to the others
			for (int k=0; k<i;k++){
				v_dot_v[i] = 0;
				for (int j=0;j<np;j++)  v_dot_v[i] += v[k][j]*v[i][j];
				if (fabs(v_dot_v[i]-mode_v[k-1]*mode_v[k])<1e-6) {isParallel_tmp = 1; break;}

			}
			if (isParallel_tmp) isParallel = 1;
			else isParallel = 0;
		}
	}

	//Gram-Schmidt process Ref: https://en.wikipedia.org/wiki/Gram-Schmidt_process
	for (int i=0; i<np; i++) {
		for (int j=0; j<np; j++){
			v[i][j] /= mode_v[i];//v_n
		}
	}
	double v_eta_eta[np][np];
	double v_eta[np][np];
	for (int n=0;n<np; n++){
		for (int j=0; j<np; j++) v_eta_eta[n][j] = 0.;
		for (int i=0;i<n-1;i++){
			for (int j=0;j<np;j++){
				// v dot eta
				v_eta[n][i] = v[n][j] * eta[i][j] ;
				// <v dot eta>eta
				v_eta_eta[i][j] += v_eta[n][i]*eta[i][j];
			}
		}
		for (int j=0;j<np;j++) eta[n][j] = v[n][j] - v_eta_eta[n][j];
		double mode_eta = GA_Mode(eta[n], np);
		for (int j=0;j<np;j++) eta[n][j] /= mode_eta;
	}
	for (int i=0; i<np; i++) {
		for (int j=0; j<np; j++){
			AnyVector_1D[i*np+j] = eta[i][j];
		}
	}
}

double Betalhd::GA(double *p, double *sigma_p, const unsigned np, double *p_limitleft, double *p_limitright, AMSEventR* ev,  double * trk_dEdx, double *trd_dEdx, double *tof_dEdx, AMSPoint * trk_pnt, AMSPoint * trd_pnt, AMSPoint * tof_pnt, const double m_p, const int charge, const int opt, BetaHR *bth, TrTrackR *trk){
	//main program
	double chi2[np+1];
	chi2[np]=10000000;
	float chi2f[np];
	double chi2_best = 10000000;
	double p_variation[np];
	double combination[np+1][np];
	double combination_tmp[np+1][np];
	double combination_best[np];
	double AnyVector[np][np];
	double random=1.;
	int np2 = np*np;
	double AnyVector_1D[np2];
	int number;
	//initialization
	double sigma_p_o[np];
	for (int i=0;i<np;i++) combination[np][i] = p[i];//[np][i] is the best chi2 one.
	for (int i=0;i<np;i++) sigma_p[i] /= 2.;
	for (int i=0;i<np;i++) sigma_p_o[i] = sigma_p[i];
	int size = np;
	int idx[np];
	int generation=1000;
	int igeneration=0;
	int multiplicity = 0;
	int max_multiplicity = (int) np*np*50;
	int CheckLimit = 0;//To check if the variables are within the limits.
	while(generation-->0 && multiplicity < max_multiplicity){
		igeneration++;
		Betalhd::CreatUnitVectors(AnyVector_1D, np);
		for (int i=0; i<np; i++) {
			random = Betalhd::variation1D(0, 1.);
			for (int j=0; j<np; j++){
				AnyVector[i][j] = AnyVector_1D[i*np+j];
				combination_tmp[i][j] = random*sigma_p[j]*AnyVector[i][j] + combination[np][j];
			}
			CheckLimit=0;
			while (CheckLimit==0){
				CheckLimit=1;
				for (int j=0; j<np; j++){// check if the variables are within the limits, tag if not.
					if (!(p_limitleft[j] < combination_tmp[i][j] && combination_tmp[i][j]< p_limitright[j])) {
						CheckLimit=0; break;
					}
				}
				if(CheckLimit==0) {
					random = Betalhd::variation1D(0, 1.);
					for (int j=0; j<np; j++) combination_tmp[i][j] = random*sigma_p[j]*AnyVector[i][j] + combination[np][j];
				}else{
					for (int j=0; j<np; j++) combination[i][j] = combination_tmp[i][j];
				}
			}
		}
		for (int i=0;i<np; i++) {
			chi2[i] = Betalhd::getchi2( combination[i],np, ev, trk_dEdx, trd_dEdx, tof_dEdx, trk_pnt, trd_pnt, tof_pnt, m_p, charge, opt, bth, trk);
			chi2f[i] = (float) chi2[i];
		}
		TMath::Sort(size,chi2f,idx);
		if (chi2[idx[np-1]]<chi2_best){
			multiplicity = 0;
			chi2_best=chi2[idx[np-1]];
			for (int i=0; i<np; i++) {
				combination_best[i] = combination[idx[np-1]][i];
				//sigma_p[i] = sigma_p_o[i];
			}
		}else {
			multiplicity++;
			//for (int i=0; i<np; i++) sigma_p[i] *= 0.9285; 
		}

		//number = Betalhd::GetChromoRoulette(chi2,np);
		//for (int i=0;i<np;i++) combination[np][i]=combination[number][i];
		//chi2[np]=chi2[number];
		for (int i=0;i<np;i++) combination[np][i]=combination_best[i];
		chi2[np]=chi2_best;
	}
	for (int i=0;i<np;i++) p[i] = combination_best[i];
	return chi2_best;
}

double Betalhd::getchi2( double *p, int np, AMSEventR *ev, double * trk_dEdx, double *trd_dEdx, double *tof_dEdx, AMSPoint * trk_pnt, AMSPoint * trd_pnt, AMSPoint * tof_pnt, const double m_p, const int charge, const int opt, BetaHR *bth , TrTrackR* trk){//option = 0: Layer 1; 1: Layer 9
	double trk_beta_tmp[9] = {0.};
	double trd_beta_tmp[20]= {0.};
	double tof_beta_tmp[4]= {0.};
	float beta = p[0];
	//	if (!Betalhd::GetMCTrueEnergy( ev, trkmcE, trdmcE, tofmcE, opt)) return 1000;
	//	if (!Betalhd::CalculateExpectedBetaMC(trk_beta_tmp_mc, trd_beta_tmp_mc, tof_beta_tmp_mc, beta, trkmcE, trdmcE, tofmcE, opt)) return 1000;

	int IsTOFPl1 = 0; //0: not TOF pl1, 1: not TOF pl1 MC true template;
	IsTOFPl1 = opt/1000;
	int IsPl1 = 0;// 0: not pl1, 1: true template, 2: TOF estimated template.
	IsPl1 = (opt%1000)/100;
	if (IsTOFPl1 == 0 || IsPl1 == 0)
		if (!Betalhd::CalculateExpectedBeta(trk_beta_tmp, trd_beta_tmp, tof_beta_tmp, beta, trk_pnt, trd_pnt, tof_pnt, m_p, charge, opt)) return 1000;


	double lhd = 1;
	if (((opt%100)/10)==0 || ((opt%100)/10)==2 || ((opt%100)/10)==3 || ((opt%100)/10)==4 ) {
		if (IsPl1 == 0) {
			if (((opt%100)/10) < 3) lhd *= Betalhd::CalculateBetaLikelihood( trk_dEdx, trd_dEdx, tof_dEdx, trk_beta_tmp, trd_beta_tmp, tof_beta_tmp);
			else lhd *= Betalhd::CalculateBetaLikelihood_cal( trk_dEdx, trd_dEdx, tof_dEdx, trk_beta_tmp, trd_beta_tmp, tof_beta_tmp);
		}
		else if (IsPl1 == 1) lhd *= Betalhd::CalculateBetaLikelihood_pl1( trk_dEdx, trd_dEdx, tof_dEdx, beta);
		else if (IsPl1 == 2) lhd *= Betalhd::CalculateBetaLikelihood_cal_pl1( trk_dEdx, trd_dEdx, tof_dEdx, beta);
	}
	if (((opt%100)/10)==1 || ((opt%100)/10)==2 || ((opt%100)/10)==4 ) {
		if (IsTOFPl1 == 0) lhd *= Betalhd::CalculateBetaLikelihood_TOF(trk, bth, trk_pnt, tof_pnt, trk_beta_tmp, tof_beta_tmp, opt%100);
		else if (IsTOFPl1 == 1)lhd *= Betalhd::CalculateBetaLikelihood_TOF_pl1(trk, bth, tof_pnt, beta);
		else lhd *= Betalhd::CalculateBetaLikelihood_TOF_cal_pl1(trk, bth, tof_pnt, beta);
	}
	double nloglhd = -log10(lhd);
	if (lhd == 1.) nloglhd = -1;
	if (nloglhd > 500) nloglhd = 500;
	//if (nloglhd > -1 && nloglhd < 500) cout << beta << ", " << nloglhd << endl;
	return nloglhd;
}
int Betalhd::betalhd_ndf;
double Betalhd::getchi2_offline( const double *par){//option = 0: Layer 1; 1: Layer 9
	double trk_beta_tmp[9] = {0.};
	double trd_beta_tmp[20] = {0.};
	double tof_beta_tmp[4]= {0.};
	double z_rich = -71.87;
	double beta_rich = 0;

	float beta = par[0];
	int IsTOFPl1 = 0; //0: not TOF pl1, 1: not TOF pl1 MC true template;
	IsTOFPl1 = betalhd_opt/1000;
	int IsPl1 = 0;// 0: not pl1, 1: true template, 2: TOF estimated template.
	IsPl1 = (betalhd_opt%1000)/100;
	Betalhd::z2zPropagation = true;
	if (IsTOFPl1 == 0 || IsPl1 == 0){
		if (Betalhd::WithTrd == 0){ 
			if (!Betalhd::CalculateExpectedBeta_TOF(trk_beta_tmp, tof_beta_tmp, beta, betalhd_trk_pnt, betalhd_tof_pnt, betalhd_m_p, betalhd_charge, betalhd_opt)) return 1000;
		}else{ 
			if (!Betalhd::CalculateExpectedBeta(trk_beta_tmp, trd_beta_tmp, tof_beta_tmp, beta, betalhd_trk_pnt, betalhd_trd_pnt, betalhd_tof_pnt, betalhd_m_p, betalhd_charge, betalhd_opt)) return 1000;
		}
	}
	if (Betalhd::IsRICH||Betalhd::IsRICHhit) {
		double rigidity = betalhd_m_p*Betalhd::BetaToGamma(tof_beta_tmp[3])*tof_beta_tmp[3]/betalhd_charge;
		TrProp prop(betalhd_tof_pnt[3], betalhd_tof_dir[3], rigidity);
		Betalhd::betalhd_prop = prop;
		if (!Betalhd::CalculateExpectedBeta_z2z(beta_rich, tof_beta_tmp[3], z_rich, (double) betalhd_tof_pnt[3].z(), betalhd_prop, betalhd_m_p, betalhd_charge, betalhd_opt)) return 1000;
	}
	//beta_rich = beta;
	double lhd = 0; // lhd is chi2
	int ndf = 0;
	if (Betalhd::IsTOF){
		if (betalhd_PathLength[0] == 0) lhd += Betalhd::CalculateBetaLikelihood_offline_TOF(betalhd_Time, betalhd_TimeE, betalhd_PathLength, tof_beta_tmp, betalhd_bthpattern, betalhd_opt%100);
		else lhd += Betalhd::CalculateBetaLikelihood_offline_TOF(betalhd_Time, betalhd_TimeE, betalhd_PathLength, tof_beta_tmp, betalhd_PathLength_trk, trk_beta_tmp, betalhd_bthpattern, betalhd_opt%100);
		if(DeBug_1) cout << "TOF Likelihood: " << lhd << endl;
		if (betalhd_bthpattern == 4444) ndf += 3;
		else ndf += 2;
	}
	if(Betalhd::IsdEdx){
		double lhd_tmp = Betalhd::CalculateBetaLikelihood_offline_dEdx(betalhd_trk_dEdx, betalhd_trd_dEdx, betalhd_tof_dEdx, trk_beta_tmp, trd_beta_tmp, tof_beta_tmp);
		lhd += lhd_tmp;
		for (int i = 0; i < 20 ; i++ ) {
			if (betalhd_trd_dEdx[i]>0 && (dEdx_opt/10)%10){
				ndf ++;
			}
			if (i < 9 && dEdx_opt%10){
				if (betalhd_trk_dEdx[0][i] > 0){
					ndf ++;
				}
				if (betalhd_trk_dEdx[1][i] > 0){
					ndf ++;
				}
			}
			if (i < 4 && betalhd_tof_dEdx[i]>0 && (dEdx_opt/100)%10) {
				ndf ++;
			}
		}

		if(DeBug_1) cout << "dEdx Likelihood: " << lhd_tmp << endl;
	}
	if(Betalhd::IsRICH){
		double lhd_tmp = Betalhd::CalculateBetaLikelihood_offline_RICH(beta_rich, betalhd_opt%100);//return chi2
		lhd += lhd_tmp;
		//if(DeBug) cout << "RICH Likelihood: " << lhd_tmp << ", RICH beta = "<< betalhd_beta_rich_rec << " +/- " << betalhd_beta_rich_err << endl;
		ndf++;
	}
	if(Betalhd::IsRICHhit){
		double lhd_tmp = Betalhd::CalculateBetaLikelihood_offline_RICH_hit(beta_rich);//chi2
		lhd += lhd_tmp;
		//if(DeBug_1) cout << "RICH Likelihood: " << lhd_tmp << ", RICH beta = "<< beta_rich << endl;
		if(DeBug_1) cout << "RICH chi2: " << lhd_tmp << ", RICH beta = "<< beta_rich << endl;
		float sigma;
		for (int i = 0; i < hit_beta[1].size(); i++){
			if (!(hit_beta[0][i]>0) && hit_beta[1][i] > 0 ){
				ndf++;
			} else if (!(hit_beta[1][i] > 0) && hit_beta[0][i] > 0 ){
				ndf++;
			} else	ndf++;
		}
	}
	if (Betalhd::betalhd_FitInnerRigidity){
		//int ndf_trk =1;
		//double lhd_tmp = Betalhd::CalculateBetaLikelihood_rigidity(ndf_trk, betalhd_trk_pnt, betalhd_trd_dir[1], betalhd_trk_dEdx, trk_beta_tmp, betalhd_m_p, betalhd_charge, betalhd_opt%100);
		//lhd *= lhd_tmp;
		//if (DeBug) cout << "Rigidity Likelihood: " << lhd_tmp << endl;
	}
	//if (DeBug_1) cout << "pow(lhd, 1/(ndf-1)) = " << "pow("<<lhd<<", 1/("<<ndf<<"-1)) = " ;
	//lhd = pow(lhd, 1./(ndf-1));
	//if (DeBug_1) cout << lhd << endl;
	//double nloglhd = -2*log(lhd);
	if (DeBug_1) cout << "lhd/(ndf-1) = " << lhd<<"/("<<ndf<<"-1)) = " ;
	betalhd_ndf = ndf - 1;
	if (DeBug_1) cout << "lhd = " << lhd << endl;
	double nloglhd = lhd; // lhd is chi2 here
	if (DeBug) {
		cout << setprecision(20)<<par[0]<<", " << setprecision(20) <<nloglhd << endl;
		//cout << "########################################################" << endl;
	}
	if (lhd == 1.) nloglhd = -1;
	if (nloglhd > 1000) nloglhd = 1000;
	return nloglhd;
}

double Betalhd::getchi2_offline_TOF( const double *par){//option = 0: Layer 1; 1: Layer 9
	double trk_beta_tmp[9] = {0.};
	double trd_beta_tmp[20] = {0.};
	double tof_beta_tmp[4]= {0.};
	float beta = par[0];
	int IsTOFPl1 = 0; //0: not TOF pl1, 1: not TOF pl1 MC true template;
	IsTOFPl1 = betalhd_opt/1000;
	int IsPl1 = 0;// 0: not pl1, 1: true template, 2: TOF estimated template.
	IsPl1 = (betalhd_opt%1000)/100;
	if (IsTOFPl1 == 0 || IsPl1 == 0){
		if (!Betalhd::WithTrd){ 
			if (!Betalhd::CalculateExpectedBeta_TOF(trk_beta_tmp, tof_beta_tmp, beta, betalhd_trk_pnt, betalhd_tof_pnt, betalhd_m_p, betalhd_charge, betalhd_opt)) return 1000;
		}else{ 
			if (!Betalhd::CalculateExpectedBeta(trk_beta_tmp, trd_beta_tmp, tof_beta_tmp, beta, betalhd_trk_pnt, betalhd_trd_pnt, betalhd_tof_pnt, betalhd_m_p, betalhd_charge, betalhd_opt)) return 1000;
		}
	}
	double lhd = 0;// lhd is chi2
	if (betalhd_PathLength[0] == 0) lhd += Betalhd::CalculateBetaLikelihood_offline_TOF(betalhd_Time, betalhd_TimeE, betalhd_PathLength, tof_beta_tmp, betalhd_bthpattern, betalhd_opt%100);
	else lhd += Betalhd::CalculateBetaLikelihood_offline_TOF(betalhd_Time, betalhd_TimeE, betalhd_PathLength, tof_beta_tmp, betalhd_PathLength_trk, trk_beta_tmp, betalhd_bthpattern, betalhd_opt%100);
	double nloglhd = lhd;
	if (lhd == 1.) nloglhd = -1;
	if (nloglhd > 500) nloglhd = 500;
	return nloglhd;
}
double Betalhd::getchi2_offline_RICH( const double *par){//option = 0: Layer 1; 1: Layer 9
	double z_rich = -71.87;
	double beta_rich = 0;
	float beta = par[0];
	int IsTOFPl1 = 0; //0: not TOF pl1, 1: not TOF pl1 MC true template;
	IsTOFPl1 = betalhd_opt/1000;
	int IsPl1 = 0;// 0: not pl1, 1: true template, 2: TOF estimated template.
	IsPl1 = (betalhd_opt%1000)/100;
	if (IsTOFPl1 == 0 || IsPl1 == 0)
		if (!Betalhd::CalculateExpectedBeta_z(beta_rich, beta, z_rich, betalhd_prop, betalhd_m_p, betalhd_charge, betalhd_opt)) return 1000;

	double lhd = 1;
	lhd *= Betalhd::CalculateBetaLikelihood_offline_RICH(beta_rich, betalhd_opt%100);
	double nloglhd = -2*log(lhd);
	if (nloglhd > 500) nloglhd = 500;
	return nloglhd;
}
double Betalhd::getchi2_offline_TOFRICH( const double *par){//option = 0: Layer 1; 1: Layer 9
	double trk_beta_tmp[9] = {0.};
	double tof_beta_tmp[4]= {0.};
	double z_rich = -71.87;
	double beta_rich = 0;
	float beta = par[0];
	int IsTOFPl1 = 0; //0: not TOF pl1, 1: not TOF pl1 MC true template;
	IsTOFPl1 = betalhd_opt/1000;
	int IsPl1 = 0;// 0: not pl1, 1: true template, 2: TOF estimated template.
	IsPl1 = (betalhd_opt%1000)/100;
	if (IsTOFPl1 == 0 || IsPl1 == 0){
		if (!Betalhd::CalculateExpectedBeta_z(beta_rich, beta, z_rich, betalhd_prop, betalhd_m_p, betalhd_charge, betalhd_opt)) return 1000;
		if (!Betalhd::CalculateExpectedBeta_TOF(trk_beta_tmp, tof_beta_tmp, beta, betalhd_trk_pnt, betalhd_tof_pnt, betalhd_m_p, betalhd_charge, betalhd_opt)) return 1000;
	}
	double lhd = 0;//lhd is chi2
	int ndf = 0;
	if (Betalhd::IsTOFRICH == 2) {
		lhd += Betalhd::CalculateBetaLikelihood_offline_RICH(beta_rich, betalhd_opt%100);
		ndf ++;
	}
	lhd += Betalhd::CalculateBetaLikelihood_offline_TOF(betalhd_Time, betalhd_TimeE, betalhd_PathLength, tof_beta_tmp, betalhd_PathLength_trk, trk_beta_tmp, betalhd_bthpattern, betalhd_opt%100);
	if (betalhd_bthpattern == 4444) ndf += 3;
	else ndf += 2;

	double nloglhd = lhd;
	if (nloglhd > 500) nloglhd = 500;
	return nloglhd;
}

TrProp Betalhd::CopyTrProp(TrProp trprop, int opt){
	double z = trprop.GetP0z();
	double p[5] = {trprop.GetP0x(), trprop.GetP0y(), trprop.GetTheta(), trprop.GetPhi(),trprop.GetRigidity() };
	double delta_p[5] = {1e-4, 1e-4 , 0.1, 0.1, trprop.GetRigidity()*0.01 };
	p[opt] += delta_p[opt]; 
	TrProp trprop_n(p[0], p[1], z, p[2], p[3], p[4]);
	return trprop_n;
}

bool Betalhd::TrPropagation(TrProp trprop_o, double z1, double J[5][5]){
	TrProp trprop = CopyTrProp(trprop_o);
	trprop.Propagate(z1);

	TrProp trprop_n[5];
	double Dp_0[5] = {trprop_o.GetP0x(), trprop_o.GetP0y(), trprop_o.GetTheta(), trprop_o.GetPhi(),trprop_o.GetRigidity() };
	double delta_p[5] = {1e-4, 1e-4 , 0.1, 0.1, trprop_o.GetRigidity()*0.01 };
	for (int i = 0; i < 5 ; i++) {
		Dp_0[i] += delta_p[i];
		trprop_n[i] = CopyTrProp(trprop_o,i);//
		trprop_n[i].Propagate(z1);
	}
	double Dp_1[5] = {
		trprop_n[0].GetP0x()-trprop.GetP0x(), 
		trprop_n[1].GetP0y()-trprop.GetP0y(), 
		trprop_n[2].GetTheta()-trprop.GetTheta(), 
		trprop_n[3].GetPhi()-trprop.GetPhi(),
		trprop_n[4].GetRigidity()-trprop.GetRigidity()
	};
	for (int i = 0 ; i < 5 ; i++){
		for (int j = 0 ; j < 5 ; j++){
			if (Dp_0[j] == 0)  return false;
			else J[i][j] = Dp_1[i]/Dp_0[j];
		}
	}

	trprop_o.Propagate(z1);

	return true;
}


double Betalhd::BetaToGamma(double beta) {
	if (fabs(beta) < 1) return 1/sqrt(1-beta*beta);
	else return 100000;
}
double Betalhd::GammaToBeta(double gamma) {return sqrt(1-1/gamma/gamma);}

/////////////////////////////////////////////////////////////////////////////////////////////////
TFile * pdffile;
TH2D * h_trk_pdf;
TH2D * h_trky_pdf;
TH2D * h_trd_pdf;
TH2D * h_tof_pdf;
bool Betalhd::IsMC = false;
double Betalhd::InterpolateGrid(double x, double x_0, double x_1, double y_0, double y_1){
	double y;
	if (x_0 == x ) y = y_0;
	else y = y_0 + (y_1 - y_0)/(x_1 - x_0)*(x - x_0);
	return y;
}
double Betalhd::EvalPDF(TH2D* h, double betagamma, double dEdx){
	int nbinx = h->GetXaxis() -> GetNbins();
	int nbiny = h->GetYaxis() -> GetNbins();
	double c, c_n, c_c_n, c_c_n_0, c_c_n_1, x_0, x_1;
	double betagamma_0, betagamma_1, dEdx_0, dEdx_1;//neighbouring bins
	int binmax;
	int binx[2] ;//neighbouring bins
	int biny[2] ;//neighbouring bins
	double c_nei[2][2];//neighbouring pdfs
	int ibinx = h->GetXaxis() -> FindBin(betagamma);
	int ibiny = h->GetYaxis() -> FindBin(dEdx);
	if (ibinx > nbinx) ibinx = nbinx;
	if (ibiny > nbiny) ibiny = nbiny;
	if (ibinx < 0) ibinx = 0;
	if (ibiny < 0) ibiny = 0;
	if (h->GetXaxis()->GetBinCenterLog(ibinx) <= betagamma){
		binx[0] = ibinx;
		binx[1] = ibinx+1;
	}else{
		binx[0] = ibinx-1;
		binx[1] = ibinx;
	}
	x_0 = h->GetXaxis()->GetBinCenterLog(binx[0]);
	x_1 = h->GetXaxis()->GetBinCenterLog(binx[1]);
	if (h->GetYaxis()->GetBinCenterLog(ibiny) <= dEdx){
		biny[0] = ibiny;
		biny[1] = ibiny+1;
	}else{
		biny[0] = ibiny-1;
		biny[1] = ibiny;
	}
	for (int i = 0 ; i < 2 ; i++){
		for (int j = 0 ; j < 2 ; j++) {
			//c_nei[i][j] = h->GetBinContent(binx[i], biny[j]);
			c_nei[i][j] = h->GetBinContent(binx[i], biny[j])/h->ProjectionY("_py", binx[i], binx[i])->Integral();
		}
	}
	double x = (betagamma - h->GetXaxis()->GetBinCenterLog(binx[0]))/(h->GetXaxis()->GetBinCenterLog(binx[1])- h->GetXaxis()->GetBinCenterLog(binx[0]));
	double y = (dEdx - h->GetYaxis()->GetBinCenterLog(biny[0]))/(h->GetYaxis()->GetBinCenterLog(biny[1])- h->GetYaxis()->GetBinCenterLog(biny[0]));

	c_c_n = c_nei[0][0]*(1-x)*(1-y)
		+c_nei[1][0]*x*(1-y)
		+c_nei[0][1]*(1-x)*y
		+c_nei[1][1]*x*y;

	/*	c = InterpolateGrid(dEdx, h->GetYaxis()->GetBinCenterLog(biny[0]),h->GetYaxis()->GetBinCenterLog(biny[1]), c_nei[0][0], c_nei[0][1]);
	//binmax = h->ProjectionY("_py", binx[0], binx[0])->GetMaximumBin();
	binmax = h->ProjectionY("_py", binx[0], binx[0])->Integral();
	c_n =  h->ProjectionY("_py", binx[0], binx[0])->GetBinContent(binmax);
	c_c_n_0 = c/c_n;

	c = InterpolateGrid(dEdx, h->GetYaxis()->GetBinCenterLog(biny[0]),h->GetYaxis()->GetBinCenterLog(biny[1]), c_nei[1][0], c_nei[1][1]);
	//binmax = h->ProjectionY("_py", binx[1], binx[1])->GetMaximumBin();
	binmax = h->ProjectionY("_py", binx[1], binx[1])->Integral();
	c_n =  h->ProjectionY("_py", binx[1], binx[1])->GetBinContent(binmax);
	c_c_n_1 = c/c_n;

	c_c_n =  InterpolateGrid(betagamma, x_0, x_1, c_c_n_0, c_c_n_1);
	*/
	return c_c_n;
}
double Betalhd::GetLikelihood(double beta, double dEdx, int detector){
	if (detector > 2) return 1e-4;
	if (!h_trk_pdf) {
		TDirectory *dsave = gDirectory;
		/*
		 * TString stn;
		 if (Betalhd::IsMC) {
		 stn = "root://eosams.cern.ch//eos/ams/group/mitep/Ntuple/ForAntiD/dEdxPDF/beta_norm.root";
		 }else {
		 stn = "root://eosams.cern.ch//eos/ams/group/mitep/Ntuple/ForAntiD/dEdxPDF/beta_norm_iss.root";
		 }
		 */
		TString stn = AMSDATADIR.amsdatadir; stn += "LAPP/dEdxPDF/";
		if (Betalhd::IsMC) {
			stn += "beta_norm.root";
		}else{
			stn += "beta_norm_iss.root";
		}

		pdffile = TFile::Open(stn);
		if (dsave) dsave->cd();
		if (pdffile) {
			cout << "Betalhd::GetLikelihood-I-Open file: "
				<< pdffile->GetName() << endl;
			h_trd_pdf = (TH2D*) pdffile->Get("h_trd_dEdx_0");
			h_trk_pdf = (TH2D*) pdffile->Get("h_tracker_dEdx_0");
			h_trky_pdf = (TH2D*) pdffile->Get("h_tracker_dEdx_y_0");
			h_tof_pdf = (TH2D*) pdffile->Get("h_tof_dEdx_0");
		}
	}
	if (!h_trd_pdf) return 1e-100;
	double lhd = 1;
	double betagamma;
	if (fabs(beta) < 1) betagamma = beta*BetaToGamma(beta);
	else betagamma = 29.9999999999999999;
	if (betagamma < 0.2) betagamma = 0.2;
	double c  = 0;
	double c_n = 1;
	double c_c_n, c_c_n_0, c_c_n_1, x_1, x_0;
	if (detector == -1) {
		c_c_n = EvalPDF(h_trky_pdf, betagamma, dEdx);
	}
	if (detector == 0) {
		c_c_n = EvalPDF(h_trk_pdf, betagamma, dEdx);
	}
	if (detector == 1) {
		c_c_n = EvalPDF(h_trd_pdf, betagamma, dEdx);
	}
	if (detector == 2) {
		int ibinx = h_tof_pdf->GetXaxis() -> FindBin(betagamma);
		if(ibinx < 125) c_c_n = 1e-4;
		else c_c_n = EvalPDF(h_tof_pdf, betagamma, dEdx);
	}
	if (!(c_c_n > 1e-4)) c_c_n = 1e-4;

	if (DeBug_1)cout << "detector["<<detector<<"] : " << "beta = "<<beta << ", betagamma = " << betagamma <<", dEdx = " << dEdx<<" >> " << c_c_n << endl;

	return c_c_n;
}
TFile * pdffile_cal;
TH2D * h_trk_pdf_cal;
TH2D * h_trky_pdf_cal;
TH2D * h_trd_pdf_cal;
TH2D * h_tof_pdf_cal;
double Betalhd::GetLikelihood_cal(double beta, double dEdx, int detector){
	if (detector > 2) return 1e-8;
	if (!h_trk_pdf_cal) {
		TDirectory *dsave = gDirectory;
		//pdffile_cal = TFile::Open("root://eosams.cern.ch//eos/ams/user/j/jfeng/dEdxPDF/beta_norm_cal.root");
		//pdffile_cal = TFile::Open("root://eosams.cern.ch//eos/ams/group/mitep/Ntuple/ForAntiD/dEdxPDF/beta_norm_cal.root");

		TString stn = AMSDATADIR.amsdatadir; stn += "LAPP/dEdxPDF/beta_norm_cal.root";
		pdffile_cal = TFile::Open(stn);

		if (dsave) dsave->cd();
		h_trd_pdf_cal = (TH2D*) pdffile_cal->Get("h_trd_dEdx_cal_0");
		h_trk_pdf_cal = (TH2D*) pdffile_cal->Get("h_tracker_dEdx_cal_0");
		h_trky_pdf_cal = (TH2D*) pdffile_cal->Get("h_tracker_dEdx_y_cal_0");
		h_tof_pdf_cal = (TH2D*) pdffile_cal->Get("h_tof_dEdx_cal_0");
	}
	double lhd = 1;
	double betagamma = beta*BetaToGamma(beta);
	double c  = 0;
	if (detector == -1) {
		int ibin = h_trky_pdf_cal->FindBin(betagamma, dEdx);
		bool isUnderOver = (h_trky_pdf_cal->IsBinOverflow(ibin) || h_trky_pdf_cal->IsBinUnderflow(ibin));
		if ( !isUnderOver)
			c = h_trky_pdf_cal->GetBinContent(ibin);
		else c = 1e-8;
		if (c < 1e-8) c = 1e-8;
	}
	if (detector == 0) {
		int ibin = h_trk_pdf_cal->FindBin(betagamma, dEdx);
		bool isUnderOver = (h_trk_pdf_cal->IsBinOverflow(ibin) || h_trk_pdf_cal->IsBinUnderflow(ibin));
		if ( !isUnderOver)
			c = h_trk_pdf_cal->GetBinContent(ibin);
		else c = 1e-8;
		if (c < 1e-8) c = 1e-8;
	}
	if (detector == 1) {
		int ibin = h_trd_pdf_cal->FindBin(betagamma, dEdx);
		bool isUnderOver = (h_trd_pdf_cal->IsBinOverflow(ibin) || h_trd_pdf_cal->IsBinUnderflow(ibin));
		if ( !isUnderOver)
			c = h_trd_pdf_cal->GetBinContent(ibin);
		else c = 1e-8;
		if (c < 1e-8) c = 1e-8;
	}
	if (detector == 2) {
		int ibin = h_tof_pdf_cal->FindBin(betagamma, dEdx);
		bool isUnderOver = (h_tof_pdf_cal->IsBinOverflow(ibin) || h_tof_pdf_cal->IsBinUnderflow(ibin));
		if ( !isUnderOver)
			c = h_tof_pdf_cal->GetBinContent(ibin);
		else c = 1e-8;
		if (c < 1e-8) c = 1e-8;
	}
	return c;
}
/////////////////////////////////////////////////////////////////////////////////////////////////
TFile * pdffile_pl1;
TH2D * h_trk_pl1_pdf[9];
TH2D * h_trd_pl1_pdf[20];
TH2D * h_tof_pl1_pdf[4];
double Betalhd::GetLikelihood_pl1(double beta, double dEdx, int detector, int ilayer){
	if (detector > 2) return 1e-8;
	if (!h_trk_pl1_pdf[0]) {
		TDirectory *dsave = gDirectory;
		TString stn = AMSDATADIR.amsdatadir; stn += "LAPP/dEdxPDF/beta_norm_pl1_extend.root";
		pdffile_pl1 = TFile::Open(stn);
		//pdffile_pl1 = TFile::Open("root://eosams.cern.ch//eos/ams/user/j/jfeng/dEdxPDF/beta_norm_pl1_extend.root");
		//pdffile_pl1 = TFile::Open("root://eosams.cern.ch//eos/ams/group/mitep/Ntuple/ForAntiD/dEdxPDF/beta_norm_pl1_extend.root");
		if (dsave) dsave->cd();
		for (int i = 0; i < 20; i++){
			h_trd_pl1_pdf[i] = (TH2D*) pdffile_pl1->Get(Form("h_trd_dEdx_pl1_%d" , i));
			if (i < 9) h_trk_pl1_pdf[i] = (TH2D*) pdffile_pl1->Get(Form("h_tracker_dEdx_pl1_%d", i));
			if (i < 4) h_tof_pl1_pdf[i] = (TH2D*) pdffile_pl1->Get(Form("h_tof_dEdx_pl1_%d", i));
		}
	}
	double lhd = 1;
	double betagamma = beta*BetaToGamma(beta);
	double c  = 0;
	if (detector == 0 && ilayer < 9) {
		int ibin = h_trk_pl1_pdf[ilayer]->FindBin(betagamma, dEdx);
		bool isUnderOver = (h_trk_pl1_pdf[ilayer]->IsBinOverflow(ibin) || h_trk_pl1_pdf[ilayer]->IsBinUnderflow(ibin));
		if ( !isUnderOver)
			c = h_trk_pl1_pdf[ilayer]->GetBinContent(ibin);
		else c = 1e-8;
		if (c < 1e-8) c = 1e-8;
	}
	if (detector == 1 && ilayer < 20) {
		int ibin = h_trd_pl1_pdf[ilayer]->FindBin(betagamma, dEdx);
		bool isUnderOver = (h_trd_pl1_pdf[ilayer]->IsBinOverflow(ibin) || h_trd_pl1_pdf[ilayer]->IsBinUnderflow(ibin));
		if ( !isUnderOver)
			c = h_trd_pl1_pdf[ilayer]->GetBinContent(ibin);
		else c = 1e-8;
		if (c < 1e-8) c = 1e-8;
	}
	if (detector == 2 && ilayer < 4) {
		int ibin = h_tof_pl1_pdf[ilayer]->FindBin(betagamma, dEdx);
		bool isUnderOver = (h_tof_pl1_pdf[ilayer]->IsBinOverflow(ibin) || h_tof_pl1_pdf[ilayer]->IsBinUnderflow(ibin));
		if ( !isUnderOver)
			c = h_tof_pl1_pdf[ilayer]->GetBinContent(ibin);
		else c = 1e-8;
		if (c < 1e-8) c = 1e-8;
	}
	return c;
}
TH2D * h_trk_cal_pl1_pdf[9];
TH2D * h_trd_cal_pl1_pdf[20];
TH2D * h_tof_cal_pl1_pdf[4];
double Betalhd::GetLikelihood_cal_pl1(double beta, double dEdx, int detector, int ilayer){
	if (detector > 2) return 1e-8;
	if (!h_trk_cal_pl1_pdf[0]) {
		TDirectory *dsave = gDirectory;
		TString stn = AMSDATADIR.amsdatadir; stn += "LAPP/dEdxPDF/beta_norm_pl1_extend.root";
		pdffile_pl1 = TFile::Open(stn);

		//pdffile_pl1 = TFile::Open("root://eosams.cern.ch//eos/ams/user/j/jfeng/dEdxPDF/beta_norm_pl1_extend.root");
		//pdffile_pl1 = TFile::Open("root://eosams.cern.ch//eos/ams/group/mitep/Ntuple/ForAntiD/dEdxPDF/beta_norm_pl1_extend.root");
		if (dsave) dsave->cd();
		for (int i = 0; i < 20; i++){
			h_trd_cal_pl1_pdf[i] = (TH2D*) pdffile_pl1->Get(Form("h_trd_dEdx_cal_pl1_%d" , i));
			if (i < 9) h_trk_cal_pl1_pdf[i] = (TH2D*) pdffile_pl1->Get(Form("h_tracker_dEdx_cal_pl1_%d", i));
			if (i < 4) h_tof_cal_pl1_pdf[i] = (TH2D*) pdffile_pl1->Get(Form("h_tof_dEdx_cal_pl1_%d", i));
		}
	}
	double lhd = 1;
	double betagamma = beta*BetaToGamma(beta);
	double c  = 0;
	if (detector == 0 && ilayer < 9) {
		int ibin = h_trk_cal_pl1_pdf[ilayer]->FindBin(betagamma, dEdx);
		bool isUnderOver = (h_trk_cal_pl1_pdf[ilayer]->IsBinOverflow(ibin) || h_trk_cal_pl1_pdf[ilayer]->IsBinUnderflow(ibin));
		if ( !isUnderOver)
			c = h_trk_cal_pl1_pdf[ilayer]->GetBinContent(ibin);
		else c = 1e-8;
		if (c < 1e-8) c = 1e-8;
	}
	if (detector == 1 && ilayer < 20) {
		int ibin = h_trd_cal_pl1_pdf[ilayer]->FindBin(betagamma, dEdx);
		bool isUnderOver = (h_trd_cal_pl1_pdf[ilayer]->IsBinOverflow(ibin) || h_trd_cal_pl1_pdf[ilayer]->IsBinUnderflow(ibin));
		if ( !isUnderOver)
			c = h_trd_cal_pl1_pdf[ilayer]->GetBinContent(ibin);
		else c = 1e-8;
		if (c < 1e-8) c = 1e-8;
	}
	if (detector == 2 && ilayer < 4) {
		int ibin = h_tof_cal_pl1_pdf[ilayer]->FindBin(betagamma, dEdx);
		bool isUnderOver = (h_tof_cal_pl1_pdf[ilayer]->IsBinOverflow(ibin) || h_tof_cal_pl1_pdf[ilayer]->IsBinUnderflow(ibin));
		if ( !isUnderOver)
			c = h_tof_cal_pl1_pdf[ilayer]->GetBinContent(ibin);
		else c = 1e-8;
		if (c < 1e-8) c = 1e-8;
	}
	return c;
}

double Betalhd::CalculateBetaLikelihood(double *trk_dEdx, double * trd_dEdx, double *tof_dEdx, double * trk_beta_tmp, double *trd_beta_tmp, double *tof_beta_tmp){
	double lhd = 1;
	for (int i = 0; i < 20 ; i++ ) {
		if (trd_dEdx[i]>0)lhd *= GetLikelihood(trd_beta_tmp[i], trd_dEdx[i], 1);
		if (i < 9 && trk_dEdx[i]>0) {
			lhd *= GetLikelihood(trk_beta_tmp[i], trk_dEdx[i], 0);
		}
		if (i < 4 && tof_dEdx[i]>0) {
			lhd *= GetLikelihood(tof_beta_tmp[i], tof_dEdx[i], 2);
		}
	}
	return lhd;
}
double Betalhd::CalculateBetaLikelihood_cal(double *trk_dEdx, double * trd_dEdx, double *tof_dEdx, double * trk_beta_tmp, double *trd_beta_tmp, double *tof_beta_tmp){
	double lhd = 1;
	for (int i = 0; i < 20 ; i++ ) {
		if (trd_dEdx[i]>0)lhd *= GetLikelihood_cal(trd_beta_tmp[i], trd_dEdx[i], 1);
		if (i < 9 && trk_dEdx[i]>0) {
			lhd *= GetLikelihood_cal(trk_beta_tmp[i], trk_dEdx[i], 0);
		}
		if (i < 4 && tof_dEdx[i]>0) {
			lhd *= GetLikelihood_cal(tof_beta_tmp[i], tof_dEdx[i], 2);
		}
	}
	return lhd;
}
double Betalhd::CalculateBetaLikelihood_pl1(double *trk_dEdx, double * trd_dEdx, double *tof_dEdx, double beta){
	double lhd = 1;
	for (int i = 0; i < 20 ; i++ ) {
		if (trd_dEdx[i]>0){
			lhd *= GetLikelihood_pl1(beta, trd_dEdx[i], 1, i);
			//cout << "trd layer " << i << " :" << beta*Betalhd::BetaToGamma(beta) << " , "  << trd_dEdx[i] << " , " << GetLikelihood_pl1(beta, trd_dEdx[i], 1, i) << endl;
		}
		if (i < 9 && trk_dEdx[i]>0) {
			lhd *= GetLikelihood_pl1(beta, trk_dEdx[i], 0, i);
		}
		if (i < 4 && tof_dEdx[i]>0) {
			lhd *= GetLikelihood_pl1(beta, tof_dEdx[i], 2, i);
		}
	}
	return lhd;

}
double Betalhd::CalculateBetaLikelihood_cal_pl1(double *trk_dEdx, double * trd_dEdx, double *tof_dEdx, double beta){
	double lhd = 1;
	for (int i = 0; i < 20 ; i++ ) {
		if (trd_dEdx[i]>0)lhd *= GetLikelihood_cal_pl1(beta, trd_dEdx[i], 1, i);
		if (i < 9 && trk_dEdx[i]>0) {
			lhd *= GetLikelihood_cal_pl1(beta, trk_dEdx[i], 0, i);
		}
		if (i < 4 && tof_dEdx[i]>0) {
			lhd *= GetLikelihood_cal_pl1(beta, tof_dEdx[i], 2, i);
		}

	}
	return lhd;

}

double Betalhd::TOF4Layersfcn(Double_t *x, Double_t *par){
	//0-3: pathlength/(cval*beta);
	//4-7: expected time
	//8:   normalization
	int n = 4;
	double x_gr[4] = {par[0], par[1], par[2], par[3]};
	double y[4] = {par[4], par[5], par[6], par[7]};
	TGraph gr_tmp(n, x_gr,y);
	double val = gr_tmp.Eval(x[0]);
	return par[8]+val;
}
double Betalhd::CalculateBetaLikelihood_TOF(TrTrackR * trk, BetaHR * bth, AMSPoint * trk_pnt, AMSPoint * tof_pnt, double * trk_beta_tmp, double * tof_beta_tmp, int opt){
	double lhd = 1.;
	double cval = 29.9792458; // cm/ns
	AMSPoint point_tmp;
	AMSDir dir_tmp;
	int fitcode =trk->iTrTrackPar(2,0,23);

	double time[4], timeE[4];
	double time_exp[4] = {0., 0.,0.,0.};
	int bthpattern = bth->GetBetaPattern();
	for (int i = 0; i < 4; i++){
		int test = ((bthpattern/((int) pow(10.,3-i)))%10 == 4);
		if (test){
			time[i] = (double)bth->GetTime(i);
			timeE[i] = (double)bth->GetETime(i);
		}else{
			time[i] = 0;
			timeE[i] = 0;
		}
	}
	double pathlength[4] = {0.,0.,0.,0.};
	double pathlength_trk[7];
	memset(pathlength_trk, 0, sizeof(pathlength_trk));
	if (opt%2==0){
		time_exp[0] = time[0];

		pathlength[0] = -trk-> Interpolate(tof_pnt[0].z(), point_tmp, dir_tmp, fitcode)/cval;//TOF + inner tracker
		pathlength[1] = -trk-> Interpolate(tof_pnt[1].z(), point_tmp, dir_tmp, fitcode)/cval;
		double time_exp_trk[7];
		memset(time_exp_trk, 0, sizeof(time_exp_trk));
		time_exp[1] = 1./tof_beta_tmp[0]*(pathlength[1]-pathlength[0]) + time_exp[0];

		pathlength_trk[0] = -trk-> Interpolate(trk_pnt[1].z(), point_tmp, dir_tmp, fitcode)/cval;
		time_exp_trk[0] = 1./tof_beta_tmp[1]*(pathlength_trk[0]-pathlength[1]) + time_exp[1];
		for (int i = 2; i < 8; i++) {
			pathlength_trk[i-1] = -trk-> Interpolate(trk_pnt[i].z(), point_tmp, dir_tmp, fitcode)/cval;
			if (trk_pnt[i].z() < 0) pathlength_trk[i-1] *= -1;
			time_exp_trk[i-1] = 1./trk_beta_tmp[i-1]*(pathlength_trk[i-1]-pathlength_trk[i-2]) + time_exp_trk[i-2];
		}
		pathlength[2] = trk-> Interpolate(tof_pnt[2].z(), point_tmp, dir_tmp, fitcode)/cval;
		time_exp[2] = 1./trk_beta_tmp[6]*(pathlength[2]-pathlength_trk[6]) + time_exp_trk[6];

		pathlength[3] = trk-> Interpolate(tof_pnt[3].z(), point_tmp, dir_tmp, fitcode)/cval;
		time_exp[3] = 1./tof_beta_tmp[2]*(pathlength[3]-pathlength[2]) + time_exp[2];
	}else{
		time_exp[3] = time[3];

		pathlength[3] = trk-> Interpolate(tof_pnt[3].z(), point_tmp, dir_tmp, fitcode)/cval;//TOF + inner tracker
		pathlength[2] = trk-> Interpolate(tof_pnt[2].z(), point_tmp, dir_tmp, fitcode)/cval;
		double time_exp_trk[7];
		memset(time_exp_trk, 0, sizeof(time_exp_trk));
		time_exp[2] = 1./tof_beta_tmp[3]*(pathlength[3] - pathlength[2]) + time_exp[3];

		pathlength_trk[6] = trk-> Interpolate(trk_pnt[7].z(), point_tmp, dir_tmp, fitcode)/cval;
		time_exp_trk[6] = 1./tof_beta_tmp[2]*(pathlength[2] - pathlength_trk[6]) + time_exp[2];
		for (int i = 6; i > 0; i--) {
			pathlength_trk[i-1] = trk-> Interpolate(trk_pnt[i].z(), point_tmp, dir_tmp, fitcode)/cval;
			if (trk_pnt[i].z() > 0) pathlength_trk[i-1] *= -1;
			time_exp_trk[i-1] = 1./trk_beta_tmp[i+1]*(pathlength_trk[i] - pathlength_trk[i-1]) + time_exp_trk[i];
		}
		pathlength[1] = -trk-> Interpolate(tof_pnt[1].z(), point_tmp, dir_tmp, fitcode)/cval;
		time_exp[1] = 1./trk_beta_tmp[1]*(pathlength_trk[0] - pathlength[1]) + time_exp_trk[0];

		pathlength[0] = -trk-> Interpolate(tof_pnt[0].z(), point_tmp, dir_tmp, fitcode)/cval;
		time_exp[0] = 1./tof_beta_tmp[1]*(pathlength[1] - pathlength[0]) + time_exp[1];
	}
	TF1 * fit_tof = new TF1("fit_tof",TOF4Layersfcn, -6, 6, 9);
	for (int i = 0; i< 4 ; i++){
		fit_tof -> FixParameter(i, pathlength[i]);
		fit_tof -> FixParameter(i+4, time_exp[i]);
	}
	fit_tof -> SetParameter(8, 0.);

	int npoints = 4;
	TGraphErrors gr_tof(npoints, pathlength,time, 0, timeE);
	for (int i = 0; i < 4; i++){
		int test = ((bthpattern/((int) pow(10.,3-i)))%10 == 4);
		if (!test) {gr_tof.RemovePoint(i); break; }
	}
	gr_tof.Fit(fit_tof,"Q");
	double chi2 = gr_tof.Chisquare(fit_tof);
	lhd = exp(-chi2);
	delete fit_tof;
	return lhd;
}
double Betalhd::CalculateBetaLikelihood_offline_dEdx(double trk_dEdx[2][9], double trd_dEdx[20], double tof_dEdx[4], double * trk_beta_tmp, double *trd_beta_tmp, double *tof_beta_tmp){
	double lhd = 1;
	int ndf = 0;
	for (int i = 0; i < 20 ; i++ ) {
		if (trd_dEdx[i]>0 && (dEdx_opt/10)%10 ){
			double lhd_tmp = GetLikelihood(trd_beta_tmp[i], betalhd_trd_dEdx[i], 1);
			lhd *= lhd_tmp;
			//if (DeBug) cout << "TRD["<< i <<"] = "<< lhd_tmp<<endl;
		}
		if (i < 9 && dEdx_opt%10 ){
			if (trk_dEdx[0][i] > 0){
				double lhd_tmp = GetLikelihood(trk_beta_tmp[i], betalhd_trk_dEdx[0][i], 0);
				lhd *= lhd_tmp;
				//	if (DeBug) cout << "TRK[0]["<< i <<"] = "<< lhd_tmp<<endl;
			}
			if (trk_dEdx[1][i] > 0){
				double lhd_tmp = GetLikelihood(trk_beta_tmp[i], betalhd_trk_dEdx[1][i], -1);
				lhd *= lhd_tmp;
				//	if (DeBug) cout << "TRK[1]["<< i <<"] = "<< lhd_tmp<<endl;
			}
		}
		if (i < 4 && tof_dEdx[i]>0 && (dEdx_opt/100)%10 ) {
			double lhd_tmp = GetLikelihood(tof_beta_tmp[i], betalhd_tof_dEdx[i], 2);
			lhd *= lhd_tmp;
			//if (DeBug) cout << "TOF["<< i <<"] = "<< lhd_tmp<<endl;
		}
	}
	//if (lhd < 1e-225) lhd = 1e-225;
	lhd = -log(lhd);
	return lhd;
}
double Betalhd::CalculateBetaLikelihood_offline_TOF(float * time, float * timeE, float * pathlength, double * tof_beta_tmp, int bthpattern, int opt){
	double lhd = 1.;
	double cval = 29.9792458; // cm/ns
	double time_exp[4]={0, 0, 0, 0};
	if (opt%2==0){
		time_exp[0] = time[0];
		time_exp[1] = 1./tof_beta_tmp[0]*(pathlength[0]-pathlength[1])/cval + time_exp[0];
		time_exp[2] = 1./tof_beta_tmp[1]*(pathlength[1]-pathlength[2])/cval + time_exp[1];
		time_exp[3] = 1./tof_beta_tmp[2]*(pathlength[2]-pathlength[3])/cval + time_exp[2];
	}else{
		time_exp[3] = time[3];
		time_exp[2] = 1./tof_beta_tmp[3]*(pathlength[2] - pathlength[3])/cval + time_exp[3];
		time_exp[1] = 1./tof_beta_tmp[2]*(pathlength[1] - pathlength[2])/cval + time_exp[2];
		time_exp[0] = 1./tof_beta_tmp[1]*(pathlength[0] - pathlength[1])/cval + time_exp[1];
	}
	TF1 * fit_tof = new TF1("fit_tof",TOF4Layersfcn, -6, 6, 9);
	for (int i = 0; i< 4 ; i++){
		fit_tof -> FixParameter(i, pathlength[i]);
		fit_tof -> FixParameter(i+4, time_exp[i]);
	}
	fit_tof -> SetParameter(8, 0.);

	int npoints = 4;
	TGraphErrors gr_tof(npoints, pathlength,time, 0, timeE);
	for (int i = 0; i < 4; i++){
		int test = ((bthpattern/((int) pow(10.,3-i)))%10 == 4);
		if (!test) {gr_tof.RemovePoint(i); npoints--; break; }
	}
	gr_tof.Fit(fit_tof,"Q");
	double chi2 = 0;
	for (int i = 0; i < gr_tof.GetN(); i++) {
		double diff = (fit_tof->Eval(pathlength[i])-gr_tof.GetY()[i])/gr_tof.GetEY()[i];
		chi2 += diff*diff + log(gr_tof.GetEY()[i]*gr_tof.GetEY()[i]*2*TMath::Pi());
	}
	//lhd = exp(-0.5*chi2);
	delete fit_tof;
	return chi2;
}

double Betalhd::CalculateBetaLikelihood_offline_TOF(float *time, float *timeE, float *pathlength, double *tof_beta_tmp, float *pathlength_trk, double *trk_beta_tmp, int bthpattern, int opt){
	double lhd = 1.;
	double cval = 29.9792458; // cm/ns
	double time_exp[4]={0, 0, 0, 0};
	double time_exp_trk[7];
	memset(time_exp_trk, 0, sizeof(time_exp_trk));
	if (opt%2==0){
		time_exp[0] = time[0];
		time_exp[1] = 1./tof_beta_tmp[0]*(pathlength[0]-pathlength[1])/cval + time_exp[0];
		time_exp_trk[0] = 1./tof_beta_tmp[1]*(pathlength[1]-pathlength_trk[0])/cval + time_exp[1];
		for (int i = 2; i < 8; i++) {
			time_exp_trk[i-1] = 1./trk_beta_tmp[i-1]*(pathlength_trk[i-2]-pathlength_trk[i-1])/cval + time_exp_trk[i-2];
		}
		time_exp[2] = 1./trk_beta_tmp[6]*(pathlength_trk[6]-pathlength[2])/cval + time_exp_trk[6];
		time_exp[3] = 1./tof_beta_tmp[2]*(pathlength[2]-pathlength[3])/cval + time_exp[2];
	}else{
		time_exp[3] = time[3];
		time_exp[2] = 1./tof_beta_tmp[3]*(pathlength[2] - pathlength[3])/cval + time_exp[3];
		time_exp_trk[6] = 1./tof_beta_tmp[2]*(pathlength_trk[6] - pathlength[2])/cval + time_exp[2];
		for (int i = 6; i > 0; i--) {
			time_exp_trk[i-1] = 1./trk_beta_tmp[i+1]*(pathlength_trk[i-1] - pathlength_trk[i])/cval + time_exp_trk[i];
		}
		time_exp[1] = 1./trk_beta_tmp[1]*(pathlength[1] - pathlength_trk[0])/cval + time_exp_trk[0];
		time_exp[0] = 1./tof_beta_tmp[1]*(pathlength[0] - pathlength[1])/cval + time_exp[1];
	}
	TF1 * fit_tof = new TF1("fit_tof",TOF4Layersfcn, -6, 6, 9);
	for (int i = 0; i< 4 ; i++){
		fit_tof -> FixParameter(i, pathlength[i]);
		fit_tof -> FixParameter(i+4, time_exp[i]);
	}
	fit_tof -> SetParameter(8, 0.);

	int npoints = 4;
	TGraphErrors gr_tof(npoints, pathlength,time, 0, timeE);
	for (int i = 0; i < 4; i++){
		int test = ((bthpattern/((int) pow(10.,3-i)))%10 == 4);
		if (!test) {gr_tof.RemovePoint(i); npoints--;break; }
	}
	gr_tof.Fit(fit_tof,"Q");
	double chi2 = 0;
	for (int i = 0; i < gr_tof.GetN(); i++) {
		double diff = (fit_tof->Eval(pathlength[i])-gr_tof.GetY()[i])/gr_tof.GetEY()[i];
		chi2 += diff*diff + log(gr_tof.GetEY()[i]*gr_tof.GetEY()[i]*2*TMath::Pi());
	}
	//lhd = exp(-0.5*chi2);
	delete fit_tof;
	//cout << "tof_chi2 = " << chi2 << endl;
	return chi2;
}
double Betalhd::CalculateBetaLikelihood_offline_RICH(double beta_rich, int opt){
	double sigma = 1;
	if (!Betalhd::betalhd_IsNaF) sigma = 0.3;
	if (Betalhd::betalhd_beta_rich_err > 0) sigma = Betalhd::betalhd_beta_rich_err;
	double x0 = beta_rich;
	double mean = Betalhd::betalhd_beta_rich_rec;
	double chi2 = (x0-mean)*(x0-mean)/sigma/sigma;
	//cout << "beta_rich = " << beta_rich << ", lhd:"<<exp(-0.5*chi2) <<endl;
	//return exp(-0.5*chi2);
	return chi2;
}
double Betalhd::CalculateBetaLikelihood_offline_RICH_hit(double beta_rich){
	if (hit_beta[0].size() != hit_beta[1].size()) {
		cout << "number of direct hits does not match number of reflected hits!" << endl;
		return 10-8;
	}
	double chi2 = 0;
	double chi2_tmp;
	float sigma;
	if (betalhd_IsNaF) {
		sigma = sigma_naf[1];
		if (IsMC)sigma *= 8.95563926791806963e-01; //MC has better resolution;
	}else {
		sigma = sigma_agl[1];
		if (IsMC) sigma *= 8.06300312723640000e-01; // MC has better resolution
	}
	if (update_variable) {
		rich_usedhits_direct = 0;
		rich_usedhits_mirror = 0;
	}
	double LnSigmaSquare2Pi = log(sigma*sigma*2*TMath::Pi()); // -2ln(1/sigma/sqrt(2*Pi))
	for (int i = 0; i < hit_beta[1].size(); i++){
		if (!(hit_beta[0][i]>0) && hit_beta[1][i] > 0 ){
			chi2_tmp = (hit_beta[1][i]-beta_rich)*(hit_beta[1][i]-beta_rich)/sigma/sigma + LnSigmaSquare2Pi;
			if (update_variable) rich_usedhits_mirror++;
			if (DeBug_1) cout << "reflected hit, no direct hit: chi2 = (" << hit_beta[1][i]<<"-"<< beta_rich<<")*("<<hit_beta[1][i] << "-" << beta_rich<<")/"<<sigma<<"/"<<sigma <<"+"<< LnSigmaSquare2Pi<<" = " <<  chi2_tmp << endl;
			chi2 += chi2_tmp;
		} else if (!(hit_beta[1][i]>0) && hit_beta[0][i] > 0 ){
			chi2_tmp = (hit_beta[0][i]-beta_rich)*(hit_beta[0][i]-beta_rich)/sigma/sigma + LnSigmaSquare2Pi;
			if (update_variable) rich_usedhits_direct++;
			if (DeBug_1) cout << "direct hit, no reflected hit: chi2 = (" << hit_beta[0][i]<<"-"<< beta_rich<<")*("<<hit_beta[0][i] << "-" << beta_rich<<")/"<<sigma<<"/"<<sigma <<" = " << chi2_tmp << endl;
			chi2 += chi2_tmp;
		} else if (hit_beta[0][i] > 0 && hit_beta[1][i] > 0){
			if (fabs(hit_beta[0][i]-beta_rich) < fabs(hit_beta[1][i]-beta_rich)) {
				chi2_tmp = (hit_beta[0][i]-beta_rich)*(hit_beta[0][i]-beta_rich)/sigma/sigma+ + LnSigmaSquare2Pi;
				if (update_variable) rich_usedhits_direct++;
				if (DeBug_1) cout << "direct hit: " << chi2_tmp << endl;
				chi2 += chi2_tmp;
			}else{
				chi2_tmp = (hit_beta[1][i]-beta_rich)*(hit_beta[1][i]-beta_rich)/sigma/sigma + LnSigmaSquare2Pi;
				if (update_variable) rich_usedhits_mirror++;
				if (DeBug_1) cout << "reflected hit: " << chi2_tmp << endl;
				chi2 += chi2_tmp;
			}
		}
	}
	//if (DeBug_1) cout << "chi2 = " << << endl;
	//return exp(-0.5*chi2);
	return chi2;
}
double Betalhd::CalculateBetaLikelihood_rigidity(int & ndf_trk, AMSPoint * trk_pnt, AMSDir trk_dir, double trk_dEdx[2][9], double trk_beta[9], double m_p, int charge, int opt){
	double lhd = 1;
	/*	AMSPoint err(10e-4, 10e-4, 100e-4);
		double rigidity = m_p * trk_beta[1] * BetaToGamma(trk_beta[1]) / charge;
		TrProp trprop(trk_pnt[1], trk_dir, rigidity);
		double J[5][5];//Jacobian matrix
		double radiationlength;
		double sigma_x = 10e-4;
		double sigma_y = 10e-4;
		for (int i = 2; i < 8 ; i++) {
		double ms_theta = 13.6e-3/trk_beta[i-1]/rigidity*sqrt(radiationlength)*(1+0.038*log(radiationlength));
		radiationlength = GetRadiationLength(trprop.GetP0(), trprop.GetDir(), trprop.GetRigidity(), trprop.GetP0z(),(double)trk_pnt[i].z() );
		TrPropagation(trprop, (double)trk_pnt[i].z() , J);
		double D_x = sigma_x; 
		double D_y = sigma_y; 
		sigma_x = J[0][0] * J[0][0] * D_x * D_x +  J[0][1] * J[0][1] * D_y * D_y + J[0][2] * J[0][2] * ms_theta * ms_theta;
		sigma_y = J[1][0] * J[1][0] * D_x * D_x +  J[1][1] * J[1][1] * D_y * D_y + J[1][2] * J[1][2] * ms_theta * ms_theta;
	///where is 10e-4
	sigma_x = sqrt(sigma_x);
	sigma_y = sqrt(sigma_y);
	if (trk_dEdx[1][i]>0) {
	lhd *= exp(-0.5*(trk_pnt.x()-trprop.GetP0x())*(trk_pnt.x()-trprop.GetP0x())/sigma_x/sigma_x);
	}
	if (trk_dEdx[1][i]>0) {
	lhd *= exp(-0.5*(trk_pnt.x()-trprop.GetP0x())*(trk_pnt.x()-trprop.GetP0x())/sigma_x/sigma_x);
	}
	rigidity = m_p * trk_beta[i] * BetaToGamma(trk_beta[i]) / charge;
	trprop.SetRigidity(rigidity);

	}
	*/
	return lhd;

}

TH2D * h_tof_deltaT_pl1_pdf[4];
double Betalhd::CalculateBetaLikelihood_TOF_pl1(TrTrackR *trk, BetaHR* bth, AMSPoint * tof_pnt, double beta){
	double lhd = 1;
	double betagamma = beta*BetaToGamma(beta);
	double c  = 0;
	if (bth->GetBetaPattern() != 4444) return 1e-200;
	if (!h_tof_deltaT_pl1_pdf[0]) {
		TDirectory *dsave = gDirectory;
		TString stn = AMSDATADIR.amsdatadir; stn += "LAPP/dEdxPDF/beta_norm_pl1_extend.root";
		pdffile_pl1 = TFile::Open(stn);
		//pdffile_pl1 = TFile::Open("root://eosams.cern.ch//eos/ams/user/j/jfeng/dEdxPDF/beta_norm_pl1_extend.root");
		//pdffile_pl1 = TFile::Open("root://eosams.cern.ch//eos/ams/group/mitep/Ntuple/ForAntiD/dEdxPDF/beta_norm_pl1_extend.root");
		if (dsave) dsave->cd();
		for (int i = 0; i < 4; i++){
			h_tof_deltaT_pl1_pdf[i] = (TH2D*) pdffile_pl1->Get(Form("h_tof_deltaT_%d", i));
		}
	}
	int fitcode = trk-> iTrTrackPar(2,0,21);
	if (bth && bth->IsTkTofMatch()){
		double time[4], timeE[4];
		int bthpattern = bth->GetBetaPattern();
		double pathlength[4] = {0.,0.,0.,0.};
		double total_time = 0;
		double total_path = 0;
		double TOF_deltaT[4] = {0.,0.,0.,0.};
		AMSPoint point;
		AMSDir dir;
		for (int ilayer=0;ilayer<4;ilayer++) {
			pathlength[ilayer] = trk -> Interpolate(tof_pnt[ilayer].z(), point, dir, fitcode);
			if (tof_pnt[ilayer].z() < 0) pathlength[ilayer] *= -1;
			total_path += pathlength[ilayer];
		}
		for (int i = 0; i < 4; i++){
			int test = ((bthpattern/((int) pow(10.,3-i)))%10 == 4);
			if (test){
				time[i] = (double)bth->GetTime(i);
			}else{
				time[i] = 0;
			}
			total_time += time[i];
		}
		if (bthpattern == 4444) {
			total_path /= 4;
			total_time /= 4;
			for (int i = 0; i < 4; i++){
				TOF_deltaT[i] = (time[i]-total_time)/(pathlength[i] - total_path);
			}
		}
		for (int ilayer = 0; ilayer < 4 ;ilayer++){
			int ibin = h_tof_deltaT_pl1_pdf[ilayer]->FindBin(betagamma, TOF_deltaT[ilayer]);
			bool isUnderOver = (h_tof_deltaT_pl1_pdf[ilayer]->IsBinOverflow(ibin) || h_tof_deltaT_pl1_pdf[ilayer]->IsBinUnderflow(ibin));
			if ( !isUnderOver)
				c = h_tof_deltaT_pl1_pdf[ilayer]->GetBinContent(ibin);
			else c = 1e-8;
			if (c < 1e-8) c = 1e-8;
			lhd *= c;
		}
	}
	return lhd;
}

TH2D * h_tof_deltaT_cal_pl1_pdf[4];
double Betalhd::CalculateBetaLikelihood_TOF_cal_pl1(TrTrackR *trk, BetaHR* bth, AMSPoint * tof_pnt, double beta){
	double lhd = 1;
	double betagamma = beta*BetaToGamma(beta);
	double c  = 0;
	if (bth->GetBetaPattern() != 4444) return 1e-200;
	if (!h_tof_deltaT_cal_pl1_pdf[0]) {
		TDirectory *dsave = gDirectory;
		TString stn = AMSDATADIR.amsdatadir; stn += "LAPP/dEdxPDF/beta_norm_pl1_extend.root";
		pdffile_pl1 = TFile::Open(stn);

		//pdffile_pl1 = TFile::Open("root://eosams.cern.ch//eos/ams/user/j/jfeng/dEdxPDF/beta_norm_pl1_extend.root");
		//pdffile_pl1 = TFile::Open("root://eosams.cern.ch//eos/ams/group/mitep/Ntuple/ForAntiD/dEdxPDF/beta_norm_pl1_extend.root");
		if (dsave) dsave -> cd();
		for (int i = 0; i < 4; i++){
			h_tof_deltaT_cal_pl1_pdf[i] = (TH2D*) pdffile_pl1->Get(Form("h_tof_deltaT_cal_%d", i));
		}
	}
	int fitcode = trk-> iTrTrackPar(2,0,21);
	if (bth && bth->IsTkTofMatch()){
		double time[4], timeE[4];
		int bthpattern = bth->GetBetaPattern();
		double pathlength[4] = {0.,0.,0.,0.};
		double total_time = 0;
		double total_path = 0;
		double TOF_deltaT[4] = {0.,0.,0.,0.};
		AMSPoint point;
		AMSDir dir;
		for (int ilayer=0;ilayer<4;ilayer++) {
			pathlength[ilayer] = trk -> Interpolate(tof_pnt[ilayer].z(), point, dir, fitcode);
			if (tof_pnt[ilayer].z() < 0) pathlength[ilayer] *= -1;
			total_path += pathlength[ilayer];
		}
		for (int i = 0; i < 4; i++){
			int test = ((bthpattern/((int) pow(10.,3-i)))%10 == 4);
			if (test){
				time[i] = (double)bth->GetTime(i);
			}else{
				time[i] = 0;
			}
			total_time += time[i];
		}
		if (bthpattern == 4444) {
			total_path /= 4;
			total_time /= 4;
			for (int i = 0; i < 4; i++){
				TOF_deltaT[i] = (time[i]-total_time)/(pathlength[i] - total_path);
			}
		}
		for (int ilayer = 0; ilayer < 4 ;ilayer++){
			int ibin = h_tof_deltaT_cal_pl1_pdf[ilayer]->FindBin(betagamma, TOF_deltaT[ilayer]);
			bool isUnderOver = (h_tof_deltaT_cal_pl1_pdf[ilayer]->IsBinOverflow(ibin) || h_tof_deltaT_cal_pl1_pdf[ilayer]->IsBinUnderflow(ibin));
			if ( !isUnderOver)
				c = h_tof_deltaT_cal_pl1_pdf[ilayer]->GetBinContent(ibin);
			else c = 1e-8;
			if (c < 1e-8) c = 1e-8;
			lhd *= c;
		}
	}
	return lhd;
}
double Betalhd::CalculateMaximumBetaLikelihood_RICH_hit() {
	double lhd = 1;
	double sigma;
	if (betalhd_IsNaF) sigma = sigma_naf[1];
	else sigma = sigma_agl[1];
	lhd *= pow(1/sigma/sqrt(2*TMath::Pi()), rich_usedhits_direct + rich_usedhits_mirror);
	return lhd;
}
double Betalhd::CalculateMaximumBetaLikelihood_TOF(){
	int npoints = 4;
	int inohit = 4;
	if (betalhd_bthpattern!=4444) for (int i = 0; i < 4; i++){
		int test = ((betalhd_bthpattern/((int) pow(10.,3-i)))%10 == 4);
		if (!test) {inohit = i;break; }
	}

	double lhd = 1;
	for (int i = 0; i < 4 ; i++) if(i!=inohit) {
		lhd *= 1/betalhd_TimeE[i]/sqrt(2*TMath::Pi());
	}
	return lhd;
}
int Betalhd::GetNDF(){return betalhd_ndf;}
double Betalhd::CalculateEnergyLoss(const AMSPoint  pnt, const AMSDir  dir, double rigidity, double z1, double z2, double m_p, double charge, int opt) {
	double elem[9] = {0.};// Abundance in mole/cm2  0:H 1:C 2:N 3:O 4:F 5:Na 6:Al 7:Si 8:Pb
	double ZA[9] = {0.99212, 0.49954, 0.49976, 0.50002, 0.47372,  0.47847,  0.48181, 0.49848, 0.39575};// charge-to-atomic mass ratio(Z/A)
	double I[9]  = {19.2e-6, 78.0e-6, 82.0e-6, 95.0e-6, 115.0e-6, 149.0e-6, 166.0e-6, 173.0e-6, 823.0e-6};//MeV
	double A[9]  = {1.0079,  12.0107, 14.0067, 15.9994, 18.9984,  22.9897,  26.9815, 28.0855, 207.2 };// atomic mass g mol-1
	int getAbundance = Betalhd::betalhd_GetElementAbundance( pnt, dir, rigidity, z1, z2, elem);// 
	if (getAbundance!=0) {
		return 0.;
	}
	double Eloss = 0.;
	double beta = 1/sqrt(1 + m_p*m_p/rigidity/rigidity*charge*charge );
	for (int i = 0; i < 9; i++){
		double dEdx_tmp = 0.001*Betalhd::BetheEquation(beta, ZA[i], I[i], 1, 1000*m_p); // GeV mol-1 cm2
		Eloss += dEdx_tmp*elem[i]*A[i];
	}
	return Eloss;
}

double Betalhd::BetheEquation(double beta, double ZA, double I, int z, double m_p) {
	if (! (beta < 1.)) beta = 0.9999999999999999;
	double K = 0.307075; //4 Pi NA re2 me c2 MeV mol-1 cm2
	double gamma = Betalhd::BetaToGamma(beta);
	double m_e = 0.511;// MeV
	double W_max = 2 * m_e * beta * beta * gamma * gamma / (1 + 2*gamma*m_e/m_p + (m_e/m_p*m_e/m_p)); // MeV
	double dEdx = K*z*z*ZA/beta/beta*(0.5*log(2*m_e*beta*beta*gamma*gamma*W_max/I/I) - beta*beta ); // MeV 
	return dEdx;
}
/////////////////////////////////////////////////////////////////////////////////////////////////
bool Betalhd::CalculateExpectedBetaMC(double *trk_beta_tmp, double * trd_beta_tmp, double *tof_beta_tmp, float & beta, double * trkmcE, double *trdmcE, double *tofmcE, int opt){
	double m_p = 0.938272;
	trk_beta_tmp[0] = beta;
	double gamma_tmp = BetaToGamma(trk_beta_tmp[0]);
	double energy_loss_tmp = trkmcE[0] - trdmcE[0];
	double gamma_get_tmp = gamma_tmp - energy_loss_tmp/m_p;
	trd_beta_tmp[0] = GammaToBeta(gamma_get_tmp);
	gamma_tmp = gamma_get_tmp;
	//TRD
	for (int i = 1; i < 20; i++){
		energy_loss_tmp = trdmcE[i] - trdmcE[i-1];
		gamma_get_tmp = gamma_tmp - energy_loss_tmp/m_p;
		trd_beta_tmp[i] = GammaToBeta(gamma_get_tmp);
		gamma_tmp = gamma_get_tmp;
	}
	//upper TOF
	energy_loss_tmp = trdmcE[20] - tofmcE[0];
	gamma_get_tmp = gamma_tmp - energy_loss_tmp/m_p;
	tof_beta_tmp[0] = GammaToBeta(gamma_get_tmp);
	gamma_tmp = gamma_get_tmp;
	energy_loss_tmp = tofmcE[0] - tofmcE[1];
	gamma_get_tmp = gamma_tmp - energy_loss_tmp/m_p;
	tof_beta_tmp[1] = GammaToBeta(gamma_get_tmp);
	gamma_tmp = gamma_get_tmp;

	//Inner Tracker
	energy_loss_tmp = tofmcE[1] - trkmcE[1];
	gamma_get_tmp = gamma_tmp - energy_loss_tmp/m_p;
	trk_beta_tmp[1] = GammaToBeta(gamma_get_tmp);
	gamma_tmp = gamma_get_tmp;
	for (int i = 2; i < 8; i++) {
		energy_loss_tmp = trkmcE[i] - trkmcE[i-1];
		gamma_get_tmp = gamma_tmp - energy_loss_tmp/m_p;
		trk_beta_tmp[i] = GammaToBeta(gamma_get_tmp);
		gamma_tmp = gamma_get_tmp;
	}

	//lower TOF
	energy_loss_tmp = trkmcE[7] - tofmcE[2];
	gamma_get_tmp = gamma_tmp - energy_loss_tmp/m_p;
	tof_beta_tmp[2] = GammaToBeta(gamma_get_tmp);
	gamma_tmp = gamma_get_tmp;
	energy_loss_tmp = tofmcE[2] - tofmcE[3];
	gamma_get_tmp = gamma_tmp - energy_loss_tmp/m_p;
	tof_beta_tmp[3] = GammaToBeta(gamma_get_tmp);
	gamma_tmp = gamma_get_tmp;
	//Layer 9
	energy_loss_tmp = tofmcE[3] - trkmcE[8];
	gamma_get_tmp = gamma_tmp - energy_loss_tmp/m_p;
	trk_beta_tmp[8] = GammaToBeta(gamma_get_tmp);
	gamma_tmp = gamma_get_tmp;

	return true;

}
/////////////////////////////////////////////////////////////////////////////////////////////////
bool Betalhd::GetMCTrueEnergy(AMSEventR * ev, double *trkmcE, double *trdmcE, double *tofmcE, int opt){
	if (ev->nMCEventg()==0) return false;
	double m_p = 0.938272;
	double momentum =ev->pMCEventg (0)->Momentum;
	if (opt==0 || opt == 20 || opt ==10) trkmcE[0] = sqrt(momentum*momentum + m_p*m_p);
	else trkmcE[8] = sqrt(momentum*momentum + m_p*m_p);
	for(int i=0;i<ev->nTrMCCluster();i++){
		TrMCClusterR trmcc = ev->TrMCCluster(i);
		int tkid = trmcc.GetTkId();
		int layJ = (std::abs(tkid/100));
		if (layJ==8) layJ=1;
		else if (layJ<8) layJ++;
		layJ--;
		if (trmcc.IsPrimary()){
			double trmcmom_tmp = trmcc.GetMomentum();
			trkmcE[layJ] = sqrt(trmcmom_tmp*trmcmom_tmp+ m_p*m_p);
		}
	}
	for (int i=0; i<ev->nTrdMCCluster() && i< 100; i++) {
		TrdMCClusterR trdmcc = ev->TrdMCCluster(i);
		if (trdmcc.ParticleNo > 0){
			int trdmc_Layer_tmp = 19-trdmcc.Layer;
			double trdmcekin_tmp = trdmcc.Ekin;
			trdmcE[trdmc_Layer_tmp] = trdmcekin_tmp + m_p;
		}
	}
	Float_t tofmcbeta_tmp[4] = {0.};
	Float_t tofmcbeta_lhd[4] = {0.};
	for (int i=0; i<ev->nTofMCCluster(); i++) {
		TofMCClusterR tofmcc = ev->TofMCCluster(i);
		if (tofmcc.ParentNo == 0){
			int toflay = tofmcc.GetLayer();
			tofmcbeta_tmp[toflay] = tofmcc.Beta;
			if (tofmcbeta_tmp[toflay] > tofmcbeta_lhd[toflay]) {
				tofmcbeta_lhd[toflay] = tofmcbeta_tmp[toflay];
			}
		}

	}
	for (int i = 0; i < 4; i++) if (tofmcbeta_lhd[i] > 0) tofmcE[i] = m_p/sqrt(1 - tofmcbeta_lhd[i]*tofmcbeta_lhd[i]); 
	bool success = true;
	for (int i = 0; i < 20; i++) {
		if (trdmcE[i] == 0) {success = false; break;}
		if (i < 9 && trkmcE[i] == 0) {success = false; break;}
		if (i < 4 && tofmcE[i] == 0) {success = false; break;}
	}
	return success;
}
/////////////////////////////////////////////////////////////////////////////////////////////////
bool Betalhd::CalculateExpectedBeta(double *trk_beta_tmp, double * trd_beta_tmp, double *tof_beta_tmp, float & beta, AMSPoint * trk_pnt,  AMSPoint * trd_pnt, AMSPoint * tof_pnt, const double m_p, const int charge, int opt){
	if (fabs(charge) < 0.5) return false;
	float beta_o = beta;
	if (beta>1) {
		for (int i = 0; i < 20; i++) trd_beta_tmp[i] = beta_o;
		for (int i = 0; i < 9; i++) trk_beta_tmp[i] = beta_o;
		for (int i = 0; i < 4; i++) tof_beta_tmp[i] = beta_o;
		return true;
	}

	double gamma = Betalhd::BetaToGamma(beta);
	double rigidity = beta*gamma*m_p/charge;
	double k[3], x2[3], x1[3];
	double energyloss;
	double totalenergy = gamma*m_p;
	double beta_tmp = beta;
	AMSPoint point_tmp;
	AMSDir dir_tmp;
	double correction[33]={
		1.05032, 
		1.07095, 
		1.20057, 
		0.814583, 
		0.90939, 
		0.914942, 
		0.822993, 
		0.825566, 
		1.24101, 
		0.903997, 
		0.815646, 
		0.814053, 
		1.23022, 
		0.815275, 
		0.888431, 
		0.836031, 
		0.866681, 
		1.06991, 
		1.19641, 
		0.902319, 
		0.866408, 
		0.896526, 
		0.807049, 
		1.24456, 
		1.01089, 
		1.18834, 
		0.723722, 
		1.45701, 
		0.722863, 
		1.63889, 
		0.509327, 
		0.773093, 
		1.68622, 
	};
	if (opt%2==0){
		for (int i = 0 ; i < 33; i++) correction[i] = 1.;
		//TRK L1
		trk_pnt[9].getp(x1[0], x1[1], x1[2]);
		trk_pnt[0].getp(x2[0], x2[1], x2[2]);
		for (int i = 0; i < 3 ; i++) {
			k[i] = x2[i] - x1[i];
		}
		point_tmp.setp(x1);
		dir_tmp.setd(k);

		if (Betalhd::z2zPropagation){
			TrProp prop(point_tmp, dir_tmp, rigidity);
			if (!Betalhd::CalculateExpectedBeta_z2z(beta_tmp, beta, x2[2], x1[2], prop, m_p, charge, opt)) return false;
			beta = beta_tmp;
			gamma = Betalhd::BetaToGamma(beta);
		}else{
			energyloss = Betalhd::CalculateEnergyLoss(point_tmp, dir_tmp, rigidity, x1[2], x2[2], m_p, charge, opt);
			totalenergy -= energyloss*correction[0];
			if (totalenergy < m_p) {totalenergy = m_p; return false;}
			gamma = totalenergy / m_p;
			beta = Betalhd::GammaToBeta(gamma);
		}
		rigidity = beta*gamma*m_p/charge;

		trk_beta_tmp[0] = beta;
		for (int i = 0; i < 3; i++) x1[i] = x2[i];

		//TRK L1 - TRD
		int itrdstart = -1;
		for (int i = 0; i < 20 ; i++){
			if (trd_pnt[i].z() > -200 && trd_pnt[i].y() > -200 ) {
				itrdstart = i;
				break;
			}
		}	
		if (itrdstart > -1){
			//TRD
			for (int ilay = itrdstart ; ilay < 20 ; ilay++) if (trd_pnt[ilay].z()>-200. && trd_pnt[ilay].y() < 200.){
				trd_pnt[ilay].getp(x2[0] ,x2[1], x2[2]);
				for (int i = 0; i < 3 ; i++) {
					k[i] = x2[i] - x1[i];
				}
				point_tmp.setp(x1);
				dir_tmp.setd(k);

				if (Betalhd::z2zPropagation){
					TrProp prop(point_tmp, dir_tmp, rigidity);
					if (!Betalhd::CalculateExpectedBeta_z2z(beta_tmp, beta, x2[2], x1[2], prop, m_p, charge, opt)) return false;
					beta = beta_tmp;
					gamma = Betalhd::BetaToGamma(beta);
				}else{
					energyloss = Betalhd::CalculateEnergyLoss(point_tmp, dir_tmp, rigidity, x1[2], x2[2], m_p, charge, opt);
					totalenergy -= energyloss*correction[ilay+1];
					if (totalenergy < m_p) {totalenergy = m_p; return false;}
					gamma = totalenergy / m_p;
					beta = Betalhd::GammaToBeta(gamma);
				}
				rigidity = beta*gamma*m_p/charge;
				trd_beta_tmp[ilay] = beta;
				for (int i = 0; i < 3; i++) x1[i] = x2[i];
			}
		}
		//TRD - upper TOF
		for (int ilay = 0 ; ilay < 2 ; ilay++) if (tof_pnt[ilay].z() > -200.){
			tof_pnt[ilay].getp(x2);
			for (int i = 0; i < 3 ; i++) {
				k[i] = x2[i] - x1[i];
			}
			point_tmp.setp(x1);
			dir_tmp.setd(k);
			if (Betalhd::z2zPropagation){
				TrProp prop(point_tmp, dir_tmp, rigidity);
				if (!Betalhd::CalculateExpectedBeta_z2z(beta_tmp, beta, x2[2], x1[2], prop, m_p, charge, opt)) return false;
				beta = beta_tmp;
				gamma = Betalhd::BetaToGamma(beta);
			}else{
				energyloss = Betalhd::CalculateEnergyLoss(point_tmp, dir_tmp, rigidity, x1[2], x2[2], m_p, charge, opt);
				totalenergy -= energyloss*correction[21+ilay];
				if (totalenergy < m_p) {totalenergy = m_p; return false;}
				gamma = totalenergy / m_p;
				beta = Betalhd::GammaToBeta(gamma);
			}
			rigidity = beta*gamma*m_p/charge;
			tof_beta_tmp[ilay] = beta;
			for (int i = 0; i < 3; i++) x1[i] = x2[i];
		}

		//Inner Tracker
		for (int ilay = 1 ; ilay < 8 ; ilay++) if (trk_pnt[ilay].z() > -200.){
			trk_pnt[ilay].getp(x2[0], x2[1], x2[2]);
			for (int i = 0; i < 3 ; i++) {
				k[i] = x2[i] - x1[i];
			}
			point_tmp.setp(x1);
			dir_tmp.setd(k);

			if (Betalhd::z2zPropagation){
				TrProp prop(point_tmp, dir_tmp, rigidity);
				if (!Betalhd::CalculateExpectedBeta_z2z(beta_tmp, beta, x2[2], x1[2], prop, m_p, charge, opt)) return false;
				beta = beta_tmp;
				gamma = Betalhd::BetaToGamma(beta);
			}else{
				energyloss = Betalhd::CalculateEnergyLoss(point_tmp, dir_tmp, rigidity, x1[2], x2[2], m_p, charge, opt);
				totalenergy -= energyloss*correction[22+ilay];
				if (totalenergy < m_p) {totalenergy = m_p; return false;}
				gamma = totalenergy / m_p;
				beta = Betalhd::GammaToBeta(gamma);
			}
			rigidity = beta*gamma*m_p/charge;
			trk_beta_tmp[ilay] = beta;
			for (int i = 0; i < 3; i++) x1[i] = x2[i];
		}

		//Inner Tracker - lower TOF 
		for (int ilay = 2 ; ilay < 4 ; ilay++) if (tof_pnt[ilay].z() > -200.){
			tof_pnt[ilay].getp(x2[0], x2[1], x2[2]);
			for (int i = 0; i < 3 ; i++) {
				k[i] = x2[i] - x1[i];
			}
			point_tmp.setp(x1);
			dir_tmp.setd(k);
			if (Betalhd::z2zPropagation){
				TrProp prop(point_tmp, dir_tmp, rigidity);
				if (!Betalhd::CalculateExpectedBeta_z2z(beta_tmp, beta, x2[2], x1[2], prop, m_p, charge, opt)) return false;
				beta = beta_tmp;
				gamma = Betalhd::BetaToGamma(beta);
			}else{
				energyloss = Betalhd::CalculateEnergyLoss(point_tmp, dir_tmp, rigidity, x1[2], x2[2], m_p, charge, opt);
				totalenergy -= energyloss*correction[28+ilay];
				if (totalenergy < m_p) {totalenergy = m_p; return false;}
				gamma = totalenergy / m_p;
				beta = Betalhd::GammaToBeta(gamma);
			}
			rigidity = beta*gamma*m_p/charge;
			tof_beta_tmp[ilay] = beta;
			for (int i = 0; i < 3; i++) x1[i] = x2[i];
		}

		//Tracker Layer 9
		trk_pnt[8].getp(x2[0], x2[1], x2[2]);
		for (int i = 0; i < 3 ; i++) {
			k[i] = x2[i] - x1[i];
		}
		point_tmp.setp(x1);
		dir_tmp.setd(k);
		if (Betalhd::z2zPropagation){
			TrProp prop(point_tmp, dir_tmp, rigidity);
			if (!Betalhd::CalculateExpectedBeta_z2z(beta_tmp, beta, x2[2], x1[2], prop, m_p, charge, opt)) return false;
			beta = beta_tmp;
			gamma = Betalhd::BetaToGamma(beta);
		}else{
			energyloss = Betalhd::CalculateEnergyLoss(point_tmp, dir_tmp, rigidity, x1[2], x2[2], m_p, charge, opt);
			totalenergy -= energyloss*correction[32];
			if (totalenergy < m_p) {totalenergy = m_p; return false;}
			gamma = totalenergy / m_p;
			beta = Betalhd::GammaToBeta(gamma);
		}
		rigidity = beta*gamma*m_p/charge;
		trk_beta_tmp[8] = beta;
		for (int i = 0; i < 3; i++) x1[i] = x2[i];
	}else {
		//TRK L9
		trk_pnt[9].getp(x1[0], x1[1], x1[2]);
		trk_pnt[8].getp(x2[0], x2[1], x2[2]);
		for (int i = 0; i < 3 ; i++) {
			k[i] = x2[i] - x1[i];
		}
		point_tmp.setp(x1);
		dir_tmp.setd(k);
		if (Betalhd::z2zPropagation){
			TrProp prop(point_tmp, dir_tmp, rigidity);
			if (!Betalhd::CalculateExpectedBeta_z2z(beta_tmp, beta, x2[2], x1[2], prop, m_p, charge, opt)) return false;
			beta = beta_tmp;
			gamma = Betalhd::BetaToGamma(beta);
		}else{
			energyloss = Betalhd::CalculateEnergyLoss(point_tmp, dir_tmp, rigidity, x1[2], x2[2], m_p, charge, opt);
			totalenergy -= energyloss;
			if (totalenergy < m_p) {totalenergy = m_p; return false;}
			gamma = totalenergy / m_p;
			beta = Betalhd::GammaToBeta(gamma);
		}
		rigidity = beta*gamma*m_p/charge;

		trk_beta_tmp[8] = beta;
		for (int i = 0; i < 3; i++) x1[i] = x2[i];
		//TRK L9 - lower TOF
		for (int ilay = 3 ; ilay > 1 ; ilay--) if (tof_pnt[ilay].z() > -200.){
			tof_pnt[ilay].getp(x2[0], x2[1], x2[2]);
			for (int i = 0; i < 3 ; i++) {
				k[i] = x2[i] - x1[i];
			}
			point_tmp.setp(x1);
			dir_tmp.setd(k);
			if (Betalhd::z2zPropagation){
				TrProp prop(point_tmp, dir_tmp, rigidity);
				if (!Betalhd::CalculateExpectedBeta_z2z(beta_tmp, beta, x2[2], x1[2], prop, m_p, charge, opt)) return false;
				beta = beta_tmp;
				gamma = Betalhd::BetaToGamma(beta);
			}else{
				energyloss = Betalhd::CalculateEnergyLoss(point_tmp, dir_tmp, rigidity, x1[2], x2[2], m_p, charge, opt);
				totalenergy -= energyloss;
				if (totalenergy < m_p) {totalenergy = m_p; return false;}
				gamma = totalenergy / m_p;
				beta = Betalhd::GammaToBeta(gamma);
			}
			rigidity = beta*gamma*m_p/charge;
			tof_beta_tmp[ilay] = beta;
			for (int i = 0; i < 3; i++) x1[i] = x2[i];
		}

		//Inner Tracker
		for (int ilay = 7 ; ilay > 0 ; ilay--) if (trk_pnt[ilay].z() > -200.){
			trk_pnt[ilay].getp(x2[0], x2[1], x2[2]);
			for (int i = 0; i < 3 ; i++) {
				k[i] = x2[i] - x1[i];
			}
			point_tmp.setp(x1);
			dir_tmp.setd(k);
			if (Betalhd::z2zPropagation){
				TrProp prop(point_tmp, dir_tmp, rigidity);
				if (!Betalhd::CalculateExpectedBeta_z2z(beta_tmp, beta, x2[2], x1[2], prop, m_p, charge, opt)) return false;
				beta = beta_tmp;
				gamma = Betalhd::BetaToGamma(beta);
			}else{
				energyloss = Betalhd::CalculateEnergyLoss(point_tmp, dir_tmp, rigidity, x1[2], x2[2], m_p, charge, opt);
				totalenergy -= energyloss;
				if (totalenergy < m_p) {totalenergy = m_p; return false;}
				gamma = totalenergy / m_p;
				beta = Betalhd::GammaToBeta(gamma);
			}
			rigidity = beta*gamma*m_p/charge;
			trk_beta_tmp[ilay] = beta;
			for (int i = 0; i < 3; i++) x1[i] = x2[i];
		}
		//upper TOF
		for (int ilay = 1 ; ilay > -1 ; ilay--) if (tof_pnt[ilay].z() > -200.){
			tof_pnt[ilay].getp(x2);
			for (int i = 0; i < 3 ; i++) {
				k[i] = x2[i] - x1[i];
			}
			point_tmp.setp(x1);
			dir_tmp.setd(k);
			if (Betalhd::z2zPropagation){
				TrProp prop(point_tmp, dir_tmp, rigidity);
				if (!Betalhd::CalculateExpectedBeta_z2z(beta_tmp, beta, x2[2], x1[2], prop, m_p, charge, opt)) return false;
				beta = beta_tmp;
				gamma = Betalhd::BetaToGamma(beta);
			}else{
				energyloss = Betalhd::CalculateEnergyLoss(point_tmp, dir_tmp, rigidity, x1[2], x2[2], m_p, charge, opt);
				totalenergy -= energyloss;
				if (totalenergy < m_p) {totalenergy = m_p; return false;}
				gamma = totalenergy / m_p;
				beta = Betalhd::GammaToBeta(gamma);
			}
			rigidity = beta*gamma*m_p/charge;
			tof_beta_tmp[ilay] = beta;
			for (int i = 0; i < 3; i++) x1[i] = x2[i];
		}
		//TRD
		int itrdstart = -1;
		for (int i = 19; i > -1  ; i--){
			if (trd_pnt[i].z() > -200 && trd_pnt[i].y() > -200 ) {
				itrdstart = i;
				break;
			}
		}	
		if (itrdstart > -1){
			for (int ilay = itrdstart ; ilay > -1 ; ilay--) if (trd_pnt[ilay].z()>-200. && trd_pnt[ilay].y() < 200.){
				trd_pnt[ilay].getp(x2[0] ,x2[1], x2[2]);
				for (int i = 0; i < 3 ; i++) {
					k[i] = x2[i] - x1[i];
				}
				point_tmp.setp(x1);
				dir_tmp.setd(k);
				if (Betalhd::z2zPropagation){
					TrProp prop(point_tmp, dir_tmp, rigidity);
					if (!Betalhd::CalculateExpectedBeta_z2z(beta_tmp, beta, x2[2], x1[2], prop, m_p, charge, opt)) return false;
					beta = beta_tmp;
					gamma = Betalhd::BetaToGamma(beta);
				}else{
					energyloss = Betalhd::CalculateEnergyLoss(point_tmp, dir_tmp, rigidity, x1[2], x2[2], m_p, charge, opt);
					totalenergy -= energyloss;
					if (totalenergy < m_p) {totalenergy = m_p; return false;}
					gamma = totalenergy / m_p;
					beta = Betalhd::GammaToBeta(gamma);
				}
				rigidity = beta*gamma*m_p/charge;
				trd_beta_tmp[ilay] = beta;
				for (int i = 0; i < 3; i++) x1[i] = x2[i];
			}
		}
		//TRK L1
		trk_pnt[0].getp(x2[0], x2[1], x2[2]);
		for (int i = 0; i < 3 ; i++) {
			k[i] = x2[i] - x1[i];
		}
		point_tmp.setp(x1);
		dir_tmp.setd(k);
		if (Betalhd::z2zPropagation){
			TrProp prop(point_tmp, dir_tmp, rigidity);
			if (!Betalhd::CalculateExpectedBeta_z2z(beta_tmp, beta, x2[2], x1[2], prop, m_p, charge, opt)) return false;
			beta = beta_tmp;
			gamma = Betalhd::BetaToGamma(beta);
		}else{
			energyloss = Betalhd::CalculateEnergyLoss(point_tmp, dir_tmp, rigidity, x1[2], x2[2], m_p, charge, opt);
			totalenergy -= energyloss;
			if (totalenergy < m_p) {totalenergy = m_p; return false;}
			gamma = totalenergy / m_p;
			beta = Betalhd::GammaToBeta(gamma);
		}
		rigidity = beta*gamma*m_p/charge;
		trk_beta_tmp[0] = beta;
		for (int i = 0; i < 3; i++) x1[i] = x2[i];
	}
	beta = beta_o;
	return true;
}
/////////////////////////////////////////////////////////////////////////////////////////////////
bool Betalhd::CalculateExpectedBeta_z(double & beta_z, float & beta, double z_aim, TrProp prop, const double m_p, const int charge, int opt){
	double beta_d = beta;
	return Betalhd::CalculateExpectedBeta_z(beta_z, beta_d, z_aim, prop, m_p, charge, opt);
}
bool Betalhd::CalculateExpectedBeta_z(double & beta_z, double & beta, double z_aim, TrProp prop, const double m_p, const int charge, int opt){
	double z_ini;
	if (opt == 0)  z_ini = 180;
	if (opt == 1)  z_ini = -180;
	return Betalhd::CalculateExpectedBeta_z2z(beta_z, beta, z_aim, z_ini, prop, m_p, charge, opt);
}
bool Betalhd::CalculateExpectedBeta_z2z(double & beta_z, float & beta, double z_aim, double z_ini, TrProp prop, const double m_p, const int charge, int opt , double step){
	double beta_d = beta;
	return Betalhd::CalculateExpectedBeta_z2z( beta_z,  beta_d,  z_aim,  z_ini,  prop,  m_p,  charge, opt, step);
}
bool Betalhd::CalculateExpectedBeta_z2z(double & beta_z, double & beta, double z_aim, double z_ini, TrProp prop, const double m_p, const int charge, int opt, double step){
	/*	if (!(opt == 0 && z_aim < 180 && z_aim > -180)) {
		beta_z = beta;
		return false;
		}
		*/
	if (beta > 1) {
		beta_z = beta;
		return true;
	}
	double beta_o = beta;
	double z_tmp  = z_ini;
	prop.Propagate(z_tmp);
	double gamma = Betalhd::BetaToGamma(beta);
	double rigidity = beta*gamma*m_p/charge;
	double x2[3], x1[3];
	double energyloss;
	double totalenergy = gamma*m_p;
	AMSPoint point_tmp;
	AMSDir dir_tmp;
	point_tmp.setp(prop.GetP0().x(),prop.GetP0().y(),prop.GetP0().z() );
	dir_tmp.setd(prop.GetDir().x(),prop.GetDir().y(),prop.GetDir().z());
	point_tmp.getp(x1[0], x1[1], x1[2]);
	int condition;
	if (opt == 0){//down going
		condition = z_tmp > (z_aim + step);
	}else{//upgoing
		condition = z_tmp < (z_aim - step);
	}
	while (condition) {
		if (opt == 0){
			z_tmp = z_tmp - step ;
			condition = z_tmp > (z_aim + step);
		}else{
			z_tmp = z_tmp + step ;
			condition = z_tmp < (z_aim - step);
		} 
		prop.Propagate(z_tmp);
		energyloss = Betalhd::CalculateEnergyLoss(point_tmp, dir_tmp, rigidity, x1[2], prop.GetP0().z(), m_p, charge, opt);
		totalenergy -= energyloss;
		if (totalenergy < m_p) {totalenergy = m_p; return false;}
		gamma = totalenergy / m_p;
		beta = Betalhd::GammaToBeta(gamma);
		rigidity = beta*gamma*m_p/charge;

		point_tmp.setp(prop.GetP0().x(),prop.GetP0().y(),prop.GetP0().z() );
		dir_tmp.setd(prop.GetDir().x(),prop.GetDir().y(),prop.GetDir().z());

		point_tmp.getp(x1[0], x1[1], x1[2]);
	}
	energyloss = Betalhd::CalculateEnergyLoss(point_tmp, dir_tmp, rigidity, x1[2], z_aim, m_p, charge, opt);
	totalenergy -= energyloss;
	if (totalenergy < m_p) {totalenergy = m_p; return false;}
	gamma = totalenergy / m_p;
	beta = Betalhd::GammaToBeta(gamma);
	beta_z = beta;
	beta = beta_o;
	//cout << beta << " -> " << beta_z << endl;
	return true;
}
bool Betalhd::CalculateExpectedBeta_TOF(double * trk_beta_tmp, double *tof_beta_tmp, float & beta, AMSPoint * trk_pnt, AMSPoint * tof_pnt, const double m_p, const int charge, int opt){
	if (fabs(charge) < 0.5) return false;
	float beta_o = beta;
	double  beta_tmp;
	if (beta>1) {
		for (int i = 0; i < 9; i++) trk_beta_tmp[i] = beta_o;
		for (int i = 0; i < 4; i++) tof_beta_tmp[i] = beta_o;
		return true;	
	}
	double gamma = Betalhd::BetaToGamma(beta);
	double rigidity = beta*gamma*m_p/charge;
	double k[3], x2[3], x1[3];
	double energyloss;
	double totalenergy = gamma*m_p;
	AMSPoint point_tmp;
	AMSDir dir_tmp;
	if (opt%2==0){
		//TOF L1
		trk_pnt[9].getp(x1[0], x1[1], x1[2]);
		for (int ilay = 0 ; ilay < 2 ; ilay++) if (tof_pnt[ilay].z() > -200.){
			tof_pnt[ilay].getp(x2);
			for (int i = 0; i < 3 ; i++) {
				k[i] = x2[i] - x1[i];
			}
			point_tmp.setp(x1);
			dir_tmp.setd(k);
			if (Betalhd::z2zPropagation){
				TrProp prop(point_tmp, dir_tmp, rigidity);
				if (!Betalhd::CalculateExpectedBeta_z2z(beta_tmp, beta, x2[2], x1[2], prop, m_p, charge, opt)) return false;
				beta = beta_tmp;
				gamma = Betalhd::BetaToGamma(beta);
			}else{
				energyloss = Betalhd::CalculateEnergyLoss(point_tmp, dir_tmp, rigidity, x1[2], x2[2], m_p, charge, opt);
				totalenergy -= energyloss;
				if (totalenergy < m_p) {totalenergy = m_p; return false;}
				gamma = totalenergy / m_p;
				beta = Betalhd::GammaToBeta(gamma);
			}
			rigidity = beta*gamma*m_p/charge;
			tof_beta_tmp[ilay] = beta;
			for (int i = 0; i < 3; i++) x1[i] = x2[i];
		}

		//Inner Tracker
		for (int ilay = 1 ; ilay < 8 ; ilay++) if (trk_pnt[ilay].z() > -200.){
			trk_pnt[ilay].getp(x2[0], x2[1], x2[2]);
			for (int i = 0; i < 3 ; i++) {
				k[i] = x2[i] - x1[i];
			}
			point_tmp.setp(x1);
			dir_tmp.setd(k);
			if (Betalhd::z2zPropagation){
				TrProp prop(point_tmp, dir_tmp, rigidity);
				if (!Betalhd::CalculateExpectedBeta_z2z(beta_tmp, beta, x2[2], x1[2], prop, m_p, charge, opt)) return false;
				beta = beta_tmp;
				gamma = Betalhd::BetaToGamma(beta);
			}else{
				energyloss = Betalhd::CalculateEnergyLoss(point_tmp, dir_tmp, rigidity, x1[2], x2[2], m_p, charge, opt);
				totalenergy -= energyloss;
				if (totalenergy < m_p) {totalenergy = m_p; return false;}
				gamma = totalenergy / m_p;
				beta = Betalhd::GammaToBeta(gamma);
			}
			rigidity = beta*gamma*m_p/charge;
			trk_beta_tmp[ilay] = beta;
			for (int i = 0; i < 3; i++) x1[i] = x2[i];
		}

		//Inner Tracker - lower TOF 
		for (int ilay = 2 ; ilay < 4 ; ilay++) if (tof_pnt[ilay].z() > -200.){
			tof_pnt[ilay].getp(x2[0], x2[1], x2[2]);
			for (int i = 0; i < 3 ; i++) {
				k[i] = x2[i] - x1[i];
			}
			point_tmp.setp(x1);
			dir_tmp.setd(k);
			if (Betalhd::z2zPropagation){
				TrProp prop(point_tmp, dir_tmp, rigidity);
				if (!Betalhd::CalculateExpectedBeta_z2z(beta_tmp, beta, x2[2], x1[2], prop, m_p, charge, opt)) return false;
				beta = beta_tmp;
				gamma = Betalhd::BetaToGamma(beta);
			}else{
				energyloss = Betalhd::CalculateEnergyLoss(point_tmp, dir_tmp, rigidity, x1[2], x2[2], m_p, charge, opt);
				totalenergy -= energyloss;
				if (totalenergy < m_p) {totalenergy = m_p; return false;}
				gamma = totalenergy / m_p;
				beta = Betalhd::GammaToBeta(gamma);
			}
			rigidity = beta*gamma*m_p/charge;
			tof_beta_tmp[ilay] = beta;
			for (int i = 0; i < 3; i++) x1[i] = x2[i];
		}
		//Tracker Layer 9
		trk_pnt[8].getp(x2[0], x2[1], x2[2]);
		for (int i = 0; i < 3 ; i++) {
			k[i] = x2[i] - x1[i];
		}
		point_tmp.setp(x1);
		dir_tmp.setd(k);
		if (Betalhd::z2zPropagation){
			TrProp prop(point_tmp, dir_tmp, rigidity);
			if (!Betalhd::CalculateExpectedBeta_z2z(beta_tmp, beta, x2[2], x1[2], prop, m_p, charge, opt)) return false;
			beta = beta_tmp;
			gamma = Betalhd::BetaToGamma(beta);
		}else{
			energyloss = Betalhd::CalculateEnergyLoss(point_tmp, dir_tmp, rigidity, x1[2], x2[2], m_p, charge, opt);
			totalenergy -= energyloss;
			if (totalenergy < m_p) {totalenergy = m_p; return false;}
			gamma = totalenergy / m_p;
			beta = Betalhd::GammaToBeta(gamma);
		}
		rigidity = beta*gamma*m_p/charge;
		trk_beta_tmp[8] = beta;
		for (int i = 0; i < 3; i++) x1[i] = x2[i];

	}else {
		trk_pnt[9].getp(x1[0], x1[1], x1[2]);
		//TRK L9 - lower TOF
		for (int ilay = 3 ; ilay > 1 ; ilay--) if (tof_pnt[ilay].z() > -200.){
			tof_pnt[ilay].getp(x2[0], x2[1], x2[2]);
			for (int i = 0; i < 3 ; i++) {
				k[i] = x2[i] - x1[i];
			}
			point_tmp.setp(x1);
			dir_tmp.setd(k);
			if (Betalhd::z2zPropagation){
				TrProp prop(point_tmp, dir_tmp, rigidity);
				if (!Betalhd::CalculateExpectedBeta_z2z(beta_tmp, beta, x2[2], x1[2], prop, m_p, charge, opt)) return false;
				beta = beta_tmp;
				gamma = Betalhd::BetaToGamma(beta);
			}else{
				energyloss = Betalhd::CalculateEnergyLoss(point_tmp, dir_tmp, rigidity, x1[2], x2[2], m_p, charge, opt);
				totalenergy -= energyloss;
				if (totalenergy < m_p) {totalenergy = m_p; return false;}
				gamma = totalenergy / m_p;
				beta = Betalhd::GammaToBeta(gamma);
			}
			rigidity = beta*gamma*m_p/charge;
			tof_beta_tmp[ilay] = beta;
			for (int i = 0; i < 3; i++) x1[i] = x2[i];
		}

		//Inner Tracker
		for (int ilay = 7 ; ilay > 0 ; ilay--) if (trk_pnt[ilay].z() > -200.){
			trk_pnt[ilay].getp(x2[0], x2[1], x2[2]);
			for (int i = 0; i < 3 ; i++) {
				k[i] = x2[i] - x1[i];
			}
			point_tmp.setp(x1);
			dir_tmp.setd(k);
			if (Betalhd::z2zPropagation){
				TrProp prop(point_tmp, dir_tmp, rigidity);
				if (!Betalhd::CalculateExpectedBeta_z2z(beta_tmp, beta, x2[2], x1[2], prop, m_p, charge, opt)) return false;
				beta = beta_tmp;
				gamma = Betalhd::BetaToGamma(beta);
			}else{
				energyloss = Betalhd::CalculateEnergyLoss(point_tmp, dir_tmp, rigidity, x1[2], x2[2], m_p, charge, opt);
				totalenergy -= energyloss;
				if (totalenergy < m_p) {totalenergy = m_p; return false;}
				gamma = totalenergy / m_p;
				beta = Betalhd::GammaToBeta(gamma);
			}
			rigidity = beta*gamma*m_p/charge;
			trk_beta_tmp[ilay] = beta;
			for (int i = 0; i < 3; i++) x1[i] = x2[i];
		}
		//upper TOF
		for (int ilay = 1 ; ilay > -1 ; ilay--) if (tof_pnt[ilay].z() > -200.){
			tof_pnt[ilay].getp(x2);
			for (int i = 0; i < 3 ; i++) {
				k[i] = x2[i] - x1[i];
			}
			point_tmp.setp(x1);
			dir_tmp.setd(k);
			if (Betalhd::z2zPropagation){
				TrProp prop(point_tmp, dir_tmp, rigidity);
				if (!Betalhd::CalculateExpectedBeta_z2z(beta_tmp, beta, x2[2], x1[2], prop, m_p, charge, opt)) return false;
				beta = beta_tmp;
				gamma = Betalhd::BetaToGamma(beta);
			}else{
				energyloss = Betalhd::CalculateEnergyLoss(point_tmp, dir_tmp, rigidity, x1[2], x2[2], m_p, charge, opt);
				totalenergy -= energyloss;
				if (totalenergy < m_p) {totalenergy = m_p; return false;}
				gamma = totalenergy / m_p;
				beta = Betalhd::GammaToBeta(gamma);
			}
			rigidity = beta*gamma*m_p/charge;
			tof_beta_tmp[ilay] = beta;
			for (int i = 0; i < 3; i++) x1[i] = x2[i];
		}
		//TRK L1
		trk_pnt[0].getp(x2[0], x2[1], x2[2]);
		for (int i = 0; i < 3 ; i++) {
			k[i] = x2[i] - x1[i];
		}
		point_tmp.setp(x1);
		dir_tmp.setd(k);
		if (Betalhd::z2zPropagation){
			TrProp prop(point_tmp, dir_tmp, rigidity);
			if (!Betalhd::CalculateExpectedBeta_z2z(beta_tmp, beta, x2[2], x1[2], prop, m_p, charge, opt)) return false;
			beta = beta_tmp;
			gamma = Betalhd::BetaToGamma(beta);
		}else{
			energyloss = Betalhd::CalculateEnergyLoss(point_tmp, dir_tmp, rigidity, x1[2], x2[2], m_p, charge, opt);
			totalenergy -= energyloss;
			if (totalenergy < m_p) {totalenergy = m_p; return false;}
			gamma = totalenergy / m_p;
			beta = Betalhd::GammaToBeta(gamma);
		}
		rigidity = beta*gamma*m_p/charge;
		trk_beta_tmp[0] = beta;
		for (int i = 0; i < 3; i++) x1[i] = x2[i];

	}
	beta = beta_o;
	/*	cout << "beta trk: " ;
		for (int i = 0; i < 9; i++){
		cout << trk_beta_tmp[i] << ", " ;
		}
		cout << endl;
		cout << "beta tof: " ;
		for (int i = 0; i < 4; i++){
		cout << tof_beta_tmp[i] << ", " ;
		}
		cout << endl;
		*/	return true;
}
/////////////////////////////////////////////////////////////////////////////////////////////////
int Betalhd::betalhd_GetElementAbundance(const AMSPoint & pnt, const AMSDir & dir, double rigidity, double z1, double z2, double elem[9]) {
	static TFile *file    = 0;
	static TH3F  *hist[9] = { 0, 0, 0, 0, 0, 0, 0 };

	for (int i = 0; i < 9; i++) elem[i] = 0;

	if (!hist[0]) {
#pragma omp critical (getelementabundance)
		{
			if (!hist[0] && !file) {
				TString sfn = AMSDATADIR.amsdatadir; sfn += "LAPP/dEdxPDF/g4mscan.root";
				//TString sfn = "root://eosams.cern.ch//eos/ams/group/mitep/Ntuple/ForAntiD/dEdxPDF/g4mscan.root";//1.0 cm precision
				//TString sfn = "root://eosams.cern.ch//eos/ams/group/mitep/Ntuple/ForAntiD/dEdxPDF/g4mscan05.root";//0.5 cm precision
				//if (getenv("G4elMap")) sfn = getenv("G4elMap");

				TDirectory *dsave = gDirectory;
				file = TFile::Open(sfn);
				if (dsave) dsave->cd();

				if (file) {
					cout << "AMSEventR::GetElementAbundance-I-Open file: "
						<< file->GetName() << endl;
					for (int i = 0; i < 9; i++)
						hist[i] = (TH3F *)file->Get(Form("hist%d", i+1));
				}
				if (!hist[0] || !hist[8]) {
					cout << "AMSEventR::GetElementAbundance-E-hist not found" << endl;
					hist[0] = (TH3F *)1;
				}
			}
		}
	}
	if (!hist[0] || hist[0] == (TH3F *)1) return -3;

	TrProp trp(pnt, dir, rigidity);

	int ib1 = hist[0]->GetZaxis()->FindBin(z1);
	int ib2 = hist[0]->GetZaxis()->FindBin(z2);
	int nb  = hist[0]->GetNbinsZ();

	if (ib2 < ib1) { int swp = ib1 ; ib1 = ib2; ib2 = swp; }

	if (ib1 <  1) ib1 = 1;
	if (ib2 > nb) ib2 = nb;

	double zp = hist[0]->GetZaxis()->GetBinCenter(ib1);
	z2 = hist[0]->GetZaxis()->GetBinCenter(ib2);
	trp.Propagate(zp);
	AMSPoint p1 = trp.GetP0();

	int   ndv = 5;
	int   err = 0;

	do {
		double b = TMath::Abs(TrFit::GuFld(p1).x());
		double l = (b > 0) ? TMath::Abs(rigidity)/0.3/b : 0;
		if (l <= 0 || l > 10) l = 10;

		zp = p1.z()+l; if (zp > z2) zp = z2;
		trp.Propagate(zp);
		AMSPoint p2 = trp.GetP0();

		ib1 = hist[0]->GetZaxis()->FindBin(p1.z());
		ib2 = hist[0]->GetZaxis()->FindBin(p2.z());
		int ns = (ib2-ib1)*ndv;

		double dl = (p2-p1).norm();
		if (dl <= 0) break;

		double cosz = (p2.z()-p1.z())/dl;
		double dz   = hist[0]->GetZaxis()->GetBinWidth(ib1)/cosz/ndv;

		for (int i = 0; i < ns; i++) {
			double   z = p1.z()+(p2.z()-p1.z())/ns*(i+0.5);
			AMSPoint p = p1+(p2-p1)/(p2.z()-p1.z())*(z-p1.z());
			int     ix = hist[0]->GetXaxis()->FindBin(p.x());
			int     iy = hist[0]->GetYaxis()->FindBin(p.y());
			if (ix <= 0 || hist[0]->GetNbinsX() < ix ||
					iy <= 0 || hist[0]->GetNbinsY() < iy) { err = -2; 
				continue; }

			for (int j = 0; j < 9; j++)
				if (hist[j]) {
					Double_t bc = hist[j]->GetBinContent(ix, iy, ib1+i/ndv);
					if (bc > 0) elem[j] += bc*dz;
				}
		}

		p1 = p2;
	} while (zp < z2);

	return err;

}
/////////////////////////////////////////////////////////////////////////////////////////////////
double Betalhd::GetRadiationLength(const AMSPoint &pnt,
		const AMSDir   &dir,
		double rigidity, double z1, double z2)
{
	static TFile *file = 0;
	static TH3F  *hist = 0;

	if (!hist) {
#pragma omp critical (getradiationlength)
		{
			if (!hist && !file) {
				TString sfn = AMSDATADIR.amsdatadir; sfn += "g4mscan.root";
				if (getenv("G4MScan")) sfn = getenv("G4MScan");

				TDirectory *dsave = gDirectory;
				file = TFile::Open(sfn);
				if (dsave) dsave->cd();

				if (file) hist = (TH3F *)file->Get("hist1");
				if (hist)
					cout << "AMSEventR::GetRadiationLength-I-Open file: "
						<< file->GetName() << endl;
				else {
					cout << "AMSEventR::GetRadiationLength-E-hist1 not found" << endl;
					hist = (TH3F *)1;
				}
			}
		}
	}
	if (!hist || hist == (TH3F *)1) return -1;

	TrProp trp(pnt, dir, rigidity);

	int ib1 = hist->GetZaxis()->FindBin(z1);
	int ib2 = hist->GetZaxis()->FindBin(z2);

	if (ib2 < ib1) { int swp = ib1 ; ib1 = ib2; ib2 = swp; }

	double ms = 0;
	for (int i = ib1; i <= ib2; i++) {
		double z = hist->GetZaxis()->GetBinCenter(i);
		trp.Propagate(z);
		int m = hist->GetBinContent(hist->GetXaxis()->FindBin(trp.GetP0x()),
				hist->GetYaxis()->FindBin(trp.GetP0y()), i);
		if (m != 0) ms += pow(10., m/10000.-3)/trp.GetD0z();
	}
	return ms;
}
////////////////////////////////////RICH BDT////////////////////////////////////////
TMVA::Reader * Betalhd::RICHBDTreader[2];
float Betalhd::RICHBDTvar[21];
double Betalhd::GetClassifierValue(float p[21], int IsNaF, TString method){
	double bdt = -2;
	int imethod = -1;

	if (method == "BDT") imethod = 0;
	else if (method == "BDTD") imethod = 1;
	else if (method == "BDTG") imethod = 2;

	if (imethod < 0) return -2; 

	if (!(IsNaF == 0 || IsNaF == 1)) {
		return -3;
	}

	if (!RICHBDTreader[IsNaF]){
		RICHBDTreader[IsNaF] = new TMVA::Reader("!Color:!Silent");
		RICHBDTreader[IsNaF]->AddVariable("rich.dcharge", &RICHBDTvar[0]);
		RICHBDTreader[IsNaF]->AddVariable("rich.q", &RICHBDTvar[1]);
		RICHBDTreader[IsNaF]->AddVariable("rich.prob", &RICHBDTvar[2]);
		RICHBDTreader[IsNaF]->AddVariable("rich.bdbeta", &RICHBDTvar[3]);
		RICHBDTreader[IsNaF]->AddVariable("rich.bq", &RICHBDTvar[4]);
		RICHBDTreader[IsNaF]->AddVariable("rich.bprobkolm", &RICHBDTvar[5]);
		RICHBDTreader[IsNaF]->AddVariable("rich.blkhd", &RICHBDTvar[6]);
		RICHBDTreader[IsNaF]->AddVariable("rich.bnpe_mirror", &RICHBDTvar[7]);
		RICHBDTreader[IsNaF]->AddVariable("rich.bnpe_direct", &RICHBDTvar[8]);
		RICHBDTreader[IsNaF]->AddVariable("rich.dist_tile_border", &RICHBDTvar[9]);
		RICHBDTreader[IsNaF]->AddVariable("rich.beta_err_expect", &RICHBDTvar[10]);
		RICHBDTreader[IsNaF]->AddVariable("rich.charge_err_expect", &RICHBDTvar[11]);
		RICHBDTreader[IsNaF]->AddVariable("maxbeta-rich.beta>-0.5?maxbeta-rich.beta:-0.1", &RICHBDTvar[12]);
		RICHBDTreader[IsNaF]->AddVariable("pvalue", &RICHBDTvar[13]);
		RICHBDTreader[IsNaF]->AddVariable("log10npebeta_pr", &RICHBDTvar[14]);
		RICHBDTreader[IsNaF]->AddVariable("rich.UDist>0?log10(rich.UDist):-5", &RICHBDTvar[15]);
		RICHBDTreader[IsNaF]->AddVariable("chi2_notused>0?log10(chi2_notused):-1", &RICHBDTvar[16]);
		RICHBDTreader[IsNaF]->AddVariable("rich.width", &RICHBDTvar[17]);
		RICHBDTreader[IsNaF]->AddVariable("(tof_beta_n[0][3]-rich.beta)/beta_lhd_err[0]", &RICHBDTvar[18]);
		RICHBDTreader[IsNaF]->AddVariable("rich.nhits_mirror", &RICHBDTvar[19]);
		RICHBDTreader[IsNaF]->AddVariable("nrichhits_used", &RICHBDTvar[20]);

		if (IsNaF){ 
			RICHBDTreader[IsNaF]->BookMVA("BDT method", "/afs/cern.ch/user/j/jfeng/public/Betalhd/rich_bdt/TMVAnalysis_iss_agl_rich_BDT.weights.xml");
			RICHBDTreader[IsNaF]->BookMVA("BDTD method", "/afs/cern.ch/user/j/jfeng/public/Betalhd/rich_bdt/TMVAnalysis_iss_agl_rich_BDTD.weights.xml");
			RICHBDTreader[IsNaF]->BookMVA("BDTG method", "/afs/cern.ch/user/j/jfeng/public/Betalhd/rich_bdt/TMVAnalysis_iss_agl_rich_BDTG.weights.xml");
		}else{
			RICHBDTreader[IsNaF]->BookMVA("BDT method", "/afs/cern.ch/user/j/jfeng/public/Betalhd/rich_bdt/TMVAnalysis_iss_agl_rich_BDT.weights.xml");
			RICHBDTreader[IsNaF]->BookMVA("BDTD method", "/afs/cern.ch/user/j/jfeng/public/Betalhd/rich_bdt/TMVAnalysis_iss_agl_rich_BDTD.weights.xml");
			RICHBDTreader[IsNaF]->BookMVA("BDTG method", "/afs/cern.ch/user/j/jfeng/public/Betalhd/rich_bdt/TMVAnalysis_iss_agl_rich_BDTG.weights.xml");
		}
	}
	for (int i = 0; i < 21 ;i++){
		RICHBDTvar[i] = p[i];
	}

	bdt = RICHBDTreader[IsNaF] -> EvaluateMVA(Form("%s method", method.Data()));
	return bdt;
}

////////////////////////////////////Tracker BDT////////////////////////////////////////
//float Betalhd::TrackerBDTvar[23];//zwengntuple
float Betalhd::TrackerBDTvar[31];
TMVA::Reader * Betalhd::TrackerBDTreader[2];
double Betalhd::GetTrackerClassifierValue(float p[23], int IsNaF, TString method){
	double bdt = -2;
	int imethod = -1;

	if (method == "BDT") imethod = 0;
	else if (method == "BDTD") imethod = 1;
	else if (method == "BDTG") imethod = 2;

	if (imethod < 0) return -2; 

	if (!(IsNaF == 0 || IsNaF == 1)) {
		return -3;
	}

	if (!TrackerBDTreader[IsNaF]){
		TrackerBDTreader[IsNaF] = new TMVA::Reader("!Color:!Silent");
		TrackerBDTreader[IsNaF]->AddVariable("hsv", &TrackerBDTvar[0]);
		TrackerBDTreader[IsNaF]->AddVariable("log10(chi2y)", &TrackerBDTvar[1]);
		TrackerBDTreader[IsNaF]->AddVariable("log10(chi2x)", &TrackerBDTvar[2]);
		TrackerBDTreader[IsNaF]->AddVariable("log10(chi2x_a)", &TrackerBDTvar[3]);
		TrackerBDTreader[IsNaF]->AddVariable("log10(chi2y_a)", &TrackerBDTvar[4]);
		TrackerBDTreader[IsNaF]->AddVariable("log10(chi2y_kf)", &TrackerBDTvar[5]);
		TrackerBDTreader[IsNaF]->AddVariable("log10(chi2x_kf)", &TrackerBDTvar[6]);
		TrackerBDTreader[IsNaF]->AddVariable("rig_kf>0?1/rig_ch-1/rig_kf:1/rig_kf-1/rig_ch", &TrackerBDTvar[7]);
		TrackerBDTreader[IsNaF]->AddVariable("rig_kf>0?1/rig_kf-1/rig_a:1/rig_a-1/rig_kf", &TrackerBDTvar[8]);
		TrackerBDTreader[IsNaF]->AddVariable("rig_kf>0?1/rig_kf-1/rig:1/rig-1/rig_kf", &TrackerBDTvar[9]);
		TrackerBDTreader[IsNaF]->AddVariable("smin", &TrackerBDTvar[10]);
		TrackerBDTreader[IsNaF]->AddVariable("minfeetdist", &TrackerBDTvar[11]);
		TrackerBDTreader[IsNaF]->AddVariable("trk_qasym", &TrackerBDTvar[12]);
		TrackerBDTreader[IsNaF]->AddVariable("qlaymax/qlaymin", &TrackerBDTvar[13]);
		TrackerBDTreader[IsNaF]->AddVariable("qlay_y_max/qlay_y_min", &TrackerBDTvar[14]);
		TrackerBDTreader[IsNaF]->AddVariable("abs(mindisty_sort[0])<0.01?1:log10(abs(mindisty_sort[0]))", &TrackerBDTvar[15]);
		TrackerBDTreader[IsNaF]->AddVariable("log10(maxresy)", &TrackerBDTvar[16]);
		TrackerBDTreader[IsNaF]->AddVariable("log10(maxresx)", &TrackerBDTvar[17]);
		TrackerBDTreader[IsNaF]->AddVariable("trk_yqinlow+trk_yqinup>0?(trk_yqinlow-trk_yqinup)/(trk_yqinlow+trk_yqinup):1", &TrackerBDTvar[18]);
		TrackerBDTreader[IsNaF]->AddVariable("trk_q78", &TrackerBDTvar[19]);
		TrackerBDTreader[IsNaF]->AddVariable("trk_yq78", &TrackerBDTvar[20]);
		TrackerBDTreader[IsNaF]->AddVariable("q_tofu+q_tofd>0?(q_tofu-q_tofd)/(q_tofu+q_tofd):1", &TrackerBDTvar[21]);
		TrackerBDTreader[IsNaF]->AddVariable("sqrt(TrkTrdMatch*TrkTrdMatch-TrkTrdMatchy*TrkTrdMatchy)", &TrackerBDTvar[22]);
		TrackerBDTreader[IsNaF]->AddVariable("TrkTrdMatchy", &TrackerBDTvar[23]);
		TrackerBDTreader[IsNaF]->AddVariable("trk_nhitx", &TrackerBDTvar[24]);
		TrackerBDTreader[IsNaF]->AddVariable("trk_nhity", &TrackerBDTvar[25]);
		TrackerBDTreader[IsNaF]->AddVariable("trk_nnoisehits", &TrackerBDTvar[26]);
		TrackerBDTreader[IsNaF]->AddVariable("ntofintime", &TrackerBDTvar[27]);
		TrackerBDTreader[IsNaF]->AddVariable("ntofhits", &TrackerBDTvar[28]);
		TrackerBDTreader[IsNaF]->AddVariable("nACC", &TrackerBDTvar[29]);
		TrackerBDTreader[IsNaF]->AddVariable("ntrdsegtrk", &TrackerBDTvar[30]);

		/*	TrackerBDTreader[IsNaF]->AddVariable("log10(trk.chi2y[1])", &TrackerBDTvar[0]);
			TrackerBDTreader[IsNaF]->AddVariable("log10(trk.chi2x[1])", &TrackerBDTvar[1]);
			TrackerBDTreader[IsNaF]->AddVariable("trk.rig[1]>0?1/trk.rig_ck[1]-1/trk.rig[1]:1/trk.rig[1]-1/trk.rig_ck[1]", &TrackerBDTvar[2]);
			TrackerBDTreader[IsNaF]->AddVariable("trk.rig[1]>0?1/trk.rig_al[1]-1/trk.rig[1]:1/trk.rig[1]-1/trk.rig_al[1]", &TrackerBDTvar[3]);
			TrackerBDTreader[IsNaF]->AddVariable("trk.rig[1]>0?1/trk.rig[4]-1/trk.rig[5]:1/trk.rig[5]-1/trk.rig[4]", &TrackerBDTvar[4]);
			TrackerBDTreader[IsNaF]->AddVariable("trk.srmin", &TrackerBDTvar[5]);
			TrackerBDTreader[IsNaF]->AddVariable("trk.minfeetdist", &TrackerBDTvar[6]);
			TrackerBDTreader[IsNaF]->AddVariable("trk.qinup+trk.qinlow>0?(trk.qinlow-trk.qinup)/(trk.qinup+trk.qinlow):1", &TrackerBDTvar[7]);
			TrackerBDTreader[IsNaF]->AddVariable("trk.qmax/trk.qmin", &TrackerBDTvar[8]);
			TrackerBDTreader[IsNaF]->AddVariable("trk.yqmax/trk.yqmin", &TrackerBDTvar[9]);
			TrackerBDTreader[IsNaF]->AddVariable("abs(trk.mindisty)<0.01?1:log10(abs(trk.mindisty))", &TrackerBDTvar[10]);
			TrackerBDTreader[IsNaF]->AddVariable("log10(trk.maxresy)", &TrackerBDTvar[11]);
			TrackerBDTreader[IsNaF]->AddVariable("log10(trk.maxresx)", &TrackerBDTvar[12]);
			TrackerBDTreader[IsNaF]->AddVariable("trk_yqinlow+trk_yqinup>0?(trk_yqinlow-trk_yqinup)/(trk_yqinlow+trk_yqinup):1", &TrackerBDTvar[13]);
			TrackerBDTreader[IsNaF]->AddVariable("trk_q78", &TrackerBDTvar[14]);
			TrackerBDTreader[IsNaF]->AddVariable("trk_yq78", &TrackerBDTvar[15]);
			TrackerBDTreader[IsNaF]->AddVariable("tof.qlow+tof.qup>0?(tof.qlow-tof.qup)/(tof.qlow+tof.qup):1", &TrackerBDTvar[16]);
			TrackerBDTreader[IsNaF]->AddVariable("log10(abs(trk.xL1-trd.xL1))", &TrackerBDTvar[17]);
			TrackerBDTreader[IsNaF]->AddVariable("log10(abs(trk.yL1-trd.yL1))", &TrackerBDTvar[18]);
			TrackerBDTreader[IsNaF]->AddVariable("trk.nhitx", &TrackerBDTvar[19]);
			TrackerBDTreader[IsNaF]->AddVariable("trk.nhity", &TrackerBDTvar[20]);
			TrackerBDTreader[IsNaF]->AddVariable("trk.hasnoisehit", &TrackerBDTvar[21]);
			TrackerBDTreader[IsNaF]->AddVariable("ntofhits", &TrackerBDTvar[22]);*/


		if (IsNaF){ 
			TrackerBDTreader[IsNaF]->BookMVA("BDT method", "/afs/cern.ch/user/j/jfeng/public/Betalhd/trk_bdt/TMVAnalysis_iss_naf_trk_BDT.weights.xml");
			TrackerBDTreader[IsNaF]->BookMVA("BDTD method", "/afs/cern.ch/user/j/jfeng/public/Betalhd/trk_bdt/TMVAnalysis_iss_naf_trk_BDTD.weights.xml");
			TrackerBDTreader[IsNaF]->BookMVA("BDTG method", "/afs/cern.ch/user/j/jfeng/public/Betalhd/trk_bdt/TMVAnalysis_iss_naf_trk_BDTG.weights.xml");
		}else{
			TrackerBDTreader[IsNaF]->BookMVA("BDT method", "/afs/cern.ch/user/j/jfeng/public/Betalhd/trk_bdt/TMVAnalysis_iss_agl_trk_BDT.weights.xml");
			TrackerBDTreader[IsNaF]->BookMVA("BDTD method", "/afs/cern.ch/user/j/jfeng/public/Betalhd/trk_bdt/TMVAnalysis_iss_agl_trk_BDTD.weights.xml");
			TrackerBDTreader[IsNaF]->BookMVA("BDTG method", "/afs/cern.ch/user/j/jfeng/public/Betalhd/trk_bdt/TMVAnalysis_iss_agl_trk_BDTG.weights.xml");
		}
	}
	for (int i = 0; i < 31 ;i++){
		//for (int i = 0; i < 23 ;i++){
		TrackerBDTvar[i] = p[i];
	}

	bdt = TrackerBDTreader[IsNaF] -> EvaluateMVA(Form("%s method", method.Data()));
	return bdt;
	}

	vector<float> Betalhd::hit_beta[2];
	float Betalhd::sigma_naf[2] = {7.44054e-03, 7.44054e-03};
	float Betalhd::sigma_agl[2] = {2.56912e-03, 2.56912e-03};
	//float Betalhd::sigma_naf[2] = {7.44054e-03, 8.10304e-03};
	//float Betalhd::sigma_agl[2] = {2.56912e-03, 2.47688e-03};
	int Betalhd::rich_usedhits_direct;
	int Betalhd::rich_usedhits_mirror;
	bool Betalhd::update_variable;
	TFile * Betalhd::rich_xy_correction_file;
	TProfile2D * Betalhd::rich_xy_agl;
	TProfile2D * Betalhd::rich_xy_naf;
	TProfile2D * Betalhd::rich_vxvy_agl;
	TProfile2D * Betalhd::rich_vxvy_naf;
	bool Betalhd::init_rich_correction(TString file){
		if (file == 0) return false;
		TDirectory *dsave = gDirectory;
		rich_xy_correction_file = TFile::Open(file);
		if (dsave) dsave->cd();
		rich_xy_agl = (TProfile2D*) rich_xy_correction_file->Get("tilebeta_agl");
		rich_xy_naf = (TProfile2D*) rich_xy_correction_file->Get("tilebeta_naf");
		rich_vxvy_agl = (TProfile2D*) rich_xy_correction_file->Get("vxvy_agl");
		rich_vxvy_naf = (TProfile2D*) rich_xy_correction_file->Get("vxvy_naf");
		return true;
	}
	double Betalhd::get_rich_xy_correction(double x, double y, bool IsNaF){
		double corr = 1;
		if (IsNaF){
			int bin = rich_xy_naf->FindBin(x,y);
			corr = 1./rich_xy_naf->GetBinContent(bin);
			if (corr > 1.01 || corr < 0.99) corr = 1;
		}else{
			int bin = rich_xy_agl->FindBin(x,y);
			corr = 1./rich_xy_agl->GetBinContent(bin);
			if (corr > 1.01 || corr < 0.99) corr = 1;
		}
		return corr;
	}
	double Betalhd::get_rich_vxvy_correction(double vx, double vy, bool IsNaF){
		double corr = 1;
		if (IsNaF){
			int bin = rich_vxvy_naf->FindBin(vx,vy);
			corr = 1./rich_vxvy_naf->GetBinContent(bin);
			if (corr > 1.01 || corr < 0.99) corr = 1;
		}else{
			int bin = rich_vxvy_agl->FindBin(vx,vy);
			corr = 1./rich_vxvy_agl->GetBinContent(bin);
			if (corr > 1.01 || corr < 0.99) corr = 1;
		}
		return corr;
	}
	TF1* Betalhd::tf_time_corr = NULL;
	TF1* Betalhd::tf_time_corr_naf = NULL;
	double Betalhd::get_time_corrected_beta(float beta, int ut, bool IsNaF){
		float beta_o = beta;
		float n;
		if (tf_time_corr==NULL){
			tf_time_corr = new TF1("tf_time_corr","1+[0]*TMath::Sin(2*TMath::Pi()*x/[1]+[2])+[3]*TMath::Sin(2*TMath::Pi()*x/[4]+[5])",1305853512, 1727492566);
			tf_time_corr_naf = new TF1("tf_time_corr_naf","1+[0]*TMath::Sin(2*TMath::Pi()*x/[1]+[2])+[3]*TMath::Sin(2*TMath::Pi()*x/[4]+[5])",1305853512, 1727492566);
			double sin[6] = {-1.471336139e-05, 5215434.563, 9.005796473, -6.914233269e-06, 31104153.44, 0.7470427487};
			double sin_naf[6] = {5.238899423e-05, 5221926.611, 11.26513244, 2.628080118e-05, 31248138.39, 1.674107684};
			tf_time_corr -> SetParameters(sin);	
			tf_time_corr_naf -> SetParameters(sin_naf);
		}
		double corr = 1;
		if (IsNaF){
			corr = 1.002657478 - ut*1.788460472e-12;
			n = 1.33;
		}else{
			corr = 1.003370909 - ut*2.461270015e-12;
			n = 1.05;
		}
		beta = 1/n*pow((1/n/n/beta_o/beta_o-1)/corr+1,-0.5);
		if (IsNaF) beta /=  tf_time_corr_naf-> Eval(ut);
		else beta /= tf_time_corr-> Eval(ut);
		return beta;
	}

	double Betalhd::get_rich_correction(double x, double y, double vx, double vy, bool IsNaF){
		double corr = 1.;
		corr *= get_rich_xy_correction(x, y, IsNaF);
		corr *= get_rich_vxvy_correction(vx, vy, IsNaF);
		return corr;
	}
	int Betalhd::GetTOFResidual(float ibeta[4], float ibeta_err[4], float residual[4], float residual_err[4], float Time[4], float TimeE[4], float PathLength[4]){
		double ibeta_d[4], ibeta_err_d[4], residual_d[4], residual_err_d[4], Time_d[4], TimeE_d[4], PathLength_d[4];
		for (int i = 0; i < 4; i++) {
			ibeta_d[i] = ibeta[i];
			ibeta_err_d[i] = ibeta_err[i];
			residual_d[i] = residual[i];
			residual_err_d[i] = residual_err[i];
			Time_d[i] = Time[i];
			TimeE_d[i] = TimeE[i];
			PathLength_d[i] = PathLength[i];
		}
		int result = GetTOFResidual(ibeta_d, ibeta_err_d, residual_d, residual_err_d, Time_d, TimeE_d, PathLength_d);
		for (int i = 0; i < 4; i++) {
			ibeta[i] = ibeta_d[i];
			ibeta_err[i] = ibeta_err_d[i];
			residual[i] = residual_d[i];
			residual_err[i] = residual_err_d[i];
			Time[i] = Time_d[i];
			TimeE[i] = TimeE_d[i];
			PathLength[i] = PathLength_d[i];
		}
		return result;
	}
	int Betalhd::GetTOFResidual(double ibeta[4], double ibeta_err[4], double residual[4], double residual_err[4], double Time[4], double TimeE[4], double PathLength[4]){
		double cval = 29.9792458; // cm/ns
		TF1 * tf1 = new TF1("tf1", "pol1", -300, 300);
		TGraphErrors * gr_fit = NULL; 
		if (extrapolation) gr_fit = new TGraphErrors(4, PathLength, PathLength, 0, 0);
		for (int ilay = 0 ; ilay < 4; ilay++){
			int hit = 0;
			for (int i = 0; i < 4; i++) if (i != ilay && Time[i] !=0 ){
				hit++;
			}
			const int nhit = hit;
			double x[nhit], y[nhit], ye[nhit];
			int ihit = 0;
			for (int i = 0; i < 4; i++) if (i != ilay && Time[i] !=0 ){
				x[ihit] = PathLength[i];
				y[ihit] = Time[i];
				ye[ihit] = TimeE[i];
				ihit++;
			}
			TGraphErrors gr_tof(nhit, x, y, 0, ye);
			gr_tof.Fit(tf1,"QC");
			ibeta[ilay] = -tf1->GetParameter(1)*cval;
			residual[ilay] = Time[ilay] - tf1->Eval(PathLength[ilay]);
			ibeta_err[ilay] = tf1->GetParError(1)*cval;
			if (extrapolation){	
				(TVirtualFitter::GetFitter())->GetConfidenceIntervals(gr_fit, 0.68);
				residual_err[ilay] = gr_fit->GetErrorY(ilay);
			}
		}
		delete tf1;
		if (extrapolation) delete gr_fit;
		tf1 = NULL;
		return 1;
	}
	bool Betalhd::extrapolation = true;

	double Betalhd::GetTOFBeta_linear(float & ibeta, float & ibeta_err, float Time[4], float TimeE[4], float PathLength[4]){
		double cval = 29.9792458; // cm/ns
		TGraphErrors gr_tof(4, PathLength, Time, 0, TimeE);
		//gr_tof.Print();
		for (int i = 0; i < 4; i++){
			if (!(Time[i] < 0)) {cout << "Remove " << i <<endl;gr_tof.RemovePoint(i); break; }
		}
		//gr_tof.Print();
		TF1 * tf1 = new TF1("tf1", "pol1", -300, 300);
		gr_tof.Fit(tf1,"Q");

		ibeta = -tf1->GetParameter(1)*cval;
		ibeta_err = tf1->GetParError(1)*cval;
		//cout << "fitted beta = " << ibeta << " +/- " << ibeta_err << endl;
		double chi2 = tf1 -> GetChisquare()/2;
		delete tf1;
		tf1 = NULL;
		return chi2;
	}

	TF1 * Betalhd::spline_TOF_time[4] = {NULL,NULL,NULL,NULL};
	double Betalhd::GetTOFTimeCorrection_UT(int ilayer, int UT){
		//TOF Time dependent Time correction
		if (IsMC) return 0;
		int ut_time_min = 1306100000;
		int ut_time_max = 1600742800;
		if (spline_TOF_time[0]==NULL){
			double par[4][8]={
				//1385487667, 1385487717, 1385487767, 0.0047628545438720042496, -6797648.6942953234538, 0.0028779575437344159967, 0, -1.3533266416660442719e-11, 
				//1385487667, 1385487717, 1385487767, -0.0027747234584159499379, -5989271.0187677936628, 0.0031589671151370669194, 0, 1.2840347842302378268e-11, 
				//1385487667, 1385487717, 1385487767, -0.0039204822765760895947, -7704814.9518095515668, -0.0054100749602775128483, 0, 1.2236095005520529774e-12, 
				//1385487667, 1385487717, 1385487767, -0.0010001399839859935297, -6462271.1040119901299, -0.0036847746730210421441, 0, 5.3754022889698022095e-13, //V1
				//1385487667, 1385487717, 1385487767, 0.0050774076597789145893, -19803890.016201820225, 0.002237193268503078012, 2.4317385347437052955e-11, -9.3833409866515090314e-12,
				//1385487667, 1385487717, 1385487767, 0.00010401673334775458067, -14130845.81167848222, 0.0030043852943897374555, 3.9126237981474215367e-11, 8.044717470917213234e-12,
				//1385487667, 1385487717, 1385487767, -0.0049299401429504004918, -18890870.294143032283, -0.0041265865922577278993, -4.9150847493308924012e-11, 6.1851225800716835186e-13,
				//1385487667, 1385487717, 1385487767, -0.0019394995367809574841, -21155839.407275423408, -0.0031569718811289833142, -4.5335687668596970705e-11, 1.7190564680717903838e-12,//V2
				//1385487667, 1385487717, 1385487767, 0.0060393880466810389435, -21152365.862504534423, 0.0024738828831472208826, 3.8735459634883017045e-11, -1.3317949655094797258e-11,
				//1385487667, 1385487717, 1385487767, -0.00070518001551376000274, -14365757.360724683851, 0.0028546366256767922998, 5.0338890156748170005e-11, 1.2439728032377619054e-11,
				//1385487667, 1385487717, 1385487767, -0.0059494956390205716579, -19658896.683442182839, -0.0048314540533803051678, -5.5720475166565229319e-11, 1.4685484369652920995e-12,
				//1385487667, 1385487717, 1385487767, -0.0022528377799334105454, -20703345.449729111046, -0.0032221776347308166318, -3.5593958590770060966e-11, 7.819754766863640667e-13, //V3
				//1385487667, 1385487717, 1385487767, 0.0017958582970129788846, -20637623.748832147568, -0.00084372071321452699402, 4.0556921498699655997e-11, -3.4110365950871045692e-12,
				//1385487667, 1385487717, 1385487767, -0.00041624089700709670592, -13760281.685120616108, -0.0017581427861851793223, 5.9659935744890758103e-11, 6.5921287833636742689e-12,
				//1385487667, 1385487717, 1385487767, -0.001187159000938099989, -19126008.369531236589, 0.0017746082201164138127, -5.9849696504055299531e-11, -1.6017131535932164885e-12,
				//1385487667, 1385487717, 1385487767, -0.0012173844365910943977, -20218640.027851406485, 0.00050588575292864550962, -3.7237393549543759762e-11, -5.278989797788849481e-13,//V4
				//1385487667, 1385487717, 1385487767, 0.0072622690587010579194, -20632882.466235499829, 0.0037078352321839374781, 4.0774299069346097241e-11, -1.595898986828123998e-11, 
				//1385487667, 1385487717, 1385487767, -0.0020607706060323375576, -13761475.800048504025, 0.001829507622433691142, 5.9808312473198989362e-11, 1.7172367514644974689e-11, 
				//1385487667, 1385487717, 1385487767, -0.0047972880567166628629, -19136070.211677391082, -0.0040251086925357521668, -5.9912816359564555612e-11, -2.5304051404485909518e-13, 
				//1385487667, 1385487717, 1385487767, -0.0016522898365779531069, -20217429.048224363476, -0.0028164453511673780486, -3.6969158241420217493e-11, -1.7644631968123090232e-14, //V5
				1385487667, 1385487717, 1385487767, 0.003787341570634455111, -19634909.207863740623, -0.00018588178790131295042, 1.677147792647434923e-11, -1.7785829899288121316e-11, 
				1385487667, 1385487717, 1385487767, -0.002764401422427657188, -14444924.969717685133, 0.001432688014449824904, 5.0284353757095019771e-11, 1.5918593575658537521e-11, 
				1385487667, 1385487717, 1385487767, -0.001280704083122289564, -18189194.820693287998, -0.00031186030422830927824, -4.5681577697978467921e-11, 8.8009681077462204145e-13, 
				1385487667, 1385487717, 1385487767, 0.00018736408739221757126, -19984538.747167300433, -0.00076780250121444423503, -2.9124843962560871677e-11, 1.6209892286736299659e-12, //v6.14
			};
			SplFit::fBlxU = 1;
			SplFit::fBlxL = 1;//v1,v2
			SplFit::fN = 3;
			spline_TOF_time[0] = new TF1(Form("spline_TOF_time_%d",0), SplFit::SpFunc, ut_time_min, ut_time_max, 8);
			spline_TOF_time[1] = new TF1(Form("spline_TOF_time_%d",1), SplFit::SpFunc, ut_time_min, ut_time_max, 8);
			spline_TOF_time[2] = new TF1(Form("spline_TOF_time_%d",2), SplFit::SpFunc, ut_time_min, ut_time_max, 8);
			spline_TOF_time[3] = new TF1(Form("spline_TOF_time_%d",3), SplFit::SpFunc, ut_time_min, ut_time_max, 8);
			for (int i = 0;i < 4; i++) {
				for (int j = 0 ; j < 8; j++){
					spline_TOF_time[i] -> SetParameter(j, par[i][j]);
				}
			}

		}
		if (!spline_TOF_time[0] ) {
			return 0;
		}
		if (UT > ut_time_max) UT = ut_time_max;
		if (ilayer < 0) ilayer = 0;
		if (ilayer > 3) ilayer = 3;
		return 	spline_TOF_time[ilayer]->Eval(UT);
	}
	/*GeomHashBetaEnsemble * Betalhd::TOFTimeHash[4] = {NULL, NULL, NULL, NULL};
	  TFile * Betalhd::file_TOFTime = NULL;
	  double Betalhd::GetTOFTimeCorrection_coo(int ilayer, float x, float y, float vx, float vy){
	  if (TOFTimeHash[0] == NULL){
	  if (file_TOFTime == NULL){
	  TDirectory *dsave = gDirectory;
	  file_TOFTime = TFile::Open("root://eosams.cern.ch//eos/ams/group/mitep/Ntuple/ForAntiD/TOFhash/v0/BetaHashes_v0_0_2.root");
	  if (dsave) dsave->cd();
	  }
	  for (int i = 0; i < 4; i++) 
	//TOFTimeHash[i] = (GeomHashBetaEnsemble *)file_TOFTime->Get(Form("TOFTimeHash_%d",i));
	TOFTimeHash[i] = (GeomHashBetaEnsemble *)file_TOFTime->Get(Form("hash_%d",i));
	}
	float boundary[4][2] = { //TOF Geometry
	65, 60,
	60, 55,
	65, 60,
	70, 62,
	};
	if (!( fabs(x)< boundary[ilayer][0] && fabs(y)< boundary[ilayer][1])) return 0;
	TOFTimeHash[ilayer]->Eval(x, y, vx, vy);
	double result = TOFTimeHash[ilayer]->MeanPeak;
//cout << "Hash table peak = " << result << endl;
return result;
}*/
TH2D * Betalhd::h_xy_corr[4] = {NULL, NULL, NULL, NULL}; //v6.01 above
TFile * Betalhd::file_TOFCoo = NULL;
double Betalhd::GetTOFTimeCorrection_coo(int ilayer, float x, float y){
	float boundary[4][2] = { //TOF Geometry
		65, 60,
		60, 55,
		65, 60,
		70, 62,
	};
	if (!( fabs(x)< boundary[ilayer][0] && fabs(y)< boundary[ilayer][1])) return 0;
	if (h_xy_corr[0] == 0){
		if (file_TOFCoo == NULL){
			TDirectory *dsave = gDirectory;
			TString sfn = AMSDATADIR.amsdatadir; 
			if (!IsMC){ 
				sfn += "LAPP/tof_correction/tof_xy_correction_v6.15.root";
				file_TOFCoo = TFile::Open(sfn);
				//file_TOFCoo = TFile::Open("/afs/cern.ch/work/j/jfeng/private/ISS.B1130/analysis/time_beta_tile/tof_xy_correction_v6.15.root");
			}
			else {
				sfn += "LAPP/tof_correction/tof_xy_correction_mc_v3.11.root";
				file_TOFCoo = TFile::Open(sfn);
				//file_TOFCoo = TFile::Open("/afs/cern.ch/work/j/jfeng/private/MC/rich_analysis/time_beta_tile/tof_xy_correction_mc_v3.11.root");
			}
			if (dsave) dsave->cd();
		}
		for (int i = 0; i < 4; i++) 
			h_xy_corr[i] = (TH2D*)file_TOFCoo->Get(Form("h_xy_corr_%d",i));//v6.01
	}
	int ibin =  h_xy_corr[ilayer]->FindBin(x, y);
	double result = h_xy_corr[ilayer] -> GetBinContent(ibin);
	//cout << "correction " << ilayer << ": " << result << endl;
	return result;
}

void Betalhd::MCtuneTime(float & time, int ilayer, int RUN, int EVENT){
	double rnd[1];
	int key = 55274996+ilayer;
	int seed = 4357;
	if (RUN==0 && EVENT==0){
		seed = 0;
	}else{
		TRandom3 rndRun(RUN);
		seed  = rndRun.Integer(0x7fffffff);
		TRandom3 rndEvent(EVENT);
		seed ^= rndEvent.Integer(0x7fffffff);

		TRandom3 rnd1(key);
		seed ^= rnd1.Integer(0x7fffffff);
	}
	TRandom3 rand(seed);
	rnd[0] = rand.Gaus();
	//cout << " : " << time << " + " << rnd[0] << "*" << fabs(Betalhd::MCtunePar_width)/1000. <<  endl;
	time+=rnd[0]*fabs(MCtunePar_width)/1000.;//ps->ns
	time+=MCtunePar_shift/1000.;//ps->ns
}
double Betalhd::MCtunePar_width = 0;
double Betalhd::MCtunePar_shift = 0;

bool Betalhd::IsTrackerOffConf(TrTrackR *track, int & patt_i,  int & patt, int iconf){
	bool ishit[9] = {1,0,0,0,0,0,0,0,0 };
	if(iconf == 1){// conf.E
		int tke[46] = {
			-106, 106,-107,107, -108,108,-109,109, -110,110,
			-207,207,-209,209,  -307,307, -309,309, -407,407,
			-409,409, -507,507, -509,509, -607,607, -609,609,
			-706, 706, -707,707, -709,709, -710,710,
			-903, 903, -904, 904, -905, 905, -906,906
		};
		for(int il=0; il<9;il++){
			TrRecHitR* hit = track->GetHitLJ(il);
			if (hit == NULL) continue;
			int tkid1 = hit->GetTkId();
			for(int i=0;i<46;i++){
				if( tkid1 == tke[i]  ) {
					ishit[il] = 1;
					break;
				}
			}
		}
	}
	int nhit = 0;
	for(int il=0;il<9;il++){
		if (ishit[il]) {
			if (il > 0 && il < 8){
				patt_i += 9*pow(10.,il);
				nhit++;
			}
			patt += 9*pow(10.,il);
		}
	}
	if(nhit < 4 ) return false;
	else return true;
}
