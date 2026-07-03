///  $Id: TrMCCluster.C,v 1.87 2022/05/07 23:06:12 qyan Exp $

//////////////////////////////////////////////////////////////////////////
///
///\file  TrMCCluster.C
///\brief Source file of AMSTrMCCluster class
///
///\date  2008/02/14 SH  First import from Gbatch
///\date  2008/03/17 SH  Compatible with new TkDBc and TkCoo
///\date  2008/04/02 SH  Compatible with new TkDBc and TkSens
///$Date: 2022/05/07 23:06:12 $
///
///$Revision: 1.87 $
///
//////////////////////////////////////////////////////////////////////////

#include "TkDBc.h"
#include "TkCoo.h"
#include "TkSens.h"
#include "tkdcards.h"
#include "TrLinearDB.h"
#include "TrGainDB.h"
#include "TrParDB.h" 
#include "geant321.h"
#include "TrMCCluster.h"

#include "TString.h"
#include "TrSimSensor.h"
#include "TrSimCluster.h"
#include "TrSim.h"
#include "TrParDB.h"
#include "HistoMan.h"

#include "root.h"

#include <cmath>
#include <iostream>
#include <cstdlib>
#include <vector>
#include <string>

#define VERBOSE 0 
#define WARNING 0

extern "C" double rnormx();
 

ClassImp(TrMCClusterR);




double qlinfun2(double X, double k);
double gain_to_gain(double* x, double* par);


int TrMCClusterR::_NoiseMarker(555);


TrMCClusterR::TrMCClusterR(int idsoft, float step, AMSPoint xgl, AMSPoint dir, float mom, float edep, int itra, integer gtrkid, int status)
  : _idsoft(idsoft), _itra(itra), _gtrkid(gtrkid), _step(step), _xgl(xgl), _dir(dir), _mom(mom), _edep(edep), Status(status) {
  Init();
  _effch=Status&0x3f;
}


TrMCClusterR::TrMCClusterR(AMSPoint xgl, integer itra, geant edep):
  _idsoft(0), _itra(itra), _step(0), _xgl(xgl), _dir(0,0,0), _mom(0), _edep(edep) {
  Init();
#ifndef __ROOTSHAREDLIBRARY__
    char chp[22]="";
    integer itrtyp;
    geant mass;
    geant charge;
    geant tlife;
    geant ub[1];
    integer nwb=0;
   
    if(abs(_itra)>=10000)_effch=int(abs(_itra)/100)%100; 
    else                 {GFPART(abs(_itra),chp,itrtyp,mass,charge,tlife,ub,nwb);_effch=fabs(charge);}
#else
        if(abs(_itra)>50)_effch=3;
        else if(abs(_itra)==47 ||abs(_itra)==49)_effch=2;
        else _effch=1;
#endif

  TkSens pp(_xgl,1);
  if(pp.LadFound()){
    int tkid = pp.GetLadTkID();
    int side = (tkid>0) ? 1 : 0;
    _idsoft = abs(tkid) + 1000*side + 10000*pp.GetSensor();
  } 
  Status = 0;
  _gtrkid = -2;
}


TrMCClusterR& TrMCClusterR::operator=(const TrMCClusterR& that) {
  if (this!=&that) {
    Copy(that);
  } 
  return *this;
}


void TrMCClusterR::Init() {
  for(int ii=0; ii<2; ii++){
    _simcl[ii] = 0;
  }
}


void TrMCClusterR::Clear() { 
  for(int ii=0; ii<2; ii++){
    if (_simcl[ii]!=0) { 
      delete _simcl[ii];
    }
    _simcl[ii] = 0; 
  }
}


void TrMCClusterR::Copy(const TrMCClusterR& that) {
  _idsoft = that._idsoft;
  _step = that._step;
  _itra = that._itra;
  _effch=that._effch;
  _xgl = that._xgl;
  _dir = that._dir;
  _mom = that._mom;
  _edep =that._edep;
  _gtrkid = that._gtrkid;
  Status = that.Status;
  for(int ii=0; ii<2; ii++){
    if (that._simcl[ii]!=0) _simcl[ii] = new TrSimCluster(*that._simcl[ii]);
    else                    _simcl[ii] = 0;
  }
}

TrMCClusterR& TrMCClusterR::operator+=(const TrMCClusterR& that){
static int mp=0;
  if(_idsoft!= that._idsoft){
    if(mp++<100)printf("TrMCClusterR::operator+= Error: cannot sum MCcluster on different sensors\n Sum not performed\n");
    else if(mp==101)printf("TrMCClusterR::operator+= Error: cannot sum MCcluster on different sensors\n Sum not performed\n Last Message\n");
    return *this;
  }
  _step += that._step;
  //  _itra = that._itra; keep the original one
  //  _xgl = ( _xgl*_edep + that._xgl*that._edep )/(_edep+that._edep);
  _xgl = ( _xgl+ that._xgl )/2;
  //  _dir = that._dir; keep the orginal one
  //  _mom = that._mom; keep the original one
  _edep +=that._edep;
  // Status |= that.Status; // keep the original one
  for(int ii=0; ii<2; ii++)
    if (that._simcl[ii]!=0) delete _simcl[ii];
  return *this;
}


int TrMCClusterR::GetTkId(){ 
  // int sensor = abs(_idsoft)/10000;
  int tkid   = abs(_idsoft)%1000;
  int ss     = abs(_idsoft)%10000-tkid;
  if(!ss) tkid*=-1;
  return tkid;
}

#ifdef __ROOTSHAREDLIBRARY__
#include <map>
#include <utility>
static double _me  = 0.5109989461e-3; 
static double _mmu = 0.1056583745;
static double _mpi0 = 0.1349766;
static double _mpi = 0.13957018;  
static double _mk0 = 0.497611;
static double _mk = 0.493677;
static double _mn = 0.9395654133;
static double _mp = 0.9382720813;
static int _some_z[16] = {0,1,-1,0,1,-1,0,+1,-1,0,+1,-1,0,1,-1,-1};
static double _some_m[16] = {0,_me,_me,0,_mmu,_mmu,_mpi0,_mpi,_mpi,_mk0,_mk,_mk,_mn,_mp,_mp,_mk0};
static map<int, pair<int,double> > _pid_map; 
static map<int, pair<int,double> >::iterator _pid_map_it;
#endif 

