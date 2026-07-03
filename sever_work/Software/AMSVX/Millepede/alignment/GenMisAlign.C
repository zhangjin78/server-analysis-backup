//---author Q.Yan qyan@cern.ch used for generating steering file and misalignment
#include <TLine.h>
#include <TSystem.h>
#include <TStyle.h>
#include <iomanip>
#include  "TMath.h"
#include "TMinuit.h"
#include "TTree.h"
#include <sstream>
#include "TString.h"
#include <fstream>
#include <iostream>
#include "TFile.h"
#include "TMath.h"
#include "TTree.h"
#include "HistoMan.h"
#include "TLegend.h"
#include "TDatime.h"
#include "TProfile.h"
#include "TF1.h"
#include "TH1D.h"
#include "TLine.h"
#include "TCanvas.h"
#include "TGraphErrors.h"
#include "TRandom.h"
#include "root_RVSP.h"
#include "TMatrixD.h"
#include "TkGeomN.h"
#include "TkDBcN.h"
//#include "/afs/cern.ch/work/q/qyan/work/alignment/RigFit.C"
//#include "/afs/cern.ch/work/q/qyan/AMSVDEV/QTool/QPlot.h"

#define USEALLBEAM
//#define USEMC
#define USETESTBEAM
//#define FIXLADALPHA
#define FIXSENSORGAMMA
#define FIXSENSOR19
//#define FIXSENSOR19X
#define CONSSHEARING
#define USEXTLAYER
#define USEFINE
//-------
//#define FIXSENSOR
//#define USELAYER
//#define USELAYER19ONLY
#define USELADDER
//#define USELADDERI
//#define USESENSOR
int calconstrain(map<int, map<int,double> > &constraint, map<int, map<int,double> >&consplus, int calsub){

  char histn[100];
  TH2D *hl[9]={0};
  for(int il=0;il<9;il++){
    sprintf(histn,"hitmapl%d",il);
//    hl[il]=new TH2D(histn,histn,5000,-64,64,5000,-64,64);
    hl[il]=new TH2D(histn,histn,1000,-64,64,1000,-64,64);
  }

  const double cirr[]={62+2,62+2,46+2,46+2,46+2,46+2,46+2,46+2,43+2};//New L1L9 Geometry(+-2cm)
  const double ciry[]={47+2,40+2,44+2,44+2,36+2,36+2,44+2,44+2,29+2};
  TkTrackN *tkgeom=TkTrackN::GetHead(); 

  map<int, map<int,TMatrixD> > dera;
  map<int,int>sev;
  map<int,vector<int> >shiftn;
  for(map<int, TkLayerN>::iterator it=tkgeom->layers_.begin();it!=tkgeom->layers_.end();it++){//layer
    TkLayerN &layer=(it->second);
    int layid=layer.getid();
#ifndef USEXTLAYER
    if(layid==0||layid==8)continue;//now only shift inner tracker
#endif
#ifdef USEFINE
    int npxy[2]={2000,2000};//must be even
#else
    int npxy[2]={500,500};//must be even
#endif
    double dim[2]={2.*cirr[layid],2.*ciry[layid]};
    double dxy[2]={0};
    for(int ixy=0;ixy<2;ixy++){dxy[ixy]=dim[ixy]/(npxy[ixy]-1.);}
    for(int ipx=0;ipx<npxy[0];ipx++){
      double xv=-dim[0]/2.+dxy[0]*ipx;//local in plane
      for(int ipy=0;ipy<npxy[1];ipy++){
        double yv=-dim[1]/2.+dxy[1]*ipy;//local in plane
        TVector3 ploc(xv,yv,0);
        TVector3 dloc(0,0,1);
        TVector3 pgob=layer.toLab3(ploc,0);//local to global
        TVector3 dgob=layer.toLab3(dloc,1);
        double dists=0;
        TkSensorN *sensor=layer.FindSensor(dists,pgob,dgob,0);
        if(!sensor)continue;
        if(dists>0)continue;//must within sensor
        TkLadderN *ladder=(TkLadderN *)sensor->getmother();
/*        double distl=ladder->GetDistToEdgeRough(pgob,dgob);
        if(distl>0)continue;//must within ladder*/
        double chanx,chany;
        sensor->FindLocalChan(chanx,chany,pgob,dgob);
        double ladchanx=sensor->GetInLadderChanX(chanx);
        int tkid=ladder->getid();
        int senid=sensor->getid();
        AMSPlaneM plml=layer.GetLocalCoo(tkid,ladchanx,chany);
        if(plml.getMStat()<=-11)cerr<<"err plm status"<<endl;
//-----
        std::vector<std::pair<int,TMatrixD> >* pder=plml.getmalignder();
        TMatrixD matp=(*pder)[2].second;//layer (3,6)
        TMatrixD matpv=matp;
        matpv.Transpose(matpv);//layer (6,3)
//-------
        TMatrixD matl=((*pder)[1]).second;//ladder (3,6)
        TMatrixD derl(6,6);//ladder->layer (6,6)
        derl=matpv*matl;
//-------
        TMatrixD mats=((*pder)[0]).second;//sensor (3,6)
        TMatrixD ders(6,6);//sensor->layer (6,6)
        ders=matpv*mats;
//-----
        if(dera.find(layid)==dera.end()||dera[layid].find(tkid)==dera[layid].end()){
          dera[layid].insert(make_pair(tkid,derl));
          shiftn[layid].push_back(tkid);//only push ladder
        }
        else {
           TMatrixD &dern=dera[layid][tkid];
           for(int ip=0;ip<dern.GetNcols();ip++){
             for(int im=0;im<dern.GetNrows();im++)dern(im,ip)+=derl(im,ip);
          }
        }
//------sensor movement maybe not needed for layer movement
/*        if(dera.find(layid)==dera.end()||dera[layid].find(senid)==dera[layid].end())dera[layid].insert(make_pair(senid,ders));
        else {
           TMatrixD &dern=dera[layid][senid];
           for(int ip=0;ip<dern.GetNcols();ip++){
             for(int im=0;im<dern.GetNrows();im++)dern(im,ip)+=ders(im,ip);
          }
        }*/
//--------
        if(sev.find(layid)==sev.end())sev[layid]=1;
        else                          sev[layid]++; 
      }
    }
    for(map<int, TkLadderN>::iterator it1=layer.ladders_.begin();it1!=layer.ladders_.end();it1++){
      TkLadderN &ladder=(it1->second);
      int tkid=ladder.getid();
#ifdef USEFINE
      int npxy1[2]={1000,500};
#else
      int npxy1[2]={300,300};//must be even
#endif
      double dim1[2]={ladder.getdim(0),ladder.getdim(1)};
      double dxy1[2]={0};
      for(int ixy=0;ixy<2;ixy++){dxy1[ixy]=dim1[ixy]/(npxy1[ixy]-1.);}
      for(int ipx=0;ipx<npxy1[0];ipx++){
        double xv=-dim1[0]/2.+dxy1[0]*ipx;//local in plane
        for(int ipy=0;ipy<npxy1[1];ipy++){
          double yv=-dim1[1]/2.+dxy1[1]*ipy;//local in plane
          TVector3 ploc(xv,yv,0);
          TVector3 dloc(0,0,1);
          TVector3 pgob=ladder.toLab3(ploc,0);//local to global
          TVector3 dgob=ladder.toLab3(dloc,1);
          double dists=0;
          TkSensorN *sensor=ladder.FindSensor(dists,pgob,dgob,0);
          if(!sensor)continue;
          if(dists>0)continue;//must within sensor
          int senid=sensor->getid();
          double chanx,chany;
          sensor->FindLocalChan(chanx,chany,pgob,dgob);
          double ladchanx=sensor->GetInLadderChanX(chanx);
          AMSPlaneM plml=ladder.GetLocalCoo(ladchanx,chany);
          if(plml.getMStat()<=-11)cerr<<"err plm status2"<<endl;
//------
          std::vector<std::pair<int,TMatrixD> >* pder=plml.getmalignder();
          TMatrixD matp=((*pder)[1]).second;//ladder (3,6)
          TMatrixD matpv=matp;
          matpv.Transpose(matpv);//ladder (6,3)
//-----
          TMatrixD matl=((*pder)[0]).second;//sensor (3,6)
          TMatrixD derl(6,6);//sensor->ladder (6,6)
          derl=matpv*matl;
//-----
          if(dera.find(tkid)==dera.end()||dera[tkid].find(senid)==dera[tkid].end()){
            dera[tkid].insert(make_pair(senid,derl));
            shiftn[tkid].push_back(senid);
          }
          else {
            TMatrixD &dern=dera[tkid][senid];
            for(int ip=0;ip<dern.GetNcols();ip++){
              for(int im=0;im<dern.GetNrows();im++)dern(im,ip)+=derl(im,ip);
            }
          }
          if(sev.find(tkid)==sev.end())sev[tkid]=1;
          else                         sev[tkid]++;
        }
      }
    }
  }

//  return dera;
  const double tk_pz[9]={159.04,53.05,29.22,25.24,1.706,-2.292,-25.26,-29.25,-136.03};
//----------
  double sz=0;
  int nsz=0;
  map<int, map<int,double> > shearing;//layer shearing in x and y
  for(int il=0;il<9;il++){//all layers
    if(il==0||il==8)continue;//now only shift inner tracker (inner tracker is the reference)
    sz+=tk_pz[il];
    nsz++;
    for(int im=0;im<6;im++){
      int labelm=-20+im;
//------
      int cp1=im;
      labelm=cp1; 
//-----
      int labeldd=il*10+(im+1);
      if((calsub&0x1)==0)continue;//layer in global
      constraint[labelm][labeldd]=1;
      if(im<3){//x,y,z
        if(il>=1&&il<4)shearing[labelm][labeldd]=1;//no average shift on top
      }
    }
  }
//--layer shearing
#ifdef CONSSHEARING
  double mz=sz/nsz;
  for(map<int, map<int,double> >::iterator it=shearing.begin();it!=shearing.end();it++){
    for(map<int,double>::iterator it1=it->second.begin();it1!=it->second.end();it1++){
//       it1->second-=mz; 
    }
    consplus.insert(*it);
  }
#endif
//-----
  for(map<int, map<int,TMatrixD> >::iterator it=dera.begin();it!=dera.end();it++){//
    int label=it->first;
    vector<int> &labelss=shiftn[label];
//    double scale=1./double(sev[label])*(it->second.size());
    double scale=1./double(sev[label])*labelss.size();
//    cout<<"module mother="<<label<<" size="<<it->second.size()<<" scale="<<scale<<endl;
    int id=0;
    for(map<int,TMatrixD>::iterator it1=it->second.begin();it1!=it->second.end();it1++){//
//-----
      int labeld=it1->first;
//      cout<<"daughter"<<(id++)<<"="<<labeld<<endl;
      TMatrixD &dern=it1->second;
      for(int im=0;im<dern.GetNrows();im++){
        for(int ip=0;ip<dern.GetNcols();ip++){
          dern(im,ip)*=scale;
          int labelm=(label>=0)?label*10+(im+1):label*10-(im+1);
          if(labelm<0)labelm=10000000+abs(labelm);//negative ladder
//-----
          int cp2=(label+1)*10+im;
          bool isladder=(abs(label)>=20);
          if(isladder){//this is ladder
            int il=abs(label/100);
            if     (il==8)il=1;
            else if(il<8)il=il+1;
            il--;
            cp2=(label<0)?((il+1)*100+abs(label)%100+50)*10+im:((il+1)*100+abs(label)%100)*10+im;//
          }
          labelm=cp2; 
//-------
          if(im<3&&ip>=3)continue;//erase rotation in shift constrain!!!
//----
/*          if(im==3&&ip==4)continue;//in Xrot erase Yrot !!!
          if(im==4&&ip==3)continue;//in Yrot erase Xrot !!!*/
          if(isladder){//this is ladder(sensors in ladder) filter:in Zrot erase X !!!
            if(im<3) {if(ip!=im)continue;dern(im,ip)=1;}//in XYZ only XYZ !!!
            if(im==3){if(ip!=im)continue;dern(im,ip)=1;}//in Xrot only senstive to Xrot !!!
            if(im==4){if(ip!=2&&ip!=4)continue;}//in Yrot only senstive to dZ!!! dZ=x*beta !!!*/
//            if(im==4){if(ip!=2)continue;}//in Yrot only senstive to dZ!!! dZ=x*beta !!!
            if(im==5){if(ip!=1)continue;}//in Zrot only senstive to dY!!! dY=x*gamma !!!
          }
          else {//this is layer(ladder in layer)
//            if(im==3){if(ip!=2)continue;}///in Xrot only senstive to dZ!!!
          }
//---
          int labeldd=(labeld>=0)?labeld*10+(ip+1):labeld*10-(ip+1);
          if(labeldd<0)labeldd=10000000+abs(labeldd);//negative ladder
          if(isladder){if((calsub&0x4)==0)continue;}//sensor in ladder
          else        {if((calsub&0x2)==0)continue;}//ladder in layer
          constraint[labelm][labeldd]=dern(im,ip);
//          cout<<dern(im,ip)<<",";
        }
//        cout<<endl;
      }
//------
    }  
  }


  ///-----
  for(map<int, map<int,double> >::iterator it=constraint.begin();it!=constraint.end();it++){//
//    cout<<"constraint="<<it->first<<endl;
//------erase small value
    double maxv=0;
    for(map<int,double>::iterator it1=it->second.begin();it1!=it->second.end();it1++){
      if(fabs(it1->second)>maxv)maxv=fabs(it1->second);
    }
    map<int,double> bacv;
    for(map<int,double>::iterator it1=it->second.begin();it1!=it->second.end();it1++){
      if(fabs(it1->second)>maxv*1e-8){bacv.insert(*it1);}
//      else if(it1->second!=0){cout<<"rmlabel="<<it->first<<","<<it1->first<<" pv="<<it1->second<<" maxv="<<maxv<<endl;}
    }
    it->second=bacv;
    if(it->second.size()==0)cerr<<"err constrain size=0"<<it->first<<endl;
//-----
    for(map<int,double>::iterator it1=it->second.begin();it1!=it->second.end();it1++){
//       cout<<"label="<<it1->first<<","<<it1->second<<endl;
    }
  }

  TFile *fo=new TFile("hitmap.root","RECREATE");
  for(int il=0;il<9;il++){hl[il]->Write();}

  return constraint.size();
}

