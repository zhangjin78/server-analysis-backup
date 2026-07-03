#include "Fit2DH.h"
#include "TrChargeDBH.h"

int Fit2DH::SkipOpt=1; //0:no skip; 1:skip <=0; 2:skip >=0; 3:skip <0; 4:skip >0; 5:skip 0;
bool Fit2DH::ErrCal=false;

void Fit2DH::Init(){
   MSplineFitSamplingH::fUseAverage=false;

   xfirst=true;
   fN=0;
   for(int i=0;i<1000;i++){
      val_sec[i]=-1.0e20;
      hImport1D[i]=0;
      gImport1D[i]=0;
      fMSplineH_fir[i]=0;
   }
   fMSplineH_sec=0;
   hImport2D=0;
   fFunction=0;
   fXFunction=0;
   fYFunction=0;

   fitresidual[0]=0;
   fitresidual[1]=0;

   min_fir=0;
   max_fir=0;
   min_sec=0;
   max_sec=0;

   //nbins_sec=0;
   logx_sec=false;
   logy_sec=false;
   extralow_sec=kLinear;
   extrahig_sec=kFlat;
   blow_sec=0;
   bhig_sec=0;
}
void Fit2DH::Release(){
   for(int i=0;i<fN;i++){
      if(hImport1D[i]) delete hImport1D[i];
      if(gImport1D[i]) delete gImport1D[i];
      if(fMSplineH_fir[i]) delete fMSplineH_fir[i];
   }
   if(fMSplineH_sec) {delete fMSplineH_sec; fMSplineH_sec=0;}
   if(hImport2D) delete hImport2D;
   if(fFunction) delete fFunction;
   if(fXFunction) delete fXFunction;
   if(fYFunction) delete fYFunction;
   if(fitresidual[0]) delete fitresidual[0];
   if(fitresidual[1]) delete fitresidual[1];
}
void Fit2DH::Add(TGraphErrors* graph, double val0_sec,int nbins, double* bins, double xmin, double xmax, bool logx, bool logy, int extralow, int extrahig, double blow, double bhig){
   if(fN>=1000) {printf("Adding Too many! exit...\n"); return;}
   if(!graph) return;
   if(graph->GetN()<3) {printf("%s has too small points(%i,%i)!\n",graph->GetName(),graph->GetN(),nbins+3); return;}

   bool sampling=true;
   int nbins0=nbins;
   double bins0[200];
   if(nbins>0) for(int ii=0;ii<=nbins;ii++) bins0[ii]=bins[ii];
   else{
      if(false){
         double maxerror=0;
         for(int ii=0;ii<graph->GetN();ii++){
            if(graph->GetY()[ii]>0){
               double error=graph->GetEY()[ii]/graph->GetY()[ii];
               if(error>maxerror) maxerror=error;
            }
         }
         if(maxerror<0.03){
            sampling=false;
            nbins0=graph->GetN();
            for(int ii=0;ii<nbins0;ii++) bins0[ii]=graph->GetX()[ii];
         }
         //else{
         //   double xlow=graph->GetX()[0];
         //   double xhig=graph->GetX()[graph->GetN()-1];
         //   double xbuff[]={0,0.02,0.1,0.18,0.26,0.34,0.42,0.5};
         //   double xbuff2[]={-0.5,-0.42,-0.34,-0.26,-0.18,-0.1,-0.02,0};
         //   double xbuff3[]={-0.5,-0.42,-0.34,-0.26,-0.18,-0.1,-0.02,0.02,0.1,0.18,0.26,0.34,0.42,0.5};
         //   bool foldeta=(!(xlow<-0.3&&xhig>0.3));
         //   bool positive=(!foldeta)?true:(xlow<-0.3?false:true);
         //   nbins0=foldeta?(positive?7:7):13;
         //   for(int ii=0;ii<=nbins0;ii++) bins0[ii]=foldeta?(positive?xbuff[ii]:xbuff2[ii]):xbuff3[ii];
         //}
      }
      if(true){
         double xbuff0[]={-0.5,-0.46,-0.42,-0.38,-0.33,-0.29,-0.25,-0.21,-0.17,-0.12,-0.08,-0.04,0};
         double xbuff1[]={0,0.04,0.08,0.12,0.17,0.21,0.25,0.29,0.33,0.38,0.42,0.46,0.5};
         double xbuff2[]={-0.5,-0.46,-0.42,-0.38,-0.33,-0.29,-0.25,-0.21,-0.17,-0.12,-0.08,-0.04,0,0,0.04,0.08,0.12,0.17,0.21,0.25,0.29,0.33,0.38,0.42,0.46,0.5};
         double xlow=graph->GetX()[0];
         double xhig=graph->GetX()[graph->GetN()-1];
         bool foldeta=(!(xlow<0&&xhig>0));
         bool positive=(!foldeta)?true:(xlow<0?false:true);
         int np=foldeta?(positive?13:13):25;
         double xbuff[30];
         for(int ii=0;ii<np;ii++) xbuff[ii]=foldeta?(positive?xbuff1[ii]:xbuff0[ii]):xbuff2[ii];

         nbins0=0;
         bins0[nbins0]=TMath::Max(xbuff[0],graph->GetX()[0]+0.001);
         for(int ii=1;ii<np;ii++){
            double binlow=bins0[nbins0];
            double binhig=xbuff[ii];
            int npgraph=0;
            double maxerror=0;
            double minerror=100;
            double aveerror=0;
            for(int i2=0;i2<graph->GetN();i2++){
               if(graph->GetX()[i2]>binhig) break;
               if(graph->GetX()[i2]>=binlow&&graph->GetX()[i2]<binhig){
                  if(graph->GetY()[i2]<=0) continue;
                  double ierror=graph->GetEY()[i2]/graph->GetY()[i2];
                  if(ierror>maxerror) maxerror=ierror;
                  if(ierror<minerror) minerror=ierror;
                  aveerror+=ierror;
                  npgraph++;
               }
            }
            if(ii==np-1){
               if(npgraph<=0) break;
               nbins0++;
               bins0[nbins0]=graph->GetX()[graph->GetN()-1]-0.001;
               break;
            }
            if(npgraph>0){
               aveerror/=npgraph;
               if(minerror>0.03&&aveerror>0.05) continue;
               nbins0++;
               bins0[nbins0]=binhig;
            }
         }
         //printf("(v2=%.3f %d(%.2f,%.2f),%d):",val0_sec,nbins0,xbuff[0],xbuff[np-1],graph->GetN());
         //for(int ii=0;ii<=nbins0;ii++){
         //   printf("%.2f%s",bins0[ii],ii==nbins0?"\n":",");
         //}
         //for(int ii=0;ii<graph->GetN();ii++){
         //   printf("%.2f%s",graph->GetX()[ii],ii+1==graph->GetN()?"\n":",");
         //}

         if(nbins0==graph->GetN()){
            sampling=false;
            nbins0=graph->GetN();
            for(int ii=0;ii<nbins0;ii++) bins0[ii]=graph->GetX()[ii];
         }
      }
   }

   if(xmax<=xmin){
      if(max_fir<=min_fir){
         min_fir=bins0[0];
         max_fir=bins0[nbins0];
      }
      xmin=min_fir;
      xmax=max_fir;
   }
   //printf("Adding Graph %s: nbins=%i xmin=%g xmax=%g\n",graph->GetName(),nbins0,xmin,xmax);
   //for(int ii=0;ii<=nbins0;ii++) printf("%g,",bins0[ii]);
   //cout<<endl;

   fMSplineH_fir[fN]=new MSplineFitSamplingH(graph);
   fMSplineH_fir[fN]->SetName(Form("MSplineFitSamplingH_fir_%i",fN));
   if(sampling) fMSplineH_fir[fN]->Fit(nbins0,bins0,logx,logy,100,xmin,xmax,extralow,extrahig,blow,bhig);
   else      fMSplineH_fir[fN]->Fit(nbins0,bins0,logx,logy,1,xmin,xmax,extralow,extrahig,blow,bhig,0,false);
   fMSplineH_fir[fN]->BuildFunction(1000,xmin,xmax);

   gImport1D[fN]=(TGraphErrors*)graph->Clone(Form("gImport1D_%i_%s",fN,graph->GetName()));
   val_sec[fN]=val0_sec;
   fN++;
}
void Fit2DH::Add(TH1D* histo, double val0_sec,int nbins, double* bins, double xmin, double xmax, bool logx, bool logy, int extralow, int extrahig, double blow, double bhig){
   if(fN>=1000) {printf("Adding Too many! exit...\n"); return;}
   if(!histo) return;

   hImport1D[fN]=(TH1D*)histo->Clone(Form("hImport1D_%i_%s",fN,histo->GetName()));
   hImport1D[fN]->SetDirectory(0);
   TGraphErrors* graph=new TGraphErrors();
   graph->SetTitle(Form(";%s;%s;%s",histo->GetXaxis()->GetTitle(),histo->GetYaxis()->GetTitle(),histo->GetTitle()));
   int np=0;
   for(int ibin=1;ibin<=histo->GetNbinsX();ibin++){
      double val=histo->GetBinContent(ibin);
      double eval=histo->GetBinError(ibin);
      if(SkipOpt>0){
         if(val<=0&&SkipOpt==1) continue;
         if(val>=0&&SkipOpt==2) continue;
         if(val<0&&SkipOpt==3) continue;
         if(val>0&&SkipOpt==4) continue;
         if(val==0&&SkipOpt==5) continue;
      }
      graph->SetPoint(np,histo->GetXaxis()->GetBinCenter(ibin),val);
      graph->SetPointError(np,0,eval);
      np++;
   }
   if(np<3){
      printf("%s has too small points(%i,%i)!\n",histo->GetName(),np,nbins+3);
      delete hImport1D[fN];
      delete graph;
      return;
   }
   else{
      if(xmax<=xmin){
         xmin=histo->GetXaxis()->GetBinLowEdge(1);
         xmax=histo->GetXaxis()->GetBinUpEdge(histo->GetNbinsX());
      }
      Add(graph,val0_sec,nbins,bins,xmin,xmax,logx,logy,extralow,extrahig,blow,bhig);
   }

   if(graph) delete graph;
}
void Fit2DH::Add(TH2D* histo, int nxbin, double* xbins, int nybin, double *ybins, double xmin, double xmax, double ymin,double ymax, int rebinx, int rebiny, bool logx, bool logy, bool logz, int extraxlow, int extraxhig, double bxlow, double bxhig, int extraylow, int extrayhig, double bylow, double byhig){
   if(!histo) return;

   if(xmax<=xmin){
      xmin=histo->GetXaxis()->GetBinLowEdge(1);
      xmax=histo->GetXaxis()->GetBinUpEdge(histo->GetNbinsX());
   }
   if(ymax<=ymin){
      ymin=histo->GetYaxis()->GetBinLowEdge(1);
      ymax=histo->GetYaxis()->GetBinUpEdge(histo->GetNbinsY());
   }

   hImport2D=(TH2D*)histo->Clone(Form("hImport2D_%s",histo->GetName()));
   hImport2D->SetDirectory(0);
   if(rebinx>1||rebiny>1) { hImport2D->Rebin2D(rebinx,rebiny); }
   if(xfirst){
      for(int ibiny=1;ibiny<=hImport2D->GetNbinsY();ibiny++){
         TH1D* hproj=hImport2D->ProjectionX(Form("ybin%i_%s",ibiny,hImport2D->GetName()),ibiny,ibiny);
         hproj->SetTitle(Form("y=%g",hImport2D->GetYaxis()->GetBinCenter(ibiny)));
         Add(hproj,hImport2D->GetYaxis()->GetBinCenter(ibiny),nxbin,xbins,xmin,xmax,logx,logz,extraxlow,extraxhig,bxlow,bxhig);
         if(hproj) delete hproj;
      }
      min_fir=xmin;
      max_fir=xmax;
      min_sec=ymin;
      max_sec=ymax;
      //nbins_sec=nybin;
      //for(int i=0;i<=nbins_sec;i++) bins_sec[i]=ybins[i];
      logx_sec=logy;
      logy_sec=logz;
      extralow_sec=extraylow;
      extrahig_sec=extrayhig;
      blow_sec=bylow;
      bhig_sec=byhig;
   }
   else{
      for(int ibinx=1;ibinx<=hImport2D->GetNbinsX();ibinx++){
         TH1D* hproj=hImport2D->ProjectionY(Form("xbin%i_%s",ibinx,hImport2D->GetName()),ibinx,ibinx);
         hproj->SetTitle(Form("x=%g",hImport2D->GetXaxis()->GetBinCenter(ibinx)));
         Add(hproj,hImport2D->GetXaxis()->GetBinCenter(ibinx),nybin,ybins,ymin,ymax,logy,logz,extraylow,extrayhig,bylow,byhig);
         if(hproj) delete hproj;
      }
      min_fir=ymin;
      max_fir=ymax;
      min_sec=xmin;
      max_sec=xmax;
      //nbins_sec=nxbin;
      //for(int i=0;i<=nbins_sec;i++) bins_sec[i]=xbins[i];
      logx_sec=logx;
      logy_sec=logz;
      extralow_sec=extraxlow;
      extrahig_sec=extraxhig;
      blow_sec=bxlow;
      bhig_sec=bxhig;
   }
}
int Fit2DH::SearchIndex(double value_sec){
   int index=-1;
   double mindis=1.0e20;
   for(int i=0;i<fN;i++){
      double dis=fabs(value_sec-val_sec[i]);
      if(dis<mindis){
         mindis=dis;
         index=i;
      }
   }
   return index;
}
void Fit2DH::reordering(){
   for(int i0=fN-1;i0>=1;i0--){
      int index=-1;
      double maximum=-1.0e20;
      for(int i1=0;i1<=i0;i1++){
         if(val_sec[i1]>maximum){
            index=i1;
            maximum=val_sec[i1];
         }
      }
      if(index>=0&&(index!=i0)){
         double vbuff=val_sec[i0];
         TH1D*  h1Dbuff=hImport1D[i0];
         TGraphErrors*  g1Dbuff=gImport1D[i0];
         MSplineFitSamplingH* fbuff=fMSplineH_fir[i0];

         val_sec[i0]=val_sec[index];
         hImport1D[i0]=hImport1D[index];
         gImport1D[i0]=gImport1D[index];
         fMSplineH_fir[i0]=fMSplineH_fir[index];

         val_sec[index]=vbuff;
         hImport1D[index]=h1Dbuff;
         gImport1D[index]=g1Dbuff;
         fMSplineH_fir[index]=fbuff;
      }
   }
   return;
}

