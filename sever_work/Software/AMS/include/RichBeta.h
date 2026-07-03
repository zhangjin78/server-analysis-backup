#ifndef _RICHBETA_
#define _RICHBETA_
#include "TString.h"
#include "RichTools.h"

class RichRingR;

//////////////////////////////////////////////////////////
// Original uniformity correction, split in NaF and Agl //
//////////////////////////////////////////////////////////

class RichBetaUniformityCorrection{
private:
  RichBetaUniformityCorrection(){_agl=_naf=0;_latest[0]=_latest[1]=_latest[2]=_latest[3];};
  ~RichBetaUniformityCorrection(){if(_agl) delete _agl;if(_naf) delete _naf;};
public:
  static RichBetaUniformityCorrection* _head;
  static RichBetaUniformityCorrection *getHead(){return _head;}
  static bool Init(TString="");

  GeomHashEnsemble *_agl;
  GeomHashEnsemble *_naf;

  float _latest[4];

  // Accessors to the correction
  float getCorrection(RichRingR *ring);
  float getCorrection(float *); 
  float getCorrection(float x,float y,float vx,float vy);
  // Accessors to several quality parameters
  float getWidth(RichRingR *ring);
  float getWidth(float *); 
  float getWidth(float x,float y,float vx,float vy);
  // Accessors to several quality parameters
  float getRms(RichRingR *ring);
  float getRms(float *); 
  float getRms(float x,float y,float vx,float vy);
};

//////////////////////////////////////////////////////////
// V2 splits in data and MC instead than in NaF and Agl //
//////////////////////////////////////////////////////////

class RichBetaUniformityCorrectionV2{
private:
  RichBetaUniformityCorrectionV2(){_data=0;_latest[0]=_latest[1]=_latest[2]=_latest[3];};
  ~RichBetaUniformityCorrectionV2(){if(_data) delete _data;};
public:
  static RichBetaUniformityCorrectionV2* _head;
  static RichBetaUniformityCorrectionV2 *getHead(){return _head;}
  static bool Init(TString="",bool data=true);

  GeomHashEnsemble *_data;

  float _latest[4];

  // Accessors to the correction
  float getCorrection(RichRingR *ring);
  float getCorrection(float *); 
  float getCorrection(float x,float y,float vx,float vy);
  // Accessors to several quality parameters
  float getWidth(RichRingR *ring);
  float getWidth(float *); 
  float getWidth(float x,float y,float vx,float vy);
  // Accessors to several quality parameters
  float getRms(RichRingR *ring);
  float getRms(float *); 
  float getRms(float x,float y,float vx,float vy);
};


//////////////////////////////////////////////////////////////////////
// Correction of residual dependences with the number of reflected  //
//////////////////////////////////////////////////////////////////////

class RichBetaAdditionalCorrections{
public:
  enum Rad {AGL=0,NAF=1};
  
  static GAMModel model[2][2];
  static RichBetaAdditionalCorrections* head;

  static RichBetaAdditionalCorrections* Init(TString filename_data,TString filename_mc);

  float getCorrection(RichRingR *ring,float run,int iMc,float z); // Interface with richring
  
  static float corrected_beta(float beta,
			      Rad radiator, // 0= AGL
			      float run,
			      float charge, 
			      float x_rad, 
			      float y_rad,
			      float theta_rad,
			      float phi_rad,
			      float nreflected,
			      float nhits,
			      int mc=0);

  static RichBetaAdditionalCorrections* getHead(){return RichBetaAdditionalCorrections::head;}

};



#endif
