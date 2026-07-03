#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <sstream>
#include <boost/format.hpp>
using std::cout;
using std::endl;
using std::string;
using std::stringstream;

#include "dbserver.hxx"
#include "bdbTraits.hxx"
#include "ManMapper.hxx"

//_____________________________________________________________________
// Sort predicates, being used only here

struct Clock {
     bool operator()(ah_dt::iter ita, ah_dt::iter itb) {
		DPS::Client::ActiveHost &a = ah_dt::ref(ita);
		DPS::Client::ActiveHost &b = ah_dt::ref(itb);
		if (b.Status == a.Status) {
			long cb = b.ClientsAllowed-b.ClientsRunning;
			long ca = a.ClientsAllowed-a.ClientsRunning;
			return  cb*b.Clock < ca*a.Clock;
		}
		return b.Status<a.Status;
     };
} Clock;

class sortAH {
public:
	ah_dt::iter _it;
	float _Prior;
	DPS::Client::HostStatus _Status;
	short _ClientsAllowed;
	short _ClientsSuspened;
	explicit sortAH() {};
	explicit sortAH(ah_dt::iter &it) {
		_it=it;
		DPS::Client::ActiveHost &ahl=ah_dt::ref(_it);
		_Prior=0;
		if (ahl.ClientsAllowed-ahl.ClientsRunning > 0) {
			if (strstr((const char*)ahl.HostName,"lxplus")) _Prior=1;
			else _Prior=2;
		}
		_Status=ahl.Status;
		_ClientsAllowed=ahl.ClientsAllowed;
		_ClientsSuspened=0;
	};
	DPS::Client::ActiveHost &ah() { return ah_dt::ref(_it); };
};

struct Clock2 {
	bool operator()(sortAH a, sortAH b) {
		if (b._Status == a._Status) {
			if (b._Prior == a._Prior) {
				int cb = static_cast<int>(b._ClientsAllowed)-b.ah().ClientsRunning;
				int ca = static_cast<int>(a._ClientsAllowed)-a.ah().ClientsRunning;
				return cb*b.ah().Clock<ca*a.ah().Clock;
			}
			return b._Prior<a._Prior;
		}
		return b._Status<a._Status;
	};
} Clock2;

struct prio {
	bool operator()(rtb_dt::iter ita, rtb_dt::iter itb) {
		DPS::Producer::RunEvInfo &a = rtb_dt::ref(ita);
		DPS::Producer::RunEvInfo &b = rtb_dt::ref(itb);
		return b.Priority < a.Priority;
	};
} prio;

struct prio1 {
	bool operator()(rtb_dt::iter ita, rtb_dt::iter itb) {
		DPS::Producer::RunEvInfo &a = rtb_dt::ref(ita);
		DPS::Producer::RunEvInfo &b = rtb_dt::ref(itb);
		return (b.LastEvent-b.FirstEvent) < (a.LastEvent-a.FirstEvent);
	};
} prio1;

//_____________________________________________________________________
// DBServer_impl

CORBA::Boolean DBServer_impl::sendId(DPS::Client::CID & cid, float mips,uinteger timeout)throw (CORBA::SystemException)
{
	if (_dbgLevel)
		dbserver::debug("DBServer_impl::sendId-I- ");
	return false;
}

void DBServer_impl::getId(DPS::Client::CID_out cid)throw (CORBA::SystemException)
{
	if (_dbgLevel)
		dbserver::debug("DBServer_impl::getId-I- ");
}

int DBServer_impl::getARS(const DPS::Client::CID &cid, DPS::Client::ARS_out ars, DPS::Client::AccessType type, uinteger id, int selffirst)throw (CORBA::SystemException)
{
	if (_dbgLevel)
		dbserver::debug("DBServer_impl::getARS-I- ");
	return 0;
}

void DBServer_impl::Exiting(const DPS::Client::CID& cid,const char * Error, DPS::Client::ClientExiting  Status)throw (CORBA::SystemException)
{
	if (_dbgLevel)
		dbserver::debug("DBServer_impl::Exiting-I- ");
}

int DBServer_impl::getEnv(const DPS::Client::CID &cid, DPS::Server::SS_out ss)throw (CORBA::SystemException)
{
	if (_dbgLevel)
		dbserver::debug("DBServer_impl::getEnv-I- ");
	return 0;
}

void DBServer_impl::setEnv(const DPS::Client::CID &cid, const char * env, const char *path)throw (CORBA::SystemException)
{
	if (_dbgLevel)
		dbserver::debug("DBServer_impl::setEnv-I- ",cid);
    dbserver &dbserv = dbserver::instance();

	try
	{
     	dbTables type;
 		switch(cid.Type) {
 		case DPS::Client::Server:
 			type = DBO_ENV;
 			break;
 		default:
 			return;
 		}
 		bdbRC db(dbserv._db);
 		db.connect2(type);
		env_dt db_env(db,type);
 		int len = db_env.length();

		string senv(env);
		for (int i=0; i<len; i++,db_env++) {
			CharStringToken st(*db_env,"=");
			if (st.tokens()>=2 && senv == st.token(0)) {
				try {
					if (senv == "AMSDBServDbg")
						_dbgLevel = boost::lexical_cast<int>(path);
					else if (senv == "AMSMaxInactiveClientsHost")
						_maxSuspHost = boost::lexical_cast<int>(path);
					else if (senv == "AMSMaxFailedClientsHost") {
						_maxFailsHost = boost::lexical_cast<double>(path);
						if (_maxFailsHost == 0)
							for (map<string,double>::iterator it=_ahsFails.begin(); it!=_ahsFails.end(); it++)
								it->second = 0;
					}
					else if (senv == "AMSMaxFailedClientsHostTolerance") {
						double maxTolerance = boost::lexical_cast<double>(path);
						if (maxTolerance <= 0 || maxTolerance >= 1) {
							cout << "DBServer_impl::setEnv-W- " << senv << " is't in valid range, ignored " << endl;
							return;
						}
						_maxFailToleranceHost = maxTolerance;
					}
				}
				catch(boost::bad_lexical_cast &ex) { 
				}
				db_env.erase_it();
				break;
			}
		}
		CORBA::String_var sv((senv+"="+path).c_str());
		db_env.insert(sv);
	}
	catch (string &ex) {
		throw DPS::DBProblem(ex.c_str());
	}
}

void DBServer_impl::sendCriticalOps(const DPS::Client::CID &cid, const DPS::Server::CriticalOps & op)throw (CORBA::SystemException)
{
	if (_dbgLevel)
		dbserver::debug("DBServer_impl::sendCriticalOps-I- ");
}

void DBServer_impl::SystemCheck()throw (CORBA::SystemException)
{
	if (_dbgLevel)
		dbserver::debug("DBServer_impl::SystemCheck-I- ");
}

CORBA::Boolean DBServer_impl::AdvancedPing()throw (CORBA::SystemException)
{
	if (_dbgLevel)
		dbserver::debug("DBServer_impl::AdvancedPing-I- ");
	return false;
}

CORBA::Boolean DBServer_impl::TypeExists(DPS::Client::ClientType type)throw (CORBA::SystemException)
{
	if (_dbgLevel)
		dbserver::debug("DBServer_impl::TypeExists-I- ");
	return false;
}

int DBServer_impl::getACS(const DPS::Client::CID &cid, DPS::Server::ACS_out acs, unsigned int & maxc)throw (CORBA::SystemException)
{
	if (_dbgLevel)
		dbserver::debug("DBServer_impl::getACS-I- ",cid);
	dbserver &dbserv = dbserver::instance();

	try {
    	dbTables type;
    	switch(cid.Type) {
    	case DPS::Client::Server:
			type = DBO_ASL;
			break;
    	case DPS::Client::Producer:
			type = DBO_ACL;
			break;
    	case DPS::Client::DBServer:
			type = DBO_ADBSL;
			break;
    	case DPS::Client::Monitor:
			type = DBO_AML;
			break;
    	default:
			string ex = string("Unable to getacs for ") + dbserver::strClientType(cid.Type);
			throw DPS::DBProblem(ex.c_str());
		}
		bdbRC db(dbserv._db);
		db.connect2(type);
		ac_dt db_dbacs(db,type);
		int len = db_dbacs.length();

		acs = new DPS::Client::ACS();
		acs->length(len);
		for (int i=0; i<len; i++,db_dbacs++)
			acs[i] = *db_dbacs;
		return len;
    }
	catch (string &ex) {
		throw DPS::DBProblem(ex.c_str());
	}
}

int DBServer_impl::getSplitACS(const DPS::Client::CID &cid, unsigned int & pos, DPS::Server::ACS_out acs, unsigned int & maxc, DPS::Producer::TransferStatus &st) throw (CORBA::SystemException)
{
	if (_dbgLevel)
		dbserver::debug("DBServer_impl::getSplitACS-I- ");
	return 0;
}

