#include "uiwrapper.h"
#include "../dbServer/dbserver.hxx"
#include "../dbServer/bdbRC.hxx"
#include "../dbServer/bdbTraits.hxx"

bool __otherFirst = true;
flm __dfCursor;
flm __dfFrame;
flm __df;
flm __transfer;
flm __scdbFile;
flm __scdb;

//_____________________________________________________________________

process::process() {
	_log = srviface::tmpnm("/tmp/monitorUI_");
}
process::~process() {
	terminate();
}
int process::run(string &cmd) {
	terminate();
	_cmd = srviface::tos()+cmd+" >& "+_log;
	int result = system(_cmd.c_str());
	if (!result) {
		_fs.open(_log.c_str(),fstream::in);
		if (!_fs.is_open())
			return -1;
	}
	return result;
}
void process::terminate() {
	if (_fs.is_open()) {
		_fs.close();
		unlink(_log.c_str());
	}
}
bool process::getline(string &s) {
	if (_fs.is_open() && !std::getline(_fs,s).eof())
		return true;
	return false;
}
string process::lastcmd() const { 	return _cmd; }

qprocess::qprocess(): QProcess() {
	_fs = 0;
}
qprocess::~qprocess() {
	terminate();
}
int qprocess::run(string &cmd) {
	terminate();
	_cmd = cmd;
    start(QString(cmd.c_str()));
    if (waitForFinished(__toSsh*1000)) {
    	_fs = new QTextStream(this);
    	return 0;
    }
	kill();
	return -1;
}
void qprocess::terminate() {
	if (_fs) {
		delete _fs;
		_fs = 0;
	}
//!!!!!-debug qprocess 	QProcess::terminate();
kill();
}
bool qprocess::getline(string &s) {
	if (_fs && !_fs->atEnd()) {
    	s = string(_fs->readLine().toUtf8().constData());
		return true;
	}
	s.clear();
	return false;
}
string qprocess::lastcmd() const { return _cmd; }

flm::flm() { 	result = -3; }
flm::flm(const string &s) { 	parse(s); }
bool flm::configured() const { return (config.size() > 2); }
bool flm::mapped() const { return (file.size() > 2); }
string flm::path() const {
	string result;
	CharStringToken st(config,"/");
	if (st.tokens() > 1)
		for (int i=0; i<st.tokens()-1; i++)
			result += string("/") + st.token(i);
	return result;
}
int flm::parse(const string &s) {
	result = -3;
	CharStringToken st(s.c_str()," \t\r\n");
	if (st.tokens() >= 9)
	{
		int timid = st.ftoken(":");
		int datid = st.rftoken("-",st.tokens()-3);
		if (timid >= 0 && datid >= 0) {
			tm t;
			time_t utc = ::time(0);
			tm *pt = localtime(&utc);
			memcpy(&t,pt,sizeof(tm));
			CharStringToken stime(st.token(timid),":");
			if (stime.tokens() >= 2) {
				t.tm_hour = atoi(stime.token(0));
				t.tm_min = atoi(stime.token(1));
				t.tm_sec = atoi(stime.token(2));
				result++;
			}
			else
				log() << "flm::set-E- reject suspicious time: " << st.token(timid) << endl;
			CharStringToken sdate(st.token(datid),"-");
			if (sdate.tokens() >= 2) {
				t.tm_year = atoi(sdate.token(0))-1900;
				t.tm_mon = atoi(sdate.token(1))-1;
				t.tm_mday = atoi(sdate.token(2));
				result++;
			}
			else
				log() << "flm::set-E- reject suspicious date: " << st.token(datid) << endl;
			time = mktime(&t);
			size = atoi(st.token(datid-1));
			file = st.token(st.tokens()-1);
			return ++result;
		}
	}
	log() << "flm::set-E- reject suspicious string: " << s << endl;
	return result;
}
int flm::parse() { return result; }

//_____________________________________________________________________
srviface::srviface(const string &versus, const string &siors, const string &siorp, const string &siord)
{
	stringstream s;

	_server = "amsprodserver -v" + versus;
	_iors = siors;
	_iorp = siorp;
	_iord = siord;
	_updated = 0;
	_pinged = 0;
	_fails = 0;
	_dbfails = 0;
	_cid.HostName = "*";
	_cid.Interface = "*";
	_cid.StatusType = DPS::Client::OneRunOnly;
	_cid.Status = DPS::Client::NOP;

	_vrdFirst = true;
	flm any_vrd;
	_vrd.push_back(any_vrd);

	struct hostent *local_hostent = gethostbyname(common::hostname());
	if (local_hostent) {
		_ipaddr = local_hostent->h_name;
		for (int i=0; local_hostent->h_aliases[i]!=0; i++)
			_ipalias.push_back(local_hostent->h_aliases[i]);
		string s = "srviface::srviface-I- ip address detected: " + _ipaddr;
		if (_ipalias.size()) {
			s += ", alias(es):";
			for (unsigned i=0; i<_ipalias.size(); i++)
				s += " " + _ipalias[i];
		}
		log()<<s<<endl;
	}
}

srviface::~srviface()
{
}

// class srviface protected

void srviface::build()
{
	_map_ahs_by_host.clear();
	for (int i=0,len=ahs()->length(); i<len; i++) {
		const DPS::Client::ActiveHost &ActiveHost = ahs()[i];
		string sHost(ActiveHost.HostName);
		CharStringToken st(sHost.c_str(),".");
		_map_ahs_by_host.insert(pair<string, int>(string(st.token(0)),i));
	}
	_map_acs_by_host.clear();
	_map_acs_by_uid.clear();
	for (int i=0,len=acs()->length(); i<len; i++) {
		const DPS::Client::ActiveClient &ActiveClient = acs()[i];
		string sHost(ActiveClient.id.HostName);
		CharStringToken st(sHost.c_str(),".");
		if (st.token(0))
			_map_acs_by_host.insert(pair<string, int>(string(st.token(0)),i));
		else
			log() << "srviface::build-W- " << "no acs host for uid=" << ActiveClient.id.uid << endl;
		_map_acs_by_uid.insert(pair<long, int>(ActiveClient.id.uid,i));
	}
	_map_nhs_by_host.clear();
	for (int i=0,len=nhs()->length(); i<len; i++) {
		const DPS::Client::NominalHost &NominalHost = nhs()[i];
		string sHost(NominalHost.HostName);
		CharStringToken st(sHost.c_str(),".");
		if (st.token(0))
			_map_nhs_by_host.insert(pair<string, int>(string(st.token(0)),i));
		else
			log() << "srviface::build-W- " << "no nhs host" << endl;
	}
	_map_res_by_host.clear();
	_map_res_by_run.clear();
	for (int i=0,len=res()->length(); i<len; i++) {
		const DPS::Producer::RunEvInfo RunEvInfo = res()[i];
		if (findRunEvInfoByRunUid(RunEvInfo.Run,RunEvInfo.uid) < 0) {
			_map_res_by_run.insert(pair<long,int>(RunEvInfo.Run,i));
			string sHost(RunEvInfo.cinfo.HostName);
			if (sHost.length() > 1) {
				CharStringToken st(sHost.c_str(),".");
				_map_res_by_host.insert(pair<string, int>(string(st.token(0)),i));
			}
		}
		else
			log() << "srviface::build-E- duplicated run " <<
			boost::format("%d") %RunEvInfo.Run << std::endl;
	}
	_map_dst_by_host.clear();
	_map_dst_by_run.clear();
	for (int i=0,len=dsts()->length(); i<len; i++) {
		const DPS::Producer::DST Dst = dsts()[i];
		_map_dst_by_run.insert(pair<long,int>(Dst.Run,i));
		CharStringToken st(Dst.Name,":");
		int hostid = findAhsByHost(string(st.token(0)));
		if (hostid >= 0) {
			CharStringToken st(ahs()[hostid].HostName,".");
			_map_dst_by_host.insert(pair<string,int>(st.token(0),i));
		}
	}
	_map_res_by_uid.clear();
	for (int i=0,len=res()->length(); i<len; i++) {
		const DPS::Producer::RunEvInfo RunEvInfo = res()[i];
		_map_res_by_uid.insert(pair<long,int>(RunEvInfo.uid,i));
	}
	_map_acs_by_run.clear();
	for (int i=0,len=acs()->length(); i<len; i++) {
		const DPS::Client::ActiveClient &ActiveClient = acs()[i];
		int run = findRunEvInfoByUid(ActiveClient.id.uid);
		if (run >= 0) {
			DPS::Producer::RunEvInfo RunEvInfo = res()[run];
			_map_acs_by_run.insert(pair<long,int>(RunEvInfo.Run,i));
		}
	}
	_map_env_by_var.clear();
	for (int i=0,len=s_env()->length(); i<len; i++) {
		const string s(s_env()[i]);
		if (s.length() > 2) {
			CharStringToken st(s,"=");
			if (st.tokens() == 2) {
				_map_env_by_var.add(st.token(0),st.token(1));
				if (!strcmp(st.token(0),"AMSDataDir"))
					__scdbFile.config = string(st.token(1)) + "/SlowControlDir/SCDB";
			}
		}
	}
	int ifkey;
	if ((ifkey=_map_env_by_var.n_key("AMSLogVrd")) >= 0) {
		CharStringToken st2(_map_env_by_var.get(ifkey),",");
		for (int i=0; i<st2.tokens(); i++) {
			if (i >= static_cast<int>(_vrd.size())) {
				flm any_vrd;
				_vrd.push_back(any_vrd);
			}
			vrd(i).config = st2.token(i);
		}
	}
	if ((ifkey=_map_env_by_var.n_key("AMSLogDf")) >= 0) {
		CharStringToken st2(_map_env_by_var.get(ifkey),":");
		if (st2.tokens() > 1) {
			__logHost["frame_decode.acron"] = st2.token(0);
			__df.config = st2.token(1);
		} else if (st2.tokens() == 1)
			__df.config = st2.token(0);
	}
	if ((ifkey=_map_env_by_var.n_key("AMSLogDfCursor")) >= 0) {
		CharStringToken st2(_map_env_by_var.get(ifkey),":");
		if (st2.tokens() > 1) {
			__logHost["frame_decode.acron"] = st2.token(0);
			__dfCursor.config = st2.token(1);
		} else if (st2.tokens() == 1)
			__dfCursor.config = st2.token(0);
	}
	if ((ifkey=_map_env_by_var.n_key("AMSLogTrc")) >= 0) {
		CharStringToken st2(_map_env_by_var.get(ifkey),":");
		if (st2.tokens() > 1) {
			__logHost["trc.acron"] = st2.token(0);
			__transfer.config = st2.token(1);
		} else if (st2.tokens() == 1)
			__transfer.config = st2.token(0);
	}
	if ((ifkey=_map_env_by_var.n_key("AMSLogScdb")) >= 0) {
		CharStringToken st2(_map_env_by_var.get(ifkey),":");
		if (st2.tokens() > 1) {
			__logHost["scdb.acron"] = st2.token(0);
			__scdb.config = st2.token(1);
		} else if (st2.tokens() == 1)
			__scdb.config = st2.token(0);
	}
	if ((ifkey=_map_env_by_var.n_key("AMSLogScdbFile")) >= 0) {
		CharStringToken st2(_map_env_by_var.get(ifkey),":");
		if (st2.tokens() > 1) {
			__logHost["scdb.acron"] = st2.token(0);
			__scdbFile.config = st2.token(1);
		} else if (st2.tokens() == 1)
			__scdbFile.config = st2.token(0);
	}
}

// class srviface public

srviface *srviface::iface(const char *name)
{
	IFACES::const_iterator it = find_if(__thrClients.begin(),__thrClients.end(),srviface_eq(name));
	if (it != __thrClients.end()) return *it;
	  else						  return 0;
}

longlong srviface::updated() const { return _updated; }

longlong srviface::pinged() const { return _pinged; }

int srviface::fails() const  { return _fails; }

int srviface::dbfails() const  { return _dbfails; }

bool srviface::pingHost(const string &host)
{
	string cmd = "ping -c 1 -w 9 " + host + " >& /dev/null";
	return system(cmd.c_str())==0;
}

const char *srviface::vno(const string &s)
{
	const char *result = strstr(s.c_str(),"-v");
	if (result) return &result[2];
	  else		return result;
}

const char *srviface::vno() const { return vno(_server); }

string srviface::tmpnm(const string &prefix)
{
	stringstream s;
	s << prefix << boost::format("%d.tmp") %static_cast<unsigned>(rand());
	return s.str();
}

string srviface::tos()
{
	stringstream s;
	s << string("/afs/cern.ch/ams/local/bin/timeout --signal 9 ") << boost::format("%u ") %__toSsh;
	return s.str();
}

string srviface::shos()
{
	return string("ssh -x -2 -o \"StrictHostKeyChecking no\" ");
}

flm &srviface::vrd(unsigned id)
{
	if (id < _vrd.size()) return _vrd[id];
	  else				  return _vrd[0];
}