int AnalysisDet(){
#define USETESTBEAMD
//   TFile *f=new TFile("/eos/ams/group/mit/qyan/Data/alignment_PrMCB1221_400testaN1f/alignment_PrMCB1221_400testaN1f/1679417979_50file.root");
//   TFile *f=new TFile("/eos/ams/group/mit/qyan/Data/alignment_BTB1130_PR400N0AV3knoms/alignment_BTB1130_PR400N0AV3knoms/1281355854_50file.root");
//   TFile *f=new TFile("/eos/ams/group/mit/qyan/Data/alignment_BTB1130_PR400N0AV3m/alignment_BTB1130_PR400N0AV3m/1281355854_3000file.root");
//   TFile *f=new TFile("/eos/ams/group/mit/qyan/Data/alignment_BTB1130_PR400All_maxtest33.root");
   TFile *f=new TFile("/eos/ams/group/mit/qyan/Data/alignment_BTB1130_PR400Allo_maxsp.root");
//   TFile *f=new TFile("/eos/ams/group/mit/qyan/Data/alignment_PrMCB1221_400testaN1knomsTB/alignment_PrMCB1221_400testaN1knomsTB/1679323491_3000file.root");
   TH1D *hld[3]={0};
   TH2D *hse[3]={0};
   TH2D *hpos[3]={0};
   TH2D *hdir[3]={0};
   TH2D *hpos2[3]={0};
   char histn[1000];
   for(int ixy=1;ixy<=2;ixy++){
      sprintf(histn,"ladnev_z1xy%d",ixy);
      hld[ixy]=(TH1D *)f->Get(histn);
      hld[ixy]->GetXaxis()->SetTitle("Ladder ID");
      sprintf(histn,"sennev_z1xy%d",ixy);
      hse[ixy]=(TH2D *)f->Get(histn);
      hse[ixy]->GetXaxis()->SetTitle("Ladder ID");
      hse[ixy]->GetYaxis()->SetTitle("Sensor ID");
      int uxy=(ixy==1)?1:0;
      sprintf(histn,"ladpos_xy%d",uxy);
      hpos[ixy]=(TH2D *)f->Get(histn);
      hpos[ixy]->GetXaxis()->SetTitle("Ladder ID");
      hpos[ixy]->GetYaxis()->SetTitle("Beam Positon [cm]");
      sprintf(histn,"laddir_xy%d",uxy);
      hdir[ixy]=(TH2D *)f->Get(histn);
      hdir[ixy]->GetXaxis()->SetTitle("Ladder ID");
      hdir[ixy]->GetYaxis()->SetTitle("Beam Direction");
      sprintf(histn,"senpos_xy%d",uxy);
      hpos2[ixy]=(TH2D *)f->Get(histn);
      hpos2[ixy]->GetXaxis()->SetTitle("Sensor ID");
      hpos2[ixy]->GetYaxis()->SetTitle("Beam Positon [cm]");
   }

//----
  TCanvas *c=new TCanvas();
  c->Divide(1,2);
  for(int ixy=1;ixy<=2;ixy++){
    c->cd(ixy);
    TH1D *hn=hld[ixy];
    hn->Draw();
  }
  for(int ih=0;ih<4;ih++){
    TH2D *hn=0;
    c=new TCanvas();
    c->Divide(1,2);
    for(int ixy=1;ixy<=2;ixy++){
      c->cd(ixy);
      if(ih==0)hn=hse[ixy];
      else if(ih==1)hn=hpos[ixy];
      else if(ih==2)hn=hdir[ixy];
      else          hn=hpos2[ixy];
      hn->Draw("COLZ");
    }
  }

//------
   double posav[3]={0},dirav[3]={0},posav2[3]={0};
   TH1D *hpos1[3]={0};
   TH1D *hdir1[3]={0};
   TH1D *hdir1m[3]={0};
   TH1D *hpos21[3]={0};
   for(int ih=0;ih<3;ih++){
     for(int ixy=1;ixy<=2;ixy++){
       TH2D *hp=0;
       if     (ih==0)hp=hpos[ixy];
       else if(ih==1)hp=hdir[ixy];
       else if(ih==2)hp=hpos2[ixy];
       TH1D *h1=(TH1D *)hp->ProjectionX();
       h1->Reset();
       TH1D *h1m=(TH1D *)h1->Clone();
       double sumv=0;
       int nlad=0;
       for(int ibx=1;ibx<=hp->GetNbinsX();ibx++){
         sprintf(histn,"%sb%d",hp->GetName(),ibx);
         TH1D *hn=hp->ProjectionY(histn,ibx,ibx);
         if(hn->GetEntries()<100)continue;
         double rms=hn->GetRMS();
         h1->SetBinContent(ibx,rms);
         h1m->SetBinContent(ibx,hn->GetMean());
         sumv+=rms;
         nlad++;
       }
       if     (ih==0){hpos1[ixy]=h1; posav[ixy]=sumv/nlad;}
       else if(ih==1){hdir1[ixy]=h1; dirav[ixy]=sumv/nlad;hdir1m[ixy]=h1m;} 
       else          {hpos21[ixy]=h1; posav2[ixy]=sumv/nlad;}
     }
   }
    
 
//-----
   int slad=0;
   int ssen=0;
#ifdef USETESTBEAMD
//   double cutfr[2]={0.01,0.1};//ladder/sensor(>=350 events)
//   double cutfr[2]={0.01,0.05};
//   double cutfr[2]={0.01,0.045};
   double cutfr[2]={0.01,0.09};
#else
   double cutfr[2]={0.01,0.01};//ladder/sensor
#endif
   vector<int>badlad;
   vector<int>badsen;
   for(int il=0;il<trconstN::nLayer;il++){
    for(int is=0;is<trconstN::nSide;is++){
      for(int ilad=0;ilad<trconstN::mLadder;ilad++){
        int ladid=trconstN::LadderId[il][is][ilad];
        int nsen=trconstN::nSensor[il][is][ilad];
        if(ladid==0||nsen<=0)continue;
        slad++;
        int tkid=ladid%1000;
        int tkid2=(tkid>0)?((tkid/100%100-1)*20+tkid%100):((tkid/100%100+1)*20+tkid%100);
        double beamwp[3]={0};
        double beamwd[3]={0};
        double beamwdm[3]={0};
        int badn=0;
        for(int ixy=1;ixy<=2;ixy++){
          int ip=(ixy==1)?1:0;//Y/X
          int label=(tkid>=0)?tkid*10+(ip+1):tkid*10-(ip+1);
          if(label<0)label=10000000+abs(label);
          TH1D *hn=hld[ixy];
          double av=hn->GetEntries()/192.;
          int ubx=hn->FindBin(tkid2);
          double cn=hn->GetBinContent(ubx);
          beamwp[ixy]=hpos1[ixy]->GetBinContent(ubx);
          beamwd[ixy]=hdir1[ixy]->GetBinContent(ubx);
          beamwdm[ixy]=hdir1m[ixy]->GetBinContent(ubx);
          if(cn<av*cutfr[0]){
           cout<<"il="<<il<<" tkid="<<tkid<<" label="<<label<<" ixy="<<ixy<<" cn="<<cn<<" av="<<av<<" beamwp="<<beamwp[ixy]<<"/"<<posav[ixy]<<"="<<(beamwp[ixy]/posav[ixy])<<" beamwd="<<beamwd[ixy]<<"/"<<dirav[ixy]<<"="<<(beamwd[ixy]/dirav[ixy])<<endl;
           if(ip+1>badn)badn=(ip+1); 
          }
        }
        double cutangle=5./180.*3.1415926;//~0.09
        double cutalpha=2.5*cutangle;//y=2.5cm/7.2cm
        double cutbeta=8*cutangle;//x=8cm/ladder-lengh
//        double cutbeta=6*cutangle;
        double cutgamma=4.3;//dx=8.6cm/ladder-length(rms~dx/2,sen=4.2cm)
        int warfg=0;
        if(beamwd[1]<cutangle&&beamwd[2]<cutangle){//dw
          warfg+=1;
          if(badn!=2)badn+=10*3;
        }
        if((fabs(beamwd[1])+fabs(beamwdm[1]))*beamwp[1]<cutalpha&&(fabs(beamwd[2])+fabs(beamwdm[2]))*beamwp[1]<cutalpha){
          warfg+=10;
          if(badn!=2)badn+=100*4;
        }
        if((fabs(beamwd[1])+fabs(beamwdm[1]))*beamwp[2]<cutbeta&&(fabs(beamwd[2])+fabs(beamwdm[2]))*beamwp[2]<cutbeta){
          warfg+=100;
          if(badn!=2)badn+=1000*5;
        }
        if(warfg>=1){
          cout<<"il="<<il<<" tkid="<<tkid<<" "<<" tkid2="<<tkid2<<" ";
          if(warfg%10>0)cout<<"dw/";
          if(warfg/10%10>0)cout<<"dalpha/";
          if(warfg/100%10>0)cout<<"dbeta/";
          cout<<" would be poorly determinated"<<" beamwd="<<beamwdm[2]<<"+-"<<beamwd[2]<<","<<beamwdm[1]<<"+-"<<beamwd[1]<<" dy="<<beamwp[1]<<" dx="<<beamwp[2]<<endl;
        }
        int ladsen[3]={0};
        int senran[2]={100,0};
        for(int i=0;i<nsen;i++){
          int senid=(tkid>0)?tkid*100+i:tkid*100-i;
          int senid2=(tkid2>=0)?tkid2*20+i:tkid2*20-i;
          ssen++;
          int bads=0;
          double beamwp2[3]={0};
          for(int ixy=1;ixy<=2;ixy++){
            int ip=(ixy==1)?1:0;//Y/X
            int label=(senid>=0)?senid*10+(ip+1):senid*10-(ip+1);
            if(label<0)label=10000000+abs(label);
             TH2D *hn=hse[ixy];
             double av=hn->GetEntries()/2284.;
             double cn=hn->GetBinContent(hn->FindBin(tkid2,i));
             beamwp2[ixy]=hpos21[ixy]->GetBinContent(hpos21[ixy]->GetXaxis()->FindBin(senid2));
             if(cn<av*cutfr[1]){
               cout<<"il="<<il<<" ixy="<<ixy<<" tkid="<<tkid<<" label="<<label<<" isen="<<i<<" cn="<<cn<<" av="<<av<<" thr="<<(av*cutfr[1])<<endl;
               if(ip+1>bads)bads=(ip+1);
             }
             else {
               ladsen[ixy]++;
               if(i<senran[0])senran[0]=i;
               if(i>senran[1])senran[1]=i;
             }
          }
//-----
          double cutgamma2[3]={0, 1.2,1};//0,dy=2cm/7.2/dx=1*2/4.1
          if((fabs(beamwp2[1])<cutgamma2[1]||bads%10==1||badn%10==1)&&(fabs(beamwp2[2])<cutgamma2[2]||bads%10==2||badn%10==2)){//dy/dx require x/y has signal
            if(bads!=2&&badn!=2){
              bads+=10000*6;//can not use gamma
              cout<<"senid2="<<senid2<<" gamma";
              cout<<" would be poorly determinated"<<" beamwd2="<<" dy2="<<beamwp2[1]<<"/"<<posav2[1]<<" dx2="<<beamwp2[2]<<"/"<<posav2[2]<<endl;
            }
          }
          for(int ib=0;ib<5;ib++){
            int label=bads/int(pow(10.,ib))%10;
            if(label==0)continue;
            if(ib==0){if(badn%10>=bads%10)continue;}//if sensor X/Y has been mark bad in ladder
            label=(senid>=0)?senid*10+label:senid*10-label;
            if(label<0)label=10000000+abs(label);
            badsen.push_back(label);
          }
        }
        for(int ixy=1;ixy<=2;ixy++){
          if(ladsen[ixy]<=3){
            cout<<"il="<<il<<" tkid="<<tkid<<" tkid2="<<tkid2<<" ixy="<<ixy<<" nsen="<<ladsen[ixy]<<" beamwp="<<beamwp[ixy]<<"/"<<posav[ixy]<<"="<<(beamwp[ixy]/posav[ixy]);
            if(ladsen[ixy]>0)cout<<" senran="<<senran[0]<<","<<senran[1]<<"/"<<nsen<<"="<<(senran[1]-senran[0]+1.)/nsen;
            cout<<" beamwd="<<beamwd[ixy]<<"/"<<dirav[ixy]<<"="<<(beamwd[ixy]/dirav[ixy]);
            cout<<endl;
          }
        }
        if(badn!=2&&(ladsen[1]==2||ladsen[2]==2)){if(badn/1000%10==0)badn+=1000*5;if(fabs(beamwp[2])<cutgamma)badn+=10000*6;cout<<"2sen with senran="<<senran[0]<<","<<senran[1]<<endl;}
//        if(badn!=2&&(ladsen[1]==2&&ladsen[2]==2)){if(badn/1000%10==0)badn+=1000*5;if(fabs(beamwp[2])<cutgamma)badn+=10000*6;}
        for(int ib=0;ib<5;ib++){
          int label=badn/int(pow(10.,ib))%10;
          if(label==0)continue;
          label=(tkid>=0)?tkid*10+label:tkid*10-label;
          if(label<0)label=10000000+abs(label);
          badlad.push_back(label);
        }
      }
    }
   }
   cout<<"slad="<<slad<<" ssen="<<ssen<<endl;
   cout<<"badlad label="<<badlad.size()<<endl;
   for(int ib=0;ib<badlad.size();ib++){
     cout<<badlad[ib]<<",";
   }
   cout<<endl;
   cout<<"badsen label="<<badsen.size()<<endl;
   for(int ib=0;ib<badsen.size();ib++){
     cout<<badsen[ib]<<",";
   }
   cout<<endl;
   return 0; 
}


int AnalysisBeam(){
//   TFile *f=new TFile("/eos/ams/group/mit/qyan/Data/alignment_BTB1130_PR400N0AV3noms/alignment_BTB1130_PR400N0AV3noms/1281355854_50file.root");
//   TFile *f=new TFile("/eos/ams/group/mit/qyan/Data/alignment_BTB1130_PR400N0AV3knoms/alignment_BTB1130_PR400N0AV3knoms/1281355854_50file.root");
//   TFile *f=new TFile("/eos/ams/group/mit/qyan/Data/alignment_BTB1130_PR400All_maxtest2.root");
//   TFile *f=new TFile("/eos/ams/group/mit/qyan/Data/alignment_BTB1130_PR400All_maxtest31.root");
   TFile *f=new TFile("/eos/ams/group/mit/qyan/Data/alignment_BTB1130_PR400All_maxtest33.root");
   char histn[1000];
   const int nl=9;
   const int nxy=2;
   TH2D *hb[nl]={0};
   TH2D *hb2[nl][nxy]={0};
   for(int il=0;il<nl;il++){
     sprintf(histn,"posxyl%d",il);
     hb[il]=(TH2D *)f->Get(histn);
     for(int ixy=0;ixy<2;ixy++){
       sprintf(histn,"beamdxy%dl%d",ixy,il);
       hb2[il][ixy]=(TH2D *)f->Get(histn);
     }
   }
   sprintf(histn,"beamrun");
   TH2D *hr=(TH2D *)f->Get(histn);

//----load beam
   const int nfb=2;
   AMSPoint *pPos[nfb]={0,0};
   AMSDir   *pDir[nfb]={0,0};
   int nPos[nfb]={0};
   for(int ifl=0;ifl<nfb;ifl++){
//     int nb=416;
     int nb=906;
     if(ifl==1)nb=896;
     char sfn[1000];
//     if(nb==906)sprintf(sfn,"/afs/cern.ch/work/q/qyan/work/alignment/TestBeamPos_%dV2.txt",nb);
//     if(nb==906)sprintf(sfn,"/afs/cern.ch/work/q/qyan/work/alignment/TestBeamPos_%dV3.txt",nb);
     if(nb==906)sprintf(sfn,"/afs/cern.ch/work/q/qyan/work/alignment/TestBeamPos_%dV33.txt",nb);
     else       sprintf(sfn,"/afs/cern.ch/ams/Offline/AMSDataDir/v5.00/TestBeamPos_%d.txt",nb);
     ifstream fin(sfn);
     if (!fin) {
       cerr << "AMSEventR::GetBeamPos-E-File not found: " << sfn << endl;
     }
     pPos[ifl]= new AMSPoint[nb];
     pDir[ifl]= new AMSDir [nb];
     for (int i = 0; i < nb; i++) {
       double x, y, z, dx, dy, dz;
       fin >> x >> y >> z >> dx >> dy >> dz;
       if (fin.eof()) break;

       pPos[ifl][i].setp( x,  y,  z);
       pDir[ifl][i].setp(dx, dy, dz);
       nPos[ifl]++;
     }
      cout<<"ifl="<<ifl<<" nPos="<<nPos[ifl]<<endl;
      if (nPos[ifl] != nb) {
        cerr << "AMSEventR::GetBeamPos-E-File format is wrong: "
           << sfn << endl;
        nPos[ifl] = -1;
     }
     fin.close();
   }
  
   const double tk_pz[9]={159.04,53.05,29.22,25.24,1.706,-2.292,-25.26,-29.25,-136.03};
//-----
   TCanvas *c=0;
   int up=0;
//   int up=1;
   for(int il=0;il<nl;il++){
     TH2D *hn=hb[il];
     if(hn==0)continue;
     c=new TCanvas();
     gPad->SetLogz();
     hn->Draw("COL");
     double x[1000],y[1000],ex[1000],ey[1000];
     for(int i = 0; i < nPos[up]; i++) {
       double z1=tk_pz[il];
       AMSPoint p21 = pPos[up][i]+pDir[up][i]/pDir[up][i].z()*(z1-pPos[up][i].z());
       x[i]=p21[0];
       y[i]=p21[1];
       ex[i]=ey[i]=1.; 
     }
     TGraphErrors *gr = new TGraphErrors(nPos[up],x,y,ex,ey);
     gr->SetMarkerStyle(20);
     gr->SetMarkerColor(1);
     gr->Draw("Psame");
   }

//-----
/*  double mpdif=0;
  double mpldif[2]={0,0};
  for(int i = 0; i < nPos[0]; i++) {
    double pos[2][2];
    for(int iex=0;iex<2;iex++){
      int il=(iex==0)?0:8; 
      double z1=tk_pz[il];
      AMSPoint p21 = pPos[0][i]+pDir[0][i]/pDir[0][i].z()*(z1-pPos[0][i].z());
      pos[iex][0]=p21[0];
      pos[iex][1]=p21[1];
    }
    double mid=1000;
    double midl[2]={1000,1000};
    int ij=-1;
    for(int j = 0; j < nPos[1]; j++) {
      double pd=0;
      double pl[2]={1000,1000};
      for(int iex=0;iex<2;iex++){
        int il=(iex==0)?0:8;
        double z1=tk_pz[il];
        AMSPoint p21 = pPos[1][j]+pDir[1][j]/pDir[1][j].z()*(z1-pPos[1][j].z());
        double dx=p21[0]-pos[iex][0];
        double dy=p21[1]-pos[iex][1];
        double d1=sqrt(dx*dx+dy*dy);
        pl[iex]=d1;
        pd+=d1;
      }
      pd/=2.;
      if(pd<mid){mid=pd;ij=j;midl[0]=pl[0];midl[1]=pl[1];}
    }
    if(i!=ij)cout<<"i="<<i<<" j="<<ij<<" mid="<<mid<<endl;
    if(mid>mpdif)mpdif=mid;
  i  for(int iex=0;iex<2;iex++){if(midl[iex]>mpldif[iex])mpldif[iex]=midl[iex];}
  }
  cout<<"maxdif="<<mpdif<<","<<mpldif[0]<<","<<mpldif[1]<<endl;
*/ 
//----
   TF1 *fun=new TF1("gaus","gaus",-100,100); 
   TH1D *hmev[nl][2]={0};
   TH1D *hmve[nl][2]={0};
   TH1D *hmvc[nl][2]={0};
   for(int il=0;il<nl;il++){
    for(int ixy=0;ixy<2;ixy++){
      TH2D *hn=hb2[il][ixy];
      if(hn==0)continue;
      c=new TCanvas();
      gPad->SetLogz();
      hn->Draw("COLZ");
//----
      double xranu[2]={hn->GetXaxis()->GetBinLowEdge(1),hn->GetXaxis()->GetBinUpEdge(hn->GetNbinsX())};
      double dc=1;
      for(int ilh=0;ilh<2;ilh++){
        double cuty=(ilh==0)?-dc:dc;
        TLine *line = new TLine(xranu[0],cuty,xranu[1],cuty);
        line->SetLineColor(6);
        line->SetLineWidth(1);
        line->Draw();
      }
//----
      TH1D *pr1=(TH1D *)hn->ProjectionX();
      pr1->Reset();
      hmvc[il][ixy]=pr1;
      sprintf(histn,"%snev",hn->GetName());
      hmev[il][ixy]=(TH1D *)pr1->Clone(histn);
      hmev[il][ixy]->SetTitle(histn);
      sprintf(histn,"%serr",hn->GetName());
      hmve[il][ixy]=(TH1D *)pr1->Clone(histn);
      hmve[il][ixy]->SetTitle(histn);
      for(int ibx=1;ibx<=hn->GetNbinsX();ibx++){
        double tn=hn->Integral(ibx,ibx,1,hn->GetNbinsY());
        hmev[il][ixy]->SetBinContent(ibx,tn);
        if(tn<100)continue;
        sprintf(histn,"%sbx%d",hn->GetName(),ibx);
        TH1 *hn1=hn->ProjectionY(histn,ibx,ibx);
        double maxv=0;
        int ibc=0;
        for(int ibn=1;ibn<=hn1->GetNbinsX();ibn++){
          double xn=hn1->GetXaxis()->GetBinCenter(ibn);
          int lb=hn1->GetXaxis()->FindBin(xn-0.7);
          int hb=hn1->GetXaxis()->FindBin(xn+0.7);
          if(lb<=1)lb=1;
          if(hb>=hn1->GetNbinsX())hb=hn1->GetNbinsX();
          double pv=hn1->Integral(lb,hb);
          if(pv>maxv){maxv=pv;ibc=ibn;}
        }
        double mean=hn1->GetXaxis()->GetBinCenter(ibc);
//-------
/*        double maxv2=0;
        int ibc2=0;
        for(int ibn=1;ibn<=hn1->GetNbinsX();ibn++){
          double xn=hn1->GetXaxis()->GetBinCenter(ibn);
          if(xn>=mean-1.5&&xn<=mean+1.5)continue;
          int lb=hn1->GetXaxis()->FindBin(xn-0.7);
          int hb=hn1->GetXaxis()->FindBin(xn+0.7);
          if(lb<=1)lb=1;
          if(hb>=hn1->GetNbinsX())hb=hn1->GetNbinsX();
          double pv=hn1->Integral(lb,hb);
          if(pv>maxv2){maxv2=pv;ibc2=ibn;}
        }*/
//-------
        double rmean=mean;
        for(int ic=0;ic<2;ic++){
          fun->SetParameter(1,rmean);
          if(ic==0)hn1->Fit(fun,"NL","sames",rmean-0.7,rmean+0.7);
          else     hn1->Fit(fun,"N","sames",rmean-0.7,rmean+0.7);
          mean=fun->GetParameter(1);
          double rms=fun->GetParameter(2);
          fun->SetParameter(1,mean);
          if(ic==0)hn1->Fit(fun,"NL","sames",mean-2*rms,mean+2*rms);
          else     hn1->Fit(fun,"N","sames",mean-2*rms,mean+2*rms);
          mean=fun->GetParameter(1);
          rms=fun->GetParameter(2);
          fun->SetParameter(1,mean);
          if(ic==0)hn1->Fit(fun,"NL","sames",mean-1.8*rms,mean+1.8*rms);
          else     hn1->Fit(fun,"N","sames",mean-1.8*rms,mean+1.8*rms);
          mean=fun->GetParameter(1);
          rms=fun->GetParameter(2);
          fun->SetParameter(1,mean);
          if(ic==0)hn1->Fit(fun,"NL","sames",mean-1.7*rms,mean+1.7*rms);
          else     hn1->Fit(fun,"N","sames",mean-1.7*rms,mean+1.7*rms);
          mean=fun->GetParameter(1);
          rms=fun->GetParameter(2);
          if(fabs(mean)<3)break;
        }
        double emean=fun->GetParError(1);
        pr1->SetBinContent(ibx,mean);
        pr1->SetBinError(ibx,emean);
        hmve[il][ixy]->SetBinContent(ibx,emean);
      }
      pr1->SetMarkerStyle(20);
      pr1->SetMarkerColor(1);
      pr1->Draw("PLsame");
/*      sprintf(histn,"%sl%dxy%dp",hn->GetName(),il,ixy);
      TProfile *pr=(TProfile *)hn->ProfileX(histn);
      pr->Draw("same");
      pr->SetMarkerStyle(20);
      pr->SetMarkerColor(1);*/
//-----
    }
   } 

//-----
  map<int, int>badbeam;
  for(int ih=0;ih<3;ih++){
    for(int ixy=0;ixy<2;ixy++){
     c=new TCanvas();
     c->Divide(3,3); 
     for(int il=0;il<nl;il++){
       c->cd(il+1);
       TH1D *hn=hmvc[il][ixy];
       if     (ih==1)hn=hmve[il][ixy];
       else if(ih==2)hn=hmev[il][ixy];
       if(hn==0)continue;
       hn->Draw();
       if(ih==1&&(il==0||il==8)){
        for(int ibx=1;ibx<=hn->GetNbinsX();ibx++){
           double xv=hn->GetXaxis()->GetBinLowEdge(ibx);
           double err=hn->GetBinContent(ibx);
           double shift=hmvc[il][ixy]->GetBinContent(ibx);
           double nev=hmev[il][ixy]->GetBinContent(ibx);
           int ibeam=int(xv+0.01);
           if(xv>905.5)continue;
           if(err==0||err>0.05||fabs(shift)>=6){
             cout<<"ixy="<<ixy<<" il="<<il<<" bad point="<<xv<<" err="<<err<<" nev="<<nev<<" shift="<<shift<<endl;
             if(badbeam.find(ibeam)==badbeam.end())badbeam[ibeam]=1;
             else badbeam[ibeam]++;
           }
         }
       }
     }
   }
  }

//-----
//  const char *beamname="TestBeamPos_906V3.txt";
  const char *beamname="TestBeamPos_906V3T.txt";
  ofstream ofile(beamname);
  for(int i = 0; i < nPos[up]; i++) {
   AMSPoint p21[2];
   for(int iel=0;iel<2;iel++){
       int il=(iel==0)?0:8;
       double z1=tk_pz[il];
       p21[iel]= pPos[up][i]+pDir[up][i]/pDir[up][i].z()*(z1-pPos[up][i].z());//prediction
       AMSPoint shift(0,0,0);
       if(badbeam.find(i)!=badbeam.end()){cout<<"i="<<i<<" bad beam"<<endl;}
       else {
         for(int ixy=0;ixy<2;ixy++){
           shift[ixy]=hmvc[il][ixy]->GetBinContent(hmvc[il][ixy]->GetXaxis()->FindBin(i+0.01));
         }
       }
       p21[iel]+=shift;
//       cout<<"iel="<<iel<<" x,y,z="<<p21[iel][0]<<","<<p21[iel][1]<<","<<p21[iel][2]<<" dxy="<<shift[0]<<","<<shift[1]<<endl;
     }
     AMSDir dir(p21[1][0]-p21[0][0],p21[1][1]-p21[0][1],p21[1][2]-p21[0][2]);
     double z0=195;
     AMSPoint p0=p21[0]+dir/dir.z()*(z0-p21[0].z());
     ofile<<p0[0]<<" "<<p0[1]<<" "<<p0[2]<<" "<<dir[0]<<" "<<dir[1]<<" "<<dir[2]<<endl;
  }
  ofile.close();
  
 
//-----
   for(int il=0;il<nl;il++){
     c=new TCanvas();
     gPad->SetLogy();
     c->Divide(1,2);
     for(int ixy=0;ixy<2;ixy++){
       if(hb2[il][ixy]==0)continue;
       TH1D *hn=hb2[il][ixy]->ProjectionY();
       c->cd(ixy+1);
       hn->Draw();
//------
       double yranu[2]={hn->GetMinimum(),hn->GetMaximum()};
       double dc=1;
       for(int ilh=0;ilh<2;ilh++){
         double cutx=(ilh==0)?-dc:dc;
         TLine *line = new TLine(cutx,yranu[0],cutx,yranu[1]);
         line->SetLineColor(6);
         line->SetLineWidth(1);
//           line->SetLineStyle(2);
         line->Draw();
       }
//-----
     }
   }

//-----
   c=new TCanvas();
   gPad->SetLogz();
   hr->Draw("COLZ");
//   double cutbr=0.004;
//   double cutbr=0.01;
   double cutbr=0.005;
   map<int,vector<int> > beams;//run/beam
   map<int,vector<double> > evs;
   map<int,vector<int> > beamsv;//beam/run
   map<int,vector<double> > evsv;
   cout<<"cutnev="<<endl;
   double minev=10000000;
   for(int ibx=0;ibx<=hr->GetNbinsX();ibx++){
     int xl=int(hr->GetXaxis()->GetBinLowEdge(ibx)+0.03);
     double ct=hr->Integral(ibx,ibx,1,hr->GetNbinsY());
     if(ct<100)continue;
     cout<<(int(ct*cutbr))<<",";
     for(int iby=0;iby<=hr->GetNbinsY();iby++){
        double cn=hr->GetBinContent(ibx,iby);
        double yc=hr->GetYaxis()->GetBinLowEdge(iby)+0.03;
        int yl=int(yc);
        if(yc<0||cn<ct*cutbr||cn<50)continue;
        beams[xl].push_back(yl);
        evs[xl].push_back(cn);
        beamsv[yl].push_back(xl);
        evsv[yl].push_back(cn);
        if(cn<minev)minev=cn;
     }
   }
   cout<<endl;
   cout<<endl;
   int npat=0;
   for(int ic=0;ic<3;ic++){
     for(map<int,vector<int> >::iterator it=beams.begin();it!=beams.end();it++){
       vector<int> &bn=(it->second);
       if(ic==0){cout<<it->first<<",";continue;}
       if(ic==1)cout<<"{";
       for(int i=0;i<bn.size();i++){
         if     (ic==1)cout<<evs[it->first][i];
         else if(ic==2){cout<<"{"<<it->first<<","<<bn[i]<<"},";npat++;}
         if(ic==1&&i+1<bn.size())cout<<",";
       }
       if(ic==1)cout<<"},";
     }
     cout<<endl;
     cout<<endl;
   }

   //appear in >=two beam
/*   cout<<"have to check beam redandent"<<endl;
   for(map<int,vector<int> >::iterator itv=beamsv.begin();itv!=beamsv.end();itv++){
     vector<int> &bnv=(itv->second); 
      if(bnv.size()>=2){
       for(int i=0;i<bnv.size();i++){
         cout<<"{"<<bnv[i]<<","<<itv->first<<"}"<<" i="<<i<<" nev="<<evsv[itv->first][i]<<endl;
       }
     }
   }*/
   cout<<"beam-spot size="<<beams.size()<<","<<beamsv.size()<<" npat="<<npat<<" minev="<<minev<<endl;

  return 0;
}


