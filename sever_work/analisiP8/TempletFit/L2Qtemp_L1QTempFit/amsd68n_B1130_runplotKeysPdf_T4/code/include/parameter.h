#ifndef PARAMETER_H
#define PARAMETER_H

#include "database.h" //2021.08.16

//------track
const int NTKL=9;
const float tk_pz[NTKL]={159.04,53.05,29.22,25.24,1.706,-2.292,-25.26,-29.25,-136.03};
const int icffv=1;
const int NSPAN=3;

//------fiducial constant
const double cirr[]={62,62,46,46,46,46,46,46,43};
const double ciry[]={47,40,44,44,36,36,44,44,29};

//------Rich
const double dRichCor = -71.87; //from AMS/include/richdbc.h richconst::RICradposs //2022.05.30

//------Ecal
const double ecal_pz=-143;
const double ecal_thickness=1.85;
const double ecalxy[]={32,32};
//2019.05.28: detail Ecal coordinate, from Mercedes (see file ~/public/ntple_prod/utils/ecal_gendim.C)   -----------
static float ecal_plane_top_z[18]={-142.827, -143.747, -144.677, -145.597, -146.527, -147.447, -148.377, -149.297, -150.227, -151.147, -152.077, -152.997, -153.927, -154.847, -155.777, -156.697, -157.627, -158.547};/// ECAL first fiber layer center z coordinate for each plane in AMS ref system (cm)
static float ecal_plane_center_z[18]={-143.195, -144.115, -145.045, -145.965, -146.895, -147.815, -148.745, -149.665, -150.595, -151.515, -152.445, -153.365, -154.295, -155.215, -156.145, -157.065, -157.995, -158.915};
static float ecal_plane_bottom_z[18]={-143.563, -144.483, -145.413, -146.333, -147.263, -148.183, -149.113, -150.033, -150.963, -151.883, -152.813, -153.733, -154.663, -155.583, -156.513, -157.433, -158.363, -159.283};/// ECAL last fiber layer center z coordinate for each plane in AMS ref system (cm)
const float ecal_fl_pitchz[2]={0.184, 0.194};//ECAL distance between adjacent fiber layers 0: belonging to same SL, 1: belonging to different Sls

static float ecal_min[2]={-32.27, -32.47};// ECAL x,y minimum (1st cell left border) coordinate value in AMS ref system (cm)
static float ecal_max[2]={32.53, 32.33};// ECAL x,y maximum  (last cell right border) coordinate value in AMS ref system (cm)

static float ecal_cell_dim=0.9; ////ECAL pixel (is a cube) dimension in cm

static float ecal_cell_x[72]={	-31.82,-30.92,-30.02,-29.12,-28.22,-27.32,-26.42,-25.52,-24.62,-23.72,
						-22.82,-21.92,-21.02,-20.12,-19.22,-18.32,-17.42,-16.52,-15.62,-14.72,
						-13.82,-12.92,-12.02,-11.12,-10.22,-9.32,-8.42,-7.52,-6.62,-5.72,
						-4.82,-3.92,-3.02,-2.12,-1.22,-0.32,0.58,1.48,2.38,3.28,
						4.18,5.08,5.98,6.88,7.78,8.68,9.58,10.48,11.38,12.28,
						13.18,14.08,14.98,15.88,16.78,17.68,18.58,19.48,20.38,21.28,
						22.18,23.08,23.98,24.88,25.78,26.68,27.58,28.48,29.38,30.28,
						31.18,32.08};// ECAL cells center x coordinate in AMS ref. system (cm)

static float ecal_cell_y[72]={	-32.02,-31.12,-30.22,-29.32,-28.42,-27.52,-26.62,-25.72,-24.82,-23.92,
						-22.02,-22.12,-21.22,-20.32,-19.42,-18.52,-17.62,-16.72,-15.82,-14.92,
						-14.02,-13.12,-12.22,-11.32,-10.42,-9.52,-8.62,-7.72,-6.82,-5.92,
						-5.02,-4.12,-3.22,-2.32,-1.42,-0.52,0.38,1.28,2.18,3.08,
						3.98,4.88,5.78,6.68,7.58,8.48,9.38,10.28,11.18,12.08,
						12.98,13.88,14.78,15.68,16.58,17.48,18.38,19.28,20.18,21.08,
						21.98,22.88,23.78,24.68,25.58,26.48,27.38,28.28,29.18,30.08,
						30.98,31.88};// ECAL cells center x coordinate in AMS ref. system (cm)


