#ifdef __arm64__

#include "libavutil/cpu.h"
#include "libavutil/cpu_internal.h"
#include "ffmpeg-config.h"

int ff_get_cpu_flags_aarch64(void)
{
    return AV_CPU_FLAG_ARMV8 * HAVE_ARMV8 |
           AV_CPU_FLAG_NEON  * HAVE_NEON  |
           AV_CPU_FLAG_VFP   * HAVE_VFP;
}


#endif
