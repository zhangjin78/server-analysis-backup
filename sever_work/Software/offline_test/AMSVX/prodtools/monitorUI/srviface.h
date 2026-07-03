#ifndef  _SRVIFACE_H
#define  _SRVIFACE_H

#include <stdio.h>
#include <stdlib.h>
#include <ctime>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <list>
#include <map>
#include <algorithm>
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>
#include <dirent.h>
#include <netdb.h>
#include <sys/socket.h>
#include <errno.h>

using std::cout;
using std::endl;
using std::string;
using std::stringstream;
using std::vector;
using std::list;
using std::map;
using std::multimap;
using std::fstream;

#include "server-cpp.h"
#include "server-cpp-stubs.h"
#include "server-cpp-skels.h"
#include "server-cpp-common.h"
#include "ManThread.hxx"
#include "ManMapper.hxx"

class srviface;
typedef vector<srviface *> IFACES;
extern IFACES __thrClients;
extern mutex __mxProcData;
extern string __baseUser;
extern string __baseHost;
extern string __thePerl;
extern unsigned int __toSsh;
extern CORBA::ORB_ptr __theOrb;
extern std::map<const char *, string> __logHost;

//_____________________________________________________________________
// Based on /afs/cern.ch/ams/local/bin/timeout
class process {
public:
	process();
	virtual ~process();
protected:
	string _cmd;
	string _log;
	fstream _fs;
public:
	int run(string &cmd);
	void terminate();
	bool getline(string &s);
	string lastcmd() const;
};
// Based on QT::QProcess. Using from main GUI
// thread might cause GUI to freeze
class qprocess: public QProcess {
public:
	qprocess();
	virtual ~qprocess();
protected:
	string _cmd;
	QTextStream *_fs;
public:
	int run(string &cmd);
	void terminate();
	bool getline(string &s);
	string lastcmd() const;
};

typedef struct flm {
private:
	int result;
public:
	string config;	// pattern name
	string file;	// actual name
	time_t time;	// actual last update time
	unsigned size;	// actual size
public:
	flm();
	flm(const string &s);
	bool configured() const;// check if has pattern name
	bool mapped() const;	   // check if has actual size
	string path() const;	   // get path of the pattern name
	// parse file name/time/size from command output:
	//   ls -ltr --dereference --time-style=full-iso ...
	int parse(const string &s);
	int parse();
} flm;

extern bool __otherFirst;
extern flm __dfCursor;
extern flm __dfFrame;
extern flm __df;
extern flm __transfer;
extern flm __scdbFile;
extern flm __scdb;

//_____________________________________________________________________
class srviface
{
	friend class sSE;
public:
	srviface(const string &versus, const string &siors, const string &siorp, const string &siord);
	virtual ~srviface();

protected:
	string _server;
	string _iors;
	string _iorp;
	string _iord;
	longlong _updated;
	longlong _pinged;
	int _fails;
	int _dbfails;
	string _ipaddr;
	vector<string> _ipalias;

	DPS::Client::CID _cid;

	DPS::Client::ARS_var _s_ars;
	DPS::Client::ACS_var _s_acs;
	DPS::Client::NHS_var _s_nhs;
	DPS::Server::NCS_var _s_ncs;
	DPS::Server::NCS_var _s_nks;
	DPS::Server::SS_var _s_env;

	DPS::Client::ARS_var _dbs_ars;
	vector<DPS::Server::DB_var> _vec_s_acs;
	DPS::Client::ACS_var _dbs_acs;

	DPS::Client::ARS_var _ars;
	DPS::Client::AHS_var _ahs;
	DPS::Client::ACS_var _acs;
	DPS::Server::NCS_var _ncs;
	DPS::Client::NHS_var _nhs;
	DPS::Producer::RES_var _res;
	DPS::Producer::DSTS_var _dsts;
	DPS::Producer::DSTIS_var _dstis;

	map<string, int> _map_ahs_by_host;
	multimap<string, int> _map_acs_by_host;
	map<long, int> _map_acs_by_run;
	map<long, int> _map_acs_by_uid;
	map<string, int> _map_nhs_by_host;
	multimap<string, int> _map_res_by_host;
	multimap<long, int> _map_res_by_run;
	map<long, int> _map_res_by_uid;
	multimap<string, int> _map_dst_by_host;
	multimap<long, int> _map_dst_by_run;
	imapper<string> _map_env_by_var;

public:
	bool _vrdFirst;
	vector<flm> _vrd;

private:
	template <typename T> void find(vector<int> &result, multimap<T,int> &source, const T &key) const
	{
		typedef typename multimap<T,int>::iterator iter;

 		pair<iter, iter> range = source.equal_range(key);
		for (iter it=range.first; it != range.second; ++it)
			result.insert(result.end(),(*it).second);
	};
	template <typename T> static void putEnums2Map(imapper<T> &m, const char *keys[], int szkeys)
	{
		if (m.len() == 0) {
			int sz = szkeys / sizeof(char *);
			for (int i=0; i<sz; i++) m.add(keys[i],static_cast<T>(i));
		}
	};
	void build();

public:
	template <typename T, typename U> static U found_nothing(map<T,U> &source)
	{
		return (U)-1;
	};
	template <typename T, typename U> static U find(map<T,U> &source, const T &key)
	{
		typedef typename map<T,U>::iterator iter;

		iter it = source.find(key);
		if (it != source.end())
			return it->second;
		return found_nothing(source);
	};
	const string &server() const { return _server; };
	const string &iors() const { return _iors; };
	const string &iorp() const { return _iorp; };
	const string &iord() const { return _iord; };
	const string &ipaddr() const { return _ipaddr; };
	const vector<string> &ipalias() const { return _ipalias; };