//------no. of cuts
const int BETA_SEL_NO = 2;
const int TKGEOM_SEL_NO = 3;
const int TRACK_SEL_NO = 6;
const int CHARGE_SEL_NO = 7;

//------limit for unbiased Tof Layer difference
const double ISSTofLQ[30] = {0., 0., 0., 0., 0., 0., 0., 0., 0., 0.5, 0., 0.5, 0., 0.5, 0., 0.6, 0., 0., 0., 0.8, 0., 0., 0., 0., 0., 0.8, 0., 0., 0., 0.};

//------element name
//for nuclei with isotope, the default mass set as 0, which is the MC with isotope mixture //2021.01.12
const string sElement[30] = {"", "He4", "Li0", "Be0", "B0", "C12", "N0", "O16", "F19", "Ne20", "Na23", "Mg24", "Al27", "Si28", "P31", "S32", "Cl35", "Ar36", "K39", "Ca40", "", "", "", "", "", "Fe56", "", "Ni58", "", ""};

const string sElementShort[30] = {"H", "He", "Li", "Be", "B", "C", "N", "O", "F", "Ne", "Na", "Mg", "Al", "Si", "P", "S", "Cl", "Ar", "K", "Ca", "", "", "", "", "Mn", "Fe", "Co", "Ni", "Cu", "Zn"};
const string sElementFull[30] = {"proton", "Helium", "Lithium", "Beryllium", "Boron", "Carbon", "Nitrogen", "Oxygen", "Fluorine", "Neon", "Sodium", "Magnesium", "Aluminium", "Silicon", "Phosphorus", "Sulfur", "Chlorine", "Argon", "Potassium", "Calcium", "", "", "", "", "", "Iron", "Cobalt", "Nickel", "Copper", "Zinc"};
const string sElementFull2[30] = {"", "helium", "lithium", "beryllium", "boron", "carbon", "nitrogen", "oxygen", "fluorine", "neon", "sodium", "magnesium", "aluminium", "silicon", "", "sulfur", "", "argon", "", "calcium", "", "", "", "", "", "ferrum", "", "nickel"}; //qy's name for the flux rootfile

//------mass of Elements (in units of proton mass), dMass[q] instead of dMass[q-1]
const double dMass[31] = {0., 0., 0., 6.5, 8., 10.7, 12, 14.5, 16, 19., 20., 23., 24., 27., 28., 31., 32., 35., 36., 39., 40., 0., 0., 0., 0., 0., 56., 0., 58., 0., 0.}; //for Li Be B and N, use the isotope averaged mass according to TWiki and/or the LiBeB paper: //2021.01.03
//Li:	50% 6Li + 50% 7Li = 6.5
//Be:	50% 7Be + 50% 9Be = 8
//B:	30% 10B + 70% 11B = 10.7
//N:	50% 14N + 50% 15N = 14.5

//------name
//----tracker pattern
const char cSpan[NSPAN][1000] = {"Inner", "L1Inner", "FullSpan"};
const string sSpan[5] = {"Inner", "L1Inner", "FullSpan", "InnerL9", "InnerTRD"};
const string sSpan2[5] = {"Inner+Max L1Q", "L1Inner", "FullSpan", "InnerL9", "Inner+Trd Q"};
const string sSpan3[6] = {"Inner", "L1Inner", "FullSpan", "Inner+Max L1Q", "InnerL9", "Inner+Trd Q"};

//----rigidity algorithm //2023.03.10
const string sRigName[3] = {"", "_GBL", "_kalman"};
const string sRigFullName[3] = {"choutko", "GBL", "kalman"}; //2023.03.24

//----Tof
//const int nSuccTofQ[3]={2, 2, 4}; //with tof path length cut
const int nSuccTofQ[3]={1, 1, 2}; //without tof path length cut

const char cTofEff[3] = {'U', 'L', 'T'};
const string sQEff[3] = {"UTofQ", "LTofQ", "UTofQ&LTofQ"};
const string sQEff2[3] = {"UTof", "LTof", "UTof&LTof"};

//----L1 efficiency
const int nSuccL1BZ=3;
const int nSuccL1PU[2]={5,4};
const int nSuccL1PUT=5;

//----L9 efficiency
//const int nSuccL9=3;
const int nSuccL9=4; //add LTofQ cut as well //2023.07.01

//----Tracking efficiecny
const char cRigMethod[3][1000] = {"Beta", "Cutoff", "Ecal"};
const string sRigMethod[3] = {"Beta", "Cutoff", "Ecal"}; //2023.03.24