void srviface::request(DPS::Server_var &server, DPS::Producer_var &producer, DPS::DBServer_var &dbserver) throw(string &)
{
    try {
    	double now = timeNow();
     	server->ping();
    	double dur = timeNow() - now;
    	log() << "srviface::request-I- " << _server << " ping time: " << boost::format("   %.06d msec") %dur << endl;
	}
	catch (CORBA::SystemException &ex) {
		_fails++;
        throw string("srviface::request-E- ping error on ")+_server+": "+ex._orbitcpp_get_repoid();
	}
    try {
    	double now = timeNow();
    	dbserver->ping();
    	double dur = timeNow() - now;
    	log() << "srviface::request-I- " << _server << " ping bdb time: " << boost::format("%.06d msec") %dur << endl;
		_dbfails = 0;
		_pinged = timeLongLong();
	}
	catch (CORBA::SystemException &ex) {
		_dbfails++;
		if (_pinged)
	        throw string("srviface::request-E- ping bdb error on ")+_server+": "+ex._orbitcpp_get_repoid();
	}

	string where;
    try {
    	// get everything from amsprodserver
    	double now = timeNow();
		CORBA::ULong maxc;
		CORBA::ULong maxr = 0;

		DPS::Client::CID cid = mycid(DPS::Client::Server);

		where = "server::getEnv";	server->getEnv(cid,_s_env);
		where = "server::getACS";	server->getACS(cid,_s_acs,maxc);
		where = "server::getNHS";	server->getNHS(cid,_s_nhs);
		where = "server::getNC";	server->getNC(cid,_s_ncs);
		where = "server::getNK";	server->getNK(cid,_s_nks);

		_vec_s_acs.clear();
		for (int i=0,len=s_acs()->length(); i<len; i++)
		{
			DPS::Server::DB_var pdb;

			const DPS::Client::ActiveClient &ActiveClient = s_acs()[i];
			where = "server::getDBSpace";
			bool result = server->getDBSpace(ActiveClient.id,CORBA_string_dup("AMSDataDir"),CORBA_string_dup("/DataBase/"),pdb);
			if (result)
				_vec_s_acs.insert(_vec_s_acs.end(),pdb);
		}
		where = "server::getNHS";	server->getNHS(cid,_nhs);

		cid = mycid(DPS::Client::DBServer);

		where = "dbserver::getACS";	server->getACS(cid,_dbs_acs,maxc);

		cid = mycid(DPS::Client::Producer);

		where = "producer::getAHS";	server->getAHS(cid,_ahs);
		where = "producer::getACS";	server->getACS(cid,_acs,maxc);
		where = "producer::getNC";	server->getNC(cid,_ncs);
		where = "producer::getRunEvInfoS";	producer->getRunEvInfoS(cid,_res,maxr);
		where = "producer::getDSTS";		producer->getDSTS(cid,_dsts);
		where = "producer::getDSTInfoS";	producer->getDSTInfoS(cid,_dstis);
    	double dur = timeNow() - now;
    	log() << "srviface::request-I- got data from " << _server << boost::format(" within %.06d msec") %dur << endl;

		// got everything ok, no faults there
		_fails = 0;
		_updated = timeLongLong();
		build();
	}
	catch (CORBA::SystemException &ex) {
		_fails++;
        throw string("srviface::request-E- get error on ")+_server+", "+where;
	}
}

void srviface::request_vrd() throw(string &)
{
	if (!vrd().configured()) return;

	string use_host = __logHost["vrd.acron"];
	for (unsigned i=0; i<_vrd.size(); i++) {
		uistring cmd;
		if (__baseUser == common::user() && use_host != common::hostname())
			cmd = shos() + use_host + " ";
		cmd += string("ls -ltr --dereference --time-style=full-iso ") + vrd(i).config + ".* | tail -1";
		double now = timeNow();

	    qprocess proc;
	    if (!proc.run(cmd)) {
			string svrd;
			proc.getline(svrd);
			if (svrd.size() > 2) {
				__mxProcData.lock();
				vrd(i).parse(svrd);
				__mxProcData.unlock();
				log()<<"srviface::request_vrd-I- got "<<vrd(i).file<<" from "<<_server<<
						boost::format(" within %.03d sec")%((timeNow()-now)/1000)<<endl;
			}
			else
				log()<<string("srviface::request_vrd-E- "+_server+", parse error on: ")<<
						proc.lastcmd()<<endl;
	    }
		else
			throw string("srviface::request_vrd-E- " + _server + ", exec error on: ") + proc.lastcmd();
	}
}

void srviface::request_other() throw(string &)
{
	string def_logPath = "/afs/cern.ch/ams/local/logs/production_logs";
	string def_dfCursor = "/pcposc0_Data/FRAMES/SCIBPB/RT";
	string def_scdbFile = "/afs/cern.ch/ams/Offline/AMSDataDir/SlowControlDir";

	// Deframing cursor
	string use_ssh;
	string use_host = __logHost["frame_decode.acron"];
	if (__baseUser == common::user() && use_host != common::hostname())
		use_ssh = shos() + use_host + " ";
	string srt = def_dfCursor;
	if (__dfCursor.configured()) srt = __dfCursor.config;
	  else try { srt = env().get("SOURCE"); } catch(int) { }
	string cmd =  use_ssh + string("ls -ltr --dereference --time-style=full-iso ") + srt + " | tail -1";
	qprocess proc;
	double now = timeNow();
	if (proc.run(cmd))
		throw string("srviface::request_other-E exec error on ") + _server;
	double dur = timeNow() - now;
	string svrd;
	string last;

	proc.getline(svrd);
	if (svrd.size() > 2) {
		__mxProcData.lock();
		__dfCursor.parse(svrd);
    	__mxProcData.unlock();
    	log() << "request_other-I- got " << __dfCursor.file << " from " << _server << boost::format(" within %.03d sec") %(dur/1000) << endl;

    	// get last frame path
    	cmd = use_ssh + "cat " + srt;
    	if (proc.run(cmd))
    		throw string("srviface::request_other-E exec error on ") + _server;
   		proc.getline(last);
    	if (last.size() < 2) last = "";
	}
	else
		log() << "srviface::request_other-E- parse error on " << _server << ": " << proc.lastcmd() << endl;

	// Last frame
	if (last.size() > 0) {
		cmd = use_ssh + "ls -ltr --dereference --time-style=full-iso " + last;
		now = timeNow();
		if (proc.run(cmd))
			throw string("srviface::request_other-E- exec error on ")+_server;
		dur = timeNow() - now;

		string svrd;
		proc.getline(svrd);
		if (svrd.size() > 2) {
			__mxProcData.lock();
			__dfFrame.parse(svrd);
	    	__mxProcData.unlock();
		   	log() << "request_other-I- got " << __dfFrame.file << " from " << _server << boost::format(" within %.03d sec") %(dur/1000) << endl;
		}
		else
			log() << "srviface::request_other-E- parse error on " << _server << ": " << proc.lastcmd() << endl;
	}

	// Deframing log
	cmd = use_ssh + "ls -ltr --dereference --time-style=full-iso ";
	if (__df.configured()) cmd += __df.config + "*";
	  else cmd += def_logPath + "/df*";
	cmd += " | tail -1";
	now = timeNow();
	if (proc.run(cmd))
		throw string("srviface::request_other-E- exec error on ")+_server;
	dur = timeNow() - now;

	proc.getline(svrd);
	if (svrd.size() > 2) {
		__mxProcData.lock();
    	__df.parse(svrd);
    	__mxProcData.unlock();
	   	log() << "request_other-I- got " << __df.file << " from " << _server << boost::format(" within %.03d sec") %(dur/1000) << endl;
	}
	else
		log() << "srviface::request_other-E- parse error on " << _server << ": " << proc.lastcmd() << endl;

	// Transfer log
	use_ssh.clear();
	use_ssh = __logHost["trc.acron"];
	if (use_ssh != common::hostname())
		use_ssh = shos() + use_ssh + " ";
	cmd = use_ssh + "ls -ltr --dereference --time-style=full-iso ";
	if (__transfer.configured()) cmd += __transfer.config + "*";
	  else cmd += def_logPath + "/transfer*";
	cmd += " | tail -1";
	now = timeNow();
	if (proc.run(cmd))
		throw string("srviface::request_other-E- exec error on ")+_server;
	dur = timeNow() - now;

	proc.getline(svrd);
	if (svrd.size() > 2) {
		__mxProcData.lock();
    	__transfer.parse(svrd);
    	__mxProcData.unlock();
	   	log() << "request_other-I- got " << __transfer.file << " from " << _server << boost::format(" within %.03d sec") %(dur/1000) << endl;
	}
	else
		log() << "srviface::request_other-E- parse error on " << _server << ": " << proc.lastcmd() << endl;

	// Last SCDB file
	use_ssh.clear();
	use_ssh = __logHost["scdb.acron"];
	if (use_ssh != common::hostname())
		use_ssh = shos() +  use_ssh + " ";
	cmd = use_ssh + "ls -ltr --dereference --time-style=full-iso ";
	if (__scdbFile.configured()) cmd += __scdbFile.config + "*";
	  else cmd += def_scdbFile + "/SCDB*";
	cmd += " | tail -1";
	now = timeNow();
	if (proc.run(cmd))
		throw string("srviface::request_other-E- exec error on ")+_server;
	dur = timeNow() - now;

	proc.getline(svrd);
	if (svrd.size() > 2) {
		__mxProcData.lock();
		__scdbFile.parse(svrd);
    	__mxProcData.unlock();
	   	log() << "request_other-I- got " << __scdbFile.file << " from " << _server << boost::format(" within %.03d sec") %(dur/1000) << endl;
	}
	else
		log() << "srviface::request_other-E- parse error on " << _server << ": " << proc.lastcmd() << endl;

	// Last SCDB log file
	cmd = use_ssh + "ls -ltr --dereference --time-style=full-iso ";
	if (__scdb.configured()) cmd += __scdb.config + "*";
	  else cmd += def_logPath + "/scdb*";
	cmd += " | tail -1";
	now = timeNow();
	if (proc.run(cmd))
		throw string("srviface::request_other-E- exec error on ")+_server;
	dur = timeNow() - now;

	proc.getline(svrd);
	if (svrd.size() > 2) {
		__mxProcData.lock();
		__scdb.parse(svrd);
    	__mxProcData.unlock();
	   	log() << "request_other-I- got " << __scdb.file << " from " << _server << boost::format(" within %.03d sec") %(dur/1000) << endl;
	}
	else
		log() << "srviface::request_other-E- parse error on " << _server << ": " << proc.lastcmd() << endl;
}

const imapper<string> &srviface::env() const { return _map_env_by_var; }

const DPS::Client::CID &srviface::mycid(DPS::Client::ClientType type)
{
	_cid.Type = type;
	return _cid;
}

DPS::Client::CID &srviface::cid(const DPS::Client::ACS_var &acs, unsigned id) const throw (string &)
{
	if (id < acs->length())
		return const_cast<DPS::Client::CID &>(acs[id].id);
	else
		throw string("srviface::cid bad id on ")+_server;
}

int srviface::findAhsByHost(const string &host)
{
	CharStringToken st(host.c_str(),".");
	return find(_map_ahs_by_host,string(st.token(0)));
}

vector<int> srviface::findAcsByHost(const string &host)
{
	vector<int> result;
	CharStringToken st(host.c_str(),".");
	find(result,_map_acs_by_host,string(st.token(0)));
	return result;
}

int srviface::findAcsByRun(long run)
{
	return find(_map_acs_by_run,run);
}

int srviface::findAcsByUid(long uid)
{
	return find(_map_acs_by_uid,uid);
}

int srviface::findNhsByHost(const string &host)
{
	CharStringToken st(host.c_str(),".");
	return find(_map_nhs_by_host,string(st.token(0)));
}

vector<int> srviface::findRunEvInfoByHost(const string &host)
{
	vector<int> result;
	CharStringToken st(host.c_str(),".");
	find(result,_map_res_by_host,string(st.token(0)));
	return result;
}

vector<int> srviface::findRunEvInfoByRun(long run)
{
	vector<int> result;
	find(result,_map_res_by_run,run);
	return result;
}

int srviface::findRunEvInfoByRunUid(long run, long uid)
{
	vector<int> runs = findRunEvInfoByRun(run);
	for (unsigned i=0; i<runs.size(); i++) {
		DPS::Producer::RunEvInfo &RunEvInfo = const_cast<DPS::Producer::RunEvInfo &>(res()[runs[i]]);
		if (RunEvInfo.uid == uid)
			return runs[i];
	}
	return -1;
}

int srviface::findRunEvInfoByUid(long uid)
{
	return find(_map_res_by_uid,uid);
}

vector<int> srviface::findDstByHost(const string &host)
{
	vector<int> result;
	CharStringToken st(host.c_str(),".");
	find(result,_map_dst_by_host,string(st.token(0)));
	return result;
}

vector<int> srviface::findDstByRun(long run)
{
	vector<int> result;
	find(result,_map_dst_by_run,run);
	return result;
}

int srviface::findLastDstByRun(long run)
{
	int result = -1;
	vector<int> dstid = findDstByRun(run);
	time_t time = 0;
	for (unsigned i=0; i<dstid.size(); i++)
	{
		const DPS::Producer::DST &Dst = dsts()[dstid[i]];
		if (Dst.Insert > time)
		{
			time = Dst.Insert;
			result = dstid[i];
		}
	}
	return result;
}

