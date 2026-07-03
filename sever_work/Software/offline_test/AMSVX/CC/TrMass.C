// $Id: TrMass.C,v 1.12 2017/12/13 13:37:21 choutko Exp $

//////////////////////////////////////////////////////////////////////////
///
///\file  TrMass.C
///\brief Source file of TrMass class
///
///$Date: 2017/12/13 13:37:21 $
///
///$Revision: 1.12 $
///
//////////////////////////////////////////////////////////////////////////
#ifdef _PGTRACK_
#include "TrMass.h"
#include "TrTrack.h"
#include "TkCoo.h"

double   TrMass::fDa[4] = { 0, 0, 0, 0 };
double  *TrMass::fPar   = 0;
double   TrMass::AMU    = 0.93149;   // Atomic Mass Unit in GeV/c^2
TRandom *TrMass::fRand  = 0;

double TrMass::GetLL(int z, int a, double beta, TrTrackR *tr)
{
  if (!tr) return -1;

  AMSPoint h[7];
  for (int i = 0; i < 7; i++) h[i] = tr->GetHitCooLJ(i+2);

  return GetLL(z, a, beta, h, tr->GetTheta());
}

double TrMass::GetLL(int z, int a, double beta, AMSPoint h[7], double theta)
{
  double zref[2] = { 27.2, -0.3 };

  int az = TMath::Abs(z);
  int aa = TMath::Abs(a);
  double mass = GetMass(az, aa); if (mass <= 0) return -2;

  if (az == 0) az = 1;
  double rgt = GetRigidity(az, beta, mass);

  if (z < 0 || a < 0) rgt = -rgt;

  for (int i = 0; i < 2; i++) {
    double a1 = GetA(zref[i], h, i*2-1, rgt);
    double a2 = GetA(zref[i], h, i*2+1, rgt);
    fDa[i] = (a1*a2 != 0) ? a1-a2 : 0;
  }
  if (fDa[0]*fDa[1] == 0) return -3;

  double rb   = beta*rgt;
  double norm = GetNorm(z, rb, theta);
  fDa[2] = fDa[0]*rb/norm;
  fDa[3] = fDa[1]*rb/norm;

  double p1 = (TMath::Abs(fDa[2]) < 20) ? TMath::Gaus(fDa[2], 0, 1, 1) : 1e-88;
  double p2 = (TMath::Abs(fDa[3]) < 20) ? TMath::Gaus(fDa[3], 0, 1, 1) : 1e-88;
  if (p1 <= 0 || p2 <= 0) return -4;

  double prob = p1*p2;
  return (prob > 0) ? -TMath::Log(prob) : -5;
}

double TrMass::GetBL2(TrTrackR *tr, double s)
{
  return (tr) ? GetBL2(tr->GetP0(), tr->GetDir(), tr->GetRigidity(), s) : 0;
}

double TrMass::GetBL2(AMSPoint p, AMSDir d, double r, double s)
{
  if (TMath::Abs(r) < 0.01) return 0;

  TrProp trp(p, d, r);
  trp.Propagate(53);

  double bl2 = 0;

  AMSPoint p0 = trp.GetP0();
  double   b0 = TrProp::GuFld(p0).x();
  while (trp.GetP0z() > -25) {
    trp.Propagate(trp.GetP0z()-s);
    AMSPoint p1 = trp.GetP0();
    double   b1 = TrProp::GuFld(p1).x();
    double   l2 = (p1.y()-p0.y())*(p1.y()-p0.y())+
                  (p1.z()-p0.z())*(p1.z()-p0.z());
    bl2 += (b0+b1)/2*l2;
    p0 = p1;
    b0 = b1;
  }

  return bl2;
}

double TrMass::GetA(double z, AMSPoint p1, AMSPoint p2, double r)
{
  if (p1.z() == 0 || p2.z() == 0) return 0;

  AMSDir dir = p2-p1;

  TrProp tr0(p1, dir, r); tr0.Propagate(p2.z());
  AMSPoint pp = p2;
  pp[0] -= tr0.GetP0x()-p2.x();
  pp[1] -= tr0.GetP0y()-p2.y();
  dir = pp-p1;

  TrProp trp(p1, dir, r); trp.Propagate(z);
  return TMath::ATan(trp.GetDyDz())*TMath::RadToDeg();
}

