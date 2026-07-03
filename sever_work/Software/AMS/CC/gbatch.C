//  $Id: gbatch.C,v 1.173 2017/08/04 16:17:07 choutko Exp $
#ifdef JUQUEEND
#ifndef JUQUEEN
#define JUQUEEN
#endif
#include <mpi.h>
#endif
#include "MPIEmulator.h"
#include <iostream>
#include <signal.h>
#include <unistd.h> 
#include "upool.h"
#include "apool.h"
#include "status.h"
#include "commons.h"
#include "geantnamespace.h"
#include "producer.h"
#ifdef __DARWIN__
#include <malloc/malloc.h>
#else
#include <malloc.h>
#endif
#include "event.h"
#include <fenv.h>
#ifdef _OPENMP
#include <omp.h>
#endif
#ifdef _PGTRACK_
#include "TrRecon.h"
#endif
#ifdef __AMSVMC__
extern amsvmc_MCApplication*  appl = new amsvmc_MCApplication("AMSVMC", "AMS VirtualMC application");
#endif
#ifdef _G4AMS_
#include "ams_g4exception.h"
#endif
const int NWGEAN=15000000;
const int NWPAW=1300000;
struct PAWC_DEF{
float q[NWPAW];
};
struct GCBANK_DEF{
float q[NWGEAN];
};
//
#define GCBANK COMMON_BLOCK(GCBANK,gcbank)
COMMON_BLOCK_DEF(GCBANK_DEF,GCBANK);
GCBANK_DEF GCBANK;

#define PAWC COMMON_BLOCK(PAWC,pawc)
COMMON_BLOCK_DEF(PAWC_DEF,PAWC);
PAWC_DEF PAWC;
void my_unexpected () {
if (!std::uncaught_exception())
std::cerr << "my_unexpected called\n";
}
void (handler)(int);
 namespace glconst{
  integer cpul=1;
 }
