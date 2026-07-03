#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <time.h>
#include <math.h>
#include <string>
#include <sstream>

#include "ManMapper.hxx"
#include "oraRC.h"
#include "dbserver.hxx"
#include "bdbTraits.hxx"

using std::cout;
using std::endl;
using std::string;
using std::stringstream;

// stream redirection, low level
#include "mstd.h"
static mstd::mstreambuf mcout(cout);
static mstd::mstreambuf mcerr(cerr);

int _dbgLevel = 1;
int _maxSuspHost = 0;
double _maxFailsHost = 0;
double _maxFailToleranceHost = 0.5;
map<string,double> _ahsFails;

dbserver& _dbserver = dbserver::instance();

// to find out all applicable arguments, search for hasArg invocation
const char *hasArg(int argc, char *argv[], const char *wanted)
{
	for (int i=1; i<argc; i++)
		if (string(argv[i]).find(wanted) == 0)
			return &argv[i][strlen(wanted)];
	return 0;
}

int main(int argc, char *argv[])
{
 	char *tns = getenv("TNS_ADMIN");
	if(!tns || !strlen(tns))
		setenv("TNS_ADMIN","/afs/cern.ch/project/oracle/admin",1);

	bdbTraits::instantiate_all();
	if (dbserver::instance().Init(argc,argv))
		dbserver::instance().RunOrbit();
	return 0;
}

//_____________________________________________________________________
// dbserver

