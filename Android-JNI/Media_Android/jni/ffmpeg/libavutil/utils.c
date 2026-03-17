#include "avutil.h"

unsigned avutil_version(void)
{
    return LIBAVUTIL_VERSION_INT;
}

const char *avutil_configuration(void)
{
    return FFMPEG_CONFIGURATION;
}

const char *avutil_license(void)
{
#define LICENSE_PREFIX "libavutil license: "
    return LICENSE_PREFIX FFMPEG_LICENSE + sizeof(LICENSE_PREFIX) - 1;
}
