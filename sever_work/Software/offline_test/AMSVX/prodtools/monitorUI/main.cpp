#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <math.h>

using std::cout;
using std::endl;
using std::fstream;

#include <QtGui/QApplication>
#include <QtGui/QKeyEvent>

#include "uiwrapper.h"
#include "oraRC.h"

#include <queue>
#include <utility>
	   
using std::queue;
using std::pair;

ui theUI;
IFACES __thrClients;
event __evAskData;
mutex __mxProcData;
string __baseUser = "ams";
string __baseHost = "pcamss0.cern.ch";
string __thePerl;
unsigned int __toSsh = 90;
CORBA::ORB_ptr __theOrb = 0;
std::map<const char *, string> __logHost;

void timer_handler(int signum);
void thrBody(srviface *iface);
void thrBodyVrd(srviface *iface);
void thrBodyOther(srviface *iface);
void thrStatusBar();

string oraPasswd(string &host);
void oraIORs(Connection *occiConn, const char *vno, string &siors, string &siorp, string &siord) throw(string &);
void oraVrdLogPath(Connection *occiConn, string &path) throw(string &);

// keep command-line args for corba restart
int    _save_argc;
char** _save_argv;

//_____________________________________________________________________
// Here are database requisites, not included into command-line string

string req_db = "pdb_ams";
string req_user = "amsdes";
string req_passwd = "";
const string req_passwd_file = "/var/www/cgi-bin/mon/lib/.oracle.oracle";

// ITIMER_REAL time interval [seconds]
time_t evAskData_Period = 300;

