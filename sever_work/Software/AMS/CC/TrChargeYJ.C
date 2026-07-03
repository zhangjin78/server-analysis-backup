//  $Id: TrChargeYJ.C,v 1.56 2025/05/13 11:00:24 choutko Exp $

#include "TrChargeYJ.h"
#include "TMath.h"
#include "timeid.h"
#include "commonsi.h"

int TrChargeYJDB::Version=1;

TrChargeYJDB::TrChargeYJDB(int HisReal){
  if(Version==0)TDVName="TrChargeYJDB";
  else          TDVName="TrChargeYJDB1";
  TDVBlock=0;
  TDVSize=0;
  tdvload=0;
  isreal=HisReal;
  LoadTDVPar();
}

void TrChargeYJDB::Init(){
}

void TrChargeYJDB::Clear(){
   if(tdvload) {delete tdvload; tdvload=0;}
   if(TDVBlock){delete TDVBlock;TDVBlock=0;}
}

TrChargeYJDB::~TrChargeYJDB() {
  Clear();
}

TrChargeYJDB* TrChargeYJDB::fHead = 0;

TrChargeYJDB* TrChargeYJDB::GetHead(int HisReal) {
  if (fHead ==0) {
    printf("TrChargeYJDB::GetHead()-V%d TrChargeYJDB singleton initialization.\n",Version);
    fHead = new TrChargeYJDB(HisReal);
 }
 return fHead;
}

int TrChargeYJDB::LoadTDVPar(){
  int iblock=0;
  int ifl=0;
  if(!TDVBlock)TDVfile[ifl++]=0;
  else         iblock=TDVfile[ifl++];

//------Load Eloss par for x
  for(int itype=0;itype<nType;itype++){
     for(int istrip=0;istrip<nStrip;istrip++){
        if(TDVBlock)nzelossx[itype][istrip]=int(TDVBlock[iblock]);
        iblock++;
        for(int ic=0;ic<nMaxZ;ic++){
           if(TDVBlock)zelossx[itype][istrip][ic]=int(TDVBlock[iblock]);
           iblock++;
           for(int il=0;il<9;il++){
             if(itype==0&&il<=1)continue;
             if(itype>=1&&il>=2&&il<=7)continue;
             for(int jj=0;jj<nint;jj++){
                 for(int kk=0;kk<npol;kk++){
                    for(int ipar=0;ipar<nelosspar;ipar++){
                       if(TDVBlock)elossxpar[itype][istrip][ic][il][jj][kk][ipar]=TDVBlock[iblock];
                       iblock++;
                    }
                  }
               }
           }
        }
     }
  }

//------Load Eloss par for y
  for(int istrip=0;istrip<nStrip;istrip++){
     if(istrip>=5)continue;
     if(TDVBlock)nzelossy[istrip]=int(TDVBlock[iblock]);
     iblock++;
     for(int ic=0;ic<nMaxZ;ic++){
        if(TDVBlock)zelossy[istrip][ic]=int(TDVBlock[iblock]);
        iblock++;
        for(int il=0;il<9;il++){ 
           for(int jj=0;jj<nint;jj++){
              for(int kk=0;kk<npol;kk++){
                 for(int ipar=0;ipar<nelosspar;ipar++){
                    if(TDVBlock)elossypar[istrip][ic][il][jj][kk][ipar]=TDVBlock[iblock];
                    iblock++;
              }
             }
          }
       }
    }
  }

//------Load Incx par 
  for(int ixy=0;ixy<2;ixy++){
     for(int istrip=0;istrip<nStrip;istrip++){
        if(ixy==1&&istrip>=5)continue;
        for(int in=0;in<nOflow;in++){
           if(ixy==0&&in>=2)continue;//consider two overflow situation
           if(ixy==1&&in>=1)continue;
           if(TDVBlock)nzincx[ixy][istrip][in]=int(TDVBlock[iblock]);
           iblock++;
           for(int ic=0;ic<nMaxZ;ic++){
              if(TDVBlock)zincx[ixy][istrip][in][ic]=int(TDVBlock[iblock]);
              iblock++;
              for(int il=0;il<9;il++){
                 for(int ipar=0;ipar<nIncxpar;ipar++){
                    if(TDVBlock)incxpar[ixy][istrip][in][ic][il][ipar]=TDVBlock[iblock];
                    iblock++;
                 }
              }
           }
        }
     }
  }

//------Load Cofx par 
  for(int ixy=0;ixy<nXY;ixy++){
     for(int mxy=0;mxy<nXY;mxy++){
        if(ixy==0&&mxy==1)continue;
        for(int istrip=0;istrip<nStrip;istrip++){
           if(mxy==0&&!(ixy==1&&istrip<=1)&&!(ixy==0&&istrip>=2&&istrip<=5))continue;
           if(mxy==1&&!(ixy==1&&istrip<=1))continue;
           if(TDVBlock)nzcofx[ixy][mxy][istrip]=int(TDVBlock[iblock]);
           iblock++;
           for(int ic=0;ic<nMaxZ;ic++){
              if(TDVBlock)zcofx[ixy][mxy][istrip][ic]=int(TDVBlock[iblock]);
              iblock++;
              for(int il=0;il<9;il++){
                 for(int ipar=0;ipar<nCofxpar;ipar++){
                    if(TDVBlock)cofxpar[ixy][mxy][istrip][ic][il][ipar]=TDVBlock[iblock];
                      iblock++;
                  }
              }
           }
        }
     }
  }

//----Load Eloss2 par for y
  for(int istrip=0;istrip<nStrip;istrip++){
     if(istrip==2||istrip==3||istrip>=5)continue;
     if(TDVBlock)nzeloss2y[istrip]=int(TDVBlock[iblock]);
      iblock++;
     for(int ic=0;ic<nMaxZ;ic++){
        if(TDVBlock)zeloss2y[istrip][ic]=int(TDVBlock[iblock]);
        iblock++;
        for(int il=0;il<9;il++){ 
           for(int jj=0;jj<nint;jj++){
               for(int kk=0;kk<npol;kk++){
                  for(int ipar=0;ipar<nelosspar;ipar++){
                    if(TDVBlock)eloss2ypar[istrip][ic][il][jj][kk][ipar]=TDVBlock[iblock];
                    iblock++;
                 }
              }
           }
        }
     }
  }

  if(!TDVBlock)TDVfile[ifl++]=iblock;
  else         iblock=TDVfile[ifl++];

//------Load VA par 
  for(int istrip=0;istrip<nStrip;istrip++){
     for(int il=0;il<9;il++){
        for(int ixy=0;ixy<nXY;ixy++){
           if(ixy==1&&istrip>=5)continue;
           for(int ilad=0;ilad<nLadM;ilad++){
              for(int iva=0;iva<nVA;iva++){
                 for(int ipar=0;ipar<nVApar;ipar++){
                    if(TDVBlock)vapar[istrip][il][ixy][ilad][iva][ipar]=TDVBlock[iblock];
                    iblock++;
                 }
              }
           }
        }
     }
  }
//------Load VA2 par
  for(int istrip=0;istrip<nStrip;istrip++){
     for(int il=0;il<9;il++){
        for(int ixy=0;ixy<nXY;ixy++){
           if((ixy==0&&istrip>=4)||(ixy==1&&istrip>=2))continue;
           for(int ilad=0;ilad<nLadM;ilad++){
              for(int iva=0;iva<nVA;iva++){
                 for(int ipar=0;ipar<nVApar;ipar++){
                    if(TDVBlock)va2par[istrip][il][ixy][ilad][iva][ipar]=TDVBlock[iblock];
                    iblock++;
                 }
              }
           }
        }
     }
  }

//------Load Channel par
  for(int istrip=0;istrip<nStrip;istrip++){
     if(istrip>=2)continue;
     if(TDVBlock)nzchannel[istrip]=int(TDVBlock[iblock]);
     iblock++;
     for(int ic=0;ic<nMaxZ;ic++){
        if(TDVBlock)zchannel[istrip][ic]=int(TDVBlock[iblock]);
        iblock++;
        for(int il=0;il<9;il++){
           for(int ipar=0;ipar<nChapar;ipar++){
              if(TDVBlock)channelpar[istrip][ic][il][ipar]=TDVBlock[iblock];
              iblock++;
           }
        }
     }
  }

//------Load edge channel par
  for(int istrip=0;istrip<nStrip;istrip++){
     if(istrip>=2)continue;
     if(TDVBlock)nzschannel[istrip]=int(TDVBlock[iblock]);
     iblock++;
     for(int ic=0;ic<nMaxZ;ic++){
        if(TDVBlock)zschannel[istrip][ic]=int(TDVBlock[iblock]);
        iblock++;
        for(int il=0;il<9;il++){
           for(int icha=0;icha<nSCha;icha++){
              if(TDVBlock)schannelpar[istrip][ic][il][icha]=TDVBlock[iblock];
              iblock++;
           }
        }
     }
  }
   
  if(!TDVBlock)TDVfile[ifl++]=iblock;
  else         iblock=TDVfile[ifl++];

//------Load Beta par
  for(int ixy=0;ixy<nXY;ixy++){
     for(int istrip=0;istrip<nStrip;istrip++){
        if(ixy==1&&istrip>=5)continue;
        if(TDVBlock)nzbeta[ixy][istrip]=int(TDVBlock[iblock]);
        iblock++;
        for(int ic=0;ic<nMaxZ;ic++){
           if(TDVBlock)zbeta[ixy][istrip][ic]=int(TDVBlock[iblock]);
           iblock++;
           for(int il=0;il<9;il++){
              for(int ipar=0;ipar<nBetapar;ipar++){
                 if(TDVBlock)betapar[ixy][istrip][ic][il][ipar]=TDVBlock[iblock];
                 iblock++;
              }
           }
        }
     }
  }
//------Load Rig par
  for(int ixy=0;ixy<nXY;ixy++){
     for(int istrip=0;istrip<nStrip;istrip++){
        if(ixy==1&&istrip>=5)continue;
        if(TDVBlock)nzrig[ixy][istrip]=int(TDVBlock[iblock]);
        iblock++;
        for(int ic=0;ic<nMaxZ;ic++){
           if(TDVBlock)zrig[ixy][istrip][ic]=int(TDVBlock[iblock]);
           iblock++;
           for(int il=0;il<9;il++){
              for(int ipar=0;ipar<nRigpar;ipar++){
                 if(TDVBlock)rigpar[ixy][istrip][ic][il][ipar]=TDVBlock[iblock];
                 iblock++;
              }
           }
        }
     }
  }
  if(!TDVBlock)TDVfile[ifl++]=iblock;
  else         iblock=TDVfile[ifl++];

//------Load resolution information
//---x strips combine
  for(int il=0;il<9;il++){
     for(int istrip=0;istrip<nStrip;istrip++){
        for(int inx=0;inx<nOflow;inx++){
           for(int ipar=0;ipar<nResx;ipar++){
              if(TDVBlock)resxpar[il][istrip][inx][ipar]=TDVBlock[iblock];
              iblock++;
           }
        }
     }
  }
//---y strips combine
  for(int il=0;il<9;il++){
     for(int istrip=0;istrip<nStrip;istrip++){
        for(int ipar=0;ipar<nResy;ipar++){
           if(TDVBlock)resypar[il][istrip][ipar]=TDVBlock[iblock];
           iblock++;
        }
     }
  }
//---x y combine
   for(int il=0;il<9;il++){
      for(int ipar=0;ipar<nResx;ipar++){
         if(TDVBlock)rescxpar[il][ipar]=TDVBlock[iblock];
         iblock++;
      }
   }
   for(int il=0;il<9;il++){
      for(int ipar=0;ipar<nResy;ipar++){
         if(TDVBlock)rescypar[il][ipar]=TDVBlock[iblock];
         iblock++;
      }
   }

  if(!TDVBlock)TDVfile[ifl++]=iblock;
  else         iblock=TDVfile[ifl++];

//------Load QStatus information
//---channel low occupancy
  for(int il=0;il<9;il++){
     for(int ilad=0;ilad<nLadM;ilad++){
        for(int ich=0;ich<nChannel;ich++){
           if(TDVBlock)ChaLowOccu[il][ilad][ich]=TDVBlock[iblock];
           iblock++;
        }
     }
  }
//---channel high occupancy
  for(int il=0;il<9;il++){
     for(int ilad=0;ilad<nLadM;ilad++){
        for(int ich=0;ich<nChannel;ich++){
           if(TDVBlock)ChaHighOccu[il][ilad][ich]=TDVBlock[iblock];
           iblock++;
        }
     }
  }

//---channel mean
  for(int il=0;il<9;il++){
     for(int ilad=0;ilad<nLadM;ilad++){
        for(int ich=0;ich<nChannel;ich++){
           if(TDVBlock)ChaMean[il][ilad][ich]=TDVBlock[iblock];
           iblock++;
        }
     }
  }
  if(!TDVBlock)TDVfile[ifl++]=iblock;
  else         iblock=TDVfile[ifl++];

  if(!TDVBlock){
    TDVSize=iblock;
    TDVBlock=new float[TDVSize];
    memset(TDVBlock,0,GetTDVLength());
  }
  return 0;
}