void TrMCClusterR::GetChargeAndMass(int& Z, double& M) {
#ifndef __ROOTSHAREDLIBRARY__
  // approx
  if (abs(_itra)>=10000) {
    Z = int(abs(_itra)/100)%100;
    M = (int(abs(_itra))%100)*0.9314940954; // approx.
    if (abs(_itra)>=20000) Z *= -1;
  }
  // best available
  else {
    char chp[22] = "";
    geant charge;
    integer itrtyp;
    geant mass;
    geant tlife;
    geant ub[1];
    integer nwb = 0; 
    GFPART(abs(_itra),chp,itrtyp,mass,charge,tlife,ub,nwb); 
    Z = charge;
    M = mass; 
  }
#else
  // most frequent ones
  if ( (abs(_itra)>0)&&(abs(_itra)<=16) ) {      
    Z = _some_z[int(abs(_itra))-1]; 
    M = _some_m[int(abs(_itra))-1]; 
    return;
  }
  else if (abs(_itra)==45) { Z = 1; M = 1.875612859; return; }
  else if (abs(_itra)==46) { Z = 1; M = 2.808921005; return; }
  else if (abs(_itra)==47) { Z = 2; M = 3.727379240; return; }
  // others can be added (grep GSPART ../F/*)
  // but this on-the-flight storage is good enough
  _pid_map_it = _pid_map.find(int(abs(_itra))); 
  if (_pid_map_it!=_pid_map.end()) { 
    Z = _pid_map_it->second.first;
    M = _pid_map_it->second.second;
    return; 
  } 
  // search onto particle list
  bool found = false;
  for (unsigned int imc=0; imc<AMSEventR::Head()->NMCEventg(); imc++) {
    MCEventgR* part = (MCEventgR*) AMSEventR::Head()->pMCEventg(imc);
    // this itra 
    int this_itra = part->Particle;  
    if (abs(this_itra)>1000000000) {
      int A = int(abs(this_itra)/10)%1000;
      int Z = int(abs(this_itra)/10000)%1000;
      int a = (A>99) ? 99 : A;
      int z = (Z>99) ? 99 : Z;
      int s = (this_itra>0) ? 0 : 1; 
      this_itra = 10000 + s*10000 + z*100 + a;
    }
    if (this_itra!=abs(_itra)) continue; 
    Z = part->Charge;
    M = part->Mass;
    found = true;   
    if (int(_pid_map.size())<1000) _pid_map.insert(make_pair(int(abs(_itra)),make_pair(Z,M)));
    break; 
  }
  if (found) return; 
  // rude approx
  Z = Status&0x3f; // no sign available
  M = 2*Z*0.9314940954; 
  // better approx
  if (abs(_itra)>=10000) {
    Z = int(abs(_itra)/100)%100;
    M = (int(abs(_itra))%100)*0.9314940954; 
    if (abs(_itra)>=20000) Z *= -1;
  }
#endif
} 

double TrMCClusterR::GetRigidity() {
  int Z = 0;
  double M = 0;
  GetChargeAndMass(Z,M);
  return (fabs(Z)!=0) ? _mom/Z : 0;
}

double TrMCClusterR::GetKn() {
  int Z = 0;
  double M = 0;
  GetChargeAndMass(Z,M);
  double p = _mom;
  double k = sqrt(p*p + M*M) - M;
  int A = floor(M/0.932+0.5);
  if (A==0) A = 1;
  return k/A;
}

std::ostream& TrMCClusterR::putout(std::ostream &ostr ){
  return ostr << _PrepareOutput(1) << std::endl;
}


void TrMCClusterR::Print(int opt) { 
  cout << _PrepareOutput(opt);
}


const char* TrMCClusterR::Info(int iRef){
  string aa;
  aa.append(Form("TrMCCluster #%d ",iRef));
  aa.append(_PrepareOutput(0));
  unsigned int len=MAXINFOSIZE;
  if(aa.size()<len) len=aa.size();
  strncpy(_Info,aa.c_str(),len+1);
  return _Info;
}


std::string TrMCClusterR::_PrepareOutput(int full) {
  std::string sout;
  sout.append(
    Form("Part:%3d  Mom(GeV):%12.6f TkId:%+04d  Sens:%2d  Edep(keV):%9.3f  Step(um):%7.1f  X:%8.3f Y:%8.3f Z:%8.3f  Cx: %8.5f Cy: %8.5f Cz: %8.5f  TrkId:%6d  Status:%4X\n",
      _itra,
      GetMomentum(),GetTkId(),GetSensor(),GetEdep()*1e+6,GetStep()*1e+4,
      GetXgl().x(),GetXgl().y(),GetXgl().z(),
      GetDir().x(),GetDir().y(),GetDir().z(),
      _gtrkid,Status
    )
  );
  return sout;
}


