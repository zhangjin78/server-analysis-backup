#ifndef _ADPMCommon
#define _ADPMCommon

//-------Common_Block USEDPMUS=2(two underscore), =3(no underscore)
#if USEDPMUS == 3
# define DTEVNO dtevno 
# define DTEVT1 dtevt1
# define DTEVT2 dtevt2
# define DTPRTA dtprta
# define DTIMPA dtimpa
# define DTGLCP dtglcp
# define POPRCS poprcs
# define POEVT1 poevt1
# define DTCOMP dtcomp
# define DPMDIR dpmdir
#else
# define DTEVNO dtevno_ 
# define DTEVT1 dtevt1_
# define DTEVT2 dtevt2_
# define DTPRTA dtprta_
# define DTIMPA dtimpa_
# define DTGLCP dtglcp_
# define POPRCS poprcs_
# define POEVT1 poevt1_
# define DTCOMP dtcomp_
# define DPMDIR dpmdir_
#endif
extern "C" {

/*========================================================*/
/* COMMON/DTEVNO/NEVENT,ICASCA                            */
/*--------------------------------------------------------*/
 typedef struct {
   int    nevent;
   int    icasca;
 } DtevnoCommon;
 extern DtevnoCommon DTEVNO;

/*========================================================*/
/* COMMON/DTEVT1/NHKK,NEVHKK,ISTHKK(NMXHKK),IDHKK(NMXHKK),*/
/*      JMOHKK(2,NMXHKK),JDAHKK(2,NMXHKK),                */
/*      PHKK(5,NMXHKK),VHKK(4,NMXHKK),WHKK(4,NMXHKK)      */
/*--------------------------------------------------------*/
 typedef struct {
   int    nhkk;
   int    nevhkk;
   int    isthkk[250000];
   int    idhkk[250000];
   int    jmohkk[250000][2];
   int    jdahkk[250000][2];
   double  phkk[250000][5];
   double  vhkk[250000][4];
   double  whkk[250000][4];
 } Dtevt1Common;
 extern Dtevt1Common DTEVT1;

/*========================================================*/
/* COMMON/DTEVT2/IDRES(NMXHKK),IDXRES(NMXHKK),NOBAM(NMXHKK),*/
/*               IDBAM(NMXHKK),IDCH(NMXHKK),NPOINT(10),   */
/*               IHIST(2,NMXHKK),PHRES(2,NMXHKK)          */
/*--------------------------------------------------------*/
 typedef struct {
   int    idres[250000];
   int    idxres[250000];
   int    nobam[250000];
   int    idbam[250000];
   int    idch[250000];
   int    npoint[10];
   int    ihist[250000][2];
   double phres[250000][2];
 } Dtevt2Common;
 extern Dtevt2Common DTEVT2;

/*========================================================*/
/* COMMON/DTPRTA/IT,ITZ,IP,IPZ,IJPROJ,IBPROJ,IJTARG,IBTARG*/
/*--------------------------------------------------------*/
 typedef struct {
   int    it;
   int    itz;
   int    ip;
   int    ipz;
   int    ijproj;
   int    ibproj;
   int    ijtarg;
   int    ibtarg;
 } DtprtaCommon;
 extern DtprtaCommon DTPRTA;

/*========================================================*/
/* COMMON /DTIMPA/ BIMIN,BIMAX,XSFRAC,ICENTR              */
/*--------------------------------------------------------*/
 typedef struct {
   double    bimin;
   double    bimax;
   double    xsfrac;
   double    icent;
 } DtimpaCommon;
 extern DtimpaCommon DTIMPA;

/*========================================================*/
/* COMMON /DTGLCP/RPROJ,RTARG,BIMPAC,NWTSAM,NWASAM,NWBSAM,*/
/*                NWTACC,NWAACC,NWBACC                    */
/*--------------------------------------------------------*/
 typedef struct {
   double  rproj;
   double  rtarg;
   double  bimpac;
   int  nwtsam;
   int  nwasam;
   int  nwbsam;
   int  nwtacc;
   int  nwtaac;
   int  nwtbac;
 } DtglcpCommon;
 extern DtglcpCommon DTGLCP;

/*========================================================*/
/* COMMON /POPRCS/IPROCE,IDNODF,IDIFR1,IDIFR2,IDDPOM,     */
/*                IPRON(15,4)                             */
/*--------------------------------------------------------*/
 typedef struct {
   int  iproce;
   int  idnodf;
   int  idifr1;
   int  idifr2;
   int  iddpom;
   int  ipron[15][4];
 } PoprcsCommon;
 extern PoprcsCommon POPRCS;

/* COMMON /POEVT1/ NEVHEP,NHEP,ISTHEP(NMXHEP),IDHEP(NMXHEP),
     &                JMOHEP(2,NMXHEP),JDAHEP(2,NMXHEP),PHEP(5,NMXHEP),
     &                VHEP(4,NMXHEP)
*/
 typedef struct {
  int    nevhep;
  int    nhep;
  int    isthep[40000];
  int    idhep [40000];
  int    jmohep[40000][2];
  int    jdahep[40000][2];
  double phep  [40000][5];
  double vhep  [40000][4];
 } Poevt1Common;
 extern Poevt1Common POEVT1;

/* COMMON /DTCOMP/ EMUFRA(NCOMPX),IEMUMA(NCOMPX),IEMUCH(NCOMPX),
     &                NCOMPO,IEMUL
*/
 typedef struct {
  double emufra[100];
  int    iemuma[100];
  int    iemuch[100];
  int    ncompo;
  int    iemul;
 } DtcompCommon;
 extern DtcompCommon DTCOMP;

 typedef struct {
  char dpmdir[132];
 } DpmdirCommon;
 extern DpmdirCommon DPMDIR; 

}

