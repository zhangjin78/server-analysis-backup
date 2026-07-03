///--Author Q.Yan qyan@cern.ch
///--Tool For Manage Cutoff
#ifndef _QCutoffManager_
#define _QCutoffManager_
#include <fstream>
#include <map>
#include <vector>
#include <algorithm>
#include <iostream>
#include "TString.h"
#include "TH1.h"

class QHistoCutoff{
///---Private
private:
  double qcutoff;
  double qbincutoff;
  TH1 *qh;
public:
  QHistoCutoff(TH1 *h);
  double GetBinCutoff(double cutoff,double margin=1.2);
  TH1 *GetHisto(){return qh;}
};

QHistoCutoff::QHistoCutoff(TH1 *h){
  qh=h;
  qcutoff=qbincutoff=0;
}

double QHistoCutoff::GetBinCutoff(double cutoff,double margin){
  if(cutoff*margin==qcutoff){
    return qbincutoff;
  }
  qcutoff=cutoff*margin;
  qbincutoff=qh->GetBinLowEdge(qh->FindBin(qcutoff)+1);//LowEdge;
  return qbincutoff;
}


class QCutoffManager{
///---Private
 private:
    map<TH1*, QHistoCutoff*> vhcf;
    static QCutoffManager *Head;
///--Public
 public:
    QCutoffManager();
    static QCutoffManager *GetHead();
    double GetBinCutoff(TH1 *h,double cutoff,double margin);
};

///----
QCutoffManager::QCutoffManager(){
  vhcf.clear();
}

///----
QCutoffManager *QCutoffManager::Head=0;

///----
QCutoffManager *QCutoffManager::GetHead(){
  if     (Head==0)Head=new QCutoffManager();
  return Head;
}

///----
double QCutoffManager::GetBinCutoff(TH1 *h,double cutoff,double margin){
  if(h==0)return 0;
  map<TH1 *,QHistoCutoff *>::iterator iter=vhcf.find(h);
  if(iter==vhcf.end()){vhcf[h]=new QHistoCutoff(h);}//New List
  return vhcf[h]->GetBinCutoff(cutoff,margin);
}

#endif
