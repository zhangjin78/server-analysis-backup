#ifndef _DBSERVER_H
#define _DBSERVER_H

#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <iostream>
using namespace std;
using std::cout;
using std::endl;
#include <string>
using std::string;
#include <sstream>
using std::stringstream;
#include <algorithm>
using std::stable_sort;
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/filesystem.hpp>

#include "server-cpp.h"
#include "server-cpp-stubs.h"
#include "server-cpp-skels.h"
#include "server-cpp-common.h"
#include "bdbRC.hxx"
#include "oraRC.h"

extern int _dbgLevel;
extern int _maxSuspHost;
extern double _maxFailsHost;
extern double _maxFailToleranceHost;
extern map<string,double> _ahsFails;

typedef unsigned int uinteger;

//_____________________________________________________________________
class DBServer_impl: public virtual POA_DPS::DBServer
{
public:
	CORBA::Boolean sendId(DPS::Client::CID & cid, float mips,uinteger timeout)throw (CORBA::SystemException);
	void getId(DPS::Client::CID_out cid)throw (CORBA::SystemException);
	int getARS(const DPS::Client::CID & cid, DPS::Client::ARS_out ars, DPS::Client::AccessType type=DPS::Client::Any,uinteger id=0, int selffirst=0)throw (CORBA::SystemException);
	void Exiting(const DPS::Client::CID& cid,const char * Error, DPS::Client::ClientExiting  Status)throw (CORBA::SystemException);
	int getEnv(const DPS::Client::CID &cid, DPS::Server::SS_out ss)throw (CORBA::SystemException);
	void setEnv(const DPS::Client::CID &cid,const char * env, const char *path)throw (CORBA::SystemException);
	void sendCriticalOps(const DPS::Client::CID &cid, const DPS::Server::CriticalOps & op)throw (CORBA::SystemException);
	void SystemCheck()throw (CORBA::SystemException);
	CORBA::Boolean AdvancedPing()throw (CORBA::SystemException);
	CORBA::Boolean TypeExists(DPS::Client::ClientType type)throw (CORBA::SystemException);
	int getACS(const DPS::Client::CID &cid, DPS::Server::ACS_out acs, unsigned int & maxc)throw (CORBA::SystemException);
	int getSplitACS(const DPS::Client::CID &cid, unsigned int & pos, DPS::Server::ACS_out acs, unsigned int & maxc, DPS::Producer::TransferStatus &st) throw (CORBA::SystemException);
	void sendAC(const DPS::Client::CID &cid,  DPS::Client::ActiveClient & ac,DPS::Client::RecordChange rc)throw (CORBA::SystemException);
	int getNC(const DPS::Client::CID &cid, DPS::Client::NCS_out nc)throw (CORBA::SystemException);
	void sendNC(const DPS::Client::CID &cid,  const DPS::Client::NominalClient & nc,DPS::Client::RecordChange rc)throw (CORBA::SystemException);
	void sendNCS(const DPS::Client::CID &cid, const DPS::Client::NCS & nc)throw (CORBA::SystemException);
	int getNK(const DPS::Client::CID &cid, DPS::Client::NCS_out nc)throw (CORBA::SystemException);
	void sendNKS(const DPS::Client::CID &cid, const DPS::Client::NCS & nc)throw (CORBA::SystemException);
	void sendNK(const DPS::Client::CID &cid,  const DPS::Client::NominalClient & nc,DPS::Client::RecordChange rc)throw (CORBA::SystemException);
	int getNHS(const DPS::Client::CID &cid,DPS::Client::NHS_out nhl)throw (CORBA::SystemException);
	void sendNHS(const DPS::Client::CID &cid,const DPS::Client::NHS & nhl)throw (CORBA::SystemException);
	void sendNH(const DPS::Client::CID &cid,  const DPS::Client::NominalHost & ah,DPS::Client::RecordChange rc)throw (CORBA::SystemException);
	int getAHS(const DPS::Client::CID &cid,DPS::Client::AHS_out ahl)throw (CORBA::SystemException);
	void sendAHS(const DPS::Client::CID &cid,const DPS::Client::AHS & ahl)throw (CORBA::SystemException);
	void sendAH(const DPS::Client::CID &cid,  const DPS::Client::ActiveHost & ah,DPS::Client::RecordChange rc)throw (CORBA::SystemException);
	CORBA::Boolean getDBSpace(const DPS::Client::CID &cid, const char * path, const char * addpath,DPS::Server::DB_out db)throw (CORBA::SystemException);
	int getTDV(const DPS::Client::CID & cid,  DPS::Producer::TDVName & tdvname, DPS::Producer::TDVbody_out body)throw (CORBA::SystemException);
	int getSplitTDV(const DPS::Client::CID & cid, unsigned int & pos, DPS::Producer::TDVName & tdvname, DPS::Producer::TDVbody_out body, DPS::Producer::TransferStatus &st)throw (CORBA::SystemException);
	void sendTDV(const DPS::Client::CID & cid, const DPS::Producer::TDVbody & tdv, DPS::Producer::TDVName & tdvname )throw (CORBA::SystemException);
	void sendTDVUpdate(const DPS::Client::CID & cid, const DPS::Producer::TDVName & tdvname )throw (CORBA::SystemException);
	int getTDVTable(const DPS::Client::CID & cid,DPS::Producer::TDVName & tdvname, unsigned int run, DPS::Producer::TDVTable_out table)throw (CORBA::SystemException);
	void sendCurrentInfo(const DPS::Client::CID & cid, const  DPS::Producer::CurrentInfo &cii, int propagate)throw (CORBA::SystemException);
	int getDSTInfoS(const DPS::Client::CID &cid, DPS::Producer::DSTIS_out res)throw (CORBA::SystemException);
	int getSplitDSTInfoS(const DPS::Client::CID &cid, DPS::Producer::DSTIS_out res, unsigned int pos, unsigned int len, DPS::Producer::TransferStatus &st) throw (CORBA::SystemException);
	void sendDSTInfoS(const DPS::Client::CID &cid, const DPS::Producer::DSTIS & res)throw (CORBA::SystemException);
	int sendSplitDSTInfoS(const DPS::Client::CID &cid, const DPS::Producer::DSTIS &acs, DPS::Producer::TransferStatus &st)throw (CORBA::SystemException);
	void sendDSTInfo(const  DPS::Producer::DSTInfo & ne,DPS::Client::RecordChange rc)throw (CORBA::SystemException);
	int getRunEvInfoS(const DPS::Client::CID &cid, DPS::Producer::RES_out res, unsigned int & maxrun)throw (CORBA::SystemException);
	int getSplitRunEvInfoS(const DPS::Client::CID &cid, DPS::Producer::RES_out res, unsigned int pos, unsigned int len, unsigned int &maxrun, DPS::Producer::TransferStatus &st) throw (CORBA::SystemException);
	void sendRunEvInfoS(const DPS::Client::CID &cid, const DPS::Producer::RES & res, unsigned int  maxrun)throw (CORBA::SystemException);
	int sendSplitRunEvInfoS(const DPS::Client::CID &cid, const DPS::Producer::RES & res, unsigned int  maxrun, DPS::Producer::TransferStatus &st)throw (CORBA::SystemException);
	void getRunEvInfo(const DPS::Client::CID &cid, DPS::Producer::RunEvInfo_out rv, DPS::Producer::DSTInfo_out dv)throw (CORBA::SystemException);
	void sendRunEvInfo(const  DPS::Producer::RunEvInfo & ne,DPS::Client::RecordChange rc)throw (CORBA::SystemException);
	int getRunsTotal() throw (CORBA::SystemException);
	int getRunsNumber(DPS::Producer::RunStatus status) throw (CORBA::SystemException);
	void sendDSTEnd(const DPS::Client::CID & cid, const  DPS::Producer::DST & ne, DPS::Client::RecordChange rc)throw (CORBA::SystemException);
	int getDSTS(const DPS::Client::CID & cid, DPS::Producer::DSTS_out dsts)throw (CORBA::SystemException);
	int getSplitDSTS(const DPS::Client::CID & cid, DPS::Producer::DSTS_out dsts, unsigned int pos, unsigned int len, DPS::Producer::TransferStatus &st) throw (CORBA::SystemException);
	int getDSTSR(const DPS::Client::CID & cid,   int  run,DPS::Producer::DSTS_out dsts)throw (CORBA::SystemException);
	void sendDSTS(const DPS::Client::CID & cid, const DPS::Producer::DSTS & dsts)throw (CORBA::SystemException);
	int sendSplitDSTS(const DPS::Client::CID &cid, const DPS::Producer::DSTS &acs, DPS::Producer::TransferStatus &st)throw (CORBA::SystemException);
	int getRun(const DPS::Client::CID &cid, const DPS::Producer::FPath & fpath, DPS::Producer::RUN_out run,DPS::Producer::TransferStatus & st)throw (CORBA::SystemException,DPS::Producer::FailedOp);
	int sendFile(const DPS::Client::CID &cid,  DPS::Producer::FPath & fpath, const  DPS::Producer::RUN & file,DPS::Producer::TransferStatus & st)throw (CORBA::SystemException,DPS::Producer::FailedOp);
	void ping()throw (CORBA::SystemException);
	void pingp()throw (CORBA::SystemException);
	char * getDBFilePath(const DPS::Client::CID &cid)throw (CORBA::SystemException);
	int getRunEvInfoSPerl(const DPS::Client::CID &cid, DPS::Producer::RES_out res, unsigned int  maxrun, unsigned  int &maxrun1)throw (CORBA::SystemException);
	int getSplitRunEvInfoSPerl(const DPS::Client::CID &cid, DPS::Producer::RES_out res, unsigned int pos, unsigned int len, unsigned int  maxrun, unsigned  int &maxrun1)throw (CORBA::SystemException);
	void sendACPerl(const DPS::Client::CID &cid,  const DPS::Client::ActiveClient & ac,DPS::Client::RecordChange rc)throw (CORBA::SystemException);
	int  getFreeHost(const DPS::Client::CID & cid, DPS::Client::ActiveHost_out host)throw (CORBA::SystemException);
	int  getFreeHostN(const DPS::Client::CID & cid)throw (CORBA::SystemException);
	void clearACS( DPS::Client::ClientType type)throw (CORBA::SystemException);
	void clearNCS( DPS::Client::ClientType type)throw (CORBA::SystemException);
	void clearAHS( DPS::Client::ClientType type)throw (CORBA::SystemException);
	void clearNHS()throw (CORBA::SystemException);
	void clearDST( DPS::Producer::DSTType type)throw (CORBA::SystemException);
	void clearDSTI()throw (CORBA::SystemException);
	void clearRunEvInfo( DPS::Producer::RunStatus status)throw (CORBA::SystemException);
	void sendmessage(const char * add, const char * sub, const char *mes)throw (CORBA::SystemException);
};

