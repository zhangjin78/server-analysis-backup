//  $Id: tkgeom.C,v 1.38 2025/04/11 16:09:59 nbelyaev Exp $
#include <strstream>
#include <vector>

#ifdef _PGTRACK_
#include "gvolume.h"  // for AMSgvolume
#include "tkdcards.h"
#include "TkDBc.h"
#include "TkLadder.h"
#include "TkPlane.h"
#include "TkCoo.h"
#include "TkFeet.h"

namespace amsgeom{
extern void tkgeom02(AMSgvolume &);
extern void tkgeomL0(AMSgvolume &);
extern void magnetgeom(AMSgvolume &);
extern void magnetgeom02(AMSgvolume &);
extern void ext1structure02(AMSgvolume &);
extern void magnetgeom02o(AMSgvolume &);
extern void magnetgeom02Test(AMSgvolume &);
extern void tofgeom02(AMSgvolume &);
extern void antigeom02(AMSgvolume &);
extern void antigeom002(AMSgvolume &);
extern void ext2structure(AMSgvolume &);
#ifdef __G4AMS__
extern void antigeom02g4(AMSgvolume &);
extern void testg4geom(AMSgvolume &);
#endif
extern void richgeom02(AMSgvolume &,float zshift=0);
extern void ecalgeom02(AMSgvolume &);
extern void trdgeom02(AMSgvolume &, float zshift=0);
//extern void srdgeom02(AMSgvolume &);
extern void Put_rad(AMSgvolume *,integer);
extern void Put_pmt(AMSgvolume *,integer);
};
using namespace amsgeom;

const bool debugL0 = false;

// Ladder support thickness in cm
static double sup_foam_w   = 0.5;
// Ladder support z-offset in cm
static double sup_foam_tol = 0.05;
// L0 ladder support thickness in cm
static double l0_sup_w   = 0.017;

int _nrot = 0;


void        BuildHoneycomb    (AMSgvolume *mvol, int plane);
AMSgvolume *BuildPlaneEnvelop (AMSgvolume *mvol, int plane);
AMSgvolume *BuildLadder       (AMSgvolume *mvol, int tkid);
void        BuildSensor       (AMSgvolume *mvol, int tkid, int s);
void        BuildHybrid       (AMSgvolume *mvol, int tkid);
void        BuildSupport      (AMSgvolume *mvol, int tkid);
void       BuildLadderFoot    (AMSgvolume *mvol, int tkid, int xfoot,int yfoot);
void      BuildPlane1NSupport (AMSgvolume *mvol);
void       BuildPlane8QLCS    (AMSgvolume *mvol, bool isTop);
void     BuildPlane8QLCSLayer (AMSgvolume* mvol, char* patt, char* mat, geant zPos, geant w);
void      BuildPlane8Cupolas  (AMSgvolume *mvol);
void     BuildPlane8Radiators (AMSgvolume *mvol);
AMSgvolume *BuildQLCSEnvelop  (AMSgvolume *mvol, bool isTop);


void amsgeom::tkgeomL0(AMSgvolume &mother){
// Build tracker L0 geometry
#ifndef __G4AMS__
  cerr<<"amsgeom::tkgeomL0-FATAL- The L0 setup AMS02P.2 cannot be used with Geant3\n";
  exit(-5);
#endif
  int nladder=0;

  AMSgvolume *vplane=BuildPlaneEnvelop(&mother, 8); // L0 is in STK8

  BuildHoneycomb(vplane, 8);

  // Top envelop is rotated by 45 degrees
  AMSgvolume *topQLCSEnvelop = BuildQLCSEnvelop(vplane, 1);
  BuildPlane8QLCS(topQLCSEnvelop, 1);

  AMSgvolume *botQLCSEnvelop = BuildQLCSEnvelop(vplane, 0);
  BuildPlane8QLCS(botQLCSEnvelop, 0);

  int minp_lay=0;
  int maxp_lay=2;
  for (int p_lay = minp_lay; p_lay<maxp_lay; p_lay++) {
    int layer=10+p_lay; // Layers 10 and 11

    // Loop for each slot
    for (int slot=1;slot<=trconst::maxlad;slot++) {
      for (int side=-1;side<=1;side+=2) {
        // Check TkId is valid or not
	      int tkid = side*(50*(p_lay)+slot);
        TkLadder *lad = TkDBc::Head->FindTkId(tkid);
	      if (!lad) {
          std::cout << "<---- TKGeom-E- The L0 ladder with tkid=" << tkid << " not found!\n";
          continue;
        }

        // Build ladder
        BuildSupport(vplane, tkid);
        BuildLadder(vplane, tkid);
        
        nladder++;
      }
    }
  }

  std::cout << "<---- TKGeom-I-" << nladder
  << " Active L0 halfladders initialized" 
  << std::endl << std::endl;

  BuildPlane8Radiators(vplane);

  BuildPlane8Cupolas(vplane);
}


void amsgeom::tkgeom02(AMSgvolume &mother)
{
// Build tracker geometry

  // Temporary arbitary assignment
  _nrot = 501;

  int nladder = 0;
  if(TkDBc::Head->GetSetup()==3||TkDBc::Head->GetSetup()==4){ // PLANE 5N aka 7
    AMSgvolume *vplane = BuildPlaneEnvelop(&mother, 7);
    // Build plane support
    BuildHoneycomb(vplane, 7);
    BuildPlane1NSupport(&mother);

  }
  // Loop on planes
  for (int plane=1; plane<=TkDBc::Head->GetNPlanes();plane++){

    // Build a plane geometry (made of VACUUM)
    AMSgvolume *vplane = BuildPlaneEnvelop(&mother, plane);
    // Build plane support
    BuildHoneycomb(vplane, plane);
    
    // Loop on Si layer attached to the plane
    int maxp_lay=1;
    int minp_lay=0;
    if(plane==1)      {minp_lay= 1;maxp_lay=2;}
    else if(plane==5) {minp_lay= 0;maxp_lay=1;}
    else              {minp_lay= 0;maxp_lay=2;}

    if((TkDBc::Head->GetSetup()==3||TkDBc::Head->GetSetup()==4)&&plane==6) 
      {minp_lay=-1;maxp_lay=0;}


    for (int p_lay = minp_lay; p_lay<maxp_lay; p_lay++) {
      int layer=2*(plane-1)+p_lay;
         
      // Loop for each slot
      for (int slot = 1; slot <= trconst::maxlad; slot++) {
	
	      // Loop for each side
	      for (int side = -1; side <= 1; side += 2) {

	        // Check TkId is valid or not
	        int tkid = side*(layer*100+slot);
	        TkLadder *lad = TkDBc::Head->FindTkId(tkid);
	        if (!lad) continue;
	  
	        // Build ladder
	        BuildLadder(vplane, tkid);
	        nladder++;
	  
	        // Build electronics
	        BuildHybrid(vplane, tkid);
	  
	        // Build ladder support
	        BuildSupport(vplane, tkid);
	        if(TRMCFFKEY.EnableTrackerFeet){
	          // Build ladder feet support
	          TkFeet ft;
	          int nf=ft.getNfeet(tkid);
	          for (int ifn=0;ifn<nf;ifn++){
	            BuildLadderFoot(vplane, tkid,ifn,0);
	            BuildLadderFoot(vplane, tkid,ifn,1);
	          }
	        }
	      } //side
      } //slot
    }//lay
  }//plane
  std::cout << "<---- TKGeom-I-" << nladder
            << " Active halfladders initialized" 
            << std::endl << std::endl;
}
//============================================================================
AMSgvolume *BuildPlaneEnvelop(AMSgvolume *mvol, int plane)
{
  // Build Plane container geometry (plane number begins from 1)
  char name[5];
  geant coo[3];
  geant par[3];
  AMSgvolume* volout;
  number nrm[3][3]={{1,0,0},{0,1,0},{0,0,1}};
  std::ostrstream ost(name,sizeof(name));
  ost << "STK" << plane << std::ends;

  TkPlane *pl = TkDBc::Head->GetPlane(plane);
  if(!pl && plane!=7 && plane!=8){
    printf(" AMSgvolume *BuildPlaneEnvelop: cannot find plane #%d\n I Give Up!!\n",plane);
      exit(2);
  }
  if(plane==8) {// L0 plane
    TkPlane *pl = TkDBc::Head->GetPlane(7);

    coo[0] = TkDBc::Head->_xpos[6];
    coo[1] = TkDBc::Head->_ypos[6];
    coo[2] = TkDBc::Head->_zpos[6];

    par[0] = 0;
    par[1] = TkDBc::Head->_plane_d1[6]; //container radius
    par[2] = TkDBc::Head->_plane_d2[6]; //container half thickness

    volout=(AMSgvolume*)mvol
      ->add(new AMSgvolume("VACUUM", 1, name, "TUBE",
                          par, 3, coo, nrm, "ONLY", 0, plane, 1));
  }
  else if(plane==7) {//PLAN -B DUMMY plane 5N
    TkPlane *pl = TkDBc::Head->GetPlane(1);
    coo[0] = pl->GetPos()[0]+pl->GetPosT()[0];
    coo[1] = pl->GetPos()[1]+pl->GetPosT()[1];
    coo[2] = -1*(pl->GetPos()[2]+pl->GetPosT()[2]);
    //		 pl->GetRotMat().GetEl(2,2)*TkDBc::Head->_dz[0]);

    par[0] = 0;
    par[1] = TkDBc::Head->_plane_d1[4]; //container radius
    par[2] = TkDBc::Head->_plane_d2[4]; //container half thickness
  
    
    AMSRotMat lrm0 = pl->GetRotMatT();
    AMSRotMat lrm = lrm0*pl->GetRotMat();
  
    for (int ii = 0; ii < 9; ii++) nrm[ii/3][ii%3] = lrm.GetEl(ii/3,ii%3);
    volout=(AMSgvolume*)mvol
      ->add(new AMSgvolume("VACUUM", _nrot++, name, "TUBE",
                          par, 3, coo, nrm, "ONLY", 0, plane, 1));    
    }
  else if(plane==6){ //PLAN -B ECAL PLANE 
      // Center is offset because of the ladder asymmetry
      coo[0] = pl->GetPos()[0]+pl->GetPosT()[0];
      coo[1] = pl->GetPos()[1]+pl->GetPosT()[1];
      coo[2] = pl->GetPos()[2]+pl->GetPosT()[2]+TkDBc::Head->_dz[plane-1];
      par[0]=TkDBc::Head->Plane6EnvelopSize[0]/2.;
      //       plan6E Y size/2.
      par[1]=TkDBc::Head->Plane6EnvelopSize[1]/2.;
      //       plan6E thickness/2.
      par[2]=TkDBc::Head->Plane6EnvelopSize[2]/2.;
      cout <<" Placing the Tk  Plan 6 Vol at "<< coo[0]<<"  "<< coo[1]<<"  "<< coo[2]<<
	"with half dim "<<par[0]<<"  "<<par[1]<<"  "<<par[2]<<endl;
    
      AMSRotMat lrm0 = pl->GetRotMatT();
      AMSRotMat lrm = lrm0*pl->GetRotMat();
    
      for (int ii = 0; ii < 9; ii++) nrm[ii/3][ii%3] = lrm.GetEl(ii/3,ii%3);
    volout= (AMSgvolume*)mvol
        ->add(new AMSgvolume("VACUUM", _nrot++, name, "BOX",
                             par, 3, coo, nrm, "ONLY", 0, plane, 1));
  }else if(plane==1){ //PZ let's add LBBX (laser boxes)
  
    coo[0] = pl->GetPos()[0]+pl->GetPosT()[0];
    coo[1] = pl->GetPos()[1]+pl->GetPosT()[1];
    coo[2] = pl->GetPos()[2]+pl->GetPosT()[2]+
      pl->GetRotMat().GetEl(2,2)*TkDBc::Head->_dz[plane-1];

    par[0] = 0;
    par[1] = TkDBc::Head->_plane_d1[plane-1]; //container radius
    par[2] = TkDBc::Head->_plane_d2[plane-1]; //container half thickness

  
    AMSRotMat lrm0 = pl->GetRotMatT();
    AMSRotMat lrm = lrm0*pl->GetRotMat();
  
    for (int ii = 0; ii < 9; ii++) nrm[ii/3][ii%3] = lrm.GetEl(ii/3,ii%3);
    volout=(AMSgvolume*)mvol
      ->add(new AMSgvolume("VACUUM", _nrot++, name, "TUBE",
                           par, 3, coo, nrm, "ONLY", 0, plane, 1));    
    
  
  }else{ //STD AMS PLANES
  
    coo[0] = pl->GetPos()[0]+pl->GetPosT()[0];
    coo[1] = pl->GetPos()[1]+pl->GetPosT()[1];
    coo[2] = pl->GetPos()[2]+pl->GetPosT()[2]+
      pl->GetRotMat().GetEl(2,2)*TkDBc::Head->_dz[plane-1];
  
    par[0] = 0;
    par[1] = TkDBc::Head->_plane_d1[plane-1]; //container radius
    par[2] = TkDBc::Head->_plane_d2[plane-1]; //container half thickness
  
    AMSRotMat lrm0 = pl->GetRotMatT();
    AMSRotMat lrm = lrm0*pl->GetRotMat();
  
    for (int ii = 0; ii < 9; ii++) nrm[ii/3][ii%3] = lrm.GetEl(ii/3,ii%3);
    volout=(AMSgvolume*)mvol
      ->add(new AMSgvolume("VACUUM", _nrot++, name, "TUBE",
                           par, 3, coo, nrm, "ONLY", 0, plane, 1));    
    
  }
  if(plane>0){
    printf("\nPlane Envelop %d\n",plane);
    printf(" Coo %f %f %f\n ",coo[0],coo[1],coo[2]);    
    printf(" Half dim %f %f %f\n ",par[0],par[1],par[2]);
    printf(" Matr  %f %f %f \n",nrm[0][0],nrm[0][1],nrm[0][2]);
    printf("        %f %f %f \n",nrm[1][0],nrm[1][1],nrm[1][2]);
    printf("        %f %f %f \n",nrm[2][0],nrm[2][1],nrm[2][2]);
  }
  return volout; 
}
//============================================================================

