#include "DeadSideCorr.h"

#include <TMVA/Version.h>
#include "commonsi.h"

TMinuit* DeadSideCorr::minuit1 = NULL;

DeadSideCorr::DeadSideCorr()
{
    ReSet();
    reader = NULL;
    readerSmoothed = NULL;
}

DeadSideCorr::~DeadSideCorr()
{
    ReSet();
    delete reader;
    delete readerSmoothed;
}

void DeadSideCorr::ReSet()
{
    DataType = 0;
    CooNew[0] = -9999; CooNew[1] = -9999; CooNew[2] = -9999;
    KCR[0] = -9999; KCR[1] = -9999;
    memset(edepattcorr,0,sizeof(edepattcorr));
    memset(elayer_corr,0,sizeof(elayer_corr));
    memset(status,0,sizeof(status));
    memset(ecalcog,0,sizeof(ecalcog));
    memset(EdepRecorr,0,sizeof(EdepRecorr));
    memset(ElayerRecorr,0,sizeof(ElayerRecorr));
    EnergyRecorr = 0;
    memset(ElayerLongFit,0,sizeof(ElayerLongFit));
    memset(DeadStatus,0,sizeof(DeadStatus));
    memset(coocrlayer,0,sizeof(coocrlayer));
    memset(Numcoocrlayer,0,sizeof(Numcoocrlayer));
    memset(coocrfiber,0,sizeof(coocrfiber));
    memset(coocrreadout,0,sizeof(coocrreadout));//wxq

    DeadIDL.clear();
    DeadIDC.clear();
    tmpdeadtotal = 0;

    memset(LateralLeak,0,sizeof(LateralLeak));//wxq

    efraclast2layers = 0;
    depositedenergy = 0;
    EnergyPIC = 0;
    RearLeakPI = 0;
    corr_factor = 0;
    correction_factor = 0;

    E3C0 = 0;
    CorrEnergyD = 0;

    //==========ECAL BDT==========//
    memset(ADCH,0,sizeof(ADCH));
    memset(MapEneDep,0,sizeof(MapEneDep));
    memset(LayerEneDep,0,sizeof(LayerEneDep));// Energy deposit [GeV] in each layer (sum of every cell of each layer)
    EneDep=0.;
    EneDep_GeV=0.;
    memset(EneDepXY,0,sizeof(EneDepXY)); // 0 = x, 1 = y
    memset(LayerMean,0,sizeof(LayerMean)); // Mean [cell] for each layer: (sum_j j*MapEneDep[i][j])/(sum_ij MapEneDep[i][j])
    memset(LayerSigma,0,sizeof(LayerSigma));
    memset(LayerSigmaNorm,0,sizeof(LayerSigmaNorm));
    memset(LayerEneFrac,0,sizeof(LayerEneFrac));
    memset(LayerEneFracNorm,0,sizeof(LayerEneFracNorm));
    for(int il=0;il<18;il++)
    {    
        LayerS1S3[il]   = 1.;
        LayerS3Frac[il] = 1.;
    }
    memset(LayerS3FracNorm,0,sizeof(LayerS3FracNorm));
    ShowerMean=0.;
    ShowerMeanNorm=0.;
    L2LFrac=0.;
    L2LFracNorm=0.;
    S3totx=0.;
    S3toty=0.;
    S3totxNorm=0.;
    S3totyNorm=0.;
    R3cmFrac=0.;
    R3cmFracNorm=0.;
    ShowerFootprintX=0.;
    ShowerFootprintY=0.;
    ShowerFootprintXNorm=0.;
    ShowerFootprintYNorm=0.;
    NEcalHits=0.;
    NEcalHitsNorm=0.;
    memset(ShowerMeanXY,0,sizeof(ShowerMeanXY)); // 0 = x, 1 = y
    memset(sigmaXY,0,sizeof(sigmaXY));  // 0 = x, 1 = y
    memset(sigmaXYZ,0,sizeof(sigmaXYZ)); // 0 = x, 1 = y
    memset(sigmaZ,0,sizeof(sigmaZ));   // 0 = x, 1 = y
    F2SLEneDep=0.;
    memset(imaxcell,0,sizeof(imaxcell));
    run=0;
    event=0;
    read_bdt=0.;

    //=========LongFit==========//
    longchi2 = 0.;
    memset(ecclEdep,0,sizeof(ecclEdep));

    // nz changes
    S13LeakXPI = 0.;
    S13LeakYPI = 0.;
}

float DeadSideCorr::ReCalEnergyE()
{
    efraclast2layers = 0;
    depositedenergy = 0;
    float EnergyLayer[18] = {0.};
    float S1S3X[3] = {0.529, 0.753, 0.922};
    float S1S3Y[3] = {0.517, 0.743, 0.933};
    float S13Rpi[2] = {0.};
    float ss1p[2]={0.};
    float ss3p[2]={0.};

    float edep_xy[2] ={0.}; //energy in each side: 0 - x side, 1 - yside
    int iMaxCell[18]={0};
    float maxcelledep[18]={0.};
    for(int il=0;il<18;il++)
    {
        int proj = 1-il/2%2;   //0 for x, 1 for y;
        for(int ic=0;ic<72;ic++)
        {
            EnergyLayer[il] += EdepRecorr[il][ic];
            edep_xy[proj] += EdepRecorr[il][ic];
            if(EdepRecorr[il][ic]>maxcelledep[il])
            {
                maxcelledep[il] = EdepRecorr[il][ic];
                iMaxCell[il] = ic;
            }
        }

        if(iMaxCell[il]<9)
        {
            for(int k=2*iMaxCell[il]+1;k<=iMaxCell[il]+9;k++)
            {
                EnergyLayer[il] += EdepRecorr[il][k];
                edep_xy[proj] += EdepRecorr[il][k];
            }
        }
        if(iMaxCell[il]>62)
        {
            for(int k=iMaxCell[il]-9;k<=2*iMaxCell[il]-72;k++)
            {
                EnergyLayer[il] += EdepRecorr[il][k];
                edep_xy[proj] += EdepRecorr[il][k];
            }
        }
    }

    for(int il=0;il<18;il++)
    {
        int proj = 1-il/2%2;   //0 for x, 1 for y;
        ss1p[proj] += maxcelledep[il];
        ss3p[proj] += maxcelledep[il];
        if(iMaxCell[il]>0) ss3p[proj] += EdepRecorr[il][iMaxCell[il]-1];
        if(iMaxCell[il]<71) ss3p[proj] += EdepRecorr[il][iMaxCell[il]+1];
    }

    if(ss3p[0]>0.) S13Rpi[0]=ss1p[0]/ss3p[0];
    if(ss3p[1]>0.) S13Rpi[1]=ss1p[1]/ss3p[1];

    if(S13Rpi[0]>S1S3X[0] && S13Rpi[0]<S1S3X[1])
    {    
        S13LeakXPI=((1-S1S3X[2])/(S1S3X[1]-S1S3X[0]))*(S13Rpi[0]-S1S3X[1]);
    }    
    else if(S13Rpi[0]<S1S3X[0])
        S13LeakXPI=S1S3X[2] -1;
   
    if(S13Rpi[1]>S1S3Y[0] && S13Rpi[1]<S1S3Y[1])
    {    
        S13LeakYPI=((1-S1S3Y[2])/(S1S3Y[1]-S1S3Y[0]))*(S13Rpi[1]-S1S3Y[1]);
    }    
    else if(S13Rpi[1]<S1S3Y[0])
        S13LeakYPI=S1S3Y[2] -1;

    // Deposited energy corrected for anode efficiency (in GeV)
    depositedenergy = (edep_xy[0]/(1.0+S13LeakXPI) + edep_xy[1]/(1.0+S13LeakYPI))/1000.;
    //depositedenergy is in GeV,  EnergyLayer are in MeV, S13LeakYPI is the correction factor for Y-side
    efraclast2layers = (EnergyLayer[16] +  EnergyLayer[17])/1000./(1.0+S13LeakYPI)/depositedenergy;

    EnergyPIC = 0.;
    RearLeakPI = 0.;
    if(efraclast2layers>0.)
    {
	float EshiftInPercent;
	float ecorr2ebeam;
	//MI new EnergyE 5 Sep 2016
	float alpha0 = -0.053; 
	float alpha1 = 0.000182; 
	float beta0 = 0.65; 
	float beta1 = -0.000998;
	float gamma0 = 7.91;
	float gamma1 = -0.00248;
	//define EnergyE
	float EnewGeV = depositedenergy * (alpha0 + beta0*efraclast2layers + gamma0 *efraclast2layers*efraclast2layers +1)/(1-depositedenergy*(alpha1+beta1*efraclast2layers+gamma1*efraclast2layers*efraclast2layers));

	ecorr2ebeam = EnewGeV/depositedenergy;
	float miEshift = 100.*(ecorr2ebeam-1.);

	//lower and upper limits to leakage corrections
	//do not apply hard cuts but reduce the phase space
	if(miEshift>30.)miEshift=30.+pow(double(miEshift-30),0.75);
	if(miEshift<-1.)miEshift=-1.*pow(-1.*miEshift,0.5);

	EshiftInPercent = miEshift;
	//MI	
	
	ecorr2ebeam=1.+EshiftInPercent/100.;
	EnergyPIC=depositedenergy*ecorr2ebeam;
	RearLeakPI= EnergyPIC - depositedenergy;
    }
    else
    {
	EnergyPIC= depositedenergy;
	RearLeakPI=0.;
    }

    return EnergyPIC;
}

float DeadSideCorr::ReCalEnergyD()
{
    int CellMax[18]={0};
    float tmpedepmax[18]={0.};
    for(int il=0;il<18;il++)
    {
        tmpedepmax[il] = -1.;
        CellMax[il] = -1;
        for(int ic=0;ic<72;ic++)
        {
             if(EdepRecorr[il][ic] > tmpedepmax[il])
             {
           	  tmpedepmax[il] = EdepRecorr[il][ic];
           	  CellMax[il] = ic;
             }
        }
    }

    float Elayer[18]={0.};
    for(int il=0;il<18;il++)
    {
        for(int ic=0;ic<72;ic++)
        {
            Elayer[il] += EdepRecorr[il][ic];
        }
        if(CellMax[il]<9)
        {
            for(int k=2*CellMax[il]+1;k<=CellMax[il]+9;k++) Elayer[il] += EdepRecorr[il][k];
        }
        if(CellMax[il]>62)
        {
            for(int k=CellMax[il]-9;k<=2*CellMax[il]-72;k++) Elayer[il] += EdepRecorr[il][k];
        }
    }

    CorrEnergyD = 0.;
    for(int il=0;il<18;il++) CorrEnergyD += (Elayer[il]/1000.);

    return CorrEnergyD;
}

void DeadSideCorr::ReCalCOGZ()
{
    int CellMax[18]={0};
    float tmpedepmax[18]={0.};
    for(int il=0;il<18;il++)
    {
        tmpedepmax[il] = -1.;
        CellMax[il] = -1;
        for(int ic=0;ic<72;ic++)
        {
             if(EdepRecorr[il][ic] > tmpedepmax[il])
             {
           	  tmpedepmax[il] = EdepRecorr[il][ic];
           	  CellMax[il] = ic;
             }
        }
    }

    float Elayer[18]={0.};
    for(int il=0;il<18;il++)
    {
        for(int ic=0;ic<72;ic++)
        {
            Elayer[il] += EdepRecorr[il][ic];
        }
        if(CellMax[il]<9)
        {
            for(int k=2*CellMax[il]+1;k<=CellMax[il]+9;k++) Elayer[il] += EdepRecorr[il][k];
        }
        if(CellMax[il]>62)
        {
            for(int k=CellMax[il]-9;k<=2*CellMax[il]-72;k++) Elayer[il] += EdepRecorr[il][k];
        }
    }

    float sumcogz = 0.;
    float sumedep = 0.;
    for(int il=0;il<18;il++)
    {
        sumcogz += (Elayer[il]*Ecal_Z[il]);
        sumedep += Elayer[il];
    }
    ecalcog[2] = sumcogz/sumedep;
}

