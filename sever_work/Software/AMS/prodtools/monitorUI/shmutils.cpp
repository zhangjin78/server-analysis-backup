#include "ManCommon.hxx"
#include "ManMapper.hxx"
#include "../dbServer/dbserver.hxx"
#include "../dbServer/bdbRC.hxx"
#include "../dbServer/bdbTraits.hxx"
#include "shmutils.h"
#include "uiwrapper.h"

const char *shmem::key() const { return _key.c_str(); }

bool shmem::create(int size, const char *key)
{
	stringstream s;
	s << key << "." << boost::format("%d") %static_cast<unsigned>(getpid());
	_key = s.str();
	setNativeKey(QString(_key.c_str()));
	return QSharedMemory::create(size);
}

bool shmem::attach(const char *key)
{
	_key = key;
	setNativeKey(QString(key));
	return QSharedMemory::attach();
}

bool shmem::attach(const shmem& pat) { return attach(pat.key()); }

//_____________________________________________________________________
// Shared memory for filtering dialog

void shFilter::range(vector<unsigned long> &dst, const string &src)
{
	dst.clear();
	if (src.find('-') == string::npos)
	{
		CharStringToken st(src,",");
		for (int i=0; i<st.tokens(); i++)
			try {
				unsigned long run = boost::lexical_cast<unsigned long>(st.token(i));
				dst.push_back(run);
			}
			catch(boost::bad_lexical_cast &ex) {
				break;
			}
	}
	else {
		CharStringToken st(src,"-");
		for (int i=0; i<st.tokens(); i++) {
			unsigned long run;
			try {
				const char *s = st.token(i);
				if (st.token(i) && strlen(s))
					run = boost::lexical_cast<unsigned long>(s);
				else
					if (i == 0) run = 0;
					else		run = ULONG_MAX;
				dst.push_back(run);
			}
			catch(boost::bad_lexical_cast &ex) {
				break;
			}
		}
		if (dst.size() < 2) dst.push_back(ULONG_MAX);
	}
}
bool shFilter::inrstatus(const DPS::Producer::RunStatus &status) const
{
	if (!inuse() || _cash._rStatus==0)
		return true;
	if (status != _cash._rStatus-1)
		return false;
	return true;
}
bool shFilter::inrange(const vector<unsigned long> &dst, const string &src, unsigned long pat) const
{
	if (!inuse() || src.size()<1)
		return true;
	if (src.find('-') == string::npos) {
		if (dst.size()<1)
			return true;
		for (unsigned i=0; i<dst.size(); i++)
			if (pat == dst[i])
				return true;
	}
	else {
		if (dst.size()<2)
			return true;
		if (pat >= dst[0] && pat <= dst[1])
			return true;
	}
	return false;
}
bool shFilter::indataset(const string &dst) const
{
	if (!inuse() || _cash._rDataSet.size()<1)
		return true;
	if (dst.find(_cash._rDataSet) == string::npos)
		return false;
	return true;
}
bool shFilter::inacstatus(const DPS::Producer::ClientStatus &status) const
{
	if (!inuse() || _cash._acStatus==0)
		return true;
	if (status != _cash._acStatus-1)
		return false;
	return true;
}
void shFilter::operator <<(const cash& var)
{
	lock();
	void *p = data();
	if (p) {
		memstr ms(p);
		ms << static_cast<const string &>(var._servers);
		ms << static_cast<const int &>(var._csServer);
		ms << static_cast<const string &>(var._csHostName);
		ms << static_cast<const int &>(var._rStatus);
		ms << static_cast<const string &>(var._rRun);
		range(_runs,var._rRun);
		ms << static_cast<const string &>(var._rUid);
		range(_uids,var._rUid);
		ms << static_cast<const string &>(var._rDataSet);
		ms << static_cast<const string &>(var._rPrior);
		range(_priors,var._rPrior);
		ms << static_cast<const string &>(var._rDataMC);
		range(_datamc,var._rDataMC);
		ms << static_cast<const int &>(var._acStatus);
		ms << static_cast<const string &>(var._acThreads);
		range(_threads,var._acThreads);
		ms << static_cast<const string &>(var._acProcessID);
		range(_processids,var._acProcessID);
	}
	unlock();
}
void shFilter::operator >>(cash& var)
{
	lock();
	void *p = data();
	if (p) {
		memstr ms(p);
		ms >> static_cast<string &>(var._servers);
		ms >> static_cast<int &>(var._csServer);
		ms >> static_cast<string &>(var._csHostName);
		ms >> static_cast<int &>(var._rStatus);
		ms >> static_cast<string &>(var._rRun);
		range(_runs,var._rRun);
		ms >> static_cast<string &>(var._rUid);
		range(_uids,var._rUid);
		ms >> static_cast<string &>(var._rDataSet);
		ms >> static_cast<string &>(var._rPrior);
		range(_priors,var._rPrior);
		ms >> static_cast<string &>(var._rDataMC);
		range(_datamc,var._rDataMC);
		ms >> static_cast<int &>(var._acStatus);
		ms >> static_cast<string &>(var._acThreads);
		range(_threads,var._acThreads);
		ms >> static_cast<string &>(var._acProcessID);
		range(_processids,var._acProcessID);
	}
	unlock();
}
bool shFilter::operator !=(const shmem &pat) const
{
	const shFilter &p = dynamic_cast<const shFilter &>(pat);
	const struct cash &pc = p._cash;
	if (strcmp(key(),p.key()) ||
		_cash._servers != pc._servers ||
		_cash._csServer != pc._csServer ||
		_cash._csHostName != pc._csHostName ||
		_cash._rStatus != pc._rStatus ||
		_cash._rRun != pc._rRun ||
		_cash._rUid != pc._rUid ||
		_cash._rDataSet != pc._rDataSet ||
		_cash._rPrior != pc._rPrior ||
		_cash._rDataMC != pc._rDataMC ||
		_cash._acStatus != pc._acStatus ||
		_cash._acThreads != pc._acThreads ||
		_cash._acProcessID != pc._acProcessID)
			return true;
	return false;
}
bool shFilter::inuse() const
{
	return theUI.actUseFilter->isChecked();
}
bool shFilter::inserver(const string &id) const
{
	if (!inuse() || _cash._csServer==0)
		return true;
	CharStringToken st(_cash._servers,",");
	if (_cash._csServer-1 < st.tokens())
		if (id != st.token(_cash._csServer-1))
			return false;
	return true;
}
bool shFilter::inhost(const string &host) const
{
	if (!inuse() || _cash._csHostName.size()<1)
		return true;
	if (host.find(_cash._csHostName) == string::npos)
		return false;
	return true;
}
bool shFilter::runBadFilt(const DPS::Producer::RunEvInfo &run) const
{
	bool result = !inrstatus(run.Status) |
				  !inrange(_runs,_cash._rRun,run.Run) |
				  !inrange(_uids,_cash._rUid,run.uid) |
				  !indataset(string(run.FilePath)) |
				  !inrange(_priors,_cash._rPrior,run.Priority) |
				  !inrange(_datamc,_cash._rDataMC,run.DataMC);
	return result;
}
bool shFilter::runUseFilt() const
{
	if (inuse()) {
		bool result = (_cash._rStatus!=0) |
					  (_cash._rRun.size()>=1) |
					  (_cash._rUid.size()>=1) |
					  (_cash._rDataSet.size()>=1) |
					  (_cash._rPrior.size()>=1) |
					  (_cash._rDataMC.size()>=1);
		return result;
	}
	return false;
}
bool shFilter::acBadFilt(const DPS::Client::ActiveClient &ac) const
{
	bool result = !inacstatus(ac.Status) |
				  !inrange(_threads,_cash._acThreads,ac.id.threads) |
				  !inrange(_processids,_cash._acProcessID,ac.id.pid);
	return result;
}
bool shFilter::acUseFilt() const
{
	if (inuse()) {
		bool result = (_cash._acStatus!=0) |
					  (_cash._acThreads.size()>=1) |
					  (_cash._acProcessID.size()>=1);
		return result;
	}
	return false;
}