int main(int argc, char *argv[])
{
	string user;
	vector<string> vs;
	bool useMylogging = false;

	// save command-line arguments
	_save_argc = argc-1;
	_save_argv = new(char *[_save_argc]);
	for (int i=1; i<argc; i++) {
		_save_argv[i-1] = new char[strlen(argv[i])+1];
		memcpy(_save_argv[i-1],argv[i],strlen(argv[i])+1);
	}
	// extract arguments
	for (int i=0; i<argc && argc>0; i++)
	{
		string s_arg = argv[i];

		if (s_arg[0] != '-')
			continue;
		if (s_arg[1] == 'v')
			vs.push_back(&s_arg[2]);
		else if (s_arg[1] == 'h')
			__baseHost = &s_arg[2];
		else if (s_arg[1] == 'l')
			useMylogging = true;
		else if (s_arg[1] == 'p')
			__thePerl = &s_arg[2];
		else if (s_arg[1] == 's')
			sscanf(&s_arg[2],"%u",&evAskData_Period);
		else if (s_arg[1] == 'u')
			user = &s_arg[2];
		else if (s_arg[1] == 'a') {
			CharStringToken st(&s_arg[2],"/");
			if (st.tokens() > 0) {
				const char *s;
				if (st.tokens() > 1) {
					req_user = st.token(0);
					s = st.token(1);
				}
				else s = st.token(0);
				CharStringToken st(s,"@");
				if (st.tokens() > 0) {
					req_passwd = st.token(0);
					if (st.tokens() > 1) req_db = st.token(1);
				}
			}
		}
		else if (s_arg[1] == 't')
			sscanf(&s_arg[2],"%u",&__toSsh);
	}
	// argumentless start
	if (vs.size() == 0) vs.push_back("4");

	// initialise gui
    cout << "::main-I- initialize gui engine" << endl;
    QApplication a(argc, argv);
    a.setStyle(QStyleFactory::create("gtk"));
    myQMainWindow mw;
    theUI.setupUi(&mw);
    theUI.acl(user);
    if (useMylogging)
    	theUI.actLogToCout->setChecked(false);
    // setup shmat classes
    if (theUI._shSelFilt.create(4096,"/tmp/shFilter")) {
	    string selfi;
		for (unsigned int i=0; i<vs.size(); i++) {
			if (i) selfi += ",";
			selfi += vs[i];
		}
		theUI._shSelFilt._cash._servers = selfi;
		theUI._shSelFilt << theUI._shSelFilt._cash;
		theUI._shSelFilt.unlock();
	}
	else {
		uistring err = theUI._shSelFilt.errorString();
        cout << "::main-E- " << theUI._shSelFilt.key() << " init failed: " << err << endl;
	}
    if (theUI._shCtrEdit.create(1024,"/tmp/shCedit")) {
		theUI._shCtrEdit._cash._upd = 0;
		theUI._shCtrEdit << theUI._shCtrEdit._cash;
		theUI._shCtrEdit.unlock();
	}
	else {
		uistring err = theUI._shCtrEdit.errorString();
        cout << "::main-E- " << theUI._shCtrEdit.key() << " init failed: " << err << endl;
 	}
    if (theUI._shLogSel.create(4096,"/tmp/shSfile")) {
		theUI._shLogSel << theUI._shLogSel._cash;
		theUI._shLogSel.unlock();
	}
	else {
		uistring err = theUI._shCtrEdit.errorString();
        cout << "::main-E- " << theUI._shLogSel.key() << " init failed: " << err << endl;
 	}

	// initialize CORBA
	if (vs.size())
	{
		string s;
		string sp = " ";
		for (int i=0; i<_save_argc; i++)  s += _save_argv[i]+sp;
		try {
			__theOrb = CORBA::ORB_init(_save_argc,_save_argv,"orbit-local-mt-orb");
		    cout << "::main-I- init CORBA::ORB_init OK, args: " << s << endl;
		}
		catch (...) {
	        cout << "::main-F- init CORBA::ORB_init failed, args: " << s << endl;
	        return 0;
		}

		// Get db password file
	    if (req_passwd.size() < 2) req_passwd = oraPasswd(__baseHost);
		// Create occi oracle environment
		char *tns = getenv("TNS_ADMIN");
		if(!tns || !strlen(tns))
			setenv("TNS_ADMIN","/afs/cern.ch/project/oracle/admin",1);
		oraRC *dbman;
		try
		{
			dbman = new oraRC();
		    cout << "::main-I- connect " << req_user << "@" << req_db << " database" << endl;
			Connection *occiConn = dbman->connect(req_user,req_passwd,req_db);
		    cout << "::main-I- " << req_db << " connected" << endl;

			for (unsigned i=0; i<vs.size(); i++)
			{
				string siors, siorp, siord;
				oraIORs(occiConn,vs[i].c_str(),siors,siorp,siord);
				if (siors.size() > 8) {
					// Start corba client
					try {
						srviface *iface = new srviface(vs[i],siors,siorp,siord);
						__thrClients.push_back(iface);
						_thread<srviface *> thr;
						thr.run(&thrBody,iface);
					}
					catch(int) {
						cout << boost::format("::main-E- thrBody start exception") << endl;
					}
				}
			}
			delete dbman;
		}
		catch (SQLException &ex) {
	        cout << "::main-F- " << ex.what() << endl;
	        return 0;
		}
		catch (string &ex) {
	        cout << "::main-F- " << ex << endl;
	        return 0;
		}

		// Try to get proper pathes for 'Other log file info'
		const char *user = getenv ("USER");
		cout << "::main-I- check acrontab " << user << "@" << __baseHost << endl;
		if (user && __baseUser == user) {
			__logHost["vrd.acron"] = __baseHost;
			__logHost["frame_decode.acron"] = __baseHost;
			__logHost["trc.acron"] = __baseHost;
			__logHost["scdb.acron"] = __baseHost;
			string cmd = "acrontab -l";
		    qprocess proc;
		    if (!proc.run(cmd)) {
		    	string l;
		    	size_t lps, lpe;
				while (proc.getline(l)) {
					if (l.size() < 1 || l[0] == '#')
						continue;
					CharStringToken st(l," ");
					int hostid = st.rftoken("*") + 1;
					if (hostid==0 || hostid>st.tokens()-2)
						continue;
					if ((lps=l.find("vrd."))!=string::npos && (lpe=l.find(".acron"))!=string::npos) {
						__logHost["vrd.acron"] = st.token(hostid);
						cout<<"::main-I- "<<l.substr(lps,lpe-lps+6)<<" host found "<<st.token(hostid)<<endl;
					}
					else if ((lps=l.find("frame_decode."))!=string::npos && (lpe=l.find(".acron"))!=string::npos) {
						__logHost["frame_decode.acron"] = st.token(hostid);
						cout<<"::main-I- "<<l.substr(lps,lpe-lps+6)<<" host found "<<st.token(hostid)<<endl;
					}
					else if ((lps=l.find("trc."))!=string::npos && (lpe=l.find(".acron"))!=string::npos) {
						__logHost["trc.acron"] = st.token(hostid);
						cout<<"::main-I- "<<l.substr(lps,lpe-lps+6)<<" host found "<<st.token(hostid)<<endl;
					}
					else if ((lps=l.find("scdb."))!=string::npos && (lpe=l.find(".acron"))!=string::npos) {
						__logHost["scdb.acron"] = st.token(hostid);
						cout<<"::main-I- "<<l.substr(lps,lpe-lps+6)<<" host found "<<st.token(hostid)<<endl;
					}
				}
		    }
			else
				cout<<"::main-E- "<<"exec error on: "<<proc.lastcmd()<<endl;
		}

	    // Install timer_handler as the signal handler for SIGPROF
	    signal(SIGALRM,&timer_handler);
	    // Configure the timer to expire after appropriate time
	    struct itimerval timer;
	    timer.it_value.tv_sec = evAskData_Period;
	    timer.it_value.tv_usec = 0;
	    // ... and set its periodicity
	    timer.it_interval.tv_sec = evAskData_Period;
	    timer.it_interval.tv_usec = 0;
	    if (setitimer(ITIMER_REAL, &timer, 0) == -1) {
	        cout << "::main-E- error timer assignment" << endl;
	    }
	    // Run status bar data thread
		_thread<> thr;
		thr.run(&thrStatusBar);
	}
	
	// Start gui
    theUI.setDataRequested();
    mw.show();
    return a.exec();
}