int GenMisAlign(){

#define CALCONSTRAIN
//-------others
#define USEPRESIGMA
//#define INITPARFILE
//#define INITPARFILENEG
//#define USEZEROBIAS
//#define FIXLAYERFILE
//--------
#ifdef USETESTBEAM
#define USEZEROBIAS
#endif
//-------
#if defined (USELAYER) || defined (USELAYER19ONLY)
#define FIXLADDER
#define FIXSENSOR
//-------
#elif defined (USELADDER)
#define FIXLAYER
//#define FIXSENSOR
//-------
#elif defined (USESENSOR)
#define FIXLAYER
#define FIXLADDER
#endif
//------
  UInt_t seed=55274336;
  TRandom *rd=new TRandom(seed);
  UInt_t seed2=55885;
  TRandom *rd2=new TRandom(seed2);
#ifdef USETESTBEAM
  double pr[6]=   {360e-4,130e-4,200e-4,0.25e-3,0.25e-3,0.12e-3};
  double prext[6]={1000e-4,1000e-4,1000e-4,0.5e-3, 0.5e-3, 0.5e-3};
#else
  double pr[6]=   {500e-4, 500e-4, 500e-4, 1e-3,   1e-3,   1e-3};//plane: 500um,1e-3*60e4~600um
  double prext[6]={1000e-4,1000e-4,1000e-4,1e-3,   1e-3,   1e-3};
#endif
  std::map<int,std::pair<TVector3,TVector3> > pp;
  std::pair<TVector3,TVector3> pps;
  int ppn=0;
#ifdef USETESTBEAM
  double lr[6]={180e-4, 138e-4,45e-4, 0.36e-3,0.38e-3,0.22e-3};
#else
  double lr[6]={300e-4, 300e-4,300e-4,1e-3,      1e-3,   1e-3};//ladder:300um,1e-3*30e4~300um
#endif
  std::map<std::pair<int,int>,std::pair<TVector3,TVector3> > lp;
  std::map<int,std::pair<TVector3,TVector3> > lps;
  std::map<int,int> lpn;
#ifdef USETESTBEAM
  double sr[6]={3e-4,   3e-4,   3e-4, 0.1e-3,  0.1e-3, 0.1e-3};
#else
  double sr[6]={50e-4, 50e-4,  50e-4,   1e-3,    1e-3,   1e-3};//sensor: 50um, 1e-3*3e4~30um
#endif
  std::map<std::pair<std::pair<int,int>,int>, std::pair<TVector3,TVector3> > sp;
  std::map<std::pair<int,int>, std::pair<TVector3,TVector3> > sps;
  std::map<std::pair<int,int>, int> spn;
//------
//  double presig[3][2]={{1000e-4,1e-3},{200e-4,1e-3},{40e-4,1e-3}};
  double presig[3][6]={
#if defined (USELADDER) || defined (USESENSOR)
    {6e-4,   6e-4,   6e-4,   0.03e-3,0.03e-3,0.03e-3},//layer  u,v,w,alpha,beta,gamma
    {6e-4,   6e-4,   6e-4,   0.03e-3,0.03e-3,0.03e-3},//ladder u,v,w,alpha,beta,gamma
    {3e-4,   3e-4,   3e-4,   0.03e-3,0.03e-3,0.03e-3},//sensor u,v,w,alpha,beta,gamma
#elif defined (USETESTBEAM)
    {360e-4, 200e-4,200e-4,0.25e-3,0.25e-3,0.12e-3},//layer  u,v,w,alpha,beta,gamma
    {180e-4, 130e-4,45e-4, 0.36e-3,0.38e-3,0.22e-3},//ladder u,v,w,alpha,beta,gamma
    {5e-4,   5e-4,   5e-4, 0.1e-3,  0.1e-3, 0.1e-3},//sensor u,v,w,alpha,beta,gamma
#else
    {1000e-4,1000e-4,1000e-4,1e-3,1e-3,1e-3},//layer  u,v,w,alpha,beta,gamma
    { 200e-4, 200e-4, 200e-4,1e-3,1e-3,1e-3},//ladder u,v,w,alpha,beta,gamma
    {  40e-4,  40e-4,  40e-4,1e-3,1e-3,1e-3},//sensor u,v,w,alpha,beta,gamma
#endif
/*    {1000e-4,1000e-4,1000e-4,  1e-3,  1e-3,  1e-3},//layer  u,v,w,alpha,beta,gamma
    { 50e-4,   50e-4,  50e-4,0.5e-3,0.5e-3,0.5e-3},//ladder u,v,w,alpha,beta,gamma
    {  4e-4,    4e-4,   4e-4,0.3e-3,0.3e-3,0.3e-3},//sensor u,v,w,alpha,beta,gamma*/
  };
  double presigext[2][6]={
#if defined (USELADDER) || defined (USESENSOR)
    {6e-4,   6e-4,   6e-4, 0.03e-3,0.03e-3,0.03e-3},//layer  u,v,w,alpha,beta,gamma
    {6e-4,   6e-4,   6e-4, 0.03e-3,0.03e-3,0.03e-3},//ladder u,v,w,alpha,beta,gamma
#elif defined (USETESTBEAM)
    {5000e-4,5000e-4,5000e-4,   10e-3,  10e-3,  10e-3},//layer u,v,w,alpha,beta,gamma
    {180e-4, 130e-4,   45e-4, 0.36e-3,0.38e-3,0.22e-3},//ladder u,v,w,alpha,beta,gamma
#else
    {1000e-4,1000e-4,1000e-4,1e-3,1e-3,1e-3},//layer  u,v,w,alpha,beta,gamma
    { 200e-4, 200e-4, 200e-4,1e-3,1e-3,1e-3},//ladder u,v,w,alpha,beta,gamma
#endif
  };
//------
  double spar[6]={0};
  for(int il=0;il<trconstN::nLayer;il++){
    TRandom *rdp=(il==0||il==8)?rd2:rd;
#ifndef USEXTLAYER
    if(il==0||il==8)continue;//now only shift inner tracker
#endif
    for(int ipar=0;ipar<6;ipar++){
      if(il==0||il==8)spar[ipar]=rdp->Gaus(0,prext[ipar]);//external layer shift
      else            spar[ipar]=rdp->Gaus(0,pr[ipar]);
    }
    TVector3 pshf(spar[0],spar[1],spar[2]);
    TVector3 prot(spar[3],spar[4],spar[5]);
    pp[il]=std::make_pair(pshf,prot);
    if(il!=0&&il!=8){
      pps.first+=pp[il].first;
      pps.second+=pp[il].second;
      ppn++;
    }
    cout<<"il="<<il<<" smear="<<(pp[il]).first[0]<<","<<pps.first[0]<<endl;
    for(int is=0;is<trconstN::nSide;is++){
      for(int ilad=0;ilad<trconstN::mLadder;ilad++){
        int ladid=trconstN::LadderId[il][is][ilad];
        int nsen=trconstN::nSensor[il][is][ilad];
        if(ladid==0||nsen<=0)continue;
        int tkid=ladid%1000;
//        if(tkid==-201||tkid==215||tkid==-701||tkid==715)continue;
        for(int ipar=0;ipar<6;ipar++){
          spar[ipar]=rdp->Gaus(0,lr[ipar]);
        }
        TVector3 lshf(spar[0],spar[1],spar[2]);
        TVector3 lrot(spar[3],spar[4],spar[5]);
        std::pair<int,int> lid(il,tkid);
        lp[lid]=std::make_pair(lshf,lrot);
        if(lps.find(il)==lps.end()){lps[il]=lp[lid];lpn[il]=1;}
        else                       {lps[il].first+=lshf;lps[il].second+=lrot;lpn[il]++;}
        for(int i=0;i<nsen;i++){
          int senid=(tkid>0)?tkid*100+i:tkid*100-i;
          for(int ipar=0;ipar<6;ipar++){
            spar[ipar]=rdp->Gaus(0,sr[ipar]);
          }
          TVector3 sshf(spar[0],spar[1],spar[2]);
          TVector3 srot(spar[3],spar[4],spar[5]);
          std::pair<pair<int,int>,int> sid(lid,senid);
          sp[sid]=std::make_pair(sshf,srot);
          if(sps.find(lid)==sps.end()){sps[lid]=sp[sid];spn[lid]=1;}
          else                        {sps[lid].first+=sshf; sps[lid].second+=srot;spn[lid]++;}
        }
      }
    }
  }

//---average to 0
  std::map<int, double> alignpar;
  std::map<int,std::vector<int> >constrain;
  std::vector<int> labels[3];
//-----
  char fapp[200]="";
#ifdef USEXTLAYER
  strcat(fapp,"ex19");
#endif
#ifdef CALCONSTRAIN
  strcat(fapp,"calcons2");
#endif
#if defined (FIXLAYERFILE)
  strcat(fapp,"fixlayf");
#elif defined (FIXLAYER)
  strcat(fapp,"fixlay");
#endif
#ifdef FIXLADDER
  strcat(fapp,"fixlad");
#endif
//--------
#ifdef FIXSENSOR
  strcat(fapp,"fixsen");
#else
#ifndef FIXSENSORGAMMA
   strcat(fapp,"2");
#endif
#if defined (FIXSENSOR19)
   strcat(fapp,"fxsen19");
#elif defined (FIXSENSOR19X)
   strcat(fapp,"fxsen19x");
#endif
#endif
//-------
#if defined (INITPARFILENEG)
  strcat(fapp,"initparfneg");
#elif defined (INITPARFILE)
  strcat(fapp,"initparf");
#endif
#ifdef USEPRESIGMA
//  strcat(fapp,"psig");
  strcat(fapp,"psig");
#endif
#ifdef CONSSHEARING
  strcat(fapp,"cshear5");
#endif
#ifdef USEZEROBIAS
  strcat(fapp,"_nobis");
#endif
#ifdef USETESTBEAM
  strcat(fapp,"tb7");
#ifdef USEALLBEAM
  strcat(fapp,"ab");
#endif
#ifdef FIXLADALPHA
  strcat(fapp,"fa");
#endif
#endif
#ifdef USEMC
  strcat(fapp,"MC");
#endif
#ifdef USEFINE
  strcat(fapp,"f");
#endif
//-----
  char fname[1000];
#if defined (USELAYER19ONLY)
  strcpy(fname,"misalignlayer19");
#elif defined (USELAYER)
  strcpy(fname,"misalignlayer3");
#elif defined (USELADDER)
  strcpy(fname,"misalignladder3");
#elif defined (USESENSOR)
  strcpy(fname,"misalignsensor3");
#else
  strcpy(fname,"misalign3");
#endif
  strcat(fname,fapp);
  strcat(fname,".txt");
  ofstream ofile(fname);
  int    label;
  double ppar;
  std::pair<TVector3,TVector3> psum;
  map<int, map<int,double> > consplus;
  map<int, map<int,double> > calcons;
  int calsub=0;
  for(std::map<int,std::pair<TVector3,TVector3> >::iterator it=pp.begin();it!=pp.end();it++){//layer
    int il=it->first;
    if(il!=0&&il!=8){
      (it->second).first-=(pps.first*(1./double(ppn)));
      (it->second).second-=(pps.second*(1./double(ppn)));
      psum.first+=(it->second).first;
      psum.second+=(it->second).second;
    }
    for(int ip=0;ip<6;ip++){
      int detid=it->first;
      label=(detid>=0)?detid*10+(ip+1):detid*10-(ip+1);
      if(label<0)label=10000000+abs(label);//layer
      labels[0].push_back(label);
#ifndef FIXLAYER
      if(ip<3)ppar=(it->second).first[ip];
      else    ppar=(it->second).second[ip-3];
      alignpar[label]=ppar;
      ofile<<label<<" "<<ppar<<endl;
      int cp1=ip;
      calsub|=0x1;//layer in global constrain
      cout<<"cp1="<<cp1<<","<<label<<endl;
      if(il!=0&&il!=8)constrain[cp1].push_back(label);
#endif
    }
    cout<<"il="<<il<<" smear="<<(it->second).first[0]<<","<<(it->second).first[1]<<","<<(it->second).first[2]<<","<<(it->second).second[0]<<","<<(it->second).second[1]<<","<<(it->second).second[2]<<endl;
    cout<<"ils="<<psum.first[0]<<","<<psum.first[1]<<","<<psum.first[2]<<","<<psum.second[0]<<","<<psum.second[1]<<","<<psum.second[2]<<endl;
    std::pair<TVector3,TVector3> lsum;
    for(std::map<std::pair<int,int>,std::pair<TVector3,TVector3> >::iterator it1=lp.begin();it1!=lp.end();it1++){//ladder
      std::pair<int,int> lid=it1->first;
      if(lid.first!=il)continue;
      (it1->second).first-=(lps[il].first*(1./double(lpn[il])));
      (it1->second).second-=(lps[il].second*(1./double(lpn[il])));
      lsum.first+=(it1->second).first;
      lsum.second+=(it1->second).second;
      for(int ip=0;ip<6;ip++){
        int detid=(it1->first).second;
        label=(detid>=0)?detid*10+(ip+1):detid*10-(ip+1);
        if(label<0)label=10000000+abs(label);//ladder
        labels[1].push_back(label);
#ifndef FIXLADDER
        if(ip<3)ppar=(it1->second).first[ip];
        else    ppar=(it1->second).second[ip-3];
        alignpar[label]=ppar;
        ofile<<label<<" "<<ppar<<endl;
#ifdef USELADDER
        double weight=1;
        map<int, TkPlaneN*> &modules=TkTrackN::GetHead()->modules_;
        if(modules.find(detid)==modules.end()){
          cerr<<"<<---err moduel not exist="<<detid<<endl;
        }
        else {
          TkLadderN *ladder=(TkLadderN *)modules[detid];
          TMatrixD rtos(3,3);
          rtos.Zero();
          const TVector3 &u=ladder->getU();
          const TVector3 &v=ladder->getV();
          TVector3 w=ladder->getNormal();
          for(int i=0;i<3;i++){//(e1,e2,e3)^{T}
            rtos(0,i)=u[i];
            rtos(1,i)=v[i];
            rtos(2,i)=w[i];
          }
          if     (ip<3)weight=rtos(ip,ip);//only use eigven
          else if(ip==3)weight=rtos(1,1)*rtos(2,2);//alpha=v*w
          else if(ip==4)weight=rtos(2,2)*rtos(0,0);//beta=u*w
          else if(ip==5)weight=rtos(0,0)*rtos(1,1);//gamma=u*v
        } 
        int cp2=ip;
        if(il!=0&&il!=8){calcons[cp2].insert(make_pair(label,weight));}
#ifdef CONSSHEARING
        if(il>=1&&il<4&&ip<3)consplus[cp2].insert(make_pair(label,weight));
#endif
        calsub=(calsub&~0x1&~0x2);
#else
        int cp2=(il+1)*10+ip;
        calsub|=0x2;//ladder in layer constrain
#endif
        cout<<"cp2="<<cp2<<","<<label<<endl;
        constrain[cp2].push_back(label);
#endif
      }
      cout<<"tkid="<<lid.second<<" smear="<<(it1->second).first[0]<<","<<(it1->second).first[1]<<","<<(it1->second).first[2]<<","<<(it1->second).second[0]<<","<<(it1->second).second[1]<<","<<(it1->second).second[2]<<endl;
      cout<<"tkids="<<lsum.first[0]<<","<<lsum.first[1]<<","<<lsum.first[2]<<","<<lsum.second[0]<<","<<lsum.second[1]<<","<<lsum.second[2]<<endl;
      std::pair<TVector3,TVector3> ssum;
      for(std::map<std::pair<std::pair<int,int>,int>,std::pair<TVector3,TVector3> >::iterator it2=sp.begin();it2!=sp.end();it2++){
        std::pair<std::pair<int,int>,int> sid=it2->first;
        if(sid.first!=lid)continue;
        cout<<"senid="<<sid.second<<" smear="<<(it2->second).first[0]<<","<<(it2->second).first[1]<<","<<(it2->second).first[2]<<","<<(it2->second).second[0]<<","<<(it2->second).second[1]<<","<<(it2->second).second[2]<<endl;
        (it2->second).first-=(sps[lid].first*(1./double(spn[lid])));
        (it2->second).second-=(sps[lid].second*(1./double(spn[lid])));
        ssum.first+=(it2->second).first;
        ssum.second+=(it2->second).second;
        for(int ip=0;ip<6;ip++){
          int detid=(it2->first).second;
          label=(detid>=0)?detid*10+(ip+1):detid*10-(ip+1);
          if(label<0)label=10000000+abs(label);//sensor
          labels[2].push_back(label);
#ifndef FIXSENSOR
          if(ip<3)ppar=(it2->second).first[ip];
          else    ppar=(it2->second).second[ip-3];
          alignpar[label]=ppar;
          ofile<<label<<" "<<ppar<<endl;
#ifdef USESENSOR
          double weight=1;
          map<int, TkPlaneN*> &modules=TkTrackN::GetHead()->modules_;
          if(modules.find(detid)==modules.end()){
             cerr<<"<<---err moduel not exist="<<detid<<endl;
          }
          else {
            TkSensorN *sensor=(TkSensorN *)modules[detid];
            TMatrixD rtos(3,3);
            rtos.Zero();
            const TVector3 &u=sensor->getU();
            const TVector3 &v=sensor->getV();
            TVector3 w=sensor->getNormal();
            for(int i=0;i<3;i++){//(e1,e2,e3)^{T}
              rtos(0,i)=u[i];
              rtos(1,i)=v[i];
              rtos(2,i)=w[i];
            }
            if     (ip<3)weight=rtos(ip,ip);//only use eigven
            else if(ip==3)weight=rtos(1,1)*rtos(2,2);//alpha=v*w
            else if(ip==4)weight=rtos(2,2)*rtos(0,0);//beta=u*w
            else if(ip==5)weight=rtos(0,0)*rtos(1,1);//gamma=u*v
          }
          int cp3=ip;
          if(il!=0&&il!=8){calcons[cp3].insert(make_pair(label,weight));}
#ifdef CONSSHEARING
          if(il>=1&&il<4&&ip<3)consplus[cp3].insert(make_pair(label,weight));
#endif
          calsub=(calsub&~0x1&~0x2&~0x4);
#else
          int cp3=(lid.second<0)?((il+1)*100+abs(lid.second)%100+50)*10+ip:((il+1)*100+abs(lid.second)%100)*10+ip;
          calsub|=0x4;//sensor in ladder constrain
#endif
          cout<<"cp3="<<cp3<<","<<label<<endl;
          constrain[cp3].push_back(label);
#endif
        }
        cout<<"senids="<<ssum.first[0]<<","<<ssum.first[1]<<","<<ssum.first[2]<<","<<ssum.second[0]<<","<<ssum.second[1]<<","<<ssum.second[2]<<endl;
      }
    }
  }
  ofile.close(); 


//----fixpar
  std::map<int,double> fixpar;
//-------push_back fixed ladder/sensors
#ifdef USETESTBEAM
/*   int nladrej=7+4*5;
   int ladrej[]={10002022,10003022,4051,10006022,6142,10007022,7142,
     10001033,10001034,10001035,10001036,
     2143,    2144,    2145,    2146,//only two close sensor fired
     3143,    3144,    3145,    3146,
     6033,    6034,    6035,    6036,
     7033,    7034,    7035,    7036,//3,4/5 would be poorly deterimined due to the limit of point in a ladder
   }*/
#if defined (USEALLBEAM)
   int nladrej=17+24;
#else
//   const int nladrej=130+6;
   int nladrej=126+6;
#endif
#ifdef USEMC
   nladrej=nladrej+1;
#endif
   int ladrej[]={
#if defined (USEALLBEAM)
8124,10002022,10003025,4051,10009034,10009044,10009054,10009064,10009074,9014,9024,9034,9044,9054,9064,9074,9084,
//----manual added
4034,5034,6134,8024,8034,8054,8084,8094,8114,8134,8144,10003024,10003034,10004034,10006134,10008024,10008034,10008044,10008074,10008094,10008114,10008124,10009014,10009084,
#else
10008023,10008033,10008034,10008043,10008044,10008053,10008054,10008063,10008064,10008074,10008084,10008094,10008103,10008104,10008113,10008114,10008123,10008124,10008133,10008143,8023,8034,8044,8054,8063,8064,8073,8074,8083,8084,8093,8094,8103,8104,8114,8124,8134,8143,8144,10001033,10001034,10001133,1033,1034,1043,1053,1103,1123,1124,10002022,10002033,10002133,2033,2034,2053,2133,2143,2144,2145,10003022,10003033,10003133,3033,3034,3053,3123,3133,3143,3144,3145,10004133,10004134,4033,4034,4035,4043,4051,10005034,10005123,10005124,10005133,10005134,10005135,5033,5034,5043,10006022,10006034,10006133,10006134,10006135,6033,6034,6035,6133,6142,10007022,10007033,10007034,10007035,10007133,10007134,10007135,7033,7034,7035,7133,7134,7135,7142,10009014,10009024,10009034,10009044,10009054,10009064,10009074,10009084,9014,9024,9034,9044,9054,9064,9074,9084,
//----manual added
10008024,10008144,8024,10001124,10003034,10004034,
#endif
#ifdef USEMC
10004051,
#endif
   };

//----sensor
#if defined (USEALLBEAM)
   int nsenrej=1231;
#else
   int nsenrej=823;
#endif
#ifdef USEMC
   nsenrej=nsenrej+2;;
#endif
   int senrej[]={
#if defined (USEALLBEAM)
10802002,10802016,10802026,10802036,10802046,10802056,10802066,10802076,10802086,10802092,10802106,10802112,10803002,10803016,10803026,10803036,10803046,10803056,10803066,10803076,10803082,10803096,10803102,10803116,10803122,10804002,10804016,10804026,10804036,10804046,10804056,10804066,10804076,10804082,10804096,10804106,10804112,10804126,10804132,10805002,10805016,10805022,10805031,10805036,10805041,10805046,10805051,10805056,10805066,10805076,10805091,10805096,10805101,10805112,10805121,10805126,10805132,10806002,10806016,10806026,10806032,10806046,10806056,10806066,10806086,10806096,10806106,10806116,10806122,10806136,10806142,10807002,10807016,10807026,10807032,10807046,10807056,10807066,10807086,10807096,10807106,10807116,10807122,10807136,10807146,10808002,10808016,10808026,10808032,10808046,10808056,10808066,10808076,10808086,10808106,10808116,10808122,10808136,10808146,10809002,10809016,10809026,10809032,10809046,10809056,10809066,10809086,10809096,10809106,10809116,10809122,10809136,10809146,10810002,10810016,10810026,10810032,10810046,10810056,10810066,10810086,10810096,10810106,10810122,10810136,10810146,10811002,10811016,10811026,10811036,10811046,10811056,10811066,10811076,10811092,10811112,10811126,10811132,10812002,10812016,10812026,10812036,10812046,10812056,10812066,10812086,10812092,10812106,10812112,10812126,10812132,10813002,10813016,10813026,10813036,10813046,10813056,10813086,10813102,10813116,10813122,10814002,10814016,10814026,10814036,10814046,10814056,10814076,10814092,10814106,10814112,802002,802016,802022,802036,802046,802056,802062,802082,802096,802102,803002,803016,803022,803036,803042,803056,803066,803076,803082,803102,803116,803122,804002,804016,804022,804036,804042,804056,804066,804092,804106,804112,804126,804132,805002,805016,805022,805032,805046,805052,805066,805076,805086,805092,805106,805112,805122,805136,805142,806002,806016,806022,806032,806046,806052,806066,806076,806086,806096,806106,806116,806122,806136,806142,807002,807016,807022,807032,807046,807056,807066,807076,807082,807096,807106,807112,807122,807136,807142,808002,808026,808032,808046,808052,808066,808076,808106,808112,808122,808136,808142,809002,809022,809032,809046,809056,809066,809076,809092,809106,809112,809122,809136,809142,810002,810016,810022,810032,810046,810052,810066,810082,810096,810106,810112,810122,810136,810142,811002,811016,811022,811032,811046,811052,811066,811086,811112,811122,811136,811142,812002,812016,812022,812036,812042,812056,812066,812076,812082,812096,812106,812112,812126,812132,813002,813016,813022,813036,813042,813056,813066,813076,813082,813096,813102,813116,813122,814002,814016,814022,814036,814046,814056,814066,814076,814082,814096,814102,10103002,10103012,10103022,10103036,10103046,10103052,10103066,10103086,10103092,10103106,10103112,10103122,10104002,10104012,10104022,10104036,10104056,10104086,10105002,10105012,10105022,10105046,10105056,10105116,10105122,10105136,10106002,10106012,10106022,10106032,10107002,10107012,10107022,10107032,10107042,10107056,10107116,10108002,10108012,10108022,10108032,10108042,10108052,10109002,10109012,10109022,10109032,10109046,10109056,10109116,10110002,10110012,10110022,10110032,10110056,10110086,10110096,10111002,10111012,10111022,10111046,10111096,10112002,10112012,10112022,10112036,10112042,10112056,10112106,10112116,10113002,10113012,10113022,10113036,10113046,10113052,10113082,10113096,10113106,10113116,10113126,103002,103012,103026,103036,103042,103052,103066,103086,103096,103102,103112,103126,104002,104012,104021,104026,104036,104042,104056,104126,104136,105002,105012,105022,105036,105042,105052,105066,105116,106001,106006,106012,106022,106032,106042,106052,107002,107012,107022,107036,107052,108002,108012,108022,108032,108056,108126,109002,109012,109022,109032,109056,109096,110002,110012,110022,110032,110042,110056,110086,111002,111012,111022,111036,111042,111056,112002,112012,112022,112036,112042,112056,112086,113002,113012,113026,113036,113042,113052,113062,113086,113096,113126,10203006,10203026,10203036,10203052,10204016,10204026,10204066,10204106,10205001,10205026,10205102,10206002,10206012,10206106,10207002,10207012,10207026,10207046,10209002,10209012,10209026,10209111,10209116,10210002,10210016,10210026,10211006,10211026,10212006,10212012,10212026,10212062,10213026,10213036,10213046,203006,203016,203036,203066,203086,204006,204016,204022,204096,205002,205016,205026,205116,206002,206016,206022,207002,207016,207026,209002,209016,210002,210012,210022,211002,211016,211026,211076,212006,212016,212022,212076,213006,213056,213086,214002,214012,214026,214036,214042,214056,214076,214092,214102,214122,214136,10302002,10302012,10302022,10302032,10302042,10302056,10302062,10302072,10302086,10302092,10302102,10302112,10302122,10302132,10303006,10303016,10303026,10303036,10303052,10303066,10304006,10304026,10305002,10305102,10306002,10306012,10306026,10307002,10307012,10309002,10309012,10310002,10310016,10310026,10311002,10311016,10311026,10311066,10312006,10312012,10312026,10312056,10312062,303006,303016,303032,303066,303082,304006,304026,305002,305016,305026,306002,306016,306022,307002,307016,309002,309012,310002,310012,310022,311002,311016,311026,312006,312026,312076,313046,313056,313086,314002,314012,314026,314032,314042,314056,314066,314076,314091,314096,314102,314116,314122,314136,10403006,10403016,10403036,10403062,10403076,10404002,10404016,10404072,10405002,10405086,10405106,10406002,10406012,10407002,10407012,10407026,10407036,10407046,10409002,10409012,10409026,10409032,10409046,10410002,10410016,10410042,10411002,10411012,10411076,10411086,10412002,10412012,10412072,10412086,10413006,403002,403012,403026,403036,403066,403076,404002,404012,404056,404086,405002,405012,405026,405036,405046,405086,405106,405116,406002,406012,407002,407012,407026,407036,407106,409002,409012,409026,410002,410012,410026,411002,411012,411096,411116,412002,412012,412072,413006,413026,413046,413072,10503002,10503016,10503026,10503036,10503056,10503066,10503086,10504002,10504016,10504072,10505002,10505011,10505016,10505021,10505026,10505041,10505046,10505081,10505086,10505091,10505096,10505101,10505106,10506002,10506012,10506056,10506066,10507002,10507012,10507026,10507072,10509002,10509012,10509026,10509031,10510002,10510016,10511002,10511012,10511046,10511086,10511096,10512002,10512012,10512066,10512072,10512081,10512086,10513006,10513056,10513066,10513082,503002,503012,503026,503036,503056,503076,504002,504012,504086,504096,505002,505012,506002,506012,506042,506076,507002,507012,507026,507036,507106,509002,509012,509026,510002,510012,510021,510026,510031,510036,510041,510046,510101,510106,510111,511002,511012,511021,511026,511041,511046,511081,511086,511096,511116,512002,512012,512076,513002,513016,513026,513036,513072,10602002,10602026,10602032,10602052,10602086,10602096,10602102,10602112,10602122,10602132,10603002,10603012,10603026,10603046,10603052,10603062,10603076,10603086,10604002,10604016,10604026,10604036,10604072,10604082,10604096,10605002,10605016,10605062,10605096,10605106,10606002,10606012,10606066,10607002,10607012,10607022,10607046,10607066,10607076,10609002,10609012,10609022,10610002,10610012,10610066,10611002,10611012,10611081,10611086,10611092,10612002,10612012,10612072,10612086,10613002,10613012,10613026,10613046,10613056,10613062,10613076,10613082,603002,603012,603026,603046,603056,603062,603072,603086,604002,604012,604026,604036,604056,605002,605012,605066,605106,606002,606012,607002,607012,607022,607036,607042,607106,609002,609012,609022,609036,609076,610002,610012,610056,611002,611012,611096,612002,612012,612026,612046,612076,613002,613012,613026,613052,613072,614002,614016,614026,614032,614046,614056,614066,614086,614101,614106,614112,614126,614132,10702002,10702026,10702032,10702046,10702052,10702066,10702082,10702096,10702102,10702112,10702122,10702132,10703002,10703012,10703046,10703052,10703062,10703076,10703082,10704002,10704012,10704062,10704082,10704092,10705002,10705016,10705062,10705096,10706002,10706012,10706066,10707002,10707012,10707022,10707056,10707066,10707076,10709002,10709012,10709022,10709036,10709076,10710002,10710011,10710016,10711002,10711092,10712002,10712012,10712066,10712076,10712086,10712096,10713006,10713012,10713046,10713056,10713062,10713076,10713082,703002,703012,703026,703046,703056,703062,703072,703086,704002,704012,704056,705002,705012,705032,705066,705106,706002,706012,706036,707002,707012,707022,707036,707086,709002,709012,709022,709032,710002,710012,710036,711002,711012,711036,711066,711096,712002,712016,712026,713002,713012,713026,713052,713076,714006,714026,714032,714046,714052,714066,714076,714086,714096,714102,714112,714136,10901002,10901012,10901026,10901046,10901056,10901076,10901086,10901096,10901106,10902002,10902012,10902076,10902082,10902092,10902106,10902116,10903002,10903012,10903026,10903046,10903056,10903076,10903082,10903096,10903106,10904002,10904012,10904026,10904046,10904052,10904076,10904082,10904096,10904106,10905002,10905012,10905036,10905046,10905052,10905076,10905082,10905096,10905106,10905116,10906002,10906012,10906026,10906046,10906051,10906056,10906076,10906082,10906092,10906106,10907002,10907012,10907026,10907052,10907076,10907082,10907091,10907096,10907106,10907116,10908002,10908012,10908026,10908046,10908056,10908076,10908082,10908096,10908106,901002,901012,901022,901036,901046,901056,901066,901076,901086,902002,902012,902022,902036,902056,902066,902076,902086,902096,903002,903012,903022,903036,903056,903066,903076,903086,903096,904002,904012,904026,904036,904056,904066,904076,904086,904096,905002,905012,905022,905056,905066,905076,905086,906002,906012,906022,906036,906056,906066,906076,906086,907002,907012,907026,907036,907046,907056,907066,907076,907086,907096,908002,908012,908022,908036,908056,908066,908076,908086,
#else
10802002,10802052,10802112,10803002,10803052,10803082,10803102,10803122,10804002,10804052,10804062,10804082,10804092,10804112,10804132,10805002,10805022,10805031,10805041,10805052,10805062,10805092,10805101,10805112,10805121,10805132,10806002,10806032,10806062,10806092,10806122,10806142,10807002,10807032,10807062,10807092,10807122,10808002,10808032,10808062,10808092,10808122,10809002,10809032,10809062,10809092,10809122,10810002,10810032,10810062,10810092,10810122,10810142,10811002,10811052,10811062,10811092,10811112,10811132,10812002,10812052,10812062,10812081,10812092,10812112,10812132,10813002,10813052,10813082,10813102,10813122,10814002,10814052,10814112,802002,802022,802042,802062,802082,802102,803002,803022,803042,803052,803072,803082,803102,803122,804002,804022,804042,804052,804062,804082,804092,804112,804132,805002,805022,805032,805052,805062,805082,805092,805112,805122,805142,806002,806022,806032,806052,806062,806082,806092,806112,806122,806142,807002,807022,807032,807052,807062,807082,807092,807112,807122,807142,808002,808032,808052,808062,808082,808092,808112,808122,808142,809002,809032,809052,809062,809082,809092,809112,809122,809142,810002,810022,810032,810052,810062,810082,810092,810112,810122,810142,811002,811022,811032,811052,811062,811082,811092,811112,811122,811142,812002,812022,812042,812052,812062,812082,812092,812112,812132,813002,813022,813042,813052,813072,813082,813102,813122,814002,814022,814042,814062,814082,814102,10103002,10103012,10103022,10103032,10103042,10103052,10103062,10103092,10103112,10103122,10104002,10104012,10104022,10104032,10104062,10104082,10105002,10105012,10105022,10105122,10106002,10106012,10106022,10106032,10107002,10107012,10107022,10107032,10107042,10107052,10108002,10108012,10108022,10108032,10108042,10108052,10108062,10109002,10109012,10109022,10109032,10109052,10110002,10110012,10110022,10110032,10111002,10111012,10111022,10112002,10112012,10112022,10112032,10112042,10112062,10113002,10113012,10113022,10113032,10113042,10113052,10113082,103002,103012,103022,103032,103042,103052,103072,103082,103102,103112,104002,104012,104022,104032,104042,105002,105012,105022,105032,105042,105052,105062,106001,106012,106022,106032,106042,107002,107012,107022,107142,108002,108012,108022,108032,109002,109012,109022,109032,110002,110012,110022,110032,110042,111002,111012,111022,111032,111042,112002,112012,112022,112032,112042,113002,113012,113022,113032,113042,113062,113072,113122,10203002,10203012,10203052,10204002,10204082,10205102,10206002,10206012,10207002,10207012,10207022,10207032,10207042,10209002,10209012,10209022,10209032,10209111,10210002,10211052,10211092,10212002,10212012,10212062,10213002,10213012,203002,203012,203032,203042,204002,204012,205002,205012,205022,206002,206022,207002,209002,210002,210012,210022,211002,211012,211022,211072,212002,212012,213002,213032,213042,213082,214002,214012,214022,214032,214041,214052,214062,214082,214092,214102,214112,214122,214132,10303002,10303012,10303052,10303062,10304002,10305002,10305102,10306002,10306012,10307002,10307012,10307022,10307042,10309002,10309012,10309022,10309032,10310002,10310082,10311001,10311052,10311092,10312002,10312012,10312062,10313002,10313012,303002,303012,303032,303042,303082,304002,304012,304022,305002,305012,305022,306002,306022,307002,309002,309012,310002,310012,310022,311002,311012,311022,312002,312012,312022,313002,313032,313042,313082,314002,314012,314022,314032,314042,314052,314062,314082,314092,314102,314112,314122,314132,10403002,10403012,10403032,10403052,10403062,10403071,10404002,10404072,10405002,10405022,10406002,10406012,10407002,10407012,10407022,10409002,10409012,10409032,10410002,10410042,10411002,10411012,10412002,10412012,10412042,10412062,10412072,10413002,10413012,10413042,10413062,403002,403012,403021,403042,403072,403082,404002,404012,404022,404052,405002,405012,405022,406002,406012,406032,406042,407002,407012,407022,409002,409012,409022,409062,410002,410012,411002,411012,411022,412002,412012,412022,412032,412072,413002,413012,413032,413042,413052,413072,10503002,10503012,10503032,10503052,10503062,10503072,10504002,10504062,10504072,10505002,10505011,10505022,10505031,10505041,10505051,10505081,10505101,10506002,10506012,10506041,10507002,10507012,10507022,10507072,10509002,10509012,10510002,10510042,10511002,10511012,10512002,10512012,10512041,10512062,10512072,10512081,10513002,10513012,10513042,10513062,10513082,503002,503012,503022,503042,503072,504002,504012,504022,505002,505012,505022,506002,506012,506032,506042,507002,507012,507022,509002,509012,509022,510002,510012,510021,510031,510041,510101,511002,511012,511022,511031,511041,511081,512002,512012,512022,512032,513002,513012,513032,513042,513052,10603002,10603012,10603022,10603032,10603052,10603062,10604002,10604062,10604072,10604082,10605002,10605061,10606002,10606012,10606022,10606042,10607002,10607012,10607022,10609002,10609012,10609022,10610002,10610012,10611002,10611012,10611022,10611042,10611092,10612002,10612012,10612062,10612072,10613002,10613012,10613032,10613042,10613062,10613082,603002,603012,603022,603032,603042,603062,603072,604002,604012,604022,604032,605002,605012,606002,606012,606022,606042,607002,607012,607022,607042,609002,609012,609022,609032,610002,610012,610022,611002,611012,611022,611041,612002,612012,612022,612032,612042,613002,613012,613022,613032,613042,613052,10703002,10703012,10703022,10703032,10703052,10703062,10703082,10704002,10704062,10704072,10704082,10704092,10705002,10705062,10706002,10706012,10706022,10706032,10706042,10707002,10707012,10707022,10709002,10709012,10709022,10710002,10710012,10711002,10711012,10711022,10711042,10711092,10712002,10712012,10712062,10712072,10713002,10713012,10713032,10713042,10713062,10713082,703002,703012,703022,703032,703042,703062,703072,704002,704012,704022,704032,705002,705012,705032,706002,706012,706022,706042,707002,707012,707022,707042,709002,709012,709022,709032,710002,710012,710022,711002,711012,711022,712002,712012,712022,712032,713002,713012,713022,713032,713042,713052,10901002,10901012,10901052,10902002,10902012,10902052,10902082,10903002,10903012,10903052,10903082,10904002,10904012,10904052,10904082,10905002,10905012,10905052,10905082,10906002,10906012,10906052,10906082,10906092,10907002,10907012,10907052,10907082,10907091,10908002,10908012,10908052,10908082,901002,901012,901022,902002,902012,902022,903002,903012,903022,904002,904012,904022,905002,905012,905022,906002,906012,906022,907002,907012,907022,908002,908012,908022,
#endif
#ifdef USEMC
10505071,10505091,
#endif
   };
#else
   const int nladrej=3;
   int ladrej[]={10002022,10004051,4051};//405001,10405001,
   const int nsenrej=22;
   int senrej[]={103001,104001,108001,10106001,10107001,10108001,10109001,10110001,106001,109001,110001,111001,112001,//13
                110001,111001,112001,411001,10505011,10505031,10505051,10505071,10505091};//9
#endif
   for(int k=0;k<3;k++){
     for(int j=0;j<labels[k].size();j++){
       int label=labels[k][j];
       int pdel=0;
       for(int i=0;i<nladrej;i++){
         if(label==ladrej[i]||(label/10==ladrej[i]/10&&ladrej[i]%10==2)){pdel=1;break;}//ladder match
         if((label/10000000==ladrej[i]/10000000)&&(label/1000%1000==ladrej[i]/10%1000)&&((label%10==1&&label%10==ladrej[i]%10)||ladrej[i]%10==2)){pdel=1;break;}//sensor match
       }
       if(pdel==0){
         for(int i=0;i<nsenrej;i++){
           if(label==senrej[i]||(label/10==senrej[i]/10&&senrej[i]%10==2)){pdel=1;break;}//sensor match
         }
       }
#ifdef USELAYER19ONLY
       if(k==0){int il=(label/10)%10;if(!(il==0||il==8))pdel=1;}//layer only accept L19
       if(k==1){int il=(label/1000)%10;if(!(il==8||il==9))pdel=1;}//ladder only accept L19
       if(k==2){int il=(label/100000)%10;if(!(il==8||il==9))pdel=1;}//sensor only accept L19
#endif
//---------
#ifdef USETESTBEAM
       if(pdel==0){//reject dw/alpha/beta in alignment for ladder/sensor due to limited angle in TB
#ifdef FIXLADALPHA
          if(k==1){if(label%10==4)pdel=1;}//fixed ladder alpha
#endif
//         if(k==1){if(label%10>=3&&label%10<=5)pdel=1;}
          if(k==2){//fixed w,alpha,beta,gamma,only use u,v (gamma should be tried, should be big effect)
#ifdef FIXSENSORGAMMA
            if(label%10>=3)pdel=1;
#else
            if(label%10>=3&&label%10<=5)pdel=1;
#endif
            if(label%10==1&&label/100000%10==9)pdel=1;//L9 do not use sensor alignment for X (strange behavior)
            if(label%10==6&&(label/100000%10==9||label/100000%10==8))pdel=1;//L1(L9) do not use sensor gamma,very small beam~1/10(1/4) fit
#if defined (FIXSENSOR19)
            if(label/100000%10==9||label/100000%10==8)pdel=1;//L1(L9) do not use sensor alignment for both X and Y
#elif defined (FIXSENSOR19X)
            if(label%10!=2&&(label/100000%10==9||label/100000%10==8))pdel=1;//L1(L9) do not use sensor alignment for X, only use Y
#endif
          }
       }
#endif
       if(pdel==0)continue;
       if(alignpar.find(label)==alignpar.end())continue;
       cout<<"del-label="<<label<<endl;
       fixpar[label]=alignpar[label];
#ifdef USEZEROBIAS
       fixpar[label]=0;
#endif
    }
  }
//-----fix from readfile
#if defined (FIXLAYERFILE)
  ifstream fin("/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayernoweight/millepede.res");
  string s;
  int ifix=0;
  while(std::getline(fin,s)){
    istringstream iss(s);
    int label;
    double var=0,fix=0;
    if(!(iss>>label>>var))continue;
    if(ifix==0){if(label>100)continue;}//fixed layer
    fixpar[label]=var;//overwrite fix layer
  }
  fin.close();
#endif
//------
  for(int k=0;k<3;k++){
#ifndef FIXLAYER
    if(k==0)continue;
#endif
#ifndef FIXLADDER
    if(k==1)continue;
#endif
#ifndef FIXSENSOR
    if(k==2)continue;
#endif
    for(int j=0;j<labels[k].size();j++){
      int label=labels[k][j]; 
      if(fixpar.find(label)!=fixpar.end())continue;//find fix
      fixpar[label]=0.;
    }
  }

//--------
#if defined (USELAYER19ONLY)
  strcpy(fname,"layercon19");
#elif defined (USELAYER)
  strcpy(fname,"layercon4");
#elif defined (USELADDER)
  strcpy(fname,"laddercon4");
#elif defined (USESENSOR)
  strcpy(fname,"sensorcon4");
#else
  strcpy(fname,"aligncon4");
#endif
  strcat(fname,fapp);
  strcat(fname,".txt");

#ifdef CALCONSTRAIN
  if(calsub>0)calconstrain(calcons,consplus,calsub);
  cout<<"calsub="<<calsub<<endl;
#endif
  ofstream cfile(fname);
//-------put constrains
  int nconstrain=0;
  for(std::map<int,std::vector<int> >::iterator it=constrain.begin();it!=constrain.end();it++){
#ifdef CALCONSTRAIN
    if(calcons.find(it->first)==calcons.end()||calcons[it->first].size()==0){
      cout<<"error could not find constrain="<<it->first<<endl;
      continue;
    }
   for(int ic=0;ic<2;ic++){
     map<int,double> caln=calcons[it->first];
     if(ic==1){//check and adding with constrain plus
       if(consplus.find(it->first)==consplus.end())continue;
       else caln=consplus[it->first];
     }
     double sumcons=0.;
     map<int,double> caln2=caln;
//----
     caln2.clear();
     for(map<int,double>::iterator it1=caln.begin();it1!=caln.end();it1++){
       int label=it1->first;
       if(fixpar.find(label)!=fixpar.end()){//this par is fixed
         cout<<"constrain::find fix-label="<<label<<endl;
         sumcons-=(it1->second)*alignpar[label];
#ifdef USEZEROBIAS
         sumcons=0;
#endif
         continue;
       }
       caln2.insert(*it1);
     }
//-----
     if(caln2.size()==0)continue;
     if(caln2.size()==1){
       map<int,double>::iterator it0=caln2.begin();
       int label=it0->first;
       double weight=it0->second; 
       if(fixpar.find(label)==fixpar.end())fixpar[label]=(weight!=0)?sumcons/weight:0;//insert to the fix par
       cout<<"single constrain mother="<<it->first<<" ic="<<ic<<" fix-label="<<label<<" val="<<fixpar[label]<<"="<<sumcons<<"/"<<weight<<endl;
       continue;
     }
     nconstrain++;
     if     (fabs(sumcons)<1e-16)cfile<<"Constraint 0.0"<<endl;
     else if(ceil(sumcons)==floor(sumcons)){//special treatment for int
       cout<<"constraint::find int="<<sumcons<<endl;
       cfile<<"Constraint "<<std::fixed<<sumcons<<endl;
       cfile.unsetf(ios_base::floatfield);
     }
     else {
       cfile<<"Constraint "<<sumcons<<endl;
     }
     for(map<int,double>::iterator it1=caln2.begin();it1!=caln2.end();it1++){
       if(ceil((*it1).second)==floor((*it1).second)){//special treatment for int
         cfile<<(*it1).first<<" "<<std::fixed<<(*it1).second<<endl;
         cfile.unsetf(ios_base::floatfield);
       }
       else {
        cfile<<(*it1).first<<" "<<(*it1).second<<endl;
       }
      }
    }
#else
    std::vector<int> &clabel=it->second;
    std::sort(clabel.begin(),clabel.end());
    double sumcons=0.;
    std::vector<int> clabel2=clabel;
//----
    clabel2.clear();
    for(std::vector<int>::iterator it1=clabel.begin();it1!=clabel.end();it1++){
      int label=(*it1);
      if(fixpar.find(label)!=fixpar.end()){//this par is fixed
        cout<<"constrain::find fix-label="<<label<<endl;
        continue;
      }
      clabel2.push_back(label);
      sumcons+=alignpar[label];
#ifdef USEZEROBIAS
      sumcons=0;
#endif
    }
//-----
    if(clabel2.size()==0)continue;
    nconstrain++;
    if     (fabs(sumcons)<1e-16)cfile<<"Constraint 0.0"<<endl;
    else if(ceil(sumcons)==floor(sumcons)){//special treatment for int
      cout<<"constraint::find int="<<sumcons<<endl;
      cfile<<"Constraint "<<std::fixed<<sumcons<<endl;
      cfile.unsetf(ios_base::floatfield);
    }
    else {
      cfile<<"Constraint "<<sumcons<<endl;
    }
    for(std::vector<int>::iterator it1=clabel2.begin();it1!=clabel2.end();it1++){
      cfile<<(*it1)<<" 1.0"<<endl;
    }
/*#ifdef CONSSHEARING
    const double tk_pz[9]={159.04,53.05,29.22,25.24,1.706,-2.292,-25.26,-29.25,-136.03};
    if(it->first==0||it->first==1){
      nconstrain++;
      double sumv=0;
      for(int ii=0;ii<2;ii++){
        if(ii==1)cfile<<"Constraint "<<sumv<<endl;
        sumv=0;
        int jj=0;
        for(std::vector<int>::iterator it=clabel.begin();it!=clabel.end();it++){
          if(jj<clabel.size()/2){if(ii==1)cfile<<(*it)<<" 1.0"<<endl;  sumv+=alignpar[(*it)];}
          else                  {if(ii==1)cfile<<(*it)<<" -1.0"<<endl;sumv-=alignpar[(*it)];}
          jj++;
        }
      }
    } 
#endif*/
#endif
  }
  cout<<"tconstrain="<<nconstrain<<endl;


//----
  if(fixpar.size()>=1){//fixed
    cfile<<"Parameter"<<endl;
    for(std::map<int,double>::iterator it=fixpar.begin();it!=fixpar.end();it++){
      if(ceil((*it).second)==floor((*it).second)){//special treatment for int
//        cout<<"parameter::find int="<<(*it).second<<endl;
        cfile<<(*it).first<<" "<<std::fixed<<(*it).second<<" -1.0"<<endl;
        cfile.unsetf(ios_base::floatfield);
      }
      else {
        cfile<<(*it).first<<" "<<(*it).second<<" -1.0"<<endl;
      }
    }
  }

//--------
  std::map<int,double> initpar;
#if defined (INITPARFILE) || defined (INITPARFILENEG)
#ifdef INITPARFILENEG
  ifstream finit("refine3itersmaller.res");
#else
  ifstream finit("/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayerladsennoweightnomstest/noiternorme/minres1iter/millepede.res");
#endif
  string s1;
  while(std::getline(finit,s1)){
    istringstream iss(s1);
    int label;
    double var=0,fix=0;
    if(!(iss>>label>>var))continue;
#ifdef INITPARFILENEG
    initpar[label]=-var;
#else
    initpar[label]=var;
#endif
  }
  finit.close();
#endif

   int npresig=0;
#if defined (USEPRESIGMA) || defined (INITPARFILE) || defined (INITPARFILENEG)
   if(fixpar.size()==0)cfile<<"Parameter"<<endl;
   for(int i=0;i<3;i++){
    std::sort(labels[i].begin(),labels[i].end());
    for(std::vector<int>::iterator it=labels[i].begin();it!=labels[i].end();it++){
      int label=(*it);
      if(fixpar.find(label)!=fixpar.end())continue;//skip fixed par
      double npar=0;
      if(initpar.find(label)!=initpar.end()){
        npar=initpar[label];
      }
      if(ceil(npar)==floor(npar)){//special treatment for int
        cfile<<label<<" "<<std::fixed<<npar;
        cfile.unsetf(ios_base::floatfield);
      }
      else {
        cfile<<label<<" "<<npar;
      }
      double sigma=0;
#ifdef USEPRESIGMA
//      sigma=((label%10)<=3)?presig[i][0]:presig[i][1];
      if     (i==0&&((label/10)==0||(label/10)==8))      sigma=presigext[i][label%10-1];//external layer
      else if(i==1&&(label/1000%10==8||label/1000%10==9))sigma=presigext[i][label%10-1];//external ladder
      else                                               sigma=presig[i][label%10-1];
#endif
      if(sigma==0)cfile<<" 0.0"<<endl;
      else        cfile<<" "<<sigma<<endl; 
      npresig++;
    }
  }
#endif
  cout<<"nfixpar="<<fixpar.size()<<" npresig+init="<<npresig<<endl;
  cfile.close();
 
  return 0;
}


