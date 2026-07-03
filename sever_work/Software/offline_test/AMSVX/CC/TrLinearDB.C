#include "TrLinearDB.h"

#include "root.h"
#include "root_setup.h"

#include "tkdcards.h"
#include "VCon.h"
#include "TrRecHit.h"
#include "TrCluster.h"
#include "commonsi.h"


ClassImp(TrLinearDB);
ClassImp(TrLinearElem);


///////////////////////////
// TrLinearDB
///////////////////////////


TrLinearDB* TrLinearDB::fHead = 0;
map<int,TrLinearElem*> TrLinearDB::fTrLinearElemIndexMap;


int TrLinearDB::DefaultCorrDepth = 3;

int    TrLinearDB::fRun  = 0;
double TrLinearDB::fTemp = 0;

TrLinearDB* TrLinearDB::GetHead() {
  if (IsNull()) {
    printf("TrLinearDB::GetHead()-V TrLinearDB singleton initialization.\n");
    fHead = new TrLinearDB(); 
  } 
  return fHead;
}


TrLinearDB::~TrLinearDB() { 
  Clear();
  if (fHead) delete fHead;
  if (fLinear) delete [] fLinear;
  fHead = 0;
  fLinear = 0;
}


void TrLinearDB::Init() {
  if (LoadDefaultTablesVer0()) 
    printf("TrLinearDB::GetHead()-V TrLinearDBver0 correction loaded, %d elements read.\n",(int)fTrLinearElemIndexMap.size());

  // Get Inner Tracker temperature
  if (fRun > 0 && AMSSetupR::gethead() && 
      (fTemp == 0 || 
       int(AMSSetupR::gethead()->fHeader.Run) != fRun)) {
    if  (AMSSetupR::gethead()->fHeader.Run)    fRun
       = AMSSetupR::gethead()->fHeader.Run;
    else {
      if (!AMSEventR::Head()) {
	AMSEventR::Head() = new AMSEventR;
	AMSEventR::Head()->fHeader.Run = fRun;
	AMSEventR::Head()->fHeader.Event = 1;
	AMSEventR::Head()->fHeader.Time[0] = fRun;
      }
      AMSSetupR::gethead()->UpdateHeader(AMSEventR::Head());
    }

    vector<double> val;
    if (!AMSSetupR::gethead()
	->fSlowControl.GetData("Sensor A", fRun, 0, val) &&
	val.size() > 0 && 0 < val[0] && val[0] < 35) {
      fTemp = val[0];
      printf("TrLinearDB::Init-I-Tracker temperature at run %d is %5.2f\n",
	     fRun, fTemp);
    }
  }

  if (!fLinear) fLinear = new float[GetSize()];  
  for (int i=0; fLinear && i<GetSize(); i++) fLinear[i] = 0;
} 


void TrLinearDB::Clear(Option_t* option) { 
  // delete all tables
  for (map<int,TrLinearElem*>::iterator it=fTrLinearElemIndexMap.begin(); it!=fTrLinearElemIndexMap.end(); it++) {
    if (it->second) {
      it->second->Clear();
      delete it->second;
    }
    it->second = 0;
  }
  fTrLinearElemIndexMap.clear();
  if (!fLinear) fLinear = new float[GetSize()];  
}


void TrLinearDB::Info() {
  printf("TrLinearDB::Info-V:\n");
  for (map<int,TrLinearElem*>::iterator it=fTrLinearElemIndexMap.begin(); it!=fTrLinearElemIndexMap.end(); it++) {
    it->second->Info();
  }
}


bool TrLinearDB::AddElem(int index, TrLinearElem* elem) {
  if (elem==0) return false;
  pair<map<int,TrLinearElem*>::iterator,bool> ret;
  // insert
  ret = fTrLinearElemIndexMap.insert(pair<int,TrLinearElem*>(index,elem));
  // if the index already exists renew the table
  if (ret.second == false) {
    map<int,TrLinearElem*>::iterator it = ret.first;
    delete it->second;
    it->second = elem;
  }
  return true;
}


TrLinearElem* TrLinearDB::GetElem(int index) {
  map<int,TrLinearElem*>::iterator it=fTrLinearElemIndexMap.find(index);
  return (it!=fTrLinearElemIndexMap.end()) ? it->second : 0;
}


TrLinearElem* TrLinearDB::GetElem(int tkid, int iva, int depth) {
  int index = CreateIndex(tkid,iva,depth);
  return GetElem(index);
}


TrLinearElem* TrLinearDB::GetValidElem(int tkid, int iva, int depth) {
  // if not valid at that depth decrease the level 
  for (int idepth=depth; idepth>=0; idepth--) {
    TrLinearElem* elem = GetElem(tkid,iva,idepth);  
    if (!elem) continue;
    if (elem->IsValid()) return elem;
  }
  return 0;
}


bool TrLinearDB::LoadDefaultTablesVer0(char* dirname) {
  int ver = (fRun > 0) ? 1 : 0;
  TString sfn = Form("%s/%s/TrLinearDBver%d/TrLinearDB.txt",dirname,AMSCommonsI::getversion(),ver);//the directory has been changed to v6.00
  FILE* file = fopen(sfn,"r");
  if (file==0) return false;
  int nload=0;
  while (!feof(file)) {
    int index,isok,ret;
    float chisq,pars[5],tcor[5]={0,0,0,0,0};
    float* tc=0;
    if (ver==1) {
      //[index] [p0_0] [p0_1] [p1] [p2_0] [p2_1] [p3] [p4]
      ret = fscanf(file,"%d%f%f%f%f%f%f%f",&index,
		   &pars[0],&tcor[0],&pars[1],
		   &pars[2],&tcor[2],&pars[3],&pars[4]); 
     if (ret > 0) {
      tc=tcor;
      isok=1;
      chisq=1;
      nload++;
     }
    }
    else
      ret = fscanf(file,"%d%d%f%f%f%f%f%f",&index,&isok,&chisq,&pars[0],&pars[1],&pars[2],&pars[3],&pars[4]); 

    if (ret<=0) continue;
    AddElem(index,new TrLinearElem(index,isok,chisq,pars,tc));
  }
  fclose(file);
  if (ver==1) {
    printf("TrLinearDB::GetHead()-V TrLinearDBver1 correction loaded, %d elements read, total %d.\n",nload, (int)fTrLinearElemIndexMap.size());
    return false;
  }
  return true;
}


int TrLinearDB::CreateIndex(int tkid, int iva, int depth) {
  // whole tracker index
  if (depth==0) return 0;
  TkLadder* ladder = TkDBc::Head->FindTkId(tkid);
  if (!ladder) { 
    printf("TrLinearDB::CreateIndex-W requested tkid (%+04d) not found in TkDBc. I will use default correction of the whole Tracker.\n",tkid);
    return 0; 
  }
  // layer index
  if (depth==1) return ladder->GetLayerJ();
  // ladder index
  int index = 10+ladder->GetCrate()*24+ladder->GetTdr();
  if (depth==2) return index; 
  // va level  
  if ( (iva<0)||(iva>9) ) {
    printf("TrLinearDB::CreateIndex-W not valid requested VA (%02d). I will use default correction of the whole Tracker.\n",iva);
    return index;
  }
  index = (10+ladder->GetCrate()*24+ladder->GetTdr())*100+iva;
  return index;
}


