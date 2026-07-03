#ifndef  _ORARC_H
#define  _ORARC_H

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
using std::cout;
using std::endl;
#include <string>
using std::string;
#include <occi.h>
using namespace oracle::occi;

#include "smart_ptr.hxx"

class oraRC
{
public:
	explicit oraRC(const oraRC *p=0) throw (SQLException &)
	{
		try {
			if (p == 0) {
				Environment *env = Environment::createEnvironment(Environment::DEFAULT);
				_occiEnv = new smart_ptr<Environment>(env);
			}
			else
				_occiEnv = new smart_ptr<Environment>(*p->_occiEnv);
		}
		catch (SQLException &ex) {
			throw ex;
		}
		_occiConn = 0;
	};
	oraRC(const oraRC &p) throw (SQLException &)
	{
		try {
			_occiEnv = new smart_ptr<Environment>(*p._occiEnv);
		}
		catch (SQLException &ex) {
			throw ex;
		}
		_occiConn = 0;
	};
	virtual ~oraRC()
	{
		try {
			disconnect();
			if (_occiEnv->last()) {
				Environment::terminateEnvironment(&(*(*_occiEnv)));
				_occiEnv->release();
			}
		}
		catch (SQLException &ex) {
		}
	};

private:
	smart_ptr<Environment> *_occiEnv;
	Connection  *_occiConn;

public:
	Connection *connect(const string user, const string passwd, const string db) throw (SQLException &)
	{
		if (_occiConn == 0)
			try {
				_occiConn = (*_occiEnv)->createConnection(user,passwd,db);
			}
			catch (SQLException &ex) {
				throw ex;
			}
		return _occiConn;
	};
	void disconnect() throw (SQLException &)
	{
		if (_occiConn == 0) return;

		try {
			(*_occiEnv)->terminateConnection(_occiConn);
			_occiConn = 0;
		}
		catch (SQLException &ex) {
			throw ex;
		}
		_occiConn = 0;
	};
	Connection  *connection() const
	{
		return _occiConn;
	};
};

#endif	// eof _ORARC_H
