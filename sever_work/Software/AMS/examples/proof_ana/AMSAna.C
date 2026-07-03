#define AMSAna_cxx
// The class definition in AMSAna.h has been generated automatically
// by the ROOT utility TTree::MakeSelector(). This class is derived
// from the ROOT class TSelector. For more information on the TSelector
// framework see $ROOTSYS/README/README.SELECTOR or the ROOT User Manual.

// The following methods are defined in this file:
//    Begin():        called every time a loop on the tree starts,
//                    a convenient place to create your histograms.
//    SlaveBegin():   called after Begin(), when on PROOF called only on the
//                    slave servers.
//    Process():      called for each event, in this function you decide what
//                    to read and fill your histograms.
//    SlaveTerminate: called at the end of the loop on the tree, when on PROOF
//                    called only on the slave servers.
//    Terminate():    called at the end of the loop on the tree,
//                    a convenient place to draw/fit your histograms.
//
// To use this file, try the following session on your Tree T:
//
// Root > T->Process("AMSAna.C")
// Root > T->Process("AMSAna.C","some options")
// Root > T->Process("AMSAna.C+")
//

#include "AMSAna.h"
#include <TH1F.h>
#include <TH2F.h>
#include <TStyle.h>




AMSAna::AMSAna(TTree * /*tree*/) : fChain(0),_event(0) { 
  BookHistos();
}



void AMSAna::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain =dynamic_cast<AMSChain*>(tree);
   TTree* tt=0;
   printf("INIT CLASS NAME %s\n",tree->ClassName());
   if(strcmp(tree->ClassName(),"TTree")==0)
     tt=tree;
   else tt=fChain->GetTree();
   _event=new AMSEventR;
   tt->SetBranchAddress("ev.",&_event);
   _event->Init(tt);
   _event->GetBranch(_event->Tree());
   return;
}

Bool_t AMSAna::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

  
  //TString fn(fChain->GetCurrentFile()->GetName());
  TString fn("MMMMMMM");
  Info("Notify", "processing file: %s", fn.Data());
  return kTRUE;
}


void AMSAna::Begin(TTree * /*tree*/)
{
   // The Begin() function is called at the start of the query.
   // When running with PROOF Begin() is only called on the client.
   // The tree argument is deprecated (on PROOF 0 is passed).
  
   TString option = GetOption();
   if(option.Length()>0)
     hman.SetOutputName(option.Data());
   Info("Begin", "starting a simple exercise with process option: %s", option.Data());
}

void AMSAna::SlaveBegin(TTree * /*tree*/)
{
  // The SlaveBegin() function is called after the Begin() function.
  // When running with PROOF SlaveBegin() is called on each slave server.
  // The tree argument is deprecated (on PROOF 0 is passed).
  
  TString option = GetOption();
  Info("SlaveBegin", "'%s' reading", "full");
  hman.CreateHistos();
  printf("B ------>  %d %d\n", hman.hlist.size(),hman.rlist.size());
  hashtable<TH1*>::iterator it;
  for( it=hman.hlist.begin();it!=hman.hlist.end();it++){
    TH1* hh=it->second;
    printf(" %x %s\n",hh,hh->ClassName());
    fOutput->Add(it->second);
  }
  
  return; 
}

Bool_t AMSAna::Process(Long64_t entry)
{
   // The Process() function is called for each entry in the tree (or possibly
   // keyed object in the case of PROOF) to be processed. The entry argument
   // specifies which entry in the currently loaded tree is to be processed.
   // It can be passed to either tt::GetEntry() or TBranch::GetEntry()
   // to read either all or the required parts of the data. When processing
   // keyed objects with PROOF, the object is already loaded and is available
   // via the fObject pointer.
   //
   // This function should contain the "body" of the analysis. It can contain
   // simple or elaborate selection criteria, run algorithms on the data
   // of the event and typically fill histograms.
   //
   // The processing can be stopped by calling Abort().
   //
   // Use fStatus to set the return value of TTree::Process().
   //
   // The return value is currently not used.
  //   printf("QUAQUA\n");
  //  if (fEntMin == -1 || entry < fEntMin) fEntMin = entry;
//    if (fEntMax == -1 || entry > fEntMax) fEntMax = entry;

//    if (fTestAbort == 1) {
//       Double_t rr = gRandom->Rndm();
//       if (rr > 0.999) {
//          Info("Process", "%lld -> %f", entry, rr); 
//          Abort("Testing file abortion", kAbortFile);
//          return kTRUE;
//       }
//   }
  //((AMSChain*)fChain)->GetEventLocal(entry);
 //printf("PROCESS CLASS NAME %s\n",fChain->ClassName());
//  myChain* ch=(myChain*)fChain;
//  twrNT=ch->GetEventPointer();

 //printf("QU0QU0\n");
 //printf("Val---> %d  %x  %x %f  \n",entry,twrNT,fPzHist,twrNT->chiSqInnerTr);
  // fPzHist->Fill(twrNT->chiSqInnerTr);
  _event->ReadHeader(entry);
  ProcessEvent(_event);

 return kTRUE;
}

void AMSAna::SlaveTerminate()
{
   // The SlaveTerminate() function is called after all entries or objects
   // have been processed. When running with PROOF SlaveTerminate() is called
   // on each slave server.

}

void AMSAna::Terminate()
{
   // The Terminate() function is the last function to be called during
   // a query. It always runs on the client, it can be used to present
   // the results graphically or save the results to file.
  //fPzHist->Draw();
  TIter hit(fOutput);
  TH1* hh;
  while ( hh=(TH1*) hit()){
    printf(" %s\n",hh->GetName());
    if(hman.GetRep(hh->GetName())) hman.AddH(hh);
  }
  printf(" ------>  %d %d\n", hman.hlist.size(),hman.rlist.size());
  //  hman.Get("nTrTrack")->Draw();
  hman.Save();
}




void AMSAna::BookHistos(){
  hman.Add(new TH1F("nTrTrack","Tracks",10,0,10));
  hman.Add(new TH2F("rr","rr",10,0,10.,50,-1,4));
  return;
}


void  AMSAna::ProcessEvent(AMSEventR* ev){

  hman.Fill("nTrTrack",ev->nTrTrack());
  if(ev->nTrTrack()>0){
    TrTrackR* tr=ev->pTrTrack(0);
    hman.Fill("rr",tr->GetNhits(),log10(tr->GetRigidity()));
  }
  return;
}
  