double TrLinearDB::GetLinearityCorrected(double ADC, int tkid, int iva, int depth) {
  TrLinearElem* elem = GetValidElem(tkid,iva,depth);  
  if (!elem) {
    printf("TrLinearDB::GetLinearityCorrected-E no correction element found (tkid=%+04d,iva=%02d,depth=%1d). Return unmodified value\n",tkid,iva,depth);
    return ADC;
  }

  return elem->GetLinearityCorrected(ADC, fTemp);
}


double TrLinearDB::ApplyNonLinearity(double ADC, int tkid, int iva, int depth) {
  TrLinearElem* elem = GetValidElem(tkid,iva,depth);
  if (!elem) {
    printf("TrLinearDB::ApplyNonLinearity-E no correction element found (tkid=%+04d,iva=%02d,depth=%1d). Return unmodified value\n",tkid,iva,depth);
    return ADC;
  }
  return elem->ApplyNonLinearity(ADC);
}

void TrLinearDB::SetUseNonLinearity(int opt, int readfromfile)
{
  TRCLFFKEY.UseNonLinearity = opt;
  TRCLFFKEY_DEF::ReadFromFile = readfromfile;
}



///////////////////////////
// TrLinearElem
///////////////////////////


TrLinearElem::TrLinearElem(int index, int succ, double chisq, double* pars, double* tcor) {
  fIndex = index;
  fSucc = succ;
  fChisq = chisq;
  for (int ipar=0; ipar<5; ipar++) fPars[ipar] = pars[ipar];
  for (int ipar=0; ipar<5; ipar++) fTcor[ipar] = (tcor) ? tcor[ipar] : 0;
}


TrLinearElem::TrLinearElem(int index, int succ, float chisq, float* pars, float* tcor) {
  fIndex = index;
  fSucc = succ;
  fChisq = chisq;
  for (int ipar=0; ipar<5; ipar++) fPars[ipar] = pars[ipar];
  for (int ipar=0; ipar<5; ipar++) fTcor[ipar] = (tcor) ? tcor[ipar] : 0;
}


void TrLinearElem::Info() {
  printf("TrLinearElem::Info-V:   Index: %5d   Succ: %2d   Chisq: %11.3f   Pars(",GetIndex(),GetSucc(),GetChisq());
  for (int ipar=0; ipar<5; ipar++) printf("%10.6f ",GetPar(ipar));
  printf(")\n");
}


bool TrLinearElem::IsValid() {
  return ( (GetChisq()<10)&&(GetSucc()>=0) );
}


double TrLinearElem::GetLinearityCorrected(double ADC, double temp) { 
  if (ADC < 0) return ADC;
  double pars[5];
  for (int i = 0; i < 5; i++) pars[i] = fPars[i]+fTcor[i]*temp;
  double tmp = (ADC>0) ? sqrt(ADC) : 0;
  return pow(p_strip_behavior(&tmp,pars),2); 
}


double TrLinearElem::ApplyNonLinearity(double ADC) {
  double tmp = (ADC>0) ? sqrt(ADC) : 0;
  double xmin = 0;
  double xmax = 1000; // crazy high
  // check range
  double eval = p_strip_behavior(&xmax,fPars);     
  if (tmp>eval) tmp = eval; // if too crazy give a flat responce  
  // find bin 
  for (int iter=0; iter<10; iter++) {
    double xmin_step,xmax_step;
    GetInterval(tmp,xmin,xmax,10,xmin_step,xmax_step);
    xmin = xmin_step;
    xmax = xmax_step; 
  }
  // linear interpolation
  double x  = tmp; 
  double y1 = xmin;
  double y2 = xmax;      
  double x1 = p_strip_behavior(&y1,fPars);   
  double x2 = p_strip_behavior(&y2,fPars);
  double interp = y1 + (x-x1)*(y2-y1)/(x2-x1);      
  return pow(interp,2); 
}


void TrLinearElem::GetInterval(double y, double xmin, double xmax, int nsteps, double& xmin_step, double& xmax_step) {
  int istep;
  for (istep=0; istep<=nsteps; istep++) {
    double x = xmin+((xmax-xmin)/nsteps)*istep;
    double eval = p_strip_behavior(&x,fPars);
    if (eval>y) break;
  }
  xmin_step = xmin+((xmax-xmin)/nsteps)*(istep-1);
  xmax_step = xmin+((xmax-xmin)/nsteps)*(istep);
}


///////////////////////////
// Functions
///////////////////////////


double p_strip_behavior(double* x, double* par) {
  double result = par[2]/(1. + exp(-(x[0]-par[0])/par[1]));
  if (x[0]<=par[0]) result += par[3]*x[0];
  else              result += par[3]*par[0]+par[4]*x[0]-par[4]*par[0];
  return result/par[3];
}

///////////////////////////
// TDV utilities
///////////////////////////

float* TrLinearDB::fLinear = 0; 
bool   TrLinearDB::fClear  = false;

bool TrLinearDB::DBToLinear() {
  int offset = 0;
  TrLinearElem el0;

  if (!TkDBc::Head) return false;

  for (int i = 0; i < 9; i++) {
    TrLinearElem *el = GetElem(i+1);
    if (!el) el = &el0; 
    el->DBToLinear(fLinear+offset);
    offset += TrLinearElem::GetSize();
  }
  int nent = TkDBc::Head->GetEntries();
  for (int i = 0; i < nent; i++) {
    TkLadder *lad = TkDBc::Head->GetEntry(i);
    if (!lad) continue;

    int index = 10+lad->GetCrate()*24+lad->GetTdr();
    TrLinearElem *el = GetElem(index);
    if (!el) el = &el0; 

    el->DBToLinear(fLinear+offset);
    offset += TrLinearElem::GetSize();

    for (int j = 0; j < 10; j++) {
      int index = (10+lad->GetCrate()*24+lad->GetTdr())*100+j;
      el = GetElem(index);
      if (!el) el = &el0; 

      el->DBToLinear(fLinear+offset);
      offset += TrLinearElem::GetSize();
    }
  }
  return true;
}


bool TrLinearDB::LinearToDB() {
  if (fClear) Clear();
  int offset = 0;
  
  for (int i = 0; i < 9; i++) {
    TrLinearElem *el = new TrLinearElem;
    el->LinearToDB(fLinear+offset);

    offset += TrLinearElem::GetSize();
    if (el->GetIndex() == i+1) AddElem(i+1, el);
  }
  int nent = TkDBc::Head->GetEntries();
  for (int i = 0; i < nent; i++) {
    TkLadder *lad = TkDBc::Head->GetEntry(i);
    if (!lad) continue;

    int index = 10+lad->GetCrate()*24+lad->GetTdr();
    TrLinearElem *el = new TrLinearElem;
    el->LinearToDB(fLinear+offset);

    offset += TrLinearElem::GetSize();
    if (el->GetIndex() == index) AddElem(index, el);

    for (int j = 0; j < 10; j++) {
      int index = (10+lad->GetCrate()*24+lad->GetTdr())*100+j;
      el = new TrLinearElem;
      el->LinearToDB(fLinear+offset);
      offset += TrLinearElem::GetSize();
      if (el->GetIndex() == index) AddElem(index, el);
    }

  }
  return true;
}