void srviface::sendActiveClient(DPS::Client::ActiveClient &ac, DPS::Client::RecordChange rc) const throw (string &)
{
	unsigned long uid = ac.id.uid;
	unsigned long pid = ac.id.pid;
	stringstream s;
	s << boost::format("%u") %uid;
	string suid = s.str();
	s.str("");
	if (rc == DPS::Client::Delete) {
		string achost(ac.id.HostName);
		if (achost.length() < 1) {
			log()<<"srviface::sendActiveClient-E- uid= "<<suid<<", no active host found";
			goto updb;
		}
		const DPS::Server::NCS_var &nks = s_nks();
		unsigned kid = 0;
		for (kid=0; kid<nks->length(); kid++) {
			const DPS::Producer::NominalClient &nk = nks[kid];
			if (achost.find(string(nk.HostName)) != string::npos)
				break;
		}
		if (kid >= nks->length()) {
			string ex = "srviface::sendActiveClient-E- uid= " + suid + ", no killing method found";
			if (achost.find("lxplus") == string::npos) {
				for (kid=0; kid<nks->length(); kid++) {
					const DPS::Producer::NominalClient &nk = nks[kid];
					if (string(nk.HostName).find("defaulthost") != string::npos)
						break;
				}
				if (kid >= nks->length()) {
					log()<<ex<<endl;
					goto updb;
				}
			}
			else {
				log()<<ex<<endl;
				goto updb;
			}
		}

		const DPS::Producer::NominalClient &nk = nks[kid];
		string script = string(nk.WholeScriptPath);
		string submit = string(nk.SubmitCommand);
		string cmd = submit + " " + achost + " " + script + " ";
		if (pid == 0) {
			if (script.find("bkill") != string::npos) {
			    string cmd = "/afs/cern.ch/ams/local/bsubs/" + suid;
				process proc;
				if (!proc.run(cmd)) {
					string si;
					while (proc.getline(si)) {
					}
				}
			}
			else if (script.find("condor") != string::npos) {
				string cmd2 = submit + " " + achost + " cat /afs/cern.ch/ams/local/csubs/" + suid;
				process proc;
				if (!proc.run(cmd2)) {
					string si;
					while (proc.getline(si)) {
						if(si.find("submitted to cluster") == string::npos)
							continue;
						CharStringToken st(si," .");
						if (st.tokens() > 5) pid = atol(st.token(5));
						break;
					}
				}
			}
		}
		if (pid) {
			s << boost::format("%u") %pid;
			if (script.find("bkill") != string::npos)
				cmd += "-s9 ";
			else if (script.find("condor_rm") != string::npos)
				s << boost::format(".%u") %ac.id.ppid;
			else if (script.find("condor_ssh_to_job")!=string::npos) {
				s << boost::format(".%u ") %ac.id.ppid;
				if (ac.id.coid==0) {
					string cmd2 = cmd;
					stringstream s;
					s << boost::format("%d.%d ") %pid %ac.id.ppid <<
						 boost::format("\"ps -ef | grep %s | grep %d,%d \" ") %achost %pid %ac.id.ppid;
					cmd2 +=  s.str();
					log()<<"srviface::sendActiveClient-I- run "<<cmd2<<endl;
					process proc;
					if (!proc.run(cmd2)) {
						string si;
						unsigned coid = ac.id.coid;
						while (proc.getline(si)) {
							char *pch=strtok(const_cast<char *>(si.c_str())," ");
							if(pch) pch=strtok(0," ");
							if(pch) {
							   coid = atol(pch);
							   if(ac.id.coid < coid) ac.id.coid = coid;
							}
						}
					}
					else {
						log()<<"srviface::sendActiveClient-E- exec error on: "<<
								proc.lastcmd()<<", use condor_rm instead"<<endl;
						cmd = submit + " " + achost + " condor_rm ";
						goto sndc;
					}
				}
				log()<<"srviface::sendActiveClient-I- CONDOR PID found "<<ac.id.coid<<endl;
				s << boost::format("kill -s 9 %d ") %ac.id.coid;
			}
			else
				cmd += "-9 ";
		}
		else {
			log()<<"srviface::sendActiveClient-E- uid= "<<suid<<", no pid found"<<endl;
			goto updb;
		}
sndc:
		// finally send job kill command
		string slog = tmpnm("/tmp/monitorUI_");
		cmd += s.str() + " >& " + slog;
		log() << "srviface::sendActiveClient-I- run " << cmd << endl;
		int result = system(cmd.c_str());
		stringstream s2;
		s2 << boost::format("%d") %result;
		if (result != -1) {
			if (result==124 && submit.find("timeout") != string::npos) {
				string ex = "pid " + s.str() + ", exit " + s2.str() +  ", timed out";
				throw ex;
			}
			else {
				fstream fs(slog.c_str(),fstream::in);
				if (fs.is_open()) {
					string lns = "exit " + s2.str() + "\n";
					string ln;
					while (std::getline(fs,ln).good())
						lns += ln + "\n";
					fs.close();
					unlink(slog.c_str());
					log() << lns << endl;
				}
				else {
					string ex = "pid " + s.str() + ", exit " + s2.str() +  ", no " + slog;
					throw ex;
				}
			}
		}
		else {
			string ex = "pid " + s.str() + ", exit " + s2.str() +  ", failed";
			throw ex;
		}
	}
	s.str("");
updb:
	DPS::Server_var pServer;
 	DPS::DBServer_var pDBServer;
 	DPS::Producer_var pProducer;
 	try {
    	CORBA::String_var ref = iors().c_str();
    	pServer = DPS::Server::_narrow(__theOrb->string_to_object(ref));
		ref = iord().c_str();
		pDBServer = DPS::DBServer::_narrow(__theOrb->string_to_object(ref));
		ref = iorp().c_str();
		pProducer = DPS::Producer::_narrow(__theOrb->string_to_object(ref));
    }
    catch (CORBA::SystemException &ex) {
        s << "CORBA::SystemException, " << ex._orbitcpp_get_repoid();
        throw s.str();
    }
    int runid = const_cast<srviface *>(this)->findRunEvInfoByUid(uid);
    try {
	    if (rc == DPS::Client::Delete && runid >= 0) {
	    	DPS::Producer::RunEvInfo &RunEvInfo = const_cast<DPS::Producer::RunEvInfo &>(res()[runid]);
	    	DPS::Producer::RunEvInfo_var runv = new DPS::Producer::RunEvInfo(RunEvInfo);
	    	runv->History = DPS::Producer::Canceled;
	    	pProducer->sendRunEvInfo(runv,DPS::Client::Update);
	    	pDBServer->sendRunEvInfo(runv,DPS::Client::Update);
	    }
		DPS::Client::CID cid = ac.id;
		DPS::Client::CID_var cidv = new DPS::Client::CID(cid);
		DPS::Client::ActiveClient_var acv = new DPS::Client::ActiveClient(ac);
		pServer->sendAC(cidv,acv,rc);
		try {
			pDBServer->sendACPerl(cidv,acv,rc);
		}
	    catch (CORBA::SystemException &ex) {
	    	log() << "srviface::sendActiveClient-W- CORBA::SystemException ex, " << ex._orbitcpp_get_repoid() << endl;
			pDBServer->sendAC(cidv,acv,rc);
	    }
    }
    catch(DPS::DBProblem &dbex) {
    	s << "DPS::DBProblem ex, " << (const char*)dbex.message;
        throw s.str();
    }
    catch (CORBA::SystemException &ex) {
        s << "CORBA::SystemException ex, " << ex._orbitcpp_get_repoid();
        throw s.str();
    }
}

void srviface::sendRun(const DPS::Producer::RunEvInfo &run, DPS::Client::RecordChange rc) const throw (string &)
{
	stringstream s;

 	DPS::Producer_var pProducer;
 	DPS::DBServer_var pDBServer;
    try {
    	CORBA::String_var ref = iorp().c_str();
    	pProducer = DPS::Producer::_narrow(__theOrb->string_to_object(ref));
		ref = iord().c_str();
		pDBServer = DPS::DBServer::_narrow(__theOrb->string_to_object(ref));
    }
    catch (CORBA::SystemException &ex) {
        s << "CORBA::SystemException ex, " << ex._orbitcpp_get_repoid();
        throw s.str();
    }
    try {
		DPS::Producer::RunEvInfo_var runv = new DPS::Producer::RunEvInfo(run);
		pProducer->sendRunEvInfo(runv,rc);
		pDBServer->sendRunEvInfo(runv,rc);
    }
    catch(DPS::DBProblem &dbex){
    	s << "DPS::DBProblem ex, " << (const char*)dbex.message;
        throw s.str();
    }
    catch (CORBA::SystemException &ex) {
        s << "CORBA::SystemException ex, " << ex._orbitcpp_get_repoid();
        throw s.str();
    }
}

const char *srviface::strHostStatus(DPS::Client::HostStatus status)
{
	const char *_strHostStatus[] = { "NoResponse", "InActive", "LastClientFailed", "OK" };
	return _strHostStatus[status];
}

const char *srviface::strClientType(DPS::Client::ClientType type)
{
	const char *_strClientType[] = {
			"Generic", "Server", "Consumer", "Producer",
			"Monitor", "Killer", "DBServer", "ODBServer",
			"Standalone" };
	return _strClientType[type];
}

const char *srviface::strClientStatusType(DPS::Client::ClientStatusType type)
{
	const char *_strClientStatusType[] = { "Permanent", "OneRunOnly" };
	return _strClientStatusType[type];
}

const char *srviface::strClientStatus(DPS::Client::ClientStatus status)
{
	const char *_strClientStatus[] = {
			"Lost", "Unknown", "Submitted", "Registered",
			"Active", "TimeOut", "Killed" };
	return _strClientStatus[status];
}

const char *srviface::strRunMode(DPS::Producer::RunMode mode)
{
	const char *_strRunMode[] = {
			"RILO", "LILO", "RIRO", "LIRO" };
	return _strRunMode[mode];
}

const char *srviface::strRunStatus(DPS::Producer::RunStatus status)
{
	const char *_strRunStatus[] = {
			"ToBeRerun", "Failed", "Processing", "Finished",
			"Unknown", "Allocated", "Foreign", "Canceled" };
	return _strRunStatus[status];
}

const char *srviface::strDSTType(DPS::Producer::DSTType type)
{
	const char *_strDSTType[] = {
			"Ntuple", "EventTag", "RootFile", "RawFile" };
	return _strDSTType[type];
}

const char *srviface::strDSTStatus(DPS::Producer::DSTStatus status)
{
	const char *_strDSTStatus[] = { "Success", "InProgress", "Failure", "Validated" };
	return _strDSTStatus[status];
}

DPS::Client::HostStatus srviface::hostStatus(const char *status)
{
	const char *_strHostStatus[] = { "NoResponse", "InActive", "LastClientFailed", "OK" };
	static imapper<DPS::Client::HostStatus> _mapHostStatus;
	putEnums2Map(_mapHostStatus,_strHostStatus,sizeof(_strHostStatus));
	try	{ return _mapHostStatus.get(status); }
	catch(int) { return static_cast<DPS::Client::HostStatus>(3); }
}

DPS::Client::ClientType srviface::clientType(const char *type)
{
	const char *_strClientType[] = {
			"Generic", "Server", "Consumer", "Producer",
			"Monitor", "Killer", "DBServer", "ODBServer",
			"Standalone" };
	static imapper<DPS::Client::ClientType> _mapClientType;
	putEnums2Map(_mapClientType,_strClientType,sizeof(_strClientType));
	try	{ return _mapClientType.get(type); }
	catch(int) { return static_cast<DPS::Client::ClientType>(0); }
}

DPS::Client::ClientStatusType srviface::clientStatusType(const char *type)
{
	const char *_strClientStatusType[] = { "Permanent", "OneRunOnly" };
	static imapper<DPS::Client::ClientStatusType> _mapClientStatusType;
	putEnums2Map(_mapClientStatusType,_strClientStatusType,sizeof(_strClientStatusType));
	try	{ return _mapClientStatusType.get(type); }
	catch(int) { return static_cast<DPS::Client::ClientStatusType>(1); }
}

DPS::Producer::RunStatus srviface::runStatus(const char *status)
{
	const char *_strRunStatus[] = {
			"ToBeRerun", "Failed", "Processing", "Finished",
			"Unknown", "Allocated", "Foreign", "Canceled"};
	static imapper<DPS::Producer::RunStatus> _mapRunStatus;
	putEnums2Map(_mapRunStatus,_strRunStatus,sizeof(_strRunStatus));
	try	{ return _mapRunStatus.get(status); }
	catch(int) { return static_cast<DPS::Producer::RunStatus>(4); }
}

DPS::Producer::RunMode srviface::runMode(const char *mode)
{
	const char *_strRunMode[] = { "RILO", "LILO", "RIRO", "LIRO" };
	static imapper<DPS::Producer::RunMode> _mapRunMode;
	putEnums2Map(_mapRunMode,_strRunMode,sizeof(_strRunMode));
	try	{ return _mapRunMode.get(mode); }
	catch(int) { return static_cast<DPS::Producer::RunMode>(0); }
}

DPS::Producer::DSTType srviface::dstType(const char *type)
{
	const char *_strDSTType[4] = { "Ntuple", "EventTag", "RootFile", "RawFile" };
	static imapper<DPS::Producer::DSTType> _mapDSTType;
	putEnums2Map(_mapDSTType,_strDSTType,sizeof(_strDSTType));
	try	{ return _mapDSTType.get(type); }
	catch(int) { return static_cast<DPS::Producer::DSTType>(0); }
}

DPS::Producer::DSTStatus srviface::dstStatus(const char *status)
{
	const char *_strDSTStatus[] = { "Success", "InProgress", "Failure", "Validated" };
	static imapper<DPS::Producer::DSTStatus> _mapDSTStatus;
	putEnums2Map(_mapDSTStatus,_strDSTStatus,sizeof(_strDSTStatus));
	try	{ return _mapDSTStatus.get(status); }
	catch(int) { return static_cast<DPS::Producer::DSTStatus>(0); }
}

// Multithreading for GUI

void srviface::resetHosts(srviface *iface)
{
	DPS::Server_var pServer;
 	DPS::DBServer_var pDBServer;
    try
    {
    	CORBA::String_var ref = iface->iors().c_str();
		pServer = DPS::Server::_narrow(__theOrb->string_to_object(ref));
		ref = iface->iord().c_str();
		pDBServer = DPS::DBServer::_narrow(__theOrb->string_to_object(ref));
    }
    catch (CORBA::SystemException &ex)
    {
    	log() << "srviface::resetHosts-E- corba narrow error on " << iface->server() << endl;
        return;
    }

    __mxProcData.lock();
	try
	{
		DPS::Client::RecordChange rc = DPS::Client::Update;
		DPS::Client::CID cid = iface->mycid(DPS::Client::Producer);

		for (int i=0,len=iface->ahs()->length(); i<len; i++)
		{
			DPS::Client::ActiveHost &ActiveHost = const_cast<DPS::Client::ActiveHost &>(iface->ahs()[i]);

			DPS::Producer::HostStatus status = ActiveHost.Status;
			if (status == DPS::Client::InActive)
				continue;
//			ActiveHost.ClientsFailed = 0;
			ActiveHost.Status = DPS::Client::OK;
			try {
				pServer->sendAH(cid,ActiveHost,rc);
				DPS::Client::CID_var cidv = new DPS::Client::CID(cid);
				DPS::Client::ActiveHost_var ahv = new DPS::Client::ActiveHost(ActiveHost);
				pDBServer->sendAH(cidv,ahv,rc);
				log() << "srviface::resetHosts-I- " << "sendAH to " << ActiveHost.HostName << endl;
			}
			catch(DPS::DBProblem &dbex) {
				log() << "srviface::resetHosts-E- ex [" << (const char*)dbex.message << "] on " << iface->server() << endl;
			}
			catch (CORBA::SystemException &ex) {
				log() << "srviface::resetHosts-E- corba send error on " << iface->server() << endl;
			}
		}
	}
	catch (string &ex)
	{
		log() << ex << endl;
	}
	__mxProcData.unlock();

	theUI.setDataRequested();
	__evAskData.raise();
}

