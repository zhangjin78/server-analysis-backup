#ifndef _MANCOMMON_H
#define _MANCOMMON_H

// For MIT AMS project
#ifndef _NONE_PVSS_API
	#define _NONE_PVSS_API
#endif

#ifdef _WIN32
  #define strdup _strdup
  #define getcwd _getcwd
#endif

// WINDOWS/LINUX PORTABILITY

#ifdef _WIN32
	#include <windows.h>
	#include <process.h>
    #include <time.h>
	#include <direct.h>
	#include <io.h>
	#include <sys/timeb.h>
#else
	#include <stdio.h>
    #include <stdarg.h>
	#include <pthread.h>
	#include <sys/time.h>
	#include <sys/types.h>
	#include <sys/stat.h>
	#include <sys/wait.h>
	#include <dlfcn.h>
	#include <pwd.h>
	#include <unistd.h>
	#include <dirent.h>
	#include <string.h>
	#include <iostream>
	#include <sstream>
	#include <boost/format.hpp>
	#include <boost/lexical_cast.hpp>
#endif

#include <math.h>
#include <time.h>

#include <vector>
using std::vector;

#ifndef _NONE_PVSS_API
  #include <Manager.hxx>
  #include <ErrHdl.hxx>
  #include <ErrClass.hxx>
#endif

// SOME COMMON DECLARATIONS

typedef unsigned char  uchar;
typedef unsigned short ushort;
typedef unsigned int   uint;
typedef unsigned long  ulong;

#ifdef _WIN32		// Windows
  // types
  typedef __int64 longlong;
  // processes
  #define getpid			_getpid
  #define pthread_t			long
  #define pthread_mutex_t 	HANDLE
  #define pid_t				int
  // sockets
  #define socklen_t			int
#else				// Linux
  // types
  typedef long long longlong;
  // sockets
  #define SOCKET			int
  #define SD_RECEIVE		0
  #define SD_SEND			1
  #define SD_BOTH			2
  #define INVALID_SOCKET	-1
  #define SOCKET_ERROR		-1
#endif

// TCPIP socket option return structure
union sock_optval
{
	char cb[64];
	int  ib[64/sizeof(int)];
};

// PVSS CharString

#ifdef _NONE_PVSS_API
  #include <string>
  using std::string;

//_____________________________________________________________________
// Our own implementation of the PVSS API CharString class

class CharString: public string
{
public:
	inline CharString(): string() {};
	inline CharString(const char* c)       { *this = c; };
	inline CharString(const string &s)     { *this = s.c_str(); };
	inline CharString(const CharString& s) { *this = s.c_str(); };
public:
	size_t len() const
	{
		return size();
	};
	void format(const char * lpszFormat, ...)
	{
		char s[512];

		va_list args;
		va_start(args,lpszFormat);
		vsprintf(s,lpszFormat,args);
		va_end(args);

		*this = s;
	};
	int replace(const char *what, const char *with)
	{
		int result = 0;
		int pos;

		while ((pos = find(what)) >= 0)
		{
			if (strlen(with) > 0) string::replace(pos,strlen(what),with);
			  else				  string::erase(pos,1);
			result++;
		}
		return result;
	};
	CharString& trim()
	{
		replace("\r","\0x0");
		replace("\n","\0x0");

		int pos;

		for (pos=0; pos<(int)size(); pos++)
			if ((*this)[pos] == ' ' || (*this)[pos] == '\t')
				string::erase(pos--,1);
			else
				break;
		for (pos=(int)size()-1; pos>=0; pos--)
			if ((*this)[pos] == ' ' || (*this)[pos] == '\t')
				string::erase(pos,1);
			else
				break;

		return *this;
	};
	CharString substring(size_t start, size_t len=(size_t)-1) const
	{
		CharString s;

		if (len < 0)
			len = size();
		for (uint i=0; start+i<size() && i<len; i++)
			s += at(start+i);
		return s;
	};
public:
	CharString& operator = (const CharString& s)
	{
		clear();
		*this += s;
		return *this;
	}
	CharString& operator = (const char* c)
	{
		clear();
		*this += c;
		return *this;
	}
	operator char* () const { return (char *)c_str(); }
	char operator [] (size_t index) const { return at(index); }
	char operator [] (int index) const { return at(index); }
};
inline CharString operator + (const char *c, const CharString &s)
{
	CharString result = c;
	result += s;
	return result;
};
inline CharString operator + (const CharString &s, const char *c)
{
	CharString result = s;
	result += c;
	return result;
};
inline CharString operator + (const CharString &s1, const CharString &s2)
{
	CharString result = s1;
	result += s2;
	return result;
};

