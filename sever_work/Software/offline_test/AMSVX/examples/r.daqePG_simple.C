///   \example r.daqePG_simple.C
///   This is an example of how to load daqePG.C (to run: root;  .x r.daqePGsimple.C)   
{

    
      gROOT->ProcessLine(".x $AMSWD/install/rootlogon.C");

// Add all AMS Root files to be analyzed 
      TChain chain("AMSRoot");
      chain.Add("root://eosams.cern.ch//eos/ams/Data/AMS02/2022/ISS.B1236/pass8/1668097626.00000001.root"); 
      chain.Process("daqePG_simple.C+","daqePG.root",100000);


}
