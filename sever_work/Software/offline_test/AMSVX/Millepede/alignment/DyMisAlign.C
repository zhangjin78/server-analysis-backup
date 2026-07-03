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
#include "/afs/cern.ch/work/q/qyan/AMSVDEV/QTool/QPlot.h"
#include "/afs/cern.ch/work/q/qyan/AMSVDEV/QTool/QSplineFit.C"

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


int DyMisAlignMC(){

  const int nfl=2;
//  const int nfl=3;
//  const int nfl=4;
//  const int nfl=4+1;
//  const int nfl=6;
  const char *filen[]={
//    "/eos/ams/group/mit/qyan/Data/alignment_BTB1130_PR054000N0_DYNOc/alignment_BTB1130_PR054000N0_DYNOc_0.root",
/*    "/eos/ams/group/mit/qyan/Data/alignment_BTB1130_PR054000N0_DYNOc/alignment_BTB1130_PR054000N0_DYNOc_1.root",
    "/eos/ams/group/mit/qyan/Data/alignment_BTB1130_PR054000N0_DYNOc/alignment_BTB1130_PR054000N0_DYNOc_2.root",
    "/eos/ams/group/mit/qyan/Data/alignment_BTB1130_PR054000N0_DYNOc/alignment_BTB1130_PR054000N0_DYNOc_3.root",*/
//    "/eos/ams/group/mit/qyan/Data/alignment_BTB1130_PR054000N0_DYNOSUPEREc/alignment_BTB1130_PR054000N0_DYNOSUPEREc_4.root",
/*    "/eos/ams/group/mit/qyan/Data/alignment_BTB1130_PR054000N0_DYNOc/alignment_BTB1130_PR054000N0_DYNOc_4.root",
    "/eos/ams/group/mit/qyan/Data/alignment_BTB1130_PR054000N0_TCHISCUTc/alignment_BTB1130_PR054000N0_TCHISCUTc_4.root",
    "/eos/ams/group/mit/qyan/Data/alignment_BTB1130_PR054000N0_T0CHISCUTc/alignment_BTB1130_PR054000N0_T0CHISCUTc_4.root",*/
/*    "/eos/ams/group/mit/qyan/Data/alignment_BTB1130_PR054000N0_DYHEc/alignment_BTB1130_PR054000N0_DYHEc_4.root",
    "/eos/ams/group/mit/qyan/Data/alignment_BTB1130_PR054000N0_DYHEVf/alignment_BTB1130_PR054000N0_DYHEVf_4.root",*/
/*    "/eos/ams/group/mit/qyan/Data/alignment_BTB1130_PR054000N0_HETCHISCUTc/alignment_BTB1130_PR054000N0_HETCHISCUTc_4.root",
    "/eos/ams/group/mit/qyan/Data/alignment_BTB1130_PR054000N0_HET0CHISCUTc/alignment_BTB1130_PR054000N0_HET0CHISCUTc_4.root",*/
//    "/eos/ams/group/mit/qyan/Data/alignment_BTB1130_PR054000N0_DYHEEXCLUDELOWEc/alignment_BTB1130_PR054000N0_DYHEEXCLUDELOWEc_4.root",
    "/eos/ams/group/mit/qyan/Data/alignment_BTB1130_PR05100N0_DYNOc/alignment_BTB1130_PR05100N0_DYNOc_4.root",
    "/eos/ams/group/mit/qyan/Data/alignment_BTB1130_PR05100N0_DYNOVf/alignment_BTB1130_PR05100N0_DYNOVf_4.root",
//    "/eos/ams/group/mit/qyan/Data/alignment_BTB1130_PR054000N0_DYEXCLUDELOWEc/alignment_BTB1130_PR054000N0_DYEXCLUDELOWEc_4.root",
//    "/eos/ams/group/mit/qyan/Data/alignment_BTB1130_PR05100N0_DYEXCLUDELOWEc/alignment_BTB1130_PR05100N0_DYEXCLUDELOWEc_0.root",
/*    "/eos/ams/group/mit/qyan/Data/alignment_BTB1130_PR05100N0_DYEXCLUDELOWEc/alignment_BTB1130_PR05100N0_DYEXCLUDELOWEc_1.root",
    "/eos/ams/group/mit/qyan/Data/alignment_BTB1130_PR05100N0_DYEXCLUDELOWEc/alignment_BTB1130_PR05100N0_DYEXCLUDELOWEc_2.root",
    "/eos/ams/group/mit/qyan/Data/alignment_BTB1130_PR05100N0_DYEXCLUDELOWEc/alignment_BTB1130_PR05100N0_DYEXCLUDELOWEc_3.root",*/ 
//    "/eos/ams/group/mit/qyan/Data/alignment_BTB1130_PR05100N0_DYEXCLUDELOWEc/alignment_BTB1130_PR05100N0_DYEXCLUDELOWEc_4.root",
/*    "/eos/ams/group/mit/qyan/Data/alignment_BTB1130_PR054000N0_EXCLUDELOWERIGLIMIT/alignment_BTB1130_PR054000N0_EXCLUDELOWERIGLIMIT_1.root",
    "/eos/ams/group/mit/qyan/Data/alignment_BTB1130_PR054000N0_EXCLUDELOWERIGLIMIT/alignment_BTB1130_PR054000N0_EXCLUDELOWERIGLIMIT_2.root",
    "/eos/ams/group/mit/qyan/Data/alignment_BTB1130_PR054000N0_EXCLUDELOWERIGLIMIT/alignment_BTB1130_PR054000N0_EXCLUDELOWERIGLIMIT_3.root",
    "/eos/ams/group/mit/qyan/Data/alignment_BTB1130_PR054000N0_EXCLUDELOWERIGLIMIT/alignment_BTB1130_PR054000N0_EXCLUDELOWERIGLIMIT_4.root",*/
//    "/eos/ams/group/mit/qyan/Data/alignment_BTB1130_PR054000N0_DYEXCLUDELOWEc/alignment_BTB1130_PR054000N0_DYEXCLUDELOWEc_0.root",
/*    "/eos/ams/group/mit/qyan/Data/alignment_BTB1130_PR054000N0_DYEXCLUDELOWEc/alignment_BTB1130_PR054000N0_DYEXCLUDELOWEc_1.root",
    "/eos/ams/group/mit/qyan/Data/alignment_BTB1130_PR054000N0_DYEXCLUDELOWEc/alignment_BTB1130_PR054000N0_DYEXCLUDELOWEc_2.root",
    "/eos/ams/group/mit/qyan/Data/alignment_BTB1130_PR054000N0_DYEXCLUDELOWEc/alignment_BTB1130_PR054000N0_DYEXCLUDELOWEc_3.root",*/
/*    "/eos/ams/group/mit/qyan/Data/alignment_BTB1130_PR054000N0_DYEXCLUDELOWEc/alignment_BTB1130_PR054000N0_DYEXCLUDELOWEc_4.root",
     "/eos/ams/group/mit/qyan/Data/alignment_BTB1130_PR054000N0_EXCLUDELOWETCHISCUTc/alignment_BTB1130_PR054000N0_EXCLUDELOWETCHISCUTc_4.root",
     "/eos/ams/group/mit/qyan/Data/alignment_BTB1130_PR054000N0_EXCLUDELOWET0CHISCUTc/alignment_BTB1130_PR054000N0_EXCLUDELOWET0CHISCUTc_4.root",*/
/*    "/eos/ams/group/mit/qyan/Data/alignment_BTB1130_PR05100N0_T0CHISCUTc/alignment_BTB1130_PR05100N0_T0CHISCUTc_4.root",
    "/eos/ams/group/mit/qyan/Data/alignment_BTB1130_PR05100N0_EXCLUDELOWET0CHISCUTc/alignment_BTB1130_PR05100N0_EXCLUDELOWET0CHISCUTc_4.root",
    "/eos/ams/group/mit/qyan/Data/alignment_BTB1130_PR05100N0_TCHISCUTc/alignment_BTB1130_PR05100N0_TCHISCUTc_4.root",
    "/eos/ams/group/mit/qyan/Data/alignment_BTB1130_PR05100N0_EXCLUDELOWETCHISCUTc/alignment_BTB1130_PR05100N0_EXCLUDELOWETCHISCUTc_4.root",*/
//    "/eos/ams/group/mit/qyan/Data/alignment_BTB1130_PR054000N0_RIGLIMITc/alignment_BTB1130_PR054000N0_RIGLIMITc_3.root",
//    "/eos/ams/group/mit/qyan/Data/alignment_BTB1130_PR054000N0_RIGLIMITc/alignment_BTB1130_PR054000N0_RIGLIMITc_4.root",
/*    "/eos/ams/group/mit/qyan/Data/alignment_BTB1130_PR054000N0_MAXSPCHISc/alignment_BTB1130_PR054000N0_MAXSPCHISc_4.root",
    "/eos/ams/group/mit/qyan/Data/alignment_BTB1130_PR054000N0_MAXSPCHIS1c/alignment_BTB1130_PR054000N0_MAXSPCHIS1c_4.root",*/
/*    "/eos/ams/group/mit/qyan/Data/alignment_BTB1130_PR054000N0_INNERCHISc/alignment_BTB1130_PR054000N0_INNERCHISc_4.root",
    "/eos/ams/group/mit/qyan/Data/alignment_BTB1130_PR054000N0_INNERCHIS1c/alignment_BTB1130_PR054000N0_INNERCHIS1c_4.root",
    "/eos/ams/group/mit/qyan/Data/alignment_BTB1130_PR054000N0_HTOFBETAc/alignment_BTB1130_PR054000N0_HTOFBETAc_4.root",*/
//    "/eos/ams/group/mit/qyan/Data/alignment_BTB1130_PR054000N0_DYHEEXCLUDELOWEb/alignment_BTB1130_PR054000N0_DYHEEXCLUDELOWEb_4.root",
/*    "/eos/ams/group/mit/qyan/Data/alignment_BTB1130_PR054000N0_DYT1CHISCUTc/alignment_BTB1130_PR054000N0_DYT1CHISCUTc_4.root",
    "/eos/ams/group/mit/qyan/Data/alignment_BTB1130_PR054000N0_DYT2CHISCUTc/alignment_BTB1130_PR054000N0_DYT2CHISCUTc_4.root",
    "/eos/ams/group/mit/qyan/Data/alignment_BTB1130_PR054000N0_DYT3CHISCUTc/alignment_BTB1130_PR054000N0_DYT3CHISCUTc_4.root",*/
//    "/eos/ams/group/mit/qyan/Data/alignment_BTB1130_PR054000N0_DYSURVIVEL9c/alignment_BTB1130_PR054000N0_DYSURVIVEL9c_4.root",
//    "/eos/ams/group/mit/qyan/Data/alignment_BTB1130_PR054000N0_DYCLEAN2c/alignment_BTB1130_PR054000N0_DYCLEAN2c_4.root",
//    "/eos/ams/group/mit/qyan/Data/alignment_BTB1130_PR054000N0_DYNEVGc/alignment_BTB1130_PR054000N0_DYNEVGc_4.root",
//    "/eos/ams/group/mit/qyan/Data/alignment_BTB1130_PR054000N0_DYCLEANAc/alignment_BTB1130_PR054000N0_DYCLEANAc_4.root",
//    "/eos/ams/group/mit/qyan/Data/alignment_BTB1130_PR054000N0_DYHEc/alignment_BTB1130_PR054000N0_DYHEc_0.root",
/*    "/eos/ams/group/mit/qyan/Data/alignment_BTB1130_PR054000N0_DYHEc/alignment_BTB1130_PR054000N0_DYHEc_1.root",
    "/eos/ams/group/mit/qyan/Data/alignment_BTB1130_PR054000N0_DYHEc/alignment_BTB1130_PR054000N0_DYHEc_2.root",
    "/eos/ams/group/mit/qyan/Data/alignment_BTB1130_PR054000N0_DYHEc/alignment_BTB1130_PR054000N0_DYHEc_3.root",*/
//    "/eos/ams/group/mit/qyan/Data/alignment_BTB1130_PR054000N0_DYHEc/alignment_BTB1130_PR054000N0_DYHEc_4.root",
//    "/eos/ams/group/mit/qyan/Data/alignment_BTB1130_PR054000N0_DYHENOSUPEREb/alignment_BTB1130_PR054000N0_DYHENOSUPEREb_4.root",       
//    "/eos/ams/group/mit/qyan/Data/alignment_BTB1130_PR054000N0_DYHESURVIVEL9c/alignment_BTB1130_PR054000N0_DYHESURVIVEL9c_4.root",
//    "/eos/ams/group/mit/qyan/Data/alignment_BTB1130_PR054000N0_DYHECLEAN2b/alignment_BTB1130_PR054000N0_DYHECLEAN2b_4.root",
//    "/eos/ams/group/mit/qyan/Data/alignment_BTB1130_PR054000N0_DYHECLEANAb/alignment_BTB1130_PR054000N0_DYHECLEANAb_4.root",
//    "/eos/ams/group/mit/qyan/Data/alignment_BTB1130_PR054000N0_DYHENEVGb/alignment_BTB1130_PR054000N0_DYHENEVGb_4.root",
   };
   const char *lengn[]={
     "Before",
     "1st",
     "2nd",
     "3rd",
     "4th",
     "After",
/*     "0.5-30GV",
     "0.5-30GV+1-1.1TV",*/
//     "0.5-30GV survive",
//     "0.5-30GV cleana",
//     "Events#times4",
//     "0.5-100GV survive",
//     "0.5-100GV cleana",
   };
   int uz=1;
   const int nl=2;
   const int ns=2;
   TH1D *h1r[nfl][nl][ns]={{0}};
   TH1D *hev[nfl]={0};
   TH1D *hrig[nfl][2]={{0}};
   char histn[1000];
   for(int ifl=0;ifl<nfl;ifl++){
     TFile *f=new TFile(filen[ifl]);
     f->cd();
     for(int ixy=0;ixy<ns;ixy++){
       for(int il=0;il<nl;il++){
         int ul=(il==0)?0:8;
         sprintf(histn,"mis1r_r3z%dl%dxy%d",uz,ul,ixy);
         TH2D *h0=(TH2D *)f->Get(histn);
         if(!h0)continue;
         double lv=5;
         int lb=h0->GetXaxis()->FindBin(lv);
         int nb=h0->GetNbinsX();
         sprintf(histn,"%sf%d",h0->GetName(),ifl);
         TH1D *hn=(TH1D *)h0->ProjectionY(histn,lb,nb);
         char titlex[100];
         if(il==0)sprintf(titlex,"%s","L1 ");
         else     sprintf(titlex,"%s","L9 ");
         strcat(titlex,"MisAlign ");
         if(ixy==0)strcat(titlex,"#DeltaX");
         else      strcat(titlex,"#DeltaY");
         strcat(titlex," [#mum]");
         hn->GetXaxis()->SetTitle(titlex);
         hn->GetYaxis()->SetTitle("Normalized Entries");
         h1r[ifl][il][ixy]=hn;
       }
     }
     sprintf(histn,"selmom1_z%d",uz);
     TH1D *h1=(TH1D *)f->Get(histn);
     h1->GetXaxis()->SetTitle("Rigidity [GV]");
     h1->GetXaxis()->SetTitleColor(1);
     h1->GetYaxis()->SetTitle("Normalized Entries");
     hev[ifl]=h1;
     for(int ir=1;ir<=2;ir++){
       sprintf(histn,"srigr%d_q%d",ir,uz);
       TH2D *h0=(TH2D *)f->Get(histn);
       if(!h0)continue;
       int lb=h0->GetXaxis()->FindBin(800);
       int hb=h0->GetXaxis()->FindBin(1200);
       sprintf(histn,"%sf%d",h0->GetName(),ifl);
       TH1D *hn=(TH1D *)h0->ProjectionY(histn,lb,hb);
       if(ir==1)hn->GetXaxis()->SetTitle("1/R_{18}^{rec}-1/R_{18}^{true} [TV-1]");
       else     hn->GetXaxis()->SetTitle("1/R_{19}^{rec}-1/R_{19}^{true} [TV-1]");
       hn->GetYaxis()->SetTitle("Normalized Entries");
       hrig[ifl][ir-1]=hn;
     }
   }

//--1d
//  int ucol[]={1,2,4,8,7,6,5}; 
//  int ucol[]={4,2,1,8,7,6,5};
//  int ucol[]={1,2,4,8,7,6,5};
//  int ucol[]={2,4,8,7,6,5};
  int ucol[]={4,8,6,2,7,kOrange,5};
//  int ucol[]={4,2,6,2,7,kOrange,5};
  set_frame_style();
  TCanvas *c=0;
  gStyle->SetOptStat(1110);
//  gStyle->SetOptStat(10);
  c=new TCanvas();
  c->Divide(2,2);
  for(int il=0;il<nl;il++){
     for(int ixy=0;ixy<ns;ixy++){
       c->cd(il*2+ixy+1);
       int ip=0;
       TLegend *leng=new TLegend(0.19,0.65,0.45,0.85);
       leng->SetBorderSize(0);
       leng->SetFillColor(0);
       TH1D *h0=0;
       double ymax=0;
       double normw=-1;
       for(int ifl=0;ifl<nfl;ifl++){
         TH1D *hn=h1r[ifl][il][ixy];
         if(!hn||hn->GetEntries()<=10)continue;
         hn->SetLineColor(ucol[ifl]);
         set_frame_style(hn);
         hn->SetTitle(0);
         double binwn=hn->GetXaxis()->GetBinWidth(1);
         if(normw==-1)normw=binwn*hn->GetEntries();
/*         if(ip==0)hn->Draw();
         else     hn->Draw("same");*/ 
         if(ip==0)hn=(TH1D *)hn->DrawNormalized("",normw/binwn);
         else     hn=(TH1D *)hn->DrawNormalized("sames",normw/binwn);
         hn->SetLineWidth(2);
         hn->GetYaxis()->SetTitleOffset(1.4);
         if(h0==0){
           h0=hn;
         }
         hn->GetXaxis()->SetRangeUser(-40,40);
         double yn=hn->GetMaximum();
         if(yn>ymax){
           ymax=yn;
           h0->GetYaxis()->SetRangeUser(ymax/10000,ymax*1.2);
         }
         leng->AddEntry(hn,lengn[ifl],"L");
         ip++;
       }
       leng->Draw("same");
     }
  }


//-----
  for(int ih=0;ih<3;ih++){
    c=new TCanvas();
    if(ih==0)gPad->SetLogx();
    TLegend *leng=new TLegend(0.19,0.65,0.45,0.85);
    leng->SetBorderSize(0);
    leng->SetFillColor(0);
    int ip=0;
    TH1D *h0=0;
    double ymax=0;
    for(int ifl=0;ifl<nfl;ifl++){
      TH1D *hn=hev[ifl];
      if(ih>=1)hn=hrig[ifl][ih-1];
      if(!hn||hn->GetEntries()<=10)continue;
      hn->SetLineColor(ucol[ifl]);
      set_frame_style(hn);
      hn->SetTitle(0);
      if(ip==0)hn=(TH1D *)hn->DrawNormalized();
      else     hn=(TH1D *)hn->DrawNormalized("sames");
      hn->SetLineWidth(2);
      hn->GetYaxis()->SetTitleOffset(1.4);
      if(h0==0){
       h0=hn;
      }
      double yn=hn->GetMaximum();
      if(yn>ymax){
        ymax=yn;
        h0->GetYaxis()->SetRangeUser(ymax/10000,ymax*1.2);
      }
      leng->AddEntry(hn,lengn[ifl],"L");
      ip++;
   }
   leng->Draw("same");
 } 

 return 0;
}