AMSgvolume *BuildQLCSEnvelop(AMSgvolume *mvol, bool isTop){
  // Creating QLCS envelop with respect to STK8
  geant angle=-atan(1.); // -45 deg
  char name[5];
  geant coo[3];
  geant par[3];
  number nrm[3][3];
  number nrmTop[3][3]={{cos(angle),-sin(angle),0},{sin(angle),cos(angle),0},{0,0,1}};
  number nrmBot[3][3]={{-1,0,0},{0,1,0},{0,0,-1}}; // X and Y also needs to be flipped to keep det=1

  par[0]=0;
  par[1]=TkDBc::Head->_sup_hc_r[6];
  par[2]=TkDBc::Head->_sup_qlcs_skin_w+0.5*TkDBc::Head->_sup_qlcs_w;

  coo[0]=0;
  coo[1]=0;
  coo[2]=0.5*TkDBc::Head->_sup_hc_w[6]+                // Half of the honeycomb
             TkDBc::Head->_sup_hc_skin_w[6]+           // Honeycomb skin
             TkDBc::Head->_sup_hc_skin_qlcs_skin_gap+  // HC skin-QLCS skin gap
             TkDBc::Head->_sup_qlcs_skin_w+            // QLCS skin
         0.5*TkDBc::Head->_sup_qlcs_w;                 // Half of QLCS thickness

  if (isTop) {
    sprintf(name,"QLET");
    for (int i=0;i<3;i++)
      for (int j=0;j<3;j++)
        nrm[i][j]=nrmTop[i][j];
    if (debugL0) printf("--TKGeom-L0Debug-I- Upper QLCS envelop z: %8.4f, w: %8.4f\n",coo[2], par[2]);
  }
  else {
    sprintf(name,"QLEB");
    for (int i=0;i<3;i++)
      for (int j=0;j<3;j++)
        nrm[i][j]=nrmBot[i][j];
    coo[2]*=-1;
    if (debugL0) printf("--TKGeom-L0Debug-I- Lower QLCS envelop z: %8.4f\n",coo[2]);
  }
  AMSgvolume *envelop=(AMSgvolume*)mvol
      ->add(new AMSgvolume("VACUUM", 1, "QLET", "TUBE",
                          par, 3, coo, nrm, "ONLY", 0, 8, 1));

  return envelop;
}

AMSgvolume *BuildLadder(AMSgvolume *mvol, int tkid)
{
  // Build ladder geometry

  TkLadder *lad = TkDBc::Head->FindTkId(tkid);
  if (!lad) return 0;

  AMSgvolume* ladd;

  if (abs(tkid)>100) { // AMS-02 ladders

    int layer = std::abs(tkid)/100;
    int islot = (tkid > 0) ? tkid%100 : -tkid%100+20;
    int iname = layer*100+islot;

    char name[5];
    std::ostrstream ost(name,sizeof(name));
    ost << "L" << iname << std::ends;

    geant par[3];
    par[0] = TkCoo::GetLadderLength(tkid)/2;
    par[1] = TkDBc::Head->_ssize_inactive[1]/2;
    par[2] = TkDBc::Head->_silicon_z/2;

    double hlen = TkCoo::GetLadderLength(tkid)/2
      -(TkDBc::Head->_ssize_inactive[0]-TkDBc::Head->_ssize_active[0])/2;
    double hwid = TkDBc::Head->_ssize_active[1]/2;

    AMSRotMat rot0 = lad->GetRotMatT();
    AMSRotMat rot = rot0*lad->GetRotMat();

    AMSPoint  pos = lad->GetPos()+lad->GetPosT();
    AMSPoint  loc(hlen, hwid, 0);
    AMSPoint  oo = rot*loc+pos;

    // Get coordinate w.r.t. the mother layer
    geant coo[3];
    coo[0] = oo.x();
    coo[1] = oo.y();
    coo[2] = oo.z();
    if(layer==1) coo[2] -= TkDBc::Head->_dz[0];
    if(layer==8) coo[2] -= TkDBc::Head->_dz[4];
    if(layer==9) coo[2] -= TkDBc::Head->_dz[5];

    AMSRotMat lrm0 = lad->GetRotMatT();
    AMSRotMat lrm = lrm0*lad->GetRotMat();
  
    number nrm[3][3];
    for (int ii = 0; ii < 9; ii++) nrm[ii/3][ii%3] = lrm.GetEl(ii/3,ii%3);

    int gid =abs(tkid)/tkid*(abs(tkid)+1000);
    // printf("Sensor name %s  %+03d   %+9d %f %f %f\n",name,tkid,gid,coo[0],coo[1],coo[2]);
    ladd=(AMSgvolume*)mvol
      ->add(new AMSgvolume("NONACTIVE_SILICON", _nrot++, name, "BOX",
	  		 par, 3, coo, nrm, "ONLY", 1, gid, 1));

    //  printf("Ladder name %s\n",name);
  
    // Build sensors
    for (int sensor = 0; sensor < lad->GetNSensors(); sensor++){
      char nameS[5];
      std::ostrstream ost2(nameS,sizeof(nameS));
      ost2 << "S" << iname << std::ends;

      geant par[3];
      par[0] = TkDBc::Head->_ssize_active[0]/2;
      par[1] = TkDBc::Head->_ssize_active[1]/2;
      par[2] = TkDBc::Head->_silicon_z/2;
    
      // Get coordinate w.r.t. the mother ladder
      geant coo[3];
      coo[0]=-TkCoo::GetLadderLength(tkid)/2 + TkDBc::Head->_ssize_inactive[0]/2. + sensor*TkDBc::Head->_SensorPitchK;
      coo[1] = coo[2] = 0;

      number nrm[3][3];
      VZERO(nrm, 9*sizeof(nrm[0][0])/4);
      nrm[0][0] = nrm[1][1] = nrm[2][2] = 1;
      int lside = (tkid>0)? 1:0;
      int gid = abs(tkid)+1000*(lside)+10000*(sensor+1); 
      //    printf("Sensor name %s  %+03d   %+9d %f %f %f\n",nameS,tkid,gid,coo[0],coo[1],coo[2]);
     ladd->add(new AMSgvolume("ACTIVE_SILICON", _nrot++, nameS, "BOX",
	  		     par, 3, coo, nrm, "ONLY", 1, gid, 1));

      return ladd;
    }
  } else { // L0 ladders
    int layer=11;
    if (abs(tkid)<50) layer=10;

    int gid=abs(tkid)/tkid*(abs(tkid)+1000);

    // Build sensors
    for (int sensor = 0; sensor < lad->GetNSensors(); sensor++){
      char name[5];
      int sensornumber;
      if (abs(tkid)<50) {
        if (tkid>0)
          sensornumber = 12*tkid+sensor;
        else
          sensornumber = 12*(-tkid+18)+sensor;
      } else {
        if (tkid>0)
          sensornumber = 12*(tkid-50+36)+sensor;
        else
          sensornumber = 12*((-tkid-50+36)+18)+sensor;
      }
      std::snprintf(name, sizeof(name), "N%03d", sensornumber);

      // Adding inactive silicon
      geant par[3];
      par[0] = 0.5*TkDBc::Head->_ssize_inactive_l0[0];
      par[1] = 0.5*TkDBc::Head->_ssize_inactive_l0[1];
      par[2] = 0.5*TkDBc::Head->_silicon_z; // <<##>>

      //double hlen = TkCoo::GetLadderLength(tkid)/2
      //  -(TkDBc::Head->_ssize_inactive_l0[0]-TkDBc::Head->_ssize_active_l0[0])/2;
      //double hwid = TkDBc::Head->_ssize_active_l0[1]/2;
  
      AMSRotMat rot0 = lad->GetRotMatT();
      AMSRotMat rot = rot0*lad->GetRotMat();

      AMSPoint pos = lad->GetPos()+lad->GetPosT();
      //AMSPoint  loc(hlen, hwid, 0);
      double sign=1.;
      if (layer==10&&tkid>0)
        sign=-1.;
      AMSPoint loc(sign*(-0.5*TkCoo::GetLadderLength(tkid) +
                          0.5*TkDBc::Head->_ssize_inactive_l0[0] + 
                          sensor*(TkDBc::Head->_L0SensorGapK+TkDBc::Head->_ssize_inactive_l0[0])),0, 0); // Fix z <<##>>
      AMSPoint oo = rot*loc+pos;

      geant coo[3];
      coo[0] = oo.x();
      coo[1] = oo.y();
      coo[2] = oo.z()+0.5*TkDBc::Head->_silicon_z*((layer==11)?1:-1);
  
      number nrm[3][3];
      for (int ii = 0; ii < 9; ii++) nrm[ii/3][ii%3] = rot.GetEl(ii/3,ii%3);

      int gid = abs(tkid)/tkid*(2000+sensornumber);

      ladd=(AMSgvolume*)mvol->add(
        new AMSgvolume("NONACTIVE_SILICON", _nrot++, name, "BOX",
		    par, 3, coo, nrm, "ONLY", 1, gid, 1));

      if (debugL0) printf("--TKGeom-L0Debug-I- Inactive silicon name %s tkid %+3d gid %+7d x %8.4f y %8.4f z %8.4f p0 %8.4f p1 %8.4f p2 %8.4f\n",
                          name,tkid,gid,coo[0],coo[1],coo[2],par[0],par[1],par[2]);

      // Adding active silicon
      // Should inactive and active silicons be added to the same volume? <<##>>
      char nameS[5];
      std::snprintf(nameS, sizeof(nameS), "A%03d", sensornumber);

      par[0] = 0.5*TkDBc::Head->_ssize_active_l0[0];
      par[1] = 0.5*TkDBc::Head->_ssize_active_l0[1];

      int lside = (tkid>0)? 1:0;
      gid = abs(tkid)/tkid*(20000+sensornumber);
      //printf("Sensor name %s  %+03d   %+9d %f %f %f\n",nameS,tkid,gid,coo[0],coo[1],coo[2]);
      ladd=(AMSgvolume*)mvol->add(new AMSgvolume("ACTIVE_SILICON", _nrot++, nameS, "BOX",
			        par, 3, coo, nrm, "ONLY", 1, gid, 1));

      if (debugL0) printf("--TKGeom-L0Debug-I- Active   silicon name %s tkid %+3d gid %+7d x %8.4f y %8.4f z %8.4f p0 %8.4f p1 %8.4f p2 %8.4f\n\n",
                          nameS,tkid,gid,coo[0],coo[1],coo[2],par[0],par[1],par[2]);
    }
    if (debugL0) std::cout << std::string(100, '=') << '\n';
    return ladd;
  }
  return 0; // Should not happen, maybe change the design <<##>>
}
//============================================================================

void BuildHybrid(AMSgvolume *mvol, int tkid)
{
// Build hybrid geometry

  TkLadder *lad = TkDBc::Head->FindTkId(tkid);
  if (!lad) return;

  int layer = std::abs(tkid)/100;
  int plane= TkDBc::Head->_plane_layer[layer-1];
  int sign  = (tkid > 0) ? 1 : -1;

  char name[5];
  std::ostrstream ost(name,sizeof(name));
  ost << ((tkid < 0) ? "ELL" : "ELR") << layer << std::ends;

  geant par[3];
  if(layer==1 ||layer==8||layer==9){
    par[0] = TkDBc::Head->_zelec[2]/2.;
    par[1] = TkDBc::Head->_zelec[1]/2.;
    par[2] = TkDBc::Head->_zelec[0]/2.;
  }else{
    par[0] = TkDBc::Head->_zelec[0]/2.;
    par[1] = TkDBc::Head->_zelec[1]/2.;
    par[2] = TkDBc::Head->_zelec[2]/2.;
  }
  double hlen = TkCoo::GetLadderLength(tkid)/2
    -(TkDBc::Head->_ssize_inactive[0]-TkDBc::Head->_ssize_active[0])/2;
  double hwid = TkDBc::Head->_ssize_active[1]/2;

  AMSRotMat rot0 = lad->GetRotMatT();
  AMSRotMat rot = rot0*lad->GetRotMat();

  AMSPoint  pos = lad->GetPos()+lad->GetPosT();
  AMSPoint  loc(hlen, hwid, 0);
  AMSPoint  oo = rot*loc+pos;

  // Get coordinate w.r.t. the mother layer
  geant coo[3];
  coo[0] = oo.x() +sign*(TkCoo::GetLadderLength(tkid)/2+par[0]+0.003);
  coo[1] = oo.y();
  coo[2] = (std::abs(oo.z())/oo.z())*(TkDBc::Head->_sup_hc_w[plane-1]/2.+par[2]+0.1); 
  if(layer==1) coo[2] -= TkDBc::Head->_dz[0];
  if(layer==8) coo[2] -= TkDBc::Head->_dz[4];
  if(layer==9) coo[2] -= TkDBc::Head->_dz[5];
  
  number nrm[3][3];
  VZERO(nrm,9*sizeof(nrm[0][0])/4);
  nrm[0][0] = nrm[1][1] = nrm[2][2] = 1;

  int gid = tkid+1000;
  mvol->add(new AMSgvolume("ELECTRONICS", _nrot++, name,
			   "BOX", par, 3, coo, nrm, "ONLY", 1, gid, 1));
}
//============================================================================