int DeadSideCorr::ReFitDirCR(int deadcelltag)
{
    float NewEnergyE = EnergyRecorr;
    float CooCOG[18];
    float CooCR[18];
    double CellWidth = 0.9;
    int NCells = 72;

    //To Be Done, DeadCell Correction,Side leackage Correction;

    //Par for CR
    //Double_t P_1[3] = {6.24423,2.47938,0.0742745};// P1 = [0]-[1]*exp(-[2]*E);  
    //Double_t P_2[3] = {-0.396049,0.0292195,1.00034};//P2 = a2+b2*log(E)+exp(-c2*log(E));
    //Double_t P1 = P_1[0] - P_1[1]*TMath::Exp(-P_1[2]*NewEnergyE);
    //Double_t P2 = P_2[0] + P_2[1]*TMath::Log(NewEnergyE) +TMath::Power(NewEnergyE,-P_2[2]);
    Double_t P1[2];   //[0] for X, [1] for y
    Double_t P2[2];   //[0] for x, [1] for y
    Double_t P_1x_ISS[3]; //ISS
    Double_t P_2x_ISS[3];//ISS
    Double_t P_1x_MC[3]; //MC
    Double_t P_2x_MC[3];//MC

    Double_t P_1y_ISS[3]; //ISS
    Double_t P_2y_ISS[3]; //ISS
    Double_t P_1y_MC[3]; //MC
    Double_t P_2y_MC[3]; //MC

    for(int it=0; it < 3; it++)
    {
        P_1x_ISS[it] = DeadSideCorrDB::CellRatio_P_1x_ISS[it];
        P_2x_ISS[it] = DeadSideCorrDB::CellRatio_P_2x_ISS[it];
        P_1y_ISS[it] = DeadSideCorrDB::CellRatio_P_1y_ISS[it];
        P_2y_ISS[it] = DeadSideCorrDB::CellRatio_P_2y_ISS[it];

        P_1x_MC[it] = DeadSideCorrDB::CellRatio_P_1x_MC[it];
        P_2x_MC[it] = DeadSideCorrDB::CellRatio_P_2x_MC[it];
        P_1y_MC[it] = DeadSideCorrDB::CellRatio_P_1y_MC[it];
        P_2y_MC[it] = DeadSideCorrDB::CellRatio_P_2y_MC[it];
    }

    Double_t P_1x[3],P_2x[3],P_1y[3],P_2y[3];
    if(DataType == DeadSideCorrDB::_MC_DATA){
        memcpy(P_1x ,P_1x_MC,sizeof(P_1x));
        memcpy(P_1y ,P_1y_MC,sizeof(P_1y));
        memcpy(P_2x ,P_2x_MC,sizeof(P_2x));
        memcpy(P_2y ,P_2y_MC,sizeof(P_2y));
    }
    else{
        memcpy(P_1x ,P_1x_ISS,sizeof(P_1x));
        memcpy(P_1y ,P_1y_ISS,sizeof(P_1y));
        memcpy(P_2x ,P_2x_ISS,sizeof(P_2x));
        memcpy(P_2y ,P_2y_ISS,sizeof(P_2y));
    }

    P1[0] = P_1x[0] - P_1x[1]*TMath::Exp(-1. * P_1x[2]*NewEnergyE);
    if(NewEnergyE>0) P2[0] = P_2x[0] + P_2x[1]*TMath::Log(NewEnergyE) + TMath::Power(NewEnergyE,-1.*P_2x[2]);
    else P2[0] = 0.;

    P1[1] = P_1y[0] - P_1y[1]*TMath::Exp(-1. * P_1y[2]*NewEnergyE);
    if(NewEnergyE>0) P2[1] = P_2y[0] + P_2y[1]*TMath::Log(NewEnergyE) + TMath::Power(NewEnergyE,-1.*P_2y[2]);
    else P2[1] =0.;

    //Par for Linear fit Weight of CR Methode
    Double_t WeightPar_ISS[18][3];
    Double_t WeightPar_MC[18][3];

    for(int il=0; il < 18; il++)
    {
        WeightPar_ISS[il][0] = DeadSideCorrDB::CellRatio_WeightPar_ISS[il][0];
        WeightPar_ISS[il][1] = DeadSideCorrDB::CellRatio_WeightPar_ISS[il][1];
        WeightPar_ISS[il][2] = DeadSideCorrDB::CellRatio_WeightPar_ISS[il][2];
        WeightPar_MC[il][0] = DeadSideCorrDB::CellRatio_WeightPar_MC[il][0];
        WeightPar_MC[il][1] = DeadSideCorrDB::CellRatio_WeightPar_MC[il][1];
        WeightPar_MC[il][2] = DeadSideCorrDB::CellRatio_WeightPar_MC[il][2];
    }

    Double_t WeightPar[18][3];

    if(DataType == DeadSideCorrDB::_MC_DATA){
        memcpy(WeightPar,WeightPar_MC,sizeof(WeightPar_MC));
    }
    else{
        memcpy(WeightPar,WeightPar_ISS,sizeof(WeightPar_ISS));
    }

    Double_t LFWeight[18];
    for(int il=0;il<18;il++){
        LFWeight[il] = WeightPar[il][0] + WeightPar[il][1]*TMath::Exp(WeightPar[il][2]*NewEnergyE);
        if(LFWeight[il]<0.05)LFWeight[il]=0.05;
        if(LFWeight[il]>5.)LFWeight[il]=5.;
    }

    //Par for Linear fit Weight of COG Method
    Double_t WPCOG0[3];
    Double_t WPCOG1[5];
    Double_t WPCOG2[5];

    WPCOG0[0] = DeadSideCorrDB::CellRatio_WPCOG0[0]; WPCOG0[1] = DeadSideCorrDB::CellRatio_WPCOG0[1]; WPCOG0[2] = DeadSideCorrDB::CellRatio_WPCOG0[2];
    WPCOG1[0] = DeadSideCorrDB::CellRatio_WPCOG1[0]; WPCOG1[1] = DeadSideCorrDB::CellRatio_WPCOG1[1]; WPCOG1[2] = DeadSideCorrDB::CellRatio_WPCOG1[2];  WPCOG1[3] = DeadSideCorrDB::CellRatio_WPCOG1[3];  WPCOG1[4] = DeadSideCorrDB::CellRatio_WPCOG1[4];
    WPCOG2[0] = DeadSideCorrDB::CellRatio_WPCOG2[0]; WPCOG2[1] = DeadSideCorrDB::CellRatio_WPCOG2[1]; WPCOG2[2] = DeadSideCorrDB::CellRatio_WPCOG2[2];  WPCOG2[3] = DeadSideCorrDB::CellRatio_WPCOG2[3];  WPCOG2[4] = DeadSideCorrDB::CellRatio_WPCOG2[4];

  Double_t WP_COG0;
  if(NewEnergyE + WPCOG0[2]>0) WP_COG0 = WPCOG0[0] + WPCOG0[1]*TMath::Log(NewEnergyE+WPCOG0[2]);
  else WP_COG0 = 0.;
  Double_t WP_COG1;
  if(NewEnergyE + WPCOG1[4]>0) WP_COG1 = WPCOG1[0] - WPCOG1[1]*TMath::Exp(-1.*WPCOG1[2]*NewEnergyE)+WPCOG1[3]*TMath::Log(NewEnergyE + WPCOG1[4]);
  else WP_COG1 = 0.;
  Double_t WP_COG2;
  if(NewEnergyE + WPCOG2[4]>0) WP_COG2 = 1./(WPCOG2[0]-WPCOG2[1]*TMath::Exp(-1.*WPCOG2[2]*NewEnergyE)+WPCOG2[3]*TMath::Log(NewEnergyE+WPCOG2[4]));
  else WP_COG2 = 0.;

  Double_t CooTrans[3][18];
  Double_t EdepCellRatio[18];
  Double_t Alpha[18];
  Double_t DisLeft[18];
  Double_t RecCoo[18];
  Double_t Weight[18];

  Double_t AngleTrk[3];
  Double_t AngleEcal[3];

  memset(CooTrans,0,sizeof(CooTrans));
  memset(EdepCellRatio,0,sizeof(EdepCellRatio));
  memset(Alpha,0,sizeof(Alpha));
  memset(DisLeft,0,sizeof(DisLeft));
  memset(RecCoo,0,sizeof(RecCoo));
  memset(CooCR,0,sizeof(CooCR));
  memset(Weight,0,sizeof(Weight));
  memset(AngleTrk,0,sizeof(AngleEcal));
  memset(AngleEcal,0,sizeof(AngleEcal));

  //Calculate COOCOG & COOCR
  int CellMax[18]={0};
  for(int il =0;il<18;il++){
      int proj = 1- il/2%2;   //0 for x, 1 for y;
      Alpha[il] =  -1.*P1[proj] - P2[proj]*(il+0.5);  //Used for Calculate COOCR for layer

      //Find Bin_Max in the Layer;
     float tmpedepmax = -1.;
     CellMax[il] = -1;
     for(int ic=0;ic<72;ic++){
	  if(edepattcorr[il][ic] >tmpedepmax){
		  tmpedepmax = edepattcorr[il][ic];
		  CellMax[il] = ic;
	  }
      }
      int pmtside = CellMax[il] /2%2;

      //Caculate CooCOG
      Double_t tmpcoo=0.;
      Double_t eccl = 0.;
      for(int ic=0;ic<72;ic++){
	  if(edepattcorr[il][ic]>0) tmpcoo += edepattcorr[il][ic] * (ic+0.5);
	  eccl += edepattcorr[il][ic];
      }
      if(eccl>0) tmpcoo/=eccl;
      else tmpcoo =-9999.;
      CooCOG[il] = (tmpcoo - 36)*CellWidth;
      //Ecal Track Alignment
      if(DataType == DeadSideCorrDB::_MC_DATA) CooCOG[il] -= DeadSideCorrDB::CommonOffSetMC[proj];
      else if(DataType == DeadSideCorrDB::_TB_DATA) CooCOG[il] -= DeadSideCorrDB::CommonOffSetBT[proj];
      else  CooCOG[il] -= DeadSideCorrDB::CommonOffSetISS[proj];
     //Side Shift
     if(DataType == DeadSideCorrDB::_MC_DATA) CooCOG[il] += DeadSideCorrDB::UM2CM * DeadSideCorrDB::OffSetMC[il/2][pmtside];
     else if(DataType == DeadSideCorrDB::_TB_DATA) CooCOG[il] += DeadSideCorrDB::UM2CM * DeadSideCorrDB::OffSetBT[il/2][pmtside];
     else  CooCOG[il] += DeadSideCorrDB::UM2CM*DeadSideCorrDB::OffSetISS[il/2][pmtside];

     //Calculate CooCR
     if(CellMax[il] == 0 || CellMax[il] == 71){
       CooCR[il] = -9999.;
     }
     else{
	  if(edepattcorr[il][CellMax[il]+1]>0.&&edepattcorr[il][CellMax[il]-1]>0.){
		  CooCR[il] = CellMax[il] + 0.5 + TMath::Log(edepattcorr[il][CellMax[il]-1]/edepattcorr[il][CellMax[il]+1])/Alpha[il];
		 //Convert Cell Unit to cm
		  CooCR[il] -= NCells/2;
		  CooCR[il] *= CellWidth;

		  //Ecal Track Alignment
		  if(DataType == DeadSideCorrDB::_MC_DATA) CooCR[il] -= DeadSideCorrDB::CommonOffSetMC[proj];
		  else if(DataType == DeadSideCorrDB::_TB_DATA) CooCR[il] -= DeadSideCorrDB::CommonOffSetBT[proj];
		  else  CooCR[il] -= DeadSideCorrDB::CommonOffSetISS[proj];

		  //Side Shift
		  if(DataType == DeadSideCorrDB::_MC_DATA) CooCR[il] += DeadSideCorrDB::UM2CM * DeadSideCorrDB::OffSetMC[il/2][pmtside];
		  else if(DataType == DeadSideCorrDB::_TB_DATA) CooCR[il] += DeadSideCorrDB::UM2CM*DeadSideCorrDB::OffSetBT[il/2][pmtside];
		  else  CooCR[il] += DeadSideCorrDB::UM2CM*DeadSideCorrDB::OffSetISS[il/2][pmtside];

		  //To do ...
		  //1.   Dynamic Alignment
	  }
	  else{
		  CooCR[il] = -9999.;
	  }
    }
  }

  //Do Linear Fit in XZ and YZ plane to get kx and ky with COOCOG
  Float_t coocr[18];
  memset(KCR,0,sizeof(KCR));
  memset(CooNew,0,sizeof(CooNew));

  //Calculate Weight used for Linear Fit;
  for(int il=0;il<18;il++){
          //Weight[il] = elayer[il];
	  if(CooCOG[il] >-100){
  	     coocr[il] = CooCOG[il]; 
        
	     if(WP_COG1>0&&WP_COG2>0){
		  Weight[il] =  WP_COG0*TMath::GammaDist(il+0.5,WP_COG1,0,WP_COG2);
                  if(Weight[il]<0.05)  Weight[il] = 0.05; //Minimum Weight for C.O.G
                  if(Weight[il]>4)  Weight[il] = 4.;  //Maximum weight for C.O.
  	     }
	     else Weight[il] =0.05;
             if(deadcelltag==0)
             {
                 if((il==6)&&(CellMax[il]>=13)&&(CellMax[il]<=20)) {Weight[il] =0.;coocr[il]=0.;}//wxq
                 if((il==7)&&(CellMax[il]>=13)&&(CellMax[il]<=20)) {Weight[il] =0.;coocr[il]=0.;}//wxq
                 if((il==14)&&(CellMax[il]>=61)&&(CellMax[il]<=68)) {Weight[il] =0.;coocr[il]=0.;}//wxq
                 if((il==15)&&(CellMax[il]>=61)&&(CellMax[il]<=68)) {Weight[il] =0.;coocr[il]=0.;}//wxq
             }
          }
	  else {Weight[il] =0.; coocr[il]=0.;}
  }
  Double_t sumz2=0.,sumy=0.,sumyz=0.,sumw=0.,sumz=0.;
  for(int isl=0;isl<9;isl+=2){
	  for(int il=2*isl;il<2*isl +2;il++){
		  if(coocr[il]<32.4 && coocr[il] > -32.4&&Weight[il]>0){
			  Double_t tmpz = DeadSideCorrDB::Ecal_Z[il]-ecalcog[2];
			  sumz2 += Weight[il] * tmpz*tmpz;
			  sumy += Weight[il]*coocr[il];
			  sumyz += Weight[il]*coocr[il]*tmpz;
			  sumw += Weight[il];
			  sumz += Weight[il] * tmpz;
		  }
	  }
  }
  if(sumw*sumz2-sumz*sumz>0){
	  CooNew[1] = (sumz2*sumy - sumz * sumyz)/(sumw*sumz2-sumz*sumz);
	  KCR[1] = (sumw*sumyz - sumz*sumy)/(sumw*sumz2 - sumz*sumz);
  }
  else { CooNew[1] = -99999.;KCR[1]=-99999.;}

  sumz2=0;
  sumw=0;
  sumz=0;
  float sumx=0,sumxz=0;
  for(int isl=1;isl<9;isl+=2){
	  for(int il=2*isl;il<2*isl +2;il++){
		  if(coocr[il]<32.4 && coocr[il] > -32.4&&Weight[il]>0){
			  Double_t tmpz =  DeadSideCorrDB::Ecal_Z[il]-ecalcog[2];
			  sumz2 += Weight[il] * tmpz*tmpz;
			  sumx += Weight[il]*coocr[il];
			  sumxz += Weight[il]*coocr[il]*tmpz;
			  sumw += Weight[il];
			  sumz += Weight[il] * tmpz;
		  }
	  }
  }
  if(sumw*sumz2-sumz*sumz>0){
	  CooNew[0] = (sumz2*sumx - sumz * sumxz)/(sumw*sumz2-sumz*sumz);
	  KCR[0] = (sumw*sumxz - sumz*sumx)/(sumw*sumz2 - sumz*sumz);
  }
  else { CooNew[0]=-99999.;KCR[0]=-99999.;}
  CooNew[2] = ecalcog[2];

  //Add Fiber Rotation to CooCR[il]
  for(int il=0;il<18;il++){
     double coof = il/2%2==0?(DeadSideCorrDB::Ecal_Z[il]-ecalcog[2])*KCR[0]+CooNew[0]:(DeadSideCorrDB::Ecal_Z[il]-ecalcog[2])*KCR[1]+CooNew[1];
     if(coof>32.4)coof=32.4;
     if(coof<-32.4)coof=-32.4;
     if( CellMax[il]/2%2==il/2%2 ){
        CooCR[il] += DeadSideCorrDB::fiberRotation[il/2]*(coof + 32.4);
     }
     else{
        CooCR[il] += DeadSideCorrDB::fiberRotation[il/2]*(coof - 32.4);
     }
  }

  //Second loop
  //Do Linear Fit in XZ and YZ plane to get kx and ky using COOCR
  memset(KCR,0,sizeof(KCR));
  memset(CooNew,0,sizeof(CooNew));

  //init values
  sumz2=0.;sumy=0.;sumyz=0.;sumw=0.;sumz=0.;
  sumx=0;sumxz=0;
  //Calculate Weight used for Linear Fit;
  for(int il=0;il<18;il++){
	  if(CooCR[il]>-100){
		  coocr[il] = CooCR[il];
		  Weight[il] = LFWeight[il];
	  }
	  else{
		  if(CooCOG[il] >-100){
			  coocr[il] = CooCOG[il]; 
                          //Weight[il] = elayer[il];
			  if(WP_COG1>0&&WP_COG2>0){
				  Weight[il] =  WP_COG0*TMath::GammaDist(il+0.5,WP_COG1,0,WP_COG2);
                                  if(Weight[il]<0.05)Weight[il] = 0.05;
                                  if(Weight[il]>4)Weight[il] = 4;
			  }
			  else Weight[il] =0.05;
                          if(deadcelltag==0)
                          {
                              if((il==6)&&(CellMax[il]>=13)&&(CellMax[il]<=20)) {Weight[il] =0.;coocr[il]=0.;}//wxq
                              if((il==7)&&(CellMax[il]>=13)&&(CellMax[il]<=20)) {Weight[il] =0.;coocr[il]=0.;}//wxq
                              if((il==14)&&(CellMax[il]>=61)&&(CellMax[il]<=68)) {Weight[il] =0.;coocr[il]=0.;}//wxq
                              if((il==15)&&(CellMax[il]>=61)&&(CellMax[il]<=68)) {Weight[il] =0.;coocr[il]=0.;}//wxq
                          }
		  }
		  else {Weight[il] =0.;coocr[il]=0.;};
	  }
  }

  for(int isl=0;isl<9;isl+=2){
	  for(int il=2*isl;il<2*isl +2;il++){
		  if(coocr[il]<32.4 && coocr[il] > -32.4&&Weight[il]>0){
			  Double_t tmpz = DeadSideCorrDB::Ecal_Z[il]-ecalcog[2];
			  sumz2 += Weight[il] * tmpz*tmpz;
			  sumy += Weight[il]*coocr[il];
			  sumyz += Weight[il]*coocr[il]*tmpz;
			  sumw += Weight[il];
			  sumz += Weight[il] * tmpz;
		  }
	  }
  }
  if(sumw*sumz2-sumz*sumz>0){
	  CooNew[1] = (sumz2*sumy - sumz * sumyz)/(sumw*sumz2-sumz*sumz);
	  KCR[1] = (sumw*sumyz - sumz*sumy)/(sumw*sumz2 - sumz*sumz);
  }
  else { CooNew[1] = -99999.;KCR[1]=-99999.;}

  sumz2=0;
  sumw=0;
  sumz=0;
  sumx=0;
  sumxz=0;
  for(int isl=1;isl<9;isl+=2){
	  for(int il=2*isl;il<2*isl +2;il++){
		  if(coocr[il]<32.4 && coocr[il] > -32.4&&Weight[il]>0){
			  Double_t tmpz =  DeadSideCorrDB::Ecal_Z[il]-ecalcog[2];
			  sumz2 += Weight[il] * tmpz*tmpz;
			  sumx += Weight[il]*coocr[il];
			  sumxz += Weight[il]*coocr[il]*tmpz;
			  sumw += Weight[il];
			  sumz += Weight[il] * tmpz;
		  }
	  }
  }
  if(sumw*sumz2-sumz*sumz>0){
	  CooNew[0] = (sumz2*sumx - sumz * sumxz)/(sumw*sumz2-sumz*sumz);
	  KCR[0] = (sumw*sumxz - sumz*sumx)/(sumw*sumz2 - sumz*sumz);
  }
  else { CooNew[0]=-99999.;KCR[0]=-99999.;}
  CooNew[2] = ecalcog[2];
  //Done KCR and CooNew Calculation

  //Calculate ChisqDirX and ChisqDirY
  float ChisqDirX = 0. ;
  float ChisqDirY = 0. ;
  float ChisqDir = 0;
  int totallayery=0;
  int totallayerx=0;
  float weightx=0;
  float weighty=0;

  for(int il=0;il<18;il++){
    int proj = 1-il/2%2;
    float coofit = CooNew[proj] + KCR[proj]*(DeadSideCorrDB::Ecal_Z[il]-ecalcog[2]);
    if(il/2%2==0){
      if(coocr[il]<32.4 && coocr[il] > -32.4&&Weight[il]>0){
          ChisqDirY +=  Weight[il]*Weight[il]*(coocr[il] - coofit)*(coocr[il] - coofit);
          weighty += Weight[il];
          totallayery++;
      }
    }
    else{
      if(coocr[il]<32.4 && coocr[il] > -32.4&&Weight[il]>0){
        ChisqDirX +=  Weight[il]*Weight[il]*(coocr[il] - coofit)*(coocr[il] - coofit);
        weightx += Weight[il];
        totallayerx++;
      }
    }
  }
  if(totallayery>0)ChisqDirY = TMath::Sqrt(ChisqDirY)/weighty;
  if(totallayerx>0)ChisqDirX = TMath::Sqrt(ChisqDirX)/weightx;
  ChisqDir = TMath::Sqrt(ChisqDirX*ChisqDirX + ChisqDirY*ChisqDirY);
  (void) ChisqDir;

  return 0;
}

