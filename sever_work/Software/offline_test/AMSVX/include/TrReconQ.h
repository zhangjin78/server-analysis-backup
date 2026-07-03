// $Id: TrReconQ.h,v 1.36 2022/06/07 14:36:54 qyan Exp $ 
#ifndef __TrReconQ__
#define __TrReconQ__

//////////////////////////////////////////////////////////////////////////
///
///\class TrReconQ
///\brief New Procedures for the Track (TrTrackR) Reconstruction. The AMS Track Reconstruction was switched to this starting from Pass7 in 2018.
///
///\date  2018/05/12 QY  Pass7 track reconstruction algorithm
///\date  2022/06/04 QY  Pass8 track reconstruction algorithm
///
/// $Date: 2022/06/07 14:36:54 $
///
/// $Revision: 1.36 $
///
///////////////////////////////////////////////////////////////////////////

#include "typedefs.h"
#include "point.h"

#include "TrCluster.h"
#include "TrTrack.h"
#include "TrRecHit.h"
#include "TMath.h"
#include "Vertex.h"
#include "amsdbc.h"
#include <cmath>
#include <vector>
#include <map>
#include <algorithm>
#include "tkpatt.h"
#include "TObject.h"
#include "root.h"
#include "TrRecon.h"

/// @cond RECDOX
class ccL{//coordinate list
  public:
    double cz;
    double cy;
    int    lay;
    int    ncx;
    int    ihit[90];//index of this hit
    int    iclx[90];
    double cx[90];//x coord
    double cy1[90];//y coord
    double cz1[90];//z coord
    int    imlt[90];// multiplicity of this hit
    double qx[90];
    double mqx;
    double qy;
    bool   hasX;
    int    ihity;
    int    icly;
    double rx2[2];
    int    ihitp;
    ccL()
    { 
      icly =-9999;
      cz = -9999;
      cy = -9999;
      lay = -9999;
      ncx=0;
      hasX=false;
      ihity=-9999;
      for(int i=0;i<90;i++) { iclx[i] = -1; qx[i] = -1;}
      qy  = -1;
      mqx = -1;
      rx2[0]=rx2[1]=0;
      ihitp=-1;
    }
    void AddX(double x,double y, double z, int ihit0, int imlt0, int lay0, int icly0, int iclx0, double tqx, double tqy){
      hasX=true;
      icly = icly0;
      if((ncx>=70&&ihit0!=ihitp)||ncx>=90) return;//~7 mult/hit, ~10 hits 
      cx[ncx] = x;
      cy1[ncx] = y;
      cz1[ncx] = z;
      qx[ncx] = tqx;
      if( tqx > mqx  ) mqx = tqx;
      iclx[ncx] = iclx0;
      if(cy == -9999 || ncx==0 ) cy=y;//average
      if(cz == -9999 || ncx==0 ) cz=z;//average
      else  {
        cy  = 1.0*(cy*ncx + y)/(ncx+1) ;
        cz  = 1.0*(cz*ncx + z)/(ncx+1) ;
      }

      if(qy < 0 || ncx==0 ) qy=tqy;
      ihitp = ihit[ncx] = ihit0;
      imlt[ncx] = imlt0;
      if(lay == -9999  ) lay=lay0;

      ncx++;
    }
    void SetOnlyY(double cy0, double cz0, int ihity0, int lay0, int icly0, double tqy, double xl0=0, double xh0=0)
    {
      icly = icly0;
      ihity = ihity0;
      if(cy == -9999) cy = cy0;
      if(cz == -9999) cz = cz0;
      if(qy < 0 )     qy = tqy;
      if(lay == -9999  ) lay=lay0;
      rx2[0]=xl0; rx2[1]=xh0;
    }
};


