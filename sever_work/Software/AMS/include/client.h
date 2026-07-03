//  $Id: client.h,v 1.37 2022/05/10 09:10:26 aegorov2 Exp $
#ifndef __AMSCLIENT__
#define __AMSCLIENT__
#include "typedefs.h"
#include "astring.h"
#include <list>
#include <fstream>
#ifndef __CORBA_LIGHT__
  #ifndef UID2DST
    #include <server-cpp-stubs.h>
  #else
    #include <serverrc-cpp-stubs.h>
  #endif
#endif
//#include <orb/orbitcpp_exception.h>
#include <strstream>

// 26.05.2016 - quick patch to avoid corba links in producer
#ifdef __CORBA_LIGHT__

#include <typeinfo>

template <typename T> class smart_ptr
{
protected:
	T*    _ptr;
	long* _count;
public:
	/*!!!!!explicit*/ smart_ptr<T>(T* p=0) throw(): _ptr(p) {
		if (p) {
			_count = new long;
			*_count = 1;
		}
		else
			_count = 0;
	};
	smart_ptr<T>(const smart_ptr<T>& p) throw() {
		assign(p);
	};
	virtual ~smart_ptr() throw() {
		dispose();
	};
public:
	void release() {
		if (_count) {
			delete _count;
			_count = 0;
		}
	};
public:
	T *ptr() { return _ptr; };
	smart_ptr<T>& operator =(const smart_ptr<T>& p) throw() {
		if (_ptr != p._ptr) dispose();
		assign(p);
		return *this;
	};
	operator T&() const throw() { return *_ptr; };
	T& operator*() const throw() { return *_ptr; };
	T& operator[](long id) const throw()  { return _ptr[id]; };
	T* operator->() const throw() { return _ptr; };
protected:
	void dispose() {
		if (_count && --*_count <= 0) {
			release();
			if (_ptr) {
				delete _ptr;
				_ptr = 0;
			}
		}
	};
	void assign(const smart_ptr<T>& p) {
		_ptr = p._ptr;
		if ((_count=p._count))
			++*_count;
	};
	void allocate(T* p) {
		if (_ptr != p) dispose();
		_ptr = p;
		if (p) {
			_count = new long;
			*_count = 1;
		}
		else _count = 0;
	};
};

namespace CORBA {
class string: public std::string {
public:
	operator const char *() const { return c_str(); };
	string& operator =(const char *s) { static_cast<std::string &>(*this)=s; return *this; };
//	operator const CORBA::string() { return c_str(); };
};

struct Object {
};

class Object_var: public smart_ptr<Object> {
public:
	Object_var(Object* p=0): smart_ptr<CORBA::Object>() { };
	Object_var(const Object_var& p): smart_ptr<CORBA::Object>(p) { };
	Object_var & operator =(Object *p) throw() { allocate(p); return (*this); };
};

struct ORB {
//		CORBA::Object_var string_to_object(char *ior) { Object_var res; return res; };
};
typedef ORB* ORB_ptr;

bool is_nil(Object_var obj) { return true; };
struct SystemException {
};
struct MARSHAL {
};
struct COMM_FAILURE {
};
struct TRANSIENT {
};
};	// CORBA

namespace DPS {

#define interface struct
#define sequence vector
#define raises throw
#define oneway
typedef bool boolean;
//!!!!!typedef CORBA::string string;
#define string CORBA::string 
typedef unsigned long time;

  struct DBProblem {
    string message;
  };
	
  interface Client{
    enum AccessType{Any,LessThan,Self,AnyButSelf,GreaterThan};
    enum RecordChange{Delete,Update,Create,ClearCreate};
    enum ErrorType{Info,Warning,Error,Severe};
    enum ClientStatus{Lost,Unknown,Submitted,Registered,Active,TimeOut,Killed};
    enum ClientStatusType{Permanent,OneRunOnly};
    enum ClientType{Generic,Server,Consumer,Producer,Monitor,Killer,DBServer,ODBServer,Standalone};
    enum ClientExiting{NOP,CInExit,SInExit,CInAbort,SInAbort,SInKill};
    enum HostStatus{NoResponse,InActive,LastClientFailed,OK};