void TrLinearDB::PrintLinear() {
  printf("TrLinearDB::PrintLinear-V content of the linear database:\n");
  for (int i=0; i<GetSize(); i++) 
    printf("%4d %20.10g\n",i,fLinear[i]);
  printf("TrLinearDB::PrintLinear-V content of the linear database --- END\n");
}

bool TrLinearElem::DBToLinear(float* offset) {
  if (!offset) return false;
  int index = 0;
  offset[index++] = (float)fIndex;
  offset[index++] = (float)fSucc;
  offset[index++] = (float)fChisq;
  for (int i = 0; i < 5; i++) offset[index++] = (float)fPars[i];
  return true;
}

bool TrLinearElem::LinearToDB(float* offset) {
  if (!offset) return false;
  fIndex=(int)offset[0];
  fSucc =(int)offset[1];
  fChisq=offset[2];
  for (int i = 0; i < 5; i++) fPars[i] = offset[i+3];
  return true;
}


#include "timeid.h"
#include "commonsi.h"

bool TrLinearDB::SaveInTDV(long int start_time, long int validity,int isReal) {
  if (!DBToLinear()) return false;
  time_t statime = time_t(start_time);
  time_t endtime = time_t(start_time+validity); 
  tm beg, end;
  localtime_r(&statime,&beg);
  localtime_r(&endtime,&end);
  AMSTimeID* tdv = new AMSTimeID(
    AMSID("TrackerLinearity",isReal),beg,end,TrLinearDB::GetLinearSize(),TrLinearDB::GetLinear(),AMSTimeID::Standalone,1
  );
  tdv->UpdateMe();
  tdv->write(AMSDATADIR.amsdatabase);
  if (tdv) delete tdv;
  return true;
} 

void FunctionLinearToDB(){
  if (!TrLinearDB::IsNull()) TrLinearDB::GetHead()->LinearToDB();
}

int TrLinearDB::LoadFromTDV(long int time, int clear, int isReal, bool force)
{
  time_t tt = time_t(time);
  static AMSTimeID* tdv=0;
#pragma omp threadprivate(tdv)
  if (tdv && force) {
    delete tdv;
    tdv = 0;
  }

  if (!fLinear) Init();
  fClear = clear;

 if (!tdv) {
  tm begin;
  tm end;
  begin.tm_isdst = end.tm_isdst = 0;
  begin.tm_sec   = begin.tm_min = begin.tm_hour =
  begin.tm_mday  = begin.tm_mon = begin.tm_year = 0;
  end.  tm_sec   = end.  tm_min = end.  tm_hour =
  end.  tm_mday  = end.  tm_mon = end.  tm_year = 0;


  tdv = new AMSTimeID(
    AMSID("TrackerLinearity",isReal),begin,end,TrLinearDB::GetLinearSize(),TrLinearDB::GetLinear(),AMSTimeID::Standalone,1,FunctionLinearToDB 
  );
 }
  int ret = tdv->validate(tt);

  return ret;
}


ClassImp(TrLinearEtaDB);

int TrLinearEtaDB::Version=3;

TrLinearEtaDB::TrLinearEtaDB(){
  if(Version==2)TDVName="TrackerLinearEta2";
  else          TDVName="TrackerLinearEta3";
  TDVBlock=0;
  TDVSize=0;
  tdvload=0;
  LoadTDVPar(); 
}

void TrLinearEtaDB::Init(){
}

void TrLinearEtaDB::Clear(Option_t* option){
   if(tdvload) {delete tdvload; tdvload=0;}
   if(TDVBlock){delete TDVBlock;TDVBlock=0;}
}

TrLinearEtaDB::~TrLinearEtaDB() {
  Clear();
}

TrLinearEtaDB* TrLinearEtaDB::fHead = 0;

TrLinearEtaDB* TrLinearEtaDB::GetHead() {
  if (fHead ==0){
#pragma omp critical (linearetadb)
      {
    printf("TrLinearEtaDB::GetHead()-V%d TrLinearEtaDB singleton initialization.\n",Version);
    fHead = new TrLinearEtaDB();
  }
  }
  return fHead;
}


int TrLinearEtaDB::SetLinearCluster(int opt, int force){

  if(opt==0){
    if(Version==2)TRCLFFKEY.ClusterCofGOpt=12;//asymmetry-Eta/VAGain-equalization Version
    else          TRCLFFKEY.ClusterCofGOpt=13;//symmetry-Eta+Offset/VAGain-equlization Version
  } 
  else  TRCLFFKEY.ClusterCofGOpt=opt;
  if(force>=1)TRCLFFKEY_DEF::ReadFromFile=0;
  return TRCLFFKEY.ClusterCofGOpt;
}


int TrLinearEtaDB::UnSetLinearCluster(){

  TRCLFFKEY.ClusterCofGOpt=1;
  return TRCLFFKEY.ClusterCofGOpt;
}


int TrLinearEtaDB::RecalAllHitCoo(){
   
  VCon* cont=GetVCon()->GetCont("AMSTrRecHit");
  if (!cont) return -1;
  for (int ii=0;ii<cont->getnelem();ii++){
    TrRecHitR* hit=(TrRecHitR*)cont->getelem(ii);
    if(!hit)continue;
    TrClusterR* clx=hit->GetXCluster();
    TrClusterR* cly=hit->GetYCluster();
    if(cly&&!cly->Used()){//not used YCluster setQ with X/Y clusterQ
      float qhit=(clx)?hit->GetQ(0):hit->GetQ(1);//X/Y ClusterQ
      cly->SetQtrk(qhit);
    }
    if(clx&&!clx->Used()){//not used XCluster setQ with X/Y clusterQ
      float qhit=(clx)?hit->GetQ(0):hit->GetQ(1);//X/Y ClusterQ
      clx->SetQtrk(qhit);
    }
    hit->BuildCoordinate();
  }
  if(cont) delete cont;
  return 0;
}


int TrLinearEtaDB::LoadZCorI(){

 for(int is=0;is<2;is++){
   zcori[is].clear();  
   for(int iz=0;iz<nzcor[is];iz++){//linear model come after
      int pz=zcor[is][iz];
      zcori[is][pz]=iz+pz*100;//iz+pz*100
   }
   for(int iz=0;iz<nzcor2[is];iz++){//parabolic model come first
      int pz=zcor2[is][iz];
      zcori[is][pz]=iz+pz*100+10000;//iz+pz*100+1000
   }
 }
 vamipi.clear();
 for(int iva=0;iva<nvamip;iva++){
   vamipi[vamip[iva]]=iva;
 }
 vamip2i.clear();
 for(int iva=0;iva<nvamip2;iva++){
   vamip2i[vamip2[iva]]=iva;
 }
 return 0;
}


