#ifndef  _SHMUTILS_H
#define  _SHMUTILS_H

#include <stdio.h>
#include <stdlib.h>

#include <QtCore/QVariant>
#include <QtCore/QStringList>
#include <QtCore/QString>
#include <QtCore/QEvent>
#include <QtCore/QSharedMemory>

#include "server-cpp.h"
#include "server-cpp-stubs.h"
#include "server-cpp-skels.h"
#include "server-cpp-common.h"

class shmem: public QSharedMemory {
public:
	struct cash {
	};
	shmem(): QSharedMemory() {
	};
	virtual ~shmem() {
		unlock();
	};
private:
	string _key;
public:
	const char *key() const;
	bool create(int size, const char *key);
	bool attach(const char *key);
	bool attach(const shmem& pat);
	virtual void operator <<(const cash& var) {
	};
	virtual void operator >>(cash& var) {
	};
	virtual bool operator !=(const shmem& pat) const {
		return false;
	};
};

//_____________________________________________________________________
// Shared memory for filtering dialog

class shFilter: public shmem {
public:
	struct cash: public shmem::cash {
		string _servers;
		int _csServer;
		string _csHostName;
		int _rStatus;
		string _rRun;
		string _rUid;
		string _rDataSet;
		string _rPrior;
		string _rDataMC;
		int _acStatus;
		string _acThreads;
		string _acProcessID;
	} _cash;
	shFilter(): shmem() {
	};
	virtual ~shFilter() {
	};
private:
	vector<unsigned long> _runs;
	vector<unsigned long> _uids;
	vector<unsigned long> _priors;
	vector<unsigned long> _datamc;
	vector<unsigned long> _threads;
	vector<unsigned long> _processids;
	void range(vector<unsigned long> &dst, const string &src);
	bool inrstatus(const DPS::Producer::RunStatus &status) const;
	bool inrange(const vector<unsigned long> &dst, const string &src,
			unsigned long pat) const;
	bool indataset(const string &dst) const;
	bool inacstatus(const DPS::Producer::ClientStatus &status) const;
public:
	void operator <<(const cash& var);
	void operator >>(cash& var);
	bool operator !=(const shmem &pat) const;
public:
	bool inuse() const;
	bool inserver(const string &id) const;
	bool inhost(const string &host) const;
	bool runBadFilt(const DPS::Producer::RunEvInfo &run) const;
	bool runUseFilt() const;
	bool acBadFilt(const DPS::Client::ActiveClient &ac) const;
	bool acUseFilt() const;
};

//_____________________________________________________________________
// Shared memory to edit control tables

class shCedit: public shmem {
public:
	struct cash: public shmem::cash {
		string _data;
		unsigned _upd;
	} _cash;
	shCedit(): shmem() {
	};
	virtual ~shCedit() {
	};
public:
	void operator <<(const cash& var);
	void operator >>(cash& var);
	bool operator !=(const shmem &pat) const;
};

//_____________________________________________________________________
// Shared memory for select log file dialog

class shSfile: public shmem {
public:
	struct cash: public shmem::cash {
		vector<string> _data;
		int _selected;
	} _cash;
	shSfile(): shmem() {
	};
	virtual ~shSfile() {
	};
public:
	void operator <<(const cash& var);
	void operator >>(cash& var);
	bool operator !=(const shmem &pat) const;
};

#endif	// eof _SHMUTILS_H
