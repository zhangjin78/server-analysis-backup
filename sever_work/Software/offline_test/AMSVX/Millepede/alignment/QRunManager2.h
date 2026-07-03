///--Author Q.Yan qyan@cern.ch
///--Tool For Manage Run
#ifndef _QRunManager_
#define _QRunManager_
#include <fstream>
#include <map>
#include <vector>
#include <algorithm>
#include <iostream>
#include "TString.h"
#include "TSystem.h"
#include "TObjArray.h"

class QRunManager{
  public:
    QRunManager();
    QRunManager(const char *fdir,const char *ntxt=0);
    int  SetFDir(const char *fdir,const char *ntxt=0);
//---Bad Run
    int  AddBadRun(unsigned int badrunb=0,unsigned int badrune=0);
    int  AddBadRun(vector<unsigned int > &badrunlist);
    bool IsBadRun(unsigned int run,int optforce=0,int optout=0);//optforce(force reload)
//---Sel Run
    int  AddSelRun(vector<unsigned int > &selrunlist,int force=0);
    bool IsSelRun(unsigned int run);
//---Process Run
    int  ProcessFiles(string fn,unsigned int run);
    int  ProcessRun(unsigned int run); 
    bool IsProcessRun(unsigned int run);
//---
    int  LoadRunList(const char *file,vector<unsigned int > &runlist,int optclean=1);
    int  LoadRunList(const char *rootdir,map <unsigned int,vector<string> > &runlist,int endadd=0,int optclean=0);
    int  LoadRunList(unsigned int time[2],vector<unsigned int > &runlist);
    int  FindRun(unsigned int run,int optinsert=0);//opt=0 just found, opt=1 if not exist insert 
    int  InsertRunList(vector<unsigned int > &runlist);//0: to file, 1: to directory
    void GetNewRunList(vector<unsigned int > &runlist,vector<unsigned int > &runlistn);
//---
    int  InsertDSTRun(const char *rootdir,int endadd=0,int optclean=0);//0: to file, 1: to directory
    int  FindDSTRun(unsigned int run,vector<string> &runlist,int offset=0);
    int  FindDSTRun(unsigned int run[2],vector<string> &runlist,int offset=0);
    static bool SortDSTRun(string run1,string run2);
//---
  public:
    vector<unsigned int>qprocessrunlist;//Process Run
    vector<pair < unsigned int, unsigned int> >qbadrunbe;//Bad Run: Begin+End
    vector<unsigned int>qbadrunlist;//Bad Run List:
    vector<unsigned int>qselrunlist;//Select Run List: 
    map <unsigned int,vector<string> > qdstrunlist; //Search DST Run
    map <string,unsigned int > qprocessfile;
//---IsReal
    int qisreal;
//--
  private:
    ofstream tcfile;
    int pry;
    int prm;
    int prd;
    vector<unsigned int>qrunlist;
    const char *qfdir;
    const char *qntxt;
//---Process Run 
    unsigned int qprocessrunn;//Now Process Run
    pair < unsigned int, bool > qbadrunn;//Now IsBadRun
};

///----
QRunManager::QRunManager(){
  qfdir=0;
  qntxt=0;
  pry=prm=prd=0;
  qrunlist.clear();
  qbadrunbe.clear();
  qbadrunlist.clear();
  qbadrunn.first=0; qbadrunn.second=1;
  qselrunlist.clear();
  qprocessrunn=0;
  qprocessrunlist.clear();
  qdstrunlist.clear();
  qisreal=-1;
}

///----
QRunManager::QRunManager(const char *fdir,const char *ntxt){
  qfdir=fdir;
  qntxt=ntxt;
  pry=prm=prd=0;
  qrunlist.clear();
  qbadrunbe.clear();
  qbadrunlist.clear();
  qbadrunn.first=0; qbadrunn.second=1;
  qselrunlist.clear();
  qprocessrunn=0;
  qprocessrunlist.clear();  
  qdstrunlist.clear();
  qisreal=-1;
}

int QRunManager::SetFDir(const char *fdir,const char *ntxt){
    qfdir=fdir;
    qntxt=ntxt;
    return 0;
}

///----
int QRunManager::AddBadRun(unsigned int badrunb,unsigned int badrune){
  int isnew=1;
  for(unsigned int i=0;i<qbadrunbe.size();i++){
    if(qbadrunbe[i].first==badrunb&&qbadrunbe[i].second==badrune){isnew=0;break;}//New List
  }
  if(isnew){qbadrunbe.push_back(make_pair(badrunb,badrune));}
  return isnew;
}

///----
int QRunManager::AddBadRun(vector<unsigned int > &badrunlist){
  int nadd=0;
  for(unsigned int irun=0;irun<badrunlist.size();irun++){
    int run=badrunlist[irun];
    vector<unsigned int>::iterator iter=std::find(qbadrunlist.begin(),qbadrunlist.end(),run);
    if(iter==qbadrunlist.end()){qbadrunlist.push_back(run);nadd++;}//New List
  }
  return nadd;
}