void BuildSupport(AMSgvolume *mvol, int tkid)
{
// Build ladder support geometry

  TkLadder *lad = TkDBc::Head->FindTkId(tkid);

  if (!lad) return;

  if (abs(tkid)>100){

    int layer = std::abs(tkid)/100;
  
    char name[5];
    std::ostrstream ost(name,sizeof(name));
    ost << "FOA" << layer << std::ends;
  
    geant par[3];
    par[0] = TkCoo::GetLadderLength(tkid)/2;
    par[1] = TkDBc::Head->_ssize_inactive[1]/2;
    par[2] = sup_foam_w/2;

    double hlen = TkCoo::GetLadderLength(tkid)/2
      -(TkDBc::Head->_ssize_inactive[0]-TkDBc::Head->_ssize_active[0])/2;
    double hwid = TkDBc::Head->_ssize_active[1]/2;
  
    AMSRotMat rot0 = lad->GetRotMatT();
    AMSRotMat rot = rot0*lad->GetRotMat();

    AMSPoint  pos = lad->GetPos()+lad->GetPosT();
    AMSPoint  loc(hlen, hwid, 0);
    AMSPoint  oo = rot*loc+pos;

    // Get coordinate w.r.t. the mother layer
    geant coo[3];
    coo[0] = oo.x();
    coo[1] = oo.y();
    coo[2] = (std::abs(oo.z())/oo.z())*(std::abs(oo.z())-TkDBc::Head->_silicon_z/2-par[2]-sup_foam_tol);
    if(layer==1) coo[2] -= TkDBc::Head->_dz[0];
    if(layer==8) coo[2] -= TkDBc::Head->_dz[4];
    if(layer==9) coo[2] -= TkDBc::Head->_dz[5];
	     
    number nrm[3][3];
    VZERO(nrm,9*sizeof(nrm[0][0])/4);
    nrm[0][0] = nrm[1][1] = nrm[2][2] = 1;

    int gid = tkid+1000;
    mvol->add(new AMSgvolume("Tr_Foam", _nrot++, name, "BOX",
	  		   par, 3, coo, nrm, "ONLY", 1, gid, 1)); // Fix material <<##>>
  
    if(TRMCFFKEY.ActivateShielding&&abs(tkid)>100){

      //shielding

      par[0] = TkCoo::GetLadderLength(tkid)/2;
      par[1] = TkDBc::Head->_ssize_inactive[1]/2;
      par[2] = 0.01/2.;  // 100 um

      coo[0] = oo.x();
      coo[1] = oo.y();
      coo[2] = (std::abs(oo.z())/oo.z())*(std::abs(oo.z())+TkDBc::Head->_silicon_z/2+par[2]+0.1);
      if(layer==1) coo[2] -= TkDBc::Head->_dz[0];
      if(layer==8) coo[2] -= TkDBc::Head->_dz[4];
      if(layer==9) coo[2] -= TkDBc::Head->_dz[5];

      VZERO(nrm,9*sizeof(nrm[0][0])/4);
      nrm[0][0] = nrm[1][1] = nrm[2][2] = 1;
      char name2[5];
      std::ostrstream ost2(name2,sizeof(name2));
      ost2 << "SHD" << layer << std::ends;
      gid = tkid+1000;
      mvol->add(new AMSgvolume("TrShield-M", _nrot++, name2, "BOX",
	    par, 3, coo, nrm, "ONLY", 1, gid, 1));
    }
  }else{
    int layer=11;
    if (abs(tkid)<50)
      layer=10;

    char name[5];
    std::ostrstream ost(name,sizeof(name));
    ost << "LS" << std::setw(2) << std::setfill('0') << ((tkid>0)?tkid:-tkid+20) << std::ends;

    geant par[3];
    par[0] = TkCoo::GetLadderLength(tkid)/2.;
    par[1] = TkDBc::Head->_ssize_inactive_l0[1]/2.;
    par[2] = l0_sup_w/2.;

    // Careful adjustment of dimensions of QLCS-ladders pairs is required <<##>>
    geant coo[3];
    coo[0] = lad->GetPos().x();
    coo[1] = lad->GetPos().y();
    coo[2] = lad->GetPos().z()+0.5*l0_sup_w*((layer==11)?-1:1);

    AMSRotMat lrm0 = lad->GetRotMatT();
    AMSRotMat lrm = lrm0*lad->GetRotMat();
  
    number nrm[3][3];
    for (int ii = 0; ii < 9; ii++) nrm[ii/3][ii%3] = lrm.GetEl(ii/3,ii%3);

    int gid = tkid+1000;
    mvol->add(new AMSgvolume("Tr_Foam", _nrot++, name, "BOX",
	  		   par, 3, coo, nrm, "ONLY", 1, gid, 1)); // Fix material <<##>>

    if (debugL0) printf("--TKGeom-L0Debug-I- Ladder Support   name %s tkid %+3d gid %+7d x %8.4f y %8.4f z %8.4f p0 %8.4f p1 %8.4f p2 %8.4f\n\n",
                        name,tkid,gid,coo[0],coo[1],coo[2],par[0],par[1],par[2]);
  }
}

//============================================================================
void  BuildLadderFoot(AMSgvolume *mvol, int tkid, int xfoot, int yfoot){
// Build and place ladder feet
  TkLadder *lad = TkDBc::Head->FindTkId(tkid);
  int footid=yfoot*10+xfoot;
  if (!lad) return;

  int layer = std::abs(tkid)/100;
  
  char name[15];
  sprintf(name,"F%+03d_%02d",tkid,footid);
  
  int type=0;
  if(layer==1||layer==8) type=2;
  else if (layer==2||layer==4||layer==7||layer==9) type=0;
  else type=1;
  // Creeate and place an envelop for the foot
  geant env_rad[3]={0.5,0.65,0.5};
  geant env_hei[3]={0.5,1.1,0.4};

  geant par[3];
  par[0] = 0; //rmin
  par[1] = env_rad[type];  //rmax
  par[2] = env_hei[type]/2.; //dz

  number nrm[3][3];
  VZERO(nrm,9*sizeof(nrm[0][0])/4);
  nrm[0][0] = nrm[1][1] = nrm[2][2] = 1;
  TkFeet ff;

  double XL = ff.getXloc(tkid,xfoot);
  double YL = ff.getYloc(tkid,yfoot);
  
  AMSRotMat rot0 = lad->GetRotMatT();
  AMSRotMat rot = rot0*lad->GetRotMat();

  AMSPoint  pos = lad->GetPos()+lad->GetPosT();
  AMSPoint  loc(XL, YL, 0);
  AMSPoint  oo = rot*loc+pos;

  geant coo[3];
  coo[0] = oo.x();
  coo[1] = oo.y();
  coo[2] = (std::abs(oo.z())/oo.z())*(
				      std::abs(oo.z())
				      -TkDBc::Head->_silicon_z/2
				      -sup_foam_w
				      -sup_foam_tol
				      -par[2]);
  if(layer==1) coo[2] -= TkDBc::Head->_dz[0];
  if(layer==8) coo[2] -= TkDBc::Head->_dz[4];
  if(layer==9) coo[2] -= TkDBc::Head->_dz[5];
  
  int gid = abs(tkid)/tkid*(abs(tkid)+1000*footid);
  AMSgvolume* gvol=new AMSgvolume("VACUUM", _nrot++, name, "TUBE",
				  par, 3, coo, nrm, "ONLY", 1, gid, 1);


  //printf("Generating Ladder Foot %02d for ladder %+03d z= %9.4f Xloc %9.4f Yloc %9.4f at %9.4f %9.4f %9.4f\n",
  //footid,tkid,oo.z(),XL,YL,coo[0],coo[1],coo[2]);

  // fill the envelop with the actual foot
  if (type==0){  // standard foot with pass-though screw layersO= 2,4,7,9
    // must account for a Screw Al=39 mg and the Bolt/foot Al=73 mg.  Honeycomb correction negligible
    geant fpar[3];
    fpar[0] = 0; //rmin
    fpar[1] = 0.1642;  //rmax
    fpar[2] = 0.49/2.; //dz
    
    geant fcoo[3];
    fcoo[0] =0;
    fcoo[1] =0;
    fcoo[2] =0;
    char fname[15];
    sprintf(fname,"T%+03d%02dF1",tkid,footid);
    int fgid= abs(tkid)/tkid*(abs(tkid)+1000*footid)+100000;
    AMSgvolume* fvol=new AMSgvolume("TrFeet-Alloy-M", _nrot++, fname, "TUBE",
				    fpar, 3, fcoo, nrm, "ONLY", 1, fgid, 1);
    gvol->add(fvol);
    
  }else if(type==1){ // foot for inner layers with PEEK bush and fixing pin, layersO= 3,5,6
    geant fpar[3];
    fpar[0] = 0; //rmin
    fpar[1] = 0.1642;  //rmax
    fpar[2] = 0.815/2.; //dz
    
    geant fcoo[3];
    fcoo[0] =0;
    fcoo[1] =0;
    fcoo[2] =0;
    char fname[15];  //Accounts for the pin Al 100 mg the foot Al 73 mg and 1/3 of the side screw Al 13 mg. Honeycomb correction negligible
    sprintf(fname,"T%+03d%02dF1",tkid,footid);
    int fgid= abs(tkid)/tkid*(abs(tkid)+1000*footid)+100000;
    AMSgvolume* fvol=new AMSgvolume("TrFeet-Alloy-M", _nrot++, fname, "TUBE",
				    fpar, 3, fcoo, nrm, "ONLY", 1, fgid, 1);
    gvol->add(fvol);

    geant fpar1[3];
    fpar1[0] = 0.1642 ; //rmin
    fpar1[1] = 0.395;  //rmax
    fpar1[2] = 0.815/2.; //dz
    
    geant fcoo1[3];
    fcoo1[0] =0;
    fcoo1[1] =0;
    fcoo1[2] =0;
    char fname1[15];
    sprintf(fname1,"T%+03d%02dF2",tkid,footid);
    int fgid1= abs(tkid)/tkid*(abs(tkid)+1000*footid)+200000;

    AMSgvolume* fvol1=new AMSgvolume("TrPEEK-M", _nrot++, fname1, "TUBE",
				    fpar1, 3, fcoo1, nrm, "ONLY", 1, fgid1, 1);
    gvol->add(fvol1);

    geant fpar2[3];
    fpar2[0] = 0.395; //rmin
    fpar2[1] = 0.4;  //rmax
    fpar2[2] =0.815/2.; //dz
    
    geant fcoo2[3];
    fcoo2[0] =0.;
    fcoo2[1] =0.;
    fcoo2[2] =0.;
    char fname2[15]; // 2/3 of a screw Al 26 mg
    sprintf(fname2,"T%+03d%02dF3",tkid,footid);
    int fgid2= abs(tkid)/tkid*(abs(tkid)+1000*footid)+300000;

    AMSgvolume* fvol2=new AMSgvolume("TrFeet-Alloy-M", _nrot++, fname2, "TUBE",
				    fpar2, 3, fcoo2, nrm, "ONLY", 1, fgid2, 1);
    gvol->add(fvol2);
  }else if(type==2){ //outer layers foot - aka LayersO=1,8 or LayerJ=1,2
    geant fpar[3];
    fpar[0] = 0.; //rmin
    fpar[1] = 0.48;  //rmax
    fpar[2] = 0.39/2.; //dz
    
    geant fcoo[3];
    fcoo[0] =0;
    fcoo[1] =0;
    fcoo[2] =0;
    char fname[15];// Screw Al 39mg  + leg Al 73 mg + insert Al=760 m. All corrected for the column of Al already present as Plane Honeycomb
    sprintf(fname,"T%+03d%02dF1",tkid,footid);
    int fgid= abs(tkid)/tkid*(abs(tkid)+1000*footid)+100000;

    AMSgvolume* fvol=new AMSgvolume("TrFeet-Alloy-M", _nrot++, fname, "TUBE",
				    fpar, 3, fcoo, nrm, "ONLY", 1, fgid, 1);
    gvol->add(fvol);

  }else if(type==3){ // the foot for the L0 ladders  
  } else 
    printf("BuildLadderFoot-E- We are in trouble unknown foot type!  THIS SHOULD NOT HAPPEN!! \n;");
  
  mvol->add(gvol);
}