int AnalysisResult(){
#define USESENSORA
//#define USELADDERA
//#define USERATIO
#define USETESTBEAMA
//  const int nfl=4;
//    const int nfl=4+1;
#ifdef USETESTBEAMA
#if defined (USESENSORA)
  const int nfl=4+1;
#elif defined (USELADDERA)
//  const int nfl=6;
//  const int nfl=5+1;
  const int nfl=5;
#else
//  const int nfl=3+3;
  const int nfl=5;
#endif
//-------
#else
  const int nfl=4+1+1+1;
#endif
  double presig[3][6]={
    {360e-4, 200e-4,200e-4,0.25e-3,0.25e-3,0.12e-3},//layer  u,v,w,alpha,beta,gamma
    {180e-4, 130e-4,45e-4, 0.36e-3,0.38e-3,0.22e-3},//ladder u,v,w,alpha,beta,gamma
    {5e-4,   5e-4,   5e-4, 0.1e-3,  0.1e-3, 0.1e-3},//sensor u,v,w,alpha,beta,gamma
  };
  const char *file[]={
#ifdef USETESTBEAMA
//    "/afs/cern.ch/work/q/qyan/work/alignment/noshear/misalign3calconsfixsenpsig_nobistb0.txt",
    "/afs/cern.ch/work/q/qyan/work/alignment/misalign3ex19calcons2fixsenpsigcshear5_nobistb3MC0.txt",
#else
    "/afs/cern.ch/work/q/qyan/work/alignment/misaligninner2.txt",
#endif
//------
#if defined (USESENSORA)
#if defined (USETESTBEAMA)
    "/afs/cern.ch/work/q/qyan/work/alignment/after/merge3itersmallerrext_after2inv2fixalpha.res",
    "/afs/cern.ch/work/q/qyan/work/alignment/after/merge3itersmallerrext_after2inv2.res",
    "/afs/cern.ch/work/q/qyan/work/alignment/after/merge3itersmallerrext_after2inv2fixalphaloose.res",
    "/afs/cern.ch/work/q/qyan/work/alignment/after/merge3itersmallerrext_after2inv2loose.res",
/*    "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayerladsennoweightnomstest/noiternorme/hugecut/errsmall/3iter/nnewcon3dataext/3itershearpresigfixsenfixoptloose/after/after2/inv2oladfixalpha/millepede.res",
    "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayerladsennoweightnomstest/noiternorme/hugecut/errsmall/3iter/nnewcon3dataext/3itershearpresigfixsenfixoptloose/after/after2/inv2olad/millepede.res",*/
#else
    "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2test/fastsennoweight/millepede.res",
//    "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2test/fastsenweight/millepede.res",
//    "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2test/fastsennoweighttest/millepede.res",
//    "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayerladsennoweightnomstest/millepede.res",
//    "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayerladsennoweightnomstest/noiter/millepede.res",
//    "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayerladsennoweightnomstest/noiter/newmc/millepede.res",
//    "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayerladsennoweightnomstest/noiter/nopresig/millepede.res",
//    "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayerladsennoweightnomstest/noiternorme/minres1iter/millepede.res",
    "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayerladsennoweightnomstest/noiternorme/hugecut/millepede.res",
    "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayerladsennoweightnomstest/noiternorme/hugecut/errsmall/millepede.res",
//    "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayerladsennoweightnomstest/noiternorme/hugecut/errsmall/2iter/millepede.res",
    "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayerladsennoweightnomstest/noiternorme/hugecut/errsmall/3iter/millepede.res",
//    "merge3itersmallerr_after2.res",
    "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayerladsennoweightnomstest/noiternorme/hugecut/errsmall/3iter/newcons/millepede.res",
    "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayerladsennoweightnomstest/noiternorme/hugecut/errsmall/3iter/newcons/testrtol/millepede.res",
    "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayerladsennoweightnomstest/noiternorme/hugecut/errsmall/3iter/newcons2/millepede.res",
    "merge2iterpede.res",
    "merge3itersmallerr.res",
    "merge2itersmallerr.res",
//    "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayerladsennoweightnomstest/noiternorme/hugecut2/millepede.res",
//    "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayerladsennoweightnomstest/noiternorme/hugecutdownweight/millepede.res",
//    "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayerladsennoweightnomstest/noiternorme/hugecutdownchis/test/millepede.res",
/*    "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayerladsennoweightnomstest/noiternorme/hugecutdownweight/2iter/millepede.res",
//    "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayerladsennoweightnomstest/noiternorme/hugecutdownweight/2itertest/millepede.res",
    "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayerladsennoweightnomstest/noiternorme/minres2iter/millepede.res",
    "./merge2iterpede.res",
    "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayerladsennoweightnomstest/noiternorme/minres3iter/millepede.res",*/
//    "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayerladsennoweightnomstest/noiternorme/minres2align/millepede.res",
//    "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayerladsennoweightnomstest/noiteroldcon/millepede.res",
//    "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayerladsennoweightnomstest/noiternorme/millepede.res",
//    "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayerladsennoweightnomstest/noiter/downweight/millepede.res",
#endif
//---------ladder
#elif defined (USELADDERA)
#ifdef USETESTBEAMA
#ifdef USEMC
/*   "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayerladsennoweightnomstest/noiternorme/hugecut/errsmall/3iter/nnewcon3mc/1iterpresigfixsenfixoptloose/millepede.res",
   "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayerladsennoweightnomstest/noiternorme/hugecut/errsmall/3iter/nnewcon3mc/3iterpresigfixsenfixoptloose/millepede.res",*/
    "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayerladsennoweightnomstest/noiternorme/hugecut/errsmall/3iter/nnewcon3mc/1itershearpresigfixsenfixoptloose/millepede.res",
//    "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayerladsennoweightnomstest/noiternorme/hugecut/errsmall/3iter/nnewcon3mc/3itershearpresigfixsenfixoptloose/millepede.res",
//    "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayerladsennoweightnomstest/noiternorme/hugecut/errsmall/3iter/nnewcon3mcext/1itershearpresigfixsenfixoptloose/millepede.res",
//    "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayerladsennoweightnomstest/noiternorme/hugecut/errsmall/3iter/nnewcon3mcext/3itershearpresigfixsenfixoptloose/millepede.res",
//    "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayerladsennoweightnomstest/noiternorme/hugecut/errsmall/3iter/nnewcon3mcext/3itershearpresigfixsenfixoptloose/inv/millepede.res",
//    "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayerladsennoweightnomstest/noiternorme/hugecut/errsmall/3iter/nnewcon3mcext/3itershearpresigfixsenfixoptloose/inv2fixalpha/millepede.res",
    "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayerladsennoweightnomstest/noiternorme/hugecut/errsmall/3iter/nnewcon3mcext/3itershearpresigfixsenfixoptloose/inv2/millepede.res",
    "/afs/cern.ch/work/q/qyan/work/alignment/after/merge3itersmallerrext_after2inv2oladfixalphaMC.res",
    "/afs/cern.ch/work/q/qyan/work/alignment/after/merge3itersmallerrext_after2inv2oladMC.res",
#else
/*    "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayerladsennoweightnomstest/noiternorme/hugecut/errsmall/3iter/nnewcon3data/1iterpresigfixsenfixoptloose/millepede.res",
    "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayerladsennoweightnomstest/noiternorme/hugecut/errsmall/3iter/nnewcon3data/3iterpresIigfixsenfixoptloose/millepede.res", */
//    "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayerladsennoweightnomstest/noiternorme/hugecut/errsmall/3iter/nnewcon3data/1itershearpresigfixsenfixoptloose/millepede.res",
//    "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayerladsennoweightnomstest/noiternorme/hugecut/errsmall/3iter/nnewcon3data/3itershearpresigfixsenfixoptloose/millepede.res",
//    "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayerladsennoweightnomstest/noiternorme/hugecut/errsmall/3iter/nnewcon3dataext/1itershearpresigfixsenfixoptloose/millepede.res",
//    "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayerladsennoweightnomstest/noiternorme/hugecut/errsmall/3iter/nnewcon3dataext/3itershearpresigfixsenfixoptloose/millepede.res",
/*    "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayerladsennoweightnomstest/noiternorme/hugecut/errsmall/3iter/nnewcon3dataext/3itershearpresigfixsenfixoptloose/inv2fixalpha/bac/millepede.res",
    "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayerladsennoweightnomstest/noiternorme/hugecut/errsmall/3iter/nnewcon3dataext/3itershearpresigfixsenfixoptloose/inv2/bac/millepede.res",*/
/*    "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayerladsennoweightnomstest/noiternorme/hugecut/errsmall/3iter/nnewcon3dataext/3itershearpresigfixsenfixoptloose/inv2fixalpha/millepede.res",
    "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayerladsennoweightnomstest/noiternorme/hugecut/errsmall/3iter/nnewcon3dataext/3itershearpresigfixsenfixoptloose/inv2/millepede.res",*/
    "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayerladsennoweightnomstest/noiternorme/hugecut/errsmall/3iter/nnewcon3dataext/3itershearpresigfixsenfixoptloose/inv2fixalpha/loose/millepede.res",
    "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayerladsennoweightnomstest/noiternorme/hugecut/errsmall/3iter/nnewcon3dataext/3itershearpresigfixsenfixoptloose/inv2/loose/millepede.res",
//    "/afs/cern.ch/work/q/qyan/work/alignment/after/merge3itersmallerrext_after2inv2fixalpha.res",
//    "/afs/cern.ch/work/q/qyan/work/alignment/after/merge3itersmallerrext_after2inv2fixalphaloose.res",
//    "/afs/cern.ch/work/q/qyan/work/alignment/after/merge3itersmallerrext_after2inv2.res",
//    "/afs/cern.ch/work/q/qyan/work/alignment/after/merge3itersmallerrext_after2inv2loose.res",
//    "/afs/cern.ch/work/q/qyan/work/alignment/after/merge3itersmallerrext_after2inv2oladfixalpha.res",
/*    "/afs/cern.ch/work/q/qyan/work/alignment/after/merge3itersmallerrext_after2inv2olad.res",
    "/afs/cern.ch/work/q/qyan/work/alignment/after/merge3itersmallerrext_after2inv2oladloose.res",*/
    "/afs/cern.ch/work/q/qyan/work/alignment/after/merge3itersmallerrext_after2inv2oladfixalpha.res",
    "/afs/cern.ch/work/q/qyan/work/alignment/after/merge3itersmallerrext_after2inv2oladfixalphaloose.res",
//    "/afs/cern.ch/work/q/qyan/work/alignment/after/merge3itersmallerrext_after2inv2olad.res",
#endif
//------
#else
//    "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2test/fastladnoweight/millepede.res",
//    "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2test/fastladweight/millepede.res",
    "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2test/fastladnoweighttest/millepede.res",
//    "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayerladnoweight/millepede.res",
//    "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayerladnoweightnomstest/millepede.res",
//    "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayerladnoweightnomstest/noiter/millepede.res",
//    "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayerladnoweightnomstest/noiteroldcon/millepede.res",
//    "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayerladsennoweightnomstest/noiter/millepede.res",
//    "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayerladsennoweightnomstest/noiter/newmc/millepede.res",
//    "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayerladsennoweightnomstest/noiternorme/millepede.res",
//    "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayerladsennoweightnomstest/noiternorme/minres1iter/millepede.res",
    "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayerladsennoweightnomstest/noiternorme/hugecut/millepede.res",
    "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayerladsennoweightnomstest/noiternorme/hugecut/errsmall/millepede.res",
//    "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayerladsennoweightnomstest/noiternorme/hugecut/errsmall/2iter/millepede.res",
    "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayerladsennoweightnomstest/noiternorme/hugecut/errsmall/3iter/millepede.res",
//    "merge3itersmallerr_after2.res",
    "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayerladsennoweightnomstest/noiternorme/hugecut/errsmall/3iter/newcons/millepede.res",
    "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayerladsennoweightnomstest/noiternorme/hugecut/errsmall/3iter/newcons/testrtol/millepede.res",
    "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayerladsennoweightnomstest/noiternorme/hugecut/errsmall/3iter/newcons2/millepede.res",
//     "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayerladsennoweightnomstest/noiternorme/hugecut/errsmall/3iter/newcon3data/1iterpresigfixsenloose/millepede.res",
    "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayerladsennoweightnomstest/noiternorme/hugecut/errsmall/3iter/newcon3data/1itershearfixsenfixladloose/millepede.res",
    "merge2iterpede.res",
    "merge3itersmallerr.res",
    "merge2itersmallerr.res",
//    "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayerladsennoweightnomstest/noiternorme/hugecut2/millepede.res",
//    "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayerladsennoweightnomstest/noiternorme/hugecutdownweight/millepede.res",
//    "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayerladsennoweightnomstest/noiternorme/hugecutdownchis/test/millepede.res",
//    "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayerladsennoweightnomstest/noiternorme/hugecutdownweight/2iter/millepede.res",
//    "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayerladsennoweightnomstest/noiternorme/hugecutdownweight/2itertest/millepede.res",
    "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayerladsennoweightnomstest/noiternorme/minres2iter/millepede.res",
    "./merge2iterpede.res",
    "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayerladsennoweightnomstest/noiternorme/minres3iter/millepede.res",
//    "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayerladsennoweightnomstest/noiternorme/minres2align/millepede.res",
#endif
//---------layer
#else
#ifdef USETESTBEAMA
#ifdef USEMC
/*   "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayerladsennoweightnomstest/noiternorme/hugecut/errsmall/3iter/nnewcon3mc/1iterpresigfixsenfixoptloose/millepede.res",
   "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayerladsennoweightnomstest/noiternorme/hugecut/errsmall/3iter/nnewcon3mc/3iterpresigfixsenfixoptloose/millepede.res",*/
/*    "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayerladsennoweightnomstest/noiternorme/hugecut/errsmall/3iter/nnewcon3mc/1itershearpresigfixsenfixoptloose/millepede.res",
    "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayerladsennoweightnomstest/noiternorme/hugecut/errsmall/3iter/nnewcon3mc/3itershearpresigfixsenfixoptloose/millepede.res",*/
    "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayerladsennoweightnomstest/noiternorme/hugecut/errsmall/3iter/nnewcon3mcext/1itershearpresigfixsenfixoptloose/millepede.res",
    "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayerladsennoweightnomstest/noiternorme/hugecut/errsmall/3iter/nnewcon3mcext/3itershearpresigfixsenfixoptloose/millepede.res",
    "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayerladsennoweightnomstest/noiternorme/hugecut/errsmall/3iter/nnewcon3mcext/3itershearpresigfixsenfixoptloose/inv2fixalpha/millepede.res",
    "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayerladsennoweightnomstest/noiternorme/hugecut/errsmall/3iter/nnewcon3mcext/3itershearpresigfixsenfixoptloose/inv2/millepede.res",
#else
/*    "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayerladsennoweightnomstest/noiternorme/hugecut/errsmall/3iter/nnewcon3data/1iterpresigfixsenfixoptloose/millepede.res",
    "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayerladsennoweightnomstest/noiternorme/hugecut/errsmall/3iter/nnewcon3data/3iterpresigfixsenfixoptloose/millepede.res",*/
//    "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayerladsennoweightnomstest/noiternorme/hugecut/errsmall/3iter/nnewcon3data/1itershearpresigfixsenfixoptloose/millepede.res",
//    "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayerladsennoweightnomstest/noiternorme/hugecut/errsmall/3iter/nnewcon3data/3itershearpresigfixsenfixoptloose/millepede.res",
/*    "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayerladsennoweightnomstest/noiternorme/hugecut/errsmall/3iter/nnewcon3dataext/1itershearpresigfixsenfixoptloose/millepede.res",
    "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayerladsennoweightnomstest/noiternorme/hugecut/errsmall/3iter/nnewcon3dataext/3itershearpresigfixsenfixoptloose/millepede.res",*/
    "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayerladsennoweightnomstest/noiternorme/hugecut/errsmall/3iter/nnewcon3dataext/3itershearpresigfixsenfixoptloose/inv2fixalpha/bac/millepede.res",
    "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayerladsennoweightnomstest/noiternorme/hugecut/errsmall/3iter/nnewcon3dataext/3itershearpresigfixsenfixoptloose/inv2/bac/millepede.res",
    "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayerladsennoweightnomstest/noiternorme/hugecut/errsmall/3iter/nnewcon3dataext/3itershearpresigfixsenfixoptloose/inv2fixalpha/millepede.res",
    "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayerladsennoweightnomstest/noiternorme/hugecut/errsmall/3iter/nnewcon3dataext/3itershearpresigfixsenfixoptloose/inv2/millepede.res",
#endif
//-------
#else
/*    "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/millepede.res",
    "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/millepede.res~",*/
//    "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayernoweight/millepede.res",
//    "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayernoweight_2iter/millepede.res",
//    "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayernoweight_berr/millepede.res",
//    "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayernoweight_2iterberr/millepede.res",
//    "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayernoweightnoms/millepede.res",
    "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayernoweightnoms_berr/millepede.res",
//    "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayerladnoweightnomstest/millepede.res",
//    "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayerladnoweightnomstest/noiter/millepede.res",
//    "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayerladsennoweightnomstest/noiter/millepede.res",
//    "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayerladsennoweightnomstest/noiternorme/minres1iter/millepede.res",
    "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayerladsennoweightnomstest/noiternorme/hugecut/millepede.res",
    "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayerladsennoweightnomstest/noiternorme/hugecut/errsmall/millepede.res",
//    "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayerladsennoweightnomstest/noiternorme/hugecut/errsmall/2iter/millepede.res",
    "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayerladsennoweightnomstest/noiternorme/hugecut/errsmall/3iter/millepede.res",
//    "merge3itersmallerr_after2.res",
    "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayerladsennoweightnomstest/noiternorme/hugecut/errsmall/3iter/newcons/millepede.res",
    "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayerladsennoweightnomstest/noiternorme/hugecut/errsmall/3iter/newcons/testrtol/millepede.res",
    "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayerladsennoweightnomstest/noiternorme/hugecut/errsmall/3iter/newcons2/millepede.res",
    "merge2iterpede.res",
    "merge3itersmallerr.res",
    "merge2itersmallerr.res",
//    "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayerladsennoweightnomstest/noiternorme/hugecut2/millepede.res",
//    "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayerladsennoweightnomstest/noiternorme/hugecutdownweight/millepede.res",
    "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayerladsennoweightnomstest/noiternorme/minres2iter/millepede.res",
    "./merge2iterpede.res",
    "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayerladsennoweightnomstest/noiternorme/minres3iter/millepede.res",
//    "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayerladsennoweightnomstest/noiternorme/minres2align/millepede.res",
//    "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayernoweightnoms_2iterberr/millepede.res",
/*    "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayernoweighttest/millepede.res",
    "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayerweight/millepede.res",
    "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayerweight2/millepede.res",*/
//    "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2test/fastlayernoweight/millepede.res",
//    "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2test/fastlayerweight/millepede.res",
#endif
//-----
#endif
  };
  const char *lengn[]={
#ifdef USETESTBEAMA
     "raw",
/*     "chis<500",
     "chis<100",
     "chis<500-noshear",
     "chis<500-prenoshear",
     "chis<500-preshear-opt",*/
/*     "chis<500-1iter-nosh",
     "chis<500-3iter-nosh",*/
     "chis<500-1iter-sh",
     "chis<500-3iter-sh",
     "chis<500-1iter-exsh",
     "chis<500-3iter-exsh",
#else
    "raw",
    "c",
//    "msc",
    "c-berr-qy",
    "c-se-1",
    "c-se-2",
    "c-se-3",
    "c-s2al",
#endif
/*    "no-downweight",
    "downweight",
    "downweight-lonly",*/
  };
  map<int,pair<double,double> >fpar[nfl];
  const int npar=6;
  TH1D *h[nfl][npar+2]={0};
  TH1D *hc[nfl][npar+2]={0};
  const int ng=2;
  double xran[ng][2]={
#if defined (USESENSORA)
    {-200,200},
    {-3,3},
#else
    {-1500,1500},
    {-3,3},
#endif
  };
  char histn[100];
  for(int ifl=0;ifl<nfl;ifl++){
    for(int ipar=0;ipar<npar+2;ipar++){
      sprintf(histn,"f%dpar%d",ifl,ipar);
#ifdef USERATIO
      h[ifl][ipar]=new TH1D(histn,histn,30,0.,3);//um
      h[ifl][ipar]->GetXaxis()->SetTitle("cor/smear");
#else
      if(ipar<3||ipar==6){
//        h[ifl][ipar]=new TH1D(histn,histn,30,xran[0][0],xran[0][1]);//um
        h[ifl][ipar]=new TH1D(histn,histn,200,xran[0][0],xran[0][1]);//um
        h[ifl][ipar]->GetXaxis()->SetTitle("Shift [#mum]");
      }
      else {
//        h[ifl][ipar]=new TH1D(histn,histn,50,xran[1][0],xran[1][1]);//mrad
        h[ifl][ipar]=new TH1D(histn,histn,200,xran[1][0],xran[1][1]);//mrad
        h[ifl][ipar]->GetXaxis()->SetTitle("Rotation [mrad]");
      }
#endif
      h[ifl][ipar]->GetXaxis()->SetTitleColor(1);
      sprintf(histn,"f%dpar%dpresigma",ifl,ipar);
      hc[ifl][ipar]=(TH1D *)h[ifl][ipar]->Clone();
      hc[ifl][ipar]->SetTitle(histn);
    }
    ifstream fin(file[ifl]);
    cout<<"ifl="<<ifl<<" fname="<<file[ifl]<<endl;
    string s;
    while(std::getline(fin,s)){
      istringstream iss(s);
      int label;
      double var=0,fix=0;
      if(!(iss>>label>>var))continue;
      fpar[ifl][label]=make_pair(var,fix);
      if(iss>>fix){
        fpar[ifl][label].second=fix;
      }
    } 
  }
 
  double maxd[nfl][npar]={0};
  int maxl[nfl][npar]={0};
  double secd[nfl][npar]={0};
  int secl[nfl][npar]={0};
  int ifl0[npar]={0};
  for(int ifl=1;ifl<nfl;ifl++){
    for(std::map<int,pair<double,double> >::iterator it=fpar[ifl].begin();it!=fpar[ifl].end();it++){
//      if((it->second).second==-1)continue;
      if((it->second).second==-1||(it->second).first==0)continue;
#if defined (USESENSORA)
      if(!(it->first/1000%1000>=100))continue; 
//      if(!(it->first/100000%10==8))continue;//L1
//      if(!(it->first/100000%10==9))continue;//L9
#elif defined (USELADDERA)
      if(!(it->first/1000%1000<100&&it->first/10%1000>=100))continue;
//      if(!(it->first/1000%10==8))continue;//L1
//      if(!(it->first/1000%10==9))continue;//L9
#else
      if(!(it->first<=110))continue; 
#endif
      double ref=fpar[0][it->first].first;
#ifdef USETESTBEAMA
#ifndef USEMC
      ref=0;
#endif
#endif
//     cout<<"ref="<<ref<<endl;
      double var=fpar[ifl][it->first].first;
      int ipar=it->first%10-1;
      int ig=(ipar<3)?0:1;
#ifdef USERATIO
      h[ifl][ipar]->Fill(var/ref);
      h[ifl][6+ig]->Fill(var/ref);
#else
      double scale=(ig==0)?1e4:1e3;
      double ww=1;
//      ww+=ifl*2e-2;
      h[ifl][ipar]->Fill((var-ref)*scale,ww);
      h[ifl][6+ig]->Fill((var-ref)*scale,ww);
      hc[ifl][ipar]->Fill(var*scale);
      hc[ifl][6+ig]->Fill(var*scale);
      if(fabs(var-ref)*scale>maxd[ifl][ipar]){maxd[ifl][ipar]=fabs(var-ref)*scale;maxl[ifl][ipar]=it->first;}
//      if(ifl==1){
      if(ifl0[ipar]==0)ifl0[ipar]=ifl; 
      if(ifl==ifl0[ipar]){
        ww=1;
        h[0][ipar]->Fill(ref*scale,ww);
        h[0][6+ig]->Fill(ref*scale,ww);
        hc[0][ipar]->Fill(ref*scale);
        hc[0][6+ig]->Fill(ref*scale);
        if(fabs(ref)*scale>maxd[0][ipar]){maxd[0][ipar]=fabs(ref)*scale;maxl[0][ipar]=it->first;}
      }
#endif
    }    
  }
 
//-----
//  set_frame_style();
  int ucol[]={1,2,8,4,6,7,kOrange,3};
  TCanvas *c=0;
  TF1 *fun=new TF1("gaus","gaus",-100,100);
  for(int ic=0;ic<2;ic++){
    int ipad=0;
    for(int ipar=0;ipar<npar+2;ipar++){
      if     (ipar==0)   {c=new TCanvas(); c->Divide(2,3); ipad=0;}
      else if(ipar>=npar){c=new TCanvas(); ipad=0;}
      c->cd(ipar+1);
      TLegend *leng=new TLegend(0.19,0.65,0.45,0.85);
      leng->SetBorderSize(0);
      leng->SetFillColor(0);
      double ymax=0;
      TH1D *h0=0;
      int ip=0;
      for(int ifl=0;ifl<nfl;ifl++){
#ifdef USETESTBEAMA
#ifndef USEMC
        if(ifl==0)continue;
#endif
#endif
        TH1D *hn=h[ifl][ipar];
        if(ic==1)hn=hc[ifl][ipar];
        if(hn==0)continue;
        hn->SetLineColor(ucol[ifl]);
        hn->SetLineWidth(2);
//        set_frame_style(hn);
        if(ip==0)hn->Draw();
        else     hn->Draw("sames");
//-----
        double mean=0,rms=0;
/*        fun->SetLineColor(ucol[ifl]);
        hn->Fit(fun,"L","sames");
        mean=fun->GetParameter(1);
        rms=fun->GetParameter(2);
        hn->Fit(fun,"L","sames",mean-2.*rms,mean+2.*rms);
        mean=fun->GetParameter(1);
        rms=fun->GetParameter(2);
        hn->Fit(fun,"L","sames",mean-1.8*rms,mean+1.8*rms);
        mean=fun->GetParameter(1);
        rms=fun->GetParameter(2);*/
//------
        hn->GetYaxis()->SetTitle("Entries"); 
        leng->AddEntry(hn,lengn[ifl],"L");
        if(h0==0)h0=hn;
        double yn=hn->GetMaximum();
        if(yn>ymax){ymax=yn;h0->GetYaxis()->SetRangeUser(0.01,ymax*1.2);}
        if(ipar<npar&&ic==0)cout<<"maxshif ipar="<<ipar<<" ifl="<<ifl<<" lengn="<<lengn[ifl]<<" shift="<<maxd[ifl][ipar]<<" label="<<maxl[ifl][ipar]<<" hmean="<<hn->GetMean()<<" hrms="<<hn->GetRMS()<<endl;
        if(ipar>=npar||ic>=1){
          if(ic==1)cout<<"presigic="<<ic<<" ipar="<<ipar<<" ifl="<<ifl<<" lengn="<<lengn[ifl]<<" mean="<<hn->GetMean()<<" rms="<<hn->GetRMS()<<","<<rms<<endl;
          else     cout<<"ic="<<ic<<" ipar="<<ipar<<" ifl="<<ifl<<" lengn="<<lengn[ifl]<<" mean="<<hn->GetMean()<<" rms="<<hn->GetRMS()<<","<<rms<<endl;
        }
        ip++;
      }
      leng->Draw("same");
    }
  }
  
  return 0;
}