int DyMisAlign(){
#define USEFINAL
#define USEFAST
//#define DRAWERRORBAND
  gStyle->SetOptFit(100);
  gStyle->SetStatX(0.95);
  gStyle->SetStatY(0.95);
  gStyle->SetStatW(0.06);
  gStyle->SetStatH(0.03);
/*  const char *listn="/eos/ams/group/mit/qyan/Data/alignment_BTB1130_P7N0Extd/alignment_BTB1130_P7N0Extd_4res.list";
  const char *odir="/eos/ams/group/mit/qyan/Data/alignment_BTB1130_P7N0Extd";*/
/*  const char *listn="/eos/ams/group/mit/qyan/Data/alignment_BTB1130_P7N0Exte/alignment_BTB1130_P7N0Exte_4res.list";
  const char *odir="/eos/ams/group/mit/qyan/Data/alignment_BTB1130_P7N0Exte";*/
  const char *listn="/eos/ams/group/mit/qyan/Data/alignment_BTB1130_P7N0Exth/alignment_BTB1130_P7N0Exth_3res.list";
  const char *odir="/eos/ams/group/mit/qyan/Data/alignment_BTB1130_P7N0Exth";
/*  const char *listn="/eos/ams/group/mit/qyan/Data/alignment_BTB1130_P7N0Exth_AfFixpar2/alignment_BTB1130_P7N0Exth_3res.list";
  const char *odir="/eos/ams/group/mit/qyan/Data/alignment_BTB1130_P7N0Exth_AfFixpar2";*/
//-----
  vector<string> files;
  ifstream fbin(listn);
  if(!fbin){cerr<<"can not open "<<listn<<endl;return -1;}
  string ss;
  while(getline(fbin,ss)){
    files.push_back(ss);
  }
  fbin.close();
//----
  const int npar=12;
  const char *parn[npar]={
    "L1 #DeltaX",
    "L1 #DeltaY", 
    "L1 #DeltaZ",
    "L1 #alpha",
    "L1 #beta",
    "L1 #gamma",
    "L9 #DeltaX",
    "L9 #DeltaY",
    "L9 #DeltaZ",
    "L9 #alpha",
    "L9 #beta",
    "L9 #gamma",
  };
#ifdef USEFINAL
  int udi[]={
/*   3,3,5,10,10,10,
   4,4,5,10,10,10,*/
   5,6,10,20,20,20,
   7,8,10,20,20,20, 
  };
#else
  int udi[]={
   -1,-1,-1,-1,-1,-1,
   -1,-1,-1,-1,-1,-1,
  };
#endif
  unsigned int t0=1305800000;
  unsigned int to=t0;
  TDatime TE(2021,05,20,00,00,00);
  int XE=TE.Convert();
  double yran[][2]={{-800,800},{-500,500},{-1300,1300},{-1.5,1.5},{-1.5,1.5},{-0.3,0.3}};
  TGraphErrors *ga[npar]={0},*gt[npar]={0};
  TGraphErrors *ga2[npar]={0},*gt2[npar]={0};
//  const int mdi=11;
  const int mdi=21;
  TH1D *hdi[npar][mdi]={{0}};
  TGraph *gdi[npar][mdi]={{0}};
  TGraph *rdi[npar]={0};
  TGraphErrors *gdis[npar]={0};
  TH1D *hdis[npar][mdi]={{0}};
  TProfile *hpar[npar]={0};
  TH1D *hvad[npar]={0};
  TH1D *hr[npar]={0};
  map<int, map<unsigned int, TkAlignParS> >aparss;
  unsigned int gtime[2]={0};
  char printn1[1000];
  char printn[300];
  char nameobj[100];
  int ipad=0;
  const int npad=3;
  char dytitle[50];
#ifdef USEFINAL
  int maxf=3600*24*30;
  sprintf(dytitle,"dyalignf");
//  sprintf(dytitle,"dyalignd");
#else
  int maxf=3600*24*365;
  sprintf(dytitle,"dyaligns");
#endif
//  int maxt=20000;
//  int maxt=60*24/3*10;
//  int maxt=60*24/3*3;
//  int maxt=60*24/3*1;
  int maxt=60*24/1.5*1;
  char ofilel[1000];
  sprintf(ofilel,"%s/%s_splinefit.list",odir,dytitle);
  ofstream fbinl(ofilel);
  set_frame_style();
  TCanvas *c=new TCanvas();
  sprintf(printn,"%s/%s_t%u.ps",odir,dytitle,t0);
  sprintf(printn1,"%s[",printn);
  c->Print(printn1);
  c->Clear();c->Divide(1,npad);
//-----
  TkTrackN *tkfinal=new TkTrackN();
  tkfinal->ConstructTracker();
  int ia=1;
  map<unsigned int, TkAlignParN> *palign=&((tkfinal->aligndb[ia]).aparns);
  int nfiles=files.size();
//  nfiles=30;
//  nfiles=3;
  for(unsigned int i=0;i<nfiles+1;i++){
    if(palign->size()>=1){
      palign->clear();  
    }
//------
    if(ga[0]&&(ga[0]->GetN()>=maxt||(ga[0]->GetN()>=1&&i==nfiles))){
      vector<QSplineFit *> effsp[npar];
      vector<TGraphErrors *> ges[npar];
      for(int ip=0;ip<npar;ip++){
        int detid=(ip<6)?ip+1:80+(ip-6)+1; 
        if(ipad!=0&&ipad%npad==0){c->Print(printn);c->Clear();c->Divide(1,npad);}
        c->cd(ipad%npad+1);
        ipad++;
        if(ga2[ip]){delete ga2[ip];ga2[ip]=0;}
        if(gt2[ip]){delete gt2[ip];gt2[ip]=0;}
        TGraphErrors *gn=ga[ip];
        gn->SetMarkerColor(2);
        gn->SetLineColor(2);
        gn->Draw("AP");
        TH1F *ha=gn->GetHistogram();
        gn->SetLineWidth(1);
        char tilen[100];
        sprintf(tilen,"%s_Time%u_%u",parn[ip],gtime[0],gtime[1]);
        ha->SetTitle(tilen);
        ha->GetXaxis()->SetTimeOffset(t0);
        ha->GetXaxis()->SetTimeDisplay(1);
//        ha->GetXaxis()->SetTimeFormat("#splitline{%Y}{%H\/%d\/%m}");
        ha->GetXaxis()->SetTimeFormat("%H\/%d\/%m\/%y");
        ha->GetXaxis()->SetTitle("Time [H/d/m/Y]");
        ha->GetXaxis()->SetTitleColor(1);
        double yrans=4;
        if     (ip%6<3) {ha->GetYaxis()->SetTitle("Shift [#mum]"); yrans=4;}
        else            {ha->GetYaxis()->SetTitle("Rotation [mrad]");yrans=5;}
        set_frame_style(ha);
        ha->GetYaxis()->SetTitleOffset(0.6);
        ha->GetYaxis()->SetTitleSize(0.06);
        ha->GetYaxis()->SetLabelSize(0.06);
        ha->GetYaxis()->SetTickLength(0.01);
//        ha->GetXaxis()->SetTitleOffset(1.2);
        ha->GetXaxis()->SetTitleSize(0.06);
        ha->GetXaxis()->SetLabelSize(0.06);
        ha->GetXaxis()->SetLabelOffset(0.011);
        double ranl=hr[ip]->GetMean()-yrans*hr[ip]->GetRMS();
        double ranh=hr[ip]->GetMean()+yrans*hr[ip]->GetRMS();
//        ha->GetYaxis()->SetRangeUser(yran[ip%6][0],yran[ip%6][1]);
        ha->GetYaxis()->SetRangeUser(ranl,ranh);
//---average error
        double averr=0;
        for(int ij=0;ij<gn->GetN();ij++){
          averr+=gn->GetErrorY(ij);
        }
        if(gn->GetN()>=1){averr/=gn->GetN();}
//        ha->GetYaxis()->SetTitle("PG-CIEMAT [um]");
      int pdi=2;
//      double scale0=0;
      double scale0=1e100;
      for(int di=2;di<mdi;di++){
        if(udi[ip]>=1){if(di!=udi[ip])continue; pdi=udi[ip];}
        if(hdi[ip][di]){delete hdi[ip][di];hdi[ip][di]=0;}
        char namese[1000];
        sprintf(namese,"pare%ddi%d",ip,di);
        hdi[ip][di]=new TH1D(namese,namese,30,0,hr[ip]->GetRMS());
        int nnodef=0,nodei=0,nodeb=0,nodea[2]={0};
        double nodef[10000],nodefi[10000],ynodefi[10000];
        double fr[2]={0},frpr[2]={0};
        int ci=0;
        double xo=0,yo=0,exo=0;
//        int di=3,dtbreak=600*6;
        int dtbreak=600*6;
        int dir=2;//di*dir=rotation
//        int di=2,dtbreak=600*3;
//        if(ip%6>=3)di=6;
        int ucol=4;
        bool isendf=(i==nfiles);
        for(int ii=0;ii<gn->GetN()+1;ii++){
          double xv=0,yv=0,exv=0;
          if(ii<gn->GetN()){gn->GetPoint(ii,xv,yv);exv=gt[ip]->GetErrorX(ii);}
          nnodef=nodei-nodeb;
          bool bznodef=(nnodef>=40);
//          bool bznodef=(nnodef>=60);
          bool timebreak=(xv<fr[0]-dtbreak||xv>fr[1]+dtbreak);//force break
          if(ii>=gn->GetN()||timebreak||bznodef)ci=0;//long break new starter
          if(ii!=0&&ci==0){
            cout<<"ipar="<<ip<<" ii="<<ii<<" nnodef="<<nnodef<<" fr="<<fr[0]<<","<<fr[1]<<endl;
            std::sort(nodef+nodeb,nodef+nodei);
//-------copy operation last points to ga2/gt2;
            if(ii>=gn->GetN()&&di==pdi&&!isendf){//end operation
             for(int ig=0;ig<2;ig++){
               TGraphErrors *gg=(ig==0)?ga[ip]:gt[ip];
               TGraphErrors *gg2=0;
               for(int jj=0;jj<gg->GetN();jj++){
                  double xv2=0,yv2=0,exv2=0,eyv2=0;
                  gg->GetPoint(jj,xv2,yv2);exv2=gg->GetErrorX(jj);eyv2=gg->GetErrorY(jj);
                  if(xv2<fr[0]||xv2>fr[1])continue;
                  if(!gg2)gg2=new TGraphErrors();
                  int ip2=gg2->GetN();
                  gg2->SetPoint(ip2,xv2,yv2);
                  gg2->SetPointError(ip2,exv2,eyv2);
                }
                if(ig==0)ga2[ip]=gg2;
                else     gt2[ip]=gg2;
//-------
                for(int jj=int(gg->GetN())-1;jj>=0;jj--){
                  double xv2=0,yv2=0;
                  gg->GetPoint(jj,xv2,yv2);
                  if(xv2<=frpr[1]||xv2<fr[0]||frpr[1]==0)break;
                  gg->RemovePoint(jj);   
                }
              }
              break;//break from the loop
            }
//--------
            if(fr[1]>nodef[nodei-1]){nodef[nodei++]=fr[1];}//add the last node
            nnodef=nodei-nodeb;
            if(nnodef>=2){//size>=2
              char namesp[100];
              sprintf(namesp,"splinef%dii%ddi%d",ip,ii,di);
              for(int in=0;in<nnodef;in++){//convert double to int
                nodefi[in]=(unsigned int)(nodef[nodeb+in]+0.5);
              }
              QSplineFit *fspline=new QSplineFit(namesp,gn,nnodef,nodefi,(QSplineFit::LinearXY|QSplineFit::ExtrapolateLE|QSplineFit::ExtrapolateLB),"b1e1");
              fspline->GetFun()->SetLineColor(ucol);
#ifdef USEFAST
              fspline->GetFun()->SetNpx(100);
#endif
              if(ucol==4)ucol=1;
              else       ucol=4;
              fspline->DoFit("+","same",fr[0]-100,fr[1]+100);
              TF1 *pfun=fspline->GetFun();
              double scale=pfun->GetChisquare();
              if(pfun->GetNDF()>=1){
                scale/=pfun->GetNDF();
                scale=sqrt(scale);//sqrt chis 
                if(scale<1)scale=1;
                if(scale<=scale0)scale0=scale;
                for(int ifp=0;ifp<pfun->GetNpar();ifp++){
                  double ferr=pfun->GetParError(ifp);
                  if(scale>1){
                    if(scale<=scale0){ferr=scale*ferr;}
                    else {//scale>scale0
                      ferr=scale0*ferr;
                      ferr=sqrt(ferr*ferr+(scale*scale-scale0*scale0)*averr*averr);
                    }
                  }
                  hdi[ip][di]->Fill(ferr);
                }
              }
//-------
#ifdef DRAWERRORBAND
              double confidential=0.68;
              TGraphErrors *ge=new TGraphErrors();
              for(int ij=0;ij<gn->GetN();ij++){
                double xv2=0,yv2=0;
                gn->GetPoint(ij,xv2,yv2);
                double exv2=gn->GetErrorX(ij);
                double eyv2=gn->GetErrorY(ij);
                if(xv2<fr[0]||xv2>fr[1])continue;
                int iga=ge->GetN();
                ge->SetPoint(iga,xv2,yv2);
                ge->SetPointError(iga,exv2,eyv2);
              }
              (TVirtualFitter::GetFitter())->GetConfidenceIntervals(ge,confidential);
              ge->SetFillColor(gn->GetLineColor());
/*              ge->SetFillColor(pfun->GetLineColor());
              ge->SetLineColor(pfun->GetLineColor());
              ge->SetMarkerColor(pfun->GetLineColor());*/
              ge->Draw("e3same");
              ges[ip].push_back(ge);
#endif
//-------
              effsp[ip].push_back(fspline);
//------
              fspline->GetFun()->GetParameters(ynodefi);
              unsigned int tpb=nodefi[0];
              unsigned int tpe=nodefi[nnodef-1];
              tpb+=t0;//back to normal time
              tpe+=t0;
#ifdef USEFINAL
              if(di==pdi){
                TkAlignParS *ppas=&(aparss[detid][tpb]);
//                cout<<"detid="<<detid<<" tpb="<<tpb<<" tpe="<<tpe<<endl;
                ppas->atime[0]=tpb;
                ppas->atime[1]=tpe;
                if(!hvad[ip]){
                  sprintf(namese,"pvard%d",ip);
                  hvad[ip]=new TH1D(namese,namese,100,-hr[ip]->GetRMS(),hr[ip]->GetRMS());
                }
                for(int in=0;in<nnodef;in++)hvad[ip]->Fill(ynodefi[in]);
                for(int in=0;in<nnodef;in++)ppas->spar[nodefi[in]]=(ip%6<3)?ynodefi[in]*1e-4:ynodefi[in]*1e-3;//to bak to the scale (xnode did not move to 0)
              }
#endif
//-------
            }
            else if(nnodef==1){
              cout<<"single point="<<" x="<<xo<<" y="<<yo<<" fr="<<fr[0]<<","<<fr[1]<<endl;
              unsigned int tpb=xo;
              unsigned int tpe=xo;
              tpb+=t0;//back to normal time
              tpe+=t0;
#ifdef USEFINAL
              if(di==pdi){
                TkAlignParS *ppas=&(aparss[detid][tpb]);
                ppas->atime[0]=tpb-60;//manual -60sec offset
                ppas->atime[1]=tpe+60;//manual +60sec offset
                ppas->spar[int(xo+0.5)]=(ip%6<3)?yo*1e-4:yo*1e-3;//(xnode did not move to 0)
              }
#endif
            }
            else              {cerr<<"err point"<<endl;}
            frpr[0]=fr[0];frpr[1]=fr[1];
            fr[0]=fr[1]=0;
            nodeb=nodea[0]=nodei;
            if(bznodef&&!timebreak){/*nodeb=nodei-4;*/nodeb=nodei-6;nodea[0]=nodei-2;fr[0]=fr[1]=nodef[nodeb];}//including previous 3 points in fitting
            if(ii>=gn->GetN())break;
          }
//          cout<<"i="<<i<<" xv="<<xv<<" yv="<<yv<<endl;
//          if(ci%di==0||ii+1==gn->GetN()){nodef[nodei++]=xv;} //more than 10 min
          if(ci%di==0){nodef[nodei++]=xv;} //more than 10 min
          ci++;
          if(fr[0]==0&&fr[1]==0){fr[0]=fr[1]=xv;}
          if(xv<fr[0]){fr[0]=xv;} 
          if(xv>fr[1]){fr[1]=xv;}
          xo=xv;yo=yv;exo=exv;
        }//end loop
        gn->Draw("Psame");
      }
     }
//---
      c->Print(printn);c->Clear();c->Divide(3,4);
      for(int ip=0;ip<npar;ip++){
        c->cd(ip+1);
        double xv=0,yv=0,xv1=0,yv1=0;
        ga[ip]->GetPoint(0,xv,yv);
        ga[ip]->GetPoint(ga[ip]->GetN()-1,xv1,yv1);
        int idn=0;
        double ymax=0;
        TH1D *h0=0;
        for(int di=1;di<mdi;di++){
          TH1D *hn=hdi[ip][di];
          if(!hn)continue;
          if(hn->GetEntries()<=1)continue;
          if(hn->Integral(1,hn->GetNbinsX())<=1)continue;
          if(idn==0)hn=(TH1D *)hn->DrawNormalized();
          else      hn=(TH1D *)hn->DrawNormalized("same");
          hn->SetLineColor(di-1);
          if(h0==0)h0=hn;
          if(hn->GetMaximum()>ymax){ymax=hn->GetMaximum();h0->GetYaxis()->SetRangeUser(0,ymax*1.2);}
          idn++;
          if(!gdi[ip][di]){
            gdi[ip][di]=new TGraph();
            sprintf(nameobj,"timepare%dn%d",ip,di);
            gdi[ip][di]->SetName(nameobj);
          }
          TGraph *gn=gdi[ip][di];
          int ii=gn->GetN();
          gn->SetPoint(ii,(xv+xv1)/2.,hn->GetMean());
        }
        if(!rdi[ip]){
          rdi[ip]=new TGraph();
          sprintf(nameobj,"timerms%d",ip);
          rdi[ip]->SetName(nameobj);
        }
        TGraph *rn=rdi[ip];
        rn->SetPoint(rn->GetN(),(xv+xv1)/2.,hr[ip]->GetRMS());
      }
//--- 
      c->Print(printn);c->Clear();c->Divide(1,npad);
      ipad=0;
//----check spline par size flush if size is large
      map<pair<unsigned int,int>,int> etimes;//etime,detid,size
      for(map<int, map<unsigned int, TkAlignParS> >::iterator it1=aparss.begin();it1!=aparss.end();it1++){//refine time tag
         map<unsigned int, TkAlignParS> aparsn; 
         map<unsigned int, TkAlignParS>::iterator itpr;
         for(map<unsigned int, TkAlignParS>::iterator it=(it1->second).begin();it!=(it1->second).end();it++){
           if(aparsn.size()>=1){
             long int atimen=(itpr->second).atime[1];//pre etime
             atimen+=(it->second).atime[0];//now btime
             atimen/=2;
             aparsn[atimen]=it->second;//now time changed
           }
           else aparsn[it->first]=it->second; 
           itpr=it;
           unsigned int et=(it->second).atime[1];
//           cout<<"etimes="<<et<<","<<it1->first<<","<<it->first<<" size="<<(it->second).GetSize()<<endl;
           etimes[make_pair(et,it1->first)]=3+(it->second).GetSize();
         }
         it1->second=aparsn;
       }
       map<int,pair<unsigned int,int> >dtimes;//detid,etime,totsize,the last2
       vector<unsigned int> setime;//detid etime
       int dsize[2]={2,2};
       for(map<pair<unsigned int,int>,int>::iterator it=etimes.begin();it!=etimes.end();it++){//sort by endtime
//         cout<<"etimes2="<<(it->first).first<<","<<(it->first).second<<" size="<<(it->second)<<endl;
         dsize[0]+=it->second;
         if(dsize[0]>38000){//find break point,new start,12*(3+40*2)~996<2000 is reserved for the overlap
           dsize[1]=2+dsize[0];
           unsigned int mtime=0;
           for(map<int,pair<unsigned int,int> >::iterator it1=dtimes.begin();it1!=dtimes.end();it1++){
             unsigned int petime=(it1->second).first;
             if(mtime==0||petime<mtime){mtime=petime;dsize[0]=dsize[1]-(it1->second).second;}//earilest time
           }
           setime.push_back(mtime);
//           cout<<"mtime="<<mtime<<" dsize="<<dsize[0]<<" dsize1="<<dsize[1]<<endl;
         }
         dtimes[(it->first).second].first=(it->first).first;//detid,etime
         dtimes[(it->first).second].second=dsize[0];//detid,totsize
       }
       bool isendf2=(i==nfiles);
       if(setime.size()>=1||isendf2){
         map<int, map<int, map<unsigned int, TkAlignParS> > >aparsg;//group,detid,btime
         for(map<int, map<unsigned int, TkAlignParS> >::iterator it1=aparss.begin();it1!=aparss.end();it1++){//detid,btime
           int ipos=0;
           map<unsigned int, TkAlignParS> &aparp=(it1->second);
           map<unsigned int, TkAlignParS>::iterator it=aparp.begin(); 
           for(;it!=aparp.end();it++){
             if(!isendf2){
               if(ipos+1>setime.size())break;
             }
             if(ipos+1<=setime.size()&&(it->second).atime[1]>=setime[ipos]){aparsg[ipos++][it1->first][it->first]=it->second;it--;continue;}//pos,detid,btime,back to same pos
             aparsg[ipos][it1->first][it->first]=it->second; 
           }
           aparp.erase(aparp.begin(),it);
         }
         for(map<int, map<int, map<unsigned int, TkAlignParS> > >::iterator it1=aparsg.begin();it1!=aparsg.end();it1++){
           (tkfinal->aligndb[ia]).aparss=it1->second;
           cout<<"block sizeabc="<<(tkfinal->aligndb[ia]).aparss.size()<<","<<(tkfinal->aligndb[ia]).GetSize(1)<<endl;
           (tkfinal->aligndb[ia]).CopyToTDVBlock(2000);
           unsigned int betime[2]={0};
           for(map<int, map<unsigned int, TkAlignParS> >::iterator it0=(it1->second).begin();it0!=(it1->second).end();it0++){
             if(betime[0]==0||(it0->second).begin()->first>betime[0]) {betime[0]=(it0->second).begin()->first;}
//             if(betime[1]==0||(it0->second).rbegin()->first<betime[1]){betime[1]=(it0->second).rbegin()->first;}
             if(betime[1]==0||((it0->second).rbegin()->second).atime[1]<betime[1]){betime[1]=((it0->second).rbegin()->second).atime[1];}
           }
           char ofiles[1000];
           sprintf(ofiles,"%s/%s_splinefit_%u_%u.res",odir,dytitle,betime[0],betime[1]);
           (tkfinal->aligndb[ia]).StreamAlignTDV(ofiles);
           fbinl<<betime[0]<<" "<<betime[1]<<" "<<ofiles<<endl;
           (tkfinal->aligndb[ia]).aparss.clear();
         }
       }
//-----
      for(int ip=0;ip<npar;ip++){//clear
        delete ga[ip];delete gt[ip];delete hr[ip];
        for(int j=0;j<effsp[ip].size();j++){delete effsp[ip][j];}
        effsp[ip].clear();
        for(int j=0;j<ges[ip].size();j++){delete ges[ip][j];}
        ges[ip].clear();
        gt[ip]=ga[ip]=0;
        hr[ip]=0;
        for(int di=1;di<mdi;di++){if(hdi[ip][di])delete hdi[ip][di];hdi[ip][di]=0;}
      }
//----open new ps files for new era(date)
      bool isnewps=(gtime[1]!=0&&gtime[1]-to>=maxf);
      if(isnewps||i==nfiles){
        c->Print(printn);c->Clear();c->Divide(3,4);
        for(int ip=0;ip<npar;ip++){
          c->cd(ip+1);
          int idn=0;
          double ymax=0;
          TH1F *h0=0;
          for(int di=1;di<mdi;di++){
            TGraph *gn=gdi[ip][di];
            if(!gn)continue;
            gn->SetLineColor(di-1);
            gn->SetMarkerColor(di-1);
            gn->SetMarkerStyle(20);
            gn->SetMarkerSize(0.8);
            if(idn==0)gn->Draw("APL");
            else      gn->Draw("PLsame");
            TH1F *ha=gn->GetHistogram();
            char tilen[100];
            sprintf(tilen,"Error_par%ddi%d",ip,di);
            ha->SetTitle(tilen);
            ha->GetXaxis()->SetTimeOffset(t0);
            ha->GetXaxis()->SetTimeDisplay(1);
            ha->GetXaxis()->SetTimeFormat("%H\/%d\/%m\/%y");
            ha->GetXaxis()->SetTitle("Time [d/m/y]");
            ha->GetXaxis()->SetTitleColor(1);
            if(h0==0)h0=ha;
            if(ha->GetMaximum()>ymax){ymax=ha->GetMaximum();h0->GetYaxis()->SetRangeUser(0,ymax*1.2);}
            idn++;
          }
          for(int di=1;di<mdi;di++){
            TGraph *gn=gdi[ip][di];
            if(!gn)continue;
            char namese[1000];
            sprintf(namese,"pare%ddi%ds",ip,di);
            if(!hdis[ip][di])hdis[ip][di]=new TH1D(namese,namese,100,0,1.5*ymax);
            TH1D *hn=hdis[ip][di];
            for(int ij=0;ij<gn->GetN();ij++){
              double xv2=0,yv2=0;
              gn->GetPoint(ij,xv2,yv2);
              hn->Fill(yv2);
            }
          }
        }
        c->Print(printn);c->Clear();c->Divide(3,4);
        for(int ip=0;ip<npar;ip++){
          c->cd(ip+1);
          int idn=0;
          for(int di=1;di<mdi;di++){
            TH1D *hn=hdis[ip][di];
            if(!hn)continue;
            hn->SetLineColor(di-1);
            if(idn==0)hn->Draw();
            else      hn->Draw("same");
            idn++;
            if(!gdis[ip]){
              gdis[ip]=new TGraphErrors();
              sprintf(nameobj,"nodespare%d",ip);
              gdis[ip]->SetName(nameobj);
            }
            TGraphErrors *gn=gdis[ip];
            int ij=gn->GetN();
            gn->SetPoint(ij,di,hn->GetMean());
            gn->SetPointError(ij,0,hn->GetRMS());
          }
        }
//-----best nodes
        c->Print(printn);c->Clear();c->Divide(3,4);
        for(int ip=0;ip<npar;ip++){
          c->cd(ip+1);
          TGraphErrors *gn=gdis[ip];
          gn->SetMarkerStyle(20);
          gn->SetMarkerSize(0.8);
          gn->Draw("APL");
        }
        c->Print(printn);c->Clear();c->Divide(1,npad);//clear again
        ipad=0;
        char fname[100];
        sprintf(fname,"%s/%s_t%u.root",odir,dytitle,to);
        TFile *fs=new TFile(fname,"RECREATE");
        for(int ip=0;ip<npar;ip++){
          if(gdis[ip]){gdis[ip]->Write();delete gdis[ip];gdis[ip]=0; }
          if(rdi[ip]){rdi[ip]->Write();delete rdi[ip];rdi[ip]=0;}
          if(hpar[ip]){hpar[ip]->Write();delete hpar[ip];hpar[ip]=0;}
          if(hvad[ip]){hvad[ip]->Write();delete hvad[ip];hvad[ip]=0;}
          for(int di=1;di<mdi;di++){
            if(gdi[ip][di]){gdi[ip][di]->Write();delete gdi[ip][di];gdi[ip][di]=0;}
            if(hdis[ip][di]){delete hdis[ip][di];hdis[ip][di]=0;} 
          }
        }
        delete fs;
      }
      if(isnewps){
        c->Print(printn);
        sprintf(printn1,"%s]",printn);
        c->Print(printn1);
        sprintf(printn,"%s/%s_t%u.ps",odir,dytitle,gtime[1]);
        sprintf(printn1,"%s[",printn);
        c->Print(printn1);
        to=gtime[1];
      }
      gtime[0]=gtime[1]=0;
    }
    if(i==nfiles)break;
//------
    if(!ga[0]){
      for(int ip=0;ip<npar;ip++){
        if(ga2[ip]){ga[ip]=ga2[ip];gt[ip]=gt2[ip];ga2[ip]=gt2[ip]=0; }//use previous last points
        else       {ga[ip]=new TGraphErrors();gt[ip]=new TGraphErrors();}
        char histn[100];
        sprintf(histn,"sparmr%d",ip);
        hr[ip]=new TH1D(histn,histn,2000,yran[ip%6][0],yran[ip%6][1]);
      }
    }
//-----
    int btime=1;
    int force=0;
    int stat=(tkfinal->aligndb[ia]).LoadAlignPar(files[i].c_str(),0,btime,force);
    for(map<unsigned int, TkAlignParN>::iterator it1=palign->begin();it1!=palign->end();it1++){
      TkAlignParN *ppar=&(it1->second);
      double btime=ppar->atime[0];
      double etime=ppar->atime[1];
      if(gtime[0]==0){gtime[0]=ppar->atime[0];gtime[1]=ppar->atime[1];}
      else {
        if(ppar->atime[0]<gtime[0])gtime[0]=ppar->atime[0];
        if(ppar->atime[1]>gtime[1])gtime[1]=ppar->atime[1];
      }
      double ptime=(etime+btime+1.)/2.;
      for(map<int,pair<TVector3,TVector3> >::iterator it=(ppar->apar).begin();it!=(ppar->apar).end();it++){//detid
        for(int ipar=0;ipar<6;ipar++){
          int ig=(it->first==0)?0:6;
          ig+=ipar;
          TGraphErrors *gan=ga[ig];
          TGraphErrors *gtn=gt[ig];
          double pv=(ipar>=0&&ipar<3)?(it->second).first[ipar]*1e4:(it->second).second[ipar-3]*1e3;
          double pe=(ipar>=0&&ipar<3)?(ppar->apae)[it->first].first[ipar]*1e4:(ppar->apae)[it->first].second[ipar-3]*1e3;
          double xv=ptime-t0;
          int ip=gan->GetN();
          gan->SetPoint(ip,xv,pv);
          gan->SetPointError(ip,0,pe);
          gtn->SetPoint(ip,xv,pv);
          gtn->SetPointError(ip,(etime+1-btime)/2.,pe);
          hr[ig]->Fill(pv);
          if(!hpar[ig]){
            sprintf(nameobj,"parvar%d",ig);
            TProfile *pr=new TProfile(nameobj,nameobj,3000,0,XE-t0);
            hpar[ig]=pr;
          }
          hpar[ig]->Fill(xv,pv);
        }
      }
    }
  }
  fbinl.close();
  c->Print(printn);
  sprintf(printn1,"%s]",printn);
  c->Print(printn1);
  return 0;
}