void DBServer_impl::sendAC(const DPS::Client::CID &cid, DPS::Client::ActiveClient & ac, DPS::Client::RecordChange rc)throw (CORBA::SystemException)
{
	if (_dbgLevel)
		dbserver::debug("DBServer_impl::sendAC-I- ",cid,rc);
	throw DPS::DBProblem("DBServer_impl::sendAC-E- no such routine");
}

int DBServer_impl::getNC(const DPS::Client::CID &cid, DPS::Client::NCS_out nc)throw (CORBA::SystemException)
{
	if (_dbgLevel)
		dbserver::debug("DBServer_impl::getNC-I- ",cid);
	dbserver &dbserv = dbserver::instance();

	try {
    	dbTables type;
    	switch(cid.Type) {
    	case DPS::Client::Server:
			type = DBO_NSL;
			break;
    	case DPS::Client::Producer:
			type = DBO_NCL;
			break;
    	default:
			string ex = string("Unable to getnc for ") + dbserver::strClientType(cid.Type);
			throw DPS::DBProblem(ex.c_str());
		}
		bdbRC db(dbserv._db);
		db.connect2(type);
		nc_dt db_ncs(db,type);
		int len = db_ncs.length();

		nc = new DPS::Client::NCS;
		nc->length(len);
		for (int i=0; i<len; i++,db_ncs++)
			nc[i] = *db_ncs;
		return len;
    }
	catch (string &ex) {
		throw DPS::DBProblem(ex.c_str());
	}
}

void DBServer_impl::sendNC(const DPS::Client::CID &cid,  const DPS::Client::NominalClient & nc, DPS::Client::RecordChange rc)throw (CORBA::SystemException)
{
	if (_dbgLevel)
		dbserver::debug("DBServer_impl::sendNC-I- ",cid,rc);
    dbserver &dbserv = dbserver::instance();

    try {
		typedef vector<DPS::Client::NominalClient>::iterator nhit;
	
    	dbTables type;
		switch(cid.Type) {
		case DPS::Client::Server:
			type = DBO_NSL;
			break;
		case DPS::Client::Producer:
			type = DBO_NCL;
			break;
		default:
			cout<<"DBServer_impl::sendNC-E- wrong CID.Type "<<cid.Type<<endl;
			return;
		}
		bdbRC db(dbserv._db);
		db.connect2(type);
		nc_dt db_ncs(db,type);

		int len = db_ncs.length();
		switch (rc) {
		case DPS::Client::Update:
		{ // keeps the same record position
			list<DPS::Client::NominalClient_var> ncs;
			for (int i=0; i<len; i++,db_ncs++) {
				DPS::Client::NominalClient_var ncc;
				if (nc.uid == (*db_ncs).uid)
					ncc = new DPS::Client::NominalClient(nc);
				else
					ncc = new DPS::Client::NominalClient(*db_ncs);
				ncs.push_back(ncc);
			}
			db_ncs.erase();
			db_ncs.rewind();
			for(list<DPS::Client::NominalClient_var>::iterator k=ncs.begin(); k!=ncs.end(); ++k)
				db_ncs.insert(*k);
		}
		/* relocates record to the bottom position
			for (int i=0; i<len; i++,db_ncs++)
				if (nc.uid == (*db_ncs).uid) {
					db_ncs.erase_it();
					db_ncs.insert(const_cast<DPS::Client::NominalClient &>(nc));
					break;
				}
		 */
			break;
		case DPS::Client::Create:
			for (int i=0; i<len; i++,db_ncs++)
				if (nc.uid == (*db_ncs).uid) {
					db_ncs.erase_it();
					break;
				}
			db_ncs.insert(const_cast<DPS::Client::NominalClient &>(nc));
			break;
		case DPS::Client::ClearCreate:
			db_ncs.erase();
			db_ncs.insert(const_cast<DPS::Client::NominalClient &>(nc));
			break;
		case DPS::Client::Delete:
			for (int i=0; i<len; i++,db_ncs++)
				if (nc.uid == (*db_ncs).uid) {
					db_ncs.erase_it();
					break;
				}
			break;
		default:
			break;
		}
    }
	catch (string &ex) {
		throw DPS::DBProblem(ex.c_str());
	}
}

void DBServer_impl::sendNCS(const DPS::Client::CID &cid, const DPS::Client::NCS & nc) throw (CORBA::SystemException)
{
	if (_dbgLevel)
		dbserver::debug("DBServer_impl::sendNCS-I- ",cid);
    dbserver &dbserv = dbserver::instance();
		
    try {
    	dbTables type;
		switch(cid.Type) {
		case DPS::Client::Server:
			type = DBO_NSL;
			break;
		case DPS::Client::Producer:
			type = DBO_NCL;
			break;
		default:
			cout<<"DBServer_impl::sendNCS-E- wrong CID.Type "<<cid.Type<<endl;
			return;
		}
		bdbRC db(dbserv._db);
		db.connect2(type);
		nc_dt db_ncs(db,type);

		db_ncs.erase();		
		for (int len=nc.length(),i=0; i<len; i++)
			db_ncs.insert(const_cast<DPS::Client::NominalClient &>(nc[i]));
    }
	catch (string &ex) {
		throw DPS::DBProblem(ex.c_str());
	}
}

int DBServer_impl::getNK(const DPS::Client::CID &cid, DPS::Client::NCS_out nc)throw (CORBA::SystemException)
{
	if (_dbgLevel)
		dbserver::debug("DBServer_impl::getNK-I- ",cid);
    dbserver &dbserv = dbserver::instance();

    try {
		bdbRC db(dbserv._db);
		db.connect2(DBO_NKL);
		nc_dt db_nks(db,DBO_NKL);
		int len = db_nks.length();

		nc = new DPS::Client::NCS;
		nc->length(len);
		for (int i=0; i<len; i++,db_nks++)
			nc[i] = *db_nks;
		return len;
    }
	catch (string &ex) {
		throw DPS::DBProblem(ex.c_str());
	}
}

void DBServer_impl::sendNKS(const DPS::Client::CID &cid, const DPS::Client::NCS & nc)throw (CORBA::SystemException)
{
	if (_dbgLevel)
		dbserver::debug("DBServer_impl::sendNKS-I- ",cid);
    dbserver &dbserv = dbserver::instance();

    try {
    	dbTables type;
		switch(cid.Type) {
		case DPS::Client::Killer:
			type = DBO_NKL;
			break;
		default:
			cout<<"DBServer_impl::sendNKS-E- wrong CID.Type "<<cid.Type<<endl;
			return;
		}
		bdbRC db(dbserv._db);
		db.connect2(type);
		nc_dt db_nks(db,type);

		db_nks.erase();
		for (int len=nc.length(),i=0; i<len; i++)
			db_nks.insert(const_cast<DPS::Client::NominalClient &>(nc[i]));
    }
	catch (string &ex) {
		throw DPS::DBProblem(ex.c_str());
	}
}

void DBServer_impl::sendNK(const DPS::Client::CID &cid,  const DPS::Client::NominalClient & nc,DPS::Client::RecordChange rc)throw (CORBA::SystemException)
{
	if (_dbgLevel)
		dbserver::debug("DBServer_impl::sendNK-I- ",cid,rc);
    dbserver &dbserv = dbserver::instance();

    try {
    	dbTables type;
		switch(cid.Type) {
		case DPS::Client::Killer:
			type = DBO_NKL;
			break;
		default:
			cout<<"DBServer_impl::sendNK-E- wrong CID.Type "<<cid.Type<<endl;
			return;
		}
		bdbRC db(dbserv._db);
		db.connect2(type);
		nc_dt db_nks(db,type);

		int len = db_nks.length();
		switch (rc) {
		case DPS::Client::Update:
		{ // keeps the same record position
			list<DPS::Client::NominalClient_var> nks;
			for (int i=0; i<len; i++,db_nks++) {
				DPS::Client::NominalClient_var ncc;
				if (nc.uid == (*db_nks).uid)
					ncc = new DPS::Client::NominalClient(nc);
				else
					ncc = new DPS::Client::NominalClient(*db_nks);
				nks.push_back(ncc);
			}
			db_nks.erase();
			db_nks.rewind();
			for(list<DPS::Client::NominalClient_var>::iterator k=nks.begin(); k!=nks.end(); ++k)
				db_nks.insert(*k);
		}
		/* relocates record to the bottom position
			for (int i=0; i<len; i++,db_nks++)
				if (nc.uid == (*db_nks).uid) {
					db_nks.erase_it();
					db_nks.insert(const_cast<DPS::Client::NominalClient &>(nc));
					break;
				}
		 */
			break;
		case DPS::Client::Create:
			for (int i=0; i<len; i++,db_nks++)
				if (nc.uid == (*db_nks).uid) {
					db_nks.erase_it();
					break;
				}
			db_nks.insert(const_cast<DPS::Client::NominalClient &>(nc));
			break;
		case DPS::Client::Delete:
			for (int i=0; i<len; i++,db_nks++)
				if (nc.uid == (*db_nks).uid) {
					db_nks.erase_it();
					break;
				}
			break;
		case DPS::Client::ClearCreate:
			db_nks.erase();
			db_nks.insert(const_cast<DPS::Client::NominalClient &>(nc));
			break;
		default:
			break;
		}
    }
	catch (string &ex) {
		throw DPS::DBProblem(ex.c_str());
	}
}

