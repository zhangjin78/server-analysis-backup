// $Id: TrGainDB.C,v 1.23 2017/06/26 21:35:12 oliva Exp $

#include "TrGainDB.h"


#include "TFile.h"

#include "amsstl.h" 
#include "timeid.h"
#include "commonsi.h"
#include "tkdcards.h"


ClassImp(TrGainDB);
ClassImp(TrLadGain);
ClassImp(TrSimSignalDB);


///////////////////////////
// TrGainDB
///////////////////////////


TrGainDB* TrGainDB::fHead = 0;
float* TrGainDB::fLinear = 0;


TrGainDB* TrGainDB::GetHead() {
  if (IsNull()) {
    printf("TrGainDB::GetHead()-V TrGainDB singleton initialization.\n");
    fHead = new TrGainDB(); 
  } 
  return fHead;
}


TrGainDB::~TrGainDB() { 
//  Clear();
  if (!fTrGainHwIdMap.empty()) {
    for (TrGainIt ladgain=fTrGainHwIdMap.begin(); ladgain!=fTrGainHwIdMap.end(); ++ladgain)
      delete (*ladgain).second;
    fTrGainHwIdMap.clear();
  }
  if (fLinear) delete [] fLinear;
  fLinear = 0;
  fHead = 0;
}


bool TrGainDB::Init() {
  if (IsNull()) return false;
  if (fTrGainHwIdMap.empty()) {
    for (int icrate=0; icrate<8; icrate++){
      for (int itdr=0; itdr<24; itdr++) {
        int hwid = icrate*100 + itdr;
        fTrGainHwIdMap[hwid] = new TrLadGain(hwid);
      }
    }
  }
  if (!fLinear) fLinear = new float[GetSize()];
  return true;
}


void TrGainDB::Clear(Option_t* option) {
  Init();
  for (TrGainIt ladgain=fTrGainHwIdMap.begin(); ladgain!=fTrGainHwIdMap.end(); ++ladgain) 
    (*ladgain).second->Clear(option);
  for (int i=0; i<GetSize(); i++) fLinear[i] = 0;
}


void TrGainDB::Info(int verbosity) {
  Init();
  for (TrGainIt ladgain=fTrGainHwIdMap.begin(); ladgain!=fTrGainHwIdMap.end(); ++ladgain)
    (*ladgain).second->Info(verbosity);
}


void TrGainDB::Dump() {
  Init();
  for (TrGainIt ladgain=fTrGainHwIdMap.begin(); ladgain!=fTrGainHwIdMap.end(); ++ladgain) {
    for (int iva=0; iva<16; iva++) {
      (*ladgain).second->Dump(iva);
    }
  }
}


TrLadGain* TrGainDB::FindGainHwId(int hwid) {
  Init();
  TrGainIt gain = fTrGainHwIdMap.find(hwid);
  if (gain!=fTrGainHwIdMap.end()) return gain->second;
  else                            return 0;
}


TrLadGain* TrGainDB::FindGainTkId(int tkid) {
  Init();
  TkLadder* ladder = TkDBc::Head->FindTkId(tkid);
  if (ladder) return FindGainHwId(ladder->GetHwId());
  else        return 0;
}


bool TrGainDB::Save(const char* filename) {
  Init();
  TFile* rootfile = TFile::Open(filename,"recreate");
  if (rootfile==0) return false;
  rootfile->WriteTObject(GetHead());
  return true;
}
 

bool TrGainDB::Load(const char* filename) {
  // Init(); // no memory loss
  TFile* rootfile = TFile::Open(filename,"read");
  if (rootfile==0) return false;
  Load(rootfile);
  rootfile->Close();
  return true;
}


bool TrGainDB::Load(TFile* rootfile) { 
  // Init(); // no memory loss
  if (rootfile==0) return false;
   TrGainDB * fh=dynamic_cast<TrGainDB*>( rootfile->Get("TrGainDB"));
   if(!fh)return false;
  if(fHead)delete fHead;
  fHead = fh;
  printf("TrGainDB::Load-V TrGainDB loaded from file %s.\n",rootfile->GetName());
  return true;
}