int AnalysisDyAlign(){

//  const int nfl=9;
  const int nfl=4;
  const char *fname[]={
    "/eos/ams/group/mit/qyan/Data/alignment_BTB1130_P7N0Exte/dyaligns_t1305800000.root",
    "/eos/ams/group/mit/qyan/Data/alignment_BTB1130_P7N0Exte/dyaligns_t1337441467.root",
    "/eos/ams/group/mit/qyan/Data/alignment_BTB1130_P7N0Exte/dyaligns_t1369050658.root",
    "/eos/ams/group/mit/qyan/Data/alignment_BTB1130_P7N0Exte/dyalignfa.root",
//    "/eos/ams/group/mit/qyan/Data/alignment_BTB1130_P7N0Exte/dyalignfa.root",
    "dyaligns_t1400639122.root",
    "dyaligns_t1432211353.root",
    "dyaligns_t1463826086.root",
    "dyaligns_t1495414536.root",
    "dyaligns_t1526983057.root",
    "dyaligns_t1558605385.root",
//    "dyaligns_t1590163708.root",
  };
  unsigned int t0=1305800000;
//-----
  const int npar=12;
  const char *parn[npar]={
    "L1 #DeltaX",
    "L1 #DeltaY",
    "L1 #DeltaZ",
    "L1 #alpha",
    "L1 #beta",
    "L1 #gamma",
    "L9 #DeltaX",
    "L9 #DeltaY",
    "L9 #DeltaZ",
    "L9 #alpha",
    "L9 #beta",
    "L9 #gamma",
  };
  const int mdi=11;
  TGraph *gdi[nfl][npar]={{0}};
  TGraph *rdi[nfl][npar]={0};
  TGraphErrors *gdis[nfl][npar]={0};
  TH1D *hpar[nfl][npar]={{0}};
  int udi[]={
   5,6,10,20,20,20,
   7,8,10,20,20,20,
  };
//-----
  char nameobj[100];
  for(int ifl=0;ifl<nfl;ifl++){
    TFile *f=new TFile(fname[ifl]);
    f->cd();
    for(int ip=0;ip<npar;ip++){
      sprintf(nameobj,"timerms%d",ip);//the vairation of the parameter in time windows
      rdi[ifl][ip]=(TGraph *)f->Get(nameobj);
      sprintf(nameobj,"nodespare%d",ip);
      gdis[ifl][ip]=(TGraphErrors *)f->Get(nameobj);//par fitting error
      sprintf(nameobj,"timepare%dn%d",ip,udi[ip]);//
      gdi[ifl][ip]=(TGraph *)f->Get(nameobj);
      sprintf(nameobj,"parvar%d",ip);
      hpar[ifl][ip]=(TProfile *)f->Get(nameobj);
    }
  }

//-----
  set_frame_style();
  TCanvas *c=0;
  int ipad=0;
  for(int ih=0;ih<4;ih++){
    ipad=0;
    for(int ip=0;ip<npar;ip++){
      if(ih>=2){if(ip%3==0){c=new TCanvas();c->Divide(1,3);ipad=0;}}
      else     {if(ip%6==0){c=new TCanvas();c->Divide(2,3);ipad=0;}}
      c->cd(ipad+1);
      ipad++;
      int ig=0;
      for(int ifl=0;ifl<nfl;ifl++){
        TH1 *ha=0;
        if(ih==3){
          ha=hpar[ifl][ip];
          if(!ha)continue;
          set_frame_style(ha);
//          ha->SetTitle(0);
/*          ha->SetLineColor(0);
          ha->SetMarkerColor(0);*/
//          ha->SetMarkerStyle(20);
//          ha->SetMarkerSize(0);
          ha->SetLineWidth(0.5);
          if(ig==0)ha->Draw("PE ");
          else     ha->Draw("PE same");
        }
        else {
          TGraph *gn=gdis[ifl][ip];
          if(ih==1)gn=rdi[ifl][ip];
          if(ih==2)gn=gdi[ifl][ip];
          if(!gn)continue;
          gn->SetLineColor(ifl+1);
          gn->SetMarkerColor(ifl+1);
          if(ig==0)gn->Draw("APL");
          else     gn->Draw("PLsame");
          gn->SetTitle(parn[ip]);
          ha=gn->GetHistogram();
          set_frame_style(ha);
          if(ip%6<3){ha->GetYaxis()->SetTitle("Shift [#mum]");}
          else      {ha->GetYaxis()->SetTitle("Rotation [mrad]");}
        }
        if(ih>=1){
          char tilen[100];
          ha->GetXaxis()->SetTimeOffset(t0);
          ha->GetXaxis()->SetTimeDisplay(1);
          ha->GetXaxis()->SetTimeFormat("%H\/%d\/%m\/%y");
          ha->GetXaxis()->SetTitle("Time [d/m/y]");
          ha->GetXaxis()->SetTitleColor(1);
        }
        ig++;   
      }
    }
  }

//-----
/*  for(int ifl=0;ifl<nfl;ifl++){
    ipad=0;
    for(int ip=0;ip<npar;ip++){
      if(ip%6==0){c=new TCanvas();c->Divide(2,3);ipad=0;}
      c->cd(ipad+1);
      ipad++;
      TGraph *gr=rdi[ifl][ip];
      if(!gr)continue;
      double xmin=gr->GetMinimum();
      double xmax=gr->GetMaximum();
      int ig=0;
      for(int di=1;di<mdi;di++){
        TGraph *gi=gdi[ifl][ip][di];
        if(!gi)continue;
        sprintf(nameobj,"%sprf%d",gi->GetName(),ifl);
        TProfile *pr=new TProfile(nameobj,nameobj,50,xmin,xmax);
        for(int ij=0;ij<gi->GetN();ij++){
          double xv2=0,yv2=0;
          gi->GetPoint(ij,xv2,yv2);
          double x0=gr->Eval(xv2);//rms
          pr->Fill(x0,yv2);//precision
        }
        pr->SetLineColor(di-1);
        pr->SetMarkerColor(di-1);
        if(ig==0)pr->Draw();
        else     pr->Draw("same");
        ig++;
      }
    }
  }*/
 

  return 0; 
}

