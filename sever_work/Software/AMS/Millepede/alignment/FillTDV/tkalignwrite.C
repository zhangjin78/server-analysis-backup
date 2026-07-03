//--author Q.Yan qyan@cern.ch fill data to database
#include <signal.h>
#include <TH1.h>
#include <TH2F.h>
#include <TF1.h>
#include <TGraphErrors.h>
#include <TApplication.h>
#include <TCanvas.h>
#include <TChain.h>
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
#include "root_RVSP.h"
#include "amschain.h"
#include "TkGeomN.h"
#include "timeid.h"
int main(int argc,char ** argv){//in_file ofile num

//--based on 400GeV/c proton TB calibration
/*   int vers=1;
   char *falign="/afs/cern.ch/work/q/qyan/work/alignment/after/merge3itersmallerrext_after2inv2olad.res";*/
   int vers=2;
   char *falign="/afs/cern.ch/work/q/qyan/work/alignment/after/merge3itersmallerrext_after2inv2fixsen19.res";
/*   int vers=3;
   char *falign="/afs/cern.ch/work/q/qyan/work/alignment/after/merge3itersmallerrext_after2inv2fixsen19x.res";*/
/*   int vers=4;
   char *falign="/afs/cern.ch/work/q/qyan/work/alignment/after/merge3itersmallerrext_after2inv2.res";*/
/*   int vers=5;
   char *falign="/afs/cern.ch/work/q/qyan/work/alignment/after/merge3itersmallerrext_after2inv3.res";*/
//   int btime=1281355854;//2010-08-09
   long int btime=1167609600;  //manual set to 2007-01-01
   long int etime=1900000000-1;//manual set to 2030-03-17
//   int etime=2000000000;
   TkTrackN *tkgeom=TkTrackN::GetHead();
   tkgeom->UseAlignVersion(vers,1);
   TkAlignParDB &aligndb=(tkgeom->aligndb[0]);
   int nmodul=tkgeom->modules_.size();
   cout<<"total tkmodule="<<nmodul<<" require TDVBlock size>="<<(2*nmodul*6)<<endl;
//----write TDV
   int stat=aligndb.LoadAlignPar(falign);
   if(stat<0){cerr<<"err load="<<falign<<endl;return -2;}
   aligndb.StreamAlignTDV();
   aligndb.WriteAlignTDV(btime,etime,vers);
//-----load from TDV to test
/*   long int  ptime=(btime+etime)/2;
   int force=1;
   aligndb.LoadAlignTDV(ptime,1,force);
   aligndb.StreamAlignTDV();*/
   return 0;

}