int TrLinearEtaDB::LoadTDVPar(){

    int iblock=0;
    int ifl=0;
    if(!TDVBlock)TDVfile[ifl++]=0;
//----Load Coupling parameters
    for(int is=0;is<2;is++){
      if(TDVBlock)nzcoup[is]=int(TDVBlock[iblock]);
      iblock++;
      for(int iz=0;iz<nMaxZ;iz++){
        if(TDVBlock)zcoup[is][iz]=int(TDVBlock[iblock]);
        iblock++;
        for(int il=0;il<9;il++){
          for(int ip=0;ip<2;ip++){
            if(TDVBlock)parcoup[is][iz][il][ip]=TDVBlock[iblock];
            iblock++;
           }
         }
       }
       if(!TDVBlock)TDVfile[ifl++]=iblock;
     }

//----Load Linear parameters 
    for(int is=0;is<2;is++){
      if(TDVBlock)nzcor[is]=int(TDVBlock[iblock]);
      iblock++;
      for(int iz=0;iz<nMaxZ;iz++){
       if(TDVBlock)zcor[is][iz]=int(TDVBlock[iblock]);
       iblock++;
        for(int il=0;il<9;il++){
          for(int ip=0;ip<3;ip++){
            if(TDVBlock)parcor[is][iz][il][ip]=TDVBlock[iblock];
            iblock++;
           }
         }
       }
       if(!TDVBlock)TDVfile[ifl++]=iblock; 
    }

//-----Load Parabolic parameter0
    for(int is=0;is<2;is++){
      if(TDVBlock)nzcorp[is]=int(TDVBlock[iblock]);
      iblock++;
      for(int iz=0;iz<nMaxZ;iz++){
       if(TDVBlock)zcorp[is][iz]=int(TDVBlock[iblock]);
       iblock++;
        for(int il=0;il<9;il++){
          for(int ip=0;ip<2;ip++){
            if(TDVBlock)parcorp[is][iz][il][ip]=TDVBlock[iblock];
            iblock++;
           }
         }
       }
       if(!TDVBlock)TDVfile[ifl++]=iblock;
    }

//-----Load Parabolic parameter1
    for(int is=0;is<2;is++){
      if(TDVBlock)nzcor2[is]=int(TDVBlock[iblock]);
      iblock++;
      for(int iz=0;iz<nMaxZ;iz++){
       if(TDVBlock)zcor2[is][iz]=int(TDVBlock[iblock]);
       iblock++;
        for(int il=0;il<9;il++){
          for(int ip=0;ip<3;ip++){
            if(TDVBlock)parcor2[is][iz][il][ip]=TDVBlock[iblock];
            iblock++;
           }
         }
       }
       if(!TDVBlock)TDVfile[ifl++]=iblock;
    }

//----Load Asymmetry 
    for(int is=0;is<2;is++){
      for(int il=0;il<9;il++){
        if(TDVBlock)parasym[is][il]=TDVBlock[iblock];
        iblock++;
      }
    }
    if(!TDVBlock)TDVfile[ifl++]=iblock;

//----Load Asymmetry offset(Alignment compensation) 
    for(int is=0;is<2;is++){
      for(int il=0;il<9;il++){
        if(TDVBlock)aligsym[is][il]=TDVBlock[iblock];
        iblock++;
      }
    }
    if(!TDVBlock)TDVfile[ifl++]=iblock;

//----Load MIP parameter
    for(int il=0;il<9;il++){
      for(int ip=0;ip<12;ip++){
        if(TDVBlock)parlaymip[il][ip]=TDVBlock[iblock];
        iblock++;
      }
    }
    if(TDVBlock)nvamip=int(TDVBlock[iblock]);
    iblock++;
    for(int iva=0;iva<2000;iva++){
      if(TDVBlock)vamip[iva]=int(TDVBlock[iblock]);
      iblock++;
      for(int ip=0;ip<12;ip++){
        if(TDVBlock)parvamip[iva][ip]=TDVBlock[iblock];
        iblock++;
      }
    }
    if(!TDVBlock)TDVfile[ifl++]=iblock;

//----Load MIP parameter2
    if(TDVBlock)nvamip2=int(TDVBlock[iblock]);
    iblock++;
    if(TDVBlock){
      nzmip2=int(TDVBlock[iblock]);
      if(nzmip2!=nMaxZ2){nvamip2=nzmip2=-1;}//2nd protection
    }
    iblock++;
    for(int iz=0;iz<nMaxZ2;iz++){
      if(TDVBlock)zmip2[iz]=int(TDVBlock[iblock]);
      iblock++;
    }
    for(int iva=0;iva<2000;iva++){
      if(TDVBlock)vamip2[iva]=int(TDVBlock[iblock]);
      iblock++;
      for(int iz=0;iz<nMaxZ2;iz++){
        if(TDVBlock)parvamip2[iva][iz]=TDVBlock[iblock];
        iblock++;
      }
    }
    if(!TDVBlock)TDVfile[ifl++]=iblock;

//----
    if(TDVBlock)LoadZCorI();
    if(!TDVBlock){
      TDVSize=iblock;
      TDVBlock=new float[TDVSize];
      memset(TDVBlock,0,GetTDVLength());
    }
    return 0;
}


int TrLinearEtaDB::LoadFromFile(const char *file[12]){

//----Load parameters
   for(int ifl=0;ifl<12;ifl++){
     if(!file[ifl])continue;
     ifstream vlfile(file[ifl],ios::in);
     if(!vlfile){
       cerr <<"<---- Error: missing "<<file[ifl]<<"--file !!: "<<endl;
     }
     for(int iblock=TDVfile[ifl];iblock<TDVfile[ifl+1];iblock++){
       if(vlfile.good()&&!vlfile.eof())vlfile>>TDVBlock[iblock];
     }
     vlfile.close();
   }
   LoadTDVPar();
   return 0;
}

int TrLinearEtaDB::PrintTDVBlock(){
  cout<<"<<----Print "<<TDVName<<endl;
  for(int i=0;i<TDVSize;i++){cout<<TDVBlock[i]<<" ";}
  cout<<'\n';
  cout<<"<<----end of Print "<<TDVName<<endl;
  return 0;
}


int TrLinearEtaDB::SaveInTDV(long int start_time, long int end_time, int isReal) {
  time_t statime = time_t(start_time);
  time_t endtime = time_t(end_time);
  tm beg, end;
  gmtime_r(&statime,&beg);
  gmtime_r(&endtime,&end);
  cout << "Begin: " <<(int)statime<<"  " <<asctime(&beg)<<endl;
  cout << "End  : " <<(int)endtime<<"  " <<asctime(&end)<<endl;
  AMSTimeID* tdv = new AMSTimeID(AMSID(TDVName,isReal),beg,end,GetTDVLength(),TDVBlock,AMSTimeID::Standalone,1);
  tdv->UpdateMe();
  int ret=tdv->write(AMSDATADIR.amsdatabase);
  delete tdv;
  return ret;
}


int  TrLinearEtaDB::LoadFromTDV(long int time, int clear, int isReal, bool force){
  time_t tt = time_t(time);
  if (tdvload && force) {
    delete tdvload;
    tdvload = 0;
  }
  
  if (!tdvload) {
   time_t statime = 1;
   time_t endtime = 0;
   tm beg, end;
   gmtime_r(&statime,&beg);
   gmtime_r(&endtime,&end);
   tdvload = new AMSTimeID(AMSID(TDVName,isReal),beg,end,GetTDVLength(),TDVBlock,AMSTimeID::Standalone,1);
  }
  int ret = tdvload->validate(tt);
  LoadTDVPar();  
  return ret;
}


int  TrLinearEtaDB::ConvertLadderId(int tkid,int &il,int &is){
   TkLadder* ladder = TkDBc::Head->FindTkId(tkid);
   if (!ladder) {
     printf("TrLinearEtaDB::CreateIndex-W requested tkid (%+04d) not found in TkDBc. I will use default correction of the whole Tracker.\n",tkid);
     return -1;
   }
   il=ladder->GetLayerJ()-1;
   is=(tkid<0)?0:1;
   return 0;
}

