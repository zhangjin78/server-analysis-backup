//  $Id: TrChargeYJ.h,v 1.25 2025/05/12 18:41:48 choutko Exp $
#ifndef __TrChargeYJ__
#define __TrChargeYJ__

#include <cmath>
#include <vector>
#include <map>
#include <algorithm>
#include "typedefs.h"
#include <cstring>

class AMSTimeID;

class TrChargeYJDB {
public:
  static const int SCharge=1;
  static const int MCharge=26;
  static const int nMaxZ=25;
  static const int nLayer=9;
  static const int nStrip=7;
  static const int nType=4;
  static const int nXY=2;
  static const int nCor=2;
  static const int nint=8;
  static const int npol=3;
  static const int nelosspar=10;
  static const int nLadM=28;
  static const int nVA=10;
  static const int nVApar=9;
  static const int nIncxpar=20;
  static const int nCofxpar=20;
  static const int nChannel=1024;
  static const int nChapar=5;
  static const int nSCha=5;//# of special channel
  static const int nBetapar=5;
  static const int nRigpar=14;
  static const int nResx=10;
  static const int nOflow=6;
  static const int nResy=13;
//========================Energy Loss for x and y=====================
//---------Conversion adc to Q
//---x side
  int nzelossx[nType][nStrip];
  int zelossx[nType][nStrip][nMaxZ];
  double elossxpar[nType][nStrip][nMaxZ][nLayer][nint][npol][nelosspar];
//---y side
  int nzelossy[nStrip];
  int zelossy[nStrip][nMaxZ];
  double elossypar[nStrip][nMaxZ][nLayer][nint][npol][nelosspar];

//========================Incx=======================================
//---------Correct dxdz only
  int nzincx[nXY][nStrip][nOflow];
  int zincx[nXY][nStrip][nOflow][nMaxZ];
  double incxpar[nXY][nStrip][nOflow][nMaxZ][nLayer][nIncxpar];

//=======================Energy loss2 for y==========================
  int nzeloss2y[nStrip];
  int zeloss2y[nStrip][nMaxZ];
  double eloss2ypar[nStrip][nMaxZ][nLayer][nint][npol][nelosspar];

//=======================VA equalization=============================
  double vapar[nStrip][nLayer][nXY][nLadM][nVA][nVApar];
  double va2par[nStrip][nLayer][nXY][nLadM][nVA][nVApar];
//---------64 channels correction for y side
  int nzchannel[nStrip];
  int zchannel[nStrip][nMaxZ];
  double channelpar[nStrip][nMaxZ][nLayer][nChapar];
//---------five channels on edge 
  int nzschannel[nStrip];
  int zschannel[nStrip][nMaxZ];
  double schannelpar[nStrip][nMaxZ][nLayer][nSCha];

//========================Cof========================================
////---------Correct cof 
  int nzcofx[nXY][nXY][nStrip];
  int zcofx[nXY][nXY][nStrip][nMaxZ];
  double cofxpar[nXY][nXY][nStrip][nMaxZ][nLayer][nCofxpar];

//======================-Rigidity Correction=========================
  int nzbeta[nXY][nStrip];
  int zbeta[nXY][nStrip][nMaxZ];
  double betapar[nXY][nStrip][nMaxZ][nLayer][nBetapar];  
  int nzrig[nXY][nStrip];
  int zrig[nXY][nStrip][nMaxZ];
  double rigpar[nXY][nStrip][nMaxZ][nLayer][nRigpar];  

//Resolution to combine strips
  double resxpar[nLayer][nStrip][nOflow][nResx];
  double resypar[nLayer][nStrip][nResy];
//Resolution to combine x and y
  double rescxpar[nLayer][nResx];
  double rescypar[nLayer][nResy];

//=========================QStatus==================================
//--------channel occupancy
  int ChaLowOccu[nLayer][nLadM][nChannel];
  int ChaHighOccu[nLayer][nLadM][nChannel];
  int ChaMean[nLayer][nLadM][nChannel];

public:
  const char *TDVName;
  int TDVfile[20];
  float *TDVBlock;
  int    TDVSize;
  int    GetTDVLength(){return TDVSize*sizeof(float);}
  AMSTimeID *tdvload;
  int    LoadTDVPar();
  int    isreal;

public:
   //! Constructor (declared public because of ROOT I/O), not to be used
  TrChargeYJDB(int HisReal = 1);
  //! Destructor
  ~TrChargeYJDB();
  /// Version
  static int Version; 
  static TrChargeYJDB* fHead;
  static void loadtdvpar(){
	  if(fHead){
		  fHead->LoadTDVPar();
          }
  }
  //! Get self-pointer
  static TrChargeYJDB* GetHead(int HisReal = 1);
  //! Head Load TDVPar
  static void HeadLoadTDVPar(int HisReal = 1){GetHead(HisReal)->LoadTDVPar();}
  //! Needed init (loading interface for GBATCH)
  void Init();
  //! Clean up the table database
  void Clear();
  //! Load from file
  int  LoadFromFile(const char *file[5],int isReal = 1);//read data from file->TDVBlock
  //! Print TDV Block
  int  PrintTDVBlock();
  //! Save in TDV
  int  SaveInTDV(long int start_time, long int end_time, int isReal = 1);
  //! Load from TDV
  int  LoadFromTDV(long int time, int clear  = 0,
                                  int isReal = 1, bool force = false);
  //! Is Valid
  bool IsValid(){return ((GetTDVLength()>0)&&(nzelossy[0]>0));}
};