void srviface::resetFailedHosts(srviface *iface)
{
	DPS::Server_var pServer;
 	DPS::DBServer_var pDBServer;
    try
    {
    	CORBA::String_var ref = iface->iors().c_str();
		pServer = DPS::Server::_narrow(__theOrb->string_to_object(ref));
		ref = iface->iord().c_str();
		pDBServer = DPS::DBServer::_narrow(__theOrb->string_to_object(ref));
    }
    catch (CORBA::SystemException &ex)
    {
    	log() << "srviface::resetFailedHosts-E- corba narrow error on " << iface->server() << endl;
        return;
    }

    __mxProcData.lock();
	try
	{
		DPS::Client::RecordChange rc = DPS::Client::Update;
		DPS::Client::CID cid = iface->mycid(DPS::Client::Server);

		string enVar = "AMSMaxFailedClientsHost";
		for (int i=0,len=iface->s_env()->length(); i<len; i++) {
			CharStringToken st(static_cast<const char *>(iface->s_env()[i]),"=");
			if (st.tokens() >= 2 && enVar == st.token(0)) {
				pServer->setEnv(cid,enVar.c_str(),"0");
				pDBServer->setEnv(cid,enVar.c_str(),"0");
				Sleep(100);
				pServer->setEnv(cid,enVar.c_str(),st.token(1));
				pDBServer->setEnv(cid,enVar.c_str(),st.token(1));
				log() << "srviface::resetFailedHosts-I- reset "  << enVar << " counters" << endl;
				break;
			}
		}

		cid = iface->mycid(DPS::Client::Producer);

		for (int i=0,len=iface->ahs()->length(); i<len; i++)
		{
			DPS::Client::ActiveHost &ActiveHost = const_cast<DPS::Client::ActiveHost &>(iface->ahs()[i]);

			DPS::Producer::HostStatus status = ActiveHost.Status;
			if (status != DPS::Client::InActive)
				continue;
//			ActiveHost.ClientsFailed = 0;
			ActiveHost.Status = DPS::Client::OK;
			try {
				pServer->sendAH(cid,ActiveHost,rc);
				DPS::Client::CID_var cidv = new DPS::Client::CID(cid);
				DPS::Client::ActiveHost_var ahv = new DPS::Client::ActiveHost(ActiveHost);
				pDBServer->sendAH(cidv,ahv,rc);
				log() << "srviface::resetFailedHosts-I- " << "sendAH to " << ActiveHost.HostName << endl;
			}
			catch(DPS::DBProblem &dbex) {
				log() << "srviface::resetFailedHosts-E- ex [" << (const char*)dbex.message << "] on " << iface->server() << endl;
			}
			catch (CORBA::SystemException &ex) {
				log() << "srviface::resetFailedHosts-E- corba send error on " << iface->server() << endl;
			}
		}
	}
	catch (string &ex)
	{
		log() << ex << endl;
	}
	__mxProcData.unlock();

	theUI.setDataRequested();
	__evAskData.raise();
}

void srviface::resetFailedRuns(prFailedRuns params)
{
	srviface *iface = params._iface;
	bool delDST = params._deldst;

 	DPS::Producer_var pProducer;
 	DPS::DBServer_var pDBServer;
    try
    {
    	CORBA::String_var ref = iface->iorp().c_str();
    	pProducer = DPS::Producer::_narrow(__theOrb->string_to_object(ref));
		ref = iface->iord().c_str();
		pDBServer = DPS::DBServer::_narrow(__theOrb->string_to_object(ref));
    }
    catch (CORBA::SystemException &ex)
    {
    	log() << "srviface::resetFailedRuns-E- corba narrow error on " << iface->server() << endl;
        return;
    }

    __mxProcData.lock();
	try
	{
		DPS::Client::RecordChange rc = DPS::Client::Update;
		DPS::Client::RecordChange rcd = DPS::Client::Delete;
		DPS::Client::CID cid = iface->mycid();

		for (unsigned i=0; i<iface->res()->length(); i++)
		{
			DPS::Producer::RunEvInfo &RunEvInfo = const_cast<DPS::Producer::RunEvInfo &>(iface->res()[i]);
			if (iface->findAcsByUid(RunEvInfo.uid) >= 0)
				continue;

			bool changed = false;
			if (RunEvInfo.Status == DPS::Producer::Processing) {
					RunEvInfo.Status = DPS::Producer::Failed;
					RunEvInfo.History = DPS::Producer::Failed;
					changed = true;
			}
			if (RunEvInfo.Priority > 1 && RunEvInfo.History == DPS::Producer::Failed) {
				RunEvInfo.History = DPS::Producer::ToBeRerun;
				RunEvInfo.CounterFail = 0;
				changed = true;
			}
			if (RunEvInfo.Status == DPS::Producer::Failed) {
				RunEvInfo.Status = DPS::Producer::ToBeRerun;
				if(RunEvInfo.Priority > 1 || RunEvInfo.CounterFail > 6) {
					RunEvInfo.History = DPS::Producer::ToBeRerun;
					RunEvInfo.CounterFail = 0;
				}
				if (RunEvInfo.uid > (1<<26))
					RunEvInfo.Status = DPS::Producer::Foreign;
				changed = true;
			}
			if (RunEvInfo.Status == DPS::Producer::Allocated) {
				RunEvInfo.Status = DPS::Producer::Finished;
				changed = true;
			}
			if (changed) {
				if (RunEvInfo.Status != DPS::Producer::Processing) {
					vector<int> rundst = iface->findDstByRun(RunEvInfo.Run);
					for (unsigned i=0,len=rundst.size(); i<len; i++) {
						DPS::Producer::DST &Dst = const_cast<DPS::Producer::DST &>(iface->dsts()[rundst[i]]);
						if (!delDST) {
							// modify DSTs
							if ((Dst.Type==DPS::Producer::Ntuple || Dst.Type==DPS::Producer::RootFile || Dst.Type==DPS::Producer::RawFile) &&
									Dst.Status==DPS::Producer::InProgress) {
								try {
									Dst.Status = DPS::Producer::Failure;
									pProducer->sendDSTEnd(cid,Dst,rc);
									DPS::Client::CID_var cidv = new DPS::Client::CID(iface->cid(iface->dbs_acs(),0));
									DPS::Producer::DST_var dstv = new DPS::Producer::DST(Dst);
									pDBServer->sendDSTEnd(cidv,dstv,rc);
									log() << "srviface::resetFailedRuns-I- " << "sendDSTEnd to run " << boost::format("%d") %Dst.Run << endl;
								}
								catch(DPS::DBProblem &dbex) {
									log() << "srviface::resetFailedRuns-E- ex [" << (const char*)dbex.message << "] on " << iface->server() << endl;
								}
								catch (CORBA::SystemException &ex) {
									log() << "srviface::resetFailedRuns-E- corba send error on " << iface->server() << endl;
								}
							}
						}
						else {
							// remove DSTs
							try {
								Dst.Status = DPS::Producer::Failure;
								pProducer->sendDSTEnd(cid,Dst,rcd);
								DPS::Client::CID_var cidv = new DPS::Client::CID(iface->cid(iface->dbs_acs(),0));
								DPS::Producer::DST_var dstv = new DPS::Producer::DST(Dst);
								pDBServer->sendDSTEnd(cidv,dstv,rcd);
								log() << "srviface::resetFailedRuns-I- " << "sendDSTEnd to run " << boost::format("%d") %Dst.Run << endl;
							}
							catch(DPS::DBProblem &dbex) {
								log() << "srviface::resetFailedRuns-E- ex [" << (const char*)dbex.message << "] on " << iface->server() << endl;
							}
							catch (CORBA::SystemException &ex) {
								log() << "srviface::resetFailedRuns-E- corba send error on " << iface->server() << endl;
							}
						}
					}
				}
				// modify RunEvInfo
				try {
					pProducer->sendRunEvInfo(RunEvInfo,rc);
					DPS::Producer::RunEvInfo_var rev = new DPS::Producer::RunEvInfo(RunEvInfo);
					pDBServer->sendRunEvInfo(rev,rc);
					log() << "srviface::resetFailedRuns-I- " << "sendRunEvInfo to run " << boost::format("%d") %RunEvInfo.Run << endl;
				}
				catch(DPS::DBProblem &dbex) {
					log() << "srviface::resetFailedRuns-E- ex [" << (const char*)dbex.message << "] on " << iface->server() << endl;
				}
				catch (CORBA::SystemException &ex) {
					log() << "srviface::resetFailedRuns-E- corba send error on " << iface->server() << endl;
				}
			}
		}
	}
	catch (string &ex) {
		log() << ex << endl;
	}
	__mxProcData.unlock();

	theUI.setDataRequested();
	__evAskData.raise();
}

void srviface::resetHistory(srviface *iface)
{
 	DPS::Producer_var pProducer;
 	DPS::DBServer_var pDBServer;
    try
    {
    	CORBA::String_var ref = iface->iorp().c_str();
    	pProducer = DPS::Producer::_narrow(__theOrb->string_to_object(ref));
		ref = iface->iord().c_str();
		pDBServer = DPS::DBServer::_narrow(__theOrb->string_to_object(ref));
    }
    catch (CORBA::SystemException &ex)
    {
    	log() << "srviface::resetHistory-E- corba narrow error on " << iface->server() << endl;
        return;
    }

    __mxProcData.lock();
	try
	{
		DPS::Client::RecordChange rc = DPS::Client::Update;
		DPS::Client::CID cid = iface->mycid();

		for (unsigned i=0; i<iface->res()->length(); i++)
		{
			DPS::Producer::RunEvInfo &RunEvInfo = const_cast<DPS::Producer::RunEvInfo &>(iface->res()[i]);
			if (iface->findAcsByUid(RunEvInfo.uid) >= 0)
							continue;

			if (RunEvInfo.History != DPS::Producer::Failed)
				if (RunEvInfo.History != DPS::Producer::Canceled)
						continue;
			if (RunEvInfo.Status != DPS::Producer::Foreign)
				RunEvInfo.Status = DPS::Producer::Failed;
			RunEvInfo.History = DPS::Producer::ToBeRerun;
			RunEvInfo.CounterFail = 0;

			try {
				pProducer->sendRunEvInfo(RunEvInfo,rc);
				pDBServer->sendRunEvInfo(RunEvInfo,rc);
				log() << "srviface::resetHistory-I- " << "sendRunEvInfo to run " << boost::format("%d") %RunEvInfo.Run << endl;
			}
			catch(DPS::DBProblem &dbex) {
				log() << "srviface::resetHistory-E- ex [" << (const char*)dbex.message << "] on " << iface->server() << endl;
			}
			catch (CORBA::SystemException &ex) {
				log() << "srviface::resetHistory-E- corba send error on " << iface->server() << endl;
			}
		}
	}
	catch (string &ex)
	{
		log() << ex << endl;
	}
	__mxProcData.unlock();

	theUI.setDataRequested();
	__evAskData.raise();
}

void srviface::deleteValidatedDST(srviface *iface)
{
 	DPS::Producer_var pProducer;
 	DPS::DBServer_var pDBServer;
 	try
 	{
    	CORBA::String_var ref = iface->iorp().c_str();
		pProducer = DPS::Producer::_narrow(__theOrb->string_to_object(ref));
		ref = iface->iord().c_str();
		pDBServer = DPS::DBServer::_narrow(__theOrb->string_to_object(ref));
    }
    catch (CORBA::SystemException &ex)
    {
    	log() << "srviface::deleteValidatedDST-E- corba narrow error on " << iface->server() << endl;
        return;
    }

    __mxProcData.lock();
	try {
		DPS::Client::RecordChange rc = DPS::Client::Delete;
		DPS::Client::CID cid = iface->mycid();

		for (int i=0,len=iface->dsts()->length(); i<len; i++) {
			DPS::Producer::DST &Dst = const_cast<DPS::Producer::DST &>(iface->dsts()[i]);

			vector<int> rids = iface->findRunEvInfoByRun(Dst.Run);
			if (rids.size())
				continue;
			if (Dst.Type == DPS::Producer::Ntuple || Dst.Type == DPS::Producer::RootFile ||
				Dst.Type == DPS::Producer::EventTag || Dst.Type == DPS::Producer::RawFile)
				try {
					pProducer->sendDSTEnd(cid,Dst,rc);
					DPS::Client::CID_var cidv = new DPS::Client::CID(iface->cid(iface->dbs_acs(),0));
					DPS::Producer::DST_var dstv = new DPS::Producer::DST(Dst);
					pDBServer->sendDSTEnd(cidv,dstv,rc);
					log() << "srviface::deleteValidatedDST-I- " << "sendDSTEnd to run " << boost::format("%d") %Dst.Run << endl;
				}
				catch(DPS::DBProblem &dbex) {
					log() << "srviface::deleteValidatedDST-E- ex [" << (const char*)dbex.message << "] on " << iface->server() << endl;
				}
				catch (CORBA::SystemException &ex) {
					log() << "srviface::deleteValidatedDST-E- corba send error on " << iface->server() << endl;
				}
		}
	}
	catch (string &ex) {
		log() << ex << endl;
	}
	__mxProcData.unlock();

	theUI.setDataRequested();
	__evAskData.raise();
}

