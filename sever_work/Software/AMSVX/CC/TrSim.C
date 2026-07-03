#include "TrSim.h"
#include "tkdcards.h"
#include "random.h"
#ifndef __ROOTSHAREDLIBRARY__
#include "geant321.h"
#include "amsgobj.h"
#include "trrec.h"
#else
#include "TrRawCluster.h"
#include "TrMCCluster.h"
#include "TrCluster.h"
#include "TrRecHit.h"
#endif

extern "C" double rnormx();

AMSPoint TrSim::sitkrefp[trconst::maxlay];
AMSPoint TrSim::sitkangl[trconst::maxlay];


TrSimSensor TrSim::_sensors[3];
bool TrSim::isbuilt = false; 


void TrSim::sitkhits(int idsoft, float vect[], float edep, float step, int itra,  int gtrkid, int status) {

#ifndef __ROOTSHAREDLIBRARY__
  AMSgObj::BookTimer.start("SiTkSimuAll"); // simulation counter
#endif

  // XXPLSS | XX=sensor| P 0 neg 1 pos | L=layer | SS=slot |
  // int sensor = abs(idsoft)/10000;
  int tkid   = abs(idsoft)%1000;
  int ss     = abs(idsoft)%10000-tkid;
  if (!ss) tkid*=-1;
  TkLadder* lad=TkDBc::Head->FindTkId(tkid);
  // Skip not existing ladders. This should never happen.
  if(!lad) { 
    printf("TrSim::sitkhits-E tkid %d unknown. This shold never happen. Skip this step.\n",tkid); 
    return;
  }
  // Skip not connected ladders 
  if(lad->GetHwId()<0) {
    //  printf("TrSim::sitkhits: tkid %d NO HWID !!!\n",tkid);
    return;
  }

  // If fast simulation make the TrRawCluster directly with gencluster() 
  if (TRMCFFKEY.SimulationType==kNoRawSim) {
    gencluster(idsoft, vect, edep, step, itra);
#ifndef __ROOTSHAREDLIBRARY__
    AMSgObj::BookTimer.stop("SiTkSimuAll");
#endif
    return;
  }

  // calculations
  // if (edep<=0) edep = 1e-9;
  AMSPoint ppa (vect[0],vect[1],vect[2]);
  AMSPoint dirg(vect[3],vect[4],vect[5]);
  float momentum = vect[6];
  AMSPoint pgl = ppa-dirg*step/2;

  /*
  // AMSPoint pmom(vect[3]*vect[6], vect[4]*vect[6], vect[5]*vect[6]);
  // AMSPoint ppb = ppa-dirg*step;
  // AMSPoint size(TkDBc::Head->_ssize_active[0]/2,TkDBc::Head->_ssize_active[1]/2,TkDBc::Head->_silicon_z/2);
  
  // Convert global coo into sensor local coo
  // The origin is the first strip of the sensor
  TkSens tksa(tkid, ppa,1);
  TkSens tksb(tkid, ppb,1);
  AMSPoint pa = tksa.GetSensCoo(); pa[2] += size[2];
  AMSPoint pb = tksb.GetSensCoo(); pb[2] += size[2];
  */

  // Set reference points and angles
  double thx = TMath::Abs(TMath::ATan(dirg.x()/dirg.z())*TMath::RadToDeg());
  double thy = TMath::Abs(TMath::ATan(dirg.y()/dirg.z())*TMath::RadToDeg());
  int ily = abs(tkid)/100-1;
  sitkrefp[ily] = pgl;
  sitkangl[ily] = AMSPoint(thx, thy, 0);

  /*
  // Range check
  for (int i = 0; i < 3; i++) {
    if (pa[i] < 0) pa[i] = 0;
    if (pb[i] < 0) pb[i] = 0;
    if (pa[i] > 2*size[i]) pa[i] = 2*size[i];
    if (pb[i] > 2*size[i]) pb[i] = 2*size[i];
  }
  */ 

  // Create a new object
  VCon* aa = GetVCon()->GetCont("AMSTrMCCluster");
  if (aa!=0)
#ifndef __ROOTSHAREDLIBRARY__
    aa->addnext(new AMSTrMCCluster(idsoft,step,pgl,dirg,momentum,edep,itra,gtrkid,status));
#else
    aa->addnext(new TrMCClusterR(idsoft,step,pgl,dirg,momentum,edep,itra,gtrkid,status));
#endif
  if (aa!=0) delete aa;
#ifndef __ROOTSHAREDLIBRARY__
  AMSgObj::BookTimer.stop("SiTkSimuAll");
#endif
}



void TrSim::gencluster(int idsoft, float vect[], float edep, float step, int itra) {
  if (edep <= 0) edep = 1e-4;
  AMSPoint ppa (vect[0],vect[1],vect[2]);
  AMSPoint dirg(vect[3],vect[4],vect[5]);
  AMSPoint pmom(vect[3]*vect[6], vect[4]*vect[6], vect[5]*vect[6]);
  AMSPoint ppb = ppa-dirg*step;
  AMSPoint pgl = ppa-dirg*step/2;
  AMSPoint size(TkDBc::Head->_ssize_active[0]/2,
                TkDBc::Head->_ssize_active[1]/2,
                TkDBc::Head->_silicon_z/2);
  //  XXPLSS  |XX=sensor|P 0 neg 1 pos |L=layer |SS=slot|
  int tkid   = abs(idsoft)%1000;
  int ss     = abs(idsoft)%10000-tkid;
  if(!ss) tkid*=-1;
  // Convert global coo into sensor local coo
  // The origin is the first strip of the sensor
  TkSens tksa(tkid, ppa,1);
  TkSens tksb(tkid, ppb,1);
  AMSPoint pa = tksa.GetSensCoo(); pa[2] += size[2];
  AMSPoint pb = tksb.GetSensCoo(); pb[2] += size[2];
  int ily = abs(tkid)/100-1;
  sitkrefp[ily] = pgl;
  TkSens tksc(tkid, pgl,1);
  AMSPoint pc = tksc.GetSensCoo();
  if (pc.x() < 0 || pc.x() > TkDBc::Head->_ssize_active[0]) return;
  VCon* cont=GetVCon()->GetCont("AMSTrCluster");
  int stx = tksc.GetStripX();
  int sty = tksc.GetStripY();
  double ipx = tksc.GetImpactPointX();
  double ipy = tksc.GetImpactPointY();
  int seedx = (ipx > 0) ? 0 : 1;
  int seedy = (ipy > 0) ? 0 : 1;
  if (ipx < 0) stx--;
  if (ipy < 0) sty--;
  double sig0  = 30;
  double sigx  = 5;
  double sigy  = 4;
  int sig_mode = 1;
  double thx = TMath::Abs(TMath::ATan(dirg.x()/dirg.z())*TMath::RadToDeg());
  double thy = TMath::Abs(TMath::ATan(dirg.y()/dirg.z())*TMath::RadToDeg());
  if (sig_mode == 1) {
    sigx = (40-1.6*thx+0.05*thx*thx)/5;
    sigy = (10+0.5*thy)/2.5;
  }
  sitkangl[ily] = AMSPoint(thx, thy, 0);

  double a1x = (ipx > 0) ? sig0*(1-ipx) : -sig0*ipx;
  double a2x = (ipx < 0) ? sig0*(1+ipx) :  sig0*ipx;
  double a1y = (ipy > 0) ? sig0*(1-ipy) : -sig0*ipy;
  double a2y = (ipy < 0) ? sig0*(1+ipy) :  sig0*ipy;

  a1x += rnormx()*sigx; a2x += rnormx()*sigx;
  a1y += rnormx()*sigy; a2y += rnormx()*sigy;
  if (a1x < 0) a1x = 0;
  if (a2x < 0) a2x = 0;
  if (a1y < 0) a1y = 0;
  if (a2y < 0) a2y = 0;
  double asx = a1x+a2x;
  double asy = a1y+a2y;

  if (seedx == 0 && a1x < a2x) seedx = 1;
  if (seedx == 1 && a1x > a2x) seedx = 0;
  if (seedy == 0 && a1y < a2y) seedy = 1;
  if (seedy == 1 && a1y > a2y) seedy = 0;

  float adc[2] = { 0, 0 };

  float dummy = 0;
  float rndx  = (float)RNDM((int)dummy);
  float rndy  = (float)RNDM((int)dummy);
  int  layer = abs(tkid)/100;
  float effx = (layer == 8) ? 0.87 : 0.90;
  float effy = 1;

  // 20-23/Dec/2009 CR test (Offline S/N thd.=3.5)
  if (tkid == -202) effy = 0.25;
  if (tkid ==  213) effx = 0.70;
  if (tkid ==  214) effx = 0.50;
  if (tkid == -405) effx = 0.50;
  if (tkid ==  405) effx = 0;
  if (tkid == -509) effx = 0.75;
  if (tkid == -512) effx = 0.50;
  if (tkid ==  504) effx = 0.70;
  if (tkid ==  510) effx = 0.35;
  

  if (rndx < effx && asx+asy > 0 && 0 < stx && stx+seedx < 383) {
    adc[0] = a1x;
    adc[1] = a2x;
#ifndef __ROOTSHAREDLIBRARY__ 
    if(cont!=0) cont->addnext(new AMSTrCluster
			   (tkid, 0, stx+640, 2, seedx, adc, 0));
#else
    if(cont!=0) cont->addnext(new TrClusterR
			   (tkid, 0, stx+640, 2, seedx, adc, 0));
#endif
  }
  if (rndy < effy && asx+asy > 0 && 0 < sty && sty+seedy < 639) {
    adc[0] = a1y;
    adc[1] = a2y;
#ifndef __ROOTSHAREDLIBRARY__ 
    if (cont) cont->addnext(new AMSTrCluster(tkid, 0, sty, 2, seedy, adc, 0));
#else
    if (cont) cont->addnext(new   TrClusterR(tkid, 0, sty, 2, seedy, adc, 0));
#endif
  }
  if (cont!=0) delete cont;
  return; 
}


void TrSim::fillreso(TrTrackR *track) {
  // if (fabs(track->GetRigidity()) < 100) return;
  for (int i = 0; i < track->GetNhits(); i++) {
    TrRecHitR *hit = track->GetHit(i);
    int        ily = hit->GetLayer()-1;
    if (sitkrefp[ily].norm() > 0) {
      int usev6=(TRCLFFKEY.recflag/10000000%10>=1);
      if(usev6){
        AMSPlaneM plm=hit->GetGCoordN(-1);
        TVector3 gcoo(sitkrefp[ily][0],sitkrefp[ily][1],sitkrefp[ily][2]);//postion
        TVector3 gdir(sitkangl[ily][0],sitkangl[ily][1],sitkangl[ily][2]);//direction
        TVector2 ploc=plm.straightLineToPlane(gcoo,gdir);//track linear interpolation to local coorinate
        TVector2 res=plm.getM()-ploc;
        if(!hit->OnlyY())hman.Fill("TrSimRx", sitkangl[ily].x(), res.X());//X
        if(!hit->OnlyX())hman.Fill("TrSimRy", sitkangl[ily].y(), res.Y());//Y
      }
      else {
        AMSPoint dpc = hit->GetCoord()-sitkrefp[ily];
        hman.Fill("TrSimRx", sitkangl[ily].x(), dpc.x()*1e4);
        hman.Fill("TrSimRy", sitkangl[ily].y(), dpc.y()*1e4);
      }
    }
  }
}



