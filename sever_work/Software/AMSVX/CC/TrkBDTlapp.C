/*#include "root_RVSP.h"
#include "amschain.h"
#include "TrdKCluster.h"
#include "Rtypes.h"
#include "EcalChi2CY.h"
#include "TofTrack.h"
#include "Tofrec02_ihep.h"
#include "TrFit.h"
#include "TrTrackSelection.h"
#include "TrExtAlignDB.h"
#include "TrRecon.h"
#include "TTree.h"
#include "TFile.h"
#include "TString.h"
#include "TMVA/Reader.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
*/
#include "Tofrec02_ihep.h"
#include "TrFit.h"
#include "TrTrackSelection.h"
#include "TrExtAlignDB.h"
#include "TrRecon.h"
#include "TTree.h"
#include "TFile.h"
#include "TString.h"
#include "TMVA/Reader.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include "TrkBDTlapp.h"
#include "commonsi.h"
TMVA::Reader *TrkBDTreaderlapp[5];
UInt_t noreFit[8] = {20, 1, 1, 0, 21, 20, 20, 20};
void TrkBDTlapp::_BubbleSort(int n, float arr_input[], float arr_output[]){
	// 1. clone
	for(int i=0; i<n; i++) arr_output[i] = arr_input[i];
	// 2. sort
	int i = n, j;
	float temp;
	while( i > 0 ){
		for(j=0; j<i-1; j++){
			if( arr_output[j] > arr_output[j+1] ){
				temp = arr_output[j];
				arr_output[j] = arr_output[j+1];
				arr_output[j+1] = temp;
			}
		}
		i--;
	}
}

float TrkBDTlapp::GetTrkBDTel(){
	AMSEventR *ev = AMSEventR::Head();
	return GetTrkBDTel(ev);
}