//_____________________________________________________________________
class dbserver
{
public:
	static dbserver& instance() {
		static dbserver _instance;
		return _instance;
	};
private:
	dbserver() {
		_orb = CORBA_OBJECT_NIL;
		_poa = CORBA_OBJECT_NIL;
		_mgr = CORBA_OBJECT_NIL;
		_db = 0;
		_dbora = 0;
	};
	dbserver(dbserver const &);
	void operator =(dbserver const &server);

protected:
	friend class DBServer_impl;
	CORBA::ORB_ptr _orb;
	PortableServer::POA_ptr _poa;
	PortableServer::POAManager_ptr _mgr;
	DBServer_impl _servant;
	DPS::Server_var _arsref;
	DPS::Producer_var _arpref;

	DPS::Client::CID _cid;
	DPS::Client::ActiveClient _ac;
	DPS::Client::ARS _ars;

	bdbRC *_db;
	oraRC *_dbora;

public:
	bool Init(int argc, char *argv[]) throw(string &);
	void RunOrbit();
	bool UpdateEverything(bool sendback, const char *repairdb);
private:
	bool UpdateARS(DPS::Server_var &server);
	bool InitDBFile(const char *dir, const char *file, const char *repairdb);
	bool SendId();
	Connection *InitOracle(int argc=0, char *argv[]=0);
	void CloseOracle();

public:
	DPS::Client::CID mycid(DPS::Client::ClientType type) const;
	static const char *strClientType(DPS::Client::ClientType type);
	static const char *strClientStatusType(DPS::Client::ClientStatusType type);
	static const char *strClientStatus(DPS::Client::ClientStatus stat);
	static const char *strRecordChange(DPS::Client::RecordChange rc);
	static const char *strRunStatus(DPS::Producer::RunStatus status);
	static const char *strDSTStatus(DPS::Producer::DSTStatus status);
	static const char *strHostStatus(DPS::Client::HostStatus status);
	static void debug(const char *head);
	static void debug(const char *head, const DPS::Client::CID &cid);
	static void debug(const char *head, DPS::Client::RecordChange rc);
	static void debug(const char *head, const DPS::Client::CID &cid, DPS::Client::RecordChange rc);
	static void debug(const char *head, const DPS::Producer::RunEvInfo &ne, DPS::Client::RecordChange rc);
	static void debug(const char *head, const DPS::Client::CID &cid, const DPS::Producer::DST &ne, DPS::Client::RecordChange rc);
	static void debug(const char *head, const DPS::Client::CID &cid, const DPS::Client::ActiveClient &ac, DPS::Client::RecordChange rc);
	static void debug(const char *head, const DPS::Client::CID &cid, const DPS::Client::ActiveHost &ah, DPS::Client::RecordChange rc);
	static int safe_strlen(const char *str, size_t max_len);
};

#endif	// eof _DBSERVER__H
