#ifndef INCLUDED_ILM_THREAD_SEMAPHORE_H
#define INCLUDED_ILM_THREAD_SEMAPHORE_H

#include "IlmBaseConfig.h"
#include "IlmThreadExport.h"
#include "IlmThreadNamespace.h"

#if defined _WIN32 || defined _WIN64
    #ifdef NOMINMAX
        #undef NOMINMAX
    #endif
    #define NOMINMAX
    #include <windows.h>
#else
    #include <pthread.h>
    #include <semaphore.h>
#endif

ILMTHREAD_INTERNAL_NAMESPACE_HEADER_ENTER


class ILMTHREAD_EXPORT Semaphore
{
  public:

    Semaphore (unsigned int value = 0);
    virtual ~Semaphore();

    void	wait();
    bool	tryWait();
    void	post();
    int		value() const;

  private:

    #if defined _WIN32 || defined _WIN64

		mutable HANDLE _semaphore;
    #else
		mutable sem_t _semaphore;
    #endif

    void operator = (const Semaphore& s);	// not implemented
    Semaphore (const Semaphore& s);		// not implemented
};


ILMTHREAD_INTERNAL_NAMESPACE_HEADER_EXIT

#endif // INCLUDED_ILM_THREAD_SEMAPHORE_H