    struct CID{
	    string HostName;
	    unsigned long uid;
	    unsigned long pid;
	    unsigned long ppid;
	    unsigned long threads;
	    long threads_change;
	    ClientType   Type;
	    ClientStatusType StatusType;
	    ClientExiting Status;
	    string Interface;
	    float Mips;
            unsigned long coid;
    };

    boolean sendId(CID &id,  float mips, time TimeOut) { return true; };      

    struct NominalClient{
	    unsigned long uid;
	    ClientType Type;
	    short MaxClients;
	    float CPUNeeded; 
	    short MemoryNeeded;
	    string WholeScriptPath;
	    string LogPath;
	    string SubmitCommand;
	    string HostName;
	    short LogInTheEnd;
    }; 

    struct ActiveRef{
	    string IOR;
		string Interface;
	    ClientType Type;
	    unsigned long uid;
    }; 
    typedef sequence<ActiveRef> ARS;

    struct ActiveClient{
    	CID id;
    	ARS ars; 
    	time LastUpdate;
    	time Start;
    	time TimeOut;
    	ClientStatus Status;
    	ClientStatusType StatusType;
    };

    struct NominalHost{
	    string HostName;
	    string Interface;
	    string OS;
	    short   CPUNumber;
	    short   Memory;
	    long    Clock;      
    };
   
    struct ActiveHost{
      string HostName;
      string Interface;
      HostStatus Status;
      short ClientsRunning;
      short ClientsAllowed; 
      long ClientsProcessed;
      long ClientsFailed;
      long ClientsKilled;
      time LastFailed;
      time LastUpdate;
      long    Clock;      
    };

	class CID_var: public smart_ptr<CID> {
	public:
		CID_var(CID* p=0): smart_ptr<DPS::Client::CID>() { };
		CID_var(const CID_var& p): smart_ptr<DPS::Client::CID>(p) { };
		CID_var & operator =(CID *p) throw() { allocate(p); return (*this); };
	};
	
 };
 
  interface Producer: Client{
    enum TransferStatus{Begin,Continue,End};
    enum RunStatus{ToBeRerun, Failed,Processing, Finished, Unknown, Allocated,Foreign,Canceled};
    enum DSTStatus{Success,InProgress,Failure,Validated};
    enum RunMode{RILO,LILO,RIRO,LIRO};
    enum DSTType{Ntuple,EventTag,RootFile,RawFile};
    struct TDVTableEntry{
     unsigned long id;
     time Insert;
     time Begin;
     time End;
    };

    struct TDVName{
     string Name;
     string File;
     long Size;
     unsigned long CRC;
     TDVTableEntry Entry;
     long DataMC;
     boolean Success;
    };  
    typedef sequence<TDVName> TDV;
    typedef sequence<unsigned long> TDVbody;
    typedef sequence<TDVTableEntry> TDVTable; 

	class TDVbody_var: public smart_ptr<TDVbody> {
	public:
		TDVbody_var(TDVbody* p=0): smart_ptr<DPS::Producer::TDVbody>() { };
		TDVbody_var(const TDVbody_var& p): smart_ptr<DPS::Producer::TDVbody>(p) { };
		TDVbody_var & operator =(DPS::Producer::TDVbody *p) throw() { allocate(p); return (*this); };
		void* get_buffer() const {
			if (_ptr->size()>0) return static_cast<void *>(&(*_ptr)[0]);
			  else return static_cast<void *>(0);
		};
	};

    struct CurrentInfo{
      unsigned long Run;
      long EventsProcessed;     
      long LastEventProcessed;
      long CriticalErrorsFound;
      long ErrorsFound;
      float CPUTimeSpent;
      float TimeSpent;
      float Mips;
      float CPUMipsTimeSpent;
      RunStatus Status;
      string HostName;
    }; 

    struct DSTInfo{
     DSTType type;
     unsigned long uid;
     string HostName;
     string OutputDirPath;
     RunMode Mode;
     long UpdateFreq;
     long DieHard;
     long FreeSpace;
     long TotalSpace;
    };
     