bool dbserver::Init(int argc, char *argv[]) throw(string &)
{
	if (_dbgLevel)
		cout << "dbserver::Init-I- " << endl;

	string siord;
	if (_orb == 0) 	{
		const char *ptmp = getenv("TMPDIR");
		const char *iorip = hasArg(argc,argv,"--ORBIIOPIPSock=");

		// POA manager
	    if (iorip) {
	    	if (ptmp) chdir(ptmp);
    		remove(iorip);

			CORBA_ORB orb = CORBA_OBJECT_NIL;
			PortableServer_POA poa = CORBA_OBJECT_NIL;
			PortableServer_POAManager mgr = CORBA_OBJECT_NIL;
			PortableServer_POA child_poa = CORBA_OBJECT_NIL;
			CORBA_Environment  ev;
			CORBA_exception_init(&ev);
			CORBA_PolicyList *policies;

			try {
				 orb = CORBA_ORB_init(&argc,argv,"orbit-local-non-threaded-orb",&ev);
				 poa = (PortableServer_POA)CORBA_ORB_resolve_initial_references(orb,"RootPOA",&ev);
				 mgr = PortableServer_POA__get_the_POAManager(poa,&ev);

				 policies = CORBA_PolicyList__alloc();
				 policies->_maximum = 1;
				 policies->_length  = 1;
				 policies->_buffer  = CORBA_PolicyList_allocbuf(1);
				 CORBA_sequence_set_release (policies, CORBA_TRUE);
				 policies->_buffer[0]=(CORBA_Policy)PortableServer_POA_create_lifespan_policy(poa,PortableServer_TRANSIENT,&ev);

				 child_poa = PortableServer_POA_create_POA(poa,"RootPOA",mgr,policies,&ev);
				 CORBA_Policy_destroy(policies->_buffer[0],&ev);
				 CORBA_free(policies);
				 CORBA_Object_release((CORBA_Object)mgr,&ev);
			}
			catch (CORBA::SystemException &ex) {
				 cerr <<"AMSServer::AMSServer-E-CorbaSysExeceptionOnInit "<<ex._orbitcpp_get_repoid()<<endl;
			}
			_orb = new CORBA::ORB(orb);
			_poa = new PortableServer::POA(child_poa);
			_mgr= _poa->the_POAManager();
			_mgr->activate();
	    }
	    else {
//!!!!!	    	_orb = CORBA::ORB_init(argc,argv,"orbit-local-mt-orb");
			_orb = CORBA::ORB_init(argc,argv,"orbit-local-non-threaded-orb");
			CORBA::Object_var initRef = _orb->resolve_initial_references("RootPOA");
			_poa = PortableServer::POA::_narrow(initRef);
			_mgr = _poa->the_POAManager();
			_mgr->activate();
	    }
		// servant
	    PortableServer::ObjectId_var objid = _poa->activate_object(&_servant);
	    CORBA::Object_var objvar = _poa->id_to_reference(objid);
	    CORBA::String_var ref = _orb->object_to_string(objvar);
	    // here is our IOR
	    siord = (const char *)ref;
	    cout << "dbserver::Init-I- initialize my " << siord << endl;
	}

	DPS::Server_var arsref;
	const char *iors = hasArg(argc,argv,"-i");
	if (iors)
		try {
            CORBA::String_var ref = iors;
            arsref = DPS::Server::_narrow(_orb->string_to_object(ref));
		}
		catch(const CORBA::Exception& ex) {
			cout << "dbserver::Init-F- unable to connect to amsprodserver " << endl;
			return false;
		}
	else {
		cout << "dbserver::Init-F- unable to get IOR from amsprodserver " << endl;
		return false;
	}

	string host = common::hostname();
	if (host.find(".") == string::npos)
		host += ".cern.ch";
	_cid.HostName = host.c_str();
	const char *uid = hasArg(argc,argv,"-U");
	if (uid) _cid.uid = boost::lexical_cast<unsigned long>(uid);
	  else	 _cid.uid = 0;
	_cid.pid = getpid();
	_cid.ppid = 0;
	_cid.threads = 1;
	_cid.threads_change = 0;
	_cid.Type = DPS::Client::DBServer;
	_cid.StatusType = DPS::Client::Permanent;
	_cid.Status = DPS::Client::NOP;
	_cid.Interface = "default";
	_cid.Mips = 1000;

	_ac.id = _cid;
	DPS::Client::ActiveRef arf;
	arf.IOR = siord.c_str();
	arf.Interface = _cid.Interface;
	arf.Type = _cid.Type;
	arf.uid = _cid.uid;
	time_t now = time(0);
	_ac.Start = now;
	_ac.LastUpdate = now;
	_ac.TimeOut = 60;
	_ac.Status = DPS::Client::Active;
	_ac.StatusType = DPS::Client::Permanent;
	_ars.length(3);
	// _ars[1,2] should be set below
	_ars[0] = arf;

	if (!UpdateARS(arsref)) {
		cout << "dbserver::Init-F- unable to get ARS from amsprodserver " << endl;
		// Comment next line for local debugging
		return false;
	}
	// finally
	_ac.ars.length(1);
	_ac.ars[0] = arf;

	const char *amsprodlogdir = hasArg(argc,argv,"-A");
	const char *amsprodlogfile = hasArg(argc,argv,"-F");
	const char *repairdb = hasArg(argc,argv,"-z");
	if (!InitDBFile(amsprodlogdir,amsprodlogfile,repairdb)) {
		if (_db) {
			string sdbfile = _db->env_path() + "/" + _db->db_file();
			cout<<"dbserver::Init-F- unable to open DB "<<sdbfile<<endl;
		} else
			cout<<"dbserver::InitDBFile-F- bad input args, full stop"<<endl;
		return false;
	}

	DPS::Client::ARS ars = dbserver::instance()._ars;
	string siors, siorp;
	for (unsigned i=0; i<ars.length(); i++)
	{
		DPS::Client::ActiveRef arf = ars[i];
		switch (arf.Type) {
		case DPS::Client::DBServer:
			siord = arf.IOR;
			break;
		case DPS::Client::Server:
			siors = arf.IOR;
			break;
		case DPS::Client::Producer:
			siorp = arf.IOR;
			break;
		default:
			break;
		}
	}
	if (!siors.length() || !siorp.length() || !siord.length()) {
		cout << "dbserver::Init-F- not all IORs found" << endl;
		return false;
	}

    try
    {
		string sdbfile = _db->env_path() + "/" + _db->db_file();
		int datamc = 0;
		const char *pdatamc = hasArg(argc,argv,"-v");
		if (pdatamc)
			datamc = boost::lexical_cast<int>(pdatamc)-4;
		stringstream s;
		Connection *occiConn = 0;
		try {
			s << "delete from Servers where dbfilename='" << sdbfile << "' and datamc=" << boost::format("%d") %datamc;
			if (_dbgLevel)
				cout << "dbserver::Init-I- " << s.str() << endl;
			occiConn = dbserver::instance().InitOracle(argc,argv);
			if (occiConn == 0) {
				std::cout << "dbserver::Init-F- oracle instantiation error " << endl;
				return false;
			}
			Statement *occiStmt = occiConn->createStatement(s.str());
			occiStmt->setAutoCommit(TRUE);
			ResultSet *occiResultSet = occiStmt->executeQuery();
			occiStmt->closeResultSet(occiResultSet);
			occiConn->terminateStatement(occiStmt);
		}
		catch (SQLException &ex) {
			std::cout << "dbserver::Init-E- occi error " << ex.what() << endl;
		}
		s.str("");
		try {
	        s << boost::format("insert into Servers values('%s','%s','%s','%s','Active',") %sdbfile %siors %siorp %siord <<
	        	 boost::format("%d,%d,%d,'%s')") %_ac.Start %time(0) %datamc %_cid.HostName;
	        if (_dbgLevel)
	        	cout << "dbserver::Init-I- " << s.str() << endl;
			Statement *occiStmt = occiConn->createStatement(s.str());
			occiStmt->setAutoCommit(TRUE);
			ResultSet *occiResultSet = occiStmt->executeQuery();
			occiStmt->closeResultSet(occiResultSet);
			occiConn->terminateStatement(occiStmt);
		    dbserver::instance().CloseOracle();
		}
		catch (SQLException &ex) {
		    dbserver::instance().CloseOracle();
			std::cout << "dbserver::Init-F- occi error " << ex.what() << endl;
	    	return false;
		}
    }
    catch(boost::bad_lexical_cast &ex)
    {
    	cout << "dbserver::Init-F- bad sql query cast" << endl;
    	return false;
    }

	if (SendId()) {
		cout << "dbServer::Init-I- " << argv[0] << " initialization completed OK" << endl;
		return true;
	}
	else {
		cout << "dbServer::Init-F- " << argv[0] << " initialization failured" << endl;
		return false;
	}
}

