#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <math.h>
#include <time.h>

using std::cout;
using std::endl;
using std::fstream;

#include <QtGui/QApplication>

#include <queue>
#include <utility>
using std::queue;
using std::pair;

#include "uiwrapper_dbvw.h"

#include "server-cpp.h"
#include "server-cpp-stubs.h"
#include "server-cpp-skels.h"
#include "server-cpp-common.h"

ui theUI;
bdbRC *theDB=0;
int _dbgLevel = 1;
void readDB(const char *);
void thrLoadDB(const char *);
void createDB(const char *);
void recoverDB(const char *);
timespec hrtime(clockid_t clk_id) {
	timespec ts;
	clock_gettime(clk_id,&ts);
	return ts;
};

// to find out all applicable arguments, search for hasArg invocation
const char *hasArg(int argc, char *argv[], const char *wanted)
{
	for (int i=1; i<argc; i++) {
		string sarg = argv[i];
		if (sarg.find(wanted) == 0) {
			if (sarg.length()-strlen(wanted) > 0)
				return &argv[i][strlen(wanted)];
			else if (i < argc-1)
				return argv[i+1];
		}
	}
	return 0;
}

int main(int argc, char *argv[])
{
	// initialise gui
    QApplication a(argc, argv);
    a.setStyle(QStyleFactory::create("gtk"));
    myQMainWindow mw;
    theUI.setupUi(&mw);
    mw.show();

    // init db api
	bdbTraits::instantiate_all();
	// get db from command-line, if any
	const char *parg = hasArg(argc,argv,"-B");
	readDB(parg);
	// let gui work
    return a.exec();
}

void readDB(const char *db)
{
	theUI.actExportDB->setEnabled(false);
	if (!db) return;
	boost::filesystem::path ipath(db);
	float version = BDB_VERSION;
	if (boost::filesystem::is_directory(ipath)) {
		ipath /= "/";
		version = 2.0;
	} else
		version = 1.0;
	string sdir = ipath.parent_path().string();
	string sfile = ipath.filename();
	try {
		if (theDB && theDB->env_path() != sdir) {
			cout<<"::readDB-I- rm env "<<theDB->env_path()<<endl;
			delete theDB;
			theDB = 0;
		}
		if (theDB == 0) {
			cout<<"::readDB-I- open env "<<sdir<<endl;
			theDB = new bdbRC(sdir,version);
			theDB->db_file(sfile);
		}
		// read db in current context
/*!!!!! version 2 tuning
double total=0;
for(int i=0; i<1000; i++) {
	timespec start_time = hrtime(CLOCK_REALTIME);*/
		thrLoadDB(0);
/*!!!!! version 2 tuning
 timespec end_time = hrtime(CLOCK_REALTIME);
	total += static_cast<double>((end_time.tv_sec-start_time.tv_sec)*1e9 + (end_time.tv_nsec-start_time.tv_nsec))/1e6;
}
cout<<"average time = "<<boost::format("%.6d") %(total/1000)<<" msec"<<endl;*/
		// ...or read db in single thread
		/*_thread<const char *> thr;
		thr.run(&thrLoadDB,0);*/
		// ...or read db in parallel
		/*for (MapCtrlForms::iterator it=theUI._map_ctrl_forms.begin();
				it != theUI._map_ctrl_forms.end(); ++it) {
			uistring sform = it->first;
			_thread<const char *> thr;
			thr.run(&thrLoadDB,sform.c_str());
		}*/
		uistring title("AMS BDB quick Viewer (");
		title += common::user() + "@" + uistring(QHostInfo::localHostName()) + ":" + db + ")";
		theUI._mw->setWindowTitle(title.q_str());
		theUI.actExportDB->setEnabled(true);
	}
	catch (string &ex) {
		cout<<"::readDB-E- "<<ex<< endl;
	}
}

void thrLoadDB(const char *form)
{
	if (form) {
		try {
			tabUtils *tab = theUI._map_ctrl_forms[form].first;
			tab->rmRow();
			tab->dbin();
			tab->hresize();
			(*tab)->update();
		}
		catch (string &ex) {
			cout<<"::thrLoadDB-E- "<<form<<", "<<ex<<endl;
		}
	}
	else {
		for (MapCtrlForms::iterator it=theUI._map_ctrl_forms.begin();
				it != theUI._map_ctrl_forms.end(); ++it) {
			uistring sform = it->first;
			try {
				tabUtils *tab = theUI._map_ctrl_forms[sform.c_str()].first;
				tab->rmRow();
		    	tab->dbin();
				tab->hresize();
				(*tab)->update();
			}
			catch (string &ex) {
				cout<<"::thrLoadDB-E- "<<sform<<", "<<ex<<endl;
			}
		}
	}
}

