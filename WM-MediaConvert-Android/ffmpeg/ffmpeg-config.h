#ifndef __FFMPEG__CONFGI__H_
#define __FFMPEG__CONFGI__H_

#if defined(FFMPEG_ARM64)
#include "config.arm64.h"
#elif defined(FFMPEG_ARMV7)
#include "config.armv7.h"
#elif defined(FFMPEG_ARMV5)
#include "config.armv5.h"
#elif defined(FFMPEG_X64)
#include "config.x64.h"
#elif defined(FFMPEG_X86)
#include "config.x86.h"
#endif

#endif