int MergeResult(){

  const int nf=2;
  const char *filen[]={
//    "merge2iterpede.res",
//    "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayerladsennoweightnomstest/noiternorme/hugecut/3iter/millepede.res",
/*    "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayerladsennoweightnomstest/noiternorme/hugecut/errsmall/3iter/newcons/millepede.res",*/
/*    "./refine3itersmallernewcons.res",
    "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayerladsennoweightnomstest/noiternorme/hugecut/errsmall/3iter/newcons/after/millepede.res",*/
//    "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayerladsennoweightnomstest/noiternorme/hugecut/errsmall/3iter/newcons/newgeom/millepede.res",
#ifdef USEMC
     "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayerladsennoweightnomstest/noiternorme/hugecut/errsmall/3iter/nnewcon3mcext/3itershearpresigfixsenfixoptloose/millepede.res",
     "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayerladsennoweightnomstest/noiternorme/hugecut/errsmall/3iter/nnewcon3mcext/3itershearpresigfixsenfixoptloose/after/millepede.res",
#else
     "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayerladsennoweightnomstest/noiternorme/hugecut/errsmall/3iter/nnewcon3dataext/3itershearpresigfixsenfixoptloose/millepede.res",
      "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayerladsennoweightnomstest/noiternorme/hugecut/errsmall/3iter/nnewcon3dataext/3itershearpresigfixsenfixoptloose/after/millepede.res",
#endif
//    "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayerladsennoweightnomstest/noiternorme/hugecut/errsmall/3iter/afternegiter3/millepede.res",
//    "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayerladsennoweightnomstest/noiternorme/hugecut/2iter/millepede.res",
//    "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayerladsennoweightnomstest/noiternorme/hugecut/2iterhugecut2/millepede.res",
//    "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayerladsennoweightnomstest/noiternorme/hugecut/2iterpresig2/millepede.res",
//    "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayerladsennoweightnomstest/noiternorme/hugecut/2iterhuge2presig2/millepede.res",
//    "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayerladsennoweightnomstest/noiternorme/hugecut/2iterhugecut3/millepede.res"
  };
//  const char *ofilen="merge2iterpede.res";
//  const char *ofilen="merge2iterhugec2pede.res";
//  const char *ofilen="merge2iterpres2pede.res";
//  const char *ofilen="merge2iterc2p2pede.res";
//  const char *ofilen="merge2itersmallerr.res";
//  const char *ofilen="merge3itersmallerr.res";
//  const char *ofilen="merge3itersmallerr.res";
#ifdef USEMC
  const char *ofilen="merge3itersmallerrext_after2MC.res";
#else
  const char *ofilen="merge3itersmallerrext_after2.res";
#endif
//  const char *ofilen="merge2iterhugec3pede.res";
//-----
  std::map<int,double> fitpar[nf];
  for(int ifl=0;ifl<nf;ifl++){
    cout<<"ifl="<<ifl<<" filen="<<filen[ifl]<<endl;
    ifstream ffit(filen[ifl]);
    string s;
    while(std::getline(ffit,s)){
      istringstream iss(s);
      int label;
      double var=0;
      if(!(iss>>label>>var))continue;
      fitpar[ifl][label]=var;
    }
    ffit.close();
  }

//--add parmater together
  ofstream ofile(ofilen);
  for(map<int, double>::iterator it=fitpar[0].begin();it!=fitpar[0].end();it++){  
    int label=it->first;
    double var=it->second;
    for(int ifl=1;ifl<nf;ifl++){
      if(fitpar[ifl].find(label)==fitpar[ifl].end()){
        cerr<<"label="<<label<<" not exist in file="<<ifl<<endl;
        continue;
      }
      var+=fitpar[ifl][label];
    }
    ofile<<label<<" "<<var<<endl;
  }
  ofile.close();
  return 0;
}


