/* 
  Copyright 2008-2013 LibRaw LLC (info@libraw.org)

LibRaw is free software; you can redistribute it and/or modify
it under the terms of the one of two licenses as you choose:

1. GNU LESSER GENERAL PUBLIC LICENSE version 2.1
   (See file LICENSE.LGPL provided in LibRaw distribution archive for details).

2. COMMON DEVELOPMENT AND DISTRIBUTION LICENSE (CDDL) Version 1.0
   (See file LICENSE.CDDL provided in LibRaw distribution archive for details).

*/

#include <math.h>

#define LIBRAW_LIBRARY_BUILD
#define LIBRAW_IO_REDEFINED
#define CLASS LibRaw::

#include "libraw_types.h"

#include "libraw.h"
#include "defines.h"
#define SRC_USES_SHRINK
#define SRC_USES_BLACK
#define SRC_USES_CURVE

/* DHT and AAHD are LGPL licensed, so include them */
#include "dht_demosaic.h"
#include "aahd_demosaic.h"
#include "var_defines.h"

/* DCB is BSD licensed, so include it */
#include "dcb_demosaicing.h"