float TrkBDTlapp::GetTrkBDTel(AMSEventR *ev,  int itr, float betah_tof){
	//initialization of variables
	npart=0;
	enE=0.;
	trk_rarcm=10.;
	trk_rirm=10.;
	trk_r1r9=10.;
	log10chi2y=1000.;
	log10trkecal_dy=1000.;
	TrkTrdMatch=-1.;
	TrkTrdMatchy=-1;
	trk_q9=1.;
	trk_qasym=1.;
	trk_qasym2=1.;
	trk_q78=1.;
	for (int i=0; i<3 ;i++){LikelihoodRatioD[i]=-1.;}
	memset(NHitsTrdlh, 0, sizeof(Int_t)*3);
	memset(nhitlay_offtrack, 0, sizeof(Int_t)*9);
	memset(nhitlay_neighbor, 0, sizeof(Int_t)*9);
	memset(nsumhit_radius,0,sizeof(Int_t)*5);
	memset(qlay,0,sizeof(Float_t)*9);
	cyc_rad[0] = 0.2;
	cyc_rad[1] = 0.5;
	cyc_rad[2] = 1.0;
	cyc_rad[3] = 2.0;
	cyc_rad[4] = 5.0;
	Track_Fitcode_Inn=-1;
	Track_Fitcode_Al=-1;
	Track_Fitcode_InnL1=-1;
	Track_Fitcode_InnL9=-1;
	Track_Fitcode_upper=-1;
	Track_Fitcode_lower=-1;
	rig_a=10.;
	rig_i=10.;
	rig_il1=10.;
	rig_il9=10.;
	rig_l1l9=10.;

	trk_upper=1.;
	trk_lower=1.;
	trk_q7=1.;
	trk_q8=1.;

	trk_rarcm=10.;
	trk_rirm=10.;
	trk_r1r9=10.;
	trk_nnoisehits=9;
	iparindex=-1;

	deltaX= 0;
	deltaY= 0;
	delta = 0;
	threshold = 15;
	trtrackerr = -1.;
	memset(trklayused, 0, sizeof(trklayused));
	//get variables
	npart=ev->nParticle();
	if (npart<1) return 2;
	int iecal=-1;
	int ns=ev->nEcalShower();
	float end_tmp=0.;
	for(int ico=0;ico<ns;ico++){
		if(ev->EcalShower(ico).EnergyD > end_tmp) {iecal=ico;end_tmp=ev->EcalShower(ico).EnergyD;}
	}
	if (iecal<0) return -2;
	ecalsh = ev->pEcalShower(iecal);
	enE = ecalsh->EnergyE;
	for(int i=0;i<3;i++){
		ecalEntry[i]=ecalsh->Entry[i];
		ecalExit[i]=ecalsh->Exit[i];
		if(ecalsh->Entry[2]<-150){ecalEntry[i]=ecalsh->Exit[i];ecalExit[i]=ecalsh->Entry[i];}
	}
	float Ecal_LayerZ=-143.2;
	if (itr<0){
		itr=0;
		int itrack=0;
		for(int ipart=0;ipart<npart;ipart++){
			itrack=ev->pParticle(ipart)->iTrTrack();
			if (itrack<0) continue;
			track = ev->pTrTrack(itrack);
			Track_Fitcode_Max=track->iTrTrackPar(1,0,reFit[0]);//Choutko,Maxspan,NoRefit
			pntL.setp(0,0,0);
			track->Interpolate(Ecal_LayerZ, pntL, dirL, Track_Fitcode_Max);
			if(ipart==0){
				itr=itrack;
				deltaX= pntL[0]-ecalEntry[0];
				deltaY= pntL[1]-ecalEntry[1];
				delta = pow(deltaX,2) + pow(deltaY,2);
			}
			if(ipart>0 && pow(pntL[0]-ecalEntry[0],2)+pow(pntL[1]-ecalEntry[1],2)<delta){
				itr=itrack;
				deltaX= pntL[0]-ecalEntry[0];
				deltaY= pntL[1]-ecalEntry[1];
				delta = pow(deltaX,2) + pow(deltaY,2);
			}
		}//look for the track with minimum deltaXY
	}
	track=ev->pTrTrack(itr);
	for(int ipart=0;ipart<npart;ipart++){
		if(ev->pParticle(ipart)->iTrTrack()==itr){
			iparindex=ipart;
			break;
		}
	}
	int ifit = 8;
	if ( (ev->Version()) >= 950 && (ev->nMCEventg()) == 0 ) while(ifit-->0) reFit[ifit] = noreFit[ifit];
	else  while(ifit-->0) reFit[ifit] = 23;
	Track_Fitcode_Max=track->iTrTrackPar(1,0,reFit[0]); // fit code for TRD  && MaxSpan
	if (Track_Fitcode_Max<0) {Track_Fitcode_Max=track->iTrTrackPar(1,0,0);}
	if (Track_Fitcode_Max<0) return -2;
	trtrackerr=track->GetErrRinv(Track_Fitcode_Max);
	chi2x=track->GetNormChisqX(Track_Fitcode_Max);
	chi2y=track->GetNormChisqY(Track_Fitcode_Max);
	rig=track->GetRigidity(Track_Fitcode_Max);
 for (int j=0;j<9;j++){
                if (track->TestHitBitsJ(j+1,Track_Fitcode_Max)) trklayused[j]=1;
        }
	if (trklayused[0]&&trklayused[8]){
		Track_Fitcode_full=track->iTrTrackPar(1,7,reFit[7]);//Choutko,Fullspan,NoRefit
		rig_l1l9=track->GetRigidity(Track_Fitcode_full);//Choutko,layer1+layer9,NoRefit
	}
	Track_Fitcode_Inn=track->iTrTrackPar(1,3,reFit[3]);//Choutko,Inner,Refit
	rig_i=track->GetRigidity(Track_Fitcode_Inn);//Choutko,Inner,NoRefit
	Track_Fitcode_Al=track->iTrTrackPar(2,0,reFit[0]);//Alcaraz,Maxspan,Refit
	rig_a=track->GetRigidity(Track_Fitcode_Al);//Alcaraz,Maxspan,NoRefit
	Track_Fitcode_upper=track->iTrTrackPar(1,1,reFit[1]);//Choutko,upper,NoRefit
	Track_Fitcode_lower=track->iTrTrackPar(1,2,reFit[2]);//Choutko,lower,NoRefit
	if (trklayused[0]){
		Track_Fitcode_InnL1=track->iTrTrackPar(1,5,reFit[5]);//Choutko,Inner+layer1,NoRefit
		rig_il1=track->GetRigidity(Track_Fitcode_InnL1);//Choutko,Inner+layer1,NoRefit
	}
	if (trklayused[8]){
		Track_Fitcode_InnL9=track->iTrTrackPar(1,6,reFit[6]);//Choutko,Inner+layer9,NoRefit
		rig_il9=track->GetRigidity(Track_Fitcode_InnL9);//Choutko,Inner+layer9,NoRefit
	}
	//noise hits
	for(int ihit=0; ihit<track->GetNhits(); ihit++){
		TrRecHitR *phit = track -> pTrRecHit(ihit);
		int layJ= phit -> GetLayerJ();
		int idhit = track->iTrRecHit(ihit); // index in hit vector

		for(int jhit=0; jhit<ev->nTrRecHit(); jhit++){
			TrRecHitR *phit_offtrack = ev->pTrRecHit(jhit);
			// NOT the hit used in track
			if( phit_offtrack->GetLayerJ()==layJ&& phit_offtrack->iTrCluster('y') != phit->iTrCluster('y')&& idhit != jhit
			  ){
				float distx = phit_offtrack->HitDist( *phit, 0 );
				float disty = phit_offtrack->HitDist( *phit, 1 );

				nhitlay_offtrack[layJ-1]++;
				if( fabs(disty) < 1 && fabs(distx) < 1 ){
					nhitlay_neighbor[layJ-1]++;
				}

				//
				for(int ir=0; ir<5; ir++){
					if( fabs(disty) < cyc_rad[ir] ){
						nsumhit_radius[ir] ++;
					}
				}
				//
			}
		}
	}

	mindisty_lay[1]= fabs(TrTrackSelection::GetMinDist(track,1).y());
	mindisty_lay[2]= fabs(TrTrackSelection::GetMinDist(track,10).y());
	mindisty_lay[3]= fabs(TrTrackSelection::GetMinDist(track,100).y());
	mindisty_lay[4]= fabs(TrTrackSelection::GetMinDist(track,1000).y());
	mindisty_lay[5]= fabs(TrTrackSelection::GetMinDist(track,10000).y());
	mindisty_lay[6]= fabs(TrTrackSelection::GetMinDist(track,100000).y());
	mindisty_lay[7]= fabs(TrTrackSelection::GetMinDist(track,1000000).y());
	mindisty_lay[0]= fabs(TrTrackSelection::GetMinDist(track,10000000).y());
	mindisty_lay[8]= fabs(TrTrackSelection::GetMinDist(track,100000000).y());
	for(int i=0; i<9; i++){
		if( mindisty_lay[i] < 1e-3 ) {mindisty_lay[i] = 10.;trk_nnoisehits--;}
	}

	// sort by distance
	_BubbleSort(9, mindisty_lay, mindisty_sort);
	for(int i=0; i<9; i++){
		log10mindisty_sort[i] = log10(mindisty_sort[i]);			        
	}
	//weiwei's BDT
	if (rig!=0&&rig_a!=0) trk_rarcm=rig_a/rig-1;
	if (rig!=0&&rig_i!=0) trk_rirm=rig_i/rig-1;
	if (rig_il9!=0&&rig_il1!=0) trk_r1r9=rig_il1/rig_il9-1;
	log10chi2y=log10(chi2y);
	if (deltaY!=0) log10trkecal_dy=log10(abs(deltaY));
	//ToF
	if (!ev->nBetaH()) TofRecH::ReBuild();
	int ibetah=-1;
	for (int i =0 ; i< ev->nBetaH();i++){
		if(
				( iecal >= 0 && ev->pBetaH(i)->iEcalShower() == iecal      )
				|| ( itr  >= 0 && ev->pBetaH(i)->iTrTrack()    == itr       )
		  ){
			ibetah = i;
			break;
		}
	}
	jf_bh=ev->pBetaH(ibetah);
	if(!jf_bh || !jf_bh->IsTkTofMatch()) return -2;
	if (betah_tof < 0.01) {
		betah_tof=jf_bh->GetBeta();
	}
	if(betah_tof<=0.6) return -2; // remove non relativistic particles
	TofChargeHR *tofcharge=jf_bh->pTofChargeH();
	q_tofd=tofcharge->GetQ(nlay,qrms,11);//Using Down TOF-Two-Layer Likelihood To PID
	//TRK-TRD matching
	trk_q9=track->GetLayerJQ(9,1, -1, 0,2);

	trk_upper=track->GetQ(1, Track_Fitcode_upper, 0, 2);
	trk_lower=track->GetQ(1, Track_Fitcode_lower, 0, 2);
	trk_qasym2=(trk_upper-trk_lower)/(trk_upper+trk_lower);
	int nup=0,nlow=0;
	qinup = qinlow =0 ;
	for(Int_t il=0; il<9; il++){
		qlay[il] = track->GetLayerJQ(il+1, betah_tof);
		if( qlay[il]>0 && il>0 && il<4 ){ //L1-L3
			nup++;
			qinup += qlay[il];
		}
		else if( qlay[il]>0 && il>=4 && il < 8 ){ //L4-L7
			nlow++;
			qinlow += qlay[il];
		}
	}
	qinup = nup>0?qinup/nup:0;
	qinlow = nlow>0?qinlow/nlow:0;
	if (qinlow+qinup==0)return -2;
	trk_qasym = (qinlow-qinup)/(qinlow+qinup);

	trk_q7=track->GetLayerJQ(7,1,-1, 0,  2);
	trk_q8=track->GetLayerJQ(8, 1, -1,  0, 2);
	if (trk_q7!=0&&trk_q8!=0)trk_q78=0.5*(trk_q7+trk_q8);
	if (trk_q7!=0&&trk_q8==0) trk_q78=trk_q7;
	if (trk_q7==0&&trk_q8!=0) trk_q78=trk_q8;
	//ACC
	nACC=ev->nAntiCluster();
	for(int itrd=0; itrd < ev->nTrdTrack();itrd++){
		ptrdtrack = ev->pTrdTrack(itrd);
		//Do refit first
		_trdcluster=TrdKCluster(ev, ptrdtrack, enE);
		if (enE>0){
			Double_t LL[3]={0,0,0};
			_trdcluster.GetLikelihoodRatio_TRDRefit(threshold,tmp_lkhd,NHitsTrdlh[2],enE, LL,1, 0);
		}
		if (track && Track_Fitcode_Max >0){
			Double_t LL[3]={0,0,0};
			_trdcluster.GetLikelihoodRatio_TRDRefit(threshold,tmp_lkhd,NHitsTrdlh[2],rig, LL,1, 1);
		}
		if (NHitsTrdlh[2]<8) continue;
		//Inner Tracker track fit 
		_trdcluster.SetTrTrack(track,Track_Fitcode_Inn); 
		int IsReadAlignmentOK=_trdcluster.IsReadAlignmentOK;
		int IsReadCalibOK=_trdcluster.IsReadCalibOK;
		if(IsReadAlignmentOK>0 && IsReadCalibOK>0){
			_trdcluster.GetLikelihoodRatio_TrTrack(threshold,LikelihoodRatioD,NHitsTrdlh[0]);
		}
	}
	//Find the pattern
	int pattN = 0;
	if( trklayused[0] && trklayused[8] ) pattN = 0; // InnL1L9
	else if( trklayused[0]==0 && trklayused[1] && trklayused[8] ) pattN = 1; // L2L9
	else if( trklayused[0]==0 && trklayused[1]==0 && trklayused[8] ) pattN = 2; // L9
	else if( trklayused[0] && trklayused[8]==0 ) pattN = 3; // L1(L2)
	else pattN = 4; // use InnL2 && InnerOnly as default

	//calculate TrkBDTlapp
	static Float_t trk_r9rf,trk_r9ri,trackerr_rig,nsumhit_radiusf, trk_nnoisehitsf,n_ACC,nhitstrd;

	//normalization of the variables.
	if (pattN==0||pattN==1||pattN==2)//L1L9 & L2L9 & L9
		trk_q9 = -(0.91+0.16*log10(enE))+trk_q9;
	trk_qasym = 0.0086+trk_qasym;
	trk_q78 = -(0.96+0.078*log10(enE))+trk_q78;
	if (pattN!=3&&pattN!=4)//not L1L2 or InnerL2
		log10mindisty_sort[0] = -(-0.32-0.13*log10(enE))+log10mindisty_sort[0];
	if (pattN==0||pattN==1||pattN==2)//L1L9 & L2L9 & L9
		q_tofd = -(1.23+0.005*exp(1.36*log10(enE)))+q_tofd;
	if (pattN==3||pattN==4)//L1L2 && InnerL2
		q_tofd = -(0.84+0.23*log10(enE))+q_tofd;

	if (pattN==0)//L1L9
		trk_r9rf = (rig_il9/rig_l1l9-1)/(0.00416*pow(enE,(float)0.756));
	if (pattN==1)//L2L9
		trk_r9ri = (rig_il9/rig_i-1)/(0.0830*pow(enE,(float)0.4));
	if (pattN==0||pattN==1||pattN==2)//L1L9 & L2L9 & L9
		trackerr_rig=(log10(trtrackerr*abs(rig))+1.05-0.0030*pow(enE,(float)0.76))/(0.294+0.0098*pow(enE,(float)0.91));
	if (pattN==3)//L1L2
		trackerr_rig=(log10(trtrackerr*abs(rig))+1.76-0.505*log10(enE))/(0.028*exp(1.11*log10(enE)));
	if (pattN==4)//InnerL2
		trackerr_rig=(log10(trtrackerr*abs(rig))+2.57-1.39*log10(enE)+0.204*log10(enE)*log10(enE))/(0.201*exp(0.344*log10(enE)));
	if (pattN==0||pattN==1||pattN==2)//L1L9 & L2L9 & L9
		nsumhit_radiusf = -(1.1+0.56*log10(enE))+nsumhit_radius[0];
	if (pattN!=2&&pattN!=4)//not L9
		trk_nnoisehitsf = -(1.6+0.3*log10(enE))+trk_nnoisehits;
	if (pattN==0)//L1L9
		n_ACC = -(-1.32+0.461*exp(log10(enE)*0.826))+nACC;
	if (pattN==1||pattN==2)//L2L9
		n_ACC = -0.0994*pow(enE,(float)0.554)+nACC;
	if (pattN==3&&pattN==4)//L1L2&&InnerL2
		n_ACC = -0.0861*exp(log10(enE)+1.31)+nACC;
	nhitstrd = NHitsTrdlh[0]-NHitsTrdlh[2];

	//Creating reader if needed
	pattName[0]="f";
	pattName[1]="l2l9";
	pattName[2]="ol9";
	pattName[3]="l1l2";
	pattName[4]="il2";

	for (int i=0;i<npatt;i++){
		if (!TrkBDTreaderlapp[i]){
			TrkBDTreaderlapp[i] = new TMVA::Reader("!Color:!Silent");
			TrkBDTreaderlapp[i]->AddVariable( "trk_rarcm", &trk_rarcm );
			if (i==0){
				TrkBDTreaderlapp[i]->AddVariable( "trk_r1r9", &trk_r1r9 );
			}
			if (i==2||i==3){//L9
				TrkBDTreaderlapp[i]->AddVariable( "trk_rirm", &trk_rirm );
			}
			TrkBDTreaderlapp[i]->AddVariable( "log10chi2y", &log10chi2y );

			TrkBDTreaderlapp[i]->AddVariable( "log10trkecal_dy", &log10trkecal_dy );
			if (i==0||i==1||i==2){//L1L9 & L2L9 & L9
				TrkBDTreaderlapp[i]->AddVariable( "trk_q9", &trk_q9 );
			}
			TrkBDTreaderlapp[i]->AddVariable( "trk_qasym", &trk_qasym );
			TrkBDTreaderlapp[i]->AddVariable( "trk_q78", &trk_q78 );
			TrkBDTreaderlapp[i]->AddVariable( "log10mindisty_sort[0]", &log10mindisty_sort[0] );
			TrkBDTreaderlapp[i]->AddVariable( "q_tofd", &q_tofd );
			if (i==0){
				TrkBDTreaderlapp[i]->AddVariable( "rig_il9/rig_l1l9-1", &trk_r9rf );
			}
			if (i==1||i==2){//L2L9&L9
				TrkBDTreaderlapp[i]->AddVariable( "rig_il9/rig_i-1", &trk_r9ri );
			}
			TrkBDTreaderlapp[i]->AddVariable( "log10(trtrackerr*abs(rig))", &trackerr_rig );
			TrkBDTreaderlapp[i]->AddVariable( "nsumhit_radius[0]", &nsumhit_radiusf );
			if (i==0||i==1||i==3){// L1L9 & L2L9 & L1L2 &L2
				TrkBDTreaderlapp[i]->AddVariable( "trk_nnoisehits", &trk_nnoisehitsf );
			}
			TrkBDTreaderlapp[i]->AddVariable( "nACC", &n_ACC );
			if (i==3){//L1L2
				TrkBDTreaderlapp[i]->AddVariable( "NHitsTrdlh[0]-NHitsTrdlh[2]", &nhitstrd );
			}
			TrkBDTreaderlapp[i]->BookMVA( "BDTG method",  Form("%s/%s/LAPP/TrkBDTweights/%stmva_sample_BDTG.weights.xml",getenv("AMSDataDir"),AMSCommonsI::getversion(),pattName[i].Data()));//This is the weights file located at CERN
		}
	}

	Float_t TrkBDTG;

	double TrkBDTD = TrkBDTreaderlapp[pattN]->EvaluateMVA( "BDTG method" );
	TrkBDTG = TrkBDTD;
	return TrkBDTG;

}