int DeadSideCorr::ReCooLayer()
{
    memset(coocrlayer,0,sizeof(coocrlayer));
    memset(Numcoocrlayer,0,sizeof(Numcoocrlayer));
    memset(coocrfiber,0,sizeof(coocrfiber));
    memset(coocrreadout,0,sizeof(coocrreadout));
    for(int il=0; il < 18; il++)
    {
        int proj = 1-il/2%2;
        coocrlayer[il][0] = CooNew[0] + KCR[0]*(DeadSideCorrDB::Ecal_Z[il]-CooNew[2]);
        coocrlayer[il][1] = CooNew[1] + KCR[1]*(DeadSideCorrDB::Ecal_Z[il]-CooNew[2]);

        if(DataType == DeadSideCorrDB::_MC_DATA)
        {
            coocrlayer[il][0] += DeadSideCorrDB::CommonOffSetMC[0];
            coocrlayer[il][1] += DeadSideCorrDB::CommonOffSetMC[1];
        }
        else if(DataType == DeadSideCorrDB::_TB_DATA)
        {
            coocrlayer[il][0] += DeadSideCorrDB::CommonOffSetBT[0];
            coocrlayer[il][1] += DeadSideCorrDB::CommonOffSetBT[1];
        }
        else
        {
            coocrlayer[il][0] += DeadSideCorrDB::CommonOffSetISS[0];
            coocrlayer[il][1] += DeadSideCorrDB::CommonOffSetISS[1];
        }

        int tmpcellmax = floor(coocrlayer[il][proj]/0.9+36.);//wxq
        int pmtside = tmpcellmax /2%2;

        if(DataType == DeadSideCorrDB::_MC_DATA) coocrlayer[il][proj] -= DeadSideCorrDB::UM2CM * DeadSideCorrDB::OffSetMC[il/2][pmtside];
        else if(DataType == DeadSideCorrDB::_TB_DATA) coocrlayer[il][proj] -= DeadSideCorrDB::UM2CM * DeadSideCorrDB::OffSetBT[il/2][pmtside];
        else coocrlayer[il][proj] -= DeadSideCorrDB::UM2CM * DeadSideCorrDB::OffSetISS[il/2][pmtside];

        double coof = il/2%2==0?(DeadSideCorrDB::Ecal_Z[il]-ecalcog[2])*KCR[0]+CooNew[0]:(DeadSideCorrDB::Ecal_Z[il]-ecalcog[2])*KCR[1]+CooNew[1];
        if(coof>32.4)coof=32.4;
        if(coof<-32.4)coof=-32.4;
        if( tmpcellmax/2%2==il/2%2 ){
            coocrlayer[il][proj] -= DeadSideCorrDB::fiberRotation[il/2]*(coof + 32.4);
        }
        else{
            coocrlayer[il][proj] -= DeadSideCorrDB::fiberRotation[il/2]*(coof - 32.4);
        }
        /*Done Shift*/

        Numcoocrlayer[il] = floor(coocrlayer[il][proj]/0.9+36);
        coocrfiber[il] = coocrlayer[il][1-proj];
        coocrreadout[il] = coocrlayer[il][proj];
    }

    return 0;
}

float DeadSideCorr::RePMTLayerCorr(float Elayer, int ilayer)
{
    float NewEnergyE = EnergyRecorr;

    float aa = DeadSideCorrDB::PMTEff_aa;
    float bb[3];
    bb[0] = DeadSideCorrDB::PMTEff_bb[0]; bb[1] = DeadSideCorrDB::PMTEff_bb[1]; bb[2] = DeadSideCorrDB::PMTEff_bb[2];

    float a1 = aa;
    float b1 = bb[0] + bb[1] * TMath::Exp(bb[2]*NewEnergyE);

    float Elpmteffsigma;

    float kk = TMath::Sqrt(1. + KCR[0]*KCR[0]+KCR[1]*KCR[1]);

    Elpmteffsigma = a1+b1*(ilayer+0.5)*kk;
    if(Elpmteffsigma<.5)Elpmteffsigma=0.55;

    int proj = 1-ilayer/2%2;

    //float tmppos = coocrlayer[ilayer][proj]/0.9;
    //float pmtpos = tmppos - TMath::FloorNint(tmppos);
    float pmtpos = coocrlayer[ilayer][proj]/0.9+36-Numcoocrlayer[ilayer];

    float Elayer_corr;

    Elayer_corr = Elayer/TMath::Gaus(pmtpos,0.5,Elpmteffsigma);
    
    return Elayer_corr;
}

int DeadSideCorr::RePMTCorr()
{
    float elayer[18];
    memset(elayer,0,sizeof(elayer));

    for(int il=0; il < 18; il++)
    {
        elayer_corr[il] = 0;
        for(int ic=0; ic < 72; ic++)
        {
            if(edepattcorr[il][ic]>0) elayer[il] += edepattcorr[il][ic];
        }
        elayer_corr[il] = RePMTLayerCorr(elayer[il],il);
    }

    return 0;
}

Double_t DeadSideCorr::func(Double_t *par, Double_t x) {return TMath::Power(par[2]*(x-par[3]),par[1]*par[2])*TMath::Exp(-par[2]*(x-par[3]));}

Double_t DeadSideCorr::integerLong(Double_t a,Double_t b,Double_t *par,Double_t epsilon)
{
    const Double_t Z1 = 1;
    const Double_t HF = Z1/2;
    const Double_t CST = 5*Z1/1000;

    Double_t x[12] = { 0.96028985649753623,  0.79666647741362674,
                       0.52553240991632899,  0.18343464249564980,
                       0.98940093499164993,  0.94457502307323258,
                       0.86563120238783174,  0.75540440835500303,
                       0.61787624440264375,  0.45801677765722739,
                       0.28160355077925891,  0.09501250983763744};

    Double_t w[12] = { 0.10122853629037626,  0.22238103445337447,
                       0.31370664587788729,  0.36268378337836198,
                       0.02715245941175409,  0.06225352393864789,
                       0.09515851168249278,  0.12462897125553387,
                       0.14959598881657673,  0.16915651939500254,
                       0.18260341504492359,  0.18945061045506850};

    Double_t h, aconst, bb, aa, c1, c2, u, s8, s16, f1, f2;
    Int_t i;


    Double_t xx;
    h = 0;
    if (b == a) return h;
    aconst = CST/TMath::Abs(b-a);
    bb = a;
    CASE1:
    aa = bb;
    bb = b;
    CASE2:
    c1 = HF*(bb+aa);
    c2 = HF*(bb-aa);
    s8 = 0;
    for (i=0;i<4;i++) {
      u     = c2*x[i];
      xx = c1+u;
      f1    = func(par,xx);
      xx = c1-u;
      f2    = func(par,xx);
      s8   += w[i]*(f1 + f2);
    }
    s16 = 0;
    for (i=4;i<12;i++) {
      u     = c2*x[i];
      xx = c1+u;
      f1    = func(par,xx);
      xx = c1-u;
      f2    = func(par,xx);
      s16  += w[i]*(f1 + f2);
    }
    s16 = c2*s16;
    if (TMath::Abs(s16-c2*s8) <= epsilon*(1. + TMath::Abs(s16))) {
      h += s16;
      if(bb != b) goto CASE1;
    }
    else {
      bb = c1;
      if(1. + aconst*TMath::Abs(c2) != 1) goto CASE2;
      h = s8;  //this is a crude approximation (cernlib function returned 0 !)
    }
  return h;
}

Double_t DeadSideCorr::GetLongChi2(Double_t *par)
{
   Double_t layeredep=0.,delta=0.,chisq=0.;
   for(Int_t i=0;i<18;i++){
       if(DeadStatus[i] == 1) continue;
       layeredep = integerLong((Double_t)i*DeadSideCorrDB::TLAYER,(Double_t)(i+1.0)*DeadSideCorrDB::TLAYER,par);
       layeredep /= TMath::Gamma(par[1]*par[2]+1);
       layeredep *= par[0]*par[2];
       if(ecclEdep[i]>0){
           delta = (ecclEdep[i] - layeredep)*(ecclEdep[i] - layeredep)/ecclEdep[i];
       }
       else delta =0;

       chisq += delta;
   }
   longchi2 = chisq;

   return chisq;
}

void DeadSideCorr::ecclFCN(Int_t &npar, Double_t *gin, Double_t &f, Double_t *par, Int_t iflag)
{
    DeadSideCorr* deadsidecorr = (DeadSideCorr*)minuit1->GetObjectFit();
    f = deadsidecorr->GetLongChi2(par);
}

int DeadSideCorr::ReLongFit()
{
    memset(ecclEdep,0,sizeof(ecclEdep));
    for(int il=0; il < 18; il++)
    {
        ecclEdep[il] = ElayerRecorr[il]/1000.; // Using the layer energy After FiberCorr.
        ElayerLongFit[il] = 0.0;
    }

   if(!minuit1)
   {
      minuit1 = new TMinuit(4);
   }
   float ELongFit  = -1;
   float LongParT = -99999.;
   float LongParB = -99999.;
   float LongParStart = 99999.;

   Double_t parinit[4],step[4];
   parinit[0] = EnergyRecorr;
   step[0] = parinit[0]/5.0;
   parinit[1] = 9.0-TMath::Log(180./EnergyRecorr);
   step[1] =parinit[1]/5.;
   parinit[2] = 0.5;
   step[2] = 0.2;
   parinit[3] = 0;
   step[3] = 0.2;
   //timer.Start();
   Double_t arglist[100];
   
   int  ierflg = 0;
   minuit1->SetPrintLevel(-1);
   minuit1->SetFCN(ecclFCN);
   minuit1->SetObjectFit(this);
   //minuit1->SetPrecision(0.000001);
   //minuit->SetMaxIterations(2000);
   arglist[0] = 2;
   minuit1->mnexcm("SET ERR",arglist,1,ierflg);
   minuit1->mnparm(0, "E0", parinit[0] , step[0],0.5*parinit[0],2*parinit[0],ierflg);
   minuit1->mnparm(1, "Zmax", parinit[1], step[1],0.5*parinit[1],1.5*parinit[1],ierflg);
   minuit1->mnparm(2, "b", parinit[2],step[2],0.2*parinit[2],5.*parinit[2],ierflg);
   minuit1->mnparm(3, "Zshift", parinit[3] , 0.5,-2.,18.,ierflg);

   minuit1->FixParameter(3);

   Double_t fAmin,fEdm,fErrdef;
   Int_t fStats1 =0,fNvpar,fNparx;
   arglist[0] = 1;
   //minuit1->mnexcm("CALL FCN", arglist, 1);
   arglist[0] = 500;
   minuit1->mnexcm("MIGRAD", arglist, 1,ierflg);
   //cout<<endl<<endl<<endl<<"**************Minuit Status*************";
   minuit1->mnstat(fAmin,fEdm,fErrdef,fNvpar,fNparx,fStats1);
   //cout<<fStats1<<endl;
     Double_t parvalue,parerror;
     Double_t par1[4],parerror1[4];
     for(Int_t i=0;i<4;i++){
       minuit1->GetParameter(i,parvalue,parerror);
       par1[i] = parvalue;
       parerror1[i] = parerror;
     }
     ELongFit = par1[0];
     LongParT = par1[1];
     LongParB = par1[2];
     LongParStart = par1[3];

   for(Int_t i=0;i<18;i++){
      ElayerLongFit[i] = integerLong((Double_t)i*DeadSideCorrDB::TLAYER,(Double_t)(i+1.0)*DeadSideCorrDB::TLAYER,par1);
      ElayerLongFit[i] /= TMath::Gamma(par1[1]*par1[2]+1);
      ElayerLongFit[i] *= par1[0]*par1[2];

      ElayerLongFit[i] *= 1000.;
   }

   int fit_stat=0;
   for(int i=0;i<18;i++)
   {
      if(ElayerLongFit[i]>999999.) fit_stat++;
   }
   if(fit_stat>0)
   {
      for(int i=0;i<18;i++) ElayerLongFit[i]=elayer_corr[i];
   } 

   (void) ELongFit;
   (void) LongParT;
   (void) LongParB;
   (void) LongParStart;
   (void) parerror1;
   return 0;
}