bool TrGainDB::LoadFromTxtFile(const char* filename) {
  Init();
  FILE* file = fopen(filename,"r");
  if (file==0) return false;
  printf("TrGainDB::LoadFromTxtFile-V reding Tracker VA Gain parameters from file %s\n",filename);
  while (!feof(file)) {
    int tkid,iva,bit0,bit1,bit2,bit3,bit4;
    float gain,offset,syserr;
    int ret = fscanf(file,"%d%d%d%d%d%d%d%f%f%f",&tkid,&iva,&bit0,&bit1,&bit2,&bit3,&bit4,&gain,&offset,&syserr);
    if (ret<=0) continue;
    TrLadGain* ladgain = (TrLadGain*) FindGainTkId(tkid); 
    if (ladgain==0) continue; 
    ladgain->SetGain(iva,gain);
    ladgain->SetOffset(iva,offset);
    ladgain->SetSysErr(iva,syserr);
    ladgain->SetStatus(iva,int((bit0<<0)|(bit1<<1)|(bit2<<2)|(bit3<<3)|(bit4<<4)));
  }
  fclose(file);
  return false;
}


bool TrGainDB::LoadFromTrParDBFile(const char* filename) {
  Init();
  TrParDB::Head->Load(filename);
  for (int i=0; i<TrParDB::Head->GetEntries(); i++) {
    TrLadPar* ladpar = TrParDB::Head->GetEntry(i);
    if (ladpar==0) continue;
    TrLadGain* ladgain = (TrLadGain*) FindGainHwId(ladpar->GetHwId());
    if (ladgain==0) continue;
    for (int iva=0; iva<16; iva++) {
      int   iside = (iva<10) ? 1 : 0;
      float lad_gain = ladpar->GetGain(iside);
      float va_gain = ladpar->GetVAGain(iva);
      float gain = va_gain*lad_gain;
      ladgain->SetGain(iva,gain);
      ladgain->SetOffset(iva,0);
      ladgain->SetSysErr(iva,0);
      ladgain->SetStatus(iva,0);
      if (gain<0.02) { // old definition of very bad
        ladgain->SetGain(iva,1); 
        ladgain->SetStatus(iva,0x3);
      }
    }
  }
  return true;
}


bool TrGainDB::GainDBToLinear() {
  Init();
  int offset = 0;
  for (TrGainIt ladgain=fTrGainHwIdMap.begin(); ladgain!=fTrGainHwIdMap.end(); ++ladgain) {
    TrLadGain* gain = (*ladgain).second;
    gain->GainDBToLinear(fLinear+offset);
    offset += TrLadGain::GetSize();
  }
  return true;
}


bool TrGainDB::LinearToGainDB() {
  Init();
  int offset = 0;
  for (TrGainIt ladgain=fTrGainHwIdMap.begin(); ladgain!=fTrGainHwIdMap.end(); ++ladgain) {
    TrLadGain* gain = (*ladgain).second;
    gain->LinearToGainDB(fLinear+offset);
    offset += TrLadGain::GetSize();
  }
  return true;
}


void TrGainDB::PrintLinear() {
  Init();
  printf("TrGainDB::PrintLinear-V content of the linear database:\n");
  for (int i=0; i<GetSize(); i++) 
    printf("%4d %20.10g\n",i,fLinear[i]);
  printf("TrGainDB::PrintLinear-V content of the linear database --- END\n");
}


bool TrGainDB::SaveInTDV(long int start_time, long int validity,int isReal) {
  Init();
  time_t statime = time_t(start_time);
  time_t endtime = time_t(start_time+validity); 
  GainDBToLinear();
  tm beg, end;
  localtime_r(&statime,&beg);
  localtime_r(&endtime,&end);
  AMSTimeID* tdv = new AMSTimeID(
    AMSID("TrackerVAGains",isReal),beg,end,TrGainDB::GetLinearSize(),TrGainDB::GetLinear(),AMSTimeID::Standalone,1
  );
  tdv->UpdateMe();
  tdv->write(AMSDATADIR.amsdatabase);
  if (tdv) delete tdv;
  return true;
} 


int TrGainDB::LoadFromTDV(long int time, int isReal) {
  Init();
  time_t tt = time_t(time);
  tm begin;
  tm end;
  begin.tm_isdst = end.tm_isdst = 0;
  begin.tm_sec   = begin.tm_min = begin.tm_hour =
  begin.tm_mday  = begin.tm_mon = begin.tm_year = 0;
  end.  tm_sec   = end.  tm_min = end.  tm_hour =
  end.  tm_mday  = end.  tm_mon = end.  tm_year = 0;
  AMSTimeID *tdv = new AMSTimeID(
    AMSID("TrackerVAGains",isReal),begin,end,TrGainDB::GetLinearSize(),TrGainDB::GetLinear(),AMSTimeID::Standalone,1,FunctionLinearToGainDB 
  );
  int ret = tdv->validate(tt);
  if (tdv) delete tdv;
  return ret;
}