void srviface::deleteFailedDST(srviface *iface)
{
 	DPS::Producer_var pProducer;
 	DPS::DBServer_var pDBServer;
 	try
 	{
    	CORBA::String_var ref = iface->iorp().c_str();
		pProducer = DPS::Producer::_narrow(__theOrb->string_to_object(ref));
		ref = iface->iord().c_str();
		pDBServer = DPS::DBServer::_narrow(__theOrb->string_to_object(ref));
    }
    catch (CORBA::SystemException &ex)
    {
    	log() << "srviface::deleteFailedDST-E- corba narrow error on " << iface->server() << endl;
        return;
    }

    __mxProcData.lock();
	try
	{
		vector<string> nchost;
		vector<string> ncname;
		vector<long>   ncrun;
		vector<time_t> nctime;

		DPS::Client::RecordChange rc = DPS::Client::Delete;
		DPS::Client::CID cid = iface->mycid();
		for (int i=0,len=iface->dsts()->length(); i<len; i++)
		{
			DPS::Producer::DST &Dst = const_cast<DPS::Producer::DST &>(iface->dsts()[i]);

			if (Dst.Type == DPS::Producer::Ntuple || Dst.Type == DPS::Producer::RootFile || Dst.Type == DPS::Producer::RawFile)
			{
				if (Dst.Status == DPS::Producer::Failure)
				{
					CharStringToken sthost(string(Dst.Name),":");
					if (sthost.tokens() < 1) continue;
					string host = sthost.token(0);
					bool hostok = false;
					if (host.find("cern.ch") < 0) {
						CharStringToken sthost(host,".");
						if (sthost.tokens() > 1 && !strcmp(sthost.token(1),"om")) {
							host = string(sthost.token(0)) + ".cern.ch";
							hostok = true;
						}
					}
					else hostok = true;
					if (!hostok) continue;

					bool ifg = false;
					for (int i=0,len=iface->dsts()->length(); i<len; i++) {
						const DPS::Producer::DST &otherDst = iface->dsts()[i];
						if (string(otherDst.Name) == string(Dst.Name) && otherDst.Status != Dst.Status) {
							ifg = true;
							break;
						}
					}

					string cmd = "ssh -2 -x " + host + " rm " + sthost.token(1);
					int result = 0;
					if (!ifg)
						result = system(cmd.c_str());
					try {
						if (result) {
							log() << "srviface::deleteFailedDST-E- " << cmd << " failed" << endl;
							pProducer->sendDSTEnd(cid,Dst,rc);
						}
						else {
							log() << "srviface::deleteFailedDST-E- " << cmd << " ok" << endl;
							pProducer->sendDSTEnd(cid,Dst,rc);
							DPS::Client::CID_var cidv = new DPS::Client::CID(iface->cid(iface->dbs_acs(),0));
							DPS::Producer::DST_var dstv = new DPS::Producer::DST(Dst);
							pDBServer->sendDSTEnd(cidv,dstv,rc);
						}
						log() << "srviface::deleteFailedDST-I- " << "sendDSTEnd to run " << boost::format("%d") %Dst.Run << endl;
					}
					catch(DPS::DBProblem &dbex) {
						log() << "srviface::deleteFailedDST-E- ex [" << (const char*)dbex.message << "] on " << iface->server() << endl;
					}
					catch (CORBA::SystemException &ex) {
						log() << "srviface::deleteFailedDST-E- corba send error on " << iface->server() << endl;
					}
				}
				else if (Dst.Status == DPS::Producer::InProgress)
				{
					for (int i=0,len=iface->acs()->length(); i<len; i++) {
						const DPS::Client::ActiveClient &ActiveClient = iface->acs()[i];
						if (ActiveClient.id.uid == static_cast<unsigned long>(Dst.Run))
						{
							CharStringToken sthost(string(Dst.Name),":");
							if (sthost.tokens() < 1) continue;
							CharStringToken stfh(sthost.token(0),".");
							if (stfh.tokens() < 2) continue;
							string ac_hostname(ActiveClient.id.HostName);
							if (!strcmp(sthost.token(0),ac_hostname.c_str()) ||
								(!strcmp(stfh.token(1),"om") && ac_hostname.find(stfh.token(0)) >= 0) ||
								(!strcmp(stfh.token(1),"local") && ac_hostname.find(stfh.token(0)) >= 0) ||
								(strstr(sthost.token(0),"ams") && ac_hostname.find("pcamsf2") >= 0))
							{
								nchost.push_back(ac_hostname);
								ncname.push_back(string(Dst.Name));
								ncrun.push_back(Dst.Run);
								nctime.push_back(Dst.Insert);
							}
						}
					}
				}
			}
		}

		int good = 0;
		ivector<string> ncgood;
		vector<string> ncgh;
		vector<long>   ncgr;

		for (unsigned i=0; i<ncrun.size(); i++) {
			if (ncrun[i] == 0)
				continue;
			time_t ltime = nctime[i];
			unsigned idef = i;
			for (unsigned j=i+1; j<ncrun.size(); j++)
				if(ncrun[i] == ncrun[j]) {
					ncrun[j] = 0;
					if(ltime < nctime[j]){
						ltime = nctime[j];
						idef = j;
					}
				}
			ncgood.add(ncname[idef]);
			ncgh.push_back(nchost[idef]);
			ncgr.push_back(ncrun[i]);
			good++;
		}
		log() << "srviface::deleteFailedDST-I- " << "in progress good=%d " << boost::format("%d") %good << endl;
		for (int i=0,len=iface->dsts()->length(); i<len; i++) {
			DPS::Producer::DST &Dst = const_cast<DPS::Producer::DST &>(iface->dsts()[i]);

			if ((Dst.Type == DPS::Producer::Ntuple || Dst.Type == DPS::Producer::RootFile || Dst.Type == DPS::Producer::RawFile) &&
				Dst.Status == DPS::Producer::InProgress && ncgood.n_value(string(Dst.Name)) < 0)
				try {
					Dst.Status = DPS::Producer::Failure;
					DPS::Client::RecordChange rc = DPS::Client::Update;
					pProducer->sendDSTEnd(cid,Dst,rc);
					log() << "srviface::deleteFailedDST-I- " << "sendDSTEnd update to run " << boost::format("%d") %Dst.Run << endl;
				}
				catch(DPS::DBProblem &dbex) {
					log() << "srviface::deleteFailedDST-E- ex [" << (const char*)dbex.message << "] on " << iface->server() << endl;
				}
				catch (CORBA::SystemException &ex) {
					log() << "srviface::deleteFailedDST-E- corba send error on " << iface->server() << endl;
				}
		}
		bool ok = true;
		for (unsigned i=0; i<ncgr.size()&&ok; i++) {
			stringstream s;
			s << boost::format("%d") %ncgr[i];
			string move = "ssh -2 -x " + ncgh[i] + " mv /dat0/local/logs/MCProducer." + s.str() + ".log /dat0/local/logs/aMCProducer." + s.str() + ".log";
			if (system(move.c_str()))
				ok = false;
		}
		if (ok)
			for (unsigned i=0; i<ncgr.size(); i++) {
				string rm = "ssh -2 -x " + ncgh[i] + " rm  -rf '/dat0/local/logs/MCProducer*.log'";
				system(rm.c_str());
				rm = "ssh -2 -x " + ncgh[i] + " rm  -rf '/dat0/local/logs/Killer*.log'";
				system(rm.c_str());
			}
		for (unsigned i=0; i<ncgr.size(); i++) {
			stringstream s;
			s << boost::format("%d") %ncgr[i];
			string move = "ssh -2 -x " + ncgh[i] + " mv /dat0/local/logs/aMCProducer." + s.str() + ".log /dat0/local/logs/MCProducer." + s.str() + ".log";
			system(move.c_str());
		}
		log() << "srviface::deleteFailedDST-I- everything finished" << endl;
	}
	catch (string &ex)
	{
		log() << ex << endl;
	}
	__mxProcData.unlock();

	theUI.setDataRequested();
	__evAskData.raise();
}

void srviface::deleteAClients(srviface *iface)
{
	__mxProcData.lock();

	vector<int> clients;
	theUI.visibleClients(iface,clients);

	for (unsigned i=0; i<clients.size(); i++) {
		DPS::Client::ActiveClient &ActiveClient = const_cast<DPS::Client::ActiveClient &>(iface->acs()[clients[i]]);
	    try {
	    	log()<<"srviface::deleteAClients-I- AC uid "<<boost::format("%u") %ActiveClient.id.uid<<
	    		" Status="<<strClientStatus(ActiveClient.Status)<<endl;
	    	iface->sendActiveClient(ActiveClient,DPS::Client::Delete);
	    	log()<<"srviface::deleteAClients-I- AC uid "<<boost::format("%u") %ActiveClient.id.uid<<
	    		" done"<<endl;
	    }
	    catch (string &ex) {
	    	log() << "srviface::deleteAClients-E- AC uid=" << boost::format("%u") %ActiveClient.id.uid << ", " << ex << endl;
	    }
	}
	__mxProcData.unlock();

	theUI.setDataRequested();
	__evAskData.raise();
}

void srviface::deleteRuns(srviface *iface)
{
	__mxProcData.lock();

	vector<int> runs, clients;
	theUI.visibleClients(iface,clients);
	theUI.visibleRuns(iface,runs);

	for (unsigned i=0; i<clients.size(); i++) {
		DPS::Client::ActiveClient &ActiveClient = const_cast<DPS::Client::ActiveClient &>(iface->acs()[clients[i]]);
	    try {
	    	log() << "srviface::deleteRuns-I- AC uid " << boost::format("%u") %ActiveClient.id.uid <<
	    		" Status=" << strClientStatus(ActiveClient.Status) << endl;
	    	iface->sendActiveClient(ActiveClient,DPS::Client::Delete);
	    }
	    catch (string &ex) {
	    	log() << "srviface::deleteRuns-E- AC uid=" << boost::format("%u") %ActiveClient.id.uid << ", " << ex << endl;
	    	for (unsigned i=0; i<runs.size(); i++) {
	    		DPS::Producer::RunEvInfo RunEvInfo = iface->res()[runs[i]];
	    		if (ActiveClient.id.uid == RunEvInfo.uid) {
	    			runs.erase(runs.begin()+i);
	    			break;
	    		}
	    	}
	    }
	}
	for (unsigned i=0; i<runs.size(); i++) {
		DPS::Producer::RunEvInfo RunEvInfo = iface->res()[runs[i]];
	    try {
	    	log() << "srviface::deleteRuns-I- RT uid=" << boost::format("%u") %RunEvInfo.uid <<
	    		" Run=" << boost::format("%u") %RunEvInfo.Run <<
				" Status=" << strRunStatus(RunEvInfo.Status) << endl;
	    	iface->sendRun(RunEvInfo,DPS::Client::Delete);
	    }
	    catch (string &ex) {
	    	log() << "srviface::deleteRuns-E- RT uid=" << boost::format("%u") %RunEvInfo.uid << ", " << ex << endl;
	    }
	}
	__mxProcData.unlock();

	theUI.setDataRequested();
	__evAskData.raise();
}

void srviface::finishFailedRuns(srviface *iface)
{
	__mxProcData.lock();
	for (unsigned i=0; i<iface->res()->length(); i++)
	{
		DPS::Producer::RunEvInfo RunEvInfo = iface->res()[i];
		bool changed = false;
		if (RunEvInfo.Status == DPS::Producer::Processing && iface->findAcsByUid(RunEvInfo.cuid) < 0) {
			RunEvInfo.Status  = DPS::Producer::Failed;
			RunEvInfo.History = DPS::Producer::Failed;
			changed = true;
		}
		if (RunEvInfo.Status == DPS::Producer::Failed || RunEvInfo.History == DPS::Producer::Failed ||
			RunEvInfo.Status == DPS::Producer::Allocated) {
			vector<int> dstsl = iface->findDstByRun(RunEvInfo.Run);
			for (unsigned i=0,len=dstsl.size(); i<len; i++)
			{
				const DPS::Producer::DST &Dst = iface->dsts()[dstsl[i]];
				if(Dst.Type == DPS::Producer::Ntuple || Dst.Type == DPS::Producer::RootFile || Dst.Type == DPS::Producer::RawFile)
					if(Dst.Status == DPS::Producer::Validated || Dst.Status == DPS::Producer::Success) {
						RunEvInfo.Status = DPS::Producer::Finished;
						changed = true;
						break;
					}
			}
			if (RunEvInfo.uid > (1<<26)) {
				RunEvInfo.Status = DPS::Producer::Foreign;
				changed = true;
			}
		}
		if(RunEvInfo.Status == DPS::Producer::Allocated) {
			RunEvInfo.Status = DPS::Producer::Finished;
			changed = true;
		}
		if (changed)
		    try {
		    	log() << "srviface::finishFailedRuns-I- RTuid " << boost::format("%u") %RunEvInfo.uid << endl;
		    	iface->sendRun(RunEvInfo,DPS::Client::Update);
		    }
		    catch (string &ex) {
		    	log() << ex << endl;
		    }
	}
	__mxProcData.unlock();

	theUI.setDataRequested();
	__evAskData.raise();
}

void srviface::updateAFSToken(srviface *iface)
{
	uistring cmd = "kinit -R";
    qprocess proc;
    if (proc.run(cmd)) {
    	log()<<"srviface::updateAFSToken-E- exec error on: "<<proc.lastcmd()<<endl;
    	return;
    }
    try {
    	kread();
    	log()<<"srviface::updateAFSToken-I- finished OK"<<endl;
    }
    catch (string &ex) {
    	log()<<"srviface::updateAFSToken-E- failed with ex: "<<ex<< endl;
    }
}