int  TrLinearEtaDB::GetVAId(int tkid, int address){

//---Convert to layer and side first
   int il=0,is=0;
   ConvertLadderId(tkid,il,is);

   int vaid=il*1000+abs(tkid)%100*10+(address/64);
   if(tkid<0)vaid*=-1;
   return vaid;    
} 


double TrLinearEtaDB::GetAsymCorADC(double adc,double ladc,double radc,int tkid){

//---Convert to layer and side first
   int il=0,is=0;
   ConvertLadderId(tkid,il,is);

   double adcc=adc;
   double asympar=parasym[is][il];
   if(asympar>=0&&ladc>0)adcc=adc-ladc*fabs(asympar);//asymmetry-multiplexing
   if(asympar<0 &&radc>0)adcc=adc-radc*fabs(asympar);//antiasymmetry-multiplexing
   return adcc;
}

double TrLinearEtaDB::GetAsymAlign(int tkid){

//---Convert to layer and side first
   int il=0,is=0;
   ConvertLadderId(tkid,il,is);
   return aligsym[is][il];
}

double TrLinearEtaDB::GetMIP0(double* x, double* par){

   return par[0]/(1+exp(-(x[0])/par[1]))+par[3]/(1+exp(-(x[0]-par[4])/par[5]))+par[2];
}


double TrLinearEtaDB::GetMIP1(double* x, double* par){

   double x0=par[10];
   double x1=par[4];
   double x2=par[7];
   if     (x[0]<x0)return (GetMIP0(&x0,par)-par[11])/x0*x[0]+par[11];//linear Q<3
   else if(x[0]<x1)return GetMIP0(x,par);//3=<Q<12
   else if(x[0]<x2)return GetMIP0(&x1,par)+par[6]*(x[0]-x1);//12<=Q<19
   else            return GetMIP0(&x1,par)+par[6]*(x2-x1)+par[8]*pow(x[0]-x2,par[9]);//Q>=19
}


double TrLinearEtaDB::GetQMIP(double q,int vaid){

  if(q<=0)return q;
  double adc=0;
  if(vamipi.find(vaid)!=vamipi.end()){//if find vaid
    int iva=vamipi[vaid];
    adc=GetMIP1(&q,parvamip[iva]);
  }
  else {//else use layer
    int il=(abs(vaid)/1000)%10;
    adc=GetMIP1(&q,parlaymip[il]);
  }
  double adc2=adc*adc;
  return adc2;
}


bool TrLinearEtaDB::IsGoodVA(int tkid,int address){

//---Convert to VAId first
   int vaid=GetVAId(tkid,address);

   bool isgoodva=1;
   if(vamipi.find(vaid)==vamipi.end())isgoodva=0;
   return isgoodva;
}

double TrLinearEtaDB::GetQ2Cor2(int z,int vaid){

   double cor2=1;
//---Search Index
   int izu=-1;
   for(int iz=0;iz<nzmip2;iz++)if(zmip2[iz]==z){izu=iz;break;}

   if(izu>=0&&vamip2i.find(vaid)!=vamip2i.end()){//if find vaid
     int iva=vamip2i[vaid];
     cor2=parvamip2[iva][izu];
   }
   return cor2;    
}


double TrLinearEtaDB::GetLinearQ2(double adc,int tkid,int address,int z){

   if(z<=1)z=1;
//---Convert to VAId first   
   int vaid=GetVAId(tkid,address);

   double q2=adc/GetQMIP(z,vaid)/GetQ2Cor2(z,vaid)*z*z;
//   cout<<"vaid="<<vaid<<" z="<<z<<" zmip="<<GetQMIP(z,vaid)<<" zmip2="<<GetQ2Cor2(z,vaid)<<" q2="<<q2<<endl;
   return q2;
}


double TrLinearEtaDB::GetCorSadc(double sadc,double cadc,double tadc,double dydz,int z,int il,int is){

  if(sadc<=0)return 0;
 
//---Search Index
   int izu=0;
   if     (z<=zcoup[is][0])izu=0;
   else if(z>=zcoup[is][nzcoup[is]-1])izu=nzcoup[is]-1;
   else {
     for(int iz=0;iz<nzcoup[is];iz++){if(zcoup[is][iz]>=z){izu=iz;break;}}
   }

//--Get coupling fraction+subtract
   double dydzu=fabs(dydz);
   if(dydzu>0.42)dydzu=0.42;
   double coupv=parcoup[is][izu][il][0]+parcoup[is][izu][il][1]*dydzu;//linear with angle
   coupv=coupv/(1-2.*coupv);
   double sadcc=sadc-coupv*(cadc-sadc);//Second-Center*Coupling

//---Use thrid strips amplitude as coupling in case of underflow
   if(sadcc<0){
      sadcc=sadc;
      if(tadc>=0)sadcc=sadc-tadc;//Second-Third(As coupling)
   }

   return sadcc;  
}


double TrLinearEtaDB::GetP2PV(double dydz,int z,int il,int is){

//---Search Index
   int izu=0;
   if     (z<=zcorp[is][0])izu=0;
   else if(z>=zcorp[is][nzcorp[is]-1])izu=nzcorp[is]-1;
   else {
     for(int iz=0;iz<nzcorp[is];iz++){if(zcorp[is][iz]>=z){izu=iz;break;}}
   }

//---Get parabolic parameter0
   double dydzu=fabs(dydz);
   if(dydzu>0.5)dydzu=0.5;
   double p2pv=parcorp[is][izu][il][0]+parcorp[is][izu][il][1]*dydzu;
   return p2pv;
}


double TrLinearEtaDB::GetLinearEta2(double cadc,double sadc,double tadc,double dydz,int z,int il,int is){
 
   if(cadc<=0)return -1;//error
 
//---Search Index
   int izu=-1;
   if(z>=100){//has && use index
     izu=z%100;z=(z/100)%100;
     if(izu<0||izu>=nzcor2[is]||zcor2[is][izu]!=z){cerr<<" TrLinearEtaDB::errori2"<<" izu="<<izu<<" z="<<z<<endl;return -2000;}
   }
   else {
     for(int iz=0;iz<nzcor2[is];iz++){if(zcor2[is][iz]==z){izu=iz;break;}}
   }
   if(izu<0)return -1000;

//---Get parabolic parameter0
   double pv=GetP2PV(dydz,z,il,is);

//---Get parabolic parameter1
   double dydzu=fabs(dydz);
   if(dydzu>0.5)dydzu=0.5;
   double fpar0=parcor2[is][izu][il][0];
   double fpar=parcor2[is][izu][il][0]+parcor2[is][izu][il][1]*dydzu+parcor2[is][izu][il][2]*dydzu*dydzu;

//---Linearized Eta by Parabolic model with iterations
   if(sadc<=0)sadc=0;
   double xcoo=sadc/(sadc+cadc);//raw coo
   double kv=(fpar-1)*(0.5-xcoo)*(pv-xcoo)*(-4)/pow(0.5-pv,2)+1;
   if(fpar0>1)kv=(fpar-1)*(1-2*xcoo)*(1-xcoo/pv)+1;
   double xcoosk=sadc/(sadc+cadc*kv);//linearized coo
   for(int ii=0;ii<6;ii++){
      kv=(fpar-1)*(0.5-xcoosk)*(pv-xcoosk)*(-4)/pow(0.5-pv,2)+1;//scaling factor
      if(fpar0>1)kv=(fpar-1)*(1-2*xcoosk)*(1-xcoosk/pv)+1;
      double sadcc=sadc;
      sadcc=GetCorSadc(sadc,(cadc*kv),tadc,dydz,z,il,is);//second strip subtract coupling
      xcoosk=sadcc/(sadc+cadc*kv);
   }

   return xcoosk;
}