class semiIndex{
  public:
    int nLength;
    bool isBad;
    int iccL[7];
    int lay[7];
    double hfqql;
    int ilay[10];
    void clear(){
      nLength = 0;isBad = false;
      hfqql=0;
      for(int i=0;i<7;i++){ lay[i]=iccL[i]=-1;}
      for(int i=0;i<10;i++)ilay[i]=-1;
    } 
    void AddHit(int iccL0){
      if(nLength>=7) return;
      iccL[nLength] = iccL0;
      nLength++;
    }
    void Setlay(int j,int lay0){
      lay[j]=lay0;
      ilay[lay0]=j;
    }
    int  GetNPlane(){
      bool  sz[7] = {0,0,0,0,0,0,0 };
      for(int i=0;i<nLength;i++) {
        int play=lay[i];
        if(play>=2 && play<=8)sz[play-2]=true;//L2-L8
      }
      int nplan=0;
      if(sz[0])       nplan++;
      if(sz[1]||sz[2])nplan++;
      if(sz[3]||sz[4])nplan++;
      if(sz[5]||sz[6])nplan++;
      return nplan;
    }
    int remove(int lay0){
      int ip=ilay[lay0];
      if(ip<0)return 0;//fast return
      hfqql-=2.3;
      nLength--;
      for(int i=ip;i<nLength;i++){
        lay[i]=lay[i+1];
        iccL[i]=iccL[i+1];
        ilay[lay[i]]=i;
      }
      ilay[lay0]=-1;//remove index
      return 1;
    }
    semiIndex(){clear();}
    bool operator <(const semiIndex &o) const{
      return (hfqql>o.hfqql);
    }
    bool same(const semiIndex &o){
      if(nLength!=o.nLength)return 0;
      for(int i=0;i<nLength;i++){
        int j=o.ilay[lay[i]];
        if(j<0)return 0;
        if(iccL[i]!=o.iccL[j])return 0;
      }
      return 1;
    }
};


class fastFitQu{
  public:
    double cx[7];
    double cy[7];
    double cz[7];
    int nhit;
    fastFitQu(){
      nhit = 0;
    }
    void addYZ(double y, double z){
      if(nhit>=7) return;
      cy[nhit] = y;
      cz[nhit] = z;
      nhit ++;
    }
    void sort(){
      for(int i=0;i<nhit-1;i++){
        for(int j=0; j<nhit-i-1;j++){
          if(cz[j] > cz[j+1]){
            double tmp;
            tmp = cz[j];        cz[j] = cz[j+1];        cz[j+1] = tmp;
            tmp = cy[j];        cy[j] = cy[j+1];        cy[j+1] = tmp;
            tmp = cx[j];        cx[j] = cx[j+1];        cx[j+1] = tmp;
          }}}
    }
    double guessY( double tz ) {
        return  TrRecon::Intpol1(cz[0],cz[nhit-1],cy[0],cy[nhit-1],tz );
    }
    void bkstack() {  if(nhit > 0) nhit--;  }
    double getCsq() {
      sort();
      double sum =0;
      double sums = 0;
      for(int i=1;i<nhit-1;i++)
      {
        double tvec = getvec( cy[i-1],cz[i-1],cy[i],cz[i],cy[i+1],cz[i+1]);
        sum += tvec;
        sums += tvec*tvec;
      }
      return sqrt( sums/(nhit-2) - (sum/(nhit-2))*(sum/(nhit-2)));
    }
    void clear(){nhit = 0;}
    double getMean(){
      sort();
      double sum =0;
      double sums = 0;
      for(int i=1;i<nhit-1;i++) {
        double tvec = getvec( cy[i-1],cz[i-1],cy[i],cz[i],cy[i+1],cz[i+1]);
        sum += tvec;
        sums += tvec*tvec;
      }
      return sum/(nhit-2);
    }

    double getvec(double ay,double az,double by, double bz,double cy, double cz )
    {
      double v1y = (ay-by)/sqrt((az-bz)*(az-bz) + (ay-by)*(ay-by) );
      double v1z = (az-bz)/sqrt((az-bz)*(az-bz) + (ay-by)*(ay-by) );
      double v2y = (cy-by)/sqrt((cz-bz)*(cz-bz) + (cy-by)*(cy-by) );
      double v2z = (cz-bz)/sqrt((cz-bz)*(cz-bz) + (cy-by)*(cy-by) );
      return    v1z*v2y - v1y*v2z;
    }
};