MPIEmulator mpi;
int main(int argc, char * argv[] ){
    std::set_unexpected (my_unexpected);
//for (char *pchar=0; argc>1 &&(pchar=argv[1],*pchar=='-'); (argv++,argc--)){
//  pchar++;
//  switch (*pchar){
//   case 'v':    //version , return buildno
//   return AMSCommonsI::getbuildno();
//  } 
//}

      using namespace gams;
#if defined(__LINUX24__) || defined(__DARWIN__) || defined(__PPC64)      
#else
     feenableexcept(FE_DIVBYZERO |  FE_INVALID | FE_OVERFLOW );
#endif
#ifdef JUQUEEN
#ifdef __G4AMS__
    // (void)*signal(SIGABRT,handler);
    //  (void)*signal(SIGSEGV,handler);
#endif
#endif
      //   *signal(SIGFPE, handler);
     (void)*signal(SIGCONT, handler);
     (void)*signal(SIGTERM, handler);
     (void)*signal(SIGXCPU,handler);
     (void)*signal(SIGINT, handler);
     (void)*signal(SIGQUIT, handler);
     (void)*signal(SIGUSR1, handler);
     (void)*signal(SIGUSR2, handler);
     (void)*signal(SIGHUP, handler);
     (void)*signal(SIGTSTP, handler);
    (void)*signal(SIGURG, handler);
     (void)*signal(SIGTTIN, handler);
     (void)*signal(SIGTTOU, handler);
    if (mpi.MPI_Init(argc, argv) != 0) {
        cerr << "MPIEmulator::MPI_Init returned error, exiting..." << endl;
        return 1;
    }
    if(mpi.getMPIStatus()==1){
     (void)*signal(SIGABRT,handler);
      (void)*signal(SIGSEGV,handler);
      cout<<"MPI-I-SIGABRTSEGVSet "<<endl;
    }

    GZEBRA(NWGEAN);
    HLIMIT(-NWPAW);
try{
#ifdef JUQUEEND
    MPI_Init(NULL,NULL);
    //MPI::Init(argc,argv);
    //         // Get the number of processes
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    // Get the rank of the process
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    // Get the name of the processor
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    int name_len;
    MPI_Get_processor_name(processor_name, &name_len);

    // Print off a hello world message
 //   printf("Hello world from processor %s, rank %d"
 //           " out of %d processors\n",
 //           processor_name, world_rank, world_size);
#endif
    UGINIT(argc,argv);
#ifndef __AMSVMC__
#ifdef __G4AMS__
    if(MISCFFKEY.G4On)g4ams::G4RUN();
    else if(MISCFFKEY.G3On)GRUN();
#else
    GRUN();
#endif
#endif

#ifdef __AMSVMC__
    std::cout<<"DEBUG: in gbatch.C, Before VMCRUN()"<<std::endl;
    amsvmc::VMCRUN(appl);
#endif
} 
catch (amsglobalerror & a){
 cerr<<a.getmessage()<< endl;
#ifdef __CORBA__
  AMSClientError ab((const char*)a.getmessage(),DPS::Client::CInAbort);
 if(AMSProducer::gethead()){
  cerr<<"setting errror"<< endl;
  AMSProducer::gethead()->Error()=ab;
 }
#endif
    UGLAST(a.getmessage());
    return 1;
}
catch (std::bad_alloc aba){
 cerr<<"  gbatch-S-std::bad_alloc "<<endl;

  #ifdef __G4AMS__
   cerr<<"Preparing for OPool Released"<<endl;
   OPool.ReleaseLastResort();
   cerr<<"OPool Released a"<<endl;
  #endif

#ifdef __CORBA__
  AMSClientError ab("NoMemoryAvailable",DPS::Client::CInAbort);
 if(AMSProducer::gethead()){
  cerr<<"setting errror"<< endl;
  AMSProducer::gethead()->Error()=ab;
 }
#endif

cerr <<"gbatch-catch-F-NoMemoryAvailable "<<endl;
  #ifdef __G4AMS__
    UGLAST("catch-F-NoMemoryAvailable ");
    
    exit(1);
    return 1;
  #endif
try{
    if(AMSEvent::gethead())AMSEvent::gethead()->Recovery(true);
}
catch (std::bad_alloc abab){
}
    UGLAST("catch-F-NoMemoryAvailable ");
    exit(1);
    return 1;
}
#ifdef _G4AMS_
catch (ams_g4exception &){
#ifdef __CORBA__
  AMSClientError ab("G4ExceptionRaised",DPS::Client::CInAbort);
 if(AMSProducer::gethead()){
  cerr<<"setting errror"<< endl;
  AMSProducer::gethead()->Error()=ab;
 }
#endif
    cerr <<"gbatch-catch-F-G4ExceptionRaised "<<endl;
try{
    if(AMSEvent::gethead())AMSEvent::gethead()->Recovery(true);
}
catch (std::bad_alloc abab){
}
    UGLAST("catch-F-G4ExceptionRaised");
    exit(1);
    return 1;
}
#endif
#ifdef __CORBA__
catch (AMSClientError & ab){
 cerr<<ab.getMessage()<<" 1"<<endl;
 if(AMSProducer::gethead()){
  AMSProducer::gethead()->Error()=ab;
 }
}
#endif
try{
#ifdef __AMSVMC__
  amsvmc::VMCLAST();
#endif

    cout <<"gbatch-I-NormalTerminationStart "<<endl;
    UGLAST();
}
catch (amsglobalerror & a){
 cerr<<a.getmessage()<< " in UGLAST" <<endl;
 return 1;
}    
return 0;
}
void (handler)(int sig){
  using namespace glconst;
#ifdef _OPENMP
  int nthr=0;
#endif
  static int tme=10;
  int tm1=10;
  int sl=0;
  static bool sigabrt=false;
  static bool sigexit=false;
  switch(sig){
/*  case SIGABRT:
    cerr <<" ABORT Detected "<<AMSCommonsI::AB_catch<<" "<<endl;
    GCFLAG.IEORUN=1;
    GCFLAG.IEOTRI=1;
#ifndef __DARWIN__
    mallopt(M_CHECK_ACTION,1);
#endif    
    if(AMSCommonsI::AB_catch>=0){
      AMSCommonsI::AB_catch=1;
      cout <<"  JUMP attempted "<<endl;
      siglongjmp(AMSCommonsI::AB_buf,0);
      
    }
    exit(1);
    break;
*/
      case SIGFPE:
    cerr <<feclearexcept(FE_ALL_EXCEPT)<<" FPE intercepted "<<endl;
    break;
  case SIGXCPU:
    if (strcmp(AMSCommonsI::getosname(), "CNK BGQ") == 0)
        cerr <<" Job Cpu limit exceeded"<<endl;
#pragma omp master 
    if(cpul){
      cerr <<" Job Cpu limit exceeded"<<endl;
      cpul=0;
      GCFLAG.IEORUN=1;
      GCFLAG.IEOTRI=1;
      AMSStatus::setmode(0);

#ifdef __G4AMS__
	cerr<<"Preparing for OPool Released"<<endl;
	OPool.ReleaseLastResort();
	cerr<<"OPool Released b"<<endl;
	
#ifdef __CORBA__
	AMSClientError ab("Job Cpu limit exceeded",DPS::Client::CInAbort);
	if(AMSProducer::gethead()){
	  cerr<<"setting errror"<< endl;
	  AMSProducer::gethead()->Error()=ab;
	}
#endif
	cerr <<"gbatch-Job Cpu limit exceeded "<<endl;
try{
	if(AMSEvent::gethead())AMSEvent::gethead()->Recovery(true);
}
catch (std::bad_alloc abab){
}
	gams::UGLAST("SIGXCPU");
        exit(1);
#endif
    }
    break;
   case SIGINT:
#ifdef __G4AMS__
   if(mpi.RunMode<1){
       cout <<"  SIGINT intercepted RunModeZero Do Nothing "<<mpi.RunMode<<endl;
       signal(SIGINT,SIG_DFL);
       break;
   }
#endif
cerr <<" SIGINT intercepted sleeping  "<<tme<<" sec "<<endl;
  GCFLAG.IEORUN=1;
  GCFLAG.IEOTRI=1;
  AMSFFKEY.CpuLimit=10;
#ifdef G4MULTITHREADED
    G4AllocatorPool::gThreshold=0;
#endif 
    tm1=tme;
    tme=12000;
    if(mpi.RunMode>2)tm1=tme;
    if(sigabrt)tm1=tme;
    else sigabrt=true;
           if(sigexit){
              sigexit=false; 
              mpi.MPI_Finalize();
           }
        else{
        cerr<<"  SIGINT will sleep "<<tm1-10<<endl;
            sleep(tm1-10);
         }
    G4FFKEY.SigTerm=2;
#ifndef JUQUEEN
    if(mpi.RunMode>=2){
           break;
    }
#endif
   case SIGTERM: 
    cerr <<" SIGTERM intercepted"<<endl;
//#pragma omp master
{
  GCFLAG.IEORUN=1;
  GCFLAG.IEOTRI=1;
  AMSFFKEY.CpuLimit=10;
#ifdef G4MULTITHREADED
    G4AllocatorPool::gThreshold=0;
#endif 
   if(mpi.RunMode>2)break; 
#ifdef _PGTRACK_
  TrRecon::SigTERM=1;
#endif
if(G4FFKEY.SigTerm && (!AMSJob::gethead()->isProduction() || G4FFKEY.SigTerm>1)){
//#ifdef __G4AMS__
//#ifdef __CORBA__
lasthope:
//#endif
//
   G4FFKEY.SigTerm=1;
  cerr<<"Preparing for OPool Released"<<endl;
  OPool.ReleaseLastResort();
  cerr<<"OPool Released c"<<endl;
#ifdef __CORBA__
  AMSClientError ab("SIGTERM intercepted",DPS::Client::CInAbort);
  if(AMSProducer::gethead()){
    cerr<<"setting errror"<< endl;
    AMSProducer::gethead()->Error()=ab;
  }
#endif
  cerr <<"gbatch-SIGTERMSIMULATION "<<endl;
  //if(AMSEvent::gethead())AMSEvent::gethead()->Recovery(true);
  gams::UGLAST("SIGTERMSIM ",1);
//  special treatment is needed if sigsegv happens in exit(1) :  goto mpi_finalize
  sigexit=true;
  exit(1);
//#endif
}

 }
 break;
 case SIGSEGV:
 case SIGABRT:
if(mpi.getMPIStatus()==1){
    if(MISCFFKEY.BCast)mpi.bcast(true);
 if(sigabrt){
     mpi.Signalled++;
     cerr<<" Subsequent SIGABRT SIGSEGV will exit promptly "<<endl;
     pthread_exit(NULL);
 }
  sl=-mpi.Terminate()+300; 
 if(sl>=0){
     if(mpi.RunMode<1){
        cerr<<" SIGSEGV SIGABRT "<<endl;
        signal(SIGSEGV,SIG_DFL);
        signal(SIGABRT,SIG_DFL);
        break;
     }
     cerr<<" SIGSEGV SIGABRT will sleep "<<sl<<" sec"<<mpi.RunMode<<endl;
     sleep(sl); 
 }
}
 GCFLAG.IEORUN=1;
  GCFLAG.IEOTRI=1;
  AMSFFKEY.CpuLimit=10;
#ifdef G4MULTITHREADED
       G4AllocatorPool::gThreshold=0;
#endif
#ifdef _PGTRACK_
        TrRecon::SigTERM=2;
#endif
        if(sigabrt || mpi.RunMode>2)tme=20000;
        else {
            sigabrt=true;
        }
        if(sigexit){
            sigexit=false;
            mpi.MPI_Finalize();
        }
        else{
        cerr<<"  SIGSEGV SIGABRT will sleep "<<tme<<endl;
         sleep(tme);
        }
  cerr<<"Preparing for OPool Released"<<endl;
   OPool.ReleaseLastResort();
   cerr<<"OPool Released c"<<endl;

 gams::UGLAST("SIGTERMSIM ",1);
 sigexit=true; 
 exit(1);
   break;
  case SIGQUIT:
cerr <<" SIGQUIT intercepted"<<endl;
#ifdef G4MULTITHREADED
  cerr<<"Preparing for OPool Released"<<endl;
  OPool.ReleaseLastResort();
  cerr<<"OPool Released d"<<endl;
    GCFLAG.IEORUN=1;
    GCFLAG.IEOTRI=1;
    G4AllocatorPool::gThreshold=0;
#else
    cerr <<" Process suspended"<<endl;
  G4FFKEY.SigTerm=2;
    pause();
#endif
    break;
  case SIGCONT:
    cerr <<" Process resumed"<<endl;
    break;
  case SIGHUP:
#ifdef __CORBA__
    cout <<"got sighup "<<AMSEvent::get_thread_num()<<endl;
    if(AMSProducer::gethead() && AMSProducer::gethead()->Progressing()){
      cout <<" sending ... "<<endl;
      AMSProducer::gethead()->sendCurrentRunInfo(false);
    }
    else{
#ifdef G4MULTITHREADED
  cerr<<"Preparing for OPool Released"<<endl;
  OPool.ReleaseLastResort();
  cerr<<"OPool Released e"<<endl;
    GCFLAG.IEORUN=1;
    GCFLAG.IEOTRI=1;
    G4AllocatorPool::gThreshold=0;

#else
          goto lasthope;
#endif
    } 
    cout << " sighup sended "<<endl;
#else
    cout << " sighup sended "<<endl;
    GCFLAG.IEORUN=1;
    GCFLAG.IEOTRI=1;
#ifdef G4MULTITHREADED
  cerr<<"Preparing for OPool Released"<<endl;
  OPool.ReleaseLastResort();
  cerr<<"OPool Released f"<<endl;
  G4AllocatorPool::gThreshold=0; 
#else
          goto lasthope;
#endif
#endif
    break;
  case SIGUSR1:
    cerr<< "New Run Forced"<<endl;
    if(GCFLAG.IEORUN==0)GCFLAG.IEORUN=2;
    break;
  case SIGUSR2:
    cerr<< "New Ntuple Forced"<<endl;
    if(GCFLAG.ITEST>0)GCFLAG.ITEST=-GCFLAG.ITEST;
    break;
  case SIGTTOU:
#pragma omp master
    {
#ifdef _OPENMP
      nthr=omp_get_num_threads();
      if(nthr>1)nthr--;
      else MISCFFKEY.DynThreads=0;
      if(MISCFFKEY.NumThreads<0)MISCFFKEY.NumThreads=nthr;
      else if(MISCFFKEY.NumThreads>1)MISCFFKEY.NumThreads--;
      cerr<<" ThreadsNumberWillBeChangedTo "<<MISCFFKEY.NumThreads<<endl;
#endif
    }
    break;
  case SIGTTIN:
#pragma omp master
    {
#ifdef _OPENMP
    nthr=omp_get_num_threads();
    if(nthr<omp_get_num_procs())nthr++;
    else MISCFFKEY.DynThreads=1;
    if(MISCFFKEY.NumThreads<0)MISCFFKEY.NumThreads=nthr;
    else if(MISCFFKEY.NumThreads<omp_get_num_procs())MISCFFKEY.NumThreads++;
    cerr<<" ThreadsNumberWillBeChangedTo "<<MISCFFKEY.NumThreads<<endl;
#endif
    }
    break;
 case SIGURG:
  case SIGTSTP:
#ifdef _OPENMP
#pragma omp master
    {
      MISCFFKEY.NumThreads=-1;
      cerr<<" ThreadsNumberWillBeChangedTo "<<MISCFFKEY.NumThreads<<endl;
    }
#endif
    break;
  }
}