double Fit2DH::Eval(double &error,double xx,double yy,bool UseNewAlgo){
   error=0;
   if(fN<1) return 0;
   double val2=(xfirst)?yy:xx;
   //if(val2<min_sec||val2>max_sec) return 0;
   double val1=(xfirst)?xx:yy;

   if(UseNewAlgo){
      if(val2<=val_sec[0]){
         //specific for angle dependence, assuming the dE proportional to path length sqrt(1+pow(dxdz,2))
         double yval0=fMSplineH_fir[0]->Eval(val1);
         if(fN<2) return yval0;
         double yval1=fMSplineH_fir[1]->Eval(val1);
         bool slpos=((yval1-yval0)*(val_sec[1]-val_sec[0])>0);
         bool norm=(val_sec[0]>0)?slpos:(!slpos);
         if(norm) return (yval0/sqrt(1+val_sec[0]*val_sec[0])*sqrt(1+val2*val2));
         else return yval0;
      }
      else if(val2>=val_sec[fN-1]){
         double yval0=fMSplineH_fir[fN-1]->Eval(val1);
         if(fN<2) return yval0;
         double yval1=fMSplineH_fir[fN-2]->Eval(val1);
         bool slpos=((yval1-yval0)*(val_sec[fN-2]-val_sec[fN-1])>0);
         bool norm=(val_sec[fN-1]>0)?slpos:(!slpos);
         if(norm) return (yval0/sqrt(1+val_sec[fN-1]*val_sec[fN-1])*sqrt(1+val2*val2));
         else return yval0;
      }
      else{
         int left=0; int right=fN-1;
         while(abs(right-left)>1){
            int middle=(int)((left+right)/2.);
            if(val2<=val_sec[middle]) {right=middle;}
            else {left=middle;}
         }
         double frac=(val2-val_sec[left])/(val_sec[right]-val_sec[left]);
         double yval1=fMSplineH_fir[left]->Eval(val1);
         double yval2=fMSplineH_fir[right]->Eval(val1);
         return frac*yval2+(1-frac)*yval1;
      }
   }
   else{
      const int nbin_sec=100;
      if(fN>nbin_sec) return 0;
      double xknots[nbin_sec];
      double yknots[nbin_sec];
      double eyknots[nbin_sec];
      //double* xknots=new double[fN];
      //double* yknots=new double[fN];
      //double* eyknots=new double[fN];
      int np=0;
      for(int i=0;i<fN;i++){
         double val1=(xfirst)?xx:yy;
         double content=0,econtent=0;
         //if(val1>=min_fir&&val1<=max_fir) content=fMSplineH_fir[i]->Eval(val1);
         content=fMSplineH_fir[i]->Eval(val1);
         if(ErrCal) econtent=fMSplineH_fir[i]->EvalError(val1);
         if(SkipOpt>0){
            if(content<=0&&SkipOpt==1) continue;
            if(content>=0&&SkipOpt==2) continue;
            if(content<0&&SkipOpt==3) continue;
            if(content>0&&SkipOpt==4) continue;
            if(content==0&&SkipOpt==5) continue;
         }
         xknots[np]=val_sec[i];
         yknots[np]=content;
         eyknots[np]=econtent;
         if ( (logx_sec)&&(xknots[np]<=0) ) printf("Fit2DH::set Logx_sec but x knot is <= 0 (%f)\n",xknots[np]);
         if ( (logy_sec)&&(yknots[np]<=0) ) printf("Fit2DH::set Logy_sec but y knot is <= 0 (%f)\n",yknots[np]);
         np++;
      }
      if(np<1){
         //delete [] xknots;
         //delete [] yknots;
         //delete [] eyknots;
         return 0;
      }
      //error calculation
      if(ErrCal){
         if(val2<xknots[0]) error=eyknots[0];
         else if(val2>xknots[np-1]) error=eyknots[np-1];
         else{
            for(int ip=0;ip<np-1;ip++){
               if(val2>=xknots[ip]&&val2<xknots[ip+1]){
                  double frac=logx_sec?( (log10(xknots[ip+1])-log10(val2)) / (log10(xknots[ip+1])-log10(xknots[ip])) ):( (xknots[ip+1]-val2) / (xknots[ip+1]-xknots[ip]) );
                  error=eyknots[ip]*frac+(1-frac)*eyknots[ip+1];
                  break;
               }
            }
         }
      }
      MSplineH* fMSplineH_sec2=new MSplineH(np,xknots,yknots,logx_sec,logy_sec,extralow_sec,extrahig_sec,blow_sec,bhig_sec);
      double result=fMSplineH_sec2->Eval(val2);
      //to smooth the angle dependence
      if(xfirst&&np>=3){  //then yy should be the theta, and xx should be eta
         double xminval,xmaxval;
         double minval=1.0e10;
         double maxval=-1.0e10;
         for(int i=0;i<np;i++){
            if(yknots[i]>maxval) {maxval=yknots[i]; xmaxval=xknots[i];}
            if(yknots[i]<minval) {minval=yknots[i]; xminval=xknots[i];}
         }
         double slope=0;
         double threshold0=fabs(2*val2/(1+val2*val2))*0.5;
         if(val2<xknots[0]&&extralow_sec==MSplineH::kLinear){ //lower extrapolation
            if((yknots[0]-yknots[1])*(xknots[0]-xknots[1])>0){
               slope=(maxval-yknots[0])/(xmaxval-xknots[0]);
            }
            if((yknots[0]-yknots[1])*(xknots[0]-xknots[1])<0){
               slope=(minval-yknots[0])/(xminval-xknots[0]);
            }
            if(fabs(slope/yknots[0])>threshold0) slope=slope>0?fabs(yknots[0]*threshold0):(-fabs(yknots[0]*threshold0));
            result=yknots[0]+slope*(val2-xknots[0]);
         }
         if(val2>xknots[np-1]&&extrahig_sec==MSplineH::kLinear){ //higher extrapolation
            if((yknots[np-1]-yknots[np-2])*(xknots[np-1]-xknots[np-2])>0){
               slope=(minval-yknots[np-1])/(xminval-xknots[np-1]);
            }
            if((yknots[np-1]-yknots[np-2])*(xknots[np-1]-xknots[np-2])<0){
               slope=(maxval-yknots[np-1])/(xmaxval-xknots[np-1]);
            }
            if(fabs(slope/yknots[np-1])>threshold0) slope=slope>0?fabs(yknots[np-1]*threshold0):(-fabs(yknots[np-1]*threshold0));
            result=yknots[np-1]+slope*(val2-xknots[np-1]);
         }
      }
      //delete [] xknots;
      //delete [] yknots;
      //delete [] eyknots;
      delete fMSplineH_sec2;
      return result;
   }
}
double Fit2DH::operator() (double *x, double *par){
   double xx=x[0];
   double yy=x[1];
   double error;
   return Eval(error,xx,yy);
}
void Fit2DH::BuildFunction(double xmin, double xmax,double ymin,double ymax,int Np){
   static int function_index = 0;
   if (fFunction) delete fFunction;
   if(xmax<=xmin){
      xmin=xfirst?min_fir:min_sec;
      xmax=xfirst?max_fir:max_sec;
   }
   if(ymax<=ymin){
      ymin=xfirst?min_sec:min_fir;
      ymax=xfirst?max_sec:max_fir;
   }
   double xybuff=0;
   fFunction=new TF2(Form("mspline_fit_%05d",function_index),this,xmin,xmax,ymin,ymax,0,"Fit2DH");
   fXFunction=new TF12(Form("xmspline_fit_%05d",function_index),fFunction,xybuff,"x");
   fYFunction=new TF12(Form("ymspline_fit_%05d",function_index),fFunction,xybuff,"y");
   function_index++;
   fFunction->SetNpx(Np);
   fFunction->SetNpy(Np);
   fXFunction->SetNpx(Np);
   fYFunction->SetNpx(Np);
   return;
}