int DBServer_impl::getNHS(const DPS::Client::CID &cid,DPS::Client::NHS_out nhl)throw (CORBA::SystemException)
{
	if (_dbgLevel)
		dbserver::debug("DBServer_impl::getNHS-I- ",cid);
    dbserver &dbserv = dbserver::instance();

    try
    {
		bdbRC db(dbserv._db);
		db.connect2(DBO_NHL);
		nh_dt db_nhs(db,DBO_NHL);
		int len = db_nhs.length();

		nhl = new DPS::Client::NHS;
		nhl->length(len);
		for (int i=0; i<len; i++,db_nhs++)
			nhl[i] = *db_nhs;
		return len;
    }
	catch (string &ex) {
		throw DPS::DBProblem(ex.c_str());
	}
}

void DBServer_impl::sendNHS(const DPS::Client::CID &cid, const DPS::Client::NHS & nhl) throw (CORBA::SystemException)
{
	if (_dbgLevel)
		dbserver::debug("DBServer_impl::sendNHS-I- ",cid);
    dbserver &dbserv = dbserver::instance();

    try {
    	dbTables type;
		switch(cid.Type) {
		case DPS::Client::Server:
		case DPS::Client::Producer:
			type = DBO_NHL;
			break;
		default:
			cout<<"DBServer_impl::sendNHS-E- wrong CID.Type "<<cid.Type<<endl;
			return;
		}
		bdbRC db(dbserv._db);
		db.connect2(type);
		nh_dt db_nhs(db,type);

		db_nhs.erase();
		for (int len=nhl.length(),i=0; i<len; i++)
			db_nhs.insert(const_cast<DPS::Client::NominalHost &>(nhl[i]));
    }
	catch (string &ex) {
		throw DPS::DBProblem(ex.c_str());
	}
}

void DBServer_impl::sendNH(const DPS::Client::CID &cid, const DPS::Client::NominalHost &ah, DPS::Client::RecordChange rc) throw (CORBA::SystemException)
{
	if (_dbgLevel)
		dbserver::debug("DBServer_impl::sendNH-I- ",cid,rc);
    dbserver &dbserv = dbserver::instance();

    try {
    	dbTables type;
		switch(cid.Type) {
		case DPS::Client::Server:
		case DPS::Client::Producer:
			type = DBO_NHL;
			break;
		default:
			cout<<"DBServer_impl::sendNH-E- wrong CID.Type "<<cid.Type<<endl;
			return;
		}
		bdbRC db(dbserv._db);
		db.connect2(type);
		nh_dt db_nhs(db,type);

		int len = db_nhs.length();
		switch (rc) {
		case DPS::Client::Update:
			for (int i=0; i<len; i++,db_nhs++)
				if (string(ah.HostName) == string((*db_nhs).HostName)) {
					db_nhs.erase_it();
					db_nhs.insert(const_cast<DPS::Client::NominalHost &>(ah));
					break;
				}
			break;
		case DPS::Client::Create:
			for (int i=0; i<len; i++,db_nhs++)
				if (string(ah.HostName) == string((*db_nhs).HostName)) {
					db_nhs.erase_it();
					break;
				}
			db_nhs.insert(const_cast<DPS::Client::NominalHost &>(ah));
			break;
		case DPS::Client::Delete:
			for (int i=0; i<len; i++,db_nhs++)
				if (string(ah.HostName) == string((*db_nhs).HostName)) {
					db_nhs.erase_it();
					break;
				}
			break;
		case DPS::Client::ClearCreate:
			db_nhs.erase();
			db_nhs.insert(const_cast<DPS::Client::NominalHost &>(ah));
			break;
		default:
			break;
		}
    }
	catch (string &ex) {
		throw DPS::DBProblem(ex.c_str());
	}
}

int DBServer_impl::getAHS(const DPS::Client::CID &cid,DPS::Client::AHS_out ahl)throw (CORBA::SystemException)
{
	if (_dbgLevel)
		dbserver::debug("DBServer_impl::getAHS-I- ",cid);
    dbserver &dbserv = dbserver::instance();

    try {
    	dbTables type;
    	switch(cid.Type) {
    	case DPS::Client::Server:
			type = DBO_AHLS;
			break;
    	case DPS::Client::Producer:
			type = DBO_AHLP;
			break;
    	case DPS::Client::DBServer:
			type = DBO_AHLS;
			break;
    	default:
			string ex = string("Unable to getahs for ") + dbserver::strClientType(cid.Type);
			throw DPS::DBProblem(ex.c_str());
		}
		bdbRC db(dbserv._db);
		db.connect2(type);
		ah_dt db_ahs(db,type);
		int len = db_ahs.length();

		DPS::Client::AHS_var orb_ahs = new DPS::Client::AHS;
		orb_ahs->length(len);
		for (int i=0; i<len; db_ahs++)
			orb_ahs[i++] = *db_ahs;
	 	ahl = orb_ahs._retn();
	 	return len;
    }
	catch (string &ex) {
		throw DPS::DBProblem(ex.c_str());
	}
}

void DBServer_impl::sendAHS(const DPS::Client::CID &cid,const DPS::Client::AHS & ahl)throw (CORBA::SystemException)
{
	if (_dbgLevel)
		dbserver::debug("DBServer_impl::sendAHS-I- ",cid);
    dbserver &dbserv = dbserver::instance();

    try {
    	dbTables type;
		switch(cid.Type) {
		case DPS::Client::Server:
			type = DBO_AHLS;
			break;
		case DPS::Client::Producer:
			type = DBO_AHLP;
			break;
		default:
			cout<<"DBServer_impl::sendAHS-E- wrong CID.Type "<<cid.Type<<endl;
			return;
		}
		bdbRC db(dbserv._db);
		db.connect2(type);
		ah_dt db_ahs(db,type);

		db_ahs.erase();
		for (int len=ahl.length(),i=0; i<len; i++)
			db_ahs.insert(const_cast<DPS::Client::ActiveHost &>(ahl[i]));
    }
	catch (string &ex) {
		throw DPS::DBProblem(ex.c_str());
	}
}

void DBServer_impl::sendAH(const DPS::Client::CID &cid,  const DPS::Client::ActiveHost &ah, DPS::Client::RecordChange rc)throw (CORBA::SystemException)
{
	if (_dbgLevel)
		dbserver::debug("DBServer_impl::sendAH-I- ",cid,ah,rc);
    dbserver &dbserv = dbserver::instance();

    try {
    	dbTables type;
		switch(cid.Type) {
		case DPS::Client::Server:
			type = DBO_AHLS;
			break;
		case DPS::Client::Producer:
			type = DBO_AHLP;
			break;
		default:
			cout<<"DBServer_impl::sendAH-E- wrong CID.Type "<<cid.Type<<endl;
			return;
		}
		bdbRC db(dbserv._db);
		db.connect2(type);
		ah_dt db_ahs(db,type);

		int len = db_ahs.length();
		switch (rc) {
		case DPS::Client::Update:
			for (int i=0; i<len; i++,db_ahs++)
				if (string(ah.HostName) == string((*db_ahs).HostName)) {
					db_ahs.erase_it();
					db_ahs.insert(const_cast<DPS::Client::ActiveHost &>(ah));
					break;
				}
			break;
		case DPS::Client::Create:
			for (int i=0; i<len; i++,db_ahs++)
				if (string(ah.HostName) == string((*db_ahs).HostName)) {
					db_ahs.erase_it();
					break;
				}
			db_ahs.insert(const_cast<DPS::Client::ActiveHost &>(ah));
			break;
		case DPS::Client::Delete:
			for (int i=0; i<len; i++,db_ahs++)
				if (string(ah.HostName) == string((*db_ahs).HostName)) {
					db_ahs.erase_it();
					break;
				}
			break;
		case DPS::Client::ClearCreate:
			db_ahs.erase();
			db_ahs.insert(const_cast<DPS::Client::ActiveHost &>(ah));
			break;
		default:
			break;
		}
    }
	catch (string &ex) {
		throw DPS::DBProblem(ex.c_str());
	}
}

CORBA::Boolean DBServer_impl::getDBSpace(const DPS::Client::CID &cid, const char * path, const char * addpath,DPS::Server::DB_out db)throw (CORBA::SystemException)
{
	if (_dbgLevel)
		dbserver::debug("DBServer_impl::getDBSpace-I- ",cid);
	return false;
}

int DBServer_impl::getTDV(const DPS::Client::CID & cid,  DPS::Producer::TDVName & tdvname, DPS::Producer::TDVbody_out body)throw (CORBA::SystemException)
{
	if (_dbgLevel)
		dbserver::debug("DBServer_impl::getTDV-I- ",cid);
	return 0;
}

