/* libjpeg-turbo build number */
#define BUILD  "20210913"

/* Compiler's inline keyword */
#undef inline

/* How to obtain function inlining. */
#define INLINE  //__forceinline

/* How to obtain thread-local storage */
#ifdef _WIN32
#define THREAD_LOCAL  __declspec(thread)
#else
#define  THREAD_LOCAL __thread
#endif
/* Define to the full name of this package. */
#define PACKAGE_NAME  "libjpeg-turbo"

/* Version number of package */
#define VERSION  "2.1.2"

/* The size of `size_t', as computed by sizeof. */
#if  defined(_M_X64) || defined(_WIN64) || defined(__x86_64__)
#define SIZEOF_SIZE_T  8
#else
#define SIZEOF_SIZE_T  4
#endif // DEBUG



/* Define if your compiler has __builtin_ctzl() and sizeof(unsigned long) == sizeof(size_t). */
/* #undef HAVE_BUILTIN_CTZL */

/* Define to 1 if you have the <intrin.h> header file. */
#ifdef _MSC_VER
#define HAVE_INTRIN_H   
#endif
#if defined(_MSC_VER) && defined(HAVE_INTRIN_H)
#if (SIZEOF_SIZE_T == 8)
#define HAVE_BITSCANFORWARD64
#elif (SIZEOF_SIZE_T == 4)
#define HAVE_BITSCANFORWARD
#endif
#endif

#if defined(__has_attribute)
#if __has_attribute(fallthrough)
#define FALLTHROUGH  __attribute__((fallthrough));
#else
#define FALLTHROUGH
#endif
#else
#define FALLTHROUGH
#endif
