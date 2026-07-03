///--Author Q.Yan qyan@cern.ch
///--Tool For Analysis Events
#ifndef _QTimeTool_
#define _QTimeTool_
#include <fstream>
#include <map>
#include <vector>
#include <algorithm>
#include <iostream>
#include "TString.h"
#include "TSystem.h"
#include "TFile.h"
#include "TRegexp.h"
#include "TH1.h"
#include "TGraphErrors.h"

using namespace std;

class QTimeTool{
public:
   QTimeTool();
   int Clear();
   int LoadFile(const char *fname,int opt=0);
   TH1 *CopyTH1(const char *name,unsigned int bt, unsigned int et,const char *subdir=0);
   std::map <pair<unsigned int,unsigned int>, TH1 *> CopyTH1Arr(const char *name,unsigned int bt,unsigned int et,unsigned int dt,int stot=0,const char *subdir=0,int rebin=1);
   std::map <pair<unsigned int,unsigned int>, TH1 *> DivideTH1Arr(std::map <pair<unsigned int,unsigned int>, TH1 *> *arr);
   int WriteTxt(const char *otxt,std::map< pair<unsigned int,unsigned int>,std::vector<double> > &fpar);
   int WriteTxt(const char *otxt,std::map< pair<float,float>,std::vector<double> > &fpar);
   int WriteRoot(const char *odir,std::map <pair<unsigned int,unsigned int>, TH1 *>&fhist,const char *opt="RECREATE");
   int WriteRoot(const char *odir,std::map <pair<unsigned int,unsigned int>, TGraphErrors *>&fhist,const char *opt="UPDATE");
   static int ReadTxt(const char *itxt,std::map< pair<unsigned int,unsigned int>,std::vector<double> > &fpar,int npar);
   static int ReadTxt(const char *itxt,std::map< pair<float,float>,std::vector<double> > &fpar,int npar);
public:
   std::map <unsigned int, TFile *> flist;
   std::map <unsigned int, string >flistname;
   int flistopt;
   int nlist;
   std::map< pair<unsigned int,unsigned int>,std::vector<double> > qpar;
};


///----
int QTimeTool::Clear(){
   flist.clear();
   flistname.clear();
   flistopt=0;
   nlist=0;
   qpar.clear();
   return 0;
}

///----
QTimeTool::QTimeTool(){
   Clear();
}

///----
int QTimeTool::LoadFile(const char *fname,int opt){
//----Clear
    Clear();
//----directory
    TString basename(fname);
    int slashpos=basename.Last('/');
    TString directory=basename(0,slashpos);
    std::vector<const char *>filearr;
    cout<<"directory="<<directory<<" first="<<directory.First("/eos")<<" kNOPOS="<<kNPOS<<endl;
    string dname=directory.Data();
//------EOS
    if(opt>=2){
      string lsopt="cat ";
      lsopt=lsopt+fname;
      TString froot=gSystem->GetFromPipe(lsopt.c_str());
      TObjArray *rootarr=froot.Tokenize("\n");
      for(int i=0;i<rootarr->GetEntries();i++){
        cout<<"i="<<i<<" f="<<rootarr->At(i)->GetName()<<endl;
        filearr.push_back(rootarr->At(i)->GetName());
      }
    }
//    if(directory.First("/eos")!=kNPOS){
    else if(dname.find("/eos")!=std::string::npos){
      string lsopt="eos ls ";
      lsopt=lsopt+dname+" | grep .root";
      TString froot=gSystem->GetFromPipe(lsopt.c_str());
      TObjArray *rootarr=froot.Tokenize("\n");
      for(int i=0;i<rootarr->GetEntries();i++){
        filearr.push_back(rootarr->At(i)->GetName());
      }
    }
//-----Normal
    else {
     basename.Remove(0,slashpos+1);
     const char *epath=gSystem->ExpandPathName(directory.Data());
     void *dir=gSystem->OpenDirectory(epath);
     TRegexp re(basename,kTRUE);
     const char *file=0;
     while(file=gSystem->GetDirEntry(dir)){
       TString s=file;
       if((basename!=file)&&s.Index(re)==kNPOS)continue;
       filearr.push_back(file);
     }
   }
//---serach for file
    string fnamen;
    for(int i=0;i<filearr.size();i++){
      const char *file=filearr[i];
      fnamen=file;
      if(opt>=2){
        TString basename(fnamen);
        int slashpos=basename.Last('/');
        basename.Remove(0,slashpos+1);
        fnamen=basename.Data();
      }
      unsigned int beg=0;
      unsigned int end=fnamen.find('_');
      string date=fnamen.substr(beg,end-beg);
      int numdate=atoi(date.c_str());
      cout<<"fname="<<fnamen<<" date="<<numdate<<endl;;
//----File
      TString s=directory+"/"+file;
      if(opt>=2)s=file;
      cout<<s.Data()<<endl;
      flistname[numdate]=s.Data();
      if(opt>0)flist[numdate]=0;
      else     flist[numdate]=TFile::Open(s.Data());
      flistopt=opt;
      nlist++;
    }
   return 0;
}