//============================================================================
void BuildHoneycomb(AMSgvolume *mvol, int plane)
{
// Build plane honeycomb support geometry (plane number begins from 1)
  geant par[3];
  geant parL0[10];
  geant coo[3];
  number nrm[3][3];
  char name[8];
  char name2[8];
  float pi=atan(1.)*4;
  std::ostrstream ost(name,sizeof(name));
  ost << "PLA" << plane << std::ends;
  if (plane==8){ // L0
    geant widthZ;

    VZERO(nrm,9*sizeof(nrm[0][0])/4);
    nrm[0][0] = nrm[1][1] = nrm[2][2] = 1;

    // === L0 Support Structure Honeycomb (SOLID3637)
    coo[0]=0;
    coo[1]=0;
    coo[2]=0; // Relative to the center of envelop volume

    widthZ=TkDBc::Head->_sup_hc_w[6];
    parL0[0]=22.5;                      // Start phi angle
    parL0[1]=360;                       // Total phi angle
    parL0[2]=8;                         // Number of sides
    parL0[3]=2;                         // Number of Z planes
    parL0[4]=-0.5*widthZ;               // Z of the lower plane
    parL0[5]=0;                         // Min distance from center to edge, lower plane
    parL0[6]=TkDBc::Head->_sup_hc_r[6]; // Max distance from center to edge, lower plane
    parL0[7]=0.5*widthZ;                // Z of the upper plane
    parL0[8]=parL0[5];                  // Min distance from center to edge, upper plane
    parL0[9]=parL0[6];                  // Max distance from center to edge, upper plane

    mvol->add(new AMSgvolume("CARBON",1,name,"PGON",parL0,10,coo,nrm,"ONLY",1,plane,1)); // M46J
    
    printf("\nPlane %d\n",plane);
    printf(" Coo    %f %f %f\n ",coo[0],coo[1],coo[2]);
    printf(" Params %f %f %f\n ",parL0[0],parL0[1],parL0[2]);
    printf("        %f %f %f\n ",parL0[3],parL0[4],parL0[5]);
    printf("        %f %f %f %f\n ",parL0[6],parL0[7],parL0[8],parL0[9]);
    printf(" Matr   %f %f %f \n",nrm[0][0],nrm[0][1],nrm[0][2]);
    printf("         %f %f %f \n",nrm[1][0],nrm[1][1],nrm[1][2]);
    printf("         %f %f %f \n",nrm[2][0],nrm[2][1],nrm[2][2]);
    
    coo[2]+=0.5*widthZ;

    // === L0 Support Structure Honeycomb's Skin CFRPs (SOLIDS 3636 and 3638)
    widthZ=TkDBc::Head->_sup_hc_skin_w[6];
    coo[2]+=0.5*widthZ;
    parL0[0]=22.5;
    parL0[1]=360;
    parL0[2]=8;
    parL0[3]=2;
    parL0[4]=-0.5*widthZ;
    parL0[5]=0;
    parL0[6]=TkDBc::Head->_sup_l0_hc_skin_l;
    parL0[7]=0.5*widthZ;
    parL0[8]=parL0[5];
    parL0[9]=parL0[6];
    sprintf(name2,"USK%d",plane);

    mvol->add(new AMSgvolume("CARBON",1,name2,"PGON",parL0,10,coo,nrm,"ONLY",1,plane,1)); // M55J
    if (debugL0) printf("--TKGeom-L0Debug-I- Honeycomb upper plane z: %8.4f\n",coo[2]);
    coo[2]*=-1;
    sprintf(name2,"LSK%d",plane);
    mvol->add(new AMSgvolume("CARBON",1,name2,"PGON",parL0,10,coo,nrm,"ONLY",1,plane,1)); // M55J
    if (debugL0) printf("--TKGeom-L0Debug-I- Honeycomb lower plane z: %8.4f\n",coo[2]);
  }
  else if(plane==7){
    par[0] = 0;
    par[1] = TkDBc::Head->_sup_hc_r[0];
    par[2] = TkDBc::Head->_sup_hc_w[0]/2. - TkDBc::Head->_sup_hc_skin_w[0];
    
    coo[0] = coo[1] = 0;
    coo[2] = 0;
    
    VZERO(nrm,9*sizeof(nrm[0][0])/4);
    nrm[0][0] = nrm[1][1] = nrm[2][2] = 1;
    
    mvol->add(new AMSgvolume("Tr_HoneyOUT", _nrot++, name,
			     "TUBE", par, 3, coo, nrm, "ONLY", 1, plane, 1));

  if(plane>0&&plane!=6){
    printf("\nPlane %d\n",plane);
    printf(" Coo %f %f %f\n ",coo[0],coo[1],coo[2]);    
    printf(" Half dim %f %f %f\n ",par[0],par[1],par[2]);
    printf(" Matr  %f %f %f \n",nrm[0][0],nrm[0][1],nrm[0][2]);
    printf("        %f %f %f \n",nrm[1][0],nrm[1][1],nrm[1][2]);
    printf("        %f %f %f \n",nrm[2][0],nrm[2][1],nrm[2][2]);
  }
    par[0] = 0;
    par[1] = TkDBc::Head->_sup_hc_r[0];
    par[2] = TkDBc::Head->_sup_hc_skin_w[0]/2.;
    coo[0] = coo[1] = 0;
    coo[2] = TkDBc::Head->_sup_hc_w[0]/2. + TkDBc::Head->_sup_hc_skin_w[0]/2.;
    sprintf(name2,"USK%d",plane);
    mvol->add(new AMSgvolume("Tr_HoneySkin", _nrot++, name2,
			     "TUBE", par, 3, coo, nrm, "ONLY", 1, plane, 1));

    coo[2] = - TkDBc::Head->_sup_hc_w[0]/2. - TkDBc::Head->_sup_hc_skin_w[0]/2.;
    sprintf(name2,"LSK%d",plane);
    mvol->add(new AMSgvolume("Tr_HoneySkin", _nrot++, name2,
			     "TUBE", par, 3, coo, nrm, "ONLY", 1, plane, 1));

  }else if(plane==6){
    //       plan6 X size/2.
    par[0]=TkDBc::Head->Plane6Size[0]/2.;
    //       plan6 Y size/2.
    par[1]=TkDBc::Head->Plane6Size[1]/2.;
    //       plan6E thickness/2.
    double p6z= TkDBc::Head->Plane6Size[2]-TkDBc::Head->Plane6_skin_w[0]*2; 
    par[2]=p6z/2.;

    
    coo[0] = coo[1] = 0;
    coo[2] =-1*TkDBc::Head->Plane6EnvelopSize[2]/2+ p6z/2 +TkDBc::Head->Plane6_skin_w[0];
    
    
    VZERO(nrm,9*sizeof(nrm[0][0])/4);
    nrm[0][0] = nrm[1][1] = nrm[2][2] = 1;
   
    mvol->add(new AMSgvolume("Tr_HoneyOUT", _nrot++, name,
			     "BOX", par, 3, coo, nrm, "ONLY", 1, plane, 1));

    printf("\nPlane %d\n",plane);
    printf(" Coo %f %f %f\n ",coo[0],coo[1],coo[2]);    
    printf(" Half dim %f %f %f\n ",par[0],par[1],par[2]);
    printf(" Matr  %f %f %f \n",nrm[0][0],nrm[0][1],nrm[0][2]);
    printf("        %f %f %f \n",nrm[1][0],nrm[1][1],nrm[1][2]);
    printf("        %f %f %f \n",nrm[2][0],nrm[2][1],nrm[2][2]);

    //ADD CARBON SKINS
    //       plan6 X size/2.
    par[0]= TkDBc::Head->Plane6Size[0]/2.;
    //       plan6 Y size/2.
    par[1]= TkDBc::Head->Plane6Size[1]/2.;
    //       plan6E thickness/2.
    par[2]= TkDBc::Head->Plane6_skin_w[0]/2.;    //       plan6E X size/2.
    
    
    coo[0] = coo[1] = 0;
    coo[2] =-1*TkDBc::Head->Plane6EnvelopSize[2]/2. + TkDBc::Head->Plane6_skin_w[0]+p6z+TkDBc::Head->Plane6_skin_w[0]/2.;
    sprintf(name2,"USK%d",plane);
    mvol->add(new AMSgvolume("pl6_HoneySkin", _nrot++, name2,
			     "BOX", par, 3, coo, nrm, "ONLY", 1, plane, 1));

    coo[2] =-1*TkDBc::Head->Plane6EnvelopSize[2]/2.  +TkDBc::Head->Plane6_skin_w[0]/2.;

    sprintf(name2,"LSK%d",plane);
    mvol->add(new AMSgvolume("pl6_HoneySkin", _nrot++, name2,
			     "BOX", par, 3, coo, nrm, "ONLY", 1, plane, 1));

    //----------------------------------------------------------------------------------
     // Add the top cover of plane 6
     sprintf(name,"COV%d",plane );

     //       plan6 X size/2.
     par[0]=TkDBc::Head->Plane6Size[0]/2.;
     //       plan6 Y size/2.
     par[1]=TkDBc::Head->Plane6Size[1]/2.;
    //       plan6E thickness/2.
     double p6zt= 0.4 -TkDBc::Head->Plane6_skin_w[1]*2 ; 
     par[2]= p6zt/2. ;    //       plan6 cover Z size/2.

    
    coo[0] = coo[1] = 0;
    coo[2] = TkDBc::Head->Plane6EnvelopSize[2]/2.  - TkDBc::Head->Plane6_skin_w[1] - p6zt/2.;
    
    
    VZERO(nrm,9*sizeof(nrm[0][0])/4);
    nrm[0][0] = nrm[1][1] = nrm[2][2] = 1;
    // PZ FIXME cross check the material
    mvol->add(new AMSgvolume("Tr_HoneyIN", _nrot++, name,
			     "BOX", par, 3, coo, nrm, "ONLY", 1, plane, 1));

    printf("\nPlane cover %d\n",plane);
    printf(" Coo %f %f %f\n ",coo[0],coo[1],coo[2]);    
    printf(" Half dim %f %f %f\n ",par[0],par[1],par[2]);
    printf(" Matr  %f %f %f \n",nrm[0][0],nrm[0][1],nrm[0][2]);
    printf("        %f %f %f \n",nrm[1][0],nrm[1][1],nrm[1][2]);
    printf("        %f %f %f \n",nrm[2][0],nrm[2][1],nrm[2][2]);


    //ADD CARBON SKINS
    //       plan6 X size/2.
    par[0]=TkDBc::Head->Plane6Size[0]/2.;
    //       plan6 Y size/2.
    par[1]=TkDBc::Head->Plane6Size[1]/2.;
    //       plan6E thickness/2.
    par[2]= TkDBc::Head->Plane6_skin_w[1]/2.;    //       plan6E X size/2.
    
    
    coo[0] = coo[1] = 0;
    coo[2] =TkDBc::Head->Plane6EnvelopSize[2]/2. - TkDBc::Head->Plane6_skin_w[1]/2.;
    sprintf(name2,"USH%d",plane);
    mvol->add(new AMSgvolume("pl6_HoneySkin", _nrot++, name2,
			     "BOX", par, 3, coo, nrm, "ONLY", 1, plane, 1));

    coo[2] =TkDBc::Head->Plane6EnvelopSize[2]/2.- TkDBc::Head->Plane6_skin_w[1] - p6zt - TkDBc::Head->Plane6_skin_w[1]/2.;

    sprintf(name2,"LSH%d",plane);
    mvol->add(new AMSgvolume("pl6_HoneySkin", _nrot++, name2,
			     "BOX", par, 3, coo, nrm, "ONLY", 1, plane, 1));



    //-------------------------------------------------------    
    

    // add small Spacers
    for(int kk=-3;kk<=3;kk++){
      sprintf(name,"SPA%d",kk+3 );
      
      //       Spacer X size/2.
      par[0]= 5.0 /2.;
      //       Spacer Y size/2.
      par[1]= 0.7 /2.;
     //       Spacer thickness/2.
      double spz=TkDBc::Head->Plane6EnvelopSize[2] - p6zt -p6z  - 4*TkDBc::Head->Plane6_skin_w[1];
      par[2]=  spz/2.;
     
     //            Spacers pitch
     coo[0] = kk * 14.2;
     coo[1] = 0;
     coo[2] = -1*TkDBc::Head->Plane6EnvelopSize[2]/2. + 2*TkDBc::Head->Plane6_skin_w[0]+p6z   + par[2];
    
    
     VZERO(nrm,9*sizeof(nrm[0][0])/4);
     nrm[0][0] = nrm[1][1] = nrm[2][2] = 1;

     mvol->add(new AMSgvolume("pl6_HoneySkin", _nrot++, name,
			      "BOX", par, 3, coo, nrm, "ONLY", 1, kk+3, 1));

    }
    // add long Spacers
    for(int kk=-1;kk<2;kk+=2){
      sprintf(name,"SPC%d",(kk>0)?1:2 );
      
      //       Spacer X size/2.
      par[0]= 72.0 /2.;
      //       Spacer Y size/2.
      par[1]= 1.4 /2.;
     //       Spacer thickness/2.
      double spz=TkDBc::Head->Plane6EnvelopSize[2] - p6zt -p6z  - 4*TkDBc::Head->Plane6_skin_w[1];
     par[2]=  spz/2.;
     
     //            Spacers pitch
     coo[0] = 0;
     coo[1] = kk*(34+par[1]);
     coo[2] =-1*TkDBc::Head->Plane6EnvelopSize[2]/2. + 2*TkDBc::Head->Plane6_skin_w[0]+p6z   + par[2];
    
    
     VZERO(nrm,9*sizeof(nrm[0][0])/4);
     nrm[0][0] = nrm[1][1] = nrm[2][2] = 1;
     // FIX ME  material !!!!!!
     mvol->add(new AMSgvolume("pl6_HoneySkin", _nrot++, name,
			      "BOX", par, 3, coo, nrm, "ONLY", 1, kk+2, 1));

    }



  }
  else if(plane==1){ //Lets Consider the LBBX
  
    par[0] = 0;
    par[1] = TkDBc::Head->_sup_hc_r[plane-1];
    par[2] = TkDBc::Head->_sup_hc_w[plane-1]/2.-TkDBc::Head->_sup_hc_skin_w[plane-1];
    
    coo[0] = coo[1] = 0;
    coo[2] = -TkDBc::Head->_dz[plane-1];


    
    
    VZERO(nrm,9*sizeof(nrm[0][0])/4);
    nrm[0][0] = nrm[1][1] = nrm[2][2] = 1;
    char mate[50];
    if(plane==1||plane==5)
      sprintf(mate,"Tr_HoneyOUT");
    else
      sprintf(mate,"Tr_HoneyIN");
    mvol->add(new AMSgvolume(mate, _nrot++, name,
			     "TUBE", par, 3, coo, nrm, "ONLY", 1, plane, 1));

    if(plane>0&&plane!=6){
      printf("\nPlane %d\n",plane);
      printf(" Coo %f %f %f\n ",coo[0],coo[1],coo[2]);    
      printf(" Half dim %f %f %f\n ",par[0],par[1],par[2]);
      printf(" Matr  %f %f %f \n",nrm[0][0],nrm[0][1],nrm[0][2]);
      printf("        %f %f %f \n",nrm[1][0],nrm[1][1],nrm[1][2]);
      printf("        %f %f %f \n",nrm[2][0],nrm[2][1],nrm[2][2]);
    }
    // ADD CARBON SKINS
    par[0] = 0;
    par[1] = TkDBc::Head->_sup_hc_r[plane-1];
    par[2] = TkDBc::Head->_sup_hc_skin_w[plane-1]/2.;
    coo[0] = coo[1] = 0;
    coo[2] = -TkDBc::Head->_dz[plane-1]+ TkDBc::Head->_sup_hc_w[plane-1]/2. + TkDBc::Head->_sup_hc_skin_w[plane-1]/2.;


    sprintf(name2,"USK%d",plane);
    mvol->add(new AMSgvolume("Tr_HoneySkin", _nrot++, name2,
			     "TUBE", par, 3, coo, nrm, "ONLY", 1, plane, 1));

    coo[2] = -TkDBc::Head->_dz[plane-1]- TkDBc::Head->_sup_hc_w[plane-1]/2. - TkDBc::Head->_sup_hc_skin_w[plane-1]/2.;

    sprintf(name2,"LSK%d",plane);
    mvol->add(new AMSgvolume("Tr_HoneySkin", _nrot++, name2,
			     "TUBE", par, 3, coo, nrm, "ONLY", 1, plane, 1));

    // ADD LBBX
    float lbcoo[5][3]={//X,Y, angle
      {-5.41,  9.125, 36.45},
      {-5.41, -5.475, 36.45},
      { 5.41, 12.575, 36.45},
      { 5.41,  1.825,-36.45},
      { 5.41,-12.775,-36.45}
    };
    par[0]=3.2/2.;
    par[1]=3.4/2.;
    par[2]=1.5/2.;
    for (int lbb=0;lbb<5;lbb++){
      coo[0]=lbcoo[lbb][0];
      coo[1]=lbcoo[lbb][1];
      coo[2]= -TkDBc::Head->_dz[plane-1]+TkDBc::Head->_sup_hc_w[plane-1]/2.+TkDBc::Head->_sup_hc_skin_w[plane-1]+par[2];

      nrm[0][0]=nrm[1][1]=cos(lbcoo[lbb][2]/180.*pi);
      nrm[0][1]=sin(lbcoo[lbb][2]/180.*pi);
      nrm[1][0]=-1*nrm[0][1];
      nrm[0][2]=nrm[2][0]=nrm[2][1]=nrm[1][2]=0;
      nrm[2][2]=1;
      sprintf(name2,"LBB%d",lbb);
      mvol->add(new AMSgvolume("LBBX-med", _nrot++, name2,
			       "BOX", par, 3, coo, nrm, "ONLY", 1, lbb, 1));
    }
  }
  else{ //normal planes
  
    par[0] = 0;
    par[1] = TkDBc::Head->_sup_hc_r[plane-1];
    par[2] = TkDBc::Head->_sup_hc_w[plane-1]/2.-TkDBc::Head->_sup_hc_skin_w[plane-1];
    
    coo[0] = coo[1] = 0;
    coo[2] = -TkDBc::Head->_dz[plane-1];
    
    
    VZERO(nrm,9*sizeof(nrm[0][0])/4);
    nrm[0][0] = nrm[1][1] = nrm[2][2] = 1;
    char mate[50];
    if(plane==1||plane==5)
      sprintf(mate,"Tr_HoneyOUT");
    else
      sprintf(mate,"Tr_HoneyIN");
    mvol->add(new AMSgvolume(mate, _nrot++, name,
			     "TUBE", par, 3, coo, nrm, "ONLY", 1, plane, 1));
  if(plane>0&&plane!=6){
    printf("\nPlane %d\n",plane);
    printf(" Coo %f %f %f\n ",coo[0],coo[1],coo[2]);    
    printf(" Half dim %f %f %f\n ",par[0],par[1],par[2]);
    printf(" Matr  %f %f %f \n",nrm[0][0],nrm[0][1],nrm[0][2]);
    printf("        %f %f %f \n",nrm[1][0],nrm[1][1],nrm[1][2]);
    printf("        %f %f %f \n",nrm[2][0],nrm[2][1],nrm[2][2]);
  }

    // ADD CARBON SKINS
    par[0] = 0;
    par[1] = TkDBc::Head->_sup_hc_r[plane-1];
    par[2] = TkDBc::Head->_sup_hc_skin_w[plane-1]/2.;
    coo[0] = coo[1] = 0;
    coo[2] = -TkDBc::Head->_dz[plane-1]+ TkDBc::Head->_sup_hc_w[plane-1]/2. + TkDBc::Head->_sup_hc_skin_w[plane-1]/2.;

    sprintf(name2,"USK%d",plane);
    mvol->add(new AMSgvolume("Tr_HoneySkin", _nrot++, name2,
			     "TUBE", par, 3, coo, nrm, "ONLY", 1, plane, 1));

    coo[2] = -TkDBc::Head->_dz[plane-1]- TkDBc::Head->_sup_hc_w[plane-1]/2. - TkDBc::Head->_sup_hc_skin_w[plane-1]/2.;
    sprintf(name2,"LSK%d",plane);
    mvol->add(new AMSgvolume("Tr_HoneySkin", _nrot++, name2,
			     "TUBE", par, 3, coo, nrm, "ONLY", 1, plane, 1));

  }


  return;
}