int RefineResult(){
#define USETESTBEAMR
//  const char *filen="/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayerladsennoweightnomstest/noiternorme/hugecut/3iter/millepede.res";
//  const char *filen="/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayerladsennoweightnomstest/noiternorme/hugecut/errsmall/3iter/millepede.res";
//  const char *filen="/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayerladsennoweightnomstest/noiternorme/hugecut/errsmall/3iter/newcons/millepede.res";
//  const char *filen="/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayerladsennoweightnomstest/noiternorme/hugecut/errsmall/3iter/newcon3data/1itershearpresigfixsenfixoptloose/millepede.res";
//  const char *filen="/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayerladsennoweightnomstest/noiternorme/hugecut/errsmall/3iter/nnewcon3dataext/3itershearpresigfixsenfixoptloose/inv2/millepede.res";
   const char *filen="/afs/cern.ch/work/q/qyan/AMSVDEV/AMS317DUMQ/MillepedeII/iccbin2/fastlayerladsennoweightnomstest/noiternorme/hugecut/errsmall/3iter/nnewcon3dataext/3itershearpresigfixsenfixoptloose/inv2/millepede.res";
//   const char *filen="/afs/cern.ch/work/q/qyan/work/alignment/after/merge3itersmallerrext_after2inv2.res";
//   const char *filen="/afs/cern.ch/work/q/qyan/work/alignment/after/merge3itersmallerrext_after2inv2loose.res";
//   const char *filen="/afs/cern.ch/work/q/qyan/work/alignment/after/merge3itersmallerrext_after2inv2olad.res";
//  const char *ofilen="refine3itersmaller.res";
//  const char *ofilen="refine3itersmallernewcons.res";
  const char *ofilen="refine3itertest.res";
//-----
   double presig[3][6]={
#ifdef USETESTBEAMR
    {360e-4, 200e-4,200e-4,0.25e-3,0.25e-3,0.12e-3},//layer  u,v,w,alpha,beta,gamma
    {180e-4, 130e-4,85e-4, 0.36e-3,0.38e-3,0.22e-3},//ladder u,v,w,alpha,beta,gamma
    {5e-4,   5e-4,   5e-4, 0.2e-3,  0.2e-3, 0.2e-3},//sensor u,v,w,alpha,beta,gamma
#else
    {-1000e-4,-1000e-4,-1000e-4,-1e-3,-1e-3,-1e-3},//layer  u,v,w,alpha,beta,gamma
    {  300e-4, 300e-4,   300e-4, 1e-3, 1e-3, 1e-3},//ladder u,v,w,alpha,beta,gamma
    {   50e-4,  50e-4,    50e-4, 1e-3, 1e-3, 1e-3},//sensor u,v,w,alpha,beta,gamma
#endif
   };
  std::map<int,double> fitpar;
  cout<<"filen="<<filen<<endl;
  cout<<"ofilen="<<ofilen<<endl;
  ifstream ffit(filen);
  string s;
  while(std::getline(ffit,s)){
    istringstream iss(s);
    int label;
    double var=0;
    if(!(iss>>label>>var))continue;
    fitpar[label]=var;
  }
  ffit.close();

//----
  ofstream ofile(ofilen);
  for(map<int, double>::iterator it=fitpar.begin();it!=fitpar.end();it++){
    int label=it->first;
    double var=it->second;
    double rvar=var;
    int ilayladsen=-1;
    if     (label/1000%1000>=100)ilayladsen=2;//sensor
    else if(label<=110)ilayladsen=0;//layer
    else               ilayladsen=1;//ladder
    int ebit=0;
    double lsig=3;
    double limit=lsig*presig[ilayladsen][label%10-1];
    if     (var> fabs(limit)){if(limit>0)var= fabs(limit);ebit=limit>0?1:-1;}
    else if(var<-fabs(limit)){if(limit>0)var=-fabs(limit);ebit=limit>0?1:-1;}
    if(label%10==4){
     if     (ebit>0)cerr<<"label="<<label<<" ilayladsen="<<ilayladsen<<" err-huge="<<rvar<<" limit="<<limit<<" nsig="<<(lsig*rvar/fabs(limit))<<endl;
     else if(ebit<0)cerr<<"label="<<label<<" ilayladsen="<<ilayladsen<<" war-huge="<<rvar<<" limit="<<limit<<" nsig="<<(lsig*rvar/fabs(limit))<<endl;
    }
    ofile<<label<<" "<<var<<endl;
  }
  ofile.close();
  return 0;


}


