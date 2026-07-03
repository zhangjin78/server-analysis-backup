///--Author Q.Yan qyan@cern.ch
///--Tool For Manage RTI
#ifndef _QRTIManager_
#define _QRTIManager_
#include <fstream>
#include <map>
#include <vector>
#include <algorithm>
#include <iostream>
#include "TString.h"
#include "root_RVSP.h"
//#include "QEvent2.h"

class QRTIManager{
///---Private
 public:
    int  InitRTI();
    int  qvers;
 private:
    static QRTIManager *Head;
///--Public
 public:
    QRTIManager(int version=-1);
    static QRTIManager *GetHead(int version=-1);
    static bool Select_RTInL1L9(unsigned int utime);
    static bool Select_RTIdL1L9(unsigned int utime);
    static bool Select_RTI(AMSSetupR::RTI &a,int opt=2);
    static bool Select_RTI(unsigned int utime,int opt=2);
//    int  Fill_RTI(QEvent *ev, unsigned int utime=0);
};

///----
int  QRTIManager::InitRTI(){
//    if(qvers<0)AMSSetupR::RTI::UseLatest(7);
    if(qvers<0)AMSSetupR::RTI::UseLatest();
    else       AMSSetupR::RTI::Version=qvers;
    return AMSSetupR::RTI::Version; 
}
 
///----
QRTIManager::QRTIManager(int version){
   qvers=version;
   InitRTI();
}

///----
QRTIManager *QRTIManager::Head=0;

///----
QRTIManager *QRTIManager::GetHead(int version){
  if     (Head==0)Head=new QRTIManager(version);
  else if(version!=Head->qvers){
     delete Head;
     Head=new QRTIManager(version);
  }
  return Head;
}

///----
bool QRTIManager::Select_RTInL1L9(unsigned int utime){
  AMSPoint nexl[2];
  AMSEventR::GetRTInL1L9(nexl,utime,90);
  bool cutntrk=(nexl[0][0]>700&&nexl[1][0]>500);
  return cutntrk;
}

///----
bool QRTIManager::Select_RTIdL1L9(unsigned int utime){
  AMSPoint pn1, pn9, pd1, pd9;
  AMSEventR::GetRTIdL1L9(0, pn1, pd1, utime, 60);
  AMSEventR::GetRTIdL1L9(1, pn9, pd9, utime, 60);
  if (pd1.y() > 35 || pd9.y() > 45)return false; // Skip current second 
  return  true;
}

///----
bool QRTIManager::Select_RTI(AMSSetupR::RTI &a,int opt){
  bool isphorun=(a.run>=1620025528&&a.run<1635856717);
  bool cut[10]={0};
  cut[0]=(a.ntrig/a.nev>0.98);
  cut[1]=(a.npart/a.ntrig>(isphorun?0.02:0.07/1600*a.ntrig)&&a.npart/a.ntrig<0.25);
  cut[2]=(a.lf>(isphorun?0.35:0.5));
  cut[3]=(a.nerr>=0&&a.nerr/a.nev<0.1);
  cut[4]=(a.npart>0&&a.nev<1800);
  cut[5]=(a.zenith<40);
  cut[6]=((a.good&0x3F)==0);
  cut[7]=(a.IsInSAA()==0);
  cut[9]=1;
  if(AMSSetupR::RTI::Version>=800&&opt!=1)cut[9]=Select_RTInL1L9(a.utime);
  bool tcut=(cut[0]&&cut[1]&&cut[2]&&cut[3]&&cut[4]&&cut[5]&&cut[6]&&cut[7]&&cut[9]);
  return tcut;
}

///----
bool QRTIManager::Select_RTI(unsigned int utime,int opt){
  AMSSetupR::RTI a;
  if(AMSEventR::GetRTI(a,utime)!=0)return false;//error false
  return Select_RTI(a,opt); 
}

///----
/*int QRTIManager::Fill_RTI(QEvent *ev,unsigned int utime){
   if(ev==0)return -2;
   ev->irti=-1;
   if(AMSEventR::GetRTI(a,utime)!=0)ev->irti=0; 
   if(ev->irti>=0){
      ev->zenith=a.zenith; 
      ev->thetas=a.theta;
      ev->phis=a.phi;
      ev->rads=a.r;
      ev->glat=a.glat;
      ev->glong=a.glong;
      ev->issaa=a.IsInSAA();
      for(int ifv=0;ifv<4;ifv++){
        for(int ipn=0;ipn<2;ipn++){ev->mcutoff[ifv][ipn]=a.cf[ifv][ipn];ev->mcutoffi[ifv][ipn]=a.cfi[ifv][ipn];}
      }
      ev->rtilf=a.lf;
      ev->rtinev=a.nev;
      ev->rtinerr=a.nerr;
      ev->rtintrig=a.ntrig;
      ev->rtinpar=a.npart;
      ev->rtigood=a.good;
   }
   return ev->irti; 
}*/
#endif