class candi{
  public:
    int nLength;
    int nLengthX;
    int    qhitbit[2];
    int    qbit;
    double quality[3];
    double chis[2][3];//simple or VC
    double rig[2];//simple or VC
    int    ntofsl;
    double toftx[2];
    double tofsx[2];
    int ihit[7];
    int ihity[7];
    int iclx[7];
    int icly[7];
    int imlt[7];
    int lay[7];
    int iccL[7];
    int ix[7] ;
    double cx[7];
    double cy[7];
    double cz[7];
    double qx[7];
    double qy[7];
    int isBad;
    bool is19 ;
    int ic19[2] ;
    int imlt19[2];
    int n19;
    double innerQ[2];
    int nrm[2];
    int ilay[10];//use lay to fast find index
    vector<map<int,int> > imlto;
    int iparent;
    double GetinnerQ(int ixy=2){
      if     (ixy==0){return (nLengthX==0)? 0: innerQ[0]/nLengthX;}
      else if(ixy==1){return (nLength==0)?  0: innerQ[1]/nLength; }
      else           {return ((nLength+nLengthX)==0)? 0: (innerQ[0]+innerQ[1])/(nLength+nLengthX);}
    }
    void clear(){
      nLength = 0;
      nLengthX = 0;
      qhitbit[0]=qhitbit[1]=0;
      qbit = 0;
      quality[0]=quality[1]=quality[2]=0;
      for(int iv=0;iv<2;iv++){
        for(int ixy=0;ixy<3;ixy++)chis[iv][ixy]=1e100;
        rig[iv]=0;
      }
      ntofsl=0;
      toftx[0]=toftx[1]=tofsx[0]=tofsx[1]=9999;
      isBad=0;
      iparent=-1;
      is19 = false;
      n19=0;
      innerQ[0]=innerQ[1]=0;
      for(int i=0;i<7;i++){
        ihit[i]=ihity[i]=iclx[i]=icly[i]=imlt[i]=lay[i]=iccL[i]=ix[i]=-1;
        cx[i]=cy[i]=cz[i]=-9999;
        qx[i]=qy[i]=0;
      }
      for(int i=0;i<10;i++)ilay[i]=-1;
      for(int i=0;i<2;i++){
        ic19[i] = -1; imlt19[i]=-1;
      }
      nrm[0]=nrm[1]=0;
      imlto.clear();
    }
    void sort(){
      for(int i=0; i<nLength-1; i++){
        if(lay[i]<0) continue;
        int tmp;
        double tmpcc;
        for(int j=0;j<nLength-i-1;j++){
          if(lay[j]<0) continue;
          if(lay[j]>lay[j+1]){
            tmp =  ihit[j];  ihit[j] =  ihit[j+1];  ihit[j+1] = tmp;
            tmp = ihity[j]; ihity[j] = ihity[j+1]; ihity[j+1] = tmp;
            tmp =  iclx[j];  iclx[j] =  iclx[j+1];  iclx[j+1] = tmp;
            tmp =  icly[j];  icly[j] =  icly[j+1];  icly[j+1] = tmp;
            tmp =  imlt[j];  imlt[j] =  imlt[j+1];  imlt[j+1] = tmp;
            tmp =  lay[j];    lay[j] =  lay[j+1];    lay[j+1] = tmp;
            tmp =  iccL[j];  iccL[j] =  iccL[j+1];  iccL[j+1] = tmp;
            tmp =    ix[j];    ix[j] =    ix[j+1];    ix[j+1] = tmp;
            tmpcc =  cx[j];    cx[j] =    cx[j+1];    cx[j+1] = tmpcc;
            tmpcc =  cy[j];    cy[j] =    cy[j+1];    cy[j+1] = tmpcc;
            tmpcc =  cz[j];    cz[j] =    cz[j+1];    cz[j+1] = tmpcc;
            tmpcc =  qx[j];    qx[j] =    qx[j+1];    qx[j+1] = tmpcc;
            tmpcc =  qy[j];    qy[j] =    qy[j+1];    qy[j+1] = tmpcc;
            ilay[lay[j]]=j;  ilay[lay[j+1]]=j+1;
          }}}
    }
    void Add19(int ihit0, int imlt0){//just used for L1 and L9
      if(n19>=2) return;
      is19=true;
      ic19[n19] = ihit0;
      imlt19[n19] = imlt0;
      n19++;
    }
    int AddHit(int ihit0, int iclx0, int icly0, int imlt0, int lay0, int iccL0, int ix0, double cx0, double cy0, double cz0, double qx0, double qy0){
      if(nLengthX>=7) return -1;
      ihit[nLength] = ihit0;
      iclx[nLength] = iclx0;
      icly[nLength] = icly0;
      imlt[nLength] = imlt0;
      lay[nLength]  = lay0;
      iccL[nLength] = iccL0;
      ix[nLength]   = ix0;
      cx[nLength] = cx0;
      cy[nLength] = cy0;
      cz[nLength] = cz0;
      qx[nLength] = qx0;
      qy[nLength] = qy0;
      ilay[lay0]=nLength;
      nLengthX++;
      nLength++;
      innerQ[0]+=qx0;
      innerQ[1]+=qy0;
      return 2;
    }
    int AddSemi(int ihity0, int icly0, int lay0, int iccL0, double cy0, double cz0, double qy0){
      int ip=ilay[lay0];
      if(ip>=0){ihity[ip]=ihity0; return -1;}//has this hit
      if(nLength>=7) return -1;
      ihit[nLength] = ihity0;
      ihity[nLength]= ihity0;
      icly[nLength] = icly0;
      lay[nLength] = lay0;
      iccL[nLength] = iccL0;
      cy[nLength] = cy0;
      cz[nLength] = cz0;
      qy[nLength] = qy0;
      ilay[lay0]=nLength;
      nLength++;
      innerQ[1]+=qy0;
      return 1;
    }
    bool hasPlane(int pl0,int ixy=1){
      if(pl0<1 || pl0>4) return false;
      for(int i=0;i<nLength;i++){
        if(ixy!=1&&ix[i]<0)continue;
        int pl=(lay[i]+1)/2;
        if(pl0==pl) return true;
      }
      return false;
    }
    int  GetNPlane(int ixy=1){
      bool  sz[7] = {0,0,0,0,0,0,0 };
      for(int i=0;i<nLength;i++) {
        if(ixy!=1&&ix[i]<0)continue;
        int play=lay[i];
        if(play>=2 && play<=8)sz[play-2]=true;
      }
      int nplan=0;
      if(sz[0])       nplan++;
      if(sz[1]||sz[2])nplan++;
      if(sz[3]||sz[4])nplan++;
      if(sz[5]||sz[6])nplan++;
      return nplan;
    }
    int removeHit(int lay0,int ixy=2){
      int ip=ilay[lay0];//fast accessor
      if(ip<0||(ixy==0&&ix[ip]<0))return 0;
      if(ix[ip]>=0){//remove X
        innerQ[0]-=qx[ip];
        quality[0]-=1.3;
        if(qhitbit[0]&(1<<lay0)){quality[0]-=0.35; qhitbit[0]&=~(1<<lay0);}
        nLengthX--;
        nrm[0]++;
      }
      if(ixy==0&&ihity[ip]>=0){//remove X only && has ihity
        ihit[ip]=ihity[ip];  
        iclx[ip]=-1;
        imlt[ip]=-1;
        ix[ip]=-1;
        cx[ip]=-9999;
        qx[ip]=0;
        return 1;
      }
      int nplan0=GetNPlane();//remove Y
      if     (nplan0>=4)quality[0]-=190;
      else if(nplan0==3)quality[0]-=50;
      innerQ[1]-=qy[ip];
      quality[0]-=23;
      if(qhitbit[1]&(1<<lay0)){qhitbit[1]-=5.9; qhitbit[1]&=~(1<<lay0);}
      nLength--;
      nrm[1]++;
      for(int i=ip;i<nLength;i++){
        ihit[i]=ihit[i+1];
        ihity[i]=ihity[i+1];
        iclx[i]=iclx[i+1];
        icly[i]=icly[i+1];
        imlt[i]=imlt[i+1];
        lay[i]=lay[i+1];
        iccL[i]=iccL[i+1];
        ix[i]=ix[i+1];
        cx[i]=cx[i+1];
        cy[i]=cy[i+1];
        cz[i]=cz[i+1];
        qx[i]=qx[i+1];
        qy[i]=qy[i+1];
        ilay[lay[i]]=i;
      }
      ilay[lay0]=-1;//remove index
//      imlto
      int nplan1=GetNPlane();
      if     (nplan1>=4)quality[0]+=190;
      else if(nplan1==3)quality[0]+=50;
      return 2;
    }
    double  Getquality() const{
      return quality[0]+quality[1]+quality[2];
    }
    candi(){clear();}
    bool operator <(const candi &o) const{
      return (Getquality()>o.Getquality());
    }
    bool operator==(const candi &o) const{
      if(nLength!=o.nLength||nLengthX!=o.nLengthX)return 0;
      for(int i=0;i<nLength;i++){
        if(ihit[i]!=o.ihit[i]||imlt[i]!=o.imlt[i])return 0;
      }
      return 1;
    }
    bool samehit(const candi &o,int ixy) const {
      if(nLength!=o.nLength)return 0;
      for(int i=0;i<nLength;i++){
        if(ixy==0){
          if(iclx[i]!=o.iclx[i])return 0;
        }
        else if(ixy==1){
          if(icly[i]!=o.icly[i])return 0;
        }
        else {
          if(ihit[i]!=o.ihit[i])return 0;
        }
      }
      return 1;
    }
    int getnsamehit(const candi &o,int ixy) const{
      int nsame=0;  
      for(int i=0;i<nLength;i++){
        int j=o.ilay[lay[i]];
        if(j<0)continue;
        if(ixy==0){
          if(iclx[i]>=0&&iclx[i]==o.iclx[j])nsame++;
        }
        else if(ixy==1){
          if(icly[i]==o.icly[j])nsame++;
        }
        else {
          if(ihit[i]==o.ihit[j])nsame++;
        }
      }
      return nsame;
    }
    int addimlto(const candi &o){
      if(this==&o)return 0;
      if(nLength!=o.nLength||nLengthX!=o.nLengthX)return -1;
      map<int,int> imlts;
      for(int i=0;i<o.nLength;i++){
        imlts[o.lay[i]]=o.imlt[i];
      }
      imlto.push_back(imlts);
      return 1;
    }
};

