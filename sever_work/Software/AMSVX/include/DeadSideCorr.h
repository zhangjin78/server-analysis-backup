//  Usage:
//
//    BeforeLoop:
//      DeadSideCorr CorrShower;
//      //input
//      AMSEventR *pevt;
//      EcalShowerR *pshower;
//      //output
//      float CorrEnergy;
//      float CorrEcalBDT;
//      float CorrCOG[3]; //Ecal COG used Cell Ratio method. 0: X, 1: Y, 2: Z
//      float CorrDir[2]; //Shower axis direction used Cell Ratio method. 0: deltaX / deltaZ, 1: deltaY / deltaZ
//
//    EventLoop:
//      //Do correction.
//      CorrShower.ProcessCorr(pevt,pshower);
//
//      //Get results after correction.
//      CorrEnergy = CorrShower.GetCorrEnergy();
//      CorrEcalBDT = CorrShower.GetCorrEcalBDT();
//      CorrShower.GetCorrShowerAxis(CorrCOG,CorrDir);

#ifndef __DeadSideCorr__
#define __DeadSideCorr__
#include <iostream>
#include <cmath>
#include <TMath.h>
#include <TMinuit.h>
#include "TMVA/Tools.h"
#include "TMVA/Reader.h"
#include "TMVA/MethodCuts.h"
#include "root.h"
#include "DeadSideCorrDB.h"

#define Debug 0

using namespace std;
using namespace DeadSideCorrDB;

class DeadSideCorr: public TObject{
    public:
        int DataType;
        float CooNew[3];
        float KCR[2];
        float edepattcorr[DeadSideCorrDB::nLayer][DeadSideCorrDB::nCell];
        float elayer_corr[DeadSideCorrDB::nLayer];
        int status[DeadSideCorrDB::nLayer][DeadSideCorrDB::nCell];
        float ecalcog[3];
        float EdepRecorr[DeadSideCorrDB::nLayer][DeadSideCorrDB::nCell]; // Finnal Result E map after correction.
        float ElayerRecorr[DeadSideCorrDB::nLayer]; // Finnal Result Elayer after correction.
        float EnergyRecorr;
        float ElayerLongFit[DeadSideCorrDB::nLayer]; // Elayer getting from longitudinal fitting, used for deadcell correction in ExpCellE()
        int DeadStatus[18]; // use for show if or not there are dead cells in the layer
        float coocrlayer[DeadSideCorrDB::nLayer][2];
        int Numcoocrlayer[DeadSideCorrDB::nLayer];
        float coocrfiber[DeadSideCorrDB::nLayer];
        float coocrreadout[DeadSideCorrDB::nLayer];//wxq

        vector<int> DeadIDL;  // layer number, deadcells
        vector<int> DeadIDC;  // cell number,  deadcells
        int tmpdeadtotal;

        float LateralLeak[18][2][10];

        //GetCorrectedEnergy
        float efraclast2layers; // corrected for anode efficiency
        float depositedenergy; //corrected for andode efficiency
        float EnergyPIC;
        float RearLeakPI;
        float corr_factor;
        float correction_factor;

        float E3C0;
        float CorrEnergyD;

        //==========ECAL BDT==========//
        float ADCH[18][72];
        float MapEneDep[18][72];
        float LayerEneDep[18];       // Energy deposit [GeV] in each layer (sum of every cell of each layer)
        float EneDep;
        float EneDep_GeV;
        float EneDepXY[2]; // 0 = x, 1 = y
        float LayerMean[18]; // Mean [cell] for each layer: (sum_j j*MapEneDep[i][j])/(sum_ij MapEneDep[i][j])
        float LayerSigma[18];
        float LayerSigmaNorm[18];
        float LayerEneFrac[18];
        float LayerEneFracNorm[18];
        float LayerS1S3[18];
        float LayerS3Frac[18];
        float LayerS3FracNorm[18];
        float ShowerMean;
        float ShowerMeanNorm;
        float L2LFrac;
        float L2LFracNorm;
        float S3totx;
        float S3toty;
        float S3totxNorm;
        float S3totyNorm;
        float R3cmFrac;
        float R3cmFracNorm;
        float ShowerFootprintX;
        float ShowerFootprintY;
        float ShowerFootprintXNorm;
        float ShowerFootprintYNorm;
        float NEcalHits;
        float NEcalHitsNorm;
        float ShowerMeanXY[2]; // 0 = x, 1 = y
        float sigmaXY[2];  // 0 = x, 1 = y
        float sigmaXYZ[2]; // 0 = x, 1 = y
        float sigmaZ[2];   // 0 = x, 1 = y
        float F2SLEneDep;
        int imaxcell[18];
        unsigned int run;
        unsigned int event;
        float read_bdt;
        TMVA::Reader *reader;
        TMVA::Reader *readerSmoothed;

        //=========LongFit==========//
        static TMinuit* minuit1;
        float longchi2;
        float ecclEdep[18];

        // nz changes
        float S13LeakXPI;
        float S13LeakYPI;

    public:
        DeadSideCorr();
        ~DeadSideCorr();

        void ReSet();
        float ReCalEnergyE();
        float ReCalEnergyD();
        void ReCalCOGZ();
        int ReFitDirCR(int deadcelltag);
        int ReCooLayer();
        float RePMTLayerCorr(float Elayer, int ilayer);
        int RePMTCorr();

        int ReLongFit();
        double ExpCellE_CalEffi(int il,int ic);
        double ExpCellE(int il, int ic);
        float ReCalEnergy(Float_t elayer_corr_get[18]);
        int StatusCheck();
        int ReDeadCorr();
        float * DeadSideCorrection(float ecal_map[18][72], int Status[18][72], float EcalCOG[3], int datatype);

        //====== Used in LongFit ===
        Double_t func(Double_t *par, Double_t x);
        Double_t integerLong(Double_t a,Double_t b,Double_t *par,Double_t epsilon=0.000001);
        Double_t GetLongChi2(Double_t *par);
        static void ecclFCN(Int_t &npar, Double_t *gin, Double_t &f, Double_t *par, Int_t iflag);

        //========== Interface ==========//
        void ProcessCorr(AMSEventR* evt, EcalShowerR *pecal);
        float GetCorrEnergy();
        float GetCorrEcalBDT(bool smoothed);
        void GetCorrShowerAxis(float corr_cog[3], float corr_dir[2]);
};
#endif