//////////////////////////////////////////////////////////////////////////
//          DataBase to calibarte dependence on time
/////////////////////////////////////////////////////////////////////////
class TrChargeYJTimeElem{
public:
  static const int nLayer=9;
  static const int nStrip=5;
  static const int nXY=2;
  static const int nTimepar=9;
  double timepar[nXY][nStrip][nLayer][nTimepar]; 
  TrChargeYJTimeElem(){std::memset(timepar,0,sizeof(timepar));}
};

class TrChargeYJTimeDB{
public:
  static const int nMaxElem=50;
  int nElem;
  map<pair<unsigned int,unsigned int>, TrChargeYJTimeElem> elems;
  TrChargeYJTimeElem *pelem;
  int LoadpElem();
public:
  unsigned int ptime;
  unsigned int prun;
  const char *TDVName;
  int TDVfile[20];
  float *TDVBlock;
  int    TDVSize;
  int    GetTDVLength(){return TDVSize*sizeof(float);}
  AMSTimeID *tdvload;
  int    LoadTDVPar();
public:
  //! Constructor (declared public because of ROOT I/O), not to be used
  TrChargeYJTimeDB();
  //! Destructor
  ~TrChargeYJTimeDB();
  /// Version
  static int Version;
  static TrChargeYJTimeDB* fHead;
   static void loadtdvpar(){
       if(fHead){
           fHead->LoadTDVPar();
	   fHead->LoadpElem();
       }
   }

  //! Get self-pointer
  static TrChargeYJTimeDB* GetHead();
  //! Head Load TDVPar
  static void HeadLoadTDVPar(){GetHead()->LoadTDVPar();}
  //! Needed init (loading interface for GBATCH)
  void Init();
  //! Clean up the table database
  void Clear();
  //! Load from file
  int  LoadFromFile(const char *file[1]);//read data from file->TDVBlock
  //! Print TDV Block
  int  PrintTDVBlock();
  //! Save in TDV
  int  SaveInTDV(long int start_time, long int end_time, int isReal = 1);
  //! Load from TDV
  int  LoadFromTDV(long int time, unsigned int run=0, int clear  = 0,
                                  int isReal = 1, bool force = false);
  //! Is Valid
  bool IsValid(){return ((GetTDVLength()>0)&&(elems.size()>0));}
};

/////////////////////////////////////////////////////////////////////////
/// TrChargeYJ
/*!
 * new Track Charge Calibration by Y. Jia 
 * \author yjia@cern.ch qyan@cern.ch
 */