int TrChargeYJDB::LoadFromFile(const char *file[5],int isReal){
  isreal=isReal;
//----Load parameters
  for(int ifl=0;ifl<5;ifl++){
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

int TrChargeYJDB::PrintTDVBlock(){
  cout<<"<<----Print "<<TDVName<<endl;
  for(int i=0;i<TDVSize;i++){cout<<TDVBlock[i]<<" ";}
  cout<<'\n';
  cout<<"<<----end of Print "<<TDVName<<endl;
  return 0;
}

int TrChargeYJDB::SaveInTDV(long int start_time, long int end_time, int isReal) {
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

int TrChargeYJDB::LoadFromTDV(long int time,int clear, int isReal, bool force){
  isreal=isReal;
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

//////////////////////////////////////////////////////////////////////////
//          DataBase to calibarte dependence on time
/////////////////////////////////////////////////////////////////////////

int TrChargeYJTimeDB::Version=1;

TrChargeYJTimeDB::TrChargeYJTimeDB(){
  if(Version==0)TDVName="TrChargeYJTimeDB";
  else          TDVName="TrChargeYJTimeDB1";
  TDVBlock=0;
  TDVSize=0;
  tdvload=0;
  ptime=prun=0;
  nElem=0;
  pelem=0;
  LoadTDVPar();
}

void TrChargeYJTimeDB::Init(){
}

void TrChargeYJTimeDB::Clear(){
   if(tdvload) {delete tdvload; tdvload=0;}
   if(TDVBlock){delete TDVBlock;TDVBlock=0;}
}

TrChargeYJTimeDB::~TrChargeYJTimeDB() {
  Clear();
}

TrChargeYJTimeDB* TrChargeYJTimeDB::fHead = 0;

TrChargeYJTimeDB* TrChargeYJTimeDB::GetHead() {
  if (fHead ==0) {
    printf("TrChargeYJTimeDB::GetHead()-V%d TrChargeYJTimeDB singleton initialization.\n",Version);
    fHead = new TrChargeYJTimeDB();
 }

 return fHead;
}

int TrChargeYJTimeDB::LoadpElem(){
  static int ic=0,ie=0;
  map<pair<unsigned int,unsigned int>, TrChargeYJTimeElem>::iterator it; 
  for(it=elems.begin();it!=elems.end();it++){
    if(ptime>=it->first.first&&ptime<=it->first.second){pelem=&(it->second);return 0;}//best
    else if(it->first.first>ptime){pelem=&(it->second);return 1;}//avoid gap or underflow
  }
  if(it==elems.end()&&elems.size()>=1){//overflow
    it--;pelem=&(it->second);
    if((ptime-it->first.second)>10000&&ic++<=3)cout<<"TrChargeYJTimeDB::MaxCalibTime="<<it->first.second<<" ptime="<<ptime<<endl;
    return 2;
  }
  pelem=0;//err
  if(ie++<=3)cerr<<"TrChargeYJTimeDB::ErrCalibTime ptime="<<ptime<<endl;
  return 3;
}

int TrChargeYJTimeDB::LoadTDVPar(){
  int iblock=0;
  int ifl=0;
  if(!TDVBlock)TDVfile[ifl++]=0;
  else         iblock=TDVfile[ifl++];

  if(TDVBlock){
    nElem=TDVBlock[iblock];
    elems.clear();
    if(nElem>nMaxElem)cerr<<"TrChargeYJTimeDB::ErrnElem>nMaxElem"<<endl;
  }
  iblock++;

//------Load par for time dependece
  for(int ielem=0;ielem<nMaxElem;ielem++){
    TrChargeYJTimeElem uelem;
    pair<unsigned int,unsigned int> utime;
    if(TDVBlock)utime.first=TDVBlock[iblock];
    iblock++;
    if(TDVBlock)utime.second=TDVBlock[iblock];
    iblock++; 
    for(int ixy=0;ixy<uelem.nXY;ixy++){
       for(int istrip=0;istrip<uelem.nStrip;istrip++){
          if(ixy==0&&istrip>=2)continue;
          if(ixy==1&&istrip>=2&&istrip<=3)continue;
          for(int il=0;il<9;il++){
            for(int ipar=0;ipar<uelem.nTimepar;ipar++){
               if(TDVBlock)uelem.timepar[ixy][istrip][il][ipar]=TDVBlock[iblock];
               iblock++;
             }
          }
        }
     }
     if(TDVBlock&&ielem<nElem)elems.insert(make_pair(utime,uelem));
  }
  if(!TDVBlock)TDVfile[ifl++]=iblock;
  else         iblock=TDVfile[ifl++];

  if(!TDVBlock){
    TDVSize=iblock;
    TDVBlock=new float[TDVSize];
    memset(TDVBlock,0,GetTDVLength());
  }
  return 0;
}

int TrChargeYJTimeDB::LoadFromFile(const char *file[1]){
//----Load parameters
  for(int ifl=0;ifl<1;ifl++){
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

int TrChargeYJTimeDB::PrintTDVBlock(){
  cout<<"<<----Print "<<TDVName<<endl;
  for(int i=0;i<TDVSize;i++){cout<<TDVBlock[i]<<" ";}
  cout<<'\n';
  cout<<"<<----end of Print "<<TDVName<<endl;
  return 0;
}

int TrChargeYJTimeDB::SaveInTDV(long int start_time, long int end_time, int isReal) {
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

int TrChargeYJTimeDB::LoadFromTDV(long int time, unsigned int run, int clear, int isReal, bool force){
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
  ptime=time; prun=run;
  int ret = tdvload->validate(tt);
  LoadTDVPar();
  LoadpElem();
  return ret;
}

//------Calibration procedure
int TrChargeYJ::DefaultCorOpt=TrChargeYJ::kPath|TrChargeYJ::kEloss|TrChargeYJ::kGain|TrChargeYJ::kIncx|TrChargeYJ::kEloss2|TrChargeYJ::kGain2|TrChargeYJ::kChannel|TrChargeYJ::kCofy|TrChargeYJ::kCofx|TrChargeYJ::kBeta|TrChargeYJ::kRig|TrChargeYJ::kTime|TrChargeYJ::kSingle;

void TrChargeYJ::Init(){
}

TrChargeYJ* TrChargeYJ::fHead = 0;

TrChargeYJ* TrChargeYJ::GetHead(){
 if (fHead ==0) {
#pragma omp critical (trchargeyj)
   {
    fHead = new TrChargeYJ();
   }
 }
 return fHead;
}


double TrChargeYJ::Getpol(float x,float par[],int npar){
  double a=0;
  for(int i=0;i<npar;i++){
    a=a+par[i]*pow(x,i);
  }
  return a;
}

double TrChargeYJ::Getgaus(float x,float par1,float par2,float par3){
  return par1*exp(-0.5*pow((x-par2)/par3,2));
}

double TrChargeYJ::GetSigmoid(float x,float par1,float par2){
  return 1.0/(1.0+exp(-par1*(x-par2)));
}

int TrChargeYJ::GetIdsoft(int tkid,int seedadd,int istrip){
  int idsoft=(abs(tkid)*10000+seedadd)*10+istrip;
  if(tkid<0)idsoft*=-1;
  return idsoft;
}

int TrChargeYJ::ConvertLadderId(int tkid,int seedadd,int &il,int &ixy){
  il=abs(tkid/100);
  if     (il==8)il=1;
  else if(il<8)il=il+1;
  il--;
  ixy=(seedadd>=640)? 0: 1;
  return 0;
}

int TrChargeYJ::ConvertIdsoft(int idsoft,int &tkid,int &seedadd,int &il,int &ixy,int &istrip){
   tkid=(idsoft/10)/10000;
   seedadd=(abs(idsoft)/10)%10000;
   istrip=abs(idsoft)%10;
   return ConvertLadderId(tkid,seedadd,il,ixy);
}

int TrChargeYJ::Get_VAId(int idsoft){
  int tkid,seedadd,il,ixy,istrip;
  ConvertIdsoft(idsoft,tkid,seedadd,il,ixy,istrip);
  int ladid=tkid%100;//Ladder%100
  int iva=(ixy==0)?((seedadd-640)/64):(seedadd/64);
  return (ladid>0)?(ladid*10+iva):ladid*10-iva;
}

bool TrChargeYJ::IsSaturated(int stripx,int istrip){
  int noverflow = stripx/1000000;
  if(noverflow>=1&&noverflow<=4&&istrip==noverflow-1){return true;}
  else                                               {return false;}
}

double TrChargeYJ::GetCorQ(float rawsignal,int idsoft,float tkcof[2],float dadz[2],int stripx,float beta,float rig,float q0,int optc){
  if(rawsignal<=0)return rawsignal;
  double signal;
//---Path Length Cor
  if(optc&kPath)   {signal=PathCor(idsoft,dadz,rawsignal);if(signal<=0)return signal;}
//---Energy Loss
  if(optc&kEloss)  {signal=ElossCor(idsoft,tkcof,dadz,stripx,signal,q0);if(signal<=0)return signal;}
//---VA equalization
  if(optc&kGain)   {signal=VAGainCor(idsoft,stripx,signal);if(signal<=0)return signal;}
//---Energy Loss of x direction for y side
  if(optc&kIncx)   {signal=IncxCor(idsoft,dadz,stripx,signal);if(signal<=0)return signal;}
//---Recorrect Energy Loss for y side 
  if(optc&kEloss2) {signal=Eloss2Cor(idsoft,tkcof,dadz,signal);if(signal<=0)return signal;}
//---Recorrect VA for y side
  if(optc&kGain2)  {signal=VAGain2Cor(idsoft,stripx,signal);if(signal<=0)return signal;}
//VA Channel correction for y side
  if(optc&kChannel){signal=VAChannelCor(idsoft,signal);if(signal<=0)return signal;}
//---Cof of y direction 
  if(optc&kCofy)   {signal=CofyCor(idsoft,tkcof,signal);if(signal<=0)return signal;}
//---Cof of x direction 
  if(optc&kCofx)   {signal=CofxCor(idsoft,tkcof,signal);if(signal<=0)return signal;}
//Beta correction
  if(optc&kBeta)   {signal=BetaCor(idsoft,beta,signal);if(signal<=0)return signal;}
//Rig correction
  if(optc&kRig)    {signal=RigCor(idsoft,rig,signal);if(signal<=0)return signal;}
//Time dependence
  if(optc&kTime)   {signal=TimeCor(idsoft,signal);if(signal<=0)return signal;}
  return signal;
}


double TrChargeYJ::PathCor(int idsoft,float dadz[2],float signal){
  int tkid,seedadd,il,ixy,istrip;
  ConvertIdsoft(idsoft,tkid,seedadd,il,ixy,istrip);
  if(ixy==1&&istrip==4)return signal;//sum of all adc, no pathcor 
  float mapinc[2] = {0};
  double inclimit=(il==0||il==8)?0.5:1.0;
  for(int ia=0;ia<2;ia++){
     mapinc[ia]=(fabs(dadz[ia])>inclimit)?inclimit:dadz[ia];
  }
  return signal*cos(atan(sqrt(pow(mapinc[0],2)+pow(mapinc[1],2))));
}

double TrChargeYJ::GetTimeCor(int idsoft,int ch){
  if(ch<=0)return 0;
  if(ch>26)ch=26;
  int tkid,seedadd,il,ixy,istrip;
  ConvertIdsoft(idsoft,tkid,seedadd,il,ixy,istrip);
  TrChargeYJTimeElem *CPar=TrChargeYJTimeDB::GetHead()->pelem;
  if(!CPar)return 0;
  float polpar[CPar->nTimepar];
  for(int ipar=0;ipar<CPar->nTimepar;ipar++){
      polpar[ipar]=CPar->timepar[ixy][istrip][il][ipar];
   }
   return Getpol(ch,polpar,CPar->nTimepar)/ch;
}

double TrChargeYJ::GetVAGain2Cor(int idsoft,int ch){
  if(ch<=0)return 0;
  int tkid,seedadd,il,ixy,istrip;
  ConvertIdsoft(idsoft,tkid,seedadd,il,ixy,istrip);
  int maxz=(ixy==1&&istrip==1)?28:26;
  if(ch>maxz)ch=maxz;
  TrChargeYJDB *CPar=TrChargeYJDB::GetHead();
  int vaid=Get_VAId(idsoft);
  int Ladnum=vaid/10+13;
  if(vaid<0)Ladnum++;
  float polpar[CPar->nVApar];
  for(int ipar=0;ipar<CPar->nVApar;ipar++){
    polpar[ipar]=CPar->va2par[istrip][il][ixy][Ladnum][abs(vaid)%10][ipar];
  }
  double cor = Getpol(ch,polpar,CPar->nVApar)/ch;
  return fabs(cor);
}

double TrChargeYJ::GetVAGainCor(int idsoft,int ch){
  if(ch<=0)return 0;
  int tkid,seedadd,il,ixy,istrip;
  ConvertIdsoft(idsoft,tkid,seedadd,il,ixy,istrip);
  int maxz=((ixy==0&&istrip==4)||(ixy==1&&istrip==1))?28:26;
  if(ch>maxz)ch=maxz;
  TrChargeYJDB *CPar=TrChargeYJDB::GetHead();
  int vaid=Get_VAId(idsoft);
  int Ladnum=vaid/10+13;
  if(vaid<0)Ladnum++;
  float polpar[CPar->nVApar];
  for(int ipar=0;ipar<CPar->nVApar;ipar++){
     polpar[ipar]=CPar->vapar[istrip][il][ixy][Ladnum][abs(vaid)%10][ipar];
  }
  double cor = Getpol(ch,polpar,CPar->nVApar)/ch;
  return fabs(cor);
}

double TrChargeYJ::VAGain2Cor(int idsoft,int stripx,double signal){
  int tkid,seedadd,il,ixy,istrip;
  ConvertIdsoft(idsoft,tkid,seedadd,il,ixy,istrip);
  if(ixy==0&&!TrChargeYJDB::GetHead()->isreal)return signal;
  if(ixy==0&&!(IsSaturated(stripx,istrip)))return signal;//correct saturated x strips
  if(ixy==1&&istrip>=2)return signal;//no 3rd,4th strip of y

  int nowch1 = int(signal+0.5);
  if(nowch1<1)nowch1 = 1;
  double cor = 1.0;
  static int nc = 0;
  const int maxnc = 200;
  while(1){
    double nowq1=GetVAGain2Cor(idsoft,nowch1)*nowch1;
    if(nowch1<0||nowch1>maxnc){if(nc++<10)cerr<<"Error:ChargeYJVA2"<<endl;cor=nowq1/nowch1;break;}
    int dir=(signal>nowq1)?1:-1;//forward/downward search 
    int nowch2=nowch1+dir;
    double nowq2=GetVAGain2Cor(idsoft,nowch2)*nowch2;
    if((signal>=nowq2&&signal<=nowq1)||(signal>=nowq1&&signal<=nowq2)){
      if(nowch1==0){cor=nowq2/nowch2;break;}
      if(nowch2==0){cor=nowq1/nowch1;break;}
      double cor1=nowq1/nowch1;
      double cor2=nowq2/nowch2;
      double ch1=signal/cor1;
      double ch2=signal/cor2;
      double ww1=fabs(ch1*ch1-nowch1*nowch1);
      double ww2=fabs(ch2*ch2-nowch2*nowch2);
      cor=((ww1+ww2)==0)?0:(ww2*cor1+ww1*cor2)/(ww1+ww2);
      break;
    } 
    else{nowch1=nowch2;}
  } 
  double newsignal = (cor<=0)?signal:signal/cor;
  return newsignal;
}

double TrChargeYJ::TimeCor(int idsoft,double signal){
  if(!TrChargeYJDB::GetHead()->isreal)return signal;//no mc calibration
  int tkid,seedadd,il,ixy,istrip;
  ConvertIdsoft(idsoft,tkid,seedadd,il,ixy,istrip);
  if(ixy==0&&istrip>=2)return signal;
  if(ixy==1&&istrip>=2&&istrip<=3)return signal;
  int nowch1 = int(signal+0.5);
  if(nowch1<1)nowch1 = 1;
  double cor = 1.0;
  static int nc = 0;
  const int maxnc = 200;
  while(1){
    double nowq1=GetTimeCor(idsoft,nowch1)*nowch1;
    if(nowch1<0||nowch1>maxnc){if(nc++<10)cerr<<"Error:ChargeYJTime"<<endl;cor=nowq1/nowch1;break;}
    int dir=(signal>nowq1)?1:-1;//forward/downward search
    int nowch2=nowch1+dir;
    double nowq2=GetTimeCor(idsoft,nowch2)*nowch2;
    if((signal>=nowq2&&signal<=nowq1)||(signal>=nowq1&&signal<=nowq2)){
      if(nowch1==0){cor=nowq2/nowch2;break;}
      if(nowch2==0){cor=nowq1/nowch1;break;}
      double cor1=nowq1/nowch1;
      double cor2=nowq2/nowch2;
      double ch1=signal/cor1;
      double ch2=signal/cor2;
      double ww1=fabs(ch1*ch1-nowch1*nowch1);
      double ww2=fabs(ch2*ch2-nowch2*nowch2);
      cor=((ww1+ww2)==0)?0:(ww2*cor1+ww1*cor2)/(ww1+ww2);
      break;
    }
    else{nowch1=nowch2;}
  }
  double newsignal = (cor<=0)?signal:signal/cor;
  return newsignal;
}

double TrChargeYJ::VAGainCor(int idsoft,int stripx,double signal){
  int tkid,seedadd,il,ixy,istrip;
  ConvertIdsoft(idsoft,tkid,seedadd,il,ixy,istrip);
  if(ixy==0&&IsSaturated(stripx,istrip))return signal;//correct not saturated

  int nowch1 = int(signal+0.5);
  if(nowch1<1)nowch1 = 1;
  double cor = 1.0;
  static int nc = 0;
  const int maxnc = 200;
  while(1){
    double nowq1=GetVAGainCor(idsoft,nowch1)*nowch1;
    if(nowch1<0||nowch1>maxnc){if(nc++<10)cerr<<"Error:ChargeYJVA"<<endl;cerr<<"il "<<il<<" ixy "<<ixy<<" istrip "<<istrip<<" nc "<<nc<<" nowch1 "<<nowch1<<" signal "<<signal<<" nowq1 "<<nowq1<<endl;cor=nowq1/nowch1;break;}
    int dir=(signal>nowq1)?1:-1;//forward/downward search
    int nowch2=nowch1+dir;
    double nowq2=GetVAGainCor(idsoft,nowch2)*nowch2;
    if((signal>=nowq2&&signal<=nowq1)||(signal>=nowq1&&signal<=nowq2)){
      if(nowch1==0){cor=nowq2/nowch2;break;}
      if(nowch2==0){cor=nowq1/nowch1;break;}
      double cor1=nowq1/nowch1;
      double cor2=nowq2/nowch2;
      double ch1=signal/cor1;
      double ch2=signal/cor2;
      double ww1=fabs(ch1*ch1-nowch1*nowch1);
      double ww2=fabs(ch2*ch2-nowch2*nowch2);
      cor=((ww1+ww2)==0)?0:(ww2*cor1+ww1*cor2)/(ww1+ww2);
      break;
    } 
    else{nowch1=nowch2;}
  } 
  double newsignal = (cor<=0)?signal:signal/cor;
  return newsignal;
} 

double TrChargeYJ::IncxCor(int idsoft,float dadz[2],int stripx,double signal){
  int tkid,seedadd,il,ixy,istrip;
  ConvertIdsoft(idsoft,tkid,seedadd,il,ixy,istrip);
  if(!TrChargeYJDB::GetHead()->isreal&&ixy==0)return signal;//no mc calibration for x side
  if(ixy==0&&IsSaturated(stripx,istrip))return signal;//correct not saturated
  int mapxy=0;//correct for inclination of x direction
  float mapinc=dadz[mapxy];
  double cor = 1.0;
  cor=GetIncxCor(idsoft,mapinc,stripx,signal);
  double newsignal = (cor<=0)?signal:signal/cor;
  return newsignal;
}

double TrChargeYJ::CofxCor(int idsoft,float tkcof[2],double signal){
  int tkid,seedadd,il,ixy,istrip;
  ConvertIdsoft(idsoft,tkid,seedadd,il,ixy,istrip);
  if(!TrChargeYJDB::GetHead()->isreal)return signal;//no mc calibration
  if(!(ixy==1&&istrip<=1)&&!(ixy==0&&istrip>=2&&istrip<=5))return signal;
  int mapxy=0;//correct for COG of x direction
  float mapcof=tkcof[mapxy];
  if(fabs(mapcof)<1e-3)return signal;//no xcluster or xcluster only seed strip 
  double cor = 1.0;
  cor=GetCofxCor(idsoft,mapcof,mapxy,signal);
  double newsignal = (cor<=0)?signal:signal/cor;
  return newsignal;
}

double TrChargeYJ::CofyCor(int idsoft,float tkcof[2],double signal){
  int tkid,seedadd,il,ixy,istrip;
  ConvertIdsoft(idsoft,tkid,seedadd,il,ixy,istrip);
  if(!TrChargeYJDB::GetHead()->isreal)return signal;//no mc calibration
  if(!(ixy==1&&istrip<=1))return signal;
  int mapxy=1;//correct for COG of y direction
  float mapcof=tkcof[mapxy];
  if(fabs(mapcof)<1e-3)return signal;//no xcluster or xcluster only seed strip 
  double cor = 1.0;
  cor=GetCofxCor(idsoft,mapcof,mapxy,signal);
  double newsignal = (cor<=0)?signal:signal/cor;
  return newsignal;
}

double TrChargeYJ::RigCor(int idsoft,float rig,double signal){
  if(fabs(rig)<1e-3)return signal;
  int nowch1=int(signal+0.5);
  if(nowch1<1)nowch1 = 1;
  double cor = 1.0;
  int dir = 1;
  static int nc = 0;
  const int maxnc = 200;
  while(1){
    double nowq1=GetRigIndexCor(idsoft,rig,nowch1,dir)*nowch1;
    if(nowch1<0||nowch1>maxnc){if(nc++<10)cerr<<"Error:ChargeYJRig"<<endl;cor=nowq1/nowch1;break;}
    if(nowch1>0&&nowq1==0){nowch1++;continue;}
    dir=(signal>nowq1)?1:-1;//forward/downward search
    int nowch2=nowch1+dir;
    double nowq2=GetRigIndexCor(idsoft,rig,nowch2,dir)*nowch2;
    while(nowq2==0){
      if(nowch2<0||nowch2>maxnc){if(nc++<10)cerr<<"Error:ChargeYJRig"<<endl;return signal;}
      nowch2+=dir;
      nowq2=GetRigIndexCor(idsoft,rig,nowch2,dir)*nowch2;
    }
    if((signal>=nowq2&&signal<=nowq1)||(signal>=nowq1&&signal<=nowq2)){
      if(nowch1==0){cor=nowq2/nowch2;break;}
      if(nowch2==0){cor=nowq1/nowch1;break;}
      double cor1=nowq1/nowch1;
      double cor2=nowq2/nowch2;
      double ch1=signal/cor1;
      double ch2=signal/cor2;
      double ww1=fabs(ch1*ch1-nowch1*nowch1);
      double ww2=fabs(ch2*ch2-nowch2*nowch2);
      cor=((ww1+ww2)==0)?0:(ww2*cor1+ww1*cor2)/(ww1+ww2);
      break;
    }
    else{nowch1=nowch2;}
  }
  double newsignal = (cor<=0)?signal:signal/cor;
  return newsignal;
}

double TrChargeYJ::BetaCor(int idsoft,float beta,double signal){
  int nowch1=int(signal+0.5);
  if(nowch1<1)nowch1 = 1;
  double cor = 1.0;
  int dir = 1;
  static int nc = 0;
  const int maxnc = 200;
  while(1){
    double nowq1=GetBetaIndexCor(idsoft,beta,nowch1,dir)*nowch1;
    if(nowch1<0||nowch1>maxnc){if(nc++<10)cerr<<"Error:ChargeYJBeta"<<endl;cor=nowq1/nowch1;break;}
    if(nowch1>0&&nowq1==0){nowch1++;continue;}
    int dir=(signal>nowq1)?1:-1;//forward/downward search
    int nowch2=nowch1+dir;
    double nowq2=GetBetaIndexCor(idsoft,beta,nowch2,dir)*nowch2;
    while(nowq2==0){
      if(nowch2<0||nowch2>maxnc){if(nc++<10)cerr<<"Error:ChargeYJBeta"<<endl;return signal;}
      nowch2+=dir;
      nowq2=GetBetaIndexCor(idsoft,beta,nowch2,dir)*nowch2;
    }
    if((signal>=nowq2&&signal<=nowq1)||(signal>=nowq1&&signal<=nowq2)){
      if(nowch1==0){cor=nowq2/nowch2;break;}
      if(nowch2==0){cor=nowq1/nowch1;break;}
      double cor1=nowq1/nowch1;
      double cor2=nowq2/nowch2;
      double ch1=signal/cor1;
      double ch2=signal/cor2;
      double ww1=fabs(ch1*ch1-nowch1*nowch1);
      double ww2=fabs(ch2*ch2-nowch2*nowch2);
      cor=((ww1+ww2)==0)?0:(ww2*cor1+ww1*cor2)/(ww1+ww2);
      break;
    }
    else{nowch1=nowch2;}
  }
  double newsignal = (cor<=0)?signal:signal/cor;
  return newsignal;
}

double TrChargeYJ::VAChannelCor(int idsoft,double signal){
  if(!TrChargeYJDB::GetHead()->isreal)return signal;//no mc calibration
  int tkid,seedadd,il,ixy,istrip;
  ConvertIdsoft(idsoft,tkid,seedadd,il,ixy,istrip);
  double cor = 1.0;
  if(ixy==0)return signal;
  if(ixy==1&&istrip>=2)return signal;//correct for seed, 2rd strips of y
  if(ixy==1)cor=GetVAChannelCor(idsoft,signal);
  double newsignal = (cor<=0)?signal:signal/cor;
  return newsignal;
}

double TrChargeYJ::Eloss2Cor(int idsoft,float tkcof[2],float dadz[2],double signal){
  int tkid,seedadd,il,ixy,istrip;
  ConvertIdsoft(idsoft,tkid,seedadd,il,ixy,istrip);
  float mapinc=dadz[ixy];
  double cor = 1.0;
  if(ixy==0)return signal;
  if(ixy==1&&(istrip==2||istrip==3))return signal;
  if(ixy==1)cor=GetEloss2YCor(il,istrip,tkcof[ixy],mapinc,signal);
  double newsignal = (cor<=0)?signal:signal/cor;
  return newsignal;
}

double TrChargeYJ::ElossCor(int idsoft,float tkcof[2],float dadz[2],int stripx,double signal,float q0){
  int tkid,seedadd,il,ixy,istrip;
  ConvertIdsoft(idsoft,tkid,seedadd,il,ixy,istrip);
  double cor = 1.0;
  float mapinc=dadz[ixy];
  if(ixy==0)cor=GetElossXCor(il,istrip,tkcof[ixy],mapinc,stripx,signal);
  if(ixy==1)cor=GetElossYCor(il,istrip,tkcof[ixy],dadz,signal,q0);
  double newsignal = (cor<=0)?sqrt(fabs(signal)):sqrt(fabs(signal/cor));
  return newsignal;
}

double TrChargeYJ::GetVAChannelCor(int idsoft,double signal){
  int nowch1=int(signal+0.5);
  if(nowch1<1)nowch1 = 1;
  double cor = 1.0;
  int dir = 1;
  static int nc = 0;
  const int maxnc = 200;
  while(1){
    double nowq1=GetVAChannelIndexCor(idsoft,nowch1,dir)*nowch1;
    if(nowch1<0||nowch1>maxnc){if(nc++<10)cerr<<"Error:ChargeYJVAChannel"<<endl;cor=nowq1/nowch1;break;}
    if(nowch1>0&&nowq1==0){nowch1++;continue;}
    int dir=(signal>nowq1)?1:-1;//forward/downward search
    int nowch2=nowch1+dir;
    double nowq2=GetVAChannelIndexCor(idsoft,nowch2,dir)*nowch2;
    while(nowq2==0){
      if(nowch2<0||nowch2>maxnc){if(nc++<10)cerr<<"Error:ChargeYJVAChannel"<<endl;return 1.0;}
      nowch2+=dir;
      nowq2=GetVAChannelIndexCor(idsoft,nowch2,dir)*nowch2;
    }
    if((signal>=nowq2&&signal<=nowq1)||(signal>=nowq1&&signal<=nowq2)){
      if(nowch1==0){cor=nowq2/nowch2;break;}
      if(nowch2==0){cor=nowq1/nowch1;break;}
      double cor1=nowq1/nowch1;
      double cor2=nowq2/nowch2;
      double ch1=signal/cor1;
      double ch2=signal/cor2;
      double ww1=fabs(ch1*ch1-nowch1*nowch1);
      double ww2=fabs(ch2*ch2-nowch2*nowch2);
      cor=((ww1+ww2)==0)?0:(ww2*cor1+ww1*cor2)/(ww1+ww2);
      break;
    }
    else{nowch1=nowch2;}
  }
  return cor;
}

double TrChargeYJ::GetEloss2YCor(int il,int istrip,float mapcof,float mapinc,double signal){
  int nowch1=int(signal+0.5);
  if(nowch1<1)nowch1 = 1;
  double cor = 1.0;
  int dir = 1;
  static int nc = 0;
  const int maxnc = 200;
  while(1){
    double nowq1=GetEloss2YIndexCor(il,istrip,mapcof,mapinc,nowch1,dir)*nowch1;
    if(nowch1<0||nowch1>maxnc){if(nc++<10)cerr<<"Error:ChargeYJEloss2Y"<<endl;cor=nowq1/nowch1;break;}
    if(nowch1>0&&nowq1==0){nowch1++;continue;}
    dir=(signal>nowq1)?1:-1;//forward/downward search
    int nowch2=nowch1+dir;
    double nowq2=GetEloss2YIndexCor(il,istrip,mapcof,mapinc,nowch2,dir)*nowch2;
    while(nowq2==0){
      if(nowch2<0||nowch2>maxnc){if(nc++<10)cerr<<"Error:ChargeYJEloss2Y"<<endl;return 1.0;}
      nowch2+=dir;
      nowq2=GetEloss2YIndexCor(il,istrip,mapcof,mapinc,nowch2,dir)*nowch2;
    }
    if((signal>=nowq2&&signal<=nowq1)||(signal>=nowq1&&signal<=nowq2)){
      if(nowch1==0){cor=nowq2/nowch2;break;}
      if(nowch2==0){cor=nowq1/nowch1;break;}
      double cor1=nowq1/nowch1;
      double cor2=nowq2/nowch2;
      double ch1=signal/cor1;
      double ch2=signal/cor2;
      double ww1=fabs(ch1*ch1-nowch1*nowch1);
      double ww2=fabs(ch2*ch2-nowch2*nowch2);
      cor=((ww1+ww2)==0)?0:(ww2*cor1+ww1*cor2)/(ww1+ww2);
      break;
    }
    else{nowch1=nowch2;}
  }
  return cor;
}

double TrChargeYJ::ElossYConvert(int il,int istrip,float tkcof,float mapinc,float q0){
   double cor = 1;
   int nowch1 = int(q0);
   static int nc = 0;
   const int maxnc = 200;
   while(1){
     double nowq1 = GetElossYIndexCor(il,istrip,tkcof,mapinc,nowch1,-1,2)*pow(nowch1,2);
     if(nowch1<0||nowch1>maxnc){if(nc++<10)cerr<<"Error:ChargeYJYConvert"<<endl;cor=nowq1/pow(nowch1,2);break;}
     if(nowch1>0&&nowq1==0){nowch1++;continue;}
     int nowch2=nowch1+1;
     double nowq2=GetElossYIndexCor(il,istrip,tkcof,mapinc,nowch2,1,2)*pow(nowch2,2);
     while(nowq2==0){
       if(nowch2<0||nowch2>maxnc){if(nc++<10)cerr<<"Error:ChargeYJYConvert"<<endl;return 1.0;}
       nowch2++;
       nowq2=GetElossYIndexCor(il,istrip,tkcof,mapinc,nowch2,1,2)*pow(nowch2,2);
     }
     if(nowch1==0){cor=nowq2/pow(nowch2,2);break;}
     double cor1=nowq1/pow(nowch1,2);
     double cor2=nowq2/pow(nowch2,2);
     double ww1=fabs(q0*q0-nowch1*nowch1);
     double ww2=fabs(q0*q0-nowch2*nowch2);
     cor=((ww1+ww2)==0)?0:(ww2*cor1+ww1*cor2)/(ww1+ww2);
     break;
   }
   return cor;
}

double TrChargeYJ::GetYInterpol(int ch,int il){
  double pow=2.0;
  if(TrChargeYJDB::GetHead()->isreal){//adjust data
    if(ch>26)pow=(il<=1)?1.2:1.3;
  }
  return pow;
}

double TrChargeYJ::GetElossYCor(int il,int istrip,float tkcof,float dadz[2],double signal,float q0){
  int real=TrChargeYJDB::GetHead()->isreal;
  int nowch1 = int(q0+0.5);
  if(nowch1<1)nowch1 = 1;
  double cor = 1;
  int dir = 1;
  float mapinc = dadz[1];
  float power = 2.0;
  int chargecut=15;
  if(real&&istrip<=1&&q0<chargecut)return ElossYConvert(il,istrip,tkcof,mapinc,q0);
  static int nc = 0;
  const int maxnc = 200;
  while(1){
    power=GetYInterpol(nowch1,il);
    double nowq1=GetElossYIndexCor(il,istrip,tkcof,mapinc,nowch1,dir,power)*pow(nowch1,power);
    if(nowch1<0||nowch1>maxnc){if(nc++<10)cerr<<"Error:ChargeYJElossY"<<endl;cor=nowq1/pow(nowch1,2);break;;}
    if(nowch1>0&&nowq1==0){nowch1++;continue;}
    dir=(signal>nowq1)?1:-1;//forward/downward search 
    int nowch2=nowch1+dir;
    power=GetYInterpol(nowch2,il);
    double nowq2=GetElossYIndexCor(il,istrip,tkcof,mapinc,nowch2,dir,power)*pow(nowch2,power);
    if(real&&istrip!=4&&((dir>0&&nowq2!=0&&nowq2<nowq1)||(dir<0&&nowq1!=0&&nowq2>nowq1))){
      cor=ElossYConvert(il,istrip,tkcof,mapinc,q0);break;
    }
    while(nowq2==0&&nowch2>0){
      if(nowch2<0||nowch2>maxnc){if(nc++<10)cerr<<"Error:ChargeYJElossY"<<endl;return 1.0;}
      nowch2+=dir;
      power=GetYInterpol(nowch2,il);
      nowq2=GetElossYIndexCor(il,istrip,tkcof,mapinc,nowch2,dir,power)*pow(nowch2,power);
    }
    if((signal>=nowq2&&signal<=nowq1)||(signal>=nowq1&&signal<=nowq2)){
      if(nowch1==0){cor=nowq2/pow(nowch2,2);break;}
      if(nowch2==0){cor=nowq1/pow(nowch1,2);break;}
      double cor1=nowq1/pow(nowch1,2);
      double cor2=nowq2/pow(nowch2,2);
      double ch1=sqrt(fabs(signal/cor1));
      double ch2=sqrt(fabs(signal/cor2));
      double ww1=fabs(ch1*ch1-nowch1*nowch1);
      double ww2=fabs(ch2*ch2-nowch2*nowch2);
      cor=((ww1+ww2)==0)?0:(ww2*cor1+ww1*cor2)/(ww1+ww2);
      break;
    }
    else{nowch1=nowch2;}
  }
  return cor;
}

double TrChargeYJ::GetIncxCor(int idsoft,float mapinc,int stripx,double signal){
  int nowch1=int(signal+0.5);
  if(nowch1<1)nowch1 = 1;
  double cor = 1.0;
  int dir = 1;
  static int nc = 0;
  const int maxnc = 200;
  while(1){
    double nowq1=GetIncxIndexCor(idsoft,mapinc,stripx,nowch1,dir)*nowch1;
    if(nowch1<0||nowch1>maxnc){if(nc++<10)cerr<<"Error:ChargeYJIncx"<<endl;cor=nowq1/nowch1;break;}
    if(nowch1>0&&nowq1==0){nowch1++;continue;}
    dir=(signal>nowq1)?1:-1;//forward/downward search
    int nowch2=nowch1+dir;
    double nowq2=GetIncxIndexCor(idsoft,mapinc,stripx,nowch2,dir)*nowch2;
    while(nowq2==0){
      if(nowch2<0||nowch2>maxnc){if(nc++<10)cerr<<"Error:ChargeYJIncx"<<endl;return 1.0;}
      nowch2+=dir;
      nowq2=GetIncxIndexCor(idsoft,mapinc,stripx,nowch2,dir)*nowch2;
    }
    if((signal>=nowq2&&signal<=nowq1)||(signal>=nowq1&&signal<=nowq2)){
      if(nowch1==0){cor=nowq2/nowch2;break;}
      if(nowch2==0){cor=nowq1/nowch1;break;}
      double cor1=nowq1/nowch1;
      double cor2=nowq2/nowch2;
      double ch1=signal/cor1;
      double ch2=signal/cor2;
      double ww1=fabs(ch1*ch1-nowch1*nowch1);
      double ww2=fabs(ch2*ch2-nowch2*nowch2);
      cor=((ww1+ww2)==0)?0:(ww2*cor1+ww1*cor2)/(ww1+ww2);
      break;
    }
    else{nowch1=nowch2;}
  }
  return cor;
}

double TrChargeYJ::GetCofxCor(int idsoft,float mapcof,int mapxy,double signal){
  int tkid,seedadd,il,ixy,istrip;
  ConvertIdsoft(idsoft,tkid,seedadd,il,ixy,istrip);
  int nowch1=int(signal+0.5);
  if(nowch1<1)nowch1 = 1;
  double cor = 1.0;
  int dir = 1;
  static int nc = 0;
  const int maxnc = 200;
  while(1){
    double nowq1=GetCofxIndexCor(idsoft,mapcof,mapxy,nowch1,dir)*nowch1;
    if(nowch1<0||nowch1>maxnc){if(nc++<10)cout<<"Error:ChargeYJCofx"<<endl;cor=nowq1/nowch1;break;}
    if(nowch1>0&&nowq1==0){nowch1++;continue;}
    dir=(signal>nowq1)?1:-1;//forward/downward search
    int nowch2=nowch1+dir;
    double nowq2=GetCofxIndexCor(idsoft,mapcof,mapxy,nowch2,dir)*nowch2;
    while(nowq2==0){
      if(nowch2<0||nowch2>maxnc){if(nc++<10)cerr<<"Error:ChargeYJCofx"<<endl;return 1.0;}
      nowch2+=dir;
      nowq2=GetCofxIndexCor(idsoft,mapcof,mapxy,nowch2,dir)*nowch2;
    }
    if((signal>=nowq2&&signal<=nowq1)||(signal>=nowq1&&signal<=nowq2)){
      if(nowch1==0){cor=nowq2/nowch2;break;}
      if(nowch2==0){cor=nowq1/nowch1;break;}
      double cor1=nowq1/nowch1;
      double cor2=nowq2/nowch2;
      double ch1=signal/cor1;
      double ch2=signal/cor2;
      double ww1=fabs(ch1*ch1-nowch1*nowch1);
      double ww2=fabs(ch2*ch2-nowch2*nowch2);
      cor=((ww1+ww2)==0)?0:(ww2*cor1+ww1*cor2)/(ww1+ww2);
      break;
    }
    else{nowch1=nowch2;}
  }
  return cor;
}

double TrChargeYJ::GetXInterpol(int istrip,int stripx,int ch){
  int striptype=stripx%10;
  double pow=2.0;
  if(TrChargeYJDB::GetHead()->isreal){//adjust data
    if(ch>26&&istrip>=4)pow=(striptype==1)?3.5:3.0;
  }
  else{if(ch>26&&istrip==0)pow=1.2;}
  return pow;
}

double TrChargeYJ::GetElossXCor(int il,int istrip,float mapcof,float mapinc,int stripx,double signal){
  double cor = 1.0;
  int dir = 1;
  float power = 2.0;
  int refZ = (istrip<=1)?8:14;
  if(istrip>=3)refZ=26;
  double adccor = GetElossXIndexCor(il,istrip,mapcof,mapinc,stripx,refZ,dir,power);
  double q0 = sqrt(fabs(signal/adccor));
  int nowch1 = int(q0+0.5);
  static int nc = 0;
  const int maxnc = 200;
  while(1){
    power=GetXInterpol(istrip,stripx,nowch1);
    double nowq1=GetElossXIndexCor(il,istrip,mapcof,mapinc,stripx,nowch1,dir,power)*pow(nowch1,power);
    if(nowch1<0||nowch1>maxnc){if(nc++<10)cerr<<"Error:ChargeYJElossX"<<endl;cor=nowq1/pow(nowch1,2);break;}
    if(nowch1>0&&nowq1==0){nowch1++;continue;}
    dir=(signal>nowq1)?1:-1;//forward/downward search
    int nowch2=nowch1+dir;
    power=GetXInterpol(istrip,stripx,nowch2);
    double nowq2=GetElossXIndexCor(il,istrip,mapcof,mapinc,stripx,nowch2,dir,power)*pow(nowch2,power);
    while(nowq2==0&&nowch2>0){
      if(nowch2<0||nowch2>maxnc){if(nc++<10)cerr<<"Error:ChargeYJElossX"<<endl;return 1.0;}
      nowch2+=dir;
      power=GetXInterpol(istrip,stripx,nowch2);
      nowq2=GetElossXIndexCor(il,istrip,mapcof,mapinc,stripx,nowch2,dir,power)*pow(nowch2,power);
    }
    if((signal>=nowq2&&signal<=nowq1)||(signal>=nowq1&&signal<=nowq2)){
      if(nowch1==0){cor=nowq2/pow(nowch2,2);break;}
      if(nowch2==0){cor=nowq1/pow(nowch1,2);break;}
      double cor1=nowq1/pow(nowch1,2);
      double cor2=nowq2/pow(nowch2,2);
      double ch1=sqrt(fabs(signal/cor1));
      double ch2=sqrt(fabs(signal/cor2));
      double ww1=fabs(ch1*ch1-nowch1*nowch1);
      double ww2=fabs(ch2*ch2-nowch2*nowch2);
      cor=((ww1+ww2)==0)?0:(ww2*cor1+ww1*cor2)/(ww1+ww2);
      break;
    }
    else{nowch1=nowch2;}
  }
  return cor;
}


double TrChargeYJ::GetIncxIndexCor(int idsoft,float mapinc,int stripx,int &ch,int dir){
  TrChargeYJDB *CPar=TrChargeYJDB::GetHead();
  int tkid,seedadd,il,ixy,istrip;
  ConvertIdsoft(idsoft,tkid,seedadd,il,ixy,istrip);
  int noverflow = stripx/1000000;
  int in = noverflow-istrip+1;
  if((ixy==0&&(in<0||in>1))||(ixy==1))in=0;

//---Search Index
  int Ich = 0;
  if(ch<=CPar->zincx[ixy][istrip][in][0]){Ich=0;}
  else if(ch>=CPar->zincx[ixy][istrip][in][CPar->nzincx[ixy][istrip][in]-1]){
    Ich=CPar->nzincx[ixy][istrip][in]-1;}
  else{
    for(int ic=0;ic<CPar->nzincx[ixy][istrip][in];ic++){
       if(dir>0&&CPar->zincx[ixy][istrip][in][ic]>=ch){Ich=ic;break;}
       if(dir<0&&CPar->zincx[ixy][istrip][in][CPar->nzincx[ixy][istrip][in]-ic-1]<=ch){
         Ich=CPar->nzincx[ixy][istrip][in]-ic-1;break;
       }
    }
    ch=CPar->zincx[ixy][istrip][in][Ich];
  }

//---protection
  double inclimit=(il==0||il==8)?0.5:1.0;
  if(mapinc>inclimit)mapinc=inclimit;
  if(mapinc<-inclimit)mapinc=-inclimit;

  double mch = 0;
  double cor = 1.0;
  for(int i=0;i<CPar->nIncxpar/2;i++){
    int index=(mapinc<0)?i:i+CPar->nIncxpar/2;
    mch+=CPar->incxpar[ixy][istrip][in][Ich][il][index]*pow(mapinc,i);
  }
  cor=fabs(mch/CPar->zincx[ixy][istrip][in][Ich]);
  return cor;
}

double TrChargeYJ::GetCofxIndexCor(int idsoft,float mapcof,int mapxy,int &ch,int dir){
  TrChargeYJDB *CPar=TrChargeYJDB::GetHead();
  int tkid,seedadd,il,ixy,istrip;
  ConvertIdsoft(idsoft,tkid,seedadd,il,ixy,istrip);

//---Search Index
  int Ich = 0;
  if(ch<=CPar->zcofx[ixy][mapxy][istrip][0]){Ich=0;}
  else if(ch>=CPar->zcofx[ixy][mapxy][istrip][CPar->nzcofx[ixy][mapxy][istrip]-1]){
    Ich=CPar->nzcofx[ixy][mapxy][istrip]-1;}
  else{
    for(int ic=0;ic<CPar->nzcofx[ixy][mapxy][istrip];ic++){
       if(dir>0&&CPar->zcofx[ixy][mapxy][istrip][ic]>=ch){Ich=ic;break;}
       if(dir<0&&CPar->zcofx[ixy][mapxy][istrip][CPar->nzcofx[ixy][mapxy][istrip]-ic-1]<=ch){
         Ich=CPar->nzcofx[ixy][mapxy][istrip]-ic-1;break;
       }
    }
    ch=CPar->zcofx[ixy][mapxy][istrip][Ich];
  }

//---protection
  mapcof=(fabs(mapcof)>0.5)?0.5:mapcof;

  double mch = 0;
  double cor = 1.0;
  for(int i=0;i<CPar->nCofxpar/2;i++){
    int index=(mapcof<0)?i:i+CPar->nCofxpar/2;
    mch+=CPar->cofxpar[ixy][mapxy][istrip][Ich][il][index]*pow(mapcof,i);
  }
  cor=fabs(mch/CPar->zcofx[ixy][mapxy][istrip][Ich]);
  return cor;
}

double TrChargeYJ::GetRigIndexCor(int idsoft,float rig,int &ch,int dir){
  TrChargeYJDB *CPar=TrChargeYJDB::GetHead();
  int tkid,seedadd,il,ixy,istrip;
  ConvertIdsoft(idsoft,tkid,seedadd,il,ixy,istrip);
 
//---Search Index
  int Ich = 0;
  if(ch<=CPar->zrig[ixy][istrip][0]){Ich=0;}
  else if(ch>=CPar->zrig[ixy][istrip][CPar->nzrig[ixy][istrip]-1]){
    Ich=CPar->nzrig[ixy][istrip]-1;}
  else{
    for(int ic=0;ic<CPar->nzrig[ixy][istrip];ic++){
       if(dir>0&&CPar->zrig[ixy][istrip][ic]>=ch){Ich=ic;break;}
       if(dir<0&&CPar->zrig[ixy][istrip][CPar->nzrig[ixy][istrip]-ic-1]<=ch){
         Ich=CPar->nzrig[ixy][istrip]-ic-1;break;
       }
    }
    ch=CPar->zrig[ixy][istrip][Ich];
  }

  float maprig=fabs(rig);
  double rigcut = 2.0;
  if(!CPar->isreal)rigcut=2.4;
  else if(ixy==0&&istrip<=1)rigcut=1.5;
  if(maprig<rigcut)maprig=rigcut;
  double cor = 1.0;
  const int natan = 2;
  const int npol1 = 6;
  const int npol2 = 6;
  float cutrig1 = (CPar->isreal)?4.0:5.5;
  float cutrig2 = (CPar->isreal)?20.0:30.0;
  if(ixy==1&&istrip<=1&&CPar->isreal){
     cutrig1=(istrip==0)?10.0:12.0;cutrig2=60.0;
  }
  float par_atan[natan] = {0};
  float par_pol1[npol1] = {0};
  float par_pol2[npol1] = {0};
  if(maprig>cutrig2){
     for(int i=0;i<natan;i++){par_atan[i]=CPar->rigpar[ixy][istrip][Ich][il][i];}
     cor=par_atan[0]+par_atan[1]*TMath::ATan(maprig);
   }
   else if(maprig>cutrig1){
     for(int i=0;i<npol1;i++){par_pol1[i]=CPar->rigpar[ixy][istrip][Ich][il][i+natan];}
     cor=Getpol(maprig-cutrig2,par_pol1,npol1);
   }
   else{
     for(int i=0;i<npol2;i++){par_pol2[i]=CPar->rigpar[ixy][istrip][Ich][il][i+natan+npol1];}
     cor=Getpol(maprig-cutrig1,par_pol2,npol2);
   }
   return fabs(cor);
}

double TrChargeYJ::GetBetaIndexCor(int idsoft,float beta,int &ch,int dir){
  TrChargeYJDB *CPar=TrChargeYJDB::GetHead();
  int tkid,seedadd,il,ixy,istrip;
  ConvertIdsoft(idsoft,tkid,seedadd,il,ixy,istrip);

//---Search Index
  int Ich = 0;
  if(ch<=CPar->zbeta[ixy][istrip][0]){Ich=0;}
  else if(ch>=CPar->zbeta[ixy][istrip][CPar->nzbeta[ixy][istrip]-1]){
    Ich=CPar->nzbeta[ixy][istrip]-1;}
  else{
    for(int ic=0;ic<CPar->nzbeta[ixy][istrip];ic++){
       if(dir>0&&CPar->zbeta[ixy][istrip][ic]>=ch){Ich=ic;break;}
       if(dir<0&&CPar->zbeta[ixy][istrip][CPar->nzbeta[ixy][istrip]-ic-1]<=ch){
         Ich=CPar->nzbeta[ixy][istrip]-ic-1;break;
       }
    }
    ch=CPar->zbeta[ixy][istrip][Ich];
  }

  double cor = 1.0; 
  float mapbeta=fabs(beta);
//---protection
  float cutbetaa=0.3,cutbetab=1;
  if(mapbeta>cutbetab)mapbeta=cutbetab;
  if(mapbeta<cutbetaa)mapbeta=cutbetaa;

  cor = 0;
  for(int i=0;i<CPar->nBetapar;i++){
     cor+=CPar->betapar[ixy][istrip][Ich][il][i]*pow(mapbeta,i);
  }
  if(cor!=0&&cor<0.9)cor=0.9;
  return fabs(cor);
}
 
double TrChargeYJ::GetVAChannelIndexCor(int idsoft,int &ch,int dir){
  int tkid,seedadd,il,ixy,istrip;
  ConvertIdsoft(idsoft,tkid,seedadd,il,ixy,istrip);
  int icha=seedadd%64;//channel index

  TrChargeYJDB *CPar=TrChargeYJDB::GetHead();
  int Ich = 0;
  double mch=0,cor=1.0;
  if(icha<=1||icha>=61){//special treat to edge channels
//---Search Index
    if(ch<=CPar->zschannel[istrip][0]){Ich=0;}
    else if(ch>=CPar->zschannel[istrip][CPar->nzschannel[istrip]-1]){
      Ich=CPar->nzschannel[istrip]-1;}
    else{
      for(int ic=0;ic<CPar->nzschannel[istrip];ic++){
         if(dir>0&&CPar->zschannel[istrip][ic]>=ch){Ich=ic;break;}
         if(dir<0&&CPar->zschannel[istrip][CPar->nzschannel[istrip]-ic-1]<=ch){
           Ich=CPar->nzschannel[istrip]-ic-1;break;
         }
      }
      ch=CPar->zschannel[istrip][Ich];
    }
    if(icha>=61)icha-=59;
    mch=CPar->schannelpar[istrip][Ich][il][icha];
    cor=fabs(mch/CPar->zschannel[istrip][Ich]);
  }
  else{
//---Search Index
    if(ch<=CPar->zchannel[istrip][0]){Ich=0;}
    else if(ch>=CPar->zchannel[istrip][CPar->nzchannel[istrip]-1]){
      Ich=CPar->nzchannel[istrip]-1;}
    else{
      for(int ic=0;ic<CPar->nzchannel[istrip];ic++){
         if(dir>0&&CPar->zchannel[istrip][ic]>=ch){Ich=ic;break;}
         if(dir<0&&CPar->zchannel[istrip][CPar->nzchannel[istrip]-ic-1]<=ch){
           Ich=CPar->nzchannel[istrip]-ic-1;break;
         }
      }
      ch=CPar->zchannel[istrip][Ich];
    }
    double par[CPar->nChapar];
    for(int ipar=0;ipar<CPar->nChapar;ipar++){par[ipar]=CPar->channelpar[istrip][Ich][il][ipar];}
    mch=par[0]+par[1]*icha+par[2]*sin(par[3]*icha)+par[4]*icha*icha;
    cor=fabs(mch/CPar->zchannel[istrip][Ich]);
  }
  return cor;
}

double TrChargeYJ::GetEloss2YIndexCor(int il,int istrip,float mapcof,float mapinc,int &ch,int dir){
  TrChargeYJDB *CPar=TrChargeYJDB::GetHead();
//---Search Index
  int Ich = 0;
  if(ch<=CPar->zeloss2y[istrip][0]){Ich=0;}
  else if(ch>=CPar->zeloss2y[istrip][CPar->nzeloss2y[istrip]-1]){
    Ich=CPar->nzeloss2y[istrip]-1;}
  else{
     for(int ic=0;ic<CPar->nzeloss2y[istrip];ic++){
        if(dir>0&&CPar->zeloss2y[istrip][ic]>=ch){Ich=ic;break;}
        if(dir<0&&CPar->zeloss2y[istrip][CPar->nzeloss2y[istrip]-ic-1]<=ch){
          Ich=CPar->nzeloss2y[istrip]-ic-1;break;
        }
     }
     ch=CPar->zeloss2y[istrip][Ich];
  }

  double mch=0,cor=1.0; 
//---protection
  mapcof=fabs(mapcof);mapinc=fabs(mapinc);
  if(mapcof>0.5)mapcof=0.5;
  double inclimit=(il==0||il==8)?0.40:0.75;
  if(mapinc>inclimit)mapinc=inclimit;
  
  double par[CPar->npol];
  double intinc=0.4;
  double fixpoint;
  double inc=(mapinc>intinc)?mapinc-intinc:mapinc;
  int readpar=(mapinc>intinc)?5:0;
  double intpoint[9]={0,1.5,3.5,5.5,7.5,10.5,14.5,16.5,20.5};
  for(int i=0;i<9;i++){intpoint[i]=intpoint[i]*0.025;}
  for(int ipart=0;ipart<CPar->nint;ipart++){
    if(mapcof>=intpoint[ipart]&&mapcof<intpoint[ipart+1]){
      if(ipart<=1)     {fixpoint=intpoint[ipart+1];}
      else             {fixpoint=intpoint[ipart];}

      for(int i=0;i<CPar->npol;i++){
         par[i]=0;
         for(int ipar=0;ipar<CPar->nelosspar/2;ipar++){
              par[i]+=CPar->eloss2ypar[istrip][Ich][il][ipart][i][ipar+readpar]*pow(inc,ipar);
        }
      }
      mch=par[0]+par[1]*pow(fabs(mapcof-fixpoint),par[2]);
      break;
    }
  }
  cor=fabs(mch/CPar->zeloss2y[istrip][Ich]);
  return cor;
}



double TrChargeYJ::GetElossYIndexCor(int il,int istrip,float mapcof,float mapinc,int &ch,int dir,float power){
  TrChargeYJDB *CPar=TrChargeYJDB::GetHead();
//---Search Index
  int Ich = 0;
  if(ch<=CPar->zelossy[istrip][0]){Ich=0;}
  else if(ch>=CPar->zelossy[istrip][CPar->nzelossy[istrip]-1]){
    Ich=CPar->nzelossy[istrip]-1;}
  else{
    for(int ic=0;ic<CPar->nzelossy[istrip];ic++){
       if(dir>0&&CPar->zelossy[istrip][ic]>=ch){////search forward
          Ich=ic;break;
       }
       if(dir<0&&CPar->zelossy[istrip][CPar->nzelossy[istrip]-ic-1]<=ch){//search downward
         Ich=CPar->nzelossy[istrip]-ic-1;break;
       }
    }
    ch=CPar->zelossy[istrip][Ich];
  }

  double mch=0,cor=1.0; 
//---protection
  mapcof=fabs(mapcof);mapinc=fabs(mapinc);
  if(mapcof>0.5)mapcof=0.5;
  double inclimit=(il==0||il==8)?0.40:0.75;
  if(mapinc>inclimit)mapinc=inclimit;
  
  double par[CPar->npol];
  double intinc=0.4;
  double fixpoint;
  double inc=(mapinc>intinc)?mapinc-intinc:mapinc;
  int readpar=(mapinc>intinc)?5:0;
  double intpoint[9]={0,1.5,3.5,5.5,7.5,10.5,14.5,16.5,20.5};
  for(int i=0;i<9;i++){intpoint[i]=intpoint[i]*0.025;}
  for(int ipart=0;ipart<CPar->nint;ipart++){
    if(mapcof>=intpoint[ipart]&&mapcof<intpoint[ipart+1]){
      if(ipart<=1)     {fixpoint=intpoint[ipart+1];}
      else             {fixpoint=intpoint[ipart];}

      for(int i=0;i<CPar->npol;i++){
         par[i]=0;
         for(int ipar=0;ipar<CPar->nelosspar/2;ipar++){
             par[i]+=CPar->elossypar[istrip][Ich][il][ipart][i][ipar+readpar]*pow(inc,ipar);
        }
      }
      mch=par[0]+par[1]*pow(fabs(mapcof-fixpoint),par[2]);
      break;
    }
  }
  cor=fabs(mch/pow(CPar->zelossy[istrip][Ich],power));
  if(!TrChargeYJDB::GetHead()->isreal&&mapcof==0&&Ich==0){
     double cf=(il==0)?0.37:0.44;
     cor=cf*cor;
  }
  return cor;
}

double TrChargeYJ::GetElossXIndexCor(int il,int istrip,float mapcof,float mapinc,int stripx,int &ch,int dir,float power){
  TrChargeYJDB *CPar=TrChargeYJDB::GetHead();
  int striptype=stripx%10;
  if(striptype<1||striptype>4)striptype=(il>=2&&il<=7)?1:2;//Only 4 types 
//---Search Index
  int Ich = 0;
  if(ch<=CPar->zelossx[striptype-1][istrip][0]){Ich=0;}
  else if(ch>=CPar->zelossx[striptype-1][istrip][CPar->nzelossx[striptype-1][istrip]-1]){
    Ich=CPar->nzelossx[striptype-1][istrip]-1;}
  else{
    for(int ic=0;ic<CPar->nzelossx[striptype-1][istrip];ic++){
       if(dir>0&&CPar->zelossx[striptype-1][istrip][ic]>=ch){//search forward
         Ich=ic;break;
       }
       if(dir<0&&CPar->zelossx[striptype-1][istrip][CPar->nzelossx[striptype-1][istrip]-ic-1]<=ch){//search downward
         Ich=CPar->nzelossx[striptype-1][istrip]-ic-1;break;
       }
    }
    ch=CPar->zelossx[striptype-1][istrip][Ich];
  }

  double mch=0,cor=1.0; 
//---protection
  mapcof=fabs(mapcof);mapinc=fabs(mapinc);
  if(mapcof>0.5)mapcof=0.5;
  double inclimit=(il==0||il==8)?0.40:0.75;
  if(mapinc>inclimit)mapinc=inclimit;
  
  double par[CPar->npol];
  double intinc=0.4;
  double fixpoint;
  double inc=(mapinc>intinc)?mapinc-intinc:mapinc;
  int readpar=(mapinc>intinc)?5:0;
  double intpoint[9]={0,1.5,3.5,5.5,7.5,10.5,14.5,16.5,20.5};
  for(int i=0;i<9;i++){intpoint[i]=intpoint[i]*0.025;}
  for(int ipart=0;ipart<CPar->nint;ipart++){
    if(mapcof>=intpoint[ipart]&&mapcof<intpoint[ipart+1]){
      if(ipart<=1)     {fixpoint=intpoint[ipart+1];}
      else             {fixpoint=intpoint[ipart];}

      for(int i=0;i<CPar->npol;i++){
         par[i]=0;
         for(int ipar=0;ipar<CPar->nelosspar/2;ipar++){
            par[i]+=CPar->elossxpar[striptype-1][istrip][Ich][il][ipart][i][ipar+readpar]*pow(inc,ipar);
         }
      }
      mch=par[0]+par[1]*pow(fabs(mapcof-fixpoint),par[2]);
      break;
    }
  } 
  cor=fabs(mch/pow(CPar->zelossx[striptype-1][istrip][Ich],power)); 
  return cor; 
}

float TrChargeYJ::GetInnerXYWeight(vector<float> wei,vector<int> &Qstatus){
  float wxy = 0;
  int n = 0;
  int wsize=(int)wei.size();
  int edgei[2]={0};//max/min wei Index
  if(wsize==1)     {return 1e-3*wei.at(0);}//only one cluster 
  else if(wsize==2){return 1e-1*(wei.at(0)+wei.at(1));}//only two cluster 
  else if((int)Qstatus.size()>0){
    if(Qstatus.at(0)>0)return 1e-1*wei.at(0);//bad status
  }
  else if(wsize>2) {
    for(int ie=0;ie<2;ie++){GetQEdge(wei,edgei);}
  } 

  for(int ii=0;ii<(int)wei.size();ii++){
     if(wei.at(ii)==0)continue;
     if(wsize>2&&ii==edgei[0])continue;
     if(wsize>2&&ii==edgei[1])continue;
     wxy += 1/fabs(wei.at(ii));
     n++;
  }  
  if(wxy!=0)wxy = n/wxy;
  return wxy;
}

void TrChargeYJ::SetInnerLayerWeight(int ixy,vector<pair<int,int> > Lstripx,int &patt,float mean,vector<float> &vwei){
  vwei.clear();
  patt = 0;
  for(int ii=0;ii<(int)Lstripx.size();ii++){
     int il = Lstripx.at(ii).first;
     int stripx = Lstripx.at(ii).second;
     patt+=(ixy+1)*pow(10,il);
     float wei = 0;
     float resxy[2]={0};
     float resqxy[2]={mean,mean};
     SetXYRes(il,resqxy,resxy,stripx);
     if(mean<=0||resxy[ixy]==0){wei=0;}
     else{wei=1/pow(resxy[ixy],2);}
     vwei.push_back(wei);
  }  
} 

void TrChargeYJ::SetXYWeight(int il,float wei[],float q[],float newq[],float res[],int nw,int stripx,int qstatus[2]){
  int noverflow=stripx/1000000;
  float cor[10]={0};
  cor[0]=1;cor[1]=1;
  if(qstatus[0]!=0&&qstatus[1]==0)cor[0]=1e-1;
  if(qstatus[1]!=0&&qstatus[0]==0)cor[1]=1e-1;
  for(int i=0;i<nw;i++){
    if(q[i]<=0||wei[i]==0||res[i]==0){wei[i]=0;}
    else{wei[i]=1/pow(res[i],2)*cor[i];}
  }
}

double TrChargeYJ::SwitchStrip(int s1,int s2,float res[],int nw){
  return pow(res[s1],2)/pow(res[s2],2);
}

void TrChargeYJ::SetWeight(int tkid,int seedadd,float wei[],float q[],float res[],int nw,int noverflow,float adc[7],float tkcof){
  int il=-1,ixy=-1;
  ConvertLadderId(tkid,seedadd,il,ixy);
  double cor[10]={0};
  for(int i=0;i<nw;i++){cor[i]=(i==0)?1e-3:0;}//seed strip is backup

  if(ixy==0){//strips alters with saturation for x side
    if(noverflow==0){
      cor[0]=cor[1]=1;
      double trpar = 0.15,adccut = 60;
      cor[1]=GetSigmoid(adc[1],trpar,adccut);
    }
    else if(noverflow>=6){cor[6]=1e3;}
    else{
      int si = noverflow;
      cor[si-1]=cor[si]=cor[si+1]=1;
      double trpar = 0.2,trpar2 = 0.02;
      double adccut = (il<=1)?400:300;
      if(il==0&&tkid%100==11)adccut /= 2;
      cor[si-1]=SwitchStrip(si-1,si,res,nw);
      if(il>=2)cor[si]=GetSigmoid(adc[si],trpar2,adccut);
      cor[si+1]=GetSigmoid(adc[si+1],trpar,adccut);
    }
  }
  else if(ixy==1){
    cor[0]=cor[1]=1;cor[2]=cor[3]=cor[4]=0;
    if(adc[2]==0){cor[1]=1e-9;}
    else if(q[1]>q[0]&&q[1]>q[4]&&q[4]<15){cor[1]=1e-9;}
    else if(q[1]<q[0]&&q[1]<q[4]&&q[4]>15&&q[1]<15){cor[1]=1e-9;}
  }
  for(int i=0;i<nw;i++){
    if(q[i]<=0||wei[i]==0||res[i]==0){wei[i]=0;}
    else{wei[i]=1/pow(res[i],2)*cor[i];}
  }
}

int TrChargeYJ::GetQChaStatus(int tkid,int seedadd){
  int status = 0;
  int idsoft = GetIdsoft(tkid,seedadd,0);
  int vaid = Get_VAId(idsoft);
  int il=-1,ixy=-1;
  ConvertLadderId(tkid,seedadd,il,ixy);
  if(il==0&&(vaid==-50||vaid==-51||vaid==-52||vaid==-55))status = 1;
  return status; 
}

int TrChargeYJ::GetQGainStatus(int tkid,int seedadd){
  int il=-1,ixy=-1;
  ConvertLadderId(tkid,seedadd,il,ixy);
  int Ladnum=tkid%100+13;
  if(tkid<0)Ladnum++;
  TrChargeYJDB *CPar=TrChargeYJDB::GetHead();
  return CPar->ChaMean[il][Ladnum][seedadd];
}

int TrChargeYJ::GetQOccuStatus(int tkid,int seedadd,int opt){//0:low, 1:high
  int il=-1,ixy=-1;
  ConvertLadderId(tkid,seedadd,il,ixy);
  int Ladnum=tkid%100+13;
  if(tkid<0)Ladnum++;
  TrChargeYJDB *CPar=TrChargeYJDB::GetHead();
  if(opt==0){return CPar->ChaLowOccu[il][Ladnum][seedadd];}
  else      {return CPar->ChaHighOccu[il][Ladnum][seedadd];}
}

double TrChargeYJ::GetCorPHeTotalQ(double signal,int il,int ixy){
  if(signal<=0)return signal;
  const int nz = 2;
  const int nxy = 2;
  const int nL = 3;
  int caliz[nz] = {1,2};
  double caliq[nz][nxy][nL]={
    {1.02, 1.04, 1.04, 1.01, 0.92, 0.90}, 
    {1.04, 1.04, 1.04, 1.00, 0.92, 0.91}
  };
  int Lindex = (il==0)?0:1;
  if(il>=2&&il<=7)Lindex=2;
  double cor = 1;
  if(signal<(caliq[0][ixy][Lindex]*caliz[0])){
    cor = caliq[0][ixy][Lindex];
  }
  else if(signal>=(caliq[nz-1][ixy][Lindex]*caliz[nz-1])){
    cor = caliq[nz-1][ixy][Lindex];
  }
  else{
    int pickz=0;
    for(int iz=1;iz<nz;iz++){
       if((caliq[iz][ixy][Lindex]*caliz[iz])>signal){pickz=iz-1;break;}
    }
    double cor1 = caliq[pickz][ixy][Lindex];
    double cor2 = caliq[pickz+1][ixy][Lindex];
    double ch1=signal/cor1;
    double ch2=signal/cor2;
    double ww1 = fabs(ch1*ch1-caliz[pickz]*caliz[pickz]);
    double ww2 = fabs(ch2*ch2-caliz[pickz+1]*caliz[pickz+1]);
    cor=((ww1+ww2)==0)?1:(ww2*cor1+ww1*cor2)/(ww1+ww2);
  }
  return signal/cor; 
}

float TrChargeYJ::GetCombineStripQ(int tkid,int seedadd,float adc[7],float qs[7],float tkcof[2],float dadz[2],int stripx,float beta,float rig,int opt){
  int real=TrChargeYJDB::GetHead()->isreal;
  int il=-1,ixy=-1;
  ConvertLadderId(tkid,seedadd,il,ixy);
  int noverflow=stripx/1000000;
  const int nq = 7;//for y, 5th strip is the sum of signal
  float newq[nq]={0};
  float res[nq]={0},wei[nq]={0};
  float q0 = 1.0;
  double totalq = 0,qcut = 2.5;
  float uadc = 30000;//avoid y-side unusual large total adc 
  if(real && !(opt&kSingle)){
    if(ixy==0 && noverflow==0){
      double qseed = GetCorQ(adc[5],GetIdsoft(tkid,seedadd,0),tkcof,dadz,stripx,beta,rig,q0,opt);
      double qsecond = GetCorQ(adc[6],GetIdsoft(tkid,seedadd,1),tkcof,dadz,stripx,beta,rig,q0,opt);
      totalq = (qsecond>0)?0.5*(qseed+qsecond):qseed;
    }
    else if(ixy==1)totalq = (adc[4]>uadc)?0:GetCorQ(adc[4],GetIdsoft(tkid,seedadd,4),tkcof,dadz,stripx,beta,rig,q0,opt);
    totalq = GetCorPHeTotalQ(totalq,il,ixy);
    if(totalq>0 && totalq<qcut)return totalq;
  }

  if(real&&ixy==1){
    int useopt=(kPath|kEloss|kGain|kIncx|kEloss2|kGain2|kChannel);
    q0=(adc[4]>uadc)?1:GetCorQ(adc[4],GetIdsoft(tkid,seedadd,4),tkcof,dadz,stripx,beta,rig,q0,useopt);
  }
  for(int is=0;is<nq;is++){
     res[is]=-1;wei[is]=-1;
     if(real){
       if(ixy==0&&is>=noverflow+2){qs[is]=qs[is-1];continue;}
       if(ixy==1&&(is==2||is==3||is>=5)){qs[is]=0;continue;}
       if(ixy==1&&is==4&&adc[4]>uadc){qs[is]=qs[0];continue;}
       qs[is]=GetCorQ(adc[is],GetIdsoft(tkid,seedadd,is),tkcof,dadz,stripx,beta,rig,q0,opt);
     }
     else{
       if(is==0){qs[is]=GetCorQ(adc[is],GetIdsoft(tkid,seedadd,is),tkcof,dadz,stripx,beta,rig,q0,opt);}
       else{qs[is]=qs[0];}
     }
  }

  if(ixy==0){SetXStripRes(il,qs,res,nq,noverflow);}
  else      {
    for(int i=0;i<nq;i++){newq[i]=qs[0];}
    SetYStripRes(il,newq,res,nq);
  }
  SetWeight(tkid,seedadd,wei,qs,res,nq,noverflow,adc,tkcof[ixy]);

  float swei=0,sq=0;
  for(int i=0;i<nq;i++){swei+=wei[i];sq+=qs[i]*wei[i];}
  float tkq=(swei==0)?0:sq/swei;
  float oldtkq = -1;

  int num = 0;
  while(fabs(oldtkq-tkq)>1e-3&&ixy==0){
    if(num>5)break;
    if(num==5&&noverflow<5)tkq=0.5*(qs[noverflow]+qs[noverflow+1]);
    oldtkq=tkq;
    
    for(int i=0;i<nq;i++){newq[i]=tkq;}
    if(ixy==0){SetXStripRes(il,newq,res,nq,noverflow);}
    else      {SetYStripRes(il,newq,res,nq);}
    SetWeight(tkid,seedadd,wei,qs,res,nq,noverflow,adc,tkcof[ixy]);
 
    swei=0,sq=0;
    for(int i=0;i<nq;i++){swei+=wei[i];sq+=qs[i]*wei[i];}
    tkq=(swei==0)?0:sq/swei;
    num++;
  }
//  if(!TrChargeYJDB::GetHead()->isreal&&!(opt&kNosmear)&&tkq>0&&rig>0){
  if(!TrChargeYJDB::GetHead()->isreal&&!(opt&kNosmear)&&tkq>0){//bug fixed by QY
    tkq=GetSmearXYQ(tkid,seedadd,tkq);
  }
  if(real && !(opt&kSingle)){
    if(totalq>=qcut&&totalq<(qcut+1)){
      float wei = -(totalq-qcut)+1;
      tkq = wei*totalq+(1-wei)*tkq;
    }
  }
  return tkq;
}

double TrChargeYJ::GetSmearXYQ(int tkid,int seedadd,float tkq){
  int il=-1,ixy=-1;
  ConvertLadderId(tkid,seedadd,il,ixy);

  const int nch = 20;
  int ch[nch]={1,2,3,4,5,6,7,8,10,12,14,16,18,20,21,22,23,24,25,26};
  int Lindex = 0;
  if(ixy==0){
    if(il==0||il==1||il==8){Lindex = 0;}
    else                   {Lindex = 1;}
  }
  else                     {Lindex = 2;}
  double newq = 0;
  double smear=AddSmear(abs(tkid),2);
  int dir = 0;
  if(tkq<ch[0]){
    double wid=GetSmearXYIndexWidth(il,dir,Lindex,0);
    newq = GetSmearXYIndexQ(tkq,wid,smear,dir,ch[0]);
  }
  else if(tkq>=ch[nch-1]){
    double wid=GetSmearXYIndexWidth(il,dir,Lindex,nch-1);
    newq = GetSmearXYIndexQ(tkq,wid,smear,dir,ch[nch-1]);
  }
  else {
    for(int ich=0;ich<nch;ich++){
       if(tkq>=ch[ich]&&tkq<ch[ich+1]){
         int dir1 = 0,dir2 = 0;
         double wid1 = GetSmearXYIndexWidth(il,dir1,Lindex,ich);
         double newq1=GetSmearXYIndexQ(tkq,wid1,smear,dir1,ch[ich]);
         double wid2 = GetSmearXYIndexWidth(il,dir2,Lindex,ich+1);
         double newq2=GetSmearXYIndexQ(tkq,wid2,smear,dir2,ch[ich+1]);
         double ww1 = fabs(tkq*tkq-ch[ich]*ch[ich]);
         double ww2 = fabs(tkq*tkq-ch[ich+1]*ch[ich+1]);
         double wei = ww2/(ww1+ww2);
         double chosepdf = AddSmear(il*100+1,1);
         newq = (chosepdf<wei)?newq1:newq2;
         break;
      }
    }
  }
  return newq;
}

double TrChargeYJ::GetSmearXYIndexWidth(int il,int &dir,int Lindex,int ich){
  const int nch = 20;
  double p0L[3][nch]={
    {1.00000,0.93263,0.95000,0.95000,0.84258,0.82999,0.80682,0.99000,0.73772,0.77191,0.50000,0.50000,0.73870,0.50576,0.51493,0.55724,0.50000,0.50000,0.54975,1.00000},
    {1.00000,0.89765,0.95000,0.95000,0.80258,0.88999,0.76541,0.99500,0.77772,0.81995,0.50000,0.70774,0.51984,0.59574,0.57827,0.50000,0.63565,0.64889,0.64848,0.51000},
    {1.00000,1.00000,0.94000,0.82000,0.78258,0.82999,0.50000,0.50000,0.65772,0.77191,0.50000,0.63416,0.50000,0.50000,0.57196,0.53992,0.56068,0.50000,0.50000,0.52075}
   };
  double p1L[3][nch]={
    {0.00000,0.00000,0.00000,0.10000,0.80000,0.86932,0.90951,0.00000,0.88146,0.87938,0.86586,0.59435,0.83362,0.50000,0.96333,0.90037,0.90584,0.93973,0.88492,1.00000},
    {0.00000,0.00000,0.00000,0.55000,0.80000,0.70000,0.90898,0.00000,0.88096,0.47981,0.92164,0.92988,0.81521,0.85548,0.84811,0.80416,0.79039,0.82322,0.71759,0.89898},
    {0.00000,0.00000,0.00000,0.00000,0.99000,0.14320,0.58755,0.67690,0.99534,0.97214,0.97941,0.93871,0.75299,0.80504,0.86595,0.60118,0.70817,0.75283,0.78194,0.78172}
   };
  double p2L[3][nch]={
    {0.20942,0.55911,1.07888,1.62525,2.20865,2.70688,3.19143,4.08865,6.03591,8.04445,10.5323,12.9757,19.2030,25.2260,29.7808,33.3382,36.3530,36.7338,40.6284,55.9143},
    {0.20489,0.53622,1.08321,1.62945,2.16179,2.76227,3.03518,4.04452,5.87065,7.83318,10.3412,13.2444,13.6934,20.3934,22.4192,23.8410,28.0521,26.5581,26.9091,28.6865},
    {0.20863,0.48842,1.07974,1.53466,2.61975,4.04750,6.71006,10.4765,7.75153,8.59381,11.0035,13.5211,12.7197,23.7877,26.4558,21.1556,30.6006,33.4347,36.7183,42.1779}
   };
  double p3L[3][nch]={
    {0.20942,0.50128,1.29466,1.78778,2.39652,3.17798,5.24341,4.08865,12.5996,13.2289,18.4232,26.8321,45.5593,43.6743,62.1936,66.4274,71.1993,70.0000,70.6836,23.8343},
    {0.20489,0.47947,1.08321,1.62945,2.16179,3.35819,5.45740,4.04452,10.1058,11.0474,17.1486,26.5969,20.3392,40.8894,43.5066,40.8410,68.3720,68.9150,90.1198,59.0171},
    {0.20863,0.48842,1.07974,1.53466,4.68674,14.7096,15.3618,13.4266,15.1718,10.5938,11.3298,20.4470,25.7982,40.2596,41.0074,49.4559,63.4242,63.4474,63.2001,55.1779}
   };
  double p4L[3][nch]={
    {0.20942,0.50128,1.07888,1.62525,2.67865,3.40688,4.55103,4.08865,6.03591,12.8445,10.5323,22.5378,10.4905,34.9674,34.2265,36.6756,38.3515,48.4895,53.4305,63.9143},
    {0.20489,0.47947,1.08321,1.62945,2.16179,2.76227,4.45773,4.04452,5.87065,7.83318,11.7960,17.4347,13.6934,20.3934,27.8360,23.8410,34.3194,27.6296,23.6317,23.8792},
    {0.20863,0.48842,1.07974,1.53466,6.00981,3.09871,5.88349,5.52630,20.9907,14.9938,16.4014,16.8003,12.0392,20.8478,28.7506,23.4838,32.5081,26.2014,28.8382,26.2014}
   };
  double p5L[3][nch]={
    {0.20942,0.50128,1.42412,1.78778,2.17865,6.09765,7.88193,4.08865,15.7719,8.04445,10.5323,27.5148,10.4905,64.0381,62.1948,62.8839,71.1988,63.0000,60.6833,63.9143},
    {0.20489,0.47947,1.55982,1.62945,4.65856,7.52853,10.8352,4.04452,22.2319,20.3222,28.7938,28.5940,35.9200,40.8955,33.1027,90.2915,100.404,95.7849,130.946,65.8792},
    {0.20863,0.48842,1.52526,1.53466,6.77207,5.98777,3.12951,4.89350,18.5972,30.5938,18.8099,25.4461,25.0392,25.0000,12.6298,13.9923,47.4242,40.7390,77.1097,26.2014}
   };
  double p6L[3][nch]={
    {0.20942,0.62660,1.66147,3.50041,4.88301,7.99236,10.0565,10.7582,6.03591,20.4633,25.1733,22.5378,34.8623,21.9508,126.298,93.3933,115.118,100.000,100.298,63.9143},
    {0.20489,0.64729,2.13314,5.57362,9.24641,9.84994,15.8237,15.8551,10.4565,30.8589,44.6804,56.5865,40.2382,60.7559,121.307,30.5800,126.298,90.0000,120.298,140.078},
    {0.20863,0.48842,2.22759,3.58317,9.87995,7.81936,10.9754,13.9890,22.9907,38.0901,30.4407,36.0876,30.1591,50.0000,63.1490,49.1163,72.4255,50.7314,55.5050,50.1779}
   };
  double mcsigL[3][nch]={
    {0.20942,0.50128,1.08432,1.62525,2.17865,2.70688,2.78193,4.08865,6.03591,8.04445,10.5323,12.5711,10.4905,21.3984,21.0000,21.3648,23.0000,23.0000,23.0000,23.8343},
    {0.20489,0.47947,1.08321,1.62945,2.16179,2.76227,2.83518,4.04452,5.87065,7.83318,10.2997,12.4276,13.6934,20.3934,23.0000,23.8410,23.0000,23.0000,23.0000,23.8792},
    {0.20863,0.48842,1.07974,1.53466,2.21547,3.09871,3.12951,4.89350,7.75153,8.59381,10.8099,12.6298,12.0392,20.8478,20.0000,13.9923,26.0000,26.2014,26.2014,26.2014}
   };

  double mcsig = mcsigL[Lindex][ich];
  if(mcsig<1e-2){dir=0;return 0;}
  double rcore = p0L[Lindex][ich];

  double p11 = p3L[Lindex][ich]+p4L[Lindex][ich];
  double p1 = (p11==0)?0:p3L[Lindex][ich]/p11;
  double rasyr1 = (1-p0L[Lindex][ich])*p1L[Lindex][ich]*p1;
  double rasyl1 = (1-p0L[Lindex][ich])*p1L[Lindex][ich]*(1-p1);
  double p22 = p5L[Lindex][ich]+p6L[Lindex][ich];
  double p2 = (p22==0)?0:p5L[Lindex][ich]/p22;
  double rasyr2 = (1-p0L[Lindex][ich])*(1-p1L[Lindex][ich])*p2;
  double rasyl2 = (1-p0L[Lindex][ich])*(1-p1L[Lindex][ich])*(1-p2);
  double iftail=AddSmear(il+1,1);
  double datasig = p2L[Lindex][ich];
  if(iftail<rcore)                       {datasig = p2L[Lindex][ich];dir=0;}
  else if(iftail<(rcore+rasyr1))              {datasig = p3L[Lindex][ich];dir=1;}
  else if(iftail<(rcore+rasyr1+rasyr2))       {datasig = p5L[Lindex][ich];dir=2;}
  else if(iftail<(rcore+rasyr1+rasyr2+rasyl1)){datasig = p4L[Lindex][ich];dir=-1;}
  else                                        {datasig = p6L[Lindex][ich];dir=-2;}

  double wch = 0;
  if(datasig>mcsig)wch = sqrt(fabs(pow(datasig,2)-pow(mcsig,2)));
  return wch;
}

double TrChargeYJ::GetSmearXYIndexQ(float tkq,double wid,double smear,int dir,int ch){
  double newq = sqrt(fabs(pow(tkq,2)+wid*smear/ch*tkq));
  if((dir>0&&newq<tkq)||(dir<0&&newq>tkq))newq=2*tkq-newq;
  return fabs(newq);
}

#ifdef __ROOTSHAREDLIBRARY__
#include "root.h"
#else
#include "commons.h"
#include "cern.h"
#include "extC.h"
#endif

double TrChargeYJ::AddSmear(int key,int type){
  double rnd[1];
  #ifdef __ROOTSHAREDLIBRARY__
  AMSEventR::GetRandArray(8993306+key, type,  1, rnd);
  #else
  rnd[0]=(type==2)?rnormx():RNDM(-1);
  #endif
  return rnd[0]; 
}

double TrChargeYJ::AdjustMCXYCor(float tkq){
  const int nch = 18;
  int ch[nch]     ={3,  4,  5,  7,  8,  10, 12, 14, 15, 16, 17, 18, 20, 21, 22, 24, 25, 26};
  double adxy[nch]={1.0,0.6,0.2,1.0,0.6,0.4,1.0,0.8,1.0,2.0,0.8,1.8,1.0,0.8,0.6,0.8,0.6,1.0};
  double cor = 1.0;
  if(tkq<ch[0]){cor = adxy[0];}
  else if(tkq>=ch[nch-1]){cor = adxy[nch-1];}
  else{
    for(int ich=0;ich<nch;ich++){
       if(tkq>=ch[ich]&&tkq<ch[ich+1]){
         double slope = (adxy[ich+1]-adxy[ich])/(ch[ich+1]-ch[ich]);
         cor = (slope*(tkq-ch[ich])+adxy[ich]);//linear interpolation
         break;
        }
    }
  }
  return cor;
}

void TrChargeYJ::SetXYRes(int il,float *q,float *res,int stripx){
  TrChargeYJDB *CPar=TrChargeYJDB::GetHead();
  res[0]=res[1]=999;
  int maxz = 26;
//---x side
  float resq=(q[0]>maxz)?maxz:q[0];
  float rescx[CPar->nResx];
  for(int i=0;i<CPar->nResx;i++){rescx[i]=CPar->rescxpar[il][i];}
  res[0]=Getpol(resq,rescx,CPar->nResx);
//---yside
  float rescy[CPar->nResy];
  for(int i=0;i<CPar->nResy;i++){rescy[i]=CPar->rescypar[il][i];}
  resq=(q[1]>maxz)?maxz:q[1];
  res[1]=Getpol(resq,rescy,10)+Getgaus(resq,rescy[10],rescy[11],rescy[12]);

  for(int ixy=0;ixy<2;ixy++){
    if(q[ixy]>maxz)res[ixy]=res[ixy]/maxz*q[ixy];
    if(res[ixy]>1)res[ixy]=1;
  }
}

float TrChargeYJ::GetCombineLayerQ(int tkid,float clq[2],int stripx,int qstatus[2]){
//----
  const int nxy = 2;
  float q[nxy]={clq[0],clq[1]},newq[nxy]={0};
  float res[nxy]={0},wei[nxy]={-1,-1};
  float oldtkq=0,tkq=0,totalwei=0;
  int num=0;
//---- 
  int il=-1,ixy=-1;
  ConvertLadderId(tkid,0,il,ixy);
  SetXYRes(il,q,res,stripx);
  SetXYWeight(il,wei,q,q,res,nxy,stripx,qstatus);//combine x y

  for(int i=0;i<nxy;i++){totalwei+=wei[i];}
  tkq=(totalwei==0)?0:(q[0]*wei[0]+q[1]*wei[1])/totalwei;
  while(fabs(oldtkq-tkq)>1e-3){
     if(num>5)break;
     oldtkq=tkq;
     newq[0]=tkq;newq[1]=tkq;
     SetXYRes(il,newq,res,stripx);
     SetXYWeight(il,wei,q,q,res,nxy,stripx,qstatus);//combine x y
     totalwei=0;
     for(int i=0;i<nxy;i++){totalwei+=wei[i];}
     tkq=(totalwei==0)?0:(q[0]*wei[0]+q[1]*wei[1])/totalwei;
     num++;
  }
  return tkq;
}

void TrChargeYJ::GetQEdge(vector<float> signal,int edgei[2]){
  float qmax=0,qmin=100;
  for (int ii=0; ii<(int)signal.size(); ii++) {
     if(signal.at(ii)>qmax){qmax=signal.at(ii);edgei[0]=ii;}
     if(signal.at(ii)<qmin){qmin=signal.at(ii);edgei[1]=ii;}
  }
}

void TrChargeYJ::RemoveBadStatusQ(vector<float> &InQs2,vector<pair<int,int> > &Lstripx,vector<int> &Qstatus,int nbadhit){
  if(((int)InQs2.size()-nbadhit)>=1){
     for(int ii=0;ii<(int)InQs2.size();ii++){
        int stripx = Lstripx.at(ii).second;
        int noverflow=stripx/1000000;
        if(Qstatus.at(ii)>0||noverflow>=6){
          InQs2.erase(InQs2.begin()+ii);
          Lstripx.erase(Lstripx.begin()+ii);
          Qstatus.erase(Qstatus.begin()+ii);
          ii--;
       }
     }
  }
}

void TrChargeYJ::RemoveTailQ(int ixy,vector<float> &InQs2,vector<pair<int,int> > &Lstripx,float mean,int iloop){
  int edgei[2]={0};//max/min Q Index
  float res[2]={0};
  float resq[2]={mean,mean};

  for(int ie=0;ie<2;ie++){//remove max/min Q out of 3 sigma
     if((int)InQs2.size()>1){
       GetQEdge(InQs2,edgei);
       SetXYRes(Lstripx.at(edgei[ie]).first,resq,res,Lstripx.at(edgei[ie]).second);//Get charge resolution at mean
       int stripx=Lstripx.at(edgei[ie]).second;
       int noverflow=stripx/1000000;
       float ures=(res[0]<res[1])? res[0]:res[1];
       bool xtail=0,ytail=0;
       if(TrChargeYJDB::GetHead()->isreal){
         xtail=(ixy==0&&ie==0&&iloop==3&&noverflow==1);
         xtail=(xtail||(ixy==0&&ie==0&&iloop==3&&InQs2.at(edgei[ie])>9&&InQs2.at(edgei[ie])<9.5));
         xtail=(xtail||(ixy==0&&ie==0&&noverflow==3&&InQs2.at(edgei[ie])<24));
         ytail=(ixy==1&&ie==0&&iloop==3&&InQs2.at(edgei[ie])>26);
         ytail=(ytail||(ixy==1&&ie==0&&iloop==3&&InQs2.at(edgei[ie])>1&&InQs2.at(edgei[ie])<4));
       }
       else{
         xtail=(xtail||(ixy==0&&ie==1&&iloop==3&&InQs2.at(edgei[ie])>2&&InQs2.at(edgei[ie])<9.8));
         const int ncutx = 7;
         float cutx[2][ncutx]={
           {2.00,12.0,13.0,14.0,16.0,18.0,22.0},
           {11.4,12.5,13.6,15.5,16.9,18.8,22.1}
         };
         for(int ii=0;ii<ncutx;ii++){
            xtail=(xtail||(ixy==0&&ie==0&&iloop==3&&InQs2.at(edgei[ie])>cutx[0][ii]&&InQs2.at(edgei[ie])<cutx[1][ii]));
         }
         ytail=(ytail||(ixy==1&&ie==0&&iloop==3&&InQs2.at(edgei[ie])<4));
         ytail=(ytail||(ixy==1&&ie==0&&iloop==3&&InQs2.at(edgei[ie])>24&&InQs2.at(edgei[ie])<27.5));
       }
       xtail=(xtail&&(int)InQs2.size()>2);
       ytail=(ytail&&(int)InQs2.size()>2);

       bool btail=(fabs(InQs2.at(edgei[ie])-mean)>3*ures);
       btail=(btail&&!(ie==0&&fabs(InQs2.at(edgei[ie])-mean)-fabs(InQs2.at(edgei[ie+1])-mean)<1e-3));
       btail=(btail&&!(ie==1&&fabs(InQs2.at(edgei[ie])-mean)-fabs(InQs2.at(edgei[ie-1])-mean)<1e-3));
       btail=(btail&&!(iloop==2&&ixy==1));
       if(btail||xtail||ytail){
         InQs2.erase(InQs2.begin()+edgei[ie]);
         Lstripx.erase(Lstripx.begin()+edgei[ie]);
       }
    }
  }
}

float TrChargeYJ::GetWeightedQMean(vector<float> signal,vector<float> wei,float &rms,int &n){
  float mean = 0;
  rms = 0;
  n   = 0;
  float totalwei = 0;
  for (int ii=0; ii<(int)signal.size(); ii++) {
     if (signal.at(ii)<0) continue;
     mean += signal.at(ii) * wei.at(ii);
     totalwei += wei.at(ii);
     n++;
  }
  if(n>=1){
     mean /= totalwei;
     for (int ii=0; ii<(int)signal.size(); ii++) {
        if (signal.at(ii)<0) continue;
        rms += pow(signal.at(ii)-mean,2) * wei.at(ii);
     }
     rms = sqrt(rms/ totalwei);
  }
  return mean;
}

float TrChargeYJ::GetQMean(vector<float> signal,float &rms,int &n,int opt){
  if((int)signal.size()>2&&opt==1){
    int edgei[2]={0};//max/min wei Index
    for(int ie=0;ie<2;ie++){
       GetQEdge(signal,edgei);
       signal.erase(signal.begin()+edgei[ie]);
     }
  }

  float mean = 0;
  rms  = 0;
  n    = 0;
  for (int ii=0; ii<(int)signal.size(); ii++) {
    if (signal.at(ii)<0) continue;
    mean += signal.at(ii);
    rms  += pow(signal.at(ii),2);
    n++;
  }
  if(n>=1){
    mean /= n;
    rms  /= n;
    rms = sqrt(rms - mean*mean);
  }
  return mean;
}

float TrChargeYJ::GetInnerQRMS(vector<float> Qx,vector<float> wx,vector<float> Qy,vector<float> wy){
  vector<float>::iterator it;
  it = Qy.begin();
  Qy.insert(it,Qx.begin(),Qx.end());//include all clusters
  it = wy.begin();
  wy.insert(it,wx.begin(),wx.end());//weight for all clusters
  float rms = 0;
  int points = 0;
  float allmean=TrChargeYJ::GetHead()->GetWeightedQMean(Qy,wy,rms,points);
  return rms;
}

void TrChargeYJ::SetYStripRes(int il,float q[],float res[],int nq){
  TrChargeYJDB *CPar=TrChargeYJDB::GetHead();
  float par[CPar->nResy];
  int npolp[7]={10,5,3,3,10,0,0};
  for(int istrip=0;istrip<nq;istrip++){
     for(int ipar=0;ipar<CPar->nResy;ipar++){
         par[ipar]=CPar->resypar[il][istrip][ipar];
     }
     float resq=(q[istrip]>28)?28:q[istrip];
     res[istrip]=Getpol(resq,par,npolp[istrip]);
//----Add gaus to decrease the weight of q in saturated range
     if(istrip==0)res[istrip]+=Getgaus(resq,par[10],par[11],par[12]);
     if(istrip==1)res[istrip]+=Getgaus(resq,par[5],par[6],par[7]);
     res[istrip]=fabs(res[istrip]);
  }
}

void TrChargeYJ::SetXStripRes(int il,float q[],float res[],int nq,int noverflow){
  TrChargeYJDB *CPar=TrChargeYJDB::GetHead();
  int resnflow=(noverflow>=CPar->nOflow)?(CPar->nOflow-1):noverflow;
  for(int istrip=0;istrip<nq;istrip++){
    float resq=(q[istrip]>28)?28:q[istrip];
    float par[CPar->nResx];
    for(int ipar=0;ipar<CPar->nResx;ipar++){
       par[ipar]=CPar->resxpar[il][istrip][resnflow][ipar];
    }
    res[istrip]=fabs(Getpol(resq,par,CPar->nResx));
  }
}

//----------
float TrQYJCluster::GetQ(float qs[7],int opt){
  return TrChargeYJ::GetHead()->GetCombineStripQ(_tkid,_seedadd,_adc,qs,_tkcof,_dadz,_stripx,_beta,_rig,opt);
}

int TrQYJCluster::GetQStatus(){
  return
    (TrChargeYJ::GetHead()->GetQChaStatus(_tkid,_seedadd)>0)*0x1+
    (TrChargeYJ::GetHead()->GetQGainStatus(_tkid,_seedadd)>0)*0x2+
    (TrChargeYJ::GetHead()->GetQOccuStatus(_tkid,_seedadd,0)>0)*0x4+
    (TrChargeYJ::GetHead()->GetQOccuStatus(_tkid,_seedadd,1)>0)*0x8;
}

int TrQYJCluster::Clear(){
  _tkid=0;
  _seedadd=-1;
  for(int ii=0;ii<7;ii++){_adc[ii]=_qs[ii]=0;}
  for(int ia=0;ia<2;ia++){_tkcof[ia]=0;_dadz[ia]=0;}
  _stripx=0;
  _beta=1;
  _rig=0;
  _qstatus=0;
  Q=0;
  return 0;
}

TrQYJCluster TrQYJCluster::Add(TrQYJCluster yjcl1,double ww1,TrQYJCluster yjcl2,double ww2){
  TrQYJCluster yjcla=(ww1>=ww2)? yjcl1: yjcl2;//choose the more weighted
  if(ww1>0&&ww2>0){
    double ww1n=ww1/(ww1+ww2);
    double ww2n=ww2/(ww1+ww2);
    yjcla.Q=yjcl1.Q*ww1n+yjcl2.Q*ww2n;
    for(int ixy=0;ixy<2;ixy++){
       double seedpos=(double(yjcl1._seedadd)+yjcl1._tkcof[ixy])*ww1n+(double(yjcl2._seedadd)+yjcl2._tkcof[ixy])*ww2n;
       yjcla._seedadd=int(seedpos+0.5);
       yjcla._tkcof[ixy]=seedpos-yjcla._seedadd;
    }
    for(int ii=0;ii<7;ii++){
      yjcla._adc[ii]=yjcl1._adc[ii]*ww1n+yjcl2._adc[ii]*ww2n;
      yjcla._qs[ii]=yjcl1._qs[ii]*ww1n+yjcl2._qs[ii]*ww2n;
    }
  }
  return yjcla;
}

int TrQYJHit::Clear(){
  qcl[0].Clear();
  qcl[1].Clear();
  Q=0;
  return 0;
}

int TrQYJHit::GetHitType(){
  if(HasCluster(0)&&HasCluster(1))return 2;
  else if(HasCluster(1))return 1;
  else                  return 0;
}

float TrQYJHit::GetHitQ(){
  int tkid=HasCluster(0)? qcl[0]._tkid: qcl[1]._tkid;
  float clq[2]={qcl[0].Q, qcl[1].Q};
  int stripx=qcl[0]._stripx;
  int qstatus[2]={qcl[0]._qstatus,qcl[1]._qstatus};
  return TrChargeYJ::GetHead()->GetCombineLayerQ(tkid,clq,stripx,qstatus);
}

int TrQYJHit::GetiLayer(){
  int tkid=HasCluster(0)? qcl[0]._tkid: qcl[1]._tkid;
  int il=-1,ixy=-1;
  TrChargeYJ::GetHead()->ConvertLadderId(tkid,0,il,ixy);
  return il;
}

int TrQYJTrack::Clear(){
  qhits.clear();
  InnerQ=InnerQRMS=InnerQPoints=InnerQPatt=0;
  for(int i=0;i<2;i++)InnerQ2[i]=InnerQRMS2[i]=InnerQPoints2[i]=InnerQPatt2[i]=0;
  return 0;
}

float TrQYJTrack::GetInnerQ(int sopt){//sopt: 0:Gaussion
  InnerQ=InnerQRMS=InnerQPoints=InnerQPatt=0;
  for(int i=0;i<2;i++)InnerQ2[i]=InnerQRMS2[i]=InnerQPoints2[i]=InnerQPatt2[i]=0;
  vector<float> InQs2[2];
  vector<float> vwei[2];//weight for combine
  vector<pair<int,int> > Lstripx[2];//lay and stripx
  vector<int> Qstatus[2];
  int nbadhit[2]={0};
  double wxy[2]={0};//weight for InnerX and InnerY
  for(int ih=0;ih<qhits.size();ih++){
     TrQYJHit &qhit=qhits[ih];
     int ilay=qhit.GetiLayer();
     if(ilay<=0||ilay>=8)continue;
     for(int ixy=0;ixy<2;ixy++){
        if(qhit.HasCluster(ixy)){
          InQs2[ixy].push_back(qhit.GetCluster(ixy)->Q);
          if(qhit.GetCluster(ixy)->_qstatus)nbadhit[ixy]++;
          Qstatus[ixy].push_back(qhit.GetCluster(ixy)->_qstatus);
          Lstripx[ixy].push_back(make_pair(ilay,qhit.GetCluster(ixy)->_stripx));
        }
     }
  }

  for(int iloop=0;iloop<4;iloop++){//more loop to identify tail Q
     InnerQPatt=InnerQPoints=0;
     for(int ixy=0;ixy<2;ixy++){ //Calculate InnerXQ & InnerYQ
        if(iloop==0)TrChargeYJ::GetHead()->RemoveBadStatusQ(InQs2[ixy],Lstripx[ixy],Qstatus[ixy],nbadhit[ixy]);
        float mean=(iloop==2)?InnerQ:TrChargeYJ::GetHead()->GetQMean(InQs2[ixy],InnerQRMS2[ixy],InnerQPoints2[ixy],1);//truncated mean
        if(iloop>=1)TrChargeYJ::GetHead()->RemoveTailQ(ixy,InQs2[ixy],Lstripx[ixy],mean,iloop);//out of 3 signal of the Gaus

        TrChargeYJ::GetHead()->SetInnerLayerWeight(ixy,Lstripx[ixy],InnerQPatt2[ixy],mean,vwei[ixy]);
        if((int)InQs2[ixy].size()<5){InnerQ2[ixy]=TrChargeYJ::GetHead()->GetWeightedQMean(InQs2[ixy],vwei[ixy],InnerQRMS2[ixy],InnerQPoints2[ixy]);}
        else                        {InnerQ2[ixy]=TrChargeYJ::GetHead()->GetQMean(InQs2[ixy],InnerQRMS2[ixy],InnerQPoints2[ixy],1);}

        float oldQ = -1;
        int num = 0;
        while(fabs(oldQ-InnerQ2[ixy])>1e-3){//loop
          if(num > 10)break;
          oldQ = InnerQ2[ixy];
          mean = (iloop==0)? InnerQ2[ixy]:InnerQ;
          TrChargeYJ::GetHead()->SetInnerLayerWeight(ixy,Lstripx[ixy],InnerQPatt2[ixy],mean,vwei[ixy]);
          if((int)InQs2[ixy].size()<5){InnerQ2[ixy]=TrChargeYJ::GetHead()->GetWeightedQMean(InQs2[ixy],vwei[ixy],InnerQRMS2[ixy],InnerQPoints2[ixy]);}
          else                        {InnerQ2[ixy]=TrChargeYJ::GetHead()->GetQMean(InQs2[ixy],InnerQRMS2[ixy],InnerQPoints2[ixy],1);}
          wxy[ixy]=TrChargeYJ::GetHead()->GetInnerXYWeight(vwei[ixy],Qstatus[ixy]);
          if(ixy==1&&!TrChargeYJDB::GetHead()->isreal)wxy[ixy]=wxy[ixy]*TrChargeYJ::GetHead()->AdjustMCXYCor(mean);
          num++;
        }
        InnerQPatt += InnerQPatt2[ixy];
        InnerQPoints += InnerQPoints2[ixy];
     }

//--------------------------Combine InnerX & InnerY Q
     if((wxy[0]+wxy[1])!=0)InnerQ=(InnerQ2[0]*wxy[0]+InnerQ2[1]*wxy[1])/(wxy[0]+wxy[1]);
     float oldQ = -1;
     int num = 0;
     while(fabs(oldQ-InnerQ)>1e-3){//loop
       if(num > 10)break;
       oldQ = InnerQ;
       for(int ixy=0;ixy<2;ixy++){
          TrChargeYJ::GetHead()->SetInnerLayerWeight(ixy,Lstripx[ixy],InnerQPatt2[ixy],InnerQ,vwei[ixy]);
          wxy[ixy]=TrChargeYJ::GetHead()->GetInnerXYWeight(vwei[ixy],Qstatus[ixy]);
          if(ixy==1&&!TrChargeYJDB::GetHead()->isreal)wxy[ixy]=wxy[ixy]*TrChargeYJ::GetHead()->AdjustMCXYCor(InnerQ);
       }
       if((wxy[0]+wxy[1])!=0)InnerQ=(InnerQ2[0]*wxy[0]+InnerQ2[1]*wxy[1])/(wxy[0]+wxy[1]);
       num++;
     }
  }
  InnerQRMS = TrChargeYJ::GetHead()->GetInnerQRMS(InQs2[0],vwei[0],InQs2[1],vwei[1]);
  return InnerQ;
}

TrQYJHit *TrQYJTrack::GetHitL(int ilay){
  for(int ih=0;ih<qhits.size();ih++){
    if(qhits[ih].GetiLayer()==ilay)return &(qhits[ih]);
  }
  return 0;
}

float TrQYJTrack::GetLQ(int ilay){
  TrQYJHit *hit=GetHitL(ilay);
  if(hit){return hit->Q;}
  else   {return 0;}
}
