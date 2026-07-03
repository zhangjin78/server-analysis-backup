//  $Id: TrMass.h,v 1.8 2017/09/30 17:42:33 shaino Exp $
#ifndef __TrMass__
#define __TrMass__

//////////////////////////////////////////////////////////////////////////
///
///\class TrMass
///\brief A static class to manage tracker mass estimator 
///
/// Examples:
///
///\date  2015/09/20 SH     First version
///\date  2017/09/07 SH/SL  Some methods added
///$Date: 2017/09/30 17:42:33 $
///
///$Revision: 1.8 $
///
//////////////////////////////////////////////////////////////////////////

#include "TMath.h"
#include "point.h"

class AMSEventR;
class TrTrackR;
class TRandom;

/// A static class to manage tracker mass estimator 
class TrMass {
public:
  /// Get -Log(Likelihood) from Z, A, beta and TrTrack pointer(tr)
  static double GetLL(int z, int a, double beta, TrTrackR *tr);

  /// Get -Log(Likelihood) from Z, A, beta and hit positions(h)
  static double GetLL(int z, int a, double beta, AMSPoint h[7], double theta);

  /// Get Int(BL^2) on the trajectory between L2 and L7 with step(s) in cm
  static double GetBL2(TrTrackR *tr, double s = 1);

  /// Get Int(BL^2) on the trajectory between L2 and L7 with step(s) in cm
  static double GetBL2(AMSPoint p, AMSDir d, double r, double s = 1);

  /// Get angle in deg at z after propagating from p1 to p2 with rigidity(r)
  static double GetA(double z, AMSPoint p1, AMSPoint p2, double r);

  /// Get angle in deg at z after propagating in p[i] with rigidity(r)
  static double GetA(double z, AMSPoint *p, int i, double r);

  /// Get normalization factor for Z, rigidity*beta(rb), and zenith angle(th)
  static double GetNorm(int z, double rb, double th);

  /// Get array of hit positions from tkml, xcog, ycog and Z (for experts only)
  static int GetH(int tkml[7], float xcog[7],
		               float ycog[7], AMSPoint h[7],
		               float theta, float phi, int z = 1,
		                                       int ismc = 0);

  /// Get hit positions from tkml, xcog, ycog and Z (for experts only)
  static AMSPoint GetH(int tkml, float xcog,  float ycog,
		                 float theta, float phi, int z = 1,
		                                         int ismc = 0);

  /// Generate array of hit positions, h for Z, A
  /// \param[out]  b    Generated beta
  /// \param[out]  th   Generated Zenith angle
  /// \param[out]  h    Generated hit positions
  /// \param[in]   cut  +1:Inside L1 +2:Inside L9 +4:Inside Ecal
  /// \return           Number of valid hits
  static int GenH(int z, int a, double &b,
		                double &th, AMSPoint h[7], int cut = 0);

  /// Calculate confidence upper limit from Nobs and Nbg
  /// \param[in]  nd   Observed number of events
  /// \param[in]  nb   Expected background events
  /// \param[in]  cf   Confidence level (default = 0.95)
  /// \return          Upper limit
  static double GetLimit(double nd, double nb, double cf = 0.95);

  /// Get beta from dE/dx measurements by Tracker, TOF and/or TRD
  /// \param[in]  ev    AMSEvent
  /// \param[in]  flag  flag=(TRD)*100+(TOF)*10+(Tracker)
  /// \return           beta
  static double GetBeta(AMSEventR *ev, int flag = 111, TrTrackR *trk = 0);

  /// Number of TRD segments which make a vertex above 
  static int GetNtrdSegTrk(AMSEventR *ev);

  /// An estimator to check the noise-pickup (by S.Lu)
  /// \param[in]  ev   AMSEvent
  /// \param[in]  trk  TrTrackR (trk=ev->pParticle(0)->pTrTrack if trk=0)
  /// \param[in]  opt  1:Inner layers only
  /// \param[out] MinProb[9]  Get min. Prob. of each layer
  /// \param[out] MaxProb[9]  Get max. Prob. of each layer
  /// \param[out] HitProb[9]  Get hit  Prob. of each layer
  /// \return                 Estimator value
  static double GetNpick(AMSEventR *ev, TrTrackR *trk = 0, int opt = 1,
			 float *MinProb = 0, float *MaxProb = 0,
			 float *HitProb = 0);

  /// Nummber of MQL PDF variables
  enum { MQL_Nv = 16, MQL_Np = 14 };

  /// MQL PDF variable ranges
  static float MQL_vmax[MQL_Nv];
  static float MQL_vmin[MQL_Nv];

  /// MQL PDF parameters
  static double *MQL_par;

  /// Mass quality -Log(Likelihood) estimator
  /// \param[in]  run     Run number
  /// \param[in]  event   Event number
  /// \param[in]  opt     1:Print information
  /// \param[out] v[Nv]   Variables
  /// \param[out] p[Nv]   Probabilities
  /// \param[out] ll[Np]  Weighted -Log(Likelihood) of each variable
  /// \return             Estimator value
  static double GetMQL(int run, int event, int opt = 0,
		       float *v = 0, float *p = 0, float *ll = 0);