void TrSim::sitkdigi() {

  if (VERBOSE) printf("TrSim::sitkdigi-V called\n");

  isbuilt = true; 

#ifndef __ROOTSHAREDLIBRARY__
  AMSgObj::BookTimer.start("SiTkDigiAll"); 
#endif

  // If fast simulation skip, digitization is already done (gencluster)!
  if (TRMCFFKEY.SimulationType==kNoRawSim) {
#ifndef __ROOTSHAREDLIBRARY__
    AMSgObj::BookTimer.stop("SiTkDigiAll");
#endif
    return;
  }

  // Book histograms (MC gen+rec mode) if not yet
//  if (!hman.IsEnabled()) {
//    hman.Enable();
//    hman.BookHistos(3);
//  }

  // Beginning of other digitizations (GBATCH, TrSim2010)
#ifndef __ROOTSHAREDLIBRARY__
  AMSgObj::BookTimer.start("SiTkDigiShow");
#endif

  // Initialize sensors if not yet
  if (_sensors[0].GetSensorType() < 0 ||
      _sensors[1].GetSensorType() < 0 ||
      _sensors[2].GetSensorType() < 0) {
    cout << "TrSim::sitkdigi-I-Initialize TrSimSensors" << endl;
    InitSensors();
  }
 
  // Rework MCCluster list (pre-digitization)
  int merge = 0;
  if ( ((int(TRMCFFKEY.MergeMCCluster/10)%10)==0)||
       ((int(TRMCFFKEY.MergeMCCluster/10)%10)>=2) ) {
    merge = TRMCFFKEY.MergeMCCluster%10;
    if      (merge==1) MergeMCCluster();
    else if (merge==2) MergeMCCluster2(); 
    else if (merge==3) MergeMCCluster3();
    else if (merge==4) MergeMCCluster4();  // only merge 1st two particles
    else if (merge==5) MergeMCCluster5();  // do not merge particles witchg charge> 1 together
    else if (merge==6) MergeMCClusterSameTrkId(); // only merge steps with same track id 
  }

  // Create the TrMCCluster map and make the simulated cluster (_shower(), GenSimCluster())
  TrMap<TrMCClusterR> MCClusterTkIdMap;
  // Generate the list of MC Cluster on each ladder
  MCClusterTkIdMap.Clear();
  // Get the pointer to the TrMCCluster container
  VCon* container = GetVCon()->GetCont("AMSTrMCCluster");
  if (container==0) {
    if (WARNING) printf("TrSim::CreateMCClusterTkIdMap-W no TrMCCluster container, skip.\n");
    return;
  }
  if (container->getnelem()==0) {
    if (WARNING) printf("TrSim::CreateMCClusterTkIdMap-W TrMCCluster container is empty, skip.\n");
    if (container!=0) delete container;
    return;
  }
  for (int ii=0; ii<container->getnelem(); ii++) {
    TrMCClusterR* cluster = (TrMCClusterR*) container->getelem(ii);
    if (cluster) {
      // Construct ideal clusters!!!    
      if      (TRMCFFKEY.SimulationType==kRawSim) cluster->_shower();
      else if (TRMCFFKEY.SimulationType==kTrSim2010) cluster->GenSimClusters();
      else if (TRMCFFKEY.SimulationType==kTrSim2017) cluster->ApplySaturation();//QY
      else if (TRMCFFKEY.SimulationType==kTrSim2010Renewed) {
        if (merge!=3) cluster->GenSimClusters3();
        if ((TRMCFFKEY.NormalizeSimCluster%10)>=2) cluster->ApplyNormalization();
      }
      MCClusterTkIdMap.Add(cluster);
    }
  }
  if (container!=0) delete container;

#ifndef __ROOTSHAREDLIBRARY__
  AMSgObj::BookTimer.stop("SiTkDigiShow");
#endif

  // Skip digitization if the number of cluster is too low  
  if (MCClusterTkIdMap.Size()<TRMCFFKEY.MinMCClusters) {
#ifndef __ROOTSHAREDLIBRARY__
    AMSgObj::BookTimer.stop("SiTkDigiAll");
#endif
    return;
  }

  // Number of "fully" simulated ladders
  int nsimladders = 0;

  // Loop on all the ladders
  for (int icrate=0; icrate<8; icrate++) {
    for (int itdr=0; itdr<24; itdr++) {
      int hwid = icrate*100 + itdr;
      int tkid = (TkDBc::Head->FindHwId(hwid))->GetTkId();

      // Skip ladders if no simulation is forseen (it is not full noise mode & there are no clusters)
      if ( (TRMCFFKEY.NoiseType!=1)&&(MCClusterTkIdMap.GetNelem(tkid)<=0) ) continue; // speed-up!

      // Initialize the ladder buffer  
      double  ladbuf[1024]; // = {0};
      memset(ladbuf,0,1024*sizeof(ladbuf[0]));

      // Take the right calibration
      TrLadCal* ladcal = TrCalDB::Head->FindCal_HwId(hwid);
      if (ladcal==0) {
        if (WARNING) printf("TrSim::AddNoiseOnBuffer-W no ladder calibration found, no noise for ladder tkid=%+04d\n",tkid);
        continue;
      }

      // Number of "ideal" cluster
      int nclu = 0;

#ifndef __ROOTSHAREDLIBRARY__
      AMSgObj::BookTimer.start("SiTkDigiLadd");
#endif

      // Loop on MC Clusters
      for (int icl=0; icl<MCClusterTkIdMap.GetNelem(tkid); icl++) {
        TrMCClusterR* cluster = (TrMCClusterR*) MCClusterTkIdMap.GetElem(tkid,icl);
        if (cluster==0) continue;  
        // Add ideal clusters
        if      (TRMCFFKEY.SimulationType==kRawSim)    nclu += AddOldSimulationSignalOnBuffer(cluster,ladbuf);
        else if ( (TRMCFFKEY.SimulationType==kTrSim2010)||
                  (TRMCFFKEY.SimulationType==kTrSim2017)||
                  (TRMCFFKEY.SimulationType==kTrSim2010Renewed) ) nclu += AddTrSimClustersOnBuffer(cluster,ladbuf);
      }

#ifndef __ROOTSHAREDLIBRARY__
      AMSgObj::BookTimer.stop("SiTkDigiLadd");
#endif

      if (nclu>0) {

        if ((TRMCFFKEY.NormalizeSimCluster%10)==0) { // strip-by-strip signal
          // gain
          TrLadPar* ladpar = TrParDB::Head->FindPar_TkId(tkid); // used at least as database for low/bad gains (conservative)
          for (int ii=0;ii<1024;ii++) {
            if(ladbuf[ii]==0. )continue;
            int iva=int(ii/64);
            int iside=(ii>639)?0:1;
            float gain = ladpar->GetGain(iside)*ladpar->GetVAGain(iva);
            if      ( gain<0.02 )     ladbuf[ii]=0.;   // VA with no gain!
            else if ( (1./gain)<0.5 ) ladbuf[ii]/=10.; // VA with bad gain!
            else {
              if (TRMCFFKEY.GainType==0) ladbuf[ii] /= gain; // old gain
              else                       ladbuf[ii] = TrGainDB::GetHead()->ApplyGain(ladbuf[ii],tkid,iva); // new gain
            } 
          }
        }

        // asymmetry
	for (int ii=1023; ii>640; ii--){ // X
	  double sn = ladbuf[ii];
	  double snm1 = ladbuf[ii-1];
	  ladbuf[ii] = sn + snm1*TRMCFFKEY.Asymmetry[0];
	}
	for (int ii=639; ii>0; ii--) { // Y
	  double sn = ladbuf[ii];
	  double snm1 = ladbuf[ii-1];
	  ladbuf[ii] = sn + snm1*TRMCFFKEY.Asymmetry[1];
	}

        // additional noise
	for (int ii=0;ii<1024;ii++){
	  int iside=(ii>639)?0:1;
	  ladbuf[ii]+=TRMCFFKEY.TrSim2010_AddNoise[iside]*rnormx();
	}

        // More noise only where there is some cluster
	int llb[1024];
	memset(llb,0,1024*sizeof(llb[0]));
	for(int ii=0;ii<1024;ii++){
	  if(ladbuf[ii]!=0){
	    for (int jj=ii-3;jj<=ii+3;jj++)
	      if(jj>=0&&jj<1024) llb[jj]=1;
	  }
	}
	for (int ii=0; ii<1024; ii++){
	  int iside = (ii>639) ? 0 : 1;
	  if( llb[ii]!=0){
	    double noise=ladcal->GetSigma(ii)*rnormx()*TRMCFFKEY.noise_fac[iside];
	    ladbuf[ii]+=noise;
	  }
	}
      }

      // Add noise simulation  
      if ( (TRMCFFKEY.NoiseType==1) ||                 // noise on all ladders
  	   ( (TRMCFFKEY.NoiseType>=2)&&(nclu>0) ) ) {  // noise on ladders with some cluster 
#ifndef __ROOTSHAREDLIBRARY__
        AMSgObj::BookTimer.start("SiTkDigiNoise");
#endif
        AddNoiseOnBuffer(ladbuf,ladcal);
#ifndef __ROOTSHAREDLIBRARY__
        AMSgObj::BookTimer.stop("SiTkDigiNoise");
#endif
      } 

      for (int ii=0;ii<1024;ii++) {
        // Put the  ADC 12-bit  hard cieling to the signal amplitude in ADC
 	if ( (TRMCFFKEY.DisableOverflow==0)&&(ladbuf[ii]+ladcal->GetPedestal(ii)>4096) ) ladbuf[ii]=4096-ladcal->GetPedestal(ii);
        // FTE off
        if ( (ladcal->Power_failureS!=0)&&(ii<640) ) ladbuf[ii] = 0;    
        if ( (ladcal->Power_failureK!=0)&&(ii>=640) ) ladbuf[ii] = 0;
      }

      // DSP Simulation
      if ( (TRMCFFKEY.NoiseType==1) ||  // DSP on all ladders
           (nclu>0) ) {                 // DSP only on ladders with some cluster/noise
#ifndef __ROOTSHAREDLIBRARY__
        AMSgObj::BookTimer.start("SiTkDigiDsp");
#endif
        // DSP on n-side
        BuildTrRawClustersWithDSP(0,tkid,ladcal,ladbuf);         
        // DSP on p-side
        BuildTrRawClustersWithDSP(1,tkid,ladcal,ladbuf); 
        // Number of ladders "fully" simulated
        nsimladders++;
#ifndef __ROOTSHAREDLIBRARY__
        AMSgObj::BookTimer.stop("SiTkDigiDsp");
#endif
      }
    }
  }

  // Rework MCCluster list (post-digitization) 
  if ((int(TRMCFFKEY.MergeMCCluster/10)%10)>=1) {
    merge = TRMCFFKEY.MergeMCCluster%10;
    if      (merge==1) MergeMCCluster();
    else if (merge==2) MergeMCCluster2();
    else if (merge==3) MergeMCCluster3();
    else if (merge==4) MergeMCCluster4(); // only merge 1st two particles
    else if (merge==5) MergeMCCluster5(); // do not merge particles witchg charge> 1 together
    else if (merge==6) MergeMCClusterSameTrkId(); // only merge steps with same track id 
  }

  // Build fake noise clusters
  if (TRMCFFKEY.NoiseType>1) {
#ifndef __ROOTSHAREDLIBRARY__
    AMSgObj::BookTimer.start("SiTkDigiFake");
#endif
    sitknoise(nsimladders, &(MCClusterTkIdMap));
#ifndef __ROOTSHAREDLIBRARY__
    AMSgObj::BookTimer.stop("SiTkDigiFake");
#endif
  }

#ifndef __ROOTSHAREDLIBRARY__
  AMSgObj::BookTimer.stop("SiTkDigiAll"); 
#endif
  
  return;
}