double TrMass::GetA(double z, AMSPoint *p, int i, double r)
{
  if (!p) return 0;

  TrFit fit;
  fit.SetRigidity(r);

  AMSPoint err(10e-4, 10e-4, 100e-4);

  AMSPoint p1, p2;
  for (int j = 0; j < 4; j++) {
    if (i+j < 0 || p[i+j].z() == 0) continue;
    fit.Add(p[i+j], err);

    if      (p1.z() == 0) p1 = p[i+j];
    else if (p2.z() == 0) p2 = p[i+j];
  }
  if (fit.GetNhit() < 3) return GetA(z, p1, p2, r);

  fit.AlcarazFit(1);
  fit.Propagate(z);
  return TMath::ATan(fit.GetDyDz())*TMath::RadToDeg();
}

double TrMass::GetNorm(int z, double rb, double th)
{
  if (!fPar) {
    fPar = new double[20];
    
    fPar[0] = 0.071; fPar[1] = 0.0040;   // Z=1
    fPar[2] = 0.080; fPar[3] = 0.0012;   // Z=2
    fPar[4] = 0.068; fPar[5] = 0.00084;  // Z=3
    fPar[6] = 0.071; fPar[7] = 0.00275;  // Z=4

    // cos(theta) correction
    fPar[10] = 0.976;
    fPar[11] = 2.36;

    // MC tuning factor
    fPar[19] = 1;
  }

  double *par = &fPar[2];
  if (1 <= z && z <= 4) par = &fPar[(z-1)*2];

  double norm = par[0]+par[1]*rb;
  double ct   = TMath::Cos(th); if (ct < 0.94) ct = 0.94;
  if (z >= 2) norm *= 1-(ct-fPar[10])*fPar[11];

  return norm;
}

AMSPoint TrMass::GetH(int tkml, float xcog,  float ycog,
		                float theta, float phi, int z, int ismc)
{
  double zdxc = 4, zdyc = 0;
  if (z >= 2) {
    zdxc = 12;
    zdyc = -4;

    float chrg = z;
    if (chrg > 2.5) zdyc = -11.0;
    if (chrg > 3.5) zdyc = -18.5;
    if (chrg > 4.5) zdyc = -23.5;
    if (chrg > 5.5) zdyc = -24.5;
    if (chrg > 6.5) zdyc = -25.5;
    if (chrg > 7.5) zdyc = -30.5;
  }

  double dzx = TMath::Tan(theta)*TMath::Cos(phi);
  double dzy = TMath::Tan(theta)*TMath::Sin(phi);

  int   imlt = TMath::Abs(tkml)/1000;
  int   tkid = tkml%1000;

  AMSPoint loc(TkCoo::GetLocalCoo(tkid, TMath::Abs(xcog), imlt),
	       TkCoo::GetLocalCoo(tkid, TMath::Abs(ycog), imlt), 0);

  AMSPoint coo = TkCoo::GetGlobalA(tkid, loc);
  if (zdxc > 5) zdxc = (1-TMath::Abs(dzx))*zdxc;

  if (!ismc) {
    TkLadder *lad = TkDBc::Head->FindTkId(tkid);
    coo[0] += (lad) ? lad->GetRotMat().GetEl(2, 2)*zdxc*dzx*1e-4 : 0;
    coo[1] += (lad) ? lad->GetRotMat().GetEl(2, 2)*zdyc*dzy*1e-4 : 0;
  }

  return coo;
}

int TrMass::GetH(int tkml[7], float xcog[7],
		              float ycog[7], AMSPoint h[7],
		              float theta, float phi, int z, int ismc)
{
  int nh = 0;

  for (int i = 0; i < 7; i++) {
    if (TMath::Abs(tkml[i]) > 100000) tkml[i] = 0;
    if (tkml[i] == 0) continue;
    h[i] = GetH(tkml[i], xcog[i], ycog[i], theta, phi, z, ismc);
    nh++;
  }

  return nh;
}

#include "TLimit.h"
#include "TLimitDataSource.h"
#include "TConfidenceLevel.h"
#include "TH1.h"
double TrMass::GetLimit(double nd, double nb, double cf)
{
  TH1D *hb = new TH1D("hb", "B.G.",   1, -4, 4);
  TH1D *hs = new TH1D("hs", "signal", 1, -4, 4);
  TH1D *hd = new TH1D("hd", "data",   1, -4, 4);
  hb->Fill(0., nb+1e-5);
  hd->Fill(0., nd+1e-5);

  double cl = 0, ns = 10;
  while (fabs(cl-cf) > 0.0001) {
    hs->Reset();
    hs->Fill(0., ns);
    TLimitDataSource *ds = new TLimitDataSource(hs, hb, hd);
    TConfidenceLevel *cc = TLimit::ComputeLimit(ds, 50000);
    cl  = 1-cc->CLs();
    ns *= 1+4*(cf-cl);
    delete ds;
    delete cc;
  }

  delete hb;
  delete hs;
  delete hd;

  return ns;
}

#include "root.h"
#include "TRandom3.h"