int DBServer_impl::getSplitTDV(const DPS::Client::CID & cid, unsigned int & pos, DPS::Producer::TDVName & tdvname, DPS::Producer::TDVbody_out body, DPS::Producer::TransferStatus &st)throw (CORBA::SystemException)
{
	if (_dbgLevel)
		dbserver::debug("DBServer_impl::getSplitTDV-I- ",cid);
	return 0;
}

void DBServer_impl::sendTDV(const DPS::Client::CID & cid, const DPS::Producer::TDVbody & tdv, DPS::Producer::TDVName & tdvname )throw (CORBA::SystemException)
{
	if (_dbgLevel)
		dbserver::debug("DBServer_impl::sendTDV-I- ",cid);
}

void DBServer_impl::sendTDVUpdate(const DPS::Client::CID & cid, const DPS::Producer::TDVName & tdvname )throw (CORBA::SystemException)
{
	if (_dbgLevel)
		dbserver::debug("DBServer_impl::sendTDVUpdate-I- ",cid);
}

int DBServer_impl::getTDVTable(const DPS::Client::CID & cid,DPS::Producer::TDVName & tdvname, unsigned int run, DPS::Producer::TDVTable_out table)throw (CORBA::SystemException)
{
	if (_dbgLevel)
		dbserver::debug("DBServer_impl::getTDVTable-I- ",cid);
	return 0;
}

void DBServer_impl::sendCurrentInfo(const DPS::Client::CID & cid, const  DPS::Producer::CurrentInfo &cii, int propagate)throw (CORBA::SystemException)
{
	if (_dbgLevel)
		dbserver::debug("DBServer_impl::sendCurrentInfo-I- ",cid);
}

int DBServer_impl::getDSTInfoS(const DPS::Client::CID &cid, DPS::Producer::DSTIS_out res)throw (CORBA::SystemException)
{
	if (_dbgLevel)
		dbserver::debug("DBServer_impl::getDSTInfoS-I- ",cid);
	return 0;
}

int DBServer_impl::getSplitDSTInfoS(const DPS::Client::CID &cid, DPS::Producer::DSTIS_out res, unsigned int pos, unsigned int len, DPS::Producer::TransferStatus &st) throw (CORBA::SystemException)
{
	if (_dbgLevel)
		dbserver::debug("DBServer_impl::getSplitDSTInfoS-I- ",cid);
	return 0;
}

void DBServer_impl::sendDSTInfoS(const DPS::Client::CID &cid, const DPS::Producer::DSTIS & res)throw (CORBA::SystemException)
{
	if (_dbgLevel)
		dbserver::debug("DBServer_impl::sendDSTInfoS-I- ",cid);
}

int DBServer_impl::sendSplitDSTInfoS(const DPS::Client::CID &cid, const DPS::Producer::DSTIS &acs, DPS::Producer::TransferStatus &st)throw (CORBA::SystemException)
{
	if (_dbgLevel)
		dbserver::debug("DBServer_impl::sendSplitDSTInfoS-I- ",cid);
	return 0;
}

void DBServer_impl::sendDSTInfo(const  DPS::Producer::DSTInfo &ne, DPS::Client::RecordChange rc)throw (CORBA::SystemException)
{
	if (_dbgLevel)
		dbserver::debug("DBServer_impl::sendDSTInfo-I- ",rc);
    dbserver &dbserv = dbserver::instance();

    try
    {
		bdbRC db(dbserv._db);
		db.connect2(DBO_DSTI);
		dsti_dt db_pdsti(db,DBO_DSTI);

		switch(rc)
		{
		case DPS::Client::Update:
			for (int i=0,len=db_pdsti.length(); i<len; i++,db_pdsti++)
				if (ne.uid == (*db_pdsti).uid) {
					db_pdsti.erase_it();
					db_pdsti.insert(const_cast<DPS::Producer::DSTInfo &>(ne));
					break;
				}
			break;
		case DPS::Client::Create:
			for (int i=0,len=db_pdsti.length(); i<len; i++,db_pdsti++)
				if (ne.uid == (*db_pdsti).uid) {
					db_pdsti.erase_it();
					break;
				}
			db_pdsti.insert(const_cast<DPS::Producer::DSTInfo &>(ne));
			break;
		case DPS::Client::Delete:
			for (int i=0,len=db_pdsti.length(); i<len; i++,db_pdsti++)
				if (ne.uid == (*db_pdsti).uid) {
					db_pdsti.erase_it();
					break;
				}
			break;
		case DPS::Client::ClearCreate:
			db_pdsti.erase();
			db_pdsti.insert(const_cast<DPS::Producer::DSTInfo &>(ne));
			break;
		}
    }
	catch (string &ex) {
		throw DPS::DBProblem(ex.c_str());
	}
}

int DBServer_impl::getRunEvInfoS(const DPS::Client::CID &cid, DPS::Producer::RES_out res, unsigned int & maxrun)throw (CORBA::SystemException)
{
	if (_dbgLevel)
		dbserver::debug("DBServer_impl::getRunEvInfoS-I- ",cid);
	return 0;
}

int DBServer_impl::getSplitRunEvInfoS(const DPS::Client::CID &cid, DPS::Producer::RES_out res, unsigned int pos, unsigned int len, unsigned int &maxrun, DPS::Producer::TransferStatus &st) throw (CORBA::SystemException)
{
	if (_dbgLevel)
		dbserver::debug("DBServer_impl::getSplitRunEvInfoS-I- ",cid);
	return 0;
}

void DBServer_impl::sendRunEvInfoS(const DPS::Client::CID &cid, const DPS::Producer::RES & res, unsigned int  maxrun)throw (CORBA::SystemException)
{
	if (_dbgLevel)
		dbserver::debug("DBServer_impl::sendRunEvInfoS-I- ",cid);
}

int DBServer_impl::sendSplitRunEvInfoS(const DPS::Client::CID &cid, const DPS::Producer::RES &res, unsigned int  maxrun, DPS::Producer::TransferStatus &st)throw (CORBA::SystemException)
{
	if (_dbgLevel)
		dbserver::debug("DBServer_impl::sendSplitRunEvInfoS-I- ",cid);
}