void TrSim::AddNoiseOnBuffer(double* ladbuf, TrLadCal* ladcal) {
  // Noise baseline 
  for (int ii=0; ii<640; ii++) {
    if (ladcal->GetStatus(ii)&TrLadCal::dead) ladbuf[ii] = DEADSTRIPADC; // dead strip    
    else  if(ladbuf[ii]==0)                   ladbuf[ii] += ladcal->Sigma(ii)*rnormx()*TRMCFFKEY.GlobalNoiseFactor[1]; // normal/hot stri
  }
 for (int ii=640; ii<1024; ii++) {
    if (ladcal->GetStatus(ii)&TrLadCal::dead) ladbuf[ii] = DEADSTRIPADC; // dead strip    
    else  if(ladbuf[ii]==0)                   ladbuf[ii] += ladcal->Sigma(ii)*rnormx()*TRMCFFKEY.GlobalNoiseFactor[0]; // normal/hot stri
  }
}


int TrSim::AddTrSimClustersOnBuffer(TrMCClusterR* cluster, double* ladbuf) {
  int nclusters = 0;
  bool isK7 = ((TkLadder*)TkDBc::Head->FindTkId(cluster->GetTkId()))->IsK7();
  // Loop on the two sides
  for (int iside=0; iside<2; iside++) {
    TrSimCluster* simcluster = cluster->GetSimCluster(iside);
    if (simcluster==0) continue;
    // Putting cluster on the ladder buffer 
    for (int ist=0; ist<simcluster->GetWidth(); ist++) {
      // Take address on buffer
      int address = simcluster->GetAddress(ist) + 640*(1-iside); 
      if ( (isK7)&&(iside==0) ) address = simcluster->GetAddressCycl(ist) + 640; // considering cyclicity
      if ( (0<=address)&&(address<1024) ) ladbuf[address] += simcluster->GetSignal(ist);
      else printf("TrSim::AddSimulatedClustersOnBuffer-E address out of bounds (%d)\n",address);
    }
    nclusters++;
  }
  return nclusters;
}


int TrSim::AddOldSimulationSignalOnBuffer(TrMCClusterR* cluster, double* ladbuf) {
  printf("TrSim::AddOldSimulationSignalOnBuffer-E OLD Simulation declared OBSOLETE!! I'm not doing anything\n");
  return -1;
  // int addK = cluster->GetAdd(0);
  // for (int jj=0;jj<cluster->GetSize(0);jj++) ladbuf[addK+jj] += cluster->GetSignal(jj,0);
  // int addS = cluster->GetAdd(1);
  // for (int jj=0;jj<cluster->GetSize(1);jj++) ladbuf[addS+jj] += cluster->GetSignal(jj,1);
  // return 2;
}


int TrSim::BuildTrRawClustersWithDSP(const int iside, const int tkid, TrLadCal* ladcal,double * ladbuf) {
  // Does the calibration exist?
  if (!ladcal) {
    if (WARNING) printf("TrSim::BuildTrRawClustersOnSide-W no ladder calibration found, no noise for ladder tkid=%+04d\n",tkid);
    return 0;
  }

  // Does the AMSTrRawCluster container exist?
  VCon* cont = GetVCon()->GetCont("AMSTrRawCluster");
  if (!cont) {
    if (WARNING) printf("TrSim::BuildTrRawClustersOnSide-W cannot find AMSTrRawCluster, no cluster produced\n");
    return 0;
  } 

  // Init
  char sidename[2] = {'x','y'};
  int nclusters = 0;
  int addmin[2] = {  0, 640};
  int addmax[2] = {640,1024};
  int used[1024]; memset(used,0,1024*sizeof(used[0]));
  int ilayer = int(std::abs(tkid/100))-1;
  int hwid = (TkDBc::Head->FindTkId(tkid))->GetHwId();
  int ientry = int(hwid/100)*24 + hwid%24;

  // Loop on address (0 - 640, or 640 - 1024)
  for (int ii=addmin[iside]; ii<addmax[iside]; ii++) {

    if (used[ii]!=0)           continue; // a strip used in a cluster 
    if (ladcal->Status(ii)!=0) continue; // a strip with status different from 0
    if ( (ladbuf[ii]/ladcal->Sigma(ii))<=TRMCFFKEY.DSPSeedThr[iside][ilayer] ) continue; // a strip under seed threshold

    // "Seed" Found (not max)
    int seed = ii;
    used[ii] = 1;

    // Left Loop
    int left = seed;
    for (int jj=seed-1; jj>=addmin[iside]; jj--) {
      if ( (ladbuf[jj]/ladcal->Sigma(jj)>TRMCFFKEY.DSPNeigThr[iside] ||(ladcal->Status(jj)!=0 && (ladcal->Status(jj)&0x8000)==0)) && used[jj]==0) {
        used[jj] = 1;
        left = jj;
      } 
      else 
        break;
    }
 
    // Right Loop
    int right = seed;
    for (int jj=seed+1; jj<addmax[iside]; jj++) {
      if ( (ladbuf[jj]/ladcal->Sigma(jj)>TRMCFFKEY.DSPNeigThr[iside] ||(ladcal->Status(jj)!=0 && (ladcal->Status(jj)&0x8000)==0)) && used[jj]==0) {
        used[jj] = 1;
        right = jj;
      } 
      else 
        break;
    }

    // at least 3 strips!
    if(left >addmin[iside])     left--;
    if(right<(addmax[iside]-1)) right++;
    int nstrips = right - left + 1;

    // another approach for overflow
    if (TRMCFFKEY.DisableOverflow==2) {
      float total = 0;
      for (int ist=0; ist<nstrips; ist++) {
        int ii = left + ist;
        if (ladbuf[ii]<0) continue;
        total += ladbuf[ii];
      }
      for (int iter=0; iter<10; iter++) {
        float total_overflows = 0;
        float total_no_overflows = 0;
        for (int ist=0; ist<nstrips; ist++) {
          int ii = left + ist;
          if (ladbuf[ii]<0) continue;
          if (ladbuf[ii]+ladcal->GetPedestal(ii)>=4096.) total_overflows += 4096.-ladcal->GetPedestal(ii);
          else total_no_overflows += ladbuf[ii];
        }
        if (fabs(total_overflows+total_no_overflows-total)<1e-3) break; 
        for (int ist=0; ist<nstrips; ist++) {
          int ii = left + ist;
          if (ladbuf[ii]<0) continue;
          ladbuf[ii] = TMath::Min(4096.-ladcal->GetPedestal(ii),ladbuf[ii]*(total-total_overflows)/total_no_overflows);
        } 
      }     
    }

    // Preparing the cluster
    //short int signal[128];
    float signal[128]; // to accomodate overflows
    for(int ist=0; ist<nstrips; ist++) { 
    //signal[ist%128] = int(8*ladbuf[left + ist]); 
      signal[ist%128] = int(8*ladbuf[left + ist])/8.;
      if ( (((ist+1)%128)==0) || ((ist+1)==nstrips) ) { 
        int cluslenraw = (((ist+1)%128)==0) ? 127 : (nstrips%128)-1; // 0->1, 1->2, ..., 127->128
        // int seedvalue  = int(4*ladbuf[maxstrip]);
        // cluslenraw |= (seedvalue<<7);
        int n128 = int((ist+1)/128);
        int clusaddraw = left + 128*n128;
        // clusaddraw |= (0<<10); // CN bits status (not simulated)
        // clusaddraw |= (0<<14); // Power Bits status (not simulated)
#ifndef __ROOTSHAREDLIBRARY__
        if (cont) cont->addnext(new AMSTrRawCluster(tkid,clusaddraw,cluslenraw,signal));
#else
        if (cont) cont->addnext(new TrRawClusterR(  tkid,clusaddraw,cluslenraw,signal));
#endif
        hman.Fill(Form("TrSimCls%c",sidename[iside]),ientry);
	nclusters++;
        if (VERBOSE) {
          printf("TrSim::TrRawCluster\n");
          TrRawClusterR* cluster = (TrRawClusterR*) cont->getelem((int)cont->getnelem()-1);
          cluster->Print();
          printf("TrSim::TrRawCluster-More Local Seed Coordinate = %f\n",TkCoo::GetLocalCoo(tkid,cluster->GetSeedAddress(),0));
        }
      }
    }
  }
  if (cont!=0) delete cont;
  return nclusters;
}