double DeadSideCorr::ExpCellE_CalEffi(int il,int ic)//wxq
{
    double Fcx[30], Rcx[30], Rtx[30];
    double Fcy[30], Rcy[30], Rty[30];

    memset(Fcx,0,sizeof(Fcx));
    memset(Fcy,0,sizeof(Fcy));
    memset(Rcx,0,sizeof(Rcx));
    memset(Rcy,0,sizeof(Rcy));
    memset(Rtx,0,sizeof(Rtx));
    memset(Rty,0,sizeof(Rty));

    /*===== Set Parameters =====*/
    double ene = EnergyRecorr;
    double logene = log(ene+1);
    if(logene<0) logene=0;
    double qcx_p0 = 2.6409-0.0313*logene,
           qcx_p1 = 0.8349-0.0433*logene,
           qcx_p2 = 1.1,
           rcx_p0 = 0.015,
           rcx_p1 = 0.3712+5.142e-6*ene*sqrt(ene+1),
           rtx_p0 = 0.6568-0.2277*logene,
           rtx_p1 = 0.5611+0.1902*logene,
           rtx_p2 = 0;

    if( qcx_p1<0. ) qcx_p1=0.;
    if( qcx_p0<1. ) qcx_p0=1.;

    for(int i=0; i<30; i++){
        double t = 0.1*i+0.001;
        Fcx[i] = qcx_p0*exp( (qcx_p1-t)/qcx_p2 - exp((qcx_p1-t)/qcx_p2) );
        Rcx[i] = rcx_p0 + rcx_p1*t;
        Rtx[i] = rtx_p0 + rtx_p1*t + 1/(t+rtx_p2);
        if( Rtx[i] > 10 ) Rtx[i] = 10.;

        Fcy[i] = Fcx[i];
        Rcy[i] = Rcx[i];
        Rty[i] = Rtx[i];
        if( Rty[i] > 10 ) Rty[i] = 10.;
    }

    int tmpL = -1; 
    double tmpE = 0;
    for(int ll=0; ll < 18; ll++)
    {
        if(tmpE < ElayerRecorr[ll])
        {
            tmpL = ll;
            tmpE = ElayerRecorr[ll];
        }
    }
    //double depth = (il+0.5)*0.9;
    //double T0 = (tmpL+0.5)*0.9;
    double depth = (-142.765-Ecal_Z[il]+0.6)*TMath::Sqrt(1+KCR[0]*KCR[0]+KCR[1]*KCR[1]);//wxq
    double T0 = (-142.765-Ecal_Z[tmpL]+0.6)*TMath::Sqrt(1+KCR[0]*KCR[0]+KCR[1]*KCR[1]);//wxq
    double vart = depth/T0;

    int i1,i2;
    double varc;

    i1 = int(vart*10);
    if(i1<0) i1=0;
    if(i1>28) i1=28;
    i2 = i1+1;
    varc = vart*10 - i1;
    if( varc > 1 ) varc = 1.;

    double qc,rc,rt;

    qc = Fcx[i1] + varc*(Fcx[i2] - Fcx[i1]);
    // Core radius
    rc = Rcx[i1] + varc*(Rcx[i2] - Rcx[i1]);
    // Tail radius
    rt = Rtx[i1] + varc*(Rtx[i2] - Rtx[i1]);

    /*===== Done Set Parameters =====*/
    double par[5];

    par[0] = 1.;
    par[1] = qc;
    par[2] = rc;
    par[3] = rt;
    par[4] = 0.45;

    double P1 = par[1]/2.;
    double P2 = 0.5-P1;
    double RC = par[2];
    double RT = par[3];
    //  double x0 = x[0]-par[4]-0.45;
    //    double x0 = TMath::Abs((ic-36)*0.9-par[4]); 
    double x0 = (ic-36)*0.9-par[4]; 
    double x1=x0+0.3;
    double x2=x0+0.6;
    double x3=x0+0.9;
    double sqc0 = 1./TMath::Sqrt(RC*RC + x0*x0);
    double sqt0 = 1./TMath::Sqrt(RT*RT + x0*x0);
    double sqc1 = 1./TMath::Sqrt(RC*RC + x1*x1);
    double sqt1 = 1./TMath::Sqrt(RT*RT + x1*x1);
    double sqc2 = 1./TMath::Sqrt(RC*RC + x2*x2);
    double sqt2 = 1./TMath::Sqrt(RT*RT + x2*x2);
    double sqc3 = 1./TMath::Sqrt(RC*RC + x3*x3);
    double sqt3 = 1./TMath::Sqrt(RT*RT + x3*x3);
    double expE = (0.6-4./3.*x0)*(P1*(x1*sqc1-x0*sqc0)+P2*(x1*sqt1-x0*sqt0))+(P1*(x2*sqc2-x1*sqc1)+P2*(x2*sqt2-x1*sqt1)) + (1.8+4/3.*x0)*(P1*(x3*sqc3-x2*sqc2)+P2*(x3*sqt3-x2*sqt2))+4./3.*(P1*(RC*RC*sqc0-RC*RC*sqc1)+P2*(RT*RT*sqt0-RT*RT*sqt1))-4./3.*(P1*(RC*RC*sqc2-RC*RC*sqc3)+P2*(RT*RT*sqt2-RT*RT*sqt3));
    expE *= par[0];
    return expE;
}

double DeadSideCorr::ExpCellE(int il,int ic)
{
    float Effi = 0.;//wxq
    for(int k=0;k<72;k++) Effi += ExpCellE_CalEffi(il,k);//wxq

    double Fcx[30], Rcx[30], Rtx[30];
    double Fcy[30], Rcy[30], Rty[30];

    memset(Fcx,0,sizeof(Fcx));
    memset(Fcy,0,sizeof(Fcy));
    memset(Rcx,0,sizeof(Rcx));
    memset(Rcy,0,sizeof(Rcy));
    memset(Rtx,0,sizeof(Rtx));
    memset(Rty,0,sizeof(Rty));

    /*===== Set Parameters =====*/
    double ene = EnergyRecorr;
    double logene = log(ene+1);
    if(logene<0) logene=0;
    double qcx_p0 = 2.6409-0.0313*logene,
           qcx_p1 = 0.8349-0.0433*logene,
           qcx_p2 = 1.1,
           rcx_p0 = 0.015,
           rcx_p1 = 0.3712+5.142e-6*ene*sqrt(ene+1),
           rtx_p0 = 0.6568-0.2277*logene,
           rtx_p1 = 0.5611+0.1902*logene,
           rtx_p2 = 0;

    if( qcx_p1<0. ) qcx_p1=0.;
    if( qcx_p0<1. ) qcx_p0=1.;

    for(int i=0; i<30; i++){
        double t = 0.1*i+0.001;
        Fcx[i] = qcx_p0*exp( (qcx_p1-t)/qcx_p2 - exp((qcx_p1-t)/qcx_p2) );
        Rcx[i] = rcx_p0 + rcx_p1*t;
        Rtx[i] = rtx_p0 + rtx_p1*t + 1/(t+rtx_p2);
        if( Rtx[i] > 10 ) Rtx[i] = 10.;

        Fcy[i] = Fcx[i];
        Rcy[i] = Rcx[i];
        Rty[i] = Rtx[i];
        if( Rty[i] > 10 ) Rty[i] = 10.;
    }

    int tmpL = -1; 
    double tmpE = 0;
    for(int ll=0; ll < 18; ll++)
    {
        if(tmpE < ElayerRecorr[ll])
        {
            tmpL = ll;
            tmpE = ElayerRecorr[ll];
        }
    }
    //double depth = (il+0.5)*0.9;
    //double T0 = (tmpL+0.5)*0.9;
    double depth = (-142.765-Ecal_Z[il]+0.6)*TMath::Sqrt(1+KCR[0]*KCR[0]+KCR[1]*KCR[1]);//wxq
    double T0 = (-142.765-Ecal_Z[tmpL]+0.6)*TMath::Sqrt(1+KCR[0]*KCR[0]+KCR[1]*KCR[1]);//wxq
    double vart = depth/T0;

    int i1,i2;
    double varc;

    i1 = int(vart*10);
    if(i1<0) i1=0;
    if(i1>28) i1=28;
    i2 = i1+1;
    varc = vart*10 - i1;
    if( varc > 1 ) varc = 1.;

    double qc,rc,rt;

    qc = Fcx[i1] + varc*(Fcx[i2] - Fcx[i1]);
    // Core radius
    rc = Rcx[i1] + varc*(Rcx[i2] - Rcx[i1]);
    // Tail radius
    rt = Rtx[i1] + varc*(Rtx[i2] - Rtx[i1]);

    /*===== Done Set Parameters =====*/
    int proj = 1-il/2%2;

    double par[5];

    par[0] = ElayerRecorr[il];

    if(DeadStatus[il] == 1) par[0] = ElayerLongFit[il];

    par[0] = par[0]/Effi;//wxq
    par[1] = qc;
    par[2] = rc;
    par[3] = rt;
    par[4] = coocrlayer[il][proj];

    double P1 = par[1]/2.;
    double P2 = 0.5-P1;
    double RC = par[2];
    double RT = par[3];
    //  double x0 = x[0]-par[4]-0.45;
    //    double x0 = TMath::Abs((ic-36)*0.9-par[4]); 
    double x0 = (ic-36)*0.9-par[4]; 
    double x1=x0+0.3;
    double x2=x0+0.6;
    double x3=x0+0.9;
    double sqc0 = 1./TMath::Sqrt(RC*RC + x0*x0);
    double sqt0 = 1./TMath::Sqrt(RT*RT + x0*x0);
    double sqc1 = 1./TMath::Sqrt(RC*RC + x1*x1);
    double sqt1 = 1./TMath::Sqrt(RT*RT + x1*x1);
    double sqc2 = 1./TMath::Sqrt(RC*RC + x2*x2);
    double sqt2 = 1./TMath::Sqrt(RT*RT + x2*x2);
    double sqc3 = 1./TMath::Sqrt(RC*RC + x3*x3);
    double sqt3 = 1./TMath::Sqrt(RT*RT + x3*x3);
    double expE = (0.6-4./3.*x0)*(P1*(x1*sqc1-x0*sqc0)+P2*(x1*sqt1-x0*sqt0))+(P1*(x2*sqc2-x1*sqc1)+P2*(x2*sqt2-x1*sqt1)) + (1.8+4/3.*x0)*(P1*(x3*sqc3-x2*sqc2)+P2*(x3*sqt3-x2*sqt2))+4./3.*(P1*(RC*RC*sqc0-RC*RC*sqc1)+P2*(RT*RT*sqt0-RT*RT*sqt1))-4./3.*(P1*(RC*RC*sqc2-RC*RC*sqc3)+P2*(RT*RT*sqt2-RT*RT*sqt3));
    expE *= par[0];
    if(expE<1.0)expE=0;
    return expE;
}

float DeadSideCorr::ReCalEnergy(Float_t elayer_corr_get[18])
{
//    //Last Layer Correction
//    float LastFrac_ECorrParA = DeadSideCorrDB::ReEnergy_LastFrac_ECorrParA;
//    float LastFrac_ECorrParB = DeadSideCorrDB::ReEnergy_LastFrac_ECorrParB;
//
//    float EnergyD_Corr = 0.;
//
//    float EnergyH;
//    for(int il=0; il < 18; il++)
//    {
//        EnergyD_Corr += elayer_corr_get[il];
//    }
//
//    float corr_factor = 1.;
//    if(EnergyD_Corr>0) corr_factor = LastFrac_ECorrParA + LastFrac_ECorrParB*(elayer_corr_get[16]+elayer_corr_get[17])/EnergyD_Corr;
//    if(EnergyD_Corr>0&&(elayer_corr_get[16]+elayer_corr_get[17])/EnergyD_Corr<0.5&&corr_factor>0) EnergyH = EnergyD_Corr/1000./corr_factor;
//    else EnergyH=(EnergyD_Corr+elayer_corr_get[16]+elayer_corr_get[17])/1000.;
//
//    EnergyD_Corr /=1000.;
//    if(EnergyH < EnergyD_Corr) EnergyH = EnergyD_Corr;
//
//    return EnergyH;

    //variables
    float energy;
    float alpha;
    float EFracL2L = 0; // corrected for anode efficiency
    float EnergyD_Corr = 0; //corrected for andode efficiency

    float kink_lowenergy;
    float kink_highenergy;
    corr_factor =1;
  
    for(int il=0; il < 18; il++) EnergyD_Corr += (elayer_corr_get[il]/1000.);
 
    EFracL2L = (elayer_corr_get[16] + elayer_corr_get[17])/1000./EnergyD_Corr;

    //define kink
    kink_highenergy = 850;
    kink_lowenergy = 3;
    
    if(EnergyRecorr<kink_lowenergy)
      alpha = 1/(1+TMath::Exp(1.581*(-6.131e-1 -EnergyRecorr)));
    else
      if(EnergyRecorr<kink_highenergy)
        alpha = 1.037 - 0.0743/EnergyRecorr-0.159/TMath::Power(EnergyRecorr,2)-1.9186e-9*TMath::Power(EnergyRecorr,2.189);  
      else
        alpha = 0.7628+460/EnergyRecorr-1.988e5/TMath::Power(EnergyRecorr,2)-6.1e-7*TMath::Power(EnergyRecorr,-2.537);
    
    corr_factor = alpha -  0.752 * EFracL2L -  5.633 * TMath::Power(EFracL2L,2);
    if(corr_factor>0 && corr_factor<LongLeakCorrLimit) corr_factor=LongLeakCorrLimit;
    
    if(corr_factor>0)
      energy = EnergyD_Corr/corr_factor;
    else
      energy = EnergyRecorr;
    
    //second iteration
    if(energy<kink_lowenergy)
      alpha = 1/(1+TMath::Exp(1.581*(-6.131e-1 -energy)));
    else
      if(energy<kink_highenergy)
        alpha = 1.037 - 0.0743/energy-0.159/TMath::Power(energy,2)-1.9186e-9*TMath::Power(energy,2.189);  
      else
        alpha = 0.7628+460/energy-1.988e5/TMath::Power(energy,2)-6.1e-7*TMath::Power(energy,-2.537);
    
    corr_factor = alpha -  0.752 * EFracL2L -  5.633 * TMath::Power(EFracL2L,2);
    if(corr_factor>0 && corr_factor<LongLeakCorrLimit) corr_factor=LongLeakCorrLimit;
    //end else for fraction
    
    //corrected value  
    if(corr_factor>0)
      energy = EnergyD_Corr/corr_factor;
    else
      energy = EnergyD_Corr;
    
    //reconstructed energy never less than deposited energy corrected for anode efficiency
    if(energy<EnergyD_Corr)
      energy =  EnergyD_Corr;
    
    return energy;
}

int DeadSideCorr::StatusCheck()
{
    tmpdeadtotal = 0;
    DeadIDL.clear();
    DeadIDC.clear();
    memset(DeadStatus,0,sizeof(DeadStatus));

    //Check status for deadcells
    for(int il=0; il < DeadSideCorrDB::nLayer; il++)
    {
        for(int ic=0; ic < DeadSideCorrDB::nCell; ic++)
        {
            if( status[il][ic] == 0 ) continue;
            else
            {
                if( ic <= Numcoocrlayer[il]+DeadSideCorrDB::deadrange && ic >= Numcoocrlayer[il]-DeadSideCorrDB::deadrange )
                {
                    DeadIDL.push_back(il);
                    DeadIDC.push_back(ic);

                    DeadStatus[il] = 1;
                    tmpdeadtotal++;
                }
            }
        }
    }

    return 0;
}

int DeadSideCorr::ReDeadCorr()
{
    if(tmpdeadtotal != 0) ReLongFit();

    for(int iset=0; iset < DeadIDL.size(); iset++) // iset = 0: layer 6,7 ; iset = 1: layer 14,15;
    {
            double Ecorrcell = ExpCellE(DeadIDL[iset],DeadIDC[iset]);
            if((Ecorrcell>=0.)&&(Ecorrcell<999999.)) EdepRecorr[DeadIDL[iset]][DeadIDC[iset]] = Ecorrcell;//wxq
            else cout << Form("Run %u, Event %u, Error EdepRecorr[%d,%d] value: %g",run,event,DeadIDL[iset],DeadIDC[iset],Ecorrcell) << endl;
    }

    for(int il=0; il < 18; il++)
    {
        if(DeadStatus[il] == 1)
        {
            float elayernew = 0;
            for(int ic=0; ic < 72; ic++)
            {
                elayernew += EdepRecorr[il][ic];
            }
            ElayerRecorr[il] = RePMTLayerCorr(elayernew,il);
        }
    }
    EnergyRecorr = ReCalEnergy(ElayerRecorr);
    return 0;
}

