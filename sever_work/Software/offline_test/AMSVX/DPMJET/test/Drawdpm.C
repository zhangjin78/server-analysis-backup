#include <TLine.h>
#include <TSystem.h>
#include <TStyle.h>
#include <iostream>
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
#include "TCanvas.h"
#include "TSpline.h"
#include "TGraphErrors.h"
#include "TCanvas.h"
#include "TAxis.h"


int Drawdpm(){

    gStyle->SetOptStat(0);
    char histn[1000];
/*    const int nfl=2;
    const char *filen[]={//open file
      "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS301_6R3N/DPMJET/dpmjet3_0vdev/test/dpmjettest_p-1k1_6.root",  
      "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS301_6R3N/DPMJET/dpmjet3_0/test/dpmjettest_p-1k1_6.root",
    };
    char *lengnf[]={"E^{exc} Norm","E^{exc} Bias","3.2017-1(Fixed)","3.2017-1(Table)"};*/
/*    const int nfl=2;
    const char *filen[]={//open file*/
/*       "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS301_6R3N/DPMJET/dpmjet3_0vdev/testvdev/dpmjettest_p4k0.root",
       "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS301_6R3N/DPMJET/dpmjet3_0vdev/test/dpmjettest_p-1k1_6.root",*/
/*       "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS301_6R3N/DPMJET/dpmjet3_0vdev/testvdev/dpmjettest_p3k0.root",
       "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS301_6R3N/DPMJET/dpmjet3_0vdev/test/dpmjettest_p-1k1_6.root",*/
/*       "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS301_6R3N/DPMJET/dpmjet3_0vdev/testvdev/dpmjettest_p1k0.root",
       "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS301_6R3N/DPMJET/dpmjet3_0vdev/test/dpmjettest_p-1k1_6.root",
    };
//    char *lengnf[nfl]={"DPM3.0-6","DPM3.0-6(Table)","DPM3.0-6(Card)"};
    char *lengnf[]={"Fixed Target","Varied Target(Table)","DPM3.0-6(Card)"};*/
    const int nfl=3;
    const char *filen[]={//open file
/*      "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS301_6R3N/DPMJET/dpmjet3_0vdev/test/dpmjettest_p-1k1_6.root",
      "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS301_6R3N/DPMJET/dpmjet3_0vdev/test/dpmjettest_p-1k1_7.root",*/
      "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS301_6R3QF/DPMJET/dpmjet3_0/testbic/hmodeltest_p-1.root",
      "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS301_6R3QF/DPMJET/dpmjet3_0/testbic1/hmodeltest_p-1.root",
      "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS301_6R3QF/DPMJET/dpmjet3_0/testbic2/hmodeltest_p-1.root",
/*      "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS301_6R3N/DPMJET/dpmjet3_0vdevdump3/test/dpmjettest_p-1k1_05.root",
      "/afs/cern.ch/work/q/qyan/AMSVDEV/AMS301_6R3N/DPMJET/dpmjet3_0vdevdump3/test/dpmjettest_p-1k1_20.root",*/
    };
    char *lengnf[]={"E^{exc} Norm","E^{exc}/2","E^{exc}#times2"};
//------
    const int nh=10;
    const char *hname[nh]={
/*      "fragzmap0f",
      "fragamap0f",
      "fragzmap0mf",
      "fragamap0mf",
      "fragnpar0f",
      "fragen0f",
      "fragen0vf",
      "frageva0vmf",
      "fragzva0f",
      "fragzva0mf",*/
/*      "fragzmap2f",
      "fragamap2f",
      "fragzmap2mf",
      "fragamap2mf",
      "fragnpar2f",
      "fragen2f",
      "fragen2vf",
      "frageva2vmf",
      "fragzva2f",
      "fragzva2mf",*/
//-------
      "fragzmap4f",
      "fragamap4f",
      "fragzmap4mf",
      "fragamap4mf",
      "fragnpar4f",
      "fragen4f",
      "fragen4vf",
      "frageva4vmf",
      "fragzva4f",
      "fragzva4mf",
    };
    char *titlenx[nh]={
      "Charge Z",
      "Mass Number A",
      "Charge Z of Secondary",
      "Mass Number A of Secondary",
      "Number of Secondary",
      "Energy[GeV]",
      "Energy/Gen",
      "Kenetic-Energy Secondary/Primary",
      "Z/A",
      "Z/A",
    };
//------
    const int nh2=2;
    const char *hname2[]={
/*     "fragzamap2f",
     "fragzamap2mf",*/
/*     "fragzamap1f",
     "fragzamap1mf",*/
     "fragzamap8f",
     "fragzamap8mf",
    }; 

//---Hist
   TH1D *h[nh][nfl]={0};
   TH2D *h2[nh2][nfl]={0};
   TH1D *h21[nh2][nfl]={0};
   for(int ifl=0;ifl<nfl;ifl++){
     TFile *f=new TFile(filen[ifl]);
     f->cd();
     for(int ih=0;ih<nh;ih++){
       h[ih][ifl]=(TH1D *)f->Get(hname[ih]);
     }
     for(int ih=0;ih<nh2;ih++){
       h2[ih][ifl]=(TH2D *)f->Get(hname2[ih]);
     }
   }

 int ucolor[]={1,2,4,1,8,2,6,1,2,4,1,3,2,kOrange,kViolet,29,39,49};
 for(int ih=0;ih<nh;ih++){
   TCanvas *canvas=new TCanvas();
   TLegend* legend = new TLegend(0.55,0.55,0.8,0.88,"","brNDC");
   legend->SetBorderSize(0);
   legend->SetFillColor(0);
   legend->SetBorderSize(0);
   int ip=0;
   for(int ifl=0;ifl<nfl;ifl++){
      TH1D *hn=h[ih][ifl];
      if(hn==0)continue;
      hn->Sumw2();
      hn->SetLineColor(ucolor[ifl]);
      hn->SetLineWidth(2);
      if(hn->Integral(1,hn->GetNbinsX())<=1)continue;
      if(ip==0)hn=(TH1D *)hn->DrawNormalized("HistE");
      else     hn=(TH1D *)hn->DrawNormalized("HistEsame");
      hn->GetXaxis()->SetTitle(titlenx[ih]);
      hn->GetXaxis()->SetTitleColor(1);
      hn->GetYaxis()->SetTitle("Normalized Entries"); 
      ip++;
      legend->AddEntry(hn,lengnf[ifl],"L"); 
   }
   legend->Draw("same");
 }

 for(int ih=0;ih<nh2;ih++){
   for(int ifl=0;ifl<nfl;ifl++){
      TCanvas *canvas=new TCanvas();
      gPad->SetLogz();
      TH2D *hn=h2[ih][ifl];
      hn->GetXaxis()->SetTitle("Charge Z");
      hn->GetXaxis()->SetTitleColor(1);
      hn->GetYaxis()->SetTitle("Mass Number A");
      if(hn==0)continue;
      hn->Draw("COLZ");
//--------
      sprintf(histn,"%sh%df%d",hn->GetName(),ih,ifl);
      TH1D *hn1=(TH1D *)hn->ProjectionY(histn);
      hn1->Reset();
      for(int ibx=1;ibx<=hn->GetNbinsX();ibx++){
        double zv=hn->GetXaxis()->GetBinLowEdge(ibx);
        for(int iby=1;iby<=hn->GetNbinsY();iby++){
           double av=hn->GetYaxis()->GetBinLowEdge(iby);
           double cv=hn->GetBinContent(ibx,iby);
           double xv=av+zv-1;
           cout<<"zv="<<zv<<" av="<<av<<" cv="<<cv<<endl;
           if(zv+0.5>3)continue;
           hn1->Fill(xv+0.5,cv);
        }
      }
      h21[ih][ifl]=hn1;
   }
 }

  for(int ih=0;ih<nh2;ih++){
   TCanvas *canvas=new TCanvas();
   TLegend* legend = new TLegend(0.55,0.55,0.8,0.88,"","brNDC");
   legend->SetBorderSize(0);
   legend->SetFillColor(0);
   legend->SetBorderSize(0);
   int ip=0;
   for(int ifl=0;ifl<nfl;ifl++){
      TH1D *hn=h21[ih][ifl];
      if(hn==0)continue;
      hn->Sumw2();
      hn->SetLineColor(ucolor[ifl]);
      hn->SetLineWidth(2);
      if(hn->Integral(1,hn->GetNbinsX())<=1)continue;
      if(ip==0)hn=(TH1D *)hn->DrawNormalized("HistE");
      else     hn=(TH1D *)hn->DrawNormalized("HistEsame");
      hn->GetXaxis()->SetTitle(titlenx[ih]);
      hn->GetXaxis()->SetTitleColor(1);
      hn->GetYaxis()->SetTitle("Normalized Entries");
      hn->GetXaxis()->SetTitle("A+Z-1");
      hn->GetXaxis()->SetTitleColor(1);
      ip++;
      legend->AddEntry(hn,lengnf[ifl],"L");
   }
   legend->Draw("same");
  }

 return 0;
}