void DBServer_impl::getRunEvInfo(const DPS::Client::CID &cid, DPS::Producer::RunEvInfo_out rv, DPS::Producer::DSTInfo_out dv)throw (CORBA::SystemException)
{
	if (_dbgLevel)
		dbserver::debug("DBServer_impl::getRunEvInfo-I- ",cid);
    dbserver &dbserv = dbserver::instance();

    try
    {
		bdbRC db(dbserv._db);
		db.connect2(DBO_RTB);
		rtb_dt db_prtb(db,DBO_RTB);
		bdbRC db2(db);
		db2.connect2(DBO_DSTI);
		dsti_dt db_pdsti(db2,DBO_DSTI);

		DPS::Producer::DSTInfo pdsti = *db_pdsti;
		for (int i=0,len=db_pdsti.length(); i<len; i++,db_pdsti++)
			if (string(cid.HostName).find((*db_pdsti).HostName) != string::npos) {
			    cout << "DBServer_impl::getRunEvInfo-I- found dst " << (*db_pdsti).OutputDirPath << endl;
			    pdsti = (*db_pdsti);
				break;
			}
			else if ((*db_pdsti).uid == 1)
			    pdsti = *db_pdsti;
		pdsti.DieHard = 0;

		vector<rtb_dt::iter> refs_prtb;
		for (int i=0,len=db_prtb.length(); i<len; i++,db_prtb++)
			refs_prtb.push_back(db_prtb._iter);
		sort(refs_prtb.begin(),refs_prtb.end(),prio1);

		unsigned long run = static_cast<unsigned long>(-1);
		for (vector<rtb_dt::iter>::iterator it=refs_prtb.begin(); it!=refs_prtb.end(); ++it) {
			DPS::Producer::RunEvInfo &rtb = rtb_dt::ref(*it);
			if (rtb.Run == run) rtb.Status = DPS::Producer::Canceled;
			  else				run = rtb.Run;
		}
        // sub prio { $b->{Priority}  <=> $a->{Priority};}
		sort(refs_prtb.begin(),refs_prtb.end(),prio);

		for (vector<rtb_dt::iter>::iterator it=refs_prtb.begin(); it!=refs_prtb.end(); ++it) {
			DPS::Producer::RunEvInfo &rtb = rtb_dt::ref(*it);
			unsigned long compar = rtb.uid;
			if (rtb.Status == DPS::Producer::ToBeRerun  || cid.StatusType == DPS::Client::OneRunOnly)
			{
				bool rtStatus = rtb.Status == DPS::Producer::Allocated || rtb.Status == DPS::Producer::Foreign;
				if (rtb.History != DPS::Producer::Failed &&
					(cid.StatusType != DPS::Client::OneRunOnly || (cid.uid == compar  && rtStatus)))
				{
					if((rtb.Status == DPS::Producer::Allocated || cid.uid != 0) && rtb.Status != DPS::Producer::Foreign)
						rtb.Status = DPS::Producer::Processing;
					else
						rtb.Status = DPS::Producer::Allocated;

					if(cid.uid) rtb.cuid = cid.uid;
					  else		rtb.cuid = compar;

					if(rtb.DataMC) {
						if(rtb.TFEvent < rtb.Run || rtb.TLEvent < rtb.Run || rtb.TFEvent-rtb.Run > 1) {
							stringstream s;
							s << "select fetime,letime from dataruns where jid=" << boost::format("%u") %rtb.uid;
							try {
								Connection *dbConn = dbserver::instance().InitOracle();
								if (dbConn) {
									Statement *dbStmt = dbConn->createStatement(s.str());
									ResultSet *dbResultSet = dbStmt->executeQuery();
									while (dbResultSet->next()) {
										rtb.TFEvent = dbResultSet->getInt(1);
										rtb.TLEvent = dbResultSet->getInt(2);
										break;
									}
									dbStmt->closeResultSet(dbResultSet);
									dbConn->terminateStatement(dbStmt);
								}
								else {
									rtb.TFEvent = rtb.Run;
									rtb.TLEvent = rtb.Run + 86400;
								}
								dbserver::instance().CloseOracle();
							}
							catch(SQLException &ex) {
								dbserver::instance().CloseOracle();
								rtb.TFEvent = rtb.Run;
								rtb.TLEvent = rtb.Run + 86400;
							}
						}
					}
					DPS::Producer::RunEvInfo_var orb_rtb = new DPS::Producer::RunEvInfo(rtb);
					rv = orb_rtb._retn();
					DPS::Producer::DSTInfo_var orb_dsti = new DPS::Producer::DSTInfo(pdsti);
					dv = orb_dsti._retn();
					if (_dbgLevel)
						cout << "DBServer_impl::getRunEvInfo-I-(1)uid=" << rtb.uid << ", Status=" << dbserver::strRunStatus(rtb.Status) << endl;
					return;
				}
			}
		}

		for (vector<rtb_dt::iter>::iterator it=refs_prtb.begin(); it!=refs_prtb.end(); ++it) {
			DPS::Producer::RunEvInfo &rtb = rtb_dt::ref(*it);
            if (rtb.Status == DPS::Producer::ToBeRerun || cid.StatusType == DPS::Producer::OneRunOnly) {
            	if (rtb.History == DPS::Producer::Failed &&
            	    (string(cid.HostName).find(rtb.cinfo.HostName) == string::npos || rtb.Status != DPS::Producer::ToBeRerun) &&
            	    (cid.StatusType != DPS::Producer::OneRunOnly ||
            	     (cid.uid == rtb.uid && (rtb.Status == DPS::Producer::Allocated || rtb.Status == DPS::Producer::Foreign))))

            	{
					if (rtb.Status == DPS::Producer::Allocated && rtb.Status != DPS::Producer::Foreign)
						rtb.Status = DPS::Producer::Processing;
					else
						rtb.Status = DPS::Producer::Allocated;
					if (cid.uid != 0) rtb.cuid = cid.uid;
					  else 			  rtb.cuid = rtb.uid;

					if(rtb.DataMC) {
						if(rtb.TFEvent < rtb.Run or rtb.TLEvent < rtb.Run or rtb.TFEvent-rtb.Run > 1) {
							stringstream s;
							s << "select fetime,letime from dataruns where jid=" <<
								 boost::format("%u") %rtb.uid;
							try {
								Connection *dbConn = dbserver::instance().InitOracle();
								if (dbConn) {
									Statement *dbStmt = dbConn->createStatement(s.str());
									ResultSet *dbResultSet = dbStmt->executeQuery();
									while (dbResultSet->next()) {
										rtb.TFEvent = dbResultSet->getInt(1);
										rtb.TLEvent = dbResultSet->getInt(2);
										break;
									}
									dbStmt->closeResultSet(dbResultSet);
									dbConn->terminateStatement(dbStmt);
								}
								else {
									rtb.TFEvent = rtb.Run;
									rtb.TLEvent = rtb.Run + 86400;
								}
								dbserver::instance().CloseOracle();
							}
							catch(SQLException &ex) {
								dbserver::instance().CloseOracle();
								rtb.TFEvent = rtb.Run;
								rtb.TLEvent = rtb.Run + 86400;
							}
						}
					}
					DPS::Producer::RunEvInfo_var orb_rtb = new DPS::Producer::RunEvInfo(rtb);
					rv = orb_rtb._retn();
					DPS::Producer::DSTInfo_var orb_dsti = new DPS::Producer::DSTInfo(pdsti);
					dv = orb_dsti._retn();
					if (_dbgLevel)
						cout << "DBServer_impl::getRunEvInfo-I-(2)uid=" << rtb.uid << ", Status=" << dbserver::strRunStatus(rtb.Status) << endl;
					return;
            	}
            }
 		}

		for (vector<rtb_dt::iter>::iterator it=refs_prtb.begin(); it!=refs_prtb.end(); ++it) {
			DPS::Producer::RunEvInfo &rtb = rtb_dt::ref(*it);
			if (rtb.Status == DPS::Producer::ToBeRerun  || cid.StatusType == DPS::Client::OneRunOnly) {
				if (rtb.History == DPS::Producer::Failed &&
					(cid.StatusType != DPS::Producer::OneRunOnly ||
					(cid.uid == rtb.uid && (rtb.Status == DPS::Producer::Allocated || rtb.Status == DPS::Producer::Foreign))))
				{
					if ((rtb.Status == DPS::Producer::Allocated)) rtb.Status = DPS::Producer::Processing;
					  else										  rtb.Status = DPS::Producer::Allocated;
					if (cid.uid) rtb.cuid = cid.uid;
					  else		 rtb.cuid = rtb.uid;

					if (rtb.DataMC) {
						if (rtb.TFEvent < rtb.Run || rtb.TLEvent < rtb.Run || rtb.TFEvent-rtb.Run > 1) {
							stringstream s;
							s << "select fetime,letime from dataruns where jid=" <<
								 boost::format("%u") %rtb.uid;
							try {
								Connection *dbConn = dbserver::instance().InitOracle();
								if (dbConn) {
									Statement *dbStmt = dbConn->createStatement(s.str());
									ResultSet *dbResultSet = dbStmt->executeQuery();
									while (dbResultSet->next()) {
										rtb.TFEvent = dbResultSet->getInt(1);
										rtb.TLEvent = dbResultSet->getInt(2);
										break;
									}
									dbStmt->closeResultSet(dbResultSet);
									dbConn->terminateStatement(dbStmt);
								}
								else {
									rtb.TFEvent = rtb.Run;
									rtb.TLEvent = rtb.Run + 86400;
								}
								dbserver::instance().CloseOracle();
							}
							catch(SQLException &ex) {
								dbserver::instance().CloseOracle();
								rtb.TFEvent = rtb.Run;
								rtb.TLEvent = rtb.Run + 86400;
							}
						}
					}
					DPS::Producer::RunEvInfo_var orb_rtb = new DPS::Producer::RunEvInfo(rtb);
					rv = orb_rtb._retn();
					DPS::Producer::DSTInfo_var orb_dsti = new DPS::Producer::DSTInfo(pdsti);
					dv = orb_dsti._retn();
					if (_dbgLevel)
						cout << "DBServer_impl::getRunEvInfo-I-(3)uid=" << rtb.uid << ", Status=" << dbserver::strRunStatus(rtb.Status) << endl;
					return;
				}
			}
		}

		for (vector<rtb_dt::iter>::iterator it=refs_prtb.begin(); it!=refs_prtb.end(); ++it) {
			DPS::Producer::RunEvInfo &rtb = rtb_dt::ref(*it);
			if(rtb.uid == cid.uid) {
				cout << "DBServer_impl::getRunEvInfo-W- problem with run allocation: Run=" << rtb.Run <<
						", uid=" << rtb.uid << ", History=" << dbserver::strRunStatus(rtb.History) <<
						", Status=" << dbserver::strRunStatus(rtb.Status) << endl;
				break;
			}
		}
		pdsti.DieHard = 1;

		db_prtb.rewind();
		DPS::Producer::RunEvInfo_var orb_rtb = new DPS::Producer::RunEvInfo(*db_prtb);
		rv = orb_rtb._retn();
		DPS::Producer::DSTInfo_var orb_dsti = new DPS::Producer::DSTInfo(pdsti);
		dv = orb_dsti._retn();
	}
	catch (string &ex) {
		throw DPS::DBProblem(ex.c_str());
	}
}

