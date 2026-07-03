//  $Id: commonsi.h,v 1.12 2016/06/08 13:13:09 bshan Exp $
//  Author V. Choutko 24-may-1996
//
//  To developpers:
//  All new vars to cblocks defs: only at the very end !!!
///

#ifndef __AMSCOMMONSI__
#define __AMSCOMMONSI__
#include "typedefs.h"
// GEANT part
#ifdef __ALPHA__
#define DECFortran
#else
#define mipsFortran
#endif
#include "cfortran.h"
#include <setjmp.h>
#include <cstdio>


class AMSDATADIR_DEF{
public:
integer amsdlength;
integer amsdblength;
char amsdatadir[128];
char amsdatabase[128];
char fname[200];
};
#define AMSDATADIR COMMON_BLOCK(AMSDATADIR,amsdatadir)
COMMON_BLOCK_DEF(AMSDATADIR_DEF,AMSDATADIR);


class AMSCommonsI{
public:
private:
 static integer _Count;
 static char _version[6];
 static uinteger _build;
 static uinteger _buildtime;
 static uinteger _os;
 static integer  _g4version;
 static char _osname[255];
 static char _osversion[255];
 static char _tmp_log_path[L_tmpnam];
 static uinteger _MaxMem;
 static float _mips;
 static pid_t _pid;
 static bool _remote;
 int getCPUInfo(int os_num, float& freq, string& model);
public:
  static int pri;
  static int prierr;
static jmp_buf  AB_buf;
static int  AB_catch;
#pragma omp threadprivate (AB_buf,AB_catch)
 AMSCommonsI();
 void init();
 static integer getg4version(int full){return full?_g4version:_g4version%65536;}
 static void setg4version(int g4version,int version){_g4version=g4version+version*(1<<16);}
 static const char * getversion(){return _version;}
 static integer getbuildno()  {return _build;}
 static uinteger getbuildtime(){return _buildtime;}
 static char * getbuildctime(){ time_t t=_buildtime;return ctime(&t);}
 static integer getmips()  {return integer(_mips);}
 static integer getpid()  {return integer(_pid);}
 static uinteger MaxMem(){return _MaxMem;}
 static void setremote(bool remote){_remote=remote;}
 static bool remote(){return _remote;}
 static integer getosno()  {return _os;}
 static char* getosname()  {return _osname;}
 static char* getosversion(){return _osversion;}
 static char* gettmplogpath(){return _tmp_log_path;}
};
static AMSCommonsI cmnI;







#endif
