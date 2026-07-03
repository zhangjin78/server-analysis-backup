//  $Id: 
// Usage
//
// setenv AMSWD $Offline/vdev
// source $Offline/vdev/amsvar.534
// root $AMSWD/l64PG3.C
// .L files.C+
// AMSChain a("FILES");
// a.Add("path_to_files/*.root");
// a.Process("files.C+","in="files_list_intended";out="file_list_missed;");
// .q
//
#define files_cxx
#include <iostream>
#include <fstream>
#include <set>
using namespace std;
ifstream fin;
ofstream fout;
set<string> v1;
// The class definition in files.h has been generated automatically
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
// Root > T->Process("files")
// Root > T->Process("files","some options")
// Root > T->Process("files+")
//

#include "files.h"
#include <TH2.h>
#include <TStyle.h>


void files::Begin(TTree * /*tree*/)
{
   // The Begin() function is called at the start of the query.
   // When running with PROOF Begin() is only called on the client.
   // The tree argument is deprecated (on PROOF 0 is passed).
   v1.clear();
   TString option = GetOption();
   if(option.Length()>1){
     string opt=(const char*)option;
     fin.clear();
     fout.clear();
{
     int pospb=opt.find("in=");
         if(pospb>=0){
           int pospe=opt.find(";",pospb+1);
           if(pospe>0){
             string in=opt.substr(pospb+3,pospe-pospb-3);
             fin.open(in.c_str());
             cout <<" in "<<in<<" "<<fin.good()<<endl;
           }
         }
 }


{
     int pospb=opt.find("out=");
         if(pospb>=0){
           int pospe=opt.find(";",pospb+1);
           if(pospe>0){
             string in=opt.substr(pospb+4,pospe-pospb-4);
             fout.open(in.c_str());
             cout <<" Open out "<<in.c_str()<<endl;
           }
         }
 }


}
}

void files::SlaveBegin(TTree * /*tree*/)
{
   // The SlaveBegin() function is called after the Begin() function.
   // When running with PROOF SlaveBegin() is called on each slave server.
   // The tree argument is deprecated (on PROOF 0 is passed).

   TString option = GetOption();
       

}

Bool_t files::Process(Long64_t entry)
{
   // The Process() function is called for each entry in the tree (or possibly
   // keyed object in the case of PROOF) to be processed. The entry argument
   // specifies which entry in the currently loaded tree is to be processed.
   // It can be passed to either files::GetEntry() or TBranch::GetEntry()
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
   GetEntry(entry);   
   string fs=file;
   int pos=fs.find("?svcClass");
   if(pos>=0){
      fs.resize(pos);
   }
   //cout <<fs<<endl;
   v1.insert(fs);
//   fout<<fs<<endl;
   return kTRUE;
}

void files::SlaveTerminate()
{
   // The SlaveTerminate() function is called after all entries or objects
   // have been processed. When running with PROOF SlaveTerminate() is called
   // on each slave server.

}

void files::Terminate()
{
set<string>::iterator it;
pair<set<string>::iterator,bool> ret;
int nin=0;
while(fin.good() && ! fin.eof()){
string line;
while(getline(fin,line)){
nin++;
ret=v1.insert(line);
if(!ret.second)v1.erase(ret.first);
}
}
fin.close();
cout <<"  v1 size "<<nin<<" "<<v1.size()<<endl;
for(set<string>::iterator i=v1.begin();i!=v1.end();i++){
fout<<*i<<endl;
}



fout.close();
   // The Terminate() function is the last function to be called during
   // a query. It always runs on the client, it can be used to present
   // the results graphically or save the results to file.

}
