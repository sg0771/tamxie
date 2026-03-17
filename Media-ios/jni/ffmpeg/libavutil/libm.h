#ifndef AVUTIL_LIBM_H
#define AVUTIL_LIBM_H

#include <math.h>
#include "attributes.h"

#if !HAVE_EXP2
#undef exp2
#define exp2(x) exp((x) * 0.693147180559945)
#endif /* HAVE_EXP2 */

#if !HAVE_EXP2F
#undef exp2f
#define exp2f(x) ((float)exp2(x))
#endif /* HAVE_EXP2F */

#if !HAVE_LLRINT
#undef llrint
#define llrint(x) ((long long)rint(x))
#endif /* HAVE_LLRINT */

#if !HAVE_LLRINTF
#undef llrintf
#define llrintf(x) ((long long)rint(x))
#endif /* HAVE_LLRINT */

#if !HAVE_LOG2
#undef log2
#define log2(x) (log(x) * 1.44269504088896340736)
#endif /* HAVE_LOG2 */

#if !HAVE_LOG2F
#undef log2f
#define log2f(x) ((float)log2(x))
#endif /* HAVE_LOG2F */

#endif /* AVUTIL_LIBM_H */