class vcos{
  public:
    vcos(){}
    double getcosyz(double *a,double *b,double *c)
    {
      return ((a[1]-b[1])*(c[1]-b[1])+(a[2]-b[2])*(c[2]-b[2]))/
        (sqrt((a[1]-b[1])*(a[1]-b[1])+(a[2]-b[2])*(a[2]-b[2]))*
         sqrt((c[1]-b[1])*(c[1]-b[1])+(c[2]-b[2])*(c[2]-b[2])));
    }
    double getcosyz(ccL  a, ccL b, ccL c)
    {
      return
        sqrt((a.cy-b.cy)*(a.cy-b.cy)+(a.cz-b.cz)*(a.cz-b.cz)) == 0 ||
        sqrt((c.cy-b.cy)*(c.cy-b.cy)+(c.cz-b.cz)*(c.cz-b.cz)) ==0  ? -1:
        ((a.cy-b.cy)*(c.cy-b.cy)+(a.cz-b.cz)*(c.cz-b.cz))/
        sqrt((a.cy-b.cy)*(a.cy-b.cy)+(a.cz-b.cz)*(a.cz-b.cz))/
        sqrt((c.cy-b.cy)*(c.cy-b.cy)+(c.cz-b.cz)*(c.cz-b.cz));
    }
    double getcosxz(double *a,double *b,double *c)
    {
      return    (sqrt((a[0]-b[0])*(a[0]-b[0])+(a[2]-b[2])*(a[2]-b[2]))*
          sqrt((c[0]-b[0])*(c[0]-b[0])+(c[2]-b[2])*(c[2]-b[2]))) == 0 ? -1 :
        ((a[0]-b[0])*(c[0]-b[0])+(a[2]-b[2])*(c[2]-b[2]))/
        (sqrt((a[0]-b[0])*(a[0]-b[0])+(a[2]-b[2])*(a[2]-b[2]))*
         sqrt((c[0]-b[0])*(c[0]-b[0])+(c[2]-b[2])*(c[2]-b[2])));
    }
    double getcosxz(double ax,  double az, double bx, double bz, double cx, double cz)
    {
      return
        (sqrt((ax-bx)*(ax-bx)+(az-bz)*(az-bz))*
         sqrt((ax-bx)*(cx-bx)+(cz-bz)*(cz-bz))) == 0
        || (  (ax-bx)*(cx-bx) + (az-bz)*(cz-bz) == 0 )  ?  -1 :
        ( (ax-bx)*(cx-bx) + (az-bz)*(cz-bz))/
        (sqrt((ax-bx)*(ax-bx)+(az-bz)*(az-bz))*
         sqrt((cx-bx)*(cx-bx)+(cz-bz)*(cz-bz)));
    }
};

