#include "TkFeet.h"
#include "TkDBc.h"
#include "point.h"
#include "TkCoo.h"


ClassImp(TkFeet);

int TkFeet::nfeet[17]={
  0, 0,0,0, 0,0,0,
  4, 0,4,5, 5,5,6, 6,6,7};

int TkFeet::off1[17]={
  0, 0,0,0, 0,0,0,
//7    9 10 11 12  13 14 15  14S
  2, 0,3, 2, 3, 3,  2, 3, 3,  2};

double TkFeet::d0[17]={
  0, 0,0,0, 0,0,0,
//7          9       10      11      12     13       14     15      14S
  7.151, 0, 11.291, 7.151, 11.291, 11.291, 7.151, 11.291, 11.291, 7.151};

double TkFeet::dl[17]={
  0, 0,0,0, 0,0,0,
//7          9       10      11      12     13       14     15      14S
  7.151, 0, 11.291, 7.151, 7.151, 11.291, 7.151,  7.151,   11.291, 7.151};

double TkFeet::dfix=12.42;

double TkFeet::x14s[7]={1.0506, 8.2016, 20.6216, 27.7726, 37.2276, 49.6476, 56.7986 };



double TkFeet::getX1(ltype tt){
  if(!db ||!lad) return -99999;
  double xina=db->_ssize_inactive[0];
  double xact=db->_ssize_active[0];

  double x1_to_edge=off1[tt]*xina+(off1[tt]-1+0.5)*0.004;
  double firststrip_off=(xina-xact)/2.;
  return (x1_to_edge- firststrip_off);
}

double TkFeet::getXloc(int tkid,int ifoot){
  SetTkid(tkid);
  if(ifoot>=nfeet[type]) return 9999;
  if(type==l14s){
    return x14s[ifoot];}
  else{
    if(ifoot==0){
      return getX1(type)-d0[type];
    }else if(ifoot==1){
      return getX1(type);
    }else if(ifoot==nfeet[type]-1){
      return getX1(type)+(nfeet[type]-3)*dfix+dl[type];      
    }else 
      return getX1(type)+(ifoot-1)*dfix;
  }
}


double TkFeet::getYloc(int tkid,int side){
  SetTkid(tkid);
  if(!db ||!lad) return -99999;
  double yina=db->_ssize_inactive[1];
  double yact=db->_ssize_active[1];
  double widthY=6;
  double y1_to_edge=(yina-widthY)/2.+0.25;
  double firststrip_off=(yina-yact)/2.;
  double Y1=(y1_to_edge-firststrip_off);
  double Y2=Y1+widthY-0.5;
  if(side==0) return Y1;
  else return Y2;
}

TkFeet::TkFeet(){
  db=TkDBc::GetHead();
  if(!db) return;
  _tkid=-999999;
  type=l7;
  //SetTkid(tkid);
}

void TkFeet::SetTkid(int tkid){
  //  if(db && lad && _tkid==tkid) return;
  lad=db->FindTkId(tkid);
  if(!lad) return;
  _tkid=tkid;
  int nsens=lad->GetNSensors();
  int lay= lad->GetLayer();
  type=(ltype)nsens;
  if(nsens==14&&
     (lay==2||lay==3||lay==6||lay==7))
    type=l14s;
  return;
}


AMSPoint TkFeet::getGlobalA(int tkid, int ifoot, int iside){

  double xloc=getXloc(tkid,ifoot);
  double yloc=getYloc(tkid,iside);
  return TkCoo::GetGlobalA(tkid,xloc,yloc);

}



AMSPoint TkFeet::getGlobalT(int tkid, int ifoot, int iside){

  double xloc=getXloc(tkid,ifoot);
  double yloc=getYloc(tkid,iside);
  return TkCoo::GetGlobalT(tkid,xloc,yloc);

}
