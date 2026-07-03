///--Author Q.Yan qyan@cern.ch
///--Tool For Analysis database
#ifndef _QAnalysisdbc_
#define _QAnalysisdbc_

namespace qanalysisconst{
   const int NTKL=9;
   const int NTOFL=4;
   const int NBARM=10;
   const int NBAR[NTOFL]={8,8,10,8};
   const float tk_pz[NTKL]={159.04,53.05,29.22,25.24,1.706,-2.292,-25.26,-29.25,-136.03};
   const double cirr[]={62,62,46,46,46,46,46,46,43};
   const double ciry[]={47,40,44,44,36,36,44,44,29};
   const double ecal_pz=-143;
   const double ecalxy[]={32,32};
   const double trd_pz[]={142,85};
   const double trd_cirr[]={92,69};
   const float mcutthu=1.2;//1.2 cutoff
   int icffv=3;//40 degree cutoff
   const bool isbinbybin=1;//binbybin reweight For MC
   const int ipgc=2;//PG+CIEMAT
   int upgc=2;
}

#endif