void FunctionLinearToGainDB(){
  if (!TrGainDB::IsNull()) TrGainDB::GetHead()->LinearToGainDB();
}


float TrGainDB::GetGainCorrected(float adc, int tkid, int iva) {
  if ( (iva<0)||(iva>15) ) return 0;
  TrLadGain* ladgain = (TrLadGain*) FindGainTkId(tkid);
  if (ladgain==0) return 0;
  return ladgain->GetGainCorrected(adc,iva);   
}


float TrGainDB::ApplyGain(float adc, int tkid, int iva) {
  if ( (iva<0)||(iva>15) ) return 0;
  TrLadGain* ladgain = (TrLadGain*) FindGainTkId(tkid);
  if (ladgain==0) return 0;
  return ladgain->ApplyGain(adc,iva);
}


TH2D* gain_map = 0;
TH2D* TrGainDB::GetGainHistogram() {
  if (!gain_map) gain_map = new TH2D("gain_map","; ladder number; VA number; gain",192,-0.5,191.5,16,-0.5,15.5);
  if (gain_map)  gain_map->Clear();
  for (int icrate=0; icrate<8; icrate++) {
    for (int itdr=0; itdr<24; itdr++) {
      int iladder = icrate*24 + itdr;
      int hwid = icrate*100 + itdr;
      TrLadGain* ladgain = (TrLadGain*) FindGainHwId(hwid);
      if (ladgain==0) return 0;
      for (int iva=0; iva<16; iva++) {
        float gain = ladgain->GetGain(iva); 
        gain_map->SetBinContent(iladder+1,iva+1,gain);
      }
    }
  }
  return gain_map;
}


bool TrGainDB::IsDefault() {
  for (int icrate=0; icrate<8; icrate++) {
    for (int itdr=0; itdr<24; itdr++) {
      int hwid = icrate*100 + itdr;
      TrLadGain* ladgain = (TrLadGain*) FindGainHwId(hwid);
      if (!ladgain->IsDefault()) return false;
    }
  }
  return true;
}


///////////////////////////
// TrLadGain
///////////////////////////


TrLadGain::TrLadGain(int hwid, float* gain, float* offset, float* syserr, int* status) { 
  Clear(); 
  SetHwId(hwid); 
  SetGain(gain); 
  SetOffset(offset); 
  SetSysErr(syserr); 
  SetStatus(status);        
}


void TrLadGain::Clear(Option_t* option) {
  fHwId = 0;
  for (int iva=0; iva<16; iva++) {
    fGain[iva] = 1;
    fOffset[iva] = 0;
    fSysErr[iva] = 0;
    fStatus[iva] = 0;
  }
  TObject::Clear(option);
}


void TrLadGain::Info(int verbosity) {
  printf(" %10s %3d","HwId: ",GetHwId());
  if (verbosity>0) printf("\n");
  printf(" %10s ","Gain: ");
  for (int iva=0; iva<16; iva++) printf("%8.4f ",GetGain(iva));
  printf("\n");
  if (verbosity>0) { 
    printf(" %10s ","Offset: ");
    for (int iva=0; iva<16; iva++) printf("%8.4f ",GetOffset(iva));
    printf("\n");
    printf(" %10s ","SysErr: ");
    for (int iva=0; iva<16; iva++) printf("%8.4f ",GetSysErr(iva));
    printf("\n");
    printf(" %10s ","Status: ");
    for (int iva=0; iva<16; iva++) printf("%8X ",GetStatus(iva));
    printf("\n");
  }
}


void TrLadGain::Dump(int iva) {
  if ( (iva<0)||(iva>15) ) return;
  printf("%03d %2d %8.4f %8.4f %8.4f %8d\n",GetHwId(),iva,GetGain(iva),GetOffset(iva),GetSysErr(iva),GetStatus(iva));
}


bool TrLadGain::GainDBToLinear(float* offset) {
  if (!offset) return false;
  int index = 0;
  offset[index++] = (float) GetHwId();
  for (int iva=0; iva<16; iva++) offset[index++] = (float) GetGain(iva);
  for (int iva=0; iva<16; iva++) offset[index++] = (float) GetOffset(iva);
  for (int iva=0; iva<16; iva++) offset[index++] = (float) GetSysErr(iva);
  for (int iva=0; iva<16; iva++) offset[index++] = (float) GetStatus(iva);
  return true;
}


