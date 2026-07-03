#ifndef  _BDBRC_H
#define  _BDBRC_H

#include <boost/filesystem.hpp>
#include "bdbTraits.hxx"
#include "smart_ptr.hxx"

typedef enum dbTables {
	DBO_AHLS, DBO_ASL, DBO_NHL, DBO_NSL,
	DBO_NKL, DBO_ENV, DBO_DB, DBO_RN,
	DBO_AHLP, DBO_ACL, DBO_NCL, DBO_RTB,
	DBO_DSTI, DBO_DSTS, DBO_ADBSL, DBO_AML
} dbTables;

//_____________________________________________________________________
class bdbRC
{
public:
	bdbRC(const string &path, float version=BDB_VERSION) throw(string &) {
		DbEnv *penv = dbstl::open_env(path.c_str(),DB_CXX_NO_EXCEPTIONS,DB_CREATE|DB_INIT_MPOOL|DB_INIT_CDB|DB_THREAD);
		if (penv) {
			_env = new smart_ptr<DbEnv>(penv);
			_db = 0;
			_env_path = path;
			_version = version;
		}
		else
			throw string("bdbRC::bdbRC-E- bad db environment");
	};
	explicit bdbRC(const bdbRC *p) throw (string &) {
		_env = new smart_ptr<DbEnv>(*p->_env);
		_db = 0;
		_env_path = p->_env_path;
		_file = p->_file;
		_type = p->_type;
		_version = p->_version;
	};
	bdbRC(const bdbRC &p) throw (string &) {
		_env = new smart_ptr<DbEnv>(*p._env);
		_db = 0;
		_env_path = p._env_path;
		_file = p._file;
		_type = p._type;
		_version = p._version;
	};
	virtual ~bdbRC() {
		disconnect();
		if (_env->last()) {
			dbstl::close_db_env(_env->ptr());
			_env->release();
		}
		delete _env;
	};

private:
	smart_ptr<DbEnv> *_env;
	Db *_db;
	string _env_path;
	string _file;
	dbTables _type;
	float _version;

protected:
	static string id2str(dbTables id) {
		stringstream s;
		try {
			s << boost::format("%d") %id;
			return s.str();
		}
	    catch(boost::bad_lexical_cast &ex) { return string(); }
	};
	static string id2str(float version) {
		stringstream s;
		try {
			s << boost::format("%d") %static_cast<dbTables>(version);
			return s.str();
		}
	    catch(boost::bad_lexical_cast &ex) { return string(); }
	};

public:
	/* was never used
	static int keycmp(Db *db, const Dbt *dbt1, const Dbt *dbt2)
	{
		int *i1 = (int *)dbt1->get_data();
		int *i2 = (int *)dbt2->get_data();
		int ir = *i1 - *i2;
		if (ir < 0) return -1;
		  else if (ir > 0) return 1;
		  else return 0;
	};
	*/
	DbEnv *env() const {
		return _env->ptr();
	};
	Db *db() {
		return _db;
	};
	string env_path() const {
		return _env_path;
	};
	string db_file() const {
		return _file;
	};
	void db_file(const string &dbfile) {
		try {
			disconnect();
			_file = dbfile;
		}
		catch(string &ex) {}
	};
	dbTables type() {
		return _type;
	};
	float version() const {
		return _version;
	};
	void  version(float vers) {
		_version = vers;
	};
	static float exists(const string &dbfile) {
		boost::filesystem::path dpath(dbfile.c_str());
		if (boost::filesystem::exists(dpath)) {
			if (boost::filesystem::is_regular_file(dpath))
				return 1.0;
			if (boost::filesystem::is_directory(dpath)) {
				boost::filesystem::path epath(string(string(dbfile)+"/"+id2str(DBO_ENV)).c_str());
				if (boost::filesystem::is_regular_file(epath))
					return 2.0;
			}
		}
		return -1.0;
	};
	// connect to DB version 0
	/* not used anymore
 	bool connect(const string &dbfile, bool create_ena=false, u_int32_t oflags=0) throw(string &) {
		if (_version >= 1.0) {
			stringstream s;
			s<<"bdbRC::connect-E- not applicable db version"<<_version;
			throw s.str();
		}
		_file = dbfile;
		bool init = false;
		try {
			_db = dbstl::open_db(_env->ptr(),_file.c_str(),DB_HASH,oflags,0);
		}
		catch(DbException &ex) 	{
			if (create_ena) {
				oflags |= DB_CREATE;
				try {
					_db = dbstl::open_db(_env->ptr(),_file.c_str(),DB_HASH,oflags,DB_DUP);
					dbstl::register_db(_db);
					cout << "bdbRC::connect-I- DB file " << _env_path << "/" << _file << " created" << endl;
					init = true;
				}
				catch(std::exception &ex) { throw string(string("bdbRC::connect-E- ")+ex.what()); }
			}
			else throw string(string("bdbRC::connect-E- ")+ex.what());
		}
		catch(std::exception &ex) { throw string(string("bdbRC::connect-E- ")+ex.what()); }
		// _db->set_h_compare(keycmp);
		return init;
	};
	*/
	// connect to DB version 1,2
	bool connect2(const string &dbfile, dbTables type, bool create_ena=false, u_int32_t oflags=0) throw(string &) {
try { disconnect(); } catch(string &ex) { string(string("bdbRC::connect2-E- ")+ex); }
		_type = type;
		string stype = bdbRC::id2str(type);
		_file = (dbfile.length())?dbfile:stype;
		const char *ptype=0;
		if (_version >= 1.0)
			ptype = stype.c_str();
		bool init = false;
		try {
			_db = dbstl::open_db(_env->ptr(),_file.c_str(),DB_HASH,oflags,0,0444,NULL,0,ptype);
		}
		catch(DbException &ex) 	{
			if (create_ena) {
				oflags |= DB_CREATE|DB_THREAD;
				try {
					_db = dbstl::open_db(_env->ptr(),_file.c_str(),DB_HASH,oflags,DB_DUP,0644,NULL,0,ptype);
					dbstl::register_db(_db);
					string s = "bdbRC::connect2-I- DB file " + _env_path + "/" + _file;
					if (ptype) s += "::" + stype;
					cout << s << " created" << endl;
					init = true;
				}
				catch(std::exception &ex) { throw string(string("bdbRC::connect2-E- ")+ex.what()); }
			}
			else throw string(string("bdbRC::connect2-E- ")+ex.what());
		}
		catch(std::exception &ex) { throw string(string("bdbRC::connect2-E- ")+ex.what()); }
		return init;
	};
	bool connect2(dbTables type, bool create_ena=false, u_int32_t oflags=0) throw(string &) {
		try {
			if (_version < 2.0)
				// many tables in the db file
				return connect2(db_file(),type,create_ena,oflags);
			else
				// single table in the db file
				return connect2(bdbRC::id2str(type),type,create_ena,oflags);
		}
		catch(string &ex) { throw ex; }
	};
	void disconnect() throw(string &) {
		if (_db) {
			dbstl::close_db(_db);
			_db = 0;
		}
	};
};

