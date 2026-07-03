bool doall(bool reconstruct=false){
  gInterpreter->Load("ntuple_slc4_PG.so");
  gInterpreter->Load("saver.C+");

  ///////////////// STORE
  if(!reconstruct){
    // Charge
    TFile *f=new TFile("/afs/cern.ch/ams/Offline/AMSDataDir/v5.00/RichChargeUniformityCorrection.root");
    GeomHashEnsemble *ChargeAgl=f->Get("ChargeAgl");
    GeomHashEnsemble *ChargeNaF=f->Get("ChargeNaF");
    GeomHashEnsembleBridge ChargeAglData;
    GeomHashEnsembleBridge ChargeNaFData;
    to_GeomHashEnsembleBridge(ChargeAglData,*ChargeAgl);
    to_GeomHashEnsembleBridge(ChargeNaFData,*ChargeNaF);
    TFile fout("RichChargeUniformityCorrection_data.root","RECREATE");
    ChargeAglData.Write("ChargeAgl_Data");
    ChargeNaFData.Write("ChargeNaF_Data");
    fout.Close();
    f->Close();
    
    
    TFile *f=new TFile("/afs/cern.ch/ams/Offline/AMSDataDir/v5.00/RichBetaUniformityCorrection.root");
    GeomHashEnsemble *BetaAgl=f->Get("BetaAgl");
    GeomHashEnsemble *BetaNaF=f->Get("BetaNaF");
    GeomHashEnsembleBridge BetaAglData;
    GeomHashEnsembleBridge BetaNaFData;
    to_GeomHashEnsembleBridge(BetaAglData,*BetaAgl);
    to_GeomHashEnsembleBridge(BetaNaFData,*BetaNaF);
    TFile fout("RichBetaUniformityCorrection_data.root","RECREATE");
    BetaAglData.Write("BetaAgl_Data");
    BetaNaFData.Write("BetaNaF_Data");
    fout.Close();
    f->Close();
  }else{

    TFile *f=new TFile("RichChargeUniformityCorrection_data.root");
    GeomHashEnsembleBridge *ChargeAglData=f->Get("ChargeAgl_Data");
    GeomHashEnsembleBridge *ChargeNaFData=f->Get("ChargeNaF_Data");
    GeomHashEnsemble ChargeAgl;
    GeomHashEnsemble ChargeNaF;
    to_GeomHashEnsemble(*ChargeAglData,ChargeAgl);
    to_GeomHashEnsemble(*ChargeNaFData,ChargeNaF);
    TFile fout("RichChargeUniformityCorrection.root","RECREATE");
    ChargeAgl.Write("ChargeAgl");
    ChargeNaF.Write("ChargeNaF");
    fout.Close();
    f->Close();
    
    
    TFile *f=new TFile("RichBetaUniformityCorrection_data.root");
    GeomHashEnsembleBridge *BetaAglData=f->Get("BetaAgl_Data");
    GeomHashEnsembleBridge *BetaNaFData=f->Get("BetaNaF_Data");
    GeomHashEnsemble BetaAgl;
    GeomHashEnsemble BetaNaF;
    to_GeomHashEnsemble(*BetaAglData,BetaAgl);
    to_GeomHashEnsemble(*BetaNaFData,BetaNaF);
    TFile fout("RichBetaUniformityCorrection.root","RECREATE");
    BetaAgl.Write("BetaAgl");
    BetaNaF.Write("BetaNaF");
    fout.Close();
    f->Close();
  }

}