int TrMass::GenH(int z, int a, double &b, double &th, AMSPoint h[7], int cut)
{
  for (int i = 0; i < 7; i++) h[i].setp(0, 0,0 );
  if (z == 0) return 0;

  if (!fRand) fRand = new TRandom3(20150921);

  double m = GetMass(z, a);
  double e = TMath::Power(10, fRand->Rndm()*2-1)*2;  // 0.2 - 20 GeV/n
  double r = TMath::Sqrt((e*a+m)*(e*a+m)-m*m)/z;
  b = GetBeta(z, r, m);

  double zl[9] = { 158.92, 53.06, 29.228, 25.212, 1.698,
		   -2.318, -25.212, -29.228, -135.882 };

  double x0, y0, z0 = 0;
  double x1, y1, z1 = (cut&1) ? zl[0] : zl[1];
  double x2, y2, z2 = (cut&6) ? zl[8] : zl[7];
  double dr0 = 50, dy0 = 35;
  double dr1 = 60, dy1 = (cut&1) ? 45 : 40;
  double dr2 = 50, dy2 = (cut&2) ? 30 : 45, dx2 = 50;
  if (cut&2)   dx2 = 45;
  if (cut&4) { dx2 = 33; if (!(cut&2)) dy2 = 33; }

  do {
    x1 = (fRand->Rndm()*2-1)*dr1; y1 = (fRand->Rndm()*2-1)*dy1;
    x2 = (fRand->Rndm()*2-1)*dx2; y2 = (fRand->Rndm()*2-1)*dy2;
    x0 = x1+(x2-x1)/(z2-z1)*(z0-z1);
    y0 = y1+(y2-y1)/(z2-z1)*(z0-z1);
  } while (x0*x0+y0*y0 > dr0*dr0 || TMath::Abs(y0) > dy0 ||
	   x1*x1+y1*y1 > dr1*dr1 || x2*x2+y2*y2 > dr2*dr2);

  AMSPoint p0(x0, y0, z0);
  AMSPoint p1(x1, y1, z1);
  AMSDir   d1 = p1-p0;

  TrProp trp(p1, d1, r);
  for (int i = 0; i < 7; i++) {
    trp.Propagate(zl[i+1]);

    AMSPoint p = trp.GetP0();
    AMSDir   d = trp.GetDir();
    int tk;
    AMSPoint po, pd;
    AMSEventR::IsInsideTracker(i+2, p, d, r, 0, tk, po, pd);
    h[i] = p;

    if (i == 1 || i == 3 || i == 5) {
      double norm = GetNorm(z, r*b, trp.GetTheta())*fPar[19];

      AMSRotMat m;
      m.SetRotAngles(0, 0, fRand->Gaus()*norm/r/b*TMath::DegToRad());

      AMSDir d = trp.GetDir();
      d = m*d;

      TrProp t(trp.GetP0(), d, r);
      trp = t;
    }
  }
  trp.Propagate(0);
  th = trp.GetTheta();

  return r;
}

#include "TrTrack.h"
#include "TrCharge.h"
#include "TrdKCluster.h"
#include "Tofrec02_ihep.h"

double TrMass::GetBeta(AMSEventR *ev, int flag, TrTrackR *trk)
{
  ParticleR *p = ev->pParticle(0);

  if (!trk)  trk = (p) ? p->pTrTrack () : 0;
  TrdTrackR *trd = (p) ? p->pTrdTrack() : 0;
  BetaHR    *bth = (p) ? p->pBetaH()    : 0;

  double bm = 0;
  int    nb = 0;

  if (flag%10 && trk) {
    mean_t mn = TrCharge::GetCombinedMean(
      TrCharge::kInner|TrCharge::kTruncMean|TrCharge::kSqrt, trk, 1,-1,
      TrClusterR::kAsym|TrClusterR::kGain|TrClusterR::kAngle|
      TrClusterR::kLoss|TrClusterR::kMIP, -1, 0);
    //TrClusterR::kBeta|TrClusterR::kRigidity

    bm += 0.94*TMath::Power(1/mn.Mean, 1.1);
    nb++;
  }

  if ((flag/10)%10 && bth) {
    int nlay;
    float qrms;
    double q = bth->GetQ(nlay, qrms, 2, 
			 TofRecH::kThetaCor|TofRecH::kBirkCor|
			 TofRecH::kReAttCor|TofRecH::kDAWeight|TofRecH::kQ2Q,
                       //TofRecH::kBetaCor
			 -2, 1, 0);

    bm += 0.99*TMath::Power(1/q, 1.22);
    nb++;
  }

  if ((flag/100)%10 && trd && trk) {
    TrdKCluster tk(ev, trd, trk->GetRigidity());

    AMSPoint p0(trd->Coo);
    AMSDir  dir(trd->Theta, trd->Phi);

    double qs = 0, qmin = 1e9, qmax = 0;
    int    ns = 0;

    for (int j = 0; j < tk.NHits(); j++) {
      TrdKHit *hit = tk.GetHit(j);
      if (!hit) continue;
      
      double len = hit->Tube_Track_3DLength(&p0, &dir);
      double amp = hit->TRDHit_Amp;
      if (len > 0.3) {
	double q = amp/len;
	qs += q;
	ns++;
	if (q > qmax) qmax = q;
	if (q < qmin) qmin = q;
      }
    }
    if (ns > 5) {
      qs -= qmax+qmin;
      qs /= (ns-2);
      bm += TMath::Sqrt(160/qs);
      nb++;
    }
  }

  return (nb > 0) ? bm/nb : 0;
}