void srviface::bdbPut(string &env, string &file, bool merge) throw (string &)
{
	bdbTraits::instantiate_all();
	fstream fsexp;
	try {
		__mxProcData.lock();
		// bdb version 0
		bdbRC db(env,0.0);
		// bdb version 1
		string file2 = file + ".v1";
		bdbRC db2(env,1.0);
		db2.version(1.0);

		// also export to text file
		string file2exp = env + "/" + file2 + ".exp";
		fsexp.open(file2exp.c_str(),fstream::out|fstream::trunc);
		if (!fsexp.is_open())
	    	log()<<"srviface::bdbPut-E- text export can not be done to "<<file2exp<<endl;

		// Server::NominalClient
		db.connect2(file,DBO_NSL,true);
		nc_dt db_ncs(db,DBO_NSL);
		db2.connect2(file2,DBO_NSL,true);
		nc_dt db_ncs2(db2,DBO_NSL);
		if (!merge) {
			db_ncs.erase();
			db_ncs2.erase();
		}
		if (fsexp.is_open())
			fsexp << "<<" << DBO_NSL << ">>" << endl;
		for (int i=0,len=s_ncs()->length(); i<len; i++) {
			const DPS::Producer::NominalClient &Nc = s_ncs()[i];
			if (!merge) {
				db_ncs.insert(const_cast<DPS::Producer::NominalClient &>(Nc));
				db_ncs2.insert(const_cast<DPS::Producer::NominalClient &>(Nc));
			}
			if (fsexp.is_open())
				fsexp << "<::uid>" << Nc.uid <<
					"<::Type>" << Nc.Type <<
					"<::MaxClients>" << Nc.MaxClients <<
					"<::CPUNeeded>" << Nc.CPUNeeded <<
					"<::MemoryNeeded>" << Nc.MemoryNeeded <<
					"<::WholeScriptPath>" << (const char*)Nc.WholeScriptPath <<
					"<::LogPath>" << (const char*)Nc.LogPath <<
					"<::SubmitCommand>" << (const char*)Nc.SubmitCommand <<
					"<::HostName>" << (const char*)Nc.HostName <<
					"<::LogInTheEnd>" << Nc.LogInTheEnd << endl;
		}
		if (fsexp.is_open()) fsexp << endl;

		// Server::NominalHost
		nh_dt db_nhs(db,DBO_NHL);
		db2.connect2(file2,DBO_NHL,true);
		nh_dt db_nhs2(db2,DBO_NHL);
		if (!merge) {
			db_nhs.erase();
			db_nhs2.erase();
		}
		if (fsexp.is_open())
			fsexp << "<<" << DBO_NHL << ">>" << endl;
		for (int i=0,len=s_nhs()->length(); i<len; i++) {
			const DPS::Producer::NominalHost &Nh = s_nhs()[i];
			if (!merge) {
				db_nhs.insert(const_cast<DPS::Producer::NominalHost &>(Nh));
				db_nhs2.insert(const_cast<DPS::Producer::NominalHost &>(Nh));
			}
			if (fsexp.is_open())
				fsexp << "<::HostName>" << (const char*)Nh.HostName <<
					"<::Interface>" << (const char*)Nh.Interface <<
					"<::OS>" << (const char*)Nh.OS <<
					"<::CPUNumber>" << Nh.CPUNumber <<
					"<::Memory>" << Nh.Memory <<
					"<::Clock>" << Nh.Clock << endl;
		}
		if (fsexp.is_open()) fsexp << endl;

		// Server::NominalKiller
		nc_dt db_nks(db,DBO_NKL);
		db2.connect2(file2,DBO_NKL,true);
		nc_dt db_nks2(db2,DBO_NKL);
		if (!merge) {
			db_nks.erase();
			db_nks2.erase();
		}
		if (fsexp.is_open())
			fsexp << "<<" << DBO_NKL << ">>" << endl;
		for (int i=0,len=s_nks()->length(); i<len; i++) {
			const DPS::Producer::NominalClient &Nk = s_nks()[i];
			if (!merge) {
				db_nks.insert(const_cast<DPS::Producer::NominalClient &>(Nk));
				db_nks2.insert(const_cast<DPS::Producer::NominalClient &>(Nk));
			}
			if (fsexp.is_open())
				fsexp << "<::uid>" << Nk.uid <<
					"<::Type>" << Nk.Type <<
					"<::MaxClients>" << Nk.MaxClients <<
					"<::CPUNeeded>" << Nk.CPUNeeded <<
					"<::MemoryNeeded>" << Nk.MemoryNeeded <<
					"<::WholeScriptPath>" << (const char*)Nk.WholeScriptPath <<
					"<::LogPath>" << (const char*)Nk.LogPath <<
					"<::SubmitCommand>" << (const char*)Nk.SubmitCommand <<
					"<::HostName>" << (const char*)Nk.HostName <<
					"<::LogInTheEnd>" << Nk.LogInTheEnd << endl;
		}
		if (fsexp.is_open()) fsexp << endl;

		// Server::SetEnvironment
		env_dt db_env(db,DBO_ENV);
		db2.connect2(file2,DBO_ENV,true);
		env_dt db_env2(db2,DBO_ENV);
		if (!merge) {
			db_env.erase();
			db_env2.erase();
		}
		if (fsexp.is_open())
			fsexp << "<<" << DBO_ENV << ">>" << endl;
		for (int i=0,len=s_env()->length(); i<len; i++) {
			const _orbitcpp::String_var<char>& svar = s_env()[i];
			if (!merge) {
				db_env.insert(const_cast<_orbitcpp::String_var<char>&>(svar));
				db_env2.insert(const_cast<_orbitcpp::String_var<char>&>(svar));
			}
			if (fsexp.is_open())
				fsexp << "<::>" << (const char*)svar << endl;
		}
		if (fsexp.is_open()) fsexp << endl;

		// Producer::NominalClient
		nc_dt db_pncs(db,DBO_NCL);
		db2.connect2(file2,DBO_NCL,true);
		nc_dt db_pncs2(db2,DBO_NCL);
		if (!merge) {
			db_pncs.erase();
			db_pncs2.erase();
		}
		if (fsexp.is_open())
			fsexp << "<<" << DBO_NCL << ">>" << endl;
		for (int i=0,len=ncs()->length(); i<len; i++) {
			const DPS::Producer::NominalClient &Nc = ncs()[i];
			if (!merge) {
				db_pncs.insert(const_cast<DPS::Producer::NominalClient &>(Nc));
				db_pncs2.insert(const_cast<DPS::Producer::NominalClient &>(Nc));
			}
			if (fsexp.is_open())
				fsexp << "<::uid>" << Nc.uid <<
					"<::Type>" << Nc.Type <<
					"<::MaxClients>" << Nc.MaxClients <<
					"<::CPUNeeded>" << Nc.CPUNeeded <<
					"<::MemoryNeeded>" << Nc.MemoryNeeded <<
					"<::WholeScriptPath>" << (const char*)Nc.WholeScriptPath <<
					"<::LogPath>" << (const char*)Nc.LogPath <<
					"<::SubmitCommand>" << (const char*)Nc.SubmitCommand <<
					"<::HostName>" << (const char*)Nc.HostName <<
					"<::LogInTheEnd>" << Nc.LogInTheEnd << endl;
		}
		if (fsexp.is_open()) fsexp << endl;

		// Producer::ActiveClient
		ac_dt db_pacs(db,DBO_ACL);
		db2.connect2(file2,DBO_ACL,true);
		ac_dt db_pacs2(db2,DBO_ACL);
		if (!merge) {
			db_pacs.erase();
			db_pacs2.erase();
		}
		if (fsexp.is_open())
			fsexp << "<<" << DBO_ACL << ">>" << endl;
		for (int i=0,len=acs()->length(); i<len; i++) {
			const DPS::Client::ActiveClient &ActiveClient = acs()[i];
			db_pacs.insert(const_cast<DPS::Client::ActiveClient &>(ActiveClient));
			db_pacs2.insert(const_cast<DPS::Client::ActiveClient &>(ActiveClient));
			if (fsexp.is_open()) {
				fsexp << "<::id::HostName>" << (const char*)ActiveClient.id.HostName <<
					"<::id::uid>" << ActiveClient.id.uid <<
					"<::id::pid>" << ActiveClient.id.pid <<
					"<::id::ppid>" << ActiveClient.id.ppid <<
					"<::id::threads>" << ActiveClient.id.threads <<
					"<::id::threads_change>" << ActiveClient.id.threads_change <<
					"<::id::Type>" << ActiveClient.id.Type <<
					"<::id::StatusType>" << ActiveClient.id.StatusType <<
					"<::id::Status>" << ActiveClient.id.Status <<
					"<::id::Interface>" << (const char*)ActiveClient.id.Interface <<
					"<::id::Mips>" << ActiveClient.id.Mips
					<< "<::id::coid>" << ActiveClient.id.coid;	// CID::coid field added
				for (unsigned j=0; j<ActiveClient.ars.length(); j++) {
					const DPS::Client::ActiveRef &arf = ActiveClient.ars[j];
					fsexp << "<::ars::IOR>" << (const char*)arf.IOR <<
						"<::ars::Interface>" << (const char*)arf.Interface <<
						"<::ars::Type>" << arf.Type <<
						"<::ars::uid>" << arf.uid;
				}
				fsexp << "<::LastUpdate>"<< ActiveClient.LastUpdate <<
						"<::Start>"<< ActiveClient.Start <<
						"<::TimeOut>"<< ActiveClient.TimeOut <<
						"<::Status>"<< ActiveClient.Status <<
						"<::StatusType>"<< ActiveClient.StatusType << endl;
			}
		}
		if (fsexp.is_open()) fsexp << endl;

		// Producer::NominalHost
		ah_dt db_pahs(db,DBO_AHLP);
		db2.connect2(file2,DBO_AHLP,true);
		ah_dt db_pahs2(db2,DBO_AHLP);
		if (!merge) {
			db_pahs.erase();
			db_pahs2.erase();
		}
		if (fsexp.is_open())
			fsexp << "<<" << DBO_AHLP << ">>" << endl;
		for (int i=0,len=ahs()->length(); i<len; i++) {
			const DPS::Producer::ActiveHost &Ah = ahs()[i];
			if (!merge) {
				db_pahs.insert(const_cast<DPS::Producer::ActiveHost &>(Ah));
				db_pahs2.insert(const_cast<DPS::Producer::ActiveHost &>(Ah));
			}
			if (fsexp.is_open())
				fsexp << "<::HostName>" << (const char*)Ah.HostName <<
					"<::Interface>" << (const char*)Ah.Interface <<
					"<::Status>" << Ah.Status <<
					"<::ClientsRunning>" << Ah.ClientsRunning <<
					"<::ClientsAllowed>" << Ah.ClientsAllowed <<
					"<::ClientsProcessed>" << Ah.ClientsProcessed <<
					"<::ClientsFailed>" << Ah.ClientsFailed <<
					"<::ClientsKilled>" << Ah.ClientsKilled <<
					"<::LastFailed>" << Ah.LastFailed <<
					"<::LastUpdate>" << Ah.LastUpdate <<
					"<::Clock>" << Ah.Clock << endl;
		}
		if (fsexp.is_open()) fsexp << endl;

		// Producer::NominalNTuple
		dsti_dt db_pdsti(db,DBO_DSTI);
		db2.connect2(file2,DBO_DSTI,true);
		dsti_dt db_pdsti2(db2,DBO_DSTI);
		if (!merge) {
			db_pdsti.erase();
			db_pdsti2.erase();
		}
		if (fsexp.is_open())
			fsexp << "<<" << DBO_DSTI << ">>" << endl;
		for (int i=0,len=dstis()->length(); i<len; i++) {
			const DPS::Producer::DSTInfo &Dsti = dstis()[i];
			if (!merge) {
				db_pdsti.insert(const_cast<DPS::Producer::DSTInfo &>(Dsti));
				db_pdsti2.insert(const_cast<DPS::Producer::DSTInfo &>(Dsti));
			}
			if (fsexp.is_open())
				fsexp << "<::type>" << Dsti.type <<
					"<::uid>" << Dsti.uid <<
					"<::HostName>" << Dsti.HostName <<
					"<::OutputDirPath>" << Dsti.OutputDirPath <<
					"<::Mode>" << Dsti.Mode <<
					"<::UpdateFreq>" << Dsti.UpdateFreq <<
					"<::DieHard>" << Dsti.DieHard <<
					"<::FreeSpace>" << Dsti.FreeSpace <<
					"<::TotalSpace>" << Dsti.TotalSpace << endl;
		}
		if (fsexp.is_open()) fsexp << endl;

		// Producer::ProducedNTuple
		dsts_dt db_pdsts(db,DBO_DSTS);
		db2.connect2(file2,DBO_DSTS,true);
		dsts_dt db_pdsts2(db2,DBO_DSTS);
		if (!merge) {
			db_pdsts.erase();
			db_pdsts2.erase();
		}
		if (fsexp.is_open())
			fsexp << "<<" << DBO_DSTS << ">>" << endl;
		for (int i=0,len=dsts()->length(); i<len; i++) {
			const DPS::Producer::DST &Dst = dsts()[i];
			db_pdsts.insert(const_cast<DPS::Producer::DST &>(Dst));
			db_pdsts2.insert(const_cast<DPS::Producer::DST &>(Dst));
			if (fsexp.is_open())
				fsexp << "<::Insert>" << Dst.Insert <<
					"<::Begin>" << Dst.Begin <<
					"<::End>" << Dst.End <<
					"<::Run>" << Dst.Run <<
					"<::FirstEvent>" << Dst.FirstEvent <<
					"<::LastEvent>" << Dst.LastEvent <<
					"<::EventNumber>" << Dst.EventNumber <<
					"<::ErrorNumber>" << Dst.ErrorNumber <<
					"<::Status>" << Dst.Status <<
					"<::Type>" << Dst.Type <<
					"<::size>" << Dst.size <<
					"<::Name>" << (const char*)Dst.Name <<
					"<::Version>" << (const char*)Dst.Version <<
					"<::crc>" << Dst.crc <<
					"<::rndm1>" << Dst.rndm1 <<
					"<::rndm2>" << Dst.rndm2 <<
					"<::FreeSpace>" << Dst.FreeSpace <<
					"<::TotalSpace>" << Dst.TotalSpace << endl;
		}
		if (fsexp.is_open()) fsexp << endl;

		// Producer::RunTable
		rtb_dt db_prtb(db,DBO_RTB);
		db2.connect2(file2,DBO_RTB,true);
		rtb_dt db_prtb2(db2,DBO_RTB);
		if (!merge) {
			db_prtb.erase();
			db_prtb2.erase();
		}
		if (fsexp.is_open())
			fsexp << "<<" << DBO_RTB << ">>" << endl;
		for (unsigned i=0; i<res()->length(); i++) {
			const DPS::Producer::RunEvInfo RunEvInfo = res()[i];
			db_prtb.insert(const_cast<DPS::Producer::RunEvInfo &>(RunEvInfo));
			db_prtb2.insert(const_cast<DPS::Producer::RunEvInfo &>(RunEvInfo));
			if (fsexp.is_open())
				fsexp << "<::uid>" << RunEvInfo.uid <<
					"<::Run>" << RunEvInfo.Run <<
					"<::FirstEvent>" << RunEvInfo.FirstEvent <<
					"<::LastEvent>" << RunEvInfo.LastEvent <<
					"<::TFEvent>" << RunEvInfo.TFEvent <<
					"<::TLEvent>" << RunEvInfo.TLEvent <<
					"<::Priority>" << RunEvInfo.Priority <<
					"<::FilePath>" << (const char*)RunEvInfo.FilePath <<
					"<::Status>" << RunEvInfo.Status <<
					"<::History>" << RunEvInfo.History <<
					"<::CounterFail>" << RunEvInfo.CounterFail <<
					"<::SubmitTime>" << RunEvInfo.SubmitTime <<
					"<::cuid>" << RunEvInfo.cuid <<
					"<::rndm1>" << RunEvInfo.rndm1 <<
					"<::rndm2>" << RunEvInfo.rndm2 <<
					"<::DataMC>" << RunEvInfo.DataMC <<
					"<::cinfo::Run>" << RunEvInfo.cinfo.Run <<
					"<::cinfo::EventsProcessed>" << RunEvInfo.cinfo.EventsProcessed <<
					"<::cinfo::LastEventProcessed>" << RunEvInfo.cinfo.LastEventProcessed <<
					"<::cinfo::CriticalErrorsFound>" << RunEvInfo.cinfo.CriticalErrorsFound <<
					"<::cinfo::ErrorsFound>" << RunEvInfo.cinfo.ErrorsFound <<
					"<::cinfo::CPUTimeSpent>" << RunEvInfo.cinfo.CPUTimeSpent <<
					"<::cinfo::TimeSpent>" << RunEvInfo.cinfo.TimeSpent <<
					"<::cinfo::Mips>" << RunEvInfo.cinfo.Mips <<
					"<::cinfo::CPUMipsTimeSpent>" << RunEvInfo.cinfo.CPUMipsTimeSpent <<
					"<::cinfo::Status>" << RunEvInfo.cinfo.Status <<
					"<::cinfo::HostName>" << (const char*)RunEvInfo.cinfo.HostName << endl;
		}
		if (fsexp.is_open()) fsexp << endl;

		if (fsexp.is_open()) fsexp.close();
		__mxProcData.unlock();
	}
	catch (string &ex) {
		if (fsexp.is_open()) fsexp.close();
		__mxProcData.unlock();
		throw;
	}
}

