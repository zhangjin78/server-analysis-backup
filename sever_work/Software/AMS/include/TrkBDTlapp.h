#include "root_RVSP.h"
#include "amschain.h"
#include "TrdKCluster.h"
#include "Rtypes.h"
#include "EcalChi2CY.h"
#include "TofTrack.h"

/// Tracker track charge confusion BDT structure
/*!
 * Tracker track charge confusion estimator developed by LAPP. <br/>
 * This estimator is correlated with other sub-detectors, for example ECAL, ToF and ACC. <br/>
 * \author jie.feng@cern.ch

*/

class TrkBDTlapp{
   public:
      /// the function to calculate charge confusion estimator for electrons. 
      /// \return BDTG value for the event.
      float GetTrkBDTel();

      /// the function to calculate charge confusion estimator for electrons.
      /*!
       * @param[in] ev:  the event you want to calculate the BDT value.
       * @param[in] itr: the track ID. By default, where itr = -1, the function will select the track which is close to the ECAL maximum energy shower entry.
       * @param[in] betah_tof: the beta calculated from BetaHR class and is used for normalized the charge measured by lower ToF. By default, where betah_tof = -1, the function will use the track ID to get this beta value. It should be 1 for electrons.
       * @return  [-1,1]:  the normal range.
       * @return  -2 : TrTrack and TOF Geometry does not match, no ECAL shower, no TrTrack or the betah_tof <= 0.6.
       * @return  2 : there is no particle in this event.
      */
      float GetTrkBDTel(AMSEventR *ev,  int itr =-1, float betah_tof = 0.);

   private:
      void _BubbleSort(int n, float arr_input[], float arr_output[]);
      //define variables
      AMSPoint pntL,trd_coo,trk_pos;
      AMSDir dirL,trd_dir,trk_dir;
      BetaHR *jf_bh;
      EcalShowerR* ecalsh;
      TrdTrackR* ptrdtrack;
      TrdKCluster _trdcluster;
      TrTrackR *track;
      enum { npatt=5 };//number of tracker pattern
      TString pattName[npatt];

      float trk_rarcm, trk_r1r9, log10chi2y, log10trkecal_dy, trk_q9, trk_qasym,trk_q78;
      int trk_nnoisehits,iparindex,nlay,nACC,npart,trklayused[9];
      int Track_Fitcode_Inn,Track_Fitcode_Al,Track_Fitcode_InnL1,Track_Fitcode_InnL9,Track_Fitcode_upper,Track_Fitcode_lower,Track_Fitcode_full,Track_Fitcode_Max, NHitsTrdlh[3];
      float rig,rig_a,rig_i,rig_il1,rig_il9,rig_l1l9,trk_rirm, TrkTrdMatch,TrkTrdMatchy,trk_upper,trk_lower,trk_qasym2,trk_q7,trk_q8;
      float deltaX,deltaY, delta,  ecalEntry[3],ecalExit[3];
      float q_tofd, chi2x,chi2y, qrms,qinup, qinlow,enE,trtrackerr;
      float threshold;
      double tmp_lkhd[3],LikelihoodRatioD[3];

      Int_t   nhitlay_offtrack[9];
      Int_t   nhitlay_neighbor[9]; // distance < 2cm

      Float_t cyc_rad[5],qlay[9];
      Int_t   nsumhit_radius[5]; // 0.2, 0.5, 1, 2, 5 cm, summed for all layers
      Float_t mindisty_lay[9]; // distance of nearest noise hit of each layer
      Float_t mindisty_sort[9]; // distance of noise hits, sorted by distance
      Float_t log10mindisty_sort[9];
      UInt_t reFit[8];

};