Int_t TrMass::GetNtrdSegTrk(AMSEventR *pev)
{
    ParticleR *part = pev->pParticle(0);
    if (!part) return 0;

    Int_t nseg = pev->nTrdSegment();
    if (nseg <= 0) return 0;

    Double_t zcen = 120;
    Double_t zmin = 120;
    Double_t zmax = 200;

    AMSPoint pt1; AMSDir dr1;
    AMSPoint pt2; AMSDir dr2;

    TrTrackR *ptrk = part->pTrTrack();
    if (!ptrk) return 0;

    ptrk->Interpolate(zmin, pt1, dr1);
    ptrk->Interpolate(zmax, pt2, dr2);

    Double_t tdx = (pt1.x()-pt2.x())/(pt1.z()-pt2.z());
    Double_t tdy = (pt1.y()-pt2.y())/(pt1.z()-pt2.z());
    Double_t tx0 =  pt1.x()-pt1.z()*tdx;
    Double_t ty0 =  pt1.y()-pt1.z()*tdy;
    Double_t txt = tx0+tdx*zcen;
    Double_t tyt = ty0+tdy*zcen;

    Int_t nsgp = 0;

    for (Int_t i = 0; i < nseg; i++) {
        TrdSegmentR *seg = pev->pTrdSegment(i);

        Int_t xy = seg->Orientation;
        if (xy == 0) {
            seg->FitPar[1] *= -1;
            seg->FitPar[0] *= -1;
        }

        Double_t x0 = seg->FitPar[1]+seg->FitPar[0]*zcen;
        Double_t dx = (xy == 1) ? txt-x0 : tyt-x0;

        if (TMath::Abs(dx) < 3) continue;

        Double_t zt = (xy == 1) ? -(tx0-seg->FitPar[1])/(tdx-seg->FitPar[0])
	                        : -(ty0-seg->FitPar[1])/(tdy-seg->FitPar[0]);

        if (zmin < zt && zt < zmax) nsgp++;
    }

    return nsgp;
}

/// An estimator to check the noise-pickup (Original code by S.Lu)
double TrMass::GetNpick(AMSEventR *ev, TrTrackR *trk, int opt,
			float *MinProb, float *MaxProb, float *HitProb)
{
  if (!trk) {
    ParticleR *part = ev->pParticle(0);
    if (!part) return 0;

    trk = part->pTrTrack();
  }
  if (!trk) return 0;

  TrRecHitR *TrHit[9];
  float    minProb[9];
  float    maxProb[9];
  float    hitProb[9];
  if (!MinProb) MinProb = minProb;
  if (!MaxProb) MaxProb = maxProb;
  if (!HitProb) HitProb = hitProb;

  for(int i=0;i<9;i++){
    TrHit  [i] = trk->GetHitLJ(i+1);
    MinProb[i] = -1;
    MaxProb[i] = -1;
    HitProb[i] = -1;           
  }
  for(int i=0;i<ev->NTrRecHit();i++){
    TrRecHitR *hit = ev->pTrRecHit(i);
    if(!hit) continue;
    int lay = hit->GetLayerJ()-1;
    if(lay<0 || lay>8)continue;
    float p = hit->GetCorrelationProb();
    if(p<=0)continue;
    if(MinProb[lay]==-1 || MinProb[lay]>p)MinProb[lay] = p;
    if(MaxProb[lay]==-1 || MaxProb[lay]<p)MaxProb[lay] = p;
    if(hit==TrHit[lay]) HitProb[lay] = p;
  } 

  // Added by SH
  float min = -1;
  for(int i=0;i<9;i++) {
    if ((opt&1) && (i==0 || i==8)) continue;
    if (HitProb[i] <= 0 || MaxProb[i] <= 0) continue;
    if (min<0 || HitProb[i]/MaxProb[i] < min) min = HitProb[i]/MaxProb[i];
  }

  return min;
}