void dbserver::RunOrbit() { _orb->run(); }

bool dbserver::UpdateEverything(bool sendback, const char *repairdb)
{
	if (sendback)
		cout << "dbserver::UpdateEverything-I- with sendback" << endl;
	else
		cout << "dbserver::UpdateEverything-I- " << endl;
    try
	{
    	DPS::Client::CID cid = mycid(DPS::Client::Server);
    	bdbRC db(_db);
    	{
    		DPS::Client::AHS_var orb_ahs;
    		db.connect2(DBO_AHLS,true);
			ah_dt db_ahs(db,DBO_AHLS);
   			cout << "dbserver::UpdateEverything-I- Server::getAHS" << endl;
			int len = _arsref->getAHS(cid,orb_ahs);
			db_ahs.erase();
			for (int i=0; i<len; i++)
				db_ahs.insert(orb_ahs[i]);
    	}

		CORBA::ULong maxc = 0;
		{
			DPS::Client::ACS_var orb_acs;
    		db.connect2(DBO_ASL,true);
			ac_dt db_acs(db,DBO_ASL);
   			cout << "dbserver::UpdateEverything-I- Server::getACS" << endl;
			int len = _arsref->getACS(cid,orb_acs,maxc);
			db_acs.erase();
			for (int i=0; i<len; i++)
				db_acs.insert(orb_acs[i]);
		}
		{
			DPS::Client::NHS_var orb_nhs;
    		db.connect2(DBO_NHL,true);
			nh_dt db_nhs(db,DBO_NHL);
			if (sendback) {
    			cout << "dbserver::UpdateEverything-I- Server::sendNHS" << endl;
				int len = db_nhs.length();
				orb_nhs = new DPS::Client::NHS;
				orb_nhs->length(len);
				for (int i=0; i<len; db_nhs++)
					orb_nhs[i++] = *db_nhs;
				_arsref->sendNHS(cid,orb_nhs);
			}
			else {
    			cout << "dbserver::UpdateEverything-I- Server::getNHS" << endl;
				int len = _arsref->getNHS(cid,orb_nhs);
				db_nhs.erase();
				for (int i=0; i<len; i++)
					db_nhs.insert(orb_nhs[i]);
			}
		}
		{
			DPS::Client::NCS_var orb_ncs;
    		db.connect2(DBO_NSL,true);
			nc_dt db_ncs(db,DBO_NSL);
			if (sendback) {
    			cout << "dbserver::UpdateEverything-I- Server::sendNCS" << endl;
				int len = db_ncs.length();
				orb_ncs = new DPS::Client::NCS;
				orb_ncs->length(len);
				for (int i=0; i<len; db_ncs++)
					orb_ncs[i++] = *db_ncs;
				_arsref->sendNCS(cid,orb_ncs);
			}
			else {
    			cout << "dbserver::UpdateEverything-I- Server::getNC" << endl;
				int len = _arsref->getNC(cid,orb_ncs);
				db_ncs.erase();
				for (int i=0; i<len; i++)
					db_ncs.insert(orb_ncs[i]);
			}
		}
		{
			DPS::Client::NCS_var orb_nks;
    		db.connect2(DBO_NKL,true);
			nc_dt db_nks(db,DBO_NKL);
			if (sendback) {
    			cout << "dbserver::UpdateEverything-I- Server::sendNKS" << endl;
				int len = db_nks.length();
				orb_nks = new DPS::Client::NCS;
				orb_nks->length(len);
				for (int i=0; i<len; db_nks++)
					orb_nks[i++] = *db_nks;
				_arsref->sendNKS(cid,orb_nks);
			}
			else {
    			cout << "dbserver::UpdateEverything-I- Server::getNK" << endl;
				int len = _arsref->getNK(cid,orb_nks);
				db_nks.erase();
				for (int i=0; i<len; i++)
					db_nks.insert(orb_nks[i]);
			}
		}
		{
			DPS::Server::SS_var orb_env;
    		db.connect2(DBO_ENV,true);
			env_dt db_env(db,DBO_ENV);
			if (sendback) {
				cout << "dbserver::UpdateEverything-I- Server::getEnv" << endl;
				int orb_len = _arsref->getEnv(cid,orb_env);
				for (int j=0; j<orb_len; j++) {
					string sorb((const char *)orb_env[j]);
					CharStringToken storb(sorb,"=");
					bool found = false;
					int len = db_env.length();
					for (int i=0; i<len; i++,db_env++) {
						string sdb(*db_env);
						CharStringToken stdb(sdb,"=");
						if (strcmp(storb.token(0),stdb.token(0)) == 0) {
							if (strcmp(storb.token(1),stdb.token(1)) != 0) {
								cout<<"dbserver::UpdateEverything-I- updated "<<sorb<<endl;
								db_env.erase_it();
								db_env.insert(orb_env[j]);
							}
							found = true;
							break;
						}
					}
					if (!found) {
						cout<<"dbserver::UpdateEverything-I- added "<<sorb<<endl;
						db_env.insert(orb_env[j]);
					}
					db_env.query();
				}
    			cout << "dbserver::UpdateEverything-I- Server::setEnv" << endl;
				int len = db_env.length();
				for (int i=0; i<len; i++,db_env++) {
					CharStringToken st(*db_env,"=");
					if (st.tokens() >=2 && strstr(st.token(0),"AMSDataDir") == 0) {
						_arsref->setEnv(cid,CORBA_string_dup(st.token(0)),CORBA_string_dup(st.token(1)));
						string token0 = st.token(0);
						try {
							if (token0 == "AMSDBServDbg")
								_dbgLevel = boost::lexical_cast<int>(st.token(1));
							else if (token0 == "AMSMaxInactiveClientsHost")
								_maxSuspHost = boost::lexical_cast<int>(st.token(1));
							else if (token0 == "AMSMaxFailedClientsHost")
								_maxFailsHost = boost::lexical_cast<double>(st.token(1));
							else if (token0 == "AMSMaxFailedClientsHostTolerance")
								_maxFailToleranceHost = boost::lexical_cast<double>(st.token(1));
						}
						catch(boost::bad_lexical_cast &ex) { }
					}
				}
			}
			else {
    			cout << "dbserver::UpdateEverything-I- Server::getEnv" << endl;
				int len = _arsref->getEnv(cid,orb_env);
				db_env.erase();
				for (int i=0; i<len; i++) {
					db_env.insert(orb_env[i]);
					CharStringToken st(orb_env[i],"=");
					if (st.tokens() >=2) {
						string token0 = st.token(0);
						try {
							if (token0 == "AMSDBServDbg")
								_dbgLevel = boost::lexical_cast<int>(st.token(1));
							else if (token0 == "AMSMaxInactiveClientsHost")
								_maxSuspHost = boost::lexical_cast<int>(st.token(1));
							else if (token0 == "AMSMaxFailedClientsHost")
								_maxFailsHost = boost::lexical_cast<double>(st.token(1));
							else if (token0 == "AMSMaxFailedClientsHostTolerance")
								_maxFailToleranceHost = boost::lexical_cast<double>(st.token(1));
						}
						catch(boost::bad_lexical_cast &ex) { }
					}
				}
			}
		}
		{
   			cout << "dbserver::UpdateEverything-I- Server::getDBSpace" << endl;
			DPS::Server::DB *porb_db;
			DPS::Server::DB_var orb_db;
    		db.connect2(DBO_DB,true);
			db_dt db_db(db,DBO_DB);
			db_db.erase();
			bool result = _arsref->getDBSpace(cid,"AMSDataDir","/DataBase/",porb_db);
			if (result) {
				orb_db = porb_db;
				db_db.insert(orb_db);
			}
			db.connect2(DBO_RN,true);
			db_dt db_rn(db,DBO_RN);
			db_rn.erase();
			result = _arsref->getDBSpace(cid,"AMSProdOutputDir","/",porb_db);
			if (result) {
				orb_db = porb_db;
				db_rn.insert(orb_db);
			}
		}

		cid = mycid(DPS::Client::Producer);
		{
			DPS::Client::AHS_var orb_pahs;
    		db.connect2(DBO_AHLP,true);
			ah_dt db_pahs(db,DBO_AHLP);
			if (sendback) {
	    		cout << "dbserver::UpdateEverything-I- Producer::sendAHS" << endl;
				int len = db_pahs.length();
				orb_pahs = new DPS::Client::AHS;
				orb_pahs->length(len);
				for (int i=0; i<len; db_pahs++)
					orb_pahs[i++] = *db_pahs;
				_arsref->sendAHS(cid,orb_pahs);
			}
			else {
	    		cout << "dbserver::UpdateEverything-I- Producer::getAHS" << endl;
				int len = _arsref->getAHS(cid,orb_pahs);
				db_pahs.erase();
				for (int i=0; i<len; i++)
					db_pahs.insert(orb_pahs[i]);
			}
			if (_ahsFails.size() == 0) {
				db_pahs.query();
				for (int i=0,len=db_pahs.length(); i<len; i++,db_pahs++) {
					string sHostName((*db_pahs).HostName);
					_ahsFails[sHostName] = 0;
				}
			}
		}
		{
			DPS::Client::ACS_var orb_pacs;
    		db.connect2(DBO_ACL,true);
			ac_dt db_pacs(db,DBO_ACL);
			if (sendback) {
	    		cout << "dbserver::UpdateEverything-I- Producer::sendAC" << endl;
				int len = db_pacs.length();
				orb_pacs = new DPS::Client::ACS;
				orb_pacs->length(len);
				for (int i=0; i<len; i++,db_pacs++) {
					DPS::Client::ActiveClient_var vac = new DPS::Client::ActiveClient(*db_pacs);
					_arsref->sendAC(cid,vac,DPS::Client::Create);
				}
			}
			else {
	    		cout << "dbserver::UpdateEverything-I- Producer::getACS" << endl;
				int len = _arsref->getACS(cid,orb_pacs,maxc);
				db_pacs.erase();
				for (int i=0; i<len; i++)
					db_pacs.insert(orb_pacs[i]);
			}
		}
		{
			DPS::Client::NCS_var orb_pncs;
    		db.connect2(DBO_NCL,true);
			nc_dt db_pncs(db,DBO_NCL);
			if (sendback) {
	    		cout << "dbserver::UpdateEverything-I- Producer::sendNCS" << endl;
				int len = db_pncs.length();
				orb_pncs = new DPS::Client::NCS;
				orb_pncs->length(len);
				for (int i=0; i<len; db_pncs++)
					orb_pncs[i++] = *db_pncs;
				_arsref->sendNCS(cid,orb_pncs);
			}
			else {
	    		cout << "dbserver::UpdateEverything-I- Producer::getNC" << endl;
				int len = _arsref->getNC(cid,orb_pncs);
				db_pncs.erase();
				for (int i=0; i<len; i++)
					db_pncs.insert(orb_pncs[i]);
			}
		}
		{
			DPS::Producer::RES_var orb_prtb;
    		db.connect2(DBO_RTB,true);
			rtb_dt db_prtb(db,DBO_RTB);
			CORBA::ULong maxr = 0;
			if (sendback) {
				// do we need to repair
				if (repairdb) {
					bdbRC db2(_db);
					db2.connect2(DBO_RTB,true);
					rtb_dt db_prtb2(db2,DBO_RTB);
					try {
						int len = db_prtb2.length();
						for (int i=0; i<len; db_prtb2++) {
							const DPS::Producer::RunEvInfo &RunEvInfo = *db_prtb2;
							if (RunEvInfo.Status == DPS::Producer::Foreign) {
								cout << "dbserver::UpdateEverything-I- RT uid " << boost::format("%u") %RunEvInfo.uid
									  << " RT Status " << RunEvInfo.Status <<  " should be deleted" << endl;
								db_prtb2.erase_it();
							}
						}
					}
					catch(string &ex) {
					}
					db_prtb.query();
				}
				// ok, do your job
	    		cout << "dbserver::UpdateEverything-I- Producer::sendRunEvInfoS" << endl;
				int len = db_prtb.length();
				orb_prtb = new DPS::Producer::RES;
				orb_prtb->length(len);
				for (int i=0; i<len; db_prtb++)
					orb_prtb[i++] = *db_prtb;
				_arpref->sendRunEvInfoS(cid,orb_prtb,maxr);
			}
			else {
	    		cout << "dbserver::UpdateEverything-I- Producer::getRunEvInfoS" << endl;
				int len = _arpref->getRunEvInfoS(cid,orb_prtb,maxr);
				db_prtb.erase();
				for (int i=0; i<len; i++)
					db_prtb.insert(orb_prtb[i]);
			}
		}
		{
			DPS::Producer::DSTIS_var orb_pdsti;
    		db.connect2(DBO_DSTI,true);
			dsti_dt db_pdsti(db,DBO_DSTI);
			if (sendback) {
	    		cout << "dbserver::UpdateEverything-I- Producer::sendDSTInfoS" << endl;
				int len = db_pdsti.length();
				orb_pdsti = new DPS::Producer::DSTIS;
				orb_pdsti->length(len);
				for (int i=0; i<len; db_pdsti++)
					orb_pdsti[i++] = *db_pdsti;
				_arpref->sendDSTInfoS(cid,orb_pdsti);
			}
			else {
	    		cout << "dbserver::UpdateEverything-I- Producer::getDSTInfoS" << endl;
				int len = _arpref->getDSTInfoS(cid,orb_pdsti);
				db_pdsti.erase();
				for (int i=0; i<len; i++)
					db_pdsti.insert(orb_pdsti[i]);
			}
		}
		{
			DPS::Producer::DSTS_var orb_pdsts;
    		db.connect2(DBO_DSTS,true);
			dsts_dt db_pdsts(db,DBO_DSTS);
			if (sendback) {
	    		cout << "dbserver::UpdateEverything-I- Producer::sendDSTS" << endl;
				int len = db_pdsts.length();
				orb_pdsts = new DPS::Producer::DSTS;
				orb_pdsts->length(len);
				for (int i=0; i<len; db_pdsts++)
					orb_pdsts[i++] = *db_pdsts;
				_arpref->sendDSTS(cid,orb_pdsts);
			}
			else {
	    		cout << "dbserver::UpdateEverything-I- Producer::getDSTS" << endl;
				int len = _arpref->getDSTS(cid,orb_pdsts);
				db_pdsts.erase();
				for (int i=0; i<len; i++)
					db_pdsts.insert(orb_pdsts[i]);
			}
		}

		cid = mycid(DPS::Client::DBServer);

		{
			DPS::Client::ACS_var orb_dbacs;
    		db.connect2(DBO_ADBSL,true);
			ac_dt db_dbacs(db,DBO_ADBSL);
    		cout << "dbserver::UpdateEverything-I- DBServer::getACS" << endl;
			int len = _arsref->getACS(cid,orb_dbacs,maxc);
			db_dbacs.erase();
			for (int i=0; i<len; i++)
				db_dbacs.insert(orb_dbacs[i]);
		}

		cid = mycid(DPS::Client::Monitor);

		{
			DPS::Client::ACS_var orb_macs;
    		db.connect2(DBO_AML,true);
			ac_dt db_macs(db,DBO_AML);
    		cout << "dbserver::UpdateEverything-I- Monitor::getACS" << endl;
			int len = _arsref->getACS(cid,orb_macs,maxc);
			db_macs.erase();
			for (int i=0; i<len; i++)
				db_macs.insert(orb_macs[i]);
		}

		cout << "dbserver::UpdateEverything-I- finished" << endl;
	}
	catch (CORBA::Exception& ex) {
		cout << "dbserver::UpdateEverything-E- corba failed" << endl;
		return false;
	}
	catch (string &ex) {
		cout << "dbserver::UpdateEverything-E- " << ex << endl;
		return false;
	}
	return true;
}