void TrSim::sitknoise(int nsimladders,TrMap<TrMCClusterR> *MCClusterTkIdMap ) {

  // does the AMSTrRawCluster container exist?
  VCon* cont = GetVCon()->GetCont("AMSTrRawCluster");
  if(!cont){
    if (WARNING) printf("TrSim::sitknoise-W cannot find AMSTrRawCluster, no fake cluster produced\n");
    return;
  }

  // init of some things
  char  sidename[2] = {'x','y'};
  float dummyfloat = 0;
  int   dummyint = 0;
  int   nstrips[2] = {384, 640};
  int   nchannel[2]; 
  int   ncluster[2]; // number of seed over 3.5 thresholds
  short int signal[128]; 
  vector<float> clusterSN;

  // gaussian probability to be over 3.5 sigma threshold
  // P(x>3.5) = \int_{3.5}^{\infty} g(x;0,1) dx
  float integral = TMath::Erfc(3.5/sqrt(2))/2.; 
  // upper and lower limit of the gaussian cumulative distributioni
  /*
   y ^          Gaussian      y  ^  
     |\                        1 |.......____________
     | \_  y = g(3.5;0,1)        |     _/
     |...\__                     |..__/  y = int(-inf,3.5) g(x;0,1) dx
     |   :  \____                | /  : 
     |   :       \_____          |/   :
    -+-------------------->     -+--------------------->
     |  3.5             x        |   3.5              x
  */
  float aminim = 1 - integral;
  float amaxim = 1.; 

  /*
    How many times the number will be over the 3.5 threshold?
    This is a binomial experiment, but
    - prob = Erfc(3.5/\sqrt{2})/2 < 0.01 (actually is close to 10^{-4})  
    - extractions > 20 (lets say 10000)
    and binomial became poissonian in this limit.
  */ 
  for (int iside=0; iside<2; iside++) {
    // considering only the not simulated ladders 
    nchannel[iside] = (192-nsimladders)*nstrips[iside]; 
    // poissonian average
    float mean = nchannel[iside]*integral;
    // tuning needed for the match on data
    if (TRMCFFKEY.NoiseType==3) mean *= 2; 
    // poisson extraction
    POISSN(mean,ncluster[iside],dummyint); 
  }
  if (VERBOSE) printf("sitknoise::pclusters=%3d nclusters=%3d\n",ncluster[0],ncluster[1]);

  // loop on the two sides
  for (int iside=0; iside<2; iside++) {
    // loop on the number of clusters
    for (int iclu=0; iclu<ncluster[iside]; iclu++) {

      // re-creation of a SN cluster 
      clusterSN.clear();

      // extract ladder where the cluster has to be placed  
      // (the ladder should not be already simulated)
      bool ladderfound = false;
      int  tkid = 0;
      int  ientry = -1;
      while (!ladderfound) {
	ientry = int(192.*RNDM(dummyint));
        int hwid = int(ientry/24)*100 + (ientry%24);
	TkLadder* ladder = TkDBc::Head->FindHwId(hwid);
	tkid = ladder->GetTkId();
	if (MCClusterTkIdMap->GetNelem(tkid)<=0) ladderfound = true;
      }
      int ilayer = int(std::abs(tkid/100))-1;

      // extract the seed signal-over-noise 
      // using gaussian cumulative inverse function on a short interval
      float extrac = aminim + (amaxim-aminim)*RNDM(dummyint);
      float seedSN = sqrt(2)*TMath::ErfInverse(2*extrac-1);
      if (VERBOSE) printf("TrSim::sitknoise-V extraction: min=%10.8g max=%10.8g extr=%7.3f seed=%7.3f\n",aminim,amaxim,extrac,seedSN);
      
      // check if the seed can exist on the extracted ladder 
      // (threshold layer by layer)
      if (seedSN<TRMCFFKEY.DSPSeedThr[iside][ilayer]) continue;

      // extract the seed position 
      int seedadd = int(nstrips[iside]*RNDM((int)dummyfloat));
      int address = seedadd;

      // take the right calibration
      TrLadCal* ladcal = TrCalDB::Head->FindCal_TkId(tkid);
      if (ladcal==0) {
        if (WARNING) printf("TrSim::sitknoise-W no ladder calibration found, no noise for ladder tkid=%+04d skipping creation of fake clusters\n",tkid);
        continue;
      }
    
      // put the seed on the cluster 
      clusterSN.push_back(seedSN);

      // left loop
      for (int add=seedadd-1; add>=0; add--) {
        // if the extraction is over 3.5 should be discarded 
        float SN = rnormx();
        while (SN>=3.5) SN = rnormx(); 
        // cout << "+- " << SN << " " << ladcal->Status(add) << endl;
        clusterSN.insert(clusterSN.begin(),SN);
        address = add;
        if ( (SN<TRMCFFKEY.DSPNeigThr[iside])&&(ladcal->Status(add)==0) ) break;
      }	
      // right loop
      for (int add=seedadd+1; add<nstrips[iside]; add++) {
        // if the extraction is over 3.5 should be discarded 
        float SN = rnormx();
        while (SN>=3.5) SN = rnormx();
        // cout << "++ " << SN << " " << ladcal->Status(add) << endl;
        clusterSN.push_back(SN); 
        if ( (SN<TRMCFFKEY.DSPNeigThr[iside])&&(ladcal->Status(add)==0) ) break;
      } 

      // normalizing to noise (no gain correction for pure noise clusters)
      address += (1-iside)*640;
      int nstrips = int(clusterSN.size());
      for (int ii=0; ii<nstrips; ii++) {
        if (ladcal->GetStatus(address+ii)&TrLadCal::dead) signal[ii] = int(8*DEADSTRIPADC);                               // dead strip    
	else                                              signal[ii] = int(8*clusterSN.at(ii)*ladcal->Sigma(address+ii)); // normal strips
      }
      // Needed fix: no repetition of used strips! < (small effect) 
      int cluslenraw = (nstrips%128)-1;
      int clusaddraw = address;
#ifndef __ROOTSHAREDLIBRARY__
      if (cont) cont->addnext(new AMSTrRawCluster(tkid,clusaddraw,cluslenraw,signal));
#else
      if (cont) cont->addnext(new TrRawClusterR(  tkid,clusaddraw,cluslenraw,signal));
#endif	
      hman.Fill(Form("TrSimFake%c",sidename[iside]),ientry); 
      if (VERBOSE) {
        printf("TrSim::TrRawCluster\n");
        TrRawClusterR* cluster = (TrRawClusterR*) cont->getelem((int)cont->getnelem()-1);
        cluster->Print(10);
        printf("TrSim::TrRawCluster-More Local Seed Coordinate = %f\n",TkCoo::GetLocalCoo(tkid,cluster->GetSeedAddress(),0));
      }
    }
  }
  if(cont) delete cont; 
  return;
}


TrSimSensor* TrSim::GetTrSimSensor(int side, int tkid) {
  TkLadder* ll = TkDBc::Head->FindTkId(tkid);
  if(!ll) {
    printf("TrSim::GetTrSimSensor-E cannot find ladder %d into the database, returning 0.\n",tkid);
    return 0;
  }
  if (side==1)    return &_sensors[0]; // S 
  if (ll->IsK7()) return &_sensors[2]; // K7
  else            return &_sensors[1]; // K5
  return 0; 
} 


void TrSim::PrintBuffer(double *_ladbuf) {
  for (int ii=0; ii<64; ii++) {
    for (int jj=0; jj<16; jj++) {
      printf("%5.2f ",_ladbuf[ii*16+jj]);
    }
    printf("\n");
  }
}


void TrSim::PrintSimPars() {
  cout << "SimulationType:             " << TRMCFFKEY.SimulationType << endl;
  cout << "MinMCClusters:              " << TRMCFFKEY.MinMCClusters << endl;
  cout << "NoiseType:                  " << TRMCFFKEY.NoiseType << endl;
  for (int ilayer=0; ilayer<9; ilayer++) {
    cout << "DSPSeedThr(n,p) layer " << ilayer << "     " << TRMCFFKEY.DSPSeedThr[0][ilayer] << " " << TRMCFFKEY.DSPSeedThr[1][ilayer] << endl;
  }
  cout << "DSPNeigThr(n,p):            " << TRMCFFKEY.DSPNeigThr[0] << " " << TRMCFFKEY.DSPNeigThr[1] << endl;
  cout << "TrSim2010_ADCMipValue(n,p): " << TRMCFFKEY.TrSim2010_ADCMipValue[0] << " " << TRMCFFKEY.TrSim2010_ADCMipValue[1] << endl;
  cout << "TrSim2010_PStripCorr:       " << TRMCFFKEY.TrSim2010_PStripCorr << endl;
  cout << "TrSim2010_ADCSat(n,p):      " << TRMCFFKEY.TrSim2010_ADCSat[0] << " " << TRMCFFKEY.TrSim2010_ADCSat[1] << endl;
  cout << "TrSim2010_Cint(n,p):        " << TRMCFFKEY.TrSim2010_Cint[0] << " " << TRMCFFKEY.TrSim2010_Cint[1] << endl;
  cout << "TrSim2010_Cbk(n,p):         " << TRMCFFKEY.TrSim2010_Cbk[0] << " " << TRMCFFKEY.TrSim2010_Cbk[1] << endl;
  cout << "TrSim2010_Cdec(n,p):        " << TRMCFFKEY.TrSim2010_Cdec[0] << " " << TRMCFFKEY.TrSim2010_Cdec[1] << endl;
  cout << "TrSim2010_DiffType(n,p):    " << TRMCFFKEY.TrSim2010_DiffType[0] << " " << TRMCFFKEY.TrSim2010_DiffType[1] << endl;
  cout << "TrSim2010_DiffRadius(n,p):  " << TRMCFFKEY.TrSim2010_DiffRadius[0] << " " << TRMCFFKEY.TrSim2010_DiffRadius[1] << endl;
  cout << "TrSim2010_FracNoise(n,p):   " << TRMCFFKEY.TrSim2010_FracNoise[0] << " " << TRMCFFKEY.TrSim2010_FracNoise[1] << endl;
  cout << "TrSim2010_AddNoise(n,p):    " << TRMCFFKEY.TrSim2010_AddNoise[0] << " " << TRMCFFKEY.TrSim2010_AddNoise[1] << endl;
  cout << "NormalizeSimCluster:        " << TRMCFFKEY.NormalizeSimCluster << endl;
  cout << "NormAdjBefore(n,p):         " << TRMCFFKEY.NormAdjBefore[0] << " " << TRMCFFKEY.NormAdjBefore[1] << endl;
  cout << "EdepAddNoise:               " << TRMCFFKEY.EdepAddNoise << endl;
  cout << "NormAdjAfter(n,p):          " << TRMCFFKEY.NormAdjAfter[0] << " " << TRMCFFKEY.NormAdjAfter[1] << endl; 
  cout << "BelauSubStep:               " << TRMCFFKEY.BelauSubStep << " um" << endl;
  cout << "BelauMinDiffusion:          " << TRMCFFKEY.BelauMinDiffusion << " um" << endl;
  cout << "BelauFactDiffusion:         " << TRMCFFKEY.BelauFactDiffusion << endl;
  cout << "BelauHallMobility(n,p):     " << TRMCFFKEY.BelauHallMobility[0] << " " << TRMCFFKEY.BelauHallMobility[1] << " cm^2/V/s" << endl;
  cout << "SetActiveSiliconCuts:       " << TRMCFFKEY.SetActiveSiliconCuts << endl;
  cout << "MaxStepInSilicon:           " << TRMCFFKEY.MaxStepInSilicon << " um" << endl;
  cout << "MinDeltaRayRange:           " << TRMCFFKEY.MinDeltaRayRange << " um" << endl;
  cout << "MinDeltaRayEnergy:          " << TRMCFFKEY.MinDeltaRayEnergy << " keV" << endl; 
  cout << "SetBetterEnergyAndDir:      " << TRMCFFKEY.SetBetterEnergyAndDir << endl;
  cout << "Asymmetry(n,p):             " << TRMCFFKEY.Asymmetry[0] << " " << TRMCFFKEY.Asymmetry[1] << endl;
}



