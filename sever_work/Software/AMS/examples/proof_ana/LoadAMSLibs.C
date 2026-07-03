{
  gROOT->Reset();
  
  // Set include path
  gInterpreter->AddIncludePath(gSystem->ExpandPathName("$AMSSRC/include"));
  TProof* p=TProof::Open("");
  // for loading AMSRoot libraries
  p->Load(gSystem->ExpandPathName("$AMSWD/lib/linuxx8664gcc5.34/ntuple_slc6_PG.so"));

}