bool dbserver::UpdateARS(DPS::Server_var &server)
{
	DPS::Client::CID cid = _cid;
	cid.Type = DPS::Client::Server;
//	DPS::Client::ARS *pars;
	try {
		cout << "dbserver::UpdateARS-I- Server" << endl;
		DPS::Client::ARS_var vars;
		long result = server->getARS(cid,vars,DPS::Client::Any,0,1);
		if (result == 0) {
			cout << "dbserver::UpdateARS-F- getARS returns zero length" << endl;
			return false;
		}
		_ars[1] = vars[0];

	   	CORBA::String_var ref = vars[0].IOR;
	   	_arsref = DPS::Server::_narrow(_orb->string_to_object(ref));
	}
	catch(const CORBA::Exception& ex) {
		cout << "dbserver::UpdateARS-F- corba failed" << endl;
		return false;
	}
	cid.Type = DPS::Client::Producer;
	try {
		cout << "dbserver::UpdateARS-I- Producer" << endl;
		DPS::Client::ARS_var vars;
		long result = server->getARS(cid,vars,DPS::Client::Any,0,1);
		if (result == 0) {
			cout << "dbserver::UpdateARS-F- getARS returns zero length" << endl;
			return false;
		}
		_ars[2] = vars[0];

	   	CORBA::String_var ref = vars[0].IOR;
	   	_arpref = DPS::Producer::_narrow(_orb->string_to_object(ref));
	}
	catch(const CORBA::Exception& ex) {
		cout << "dbserver::UpdateARS-F- corba failed" << endl;
		return false;
	}
	return true;
}