///---
bool QRunManager::IsBadRun(unsigned int run,int optforce,int optout){
//--previous stat
  if(optforce==0&&run==qbadrunn.first)return qbadrunn.second;
  qbadrunn.first=run;
//---
  qbadrunn.second=0;//goodrun by default 
  for(unsigned int i=0;i<qbadrunbe.size();i++){
    if(run>=qbadrunbe[i].first&&run<=qbadrunbe[i].second){qbadrunn.second=1;break;}//badrun find
  }
  if(!qbadrunn.second){//goodrun->try to search badrun
    vector<unsigned int>::iterator iter=std::find(qbadrunlist.begin(),qbadrunlist.end(),run);
    if(iter!=qbadrunlist.end()){qbadrunn.second=1;}//badrun find
  }
  if(qbadrunn.second){if(optout)cout<<"BadRun="<<run<<endl;}
  return qbadrunn.second;
}

///----
int QRunManager::AddSelRun(vector<unsigned int > &selrunlist, int force){
  int nadd=0;
  for(unsigned int irun=0;irun<selrunlist.size();irun++){
    int run=selrunlist[irun];
    vector<unsigned int>::iterator iter=std::find(qselrunlist.begin(),qselrunlist.end(),run);
    if(iter==qselrunlist.end()){qselrunlist.push_back(run);nadd++;}//New List
  }
  if(force>=1&&qselrunlist.size()==0)qselrunlist.push_back(0);//put selected run=0 as dump
  return nadd;
}

///---
bool QRunManager::IsSelRun(unsigned int run){
  bool issel=1;
  if(qselrunlist.size()>=1){
    vector<unsigned int>::iterator iter=std::find(qselrunlist.begin(),qselrunlist.end(),run);
    if(iter==qselrunlist.end())issel=0;//notsel find
//    if(issel)cout<<"list="<<qselrunlist[0]<<" run="<<run<<endl;
  }
  return issel; 
}

///---
int  QRunManager::ProcessFiles(string fn,unsigned int run){
   qprocessfile[fn]=run;
   return 0;
}

///---
int  QRunManager::ProcessRun(unsigned int run){
   if(run==qprocessrunn)return 0;
   qprocessrunn=run;
   vector<unsigned int>::iterator iter=std::find(qprocessrunlist.begin(),qprocessrunlist.end(),run);
   if(iter==qprocessrunlist.end()){qprocessrunlist.push_back(run);return 1;}
   else                           {cout<<"Exist Run="<<run<<endl;return 2;}
}

///---
bool  QRunManager::IsProcessRun(unsigned int run){
   bool ispro=1;
   vector<unsigned int>::iterator iter=std::find(qprocessrunlist.begin(),qprocessrunlist.end(),run);
   if(iter==qprocessrunlist.end())ispro=0;
   return ispro; 
}

///----
int QRunManager::LoadRunList(const char *file,vector<unsigned int > &runlist,int optclean){
  if(optclean)runlist.clear();
  ifstream tsfile(file,ios::in);
  if(!tsfile)return 0;
  unsigned int run;
//---
  while (tsfile.good()){
     tsfile>>run;
     if(!tsfile.good())continue;
     vector<unsigned int>::iterator iter=std::find(runlist.begin(),runlist.end(),run);
     if(iter==runlist.end())runlist.push_back(run);//Not Found
   }
  tsfile.close();
  return runlist.size();
}

///----
int QRunManager::LoadRunList(const char *rootdir,map <unsigned int,vector<string> > &runlist,int endadd,int optclean){
///--
  if(optclean)runlist.clear();
//---Find
  string indir=rootdir;
  size_t fonde=indir.find("/eos");
  size_t fondc=indir.find("/castor");
  TString lsopt;
  if     (fonde!=string::npos)lsopt="/afs/cern.ch/project/eos/installation/0.3.15/bin/eos.select ls ";
  else if(fondc!=string::npos)lsopt="nsls ls ";
  else                        lsopt="ls ";
  TString froot=gSystem->GetFromPipe(lsopt+indir.c_str()+Form(" | grep .root"));
  TObjArray *rootarr = froot.Tokenize("\n");
//----
  for (int i=0;i<rootarr->GetEntries();i++){
     string fname=rootarr->At(i)->GetName();
//     unsigned bp=fname.rfind("/");
     std::size_t bp=0;
     string cfname=fname.substr(bp,bp+10);
     unsigned int run=atoi(cfname.c_str());
     fname=indir+"/"+fname;
     if(endadd>0){
       if     (fonde!=string::npos)fname="root://eosams/"+fname;
       else if(fondc!=string::npos)fname="root://castorpublic.cern.ch//"+fname; 
     }
     vector<string>::iterator iter=std::find(runlist[run].begin(),runlist[run].end(),fname);
     if(iter==runlist[run].end()){
       runlist[run].push_back(fname); 
     }
  }
  return runlist.size();
}