void BuildPlane1NSupport(AMSgvolume* mvol){
//Leps honeycomb support
  float coo[3];
  float par[20];
  number nrm[3][3];
  char name[5];
  float pi=atan(1.)*4;

  sprintf(name,"P1NS");
  coo[0]=TkDBc::Head->P1NSupportCoo[0];
  coo[1]=TkDBc::Head->P1NSupportCoo[1];
  coo[2]=TkDBc::Head->P1NSupportCoo[2];

  //Al- honeycomb
  par[0]=0;    //inital phi
  par[1]=360;  // phi amplitude
  par[2]=8;    // pgon sides
  par[3]=2;    // n z planes
  par[4]=-TkDBc::Head->P1NSupportThickness/2.;   // z1
  par[5]=0;
  par[6]=TkDBc::Head->P1NSupportRadius;
  par[7]=TkDBc::Head->P1NSupportThickness/2.;
  par[8]=0;
  par[9]=TkDBc::Head->P1NSupportRadius;
  VZERO(nrm,9*sizeof(nrm[0][0])/4);
  nrm[0][0] = nrm[1][1] = nrm[2][2] = 1;  
  nrm[0][0] = nrm[1][1] =cos(22.5/180.*pi);
  nrm[0][1] = sin(22.5/180.*pi);
  nrm[1][0] = -1*sin(22.5/180.*pi);
  nrm[2][2] = 1;  
  
  mvol->add(new AMSgvolume("Tr_HoneyOUT", _nrot++, name,
			      "PGON", par, 10, coo, nrm, "ONLY", 1, 1, 1));


  sprintf(name, "P1NK");
  coo[0]=TkDBc::Head->P1NSupportCoo[0];
  coo[1]=TkDBc::Head->P1NSupportCoo[1];
  coo[2]=TkDBc::Head->P1NSupportCoo[2]+
    TkDBc::Head->P1NSupportThickness/2.+
    TkDBc::Head->P1NSupportSkinThickness/2.;

  //Al- honeycomb skin
  par[0]=0;    //inital phi
  par[1]=360;  // phi amplitude
  par[2]=8;    // pgon sides
  par[3]=2;    // n z planes
  par[4]=-TkDBc::Head->P1NSupportSkinThickness/2.;   // z1
  par[5]=0;
  par[6]=TkDBc::Head->P1NSupportRadius;
  par[7]=TkDBc::Head->P1NSupportSkinThickness/2.;
  par[8]=0;
  par[9]=TkDBc::Head->P1NSupportRadius;
  VZERO(nrm,9*sizeof(nrm[0][0])/4);
  nrm[0][0] = nrm[1][1] =cos(22.5/180.*pi);
  nrm[0][1] = sin(22.5/180.*pi);
  nrm[1][0] = -1*sin(22.5/180.*pi);
  nrm[2][2] = 1;  
  
  mvol->add(new AMSgvolume("P1NS_HoneySkin", _nrot++, name,
			      "PGON", par, 10, coo, nrm, "ONLY", 1, 1, 1));

  coo[2]=TkDBc::Head->P1NSupportCoo[2]-
    TkDBc::Head->P1NSupportThickness/2.-
    TkDBc::Head->P1NSupportSkinThickness/2.;

  mvol->add(new AMSgvolume("P1NS_HoneySkin", _nrot++, name,
			   "PGON", par, 10, coo, nrm, "ONLY", 1, 2, 1));



  sprintf(name,"P1NC");
  coo[0]=TkDBc::Head->P1NSupportCoo[0];
  coo[1]=TkDBc::Head->P1NSupportCoo[1];
  coo[2]=TkDBc::Head->P1NSupportCoo[2]-
    TkDBc::Head->P1NSupportThickness/2.-
    TkDBc::Head->P1NSupportSkinThickness-
    TkDBc::Head->P1NSCThickness/2.;

  //Al- honeycomb C struct
  par[0]=0;    //inital phi
  par[1]=360;  // phi amplitude
  par[2]=8;    // pgon sides
  par[3]=2;    // n z planes
  par[4]=-TkDBc::Head->P1NSCThickness/2.;   // z1
  par[5]=TkDBc::Head->P1NSCRadius-0.1;
  par[6]=TkDBc::Head->P1NSCRadius;
  par[7]=TkDBc::Head->P1NSCThickness/2.;
  par[8]=TkDBc::Head->P1NSCRadius-0.1;
  par[9]=TkDBc::Head->P1NSCRadius;
  VZERO(nrm,9*sizeof(nrm[0][0])/4);
  nrm[0][0] = nrm[1][1] = nrm[2][2] = 1;  
  nrm[0][0] = nrm[1][1] =cos(22.5/180.*pi);
  nrm[0][1] = sin(22.5/180.*pi);
  nrm[1][0] = -1*sin(22.5/180.*pi);
  nrm[2][2] = 1;  
  
  mvol->add(new AMSgvolume("P1NS_HoneySkin", _nrot++, name,
			      "PGON", par, 10, coo, nrm, "ONLY", 1, 1, 1));
  //
  //--->MLI on top of Plane-1 support(simplified structure,tube shape):
  //
  int gid(1);
  float mlidz1=0.055;//Alum-parts
  float mlidz2=0.0089;//Mylar-parts
  float mlidz3=0.019;//betacloth
  float mlihdz=0.025;//hole teflon
  float mliri=15;//tempor
  float mliro=TkDBc::Head->P1NSupportRadius+9;// "9" to have outer radious covering whole trkL1supp.
  float logoxs=50;
  float logoys=120;
  float logodz1=0.034;//logo's bcloth
  float logodz2=0.025;//logo's teflon (neglect 25mkm Al)
  float logoxp=55;
  //mli:              
  coo[0]=TkDBc::Head->P1NSupportCoo[0];
  coo[1]=TkDBc::Head->P1NSupportCoo[1];
  coo[2]=TkDBc::Head->P1NSupportCoo[2]+
         TkDBc::Head->P1NSupportThickness/2.+
         TkDBc::Head->P1NSupportSkinThickness + 0.01 + 0.01 + mlihdz + mlidz1/2;//0.01=secur.gap
  par[0]=mliri;
  par[1]=mliro;
  par[2]=mlidz1/2;
  mvol->add(new AMSgvolume("ALUMLI",0,"MLI1",
			      "TUBE", par, 3, coo, nrm, "ONLY", 1, gid, 1));//Alum
  par[2]=mlidz2/2;
  coo[2]=coo[2]+mlidz1/2+mlidz2/2;
  mvol->add(new AMSgvolume("MYLARMLI",0,"MLI2",
			      "TUBE", par, 3, coo, nrm, "ONLY", 1, gid, 1));//Mylar
  par[2]=mlidz3/2;
  coo[2]=coo[2]+mlidz2/2+mlidz3/2;
  mvol->add(new AMSgvolume("BCLOTHMLI",0,"MLI3",
			      "TUBE", par, 3, coo, nrm, "ONLY", 1, gid, 1));//betacloth
  //hole:
  coo[2]=TkDBc::Head->P1NSupportCoo[2]+
         TkDBc::Head->P1NSupportThickness/2.+
         TkDBc::Head->P1NSupportSkinThickness + 0.01+mlihdz/2;
  par[0]=0;
  par[1]=mliri+5.;
  par[2]=mlihdz/2;
  mvol->add(new AMSgvolume("TEFLONMLI",0,"MLIH",
			      "TUBE", par, 3, coo, nrm, "ONLY", 1, gid, 1));
  //Logos:
  coo[0]=TkDBc::Head->P1NSupportCoo[0]-logoxp;
  coo[1]=TkDBc::Head->P1NSupportCoo[1];
  coo[2]=TkDBc::Head->P1NSupportCoo[2]+
         TkDBc::Head->P1NSupportThickness/2.+
         TkDBc::Head->P1NSupportSkinThickness + 0.01 + 0.01 + mlihdz+mlidz1+mlidz2+mlidz3+logodz1/2;
  par[0]=logoxs/2;	 
  par[1]=logoys/2;
  par[2]=logodz1/2;
  mvol->add(new AMSgvolume("BCLOTHMLI",0,"LOG1",
			      "BOX", par, 3, coo, nrm, "ONLY", 1, gid, 1));//"-X" logo,bcloth
  coo[0]=TkDBc::Head->P1NSupportCoo[0]+logoxp;
  mvol->add(new AMSgvolume("BCLOTHMLI",0,"LOG2",
			      "BOX", par, 3, coo, nrm, "ONLY", 1, gid, 1));//"+X" logo,bcloth
			      
  par[2]=logodz2/2;
  coo[0]=TkDBc::Head->P1NSupportCoo[0]-logoxp;
  coo[2]=coo[2]+logodz1/2+logodz2/2;
  mvol->add(new AMSgvolume("TEFLONMLI",0,"LOG3",
			      "BOX", par, 3, coo, nrm, "ONLY", 1, gid, 1));//"-X" logo,teflon
  coo[0]=TkDBc::Head->P1NSupportCoo[0]+logoxp;
  mvol->add(new AMSgvolume("TEFLONMLI",0,"LOG4",
			      "BOX", par, 3, coo, nrm, "ONLY", 1, gid, 1));//"+X" logo,teflon
//
}

