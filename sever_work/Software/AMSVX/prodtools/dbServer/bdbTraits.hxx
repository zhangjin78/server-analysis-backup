// A very simple wrapper to Berkeley DB C++/STL

#ifndef _BDB_TRAITS_H
#define _BDB_TRAITS_H

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <utility>
using namespace std;
using std::cout;
using std::endl;
using std::pair;
#include <string>
using std::string;

#include "server-cpp.h"
#include "server-cpp-stubs.h"
#include "server-cpp-skels.h"
#include "server-cpp-common.h"

#include "db.h"
#include "db_cxx.h"
#include "dbstl_common.h"
#include "dbstl_map.h"
#include "dbstl_vector.h"

extern int _dbgLevel;

//_____________________________________________________________________
// Simple memory stream, no (re)allocation

class memstr
{
public:
	memstr(const void *p=0) {
		_po = static_cast<char *>(const_cast<void *>(p));
	};
private:
	char *_po;

public:
	template <typename T> u_int32_t sz(const T& var) {
		return sizeof(T);
	};
	template <typename T> void operator <<(const T& var) {
		int l = sz(var);
		memcpy(_po,&var,l);
		_po += l;
	};
	template <typename T> void operator >>(T& var) {
		int l = sz(var);
		memcpy(&var,_po,l);
		_po += l;
	};

//_____________________________________________________________________
	inline u_int32_t sz(const string& var)
	{
		return var.size()+1;
	};
	inline void operator <<(const string& var)
	{
			int l = sz(var);
			memcpy(_po,var.c_str(),l);
			_po += l;
	};
	// added for monitorUI::FilterSettngs dialog
	inline void operator >>(string& var)
	{
			int l = strlen(_po);
			if (l) {
				string s;
				s += _po;
				var = s.c_str();
			}
			else
				var = (const char *)"\0";
			_po += l+1;
	};

//_____________________________________________________________________
	inline u_int32_t sz(const CORBA::String_var& var)
	{
		if (var) {
			string s(var);
			return sz(s);
		}
		return 1;
	};
	inline void operator <<(const CORBA::String_var& var)
	{
		if (var) {
			string s(var);
			(*this) << s;
		}
		else
			*_po++ = '\0';
	};
	inline void operator >>(CORBA::String_var& var)
	{
		int l = strlen(_po);
		if (l) {
			string s(_po);
			var = s.c_str();
		}
		else
			var = (const char *)"\0";
		_po += l+1;
	};

//_____________________________________________________________________
	inline u_int32_t sz(const DPS::Client::NominalClient &elem)
	{
		u_int32_t result = sizeof(elem);
		result += sz(elem.WholeScriptPath);
		result += sz(elem.LogPath);
		result += sz(elem.SubmitCommand);
		result += sz(elem.HostName);
		return result;
	};
	inline void operator <<(const DPS::Client::NominalClient& var)
	{
		(*this) << var.uid;
		(*this) << var.Type;
		(*this) << var.MaxClients;
		(*this) << var.CPUNeeded;
		(*this) << var.MemoryNeeded;
		(*this) << var.WholeScriptPath;
		(*this) << var.LogPath;
		(*this) << var.SubmitCommand;
		(*this) << var.HostName;
		(*this) << var.LogInTheEnd;
	};
	inline void operator >>(DPS::Client::NominalClient& var)
	{
		(*this) >> var.uid;
		(*this) >> var.Type;
		(*this) >> var.MaxClients;
		(*this) >> var.CPUNeeded;
		(*this) >> var.MemoryNeeded;
		(*this) >> var.WholeScriptPath;
		(*this) >> var.LogPath;
		(*this) >> var.SubmitCommand;
		(*this) >> var.HostName;
		(*this) >> var.LogInTheEnd;
	};

//_____________________________________________________________________
	inline u_int32_t sz(const DPS::Client::CID &elem)
	{
		u_int32_t result = sizeof(elem);
		result += sz(elem.HostName);
		result += sz(elem.Interface);
		return result;
	};
	inline void operator <<(const DPS::Client::CID& var)
	{
		(*this) << var.HostName;
		(*this) << var.uid;
		(*this) << var.pid;
		(*this) << var.ppid;
		(*this) << var.threads;
		(*this) << var.threads_change;
		(*this) << var.Type;
		(*this) << var.StatusType;
		(*this) << var.Status;
		(*this) << var.Interface;
		(*this) << var.Mips;
		(*this) << var.coid;
	};
	inline void operator >>(DPS::Client::CID& var)
	{
		(*this) >> var.HostName;
		(*this) >> var.uid;
		(*this) >> var.pid;
		(*this) >> var.ppid;
		(*this) >> var.threads;
		(*this) >> var.threads_change;
		(*this) >> var.Type;
		(*this) >> var.StatusType;
		(*this) >> var.Status;
		(*this) >> var.Interface;
		(*this) >> var.Mips;
		(*this) >> var.coid;
	};

//_____________________________________________________________________
	u_int32_t sz(const DPS::Client::ActiveRef &elem)
	{
		u_int32_t result = sizeof(elem);
		result += sz(elem.IOR);
		result += sz(elem.Interface);
		return result;
	};
	inline void operator <<(const DPS::Client::ActiveRef& var)
	{
		(*this) << var.IOR;
		(*this) << var.Interface;
		(*this) << var.Type;
		(*this) << var.uid;
	};
	inline void operator >>(DPS::Client::ActiveRef& var)
	{
		(*this) >> var.IOR;
		(*this) >> var.Interface;
		(*this) >> var.Type;
		(*this) >> var.uid;
	};

//_____________________________________________________________________
	inline u_int32_t sz(const DPS::Client::ARS &elem)
	{
		u_int32_t result = sizeof(elem);
		unsigned nars = elem.length();
		result  += nars;
		for (unsigned i=0; i<nars; i++)
			result += sz(elem[i]);
		return result;
	};
	inline void operator <<(const DPS::Client::ARS& var)
	{
		unsigned nars = var.length();
		(*this) << nars;
		for (unsigned i=0; i<nars; i++)
			(*this) << var[i];
	};
	inline void operator >>(DPS::Client::ARS& var)
	{
		unsigned nars;
		(*this) >> nars;
		var.length(nars);
		for (unsigned i=0; i<nars; i++) {
			DPS::Client::ActiveRef ar;
			(*this) >> ar;
			var[i] = ar;
		}
	};

//_____________________________________________________________________
	inline u_int32_t sz(const DPS::Client::ActiveClient &elem)
	{
		u_int32_t result = sizeof(elem);
		result += sz(elem.id);
		result += sz(elem.ars);
		return result;
	};
	inline void operator <<(const DPS::Client::ActiveClient& var)
	{
		(*this) << var.id;
		(*this) << var.ars;
		(*this) << var.LastUpdate;
		(*this) << var.Start;
		(*this) << var.TimeOut;
		(*this) << var.Status;
		(*this) << var.StatusType;
	};
	inline void operator >>(DPS::Client::ActiveClient& var)
	{
		(*this) >> var.id;
		(*this) >> var.ars;
		(*this) >> var.LastUpdate;
		(*this) >> var.Start;
		(*this) >> var.TimeOut;
		(*this) >> var.Status;
		(*this) >> var.StatusType;
	};

//_____________________________________________________________________
	inline u_int32_t sz(const DPS::Client::NominalHost &elem)
	{
		u_int32_t result = sizeof(elem);
		result += sz(elem.HostName);
		result += sz(elem.Interface);
		result += sz(elem.OS);
		return result;
	};
	inline void operator <<(const DPS::Client::NominalHost& var)
	{
		(*this) << var.CPUNumber;
		(*this) << var.Memory;
		(*this) << var.Clock;
		(*this) << var.HostName;
		(*this) << var.Interface;
		(*this) << var.OS;
	};
	inline void operator >>(DPS::Client::NominalHost& var)
	{
		(*this) >> var.CPUNumber;
		(*this) >> var.Memory;
		(*this) >> var.Clock;
		(*this) >> var.HostName;
		(*this) >> var.Interface;
		(*this) >> var.OS;
	};

//_____________________________________________________________________
	inline u_int32_t sz(const DPS::Client::ActiveHost &elem)
	{
		u_int32_t result = sizeof(elem);
		result += sz(elem.HostName);
		result += sz(elem.Interface);
		return result;
	};
	inline void operator <<(const DPS::Client::ActiveHost& var)
	{
		(*this) << var.HostName;
		(*this) << var.Interface;
		(*this) << var.Status;
		(*this) << var.ClientsRunning;
		(*this) << var.ClientsAllowed;
		(*this) << var.ClientsProcessed;
		(*this) << var.ClientsFailed;
		(*this) << var.ClientsKilled;
		(*this) << var.LastFailed;
		(*this) << var.LastUpdate;
		(*this) << var.Clock;
	};
	inline void operator >>(DPS::Client::ActiveHost& var)
	{
		(*this) >> var.HostName;
		(*this) >> var.Interface;
		(*this) >> var.Status;
		(*this) >> var.ClientsRunning;
		(*this) >> var.ClientsAllowed;
		(*this) >> var.ClientsProcessed;
		(*this) >> var.ClientsFailed;
		(*this) >> var.ClientsKilled;
		(*this) >> var.LastFailed;
		(*this) >> var.LastUpdate;
		(*this) >> var.Clock;
	};

//_____________________________________________________________________
	inline u_int32_t sz(const DPS::Producer::CurrentInfo &elem)
	{
		u_int32_t result = sizeof(elem);
		result += sz(elem.HostName);
		return result;
	};
	inline void operator <<(const DPS::Producer::CurrentInfo& var)
	{
		(*this) << var.Run;
		(*this) << var.EventsProcessed;
		(*this) << var.LastEventProcessed;
		(*this) << var.CriticalErrorsFound;
		(*this) << var.ErrorsFound;
		(*this) << var.CPUTimeSpent;
		(*this) << var.TimeSpent;
		(*this) << var.Mips;
		(*this) << var.CPUMipsTimeSpent;
		(*this) << var.Status;
		(*this) << var.HostName;
	};
	inline void operator >>(DPS::Producer::CurrentInfo& var)
	{
		(*this) >> var.Run;
		(*this) >> var.EventsProcessed;
		(*this) >> var.LastEventProcessed;
		(*this) >> var.CriticalErrorsFound;
		(*this) >> var.ErrorsFound;
		(*this) >> var.CPUTimeSpent;
		(*this) >> var.TimeSpent;
		(*this) >> var.Mips;
		(*this) >> var.CPUMipsTimeSpent;
		(*this) >> var.Status;
		(*this) >> var.HostName;
	};

//_____________________________________________________________________
	inline u_int32_t sz(const DPS::Producer::RunEvInfo &elem)
	{
		u_int32_t result = sizeof(elem);
		result += sz(elem.FilePath);
		result += sz(elem.cinfo);
		return result;
	};
	inline void operator <<(const DPS::Producer::RunEvInfo& var)
	{
		(*this) << var.uid;
		(*this) << var.Run;
		(*this) << var.FirstEvent;
		(*this) << var.LastEvent;
		(*this) << var.TFEvent;
		(*this) << var.TLEvent;
		(*this) << var.Priority;
		(*this) << var.FilePath;
		(*this) << var.Status;
		(*this) << var.History;
		(*this) << var.CounterFail;
		(*this) << var.SubmitTime;
		(*this) << var.cuid;
		(*this) << var.rndm1;
		(*this) << var.rndm2;
		(*this) << var.DataMC;
		(*this) << var.cinfo;
	};
	inline void operator >>(DPS::Producer::RunEvInfo& var)
	{
		(*this) >> var.uid;
		(*this) >> var.Run;
		(*this) >> var.FirstEvent;
		(*this) >> var.LastEvent;
		(*this) >> var.TFEvent;
		(*this) >> var.TLEvent;
		(*this) >> var.Priority;
		(*this) >> var.FilePath;
		(*this) >> var.Status;
		(*this) >> var.History;
		(*this) >> var.CounterFail;
		(*this) >> var.SubmitTime;
		(*this) >> var.cuid;
		(*this) >> var.rndm1;
		(*this) >> var.rndm2;
		(*this) >> var.DataMC;
		(*this) >> var.cinfo;
	};

//_____________________________________________________________________
	inline u_int32_t sz(const DPS::Producer::DSTInfo &elem)
	{
		u_int32_t result = sizeof(elem);
		result += sz(elem.HostName);
		result += sz(elem.OutputDirPath);
		return result;
	};
	inline void operator <<(const DPS::Producer::DSTInfo& var)
	{
		(*this) << var.type;
		(*this) << var.uid;
		(*this) << var.HostName;
		(*this) << var.OutputDirPath;
		(*this) << var.Mode;
		(*this) << var.UpdateFreq;
		(*this) << var.DieHard;
		(*this) << var.FreeSpace;
		(*this) << var.TotalSpace;
	};
	inline void operator >>(DPS::Producer::DSTInfo& var)
	{
		(*this) >> var.type;
		(*this) >> var.uid;
		(*this) >> var.HostName;
		(*this) >> var.OutputDirPath;
		(*this) >> var.Mode;
		(*this) >> var.UpdateFreq;
		(*this) >> var.DieHard;
		(*this) >> var.FreeSpace;
		(*this) >> var.TotalSpace;
	};

//_____________________________________________________________________
	inline u_int32_t sz(const DPS::Producer::DST &elem)
	{
		u_int32_t result = sizeof(elem);
		result += sz(elem.Name);
		result += sz(elem.Version);
		return result;
	};
	inline void operator <<(const DPS::Producer::DST& var)
	{
		(*this) << var.Insert;
		(*this) << var.Begin;
		(*this) << var.End;
		(*this) << var.Run;
		(*this) << var.FirstEvent;
		(*this) << var.LastEvent;
		(*this) << var.EventNumber;
		(*this) << var.ErrorNumber;
		(*this) << var.Status;
		(*this) << var.Type;
		(*this) << var.size;
		(*this) << var.Name;
		(*this) << var.Version;
		(*this) << var.crc;
		(*this) << var.rndm1;
		(*this) << var.rndm2;
		(*this) << var.FreeSpace;
		(*this) << var.TotalSpace;
	};
	inline void operator >>(DPS::Producer::DST& var)
	{
		(*this) >> var.Insert;
		(*this) >> var.Begin;
		(*this) >> var.End;
		(*this) >> var.Run;
		(*this) >> var.FirstEvent;
		(*this) >> var.LastEvent;
		(*this) >> var.EventNumber;
		(*this) >> var.ErrorNumber;
		(*this) >> var.Status;
		(*this) >> var.Type;
		(*this) >> var.size;
		(*this) >> var.Name;
		(*this) >> var.Version;
		(*this) >> var.crc;
		(*this) >> var.rndm1;
		(*this) >> var.rndm2;
		(*this) >> var.FreeSpace;
		(*this) >> var.TotalSpace;
	};

//_____________________________________________________________________
	inline u_int32_t sz(const DPS::Server::DB &elem)
	{
		u_int32_t result = sizeof(elem);
		result += sz(elem.fs);
		return result;
	};
	inline void operator <<(const DPS::Server::DB& var)
	{
		(*this) << var.fs;
		(*this) << var.dbfree;
		(*this) << var.dbtotal;
		(*this) << var.bs;
	};
	inline void operator >>(DPS::Server::DB& var)
	{
		(*this) >> var.fs;
		(*this) >> var.dbfree;
		(*this) >> var.dbtotal;
		(*this) >> var.bs;
	};

public:
	void *ptr() const
	{
		return static_cast<void *>(_po);
	};
	void operator +=(u_int32_t offs)
	{
		_po += offs;
	};
	template<typename T> static string type(const T &elem)
	{
		return typeid(elem).name();
	};
	template <typename T> bool hasType(T& var)
	{
		string t = type(var);
		if (memcmp(_po,t.c_str(),t.length()) == 0) {
			_po +=  sz(t);
			return true;
		}
		else
			return false;
	};
	string debug(int maxlen=32)
	{
		string s;
		for (int i=0; i<maxlen; i++) {
			char sym = _po[i];
			if (sym < 0x20 || sym > 0x7f) break;
			s += sym;
		}
		return s;
	}
};