///---
TH1* QTimeTool::CopyTH1(const char *name,unsigned int bt, unsigned int et,const char *subdir){
  
  TH1 *h=0;
//  std::map <unsigned int, TFile *> ::iterator iter;
//  for(iter=flist.begin();iter!=flist.end();iter++){
//    if((*iter).first<bt)continue;//Time
  std::map <unsigned int, TFile *> ::iterator iter=(bt==0)?flist.begin():flist.lower_bound(bt);
  cout<<"bt="<<bt<<" et="<<et<<" iter.first="<<(*iter).first<<endl;
  for(;iter!=flist.end();iter++){
    if(et!=0&&(*iter).first>et)break;
    if(flistopt>0&&(*iter).second==0){(*iter).second=TFile::Open(flistname[(*iter).first].c_str());}
    if((*iter).second==0)continue;//TFile 
    (*iter).second->cd();
    string names="";
    if(subdir){names+=subdir;names+="/";}
    names+=name;
    TH1 *hn=(TH1 *)(*iter).second->Get(names.c_str());
//    cout<<"hn="<<hn<<" names="<<names.c_str()<<" flistopt="<<flistopt<<" filename="<<(flistname[(*iter).first].c_str())<<endl;
    if(h==0){
      if(hn){h=(TH1 *)hn->Clone();h->SetDirectory(0);}
    }
    else     {
      if(hn)h->Add(hn);
    }
    if(flistopt>0&&(*iter).second){(*iter).second->Close(); (*iter).second=0;}
  }
  return h;
}

///---
std::map <pair<unsigned int,unsigned int>, TH1 *> QTimeTool::CopyTH1Arr(const char *name,unsigned int bt,unsigned int et,unsigned int dt,int stot,const char *subdir,int rebin){

  std::map <pair<unsigned int,unsigned int>, TH1 *> harr;
  for(unsigned int nt=bt;nt<et;nt+=dt){
     unsigned int t0=nt;
     unsigned int t1=nt+dt-1;
     if(t1>et)t1=et;
     cout<<"t0="<<t0<<" t1="<<t1<<endl;
     TH1 *hn=CopyTH1(name,t0,t1,subdir);
     if(hn==0)continue;
     pair<unsigned int,unsigned int>tp(t0,t1);
     if(hn&&rebin>=2)hn->Rebin(rebin);
     harr[tp]=hn; 
  }
  if(stot>=1){
    pair<unsigned int,unsigned int>tp(et+1,et+1); 
    TH1 *hn=CopyTH1(name,bt,et,subdir);
    if(hn&&rebin>=2)hn->Rebin(rebin);
    if(hn)harr[tp]=hn;
  }
  return harr;
}

///---
std::map <pair<unsigned int,unsigned int>, TH1 *> QTimeTool::DivideTH1Arr(std::map <pair<unsigned int,unsigned int>, TH1 *> *arr){

  std::map <pair<unsigned int,unsigned int>, TH1 *> harr;
  std::map <pair<unsigned int,unsigned int>, TH1 *>::iterator iter; 
  std::map <pair<unsigned int,unsigned int>, TH1 *>::iterator iters;
  for(iter=arr[0].begin();iter!=arr[0].end();iter++){
    iters=arr[1].find((*iter).first);
    if(iters==arr[1].end())continue;
//---
    TH1 *h1=(*iters).second;//arr[1]/arr[0]
    TH1 *h0=(*iter).second;
    if(h0==0||h1==0)continue;
    TH1 *hn=(TH1 *)h1->Clone();
    hn->Sumw2();
    h0->Sumw2();
    hn->Divide(hn,h0,1,1,"b");
//---Check 100%
   for(int ibx=1;ibx<=hn->GetNbinsX();ibx++){
     if(hn->GetBinContent(ibx)==1){//Efficiency 100%
       double np=h1->GetBinContent(ibx);//Pass
       if(np==0)continue;
       double fp=np/(np+1);
       double ep=sqrt(fp*(1-fp)/np);
       hn->SetBinError(ibx,ep);
     }
    } 
//--- 
    harr[(*iter).first]=hn;
  }
  return harr;
}