void TrSim::MergeMCCluster(){
  vector<TrMCClusterR> locstor;
 
  // Get the pointer to the TrMCCluster container
  VCon* container = GetVCon()->GetCont("AMSTrMCCluster");
  if (container==0) {
    if (WARNING) printf("TrSim::MergeMCCluster-W no TrMCCluster container, skip.\n");
    return;
  }
  int clen=container->getnelem();
  if (clen==0) {
    if (WARNING) printf("TrSim::MergeMCCluster-W TrMCCluster container is empty, skip.\n");
    if (container!=0) delete container;
    return;
  }
  //   printf("\n\n List from Merge Cluster BEGIN\n");
  //   for (int ii=0;ii<clen;ii++)
  // #ifdef __ROOTSHAREDLIBRARY__
  //     ((TrMCClusterR*) container->getelem(ii))->Print();
  // #else
  //    ((AMSTrMCCluster*) container->getelem(ii))->Print();
  // #endif
  //   if(container) delete container;
  //   printf("List from Merge Cluster END\n\n");
  //   return;
  map<int,vector< pair<int,TrMCClusterR*> > > TrLadMap;
  map<int,vector< pair<int,TrMCClusterR*> > >::iterator iter;

  for (int jj=0; jj<clen; jj++){
    TrMCClusterR* cl0 = (TrMCClusterR*) container->getelem(jj);
    int tkid=cl0->GetTkId();
    TrLadMap[tkid].push_back(std::make_pair(0,cl0));
  }
  
  for(iter=TrLadMap.begin();iter!=TrLadMap.end();iter++){
    vector< pair<int,TrMCClusterR*> > & mm= iter->second;
    for(unsigned int jj=0;jj<mm.size();jj++){
      if(mm[jj].first==1) continue;
      locstor.push_back(*(mm[jj].second));
      mm[jj].first=1;
      for (unsigned int ii=jj+1; ii<mm.size(); ii++) {
	if(mm[ii].first==1) continue;
	TrMCClusterR* cl = mm[ii].second;
	AMSPoint dd=locstor.rbegin()->_xgl-cl->_xgl;
	if((fabs(dd[0])+fabs(dd[1]))<0.015) {
	  *(locstor.rbegin())+=(*cl);
	  mm[ii].first=1;
	}
      }
    }
  }

  if(locstor.size()>0){
    container->eraseC();
    for(unsigned int ii=0;ii<locstor.size();ii++)
#ifdef __ROOTSHAREDLIBRARY__
      container->addnext( new TrMCClusterR(locstor[ii]) );
#else
      container->addnext( new AMSTrMCCluster(locstor[ii]) );
#endif
  }
  
  if (container!=0) delete container;
  return;
}


static bool sort_with_gtrkid(TrMCClusterR* a, TrMCClusterR* b) { 
  if      ( (a->_gtrkid> 0)&&(b->_gtrkid> 0) ) return (a->_gtrkid<b->_gtrkid);
  else if ( (a->_gtrkid> 0)&&(b->_gtrkid<=0) ) return true;
  else if ( (a->_gtrkid<=0)&&(b->_gtrkid> 0) ) return false;
  return (a->_gtrkid>b->_gtrkid);
}


static bool sort_pzm(TrMCClusterR* a, TrMCClusterR* b) {
// primary Z momentum
    if(a->_itra>=0 && b->_itra<0)return true;
    else if(a->_itra<0 && b->_itra>=0)return false;
    geant chargea,chargeb;  
#ifndef __ROOTSHAREDLIBRARY__
    char chp[22]="";
    integer itrtyp;
    geant mass;
    geant tlife;
    geant ub[1];
    integer nwb=0;
    if(abs(a->_itra)>=10000)chargea=int(abs(a->_itra)/100)%100;
    else                   {GFPART(abs(a->_itra),chp,itrtyp,mass,chargea,tlife,ub,nwb); chargea=fabs(chargea);}
    if(abs(b->_itra)>=10000)chargeb=int(abs(b->_itra)/100)%100;
    else                   {GFPART(abs(b->_itra),chp,itrtyp,mass,chargeb,tlife,ub,nwb); chargeb=fabs(chargeb);}
#else
           chargea=a->getZ();
           chargeb=b->getZ();
#endif
         if(chargea>chargeb)return true;
         else if(chargea<chargeb)return false;

    return (a->_mom>b->_mom);
}
static bool sort_pe(TrMCClusterR* a, TrMCClusterR* b) {
// primary edep 
    if(a->_itra>=0 && b->_itra<0)return true;
    else if(a->_itra<0 && b->_itra>=0)return false;
    return (a->_edep>b->_edep);
}

static bool sort_with_momentum(TrMCClusterR* a, TrMCClusterR* b) {
  return (a->_mom>b->_mom);
}


void TrSim::MergeMCCluster2() {

  // get container 
  VCon* container = GetVCon()->GetCont("AMSTrMCCluster");
  if (container==0) {
    if (WARNING) printf("TrSim::MergeMCCluster2-W no TrMCCluster container, skip.\n");
    return;
  }

  // check container
  int clen = container->getnelem();
  if (clen==0) {
    if (WARNING) printf("TrSim::MergeMCCluster2-W TrMCCluster container is empty, skip.\n");
    if (container!=0) delete container;
    return;
  }

  // local storage of new clusters
  vector<TrMCClusterR*> locstor;

  // create a map with key tkid of [clusters,isused]
  map<int,vector<TrMCClusterR*> > TrLadMap;
  map<int,vector<TrMCClusterR*> >::iterator TrLadMapIter;
  for (int jj=0; jj<clen; jj++) {
    TrMCClusterR* cluster = (TrMCClusterR*) container->getelem(jj);
    int tkid = cluster->GetTkId();
    TrLadMap[tkid].push_back(cluster);
  }
 
  // loop on ladders
  for (TrLadMapIter=TrLadMap.begin(); TrLadMapIter!=TrLadMap.end(); TrLadMapIter++) {

    // create list of steps belonging to the same track
    map<int,vector<TrMCClusterR*> > TrTrkIdMap;
    map<int,vector<TrMCClusterR*> >::iterator TrTrkIdMapIter;
    vector<TrMCClusterR*>& cluster_list = TrLadMapIter->second;
    for(unsigned int jj=0; jj<cluster_list.size(); jj++) { 
      TrMCClusterR* cluster = (TrMCClusterR*) cluster_list.at(jj);
      if (!cluster) { 
        printf("TrSim::MergeMCCluster2-W found an empty pointer in the AMSTrMCCluster container. Jump!");
        continue;
      }
      int trkid = cluster->_gtrkid;
      TrTrkIdMap[trkid].push_back(cluster);  
    }

    // merge primaries
    TrMCClusterR* primary = 0; 
    for (TrTrkIdMapIter=TrTrkIdMap.begin(); TrTrkIdMapIter!=TrTrkIdMap.end(); TrTrkIdMapIter++) {   
      if (TrTrkIdMapIter->first!=1) continue; 
      vector<TrMCClusterR*>& cluster_list = TrTrkIdMapIter->second;
      int       gtrkid = cluster_list.at(0)->_gtrkid;
      int       idsoft = cluster_list.at(0)->_idsoft; 
      short int itra   = cluster_list.at(0)->_itra; 
      float     mom    = cluster_list.at(0)->_mom; 
      int       status = cluster_list.at(0)->Status;
      int       size   = cluster_list.size();   
      AMSDir    dir    = cluster_list.at(0)->GetDir(); 
      AMSPoint  head   = cluster_list.at(0)->GetStartPoint();
      AMSPoint  tail   = cluster_list.at(size-1)->GetEndPoint();
      // recalculate xgl, direction and step 
      float     step   = (tail-head).norm();
      if(step>0)dir    = (tail-head)/step;//bug fixed by QY(G4step could be 0)
//      AMSDir    dir    = (tail-head)/step;
      AMSPoint  xgl    = (tail+head)/2; 
      // sum edep 
      double   edep = 0;
      for(int jj=0; jj<size; jj++) {
        TrMCClusterR* cluster = (TrMCClusterR*) cluster_list.at(jj);
        edep += cluster->_edep;
      }
      // create a new cluster and add to the list of new MC clusters  
      primary = new TrMCClusterR(idsoft,step,xgl,dir,mom,edep,itra,gtrkid,status);
      locstor.push_back(primary);
    }

    // loop on secondaries
    AMSPoint point_primary_not_changed = (primary) ? primary->GetXgl() : AMSPoint(0,0,0);
    for (TrTrkIdMapIter=TrTrkIdMap.begin(); TrTrkIdMapIter!=TrTrkIdMap.end(); TrTrkIdMapIter++) {
      if (TrTrkIdMapIter->first==1) continue;
      vector<TrMCClusterR*>& cluster_list = TrTrkIdMapIter->second;
      int size = cluster_list.size();
      for(int jj=0; jj<size; jj++) {
        TrMCClusterR* secondary = (TrMCClusterR*) cluster_list.at(jj);
        // if no primary just add the secondary to the list 
        if (!primary) {
          locstor.push_back(new TrMCClusterR(*secondary));   
          continue;  
        }
        // far away we just add to the list 
        AMSPoint dist = secondary->GetXgl()-point_primary_not_changed;
        if (sqrt(dist[0]*dist[0]+dist[1]*dist[1])>0.0150) {  
          locstor.push_back(new TrMCClusterR(*secondary));   
          continue;
        }    
        // add this cluster to the primary (gtrkid, idsoft, itra, dir e mom no change)
        // use a shift based on weighted sum of energy deposition. 
        // this idea is just bidimensional (plane xy), NOT USING Z.
        double x = (primary->_xgl[0]*primary->_edep + secondary->_xgl[0]*secondary->_edep)/(primary->_edep + secondary->_edep);
        double y = (primary->_xgl[1]*primary->_edep + secondary->_xgl[1]*secondary->_edep)/(primary->_edep + secondary->_edep);
        double edep = primary->_edep + secondary->_edep;
        primary->_xgl[0] = x;
        primary->_xgl[1] = y;
        primary->_edep = edep;
      }
    }
  }   

  // reorder
  sort(locstor.begin(),locstor.end(),sort_with_gtrkid);

  // redo container
  if(locstor.size()>0) {
    container->eraseC();
    for(unsigned int ii=0;ii<locstor.size();ii++)
#ifdef __ROOTSHAREDLIBRARY__
      container->addnext( new TrMCClusterR(*locstor[ii]) );
#else
      container->addnext( new AMSTrMCCluster(*locstor[ii]) );
#endif
  }
  if (container!=0) delete container;
  return; 
}