#else	// #ifndef _NONE_PVSS_API

// The original PVSS API CharString class
// There is a bug in the PVSS 3.8 API, which doesn't let to apply 
// CharString formating for the cace below
//          args.format("%s ...",(char *)args,...); 
// it is very pity that ETM doesn't want to deal with that

inline void CharString::format(const char * lpszFormat, ...)
{
	char s[512];

	va_list args;
	va_start(args,lpszFormat);
	vsprintf(s,lpszFormat,args);
	va_end(args);

	*this = s;
};
#endif

//_____________________________________________________________________
// Generic

class ManException
{
public:
	ManException() { };
	ManException(const char *message) {
		m_Message = message;
	};
	ManException(const CharString &message) {
		m_Message = message;
	};
	virtual ~ManException() { };

private:
	CharString m_Message;

public:
	const char *mess() const {
		return (char *)m_Message;
	};
};

//_____________________________________________________________________
// Time manipulation

// Get UTC time, in msec
static double timeNow()
{
#ifdef _WIN32
	struct _timeb timebuffer;

	_ftime(&timebuffer);
	return 1000.0 * (double)timebuffer.time + (double) timebuffer.millitm;
#else
	struct timeval tv;
	struct timezone tz;

	gettimeofday(&tv, &tz);
	return 1000.0*(double)tv.tv_sec + (double)(tv.tv_usec)/1000.0;
#endif
};
static double timeNow(time_t sec, time_t msec)
{
	return (double)sec * 1000 + msec;
};
static longlong timeLongLong()
{
#ifdef _WIN32
	struct _timeb timebuffer;

	_ftime( &timebuffer ); // overflows 2038
	return 1000 * (longlong)timebuffer.time + (longlong) timebuffer.millitm;
#else
	struct timeval tv;
	struct timezone tz;

	gettimeofday(&tv, &tz);
	return 1000*(longlong)tv.tv_sec + ((longlong)(tv.tv_usec))/1000;
#endif
};
static longlong timeLongLong(time_t sec, time_t msec)
{
	return (longlong)sec * 1000 + msec;
};
// Time interval calculation
static double difftimeNow(double end, double start)
{
	return end-start;
};
static longlong difftimeLongLong(longlong end, longlong start)
{
	return end-start;
};
// Time manipulation
#ifndef _NONE_PVSS_API
  static TimeVar timeVariable(long sec, long milli)
  {
	TimeVar tvs;

	tvs.setSeconds(sec);
	tvs.setMilli((PVSSshort)milli);

	return tvs;
  };
