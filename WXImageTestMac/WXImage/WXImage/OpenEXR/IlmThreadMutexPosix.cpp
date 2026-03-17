#ifndef _WIN32

#include "IlmBaseConfig.h"

#include "IlmThreadMutex.h"
#include "Iex.h"
#include <assert.h>

ILMTHREAD_INTERNAL_NAMESPACE_SOURCE_ENTER


Mutex::Mutex ()
{
    if (int error = ::pthread_mutex_init (&_mutex, 0))
        IEX_INTERNAL_NAMESPACE::throwErrnoExc ("Cannot initialize mutex (%T).", error);
}


Mutex::~Mutex ()
{
    int error = ::pthread_mutex_destroy (&_mutex);
    assert (error == 0);
}


void
Mutex::lock () const
{
    if (int error = ::pthread_mutex_lock (&_mutex))
        IEX_INTERNAL_NAMESPACE::throwErrnoExc ("Cannot lock mutex (%T).", error);
}


void
Mutex::unlock () const
{
    if (int error = ::pthread_mutex_unlock (&_mutex))
        IEX_INTERNAL_NAMESPACE::throwErrnoExc ("Cannot unlock mutex (%T).", error);
}


ILMTHREAD_INTERNAL_NAMESPACE_SOURCE_EXIT

#endif