bool dbserver::InitDBFile(const char *dir, const char *file, const char *repairdb)
{
	string sdir, sfile;
	// on start use default bdb schema, and suppose that db already exists
	float version = BDB_VERSION;
	bool create_db = false;

	if (file == 0) {
		if (dir == 0)
			return false;
		sdir = dir;
		if (version<2.0)
			sfile = mktemp(const_cast<char *>(string(sdir+"/fileXXXXXX").c_str()));
		create_db = true;
	}
	else {
		string s = file;
		float maybe = bdbRC::exists(file);
		if (maybe==1.0) {
			version = maybe;
			sfile = s.substr(s.find_last_of('/')+1);;
			sdir = s.substr(0,s.find_last_of('/'));
		}
		else if (maybe==2.0) {
			version = maybe;
			sdir = s;
		}
		else {
			if (version<2.0) {
				sfile = s.substr(s.find_last_of('/')+1);;
				sdir = s.substr(0,s.find_last_of('/'));
			}
			else
				sdir = s;
			create_db = true;
		}
	}
	try {
		if (_db) delete _db;
		_db = new bdbRC(sdir,version);
		_db->db_file(sfile);
	}
	catch (string &ex) {
		cout << "dbserver::InitDBFile-F- " << ex << endl;
		return false;
	}

	// Detect bdb schema
	bool sendback;
	try {
		sendback = !_db->connect2(DBO_ENV,create_db);
		cout<<"dbserver::InitDBFile-I- open db "<<_db->env_path()<<"/"<<_db->db_file()<<
				", schema "<<_db->version()<<" detected"<<endl;
		_db->disconnect();
	}
	catch (string &ex) {
		_db->disconnect();
		cout << "dbserver::InitDBFile-F- " << ex << endl;
		return false;
	}
	// Finally synchronize the server with bdb
	try {
		return UpdateEverything(sendback,repairdb);
	}
	catch (string &ex) {
		cout << "dbserver::InitDBFile-F- " << ex << endl;
	}
	return false;
}