//_____________________________________________________________________
// Shared memory to edit control tables

void shCedit::operator <<(const cash& var)
{
	lock();
	void *p = data();
	if (p) {
		memstr ms(p);
		ms << static_cast<const string &>(var._data);
		ms << static_cast<const unsigned &>(var._upd);
	}
	unlock();
}

void shCedit::operator >>(cash& var)
{
	lock();
	void *p = data();
	if (p) {
		memstr ms(p);
		ms >> static_cast<string &>(var._data);
		ms >> static_cast<unsigned &>(var._upd);
	}
	unlock();
}

bool shCedit::operator !=(const shmem &pat) const
{
	const shCedit &p = dynamic_cast<const shCedit &>(pat);
	const struct cash &pc = p._cash;
	if (strcmp(key(),p.key()) || _cash._upd != pc._upd)
		return true;
	return false;
}

//_____________________________________________________________________
// Shared memory for select log file dialog

void shSfile::operator <<(const cash& var)
{
	lock();
	void *p = data();
	if (p) {
		memstr ms(p);
		unsigned szdata = var._data.size();
		ms << szdata;
		for (unsigned i=0; i<szdata; i++)
			ms << var._data[i];
		ms << var._selected;
	}
	unlock();
}

void shSfile::operator >>(cash& var)
{
	lock();
	void *p = data();
	if (p) {
		memstr ms(p);
		unsigned szdata;
		ms >> szdata;
		for (unsigned i=0; i<szdata; i++) {
			string s;
			ms >> s;
			var._data.push_back(s);
		}
		ms >> var._selected;
	}
	unlock();
}

bool shSfile::operator !=(const shmem &pat) const
{
	const shSfile &p = dynamic_cast<const shSfile &>(pat);
	const struct cash &pc = p._cash;
	if (strcmp(key(),p.key()))
		return true;
	return false;
}
