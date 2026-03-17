#include "avcore.h"

unsigned avcore_version(void)
{
    return LIBAVCORE_VERSION_INT;
}

const char *avcore_configuration(void)
{
    return FFMPEG_CONFIGURATION;
}

const char *avcore_license(void)
{
#define LICENSE_PREFIX "libavcore license: "
    return LICENSE_PREFIX FFMPEG_LICENSE + sizeof(LICENSE_PREFIX) - 1;
}