void DBServer_impl::sendRunEvInfo(const DPS::Producer::RunEvInfo &ne, DPS::Client::RecordChange rc) throw (CORBA::SystemException)
{
	if (_dbgLevel)
		dbserver::debug("DBServer_impl::sendRunEvInfo-I- ",ne,rc);
	dbserver &dbserv = dbserver::instance();

	try {
		bdbRC db(dbserv._db);
		db.connect2(DBO_RTB);
		rtb_dt db_prtb(db,DBO_RTB);

		int len = db_prtb.length();
		switch (rc) {
		case DPS::Client::Update:
			for (int i=0; i<len; i++,db_prtb++)
				if (ne.uid == (*db_prtb).uid) {
					if (_maxFailsHost>0) {
						string sHostName(ne.cinfo.HostName);
						if ((*db_prtb).Status != ne.Status && _ahsFails.find(sHostName) != _ahsFails.end()) {
							if (ne.Status == DPS::Producer::Failed) {
								_ahsFails[sHostName] += _maxFailToleranceHost /(1-_maxFailToleranceHost);
							}
							else if (ne.Status == DPS::Producer::Finished) {
								if (--_ahsFails[sHostName] < 0)
									_ahsFails[sHostName] = 0;
							}
						}
					}
					db_prtb.erase_it();
					db_prtb.insert(const_cast<DPS::Producer::RunEvInfo &>(ne));
					break;
				}
			break;
		case DPS::Client::Create:
			for (int i=0; i<len; i++,db_prtb++)
				if (ne.uid == (*db_prtb).uid) {
					db_prtb.erase_it();
					break;
				}
			db_prtb.insert(const_cast<DPS::Producer::RunEvInfo &>(ne));
			break;
		case DPS::Client::Delete:
			for (int i=0; i<len; i++,db_prtb++)
				if (ne.uid == (*db_prtb).uid) {
					db_prtb.erase_it();
					break;
				}
			break;
		case DPS::Client::ClearCreate:
			db_prtb.erase();
			db_prtb.insert(const_cast<DPS::Producer::RunEvInfo &>(ne));
			break;
		default:
			break;
		}
	}
	catch (string &ex) {
		throw DPS::DBProblem(ex.c_str());
	}
}

int DBServer_impl::getRunsTotal() throw (CORBA::SystemException)
{
	if (_dbgLevel)
		dbserver::debug("DBServer_impl::getRunsTotal-I- ");
    dbserver &dbserv = dbserver::instance();

    try {
		bdbRC db(dbserv._db);
		db.connect2(DBO_RTB);
		rtb_dt db_prtb(db,DBO_RTB);

		int len = db_prtb.length();
		return len;
    }
	catch (string &ex) {
		throw DPS::DBProblem(ex.c_str());
	}
}

int DBServer_impl::getRunsNumber(DPS::Producer::RunStatus status) throw (CORBA::SystemException)
{
	if (_dbgLevel)
		dbserver::debug("DBServer_impl::getRunsNumber-I- ");
    dbserver &dbserv = dbserver::instance();

    try {
		bdbRC db(dbserv._db);
		db.connect2(DBO_RTB);
		rtb_dt db_prtb(db,DBO_RTB);

		int result = 0;
		for (int i=0,len=db_prtb.length(); i<len; i++,db_prtb++) {
			DPS::Producer::RunStatus rs = (*db_prtb).Status;
			if (rs == status)
				result++;
		}
		return result;
    }
	catch (string &ex) {
		throw DPS::DBProblem(ex.c_str());
	}
}

void DBServer_impl::sendDSTEnd(const DPS::Client::CID & cid, const  DPS::Producer::DST &ne, DPS::Client::RecordChange rc)throw (CORBA::SystemException)
{
	if (_dbgLevel)
		dbserver::debug("DBServer_impl::sendDSTEnd-I- ",cid,ne,rc);
    dbserver &dbserv = dbserver::instance();

	CharStringToken first(ne.Name,":");
	CharStringToken last(ne.Name,"/");
	if (first.tokens() == 0 || last.tokens() == 0)
		return;

    try
    {
		bdbRC db(dbserv._db);
		db.connect2(DBO_DSTS);
		dsts_dt db_pdsts(db,DBO_DSTS);

		switch(rc)
		{
		case DPS::Client::Update: {
			for (int i=0,len=db_pdsts.length(); i<len; i++,db_pdsts++) {
				string name((*db_pdsts).Name);
				if (name.find(first.token(0)) != string::npos &&
					name.find(last.token(last.tokens()-1)) != string::npos)
				{
					db_pdsts.erase_it();
					db_pdsts.insert(const_cast<DPS::Producer::DST &>(ne));
					break;
				}
			}
			break;
		}
		case DPS::Client::Create: {
			for (int i=0,len=db_pdsts.length(); i<len; i++,db_pdsts++) {
				string name((*db_pdsts).Name);
				if (name.find(first.token(0)) != string::npos &&
					name.find(last.token(last.tokens()-1)) != string::npos)
				{
					db_pdsts.erase_it();
					break;
				}
			}
			db_pdsts.insert(const_cast<DPS::Producer::DST &>(ne));
			break;
		}
		case DPS::Client::Delete: {
			for (int i=0,len=db_pdsts.length(); i<len; i++,db_pdsts++) {
				string name((*db_pdsts).Name);
				if (name.find(first.token(0)) != string::npos &&
					name.find(last.token(last.tokens()-1)) != string::npos)
				{
				    db_pdsts.erase_it();
					break;
				}
			}
			break;
		}
		case DPS::Client::ClearCreate:
			db_pdsts.erase();
			db_pdsts.insert(const_cast<DPS::Producer::DST &>(ne));
			break;
		}
    }
	catch (string &ex) {
		throw DPS::DBProblem(ex.c_str());
	}
}

int DBServer_impl::getDSTS(const DPS::Client::CID & cid, DPS::Producer::DSTS_out dsts)throw (CORBA::SystemException)
{
	if (_dbgLevel)
		dbserver::debug("DBServer_impl::getDSTS-I- ",cid);
	return 0;
}

int DBServer_impl::getSplitDSTS(const DPS::Client::CID & cid, DPS::Producer::DSTS_out dsts, unsigned int pos, unsigned int len, DPS::Producer::TransferStatus &st) throw (CORBA::SystemException)
{
	if (_dbgLevel)
		dbserver::debug("DBServer_impl::getSplitDSTS-I- ",cid);
	return 0;
}

int DBServer_impl::getDSTSR(const DPS::Client::CID & cid, int  run,DPS::Producer::DSTS_out dsts)throw (CORBA::SystemException)
{
	if (_dbgLevel)
		dbserver::debug("DBServer_impl::getDSTSR-I- ",cid);
	return 0;
}

void DBServer_impl::sendDSTS(const DPS::Client::CID & cid, const DPS::Producer::DSTS & dsts)throw (CORBA::SystemException)
{
	if (_dbgLevel)
		dbserver::debug("DBServer_impl::sendDSTS-I- ",cid);
}

int DBServer_impl::sendSplitDSTS(const DPS::Client::CID &cid, const DPS::Producer::DSTS &acs, DPS::Producer::TransferStatus &st)throw (CORBA::SystemException)
{
	if (_dbgLevel)
		dbserver::debug("DBServer_impl::sendSplitDSTS-I- ",cid);
	return 0;
}

int DBServer_impl::getRun(const DPS::Client::CID &cid, const DPS::Producer::FPath & fpath, DPS::Producer::RUN_out run,DPS::Producer::TransferStatus & st)throw (CORBA::SystemException,DPS::Producer::FailedOp)
{
	if (_dbgLevel)
		dbserver::debug("DBServer_impl::getRun-I- ",cid);
	return 0;
}

int DBServer_impl::sendFile(const DPS::Client::CID &cid,  DPS::Producer::FPath & fpath, const  DPS::Producer::RUN & file,DPS::Producer::TransferStatus & st)throw (CORBA::SystemException,DPS::Producer::FailedOp)
{
	if (_dbgLevel)
		dbserver::debug("DBServer_impl::sendFile-I- ",cid);
	return 0;
}

void DBServer_impl::ping()throw (CORBA::SystemException)
{
	if (_dbgLevel)
		dbserver::debug("DBServer_impl::ping-I- ");
}

void DBServer_impl::pingp()throw (CORBA::SystemException)
{
	if (_dbgLevel)
		dbserver::debug("DBServer_impl::pingp-I- ");
}

char * DBServer_impl::getDBFilePath(const DPS::Client::CID &cid)throw (CORBA::SystemException)
{
	if (_dbgLevel)
		dbserver::debug("DBServer_impl::getDBFilePath-I- ",cid);
    dbserver &dbserv = dbserver::instance();

    return CORBA_string_dup(string(dbserv._db->env_path()+"/"+dbserv._db->db_file()).c_str());
}