double TrMass::Rndm(int run)
{
  static TRandom3 *rnd = 0;
  static int brun = 0;

  if (run != brun) { delete rnd; rnd = 0; brun = run; }
  if (!rnd) rnd = new TRandom3(run);

  return rnd->Rndm();
}


namespace MQL {
  enum { Nv = TrMass::MQL_Nv, N = TrMass::MQL_Np };
};

using MQL::N;
using MQL::Nv;

float TrMass::MQL_vmin[MQL_Nv] = { -3, -3, -3, -3, -3, -3, -1, -1, -1, 
				   -5,  0, -3,  0,  0,  0,  0 };
float TrMass::MQL_vmax[MQL_Nv] = {  3,  3,  3,  3,  3,  3,  1,  1,  1,
				    0,  4,  0,  1.5,
		 static_cast<float>(TMath::Log10(-TMath::Log(1e-88)*2)),
				    0,  9 };

double *TrMass::MQL_par = 0;


double TrMass::GetMQL(AMSEventR *ev, TrTrackR *trk, int opt, float *v,
		                                             float *p,
		                                             float *ll)
{
  if (!ev) {
    if (opt == 1) cout << "TrMass::GetMQL-E-Event is null" << endl;
    return -95;
  }

  float vv[Nv], pp[Nv];
  if (!v) v = vv;
  if (!p) p = pp;

  float beta;
  int   ret = GetMQLv(ev, v, beta, trk, opt);
  if (opt == 1 && ret < 0) {
    cout << "ret= " << ret << " ";
    if (ret == -1) cout << "No TrTrack" << endl;
    if (ret == -2) cout << "No BetaH" << endl;
    if (ret == -3) cout << "No TrTrack-BetaH matching" << endl;
    if (ret == -4) cout << "No BetaH->GetBeta()" << endl;
    if (ret == -5) cout << "No Choutko/Chikanian fit" << endl;
    if (ret == -6) cout << "Choutko/Chikanian sign mismatch" << endl; 
    if (ret == -7) cout << "BetaH::GetZ != 1" << endl;
  }
  if (ret < 0) return ret;

  return GetMQL(beta, v, p, ll, opt);
}