#endif
static CharString gmtimeString(time_t s, bool full=true, int resol=2)
{
  	tm *t = gmtime(&s);

  	CharString ts;
  	if (full)
  		ts.format("%04d.%02d.%02d %02d:%02d:%02d",
  				  t->tm_year+1900,t->tm_mon+1,t->tm_mday,t->tm_hour,t->tm_min,t->tm_sec);
  	else {
  		// Indication of time intervals here
  		std::stringstream str;
  		str << "%0" << boost::format("%d") %resol << "d:%02d:%02d";
  		ts.format(str.str().c_str(),(t->tm_mday-1)*24+t->tm_hour,t->tm_min,t->tm_sec);
  	}
  	return ts;
};
static CharString timeString(time_t s, time_t ms)
{
	struct tm *t = localtime(&s);

	CharString ts;
	ts.format("%04d.%02d.%02d %02d:%02d:%02d.%03d",
			  t->tm_year+1900,t->tm_mon+1,t->tm_mday,t->tm_hour,t->tm_min,t->tm_sec,
			  ms);
	return ts;
};
static CharString timeString(time_t s, bool full=true)
{
	tm *t = localtime(&s);

	CharString ts;
	if (full)
		ts.format("%04d.%02d.%02d %02d:%02d:%02d",
				  t->tm_year+1900,t->tm_mon+1,t->tm_mday,t->tm_hour,t->tm_min,t->tm_sec);
	else
		ts.format("%02d:%02d:%02d",t->tm_hour,t->tm_min,t->tm_sec);
	return ts;
};
static CharString timeString(longlong utc, bool full=true)
{
	longlong s   = utc / 1000;
	longlong ms  = utc % 1000;
	time_t time  = (time_t)s;
	struct tm *t = localtime(&time);

	CharString ts;

	if (full)
		ts.format("%04d.%02d.%02d %02d:%02d:%02d.%03d",
				  t->tm_year+1900,t->tm_mon+1,t->tm_mday,t->tm_hour,t->tm_min,t->tm_sec,
				  ms);
	else
		ts.format("%02d:%02d:%02d.%03d",
				  t->tm_hour,t->tm_min,t->tm_sec,
				  ms);
	return ts;
};
static CharString timeString(double utc, bool full=true)
{
	double s;

	double ms    = modf(utc/1000,&s);
	time_t time  = (time_t)s;
	struct tm *t = localtime(&time);

	CharString ts;

	if (full)
		ts.format("%04d.%02d.%02d %02d:%02d:%02d.%03d",
				  t->tm_year+1900,t->tm_mon+1,t->tm_mday,t->tm_hour,t->tm_min,t->tm_sec,
				  (long)(ms*1000));
	else
		ts.format("%02d:%02d:%02d.%03d",
				  t->tm_hour,t->tm_min,t->tm_sec,
				  (long)(ms*1000));
	return ts;
};
#ifdef _WIN32
  // Change or add an environment variable
  static int setenv(const char *name, const char *value, int overwrite)
  {
	CharString env = name;
	env += "=";
	env += value;
	return putenv((char *)env);
  };
#else
  // Sleep for ms milliseconds for default factor, for 
  // microsec sleep - factor should be equal to 1000 
  static void Sleep(long ms, long factor=1)
  {
	timespec tms_in, tms_out;

	long nano = 1000000000;
	long sec_factor = factor * 1000;
	tms_in.tv_sec = ms / sec_factor;
	tms_in.tv_nsec = (ms % sec_factor) * (nano / sec_factor);
	nanosleep(&tms_in,&tms_out);
  };
#endif

//_____________________________________________________________________
// System dependent