//_____________________________________________________________________
// Threads

void timer_handler(int signum)
{
	// Clear icons status
	for (int i=0; i<theUI.tabWidget->count(); i++) {
		uistring s(theUI.tabWidget->tabText(i));
		theUI.icon(s.c_str(),ui::ICONST_OK,true);
	}
	// Request data if no lock update
	if (theUI.actLockUpdate->isChecked() == false) {
		theUI.setDataRequested();
		__evAskData.raise();
	}
	// refresh kerberos tickets
}

void thrBody(srviface *iface)
{
 	DPS::Server_var pServer;
	DPS::Producer_var pProducer;
	DPS::DBServer_var pDBServer;

	try {
    	CORBA::String_var ref = iface->iors().c_str();
		pServer = DPS::Server::_narrow(__theOrb->string_to_object(ref));
		log() << "::thrBody-I- " << iface->server() << ", init server ior: " << iface->iors() << endl;
        ref = iface->iorp().c_str();
		pProducer = DPS::Producer::_narrow(__theOrb->string_to_object(ref));
		log() << "::thrBody-I- " << iface->server() << ", init producer ior: " << iface->iorp() << endl;
    	ref = iface->iord().c_str();
		pDBServer = DPS::DBServer::_narrow(__theOrb->string_to_object(ref));
		log() << "::thrBody-I- " << iface->server() << ", init dbserver ior: " << iface->iord() << endl;
	}
    catch (CORBA::SystemException &ex) {
    	log() << "::thrBody-E- " <<  iface->server() << ", " << ex._orbitcpp_get_repoid() << endl;
    }
    Sleep(3000);

    while (1)
	{
	    try
		{
	    	if (timeLongLong()-iface->updated() > 500)
	    	{
	    		__mxProcData.lock();

	    		// Get data from amsprodserver
		        if (iface->fails() || (iface->dbfails() && iface->pinged())) {
		    		oraRC *dbman;
		    		try {
			    		dbman = new oraRC();
						Connection *occiConn = dbman->connect(req_user,req_passwd,req_db);
						oraIORs(occiConn,
								iface->vno(),
								const_cast<string &>(iface->iors()),
								const_cast<string &>(iface->iorp()),
								const_cast<string &>(iface->iord()));
						string s;
						string sp = " ";
						for (int i=0; i<_save_argc; i++)  s += _save_argv[i]+sp;
						try {
							__theOrb = CORBA::ORB_init(_save_argc,_save_argv,"orbit-local-mt-orb");
							log() << "::thrBody-I- reinit CORBA::ORB_init OK, args: " << s << endl;
						}
						catch (...) {
							log() << "::thrBody-E- reinit CORBA::ORB_init failed, args: " << s << endl;
						}
					    try {
					    	CORBA::String_var ref = iface->iors().c_str();
							pServer = DPS::Server::_narrow(__theOrb->string_to_object(ref));
							log() << "::thrBody-I- " << iface->server() << ", reinit server ior: " << iface->iors() << endl;
					        ref = iface->iorp().c_str();
							pProducer = DPS::Producer::_narrow(__theOrb->string_to_object(ref));
							log() << "::thrBody-I- " << iface->server() << ", reinit producer ior: " << iface->iorp() << endl;
							ref = iface->iord().c_str();
							pDBServer = DPS::DBServer::_narrow(__theOrb->string_to_object(ref));
							log() << "::thrBody-I- " << iface->server() << ", reinit dbserver ior: " << iface->iord() << endl;
					    }
					    catch (CORBA::SystemException &ex) {
					    	log() << "::thrBody-E- " << iface->server() << ", " << ex._orbitcpp_get_repoid() << endl;
					    }
		    		}
		    		catch (SQLException &ex) {
		    			log() << "::thrBody-E- " << ex.what() << endl;
		    		}
		    		catch (string &ex) {
		    			log() << "::thrBody-E- " << ex << endl;
		    		}
	    			delete dbman;
	 	        }
//!!!!! investigate memory leaks
// for (int i=0; i<20; i++)
				iface->request(pServer,pProducer,pDBServer);
				__mxProcData.unlock();
				theUI.kickev(uiEvent::ACT_DATACHANGED,iface);
	    	}

			// Start vrd log
			if (iface->_vrdFirst && iface->vrd().configured()) {
				try {
					_thread<srviface *> thr;
					thr.run(&thrBodyVrd,iface);
					iface->_vrdFirst = false;
				}
				catch(int) {
					log() << boost::format("::thrBody-E- Vrd start exception") << endl;
				}
			}

			// Start other log
			__mxProcData.lock();
    		if (__otherFirst) {
    			try {
    				_thread<srviface *> thr;
					thr.run(&thrBodyOther,iface);
					__otherFirst = false;
				}
				catch(int) {
					throw string("::thrBody-E- Other start exception");
				}
    		}
    		__mxProcData.unlock();
		}
		catch (string &ex)
		{
			__mxProcData.unlock();
	    	log() << "::thrBody-E- " << ex << endl;
        	theUI.kickev(uiEvent::ACT_DATACHANGED,iface);
		}
		__evAskData.wait();
    }
}