void srviface::bdbPutPerl(string &env, string &file) throw (string &)
{
	string shost;
	char host[128];
	if (gethostname(host,128)) shost = "ams";
	  else shost = host;
	int id = 1;
	if (s_acs()->length() > 0)
		id = s_acs()[0].id.uid;
	string slog = srviface::tmpnm("/tmp/monitorUI_");

	stringstream  cmd;
	cmd << "ssh " << shost << " -2 -x -o \'StrictHostKeyChecking no\' ";
	cmd << "\'cd " << env << " && ./dbserver.exp.perl ";
	cmd << "-v" <<  vno() << " ";
	cmd << "-U" <<  boost::format("%d") %id << " ";
	cmd << "-i" << _iors << " ";
	fstream fs(file.c_str(),fstream::in|ios_base::out);
    if (fs.good()) cmd << "-F";
      else		   cmd << "-A";
	cmd << file << "\' ";
	cmd << ">& " << slog;

	__mxProcData.lock();
	system(cmd.str().c_str());
	fstream fsl(slog.c_str(),fstream::in);
	if (fsl.is_open()) {
		string s;
		while (!std::getline(fsl,s).eof())
			log() << s << endl;
		fsl.close();
		unlink(slog.c_str());
	}
	else {
		__mxProcData.unlock();
		throw string("srviface::bdbPutPerl-E- open error on " + _server + ", failed command: " + cmd.str());
	}
	__mxProcData.unlock();
}