TH2D* Fit2DH::GetFitRes(bool sigmaunits){
   double xmin=xfirst?min_fir:min_sec;
   double xmax=xfirst?max_fir:max_sec;
   double ymin=xfirst?min_sec:min_fir;
   double ymax=xfirst?max_sec:max_fir;
   TH2D* hres=0;
   if(hImport2D){
      hres=(TH2D*)hImport2D->Clone(Form("fitresult_%s",sigmaunits?"nsigma":"percentage"));
      hres->Reset();
   }
   else{
      int nbin_fir=gImport1D[0]->GetN();
      double bins_fir[100];
      int nbin_sec=fN;
      double bins_sec[100];
      for(int ii=0;ii<=nbin_fir;ii++){
         double binwidth;
         if(ii+1<=nbin_fir-1){
            binwidth=(gImport1D[0]->GetX()[ii+1]-gImport1D[0]->GetX()[ii]);
         }
         else{
            binwidth=(gImport1D[0]->GetX()[nbin_fir-1]-gImport1D[0]->GetX()[nbin_fir-2]);
         }
         if(ii<nbin_fir) bins_fir[ii]=gImport1D[0]->GetX()[ii]-binwidth/2;
         else bins_fir[ii]=gImport1D[0]->GetX()[ii-1]+binwidth/2;
      }
      for(int ii=0;ii<=nbin_sec;ii++){
         double binwidth;
         if(ii+1<=nbin_sec-1){
            binwidth=(val_sec[ii+1]-val_sec[ii]);
         }
         else{
            binwidth=(val_sec[nbin_sec-1]-val_sec[nbin_sec-2]);
         }
         if(ii<nbin_sec) bins_sec[ii]=val_sec[ii]-binwidth/2;
         else bins_sec[ii]=val_sec[ii-1]+binwidth/2;
      }
      if(xfirst) hres=new TH2D(Form("fitresult_%s",sigmaunits?"nsigma":"percentage"),";x;y;ratio",nbin_fir,bins_fir,nbin_sec,bins_sec);
      else       hres=new TH2D(Form("fitresult_%s",sigmaunits?"nsigma":"percentage"),";x;y;ratio",nbin_sec,bins_sec,nbin_fir,bins_fir);
   }
   for(int ibinx=1;ibinx<=hres->GetNbinsX();ibinx++){
      for(int ibiny=1;ibiny<=hres->GetNbinsY();ibiny++){
         double xx=hres->GetXaxis()->GetBinCenter(ibinx);
         double yy=hres->GetYaxis()->GetBinCenter(ibiny);
         double fitvalue=fFunction->Eval(xx,yy);
         double value_orig,evalue_orig=0;
         if(hImport2D){
            value_orig=hImport2D->GetBinContent(ibinx,ibiny);
            evalue_orig=hImport2D->GetBinError(ibinx,ibiny);
         }
         else{
            if(xfirst) value_orig=gImport1D[SearchIndex(yy)]->Eval(xx);
            else       value_orig=gImport1D[SearchIndex(xx)]->Eval(yy);
         }
         double bincontent;
         if(sigmaunits) bincontent=(evalue_orig!=0)?((fitvalue-value_orig)/evalue_orig):0;
         else           bincontent=(fitvalue!=0)?(value_orig/fitvalue):0;
         hres->SetBinContent(ibinx,ibiny,bincontent);
      }
   }
   return hres;
}
int Fit2DH::IsFittingGood(double maxlimit,bool sigmaunits){
   if(!fitresidual[sigmaunits]){
      fitresidual[sigmaunits]=GetFitRes(sigmaunits);
   }
   if(fitresidual[sigmaunits]){
      int np=0;
      TH2D* hbuff=fitresidual[sigmaunits];
      for(int ibinx=1;ibinx<=hbuff->GetNbinsX();ibinx++){
         for(int ibiny=1;ibiny<=hbuff->GetNbinsY();ibiny++){
            if(!sigmaunits) {if(fabs(hbuff->GetBinContent(ibinx,ibiny)-1)>maxlimit) np++;}
            else            {if(hbuff->GetBinContent(ibinx,ibiny)>maxlimit) np++;}
         }
      }
      return np;
   }
   else return 0;
}
void Fit2DH::DrawXYFitting(bool sigmaunits){
   if(!fitresidual[sigmaunits]){
      fitresidual[sigmaunits]=GetFitRes(sigmaunits);
   }
   if(fitresidual[sigmaunits]){
      fitresidual[sigmaunits]->Draw("colz");
      if(gPad) gPad->Update();
      if(gPad) gPad->WaitPrimitive();
   }
}
void Fit2DH::DrawXFitting(double yy,bool drawfitting){
   //printf("drawing the fitting result for X axis when y=%g\n",yy);
   if(hImport2D){
      int biny=hImport2D->GetYaxis()->FindBin(yy);
      TH1D* hprojx=hImport2D->ProjectionX(Form("projx_%s",hImport2D->GetName()),biny,biny);
      hprojx->SetTitle(Form("x_%s",hImport2D->GetTitle()));
      if(drawfitting){
         fXFunction->SetXY(yy);
         fXFunction->Draw("");
      }
      //Tools::ApplyStyle(hprojx,4,4,1.0,1,1);
      hprojx->Draw(drawfitting?"same":"");
      if(gPad) gPad->Update();
      if(gPad) gPad->WaitPrimitive();
      delete hprojx;
   }
   else{
      TGraphErrors* gr=0;
      int yy0;
      if(xfirst){
         gr=(TGraphErrors*)gImport1D[SearchIndex(yy)]->Clone("ForFittingPlot");
         yy0=val_sec[SearchIndex(yy)];
      }
      else{
         gr=new TGraphErrors();
         for(int ii=0;ii<fN;ii++){
            gr->SetPoint(ii,val_sec[ii],gImport1D[ii]->Eval(yy));
         }
         yy0=yy;
      }
      if(drawfitting){
         fXFunction->SetXY(yy0);
         fXFunction->Draw("");
      }
      //Tools::ApplyStyle(gr,4,4,1.0,1,1);
      gr->Draw(drawfitting?"p":"ap");
      if(gPad) gPad->Update();
      if(gPad) gPad->WaitPrimitive();
      delete gr;
   }
   //printf("end drawing\n");
}
void Fit2DH::DrawXFitting(int biny,bool drawfitting){
   if(!xfirst){
      if(!hImport2D) return;
      if(biny<=0||biny>hImport2D->GetNbinsY()) return;
      TH1D* hprojx=hImport2D->ProjectionX(Form("biny%i_%s",biny,hImport2D->GetName()),biny,biny);
      hprojx->Draw("");
      hprojx->SetTitle(Form("y=%g",hImport2D->GetYaxis()->GetBinCenter(biny)));
      fXFunction->SetXY(hImport2D->GetYaxis()->GetBinCenter(biny));
      fXFunction->Draw("same");
      if(gPad) gPad->Update();
      if(gPad) gPad->WaitPrimitive();
      delete hprojx;
      return;
   }
   if(biny<=0||biny>fN) return;
   //printf("drawing the fitting result for X axis for biny=%i\n",biny);
   gImport1D[biny-1]->SetTitle(Form("y=%g",val_sec[biny-1]));
   gImport1D[biny-1]->Draw("ap");
   fMSplineH_fir[biny-1]->fFunction->Draw("same");
   TText text;
   text.DrawTextNDC(0.2,0.80,Form("Chisq=%.1f, maxdis=(%.1f[%%],%.1f[sigma],%.1f[%%])",GetChi2Max(biny,0),GetChi2Max(biny,1)*100,GetChi2Max(biny,2),GetChi2Max(biny,3)*100));
   if(gPad) gPad->Update();
   if(gPad) gPad->WaitPrimitive();
}
void Fit2DH::DrawYFitting(double xx,bool drawfitting){
   //printf("drawing the fitting result for Y axis when x=%g\n",xx);
   if(hImport2D){
      int binx=hImport2D->GetXaxis()->FindBin(xx);
      TH1D* hprojy=hImport2D->ProjectionY(Form("projy_%s",hImport2D->GetName()),binx,binx);
      hprojy->SetTitle(Form("y_%s",hImport2D->GetTitle()));
      //Tools::ApplyStyle(hprojy,4,4,1.0,1,1);
      hprojy->Draw(drawfitting?"":"");
      hprojy->SetTitle(Form("x=%g",xx));
      if(drawfitting){
         fYFunction->SetXY(xx);
         fYFunction->Draw("same");
      }
      if(gPad) gPad->Update();
      if(gPad) gPad->WaitPrimitive();
      delete hprojy;
   }
   else{
      TGraphErrors* gr=0;
      int xx0;
      if(!xfirst){
         gr=(TGraphErrors*)gImport1D[SearchIndex(xx)]->Clone("ForFittingPlot");
         xx0=val_sec[SearchIndex(xx)];
      }
      else{
         gr=new TGraphErrors();
         for(int ii=0;ii<fN;ii++){
            gr->SetPoint(ii,val_sec[ii],gImport1D[ii]->Eval(xx));
         }
         xx0=xx;
      }
      //Tools::ApplyStyle(gr,4,4,1.0,1,1);
      gr->Draw(drawfitting?"ap":"ap");
      if(drawfitting){
         fYFunction->SetXY(xx0);
         fYFunction->Draw("same");
      }
      if(gPad) gPad->Update();
      if(gPad) gPad->WaitPrimitive();
      delete gr;
   }
   //printf("end drawing\n");
}
void Fit2DH::DrawYFitting(int binx,bool drawfitting){
   if(xfirst){
      if(!hImport2D) return;
      if(binx<=0||binx>hImport2D->GetNbinsY()) return;
      TH1D* hprojy=hImport2D->ProjectionY(Form("binx%i_%s",binx,hImport2D->GetName()),binx,binx);
      hprojy->Draw("");
      hprojy->SetTitle(Form("x=%g",hImport2D->GetXaxis()->GetBinCenter(binx)));
      fYFunction->SetXY(hImport2D->GetXaxis()->GetBinCenter(binx));
      fYFunction->Draw("same");
      if(gPad) gPad->Update();
      if(gPad) gPad->WaitPrimitive();
      delete hprojy;
      return;
   }
   if(binx>fN||binx<1) return;
   //printf("drawing the fitting result for Y axis for binx=%i\n",binx);
   gImport1D[binx-1]->SetTitle(Form("x=%g",val_sec[binx-1]));
   gImport1D[binx-1]->Draw("ap");
   fMSplineH_fir[binx-1]->fFunction->Draw("same");
   TText text;
   text.DrawTextNDC(0.2,0.80,Form("Chisq=%.1f, maxdis=(%.1f[%%],%.1f[sigma],%.1f[%%])",GetChi2Max(binx,0),GetChi2Max(binx,1)*100,GetChi2Max(binx,2),GetChi2Max(binx,3)*100));
   if(gPad) gPad->Update();
   if(gPad) gPad->WaitPrimitive();
}
double Fit2DH::GetChi2Max(int bin,int type){
   double maxchi2=0;
   for(int ii=0;ii<fN;ii++){
      if(bin>=1&&bin<=fN&&bin!=ii+1) continue;
      double chi2=fMSplineH_fir[ii]->GetBestFit()->GetChi2(type);
      if(chi2>maxchi2) maxchi2=chi2;
   }
   return maxchi2;
}