//-----Function this part, USEDPMUS=2(two underscore),=3(no underscore)
#if USEDPMUS == 2
# define dt_init   dt_init__
# define dt_dtuini dt_dtuini__
# define dt_getemu de_getemu__
# define dt_kkinc  dt_kkinc__
# define pho_phist pho_phist__
# define dt_dtuout dt_dtuout__
# define dt_rndm   dt_rndm__
# define dt_rndmst dt_rndmst__
# define dt_rndmin dt_rndmin__
# define dt_rndmou dt_rndmou__
# define dpmjet_openinp dpmjet_openinp__
#elif USEDPMUS == 3

#else
# define dt_init   dt_init_
# define dt_dtuini dt_dtuini_
# define dt_getemu de_getemu_
# define dt_kkinc  dt_kkinc_
# define pho_phist pho_phist_
# define dt_dtuout dt_dtuout_
# define dt_rndm   dt_rndm_
# define dt_rndmst dt_rndmst_
# define dt_rndmin dt_rndmin_
# define dt_rndmou dt_rndmou_
# define dpmjet_openinp dpmjet_openinp_
#endif
extern "C" {
  void    dt_init(int & , double &, int & , int &,
               int &, int &, int &, int &);
  void    dt_dtuini(int & , double &, int & , int &,
               int &, int &, int &, int &);
  double  dt_getemu(int &, int &, int &, int &);
  void    dt_kkinc(int &, int &, int &, int &,
               int &, double &, int &, int &);
  void    pho_phist(int &, double &);
  void    dt_dtuout();
  double  dt_rndm(int &);
  void    dt_rndmst(int &, int &, int &, int &);
  void    dt_rndmin(int &, int &, int &, int &, int &, int &);
  void    dt_rndmou(int &, int &, int &, int &, int &, int &);
  void    dpmjet_openinp(int*, int*, char*);
}
#if defined _OPENMP and defined G4MULTITHREADED
#pragma omp threadprivate (DTPRTA,DTEVT1,DTEVT2,DTEVNO,POEVT1,DTCOMP,DPMDIR,POPRCS,DTGLCP,DTIMPA)
#endif
#endif