//_____________________________________________________________________
// DBSTL traits specification

// bdbTraits statistics for bdbTraits::restore<>
int _stGood; // good results
int _stBad;  // bad results

struct bdbTraits
{
	static void instantiate_all() {
		instantiate<DPS::Client::CID>();
		instantiate<DPS::Client::NominalClient>();
		instantiate<DPS::Client::ActiveRef>();
		instantiate<DPS::Client::ARS>();
		instantiate<DPS::Client::ActiveClient>();
		instantiate<DPS::Client::NominalHost>();
		instantiate<DPS::Client::ActiveHost>();
		instantiate<DPS::Producer::CurrentInfo>();
		instantiate<DPS::Producer::RunEvInfo>();
		instantiate<DPS::Producer::DSTInfo>();
		instantiate<DPS::Producer::DST>();
		instantiate<DPS::Server::DB>();
		instantiate<CORBA::String_var>();
	};
	template<typename T> static u_int32_t size(const T &elem) {
		memstr ms;
		u_int32_t result = ms.sz(memstr::type(elem));
		result += ms.sz(elem);
		return result;
	};
	template<typename T> static void copy(void *dest, const T &elem) {
		memstr ms(dest);
		ms << memstr::type(elem);
		ms << elem;
	};
	template<typename T> static void restore(T &dest, const void *srcdata) {
		memstr ms(srcdata);
		if (!ms.hasType(dest)) {
			_stBad++;
			if (_dbgLevel > 2)
				cout<<"bdbTraits::restore<"<<memstr::type(&dest)<<">-W- bad class "<<ms.debug()<<endl;
			return;
		}
		_stGood++;
		if (_dbgLevel > 3)
			cout<<"bdbTraits::restore<"<<memstr::type(&dest)<<">-I- ok"<<endl;
		ms >> dest;
	};
	template<typename T> static void instantiate() {
		dbstl::DbstlElemTraits<T>::instance()->set_size_function(size<T>);
		dbstl::DbstlElemTraits<T>::instance()->set_copy_function(copy<T>);
		dbstl::DbstlElemTraits<T>::instance()->set_restore_function(restore<T>);
	};
	template<typename T> static u_int32_t seqlen(const T &elem) { return elem.length(); };
	template<typename T> static void instantiate2() {
		dbstl::DbstlElemTraits<T>::instance()->set_size_function(size<T>);
		dbstl::DbstlElemTraits<T>::instance()->set_copy_function(copy<T>);
		dbstl::DbstlElemTraits<T>::instance()->set_restore_function(restore<T>);
		dbstl::DbstlElemTraits<T>::instance()->set_sequence_len_function(seqlen<T>);
	};
	static void resetstat() { _stGood = _stBad = 0; };
	static int good() { return _stGood; };
	static int bad() { return _stBad; };
};

/* trait pattern, change T to your corba object
//_____________________________________________________________________
template<> inline u_int32_t bdbTraits::size<T>(const T &elem)
{
	memstr ms;
	u_int32_t result = ms.sz(elem);
	return result;
};
template<> inline void bdbTraits::copy<T>(void *dest, const T &elem)
{
	memstr ms(dest);
};
template<> inline void bdbTraits::restore<T>(T &dest, const void *srcdata)
{
	memstr ms(srcdata);
}; */

#endif	// eof _BDB_TRAITS_H
