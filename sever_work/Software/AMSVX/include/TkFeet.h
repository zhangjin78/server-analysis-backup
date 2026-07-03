#ifndef tkfeet_h
#define tkfeet_h
#include <cstdlib>
#include "TkObject.h"

class TkLadder;
class TkDBc;
class AMSPoint ;

//////////////////////////////////////////////////////////////////////////
///
///
///\class TkFeet
///\brief An AMS Tracker class for the ladder feet specific information
///\ingroup tkdbc
///
/// An AMS Tracker class used to get information about tracker feet
/// 
///
/// author P.Zuccon -- MIT 01/02/2016 
///
///\date  2008/02/1 PZ  First version
///
///$Revision: 1.2 $
///
//////////////////////////////////////////////////////////////////////////
class TkFeet{
private:
typedef  enum ltype{
    l7=7,
    l9=9,
    l10=10,
    l11=11,
    l12=12,
    l13=13,
    l14=14,
    l15=15,
    l14s=16
  } ltype;
  static int nfeet[17];
  static int off1[17];
  static double d0[17];
  static double dl[17];
  static double dfix;
  static double x14s[7];

  TkDBc* db;
  TkLadder * lad;
  ltype type;
  double getX1(ltype tt);
  int _tkid;
  void SetTkid(int tkid);
public:
  //                      X=0     X=1       X=2          X=3
  // Chan 640  ---------------------------------------------- 
  //           | H   |     #       #         #            # |Y=1
  //           | Y   |                                      |
  //           | B   |     #       #         #            # |Y=0
  // Chan 0    ----------------------------------------------

  TkFeet();
  virtual ~TkFeet(){db=0; lad=0;};
 

  /// Returns the number of feet along  X on a ladder identified by TkId 
  int getNfeet(int tkid){ SetTkid(tkid); return nfeet[type];}
  /// Returns the X coo (ladder frame of reference) for the i-th foot  along X
  double getXloc(int tkid, int ifoot);
  /// Returns the Y coo of a foot in a ladder, 0 closer to ladder origin, 1 farther from ladder origin
  double getYloc(int tkid, int side);

  /// Returns the global coo of the X i-th foot  on the 0 or 1 Y side
  AMSPoint getGlobalA(int tkid, int ifoot, int iside);

  /// Returns the global coo(MCalignment) of the X i-th foot  on the 0 or 1 Y side
  AMSPoint getGlobalT(int tkid, int ifoot, int iside);
  ClassDef(TkFeet,1);

};


#endif