bool dbserver::SendId()
{
	cout << "dbserver::SendId-I- " << endl;
	try {
		bool result = _arsref->sendId(_cid,2000,30);
		if (!result) {
			cout << "dbserver::SendId-E- sendId 1st failure" << endl;
			Sleep(1000);
			result = _arsref->sendId(_cid,2000,30);
			if (!result) {
				cout << "dbserver::SendId-F- sendId 2nd failure, no sendAC may be sent" << endl;
				return false;
			}
		}
		_arsref->sendAC(_cid,_ac,DPS::Client::Update);
		_servant.sendACPerl(_cid,_ac,DPS::Client::ClearCreate);
	}
	catch(const CORBA::Exception& ex) {
		cout << "dbserver::SendId-F- corba failed" << endl;
		return false;
	}
	return true;
}

//_____________________________________________________________________
// Here are database requisites, not included into command-line string

const string req_db = "pdb_ams";
const string req_user = "amsdes";
const string req_passwd = "/var/www/cgi-bin/mon/lib/.oracle.oracle";

Connection *dbserver::InitOracle(int argc, char *argv[])
{
	static string ora_passwd;
	if (argc > 0) {
		const char *reqhost = hasArg(argc,argv,"-h");
		string dbhost;
		if (reqhost) dbhost = reqhost;
		  else		 dbhost = "pcamss0.cern.ch";

		std::ifstream ifora(req_passwd.c_str());
		if (ifora.is_open()) {
	        cout<<"dbserver::InitOracle-I- get password from local host"<<endl;
	        std::getline(ifora,ora_passwd);
			ifora.close();
		}
		else {
			cout<<"dbserver::InitOracle-I- get password from host "<<dbhost<<endl;
			string slog("/tmp/");
			slog += mktemp(const_cast<char *>(string("fileXXXXXX").c_str()));
			string cmd("/afs/cern.ch/ams/local/bin/timeout --signal 9 60 ssh ");
			cmd += dbhost + " cat " + req_passwd + " &>" + slog;
			if (system(cmd.c_str()) == 0) {
				fstream fs(slog.c_str(),ios_base::in);
				getline(fs,ora_passwd);
				fs.close();
				unlink(slog.c_str());
			}
			else {
				cout<<"dbserver::InitOracle-F- no oracle access"<<endl;
				return 0;
			}
		}
	}
	if (_dbora == 0) _dbora = new oraRC();
	try {
		return _dbora->connect(req_user,ora_passwd,req_db);
	}
	catch (SQLException &ex) {
        cout << "dbserver::InitOracle-F- " << ex.what() << endl;
        return 0;
	}
}

