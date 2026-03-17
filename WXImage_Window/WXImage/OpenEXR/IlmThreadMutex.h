#ifndef INCLUDED_ILM_THREAD_MUTEX_H
#define INCLUDED_ILM_THREAD_MUTEX_H

#include "IlmThreadExport.h"
#include "IlmBaseConfig.h"
#include "IlmThreadNamespace.h"

#if defined _WIN32 || defined _WIN64
    #ifdef NOMINMAX
        #undef NOMINMAX
    #endif
    #define NOMINMAX
    #include <windows.h>
#else
    #include <pthread.h>
#endif

ILMTHREAD_INTERNAL_NAMESPACE_HEADER_ENTER

class Lock;
class ILMTHREAD_EXPORT Mutex
{
  public:

    Mutex ();
    virtual ~Mutex ();

  private:

    void	lock () const;
    void	unlock () const;

    #if defined _WIN32 || defined _WIN64
	mutable CRITICAL_SECTION _mutex;
    #else
	mutable pthread_mutex_t _mutex;
    #endif

    void operator = (const Mutex& M);	// not implemented
    Mutex (const Mutex& M);		// not implemented
    
    friend class Lock;
};


class ILMTHREAD_EXPORT Lock
{
  public:

    Lock (const Mutex& m, bool autoLock = true):
	_mutex (m),
	_locked (false)
    {
        if (autoLock)
        {
            _mutex.lock();
            _locked = true;
        }
    }
    
    ~Lock ()
    {
        if (_locked)
            _mutex.unlock();
    }
    
    void acquire ()
    {
        _mutex.lock();
        _locked = true;
    }
    
    void release ()
    {
        _mutex.unlock();
        _locked = false;
    }
    
    bool locked ()
    {
        return _locked;
    }

  private:

    const Mutex &	_mutex;
    bool		_locked;
};


ILMTHREAD_INTERNAL_NAMESPACE_HEADER_EXIT

#endif // INCLUDED_ILM_THREAD_MUTEX_H