void BuildPlane8QLCSLayer(AMSgvolume* mvol, char* patt, char* mat, geant zPos, geant w){
  // Building a QLCS layer (QLCS itself or its skin, each consists of 4 parts by the L0 design)
  //
  //                              dy3
  //                              ___
  //                    _________|dx3| _
  //                   |             | ^
  //                   |<----dy1---->| |
  //                   |             | |
  //            -------              | w
  //           |  ^                  | i
  //           |  |                  | d
  //           |<--------dy2-------->| t
  //    -------   |                  | h
  //   | ^        | dx2              | X
  //   | | dx1    |                  | |
  //   | v        v                  | |
  //   ------------------------------| v
  //   <-----------widthY----------->

  // Combined 4 QLCS window structure near the center of the plane
  // Should be partially substracted from each QLCS segment shown above
  //
  //                            |------------------| ^
  //                            |                  |dx5
  //        <----dy5---->       |            ______| v
  //        |-----------|       |           |        ^
  //        |           |       |           |       dx6
  //  ______|           |<-gap->|___________|        v
  // |                  |       ^                                
  // |                  |      dx4
  // |__________________|       v
  // <--------dy4------->

  AMSgvolume *gvol;
  geant coo[3];
  geant par[3];
  char name[5];
  int ipart=0;
  number nrm[3][3]={{1,0,0},{0,1,0},{0,0,1}};

  geant widthX=TkDBc::Head->_sup_qlcs_l[0];
  geant widthY=TkDBc::Head->_sup_qlcs_l[1];
  geant dx1=TkDBc::Head->_sup_qlcs_dx[0];
  geant dx2=TkDBc::Head->_sup_qlcs_dx[1];
  geant dx3=TkDBc::Head->_sup_qlcs_dx[2];
  geant dx5=0.5;
  geant dx6=0.49;
  geant dy1=TkDBc::Head->_sup_qlcs_dy[0];
  geant dy2=TkDBc::Head->_sup_qlcs_dy[1];
  geant dy3=TkDBc::Head->_ssize_inactive[1];
  geant dy4 = 11.31;
  geant dy5 = 5.99;
  geant gapBetweenQLCS=0.02;
  geant gapBetweenLadders=0.012;
  geant gap=0.02; // Secure gap for substracting the volumes

#ifdef __G4AMS__
  for (int signX=-1;signX<2;signX+=2){
    for (int signY=-1;signY<2;signY+=2){
      sprintf(name, "%s%d", patt, ++ipart); // Make global
      // The first step (space for long ladders)
      coo[0]=signX*0.5*(widthX+gapBetweenQLCS);
      coo[1]=signY*0.5*(dy1+gapBetweenQLCS);
      coo[2]=zPos;
      par[0]=0.5*widthX;
      par[1]=0.5*dy1;
      par[2]=0.5*w;
      gvol=new AMSgvolume(mat,0,name,"BOX",par,3,coo,nrm,"BOOL",1,8,1); // Base box

      // The second step (space for medium ladders)
      coo[0]=-signX*0.5*(widthX-dx2); // Coordinates are relative to the base box
      coo[1]=signY*0.5*dy2;
      coo[2]=0; // There is no any relative shift in Z direction
      par[0]=0.5*dx2;
      par[1]=0.5*(dy2-dy1);
      gvol->addboolean("BOX",par,3,coo,nrm,'+');

      // The third step (space for short ladders)
      coo[0]=-signX*0.5*(widthX-dx1); // Coordinates are still relative to the base box
      coo[1]=signY*0.5*(widthY+dy2-dy1);
      par[0]=0.5*dx1;
      par[1]=0.5*(widthY-dy2);
      gvol->addboolean("BOX",par,3,coo,nrm,'+');

      // The bump for the ladder area closest to center inside the first step area
      coo[0]=signX*0.5*(widthX+dx3); // Coordinates are relative to the base box
      coo[1]=signY*0.5*(-dy1+dy3);
      par[0]=0.5*dx3;
      par[1]=0.5*dy3;
      gvol->addboolean("BOX",par,3,coo,nrm,'+');

      // The window for the ladder arrea near the center of the plane
      if (signX*signY==1){
        coo[0]=signX*0.5*(-widthX+dx5-0.5*gapBetweenLadders);
        coo[1]=signY*0.5*(-dy1+dy4-0.5*gapBetweenLadders);
        par[0]=0.5*dx5+0.25*gapBetweenLadders;
        par[1]=0.5*dy4+0.25*gapBetweenLadders;
      } else if (signX*signY==-1) {
        coo[0]=signX*0.5*(-widthX+dx6-0.5*gapBetweenLadders);
        coo[1]=signY*0.5*(-dy1+dy5-0.5*gapBetweenLadders);
        par[0]=0.5*dx6+0.25*gapBetweenLadders;
        par[1]=0.5*dy5+0.25*gapBetweenLadders;
      } else {
          cerr<<"amsgeom::tkgeomL0-FATAL- Unknown configuration for the QLCS geometry generation\n";
          exit(-5);
      }
      par[2]+=gap;
      gvol->addboolean("BOX",par,3,coo,nrm,'-');

      mvol->add(gvol);
    }
  }
#endif
}


void BuildPlane8QLCS(AMSgvolume* mvol, bool isTop){
  char name[5];
  geant zPos, w;

  zPos=-0.5*(TkDBc::Head->_sup_qlcs_w+TkDBc::Head->_sup_qlcs_skin_w);
  w=TkDBc::Head->_sup_qlcs_skin_w;
  sprintf(name, isTop?"QDT":"QDB");
  if (debugL0) printf("--TKGeom-L0Debug-I- %s plane, QLCS lower skin z: %8.4f w: %8.4f\n", isTop?"Top":"Bottom", zPos, w);
  BuildPlane8QLCSLayer(mvol, name, "CARBON", zPos, w); // M46J

  zPos=0;
  w=TkDBc::Head->_sup_qlcs_w;
  sprintf(name, isTop ?"QLT":"QLB");
  if (debugL0) printf("--TKGeom-L0Debug-I- %s plane, QLCS main body z: %8.4f w: %8.4f\n", isTop?"Top":"Bottom", zPos, w);
  BuildPlane8QLCSLayer(mvol, name, "CARBON", zPos, w); // M46J

  zPos=0.5*(TkDBc::Head->_sup_qlcs_w+TkDBc::Head->_sup_qlcs_skin_w);
  w=TkDBc::Head->_sup_qlcs_skin_w;
  sprintf(name, isTop ?"QUT":"QUB");
  if (debugL0) printf("--TKGeom-L0Debug-I- %s plane, QLCS upper skin z: %8.4f w: %8.4f\n", isTop?"Top":"Bottom", zPos, w);
  BuildPlane8QLCSLayer(mvol, name, "CARBON", zPos, w); // M46J
}