bool Fit2DH::DumpMap(char* filename){
   FILE* fout=fopen(filename,"w");
   if(!fout) return false;
   //dump the global setting parameters
   fprintf(fout,"%d\t%d\t%+.2g\t%+.2g\t%+.2g\t%+.2g\t%d\t%d\t%d\t%d\t%+.2g\t%+.2g\n",fN,xfirst,min_fir,max_fir,min_sec,max_sec,logx_sec,logy_sec,extralow_sec,extrahig_sec,blow_sec,bhig_sec);
   for(int ii=0;ii<fN;ii++){
      //dump the spline parameters for each bin
      MSplineH* spl=fMSplineH_fir[ii]->GetBestFit();
      fprintf(fout,"%+.2g\t%d\t%d\t%d\t%d\t%d\t%+.2g\t%+.2g\t%g\t%g\t%g\t%g\n",val_sec[ii],spl->fN,spl->fLogx,spl->fLogy,spl->fExtraLow,spl->fExtraHig,spl->fBlow,spl->fBhig,spl->GetChi2(0),spl->GetChi2(1),spl->GetChi2(2),spl->GetChi2(3));
      for(int i2=0;i2<spl->fN;i2++) {fprintf(fout,"%.3g\t%.5g",spl->fX[i2],spl->fY[i2]); if(i2!=spl->fN-1) fprintf(fout,"\t"); else fprintf(fout,"\n");}
   }
   fclose(fout);
   return true;
}
bool Fit2DH::ReadMap(char* filename){
   FILE* fin=fopen(filename,"r");
   if(!fin) return false;
   //read the global setting parameters
   int xfirst_buff,logx_sec_buff,logy_sec_buff;
   fscanf(fin,"%d%d%lf%lf%lf%lf%d%d%d%d%lf%lf",&fN,&xfirst_buff,&min_fir,&max_fir,&min_sec,&max_sec,&logx_sec_buff,&logy_sec_buff,&extralow_sec,&extrahig_sec,&blow_sec,&bhig_sec);
   xfirst=xfirst_buff;
   logx_sec=logx_sec_buff;
   logy_sec=logy_sec_buff;
   //printf("%d\t%d\t%+.2g\t%+.2g\t%+.2g\t%+.2g\t%d\t%d\t%d\t%d\t%+.2g\t%+.2g\n",fN,xfirst,min_fir,max_fir,min_sec,max_sec,logx_sec,logy_sec,extralow_sec,extrahig_sec,blow_sec,bhig_sec);
   for(int ii=0;ii<fN;ii++){
      //read the spline parameters for each bin
      fMSplineH_fir[ii]=new MSplineFitSamplingH(0);
      fMSplineH_fir[ii]->fnSamples=1;
      fMSplineH_fir[ii]->fFlags=new int[fMSplineH_fir[ii]->fnSamples];
      fMSplineH_fir[ii]->fFlags[0]=0;
      int nknot,logx,logy,extralow,extrahig;
      double blow,bhig,chi2,percentage,nsigma,graphmax;
      fscanf(fin,"%lf%d%d%d%d%d%lf%lf%lf%lf%lf%lf\n",&(val_sec[ii]),&nknot,&logx,&logy,&extralow,&extrahig,&blow,&bhig,&chi2,&percentage,&nsigma,&graphmax);
      double xknot[100],yknot[100];
      for(int i2=0;i2<nknot;i2++) fscanf(fin,"%lf%lf",&(xknot[i2]),&(yknot[i2]));
      fMSplineH_fir[ii]->fSamples=new MSplineH*[fMSplineH_fir[ii]->fnSamples];
      fMSplineH_fir[ii]->fSamples[0]=new MSplineH(nknot,xknot,yknot,logx,logy,extralow,extrahig,blow,bhig);
      fMSplineH_fir[ii]->fSamples[0]->fitquality[0]=chi2;
      fMSplineH_fir[ii]->fSamples[0]->fitquality[1]=percentage;
      fMSplineH_fir[ii]->fSamples[0]->fitquality[2]=nsigma;
      fMSplineH_fir[ii]->fSamples[0]->fitquality[3]=graphmax;
   }
   double thetamin,thetamax;
   if(xfirst) {thetamin=TMath::Min(min_sec,-2.); thetamax=TMath::Max(max_sec,2.);}
   else       {thetamin=TMath::Min(min_fir,-2.); thetamax=TMath::Max(max_fir,2.);}
   if(xfirst) BuildFunction(min_fir,max_fir,thetamin,thetamax);
   else       BuildFunction(min_sec,max_sec,thetamin,thetamax);
   fclose(fin);
   return true;
}