void TrMCClusterR::GenSimClusters(){

  // montecarlo truth
  char     sidename[2] = {'x','y'};
  float    step = GetStep()*1.e4;     // step [um] 
  AMSPoint glo = GetXgl();            // coordinate [cm]
  AMSDir   dir = GetDir();            // direction 
  float    edep = GetEdep()*1.e6;     // energy deposition [keV] 
  unsigned int hcharge=0;
  if(abs(_itra)>=47) hcharge=1;
  if(abs(_itra)>=61) hcharge=2;
  if(TRMCFFKEY.MergeMCCluster==5 &&(int(TRMCFFKEY.MergeD[4])%100)/10 && _itra<0){
   double mass=0;
   if(abs(_itra)==14 || abs(_itra)==15)mass=0.93827;
   else if(abs(_itra)==45)mass=1.875613;
   else if(abs(_itra)==46)mass=2.80925;
   else if(abs(_itra)==47)mass=3.727417;
   else if(abs(_itra)==49)mass=2.80923;
   double velocity=mass?_mom/sqrt(_mom*_mom+mass*mass):1;
//   cout <<" velocity "<<velocity<<" "<<_mom<<endl;
   hcharge= (hcharge+0.5)/pow(velocity,0.66);
   if(hcharge>2)hcharge=2;
   if(_effch>2)hcharge=2;
   else if(_effch==2 && hcharge==0)hcharge=1;
  }
  if(TRMCFFKEY.MergeD[4]/100)cout <<" newcharge "<<_itra<<" "<<hcharge<<endl;
  if (edep<1) return;                 // if energy deposition < 1 keV 
  //  if (momentum<1e-6) return;          // if momentum < keV/c

  TkSens _glo2loc(1);
  _glo2loc.SetGlobal(GetTkId(),glo,dir);                                    // from global to local

  int  nsensor = _glo2loc.GetSensor();                                      // sensor number
  double height = _glo2loc.GetSensCoo().z()*1e+4;                           // position w/ the center of the silicon
  double ip[2] = {_glo2loc.GetSensCoo().x(),_glo2loc.GetSensCoo().y()};     // sensor impact point
  double ia[2] = {_glo2loc.GetImpactAngleXZ(),_glo2loc.GetImpactAngleYZ()}; // sensor impact angle
  double tip[2] = {_glo2loc.GetImpactPointX(),_glo2loc.GetImpactPointY()};   // true impact point [0,1]
  double coo[2] = {_glo2loc.GetLaddCoo().x(),_glo2loc.GetLaddCoo().y()};
  int imult = _glo2loc.GetMultIndex();

  if(tip[0]<0)tip[0]+=1;
  if(tip[1]<0)tip[1]+=1;
  // Print();
  if (VERBOSE) {
    printf("TrSim::GenSimClusters-V  tkid=%+4d   loc(x,y)=(%7.4f,%7.4f)   step(um)=%7.4f  theta(xz,yz)=(%7.4f,%7.4f)  edep(keV)=%7.2f  nsens=%2d  itra=%4d\n",
           GetTkId(),ip[0],ip[1],ia[0],ia[1],step,edep,nsensor,(int)abs(_itra));
//    printf("TrSim::GenSimClusters-V  laddcoo(x,y) = (%7.4f,%7.4f)   readout(x,y) = (%4d,%4d)   mult = %2d\n",
//           _glo2loc.GetLaddCoo().x(),_glo2loc.GetLaddCoo().y(),_glo2loc.GetStripX(),_glo2loc.GetStripY(),imult);
  }

  //                         p_x     p_y       He_x    He_y
  // moved to dcard  double SmearPos[2][2]={{0.0008,      0.},{0.0008,  0.0}};
  // loop on two sides of the ladder
  for (int iside=0; iside<2; iside++) {

    if(step<1) ia[iside]=0;
    // create the simulated cluster

    // SMEAR the position
    int hcharge_smear = (hcharge>1) ? 1 : hcharge; // ions = He
    float ipsmear=ip[iside];
    ipsmear=ip[iside]+rnormx()*TRMCFFKEY.SmearPos[hcharge_smear][iside];

    // // SMEAR outer layers
    // int lay = abs(GetTkId())/100;
    // if (lay == 8 || lay == 9)
    //  ipsmear+=rnormx()*TRMCFFKEY.OuterSmearing[lay-8][iside];

    // Create the cluster
    TrSimCluster simcluster = TrSim::GetTrSimSensor(iside,GetTkId())->MakeCluster(ipsmear,ia[iside],nsensor,step*dir[2],height);

    // from time to time the cluster is empty
    if (simcluster.GetWidth()==0) continue;

    // put the cluster in the TrMCCluster object
    if(_simcl[iside]) delete _simcl[iside];
    _simcl[iside] = new TrSimCluster(simcluster);
    // raw signal
//    hman.Fill(Form("TrSimSig%c",sidename[iside]),_simcl[iside]->GetEta(),_simcl[iside]->GetTotSignal());
    
    // simulation tuning parameter 1: gaussianize a fraction of the strip signal
    int hcharge_gauss = (hcharge>1) ? 1 : hcharge; // ions = He    
    // no extra-multiplication if too inclined or ions  
    if (fabs(ia[iside])<0.8)
      _simcl[iside]->GaussianizeFraction(iside,hcharge_gauss,TRMCFFKEY.TrSim2010_FracNoise[iside], tip[iside]);

    // p and He
    // - non-linear edep 
    if (hcharge<2) {
      // moved to datacard (PZ)
      // // Enegy smearing, scaling, and convert to ADC
      // //                          p_x p_y  He_x  He_y
      // double ADCMipValue[2][2]={ {44, 32},{46,    32.}};
      // double SigQuadLoss[2][2]={{0.0002,0.0004},{0.0001,0.00022}};
      double edep_c2=edep;
      if(iside==0) {
        double edep_c=qlinfun2(edep,TRMCFFKEY.SigQuadLoss[hcharge][iside]); // 
        edep_c2=TRMCFFKEY.ADCMipValue[hcharge][0]*edep_c/81;
      }
      else {
        double edep_c=edep*(1+rnormx()*0.15);
        edep_c=qlinfun2(edep_c,TRMCFFKEY.SigQuadLoss[hcharge][iside]);	
        edep_c2=TRMCFFKEY.ADCMipValue[hcharge][iside]*edep_c/81+edep_c/81*edep_c/81-4;
      }
      // if side Y some additional edep vs eta dependence
      // double pc[5]={0.8169,2.23,-8.996,13.581,-6.849};
      // if(iside==1 && tip[iside]>0.3 && tip[iside]<0.7) edep_c2*=pc[0]+pc[1]*tip[iside]+pc[2]*pow(tip[iside],2)+pc[3]*pow(tip[iside],3)+pc[4]*pow(tip[iside],4);
      _simcl[iside]->Multiply(edep_c2);
    }
    // ion tuning (with/without non-linearities) 
    else { 
      // some more tuning of lithium-to-oxygen signal (>B1030) only for non-linearity
      int z = Status&0x3f;
      if(TRMCFFKEY.MergeMCCluster==5 &&(int(TRMCFFKEY.MergeD[4])%100)/10 &&z<_effch)z=_effch;
      if (z>8) z = 8; 
      if (z<3) z = 3;
      double mis_calib[6] = {0,0,0.0047,0.0073,0.0173,0.0273};
      double fine_tuning[2][6] = { {1,1,1,1,1,1}, {0.71,0.72,0.73,0.92,0.90,0.88} };
      double mev_on_adc = TRMCFFKEY.ADCMipValue[1][iside]*edep/81;
      // X-side (only signal tuning)
      if (iside==0) _simcl[iside]->Multiply(mev_on_adc*fine_tuning[0][z-3]*0.93);
      else {   
        // Y-side
        if ((TRMCFFKEY.UseNonLinearity%10)==0) _simcl[iside]->Multiply(mev_on_adc*0.95); // better lithium
        else {
          _simcl[iside]->Multiply(mev_on_adc*fine_tuning[1][z-3]*1.35*0.85);
          for (int ist=0; ist<_simcl[iside]->GetWidth(); ist++) { 
            int iva = int(_simcl[iside]->GetAddress(ist)/64);
            double adc1 = _simcl[iside]->GetSignal(ist);
            double corr = 1;
            if(adc1>1e-8){//Bug fixed by QY
              corr = TrLinearDB::GetHead()->ApplyNonLinearity(adc1,GetTkId(),iva,3)/adc1;
              corr -= mis_calib[z-3]; // simulate a residual "mis-calibration" (used as tuning parameter) 
            }
            double adc2 = corr*adc1;
            _simcl[iside]->SetSignal(ist,adc2);
          }
        }
      }
    }
 
    if (_itra>0) {
//      hman.Fill((sidename[iside]=='x'?"TrSimEtaPrimx":"TrSimEtaPrimy"),ia[iside],_simcl[iside]->GetEta());
      hman.Fill((sidename[iside]=='x'?"TrSimResPrimx":"TrSimResPrimy"),ia[iside],1e+4*(coo[iside]-_simcl[iside]->GetX(iside,GetTkId(),2,imult)));
    }

  }
  return;
}