void thrBodyVrd(srviface *iface)
{
	Sleep(3000);
	longlong updated = 0;
    while (!iface->_vrdFirst)
	{
	    try {
			if (timeLongLong()-updated > 1000) {
				iface->request_vrd();
				theUI.kickev(uiEvent::ACT_VRDLOGCHANGED,iface);
				updated = timeLongLong();
			}
		}
		catch (string &ex) 	{
			log() << "::thrBodyVrd-E- " << ex << endl;
	    	updated = timeLongLong();
		}
		__evAskData.wait();
    }
}

void thrBodyOther(srviface *iface)
{
	Sleep(3000);
	longlong updated = 0;
    while (!__otherFirst)
	{
	    try {
			if (timeLongLong()-updated > 1000) {
				iface->request_other();
				theUI.kickev(uiEvent::ACT_OTHERLOGCHANGED,iface);
				updated = timeLongLong();
			}
	    }
		catch (string &ex) {
			log() << "::thrBodyOther-E- " << ex << endl;
	    	updated = timeLongLong();
		}
		__evAskData.wait();
    }
}

void thrStatusBar()
{
	Sleep(3000);
	while (1) {
    	theUI.kickev(uiEvent::ACT_UPDSTATUSBAR);
		Sleep(1000);
	}
}

//_____________________________________________________________________

string oraPasswd(string &host)
{
	string passwd = "";

	std::ifstream ifora(req_passwd_file.c_str());
	if (ifora.is_open()) {
        cout<<"::oraPasswd-I- get password from local host"<<endl;
        std::getline(ifora,passwd);
		ifora.close();
	}
	else {
		cout<<"::oraPasswd-I- get password from host "<<host<<endl;
		string cmd = "ssh " + host + " cat " + req_passwd_file;
		qprocess proc;
		if (!proc.run(cmd))
			proc.getline(passwd);
		else
			cout<<"::oraPasswd-E- got no oracle password"<<endl;
	}
	return passwd;
}

void oraIORs(Connection *occiConn, const char *vno, string &siors, string &siorp, string &siord) throw(string &)
{
	stringstream s;
	try {
		int datamc = boost::lexical_cast<int>(vno)-4;
		s << boost::format("%d") %datamc;
	}
    catch(boost::bad_lexical_cast &ex) {
        throw(string("::oraIORs - invalid -v argument"));
    }
	try {
		string sql("select lastupdate,IORS,IORP,IORD from Servers where status='Active' and datamc=");
		sql += s.str();
		Statement *occiStmt = occiConn->createStatement(sql);
		ResultSet *occiResultSet = occiStmt->executeQuery();

		int lastUpdate = 0;
		while (occiResultSet->next()) {
			int updated = occiResultSet->getInt(1);
			string iors = occiResultSet->getString(2);
			string iorp = occiResultSet->getString(3);
			string iord = occiResultSet->getString(4);
			if (updated > lastUpdate) {
				lastUpdate = updated;
				siors = iors;
				siorp = iorp;
				siord = iord;
			}
		}
		occiStmt->closeResultSet(occiResultSet);
		occiConn->terminateStatement(occiStmt);
	}
	catch (SQLException &ex) {
        throw(string("::oraIORs - ") + ex.what());
	}
}

void oraVrdLogPath(Connection *occiConn, string &path) throw(string &)
{
	try {
		string sql("select ValidationDirPath from Environment");
		Statement *occiStmt = occiConn->createStatement(sql);
		ResultSet *occiResultSet = occiStmt->executeQuery();

		while (occiResultSet->next()) {
			path = occiResultSet->getString(1);
		}
		occiStmt->closeResultSet(occiResultSet);
		occiConn->terminateStatement(occiStmt);
	}
	catch (SQLException &ex) {
        throw(string("::oraIORs - ") + ex.what());
	}
}