float* DeadSideCorr::DeadSideCorrection(float ecal_map[18][72], int Status[18][72], float EcalCOG[3], int datatype)
{
    DataType = datatype;

    for(int il=0; il < DeadSideCorrDB::nLayer; il++)
    {
        for(int ic=0; ic < DeadSideCorrDB::nCell; ic++)
        {
            edepattcorr[il][ic] = ecal_map[il][ic];
            EdepRecorr[il][ic] = ecal_map[il][ic];
            if(EdepRecorr[il][ic] < 0) cout << Form("Run %u, Event %u, Orignal Input Energy Map Error [%d,%d] value: %g",run,event,il,ic,EdepRecorr[il][ic]) << endl;
            status[il][ic] = Status[il][ic];
        }
    }
    ecalcog[0] = EcalCOG[0]; ecalcog[1] = EcalCOG[1]; ecalcog[2] = EcalCOG[2];

    EnergyRecorr = ReCalEnergyE();
    ReFitDirCR(0); // CooNew KCR
    ReCooLayer();
    StatusCheck();
    RePMTCorr(); // elayer_corr  //using RePMTLayerCorr();

    for(int il=0; il < 18; il++)
    {
        ElayerRecorr[il] = elayer_corr[il];
    }

    if(tmpdeadtotal != 0)
    {
        ReDeadCorr();
        for(int iset=0; iset < DeadIDL.size(); iset++)
        {
            edepattcorr[DeadIDL[iset]][DeadIDC[iset]] = EdepRecorr[DeadIDL[iset]][DeadIDC[iset]];
        }

        ReCalCOGZ();
        ReFitDirCR(1);
        ReCooLayer();
        StatusCheck();

        for(int il=0;il<18;il++)
        {
            for(int ic=0;ic<72;ic++)
            {
                if(status[il][ic]==1)
                {
                    edepattcorr[il][ic]=0.;
                    EdepRecorr[il][ic]=0.;
                }
            }
        }

        ReDeadCorr();
        for(int il=0; il < 18; il++)
        {
            elayer_corr[il] = ElayerRecorr[il];
        }
        for(int iset=0; iset < DeadIDL.size(); iset++)
        {
            edepattcorr[DeadIDL[iset]][DeadIDC[iset]] = EdepRecorr[DeadIDL[iset]][DeadIDC[iset]];
            MapEneDep[DeadIDL[iset]][DeadIDC[iset]] = EdepRecorr[DeadIDL[iset]][DeadIDC[iset]];
        }
    }

    ReCalEnergyD();

    memset(LateralLeak,0,sizeof(LateralLeak));
    int CellMax[18]={0};
    float tmpedepmax[18]={0.};
    for(int il=0;il<18;il++)
    {
        tmpedepmax[il] = -1.;
        CellMax[il] = -1;
        for(int ic=0;ic<72;ic++)
        {
             if(MapEneDep[il][ic] > tmpedepmax[il])
             {
                 tmpedepmax[il] = MapEneDep[il][ic];
                 CellMax[il] = ic;
             }
        }
    }
    
    for(int ilayer=0;ilayer<18;ilayer++)
    {
        //case 1: cell at left border -> simmetrize shower
        if (CellMax[ilayer]==0)
        {
            int iLateralLeak=1;
            while(MapEneDep[ilayer][CellMax[ilayer]+iLateralLeak]>0.&&iLateralLeak<10)
            {
                LateralLeak[ilayer][0][iLateralLeak-1] = MapEneDep[ilayer][CellMax[ilayer]+iLateralLeak];
                iLateralLeak++;
            }
        }
        //case 2: cell close to left border within 10 cells 
        //-> simmetrize shower using ratio of adjacent cells
        else if (CellMax[ilayer]<9)
        {
            //find ratio
            float LatRatio = 1.;
            if(MapEneDep[ilayer][CellMax[ilayer]+1]>0) LatRatio = MapEneDep[ilayer][CellMax[ilayer]-1]/MapEneDep[ilayer][CellMax[ilayer]+1] ;
            int iLateralLeak=1;
            while(MapEneDep[ilayer][CellMax[ilayer]+iLateralLeak]>0.&&iLateralLeak<10)
            {
                if ((int)CellMax[ilayer]-iLateralLeak < 0) LateralLeak[ilayer][0][iLateralLeak-CellMax[ilayer]-1] = LatRatio*MapEneDep[ilayer][CellMax[ilayer]+iLateralLeak];
                iLateralLeak++;
            }
        }
        //case 3: cell at right border -> simmetrize shower
        if (CellMax[ilayer]==71)
        {
            int iLateralLeak=1;
            while(MapEneDep[ilayer][CellMax[ilayer]-iLateralLeak]>0.&&iLateralLeak<10)
            {
                LateralLeak[ilayer][1][iLateralLeak-1] = MapEneDep[ilayer][CellMax[ilayer]-iLateralLeak];
                iLateralLeak++;
            }
        }
        //case 4: cell close to right border within 10 cells 
        //-> simmetrize shower using ratio of adjacent cells
        else if (CellMax[ilayer]>62)
        {
            //find ratio
            float LatRatio = 1.;
            if(MapEneDep[ilayer][CellMax[ilayer]-1]>0) LatRatio = MapEneDep[ilayer][CellMax[ilayer]+1]/MapEneDep[ilayer][CellMax[ilayer]-1] ;
            int iLateralLeak=1;
            while(MapEneDep[ilayer][CellMax[ilayer]-iLateralLeak]>0.&&iLateralLeak<10)
            {
                if ((int)CellMax[ilayer]+iLateralLeak > 71) LateralLeak[ilayer][1][iLateralLeak+CellMax[ilayer]-72] = LatRatio*MapEneDep[ilayer][CellMax[ilayer]-iLateralLeak];
                iLateralLeak++;
            }
        }
    }

    return &(EdepRecorr[0][0]);
}

void DeadSideCorr::ProcessCorr(AMSEventR* evt, EcalShowerR *pshower)
{
    ReSet();

    int DeadCellStatus[18][72]={0};
    DeadCellStatus[6][16]=1;
    DeadCellStatus[6][17]=1;
    DeadCellStatus[7][16]=1;
    DeadCellStatus[7][17]=1;
    DeadCellStatus[14][64]=1;
    DeadCellStatus[14][65]=1;
    DeadCellStatus[15][64]=1;
    DeadCellStatus[15][65]=1;

    run = evt->Run();
    event = evt->Event();
    int datatype=0;
    float shower_map[18][72]={0.};
    float EcalCOG[3]={0.};
    MCEventgR *pmc = evt->pMCEventg(0);
    if(!pmc)
    {
        if(run<1305331200) datatype = DeadSideCorrDB::_TB_DATA;
        else datatype = DeadSideCorrDB::_ISS_DATA;
    }
    else
    {
        datatype = DeadSideCorrDB::_MC_DATA;
    }

    E3C0 = pshower->Energy3C[0];
    for(int i=0;i<3;i++) EcalCOG[i] = pshower->CofG[i];

    for (int i2dcl=0;i2dcl<pshower->NEcal2DCluster();i2dcl++)
    {
        Ecal2DClusterR *p2dcl = pshower->pEcal2DCluster(i2dcl);
        if(!p2dcl || !((p2dcl->Status)&32)) continue;
        for(int i1dcl=0;i1dcl<p2dcl->NEcalCluster();i1dcl++)
        {
           EcalClusterR *p1dcl = p2dcl->pEcalCluster(i1dcl);
           if(!p1dcl || !((p1dcl->Status)&32)) continue;
           for(int ihit=0; ihit<p1dcl->NEcalHit(); ihit++)
           {
              EcalHitR *phit = p1dcl->pEcalHit(ihit);
              if(!phit) continue;
              if((phit->Status)&32)
              {
                  shower_map[phit->Plane][phit->Cell] = phit->Edep;
                  ADCH[phit->Plane][phit->Cell] = phit->ADC[0];
              }
           }
        }
    }

    for(int il=0;il<18;il++)
    {
        for(int ic=0;ic<72;ic++)
        {
            if(ADCH[il][ic]>4) MapEneDep[il][ic] = shower_map[il][ic];
            else MapEneDep[il][ic] = 0.;

            if(DeadCellStatus[il][ic]==1)
            {
                shower_map[il][ic]=0.;
                MapEneDep[il][ic] = 0.;
            }
        }
    }

    DeadSideCorrection(shower_map,DeadCellStatus,EcalCOG,datatype);
}

float DeadSideCorr::GetCorrEnergy()
{
    ReCalEnergyE();
    //variables
    float energy;
    float alpha;
    float kink_lowenergy;
    float kink_highenergy;
    correction_factor =1;

    //define kink
    kink_highenergy = 850;
    kink_lowenergy = 3;
    
    if(EnergyPIC<kink_lowenergy)
      alpha = 1/(1+TMath::Exp(1.581*(-6.131e-1 -EnergyPIC)));
    else
      if(EnergyPIC<kink_highenergy)
        alpha = 1.037 - 0.0743/EnergyPIC-0.159/TMath::Power(EnergyPIC,2)-1.9186e-9*TMath::Power(EnergyPIC,2.189);  
      else
        alpha = 0.7628+460/EnergyPIC-1.988e5/TMath::Power(EnergyPIC,2)-6.1e-7*TMath::Power(EnergyPIC,-2.537);

    correction_factor = alpha -  0.752 * efraclast2layers -  5.633 * TMath::Power(efraclast2layers,2);
    if(correction_factor>0 && correction_factor<LongLeakCorrLimit) correction_factor=LongLeakCorrLimit;

    if(correction_factor>0)
      energy = depositedenergy/correction_factor;
    else
      energy = EnergyPIC;

    //second iteration
    if(energy<kink_lowenergy)
      alpha = 1/(1+TMath::Exp(1.581*(-6.131e-1 -energy)));
    else
      if(energy<kink_highenergy)
        alpha = 1.037 - 0.0743/energy-0.159/TMath::Power(energy,2)-1.9186e-9*TMath::Power(energy,2.189);  
      else
        alpha = 0.7628+460/energy-1.988e5/TMath::Power(energy,2)-6.1e-7*TMath::Power(energy,-2.537);
    
    correction_factor = alpha -  0.752 * efraclast2layers -  5.633 * TMath::Power(efraclast2layers,2);
    if(correction_factor>0 && correction_factor<LongLeakCorrLimit) correction_factor=LongLeakCorrLimit;
    //end else for fraction
    
    //corrected value  
    if(correction_factor>0)
      energy = depositedenergy/correction_factor;
    else
      energy = depositedenergy;
    
    //reconstructed energy never less than deposited energy corrected for anode efficiency
    if(energy<depositedenergy)
      energy =  depositedenergy;

    if(energy!=energy) energy=-999.;
    
    return energy;
}