void TrMCClusterR::GenSimClusters3(){//QY

  // montecarlo truth
  char     sidename[2] = {'x','y'};
  float    step = GetStep()*1.e4;     // step [um] 
  AMSPoint glo = GetXgl();            // coordinate [cm]
  AMSDir   dir = GetDir();            // direction 
  float    edep = GetEdep()*1.e6;     // energy deposition [keV] 
  int Z = 0;
  double M = 0;
  GetChargeAndMass(Z,M);

  // is there any deposit?
  if (edep<=0) {
    if (VERBOSE) printf("TrMCClusterR::GenSimClusters3-W error edep=%7.2f keV\n",edep);
    return;
  }

  // local coo
  TkSens _glo2loc(1);
  _glo2loc.SetGlobal(GetTkId(),glo,dir);
  int  nsensor = _glo2loc.GetSensor();                                      // sensor number
  double height = _glo2loc.GetSensCoo().z()*1e+4;                           // position w/ the center of the silicon
  double ip[2] = {_glo2loc.GetSensCoo().x(),_glo2loc.GetSensCoo().y()};     // sensor impact point
  double ia[2] = {_glo2loc.GetImpactAngleXZ(),_glo2loc.GetImpactAngleYZ()}; // sensor impact angle
  double coo[2] = {_glo2loc.GetLaddCoo().x(),_glo2loc.GetLaddCoo().y()};
  int imult = _glo2loc.GetMultIndex();

  // hall effect
  double B = 0;
  float  hallproj[2] = {0,0};
  if ( (fabs(TRMCFFKEY.BelauHallMobility[0])>1e-6)||
       (fabs(TRMCFFKEY.BelauHallMobility[1])>1e-6) ) {
    AMSDir halldir = GetSensHallDir(B);
    hallproj[0] = B*halldir.x();  
    hallproj[1] = B*halldir.y(); 
  }

  // Print();
  if (VERBOSE) {
    printf("TrSim::GenSimClusters-V tkid=%+4d loc(x,y,z)[cm]=(%7.4f,%7.4f,%7.4f) theta(xz,yz)[rad]=(%7.4f,%7.4f) edep[keV]=%7.2f nsens=%2d itra=%4d\n",
           GetTkId(),ip[0],ip[1],height,ia[0],ia[1],edep,nsensor,(int)abs(_itra));
    // printf("TrSim::GenSimClusters-V  laddcoo(x,y) = (%7.4f,%7.4f)   readout(x,y) = (%4d,%4d)   mult = %2d\n",
    //       _glo2loc.GetLaddCoo().x(),_glo2loc.GetLaddCoo().y(),_glo2loc.GetStripX(),_glo2loc.GetStripY(),imult);
  }

  // loop on two sides of the ladder
  for (int iside=0; iside<2; iside++) {

    if(step<1) ia[iside]=0;

    // Create the cluster, no smear L1/L9(meaningless!!!)
    float ipsmear=ip[iside];

    // Create the cluster
    TrSimCluster simcluster = TrSim::GetTrSimSensor(iside,GetTkId())->MakeCluster(ipsmear,ia[iside],nsensor,step*dir[2],height,hallproj[iside],Z);

    // from time to time the cluster can be empty
    if (simcluster.GetWidth()==0) continue;
    double signal = simcluster.GetTotSignal();
    if (signal<=0) continue;

    // put the cluster in the TrMCCluster object
    if (_simcl[iside]) delete _simcl[iside];
    _simcl[iside] = new TrSimCluster(simcluster);

    // intrinsic resolution 
    if (_itra>0) hman.Fill((sidename[iside]=='x'?"TrSimResPrimx":"TrSimResPrimy"),ia[iside],1e+4*(coo[iside]-_simcl[iside]->GetX(iside,GetTkId(),2,imult)));

    // normalization
    if ((TRMCFFKEY.NormalizeSimCluster%10)==1) {
      // keV to ADC
      TkLadder* ladder = (TkLadder*)TkDBc::Head->FindTkId(GetTkId());
      int ist = _simcl[iside]->GetSeedIndex(); 
      int address = _simcl[iside]->GetAddress(ist) + 640*(1-iside);
      if ( (ladder->IsK7())&&(iside==0) ) address = _simcl[iside]->GetAddressCycl(ist) + 640; // considering cyclicity
      int iva = int(address/64);
      double q = sqrt(edep/100); 
      double ADC = pow(TrSimSignalDB::GetHead()->GetValue(q,ladder->GetCrate(),ladder->GetTdr(),iva),2);
      _simcl[iside]->Multiply(ADC); 
    }
    else {
      // multiply by edep to be able merge with other trsimcluster
      _simcl[iside]->Multiply(edep);
    }
  }
}


int TrMCClusterR::AddSimCluster(TrMCClusterR &that){//QY
  for (int ii=0; ii<2; ii++) {
    if (that._simcl[ii]==0) continue;
    if (_simcl[ii]==0) _simcl[ii] = new TrSimCluster(*(that._simcl[ii]));//copy
    else { //add
      TrSimSensor *simsensor = TrSim::GetTrSimSensor(ii,GetTkId());
      _simcl[ii]->AddCluster(*(that._simcl[ii]),simsensor->GetNChannels(),simsensor->IsK7());
    }
  }
  return 0;
}