// L0 radiators
void BuildPlane8Radiators(AMSgvolume* mvol){
  char name[5];
  double gap;
  double r, ang;
  double petalBase, petalSmallSide, petalLargeSide;
  int nrad=0;
  double angle=0;
  geant par[3]={0};
  geant parTube[5]={0};
  geant parTrap[11]={0};
  geant coo[3]={0,0,0};
  number nrm[3][3]={0};
  std::vector<double> dx, dy;
  std::vector<double> x, y, z;
  std::vector<double> widthsEnv, widthsRad;
  std::vector<double> centersX, centersY, centersZ;
  std::vector<std::vector<double>> n;
  AMSgvolume *env, *gvol1, *gvol2;

  // AMS coordinate system
  double coo1=137.769;
  double coo2=coo1/sqrt(2.);
  centersX={0.00, coo2, coo1,  coo2,  0.00, -coo2, -coo1, -coo2};
  centersY={coo1, coo2, 0.00, -coo2, -coo1, -coo2,  0.00,  coo2};
  centersZ={0.5*(176.0+215.925)};
  widthsEnv={104.8,39.925,16.137}; // {X,Y,Z} 
  widthsRad={48.4,20.163,0.3};
  r=2.0;
  ang=40; // deg
  petalBase=6.383;
  petalSmallSide=3;
  petalLargeSide=23.9;
  gap=0.02; // Secure gap for substracting the volumes

  // Creating 8 envelops for all the radiator sets
  // Starting from 12:00, clockwise, according to the XY plane
  for (int i=0;i<8;i++){
    if (i==2||i==4||i==6) {
      if (i!=4) widthsRad[1]=20.;
      dx={8.45,12.8,18.25,6.0,7.926,5.8};
      dy={3.663,7.0,9.5,8.0,6.0};
      if (i==2||i==6) {
        dx.push_back(26.4); // Extra top parallelepiped
        dy.push_back(9.0);
      }
    } else {
      if (i==5||i==7) widthsRad[1]=20.7;
      dx={7.75,14.2,19.45};
      dy={4.2,7.0,9.5,1.0}; // The last value is a diff between two windows' widths
      if (i==1){
        dx.push_back(5.);
        dy.push_back(4.663);
        dy.push_back(5.);
      } else if (i==3){
        dx.push_back(19.); // The width of small radiator
        dx.push_back(11.8); // The distance from the edge to window
      }
    }

    // Envelop for each radiator block to get the proper rotation of boolean solids
    // Now the coordinate system is {X,-Z,Y} (relative)
    coo[0]=centersX[i];
    coo[1]=centersY[i];
    coo[2]=centersZ[0];
    par[0]=0.5*widthsEnv[0];
    par[1]=0.5*widthsEnv[1];
    par[2]=0.5*widthsEnv[2];
    angle=-atan(1.)*i;
    n={{-cos(angle),0,-sin(angle)},{-sin(angle),0,cos(angle)},{0,1,0}};
    for (int i=0;i<3;++i) std::copy(n[i].begin(), n[i].end(), nrm[i]);
    sprintf(name, "RAE%d",++nrad);
    env=(AMSgvolume*)mvol
      ->add(new AMSgvolume("VACUUM", 1, name, "BOX",
                          par, 3, coo, nrm, "ONLY", 1, 8, 0));

    // The base parallelepiped of left and right radiators
    // according to the AMS coordinate system
    sprintf(name, "RAL%d",nrad); // Left
    coo[0]=-0.5*(widthsEnv[0]-widthsRad[0]);
    coo[1]=-0.5*(widthsEnv[1]-widthsRad[1]);
    coo[2]=-0.5*(widthsEnv[2]-widthsRad[2]);
    par[0]=0.5*widthsRad[0];
    par[1]=0.5*widthsRad[1];
    par[2]=0.5*widthsRad[2];
    n={{1,0,0},{0,1,0},{0,0,1}};
    for (int i=0;i<3;++i) std::copy(n[i].begin(), n[i].end(), nrm[i]);
    gvol1=new AMSgvolume("1/2ALUM",0,name,"BOX",par,3,coo,nrm,"BOOL",1,8,1);
    sprintf(name, "RAR%d",nrad); // Right
    coo[0]*=-1.;
    if (i==3) { // Short right radiator in block 4
      coo[0]=0.5*(widthsEnv[0]-2*widthsRad[0]+dx[3]);
      par[0]=0.5*dx[3];
    }
    gvol2=new AMSgvolume("1/2ALUM",0,name,"BOX",par,3,coo,nrm,"BOOL",1,8,1);

#ifdef __G4AMS__
    // First window
    coo[0]=-(0.5*widthsRad[0]-(dx[0]+0.5*dx[1]));
    coo[1]=0.5*(dy[2]-dy[0]);
    coo[2]=0;
    par[0]=0.5*dx[1];
    par[1]=0.5*dy[1];
    par[2]+=gap; // Slightly more than needed to capture the border
    gvol1->addboolean("BOX",par,3,coo,nrm,'-');
    if (i!=3) {
      coo[0]*=-1.;
      gvol2->addboolean("BOX",par,3,coo,nrm,'-');
    }

    // Second window
    coo[0]=-(0.5*widthsRad[0]-(dx[0]+dx[1]+dx[2]+0.5*(widthsRad[0]-dx[0]-dx[1]-dx[2])));
    coo[1]=0.5*(dy[2]-dy[0]);
    par[0]=0.5*(widthsRad[0]-dx[0]-dx[1]-dx[2]);
    par[1]=0.5*(dy[1]+dy[3]);
    par[0]+=gap; // Slightly more than needed to capture the border
    coo[0]+=0.5*gap;
    if (i==2||i==4||i==6) {
      coo[1]=-0.5*dy[0];
      par[1]=0.5*(widthsRad[1]-dy[0]);
      par[1]+=gap;
      coo[1]+=0.5*gap;
    }
    gvol1->addboolean("BOX",par,3,coo,nrm,'-');
    coo[0]*=-1.;
    if (i==3) {
      par[0]=0.5*(dx[3]-dx[4]);
      par[0]+=gap;
      coo[0]=0.5*dx[3]-(dx[4]+0.5*(dx[3]-dx[4]));
      coo[0]-=0.5*gap;
    }
    gvol2->addboolean("BOX",par,3,coo,nrm,'-');

    // Changing extra elements for some radiator blocks
    if (i==2||i==4||i==6) {
      coo[0]=-(0.5*widthsRad[0]-dx[0]-dx[1]-dx[2]+0.5*dx[3]);
      coo[1]=-0.5*widthsRad[1]+0.5*dy[3];
      par[0]=0.5*dx[3];
      par[1]=0.5*dy[3];
      par[1]+=gap;
      coo[1]-=0.5*gap;
      gvol1->addboolean("BOX",par,3,coo,nrm,'-');
      coo[0]*=-1.;
      gvol2->addboolean("BOX",par,3,coo,nrm,'-');

      coo[0]=-(0.5*widthsRad[0]-dx[0]-dx[1]-dx[2]+dx[3]+0.5*dx[4]);
      coo[1]=-0.5*widthsRad[1]+0.5*dy[4];
      par[0]=0.5*dx[4];
      par[1]=0.5*dy[4];
      par[0]+=gap;
      coo[0]+=0.5*gap;
      par[1]+=gap;
      coo[1]-=0.5*gap;
      gvol1->addboolean("BOX",par,3,coo,nrm,'-');
      coo[0]*=-1.;
      gvol2->addboolean("BOX",par,3,coo,nrm,'-');

      if (i==2||i==6) {
        coo[0]=0.5*(widthsRad[0]-dx[6]);
        coo[1]=0.5*(widthsRad[1]+dy[5]);
        par[0]=0.5*dx[6];
        par[1]=0.5*dy[5];
        gvol1->addboolean("BOX",par,3,coo,nrm,'+');
        coo[0]*=-1.;
        gvol2->addboolean("BOX",par,3,coo,nrm,'+');
      } 
    } else if (i==1){
      coo[0]=-0.5*(widthsRad[0]-dx[3]);
      coo[1]=0.5*widthsRad[1]-dy[4]-0.5*dy[5];
      par[0]=0.5*dx[3];
      par[1]=0.5*dy[5];
      par[0]+=gap;
      coo[0]-=0.5*gap;
      gvol1->addboolean("BOX",par,3,coo,nrm,'-');
      coo[0]*=-1.;
      gvol2->addboolean("BOX",par,3,coo,nrm,'-');
    }

    env->add(gvol1);
    env->add(gvol2);

    if (i==0||i==1||i==3||i==4||i==7) {
      sprintf(name, "RTL%d",nrad);
      coo[0]=-0.5*widthsEnv[0]+0.5*widthsRad[0];
      if (i==1) coo[0]+=0.5*(widthsRad[0]-43.4);
      coo[1]=-0.5*widthsEnv[1]+widthsRad[1];
      coo[2]=-0.5*widthsEnv[2]+widthsRad[2]+r;
      parTube[0]=r;
      parTube[1]=r+widthsRad[2];
      parTube[2]=0.5*widthsRad[0];
      if (i==1) parTube[2]-=0.5*(widthsRad[0]-43.4);
      parTube[3]=180.-ang;
      parTube[4]=180.;
      n={{0,0,1},{0,1,0},{1,0,0}};
      for (int i=0;i<3;++i) std::copy(n[i].begin(), n[i].end(), nrm[i]);
      if (i!=7) {
        env->add(new AMSgvolume("1/2ALUM",1,name,"TUBS",parTube,5,coo,nrm,"ONLY",1,8,1));
      }
      sprintf(name, "RTR%d",nrad);
      coo[0]*=-1.;
      if (i!=3) {
        env->add(new AMSgvolume("1/2ALUM",1,name,"TUBS",parTube,5,coo,nrm,"ONLY",1,8,1));
      }

      coo[0]*=-1.;
      coo[1]=-0.5*widthsEnv[1]+widthsRad[1]+(r+0.5*widthsRad[2])*sin(ang*degree)+0.5*petalLargeSide*cos(ang*degree);
      coo[2]=-0.5*widthsEnv[2]+widthsRad[2]+r-(r+0.5*widthsRad[2])*cos(ang*degree)+0.5*petalLargeSide*sin(ang*degree);
      parTrap[0]=0.5*widthsRad[0];   // dz
      parTrap[1]=0;                  // Theta
      parTrap[2]=0.;                 // Phi
      parTrap[3]=0.5*petalLargeSide; // dy1
      parTrap[4]=0.5*widthsRad[2];   // dx1
      parTrap[5]=parTrap[4];         // xd2
      parTrap[6]=0;                  // Alpha1
      parTrap[7]=0.5*petalSmallSide; // dy2
      parTrap[8]=0.5*widthsRad[2];   // dx3
      parTrap[9]=parTrap[8];         // dx4
      parTrap[10]=parTrap[6];        // Alpha2
      n={{0,0,-1},{-sin(ang*degree),cos(ang*degree),0},{cos(ang*degree),sin(ang*degree),0}};
      for (int i=0;i<3;++i) std::copy(n[i].begin(), n[i].end(), nrm[i]);
      sprintf(name, "RCL%d",nrad);
      gvol1=new AMSgvolume("1/2ALUM",1,name,"TRAP",parTrap,11,coo,nrm,"BOOL",1,8,1);
      coo[0]*=-1.;
      parTrap[3]=0.5*petalSmallSide;
      parTrap[7]=0.5*petalLargeSide;
      sprintf(name, "RCR%d",nrad);
      gvol2=new AMSgvolume("1/2ALUM",1,name,"TRAP",parTrap,11,coo,nrm,"BOOL",1,8,1);

      coo[0]=0;
      coo[1]=-0.25*petalLargeSide;
      coo[2]=0;
      par[0]=0.5*widthsRad[2];     // local Z
      par[1]=0.25*petalLargeSide;  // local Y
      par[2]=parTrap[0];           // local X
      n={{1,0,0},{0,1,0},{0,0,1}};
      for (int i=0;i<3;++i) std::copy(n[i].begin(), n[i].end(), nrm[i]);
      gvol1->addboolean("BOX",par,3,coo,nrm,'+');
      gvol2->addboolean("BOX",par,3,coo,nrm,'+');

      if (i==1) {
        double widthWindowX=5.;
        double widthWindowY=6.;
        coo[1]=-0.5*petalLargeSide+0.5*widthWindowY;
        coo[2]=0.5*widthsRad[0]-0.5*widthWindowX;
        par[1]=0.5*widthWindowY; // local Y
        par[2]=0.5*widthWindowX; // local X
        par[0]+=gap;
        par[1]+=gap;
        par[2]+=gap;
        coo[1]-=0.5*gap;
        coo[2]+=0.5*gap;
        gvol1->addboolean("BOX",par,3,coo,nrm,'-');
        coo[2]*=-1.;
        gvol2->addboolean("BOX",par,3,coo,nrm,'-');
      }

      if (i!=7) env->add(gvol1);
      if (i!=3) env->add(gvol2);
    }
#endif
  }
}


void BuildPlane8Cupolas(AMSgvolume* mvol){
  // L0 cupolas (identical shapes for top and bottom)
  char name[5];
  geant par[1]={0};
  geant coo[3]={0,0,0};
  geant angle=atan(1.)/2.; // 22.5 deg
  number nrmTop[3][3]={{ cos(angle),-sin(angle),0},{ sin(angle),cos(angle),0},{0,0, 1}};
  number nrmBot[3][3]={{-cos(angle),-sin(angle),0},{-sin(angle),cos(angle),0},{0,0,-1}};
  // Bottom matrix needs to be a true rotation matrix, not a reflection one with det=-1
  // For this, one need to not only flip Z, but also X or Y, simulating also 180 rotation
  // One can multiply top matrix to {{-1,0,0},{0,1,0},{0,0,-1}}

  // Top cupola
  coo[2]=0.5*TkDBc::Head->_sup_hc_w[6]+
             TkDBc::Head->_sup_hc_skin_w[6]+
             TkDBc::Head->_sup_qlcs_w+
           2*TkDBc::Head->_sup_qlcs_skin_w;
  std::strcpy(name, "CPLT");
  mvol->add(new AMSgvolume("CARBON",1,name,"CUPOLA",par,1,coo,nrmTop,"ONLY",1,8,1)); // M46J

  // Bottom cupola
  coo[2]*=-1;
  std::strcpy(name, "CPLB");
  mvol->add(new AMSgvolume("CARBON",1,name,"CUPOLA",par,1,coo,nrmBot,"ONLY",1,8,1)); // M46J
}


#else
//==================-----------------============================

#include "typedefs.h"
#include "node.h"
#include "snode.h"
#include "amsdbc.h"
#include "gmat.h"
#include "extC.h"
#include <stdlib.h>
#include "gvolume.h"
#include "amsgobj.h"

#include "commons.h"
#include "tkdbc.h"
extern "C" void mtx_(geant nrm[][3],geant vect[]);
extern "C" void mtx2_(number nrm[][3],geant  xnrm[][3]);
#define MTX mtx_
#define MTX2 mtx2_
namespace amsgeom{
extern void tkgeom02(AMSgvolume &);
extern void tkgeom02d(AMSgvolume &);
extern void magnetgeom(AMSgvolume &);
extern void magnetgeom02(AMSgvolume &);
extern void ext1structure02(AMSgvolume &);
extern void magnetgeom02o(AMSgvolume &);
extern void magnetgeom02Test(AMSgvolume &);
extern void tofgeom02(AMSgvolume &);
extern void antigeom02(AMSgvolume &);
extern void antigeom002(AMSgvolume &);
extern void ext2structure(AMSgvolume &);
#ifdef __G4AMS__
extern void antigeom02g4(AMSgvolume &);
extern void testg4geom(AMSgvolume &);
#endif
extern void richgeom02(AMSgvolume &,float zshift=0);
extern void ecalgeom02(AMSgvolume &);
extern void trdgeom02(AMSgvolume &, float zshift=0);
//extern void srdgeom02(AMSgvolume &);
extern void Put_rad(AMSgvolume *,integer);
extern void Put_pmt(AMSgvolume *,integer);
};
using namespace amsgeom;