//////////////////////////////////////////////////////////////////////////
class TrChargeYJ{
private:
  static TrChargeYJ* fHead;
public:
  enum CorrectOpt{
    /// Path Length
    kPath   =0x1,
    /// Energy Loss
    kEloss  =0x2,
    /// VA equalization
    kGain   =0x4,
    /// inclination of x direction for y side
    kIncx   =0x8,
    /// Recorrect Energy Loss for y side
    kEloss2 =0x10,
    /// Recorrect VA
    kGain2  =0x20,
    /// VA Channel correction
    kChannel=0x40,
    /// COG of x direction 
    kCofx   =0x80,
    /// COG of y direction  
    kCofy   =0x100,
    /// Beta correction
    kBeta   =0x200,
    /// Rigidity correction
    kRig    =0x400,
    /// Time correction
    kTime   =0x800,
    /// eta parameter interpolation
    kPow    =0x1000,
    /// Do not put any smearing to MC charge 
    kNosmear=0x2000,
    /// Use single strip for proton and helium
    kSingle =0x4000
  };
  static int DefaultCorOpt; 
public:
  void Init();
  TrChargeYJ(){Init();}
  static TrChargeYJ* GetHead();
  int    GetIdsoft(int tkid,int seedadd,int istrip);
  int    ConvertLadderId(int tkid,int seedadd,int &il,int &ixy);
  int    ConvertIdsoft(int idsoft,int &tkid,int &seedadd,int &il,int &ixy,int &istrip);
  int    Get_VAId(int idsoft);
  bool   IsSaturated(int stripx,int istrip);
  double Getpol(float x,float par[],int npar);
  double Getgaus(float x,float par1,float par2,float par3);
  double GetSigmoid(float x,float par1,float par2);
  double SwitchStrip(int s1,int s2,float res[],int nw);
  void   SetWeight(int tkid,int seedadd,float wei[],float q[],float res[],int nw,int noverflow,float adc[7],float tkcof);
  void   SetXYWeight(int il,float wei[],float q[],float newq[],float res[],int nw,int stripx,int qstatus[2]);
  void   SetInnerLayerWeight(int ixy,vector<pair<int,int> > Lstripx,int &patt,float mean,vector<float> &vwei);
  float  GetInnerXYWeight(vector<float> wei,vector<int> &Qstatus);
  void   SetXStripRes(int il,float q[],float res[],int nq,int noverflow);
  void   SetYStripRes(int il,float q[],float res[],int nq);
  void   SetXYRes(int il,float *q,float *res,int stripx);
  double AddSmear(int il,int type=2);
  double GetSmearXYQ(int tkid,int seedadd,float tkq);
  double GetSmearXYIndexWidth(int il,int &dir,int Lindex,int ich);
  double GetSmearXYIndexQ(float tkq,double wid,double smear,int dir,int ch);
  double AdjustMCXYCor(float tkq);
  double GetCorQ(float rawsignal,int idsoft,float tkcof[2],float dadz[2],int stripx,float beta,float rig,float q0,int optc);
  double GetCorPHeTotalQ(double signal,int il,int ixy);
  float  GetCombineStripQ(int tkid,int seedadd,float adc[7],float qs[7],float tkcof[2],float dadz[2],int stripx,float beta=1,float rig=0,int opt=DefaultCorOpt);
  float  GetCombineLayerQ(int tkid,float clq[2],int stripx,int qstatus[2]);
  float  GetQMean(vector<float> ql,float &rms,int &n,int opt=0);
  float  GetWeightedQMean(vector<float> ql,vector<float> wei,float &rms,int &n);
  float  GetInnerQRMS(vector<float> Qx,vector<float> wx,vector<float> Qy,vector<float> wy);
  void   GetQEdge(vector<float> ql,int edgei[2]);
  void   RemoveBadStatusQ(vector<float> &InQs2,vector<pair<int,int> > &Lstripx,vector<int> &Qstatus,int nbadhit);
  void   RemoveTailQ(int ixy,vector<float> &InQs2,vector<pair<int,int> > &Lstripx,float mean,int iloop);
  double PathCor(int idsoft,float dadz[2],float signal);
  double ElossCor(int idsoft,float tkcof[2],float dadz[2],int stripx,double signal,float q0);
  double GetElossXCor(int il,int istrip,float mapcof,float mapinc,int stripx,double signal);
  double GetElossYCor(int il,int istrip,float tkcof,float dadz[2],double signal,float q0);
  double ElossYConvert(int il,int istrip,float tkcof,float mapinc,float q0);
  double GetElossXIndexCor(int il,int istrip,float mapcof,float mapinc,int stripx,int &ch,int dir,float power);
  double GetElossYIndexCor(int il,int istrip,float mapcof,float mapinc,int &ch,int dir,float power);
  double GetXInterpol(int istrip,int stripx,int ch);
  double GetYInterpol(int ch,int il);
  double VAGainCor(int idsoft,int stripx,double signal);
  double GetVAGainCor(int idsoft,int ch);
  double VAGain2Cor(int idsoft,int stripx,double signal);
  double GetVAGain2Cor(int idsoft,int ch);
  double IncxCor(int idsoft,float dadz[2],int stripx,double signal);
  double GetIncxCor(int idsoft,float mapinc,int stripx,double signal);
  double GetIncxIndexCor(int idsoft,float mapinc,int stripx,int &ch,int dir);
  double CofxCor(int idsoft,float tkcof[2],double signal);
  double CofyCor(int idsoft,float tkcof[2],double signal);
  double GetCofxCor(int idsoft,float mapcof,int mapxy,double signal);
  double GetCofxIndexCor(int idsoft,float mapcof,int mapxy,int &ch,int dir);
  double Eloss2Cor(int idsoft,float tkcof[2],float dadz[2],double signal);
  double GetEloss2YCor(int il,int istrip,float mapcof,float mapinc,double signal);
  double GetEloss2YIndexCor(int il,int istrip,float mapcof,float mapinc,int &ch,int dir);
  double VAChannelCor(int idsoft,double signal);
  double GetVAChannelCor(int idsoft,double signal);
  double GetVAChannelIndexCor(int idsoft,int &ch,int dir);
  double BetaCor(int idsoft,float beta,double signal);
  double GetBetaIndexCor(int idsoft,float beta,int &ch,int dir);
  double RigCor(int idsoft,float rig,double signal);
  double GetRigIndexCor(int idsoft,float rig,int &ch,int dir);
  double TimeCor(int idsoft,double signal);
  double GetTimeCor(int idsoft,int ch);
  int    GetQOccuStatus(int tkid,int seedadd,int opt);//0:low, 1:high
  int    GetQGainStatus(int tkid,int seedadd);
  int    GetQChaStatus(int tkid,int seedadd);
};