double TrLinearEtaDB::GetLinearEta1(double cadc,double sadc,double tadc,double dydz,int z,int il,int is){

   if(cadc<=0)return -1;//error

//---Search Index
   int izu=-1;
   if(z>=100){//has && use index
     izu=z%100;z=(z/100)%100;
     if(izu<0||izu>=nzcor[is]||zcor[is][izu]!=z){cerr<<" TrLinearEtaDB::errori1"<<" izu="<<izu<<" z="<<z<<endl;return -2000;}
   }
   else {
     for(int iz=0;iz<nzcor[is];iz++){if(zcor[is][iz]==z){izu=iz;break;}}
   }
   if(izu<0)return -1000;

//---Get saturation parameters for seed strip
   double dydzu=fabs(dydz);
   if(dydzu>0.5)dydzu=0.5;
   double fpar=parcor[is][izu][il][0]+parcor[is][izu][il][1]*dydzu+parcor[is][izu][il][2]*dydzu*dydzu;//pol2 with angle

//---Linearized Eta by Linear model with iterations
   if(sadc<=0)sadc=0;
   double xcoo=sadc/(sadc+cadc);//raw coo
   double cv=(fpar-1)*(1-2.*xcoo)+1;
   double xcool=sadc/(sadc+cadc*cv);//linearized coo
   for(int ii=0;ii<6;ii++){
     cv=(fpar-1)*(1-2.*xcool)+1;//scaling factor
     double sadcc=sadc;
     sadcc=GetCorSadc(sadc,(cadc*cv),tadc,dydz,z,il,is);//second strip subtract coupling
     xcool=sadcc/(sadc+cadc*cv);
   }

   return xcool;
}


bool TrLinearEtaDB::IsValid(){

   return (zcori[0].size()>0&&zcori[1].size()>0);
}


int TrLinearEtaDB::GetZCorI(int z, int is, int iz[2]){

   if(zcori[is].size()==0)return -1;

   map<int, int>::iterator it=zcori[is].lower_bound(z);//z_it>=z
   if(it==zcori[is].end()){//z_it<z
     it--;
     iz[0]=iz[1]=(*it).second;
   }
   else if((*it).first==z){//z_it==z
     iz[0]=iz[1]=(*it).second;
   }
   else {//z_it>z
     iz[1]=(*it).second;
     if(it!=zcori[is].begin())it--;
     iz[0]=(*it).second;
   }
   return 0;
}


double TrLinearEtaDB::GetLinearEta(double cadc,double sadc,double tadc,double dydz,int z,int il,int is){

  double eta=-1000;

//---Get Corrected Z
  int iz[2]={-1,-1};
  int ret=GetZCorI(z,is,iz);
  if(ret<0)return eta;

//---Get Eta
  if(iz[0]==iz[1]){//find one
    if((iz[0]/10000)%10>=1)eta=GetLinearEta2(cadc,sadc,tadc,dydz,iz[0],il,is);//Parabolic model
    else                   eta=GetLinearEta1(cadc,sadc,tadc,dydz,iz[0],il,is);//Linear model
  }
  else {//find two
    int uz[2]={0};
    double ueta[2]={-1000,-1000};
    for(int iu=0;iu<2;iu++){
      uz[iu]=(iz[iu]/100)%100;
      if((iz[iu]/10000)%10>=1)ueta[iu]=GetLinearEta2(cadc,sadc,tadc,dydz,iz[iu],il,is);//Parabolic model
      else                    ueta[iu]=GetLinearEta1(cadc,sadc,tadc,dydz,iz[iu],il,is);//Linear model
    }
    double ww0=uz[1]-z,ww1=z-uz[0];
    if(ww0<=0||ww1<=0){
      cerr<<"  TrLinearEtaDB::errorw"<<" ww0="<<ww0<<" ww1="<<ww1<<endl;
      eta=-2000;
    }
    else eta=(ueta[0]*ww0+ueta[1]*ww1)/(ww0+ww1);
  }
  return eta;
}



double TrLinearEtaDB::GetLinearEta(double cadc,double sadc,double tadc,double dydz,int z,int tkid){

//---Convert to layer and side first
   int il=0,is=0;
   ConvertLadderId(tkid,il,is);

   return GetLinearEta(cadc,sadc,tadc,dydz,z,il,is);
}


double TrLinearEtaDB::GetLinearX(double cpos,double spos,double cadc,double sadc,double tadc,double dydz,int z,int tkid){

//---Get Eta   
   double eta=GetLinearEta(cadc,sadc,tadc,dydz,z,tkid);
   double xcoo=cpos+(spos-cpos)*eta;
   return xcoo;
}

//--------------
ClassImp(TrLinearXEtaDB);

int TrLinearXEtaDB::Version=1;

TrLinearXEtaDB::TrLinearXEtaDB(){
  if(Version==0)TDVName="TrackerLinearXEta";
  else          TDVName="TrackerLinearXEta1";
  TDVBlock=0;
  TDVSize=0;
  tdvload=0;
  for(int ip=0;ip<nPatt;ip++){//ipatt
    for(int iz=0;iz<nMaxZ;iz++){
      for(int il=0;il<9;il++){
        for(int ik=0;ik<nMaxK;ik++){
          for(int ia=0;ia<nMaxA;ia++){
             splinec[ip][iz][il][ik][ia]=0;
           }
         }
       }
     }
  }
  LoadTDVPar();
}

void TrLinearXEtaDB::Init(){
}

void TrLinearXEtaDB::Clear(Option_t* option){
   if(tdvload) {delete tdvload; tdvload=0;}
   if(TDVBlock){delete TDVBlock;TDVBlock=0;}
   for(int ip=0;ip<nPatt;ip++){//ipatt
     for(int iz=0;iz<nMaxZ;iz++){
       for(int il=0;il<9;il++){
         for(int ik=0;ik<nMaxK;ik++){
           for(int ia=0;ia<nMaxA;ia++){
              if(splinec[ip][iz][il][ik][ia])delete splinec[ip][iz][il][ik][ia];
              splinec[ip][iz][il][ik][ia]=0;
            }
          }
        }
      } 
   }
}

TrLinearXEtaDB::~TrLinearXEtaDB() {
  Clear();
}

TrLinearXEtaDB* TrLinearXEtaDB::fHead = 0;

TrLinearXEtaDB* TrLinearXEtaDB::GetHead() {
  if (fHead ==0) {
#pragma omp critical (linearxetadb)
      {
    printf("TrLinearXEtaDB::GetHead()-V%d TrLinearXEtaDB singleton initialization.\n",Version);
    fHead = new TrLinearXEtaDB();
  }
 }
      return fHead;
}

