///--Author Q.Yan qyan@cern.ch
///--Tool For Event Analysis
#ifndef _QAnalysis_
#define _QAnalysis_
#include <fstream>
#include <map>
#include <vector>
#include <algorithm>
#include <iostream>
#include "TString.h"
#include "TFile.h"
#include "TTree.h"
#include "TChain.h"
#include "QAnalysisdbc.h"
#include "QEvent2.h"
#include "QRunManager2.h"

class QAnalysis{

protected:
   TChain *qch;
   TFile  *qfile;
   TTree  *qtr;
   QEvent *qev;
   QRunManager *qrunmanager;
public:
   QAnalysis(TChain *ch);
   QRunManager *GetQRunManager(){return qrunmanager;}
   QEvent      *GetEvent(int iev=-1);
   virtual int  BookHistos(const char *ofile)=0;
   virtual int  SelectGoodRun(int opt=0);//opt=0 all,opt=1 p4-time, opt=2 p6-extend
   virtual int  InitEvent(QEvent *ev0=0){return 0;}
   virtual int  SelectEvent(QEvent *ev0=0,int pos=0){return 0;}
   virtual int  ProcessEvents(Long64_t num=-1,unsigned int run=0,unsigned int event=0);
   virtual int  CloneTree();
   virtual int  FillTree();
   virtual int  WriteTree();
public:
   int qid;
};

///----
QAnalysis::QAnalysis(TChain *ch){
  qch=ch;
  qfile=0;
  qtr=0;
  qrunmanager=new QRunManager();
  qev=new QEvent();
  qev->SetBranch(qch,0);//Header 
  if(GetEvent(0)){//Init One Event
    qev->SetBranch(qch,1);//Other
    qrunmanager->qisreal=(qev->isreal)?1:0;//ISS or MC 
  }
  qid=1;
}

///--
QEvent *QAnalysis::GetEvent(int iev){
    if(iev>=0){
       if(iev>=qch->GetEntries())return 0;
       qch->GetEntry(iev);
    }
    return qev;
}

///--
int  QAnalysis::SelectGoodRun(int opt){
     QEvent *ev=GetEvent(0);
     if(ev&&ev->isreal){//ISS BadRun
       qrunmanager->AddBadRun(1306219312,1306219312);
       qrunmanager->AddBadRun(1306219522,1306219522);
       qrunmanager->AddBadRun(1306233745,1306233745);
       qrunmanager->AddBadRun(1307125541,1307218054);//BADRUN
       qrunmanager->AddBadRun(1321198167,1321198167);
       qrunmanager->AddBadRun(1411995797,1417184590);//TTCS Off
       qrunmanager->AddBadRun(1434801178,1434841341);//Bad TOF-SDR
       int opt1=(opt%10);//<
       int opt2=((opt/10)%10);//>
//			     2013-11-26(2.5years),2014-09-29(TTCSon),2016-05-26(5years),2017-05-12(6years),2018-05-28(7years)
       unsigned int rune[5]={1385483969,          1411991495,        1464298202,        1494599304,        1527490046};
       if(opt1>=1&&opt1<=5){qrunmanager->AddBadRun(rune[opt1-1]+1,2000000000);}//(1,rune[opt1-1]], Run<=rune[opt1-1]
       if(opt2>=1&&opt2<=5){qrunmanager->AddBadRun(1,rune[opt2-1]);}//  (rune[opt2-1],2000000000), Run>rune[opt2-1]
       if(opt2==9){//after new DAQ
          qrunmanager->AddBadRun(1,1456503197-1);//[1456503197,2000000000), Run>=1456503197
       }
    }
    return 0;
}

///----
int QAnalysis::ProcessEvents(Long64_t num,unsigned int run,unsigned int event){
    Long64_t numr=qch->GetEntries();//raw tree entries
    Long64_t num2=((num>=0)&&(num<=numr))?num:numr;
    cout<<"num="<<num<<" numr="<<numr<<" num2="<<num2<<endl;
    int nstat=0;//error bit
    for(Long64_t ii=0;ii<num2;ii++){
       if(ii%100000==0) printf("Processed %7lld out of %7lld\n",ii,num2);
       QEvent *ev=GetEvent(ii);
       Long64_t numt=qch->GetEntries();//tree entries
       if((numt!=numr)||(ii>=numt)){nstat|=(1<<0);}//raw files entries changed
       if(ev==0)continue;
       TTree *evtree=qch->GetTree();
       if(evtree->GetReadEvent()==0){
         string nf=evtree->GetCurrentFile()->GetName();
//         cout<<"ii="<<ii<<" nf="<<nf<<" run="<<ev->run<<endl;
         qrunmanager->ProcessFiles(nf,ev->run);
       }
       if(run!=0){
         if(ev->run!=run)continue;
         if(event!=0){if(ev->event!=event)continue;}
       }
       if(!qrunmanager->IsSelRun(ev->run))continue;//Run must-be selected
       if(qrunmanager->IsBadRun(ev->run,0,1))continue;
       qrunmanager->ProcessRun(ev->run);
       if(num==-10)continue;//fast/just record run
       int pos=1;//Middel
       if(ii==num2-1)pos=0;//End Fill MC
       InitEvent(ev);
       int rt=SelectEvent(ev,pos);
       if(rt==-1000){nstat|=(1<<7);break;}//receive stop signal, error receive,shell only support 0-255
     }
     return nstat;  
}

///----
int QAnalysis::CloneTree(){
   if(qch){qtr=(TTree *)qch->CloneTree(0);return 0;}
   else   {return -2;}
}

///----
int QAnalysis::FillTree(){
   if(qtr){return qtr->Fill();} 
   else   {return -2;}
}

///----
int QAnalysis::WriteTree(){
   if(qtr){return qtr->Write();}
   else   {return -2;}
}
#endif