void srviface::bdbGetText(string &env, string &file, bool merge) throw (string &)
{
	string file4imp = env + "/" + file;
	fstream fsimp(file4imp.c_str(),fstream::in);
	if (!fsimp.is_open())
		throw string(file4imp + " open error");
	string dbfile = file + "-imported";

	bdbTraits::instantiate_all();
	// bdb version 1 only
	bdbRC db(env,1.0);
	db.version(1.0);

	try {
		__mxProcData.lock();

		db.connect2(dbfile,DBO_NSL,true);
			nc_dt db_NSL(db,DBO_NSL);
			db_NSL.erase();
		db.connect2(dbfile,DBO_NHL,true);
			nh_dt db_NHL(db,DBO_NHL);
			db_NHL.erase();
		db.connect2(dbfile,DBO_NKL,true);
			nc_dt db_NKL(db,DBO_NKL);
			db_NKL.erase();
		db.connect2(dbfile,DBO_ENV,true);
			env_dt db_ENV(db,DBO_ENV);
			db_ENV.erase();
		db.connect2(dbfile,DBO_NCL,true);
			nc_dt db_NCL(db,DBO_NCL);
			db_NCL.erase();
		db.connect2(dbfile,DBO_ACL,true);
			ac_dt db_ACL(db,DBO_ACL);
			db_ACL.erase();
		db.connect2(dbfile,DBO_AHLP,true);
			ah_dt db_AHLP(db,DBO_AHLP);
			db_AHLP.erase();
		db.connect2(dbfile,DBO_DSTI,true);
			dsti_dt db_DSTI(db,DBO_DSTI);
			db_DSTI.erase();
		db.connect2(dbfile,DBO_DSTS,true);
			dsts_dt db_DSTS(db,DBO_DSTS);
			db_DSTS.erase();
		db.connect2(dbfile,DBO_RTB,true);
			rtb_dt db_RTB(db,DBO_RTB);
			db_RTB.erase();

		DPS::Producer::NominalClient Nc;
		DPS::Producer::NominalHost Nh;
		DPS::Producer::NominalClient Nk;
		CORBA::String_var senv;
		DPS::Client::ActiveClient ActiveClient;
		DPS::Client::ActiveRef arf;
		int ars_len = 0;
		DPS::Producer::ActiveHost Ah;
		DPS::Producer::DSTInfo Dsti;
		DPS::Producer::DST Dst;
		DPS::Producer::RunEvInfo rei;

		string s;
		dbTables type;
		while (!std::getline(fsimp,s).eof()) {
			if (s.find("<<") == 0) {	// new bdb table found
				type = (dbTables)boost::lexical_cast<int>(s.substr(2,s.find(">>")-2));
				continue;
			}
			CharStringToken st(s);
			st.strtok2("<::");
			try {
				int ntokens = 0;
				for (int i=0; i<st.tokens(); i++) {
					string ss = st.token(i);
					string ssv;
					if (ss.find('>') != string::npos)
						ssv = ss.substr(ss.find('>')+1);
					ntokens++;
					switch (type) {
					case DBO_NSL: {
						if (ss.find("uid>") == 0)
							Nc.uid = boost::lexical_cast<unsigned long>(ssv);
						else if (ss.find("Type>") == 0)
							Nc.Type = (DPS::Producer::ClientType)boost::lexical_cast<int>(ssv);
						else if (ss.find("MaxClients>") == 0)
							Nc.MaxClients = boost::lexical_cast<short>(ssv);
						else if (ss.find("CPUNeeded>") == 0)
							Nc.CPUNeeded = boost::lexical_cast<float>(ssv);
						else if (ss.find("MemoryNeeded>") == 0)
							Nc.MemoryNeeded = boost::lexical_cast<short>(ssv);
						else if (ss.find("WholeScriptPath>") == 0)
							Nc.WholeScriptPath = ssv.c_str();
						else if (ss.find("LogPath>") == 0)
							Nc.LogPath = ssv.c_str();
						else if (ss.find("SubmitCommand>") == 0)
							Nc.SubmitCommand = ssv.c_str();
						else if (ss.find("HostName>") == 0)
							Nc.HostName = ssv.c_str();
						else if (ss.find("LogInTheEnd>") == 0)
							Nc.LogInTheEnd = boost::lexical_cast<short>(ssv);
						else
							ntokens--;
					}
						break;
					case DBO_NHL: {
						if (ss.find("HostName>") == 0)
							Nh.HostName = ssv.c_str();
						else if (ss.find("Interface>") == 0)
							Nh.Interface = ssv.c_str();
						else if (ss.find("OS>") == 0)
							Nh.OS = ssv.c_str();
						else if (ss.find("CPUNumber>") == 0)
							Nh.CPUNumber = boost::lexical_cast<short>(ssv);
						else if (ss.find("Memory>") == 0)
							Nh.Memory = boost::lexical_cast<short>(ssv);
						else if (ss.find("Clock>") == 0)
							Nh.Clock = boost::lexical_cast<long>(ssv);
						else
							ntokens--;
					}
						break;
					case DBO_NKL: {
						if (ss.find("uid>") == 0)
							Nk.uid = boost::lexical_cast<unsigned long>(ssv);
						else if (ss.find("Type>") == 0)
								Nk.Type = (DPS::Producer::ClientType)boost::lexical_cast<int>(ssv);
						else if (ss.find("MaxClients>") == 0)
								Nk.MaxClients = boost::lexical_cast<short>(ssv);
						else if (ss.find("CPUNeeded>") == 0)
								Nk.CPUNeeded = boost::lexical_cast<float>(ssv);
						else if (ss.find("MemoryNeeded>") == 0)
								Nk.MemoryNeeded = boost::lexical_cast<short>(ssv);
						else if (ss.find("WholeScriptPath>") == 0)
								Nk.WholeScriptPath = ssv.c_str();
						else if (ss.find("LogPath>") == 0)
								Nk.LogPath = ssv.c_str();
						else if (ss.find("SubmitCommand>") == 0)
								Nk.SubmitCommand = ssv.c_str();
						else if (ss.find("HostName>") == 0)
								Nk.HostName = ssv.c_str();
						else if (ss.find("LogInTheEnd>") == 0)
								Nk.LogInTheEnd  = boost::lexical_cast<short>(ssv);
						else
							ntokens--;
					}
						break;
					case DBO_ENV: {
						senv = ssv.c_str();
					}
						break;
					case DBO_NCL: {
						if (ss.find("uid>") == 0)
							Nc.uid = boost::lexical_cast<unsigned long>(ssv);
						else if (ss.find("Type>") == 0)
							Nc.Type = (DPS::Producer::ClientType)boost::lexical_cast<int>(ssv);
						else if (ss.find("MaxClients>") == 0)
							Nc.MaxClients = boost::lexical_cast<short>(ssv);
						else if (ss.find("CPUNeeded>") == 0)
							Nc.CPUNeeded = boost::lexical_cast<float>(ssv);
						else if (ss.find("MemoryNeeded>") == 0)
							Nc.MemoryNeeded = boost::lexical_cast<short>(ssv);
						else if (ss.find("WholeScriptPath>") == 0)
							Nc.WholeScriptPath = ssv.c_str();
						else if (ss.find("LogPath>") == 0)
							Nc.LogPath = ssv.c_str();
						else if (ss.find("SubmitCommand>") == 0)
							Nc.SubmitCommand = ssv.c_str();
						else if (ss.find("HostName>") == 0)
							Nc.HostName = ssv.c_str();
						else if (ss.find("LogInTheEnd>") == 0)
							Nc.LogInTheEnd = boost::lexical_cast<short>(ssv);
						else
							ntokens--;
					}
						break;
					case DBO_ACL: {
						if (ss.find("id::HostName>") == 0)
								ActiveClient.id.HostName = ssv.c_str();
						else if (ss.find("id::uid>") == 0)
								ActiveClient.id.uid = boost::lexical_cast<unsigned long>(ssv);
						else if (ss.find("id::pid>") == 0)
								ActiveClient.id.pid = boost::lexical_cast<unsigned long>(ssv);
						else if (ss.find("id::ppid>") == 0)
								ActiveClient.id.ppid = boost::lexical_cast<unsigned long>(ssv);
						else if (ss.find("id::threads>") == 0)
								ActiveClient.id.threads = boost::lexical_cast<unsigned long>(ssv);
						else if (ss.find("id::threads_change>") == 0)
								ActiveClient.id.threads_change = boost::lexical_cast<long>(ssv);
						else if (ss.find("id::Type>") == 0)
								ActiveClient.id.Type = (DPS::Producer::ClientType)boost::lexical_cast<int>(ssv);
						else if (ss.find("id::StatusType>") == 0)
								ActiveClient.id.StatusType = (DPS::Producer::ClientStatusType)boost::lexical_cast<int>(ssv);
						else if (ss.find("id::Status>") == 0)
								ActiveClient.id.Status = (DPS::Producer::ClientExiting)boost::lexical_cast<int>(ssv);
						else if (ss.find("id::Interface>") == 0)
								ActiveClient.id.Interface = ssv.c_str();
						else if (ss.find("id::Mips>") == 0)
								ActiveClient.id.Mips = boost::lexical_cast<float>(ssv);
						else if (ss.find("id::coid>") == 0)
								ActiveClient.id.coid = boost::lexical_cast<unsigned long>(ssv);
						else if (ss.find("ars::IOR>") == 0)
								arf.IOR = ssv.c_str();
						else if (ss.find("ars::Interface>") == 0)
								arf.Interface = ssv.c_str();
						else if (ss.find("ars::Type>") == 0)
								arf.Type = (DPS::Producer::ClientType)boost::lexical_cast<int>(ssv);
						else if (ss.find("ars::uid>") == 0) {
								arf.uid = boost::lexical_cast<unsigned long>(ssv);
								ActiveClient.ars.length(ars_len+1);
								DPS::Client::ActiveRef_var arfv = new DPS::Client::ActiveRef(arf);
								ActiveClient.ars[ars_len++] = arfv;
						}
						else if (ss.find("LastUpdate>") == 0)
								ActiveClient.LastUpdate = boost::lexical_cast<time_t>(ssv);
						else if (ss.find("Start>") == 0)
								ActiveClient.Start = boost::lexical_cast<time_t>(ssv);
						else if (ss.find("TimeOut>") == 0)
								ActiveClient.TimeOut = boost::lexical_cast<time_t>(ssv);
						else if (ss.find("Status>") == 0)
								ActiveClient.Status = (DPS::Producer::ClientStatus)boost::lexical_cast<int>(ssv);
						else if (ss.find("StatusType>") == 0)
								ActiveClient.StatusType = (DPS::Producer::ClientStatusType)boost::lexical_cast<int>(ssv);
						else
							ntokens--;
					}
						break;
					case DBO_AHLP: {
						if (ss.find("HostName>") == 0)
								Ah.HostName = ssv.c_str();
						else if (ss.find("Interface>") == 0)
								Ah.Interface = ssv.c_str();
						else if (ss.find("Status>") == 0)
								Ah.Status = (DPS::Producer::HostStatus)boost::lexical_cast<int>(ssv);
						else if (ss.find("ClientsRunning>") == 0)
								Ah.ClientsRunning = boost::lexical_cast<short>(ssv);
						else if (ss.find("ClientsAllowed>") == 0)
								Ah.ClientsAllowed = boost::lexical_cast<short>(ssv);
						else if (ss.find("ClientsProcessed>") == 0)
								Ah.ClientsProcessed = boost::lexical_cast<long>(ssv);
						else if (ss.find("ClientsFailed>") == 0)
								Ah.ClientsFailed = boost::lexical_cast<long>(ssv);
						else if (ss.find("ClientsKilled>") == 0)
								Ah.ClientsKilled = boost::lexical_cast<long>(ssv);
						else if (ss.find("LastFailed>") == 0)
								Ah.LastFailed = boost::lexical_cast<time_t>(ssv);
						else if (ss.find("LastUpdate>") == 0)
								Ah.LastUpdate = boost::lexical_cast<time_t>(ssv);
						else if (ss.find(":Clock>") == 0)
								Ah.Clock = boost::lexical_cast<long>(ssv);
						else
							ntokens--;
					}
						break;
					case DBO_DSTI: {
						if (ss.find("type>") == 0)
							Dsti.type = (DPS::Producer::DSTType)boost::lexical_cast<int>(ssv);
						else if (ss.find("uid>") == 0)
							Dsti.uid = boost::lexical_cast<unsigned long>(ssv);
						else if (ss.find("HostName>") == 0)
							Dsti.HostName = ssv.c_str();
						else if (ss.find("OutputDirPath>") == 0)
							Dsti.OutputDirPath = ssv.c_str();
						else if (ss.find("Mode>") == 0)
							Dsti.Mode = (DPS::Producer::RunMode)boost::lexical_cast<int>(ssv);
						else if (ss.find("UpdateFreq>") == 0)
							Dsti.UpdateFreq = boost::lexical_cast<long>(ssv);
						else if (ss.find("DieHard>") == 0)
							Dsti.DieHard = boost::lexical_cast<long>(ssv);
						else if (ss.find("FreeSpace>") == 0)
							Dsti.FreeSpace = boost::lexical_cast<long>(ssv);
						else if (ss.find("TotalSpace>") == 0)
							Dsti.TotalSpace = boost::lexical_cast<long>(ssv);
						else
							ntokens--;
					}
						break;
					case DBO_DSTS: {
						if (ss.find("Insert>") == 0)
							Dst.Insert = boost::lexical_cast<time_t>(ssv);
						else if (ss.find("Begin>") == 0)
							Dst.Begin = boost::lexical_cast<time_t>(ssv);
						else if (ss.find("End>") == 0)
							Dst.End = boost::lexical_cast<time_t>(ssv);
						else if (ss.find("Run>") == 0)
							Dst.Run = boost::lexical_cast<long>(ssv);
						else if (ss.find("FirstEvent>") == 0)
							Dst.FirstEvent = boost::lexical_cast<long>(ssv);
						else if (ss.find("LastEvent>") == 0)
							Dst.LastEvent = boost::lexical_cast<long>(ssv);
						else if (ss.find("EventNumber>") == 0)
							Dst.EventNumber = boost::lexical_cast<long>(ssv);
						else if (ss.find("ErrorNumber>") == 0)
							Dst.ErrorNumber = boost::lexical_cast<long>(ssv);
						else if (ss.find("Status>") == 0)
							Dst.Status = (DPS::Producer::DSTStatus)boost::lexical_cast<int>(ssv);
						else if (ss.find("Type>") == 0)
							Dst.Type = (DPS::Producer::DSTType)boost::lexical_cast<int>(ssv);
						else if (ss.find("size>") == 0)
							Dst.size = boost::lexical_cast<long>(ssv);
						else if (ss.find("Name>") == 0)
							Dst.Name = ssv.c_str();
						else if (ss.find("Version>") == 0)
							Dst.Version = ssv.c_str();
						else if (ss.find("crc>") == 0)
							Dst.crc = boost::lexical_cast<unsigned long>(ssv);
						else if (ss.find("rndm1>") == 0)
							Dst.rndm1 = boost::lexical_cast<unsigned long>(ssv);
						else if (ss.find("rndm2>") == 0)
							Dst.rndm2 = boost::lexical_cast<unsigned long>(ssv);
						else if (ss.find("FreeSpace>") == 0)
							Dst.FreeSpace = boost::lexical_cast<long>(ssv);
						else if (ss.find("TotalSpace>") == 0)
							Dst.TotalSpace = boost::lexical_cast<long>(ssv);
						else
							ntokens--;
					}
						break;
					case DBO_RTB: {
						if (ss.find("uid>") == 0)
							rei.uid = boost::lexical_cast<unsigned long>(ssv);
						else if (ss.find("Run>") == 0)
							rei.Run = boost::lexical_cast<unsigned long>(ssv);
						else if (ss.find("FirstEvent>") == 0)
							rei.FirstEvent = boost::lexical_cast<long>(ssv);
						else if (ss.find("LastEvent>") == 0)
							rei.LastEvent = boost::lexical_cast<long>(ssv);
						else if (ss.find("TFEvent>") == 0)
							rei.TFEvent = boost::lexical_cast<time_t>(ssv);
						else if (ss.find("TLEvent>") == 0)
							rei.TLEvent = boost::lexical_cast<time_t>(ssv);
						else if (ss.find("Priority>") == 0)
							rei.Priority = boost::lexical_cast<long>(ssv);
						else if (ss.find("FilePath>") == 0)
							rei.FilePath = ssv.c_str();
						else if (ss.find("Status>") == 0)
							rei.Status = (DPS::Producer::RunStatus)boost::lexical_cast<int>(ssv);
						else if (ss.find("History>") == 0)
							rei.History = (DPS::Producer::RunStatus)boost::lexical_cast<int>(ssv);
						else if (ss.find("CounterFail>") == 0)
							rei.CounterFail = boost::lexical_cast<unsigned long>(ssv);
						else if (ss.find("SubmitTime>") == 0)
							rei.SubmitTime = boost::lexical_cast<time_t>(ssv);
						else if (ss.find("cuid>") == 0)
							rei.cuid = boost::lexical_cast<unsigned long>(ssv);
						else if (ss.find("rndm1>") == 0)
							rei.rndm1 = boost::lexical_cast<unsigned long>(ssv);
						else if (ss.find("rndm2>") == 0)
							rei.rndm2 = boost::lexical_cast<unsigned long>(ssv);
						else if (ss.find("DataMC>") == 0)
							rei.DataMC = boost::lexical_cast<long>(ssv);
						else if (ss.find("cinfo::Run>") == 0)
							rei.cinfo.Run = boost::lexical_cast<unsigned long>(ssv);
						else if (ss.find("cinfo::EventsProcessed>") == 0)
							rei.cinfo.EventsProcessed = boost::lexical_cast<long>(ssv);
						else if (ss.find("cinfo::LastEventProcessed>") == 0)
							rei.cinfo.LastEventProcessed = boost::lexical_cast<long>(ssv);
						else if (ss.find("cinfo::CriticalErrorsFound>") == 0)
							rei.cinfo.CriticalErrorsFound = boost::lexical_cast<long>(ssv);
						else if (ss.find("cinfo::ErrorsFound>") == 0)
							rei.cinfo.ErrorsFound = boost::lexical_cast<long>(ssv);
						else if (ss.find("cinfo::CPUTimeSpent>") == 0)
							rei.cinfo.CPUTimeSpent = boost::lexical_cast<float>(ssv);
						else if (ss.find("cinfo::TimeSpent>") == 0)
							rei.cinfo.TimeSpent = boost::lexical_cast<float>(ssv);
						else if (ss.find("cinfo::Mips>") == 0)
							rei.cinfo.Mips = boost::lexical_cast<float>(ssv);
						else if (ss.find("cinfo::CPUMipsTimeSpent>") == 0)
							rei.cinfo.CPUMipsTimeSpent = boost::lexical_cast<float>(ssv);
						else if (ss.find("cinfo::Status>") == 0)
							rei.cinfo.Status = (DPS::Producer::RunStatus)boost::lexical_cast<int>(ssv);
						else if (ss.find("cinfo::HostName>") == 0)
							rei.cinfo.HostName = ssv.c_str();
						else
							ntokens--;
					}
						break;
					default:
						ntokens = 0;
						break;
					}
				}
				if (ntokens > 0) {
					switch (type) {
					case DBO_NSL:
						db_NSL.insert(Nc);
						break;
					case DBO_NHL:
						db_NHL.insert(Nh);
						break;
					case DBO_NKL:
						db_NKL.insert(Nk);
						break;
					case DBO_ENV:
						db_ENV.insert(senv);
						break;
					case DBO_NCL:
						db_NCL.insert(Nc);
						break;
					case DBO_ACL:
						db_ACL.insert(ActiveClient);
						ActiveClient.ars.length(0);
						ars_len = 0;
						break;
					case DBO_AHLP:
						db_AHLP.insert(Ah);
						break;
					case DBO_DSTI:
						db_DSTI.insert(Dsti);
						break;
					case DBO_DSTS:
						db_DSTS.insert(Dst);
						break;
					case DBO_RTB:
						db_RTB.insert(rei);
						break;
					default:
						break;
					}
				}
			}
		    catch(boost::bad_lexical_cast &ex) {
		    	throw string("boost::bad_lexical_cast ex");
		    }
		}

		fsimp.close();
		__mxProcData.unlock();
	}
	catch (string &ex) {
		fsimp.close();
		__mxProcData.unlock();
		throw;
	}
	log()<<"srviface::bdbGetText-I- imported to "<<env<<"/"<<dbfile<<" bdb"<<endl;
}

void srviface::klist() throw (string &)
{
	string cmd = "klist -f";
    qprocess proc;
    if (!proc.run(cmd)) {
		const char *key = "FILE:/tmp/krb5";
		const char *logf = "/tmp/acrontabklist.log";
		string result;
		proc.getline(result);
		int pos2 = result.find(key);
		if (pos2 >= 0) {
			int pose2 = result.find("\0",pos2);
			string s2 = result.substr(pos2+strlen(key),pose2+1);
			fstream fs(logf,fstream::in);
			if (fs.is_open()) {
				string s;
				while (!std::getline(fs,s).eof()) {
					int pos4 = s.find(key);
					if (pos4 >= 0) {
						int pose4 = s.find("\0",pos4);
						string s4 = s.substr(pos4+strlen(key),pose4+1);
						cmd = string("cp /tmp/_krb5") + s4 + " /tmp/krb5" + s2;
						fs.close();
						if (proc.run(cmd)) {
						    string ex = proc.lastcmd() + " failed";
						    throw ex;
						}
						log()<<"srviface::klist-I- "<<proc.lastcmd()<<endl;
						return;
					}
				}
				fs.close();
			    string ex = string("no ") + key + " found in " + logf;
			    throw ex;
			}
			else {
			    string ex = string("no ") + logf + " found";
			    throw ex;
			}
		}
		else {
		    string ex = string("no ") + key + " found";
		    throw ex;
		}
    }
    string ex = "klist -f timeout";
    throw ex;
}

void srviface::kread() throw (string &)
{
	string cmd = "/usr/sue/bin/klist -f";
	string cmd2 = "/usr/bin/klist -f";
    qprocess proc;
    proc.run(cmd);
    if (!proc.run(cmd) || !proc.run(cmd2)) {
    	cmd2 = proc.lastcmd();
		const char *key = "FILE:/tmp/krb5";
		const char *logf = "/tmp/acrontabklist.log";
		string result;
		proc.getline(result);
		int pos4 = result.find(key);
		if (pos4 >= 0) {
			int pose4 = result.find("\0",pos4);
			string s4 = result.substr(pos4+strlen(key),pose4+1);
			cmd = string("cp /tmp/krb5") + s4 + " /tmp/_krb5" + s4;
			if (proc.run(cmd)) {
				string ex = proc.lastcmd() + " failed";
				throw ex;
			}
			log()<<"srviface::kread-I- "<<proc.lastcmd()<<endl;
			if (proc.run(cmd2)) {
				string ex = proc.lastcmd() + " failed";
				throw ex;
			}
			fstream fs(logf,fstream::out|fstream::trunc);
			if (fs.is_open()) {
				while (proc.getline(result))
					fs<<result<<endl;
				fs.close();
				log()<<"srviface::kread-I- update "<<logf<<endl;
				return;
			}
			else {
			    string ex = string("can not write to ") + logf;
			    throw ex;
			}
		}
		else {
		    string ex = string("no ") + key + " found ";
		    throw ex;
		}
    }
    string ex = "klist -f timeout";
    throw ex;
}