int TrLinearXEtaDB::LoadZCorI(){

 for(int ip=0;ip<nPatt;ip++){
   zcori[ip].clear();
   for(int iz=0;iz<nzcor[ip];iz++){//linear model come after
      int pz=zcor[ip][iz];
      zcori[ip][pz]=iz+pz*100;//iz+pz*100
   }
 }
 return 0;
}

int TrLinearXEtaDB::LoadTDVPar(){

  int iblock=0;
  int ifl=0;
  if(!TDVBlock)TDVfile[ifl++]=0;
  else         iblock=TDVfile[ifl++];
 
//----Load Asymmetry 
  for(int il=0;il<9;il++){
    if(TDVBlock)parasym[il]=TDVBlock[iblock];
    iblock++;
  }
  if(!TDVBlock)TDVfile[ifl++]=iblock;
  else         iblock=TDVfile[ifl++];

//----Load Asymmetry offset(Alignment compensation) 
  for(int il=0;il<9;il++){
    if(TDVBlock)aligsym[il]=TDVBlock[iblock];
    iblock++;
  }
  if(!TDVBlock)TDVfile[ifl++]=iblock;
  else         iblock=TDVfile[ifl++];

//----Load Parameters
  char sname[100];
  for(int ip=0;ip<nPatt;ip++){//ipatt
    int nxcorn=nMaxP;
    if(TDVBlock)nxcorn=nxcor[ip]=int(TDVBlock[iblock]);
    iblock++;
    for(int ix=0;ix<nxcorn;ix++){
      if(TDVBlock)xcor[ip][ix]=TDVBlock[iblock];
      iblock++;
    }
    int nmaxzn=nMaxZ;
    if(TDVBlock)nmaxzn=nzcor[ip]=int(TDVBlock[iblock]);
    iblock++;
    for(int iz=0;iz<nmaxzn;iz++){//iz
       if(TDVBlock)zcor[ip][iz]=int(TDVBlock[iblock]);
       iblock++;
       for(int il=0;il<9;il++){//il
         int nk=TDVBlock?int(TDVBlock[iblock]):nMaxK;
         iblock++; 
         for(int ik=0;ik<nk;ik++){//type
           int uk=TDVBlock?int(TDVBlock[iblock]):ik;
           iblock++;
           int ndxzn=nMaxA;
           if(TDVBlock)ndxzn=ndxz[ip][iz][il][uk]=int(TDVBlock[iblock]);
           iblock++;
           for(int ia=0;ia<ndxzn;ia++){//angle
             if(TDVBlock)udxz[ip][iz][il][uk][ia]=TDVBlock[iblock];
             iblock++;
             for(int ix=0;ix<nxcorn;ix++){
               if(TDVBlock)ycor[ip][iz][il][uk][ia][ix]=TDVBlock[iblock];
               iblock++;
             }
             if(TDVBlock){
               sprintf(sname,"trlxspline%dz%dl%dk%da%d",ip,iz,il,uk,ia);
               if(splinec[ip][iz][il][uk][ia]){delete splinec[ip][iz][il][uk][ia];}
               splinec[ip][iz][il][uk][ia]=new TSpline3(sname,xcor[ip],ycor[ip][iz][il][uk][ia],nxcor[ip],"b1e1",0,0);
              }
            }
          }
        }
     }
     if(!TDVBlock)TDVfile[ifl++]=iblock;
     else         iblock=TDVfile[ifl++];
  }
  

//----
  if(TDVBlock)LoadZCorI();
  if(!TDVBlock){
    TDVSize=iblock;
    TDVBlock=new float[TDVSize];
    memset(TDVBlock,0,GetTDVLength());
  }
  return 0;
}

int TrLinearXEtaDB::LoadFromFile(const char *file[6]){

//----Load parameters
  for(int ifl=0;ifl<6;ifl++){
     if(!file[ifl])continue;
     ifstream vlfile(file[ifl],ios::in);
     if(!vlfile){
       cerr <<"<---- Error: missing "<<file[ifl]<<"--file !!: "<<endl;
     }
     for(int iblock=TDVfile[ifl];iblock<TDVfile[ifl+1];iblock++){
       if(vlfile.good()&&!vlfile.eof())vlfile>>TDVBlock[iblock];
     }
     vlfile.close();
   }
   LoadTDVPar();
   return 0;
}

int TrLinearXEtaDB::PrintTDVBlock(){
  cout<<"<<----Print "<<TDVName<<endl;
  for(int i=0;i<TDVSize;i++){cout<<TDVBlock[i]<<" ";}
  cout<<'\n';
  cout<<"<<----end of Print "<<TDVName<<endl;
  return 0;
}

int TrLinearXEtaDB::SaveInTDV(long int start_time, long int end_time, int isReal) {
  time_t statime = time_t(start_time);
  time_t endtime = time_t(end_time);
  tm beg, end;
  gmtime_r(&statime,&beg);
  gmtime_r(&endtime,&end);
  cout << "Begin: " <<(int)statime<<"  " <<asctime(&beg)<<endl;
  cout << "End  : " <<(int)endtime<<"  " <<asctime(&end)<<endl;
  AMSTimeID* tdv = new AMSTimeID(AMSID(TDVName,isReal),beg,end,GetTDVLength(),TDVBlock,AMSTimeID::Standalone,1);
  tdv->UpdateMe();
  int ret=tdv->write(AMSDATADIR.amsdatabase);
  delete tdv;
  return ret;
}

int  TrLinearXEtaDB::LoadFromTDV(long int time, int clear, int isReal, bool force){
  time_t tt = time_t(time);
  if (tdvload && force) {
    delete tdvload;
    tdvload = 0;
  }

  if (!tdvload) {
   time_t statime = 1;
   time_t endtime = 0;
   tm beg, end;
   gmtime_r(&statime,&beg);
   gmtime_r(&endtime,&end);
   tdvload = new AMSTimeID(AMSID(TDVName,isReal),beg,end,GetTDVLength(),TDVBlock,AMSTimeID::Standalone,1);
  }
  int ret = tdvload->validate(tt);
  LoadTDVPar();
  return ret;
}

int  TrLinearXEtaDB::ConvertLadderId(int tkid,int &il,int &is){
   TkLadder* ladder = TkDBc::Head->FindTkId(tkid);
   if (!ladder) {
     printf("TrLinearXEtaDB::CreateIndex-W requested tkid (%+04d) not found in TkDBc. I will use default correction of the whole Tracker.\n",tkid);
     return -1;
   }
   il=ladder->GetLayerJ()-1;
   is=(tkid<0)?0:1;
   return 0;
}

double TrLinearXEtaDB::GetAsymCorADC(double adc,double ladc,double radc,int tkid){

//---Convert to layer and side first
   int il=0,is=0;
   ConvertLadderId(tkid,il,is);

   double adcc=adc;
   double asympar=parasym[il];
   if(asympar>=0&&ladc>0)adcc=adc-ladc*fabs(asympar);//asymmetry-multiplexing
   if(asympar<0 &&radc>0)adcc=adc-radc*fabs(asympar);//antiasymmetry-multiplexing

   return adcc;
}

double TrLinearXEtaDB::GetAsymAlign(int tkid){

//---Convert to layer and side first
   int il=0,is=0;
   ConvertLadderId(tkid,il,is);
   return aligsym[il];
}


bool TrLinearXEtaDB::IsValid(){

   return (zcori[0].size()>0&&zcori[1].size()>0);
}