struct nix_L{
   std::vector<int> mixL;
   std::vector<int> nixL;
   std::vector<int> inx;
   int ntmp;
   int nlevel;
   nix_L(){ntmp=0;nlevel=0;}
   void Add(int ivl,int ix)
    {
      if(nlevel>=7) return;
       nixL.push_back(ivl);
       ntmp += ivl;
       mixL.push_back(ntmp);
       inx.push_back(ix);
       nlevel++;
    }
    int GetLevel( int idx)
    {
      if (idx < mixL[0] ) return 0;
      for(int i=1;i<nlevel;i++  )
      {
        if(  idx >= mixL[i-1] &&  idx < mixL[i]  )  return i;
      }
      return -1;
    }

    int GetIndex(int ivl ,int &plc)
    {
      if(ivl<mixL[0]) {
        plc = ivl;
        return inx[0];
      }
      else {
        for(int i=1;i<nlevel;i++) {
            if(ivl<mixL[i]&& ivl>= mixL[i-1])
            {
              plc = ivl - mixL[i-1];
              return inx[i];
            }
        }
      }
      return 0;
    }
};
/// @endcond


class TrReconQ: public TrRecon{
public:
  /// Constructor
  TrReconQ(AMSEventR *ttevt=0);
  /// Destructor 
  ~TrReconQ(){
    Clear();
  };
  void Clear();
  /// Build all TrTracks
  int BuildTracks(int trstat = 0, int hist = 0);
  /// Build TrTracks from given hits set
  int BuildAll();
  /// NO of tracks.
  int GetNTrTrack();
  /// Access to itk's track
  TrTrackR *pTrTrack(int itk);
  /// Init Build
  int InitBuild(int select_tag=0);
  /// Build ccList
  int BuildccList(vector<ccL> &ccList, vector<ccL> &ccList19, int select_tag, int nmaxcclistscale=1);
  /// Build semiIdx
  int BuildsemiIdx(vector<semiIndex> &semiIdx, vector<ccL> &ccList,int select_tag, float kiklimtscale=1);
  /// Build TrkCd
  int BuildtrkCd(vector<candi> &trkCd, vector<semiIndex> &semiIdx, vector<ccL> &ccList, vector<ccL> &ccList19, float xwscale=1.);
  /// Build TrTrack
#ifndef __ROOTSHAREDLIBRARY__
  AMSTrTrack *BuildTrack(candi &pcad, int select_tag);
#else
  TrTrackR   *BuildTrack(candi &pcad, int select_tag);
#endif
  /// Build TrTrack
  int BuildTrack(vector<candi> &trkCd, int select_tag, int nmaxtracks=-1);
  /// Try to Dump Hits if deviate too much from track.
  int TryDumpHits( TrTrackR *track, int idx[3], int ixy);
  /// Try to Drop YHits if deviate too much from track.
  int TryDropYHits(TrTrackR *track, int mfit);
  /// Try to Drop XHits if deviate too much from track.
  int TryDropXHits(TrTrackR *track, int mfit);
  /// Try to Merge (refind) External or inner hits to track
  int TryMergeHits(TrTrackR *track, int mfit, int select_tag, int iadd[10], int mlay=-1);//-1 external, -2 inner
  /// Inner track reconstruction
  int ProcessTrackInner(TrTrackR *track, int select_tag);
  /// external track reconstruction
  int ProcessTrackExt(TrTrackR *track, int mfit, int select_tag);
  /// Merge tracks to VCon
  int MergeClean();
  /// Reset all hits status in production
  void ResetHitStat();
  /// Reset tracker hit to used
  void ResetHitUsed();
  /// Calculate the nuclei status
  double evaNuclei(int reorderhits);
  /// Track type definition
  int _tkStat;
  /// reach cclist cut
  int _cutccList;
  /// Add Rebuild tracks from existing ntuple.
  int AddRebuildTracks( int rtype );
  /// Add TrTracks to the current AMSEventR
  /*!
   * \param[in] clear   Clear TrTrack vector in AMSEventR before adding
   * \param[in] update  Update association in ParticleR
   * \return    Number of tracks added
   */
  int AddTracks(int clear = 1, int update = 1);
  /** Version of the TrTree ROOT files for "AMSEventR::UseReTrackGroup() / TrReconQ::AddRebuildTracks / TrReconQ::AddTrackFromIndex".
   * Version 3: Created with TRCLFFKEY.TrParticleType == 1 and AMS software version B1100. Covers data until 26th November 2016.
   * Version 2: Created with TRCLFFKEY.TrParticleType == 1 and AMS software version ?. Covers data until 26th May 2016.
   * Version 1: Created before TRCLFFKEY.TrParticleType was added, AMS software version ?. Covers data until 26th May 2016.
   */
  static int TrTreeVersion;
  /// preferance of nuclei reconstruction, tight Q; default 0=no; 1=yes;
  static int BdNclei;
protected:
  /// Reorder Tag
  int    _reorderHits;
  /// nuclei optimization
  bool   _isNuclei;
  double _qNuclei;
  /// Container of hits
  vector<TrRecHitR*> _rhits;
  /// Container of built tracks
#ifndef __ROOTSHAREDLIBRARY__
  vector<AMSTrTrack*> _atracks;
#else
  vector<TrTrackR*> _atracks;
#endif
  /// Set New file address
  int SetBuildAddress();
  /// Recover tracks from rebuild ntuple 
  int AddTrackFromIndex( int isearch ,int rtype   );
  /// Fast location of Events.
  int SearchEventFast();
  /// false pointer of Runs
  static unsigned int flsrun ;
  /// history pointer of filename
  static TString hisname ;
  /// history pointer of Runs
  static unsigned int hisrun ;
  /// history pointer of TTree
  static TChain *hisTree;
  static int _rrun;
  static int _revt;
  static int _rentry;
  AMSEventR *evt; 

  //............for vertex reconstruction............
public: 
  /// build tracks and vertex 
  int BuildTracksVertex();
  /// get the AMSPoint vertex without vertex fit
  int GetVertex(AMSPoint &vtex , AMSPoint &dist , int idx[2]);
  /// build vertex main function
  double BuildVertex();// AMSPoint &siVertex, AMSPoint &tktkdist );
  /// track- track distance.
  double TkTkDist( TrTrackR* tr1, TrTrackR* tr2 , AMSPoint &vertex, AMSPoint &dist );
  /// select the primary track
  int PriTrkSelt( vector<TrTrackR*> &trks, vector<int> &idx) ;
  /// build vertex along trtrack reconstruction , 0=no, 1=yes 
  static int BdVtx;
  int trType;
protected:
  /// simple vertex position
  AMSPoint _svtx; 
  /// simple vertex distance
  AMSPoint _sdst;
  /// simple vertex distance sqrt(x^2 +y^2)
  double   _sddst;
  /// simple vertex track index
  int _vtrIdx[2];
  /// number of simple vertex
  int _nsvtx;
};



#endif