	const DPS::Client::ARS_var &s_ars() const { return _s_ars; };
	const DPS::Client::ACS_var &s_acs() const { return _s_acs; };
	const DPS::Client::NHS_var &s_nhs() const { return _s_nhs; };
	const DPS::Server::NCS_var &s_ncs() const { return _s_ncs; };
	const DPS::Server::NCS_var &s_nks() const { return _s_nks; };
	const DPS::Server::SS_var &s_env()  const { return _s_env; };

	const DPS::Client::ARS_var &dbs_ars() const { return _dbs_ars; };
	const vector<DPS::Server::DB_var> &vec_s_acs() { return _vec_s_acs; };
	const DPS::Client::ACS_var &dbs_acs() const { return _dbs_acs; };

	const DPS::Client::ARS_var &ars() const { return _ars; };
	const DPS::Client::AHS_var &ahs() const { return _ahs; };
	const DPS::Client::ACS_var &acs() const { return _acs; };
	const DPS::Client::NHS_var &nhs() const { return _nhs; };
	const DPS::Server::NCS_var &ncs() const { return _ncs; };
	const DPS::Producer::RES_var &res()  const { return _res; };
	const DPS::Producer::DSTS_var &dsts()  const { return _dsts; };
	const DPS::Producer::DSTIS_var &dstis()  const { return _dstis; };
	const map<long, int> &map_res_by_uid() const { return _map_res_by_uid; };

public:
	static srviface *iface(const char *name);
	longlong updated() const;
	longlong pinged() const;
	int fails() const;
	int dbfails() const;
	static bool pingHost(const string &host);
	static const char *vno(const string &s);
	const char *vno() const;
	static string tmpnm(const string &prefix);
	static string tos();
	static string shos();
	flm &vrd(unsigned id=0);

	void request(DPS::Server_var &server, DPS::Producer_var &producer, DPS::DBServer_var &dbserver) throw(string &);
	void request_vrd() throw(string &);
	void request_other() throw(string &);
	const imapper<string> &env() const;
	const DPS::Client::CID &mycid(DPS::Client::ClientType type=DPS::Client::Monitor);
	DPS::Client::CID &cid(const DPS::Client::ACS_var &s_acs, unsigned id) const throw (string &);

	int findAhsByHost(const string &host);
	vector<int> findAcsByHost(const string &host);
	int findAcsByRun(long run);
	int findAcsByUid(long uid) ;
	int findNhsByHost(const string &host);
	vector<int> findRunEvInfoByHost(const string &host);
	vector<int> findRunEvInfoByRun(long run);
	int findRunEvInfoByRunUid(long run, long uid);
	int findRunEvInfoByUid(long uid);
	vector<int> findDstByHost(const string &host);
	vector<int> findDstByRun(long run);
	int findLastDstByRun(long run);

	void sendActiveClient(DPS::Client::ActiveClient &ac, DPS::Client::RecordChange rc) const throw (string &);
	void sendRun(const DPS::Producer::RunEvInfo &run, DPS::Client::RecordChange rc) const throw (string &);

	static const char *strHostStatus(DPS::Client::HostStatus status);
	static const char *strClientType(DPS::Client::ClientType type);
	static const char *strClientStatusType(DPS::Client::ClientStatusType type);
	static const char *strClientStatus(DPS::Client::ClientStatus status);
	static const char *strRunMode(DPS::Producer::RunMode mode);
	static const char *strRunStatus(DPS::Producer::RunStatus status);
	static const char *strDSTType(DPS::Producer::DSTType type);
	static const char *strDSTStatus(DPS::Producer::DSTStatus status);

	static DPS::Client::HostStatus hostStatus(const char *status);
	static DPS::Client::ClientType clientType(const char *type);
	static DPS::Client::ClientStatusType clientStatusType(const char *type);
	static DPS::Producer::RunStatus runStatus(const char *status);
	static DPS::Producer::RunMode runMode(const char *mode);
	static DPS::Producer::DSTType dstType(const char *type);
	static DPS::Producer::DSTStatus dstStatus(const char *status);

	// Multithreading for GUI
	static void resetHosts(srviface *iface);
	static void resetFailedHosts(srviface *iface);
	typedef struct prFailedRuns {
		srviface *_iface;
		bool _deldst;
		prFailedRuns() { _iface = 0; _deldst = false; };
		prFailedRuns(srviface *iface, bool deldst) { _iface = iface; _deldst = deldst; };
		prFailedRuns(const prFailedRuns &p) { _iface = p._iface; _deldst = p._deldst; };
		void operator =(const prFailedRuns &p) { _iface = p._iface; _deldst = p._deldst; };
	} prFailedRuns;
	static void resetFailedRuns(prFailedRuns params);
	static void resetHistory(srviface *iface);
	static void deleteValidatedDST(srviface *iface);
	static void deleteFailedDST(srviface *iface);
	static void deleteAClients(srviface *iface);
	static void deleteRuns(srviface *iface);
	static void finishFailedRuns(srviface *iface);
	static void updateAFSToken(srviface *iface);

	// Berkeley DB
	void bdbPut(string &env, string &file, bool merge) throw (string &);
	void bdbPutPerl(string &env, string &file) throw (string &);
	void bdbGetText(string &env, string &file, bool merge) throw (string &);

	// Python klist.py and kread.py equivalents
	static void klist() throw (string &);
	static void kread() throw (string &);
};

//_____________________________________________________________________
// some predicates

struct srviface_eq
{
private:
	string _key;
public:
	srviface_eq(const char *key) { _key = key; };
	bool operator ()(const IFACES::value_type &it) const { return it->server() == _key; };
};

#endif	// eof _SRVIFACE_H