int MergeResult(){
  const int nf=2;
  const char *filen[]={
    "/eos/ams/group/mit/qyan/Data/alignment_BTB1130_P7N0Exth/dyalignf_splinefit.list",
    "/eos/ams/group/mit/qyan/Data/alignment_BTB1130_P7N0Exth_AfFixpar2/dyalignf_splinefit.list",
  };
  const int npmax=6;
  int uid[nf][npmax]={
   -1,-1,-1,-1,-1,-1,
    1, 2, 3,81,82,83,
  };
  const char *dytitle="dyalignf";
  const char *odir="/eos/ams/group/mit/qyan/Data/alignment_BTB1130_P7N0Exth_All";
  std::map<pair<unsigned int,unsigned int>,string> fitpar[nf];
  for(int ifl=0;ifl<nf;ifl++){
    cout<<"ifl="<<ifl<<" filen="<<filen[ifl]<<endl;
    ifstream ffit(filen[ifl]);
    string s;
    while(std::getline(ffit,s)){
      istringstream iss(s);
      unsigned int ptime[2];
      string pname;
      if(!(iss>>ptime[0]>>ptime[1]>>pname))continue;
      pair<unsigned int,unsigned int> tn=make_pair(ptime[0],ptime[1]);
      fitpar[ifl][tn]=pname;
    }
    ffit.close();
    cout<<"ifl="<<ifl<<" size="<<fitpar[ifl].size()<<endl;
  }

//----load to memory
  map<int, map<unsigned int, TkAlignParS> > aparss;
  for(int ifl=0;ifl<nf;ifl++){
    for(map<pair<unsigned int,unsigned int>,string>::iterator it=fitpar[ifl].begin();it!=fitpar[ifl].end();it++){
      TkAlignParDB aligndb;
      int btime=1;
      int force=0;
      int stat=aligndb.LoadAlignPar((it->second).c_str(),0,btime,force);
      if(stat<0){cerr<<"ifl=0 err load="<<(it->second)<<endl;return -2;}
      map<int, map<unsigned int, TkAlignParS> > &paparss=aligndb.aparss;
      for(map<int, map<unsigned int, TkAlignParS> >::iterator it1=paparss.begin();it1!=paparss.end();it1++){
        if(ifl!=0){
          int detid0=-1;
          for(int ipar=0;ipar<npmax;ipar++){
            int detid=uid[ifl][ipar];
            if(detid<=0)continue;
            if(detid==it1->first){detid0=it1->first;break;}
          }
          if(detid0<=0)continue;
          if(it==fitpar[ifl].begin()){//only to once clean
            map<int, map<unsigned int, TkAlignParS> >::iterator it0=aparss.find(detid0);
            if(it0!=aparss.end()){aparss.erase(it0);}//aligndb2 has detid
          }
        }
        for(map<unsigned int, TkAlignParS>::iterator it2=(it1->second).begin();it2!=(it1->second).end();it2++){
           aparss[it1->first][it2->first]=it2->second;//copy
        } 
      }
    }
  }

//-------find etimes
  map<pair<unsigned int,int>,int> etimes;//etime,detid,size
  for(map<int, map<unsigned int, TkAlignParS> >::iterator it1=aparss.begin();it1!=aparss.end();it1++){//refine time tag
    map<unsigned int, TkAlignParS> aparsn;
    map<unsigned int, TkAlignParS>::iterator itpr;
    for(map<unsigned int, TkAlignParS>::iterator it=(it1->second).begin();it!=(it1->second).end();it++){
      if(aparsn.size()>=1){
        long int atimen=(itpr->second).atime[1];//pre etime
        atimen+=(it->second).atime[0];//now btime
        atimen/=2;
        aparsn[atimen]=it->second;//now time changed
      }
      else aparsn[it->first]=it->second;
      itpr=it;
      unsigned int et=(it->second).atime[1];
      etimes[make_pair(et,it1->first)]=3+(it->second).GetSize();
    }
    it1->second=aparsn;
  }
  map<int,pair<unsigned int,int> >dtimes;//detid,etime,totsize,the last2
  vector<unsigned int> setime;//detid etime
  int dsize[2]={2,2};
  for(map<pair<unsigned int,int>,int>::iterator it=etimes.begin();it!=etimes.end();it++){//sort by endtime
    dsize[0]+=it->second;
    if(dsize[0]>38000){//find break point,new start,12*(3+40*2)~996<2000 is reserved for the overlap
      dsize[1]=2+dsize[0];
      unsigned int mtime=0;
      for(map<int,pair<unsigned int,int> >::iterator it1=dtimes.begin();it1!=dtimes.end();it1++){
        unsigned int petime=(it1->second).first;
        if(mtime==0||petime<mtime){mtime=petime;dsize[0]=dsize[1]-(it1->second).second;}//earilest time
      }
      setime.push_back(mtime);
    }
    dtimes[(it->first).second].first=(it->first).first;//detid,etime
    dtimes[(it->first).second].second=dsize[0];//detid,totsize
  }
//---dtimes
  map<int, map<int, map<unsigned int, TkAlignParS> > >aparsg;//group,detid,btime
  for(map<int, map<unsigned int, TkAlignParS> >::iterator it1=aparss.begin();it1!=aparss.end();it1++){//detid,btime
    int ipos=0;
    map<unsigned int, TkAlignParS> &aparp=(it1->second);
    map<unsigned int, TkAlignParS>::iterator it=aparp.begin();
    for(;it!=aparp.end();it++){
      if(ipos+1<=setime.size()&&(it->second).atime[1]>=setime[ipos]){aparsg[ipos++][it1->first][it->first]=it->second;it--;continue;}//pos,detid,btime,back to same pos
      aparsg[ipos][it1->first][it->first]=it->second;
    }
    aparp.erase(aparp.begin(),it);
  }
//-----Write
  char ofilel[1000];
  sprintf(ofilel,"%s/%s_splinefit.list",odir,dytitle);
  ofstream fbinl(ofilel);
  unsigned int prbetime[2]={0,0};
  for(map<int, map<int, map<unsigned int, TkAlignParS> > >::iterator it1=aparsg.begin();it1!=aparsg.end();it1++){
    TkAlignParDB aligndb;
    aligndb.aparss=it1->second;
    cout<<"block sizeabc="<<aligndb.aparss.size()<<","<<aligndb.GetSize(1)<<endl;
    aligndb.CopyToTDVBlock();
    unsigned int betime[2]={0};
    for(map<int, map<unsigned int, TkAlignParS> >::iterator it0=(it1->second).begin();it0!=(it1->second).end();it0++){
      if(betime[0]==0||(it0->second).begin()->first>betime[0]) {betime[0]=(it0->second).begin()->first;}
      if(betime[1]==0||((it0->second).rbegin()->second).atime[1]<betime[1]){betime[1]=((it0->second).rbegin()->second).atime[1];}
    }
    char ofiles[1000];
    sprintf(ofiles,"%s/%s_splinefit_%u_%u.res",odir,dytitle,betime[0],betime[1]);
    aligndb.StreamAlignTDV(ofiles);
    fbinl<<betime[0]<<" "<<betime[1]<<" "<<ofiles<<endl;
    if(prbetime[1]!=0&&betime[0]>prbetime[1])cerr<<"<<-----------Error-missing-timeblock="<<" pr="<<prbetime[0]<<","<<prbetime[1]<<" nt="<<betime[0]<<","<<betime[1]<<endl;
    prbetime[0]=betime[0];
    prbetime[1]=betime[1];
  }
  fbinl.close();
  return 0;
}