void TrMCClusterR::ApplySaturation(){//QY

  // char     sidename[2] = {'x','y'};
  //float    step = GetStep()*1.e4;     // step [um] 
  AMSPoint glo = GetXgl();            // coordinate [cm]
  AMSDir   dir = GetDir();            // direction 
  float    edep = GetEdep()*1.e6;     // energy deposition [keV] 
  if (edep<1){// if energy deposition < 1 keV
    for(int ii=0; ii<2; ii++){if(_simcl[ii])delete _simcl[ii];_simcl[ii]=0;}
    return;
  }
  unsigned int hcharge=0;
  if(abs(_itra)>=47) hcharge=1;
  if(abs(_itra)>=61) hcharge=2;
   TkSens _glo2loc(1);
  _glo2loc.SetGlobal(GetTkId(),glo,dir);                                    // from global to local

  // double ip[2] = {_glo2loc.GetSensCoo().x(),_glo2loc.GetSensCoo().y()};     // sensor impact point
//  double ia[2] = {_glo2loc.GetImpactAngleXZ(),_glo2loc.GetImpactAngleYZ()}; // sensor impact angle
  double tip[2]= {_glo2loc.GetImpactPointX(),_glo2loc.GetImpactPointY()};   // true impact point [0,1]
  // int imult = _glo2loc.GetMultIndex();

  if(tip[0]<0)tip[0]+=1;
  if(tip[1]<0)tip[1]+=1;
  //                         p_x     p_y       He_x    He_y
  // moved to dcard  double SmearPos[2][2]={{0.0008,      0.},{0.0008,  0.0}};
  // loop on two sides of the ladder
  // int hcharge_gauss = (hcharge>1) ? 1 : hcharge; // ions = He 
   for (int iside=0; iside<2; iside++) {
     if(_simcl[iside]==0)continue;
//     if(step<1) ia[iside]=0;
      _simcl[iside]->Multiply(1./edep);//normalized to the edep
//     hman.Fill(Form("TrSimSig%c",sidename[iside]),_simcl[iside]->GetEta(),_simcl[iside]->GetTotSignal());

/*     if (fabs(ia[iside])<0.8)
    _simcl[iside]->GaussianizeFraction(iside,hcharge_gauss,TRMCFFKEY.TrSim2010_FracNoise[iside], tip[iside]);//this part have to be retuned */

     if (hcharge<2) {//p/he - non-linear edep
       double edep_c2=edep;
       if(iside==0) {
         double edep_c=qlinfun2(edep,TRMCFFKEY.SigQuadLoss[hcharge][iside]); // 
         edep_c2=TRMCFFKEY.ADCMipValue[hcharge][0]*edep_c/81;
       }
       else {
         double edep_c=edep*(1+rnormx()*0.15);
         edep_c=qlinfun2(edep_c,TRMCFFKEY.SigQuadLoss[hcharge][iside]);
         edep_c2=TRMCFFKEY.ADCMipValue[hcharge][iside]*edep_c/81+edep_c/81*edep_c/81-4;
       }
       _simcl[iside]->Multiply(edep_c2);
     } 
     else {//ion non-linear edep
  // some more tuning of lithium-to-oxygen signal (>B1030) only for non-linearity
      int z = Status&0x3f;
      if (z>8) z = 8;
      if (z<3) z = 3;
      double mis_calib[6] = {0,0,0.0047,0.0073,0.0173,0.0273};
      double fine_tuning[2][6] = { {1,1,1,1,1,1}, {0.71,0.72,0.73,0.92,0.90,0.88} };
      double mev_on_adc = TRMCFFKEY.ADCMipValue[1][iside]*edep/81;
      if (iside==0) _simcl[iside]->Multiply(mev_on_adc*fine_tuning[0][z-3]*0.93);//X side
      else {  // Y-side
          if ((TRMCFFKEY.UseNonLinearity%10)==0) _simcl[iside]->Multiply(mev_on_adc*0.95); // better lithium
          else {
            _simcl[iside]->Multiply(mev_on_adc*fine_tuning[1][z-3]*1.35*0.85);
          for (int ist=0; ist<_simcl[iside]->GetWidth(); ist++) {
            int iva = int(_simcl[iside]->GetAddress(ist)/64);
            double adc1 = _simcl[iside]->GetSignal(ist);
            double corr = 1;
            if(adc1>1e-8){//Bug fixed by QY
              corr = TrLinearDB::GetHead()->ApplyNonLinearity(adc1,GetTkId(),iva,3)/adc1;
              corr -= mis_calib[z-3]; // simulate a residual "mis-calibration" (used as tuning parameter) 
            }
            double adc2 = corr*adc1;
            _simcl[iside]->SetSignal(ist,adc2);
          }
        }
       }
     }
   }
   return; 
}


// parameters from fit to TrSimEDepTot (B1110, 22/05/2017)
static double mip_norm[8][4] = {
  {0.97769,0.106869,8.22245, 0.67904}, // --> problem in reshaping given by the energy deposition fluctuations
  {0.99491,0.106112,7.45000,-0.34356},
  {1.00365,0.105706,7.54833,-0.36091},
  {1.00531,0.103615,7.45164,-0.66819},
  {1.00469,0.107947,7.39542,-0.33103},
  {1.00458,0.106883,7.37415,-0.44463},
  {1.00347,0.108159,7.33587,-0.36293},
  {1.00218,0.108062,7.32315,-0.37251}
};

// this normalization depends on the charge loss amount: 6% for X (600,30,60), and 0% for Y (600,0,60)
static double norm_adj_before[2][5] = {
  {1.06*1.02*1.02,1.06*1.05*1.03,1.06*1.03,1.06*1.01,1.06}, 
  {1.06,1.01*1.03,1,1,1},
}; 

/*
static double norm_adj_before2[2][5] = {
  {1.06*1.02*1.02*1.04,1.06*1.05*1.03,1.06*1.03*1.02,1.06*1.01*1.02,1.06},
  {1.06,1.01*1.03,1.03,1,1},
};
*/

static double norm_adj_before2[2][5] = {
  {1.06*1.02*1.02*1.04,1.06*1.05*1.03,1.06*1.03,1.06*1.01*1.02,1.06},
  {1.06,1.01*1.03,1,1,1},
};