bool TrLadGain::LinearToGainDB(float* offset) {
  if (!offset) return false;
  SetHwId((int)offset[0]);
  SetGain(offset+1);
  SetOffset(offset+1+16);
  SetSysErr(offset+1+16+16);
  SetStatus(offset+1+16+16+16);
  return true;
}


float TrLadGain::GetGainCorrected(float adc, int iva) { 
  if ( (iva<0)||(iva>15) ) return 0;
  return (adc + GetOffset(iva))*GetGain(iva);
}


float TrLadGain::ApplyGain(float adc, int iva) {
  if ( (iva<0)||(iva>15) ) return 0;
  return adc/GetGain(iva) - GetOffset(iva); 
}


bool TrLadGain::IsDefault() {
  for (int iva=0; iva<16; iva++) 
    if ( (GetGain(iva)!=1.)||(GetOffset(iva)!=0.)||(GetSysErr(iva)!=0.)||(GetStatus(iva)!=0) ) 
      return false;
  return true;
}


///////////////////////////
// TrSimSignalDB
///////////////////////////
 

TrSimSignalDB* TrSimSignalDB::fHead = 0;
MonoSpline* TrSimSignalDB::fTrSimSignalMap[8*16*24+2] = {0};
float* TrSimSignalDB::fLinear = 0;


TrSimSignalDB::~TrSimSignalDB() {
  CleanMap();
  if (fLinear) delete [] fLinear;
  fLinear = 0;
  fHead = 0;
}


TrSimSignalDB* TrSimSignalDB::GetHead() {
  if (fHead==0) {
    printf("TrSimSignalDB::GetHead()-V TrSimSignalDB singleton initialization.\n");
    fHead = new TrSimSignalDB();
  }
  if (!fLinear) fLinear = new float[GetSize()];
  return fHead;
}


void TrSimSignalDB::CleanMap() {
  for (int id=0; id<8*16*24+2; id++) { 
    if (fTrSimSignalMap[id]) { 
      delete fTrSimSignalMap[id]; 
      fTrSimSignalMap[id] = 0; 
    }
  }
} 


int TrSimSignalDB::LoadFromFile(const char *filename) {
  CleanMap();
  FILE* file = fopen(filename,"r"); 
  while (!feof(file)) {
    int id;
    int n;
    double Z[10] = {0};
    double sqrtADC[10] = {0}; 
    int ret = fscanf(file,"%d %d",&id,&n); 
    if (ret!=2) continue;
    for (int i=0; i<9; i++) {
      int z;
      float val;
      int ret = fscanf(file,"%d %f",&z,&val);
      if (ret!=2) continue;
      if (i<n) {
        Z[i+1] = z;
        sqrtADC[i+1] = val;
      }
    }
    fTrSimSignalMap[id] = new MonoSpline(n+1,Z,sqrtADC);
  }
  CreateAverages();
  Fix();
  return 0;
}


bool TrSimSignalDB::DBToLinear() {
  int index = 0;
  for (int id=0; id<8*24*16; id++) {
    MonoSpline* spline = fTrSimSignalMap[id]; 
    fLinear[index++] = (float) id;
    fLinear[index++] = (float) ((spline)?spline->fN-1:0); 
    for (int i=1; i<10; i++) {
      if ( (spline)&&(i<spline->fN) ) {
        fLinear[index++] = (float) spline->fX[i];
        fLinear[index++] = (float) spline->fY[i];
      }
      else {
        fLinear[index++] = 0.;
        fLinear[index++] = 0.;
      }
    }
  }
  return true;
}


bool TrSimSignalDB::LinearToDB() {
  CleanMap();
  int index = 0;
  for (int id=0; id<8*24*16; id++) { 
    int iva = (int) fLinear[index++];
    (void) iva;
    int n = (int) fLinear[index++];
    double Z[10] = {0};
    double sqrtADC[10] = {0};
    for (int j=1; j<10; j++) {
      Z[j] = (double) fLinear[index++];
      sqrtADC[j] = (double) fLinear[index++];
    }
    fTrSimSignalMap[id] = new MonoSpline(n+1,Z,sqrtADC);
  }
  CreateAverages(); 
  Fix();
  return true;
}


