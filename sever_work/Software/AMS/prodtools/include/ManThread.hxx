#ifndef _MANTHREAD_H
#define _MANTHREAD_H

#ifdef _WIN32
#else
  #include <sys/syscall.h>
#endif

// Thread identification
struct thr_ident
{
	pid_t     id;	// Thread PID
	pthread_t hdl;	// Thread ID (or a handle in Windows notation)
#ifdef _WIN32
	HANDLE    phdl;	// Thread process handle
#endif
};

//_____________________________________________________________________
// 3 classes for multithreding

#ifndef _WIN32

template <typename T=void*> class _thread
{
private:
	explicit _thread(const _thread& thr);

public:
	_thread() {
	};
	virtual ~_thread() {
	};

	// OPERATION
public:
	// Parametrization
	struct tpar {
		tpar(void (*f)()) {
			_fn = f;
			_nargs = 0;
		};
		tpar(void (*f)(T), T arg) {
			_f = f;
			_arg = arg;
			_nargs = 1;
		};
		union {
			void (*_fn)();
			void (*_f)(T);
		};
		T _arg;
		int _nargs;
	};
	// Set cpu affinity
	static int cpuaf(int ncpu=0) {
		cpu_set_t cpuset;
		CPU_ZERO(&cpuset);
		int maxcpu = sysconf(_SC_NPROCESSORS_ONLN);
		if (maxcpu > 0) { if (ncpu<=0 || ncpu>maxcpu) ncpu = maxcpu; }
		  else ncpu = 1;
		CPU_SET(ncpu,&cpuset);
		sched_setaffinity(0,sizeof(cpuset),&cpuset);
		return ncpu;
	};
	// Start asynchronously
	static void run(void (*f)()) throw(int)
	{
		// Initialize thread
		pthread_t thr;
		tpar *tp = new tpar(f);
		if (pthread_create(&thr,0,init,tp)) {
			delete tp;
			throw(0);
		}
	};
	static void run(void (*f)(T), T arg) throw(int)
	{
		// Initialize thread
		pthread_t thr;
		tpar *tp = new tpar(f,arg);
		if (pthread_create(&thr,0,init,tp)) {
			delete tp;
			throw(0);
		}
	};
	// Relinquish the reminder of time slice
	static void yield() { pthread_yield(); };

	// INTERNALS
private:
	// Thread initializer
	static void *init(void *arg)
	{
		// Set cancelation mode
		int state = PTHREAD_CANCEL_ENABLE,
			type  = PTHREAD_CANCEL_ASYNCHRONOUS,
			*oldst = 0;
		pthread_setcanceltype(type,oldst);
		pthread_setcancelstate(state,oldst);
		// Run thread
		tpar *tp = reinterpret_cast<tpar *>(arg);
		if (tp->_nargs) tp->_f(tp->_arg);
		  else tp->_fn();
		// Exit thread
		delete tp;
		pthread_exit(0);
	};

private:
	void operator =(const _thread& thr);
};

class mutex
{
public:
    mutex()
    {
        pthread_mutex_init(&m_handle,0);
    };
    virtual ~mutex()
    {
        pthread_mutex_destroy(&m_handle);
    };
private:
    mutex(const mutex &);
    void operator =(const mutex &);

private:
    pthread_mutex_t	m_handle;

public:
    void lock()
    {
        pthread_mutex_lock(&m_handle);
    };
    void unlock()
    {
        pthread_mutex_unlock(&m_handle);
    };
};
class event
{
public:
    event()
    {
        pthread_mutex_init(&m_handle,0);
        pthread_cond_init(&m_cond,0);
    };
    virtual ~event()
    {
        pthread_cond_destroy(&m_cond);
        pthread_mutex_destroy(&m_handle);
    };
private:
    event(const event &);
    void operator =(const event &);

private:
    pthread_mutex_t	m_handle;
    pthread_cond_t  m_cond;

    void lock()
    {
        pthread_mutex_lock(&m_handle);
    };
    void unlock()
    {
        pthread_mutex_unlock(&m_handle);
    };

public:
    void wait()
    {
        lock();
        pthread_cond_wait(&m_cond,&m_handle);
        unlock();
    };
    void raise()
    {
        lock();
        pthread_cond_broadcast(&m_cond);
        unlock();
    };
};
#endif	// eof #ifndef _WIN32

#endif	// eof _MANTHREAD_H