void amsgeom::tkgeom02(AMSgvolume & mother){
  TKDBc::read();





  AMSID amsid;
  geant xx[3]={0.9,0.2,0.2};
  geant  xnrm[3][3];
  geant par[6]={0.,0.,0.,0.,0.,0.};
  number nrm[3][3]={1.,0.,0.,0.,1.,0.,0.,0.,1.};
  number inrm[3][3];
  char name[5];
  geant coo[3]={0.,0.,0.};

  integer gid=0;
  integer nrot=501; // Temporary arbitary assignment
  //static AMSgvolume mother(0,0,AMSDBc::ams_name,"BOX",par,3,coo,nrm,"ONLY",
  //                         0,gid);  // temporary a dummy volume
  AMSNode * cur;
  AMSgvolume * dau;
  int i;
  int nhalfL=0; 
  for ( i=0;i<TKDBc::nlay();i++){
    std::ostrstream ost(name,sizeof(name));
    ost << "STK"<<i+1<<ends;
    coo[0]=TKDBc::xposl(i);
    coo[1]=TKDBc::yposl(i);
    coo[2]=TKDBc::zposl(i);
    nrm[0][0]=TKDBc::nrml(0,0,i);
    nrm[0][1]=TKDBc::nrml(0,1,i); 
    nrm[0][2]=TKDBc::nrml(0,2,i); 
    nrm[1][0]=TKDBc::nrml(1,0,i); 
    nrm[1][1]=TKDBc::nrml(1,1,i); 
    nrm[1][2]=TKDBc::nrml(1,2,i); 
    nrm[2][0]=TKDBc::nrml(2,0,i); 
    nrm[2][1]=TKDBc::nrml(2,1,i); 
    nrm[2][2]=TKDBc::nrml(2,2,i); 
    int ii;
    gid=i+1;
    integer status=1;
    integer rgid;
    if(TKDBc::update())TKDBc::SetLayer(i+1,status,coo,nrm,gid);
    else               TKDBc::GetLayer(i+1,status,coo,nrm,rgid);
#ifdef __AMSDEBUG__
    if(i==2 && AMSgvolume::debug){
      MTX(xnrm,xx);
      int a1,a2;
      for( a1=0;a1<3;a1++)for( a2=0;a2<3;a2++)nrm[a2][a1]=xnrm[a1][a2];
    }
#endif
    int npar=5;
    if(npar==5){
      for ( ii=0;ii<npar;ii++)par[ii]=TKDBc::layd(i,ii);
      dau=(AMSgvolume*)mother.add(new AMSgvolume(
						 "VACUUM",nrot++,name,"CONE",par,npar,coo,nrm, "ONLY",0,gid,1));
      //      cout <<" layer "<<i<<" "<<nrot<<endl;
    }
    else{
      par[0]=TKDBc::layd(i,1);
      par[1]=TKDBc::layd(i,2);
      par[2]=TKDBc::layd(i,0);
      dau=(AMSgvolume*)mother.add(new AMSgvolume(
						 "VACUUM",nrot++,name,"TUBE",par,npar,coo,nrm, "ONLY",0,gid,1));
      //      cout <<" layer "<<i<<" "<<nrot<<endl;
    }
    int j;
    for (j=0;j<TKDBc::nlad(i+1);j++){
      int k;
      AMSgvolume * lad[2];
      for(k=0;k<2;k++){
	ost.clear();
	ost.seekp(0);
	ost << "L" << i+1<<(j+1)*2+k<<ends;
	double pairx=0;
	if(k==0){
	  par[0]=TKDBc::ssize_inactive(i,0)*TKDBc::nhalf(i+1,j+1)/2.;
	  pairx=TKDBc::ssize_inactive(i,0)*
	    (TKDBc::nsen(i+1,j+1)-TKDBc::nhalf(i+1,j+1))/2.;
        }
	else{
	  par[0]=TKDBc::ssize_inactive(i,0)*
	    (TKDBc::nsen(i+1,j+1)-TKDBc::nhalf(i+1,j+1))/2.;
	  pairx=TKDBc::ssize_inactive(i,0)*TKDBc::nhalf(i+1,j+1)/2.;
	}
	par[1]=TKDBc::ssize_inactive(i,1)/2;
	par[2]=TKDBc::silicon_z(i)/2;
	coo[0]=(2*k-1)*(TKDBc::ssize_inactive(i,0)*TKDBc::nsen(i+1,j+1)/2+
			TKDBc::halfldist(i)-par[0]);
	if(pairx==0){
	  //coo[0]+=-(2*k-1)*TKDBc::halfldist(i);
	  //cout <<"  Coo[0] was changed "<<" "<<i<<" "<<coo[0]<<endl;         
	}
	coo[1]=(TKDBc::nlad(i+1)-j)*TKDBc::c2c(i)-
	  (TKDBc::nlad(i+1)+1)*TKDBc::c2c(i)/2.;
	//coo[1]+=(2*k-1)*0.008;
        if(coo[1]<0)coo[1]+=-TKDBc::c2cgap(i)/2;
        else coo[1]+=TKDBc::c2cgap(i)/2;
	coo[2]=TKDBc::zpos(i);
	VZERO(nrm,9*sizeof(nrm[0][0])/4);
	if(k==0){
          nrm[0][0]=1;
          nrm[1][1]=1;
          nrm[2][2]=1;
	}
	else{
          nrm[0][0]=-1;
          nrm[1][1]=-1;
          nrm[2][2]= 1;
	}
#ifdef __AMSDEBUG__
	if(TKDBc::nsen(i+1,j+1)==TKDBc::nhalf(i+1,j+1) ||
	   TKDBc::nhalf(i+1,j+1)==0){
          if(par[0]!=0){
	    cout << i+1<<" "<<j+1<<" "<<coo[0]<<" "<<coo[1]<<" "<<coo[2]<<" "<<nrm[0][0]<<endl;
          }
	}
#endif
	//
	//  Ladder 
	//
        gid=i+1+10*(j+1)+100000;
        integer status=1;
        int rgid;
        if(!par[0]>0){
          status=0;
        }
        //cout <<"Lad "<<i<<" "<<j<<" "<<k<<" "<< coo[0]<<" "<<coo[1]<<" "<<coo[2]<<" "<<par[0]<<" "<<par[1]<<" "<<par[2]<<endl;
        if(TKDBc::update())TKDBc::SetLadder(i,j,k,status,coo,nrm,gid);
        else               TKDBc::GetLadder(i,j,k,status,coo,nrm,rgid);
        if(TKDBc::update()){
	  cout <<"Lad "<<i<<" "<<j<<" "<<k<<" "<< coo[0]<<" "<<coo[1]<<" "<<coo[2]<<" "<<par[0]<<" "<<par[1]<<" "<<par[2]<<endl;
        }
        //        if(i==5){
        //          cout <<"Lad "<<j<<" "<<k<<" "<< coo[0]<<" "<<coo[1]<<" "<<coo[2]<<endl;
        //        }
        if(par[0]>0)lad[k]=(AMSgvolume*)dau->add(new AMSgvolume(
								"NONACTIVE_SILICON",nrot++,name,"BOX",par,3,coo,nrm,"ONLY",1,gid,1));
        else lad[k]=0; 
        if(lad[k]){
	  ost.clear();
	  ost.seekp(0);
	  ost << (k==0?"ELL":"ELR") << i+1<<ends;
	  par[0]=TKDBc::zelec(i,1)/2.;
	  par[1]=TKDBc::c2c(i)/2.;
	  //        par[2]=TKDBc::zelec(i,0)/2;
	  par[2]=(dau->getpar(0)- TKDBc::zelec(i,2))/2;
	  coo[0]=lad[k]->getcoo(0)+(2*k-1)*(lad[k]->getpar(0)+par[0]);
	  coo[1]=(TKDBc::nlad(i+1)-j)*TKDBc::c2c(i)-
	    (TKDBc::nlad(i+1)+1)*TKDBc::c2c(i)/2.;
       if(coo[1]<0)coo[1]+=-TKDBc::c2cgap(i)/2;
        else coo[1]+=TKDBc::c2cgap(i)/2;

	  coo[2]=TKDBc::zelec(i,2)+par[2];
	  VZERO(nrm,9*sizeof(nrm[0][0])/4);
	  nrm[0][0]=1;
	  nrm[1][1]=1;
	  nrm[2][2]=1;
	  gid=i+1+10*(j+1);
	  cur=dau->add(new AMSgvolume(
				      "ELECTRONICS",nrot++,name,"BOX",par,3,coo,nrm,"ONLY",1,gid,1));
        }



      }

    
      // Now Sensors

      for ( k=0;k<TKDBc::nhalf(i+1,j+1);k++){
	ost.clear();
	ost.seekp(0);
	ost << "S" << i+1<<(j+1)*2<<ends;
	par[0]=TKDBc::ssize_active(i,0)/2;
	par[1]=TKDBc::ssize_active(i,1)/2;
	par[2]=TKDBc::silicon_z(i)/2;
	coo[0]=-TKDBc::ssize_inactive(i,0)/2.+
	  (2*k+2-TKDBc::nhalf(i+1,j+1))*TKDBc::ssize_inactive(i,0)/2.;
	coo[1]=0;
	coo[2]=0;
	VZERO(nrm,9*sizeof(nrm[0][0])/4);
	nrm[0][0]=1;
	nrm[1][1]=1;
	nrm[2][2]=1;
#ifdef __AMSDEBUG__
	if(i==2 && AMSgvolume::debug){
          MTX(xnrm,xx);
          int a1,a2;
          for( a1=0;a1<3;a1++)for( a2=0;a2<3;a2++)nrm[a2][a1]=xnrm[a1][a2];
	}
#endif
	gid=i+1+10*(j+1)+1000*(k+1);
	if(TKDBc::activeladdshuttle(i+1,j+1,0)){  
          if(k==0)nhalfL++;
          integer status=1;
          if(TKDBc::update())TKDBc::SetSensor(i,j,k,status,coo,nrm,gid);
          else               TKDBc::GetSensor(i,j,k,status,coo,nrm);
          if(status){
	    cur=lad[0]->add(new AMSgvolume(
					   "ACTIVE_SILICON",nrot++,name,"BOX",par,3,coo,nrm,"ONLY",1,gid,1));
	    //          cout <<"add volume "<<name<<" "<<gid<<endl;
	    //          cout <<" to "<<*(lad[0]);
          }
	}
      }

      for ( k=TKDBc::nhalf(i+1,j+1);k<TKDBc::nsen(i+1,j+1);k++){
	ost.clear();
        ost.seekp(0);
        ost << "S" << i+1<<(j+1)*2+1<<ends;
        par[0]=TKDBc::ssize_active(i,0)/2;
        par[1]=TKDBc::ssize_active(i,1)/2;
        par[2]=TKDBc::silicon_z(i)/2;
        coo[0]=-TKDBc::ssize_inactive(i,0)/2.+
	  (2*(k-TKDBc::nhalf(i+1,j+1))+2+
	   TKDBc::nhalf(i+1,j+1)-TKDBc::nsen(i+1,j+1))*
	  TKDBc::ssize_inactive(i,0)/2.;
        coo[1]=0;
        coo[2]=0;
        VZERO(nrm,9*sizeof(nrm[0][0])/4);
        nrm[0][0]=1;
        nrm[1][1]=1;
        nrm[2][2]=1;
#ifdef __AMSDEBUG__
	if(i==2 && AMSgvolume::debug){
          MTX(xnrm,xx);
          int a1,a2;
          for( a1=0;a1<3;a1++)for( a2=0;a2<3;a2++)nrm[a2][a1]=xnrm[a1][a2];
	}
#endif
        gid=i+1+10*(j+1)+1000*(k+1);
        if(TKDBc::activeladdshuttle(i+1,j+1,1)){  
	  if(k==TKDBc::nhalf(i+1,j+1))nhalfL++;
	  integer status=1;
	  if(TKDBc::update())TKDBc::SetSensor(i,j,k,status,coo,nrm,gid);
	  else               TKDBc::GetSensor(i,j,k,status,coo,nrm);
	  if(status){
	    cur=lad[1]->add(new AMSgvolume(
					   "ACTIVE_SILICON",nrot++,name,"BOX",par,3,coo,nrm,"ONLY",1,gid,1));
	    //          cout <<"add volume "<<name<<" "<<gid<<endl;
	    //          cout <<" to "<<*(lad[1]);
	  }
        }
      }


    }
    // Now Support foam
 
    for ( j=0;j<TKDBc::nlad(i+1);j++){
      ost.clear();
      ost.seekp(0);
      ost << "FOA" << i+1<<ends;
      par[0]=TKDBc::ssize_inactive(i,0)*TKDBc::nsen(i+1,j+1)/2.;
      par[1]=TKDBc::c2c(i)/2;
      par[2]=(TKDBc::support_foam_w(i))/2;    
      coo[0]=0;
      coo[1]=(TKDBc::nlad(i+1)-j)*TKDBc::c2c(i)-(TKDBc::nlad(i+1)+1)*TKDBc::c2c(i)/2.;
//       if(coo[1]<0)coo[1]+=-TKDBc::c2cgap(i)/2;
//        else coo[1]+=TKDBc::c2cgap(i)/2;

      coo[2]=TKDBc::zpos(i)-TKDBc::silicon_z(i)/2.-par[2]-TKDBc::support_foam_tol(i);
      VZERO(nrm,9*sizeof(nrm[0][0])/4);
      nrm[0][0]=1;
      nrm[1][1]=1;
      nrm[2][2]=1;
      gid=i+1+10*(j+1);
      cur=dau->add(new AMSgvolume(
				  "Tr_Foam",nrot++,name,"BOX",par,3,coo,nrm,"ONLY",1,gid,1));

    }

    // Now Honecomb Planes

    ost.clear();
    ost.seekp(0);
    ost << "PLA" << i+1<<ends;
    par[0]=0;
    par[1]=TKDBc::support_hc_r(i);
    par[2]=TKDBc::support_hc_w(i)/2;
    coo[0]=0;
    coo[1]=0;
    coo[2]=TKDBc::zpos(i)-TKDBc::silicon_z(i)/2.+TKDBc::support_hc_z(i);
    VZERO(nrm,9*sizeof(nrm[0][0])/4);
    nrm[0][0]=1;
    nrm[1][1]=1;
    nrm[2][2]=1;
    gid=i+1;
    cur=dau->add(new AMSgvolume(
				"Tr_Honeycomb",nrot++,name,"TUBE",par,3,coo,nrm,"ONLY",1,gid,1));



  }
  cout <<"<---- TKGeom-I-"<<nhalfL<<" Active halfladders initialized"<<endl<<endl;

}

#endif