int TrMass::GetMQLv(AMSEventR *ev, float *v, float &beta, 
		    TrTrackR *trk, int opt)
{
  float *vmin = TrMass::MQL_vmin;
  float *vmax = TrMass::MQL_vmax;

  beta = 0;
  for (int i = 0; v && i < Nv; i++) v[i] = vmax[i]-(vmax[i]-vmin[i])*1e-7;

  if (!ev) return -9;

  BetaHR *bth = 0;
  if (trk) {
    for (int i = 0; i < ev->NBetaH(); i++)
      if (ev->pBetaH(i)->pTrTrack() == trk) { bth = ev->pBetaH(i); break; }
  }

  if (!trk) {
    ParticleR *part = ev->pParticle(0);
    trk = (part) ? part->pTrTrack() : 0;
    bth = (part) ? part->pBetaH() : 0;
  }
  else {
    for (int i = 0; !bth && i < ev->NBetaH(); i++)
    if (ev->pBetaH(i) && ev->pBetaH(i)->pTrTrack() == trk) bth = ev->pBetaH(i);
  }

  if (!trk) return -1;
  if (!bth) return -2;

  if (bth->pTrTrack() != trk ||
      bth->GetBuildType() != 1) return -3;

  float btof = beta = bth->GetBeta();
  float babs = TMath::Abs(btof);
  if (btof == 0) return -4;

  int it1 = trk->iTrTrackPar(1, 3, 1);  // Choutko fit
  int it2 = trk->iTrTrackPar(3, 3, 1);  // Chikanian fit
  if (it1 < 0 || it2 < 0) return -5;

  float r1 = trk->GetRigidity(it1);
  float r2 = trk->GetRigidity(it2);
  if (r1*r2 <= 0) return -6;

  int nl;
  float pb, rms;
  if (bth->GetZ(nl, pb, 0) != 1) return -7;

  float rgt = r1;
  int nhi = 0, nhx = 0;
  for (int i = 0; i < 7; i++) if (trk->TestHitLayerJ     (i+2)) nhi++;
  for (int i = 0; i < 7; i++) if (trk->TestHitLayerJHasXY(i+2)) nhx++;

  int    qopt = TofClusterHR::DefaultQOptIonW;
  float  qup  = bth->GetQ(nl, rms, 2, qopt, 1100, 0, rgt);
  float  qlow = bth->GetQ(nl, rms, 2, qopt,   11, 0, rgt);

  float qlm = 0;;
  for (int i = 0; i < 7; i++) {
    float ql = trk->GetLayerJQ(i+2, babs);
    if (ql > 0 && (qlm == 0 || ql < qlm)) qlm = ql;
  }

  int nbp = 0;
  for (int j = 0, d = 1; j < 4; j++) {
    if ((bth->GetBetaPattern()/d)%10 == 4) nbp += 2;
    if ((bth->GetBetaPattern()/d)%10 == 3) nbp += 1;
    d *= 10;
  }

  TrdKCluster tk(ev, trk, trk->Gettrdefaultfit());
  int nhtk = 0;
  double llr[3];
  tk.GetLikelihoodRatio_TrTrack(15, llr, nhtk);

  int nhp = nhi;
  nhp += nhx *10;
  nhp += nhtk*100;
  if (!trk->TestHitLayerJ(2)) nhp = -nhp;

  float bt1 = GetBeta (ev,   1);
  float bt2 = GetBeta (ev,  10);
  float bt3 = GetBeta (ev, 100);
  float npk = GetNpick(ev, trk);
  float sgn = (rgt > 0) ? 1 : -1;
  float ll1 = GetLL(1, sgn*1, babs, trk);
  float ll2 = GetLL(1, sgn*2, babs, trk);

  v[0]  = bth->GetNormChi2C();
  v[1]  = bth->GetNormChi2T();
  v[2]  = trk->GetNormChisqX(it1);
  v[3]  = trk->GetNormChisqX(it2);
  v[4]  = trk->GetNormChisqY(it1);
  v[5]  = trk->GetNormChisqY(it2);
  v[6]  = (bt1  > 0) ? (bt1/babs-1)*babs  : vmin[4];
  v[7]  = (bt2  > 0) ? (bt2/babs-1)*babs  : vmin[5];
  v[8]  = (bt3  > 0) ?  bt3/babs-1        : vmin[6];
  v[9]  = (1-pb > 0) ? TMath::Log10(1-pb) : vmax[7];
  v[10] = qup/qlow;
  v[11] = (npk  > 0) ? TMath::Log10(npk)  : vmin[11];
  v[12] = qlm;
  v[13] = TMath::Min(ll1, ll2);

  v[14] = nhp;
  v[15] = nbp;

  for (int i = 0; i < Nv; i++) {
    if (i <= 5 || i == 13)
      v[i] = (v[i] > 0) ? TMath::Log10(v[i]) : vmax[i];

    if (vmin[i] < vmax[i]) {
      if (v[i] <  vmin[i]) v[i] = vmin[i];
      if (v[i] >= vmax[i]) v[i] = vmax[i]-(vmax[i]-vmin[i])*1e-7;
    }
  }

  if (opt == 1) {
    ParticleR *part = ev->pParticle(0);
    if (part) part->Info(0, ev);

    cout << "=== RUN: " << ev->Run() << " Event: " << ev->Event()
	 << " === " << endl;
    cout << " NParticle: " << ev->NParticle()
	 << "  NTrTrack: " << ev->NTrTrack()
	 << "  NBetaH: "   << ev->NBetaH() << endl;
    cout << " NTrdSeg: "   << ev->NTrdSegment()
	 << "  NTrHit: "   << ev->NTrRecHit()
	 << "  NTofCls: "  << ev->NTofCluster()
	 << "  NAntiCls: " << ev->NAntiCluster()
	 << "  NRichHit: " << ev->NRichHit() << endl;
    if (part)
    cout << " Particle-Momentum: " << part->Momentum
	 << "  Beta: " << part->Beta << "  Mass: " << part->Mass << endl;
    cout << " TrTrack-Rigidity: " << r1 << " " << r2
	 << "  NhitI: " << nhi; if (nhi < 6) cout << " <<<";
    cout << endl;
    cout << "        -ChisqX: "
	 << trk->GetNormChisqX(it1) << "  " << trk->GetNormChisqX(it2)
	 << "  ChisqY: "
	 << trk->GetNormChisqY(it1) << "  " << trk->GetNormChisqY(it2) << endl;
    cout << " Track-Qx:";
    for (int i = 0; i < 7; i++) {
      TrRecHitR *h = trk->GetHitLJ(i+2);
      if (h && !h->OnlyY()) cout << Form("  %4.2f", h->GetQ(0, btof));
      else cout << "  ----";
    }
    cout << endl;
    cout << " Track-Qy:";
    for (int i = 0; i < 7; i++) {
      TrRecHitR *h = trk->GetHitLJ(i+2);
      if (h) cout << Form("  %4.2f", h->GetQ(1, btof));
      else cout << "  ----";
    }
    cout << endl;

    cout << " BetaH-Beta: " << btof 
	 << "  Pattern: " << bth->GetBetaPattern();
    if (bth->GetBetaPattern() != 4444) cout << " <<<";
    cout << endl;

    if (part) {
      BetaR *bb = part->pBeta();
      if (bb) cout << " Beta-Beta: " << bb->Beta
		   << "  Pattern: "  << bb->Pattern << endl;
      RichRingR *rr = part->pRichRing();
      if (rr) cout << " RICH-Beta: " << rr->getBeta() << "  "
		   << ((rr->IsNaF()) ? "NaF" : "Agel")
		   << "  Nh: "  << rr->getUsedHits() << " " << rr->getHits()
		   << "  Clean: " << rr->IsClean() << endl;
    }
  }

  return 0;
}