class TrQYJCluster{
public:
  int   _tkid;
  int   _seedadd;
  float _adc[7];
  float _qs[7];
  float _tkcof[2];
  float _dadz[2];
  int   _stripx;
  float _beta;
  float _rig;
  int   _qstatus;
  float Q;
public:
  int Clear();
  float GetQ(float qs[7],int opt=TrChargeYJ::DefaultCorOpt);
  int   GetQStatus();
  TrQYJCluster(){Clear();}
  TrQYJCluster(int tkid,int seedadd,float adc[7],float tkcof[2],float dadz[2],int stripx,float beta,float rig,int qopt):_tkid(tkid),_seedadd(seedadd),_stripx(stripx),_beta(beta),_rig(rig){
    for(int ii=0;ii<7;ii++){_adc[ii]=adc[ii];_qs[ii]=0;}
    for(int ia=0;ia<2;ia++){_tkcof[ia]=tkcof[ia];_dadz[ia]=dadz[ia];}
    _qstatus=GetQStatus();
    Q=GetQ(_qs,qopt);
  }
  TrQYJCluster Add(TrQYJCluster yjcl1,double ww1,TrQYJCluster yjcl2,double ww2);
};


class TrQYJHit{
public:
  TrQYJCluster qcl[2];
  float Q;
public:
  int Clear();
  float GetHitQ();
  bool HasCluster(int ixy){return qcl[ixy].Q>0; }
  int  GetHitType();//0:X, 1:Y, 2:X+Y
  int SetCluster(TrQYJCluster clx,TrQYJCluster cly){qcl[0]=clx; qcl[1]=cly; Q=GetHitQ(); return 0;}
  TrQYJHit(){Clear();}
  TrQYJHit(TrQYJCluster clx,TrQYJCluster cly){SetCluster(clx,cly);}
  TrQYJCluster *GetCluster(int ixy){return HasCluster(ixy)? &(qcl[ixy]): 0;}
  int GetiLayer();
};


class TrQYJTrack{
public:
  vector<TrQYJHit> qhits;
  float InnerQ;
  float InnerQRMS;
  int   InnerQPoints;
  int   InnerQPatt;
  float InnerQ2[2];
  float InnerQRMS2[2];
  int   InnerQPoints2[2];
  int   InnerQPatt2[2];
public:
  int Clear();
  float GetInnerQ(int sopt=0);//sopt: 0:Gaussion
  int SetHits(vector<TrQYJHit> hits,int sopt=0){qhits=hits; GetInnerQ(sopt); return 0;}
  TrQYJTrack(){Clear();}
  TrQYJTrack(vector<TrQYJHit> hits,int sopt=0){SetHits(hits,sopt);}
  TrQYJHit* GetHitL(int ilay);
  float     GetLQ(int ilay);
};

#endif
