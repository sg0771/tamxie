#ifdef _WIN32
#include "IlmThread.h"
#include "Iex.h"
#include <iostream>
#include <assert.h>

ILMTHREAD_INTERNAL_NAMESPACE_SOURCE_ENTER


bool
supportsThreads ()
{
    return true;
}

namespace {

unsigned __stdcall
threadLoop (void * t)
{
    reinterpret_cast<Thread*>(t)->run();
    _endthreadex (0);
    return 0;
}

} // namespace


Thread::Thread ()
{
    // empty
}


Thread::~Thread ()
{
    DWORD status = ::WaitForSingleObject (_thread, INFINITE);
    assert (status ==  WAIT_OBJECT_0);
    bool ok = ::CloseHandle (_thread) != FALSE;
    assert (ok);
}


void
Thread::start ()
{
    unsigned id;
    _thread = (HANDLE)::_beginthreadex (0, 0, &threadLoop, this, 0, &id);

    if (_thread == 0)
        IEX_NAMESPACE::throwErrnoExc ("Cannot create new thread (%T).");
}


ILMTHREAD_INTERNAL_NAMESPACE_SOURCE_EXIT

#endif