int DBServer_impl::getRunEvInfoSPerl(const DPS::Client::CID &cid, DPS::Producer::RES_out res, unsigned int  maxrun, unsigned  int &maxrun1)throw (CORBA::SystemException)
{
	if (_dbgLevel)
		dbserver::debug("DBServer_impl::getRunEvInfoSPerl-I- ",cid);
    dbserver &dbserv = dbserver::instance();

	CORBA::ULong maxr = 0;
	try {
		bdbRC db(dbserv._db);
		db.connect2(DBO_RTB);
		rtb_dt db_prtb(db,DBO_RTB);
		int len = db_prtb.length();

		DPS::Producer::RES_var orb_prtb = new DPS::Producer::RES();
		orb_prtb->length(len);
		for (int i=0; i<len; db_prtb++)
			orb_prtb[i++] = *db_prtb;
		res = orb_prtb._retn();
		return len;
	}
	catch (string &ex) {
		throw DPS::DBProblem(ex.c_str());
	}
}

int DBServer_impl::getSplitRunEvInfoSPerl(const DPS::Client::CID &cid, DPS::Producer::RES_out res, unsigned int pos, unsigned int len, unsigned int  maxrun, unsigned  int &maxrun1)throw (CORBA::SystemException)
{
	if (_dbgLevel)
		dbserver::debug("DBServer_impl::getSplitRunEvInfoSPerl-I- ",cid);
	return 0;
}

void DBServer_impl::sendACPerl(const DPS::Client::CID &cid,  const DPS::Client::ActiveClient & ac,DPS::Client::RecordChange rc) throw (CORBA::SystemException)
{
	if (_dbgLevel)
		dbserver::debug("DBServer_impl::sendACPerl-I- ",cid,ac,rc);
	dbserver &dbserv = dbserver::instance();

	try {
    	dbTables type;
    	switch(cid.Type) {
    	case DPS::Client::Server:
			type = DBO_ASL;
			break;
    	case DPS::Client::Producer:
			type = DBO_ACL;
			break;
    	case DPS::Client::DBServer:
			type = DBO_ADBSL;
			break;
    	default:
			string ex = string("Unable to sendACPerl for ") + dbserver::strClientType(cid.Type);
			throw DPS::DBProblem(ex.c_str());
		}
		bdbRC db(dbserv._db);
		db.connect2(type);
		ac_dt db_dbacs(db,type);

	    int len = db_dbacs.length();
		switch (rc) {
		case DPS::Client::Update:
		    for (int i=0; i<len; i++,db_dbacs++)
		    	if (cid.uid == (*db_dbacs).id.uid) {
		    		db_dbacs.erase_it();
				    db_dbacs.insert(const_cast< DPS::Client::ActiveClient &>(ac));
				    break;
		    	}
			break;
		case DPS::Client::Create:
		    for (int i=0; i<len; i++,db_dbacs++)
		    	if (cid.uid == (*db_dbacs).id.uid) {
		    		db_dbacs.erase_it();
				    break;
		    	}
		    db_dbacs.insert(const_cast< DPS::Client::ActiveClient &>(ac));
			break;
		case DPS::Client::Delete:
		    for (int i=0; i<len; i++,db_dbacs++)
		    	if (cid.uid == (*db_dbacs).id.uid) {
		    		db_dbacs.erase_it();
		    		break;
		    	}
			break;
		case DPS::Client::ClearCreate:
		    db_dbacs.erase();
		    db_dbacs.insert(const_cast< DPS::Client::ActiveClient &>(ac));
			break;
		default:
			string ex = string("Unknown sendACPerl request");
			throw DPS::DBProblem(ex.c_str());
		}
	}
	catch (string &ex) {
		throw DPS::DBProblem(ex.c_str());
	}
}

int  DBServer_impl::getFreeHost(const DPS::Client::CID & cid, DPS::Client::ActiveHost_out host)throw (CORBA::SystemException)
{
	if (_dbgLevel)
		dbserver::debug("DBServer_impl::getFreeHost-I- ",cid);
    dbserver &dbserv = dbserver::instance();

    try
    {
		DPS::Client::ActiveHost_var orb_result;
		bdbRC db(dbserv._db);

		if (cid.Type == DPS::Client::Server)
		{
			db.connect2(DBO_AHLS);
			ah_dt db_ahs(db,DBO_AHLS);
			bdbRC db2(db);
			db2.connect2(DBO_NSL);
			nc_dt db_ncs(db2,DBO_NSL);
			bdbRC db3(db);
			db3.connect2(DBO_ASL);
			ac_dt db_acs(db3,DBO_ASL);

			if (db_ncs.length() > 0 && db_acs.length() < (*db_ncs).MaxClients) {
				vector<ah_dt::iter> refs_ahs;
				for (int i=0,len=db_ahs.length(); i<len; i++,db_ahs++)
					refs_ahs.push_back(db_ahs._iter);
				stable_sort(refs_ahs.begin(),refs_ahs.end(),Clock);

				for (vector<ah_dt::iter>::iterator it=refs_ahs.begin(); it!=refs_ahs.end(); ++it) {
					DPS::Client::ActiveHost &ahl = ah_dt::ref(*it);
					if (ahl.Status == DPS::Client::NoResponse || ahl.Status == DPS::Client::InActive)
						continue;
					if (ahl.ClientsRunning < ahl.ClientsAllowed) {
						ahl.Status = DPS::Client::InActive;
						sendAH(cid,ahl,DPS::Client::Update);
						orb_result = new DPS::Client::ActiveHost(ahl);
						host = orb_result._retn();
						return 1;
					}
				}
			}
			db_ahs.rewind();
			orb_result = new DPS::Client::ActiveHost(*db_ahs);
			host = orb_result._retn();
			return 0;
		}
		else if (cid.Type == DPS::Client::Producer)
		{
			typedef vector<sortAH>::iterator ahsit;
			
			db.connect2(DBO_NHL);
			nh_dt db_pnhs(db,DBO_NHL);
			bdbRC db2(db);
			db2.connect2(DBO_AHLP);
			ah_dt db_pahs(db2,DBO_AHLP);
			bdbRC db3(db);
			db3.connect2(DBO_NCL);
			nc_dt db_pncs(db3,DBO_NCL);
			bdbRC db4(db);
			db4.connect2(DBO_ACL);
			ac_dt db_pacs(db4,DBO_ACL);
			bdbRC db5(db);
			db5.connect2(DBO_RTB);
			rtb_dt db_prtb(db5,DBO_RTB);

			int result = 0;
			vector<DPS::Client::ActiveHost> cash_ahs;
			if (db_pncs.length() > 0 && db_pacs.length() < (*db_pncs).MaxClients) {
				vector<sortAH> refs_ahs;

				for (int i=0,len=db_pahs.length(); i<len; i++,db_pahs++) 
					refs_ahs.push_back(sortAH(db_pahs._iter));
				
				int runsToRerun = 0;
				for (int i=0,len=db_prtb.length(); i<len; i++,db_prtb++) {
					if ((*db_prtb).Status == DPS::Producer::Allocated ||
						(*db_prtb).Status == DPS::Producer::ToBeRerun ||
						(*db_prtb).Status == DPS::Producer::Processing)
							runsToRerun++;
				}
				if (db_pacs.length() < runsToRerun) {
					for (ahsit it=refs_ahs.begin(); it!=refs_ahs.end(); ++it) {
						DPS::Client::ActiveHost &ahl = it->ah();
						string sHostName(ahl.HostName);
							
						// don't take hosts with too many failed runs
						if (_maxFailsHost>0 && ahl.Status != DPS::Client::InActive) {
							if (_ahsFails[sHostName] >= _maxFailsHost) {
								if (_dbgLevel)
									cout<<"DBServer_impl::getFreeHost-W- "<<_ahsFails[sHostName]<<" failed found "<<sHostName<<endl;
								it->_Status = ahl.Status = DPS::Client::InActive;
								cash_ahs.push_back(ahl);
							}
						}

						// don't take hosts with too many suspended clients
						db_pacs.rewind();
						for (int i=0,len=db_pacs.length(); i<len; i++,db_pacs++) {
							DPS::Client::ActiveClient &acl = *db_pacs;
							if (acl.Status == DPS::Client::Submitted && sHostName == string(acl.id.HostName))
								it->_ClientsSuspened++;
						}
						if (it->_ClientsSuspened && it->_Status != DPS::Client::InActive) 
							it->_Status = ahl.Status = DPS::Client::NoResponse;
						
						// prioritize host also taken into acc max cpu number
						db_pnhs.rewind();
						for (int i=0,len=db_pnhs.length(); i<len; i++,db_pnhs++) {
							DPS::Client::NominalHost &nhl = *db_pnhs;

							if(sHostName == string(nhl.HostName)) {
								short ca = ahl.ClientsAllowed - ahl.ClientsRunning;
								if (ca>0 && ca<nhl.CPUNumber)
									it->_ClientsAllowed = ahl.ClientsRunning+ca;
								break;
							}
						}
					}
					stable_sort(refs_ahs.begin(),refs_ahs.end(),Clock2);

					for (ahsit it=refs_ahs.begin(); it!=refs_ahs.end(); ++it) {
						DPS::Client::ActiveHost &ahl = it->ah();
						string sHostName(ahl.HostName);

						if(it->_Status ==  DPS::Client::NoResponse) {
							short nsusp = it->_ClientsSuspened;
							if(_maxSuspHost && nsusp >= _maxSuspHost) {
								if (_dbgLevel)
									cout<<"DBServer_impl::getFreeHost-I- "<<nsusp<<" submitted found "<<sHostName<<endl;
							}
							else
								it->_Status = ahl.Status = DPS::Client::OK;
						}
						if (it->_Status != DPS::Client::NoResponse && it->_Status != DPS::Client::InActive) {
							if (ahl.ClientsRunning < 0)
								ahl.ClientsRunning = 0;
							if (ahl.ClientsRunning < ahl.ClientsAllowed) {
								ahl.Status = DPS::Client::InActive;
								orb_result = new DPS::Client::ActiveHost(ahl);
								host = orb_result._retn();
								cout << "DBServer_impl::getFreeHost-I- host chosen " << host->HostName 
									 << ": running=" << ahl.ClientsRunning << ", allowed=" << ahl.ClientsAllowed << endl;
								cash_ahs.push_back(ahl);
								result = 1;
								break;
							}
						}
					}
				}
			}
			if (result < 1) {
				db_pahs.rewind();
				orb_result = new DPS::Client::ActiveHost(*db_pahs);
				host = orb_result._retn();
				if (_dbgLevel)
					cout<<"DBServer_impl::getFreeHost-W- no host may be chosen"<<endl;
			}
			for (unsigned i=0,l=cash_ahs.size(); i<l; i++)
				sendAH(cid,cash_ahs[i],DPS::Client::Update);
			return result;
		}
		else {
			string ex = string("Unable to getahs for ") + dbserver::strClientType(cid.Type);
			throw DPS::DBProblem(ex.c_str());
		}
	}
	catch (string &ex) {
		throw DPS::DBProblem(ex.c_str());
	}
    return 1;
}