void dbserver::CloseOracle() { _dbora->disconnect(); }

DPS::Client::CID dbserver::mycid(DPS::Client::ClientType type) const
{
	DPS::Client::CID cid = _cid;
	cid.Type = type;
	return cid;
}

const char *dbserver::strClientType(DPS::Client::ClientType type)
{
	const char *ex = " dbserver::strClientType-E- ";
	const char *_strClientType[] = {
			"Generic", "Server", "Consumer", "Producer",
			"Monitor", "Killer", "DBServer", "ODBServer",
			"Standalone" };
	if (type>=0 && type<static_cast<int>(sizeof(_strClientType)/sizeof(_strClientType[0])))
		return _strClientType[type];
	return ex;
}

const char *dbserver::strClientStatusType(DPS::Client::ClientStatusType type)
{
	const char *ex = " dbserver::strClientStatusType-E- ";
	const char *_strClientStatusType[] = {
			"Permanent", "OneRunOnly" };
	if (type>=0 && type<static_cast<int>(sizeof(_strClientStatusType)/sizeof(_strClientStatusType[0])))
		return _strClientStatusType[type];
	return ex;
}

const char *dbserver::strClientStatus(DPS::Client::ClientStatus stat) {
	const char *ex = " dbserver::strClientStatus-E- ";
	const char *_ClientStatus[] = {
			"Lost", "Unknown", "Submitted", "Registered", "Active", "TimeOut", "Killed" };
	if (stat>=0 && stat<static_cast<int>(sizeof(_ClientStatus)/sizeof(_ClientStatus[0])))
		return _ClientStatus[stat];
	return ex;
}