int DrawCompare(){
  const int nfl=2;
#define USEAFTER2
#ifdef USEAFTER2
  const char *filen[]={
/*     "/eos/ams/group/mit/qyan/Data/alignment_BTB1130_PR400N0AV3knoms_nnnafter/alignment_BTB1130_PR400N0AV3knoms_nnnafter/1281355854_3000file.root",
      "/eos/ams/group/mit/qyan/Data/alignment_BTB1130_PR400N0AV3knoms_nnnafter2/alignment_BTB1130_PR400N0AV3knoms_nnnafter2/1281355854_3000file.root",*/
    "/eos/ams/group/mit/qyan/Data/alignment_PrMCB1221_400testaN1TBm_nosmear2/alignment_PrMCB1221_400testaN1TBm_nosmear2.root",
    "/eos/ams/group/mit/qyan/Data/alignment_PrMCB1221_400testaN1knomsTB_nnnafter2/alignment_PrMCB1221_400testaN1knomsTB_nnnafter2/1679323491_3000file.root",
   };
   double scalev[nfl]={1.,1};
   const char *lengn[nfl]={
/*     "1st",
       "1st+2nd",*/
     "No Displacement",
     "1st+2nd Align",
   };
#else
 const char *filen[]={
/*     "/eos/ams/group/mit/qyan/Data/alignment_BTB1130_PR400N0AV3knoms/alignment_BTB1130_PR400N0AV3knoms/1281355854_50file.root",
       "/eos/ams/group/mit/qyan/Data/alignment_BTB1130_PR400N0AV3knoms_nnnafter/alignment_BTB1130_PR400N0AV3knoms_nnnafter/1281355854_3000file.root",*/
    "/eos/ams/group/mit/qyan/Data/alignment_PrMCB1221_400testaN1knomsTB/alignment_PrMCB1221_400testaN1knomsTB/1679323491_3000file.root",
    "/eos/ams/group/mit/qyan/Data/alignment_PrMCB1221_400testaN1knomsTB_nnnafter/alignment_PrMCB1221_400testaN1knomsTB_nnnafter/1679323491_3000file.root",
   };
   double scalev[nfl]={1./10.,1};
   const char *lengn[nfl]={
     "Before",
     "After 1st",
   };
#endif
   const int nl=9;
   const int ns=2;
   TH1D *h1r[nfl][nl][ns]={{0}};
   const int nh2=2;
   TH2D *h2r[nfl][nh2][ns]={0};
   TProfile *h2p[nfl][nh2][ns]={0};
   TH1D *h2pl[nfl][nh2][ns][9]={0};
   char histn[1000];
   for(int ifl=0;ifl<nfl;ifl++){
     TFile *f=new TFile(filen[ifl]);
     f->cd();
     for(int ixy=0;ixy<ns;ixy++){
       for(int il=0;il<nl;il++){
         sprintf(histn,"res_z1l%dxy%d",il,ixy);
         TH1D *hn=(TH1D *)f->Get(histn);
         if(!hn)continue;
         hn->Scale(scalev[ifl]);
         hn->GetXaxis()->SetTitle("Residual [#mum]");
         hn->GetYaxis()->SetTitle("Entries");
         h1r[ifl][il][ixy]=hn;
       }
       for(int ih=0;ih<nh2;ih++){
         if(ih==0)sprintf(histn,"senres_xy%d",ixy);
         else     sprintf(histn,"ladres_xy%d",ixy);
         TH2D *h2=(TH2D *)f->Get(histn);
         if(!h2)continue;
         h2->GetXaxis()->SetTitle("ID");
         h2->GetYaxis()->SetTitle("Residual [#mum]");
//         if(ih==0)h2->RebinX(20);
         h2r[ifl][ih][ixy]=h2;
       }
     }
   }

//--1d
//  int ucol[]={1,2,4,8,7,6,5}; 
  int ucol[]={4,2,1,8,7,6,5};
//  set_frame_style();
  TCanvas *c=0;
#ifdef USEAFTER2
  gStyle->SetOptStat(1110);
#else
  gStyle->SetOptStat(10);
#endif
  for(int ixy=0;ixy<ns;ixy++){
     c=new TCanvas();
     c->Divide(3,3);
     for(int il=0;il<nl;il++){
       c->cd(il+1);
       int ip=0;
       TLegend *leng=new TLegend(0.19,0.65,0.45,0.85);
       leng->SetBorderSize(0);
       leng->SetFillColor(0);
       TH1D *h0=0;
       double ymax=0;
       for(int ifl=0;ifl<nfl;ifl++){
#ifdef USEAFTER2
//         if(ifl==0)continue;
#endif
         TH1D *hn=h1r[ifl][il][ixy];
         if(!hn)continue;
         hn->SetLineColor(ucol[ifl]);
//         set_frame_style(hn);
         hn->SetTitle(0);
         if(ip==0)hn->Draw();
         else     hn->Draw("sames");
         hn->SetLineWidth(2);
         hn->GetYaxis()->SetTitleOffset(1.4);
         if(h0==0){
           h0=hn;
#ifndef USEAFTER2
           h0->GetXaxis()->SetRangeUser(-300,300);
#endif
         }
         double yn=hn->GetMaximum();
         if(yn>ymax){
           ymax=yn;
#ifdef USEAFTER2
           h0->GetYaxis()->SetRangeUser(0.01,ymax*1.2);
#else
           if(ixy==0)h0->GetYaxis()->SetRangeUser(0.1*ymax,ymax*1.2);
           else      h0->GetYaxis()->SetRangeUser(0.005*ymax,ymax*1.2);
#endif
         }
         leng->AddEntry(hn,lengn[ifl],"L");
         ip++;
       }
       leng->Draw("same");
     }
  }

//---2d
  for(int ih=0;ih<nh2;ih++){
    TH1D *ha[nfl][ns]={0};
    for(int ifl=0;ifl<nfl;ifl++){
      c=new TCanvas();
      c->Divide(1,2);
      for(int ixy=0;ixy<ns;ixy++){
        c->cd(ixy+1);
        gPad->SetLogz();
        TH2D *hn=h2r[ifl][ih][ixy];
        if(!hn)continue;
//        set_frame_style(hn);
        hn->Draw("COLZ");
        sprintf(histn,"%spa%dxy%d",hn->GetName(),ifl,ixy);
        TH1D *pa=(TH1D *)hn->ProjectionY(histn);
        pa->Reset();
        ha[ifl][ixy]=pa;
        sprintf(histn,"%spf%dxy%d",hn->GetName(),ifl,ixy);
        TProfile *pr=(TProfile *)hn->ProfileX(histn);
        pr->Draw("same");
        pr->SetMarkerStyle(20);
        pr->SetMarkerColor(1);
        for(int ibx=1;ibx<=hn->GetNbinsX();ibx++){
          double sum=hn->Integral(ibx,ibx,1,hn->GetNbinsY());
          if(sum<100){pr->SetBinContent(ibx,0);pr->SetBinError(ibx,0);}
          else        pa->Fill(pr->GetBinContent(ibx));
        }
        h2p[ifl][ih][ixy]=pr;
     }
   }
   c=new TCanvas();
   c->Divide(1,2);
   for(int ixy=0;ixy<ns;ixy++){
     c->cd(ixy+1);
     gPad->SetLogy();
     int ip=0;
     for(int ifl=0;ifl<nfl;ifl++){
       TH1D *hp=ha[ifl][ixy];
       if(hp==0)continue;
       hp->SetLineColor(ucol[ifl]);
       if(ip==0)hp->Draw();
       else     hp->Draw("sames");
       ip++;
     }
   }
 }


//-----
  for(int ih=0;ih<nh2;ih++){
    for(int ifl=0;ifl<nfl;ifl++){
      c=new TCanvas();
      c->Divide(2,1);
      for(int ixy=0;ixy<ns;ixy++){
        c->cd(ixy+1);
        TH2D *hn=h2r[ifl][ih][ixy];
        TProfile *pr=h2p[ifl][ih][ixy];
        if(!hn||!pr)continue;
        TH1D *hev[2]={0};
        for(int ib=0;ib<2;ib++){
          sprintf(histn,"%sb%dnev",pr->GetName(),ib);
          if(ih==0)hev[ib]=new TH1D(histn,histn,5000,0,100000);
          else     hev[ib]=new TH1D(histn,histn,100,0,300000);
        }
        for(int ibx=1;ibx<=pr->GetNbinsX();ibx++){
          double sum=hn->Integral(ibx,ibx,1,hn->GetNbinsY());
          if(sum<20)continue;
//-----
          double xv=pr->GetXaxis()->GetBinLowEdge(ibx);
          int tkid2=int(xv)/20;
          int senid2=int(xv)%20;
          int il=tkid2/20;
          if(tkid2>0)il++;
          else       il--;
          int tkid=il*100+tkid2%20;
          int label=tkid*100+senid2;
          if(label<0){label=-label; label+=1000000;}
//-----
          int ib=(fabs(pr->GetBinContent(ibx))<10)?0:1;
          TProfile *pr0=h2p[0][ih][ixy];
          if(ifl==1){
             if(fabs(pr->GetBinContent(ibx))>fabs(pr0->GetBinContent(ibx))+5)ib=1;//becoming worse
          }
          if(ifl==1&&ib==1&&ih==0){
//---
            int tkid2b=(tkid>0)?((tkid/100%100-1)*20+tkid%100):((tkid/100%100+1)*20+tkid%100);
            int senid2b=(tkid2b>=0)?tkid2b*20+abs(senid2)%100:tkid2*20-abs(senid2)%100;
            cout<<"xv="<<xv<<" senid2="<<senid2b<<endl;
//-----
            cout<<hn->GetName()<<" xv="<<xv<<" tkid="<<tkid<<" senid2="<<senid2<<" label="<<label<<" nev="<<sum<<" nevav="<<(hn->GetEntries()/2284.)<<" res="<<pr->GetBinContent(ibx)<<" raw="<<pr0->GetBinContent(ibx)<<endl;
          }
          hev[ib]->Fill(sum);
          TH1D *pal=h2pl[ifl][ih][ixy][abs(il)-1];
          if(pal==0){
            sprintf(histn,"%sl%d",pr->GetName(),abs(il));
            pal=new TH1D(histn,histn,100,-20,20);
            h2pl[ifl][ih][ixy][abs(il)-1]=pal;
          }
          pal->Fill(pr->GetBinContent(ibx));
        }
        TH1D *h0=hev[0];
        TH1D *h1=hev[1];
        if(h0->GetEntries()>1)h0=(TH1D*)h0->DrawNormalized();
        if(h1->GetEntries()>1)h1=(TH1D*)h1->DrawNormalized("sames");
        h0->SetLineColor(2);
        h1->SetLineColor(4);
      }
   }
 }


//-----
  for(int ih=0;ih<nh2;ih++){
   if(ih!=0)continue;
   for(int ixy=0;ixy<ns;ixy++){
     c=new TCanvas();
     c->Divide(3,3);
     for(int il=0;il<9;il++){
       c->cd(il+1);
       int ip=0;
       for(int ifl=0;ifl<nfl;ifl++){
         TH1D *hn=h2pl[ifl][ih][ixy][il];
         if(hn==0)continue;
         if(ip==0)hn->Draw();
         else     hn->Draw("sames");
         hn->SetLineColor(ucol[ifl]);
         ip++;
       }
     }
    }
  }

 return 0;
}