int  DBServer_impl::getFreeHostN(const DPS::Client::CID & cid) throw (CORBA::SystemException)
{
	if (_dbgLevel) {
		stringstream s;
		s<<"DBServer_impl::getFreeHostN-I- _maxSuspHost="<<boost::format("%d") %_maxSuspHost<<
			 " _maxFailsHost=" << boost::format("%.3f") %_maxFailsHost<<
			 " _maxFailToleranceHost="<<boost::format("%.3f") %_maxFailToleranceHost<<", ";
		dbserver::debug(s.str().c_str(),cid);
	}
    dbserver &dbserv = dbserver::instance();

	bdbRC db(dbserv._db);
    try
    {
		if (cid.Type == DPS::Client::Server)
		{
			db.connect2(DBO_AHLS);
			ah_dt db_ahs(db,DBO_AHLS);
			bdbRC db2(db);
			db2.connect2(DBO_NSL);
			nc_dt db_ncs(db2,DBO_NSL);
			bdbRC db3(db);
			db3.connect2(DBO_ASL);
			ac_dt db_acs(db3,DBO_ASL);

			if (db_ncs.length() > 0 && db_acs.length() < (*db_ncs).MaxClients) {
				vector<ah_dt::iter> refs_ahs;
				for (int i=0,len=db_ahs.length(); i<len; i++,db_ahs++)
					refs_ahs.push_back(db_ahs._iter);
				stable_sort(refs_ahs.begin(),refs_ahs.end(),Clock);

				for (vector<ah_dt::iter>::iterator it=refs_ahs.begin(); it!=refs_ahs.end(); ++it) {
					DPS::Client::ActiveHost &ahl = ah_dt::ref(*it);
					if (ahl.Status != DPS::Client::NoResponse && ahl.Status != DPS::Client::InActive)
						if (ahl.ClientsRunning < ahl.ClientsAllowed)
							return 1;
				}
			}
			return 0;
		}
		else if (cid.Type == DPS::Client::Producer)
		{
			typedef vector<sortAH>::iterator ahsit;
			
			db.connect2(DBO_NHL);
			nh_dt db_pnhs(db,DBO_NHL);
			bdbRC db2(db);
			db2.connect2(DBO_AHLP);
			ah_dt db_pahs(db2,DBO_AHLP);
			bdbRC db3(db);
			db3.connect2(DBO_NCL);
			nc_dt db_pncs(db3,DBO_NCL);
			bdbRC db4(db);
			db4.connect2(DBO_ACL);
			ac_dt db_pacs(db4,DBO_ACL);
			bdbRC db5(db);
			db5.connect2(DBO_RTB);
			rtb_dt db_prtb(db5,DBO_RTB);

			short maxclients = (*db_pncs).MaxClients;
			if (maxclients <= 0)
				if (_dbgLevel)
					cout<<"DBServer_impl::getFreeHostN-W- "<<(*db_pncs).HostName<<" maxclients="<<maxclients<<endl;

			if (db_pncs.length() > 0 && db_pacs.length() < (*db_pncs).MaxClients) {
				int runstorerun = 0;
				int runstotal = 0;
				for (int i=0,len=db_prtb.length(); i<len; i++,db_prtb++) {
					DPS::Producer::RunStatus status = (*db_prtb).Status;
					if (status == DPS::Producer::ToBeRerun) {
						runstorerun++;
						runstotal++;
					}
					else if (status == DPS::Producer::Processing || status == DPS::Producer::Allocated)
						runstotal++;
				}
				vector<sortAH> refs_ahs;
				for (int i=0,len=db_pahs.length(); i<len; i++,db_pahs++) 
					refs_ahs.push_back(sortAH(db_pahs._iter));
				stable_sort(refs_ahs.begin(),refs_ahs.end(),Clock2);
				
				for (ahsit it=refs_ahs.begin(); it!=refs_ahs.end(); ++it) {
					DPS::Client::ActiveHost &ahl = it->ah();
					string sHostName(ahl.HostName);
					if (ahl.Status == DPS::Producer::NoResponse || ahl.Status == DPS::Producer::InActive ||
					ahl.ClientsAllowed <= 0 || ahl.ClientsRunning >= ahl.ClientsAllowed)
						continue;
					int result = static_cast<int>((db_pacs.length()<runstotal && runstorerun>0));
					if (result) {
						if (_dbgLevel)
							cout<<"DBServer_impl::getFreeHostN-I- free host found "<<sHostName<<
							": status="<<ahl.Status<<", running="<<ahl.ClientsRunning<<
							", allowed="<<ahl.ClientsAllowed<<endl;
					}
					else {
						if (_dbgLevel)
							cout<<"DBServer_impl::getFreeHostN-I- skipped, runstorerun="<<runstorerun<<
							", candidate "<<sHostName<<": status="<<ahl.Status<<
							", running="<<ahl.ClientsRunning<<", allowed="<<ahl.ClientsAllowed<<endl;
					}
					return result;
				}
				if (_dbgLevel)
					cout << "DBServer_impl::getFreeHostN-I- no free host found " << endl;
			}
			return 0;
		}
		else {
			string ex = string("Unable to getahs for ") + dbserver::strClientType(cid.Type);
			throw DPS::DBProblem(ex.c_str());
		}
    }
	catch (string &ex) {
		throw DPS::DBProblem(ex.c_str());
	}
    return 0;
}

void DBServer_impl::clearACS( DPS::Client::ClientType type)throw (CORBA::SystemException)
{
	if (_dbgLevel)
		dbserver::debug("DBServer_impl::clearACS-I- ");
}

void DBServer_impl::clearNCS( DPS::Client::ClientType type)throw (CORBA::SystemException)
{
	if (_dbgLevel)
		dbserver::debug("DBServer_impl::clearNCS-I- ");
}

void DBServer_impl::clearAHS( DPS::Client::ClientType type)throw (CORBA::SystemException)
{
	if (_dbgLevel)
		dbserver::debug("DBServer_impl::clearAHS-I- ");
}

void DBServer_impl::clearNHS()throw (CORBA::SystemException)
{
	if (_dbgLevel)
		dbserver::debug("DBServer_impl::clearNHS-I- ");
}

void DBServer_impl::clearDST( DPS::Producer::DSTType type)throw (CORBA::SystemException)
{
	if (_dbgLevel)
		dbserver::debug("DBServer_impl::clearDST-I- ");
}

void DBServer_impl::clearDSTI()throw (CORBA::SystemException)
{
	if (_dbgLevel)
		dbserver::debug("DBServer_impl::clearDSTI-I- ");
}

void DBServer_impl::clearRunEvInfo( DPS::Producer::RunStatus status)throw (CORBA::SystemException)
{
	if (_dbgLevel)
		dbserver::debug("DBServer_impl::clearRunEvInfo-I- ");
}

void DBServer_impl::sendmessage(const char * add, const char * sub, const char *mes)throw (CORBA::SystemException)
{
	if (_dbgLevel)
		dbserver::debug("DBServer_impl::sendmessage-I- ");
}