const char *dbserver::strRecordChange(DPS::Client::RecordChange rc)
{
	const char *ex = " dbserver::strRecordChange-E- ";
	const char *_strRecordChange[] = {
			"Delete", "Update", "Create", "ClearCreate" };
	if (rc>=0 && rc<static_cast<int>(sizeof(_strRecordChange)/sizeof(_strRecordChange[0])))
		return _strRecordChange[rc];
	return ex;
}

const char *dbserver::strRunStatus(DPS::Producer::RunStatus status)
{ 
	const char *ex = " dbserver::strRunStatus-E- ";
	const char *_strRunStatus[] = {
			"ToBeRerun", "Failed", "Processing", "Finished",
			"Unknown", "Allocated", "Foreign", "Canceled" };
	if (status>=0 && status<static_cast<int>(sizeof(_strRunStatus)/sizeof(_strRunStatus[0])))
		return _strRunStatus[status];
	return ex;
}

const char *dbserver::strDSTStatus(DPS::Producer::DSTStatus status)
{
	const char *ex = " dbserver::strDSTStatus-E- ";
	const char *_strDSTStatus[] = {
			"Success", "InProgress", "Failure", "Validated" };
	if (status>=0 && status<static_cast<int>(sizeof(_strDSTStatus)/sizeof(_strDSTStatus[0])))
		return _strDSTStatus[status];
	return ex;
}

const char *dbserver::strHostStatus(DPS::Client::HostStatus status) 
{
	const char *ex = " dbserver::strHostStatus-E- ";
	const char *_strHostStatus[] = {
			"NoResponse", "InActive", "LastClientFailed", "OK" };
	if (status>=0 && status<static_cast<int>(sizeof(_strHostStatus)/sizeof(_strHostStatus[0])))
		return _strHostStatus[status];
	return ex;
}

void dbserver::debug(const char *head)
{
    cout << head << endl;
}

void dbserver::debug(const char *head, const DPS::Client::CID &cid)
{
    cout << head << strClientType(cid.Type) << ", cid.HostName=" << cid.HostName <<
    		", cid.uid=" << cid.uid << ", cid.StatusType=" << strClientStatusType(cid.StatusType) << endl;
}

void dbserver::debug(const char *head, DPS::Client::RecordChange rc)
{
    cout << head << "with " << strRecordChange(rc) << endl;
}

void dbserver::debug(const char *head, const DPS::Client::CID &cid, DPS::Client::RecordChange rc)
{
	cout << head << strClientType(cid.Type) << " with " << strRecordChange(rc) << ", cid.HostName=" << cid.HostName <<
			", cid.uid=" << cid.uid << ", cid.StatusType=" << strClientStatusType(cid.StatusType)	<< endl;
}

void dbserver::debug(const char *head, const DPS::Producer::RunEvInfo &ne, DPS::Client::RecordChange rc)
{
    cout << head << "with " << strRecordChange(rc) << ", uid=" << ne.uid << ", Status=" << strRunStatus(ne.Status) <<
    		", History=" << strRunStatus(ne.History) << endl;
}

void dbserver::debug(const char *head, const DPS::Client::CID &cid, const DPS::Producer::DST &ne, DPS::Client::RecordChange rc)
{
    cout << head << strClientType(cid.Type) << " with " << strRecordChange(rc) << ", Status=" << strDSTStatus(ne.Status) << endl;
}

void dbserver::debug(const char *head, const DPS::Client::CID &cid, const DPS::Client::ActiveClient &ac, DPS::Client::RecordChange rc)
{
	cout << head << strClientType(cid.Type) << " with " << strRecordChange(rc) << ", cid.HostName=" << cid.HostName <<
			", cid.uid=" << cid.uid << ", ac.Status=" << strClientStatus(ac.Status) << ", ac.id.uid=" <<
			ac.id.uid << ", ac.id.HostName=" << ac.id.HostName << ", ac.id.Interface=" << ac.id.Interface << endl;
}

void dbserver::debug(const char *head, const DPS::Client::CID &cid, const DPS::Client::ActiveHost &ah, DPS::Client::RecordChange rc)
{
    cout << head << strClientType(cid.Type) << " with " << strRecordChange(rc) << ", ah.HostName=" << ah.HostName <<
    		", ah.Interface=" << ah.Interface << ", ah.Status=" << strHostStatus(ah.Status) << ", ah.ClientsRunning="
    		<< ah.ClientsRunning << " ah.ClientsAllowed=" << ah.ClientsAllowed << endl;
}

int dbserver::safe_strlen(const char *str, size_t max_len)
{
	if (str) {
		const char *eol = (const char *)memchr(str,'\0',max_len);
		if (eol != NULL) return eol-str;
	}
	return -1;
}