int CheckRig(){
//#define USENEGBETA
//------
#ifndef USENEGBETA
  const int nfl=3;
  const char *filen[]={
   "/eos/ams/group/mit/qyan/Data/alignment_BTB1130_PR400N0Allo_maxsppgrig.root",
   "/eos/ams/group/mit/qyan/Data/alignment_BTB1130_PR400N0Allo_maxspafter2rig.root",
   "/eos/ams/group/mit/qyan/Data/alignment_BTB1130_PR400N0Allo_maxsplooseafter2rig.root",
//   "/eos/ams/group/mit/qyan/Data/alignment_BTB1130_PR400Allo_maxspafter2nrig2.root",
//   "/eos/ams/group/mit/qyan/Data/alignment_BTB1130_PR400Allo_maxsplooseafter2nrig2.root",
//    "/eos/ams/group/mit/qyan/Data/alignment_BTB1130_PR400N0AV3n_maxspfnoafter2fast3rig/alignment_BTB1130_PR400N0AV3n_maxspfnoafter2fast3rig.root",
//    "/eos/ams/group/mit/qyan/Data/alignment_BTB1130_PR400N0AV3m_maxsppgrig/alignment_BTB1130_PR400N0AV3m_maxsppgrig.root",
   };
#else
  const int nfl=3;
  const char *filen[]={
/*    "/eos/ams/group/mit/qyan/Data/alignment_BTB1130_PR400_60FebBackN0AV4o_pgrig/alignment_BTB1130_PR400_60FebBackN0AV4o_pgrig.root",
//    "/eos/ams/group/mit/qyan/Data/alignment_BTB1130_PR400_60FebBackN0AV4o_after2rigo/alignment_BTB1130_PR400_60FebBackN0AV4o_after2rigo.root",
    "/eos/ams/group/mit/qyan/Data/alignment_BTB1130_PR400_60FebBackN0AV4o_after2rig2/alignment_BTB1130_PR400_60FebBackN0AV4o_after2rig2.root",*/
    "/eos/ams/group/mit/qyan/Data/alignment_BTB1130_PR400_60FebBackN0AV4o_after2nrig2/alignment_BTB1130_PR400_60FebBackN0AV4o_after2nrig2.root",
//    "/eos/ams/group/mit/qyan/Data/alignment_BTB1130_PR400_60FebBackN0AV4o_looseafter2nrig2/alignment_BTB1130_PR400_60FebBackN0AV4o_looseafter2nrig2.root",
//    "/eos/ams/group/mit/qyan/Data/alignment_BTB1130_PR400_60FebBackN0AV4o_oladafter2rig2/alignment_BTB1130_PR400_60FebBackN0AV4o_oladafter2rig2.root",
//    "/eos/ams/group/mit/qyan/Data/alignment_BTB1130_PR400_60FebBackN0AV4o_test1after2rig2/alignment_BTB1130_PR400_60FebBackN0AV4o_test1after2rig2.root",
//    "/eos/ams/group/mit/qyan/Data/alignment_BTB1130_PR400_60FebBackN0AV4o_maxsppgrig/alignment_BTB1130_PR400_60FebBackN0AV4o_maxsppgrig.root", 
//    "/eos/ams/group/mit/qyan/Data/alignment_BTB1130_PR400_60FebBackN0AV4o_maxspafter2backrig2/alignment_BTB1130_PR400_60FebBackN0AV4o_maxspafter2backrig2.root",
    "/eos/ams/group/mit/qyan/Data/alignment_BTB1130_PR400_60FebBackN0AV4o_maxspafter2nbackrig2/alignment_BTB1130_PR400_60FebBackN0AV4o_maxspafter2nbackrig2.root",
     "/eos/ams/group/mit/qyan/Data/alignment_BTB1130_PR400_60FebN0AV3o_maxspafter2nrig2/alignment_BTB1130_PR400_60FebN0AV3o_maxspafter2nrig2.root",
//    "/eos/ams/group/mit/qyan/Data/alignment_BTB1130_PR400_60FebBackN0AV4o_maxsptest1after2backrig2/alignment_BTB1130_PR400_60FebBackN0AV4o_maxsptest1after2backrig2.root",
//    "/eos/ams/group/mit/qyan/Data/alignment_BTB1130_PR400_60FebBackN0AV4o_maxspoladafter2backrig2/alignment_BTB1130_PR400_60FebBackN0AV4o_maxspoladafter2backrig2.root",
//    "/eos/ams/group/mit/qyan/Data/alignment_BTB1130_PR400_60FebBackN0AV4o_maxspafter2rig/alignment_BTB1130_PR400_60FebBackN0AV4o_maxspafter2rig.root",
//    "/eos/ams/group/mit/qyan/Data/alignment_BTB1130_PR400_60FebBackN0AV4o_maxspafter2nrig2/alignment_BTB1130_PR400_60FebBackN0AV4o_maxspafter2nrig2.root",
//    "/eos/ams/group/mit/qyan/Data/alignment_BTB1130_PR400_60FebBackN0AV4o_maxsplooseafter2nrig2/alignment_BTB1130_PR400_60FebBackN0AV4o_maxsplooseafter2nrig2.root",
//    "/eos/ams/group/mit/qyan/Data/alignment_BTB1130_PR400_60FebBackN0AV4o_maxspoladafter2nrig2/alignment_BTB1130_PR400_60FebBackN0AV4o_maxspoladafter2nrig2.root",
//    "/eos/ams/group/mit/qyan/Data/alignment_BTB1130_PR400_60FebBackN0AV4o_maxsptest1after2rig2/alignment_BTB1130_PR400_60FebBackN0AV4o_maxsptest1after2rig2.root",
//   "/eos/ams/group/mit/qyan/Data/alignment_BTB1130_PR400_60FebBackN0AV4o_maxspafter2rigo/alignment_BTB1130_PR400_60FebBackN0AV4o_maxspafter2rigo.root",*/
  };
#endif
  TH1D *h1r[nfl]={0};
  TH2D *h2r[nfl]={0};
  char histn[1000];
  for(int ifl=0;ifl<nfl;ifl++){
     TFile *f=new TFile(filen[ifl]);
     f->cd();
/*     h1r[ifl]=(TH1D *)f->Get("srigr1_q1");
     h2r[ifl]=(TH2D *)f->Get("srigr1b");*/
#ifndef USENEGBETA
//     h1r[ifl]=(TH1D *)f->Get("srigr1_q1");//fullsp
     h1r[ifl]=(TH1D *)f->Get("srigr1_q1b0");//masp
#else
//     h1r[ifl]=(TH1D *)f->Get("srigr1_q1");//fullsp
     h1r[ifl]=(TH1D *)f->Get("srigr1_q1b0");//masp
//     h1r[ifl]=(TH1D *)f->Get("srigr1_q1b4");
//     h1r[ifl]=(TH1D *)f->Get("srigr1s0_q1b4");
//     h1r[ifl]=(TH1D *)f->Get("srigr1s0_q1b0");
//     h1r[ifl]=(TH1D *)f->Get("srigr1s1_q1b4");
#endif
     h2r[ifl]=(TH2D *)f->Get("srigr1b0");
  }

//-----2d
  TCanvas *c=0;
//  set_frame_style();
  TH1D *ha[nfl]={0};
  for(int ifl=0;ifl<nfl;ifl++){
    c=new TCanvas();
    TH2D *hn=h2r[ifl];
    if(!hn)continue;
//    set_frame_style(hn);
    gPad->SetLogz();
    hn->Draw("COLZ");
    hn->GetXaxis()->SetTitle("Beam Point ID");
    hn->GetXaxis()->SetTitleColor(1);
    hn->GetYaxis()->SetTitle("Tracker Resolution #Delta#frac{1}{R}  [TV^{-1} ]");
    sprintf(histn,"%spa%d",hn->GetName(),ifl);
    TH1D *pa=new TH1D(histn,histn,100,-3,3);
    pa->Reset();
    ha[ifl]=pa;
    sprintf(histn,"%spf%d",hn->GetName(),ifl);
    TProfile *pr=(TProfile *)hn->ProfileX(histn);
    pr->SetMarkerStyle(20);
    pr->SetMarkerColor(1);
    pr->Draw("same");
    for(int ibx=1;ibx<=hn->GetNbinsX();ibx++){
      double sum=hn->Integral(ibx,ibx,1,hn->GetNbinsY());
      if(sum<1000){pr->SetBinContent(ibx,0);pr->SetBinError(ibx,0);}
      else        pa->Fill(pr->GetBinContent(ibx));
    }
  }

//-----1d
  int ucol[]={2,4,8,7,1,6,5};
  int ip=0;
  c=new TCanvas();
  for(int ifl=0;ifl<nfl;ifl++){
    TH1D *hp=ha[ifl];
    if(hp==0)continue;
    hp->SetLineColor(ucol[ifl]);
    if(ip==0)hp->Draw();
    else     hp->Draw("sames");
    hp->GetXaxis()->SetTitleColor(1);
    ip++;
  }

  
//---
  ip=0;
  c=new TCanvas();
  gPad->SetLogy();
  for(int ifl=0;ifl<nfl;ifl++){
    TH1D *hn=h1r[ifl];
    if(!hn)continue;
//    set_frame_style(hn);
    if(ip==0)hn=(TH1D *)hn->DrawNormalized();
    else     hn=(TH1D *)hn->DrawNormalized("sames");
/*    if(ip==0)hn->Draw();
    else     hn->Draw("sames");*/
    hn->SetLineColor(ucol[ifl]);
    hn->GetXaxis()->SetTitle("Tracker Resolution #Delta#frac{1}{R}  [TV^{-1} ]");
    hn->GetYaxis()->SetTitle("Events");
    hn->SetLineWidth(2);
    hn->GetXaxis()->SetTitleColor(1);
    ip++;
  }


  return 0;
}