double TrMass::GetMQL(float beta, float *v, float *p, float *ll, int opt)
{
  for (int i = 0; p && i < N; i++) p[i] = 0;

  if (!MQL_par) {
    enum { Np = 5+2 };

    MQL_par = new double[N*Np];

    float par[N*Np] = 
 { 0.05, 0.15, 0.2, 0.3, 0.25, 0.25,  -1,  -1,  -1,-4.4,  0,   -3,   0,0.28,
      3,    3,   3,   3,    3,    3,   1,   1,0.05,   0,  1,    0, 0.8,   3,
   0.05, 0.15, 0.2, 0.3,  0.2, 0.25,0.003,  0,0.05,-4.4,  1, 10.6,   1,   0,
   0.39, 0.57, 0.8, 0.6, 0.74, 0.63,0.05,0.04,0.18,0.52,0.1, 3.25,0.22,0.53,
      1,    1,   1,   1,    1,    1,   1,   1,   1,   1,  1,4.2e4,2.22,1.31,
   0.61, 0.52,0.30,1.63, 1.82, 2.41,0.60,0.30,0.35,0.73,0.63,0.68,0.27,   0,
   0.53, 0.49,0.16,1.08, 0.61, 1.41,1.14,0.53,0.32,0.21,1.41,2.50,0.05,   0 };

    for (int i = 0; i < N*Np; i++) MQL_par[i] = par[i];
  }
  if (!v || opt == 2) return 0;

  double *pr = MQL_par;
  double *p0 = pr; pr += N;
  double *p1 = pr; pr += N;
  double *p2 = pr; pr += N;
  double *p3 = pr; pr += N;
  double *p4 = pr; pr += N;
  double *w  = (beta > 0) ? pr : &pr[N]; pr += N*2;

  float *vmin = TrMass::MQL_vmin;
  float *vmax = TrMass::MQL_vmax;

  float bref = (0 < beta) ? ((beta < 0.9) ? beta : 0.9)
                          : ((beta >  -1) ? beta :  -1);
  float wb = TMath::Erf((0.85-TMath::Abs(beta))/0.03)/2+0.5;

  double lmax = 200;
  double sum  = 0;
  for (int i = 0; i < N; i++) {
    p[i] = (p0[i] < v[i] && v[i] < p1[i]) 
      ? p4[i]*TMath::Exp(-(v[i]-p2[i])/p3[i]
			 *(v[i]-p2[i])/p3[i]) : 1;

    float wp = 1;
    if (i ==  2 && v[i] > vmax[i]-(vmax[i]-vmin[i])*2e-7) p[i] = 0;
    if (i ==  3 && v[i] > vmax[i]-(vmax[i]-vmin[i])*2e-7) p[i] = 0;
    if (i ==  4 && v[i] < vmin[i]+(vmax[i]-vmin[i])*2e-7) p[i] = 0;
    if (i ==  8 && v[i] < vmin[i]+(vmax[i]-vmin[i])*2e-7)
      p[i] = (beta > 0) ? 0.28/bref-0.24 : -0.62/bref-0.49;
    else if      (i == 8) wp = wb;
    if (6 <= i && i <= 7) wp = wb;

    if (p[i] > 1) p[i] = 1;

    float lp = (p[i] > 0) ? -TMath::Log(p[i])*w[i]*wp : lmax;
    sum += lp;

    if (ll) ll[i] = lp;
  }

  int   nh  = (int)(v[14]+0.5);
  int   np  = (int)(v[15]+0.5);
  int   l2  = (nh > 0) ? 1 : 0; nh = TMath::Abs(nh);
  int   ntd = nh/100; nh %= 100;
  int   nx  = nh/10;  nh %= 10;
  float pnp = (np >=  8) ? 1 :
             ((np >=  6) ? 0.01 : 0);

  if ((nh  < 5) ||
      (nh == 5 && nx  < 5) ||
      (nh == 5 && nx == 5 && ntd < 10) ||
         (!l2 && (nx < nh || ntd < 10))) sum += lmax;
  else if (nh == 5) sum += 1;

  sum += -TMath::Log(pnp);

  if (opt == 1) {
    TString str[N] = { "Chi2C", "Chi2T", "ChisqX(1)", "ChisqX(3)",
		       "ChisqY(1)", "ChisqY(3)",
		       "Btrkq/Btof", "Btofq/Btof", "Btrdq/Btof", "1-Prob",
		       "Qup/Qlow", "Npick", "Min(Ql)", "Min(Lp,Ld)" };
		       
    for (int i = 0; i < N; i++) {
      cout << Form(" %-11s  v[%2d]: %6.3f  p[%2d]: %6.4f", 
		   str[i].Data(), i, v[i], i, p[i]);
      if (p[i] < 0.1)   cout << " <";
      if (p[i] < 0.01)  cout << "<";
      if (p[i] < 0.001) cout << "<";
      cout << endl;
    }
    cout << "MQL: " << sum << " Prob: " << GetProb(sum, beta) << endl;
  }

  if (sum >= lmax) sum = lmax*(1-1e-7);
  return sum;
}

