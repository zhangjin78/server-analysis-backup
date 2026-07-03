{
  AMSChain a;
  a.Add("root://eosams.cern.ch//eos/ams/Data/AMS02/2014/ISS.B950/pass6/1327191748.00000001.root");
  int n=a.GetEntries();
  for(int i=0;i<n;i++){
    cout<<"EVENT "<<i<<endl;
    AMSEventR *ev=a.GetEvent(i);
    if(!ev) break;
    if(ev->nParticle()==0) continue;
    ParticleR *p=ev->pParticle(0);
    if(p->pTrTrack()==0)  continue;
    TrTrackR *tr=p->pTrTrack();
    // Assume it is a proton and compute the beta
    double r=fabs(tr->GetRigidity());
    double beta=r/sqrt(r*r+0.93827*0.93827);

    //////////////////////////////////////////////////////////////////////////////
    // Computation of expected number of   photoelectrons for give beta and z=1 //
    //////////////////////////////////////////////////////////////////////////////
    double npexp=RichRingR::ComputeNpExp(tr,beta);
    cout<<"THIS EVENT NpExp "<<npexp<<endl;

    /////////////////////////////
    // Try to build a new ring //
    /////////////////////////////
    RichRingR *nr=RichRingR::Build(tr);
    if(nr){
      cout<<"FOUND RING WITH BETA "<<nr->getBeta()<<endl;
      delete nr;
    }
  }
}
