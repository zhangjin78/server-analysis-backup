//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Wed Apr  8 17:03:49 2015 by ROOT version 5.34/09
// from TChain AMSAna/
//////////////////////////////////////////////////////////

#ifndef AMSAna_h
#define AMSAna_h

#include "root_RVSP.h"
#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include "HistoProofMan.h"
#include "amschain.h"
#include <TSelector.h>
// Header file for the classes stored in the TTree if any.
#include <TObject.h>

// Fixed size dimensions of array or collections stored in the TTree if any.

class AMSAna : public TSelector {
public :
   AMSChain          *fChain;   //!pointer to the analyzed TTree or TChain
  //Output hist
  HistoProofMan hman;
  // Declaration of leaf types
  AMSEventR *_event;

  virtual void BookHistos();
  virtual void ProcessEvent(AMSEventR* ev);
  AMSAna(TTree * /*tree*/ =0);
  virtual ~AMSAna() { _event=0; fChain=0;}
  virtual Int_t   Version() const { return 2; }
  virtual void    Begin(TTree *tree);
  virtual void    SlaveBegin(TTree *tree);
  virtual void    Init(TTree *tree);
  virtual Bool_t  Notify();
  virtual Bool_t  Process(Long64_t entry);
  virtual Int_t   GetEntry(Long64_t entry, Int_t getall = 0) { return fChain ? fChain->GetTree()->GetEntry(entry, getall) : 0; }
  virtual void    SetOption(const char *option) { fOption = option; }
  virtual void    SetObject(TObject *obj) { fObject = obj; }
  virtual void    SetInputList(TList *input) { fInput = input; }
  virtual TList  *GetOutputList() const { return fOutput; }
  virtual void    SlaveTerminate();
  virtual void    Terminate();
  
  ClassDef(AMSAna,0);
};

#endif