float DeadSideCorr::GetCorrEcalBDT(bool smoothed)
{
    EneDep = 0.;
    EneDep_GeV = 0.;
    ShowerMean = 0.;
    ShowerMeanNorm = 0.;
    L2LFrac = 0.;
    L2LFracNorm = 0.;
    S3totx = 0.;
    S3toty = 0.;
    S3totxNorm = 0.;
    S3totyNorm = 0.;
    R3cmFrac = 0.;
    R3cmFracNorm = 0.;
    ShowerFootprintX = 0.;
    ShowerFootprintY = 0.;
    ShowerFootprintXNorm = 0.;
    ShowerFootprintYNorm = 0.;
    NEcalHits = 0.;
    NEcalHitsNorm = 0.;
    F2SLEneDep = 0.;
    read_bdt=0.;

    memset(LayerEneDep,0,sizeof(LayerEneDep));// Energy deposit [GeV] in each layer (sum of every cell of each layer)
    memset(EneDepXY,0,sizeof(EneDepXY)); // 0 = x, 1 = y
    memset(LayerMean,0,sizeof(LayerMean)); // Mean [cell] for each layer: (sum_j j*MapEneDep[i][j])/(sum_ij MapEneDep[i][j])
    memset(LayerSigma,0,sizeof(LayerSigma));
    memset(LayerSigmaNorm,0,sizeof(LayerSigmaNorm));
    memset(LayerEneFrac,0,sizeof(LayerEneFrac));
    memset(LayerEneFracNorm,0,sizeof(LayerEneFracNorm));
    for(int il=0;il<18;il++)
    {    
        LayerS1S3[il]   = 1.;
        LayerS3Frac[il] = 1.;
    }
    memset(LayerS3FracNorm,0,sizeof(LayerS3FracNorm));
    memset(ShowerMeanXY,0,sizeof(ShowerMeanXY)); // 0 = x, 1 = y
    memset(sigmaXY,0,sizeof(sigmaXY));  // 0 = x, 1 = y
    memset(sigmaXYZ,0,sizeof(sigmaXYZ)); // 0 = x, 1 = y
    memset(sigmaZ,0,sizeof(sigmaZ));   // 0 = x, 1 = y
    memset(imaxcell,0,sizeof(imaxcell));

    if(!reader)
    {
        reader = new TMVA::Reader( "Color:Silent" );
        reader->AddSpectator("EnergyD",&CorrEnergyD);
        reader->AddVariable( "ShowerMeanNorm", &ShowerMeanNorm);
        reader->AddVariable( "F2SLEneDep", &F2SLEneDep);
        reader->AddVariable( "L2LFracNorm", &L2LFracNorm);
        reader->AddVariable( "R3cmFracNorm", &R3cmFracNorm);
        reader->AddVariable( "S3totxNorm", &S3totxNorm);
        reader->AddVariable( "S3totyNorm", &S3totyNorm);
        reader->AddVariable( "NEcalHitsNorm", &NEcalHitsNorm);
        reader->AddVariable( "ShowerFootprintXNorm", &ShowerFootprintXNorm);
        reader->AddVariable( "ShowerFootprintYNorm", &ShowerFootprintYNorm);
        reader->AddVariable( "LayerEneFracNorm0", &LayerEneFracNorm[0]);
        reader->AddVariable( "LayerEneFracNorm1", &LayerEneFracNorm[1]);
        reader->AddVariable( "LayerEneFracNorm2", &LayerEneFracNorm[2]);
        reader->AddVariable( "LayerEneFracNorm3", &LayerEneFracNorm[3]);
        reader->AddVariable( "LayerEneFracNorm4", &LayerEneFracNorm[4]);
        reader->AddVariable( "LayerEneFracNorm5", &LayerEneFracNorm[5]);
        reader->AddVariable( "LayerEneFracNorm6", &LayerEneFracNorm[6]);
        reader->AddVariable( "LayerEneFracNorm7", &LayerEneFracNorm[7]);
        reader->AddVariable( "LayerEneFracNorm8", &LayerEneFracNorm[8]);
        reader->AddVariable( "LayerEneFracNorm9", &LayerEneFracNorm[9]);
        reader->AddVariable( "LayerEneFracNorm10", &LayerEneFracNorm[10]);
        reader->AddVariable( "LayerEneFracNorm11", &LayerEneFracNorm[11]);
        reader->AddVariable( "LayerEneFracNorm12", &LayerEneFracNorm[12]);
        reader->AddVariable( "LayerEneFracNorm13", &LayerEneFracNorm[13]);
        reader->AddVariable( "LayerEneFracNorm14", &LayerEneFracNorm[14]);
        reader->AddVariable( "LayerEneFracNorm15", &LayerEneFracNorm[15]);
        reader->AddVariable( "LayerEneFracNorm16", &LayerEneFracNorm[16]);
        reader->AddVariable( "LayerEneFracNorm17", &LayerEneFracNorm[17]);
        reader->AddVariable( "LayerS1S31", &LayerS1S3[1]);
        reader->AddVariable( "LayerS1S32", &LayerS1S3[2]);
        reader->AddVariable( "LayerS1S33", &LayerS1S3[3]);
        reader->AddVariable( "LayerS1S34", &LayerS1S3[4]);
        reader->AddVariable( "LayerS1S35", &LayerS1S3[5]);
        reader->AddVariable( "LayerS1S36", &LayerS1S3[6]);
        reader->AddVariable( "LayerS1S37", &LayerS1S3[7]);
        reader->AddVariable( "LayerSigmaNorm8", &LayerSigmaNorm[8]);
        reader->AddVariable( "LayerSigmaNorm9", &LayerSigmaNorm[9]);
        reader->AddVariable( "LayerSigmaNorm10", &LayerSigmaNorm[10]);
        reader->AddVariable( "LayerSigmaNorm11", &LayerSigmaNorm[11]);
        reader->AddVariable( "LayerSigmaNorm12", &LayerSigmaNorm[12]);
        reader->AddVariable( "LayerSigmaNorm13", &LayerSigmaNorm[13]);
        reader->AddVariable( "LayerSigmaNorm14", &LayerSigmaNorm[14]);
        reader->AddVariable( "LayerSigmaNorm15", &LayerSigmaNorm[15]);
        reader->AddVariable( "LayerSigmaNorm16", &LayerSigmaNorm[16]);
        reader->AddVariable( "LayerSigmaNorm17", &LayerSigmaNorm[17]);
        reader->AddVariable( "LayerS3FracNorm1", &LayerS3FracNorm[1]);
        reader->AddVariable( "LayerS3FracNorm2", &LayerS3FracNorm[2]);
        reader->AddVariable( "LayerS3FracNorm3", &LayerS3FracNorm[3]);
        reader->AddVariable( "LayerS3FracNorm4", &LayerS3FracNorm[4]);
        reader->AddVariable( "LayerS3FracNorm5", &LayerS3FracNorm[5]);
        reader->AddVariable( "LayerS3FracNorm6", &LayerS3FracNorm[6]);
        reader->AddVariable( "LayerS3FracNorm7", &LayerS3FracNorm[7]);
        reader->AddVariable( "LayerS3FracNorm8", &LayerS3FracNorm[8]);
        reader->AddVariable( "LayerS3FracNorm9", &LayerS3FracNorm[9]);
        reader->AddVariable( "LayerS3FracNorm10", &LayerS3FracNorm[10]);
        reader->AddVariable( "LayerS3FracNorm11", &LayerS3FracNorm[11]);
        reader->AddVariable( "LayerS3FracNorm12", &LayerS3FracNorm[12]);
        reader->AddVariable( "LayerS3FracNorm13", &LayerS3FracNorm[13]);
        reader->AddVariable( "LayerS3FracNorm14", &LayerS3FracNorm[14]);
        reader->AddVariable( "LayerS3FracNorm15", &LayerS3FracNorm[15]);
        reader->AddVariable( "LayerS3FracNorm16", &LayerS3FracNorm[16]);
        reader->AddVariable( "LayerS3FracNorm17", &LayerS3FracNorm[17]);

#if TMVA_VERSION_CODE < TMVA_VERSION(4,2,0)
        const char* bdtFileNamePrefix = "ECAL_PISA_BDT_414_v7final";
#else
        const char* bdtFileNamePrefix = "bdt_root6/ECAL_PISA_BDT_414_v7final";
#endif

        reader->BookMVA( "ODD", Form("%s%s_ODD.weights.xml", AMSDATADIR.amsdatadir, bdtFileNamePrefix) );//the directory has been changed to v6.00
        reader->BookMVA( "EVEN", Form("%s%s_EVEN.weights.xml", AMSDATADIR.amsdatadir, bdtFileNamePrefix) );
    }

    if(!readerSmoothed)
    {
        readerSmoothed = new TMVA::Reader( "Color:Silent" );
        readerSmoothed->AddSpectator("EnergyD",&CorrEnergyD);
        readerSmoothed->AddVariable( "ShowerMeanNorm", &ShowerMeanNorm);
        readerSmoothed->AddVariable( "F2SLEneDep", &F2SLEneDep);
        readerSmoothed->AddVariable( "L2LFracNorm", &L2LFracNorm);
        readerSmoothed->AddVariable( "R3cmFracNorm", &R3cmFracNorm);
        readerSmoothed->AddVariable( "S3totxNorm", &S3totxNorm);
        readerSmoothed->AddVariable( "S3totyNorm", &S3totyNorm);
        readerSmoothed->AddVariable( "NEcalHitsNorm", &NEcalHitsNorm);
        readerSmoothed->AddVariable( "ShowerFootprintXNorm", &ShowerFootprintXNorm);
        readerSmoothed->AddVariable( "ShowerFootprintYNorm", &ShowerFootprintYNorm);
        readerSmoothed->AddVariable( "LayerEneFracNorm0", &LayerEneFracNorm[0]);
        readerSmoothed->AddVariable( "LayerEneFracNorm1", &LayerEneFracNorm[1]);
        readerSmoothed->AddVariable( "LayerEneFracNorm2", &LayerEneFracNorm[2]);
        readerSmoothed->AddVariable( "LayerEneFracNorm3", &LayerEneFracNorm[3]);
        readerSmoothed->AddVariable( "LayerEneFracNorm4", &LayerEneFracNorm[4]);
        readerSmoothed->AddVariable( "LayerEneFracNorm5", &LayerEneFracNorm[5]);
        readerSmoothed->AddVariable( "LayerEneFracNorm6", &LayerEneFracNorm[6]);
        readerSmoothed->AddVariable( "LayerEneFracNorm7", &LayerEneFracNorm[7]);
        readerSmoothed->AddVariable( "LayerEneFracNorm8", &LayerEneFracNorm[8]);
        readerSmoothed->AddVariable( "LayerEneFracNorm9", &LayerEneFracNorm[9]);
        readerSmoothed->AddVariable( "LayerEneFracNorm10", &LayerEneFracNorm[10]);
        readerSmoothed->AddVariable( "LayerEneFracNorm11", &LayerEneFracNorm[11]);
        readerSmoothed->AddVariable( "LayerEneFracNorm12", &LayerEneFracNorm[12]);
        readerSmoothed->AddVariable( "LayerEneFracNorm13", &LayerEneFracNorm[13]);
        readerSmoothed->AddVariable( "LayerEneFracNorm14", &LayerEneFracNorm[14]);
        readerSmoothed->AddVariable( "LayerEneFracNorm15", &LayerEneFracNorm[15]);
        readerSmoothed->AddVariable( "LayerEneFracNorm16", &LayerEneFracNorm[16]);
        readerSmoothed->AddVariable( "LayerEneFracNorm17", &LayerEneFracNorm[17]);
        readerSmoothed->AddVariable( "LayerS1S31", &LayerS1S3[1]);
        readerSmoothed->AddVariable( "LayerS1S32", &LayerS1S3[2]);
        readerSmoothed->AddVariable( "LayerS1S33", &LayerS1S3[3]);
        readerSmoothed->AddVariable( "LayerS1S34", &LayerS1S3[4]);
        readerSmoothed->AddVariable( "LayerS1S35", &LayerS1S3[5]);
        readerSmoothed->AddVariable( "LayerS1S36", &LayerS1S3[6]);
        readerSmoothed->AddVariable( "LayerS1S37", &LayerS1S3[7]);
        readerSmoothed->AddVariable( "LayerSigmaNorm8", &LayerSigmaNorm[8]);
        readerSmoothed->AddVariable( "LayerSigmaNorm9", &LayerSigmaNorm[9]);
        readerSmoothed->AddVariable( "LayerSigmaNorm10", &LayerSigmaNorm[10]);
        readerSmoothed->AddVariable( "LayerSigmaNorm11", &LayerSigmaNorm[11]);
        readerSmoothed->AddVariable( "LayerSigmaNorm12", &LayerSigmaNorm[12]);
        readerSmoothed->AddVariable( "LayerSigmaNorm13", &LayerSigmaNorm[13]);
        readerSmoothed->AddVariable( "LayerSigmaNorm14", &LayerSigmaNorm[14]);
        readerSmoothed->AddVariable( "LayerSigmaNorm15", &LayerSigmaNorm[15]);
        readerSmoothed->AddVariable( "LayerSigmaNorm16", &LayerSigmaNorm[16]);
        readerSmoothed->AddVariable( "LayerSigmaNorm17", &LayerSigmaNorm[17]);
        readerSmoothed->AddVariable( "LayerS3FracNorm1", &LayerS3FracNorm[1]);
        readerSmoothed->AddVariable( "LayerS3FracNorm2", &LayerS3FracNorm[2]);
        readerSmoothed->AddVariable( "LayerS3FracNorm3", &LayerS3FracNorm[3]);
        readerSmoothed->AddVariable( "LayerS3FracNorm4", &LayerS3FracNorm[4]);
        readerSmoothed->AddVariable( "LayerS3FracNorm5", &LayerS3FracNorm[5]);
        readerSmoothed->AddVariable( "LayerS3FracNorm6", &LayerS3FracNorm[6]);
        readerSmoothed->AddVariable( "LayerS3FracNorm7", &LayerS3FracNorm[7]);
        readerSmoothed->AddVariable( "LayerS3FracNorm8", &LayerS3FracNorm[8]);
        readerSmoothed->AddVariable( "LayerS3FracNorm9", &LayerS3FracNorm[9]);
        readerSmoothed->AddVariable( "LayerS3FracNorm10", &LayerS3FracNorm[10]);
        readerSmoothed->AddVariable( "LayerS3FracNorm11", &LayerS3FracNorm[11]);
        readerSmoothed->AddVariable( "LayerS3FracNorm12", &LayerS3FracNorm[12]);
        readerSmoothed->AddVariable( "LayerS3FracNorm13", &LayerS3FracNorm[13]);
        readerSmoothed->AddVariable( "LayerS3FracNorm14", &LayerS3FracNorm[14]);
        readerSmoothed->AddVariable( "LayerS3FracNorm15", &LayerS3FracNorm[15]);
        readerSmoothed->AddVariable( "LayerS3FracNorm16", &LayerS3FracNorm[16]);
        readerSmoothed->AddVariable( "LayerS3FracNorm17", &LayerS3FracNorm[17]);

        const char* bdtFileNamePrefix = "ECAL_PISA_BDTS_414_v7final";
        readerSmoothed->BookMVA( "ODD", Form("%s%s_ODD.weights.xml", AMSDATADIR.amsdatadir, bdtFileNamePrefix) );//the directory has been changed to v6.00
        readerSmoothed->BookMVA( "EVEN", Form("%s%s_EVEN.weights.xml", AMSDATADIR.amsdatadir, bdtFileNamePrefix) );
    }

    // Compute mean, s1, s3 and energies
    for (unsigned int ilayer = 0; ilayer < 18; ++ilayer)
    {
       UChar_t proj = !((ilayer/2) % 2);
 
       float maxcellene      = 0.;
 
       for (unsigned int icell = 0; icell < 72; ++icell)
       {
           if (MapEneDep[ilayer][icell] <= EneDepThreshold ) continue;
 	
           LayerEneDep[ilayer] += MapEneDep[ilayer][icell];
 	
           ++NEcalHits;
 
         if (MapEneDep[ilayer][icell] >= maxcellene)
          {
             maxcellene = MapEneDep[ilayer][icell];
             imaxcell[ilayer]   = icell;
          }
 
          LayerMean[ilayer]  += (icell + 1)*MapEneDep[ilayer][icell];
          ShowerMeanXY[proj] += (icell + 1)*MapEneDep[ilayer][icell];
       }

        if(imaxcell[ilayer]<9)
        {
            for(int ic=0;ic<10;ic++)
            {
                if(LateralLeak[ilayer][0][ic]>EneDepThreshold)
                {
                    LayerEneDep[ilayer]+=LateralLeak[ilayer][0][ic];
                    ++NEcalHits;
                }
            }
        }

        if(imaxcell[ilayer]>62)
        {
            for(int ic=0;ic<10;ic++)
            {
                if(LateralLeak[ilayer][1][ic]>EneDepThreshold)
                {
                    LayerEneDep[ilayer]+=LateralLeak[ilayer][1][ic];
                    ++NEcalHits;
                }
            }
        }

        ShowerMean += (ilayer + 1)*LayerEneDep[ilayer];
        EneDep         += LayerEneDep[ilayer];
        EneDepXY[proj] += LayerEneDep[ilayer];
  
        float S1 = MapEneDep[ilayer][imaxcell[ilayer]];
        float S3 = S1;
        if (imaxcell[ilayer]==0) S3 += LateralLeak[ilayer][0][0];
        if (imaxcell[ilayer]==71) S3 += LateralLeak[ilayer][1][0];
        if (imaxcell[ilayer] > 0) S3 += MapEneDep[ilayer][imaxcell[ilayer] - 1];
        if (imaxcell[ilayer] < 71) S3 += MapEneDep[ilayer][imaxcell[ilayer] + 1];
  
        if (!proj) S3totx += S3;
        else S3toty += S3;
  
        if (S1*S3 > 0.) LayerS1S3[ilayer] = S1/S3;
  
        if (LayerEneDep[ilayer] > 0.)
        {
           LayerS3Frac[ilayer] = S3/LayerEneDep[ilayer];

           //==========wxq1==========
           if(imaxcell[ilayer]<9)
           {
              for(int p1=0;p1<10;p1++)
              {
                 if (LateralLeak[ilayer][0][p1] <= EneDepThreshold ) continue;
                 LayerMean[ilayer] += (-p1)*LateralLeak[ilayer][0][p1];
                 ShowerMeanXY[proj] += (-p1)*LateralLeak[ilayer][0][p1];
              }
           }
           if(imaxcell[ilayer]>62)
           {
              for(int p2=0;p2<10;p2++)
              {
                 if (LateralLeak[ilayer][1][p2] <= EneDepThreshold ) continue;
                 LayerMean[ilayer] += (p2+73)*LateralLeak[ilayer][1][p2];
                 ShowerMeanXY[proj] += (p2+73)*LateralLeak[ilayer][1][p2];
              }
           }
           //==========wxq2==========

           LayerMean[ilayer]   = LayerMean[ilayer]/LayerEneDep[ilayer];
        }
        else
        {
           LayerMean[ilayer] = -1.;
        }

    } //end of loop on layers
     
    //if (EneDep <= 0. || LayerEnergy <= 0. || EneDepXY[0] <= 0. || EneDepXY[1] <= 0.) return -0.999;
 
    ShowerMeanXY[0] /= EneDepXY[0];
    ShowerMeanXY[1] /= EneDepXY[1];
    ShowerMean      /= EneDep;
    EneDep_GeV = EneDep/1000.;
 
    F2SLEneDep = (LayerEneDep[0] + LayerEneDep[1] + LayerEneDep[2] + LayerEneDep[3])/1000.;
    L2LFrac    = (LayerEneDep[16] + LayerEneDep[17])/EneDep;
 
    S3totx /= EneDepXY[0];
    S3toty /= EneDepXY[1];
 
    for (unsigned int ilayer = 0; ilayer < 18; ++ilayer)
    {
       if (LayerEneDep[ilayer] == 0.) continue;
 
       UChar_t proj = !((ilayer/2) % 2);
 
       LayerEneFrac[ilayer] = LayerEneDep[ilayer]/EneDep;
 
       for (unsigned int icell = 0; icell < 72; ++icell)
       {
          if (MapEneDep[ilayer][icell]  <= EneDepThreshold) continue;
          LayerSigma[ilayer] += TMath::Power(icell + 1 - LayerMean[ilayer], 2)*MapEneDep[ilayer][icell];
          sigmaXY[proj]  += TMath::Power(icell + 1 - ShowerMeanXY[proj], 2)*MapEneDep[ilayer][icell];
          sigmaXYZ[proj] += (icell + 1 - ShowerMeanXY[proj])*(ilayer + 1 - ShowerMean)*MapEneDep[ilayer][icell];
          sigmaZ[proj]   += TMath::Power(ilayer + 1 - ShowerMean, 2)*MapEneDep[ilayer][icell];
       }

       //==========wxq1==========
       if(imaxcell[ilayer]<9)
       {
          for(int p1=0;p1<10;p1++)
          {
             if (LateralLeak[ilayer][0][p1] <= EneDepThreshold ) continue;
             LayerSigma[ilayer] += TMath::Power(-p1 - LayerMean[ilayer], 2)*LateralLeak[ilayer][0][p1];
             sigmaXY[proj]  += TMath::Power(-p1 - ShowerMeanXY[proj], 2)*LateralLeak[ilayer][0][p1];
             sigmaXYZ[proj] += (-p1 - ShowerMeanXY[proj])*(ilayer +1 - ShowerMean)*LateralLeak[ilayer][0][p1];
             sigmaZ[proj]   += TMath::Power(ilayer + 1 - ShowerMean, 2)*LateralLeak[ilayer][0][p1];
          }
       }
       if(imaxcell[ilayer]>62)
       {
          for(int p2=0;p2<10;p2++)
          {
             if (LateralLeak[ilayer][1][p2] <= EneDepThreshold ) continue;
             LayerSigma[ilayer] += TMath::Power(p2+73 - LayerMean[ilayer], 2)*LateralLeak[ilayer][1][p2];
             sigmaXY[proj]  += TMath::Power(p2+73 - ShowerMeanXY[proj], 2)*LateralLeak[ilayer][1][p2];
             sigmaXYZ[proj] += (p2+73 - ShowerMeanXY[proj])*(ilayer + 1 - ShowerMean)*LateralLeak[ilayer][1][p2];
             sigmaZ[proj]   += TMath::Power(ilayer + 1 - ShowerMean, 2)*LateralLeak[ilayer][1][p2];
          }
       }
       //==========wxq2==========

       if (LayerEneDep[ilayer] > 0.)
          LayerSigma[ilayer] = TMath::Sqrt(LayerSigma[ilayer]/LayerEneDep[ilayer]);
       else LayerSigma[ilayer] = -1.;
    }
 
    ShowerMean  = ShowerMean - 1.;
 
    for (unsigned int iproj = 0; iproj < 2; ++iproj)
    {
       sigmaXY[iproj]  /= EneDepXY[iproj];
       sigmaXYZ[iproj] /= EneDepXY[iproj];
       sigmaZ[iproj]   /= EneDepXY[iproj];
    }

    ShowerFootprintX = TMath::Sqrt(TMath::Abs(sigmaXY[0]*sigmaZ[0] - TMath::Power(sigmaXYZ[0], 2)));
    ShowerFootprintY = TMath::Sqrt(TMath::Abs(sigmaXY[1]*sigmaZ[1] - TMath::Power(sigmaXYZ[1], 2)));
 
    // Compute R3cmFrac
    if(CorrEnergyD>10.)
    {
        float distance=0., r3_energy=0., r5_energy=0.;
        for(int k1=0;k1<18;k1++)
        {
            int proj = 1-k1/2%2;
            float cosa = fabs(cos(atan(KCR[proj])));
            for(int k2=0;k2<72;k2++)
            {
                if (MapEneDep[k1][k2]  <= EneDepThreshold) continue;
                distance = fabs((k2-35.5)*0.9-coocrreadout[k1])*cosa;
                if(distance<3) r3_energy += MapEneDep[k1][k2];
                if(distance<5) r5_energy += MapEneDep[k1][k2];
            }

            if(imaxcell[k1]<9)
            {
               for(int p1=0;p1<10;p1++)
               {
                   if (LateralLeak[k1][0][p1] <= EneDepThreshold ) continue;
                   distance = fabs((-p1-1-35.5)*0.9-coocrreadout[k1])*cosa;
                   if(distance<3) r3_energy += LateralLeak[k1][0][p1];
                   if(distance<5) r5_energy += LateralLeak[k1][0][p1];
               }
            }
            if(imaxcell[k1]>62)
            {
               for(int p1=0;p1<10;p1++)
               {
                   if (LateralLeak[k1][1][p1] <= EneDepThreshold ) continue;
                   distance = fabs((p1+72-35.5)*0.9-coocrreadout[k1])*cosa;
                   if(distance<3) r3_energy += LateralLeak[k1][1][p1];
                   if(distance<5) r5_energy += LateralLeak[k1][1][p1];
               }
            }
        }

        R3cmFrac = r3_energy/EneDep;
    }
    else
    {
        R3cmFrac = E3C0;
    }


    //********************************
    //*****Normalize the Variables****
    //********************************
    // NOTE: starting from version 5, variable 2 has changed from ShowerSigma to F2SLEneDep
    //       and from LayerSigmaNorm1-7 to LayerS1S31-7 .
    //       The new variables ARE NOT normalized.
    // fix: normalization may be NaN below 2 GeV or above 1000 GeV;
    float energyd = CorrEnergyD; 
    energyd = energyd < 1. ? 1. : (energyd > 1000. ? 1000. : energyd);
    float mean, sigma;
    float x = log(energyd);
 
    //if (MDT_DEBUG) cout << Form("RUN %d EVENT %d --- EnergyD=%f",pev->Run(),pev->Event(),energyd) <<endl;
    //if (BDT_DEBUG) std::cout << Form(" ??? x=%f\n", x) << flush;
 
    //ivar = 0;
 
    mean = 5.81753*(((((1+(-0.0652423*x))+(0.123152*pow(x,2)))+(-0.0309516*(pow(x,3))))+(0.00355938*(pow(x,4))))+(-0.000154605*(pow(x,5))));
    sigma = 0.654693*(((((1+(0.349885*x))+(-0.258381*pow(x,2)))+(0.0762609*(pow(x,3))))+(-0.0101505*(pow(x,4))))+(0.000480682*(pow(x,5))));
    ShowerMeanNorm = (ShowerMean-mean)/sigma;
 
    //pisanormvar[ivar++] = F2SLEneDep;
 
    mean = -0.00563546*(((((1+(-4.90888*x))+(3.06143*pow(x,2)))+(-1.19568*(pow(x,3))))+(0.200186*(pow(x,4))))+(-0.0124911*(pow(x,5))));
    sigma = 0.0202379*(((((1+(-0.700595*x))+(0.315499*pow(x,2)))+(-0.0637316*(pow(x,3))))+(0.00687652*(pow(x,4))))+(-0.000297002*(pow(x,5))));
    L2LFracNorm = (L2LFrac-mean)/sigma;
 
    mean = 0.976096*(((((1+(-0.00594019*x))+(0.00626969*pow(x,2)))+(-0.00277484*(pow(x,3))))+(0.000469374*(pow(x,4))))+(-2.69748e-05*(pow(x,5))));
    sigma = 0.0202693*(((((1+(-0.450749*x))+(0.156631*pow(x,2)))+(-0.0379591*(pow(x,3))))+(0.00471605*(pow(x,4))))+(-0.000217455*(pow(x,5))));
    R3cmFracNorm = (R3cmFrac-mean)/sigma;
 
    mean = 0.947438*(((((1+(-0.0958179*x))+(0.0542999*pow(x,2)))+(-0.0149508*(pow(x,3))))+(0.00197748*(pow(x,4))))+(-9.96043e-05*(pow(x,5))));
    sigma = 0.0514221*(((((1+(-0.191206*x))+(-0.081866*pow(x,2)))+(0.0396561*(pow(x,3))))+(-0.00606394*(pow(x,4))))+(0.000326768*(pow(x,5))));
    S3totxNorm = (S3totx-mean)/sigma;
 
    mean = 0.894616*(((((1+(-0.0830502*x))+(0.0602628*pow(x,2)))+(-0.0178786*(pow(x,3))))+(0.00239022*(pow(x,4))))+(-0.000118785*(pow(x,5))));
    sigma = 0.074716*(((((1+(-0.194879*x))+(-0.118629*pow(x,2)))+(0.0562894*(pow(x,3))))+(-0.00869822*(pow(x,4))))+(0.000470664*(pow(x,5))));
    S3totyNorm = (S3toty-mean)/sigma;
 
    mean = 17.7832*(((((1+(4.11748*x))+(-1.32613*pow(x,2)))+(0.378096*(pow(x,3))))+(-0.0420122*(pow(x,4))))+(0.00192212*(pow(x,5))));
    sigma = -7.45314*(((((1+(-4.362*x))+(3.04245*pow(x,2)))+(-0.998655*(pow(x,3))))+(0.144479*(pow(x,4))))+(-0.00759756*(pow(x,5))));
    NEcalHitsNorm = (NEcalHits-mean)/sigma;
 
    mean = 2.08418*(((((1+(1.53685*x))+(-0.960584*pow(x,2)))+(0.28049*(pow(x,3))))+(-0.037519*(pow(x,4))))+(0.00186672*(pow(x,5))));
    sigma = 1.08555*(((((1+(0.301398*x))+(-0.486968*pow(x,2)))+(0.167722*(pow(x,3))))+(-0.0236229*(pow(x,4))))+(0.00120627*(pow(x,5))));
    ShowerFootprintXNorm = (ShowerFootprintX-mean)/sigma;
 
    mean = 3.06557*(((((1+(1.38162*x))+(-0.972884*pow(x,2)))+(0.288618*(pow(x,3))))+(-0.0381605*(pow(x,4))))+(0.00186877*(pow(x,5))));
    sigma = 1.97309*(((((1+(0.0319007*x))+(-0.296926*pow(x,2)))+(0.108391*(pow(x,3))))+(-0.0151688*(pow(x,4))))+(0.000761558*(pow(x,5))));
    ShowerFootprintYNorm = (ShowerFootprintY-mean)/sigma;
 
    mean = 0.0154335*(((((1+(-0.0374406*x))+(-0.1801*pow(x,2)))+(0.0575964*(pow(x,3))))+(-0.00701209*(pow(x,4))))+(0.000309315*(pow(x,5))));
    sigma = -0.0116612*(((((1+(-2.62644*x))+(1.4815*pow(x,2)))+(-0.352021*(pow(x,3))))+(0.0384267*(pow(x,4))))+(-0.00159106*(pow(x,5))));
    LayerEneFracNorm[0] = (LayerEneFrac[0]-mean)/sigma;
 
    mean = 0.0633498*(((((1+(-0.114734*x))+(-0.154378*pow(x,2)))+(0.0562252*(pow(x,3))))+(-0.00731868*(pow(x,4))))+(0.000337497*(pow(x,5))));
    sigma = 0.0316538*(((((1+(-0.137904*x))+(-0.15952*pow(x,2)))+(0.0611123*(pow(x,3))))+(-0.00813668*(pow(x,4))))+(0.000378855*(pow(x,5))));
    LayerEneFracNorm[1] = (LayerEneFrac[1]-mean)/sigma;
 
    mean = 0.0981715*(((((1+(0.086794*x))+(-0.25229*pow(x,2)))+(0.076108*(pow(x,3))))+(-0.00930538*(pow(x,4))))+(0.000418156*(pow(x,5))));
    sigma = 0.0584895*(((((1+(-0.518405*x))+(0.147205*pow(x,2)))+(-0.0279128*(pow(x,3))))+(0.00302415*(pow(x,4))))+(-0.000133801*(pow(x,5))));
    LayerEneFracNorm[2] = (LayerEneFrac[2]-mean)/sigma;
 
    mean = 0.0913995*(((((1+(0.606592*x))+(-0.534856*pow(x,2)))+(0.142581*(pow(x,3))))+(-0.0169281*(pow(x,4))))+(0.000761466*(pow(x,5))));
    sigma = 0.0521775*(((((1+(-0.370914*x))+(0.102182*pow(x,2)))+(-0.0221373*(pow(x,3))))+(0.00253141*(pow(x,4))))+(-0.000108831*(pow(x,5))));
    LayerEneFracNorm[3] = (LayerEneFrac[3]-mean)/sigma;
 
    mean = 0.0900602*(((((1+(0.623416*x))+(-0.425984*pow(x,2)))+(0.097747*(pow(x,3))))+(-0.010668*(pow(x,4))))+(0.000463787*(pow(x,5))));
    sigma = 0.0505887*(((((1+(-0.486493*x))+(0.195421*pow(x,2)))+(-0.0430779*(pow(x,3))))+(0.00438646*(pow(x,4))))+(-0.000166038*(pow(x,5))));
    LayerEneFracNorm[4] = (LayerEneFrac[4]-mean)/sigma;
 
    mean = 0.0820018*(((((1+(0.588403*x))+(-0.295374*pow(x,2)))+(0.0567273*(pow(x,3))))+(-0.00600898*(pow(x,4))))+(0.000280536*(pow(x,5))));
    sigma = 0.0542134*(((((1+(-0.524149*x))+(0.136043*pow(x,2)))+(-0.00183907*(pow(x,3))))+(-0.00348656*(pow(x,4))))+(0.000309278*(pow(x,5))));
    LayerEneFracNorm[5] = (LayerEneFrac[5]-mean)/sigma;
 
    mean = 0.0647088*(((((1+(0.755927*x))+(-0.322877*pow(x,2)))+(0.0691826*(pow(x,3))))+(-0.00892412*(pow(x,4))))+(0.000476596*(pow(x,5))));
    sigma = 0.0460868*(((((1+(-0.426239*x))+(0.0999163*pow(x,2)))+(-0.0125167*(pow(x,3))))+(0.00141731*(pow(x,4))))+(-0.000105953*(pow(x,5))));
    LayerEneFracNorm[6] = (LayerEneFrac[6]-mean)/sigma;
 
    mean = 0.0591098*(((((1+(0.490957*x))+(-0.117617*pow(x,2)))+(0.0186932*(pow(x,3))))+(-0.00285173*(pow(x,4))))+(0.000169852*(pow(x,5))));
    sigma = 0.0441187*(((((1+(-0.424288*x))+(0.120773*pow(x,2)))+(-0.0303416*(pow(x,3))))+(0.00522312*(pow(x,4))))+(-0.000345832*(pow(x,5))));
    LayerEneFracNorm[7] = (LayerEneFrac[7]-mean)/sigma;
 
    mean = 0.0544055*(((((1+(0.185537*x))+(0.115868*pow(x,2)))+(-0.0463891*(pow(x,3))))+(0.00617888*(pow(x,4))))+(-0.000325102*(pow(x,5))));
    sigma = 0.0358471*(((((1+(-0.404967*x))+(0.161259*pow(x,2)))+(-0.0523526*(pow(x,3))))+(0.00851722*(pow(x,4))))+(-0.000492585*(pow(x,5))));
    LayerEneFracNorm[8] = (LayerEneFrac[8]-mean)/sigma;
 
    mean = 0.0586046*(((((1+(-0.35631*x))+(0.418475*pow(x,2)))+(-0.123614*(pow(x,3))))+(0.0161746*(pow(x,4))))+(-0.000828849*(pow(x,5))));
    sigma = 0.0195272*(((((1+(0.15923*x))+(-0.127709*pow(x,2)))+(0.0134291*(pow(x,3))))+(0.000637799*(pow(x,4))))+(-9.40079e-05*(pow(x,5))));
    LayerEneFracNorm[9] = (LayerEneFrac[9]-mean)/sigma;
 
    mean = 0.0384656*(((((1+(-0.106815*x))+(0.243958*pow(x,2)))+(-0.0433174*(pow(x,3))))+(0.00189987*(pow(x,4))))+(4.54321e-05*(pow(x,5))));
    sigma = 0.0193138*(((((1+(0.109858*x))+(-0.222064*pow(x,2)))+(0.0902414*(pow(x,3))))+(-0.0161111*(pow(x,4))))+(0.00103071*(pow(x,5))));
    LayerEneFracNorm[10] = (LayerEneFrac[10]-mean)/sigma;
 
    mean = 0.0288085*(((((1+(-0.203208*x))+(0.392827*pow(x,2)))+(-0.0992274*(pow(x,3))))+(0.0124898*(pow(x,4))))+(-0.000668279*(pow(x,5))));
    sigma = 0.0276025*(((((1+(-0.717605*x))+(0.363821*pow(x,2)))+(-0.0791709*(pow(x,3))))+(0.0067021*(pow(x,4))))+(-0.000146176*(pow(x,5))));
    LayerEneFracNorm[11] = (LayerEneFrac[11]-mean)/sigma;
 
    mean = 0.0113727*(((((1+(1.25241*x))+(-0.32621*pow(x,2)))+(0.160578*(pow(x,3))))+(-0.0269675*(pow(x,4))))+(0.00150497*(pow(x,5))));
    sigma = -0.00706408*(((((1+(-4.56671*x))+(2.99638*pow(x,2)))+(-0.934686*(pow(x,3))))+(0.136241*(pow(x,4))))+(-0.00736457*(pow(x,5))));
    LayerEneFracNorm[12] = (LayerEneFrac[12]-mean)/sigma;
 
    mean = -0.0014262*(((((1+(-18.5479*x))+(8.50689*pow(x,2)))+(-2.91282*(pow(x,3))))+(0.428928*(pow(x,4))))+(-0.0233242*(pow(x,5))));
    sigma = 0.000632082*(((((1+(29.7718*x))+(-20.4101*pow(x,2)))+(6.333*(pow(x,3))))+(-0.886025*(pow(x,4))))+(0.0453158*(pow(x,5))));
    LayerEneFracNorm[13] = (LayerEneFrac[13]-mean)/sigma;
 
    mean = 0.00206941*(((((1+(6.37561*x))+(-2.65265*pow(x,2)))+(1.07375*(pow(x,3))))+(-0.171301*(pow(x,4))))+(0.0102091*(pow(x,5))));
    sigma = -0.0135454*(((((1+(-2.7593*x))+(1.80845*pow(x,2)))+(-0.554322*(pow(x,3))))+(0.0783147*(pow(x,4))))+(-0.0041224*(pow(x,5))));
    LayerEneFracNorm[14] = (LayerEneFrac[14]-mean)/sigma;
 
    mean = -0.00391276*(((((1+(-4.63399*x))+(2.29847*pow(x,2)))+(-0.803834*(pow(x,3))))+(0.122204*(pow(x,4))))+(-0.00712181*(pow(x,5))));
    sigma = -0.0100079*(((((1+(-2.74935*x))+(1.60623*pow(x,2)))+(-0.42156*(pow(x,3))))+(0.0491247*(pow(x,4))))+(-0.002086*(pow(x,5))));
    LayerEneFracNorm[15] = (LayerEneFrac[15]-mean)/sigma;
 
    mean = 0.0118102*(((((1+(-0.976311*x))+(0.686435*pow(x,2)))+(-0.133816*(pow(x,3))))+(0.0104236*(pow(x,4))))+(-6.07892e-05*(pow(x,5))));
    sigma = -0.0132099*(((((1+(-2.47328*x))+(1.61924*pow(x,2)))+(-0.501154*(pow(x,3))))+(0.0719867*(pow(x,4))))+(-0.0038697*(pow(x,5))));
    LayerEneFracNorm[16] = (LayerEneFrac[16]-mean)/sigma;
 
    mean = 0.018997*(((((1+(-1.46883*x))+(0.958373*pow(x,2)))+(-0.244195*(pow(x,3))))+(0.0286415*(pow(x,4))))+(-0.00117624*(pow(x,5))));
    sigma = -0.00102647*(((((1+(-4.50726*x))+(0.00253443*pow(x,2)))+(0.603813*(pow(x,3))))+(-0.144358*(pow(x,4))))+(0.00928059*(pow(x,5))));
    LayerEneFracNorm[17] = (LayerEneFrac[17]-mean)/sigma;
 
    //pisanormvar[ivar++] = LayerS1S3[1];
 
    //pisanormvar[ivar++] = LayerS1S3[2];
 
    //pisanormvar[ivar++] = LayerS1S3[3];
 
    //pisanormvar[ivar++] = LayerS1S3[4];
 
    //pisanormvar[ivar++] = LayerS1S3[5];
 
    //pisanormvar[ivar++] = LayerS1S3[6];
 
    //pisanormvar[ivar++] = LayerS1S3[7];
 
    mean = 0.557175*(((((1+(2.12671*x))+(-1.4178*pow(x,2)))+(0.409941*(pow(x,3))))+(-0.0540956*(pow(x,4))))+(0.00267714*(pow(x,5))));
    sigma = 0.00897424*(((((1+(80.9882*x))+(-57.6913*pow(x,2)))+(16.3878*(pow(x,3))))+(-2.09909*(pow(x,4))))+(0.100461*(pow(x,5))));
    LayerSigmaNorm[8] = (LayerSigma[8]-mean)/sigma;
 
    mean = 0.825791*(((((1+(1.22133*x))+(-0.880486*pow(x,2)))+(0.267216*(pow(x,3))))+(-0.0366376*(pow(x,4))))+(0.00186991*(pow(x,5))));
    sigma = 0.465602*(((((1+(0.508751*x))+(-0.654088*pow(x,2)))+(0.221749*(pow(x,3))))+(-0.0314511*(pow(x,4))))+(0.00161545*(pow(x,5))));
    LayerSigmaNorm[9] = (LayerSigma[9]-mean)/sigma;
 
    mean = 0.910081*(((((1+(0.739722*x))+(-0.509627*pow(x,2)))+(0.159914*(pow(x,3))))+(-0.0229559*(pow(x,4))))+(0.001221*(pow(x,5))));
    sigma = 0.205266*(((((1+(2.68981*x))+(-2.12361*pow(x,2)))+(0.623894*(pow(x,3))))+(-0.0819358*(pow(x,4))))+(0.0040157*(pow(x,5))));
    LayerSigmaNorm[10] = (LayerSigma[10]-mean)/sigma;
 
    mean = 0.0475479*(((((1+(46.9903*x))+(-28.6912*pow(x,2)))+(8.10759*(pow(x,3))))+(-1.06855*(pow(x,4))))+(0.0529542*(pow(x,5))));
    sigma = -0.0118896*(((((1+(-94.7724*x))+(68.6104*pow(x,2)))+(-19.8302*(pow(x,3))))+(2.58454*(pow(x,4))))+(-0.125683*(pow(x,5))));
    LayerSigmaNorm[11] = (LayerSigma[11]-mean)/sigma;
 
    mean = -1.15196*(((((1+(-4.18659*x))+(2.54371*pow(x,2)))+(-0.698499*(pow(x,3))))+(0.0890199*(pow(x,4))))+(-0.00427228*(pow(x,5))));
    sigma = -0.422523*(((((1+(-5.21731*x))+(3.55303*pow(x,2)))+(-0.9936*(pow(x,3))))+(0.125859*(pow(x,4))))+(-0.00596396*(pow(x,5))));
    LayerSigmaNorm[12] = (LayerSigma[12]-mean)/sigma;
 
    mean = -2.29679*(((((1+(-2.7974*x))+(1.5947*pow(x,2)))+(-0.416448*(pow(x,3))))+(0.0507933*(pow(x,4))))+(-0.00234097*(pow(x,5))));
    sigma = -1.22776*(((((1+(-2.77693*x))+(1.71971*pow(x,2)))+(-0.457114*(pow(x,3))))+(0.0557562*(pow(x,4))))+(-0.00255766*(pow(x,5))));
    LayerSigmaNorm[13] = (LayerSigma[13]-mean)/sigma;
 
    mean = -3.65486*(((((1+(-2.17485*x))+(1.16497*pow(x,2)))+(-0.291875*(pow(x,3))))+(0.0345623*(pow(x,4))))+(-0.00155801*(pow(x,5))));
    sigma = -1.55552*(((((1+(-2.25803*x))+(1.25426*pow(x,2)))+(-0.303822*(pow(x,3))))+(0.0341468*(pow(x,4))))+(-0.00145752*(pow(x,5))));
    LayerSigmaNorm[14] = (LayerSigma[14]-mean)/sigma;
 
    mean = -5.07802*(((((1+(-1.84923*x))+(0.927057*pow(x,2)))+(-0.219636*(pow(x,3))))+(0.0247829*(pow(x,4))))+(-0.00107054*(pow(x,5))));
    sigma = -2.64475*(((((1+(-1.62379*x))+(0.757685*pow(x,2)))+(-0.156366*(pow(x,3))))+(0.0149065*(pow(x,4))))+(-0.000533412*(pow(x,5))));
    LayerSigmaNorm[15] = (LayerSigma[15]-mean)/sigma;
 
    mean = -8.09602*(((((1+(-1.62571*x))+(0.788618*pow(x,2)))+(-0.181389*(pow(x,3))))+(0.0199756*(pow(x,4))))+(-0.000846765*(pow(x,5))));
    sigma = -3.55731*(((((1+(-1.67015*x))+(0.82708*pow(x,2)))+(-0.184353*(pow(x,3))))+(0.0193368*(pow(x,4))))+(-0.000777463*(pow(x,5))));
    LayerSigmaNorm[16] = (LayerSigma[16]-mean)/sigma;
 
    mean = -3.12835*(((((1+(-1.7683*x))+(0.662891*pow(x,2)))+(-0.110456*(pow(x,3))))+(0.00795358*(pow(x,4))))+(-0.0001761*(pow(x,5))));
    sigma = -2.70441*(((((1+(-1.70159*x))+(0.783491*pow(x,2)))+(-0.157919*(pow(x,3))))+(0.0146316*(pow(x,4))))+(-0.000505912*(pow(x,5))));
    LayerSigmaNorm[17] = (LayerSigma[17]-mean)/sigma;
 
    mean = 2.71022*(((((1+(-0.130137*x))+(0.129101*pow(x,2)))+(-0.0405934*(pow(x,3))))+(0.00526541*(pow(x,4))))+(-0.000254535*(pow(x,5))));
    sigma = 0.433849*(((((1+(1.74973*x))+(-1.29381*pow(x,2)))+(0.370721*(pow(x,3))))+(-0.0480519*(pow(x,4))))+(0.00236235*(pow(x,5))));
    LayerS3FracNorm[1] = (LayerS3Frac[1]-mean)/sigma;
 
    mean = 0.830443*(((((1+(0.306295*x))+(-0.149546*pow(x,2)))+(0.0335532*(pow(x,3))))+(-0.00362516*(pow(x,4))))+(0.000148019*(pow(x,5))));
    sigma = -0.0215234*(((((1+(-17.32*x))+(10.8897*pow(x,2)))+(-2.92155*(pow(x,3))))+(0.366721*(pow(x,4))))+(-0.0177822*(pow(x,5))));
    LayerS3FracNorm[2] = (LayerS3Frac[2]-mean)/sigma;
 
    mean = 0.878041*(((((1+(0.168915*x))+(-0.0904153*pow(x,2)))+(0.0220529*(pow(x,3))))+(-0.00257015*(pow(x,4))))+(0.000113793*(pow(x,5))));
    sigma = -0.0332275*(((((1+(-4.50798*x))+(2.89585*pow(x,2)))+(-0.803476*(pow(x,3))))+(0.103883*(pow(x,4))))+(-0.00512917*(pow(x,5))));
    LayerS3FracNorm[3] = (LayerS3Frac[3]-mean)/sigma;
 
    mean = 1.08295*(((((1+(-0.117056*x))+(0.0453468*pow(x,2)))+(-0.00841535*(pow(x,3))))+(0.000742509*(pow(x,4))))+(-2.56053e-05*(pow(x,5))));
    sigma = 0.492885*(((((1+(-0.864558*x))+(0.31567*pow(x,2)))+(-0.0581046*(pow(x,3))))+(0.00531972*(pow(x,4))))+(-0.000192369*(pow(x,5))));
    LayerS3FracNorm[4] = (LayerS3Frac[4]-mean)/sigma;
 
    mean = 0.966137*(((((1+(-0.0447478*x))+(0.024797*pow(x,2)))+(-0.00580008*(pow(x,3))))+(0.000631017*(pow(x,4))))+(-2.67751e-05*(pow(x,5))));
    sigma = 0.324553*(((((1+(-0.739022*x))+(0.220331*pow(x,2)))+(-0.0297578*(pow(x,3))))+(0.00153657*(pow(x,4))))+(-4.23326e-06*(pow(x,5))));
    LayerS3FracNorm[5] = (LayerS3Frac[5]-mean)/sigma;
 
    mean = 0.918183*(((((1+(0.0245774*x))+(-0.0165454*pow(x,2)))+(0.00513393*(pow(x,3))))+(-0.000687314*(pow(x,4))))+(3.29047e-05*(pow(x,5))));
    sigma = 0.144763*(((((1+(-0.652088*x))+(0.223288*pow(x,2)))+(-0.0435651*(pow(x,3))))+(0.00449212*(pow(x,4))))+(-0.000187523*(pow(x,5))));
    LayerS3FracNorm[6] = (LayerS3Frac[6]-mean)/sigma;
 
    mean = 0.907456*(((((1+(-0.00101406*x))+(0.00160197*pow(x,2)))+(-5.47284e-05*(pow(x,3))))+(4.57173e-06*(pow(x,4))))+(-2.01113e-06*(pow(x,5))));
    sigma = 0.137874*(((((1+(-0.50364*x))+(0.11603*pow(x,2)))+(-0.0126287*(pow(x,3))))+(0.000406567*(pow(x,4))))+(1.59863e-05*(pow(x,5))));
    LayerS3FracNorm[7] = (LayerS3Frac[7]-mean)/sigma;
 
    mean = 0.914388*(((((1+(-0.157968*x))+(0.113621*pow(x,2)))+(-0.0320707*(pow(x,3))))+(0.00411686*(pow(x,4))))+(-0.000198991*(pow(x,5))));
    sigma = 0.29723*(((((1+(-0.714645*x))+(0.237853*pow(x,2)))+(-0.0420266*(pow(x,3))))+(0.00377121*(pow(x,4))))+(-0.000133952*(pow(x,5))));
    LayerS3FracNorm[8] = (LayerS3Frac[8]-mean)/sigma;
 
    mean = 0.912766*(((((1+(-0.211328*x))+(0.147518*pow(x,2)))+(-0.0414031*(pow(x,3))))+(0.00532721*(pow(x,4))))+(-0.000258884*(pow(x,5))));
    sigma = 0.267796*(((((1+(-0.496191*x))+(0.0753246*pow(x,2)))+(0.0053784*(pow(x,3))))+(-0.00245852*(pow(x,4))))+(0.000172211*(pow(x,5))));
    LayerS3FracNorm[9] = (LayerS3Frac[9]-mean)/sigma;
 
    mean = 0.882403*(((((1+(-0.155903*x))+(0.108684*pow(x,2)))+(-0.0309816*(pow(x,3))))+(0.00412699*(pow(x,4))))+(-0.00020889*(pow(x,5))));
    sigma = 0.251275*(((((1+(-0.532008*x))+(0.1279*pow(x,2)))+(-0.0150123*(pow(x,3))))+(0.000638418*(pow(x,4))))+(7.92237e-06*(pow(x,5))));
    LayerS3FracNorm[10] = (LayerS3Frac[10]-mean)/sigma;
 
    mean = 0.91556*(((((1+(-0.273105*x))+(0.178677*pow(x,2)))+(-0.0495976*(pow(x,3))))+(0.00643063*(pow(x,4))))+(-0.000316353*(pow(x,5))));
    sigma = 0.367702*(((((1+(-0.592629*x))+(0.151355*pow(x,2)))+(-0.0180565*(pow(x,3))))+(0.000785277*(pow(x,4))))+(4.72664e-06*(pow(x,5))));
    LayerS3FracNorm[11] = (LayerS3Frac[11]-mean)/sigma;
 
    mean = 0.89048*(((((1+(-0.417808*x))+(0.285339*pow(x,2)))+(-0.0797402*(pow(x,3))))+(0.0102105*(pow(x,4))))+(-0.00049191*(pow(x,5))));
    sigma = 0.464536*(((((1+(-0.56921*x))+(0.141747*pow(x,2)))+(-0.0176432*(pow(x,3))))+(0.000979462*(pow(x,4))))+(-1.3265e-05*(pow(x,5))));
    LayerS3FracNorm[12] = (LayerS3Frac[12]-mean)/sigma;
 
    mean = 0.914006*(((((1+(-0.505655*x))+(0.329083*pow(x,2)))+(-0.0891581*(pow(x,3))))+(0.0111151*(pow(x,4))))+(-0.000522036*(pow(x,5))));
    sigma = 0.561738*(((((1+(-0.521007*x))+(0.0973216*pow(x,2)))+(-0.0042303*(pow(x,3))))+(-0.000662862*(pow(x,4))))+(5.63393e-05*(pow(x,5))));
    LayerS3FracNorm[13] = (LayerS3Frac[13]-mean)/sigma;
 
    mean = 1.13152*(((((1+(-0.645211*x))+(0.370763*pow(x,2)))+(-0.0952597*(pow(x,3))))+(0.0115274*(pow(x,4))))+(-0.000531245*(pow(x,5))));
    sigma = 0.449899*(((((1+(-0.263936*x))+(-0.0807557*pow(x,2)))+(0.0442253*(pow(x,3))))+(-0.00659785*(pow(x,4))))+(0.000328546*(pow(x,5))));
    LayerS3FracNorm[14] = (LayerS3Frac[14]-mean)/sigma;
 
    mean = 1.22631*(((((1+(-0.728916*x))+(0.404153*pow(x,2)))+(-0.101055*(pow(x,3))))+(0.0119479*(pow(x,4))))+(-0.000539546*(pow(x,5))));
    sigma = 0.354251*(((((1+(-0.00640648*x))+(-0.227689*pow(x,2)))+(0.0758959*(pow(x,3))))+(-0.00957225*(pow(x,4))))+(0.000429167*(pow(x,5))));
    LayerS3FracNorm[15] = (LayerS3Frac[15]-mean)/sigma;
 
    mean = 1.17096*(((((1+(-0.753461*x))+(0.413966*pow(x,2)))+(-0.101175*(pow(x,3))))+(0.0116518*(pow(x,4))))+(-0.000512023*(pow(x,5))));
    sigma = -0.400448*(((((1+(-2.29336*x))+(1.24944*pow(x,2)))+(-0.2953*(pow(x,3))))+(0.0323842*(pow(x,4))))+(-0.00134959*(pow(x,5))));
    LayerS3FracNorm[16] = (LayerS3Frac[16]-mean)/sigma;
 
    mean = 0.972798*(((((1+(-0.549714*x))+(0.27782*pow(x,2)))+(-0.0619123*(pow(x,3))))+(0.00661875*(pow(x,4))))+(-0.000274428*(pow(x,5))));
    sigma = -0.354826*(((((1+(-1.9982*x))+(0.923192*pow(x,2)))+(-0.181456*(pow(x,3))))+(0.0161776*(pow(x,4))))+(-0.000531338*(pow(x,5))));
    LayerS3FracNorm[17] = (LayerS3Frac[17]-mean)/sigma;

    TMVA::Reader* useReader = smoothed ? readerSmoothed : reader;
    if(event%2 == 1)
        read_bdt = useReader->EvaluateMVA( "ODD" );
    else
        read_bdt = useReader->EvaluateMVA( "EVEN" );
    if(EneDep <= 0. || EneDepXY[0] <= 0. || EneDepXY[1] <= 0.) read_bdt = -0.999;

    if(read_bdt!=read_bdt) read_bdt=-999.;

    return read_bdt;
}

void DeadSideCorr::GetCorrShowerAxis(float corr_cog[3], float corr_dir[2])
{
    for(int k=0;k<3;k++)
    {
        if(CooNew[k]!=CooNew[k]) CooNew[k]=-999.;
    }
    for(int k=0;k<2;k++)
    {
        if(KCR[k]!=KCR[k]) KCR[k]=-999.;
    }

    for(int k=0;k<3;k++) corr_cog[k]=CooNew[k];
    for(int k=0;k<2;k++) corr_dir[k]=KCR[k];
}