bool TrSimSignalDB::SaveInTDV(long int start_time, long int validity, int isReal) {
  time_t statime = time_t(start_time);
  time_t endtime = time_t(start_time+validity);
  DBToLinear();
  tm beg, end;
  localtime_r(&statime,&beg);
  localtime_r(&endtime,&end);
  AMSTimeID* tdv = new AMSTimeID(AMSID("TrackerSimSignal1",isReal),beg,end,TrSimSignalDB::GetLinearSize(),TrSimSignalDB::GetLinear(),AMSTimeID::Standalone,1);
  tdv->UpdateMe();
  tdv->write(AMSDATADIR.amsdatabase);
  if (tdv) delete tdv;
  return true;
}


int TrSimSignalDB::LoadFromTDV(long int time, int isReal) {
  time_t tt = time_t(time);
  tm begin;
  tm end;
  begin.tm_isdst = end.tm_isdst = 0;
  begin.tm_sec   = begin.tm_min = begin.tm_hour =
  begin.tm_mday  = begin.tm_mon = begin.tm_year = 0;
  end.  tm_sec   = end.  tm_min = end.  tm_hour =
  end.  tm_mday  = end.  tm_mon = end.  tm_year = 0;
  AMSTimeID *tdv = new AMSTimeID(AMSID("TrackerSimSignal1",isReal),begin,end,TrSimSignalDB::GetLinearSize(),TrSimSignalDB::GetLinear(),AMSTimeID::Standalone,1,FunctionLinearToSimSignalDB);
  int ret = tdv->validate(tt);
  if (tdv) delete tdv;
  return ret;
}


MonoSpline* TrSimSignalDB::GetSpline(int icrate, int itdr, int iva) {
  return fTrSimSignalMap[GetIndex(icrate,itdr,iva)];
} 


// fix out-of-fs VA (tmp fix)
static int trsimsignaldb_fix = true; 
static int trsimsignaldb_m602[16] = {2723, 804,2824,2978,1361, 182, 872, 448, 374,1718,1933,2572, 684,1212,1802,2938}; // 1936
static int trsimsignaldb_m702[16] = { 440,  50,  33, 482,2338, 760,1841, 247,1445,  34,1834,1518,2170, 461,  93, 381}; // 2000
static int trsimsignaldb_p614[16] = {1296,3016,1153,2870, 616,2640,2052,1048,2777,2256,1868,1183, 623, 714,1823,1279}; // 2704
static int trsimsignaldb_p714[16] = {1687,2037,1250,1463,1460, 965,2296,2500,2949,2499,2879,1914,  30,1151,2782, 892}; // 2768 
static int trsimsignaldb_p703[ 2] = {2329,2329}; // 1576
static int trsimsignaldb_p603[ 2] = { 355,1257}; // 1648
static int trsimsignaldb_m603[ 2] = {1463,2039}; // 2200
static int trsimsignaldb_m703[ 2] = {1153,1170}; // 2240
static int trsimsignaldb_m713[ 2] = {2550, 673}; // 2344
static int trsimsignaldb_m613[ 2] = { 613, 149}; // 2416
static int trsimsignaldb_p613[ 2] = {1798,1777}; // 2968
static int trsimsignaldb_p713[ 2] = {1747,1976}; // 3008

int TrSimSignalDB::GetIndex(int icrate, int itdr, int iva) { 
  int id = iva + itdr*16 + icrate*16*24; 
  if (trsimsignaldb_fix) {
  if      ( (id>=1936)&&(id<1936+16) ) id = trsimsignaldb_m602[id-1936];
    else if ( (id>=2000)&&(id<2000+16) ) id = trsimsignaldb_m702[id-2000];
    else if ( (id>=2704)&&(id<2704+16) ) id = trsimsignaldb_p614[id-2704];
    else if ( (id>=2768)&&(id<2768+16) ) id = trsimsignaldb_p714[id-2768];
    else if ( (id>=1576)&&(id<1576+ 2) ) id = trsimsignaldb_p703[id-1576];
    else if ( (id>=1648)&&(id<1648+ 2) ) id = trsimsignaldb_p603[id-1648];
    else if ( (id>=2200)&&(id<2200+ 2) ) id = trsimsignaldb_m603[id-2200];
    else if ( (id>=2240)&&(id<2240+ 2) ) id = trsimsignaldb_m703[id-2240];
    else if ( (id>=2344)&&(id<2344+ 2) ) id = trsimsignaldb_p713[id-2344];
    else if ( (id>=2416)&&(id<2416+ 2) ) id = trsimsignaldb_p613[id-2416];
    else if ( (id>=2968)&&(id<2968+ 2) ) id = trsimsignaldb_m613[id-2968];
    else if ( (id>=3008)&&(id<3008+ 2) ) id = trsimsignaldb_m713[id-3008];
  }
  if (icrate==-1) id = 8*24*16 + int(iva/10);
  return id; 
}


