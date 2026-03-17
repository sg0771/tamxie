
#include "IlmBaseConfig.h"

#ifndef _WIN32

#include "IlmThreadSemaphore.h"
#include "Iex.h"
#include <assert.h>
#include <errno.h>

ILMTHREAD_INTERNAL_NAMESPACE_SOURCE_ENTER


Semaphore::Semaphore (unsigned int value)
{
    if (::sem_init (&_semaphore, 0, value))
	IEX_NAMESPACE::throwErrnoExc ("Cannot initialize semaphore (%T).");
}


Semaphore::~Semaphore ()
{
    int error = ::sem_destroy (&_semaphore);
    assert (error == 0);
}


void
Semaphore::wait ()
{
    while( ::sem_wait( &_semaphore ) == -1 && errno == EINTR )
    {
    }
}


bool
Semaphore::tryWait ()
{
    return sem_trywait (&_semaphore) == 0;
}


void
Semaphore::post ()
{
    if (::sem_post (&_semaphore))
        IEX_NAMESPACE::throwErrnoExc ("Post operation on semaphore failed (%T).");
}


int
Semaphore::value () const
{
    int value;

    if (::sem_getvalue (&_semaphore, &value))
        IEX_NAMESPACE::throwErrnoExc ("Cannot read semaphore value (%T).");

    return value;
}


ILMTHREAD_INTERNAL_NAMESPACE_SOURCE_EXIT

#endif