void TrSim::MergeMCCluster3() {

  // get container 
  VCon* container = GetVCon()->GetCont("AMSTrMCCluster");
  if (container==0) {
    if (WARNING) printf("TrSim::MergeMCCluster3-W no TrMCCluster container, skip.\n");
    return;
  }

  // check container
  int clen = container->getnelem();
  if (clen==0) {
    if (WARNING) printf("TrSim::MergeMCCluster3-W TrMCCluster container is empty, skip.\n");
    if (container) delete container;
    return;
  }

  // local storage of MC clusters
  vector<TrMCClusterR*> local_storage1;
  vector<TrMCClusterR*> local_storage2;
  vector<TrMCClusterR*> local_storage3;
  vector<TrMCClusterR*> local_storage4; 

  // create a map with tkid key 
  map<int,vector<TrMCClusterR*> > TrLadMap;
  map<int,vector<TrMCClusterR*> >::iterator TrLadMapIter;
  for (int jj=0; jj<clen; jj++) {
    TrMCClusterR* cluster = (TrMCClusterR*) container->getelem(jj);
    // ignore already merged clusters (they will be deleted)
    if ( ((int(TRMCFFKEY.MergeMCCluster/100000)%10)==1)&&(cluster->checkstatus(0x20)) ) continue; 
    // create simulated clusters
    if ( (TRMCFFKEY.SimulationType==kTrSim2017)||(TRMCFFKEY.SimulationType==kTrSim2010Renewed) ) cluster->GenSimClusters3();
    cluster->clearstatus(0x20);
    cluster->clearstatus(0x40);
    int tkid = cluster->GetTkId();
    TrLadMap[tkid].push_back(cluster);
  }

  // loop on ladders
  for (TrLadMapIter=TrLadMap.begin(); TrLadMapIter!=TrLadMap.end(); TrLadMapIter++) {

    // create list of steps belonging to the same track
    map<int,vector<TrMCClusterR*> > TrTrkIdMap;
    map<int,vector<TrMCClusterR*> >::iterator TrTrkIdMapIter;
    vector<TrMCClusterR*>& cluster_list = TrLadMapIter->second;
    for(unsigned int jj=0; jj<cluster_list.size(); jj++) {
      TrMCClusterR* cluster = (TrMCClusterR*) cluster_list.at(jj);
      if (!cluster) {
        printf("TrSim::MergeMCCluster3-W found an empty pointer in the AMSTrMCCluster container. Jump!");
        continue;
      }
      int trkid = cluster->_gtrkid;
      if ( (trkid<0)||                                                             // exclude indirect association trkid from map 
           ( (cluster->_itra<0)&&(cluster->GetKn()<TRMCFFKEY.MaxSecondaryKn) ) ) { // exclude low energy secondaries (they can travel long distances inside the silicon)
        local_storage1.push_back(new TrMCClusterR(*cluster));
        continue; 
      }
      TrTrkIdMap[trkid].push_back(cluster);
    }

    // merge steps of the same track  
    for (TrTrkIdMapIter=TrTrkIdMap.begin(); TrTrkIdMapIter!=TrTrkIdMap.end(); TrTrkIdMapIter++) {
      vector<TrMCClusterR*>& cluster_list = TrTrkIdMapIter->second;
      int size = cluster_list.size();
      if (size==1) { // only one
        local_storage1.push_back(new TrMCClusterR(*(cluster_list.at(0))));
      } 
      else { // more than one
        TrMCClusterR* cluster = (TrMCClusterR*) cluster_list.at(0);
        cluster->setstatus(0x20);
        local_storage4.push_back(new TrMCClusterR(*cluster)); 
        float    mom   = cluster->_mom;
        AMSDir   dir   = cluster->GetDir();
        AMSPoint head  = cluster->GetStartPoint();
        AMSPoint tail  = cluster_list.at(size-1)->GetEndPoint();
        AMSPoint maxp  = head;
        AMSPoint minp  = tail;
        double   edep  = cluster->_edep;
        double   zstep = cluster->GetEndPoint().z() - cluster->GetStartPoint().z();
        for (int jj=1; jj<size; jj++) {
          TrMCClusterR* another = (TrMCClusterR*) cluster_list.at(jj);
          another->setstatus(0x20);
          local_storage4.push_back(new TrMCClusterR(*another)); 
          // max momentum 
          if (another->_mom>mom) mom = another->_mom;
          // start/end
          zstep += another->GetEndPoint().z() - another->GetStartPoint().z(); // accumulate zstep
          if (another->GetStartPoint().z()>maxp.z()) maxp = another->GetStartPoint();
          if (another->GetEndPoint().z()  >maxp.z()) maxp = another->GetEndPoint();
          if (another->GetStartPoint().z()<minp.z()) minp = another->GetStartPoint();
          if (another->GetEndPoint().z()  <minp.z()) minp = another->GetEndPoint();
          // sum together
          edep += another->_edep;
          if ( (TRMCFFKEY.SimulationType==kTrSim2017)||(TRMCFFKEY.SimulationType==kTrSim2010Renewed) ) cluster->AddSimCluster(*another);
        }
        // recalculate xgl, direction and step 
        if      (zstep<0) { head = maxp; tail = minp; } // Downward
        else if (zstep>0) { head = minp; tail = maxp; } // Upward
        float      step = (tail-head).norm();
        if(step>0) dir  = (tail-head)/step; // bug fixed by QY(G4step could be 0)
        AMSPoint   xgl  = (tail+head)/2;
        // add the merged cluster to the list of new MC clusters  
        TrMCClusterR* clone = new TrMCClusterR(*cluster);
        clone->clearstatus(0x20);
        clone->setstatus(0x40);
        clone->_xgl  = xgl; 
        clone->_dir  = dir;
        clone->_mom  = mom; 
        clone->_step = step; 
        clone->_edep = edep;
        local_storage1.push_back(clone);
      } 
    }

    // reorder by momentum 
    sort(local_storage1.begin(),local_storage1.end(),sort_with_momentum);

    // add up  
    for (int jj=0; jj<(int)local_storage1.size(); jj++) {
      TrMCClusterR* primary = (TrMCClusterR*) local_storage1.at(jj);
      if (primary->checkstatus(0x20)) continue;
      for (int ii=jj+1; ii<(int)local_storage1.size(); ii++) {
        TrMCClusterR* secondary = (TrMCClusterR*) local_storage1.at(ii); 
        if (secondary->checkstatus(0x20)) continue;  
        AMSPoint dist = primary->GetXgl() - secondary->GetXgl();
        // close enough
        if (sqrt(fabs(dist[0]*dist[0]+dist[1]*dist[1]))<TRMCFFKEY.MergeD[2]) {
          secondary->setstatus(0x20);
          local_storage3.push_back(secondary);
        }
      }
      if (local_storage3.empty()) local_storage2.push_back(new TrMCClusterR(*primary));
      else { 
        primary->setstatus(0x20);
        local_storage4.push_back(new TrMCClusterR(*primary)); 
        // add these cluster to the primary (gtrkid, idsoft, itra, dir e mom no change)
        // use a shift based on weighted sum of energy deposition (crude approx. not modifing z and direction)
        TrMCClusterR* clone = new TrMCClusterR(*primary);
        clone->clearstatus(0x20);
        clone->setstatus(0x40); 
        double x = clone->_xgl[0]*clone->_edep;
        double y = clone->_xgl[1]*clone->_edep;
        double edep = clone->_edep; 
        for (int ii=0; ii<(int)local_storage3.size(); ii++) {
          TrMCClusterR* secondary = (TrMCClusterR*) local_storage3.at(ii);
          x += secondary->_xgl[0]*secondary->_edep;
          y += secondary->_xgl[1]*secondary->_edep;
          edep += secondary->_edep;
          if ( (TRMCFFKEY.SimulationType==kTrSim2017)||(TRMCFFKEY.SimulationType==kTrSim2010Renewed) ) clone->AddSimCluster(*secondary); 
          local_storage4.push_back(new TrMCClusterR(*secondary));
        }
        if (edep<=0) { 
          x = clone->_xgl[0]; 
          y = clone->_xgl[1]; 
        }
        else { 
          x /= edep; 
          y /= edep; 
        }
        if ( (int(TRMCFFKEY.MergeMCCluster/100)%10)==0) { // disable weighted sum of energy deposition
          clone->_xgl[0] = x;
          clone->_xgl[1] = y;
        }
        clone->_edep = edep;    
        local_storage2.push_back(new TrMCClusterR(*clone));  
      } 
      // clean-up pointers 
      local_storage3.clear();
    }
    // clean-up 
    for (int jj=0; jj<(int)local_storage1.size(); jj++) if (local_storage1.at(jj)) delete local_storage1.at(jj);
    local_storage1.clear();
    TrTrkIdMap.clear();
  }

  // reorder by track id
  sort(local_storage2.begin(),local_storage2.end(),sort_with_gtrkid);

  // redo container
  container->eraseC();
  for (int ii=0; ii<(int)local_storage2.size(); ii++) {
#ifdef __ROOTSHAREDLIBRARY__
    container->addnext(new TrMCClusterR(*local_storage2.at(ii)));
#else
    container->addnext(new AMSTrMCCluster(*local_storage2.at(ii)));
#endif
  }
  if ((int(TRMCFFKEY.MergeMCCluster/10000)%10)==1) { 
    for (int ii=0; ii<(int)local_storage4.size(); ii++) {
#ifdef __ROOTSHAREDLIBRARY__
      container->addnext(new TrMCClusterR(*local_storage4.at(ii)));
#else
      container->addnext(new AMSTrMCCluster(*local_storage4.at(ii)));
#endif
    }
  }
  if (container) delete container;

  // clean-up
  for (int jj=0; jj<(int)local_storage2.size(); jj++) if (local_storage2.at(jj)) delete local_storage2.at(jj);
  local_storage2.clear();
  for (int jj=0; jj<(int)local_storage4.size(); jj++) if (local_storage4.at(jj)) delete local_storage4.at(jj);
  local_storage4.clear(); 
  TrLadMap.clear();
}