void TrMCClusterR::ApplyNormalization() {
  TkLadder* ladder = (TkLadder*)TkDBc::Head->FindTkId(GetTkId());
  if (!ladder) return;
  bool isK7 = ladder->IsK7();
  float edep = GetEdep()*1.e6; // energy deposition [keV] 
  AMSPoint glo = GetXgl(); // coordinate [cm]
  AMSDir   dir = GetDir(); // direction 
  TkSens _glo2loc(1);
  _glo2loc.SetGlobal(GetTkId(),glo,dir);                                    // from global to local
  double ia[2] = {_glo2loc.GetImpactAngleXZ(),_glo2loc.GetImpactAngleYZ()}; // sensor impact angle
  double costheta = 1./sqrt(1.+ia[0]*ia[0]+ia[1]*ia[1]);
  int Z = 0; 
  double M = 0;
  GetChargeAndMass(Z,M);
  double rigidity = (fabs(Z)>0) ? GetMomentum()/Z : 0;
  if ( (fabs(Z)>0)&&(fabs(rigidity)>0)&&(_itra>0) ) hman.Fill("TrSimEDepTot",log10(fabs(rigidity)),sqrt(edep*costheta/100)/fabs(Z));
  double normali = ((abs(Z)<=8)&&(abs(Z)>=1)) ? 1./sqrt(mip_norm[abs(Z)-1][0]) : 1./sqrt(mip_norm[7][0]);
  double reshape = 1;
  if ( ((TRMCFFKEY.NormalizeSimCluster%10)==3)&&(M>0) ) reshape = GetReshapedEnergyLossStretch(edep,costheta,Z,fabs(rigidity*Z/M)); 
  for (int is=0; is<2; is++) {
    if (!_simcl[is]) continue; 
    double edep_meas = _simcl[is]->GetTotSignal();
    if (edep_meas<=0) continue;
    if (TRMCFFKEY.TrSim2010_AddNoise[is]>0) {
      for (int jj=0; jj<_simcl[is]->GetWidth(); jj++) {
        double sig = _simcl[is]->GetSignal(jj);
        sig += edep_meas*TRMCFFKEY.TrSim2010_AddNoise[is]*rnormx();
        if (sig<0) sig = 0;
        _simcl[is]->SetSignal(jj,sig);
      }
      edep_meas = _simcl[is]->GetTotSignal();
      if (edep_meas<=0) continue;
    }
    double max_meas_edep = 0;
    double max_meas_edep_add = -1;
    for (int ist=0; ist<_simcl[is]->GetWidth(); ist++) {
      int address = _simcl[is]->GetAddress(ist) + 640*(1-is);
      if ( (isK7)&&(is==0) ) address = _simcl[is]->GetAddressCycl(ist) + 640; // considering cyclicity
      if (!((0<=address)&&(address<1024))) continue;
      if (_simcl[is]->GetSignal(ist)>max_meas_edep) {
        max_meas_edep = _simcl[is]->GetSignal(ist);
        max_meas_edep_add = address;
      }
    }
    if (max_meas_edep_add<0) continue;
    int    iva = int(max_meas_edep_add/64);
    double ADC = 0;
    double q = (TRMCFFKEY.NormalizeSimCluster<10) ? sqrt(edep/100) : sqrt(edep_meas/100);
    if (TRMCFFKEY.NormAdjBefore[is]<=0) { 
      int index = ( (abs(Z)<=4)&&(abs(Z)>0) ) ? abs(Z)-1 : 4;
      if (int(TRMCFFKEY.NormAdjBefore[is])==-1) q *= normali*reshape*norm_adj_before[is][index]; 
      else                                      q *= normali*reshape*norm_adj_before2[is][index];
    }
    else {
      q *= normali*reshape*TRMCFFKEY.NormAdjBefore[is]; 
    }
   if (TRMCFFKEY.EdepAddNoise>0) q += q*TRMCFFKEY.EdepAddNoise*rnormx();
    ADC = pow(TrSimSignalDB::GetHead()->GetValue(q,ladder->GetCrate(),ladder->GetTdr(),iva),2);
    double keV_to_ADC = (edep_meas>0) ? ADC/edep_meas : 0;
    _simcl[is]->Multiply(keV_to_ADC);
  }
}


double TrMCClusterR::GetMeanEnergyLoss(double bg, int z) {
  if ( (bg<=0)||(abs(z)<=0) ) return 1;
  double beta = 1./sqrt(1+pow(bg,-2));
  double xx = log10(bg);
  double I = 173e-6; // MeV
  double hnu = 31.05e-6; // MeV
  double C = 1+2*log(I/hnu);
  double x0 = 0.2;
  double x1 = 3;
  double xa = C/4.606;
  double m = 3;
  double a = 4.606*(xa-x0)*pow(x1-x0,-m);
  double delta = 0;
  if      (xx<x0) delta = 0;
  else if (xx<x1) delta = 4.606*xx-C+a*pow(x1-xx,m);
  else            delta = 4.606*xx-C;
  int index = (!((abs(z)<=8)&&(abs(z)>=1))) ? 7 : abs(z)-1; 
  index=7;
  double k0 = mip_norm[index][1]/mip_norm[index][0];   
  double k1 = mip_norm[index][2];
  double k2 = mip_norm[index][3];
  return k0*pow(beta,-2)*(k1+log(bg)-pow(beta,2)*k2-0.5*delta);
}


// parameters from fit .... 
static double mip_poly[8][6] = {
  {-11.541188,46.297040,-67.806619,49.395430,-17.927067,2.595514},
  {-12.005399,45.973887,-61.688324,38.026149,-9.936511,0.631566},
  {-142.639358,667.044365,-1238.125446,1147.941127,-531.616997,98.392375},
  {-334.078112,1608.945443,-3092.216281,2973.196733,-1430.284846,275.431993},
  {-318.224261,1502.088693,-2827.964037,2663.076117,-1254.631245,236.650820},
  {-318.224261,1502.088693,-2827.964037,2663.076117,-1254.631245,236.650820},
  {-1124.872352,5389.952727,-10334.140449,9920.979944,-4770.125508,919.202480},
  {-728.440595,3344.131363,-6124.732423,5603.416844,-2562.053990,468.676019},
}; 


double TrMCClusterR::GetReshapedEnergyLossStretch(double edep, double costheta, double z, double bg) { 
  if (bg<=0) return 1;
  double fact = GetMeanEnergyLoss(bg,z); 
  double x0 = sqrt(0.01*edep*costheta/fact)/fabs(z);
  int index = (!((abs(z)<=8)&&(abs(z)>=1))) ? 7 : abs(z)-1;   //!
  index=7;
  double x1 = 0;
  for (int i=0; i<6; i++) x1 += mip_poly[index][i]*pow(x0,i);
  if (x1>1.1) x1 = 1.1;
  if (x1<0.9) x1 = 0.9;
  return x1; 
}


////////////////////////////////////////////////////////////////////////////////////
// OLD GBATCH SIMULATION -> could be re-implemented using TrSimCluster middle class
////////////////////////////////////////////////////////////////////////////////////

