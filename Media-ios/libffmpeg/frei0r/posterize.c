/*
 * This file is a port of com.jhlabs.image.PosterizeFilter.java
 * Copyright 2006 Jerry Huxtable
 *
 * posterize.c
 * Copyright 2012 Janne Liljeblad 
 *
 * This file is a Frei0r plugin.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <stdlib.h>
#include <assert.h>

#include "frei0r.h"
#include "frei0r_math.h"

typedef struct posterize_instance
{
  unsigned int width;
  unsigned int height;
  double levels;
} posterize_instance_t;

static int f0r_init()
{
  return 1;
}

static void f0r_deinit()
{ /* no initialization required */ }

static void f0r_get_plugin_info(f0r_plugin_info_t* posterize_info)
{
}

static void f0r_get_param_info(f0r_param_info_t* info, int param_index)
{
  switch(param_index)
  {
  case 0:
    info->name = "levels"; 
    info->type = F0R_PARAM_DOUBLE;
    info->explanation = "Number of values per channel";
    break;
  }
}

static f0r_instance_t f0r_construct(unsigned int width, unsigned int height)
{
	posterize_instance_t* inst = (posterize_instance_t*)calloc(1, sizeof(*inst));
	inst->width = width; 
  inst->height = height;
	inst->levels = 5.0 / 48.0;// input range 0 - 1 will be interpreted as levels range 2 - 50
	return (f0r_instance_t)inst;
}

static void f0r_destruct(f0r_instance_t instance)
{
  free(instance);
}

static void f0r_set_param_value(f0r_instance_t instance, 
                         f0r_param_t param, int param_index)
{
  assert(instance);
  posterize_instance_t* inst = (posterize_instance_t*)instance;

  switch(param_index)
  {
  case 0:
    inst->levels = *((double*)param);
    break;
  }
}

static void f0r_get_param_value(f0r_instance_t instance,
                         f0r_param_t param, int param_index)
{
  assert(instance);
  posterize_instance_t* inst = (posterize_instance_t*)instance;
  
  switch(param_index)
  {
  case 0:
    *((double*)param) = inst->levels;
    break;
  }
}

static void f0r_update(f0r_instance_t instance, double time,
                const uint32_t* inframe, uint32_t* outframe)
{
  assert(instance);
  posterize_instance_t* inst = (posterize_instance_t*)instance;
  unsigned int len = inst->width * inst->height;

  // convert input value 0.0-1.0 to int value 2-50
  double levelsInput = inst->levels * 48.0;
  levelsInput = CLAMP(levelsInput, 0.0, 48.0) + 2.0;
  int numLevels = (int)levelsInput;

  // create levels table
  uint8_t levels[256];
  int i;
  for (i = 0; i < 256; i++)
  {
		  levels[i] = 255 * (numLevels*i / 256) / (numLevels-1);
  }

  uint8_t* dst = (uint8_t*)outframe;
  const uint8_t* src = (uint8_t*)inframe;
  uint8_t r,g,b = 0;
  while (len--)
  {
    r = *src++;
    g = *src++;
    b = *src++;

    r = levels[r];
    g = levels[g];
    b = levels[b];

    *dst++ = r;
    *dst++ = g;
    *dst++ = b;
    *dst++ = *src++;//copy alpha
  }
}




//==================================================================================================
//export
filter_dest(posterize,
	F0R_PLUGIN_TYPE_FILTER,
	F0R_COLOR_MODEL_RGBA8888,
	0,
	1,
	1,
	f0r_update,
	NULL);