void TrSim::MergeMCCluster4() {

  // get container 
  VCon* container = GetVCon()->GetCont("AMSTrMCCluster");
  if (container==0) {
    if (WARNING) printf("TrSim::MergeMCCluster4-W no TrMCCluster container, skip.\n");
    return;
  }

  // check container
  int clen = container->getnelem();
  if (clen==0) {
    if (WARNING) printf("TrSim::MergeMCCluster4-W TrMCCluster container is empty, skip.\n");
    if (container!=0) delete container;
    return;
  }

  // local storage of new clusters
  vector<TrMCClusterR*> local_storage1;
  vector<TrMCClusterR*> local_storage2;
  vector<TrMCClusterR*> local_storage3;

  // create a map with key tkid of [clusters,isused]
  map<int,vector<TrMCClusterR*> > TrLadMap;
  map<int,vector<TrMCClusterR*> >::iterator TrLadMapIter;
  for (int jj=0; jj<clen; jj++) {
    TrMCClusterR* cluster = (TrMCClusterR*) container->getelem(jj);
    int tkid = cluster->GetTkId();
    TrLadMap[tkid].push_back(cluster);
  }

  // loop on ladders
  for (TrLadMapIter=TrLadMap.begin(); TrLadMapIter!=TrLadMap.end(); TrLadMapIter++) {

    // create list of steps belonging to the same track
    map<int,vector<TrMCClusterR*> > TrTrkIdMap;
    map<int,vector<TrMCClusterR*> >::iterator TrTrkIdMapIter;
    vector<TrMCClusterR*>& cluster_list = TrLadMapIter->second;
    for(unsigned int jj=0; jj<cluster_list.size(); jj++) {
      TrMCClusterR* cluster = (TrMCClusterR*) cluster_list.at(jj);
      if (!cluster) {
        printf("TrSim::MergeMCCluster3-W found an empty pointer in the AMSTrMCCluster container. Jump!");
        continue;
      }
      int trkid = cluster->_gtrkid;
      TrTrkIdMap[trkid].push_back(cluster);
    }

    // merge steps of the same track  
    for (TrTrkIdMapIter=TrTrkIdMap.begin(); TrTrkIdMapIter!=TrTrkIdMap.end(); TrTrkIdMapIter++) {
      vector<TrMCClusterR*>& cluster_list = TrTrkIdMapIter->second;
      int size = cluster_list.size();
      if (size==1) { // only one
        local_storage1.push_back(new TrMCClusterR(*(cluster_list.at(0))));
      }
      else { // more than one
        // properties
        int       gtrkid = cluster_list.at(0)->_gtrkid;
        int       idsoft = cluster_list.at(0)->_idsoft;
        short int itra   = cluster_list.at(0)->_itra;
        float     mom    = cluster_list.at(0)->_mom;
        int       status = cluster_list.at(0)->Status;
        AMSDir    dir    = cluster_list.at(0)->GetDir();
        AMSPoint  head   = cluster_list.at(0)->GetStartPoint();
        AMSPoint  tail   = cluster_list.at(size-1)->GetEndPoint();
        AMSPoint  maxp=head;
        AMSPoint  minp=tail;
        double zstep=0;
        for (int jj=0; jj<size; jj++) {
          TrMCClusterR* cluster = (TrMCClusterR*) cluster_list.at(jj);
//-----bug fixed by QY
          if (cluster->_mom>mom) {//largest mom
            mom  = cluster->_mom;
          }
          zstep+=cluster->GetEndPoint().z()-cluster->GetStartPoint().z();//accumulate zstep
          if (cluster->GetStartPoint().z()>maxp.z()){maxp=cluster->GetStartPoint();}
          if (cluster->GetEndPoint().z()>maxp.z())  {maxp=cluster->GetEndPoint();}
          if (cluster->GetStartPoint().z()<minp.z()){minp=cluster->GetStartPoint();}
          if (cluster->GetEndPoint().z()<minp.z())  {minp=cluster->GetEndPoint();}
//---------
        }
        // recalculate xgl, direction and step
        if     (zstep<0){head=maxp; tail=minp;}//Downward
        else if(zstep>0){head=minp; tail=maxp;}//Upward
        float     step   = (tail-head).norm();
        if(step>0)dir    = (tail-head)/step;//bug fixed by QY(G4step could be 0)
        AMSPoint  xgl    = (tail+head)/2;
        // sum edep 
        double   edep = 0;
        for (int jj=0; jj<size; jj++) {
          TrMCClusterR* cluster = (TrMCClusterR*) cluster_list.at(jj);
          edep += cluster->_edep;
        }
        // create a new cluster and add to the list of new MC clusters  
        local_storage1.push_back(new TrMCClusterR(idsoft,step,xgl,dir,mom,edep,itra,gtrkid,status));
      } 
    }

    // reorder by momentum 
    sort(local_storage1.begin(),local_storage1.end(),sort_pzm);
    // add up  
    for (int jj=0; jj<(int)local_storage1.size(); jj++) {
      if(TRMCFFKEY.MergeD[3]>=0 && jj>=TRMCFFKEY.MergeD[3])break;
      TrMCClusterR* primary = (TrMCClusterR*) local_storage1.at(jj);
      if (primary->_edep<=0) continue;
      if(TRMCFFKEY.SimulationType==kTrSim2017)primary->GenSimClusters3();//QY
      for (int ii=jj+1; ii<(int)local_storage1.size(); ii++) {
        TrMCClusterR* secondary = (TrMCClusterR*) local_storage1.at(ii); 
        if (secondary->_edep<=0) continue;
        AMSPoint dist = primary->GetXgl() - secondary->GetXgl();
        // close enough
        if(TRMCFFKEY.SimulationType==kTrSim2017){//QY
          if (TRMCFFKEY.MergeD[2]>0){
            if (sqrt(dist[0]*dist[0]+dist[1]*dist[1])<TRMCFFKEY.MergeD[2]){
              local_storage3.push_back(secondary);
            }
          }
          else {
            if (fabs(dist[0])<TRMCFFKEY.MergeD[0] && fabs(dist[1])<TRMCFFKEY.MergeD[1]){
             local_storage3.push_back(secondary);
            }
          }
        }
        else { 
          if(primary->_itra>=0){
            if (sqrt(dist[0]*dist[0]+dist[1]*dist[1])<TRMCFFKEY.MergeD[2]){
             local_storage3.push_back(secondary);
            //cout <<" primary merged with "<<primary->_itra<<" "<<secondary->_itra<<endl;
           }
         }
         else{
           if (fabs(dist[0])<TRMCFFKEY.MergeD[0] && fabs(dist[1])<TRMCFFKEY.MergeD[1]){
              local_storage3.push_back(secondary);
            //cout <<" 2ndry merged with "<<primary->_itra<<" "<<secondary->_itra<<endl;
          }
         }
        }
      }
      // add these cluster to the primary (gtrkid, idsoft, itra, dir e mom no change)
      // use a shift based on weighted sum of energy deposition (crude approx. not modifing z and direction)
      double x = primary->_xgl[0]*primary->_edep;
      double y = primary->_xgl[1]*primary->_edep;
      double edep = primary->_edep; 
      for (int ii=0; ii<(int)local_storage3.size(); ii++) {
        TrMCClusterR* secondary = (TrMCClusterR*) local_storage3.at(ii);
        x += secondary->_xgl[0]*secondary->_edep;
        y += secondary->_xgl[1]*secondary->_edep;
        edep += secondary->_edep;
        if(TRMCFFKEY.SimulationType==kTrSim2017){//QY
           secondary->GenSimClusters3();//MakeCluster first
           primary->AddSimCluster(*secondary);//linear added
        }
        secondary->_edep = 0; // secondary is used!
      }
      if (edep<=0) {
        x = primary->_xgl[0];
        y = primary->_xgl[1];
      }
      else { 
        x /= edep;
        y /= edep;
      }
      primary->_xgl[0] = x;
      primary->_xgl[1] = y;
      primary->_edep = edep;
      local_storage2.push_back(new TrMCClusterR(*primary));  
      primary->_edep = 0;
      // clean-up pointers 
      local_storage3.clear();
    }

    // clean-up 
    for (int jj=0; jj<(int)local_storage1.size(); jj++) if (local_storage1.at(jj)) delete local_storage1.at(jj);
    local_storage1.clear();
    TrTrkIdMap.clear();
  }

  // reorder by track id
  sort(local_storage2.begin(),local_storage2.end(),sort_with_gtrkid);

  // redo container
  if(local_storage2.size()>0) {
    container->eraseC();
    for(unsigned int ii=0;ii<local_storage2.size();ii++) {
#ifdef __ROOTSHAREDLIBRARY__
      container->addnext( new TrMCClusterR(*local_storage2[ii]) );
#else
      container->addnext( new AMSTrMCCluster(*local_storage2[ii]) );
#endif
    }
  }
  if (container!=0) delete container;

  // clean-up
  for (int jj=0; jj<(int)local_storage2.size(); jj++) if (local_storage2.at(jj)) delete local_storage2.at(jj);
  local_storage2.clear();
  TrLadMap.clear();
}


