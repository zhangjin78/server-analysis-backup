///  This is an example to use TofH(BetaH) software Select_Tof()
/*!
  BetaHR and TofClusterHR can be accessed from AMSEventR, ParticleR or ChargeR  \n
  BetaH Version Software Based on IHEP version Calibration and Reconstruction \n
*/
// -----------------------------------------------------------
//        Created:       2012-Aug-17  Q.Yan  qyan@cern.ch
// -----------------------------------------------------------

int Select_Tof(){

   int          tof_barid[4]={0};
   unsigned int tof_bstatus[4][2]={{0}};
   float        tof_adcdr[4][2][3]={{{0}}};
   float        tof_betah=0;
   int          tof_nhith=0;
   int          tof_hsumh=0;
   int          tof_hsumhu=0;
   int          tof_nhithl[4]={0};

//---BetaH Charge
   float        tof_hql[4]={0};
   float        tof_hqc[4]={0};
   int          tof_hz=0;
   float        tof_hprobz=0;
   int          tof_hzu=0;//Up Tof Z: TofChargeHR Support Dynamic Likelihood ReFit For select Pattern
   int          tof_hzd=0;//Dow Tof Z
   float        tof_hq=0;
//--Time
   float        tof_htl[4]={0};
   float        tof_htc[4]={0};
   float        tof_temp[4][2]={{0}};
   float        tof_tempC[4][2]={{0}};
   float        tof_tempP[4][2]={{0}};
   float        tof_chist=0;
   float        tof_chisc=0;

//---
     TofRecH::ReBuild();
     tof_nhith=pev->nTofClusterH();//Sum of TofClusterHR (Tof-Counter Number Fired)
     BetaHR *betah=pev->pParticle(iparindex)->pBetaH();


//     if(!betah)return false;
     if(betah&&betah->IsTkTofMatch()){//Tk Matched with TOF
       float frig=(betah->pTrTrack())?betah->pTrTrack()->GetRigidity(0):0;
       tof_betah=betah->GetBeta();//Beta Measurement
       tof_chist=betah->GetChi2T();///Time Chis
       tof_chisc=betah->GetChi2C();// Space Chis
       tof_hsumh=betah->GetSumHit();// SumHit Match by BetaH
       tof_hsumhu=betah->GetUseHit();//Use TOF-Layer For Beta-Fit

//--TOFQ
       int nlay; float qrms;
//----New charge rigidity corrretion(recommended!!!, especially good for Ion and high rigidity)
       tof_hqt=betah->GetQ(nlay,qrms,2,TofClusterHR::DefaultQOptIonW,-2,0,frig);//Trancate Mean Q => MaxSPan Rigidity
       tof_hqg=betah->GetQ(nlay,qrms,2,TofClusterHR::DefaultQOptIonW,-1,0,frig);//Gaus Mean Q
//----Access To TofChargeHR
       TofChargeHR *tofcharge=betah->pTofChargeH();
//----New charge rigidity corrretion(recommended!!!, especially good for Ion and high rigidity)
       tofcharge->ReFit(0,TofChargeHR::DefaultQOptIonW,frig);
       tof_hq=tofcharge->GetQ(nlay,qrms);//Q From TofChargeHR better than BetaH For Z~3-8, Due to better tuning)
       tof_hz=tofcharge->GetZ(nlay,tof_hprobz);//Max-Prob Integer Z+ProbZ
       float probu,probd;
       tof_hzu=tofcharge->GetZ(nlay,probu,0,1100); //Using Up TOF-Two-Layer Likelihood To PID ///
       tof_hzd=tofcharge->GetZ(nlay,probd,0,11); //Using Down TOF-Two-Layer Likelihood To PID

//Layer
       for(int ilay=0;ilay<4;ilay++){
          tof_nhithl[ilay]=betah->GetAllFireHL(ilay);//Number of Bar of ilay-TOF fired
//--Test if This Layer Exist-By BetaH
         if(!betah->TestExistHL(ilay)){
           tof_hql[ilay]=tofcharge->GetQL(ilay);//Q-Measurment for Each TOF-Layer
           if(betah->IsBetaUseHL(ilay))tof_htl[ilay]=betah->GetTime(ilay);//Time-Measurment for Each TOF-Layer
//---TofClusterHR
           TofClusterHR *tofclh=betah->GetClusterHL(ilay);//Access to TofClusterHR
           tof_barid[ilay]=tofclh->Bar;//TOF-Barid
           tof_hqc[ilay]=  tofclh->GetQSignal();//Q Measurement From TofClusterHR(From BetaH is much better)
           if(tofclh->IsGoodTime())tof_htc[ilay]=tofclh->Time;//Time-Measurment From TofClusterHR
           for(int is=0;is<2;is++){
             tof_bstatus[ilay][is]=tofclh->SideBitPat[is];
             for(int ipm=0;ipm<3;ipm++)tof_adcdr[ilay][is][ipm]=tofclh->Dadc[is][ipm];
//--TofRawSideHR
             if(betah->GetClusterHL(ilay)->TestExistHS(is)){
                tof_temp[ilay][is]= tofclh->GetRawSideHS(is)->temp;//Assess to TofRawSideR
                tof_tempC[ilay][is]=tofclh->GetRawSideHS(is)->tempC;
                tof_tempP[ilay][is]=tofclh->GetRawSideHS(is)->tempP;
              }
           }
        }
//--Test
      }//End Layer

//---
   }
 
   return 0;
}