static int trsimsignaldb_dead[69] = {
  170, // +109 VA 10 
  298,299,300,301,302,303, // +405 X-side 
  496,497,498,499,500,501,502,503,504,505, // -202 Y-side
  560,561,562,563,564,565,566, // // -302 VA 0,1,2,3,4,5,6
  730,731,732,733,734,735, // -405 X-side 
  813,815, // -209 VA 13,15
  1267,1268,1269,1270,1271,1272,1273,1275,1278, // +214 VA 3,4,5,6,7,8,9,11,14
  1328,1329,1330,1331,1332,1333,1334,1335, // +314 Y-side
  1706,1707,1708,1711, // -805 10,11,12,15
  1790, // +504 VA 14 
  2171,2172,2173,2174,2175, // -505 X-side
  2391, // -609 VA 7
  2556,2557,2559, // -512 VA 12,13,15
  2747,2750, // +510 VA 11,14
  2939,2940,2942,2943 // +511 VA 11,12,14,15 
};

 
double TrSimSignalDB::GetValue(double q, int icrate, int itdr, int iva, bool inverse) {
  // check dead (low-gain?)
  int id = iva + itdr*16 + icrate*16*24;
  // VC if ( (!inverse)&&(find(begin(trsimsignaldb_dead),end(trsimsignaldb_dead),id)!=end(trsimsignaldb_dead)) ) return 0;
  if ( (!inverse)&&(AMSbins(trsimsignaldb_dead,id,sizeof(trsimsignaldb_dead)/sizeof(trsimsignaldb_dead[0]))>0) ) return 0;
  MonoSpline* spline = GetSpline(icrate,itdr,iva); 
  // chek bad determination, use average instead
  if ( (!spline)||(spline->fN<8) ) spline = GetSpline(-1,-1,iva); 
  if (!spline) return 0;  
  double k = TRMCFFKEY.NormAdjAfter[1-int(iva/10)];
  return (inverse) ? spline->EvalInverse(q/k) : spline->Eval(q)*k;
}


double TrSimSignalDB::GetValue(double q, int tkid, int iva, bool inverse) {
  TkLadder* ladder = TkDBc::Head->FindTkId(tkid);
  return GetValue(q,ladder->GetCrate(),ladder->GetTdr(),iva,inverse); 
}


void TrSimSignalDB::CreateAverages() {
  // loop on sides
  for (int is=0; is<2; is++) {
    double x[10] = {0};
    double y[10] = {0};
    // loop on points 
    for (int ip=0; ip<10; ip++) {
      int   n = 0;
      // take values
      for (int icrate=0; icrate<8; icrate++) {  
        for (int itdr=0; itdr<24; itdr++) { 
          for (int iva=10*is; iva<10+6*is; iva++) {       
            MonoSpline* spline = GetSpline(icrate,itdr,iva);
            if (spline->fN!=10) continue; 
            n++;
            x[ip] += spline->fX[ip];
            y[ip] += spline->fY[ip]; 
          }
        }
      }
      if (n<=0) continue; 
      x[ip] /= n;
      y[ip] /= n;
    }
    int id = 8*24*16 + is;  
    fTrSimSignalMap[id] = new MonoSpline(10,x,y);  
  }
}


void TrSimSignalDB::Fix() {
  MonoSpline* spline = 0;
  spline = GetSpline(6,10,11); if ( (spline)&&(!spline->IsMonotonic()) ) spline->RemoveKnot(9);
  spline = GetSpline(6,21,13); if ( (spline)&&(!spline->IsMonotonic()) ) spline->RemoveKnot(3);
}


void TrSimSignalDB::Dump() const {
  for (int id=0; id<8*16*24+2; id++) {
    cout << id << endl;
    if (fTrSimSignalMap[id]) fTrSimSignalMap[id]->Dump();
  }
}


void FunctionLinearToSimSignalDB() {
  if (!TrSimSignalDB::IsNull()) TrSimSignalDB::GetHead()->LinearToDB();
}