    struct RunEvInfo{
    	unsigned long uid;
    	unsigned long Run;
    	long FirstEvent;
    	long LastEvent;
    	time TFEvent;
    	time TLEvent; 
    	long Priority;
    	string FilePath;
    	RunStatus Status;
    	RunStatus History;
    	unsigned long      CounterFail;
    	time SubmitTime;
    	unsigned long cuid;
    	unsigned long rndm1;
    	unsigned long rndm2;
    	long DataMC;
    	CurrentInfo cinfo;     
    };
    typedef sequence<DSTInfo> DSTIS;
    
    void sendDSTInfo( DSTInfo info,  RecordChange rc)raises(DBProblem) {};
    typedef sequence<RunEvInfo> RES;

    struct DST{
     time   Insert;
     time   Begin;
     time   End;
     long   Run;
     long   FirstEvent;
     long   LastEvent;
     long   EventNumber;
     long   ErrorNumber;
     DSTStatus Status;
     DSTType Type;
     long   size;
     string Name;
     string Version;
     unsigned long crc;
     unsigned long rndm1;
     unsigned long rndm2;
     long FreeSpace;
     long TotalSpace;

    };    

    struct FailedOp{
     string message;
    };
    struct FPath{
     string fname;
     unsigned long pos;
    };  

	class RunEvInfo_var: public smart_ptr<RunEvInfo> {
	public:
		RunEvInfo_var(RunEvInfo* p=0): smart_ptr<DPS::Producer::RunEvInfo>() { };
		RunEvInfo_var(const RunEvInfo_var& p): smart_ptr<DPS::Producer::RunEvInfo>(p) { };
		RunEvInfo_var & operator =(RunEvInfo *p) throw() { allocate(p); return (*this); };
	};
	
	class DSTInfo_var: public smart_ptr<DSTInfo> {
	public:
		DSTInfo_var(DSTInfo* p=0): smart_ptr<DPS::Producer::DSTInfo>(p) { };
		DSTInfo_var(const DSTInfo_var& p): smart_ptr<DPS::Producer::DSTInfo>(p) { };
		DSTInfo_var & operator =(DSTInfo *p) throw() { allocate(p); return (*this); };
	};
  };

  class Producer_var: public smart_ptr<DPS::Producer> {
	public:
		Producer_var(DPS::Producer* p=0): smart_ptr<DPS::Producer>() { };
		Producer_var(const Producer_var& p): smart_ptr<DPS::Producer>(p) { };
		Producer_var & operator =(DPS::Producer *p) throw() { allocate(p); return (*this); };
  };
  
  interface Server: Client{

    enum OpType{StartClient,KillClient,CheckClient,ClearStartClient,ClearKillClient,ClearCheckClient};

    struct CriticalOps{
    	time TimeOut;
    	OpType Action;
    	time TimeStamp;
    	ClientType Type;
    	unsigned long id; 
    };
  };
#undef string
};

#endif	// __CORBA_LIGHT__

class AMSClientError{
protected:
 DPS::Client::ClientExiting _exit;
AString _message;
public:
const char* getMessage() const {return (const char*)_message;}
DPS::Client::ClientExiting ExitReason() const {return _exit;}
AMSClientError(const char * message=" Normal Exit",DPS::Client::ClientExiting exit=DPS::Client::CInExit):
_message(message),_exit(exit){};
};