///---
int QTimeTool::WriteTxt(const char *otxt,std::map< pair<unsigned int,unsigned int>,std::vector<double> > &fpar){
  ofstream tcfile(otxt,ios::out|ios::trunc);
  if(!tcfile){
      cout<<"<---- error opening file for output"<<otxt<<'\n';
      exit(8);
  }
  std::map <pair<unsigned int,unsigned int>, std::vector<double> >::iterator iter;
  for(iter=fpar.begin();iter!=fpar.end();iter++){
    tcfile<<(*iter).first.first<<" "<<(*iter).first.second<<" ";
    tcfile.setf(ios::fixed);
    tcfile.width(8);
    tcfile.precision(6);// precision for s
    std::vector<double> vpar=(*iter).second;
    for(int ipar=0;ipar<vpar.size();ipar++){
      tcfile <<vpar[ipar]<<" ";
    }
    tcfile<<endl;
  }
  tcfile.close();
  return 0; 
}

///---
int QTimeTool::WriteTxt(const char *otxt,std::map< pair<float,float>,std::vector<double> > &fpar){
  ofstream tcfile(otxt,ios::out|ios::trunc);
  if(!tcfile){
      cout<<"<---- error opening file for output"<<otxt<<'\n';
      exit(8);
  }
  std::map <pair<float,float>, std::vector<double> >::iterator iter;
  for(iter=fpar.begin();iter!=fpar.end();iter++){
    tcfile<<(*iter).first.first<<" "<<(*iter).first.second<<" ";
    tcfile.setf(ios::fixed);
    tcfile.width(8);
    tcfile.precision(6);// precision for s
    std::vector<double> vpar=(*iter).second;
    for(int ipar=0;ipar<vpar.size();ipar++){
      tcfile <<vpar[ipar]<<" ";
    }
    tcfile<<endl;
  }
  tcfile.close();
  return 0;
}


///---
int QTimeTool::ReadTxt(const char *itxt,std::map< pair<unsigned int,unsigned int>,std::vector<double> > &fpar,int npar){
  ifstream tcfile(itxt,ios::in);
  if(!tcfile){
      cout<<"<---- error opening file for input"<<itxt<<'\n';
      exit(8);
  }
  while (tcfile.good()){
    unsigned int t0,t1;
    tcfile>>t0>>t1;
    pair<unsigned int,unsigned int>tp(t0,t1);
    if(!tcfile.good())break;
    cout<<t0<<" "<<t1<<" ";
    double vpar;
    for(int ipar=0;ipar<npar;ipar++){
      tcfile>>vpar;
      if(!tcfile.good())break;
      fpar[tp].push_back(vpar);
      cout<<vpar<<" ";
    }
    cout<<endl; 
  }
  tcfile.close();
  return 0;
}

///---
int QTimeTool::ReadTxt(const char *itxt,std::map< pair<float,float>,std::vector<double> > &fpar,int npar){
  ifstream tcfile(itxt,ios::in);
  if(!tcfile){
      cout<<"<---- error opening file for input"<<itxt<<'\n';
      exit(8);
  }
  while (tcfile.good()){
    float t0,t1;
    tcfile>>t0>>t1;
    pair<float,float>tp(t0,t1);
    if(!tcfile.good())break;
    cout<<t0<<" "<<t1<<" ";
    double vpar;
    for(int ipar=0;ipar<npar;ipar++){
      tcfile>>vpar;
      if(!tcfile.good())break;
      fpar[tp].push_back(vpar);
      cout<<vpar<<" ";
    }
    cout<<endl;
  }
  tcfile.close();
  return 0;
}

///---
int QTimeTool::WriteRoot(const char *odir,std::map <pair<unsigned int,unsigned int>, TH1 *>& fhist,const char *opt){
  std::map <pair<unsigned int,unsigned int>, TH1 *>::iterator iter;
  for(iter=fhist.begin();iter!=fhist.end();iter++){
    TFile *fn=new TFile(Form("%s/%u_%u.root",odir,(*iter).first.first,(*iter).first.second),opt);
    fn->cd();
    ((*iter).second)->Write();
    delete fn;
  }
  return 0;
}

int QTimeTool::WriteRoot(const char *odir,std::map <pair<unsigned int,unsigned int>, TGraphErrors *>& fhist,const char *opt){
  std::map <pair<unsigned int,unsigned int>, TGraphErrors *>::iterator iter;
  for(iter=fhist.begin();iter!=fhist.end();iter++){
    TFile *fn=new TFile(Form("%s/%u_%u.root",odir,(*iter).first.first,(*iter).first.second),opt);
    fn->cd();
    ((*iter).second)->Write();
    delete fn;
  }
  return 0;
}

#endif
