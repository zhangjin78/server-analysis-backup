#ifndef __MSTD_H__
#define __MSTD_H__

// stream redirection, low level
namespace mstd
{
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <streambuf>
#include <sstream>
#include <sys/time.h>
using std::string;
using std::ostream;
using std::streambuf;

class mstreambuf: public streambuf
{
public:
	explicit mstreambuf(ostream &os): streambuf() {
		_os = &os;
		_savbuf = _os->rdbuf(this);
		_eol = true;
	};
	virtual ~mstreambuf() {
		_os->rdbuf(_savbuf);
	};
private:
	ostream   *_os;
	streambuf *_savbuf;
	bool _eol;
	string _s;
private:
	mstreambuf();
	mstreambuf(mstreambuf const&);
	void operator=(mstreambuf const&);
	// prompt for text line
	void prompt() {
		char res[32];
		struct timeval tv;
		gettimeofday(&tv,0);
		struct tm *t = localtime(&tv.tv_sec);
		sprintf(res,"<%02d.%02d %02d:%02d:%02d.%03d> ",
				t->tm_mon+1,t->tm_mday,t->tm_hour,t->tm_min,t->tm_sec,tv.tv_usec/1000);
		_s = res;
	};
protected:
	virtual int_type overflow(int_type c) {
		if (c >= 0x20) {
			if (_eol) prompt();
			_s += c;
			_eol = false;
		}
		else if (!_eol) {
			puts(_s.c_str());
			fflush(stdout);
			_eol = true;
		}
		return c;
	};
public:
	static  char* ctime(const time_t * t) {
		string res = ::ctime(t);
		size_t last = res.rfind('\n');
		if (last != std::string::npos)
			res.replace (last,1,"\0");
		return const_cast<char *>(res.c_str());
	};
};

};
#endif