void createDB(const char *db)
{
	for (MapCtrlForms::iterator it=theUI._map_ctrl_forms.begin();
			it != theUI._map_ctrl_forms.end(); ++it) {
		uistring sform = it->first;
		try {
			tabUtils *tab = theUI._map_ctrl_forms[sform.c_str()].first;
			tab->rmRow();
			tab->hresize();
			(*tab)->update();
		}
		catch (string &ex) {
			cout<<"::thrLoadDB-E- "<<sform<<", "<<ex<<endl;
		}
	}
	uistring title("AMS BDB quick Viewer (");
	title += common::user() + "@" + uistring(QHostInfo::localHostName()) + ":)";
	theUI._mw->setWindowTitle(title.q_str());
	if (theDB) {
		delete theDB;
		theDB = 0;
	}
	if (!db) return;

	string sdir(db), sfile;
	float version = -1.0;
	boost::filesystem::path cpath(db);
	if (boost::filesystem::exists(cpath) && boost::filesystem::is_directory(cpath)) {
		version = 2.0;
	} else {
		boost::filesystem::path dpath = cpath.parent_path();
		if (boost::filesystem::exists(dpath) && boost::filesystem::is_directory(dpath)) {
			version = 1.0;
			sdir = dpath.string();
			sfile = cpath.filename();
		}
	}
	if (version < 1.0) {
		cout<<"::createDB-E- "<<db<<" is invalid DB"<<endl;
		return;
	}

	try {
		theDB = new bdbRC(sdir,version);
		theDB->db_file(sfile);
		bdbRC db(theDB);
 		db.connect2(DBO_NSL,true);
			nc_dt db_NSL(db,DBO_NSL);
		db.connect2(DBO_NHL,true);
			nh_dt db_NHL(db,DBO_NHL);
		db.connect2(DBO_NKL,true);
			nc_dt db_NKL(db,DBO_NKL);
		db.connect2(DBO_ENV,true);
			env_dt db_ENV(db,DBO_ENV);
		db.connect2(DBO_NCL,true);
			nc_dt db_NCL(db,DBO_NCL);
		db.connect2(DBO_ACL,true);
			ac_dt db_ACL(db,DBO_ACL);
		db.connect2(DBO_AHLP,true);
			ah_dt db_AHLP(db,DBO_AHLP);
		db.connect2(DBO_DSTI,true);
			dsti_dt db_DSTI(db,DBO_DSTI);
		db.connect2(DBO_DSTS,true);
			dsts_dt db_DSTS(db,DBO_DSTS);
		db.connect2(DBO_RTB,true);
			rtb_dt db_RTB(db,DBO_RTB);

		title = "AMS BDB quick Viewer (";
		title += common::user() + "@" + uistring(QHostInfo::localHostName()) + ":" + sdir + ")";
		theUI._mw->setWindowTitle(title.q_str());
	}
	catch (string &ex) {
		cout<<"::createDB-E- "<<ex<< endl;
	}
}

void recoverDB(const char *db)
{
	if (!db) return;
	cout<<"::recoverDB-I- recover DB for "<<db<< endl;
	try {
		DB_ENV *dbe;
		int result = db_env_create(&dbe,0);
		if (result) {
			cout<<"::recoverDB::db_env_create-E- failed with "<<db_strerror(result)<< endl;
			return;
		}
//!!!!!		int flags = DB_CREATE|DB_INIT_LOG|DB_INIT_MPOOL|DB_INIT_TXN|DB_USE_ENVIRON|DB_RECOVER|DB_INIT_LOCK;
		int flags = DB_CREATE|DB_INIT_LOCK|DB_INIT_LOG|DB_INIT_MPOOL|DB_THREAD|DB_INIT_TXN|DB_RECOVER;
		result = dbe->open(dbe,db,flags,0);
		if (result)
			cout<<"::recoverDB::open-E- failed with "<<db_strerror(result)<< endl;
		result = dbe->close(dbe,0);
		if (result)
			cout<<"::recoverDB::close-E- failed with "<<db_strerror(result)<< endl;
	}
	catch (string &ex) {
		cout<<"::recoverDB-E- "<<ex<< endl;
	}
}