  /// Mass quality -Log(Likelihood) estimator
  /// \param[in]  ev      AMSEvent
  /// \param[in]  trk     TrTrackR (trk=ev->pParticle(0)->pTrTrack if trk=0)
  /// \param[in]  opt     1:Print information
  /// \param[out] v[Nv]   Variables
  /// \param[out] p[Nv]   Probabilities
  /// \param[out] ll[Np]  Weighted -Log(Likelihood) of each variable
  /// \return             Estimator value
  static double GetMQL(AMSEventR *ev, TrTrackR *trk = 0, int opt = 0,
		       float *v = 0, float *p = 0, float *ll = 0);

  /// Get variables for Mass quality -Log(Likelihood) estimator
  /// \param[in]  ev     AMSEvent
  /// \param[out] v[Nv]  Variables
  /// \param[out] beta   Beta measurement
  /// \param[in]  trk    TrTrackR (trk=ev->pParticle(0)->pTrTrack if trk=0)
  /// \param[in]  opt    Reserved
  /// \return            0: Success <0: Error
  static int GetMQLv(AMSEventR *ev, float *v, float &beta, 
		     TrTrackR *trk = 0, int opt = 0);

  /// Get Mass quality -Log(Likelihood) estimator from variables
  /// \param[in]  beta    Beta for the normalization
  /// \param[in]  v [Nv]  Variables
  /// \param[out] p [Np]  Probabilities
  /// \param[out] ll[Np]  Weighted -Log(Likelihood) of each variable
  /// \param[in]  opt     1:Print information
  /// \return             Estimator value
  static double GetMQL(float beta, float *v, float *p, float *ll = 0,
		       int opt = 0);
			
  /// Get the probability to have the given MQL value
  /// \param[in]  mql     Output of GetMQL
  /// \param[in]  beta    Beta for the normalization
  /// \return             Probability
  static double GetProb(double mql, double beta);


  /// Generate uniform random number r (0<g<1)
  /// \param[in]  run     Run number (used as seed)
  /// \return             Random number
  static double Rndm(int run);


  /// PDF parameters
  static double *fPar;

  /// Internal values for debug
  static double fDa[4];

  /// Atomic Mass Unit in GeV/c^2
  static double AMU;

  /// Random engine
  static TRandom *fRand;

  /// Get beta from Z, rigidity(r) and mass(m)
  static double GetBeta(int z, double r, double m) {
    return 1/TMath::Sqrt(1+m*m/(r*r*z*z));
  }

  /// Get rigidity in GV from Z, beta(b) and mass(m)
  static double GetRigidity(int z, double b, double m) {
    return (z != 0 && b*b < 1) ? m*b/z*GetGamma(b) : 0;
  }

  /// Get Ekin/n in GeV/n from Z, A and rigidity(r)
  static double GetEkin(int z, int a, double r) {
    double m = GetMass(z, a);
    return (TMath::Sqrt(r*r*z*z+m*m)-m)/a;
  }

  /// Get Ekin/n in GeV/n from beta(b)
  static double GetEkin(double b) { return AMU*(GetGamma(b)-1); }

  /// GetMass in GeV/c^2 from Z, rigidity(r) and beta(b)
  static double GetMass(int z, double r, double b) {
    return (z != 0 && b*b < 1) ? r*z/b/GetGamma(b) : 0;
  }

  /// Get Lorentz gamma factror from beta(b)
  static double GetGamma(double b) {
    return (b*b < 1) ? 1/TMath::Sqrt(1-b*b) : 1;
  }

  /// Get mass in GeV/c^2 for Z and A nuclei; Z=0:lepton/meson mass
  static double GetMass(int z, int a) {
    if (z == 0 && a ==  0) return  0.511e-3; //   e
    if (z == 0 && a ==  1) return  0.1057;   //  mu
    if (z == 0 && a ==  2) return  0.1396;   //  pi
    if (z == 0 && a ==  3) return  0.4937;   //   K
    if (z == 1 && a ==  1) return  0.9383;   //  1H
    if (z == 1 && a ==  2) return  1.876;    //  2H
    if (z == 1 && a ==  3) return  2.809;    //  3H
    if (z == 2 && a ==  3) return  2.809;    //  3He
    if (z == 2 && a ==  4) return  3.727;    //  4He
    if (z == 3 && a ==  6) return  5.603;    //  6Li
    if (z == 3 && a ==  7) return  6.535;    //  7Li
    if (z == 4 && a ==  7) return  6.536;    //  7Be
    if (z == 4 && a ==  9) return  8.395;    //  9Be
    if (z == 4 && a == 10) return  9.328;    // 10Be
    if (z == 5 && a == 10) return  9.327;    // 10B
    if (z == 5 && a == 11) return 10.255;    // 11B
    if (z == 6 && a == 12) return 11.178;    // 12C
    if (z == 7 && a == 14) return 13.044;    // 14N
    if (z == 7 && a == 15) return 13.973;    // 15N
    if (z == 8 && a == 16) return 14.899;    // 16O
    return 0;
  }
};

#endif