int TrLinearXEtaDB::GetZCorI(int z, int ip, int iz[2]){

   if(zcori[ip].size()==0)return -1;

   map<int, int>::iterator it=zcori[ip].lower_bound(z);//z_it>=z
   if(it==zcori[ip].end()){//z_it<z
     it--;
     iz[0]=iz[1]=(*it).second;
   }
   else if((*it).first==z){//z_it==z
     iz[0]=iz[1]=(*it).second;
   }
   else {//z_it>z
     iz[1]=(*it).second;
     if(it!=zcori[ip].begin())it--;
     iz[0]=(*it).second;
   }
   return 0;
}


double TrLinearXEtaDB::GetLinearCofG0(double xcoo,int ip,double dxdz,int z,int il,int ik){

   int izu=-1;
   if(z>=100){//has && use index
     izu=z%100;z=(z/100)%100;
     if(izu<0||izu>=nzcor[ip]||zcor[ip][izu]!=z){cerr<<" TrLinearEtaXDB::errori1"<<" izu="<<izu<<" z="<<z<<endl;return -2000;}
   }
   else {
     for(int iz=0;iz<nzcor[ip];iz++){if(zcor[ip][iz]==z){izu=iz;break;}}
   }
   if(izu<0)return -1000;
  
//------find angle
   dxdz=fabs(dxdz);
   int ual=0,uah=0;
   double wal=1,wah=1;
   int iamax=ndxz[ip][izu][il][ik]-1;
   if     (dxdz<=udxz[ip][izu][il][ik][0])ual=uah=0;
   else if(dxdz>=udxz[ip][izu][il][ik][iamax])ual=uah=iamax;
   else {
     for(int ia=0;ia<iamax;ia++){
       if(dxdz>=udxz[ip][izu][il][ik][ia]&&dxdz<udxz[ip][izu][il][ik][ia+1]){
         ual=ia;uah=ia+1;
         wal=udxz[ip][izu][il][ik][uah]-dxdz;
         wah=dxdz-udxz[ip][izu][il][ik][ual];
         break;
       }
     }
   }

//------correction value
   double xcoor=fabs(xcoo);
   if(xcoor>0.5)xcoor=0.5;
   double dcool=splinec[ip][izu][il][ik][ual]->Eval(xcoor);
   double dcooh=splinec[ip][izu][il][ik][uah]->Eval(xcoor);
   double dcoo=(wal*dcool+wah*dcooh)/(wal+wah);
   double xcooc=fabs(xcoo)+dcoo;
   if(xcoo<0)xcooc=-xcooc;
   return xcooc;
}



double TrLinearXEtaDB::GetLinearCofG(double xcoo,int upatt,double dxdz,int z,int tkid,int striptype){

//-----Covert Id
  striptype=striptype%1000;
  int ip=upatt-2;
  int ik=-1;
  if     (striptype==102||striptype==202)ik=0;//niters=1(2)/K5/type=2
  else if(striptype==113)                ik=1;//niters=1   /K7/type=3
  else if(striptype==114)                ik=2;//niters=1   /K7/type=4
  else if(striptype== 14)                ik=3;//niters=0   /K7/type=4
  if(ik<0){cerr<<"error striptype="<<striptype<<endl;return xcoo;}

  int il=0,is=0;
  ConvertLadderId(tkid,il,is);
  
//----- 
  double xcool=-1000;

//---Get Corrected Z
  int iz[2]={-1,-1};
  int ret=GetZCorI(z,ip,iz);
  if(ret<0)return xcool;

//---Get Eta
  if(iz[0]==iz[1]){//find one
     xcool=GetLinearCofG0(xcoo,ip,dxdz,iz[0],il,ik);
  }
  else {//find two
    int uz[2]={0};
    double uxcool[2]={-1000,-1000};
    for(int iu=0;iu<2;iu++){
      uz[iu]=(iz[iu]/100)%100;
      uxcool[iu]=GetLinearCofG0(xcoo,ip,dxdz,iz[iu],il,ik);
    }
    double ww0=uz[1]-z,ww1=z-uz[0];
    if(ww0<=0||ww1<=0){
      cerr<<"  TrLinearXEtaDB::errorw"<<" ww0="<<ww0<<" ww1="<<ww1<<endl;
      xcool=-2000;
    }
    else xcool=(uxcool[0]*ww0+uxcool[1]*ww1)/(ww0+ww1);
  }
  return xcool;
}

 
double TrLinearXEtaDB::GetLinearCofG(double adc[5],int patt,int &upatt,double dxdz,int z,int tkid,int striptype){

   upatt=patt;
   if     (upatt>=5)upatt=5;
   else if(upatt<=1)upatt=1;
   const int nm=5;
   int ic=(nm-1)/2;
   if(adc[ic-1]<=0&&adc[ic+1]<=0)upatt=1;
   if(upatt==1)return 0;
//---------
   double xcoo2=(adc[ic-1]>adc[ic+1])?-adc[ic-1]/(adc[ic-1]+adc[ic]):adc[ic+1]/(adc[ic]+adc[ic+1]);//left or right
   bool   tcut3=(adc[ic-1]>0&&adc[ic+1]>0);
   double xcoo3=(-adc[ic-1]+adc[ic+1])/(adc[ic-1]+adc[ic+1])/2.;
   bool   tcut4=tcut3;
   if(adc[ic-1]>adc[ic+1])tcut4=(tcut4&&adc[ic-2]>0);
   else                   tcut4=(tcut4&&adc[ic+2]>0);
   double xcoo4=(adc[ic-1]>adc[ic+1])?-adc[ic-2]/(adc[ic-2]+adc[ic+1]):adc[ic+2]/(adc[ic-1]+adc[ic+2]);
   bool   tcut5=(tcut3&&adc[ic-2]>0&&adc[ic+2]>0);
   double xcoo5=(-adc[ic-2]+adc[ic+2])/(adc[ic-2]+adc[ic+2])/2.;
//--------
   double xcoo=xcoo2;
   if(upatt==5){
     if(tcut5)xcoo=xcoo5;
     else     upatt--;
   }
   if(upatt==4){
     if(tcut4)xcoo=xcoo4;
     else     upatt--;
   }
   if(upatt==3){
     if(tcut3)xcoo=xcoo3;
     else     upatt--; 
   }
   if(upatt==2){
      xcoo=xcoo2;
   }
   double xcool=GetLinearCofG(xcoo,upatt,dxdz,z,tkid,striptype);
   return xcool;
}

double TrLinearXEtaDB::GetLinearX(double adc[5],double pos[5],int patt,int &upatt,double dxdz,int z,int tkid,int striptype){

//---Get Eta
   double cofg=GetLinearCofG(adc,patt,upatt,dxdz,z,tkid,striptype);
   const int nm=5;
   int ic=(nm-1)/2;
   double xcool=pos[ic];
   if     (cofg==0)xcool=pos[ic];
   else if(cofg<0){
     if(adc[ic-1]<=0)xcool=pos[ic];
     else            xcool=pos[ic]+(pos[ic]-pos[ic-1])*cofg;//cpos+pitch*cofg
   }
   else {
     if(adc[ic+1]<=0)xcool=pos[ic];
     else            xcool=pos[ic]+(pos[ic+1]-pos[ic])*cofg;//cpos+pitch*cofg
   }
   return xcool;
}
 
