#include <cstdio>
#include "root_RVSP.h"
#include "amschain.h"
#include "analyze_CINT.C"

int run_analyze_CINT(){

// for using ROOT CINT C++ Interpreter, please uncomment following line or "cp ../../install/rootlogon.C ~/.rootlogon.C"
// gROOT->ProcessLine(".x ../../install/rootlogon.C");

  AMSChain ch;
  ch.Add("root://eosams.cern.ch//eos/ams/Data/AMS02/2022/ISS.B1236/pass8/1668097626.00000001.root");
  const char *ofile="HistoOut.root";
  return analyze_CINT(&ch,ofile,10000);
}

int main(int argc,char ** argv){
  return run_analyze_CINT();
}