struct common
{
	// Double rounding
	template<typename T> inline static T round(double val)
	{
		if (val != 0)
		{
			double sign = val / fabs(val);
			val += sign * 0.5;
		}
		return static_cast<T>(val);
	};
	// More complex string duplication, accepts 0 pointer on Linux,
	// also lets to avoid malloc/free usage
	static char *strdup(const char *s, int len=-1)
	{
		if (s == 0 || len == 0) return 0;

		if (len < 0)
			len = strlen(s);
		char *result = new char[len+1];
		if (len > 0)
			strncpy(result,s,len);
		result[len] = '\0';

		return result;
	};
	static char *strdup(const CharString &s)
	{
		return common::strdup((char *)s,s.len());
	};
	// A little bit paranoic string truncation
	static void trim(CharString &s)
	{
		s.replace("\r","\0x0");
		s.replace("\n","\0x0");
		s.trim();
	};
	static void trim(char *s)
	{
		if (s)
		{
			CharString ts = s;
			common::trim(ts);
			if (ts.len() > 0) strcpy(s,(char *)ts);
			  else			  s[0] = '\0';
		}
	};
	// Convert string to command line argument format
	// The caller is responsible to release memory
	// Not system dependent
	static char **str2argv(const char *string, int &argc)
	{
		argc = 0;

		int len=strlen(string);
		if (len < 1)
			return 0;

		char *s = common::strdup(string);
		char last = '\0';
		{
			for (int i=0; i<len; i++)
			{
				if (s[i] == ' ' || s[i] == '\t' || s[i] == '\r' || s[i] == '\n')
				{
					s[i] = '\0';
					argc++;
				}
				last = s[i];
			}
		}
		if (last != '\0') argc++;

		char **argv = new char*[argc];
		{
			for (int i=0,k=0; i<len; i++)
				if (s[i] != '\0')
				{
					argv[k++] = common::strdup(&s[i]);
					i += strlen(&s[i]);
				}
		}
		delete s;

		return argv;
	};
	// Remove all file(s) from folder, not recursive.
	// The input argument:
	//   path - path to folder, absolute or relative
	static void rmdir(const char *path) throw (ManException &)
	{
		CharString spath = path;
		char last = spath[spath.len()-1];
		if (last != '\\' && last != '/')
			spath += "/";

#ifdef _WIN32
		CharString sfile = spath + "*";
		struct _finddata_t fileinfo;
		int result = 0;

		long scandir_id = _findfirst((char *)sfile,&fileinfo);
		while (scandir_id >= 0 && result >= 0)
		{
			bool rmfile = fileinfo.attrib != _A_SUBDIR;
			sfile = spath + fileinfo.name;
			
			result = _findnext(scandir_id,&fileinfo);

			if (rmfile && remove((char *)sfile) < 0)
			{
				CharString err = "common::rmdir - error on removing " + sfile;
				throw ManException(err);
			}
		}
#else
		DIR *dp;
		struct dirent *ep;
		struct stat ep_stat;

		if ((dp = opendir((char *)spath)) == 0)
			return;
		while ((ep=readdir(dp)) != 0)
		{
			CharString sfile = CharString(spath) + ep->d_name;

			if (::stat((char *)sfile,&ep_stat) == -1)
			{
				CharString err = "common::rm - bad " + sfile + " attributes";
				throw ManException(err);
			}
			int rmfile = S_ISREG(ep_stat.st_mode);
			if (rmfile && remove((char *)sfile) < 0)
			{
				CharString err = "common::rm - error on removing " + sfile;
				throw ManException(err);
			}
		}
		closedir(dp);
#endif
	};
	// Open dynamic library
	static void dllOpen(void *&handle, const char *pathname) 
		throw (ManException &)
	{
		handle = 0;

#ifdef _WIN32
		handle = (void *)LoadLibrary(pathname);
		if (handle == 0)
		{
			char msgBuf[128];

			FormatMessage( 
			FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			GetLastError(),
			0,
			(LPTSTR)&msgBuf,
			128,
			NULL);

			throw ManException(CharString(pathname)+": "+(char *)msgBuf);
		}
#else
		handle = dlopen(pathname,RTLD_LAZY);
		if (handle == 0)
			throw ManException(dlerror());
#endif
	};
	// Get the address of the symbol of the dynamic library
	static void dllSym(void *&address, void *handle, const char *name)
		throw (ManException &)
	{
#ifdef _WIN32
		address = (void *)GetProcAddress((HINSTANCE)handle,name);
		if (address == 0)
		{
			char msgBuf[128];

			FormatMessage( 
			FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			GetLastError(),
			0,
			(LPTSTR)&msgBuf,
			128,
			NULL);

			throw ManException(CharString(name)+": "+(char *)msgBuf);
		}
#else
		address = (void *)dlsym(handle,name);
		if (address == 0)
			throw ManException(dlerror());
#endif
	};
	// Get hostname
	static CharString hostname()
	{
		char s[256];

		if (gethostname(s,256) == 0)
			return CharString(s);
		else
			return CharString("unknown");
	}
	// Get effective user
	static CharString user()
	{
		CharString result;

#ifdef _WIN32
#else
		passwd *me = getpwuid(geteuid());
		if (me && me->pw_name)
			result = me->pw_name;
#endif
		return result;
	};
	// Get the amount of time [msec] that process has executed in kernel and user mode
	static double cpuTime()
	{
#ifdef _WIN32
		FILETIME ftCreationTime, ftExitTime, ftKernelTime, ftUserTime;

		if (GetProcessTimes(GetCurrentProcess(),&ftCreationTime,&ftExitTime,&ftKernelTime,&ftUserTime))
		{
			longlong kernelTime = ftKernelTime.dwHighDateTime;
			kernelTime = ((kernelTime<<32) + ftKernelTime.dwLowDateTime) / 10000;

			longlong userTime = ftUserTime.dwHighDateTime;
			userTime = ((userTime<<32) + ftUserTime.dwLowDateTime) / 10000;

			return (double)kernelTime + userTime;
		}
		else
			return timeNow();
#else
		return 0;
#endif
	};
};

#endif