void TrMCClusterR::_shower() {
  printf(" TrMCClusterR::_shower-E- NO-DIGITIZATION!!  This method of digitizing the tracker has been declare OBSOLETE and commented out. \n");
  /*
  AMSDir   dir(_Momentum);
  AMSPoint entry  = _xgl;
  AMSPoint dentry(dir[0]/dir[2]*0.0015,
                  dir[0]/dir[2]*0.0015,
                  0.0015);
  for (int i = 0; i < 5; i++) _ss[0][i] = _ss[1][i] = 0;
  // int sensor = abs(_idsoft)/10000;
  int tkid   = abs(_idsoft)%1000;
  int ss     = abs(_idsoft)%10000-tkid;
  if(!ss) tkid*=-1;
  int layer = abs(tkid)/100;
  TkLadder* ll = TkDBc::Head->FindTkId(tkid);
  if(!ll){
    printf(" TrMCClusterR::_shower: ERROR cant find ladder %d into the database\n",tkid);
    return ;
  } 
  int nchx  = (ll->IsK7()) ? TkDBc::Head->_NReadStripK7 
                                         : TkDBc::Head->_NReadStripK5;
  int nchy  = TkDBc::Head->_NReadoutChanS;
  TkSens tks(tkid, _xgl,1);
  int bcen0 = tks.GetStripX();
  int bcen1 = tks.GetStripY();
  int mult = 0, bctdr0 = bcen0;
  // Convert from TDR address into sensor strip
  if (ll->IsK7()) {
    int nad = TkDBc::Head->_NReadoutChanK;
    int sen = tks.GetSensor();
    int idx = ((sen+1)*nchx-bcen0)/nad;
    bcen0 = bcen0+idx*nad-sen*nchx;
  }
  else if (tks.GetSensor()%2 == 1) bcen0 -= 192;
  if (bcen0 < 0) bcen0 = 0;
  if (bcen1 < 0) bcen1 = 0;
  if (bcen0 >= nchx) bcen0 = nchx-1;
  if (bcen1 >= nchy) bcen1 = nchy-1;
  _center[0] = bcen0;
  _center[1] = bcen1;
  _left  [0] = max(0,_center[0]-(5/2));
  _left  [1] = max(0,_center[1]-(5/2));
  _right [0] = min(nchx-1, _center[0]+(5/2));
  _right [1] = min(nchy-1, _center[1]+(5/2));
  float xpr = _sum*TRMCFFKEY.dedx2nprel;
  if (xpr <= 0) return;
  for (int k = 0; k < 2; k++) {
    // fast algo
    //    if(fabs(dentry[k])/(xpr)<TRMCFFKEY.fastswitch){
    if(1){
      double s = strip2x(tkid, k, _center[k], mult);
      double e = entry[k]-s;
      for (int i =_left[k]; i <= _right[k]; i++){
        double a1 = strip2x(tkid, k, i, mult)-s;
        double a2 = a1+strip2x(tkid, k, i+1, mult)-strip2x(tkid, k, i, mult);
        _ss[k][i-_left[k]] = TRMCFFKEY.delta[k]*xpr*fint2(a1, a2, e, dentry[k], a2-a1);
      }
    }
    //slow algo
    // else std::cerr << "Error: slow algo is not implemented" << std::endl;
    for (int i = _left[k]; i <= _right[k]; i++)
      _ss[k][i-_left[k]] *= 1+rnormx()*TRMCFFKEY.gammaA[k];
  }
  int offs = 640+bctdr0-bcen0;
  _left[0]+=offs;  _center[0]+=offs;  _right[0]+=offs;
  if (_right[0] > 1023) _right[0] = 1023;
  if (_left [0] < 0   ) _left [0] = 0;
  */
}


/*
float TrMCClusterR::strip2x(int tkid, int side, int strip, int mult) {
   int layer = abs(tkid)/100;
   TkLadder* ll = TkDBc::Head->FindTkId(tkid);
   if(!ll){
     printf("TrMCClusterR::strip2x: ERROR cant find ladder %d into the database\n",tkid);
     return -1;
   } 
   int nch   = (side  == 1) ? TkDBc::Head->_NReadoutChanS
     : (ll->IsK7()) ? TkDBc::Head->_NReadStripK7 
     : TkDBc::Head->_NReadStripK5;

   int ss = strip;
   if (ss <=   0) ss = 1;
   if (ss >= nch) ss = nch-1;

   ss += (side == 0) ? TkDBc::Head->_NReadoutChanS : 0;
   float ss0 = TkCoo::GetLocalCoo(tkid, ss-1, mult);
   float ss1 = TkCoo::GetLocalCoo(tkid, ss,   mult);

   if (strip <=   0) return ss0-(ss0+ss1)/2;
   if (strip >= nch) return ss1+(ss0+ss1)/2;

   return (ss0+ss1)/2;
}

double TrMCClusterR::fints(double a, double b) {
  if      (a >  0 && b >  0) return fdiff(a, 0)-fdiff(b, 0);
  else if (a <= 0 && b <= 0) return fdiff(b, 0)-fdiff(a, 0);
  return 2*fdiff(0, 0)-fdiff(a, 0)-fdiff(b, 0);
}

double TrMCClusterR::fint2(double a, double b, 
 			  double av, double dav, double dix) {
  if (std::abs(dav)/dix <= 0.01) return fints(a-av, b-av);

  double dlmin = av-std::abs(dav);
  double dlmax = av+std::abs(dav);
  if (a <= dlmin && b >= dlmax) return fintc(a, b, dlmin, dlmax);
  if (a <= dlmin && b <= dlmin) return fintl(a, b, dlmin, dlmax);
  if (a >= dlmax && b >= dlmax) return fintr(a, b, dlmin, dlmax);

  if (a <= dlmin && b <= dlmax)
    return fintc(a, b, dlmin, b)*(b-dlmin)/(dlmax-dlmin)
          +fintl(a, b, b, dlmax)*(dlmax-b)/(dlmax-dlmin);

  if (a >= dlmin && b >= dlmax)
    return fintc(a, b, a, dlmax)*(dlmax-a)/(dlmax-dlmin)
          +fintr(a, b, dlmin, a)*(a-dlmin)/(dlmax-dlmin);

  return fintc(a, b, a,     b)*    (b-a)/(dlmax-dlmin)
        +fintr(a, b, dlmin, a)*(a-dlmin)/(dlmax-dlmin)
        +fintl(a, b, b, dlmax)*(dlmax-b)/(dlmax-dlmin);
}

double TrMCClusterR::fintc(double a, double b, double c, double d) {
  return 2*fdiff(0,0)-(fdiff(b-d,1)-fdiff(b-c,1))/(d-c)
                     +(fdiff(a-d,1)-fdiff(a-c,1))/(d-c);
}

double TrMCClusterR::fintr(double a, double b, double c, double d) {
  return (-fdiff(b-d,1)+fdiff(b-c,1)
          +fdiff(a-d,1)-fdiff(a-c,1))/(d-c);
}

double TrMCClusterR::fintl(double a, double b, double c, double d) {
  return (-fdiff(b-d,1)+fdiff(b-c,1)
 	  +fdiff(a-d,1)-fdiff(a-c,1))/(d-c);
}

double TrMCClusterR::fdiff(double a, int ialpha) {
  double xl = std::abs(a)*TRMCFFKEY.alpha;
  if (xl > 70) return 0;

  double diff = TRMCFFKEY.beta*std::exp(-xl);
  if (ialpha) diff /= TRMCFFKEY.alpha;

  return diff;
}
*/