///---
int QRunManager::LoadRunList(unsigned int time[2],vector<unsigned int > &runlist){
  runlist.clear();
  vector<unsigned int > trunlist;
  int tpry=0,tprm=0,tprd=0;
  for(int tn=time[0];tn<time[1];tn++){
     time_t tt=tn;
     tm *tmp=gmtime(&tt);
     if((tmp->tm_year!=tpry||tmp->tm_mon!=tprm||tmp->tm_mday!=tprd)){
        tpry=tmp->tm_year; tprm=tmp->tm_mon; tprd=tmp->tm_mday;
        char nfile[256],nfile1[1000];
        strftime(nfile,80,"%Y_%j",tmp);
        if(qntxt!=0)sprintf(nfile1,"%s/%sRun_%s-24H.cvs",qfdir,qntxt,nfile);
        else        sprintf(nfile1,"%s/Run_%s-24H.cvs",qfdir,nfile);
        cout<<"LoadN_name="<<nfile1<<endl;
        LoadRunList(nfile1,trunlist,0);
     }
  }
  for(vector<unsigned int > ::iterator it=trunlist.begin();it!=trunlist.end();it++){
      if((*it)>=time[0]&&(*it)<time[1])runlist.push_back(*it);
  }
  return runlist.size(); 
}

///---
int QRunManager::FindRun(unsigned int run,int optinsert){
   time_t tt=run;
   tm *tmp=gmtime(&tt);
   cout<<"grun="<<run<<" optinsert="<<optinsert<<endl;
   if((tmp->tm_year!=pry||tmp->tm_mon!=prm||tmp->tm_mday!=prd)){
     pry=tmp->tm_year; prm=tmp->tm_mon; prd=tmp->tm_mday;
     char nfile[256],nfile1[1000];
     strftime(nfile,80,"%Y_%j",tmp);
     if(qntxt!=0)sprintf(nfile1,"%s/%sRun_%s-24H.cvs",qfdir,qntxt,nfile);
     else        sprintf(nfile1,"%s/Run_%s-24H.cvs",qfdir,nfile);
     cout<<"Load_name="<<nfile1<<" Run="<<run<<" optinsert="<<optinsert<<" size0="<<qrunlist.size()<<endl;
     LoadRunList(nfile1,qrunlist,1);
     tcfile.close();
     tcfile.open(nfile1,ios::out|ios::app);
     cout<<"size1="<<qrunlist.size()<<endl;
   }
   cout<<"qrunlistsize="<<qrunlist.size()<<" "<<"run="<<run<<" optinsert="<<optinsert<<endl;
   vector<unsigned int>::iterator iter=std::find(qrunlist.begin(),qrunlist.end(),run);
   if(iter==qrunlist.end()){
      if(optinsert==1){
         qrunlist.push_back(run);//Update Runlist
         cout<<"run="<<run<<endl;
         tcfile<<run<<endl;//Update RunFile
      }
      return 0;
   }
   else  return run;
}

///---
int QRunManager::InsertRunList(vector<unsigned int >&runlist){
  for(vector<unsigned int > ::iterator it=runlist.begin();it!=runlist.end();it++){
     FindRun(*it,1);
  }
  return 0; 
}

///--
void QRunManager::GetNewRunList(vector<unsigned int >& runlist,vector<unsigned int >& runlistn){
  cout<<"size0="<<runlist.size()<<","<<runlistn.size()<<endl;
  for(vector<unsigned int > ::iterator it=runlist.begin();it!=runlist.end();it++){
     cout<<"prun0="<<(*it)<<endl;
     if(FindRun(*it,0)==0){runlistn.push_back(*it);cout<<"new"<<endl;}//Not Exist
     else                 {cout<<"old"<<endl;}
  }
  cout<<"sizef="<<runlist.size()<<","<<runlistn.size()<<endl;
}

///---
int QRunManager::InsertDSTRun(const char *rootdir,int endadd,int optclean){
  map <unsigned int,vector<string> > runlist;
  LoadRunList(rootdir,qdstrunlist,endadd,optclean);
  return 0;
}

///--
bool QRunManager::SortDSTRun(string run1,string run2){
   string run[2];
   for(int irun=0;irun<2;irun++){
    string runn=(irun==0)?run1:run2;
    std::size_t bp=runn.rfind("/");
    run[irun]=runn.substr(bp+1);
   }
   return run[0]<run[1];
}

///---
int QRunManager::FindDSTRun(unsigned int run,vector<string> &runlist,int offset){
  map <unsigned int,vector<string> >::iterator iter;
  for(iter=qdstrunlist.begin();iter!=qdstrunlist.end();iter++){
    if(run<(*iter).first)break; 
  }
  int nrun=0;
  while(iter!=qdstrunlist.begin()){
   iter--;
   for(unsigned int iroot=0;iroot<(*iter).second.size();iroot++){
     string rootn=(*iter).second.at(iroot);
     if(std::find(runlist.begin(),runlist.end(),rootn)==runlist.end()){
       runlist.push_back(rootn);
       nrun++; 
     }
   }
   if     (offset==0)break;
   else if((*iter).first<run-offset)break;
  }
  sort(runlist.begin(),runlist.end(),SortDSTRun);
  return nrun;
}

///---
int QRunManager::FindDSTRun(unsigned int run[2],vector<string> &runlist,int offset){
  int offsetn=run[1]-run[0]+offset;
  return FindDSTRun(run[1],runlist,offsetn);
}
#endif