//static const int nSuccTk=9;
static const int nSuccTk=10; //use Z-2.5 for all nuclei, and add InnerQ cut for event selection as last successive cut //2023.02.20
static const int nSuccTkSam=20;
static const int nSuccTkSamPresel=5;
static const int nSuccTkSamR1=nSuccTkSam-3;
static const int nSuccTkSamVal=12;
static const int nSuccTkSamValR1=nSuccTkSamVal-4;
static const int nSuccEx=5;
static const int nSuccEx_l9=3;

//const char cSuccTk[nSuccTk][1000] = {"Tk Particle", "#beta>0.4", "nhit>=5", "hit pattern", "#chi^{2}_{Y, Inn}<10", "InnerQ cut", "Qrms<0.55", "fiducial volume1", "fiducial volume2"};
//const char cSuccEx[NSPAN][nSuccEx][1000] = {{"XY hit on L1", "#chi^{2}_{Y,L1Inn}<10", "#chi^{2}_{Y,L1}<10", "L1Q cut", "L1Q_Status"},  
//						    {"XY hit on L1", "#chi^{2}_{Y,FS}<10", "#chi^{2}_{Y,L1}<10", "L1Q cut", "L1Q_Status"}, 
//						    {"XY hit on L9", "#chi^{2}_{Y,FS}<10", "L9Q cut", "", ""}};

//--naming for MC truth properties of EffSec sample and selection //2023.06.29
const string sEffSecMCTruth[4] = {"Pri", "BelowL2", "L1-L2", "AboveL1"};

//----charge calibration
const int NQDIS=6;
const char cQDis[NQDIS][1000] = {"InnerQ", "UTofQ", "LTofQ", "InnInnerQ", "L1Q", "L9Q"};
const string sQDis[NQDIS] = {"InnerQ", "UTofQ", "LTofQ", "InnInnerQ", "L1Q", "L9Q"};
//const string sQDis2[NQDIS] = {"Inner Silicon Tracker Charge", "Upper TOF Charge", "Lower TOF Charge", "InnInnerQ", "Tracker Layer 1 Charge", "Tracker Layer 9 Charge"};
const string sQDis2[NQDIS] = {"Inner Tracker Charge", "Upper TOF Charge", "Lower TOF Charge", "InnInnerQ", "Tracker Layer 1 Charge", "Tracker Layer 9 Charge"}; //2023.02.14

//----beta
const string sBeta[3] = {"TOF", "NaF", "AgL"};
const string sBetaFull[3] = {"TOF", "Rich-NaF", "Rich-AgL"};

//------MC version
std::string cMCVer[26] = {"", "", "", "", "", "", "", "", "", "B1116", "", "B1116", "B1116", "B1117", "", "B1116", "", "B1116", "", "B1116", "", "", "", "", "", "B1116"};

//rich cut from JiaHui //2019.06.02
const float kCutRichProb                  = 0.01;          //  Kolmogorov test probability
const float kCutRichPmt                   = 3;             //  number of pmts
const float kCutRichCollOverTotal         = 0.4;           //  ring photoelctrons / total photoelectrons in the event
const float kCutRichChargeConsistency     = 15;            //  hit/PMT charge consistency test
const float kCutRichBetaConsistency[2]    = {0.01,0.005};  //  beta_lip vs beta_ciemat consistency ([0]=NaF, [1]=aerogel)
const float kCutRichExpPhe[2]             = {1,2};         //  expected number of photoelectrons   ([0]=NaF, [1]=aerogel)
const float kCutRichAerogelExternalBorder = 3500.;         //  aerogel external border (r**2)
const float kCutRichAerogelNafBorder[2]   = {17.,19.};     //  aerogel/NaF border                  ([0]=NaF, [1]=aerogel)
const int kNBadTiles = 5;                            
int kBadTile[kNBadTiles] = {
    3,
    7,
    87,
    100,
    108
};   //  tiles with bad beta recosntruction

//------L8L9 material missing //2020.04.03
const double dL8L9MissingMat=0.035;

//------Beta cutoff safety factor for Tof, Rich-NaF and Rich-AgL, same for the value in JH's thesis, will optimized after //2021.09.06
const double dBetaCutoffSF[3] = {.06, .005, .0005};

//------Trd Q //2021.09.28
const string sTrdqPart[3] = {"T", "U10", "L10"};
#endif //PARAMETER_H