double qlinfun(double X, double k){
  double th=135*TMath::Pi()/180.;
  double ss=sin(th);
  double cc=cos(th);
  double delta=cc*cc+4*k*ss*X;
  if(delta>0){
    double xip=(-cc+sqrt(delta))/(2*k*ss);
    double xim=(-cc-sqrt(delta))/(2*k*ss);
    double Ym=-xim*ss+k*xim*xim*cc;
    double Yp=-xip*ss+k*xip*xip*cc;
    return (Ym>0)?Ym:Yp;
  } else
  return 0;
}

double qlinfun2(double X, double k){
  double flim=750;
  if(X<flim){
    return qlinfun(X,k);
  }else{
    double p[3]={11.9216,0.5,106.043};
    double ylp=(qlinfun(flim,k)-qlinfun(flim-30,k))/30.;
    p[0]=ylp/(p[1]*pow(flim,p[1]-1));
    double yl=qlinfun(flim,k);
    p[2]=yl-p[0]*pow(flim,p[1]);
    return p[0]*pow(X,p[1])+p[2];
  }
}



double gain_to_gain(double* x, double* par) {
  // y = c*x         x < x1        c = MeVtoADC
  // y = k + a*x     x1 < x < x2   with a ~ 0
  // y = o + g*x     x > x2        a < g < 1
  // 7 parameters: c, k, a, o, g, x1, x2 
  // 2 continuity conditions: y(x1-) = y(x1+) and y(x2-) = y(x2+)
  // y(x1-) = c*x1 = y(x1+) = k + a*x1   ==>   k = x1*(c-a)
  // y(x2-) = x1*(c-a) + a*x2 = y(x2+) = o + g*x2   ==>    o = x1*(c-a) + x2*(a-g)
  double xx = x[0];
  double x1 = par[0];
  double x2 = par[1];
  double c  = par[2];
  double a  = par[3];
  double g  = par[4];
  double k  = x1*(c-a);
  double o  = x1*(c-a) + x2*(a-g);
  if      (xx<x1) return c*xx;
  else if (xx>x2) return o + g*xx;
  return k + a*xx;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////


#include "MagField.h"


AMSPoint TrMCClusterR::GetSensMagField(int algo) { 
  if (algo==0) { 
    // 1. Simplest rule
    // Local magnetic field is oriented along X. 
    // It is positive for ladders placed on negative side (X<0) and positive for the other.
    // (what about rotation of the layer 1?, how the tkids were assigned?) 
    double Field = 0.150; // T 
    int sign = -1; 
    if (GetTkId()<0) sign = +1;
    return AMSDir(sign,0,0)*Field; 
  }
  else if (algo==1) { 
    // 2. More complex and precise rule 
    // Field has a constant intensity along AMS x-axis. 
    // Rotate x-axis in the local frame.
    double   Field = 0.150; // T 
    AMSPoint SensorCoo;
    AMSDir   SensorFieldDir;
    GlobalToLocal(GetXgl(),AMSPoint(1,0,0),SensorCoo,SensorFieldDir);
    return SensorFieldDir*Field;
  }
  else {
    // 3. Even more complex rule 
    // Take the magnetic field from the map (TO-BE-FIXED: you must be sure that MagneticFieldMap is already loaded). 
    // Rotate it to the local frame.
    AMSPoint FieldVector = MagField::GetPtr()->GuFld(GetXgl())*0.1; // kG -> T
    double   Field = FieldVector.norm();
    AMSDir   FieldDir = FieldVector/Field;
    AMSPoint SensorCoo;
    AMSDir   SensorFieldDir;
    GlobalToLocal(GetXgl(),FieldDir,SensorCoo,SensorFieldDir);
    return SensorFieldDir*Field;
  }
  return AMSPoint(0,0,0);
}


AMSDir TrMCClusterR::GetSensHallDir(double& B) {
  AMSPoint Bvec = GetSensMagField();
  B = Bvec.norm();
  if (B<=0) return AMSDir(0,1,0); 
  AMSDir Bdir = Bvec/B;
  AMSDir Edir(0,0,1); 
  return Edir.cross(Bdir);
}


AMSPoint TrMCClusterR::GetSensMiddleCoo() {
  AMSPoint coo = GetSensCoo();
  AMSDir   dir = GetSensDir();
  coo[0] -= coo[2]*dir[0]/dir[2];
  coo[1] -= coo[2]*dir[0]/dir[2];
  coo[2] -= coo[2];
  return coo;
}


bool TrMCClusterR::GlobalToLocal(AMSPoint GlobalCoo, AMSDir GlobalDir, AMSPoint& SensorCoo, AMSDir& SensorDir) {
  // default values
  SensorCoo = AMSPoint(-10000,-10000,-10000);
  SensorDir = AMSDir(0,0,-1);
  // global-to-local coordinate converter for simulation (--> also intrinsic resolution)
  TkSens sensor(true); 
  sensor.SetGlobal(GlobalCoo,GlobalDir);
  // ladder not found
  if (!sensor.LadFound()) {
    printf("TrMCClusterR::GlobalToLocal-W ladder not found.\n");
    return false;
  }
  // coordinate outside from active area 
  AMSPoint local = sensor.GetSensCoo();
  if ( (local.x()<0) || (local.x()>TkDBc::Head->_ssize_active[0]) || 
       (local.y()<0) || (local.y()>TkDBc::Head->_ssize_active[1]) ) {
    printf("TrMCClusterR::GlobalToLocal-W coordinate outside sensor active area.\n");
    return false;
  }
  // tkid different from what is stored 
  if (sensor.GetLadTkID()!=GetTkId()) { 
    printf("TrMCClusterR::GlobalToLocal-W found ladder with tkid %+4d instead of stored tkid %+4d.\n",sensor.GetLadTkID(),GetTkId());
    return false;
  }
  // number of sensor different from what is stored: WHY THIS HAPPENS?
  // if (sensor.GetSensor()!=GetSensor()) {
  //   printf("TrMCClusterR::GlobalToLocal-V found sensor number %d instead of stored sensor number %d.\n",sensor.GetSensor(),GetSensor());
  //   return false;
  // }
  // set 
  SensorCoo = sensor.GetSensCoo();
  SensorDir = sensor.GetSensDir();
  return true;
}