//_____________________________________________________________________

template <typename T> class dataset: virtual public dbstl::db_multimap<dbTables, T>
{
public:
	dataset<T>(bdbRC &db, dbTables type): dbstl::db_multimap<dbTables, T>(db.db(),db.env()) {
		_bdbrc = &db;
		_type = type;
		query();
	};
	dataset<T>(bdbRC &db): dbstl::db_multimap<dbTables, T>(db.db(),db.env()) {
		_bdbrc = &db;
		_type = db.type();
		query();
	};
	virtual ~dataset<T>() { };
	typedef typename dbstl::db_multimap<dbTables, T>::iterator iter;
private:
	friend class DBServer_impl;
	friend struct Clock;
	friend struct prio;
	friend struct prio1;
	friend struct find_AhByName;
	bdbRC *_bdbrc;	// appropriate bdb connection
	dbTables _type;	// don't take _type from _bdbrc
	iter _iter;
	pair<iter, iter> _range;

public:
	void erase() {
		dbstl::db_multimap<dbTables, T>::erase(_type);
	};
	iter &it() {
		return _iter;
	};
	void erase_it() {
		dbstl::db_multimap<dbTables, T>::erase(_iter);
	};
	int query() {
		bdbTraits::resetstat();
		if (_bdbrc->version() < 2.0) {
			// many tables in the db file
			_range = dbstl::db_multimap<dbTables, T>::equal_range(_type);
		} else {
			// single table in the db file
			_range.first = begin();
			_range.second = end();
		}
		rewind();
		int len = length();
		if (len > bdbTraits::good())
			cout<<"dataset<"<<typeid(T).name()<<">-E- bad dataset read, len="<<len<<
				" good="<<bdbTraits::good()<<" bad="<<bdbTraits::bad()<<endl;
		else if (_dbgLevel > 1)
			cout<<"dataset<"<<typeid(T).name()<<">-I- len="<<len<<
				", good="<<bdbTraits::good()<<", bad="<<bdbTraits::bad()<<endl;
		return len;
	};
	int length() {
		return distance(_range.first,_range.second);
	};
	void rewind() {
		_iter = _range.first;
	};
	void operator ++(int) {
		++_iter;
	};
	T& operator *() throw(string &) {
		if (_iter == _range.second) {
			stringstream ex;
			ex << "dataset::operator*-E- eof table reached for dataset type " << _type;
			cout << ex.str() << endl;
			throw ex.str();
		}
		return _iter->second;
	};
	static T& ref(iter &it) {
		return it->second;
	};
	void insert(T &element) {
		dbstl::db_multimap<dbTables, T>::insert(make_pair(_type,element));
	};
};

typedef dataset<DPS::Client::NominalHost> nh_dt;
typedef dataset<DPS::Client::NominalClient> nc_dt;
typedef dataset<CORBA::String_var> env_dt;
typedef dataset<DPS::Server::DB> db_dt;
typedef dataset<DPS::Client::ActiveHost> ah_dt;
typedef dataset<DPS::Client::ActiveClient> ac_dt;
typedef dataset<DPS::Producer::RunEvInfo> rtb_dt;
typedef dataset<DPS::Producer::DSTInfo> dsti_dt;
typedef dataset<DPS::Producer::DST> dsts_dt;

struct find_ReiByUid {
	const DPS::Producer::RunEvInfo &_a;
	explicit find_ReiByUid(const DPS::Producer::RunEvInfo &a):_a(a) {};
	bool operator()(const DPS::Producer::RunEvInfo &ne) { return ne.uid == _a.uid; };
};

#endif	// eof _BDBRC_H