int DrawPreDyMis(){
  const int nfl=1;
  const char *fname[]={
    "/eos/ams/group/mit/qyan/Data/alignment_BTB1130_P7N0Exte/alignment_BTB1130_P7N0Exte_0.root",
  };
  const int nh=3;
  const char *hname[nh]={
    "adt",
    "accev",
    "cosrig",
  };
  const char *xtitle[nh]={
    "#Delta_{t}",
    "Number of Events for Alignment per #Delta_{t}",
    "Rigidity [GV]"
  };
  TH1D *h[nfl][nh]={0};
  for(int ifl=0;ifl<nfl;ifl++){
    TFile *f=new TFile(fname[ifl]);
    f->cd();
    for(int ih=0;ih<nh;ih++){
      h[ifl][ih]=(TH1D *)f->Get(hname[ih]);
    }
  }
//-----
  gStyle->SetOptStat(1110);
  set_frame_style();
  for(int ih=0;ih<nh;ih++){
    TCanvas *c=new TCanvas();
    if(ih==2)gPad->SetLogx();
    int ip=0;
    for(int ifl=0;ifl<nfl;ifl++){
      TH1D *hn=h[ifl][ih];
      if(hn==0)continue;
      set_frame_style(hn);
      if(ip==0)hn->Draw();
      else     hn->Draw("same");
      hn->SetTitle(0);
      hn->SetLineColor(2);
      hn->SetLineWidth(2);
      hn->GetXaxis()->SetTitle(xtitle[ih]);
      hn->GetXaxis()->SetTitleColor(1);
      hn->GetXaxis()->SetTitleOffset(1.2);
      hn->GetYaxis()->SetTitleOffset(1.2);
      ip++;
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
//   "/eos/ams/group/mit/qyan/Data/alignment_BTB1130_PR400N0Allo_maxspfs19xafter2rig2.root",
//   "/eos/ams/group/mit/qyan/Data/alignment_BTB1130_PR400N0Allo_maxsplooseafter2rig.root",
   "/eos/ams/group/mit/qyan/Data/alignment_BTB1130_PR400Allo_maxspafter2nrig2.root",
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
  set_frame_style();
  TH1D *ha[nfl]={0};
  for(int ifl=0;ifl<nfl;ifl++){
    c=new TCanvas();
    TH2D *hn=h2r[ifl];
    if(!hn)continue;
    set_frame_style(hn);
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
    set_frame_style(hn);
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