class AMSClient{
protected:
 bool _ExitInProgress;
bool _GlobalError;
 AMSClientError  _error;
 int _debug;
 bool _Oracle;
 bool _Oraperl;
 bool __MT;
 bool _OracleW;
 bool _OracleWarm;
 bool _MC;
 int _v5;
uinteger _LastServiceTime;
 char * _DBFileName;
 int _MaxDBProcesses;
 CORBA::ORB_ptr _orb;
 fstream _fbin;
 fstream _fbin2;
 DPS::Client::CID _pid;
 DPS::Client::ClientExiting _exit;
 void _openLogFile(char * prefix,bool solo);
 bool _getpidhost(uinteger pid, const char* iface=0);
 static char _streambuffer[1024]; 
 static std::ostrstream _ost;
public:
AMSClient(int debug=0):_debug(debug),_Oracle(false),_Oraperl(false),__MT(false),_MC(false),_v5(0),_OracleW(false),_OracleWarm(false),_DBFileName(0),_MaxDBProcesses(0),_error(" "),_ExitInProgress(false),_GlobalError(false),_LastServiceTime(0)
{_pid.Status=DPS::Client::NOP;_pid.StatusType=DPS::Client::Permanent;};
virtual ~AMSClient(){};
const char * getdbfile() const {return _DBFileName;}
void setdbfile(const char * db);
void resetdbfile();
int getmaxdb()const{return _MaxDBProcesses;}
bool IsOracle() const {return _Oracle;}
bool IsOraperl() const {return _Oraperl;}
bool MT() const { return __MT;} 
bool IsMC() const { return _MC;}
int Isv5() const { return _v5;}
bool & GlobalError(){return _GlobalError;}
uinteger & LastServiceTime(){return _LastServiceTime;}
bool InitOracle()     const {return _OracleW;}
bool WarmOracleInit() const {return _OracleWarm;}
bool DBServerExists() const{return _MaxDBProcesses!=0 || _Oracle;}
AMSClientError & Error(){return _error;}
int  Debug() const{return _debug;}
const DPS::Client::CID & getcid()const {return _pid;}
DPS::Client::ClientExiting & ExitReason(){return _exit;}
void FMessage(const char * ch, DPS::Client::ClientExiting res);
void EMessage(const char * ch);
void IMessage(const char * ch);
void LMessage(const char * ch);
virtual void Exiting(const char * message=0)=0;
static char * print(const DPS::Producer::TDVTableEntry & a, const char *m=" ");
static char * print(const DPS::Producer::TDVName & a, const char *m=" ");
static char * print(const DPS::Client::ActiveClient & a, const char *m=" ");
static char * print(const DPS::Client::NominalClient & a, const char *m=" ");
static char * print(const DPS::Client::CID & a, const char *m=" ");
static ostream &  print(const DPS::Client::CID & a, ostream & o);
static char * print(const DPS::Client::ActiveHost & a, const char *m=" ");
static char * print(const DPS::Client::NominalHost & a, const char *m=" ");
static char * print(const DPS::Producer::RunEvInfo & a, const char *m=" ");
static char * print(const DPS::Producer::DSTInfo & a, const char *m=" ");
static char * print(const DPS::Server::CriticalOps & a,const  char *m=" ");
static char * print(const DPS::Producer::CurrentInfo & a, const char *m=" ");
static ostream & print(const DPS::Producer::CurrentInfo & a, ostream & o);
static ostream & print(const DPS::Producer::TDVTableEntry & a, ostream & o);
static char * print(const DPS::Producer::DST & a, const char *m=" ");
static char * CS2string(DPS::Client::ClientStatus a);
static char * CT2string(DPS::Client::ClientType a);
static char * CST2string(DPS::Client::ClientStatusType a);
static char * CSE2string(DPS::Client::ClientExiting);
static char * HS2string(DPS::Client::HostStatus a);
static char * RS2string(DPS::Producer::RunStatus a);
static char * DSTS2string(DPS::Producer::DSTStatus a);
static char * DSTT2string(DPS::Producer::DSTType a);
static char * RunMode2string(DPS::Producer::RunMode a);
static char * OPS2string(DPS::Server::OpType a);
static char * RC2string(DPS::Client::RecordChange a);
};

class Less{
protected:
DPS::Client::CID _cid;
int _selffirst;
public:

Less(){};
Less(DPS::Client::CID cid, int selffirst=0):_cid(cid),_selffirst(selffirst){};
bool operator () (const DPS::Client::ActiveClient & a,const DPS::Client::ActiveClient & b){
if(_selffirst){
  if(!strstr((const char*)_cid.HostName,(const char *)a.id.HostName) && strstr((const char*)_cid.HostName,(const char *)b.id.HostName))
    return 0;
}
return (a.id.uid<b.id.uid);
}

bool operator()(const DPS::Producer::RunEvInfo &a,const DPS::Producer::RunEvInfo &b){
if(a.Priority!=b.Priority)return a.Priority>b.Priority;
else return a.Status<b.Status;
}

bool operator ()(const DPS::Client::ActiveHost &a,const DPS::Client::ActiveHost &b){
if(a.Status ==b.Status)return a.Clock>b.Clock;
else if( b.Status==DPS::Client::OK)return false;
else if(a.Status != DPS::Client::NoResponse)return true;
else return false;


}


};


#endif