void TrSim::MergeMCCluster5() {

  // get container 
  VCon* container = GetVCon()->GetCont("AMSTrMCCluster");
  if (container==0) {
    if (WARNING) printf("TrSim::MergeMCCluster3-W no TrMCCluster container, skip.\n");
    return;
  }

  // check container
  int clen = container->getnelem();
  if (clen==0) {
    if (WARNING) printf("TrSim::MergeMCCluster3-W TrMCCluster container is empty, skip.\n");
    if (container!=0) delete container;
    return;
  }

  // local storage of new clusters
  vector<TrMCClusterR*> local_storage1;
  vector<TrMCClusterR*> local_storage2;
  vector<TrMCClusterR*> local_storage3;

  // create a map with key tkid of [clusters,isused]
  map<int,vector<TrMCClusterR*> > TrLadMap;
  map<int,vector<TrMCClusterR*> >::iterator TrLadMapIter;
  for (int jj=0; jj<clen; jj++) {
    TrMCClusterR* cluster = (TrMCClusterR*) container->getelem(jj);
    int tkid = cluster->GetTkId();
    TrLadMap[tkid].push_back(cluster);
  }

  // loop on ladders
  for (TrLadMapIter=TrLadMap.begin(); TrLadMapIter!=TrLadMap.end(); TrLadMapIter++) {

    // create list of steps belonging to the same track
    map<int,vector<TrMCClusterR*> > TrTrkIdMap;
    map<int,vector<TrMCClusterR*> >::iterator TrTrkIdMapIter;
    vector<TrMCClusterR*>& cluster_list = TrLadMapIter->second;
    for(unsigned int jj=0; jj<cluster_list.size(); jj++) {
      TrMCClusterR* cluster = (TrMCClusterR*) cluster_list.at(jj);
      if (!cluster) {
        printf("TrSim::MergeMCCluster3-W found an empty pointer in the AMSTrMCCluster container. Jump!");
        continue;
      }
      int trkid = cluster->_gtrkid;
      TrTrkIdMap[trkid].push_back(cluster);
    }

    // merge steps of the same track  
    for (TrTrkIdMapIter=TrTrkIdMap.begin(); TrTrkIdMapIter!=TrTrkIdMap.end(); TrTrkIdMapIter++) {
      vector<TrMCClusterR*>& cluster_list = TrTrkIdMapIter->second;
      int size = cluster_list.size();
      if (size==1) { // only one
        local_storage1.push_back(new TrMCClusterR(*(cluster_list.at(0))));
      }
      else { // more than one
        // properties
        int       gtrkid = cluster_list.at(0)->_gtrkid;
        int       idsoft = cluster_list.at(0)->_idsoft;
        short int itra   = cluster_list.at(0)->_itra;
        float     mom    = cluster_list.at(0)->_mom;
        int       status = cluster_list.at(0)->Status;
        AMSDir    dir    = cluster_list.at(0)->GetDir();
        AMSPoint  head   = cluster_list.at(0)->GetStartPoint();
        AMSPoint  tail   = cluster_list.at(size-1)->GetEndPoint();
        AMSPoint  maxp=head;
        AMSPoint  minp=tail;
        double zstep=0;
        for (int jj=0; jj<size; jj++) {
          TrMCClusterR* cluster = (TrMCClusterR*) cluster_list.at(jj);
//-----bug fixed by QY
          if (cluster->_mom>mom) {//largest mom
            mom  = cluster->_mom;
          }
          zstep+=cluster->GetEndPoint().z()-cluster->GetStartPoint().z();//accumulate zstep
          if (cluster->GetStartPoint().z()>maxp.z()){maxp=cluster->GetStartPoint();}
          if (cluster->GetEndPoint().z()>maxp.z())  {maxp=cluster->GetEndPoint();}
          if (cluster->GetStartPoint().z()<minp.z()){minp=cluster->GetStartPoint();}
          if (cluster->GetEndPoint().z()<minp.z())  {minp=cluster->GetEndPoint();}
//-------
        }
        // recalculate xgl, direction and step
        if     (zstep<0){head=maxp; tail=minp;}//Downward
        else if(zstep>0){head=minp; tail=maxp;}//Upward
        float     step   = (tail-head).norm();
        if(step>0)dir    = (tail-head)/step;//bug fixed by QY(G4step could be 0)
        AMSPoint  xgl    = (tail+head)/2;
        // sum edep 
        double   edep = 0;
        for (int jj=0; jj<size; jj++) {
          TrMCClusterR* cluster = (TrMCClusterR*) cluster_list.at(jj);
          edep += cluster->_edep;
        }
        // create a new cluster and add to the list of new MC clusters  
        local_storage1.push_back(new TrMCClusterR(idsoft,step,xgl,dir,mom,edep,itra,gtrkid,status));
      } 
    }

    // reorder by momentum 
    sort(local_storage1.begin(),local_storage1.end(),sort_pzm);

    // add up  
    for (int jj=0; jj<(int)local_storage1.size(); jj++) {
      if(TRMCFFKEY.MergeD[3]>=0 && jj>=TRMCFFKEY.MergeD[3])break;
      TrMCClusterR* primary = (TrMCClusterR*) local_storage1.at(jj);
      if (primary->_edep<=0) continue;
      for (int ii=jj+1; ii<(int)local_storage1.size(); ii++) {
        TrMCClusterR* secondary = (TrMCClusterR*) local_storage1.at(ii); 
        if (secondary->_edep<=0) continue;      
        AMSPoint dist = primary->GetXgl() - secondary->GetXgl();
        // close enough
          if (sqrt(dist[0]*dist[0]+dist[1]*dist[1])<TRMCFFKEY.MergeD[2]){
           bool real_primary=(primary->_itra>=0);
           if(secondary->effch()<primary->getZ() && secondary->_itra<0)secondary->effch()=primary->getZ();
           else if(primary->effch()<secondary->getZ() && primary->_itra<0)primary->effch()=secondary->getZ();
           bool hc1=abs(primary->_itra)>50 ||abs(primary->_itra)==47 ||abs(primary->_itra)==49;
           int icase=int(TRMCFFKEY.MergeD[4])%10;
           bool hc2=false;
           if(icase==2){
               hc2=abs(secondary->_itra)>50 ||abs(secondary->_itra)==47 ||abs(secondary->_itra)==49;
           }
           else if(icase==1){
           hc2=(abs(secondary->_itra)>44 ||abs(secondary->_itra)==14);
           }
           else if(icase==0){
            hc2= (!(abs(secondary->_itra)==2 || abs(secondary->_itra)==3)) ;
           } 

           if(!hc1 || !hc2 || real_primary){
             local_storage3.push_back(secondary);
             if(int(TRMCFFKEY.MergeD[4])/100)cout <<"  merged with "<<primary->_itra<<" "<<secondary->_itra<<endl;
           }
          }
      }
      // add these cluster to the primary (gtrkid, idsoft, itra, dir e mom no change)
      // use a shift based on weighted sum of energy deposition (crude approx. not modifing z and direction)
      double x = primary->_xgl[0]*primary->_edep;
      double y = primary->_xgl[1]*primary->_edep;
      double edep = primary->_edep; 
      for (int ii=0; ii<(int)local_storage3.size(); ii++) {
        TrMCClusterR* secondary = (TrMCClusterR*) local_storage3.at(ii);
        x += secondary->_xgl[0]*secondary->_edep;
        y += secondary->_xgl[1]*secondary->_edep;
        edep += secondary->_edep;
        secondary->_edep = 0; // secondary is used!
      }
      if (edep<=0) {
        x = primary->_xgl[0];
        y = primary->_xgl[1];
      }
      else { 
        x /= edep;
        y /= edep;
      }
      primary->_xgl[0] = x;
      primary->_xgl[1] = y;
      primary->_edep = edep;
      local_storage2.push_back(new TrMCClusterR(*primary));  
      primary->_edep = 0;
      // clean-up pointers 
      local_storage3.clear();
    }

    // clean-up 
    for (int jj=0; jj<(int)local_storage1.size(); jj++) if (local_storage1.at(jj)) delete local_storage1.at(jj);
    local_storage1.clear();
    TrTrkIdMap.clear();
  }

  // reorder by track id
  sort(local_storage2.begin(),local_storage2.end(),sort_with_gtrkid);

  // redo container
  if(local_storage2.size()>0) {
    container->eraseC();
    for(unsigned int ii=0;ii<local_storage2.size();ii++) {
#ifdef __ROOTSHAREDLIBRARY__
      container->addnext( new TrMCClusterR(*local_storage2[ii]) );
#else
      container->addnext( new AMSTrMCCluster(*local_storage2[ii]) );
#endif
    }
  }
  if (container!=0) delete container;

  // clean-up
  for (int jj=0; jj<(int)local_storage2.size(); jj++) if (local_storage2.at(jj)) delete local_storage2.at(jj);
  local_storage2.clear();
  TrLadMap.clear();
}

void TrSim::MergeMCClusterSameTrkId() {
  // get container 
  VCon* container = GetVCon()->GetCont("AMSTrMCCluster");
  if (container==0) {
    if (WARNING) printf("TrSim::MergeMCClusterSameTrkId-W no TrMCCluster container, skip.\n");
    return;
  }
  // check container
  int clen = container->getnelem();
  if (clen==0) {
    if (WARNING) printf("TrSim::MergeMCClusterSameTrkId-W TrMCCluster container is empty, skip.\n");
    if (container!=0) delete container;
    return;
  }
  // local storage of new clusters
  vector<TrMCClusterR*> local_storage;
  // create a ladder map (key is tkid)   
  map<int,vector<TrMCClusterR*> > TrLadMap;
  map<int,vector<TrMCClusterR*> >::iterator TrLadMapIter;
  for (int jj=0; jj<clen; jj++) {
    TrMCClusterR* cluster = (TrMCClusterR*) container->getelem(jj);
    int tkid = cluster->GetTkId();
    TrLadMap[tkid].push_back(cluster);
  }
  // loop on ladders
  for (TrLadMapIter=TrLadMap.begin(); TrLadMapIter!=TrLadMap.end(); TrLadMapIter++) {
    // create a map lof steps belonging to the same track (key is track id)
    map<int,vector<TrMCClusterR*> > TrTrkIdMap;
    map<int,vector<TrMCClusterR*> >::iterator TrTrkIdMapIter;
    vector<TrMCClusterR*>& cluster_list = TrLadMapIter->second;
    for(unsigned int jj=0; jj<cluster_list.size(); jj++) {
      TrMCClusterR* cluster = (TrMCClusterR*) cluster_list.at(jj);
      if (!cluster) {
        printf("TrSim::MergeMCClusterSameTrkId-W found an empty pointer in the AMSTrMCCluster container. Jump!");
        continue;
      }
      int trkid = cluster->_gtrkid;
      TrTrkIdMap[trkid].push_back(cluster);
    }
    // merge steps of the same track  
    for (TrTrkIdMapIter=TrTrkIdMap.begin(); TrTrkIdMapIter!=TrTrkIdMap.end(); TrTrkIdMapIter++) {
      vector<TrMCClusterR*>& cluster_list = TrTrkIdMapIter->second;
      int size = cluster_list.size();
      if (size==1) { // only one
        local_storage.push_back(new TrMCClusterR(*(cluster_list.at(0))));
      }
      else { // more than one
        // properties
        int       gtrkid = cluster_list.at(0)->_gtrkid;
        int       idsoft = cluster_list.at(0)->_idsoft;
        short int itra   = cluster_list.at(0)->_itra;
        float     mom    = cluster_list.at(0)->_mom;
        int       status = cluster_list.at(0)->Status;
        AMSDir    dir    = cluster_list.at(0)->GetDir();
        AMSPoint  head   = cluster_list.at(0)->GetStartPoint();
        AMSPoint  tail   = cluster_list.at(size-1)->GetEndPoint();
        AMSPoint  maxp=head;
        AMSPoint  minp=tail;
        double zstep=0;
        for (int jj=0; jj<size; jj++) {
          TrMCClusterR* cluster = (TrMCClusterR*) cluster_list.at(jj);
            if (cluster->_mom>mom) {//largest mom
            mom  = cluster->_mom;
          }
          zstep += cluster->GetEndPoint().z()-cluster->GetStartPoint().z(); // accumulate zstep
          if (cluster->GetStartPoint().z()>maxp.z()) { maxp=cluster->GetStartPoint(); }
          if (cluster->GetEndPoint().z()  >maxp.z()) { maxp=cluster->GetEndPoint();   }
          if (cluster->GetStartPoint().z()<minp.z()) { minp=cluster->GetStartPoint(); }
          if (cluster->GetEndPoint().z()  <minp.z()) { minp=cluster->GetEndPoint();   }
        }
        // recalculate xgl, direction and step
        if      (zstep<0) { head = maxp; tail = minp; } // downward
        else if (zstep>0) { head = minp; tail = maxp; } // upward
        float      step  = (tail-head).norm();
        if(step>0) dir   = (tail-head)/step;
        AMSPoint   xgl   = (tail+head)/2;
        // sum edep 
        double edep = 0;
        for (int jj=0; jj<size; jj++) {
          TrMCClusterR* cluster = (TrMCClusterR*) cluster_list.at(jj);
          edep += cluster->_edep;
        }
        // create a new cluster and add to the list of new MC clusters  
        local_storage.push_back(new TrMCClusterR(idsoft,step,xgl,dir,mom,edep,itra,gtrkid,status));
      }
    }
  }
  // reorder by track id
  sort(local_storage.begin(),local_storage.end(),sort_with_gtrkid);
  // redo container
  if (local_storage.size()>0) {
    container->eraseC();
    for (unsigned int ii=0; ii<local_storage.size(); ii++) {
#ifdef __ROOTSHAREDLIBRARY__
      container->addnext(new TrMCClusterR(*local_storage[ii]));
#else
      container->addnext(new AMSTrMCCluster(*local_storage[ii]));
#endif
    }
  }
  if (container!=0) delete container;
  // clean-up
  for (int jj=0; jj<(int)local_storage.size(); jj++) if (local_storage.at(jj)) delete local_storage.at(jj);
  local_storage.clear();
  TrLadMap.clear();
} 