#ifdef __ROOTSHAREDLIBRARY__
#include "SpFold.h"

double TrMass::GetProb(double mql, double beta)
{
  enum { N = 7 };

  static double x[N]  = { 1, 2, 5, 10, 15, 50, 120 };
  static double p[N*2+4] = 
    { 0.852, 0.666, 0.397, 0.282, 0.246, 0.160, 0.142, -0.187, 0, 
      0.868, 0.704, 0.435, 0.291, 0.250, 0.171, 0.137, -0.165, 0 };

  static double *d = 0;

  if (!d) {
    d = new double[N*2];
    for(int i=0;i<N*2;i++)d[i]=0;

    SplFit::Spline(N, x, &p[0],   &p[N],     &d[0]);
    SplFit::Spline(N, x, &p[N+2], &p[N*2+2], &d[N]);
  }

  if (mql > x[N-1]) mql = x[N-1];

  double prob = (beta > 0) ? SplFit::Splint(N, x, &p[0],   &d[0], mql)
                           : SplFit::Splint(N, x, &p[N+2], &d[N], mql);  

  if (prob > 1) prob = 1-1e-7;
  if (prob < 0) prob = 0;

  return prob;
}


#include "amschain.h"
#include "TSystem.h"

double TrMass::GetMQL(int run, int event, int opt,
		      float *v, float *p, float *ll)
{
  TString xrd = "root://eosams.cern.ch/";
  TString eos = "/afs/cern.ch/project/eos/installation/0.3.15/bin/eos.select";
  TString sph = "/eos/ams/Data/AMS02/2014/ISS.B950/pass6/";

  static AMSChain *ach = 0;
  static int srun = 0;

  if (!ach) ach = new AMSChain;

  if (run != srun) {
    TString srr = xrd+sph;
    TString str = gSystem->GetFromPipe(eos+" ls "+sph+Form(" | grep %d", run));
           
    TObjArray *sar = str.Tokenize("\n");
    if (sar->GetEntries() == 0) {
      cout << "TrMass::GetMQL-E-No AMSRoot file found" << endl;
      delete sar;
      return -99;
    }

    ach->Reset();
    for (int i = 0; i < sar->GetEntries(); i++)
      ach->Add(srr+sar->At(i)->GetName());
    delete sar;

    int ntr = ach->GetNtrees(), nen = ach->GetEntries();
    if (ntr <= 0 || nen <= 0) {
      cout << "TrMass::GetMQL-E-Invalid Ntrees,Entreis: "
	   << ntr << " " << nen << endl;
      return -98;
    }
    srun = run;
  }

  int ntry = 0, eofs = -1;
  AMSEventR *evt = ach->GetEvent(0);
  eofs = -evt->Event();

  while ((!evt || int(evt->Event()) != event) && ntry++ < 5) {
    evt   = ach->GetEvent(event+eofs);
    eofs -= evt->Event()-event;
  }
  if (!evt) {
    cout << "TrMass::GetMQL-E-No event found for: " << event << endl;
    return -97;
  }
  if (int(evt->Event()) != event) {
    cout << "TrMass::GetMQL-E-Event number mismatch: " 
	 << evt->Event() << " " << event << endl;
    return -96;
  }

  if (opt == 1) cout << endl;
  return GetMQL(evt, 0, opt, v, p, ll);
}

#else

double TrMass::GetProb(double, double)
{
  return 0;
}

double TrMass::GetMQL(int, int, int, float *, float *, float *)
{
  return 0;
}
#endif
#endif
