/* 
 * spillsupress.c
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
#include <string.h>

#include "frei0r.h"

void green_limited_by_blue(unsigned int len, const uint32_t* inframe, uint32_t* outframe)
{

  uint8_t* dst = (uint8_t*)outframe;
  const uint8_t* src = (uint8_t*)inframe;
  uint8_t b = 0;
  uint8_t g = 0;

  while (len--)
  {
    *dst++ = *src++;
    g = *src++;
    *dst++;
    b = *src++;
    *dst++;
    *dst++ = *src++;

    if( g > b )
    {
      *(dst - 3) = b;
      *(dst - 2) = b;
    }
    else
    {
      *(dst - 3) = g;
      *(dst - 2) = b;
    }
  
  }
}

void blue_limited_by_green(unsigned int len, const uint32_t* inframe, uint32_t* outframe)
{

  uint8_t* dst = (uint8_t*)outframe;
  const uint8_t* src = (uint8_t*)inframe;
  uint8_t b = 0;
  uint8_t g = 0;

  while (len--)
  {
    *dst++ = *src++;
    g = *src++;
    *dst++;
    b = *src++;
    *dst++;
    *dst++ = *src++;

    if( b > g )
    {
      *(dst - 3) = g;
      *(dst - 2) = g;
    }
    else
    {
      *(dst - 3) = g;
      *(dst - 2) = b;
    }
  
  }
}

typedef struct spillsupress_instance
{
  unsigned int width;
  unsigned int height;
  double supress_type; /* type of spill supression applied to image 
                        <= 0.5, green supress
                        > 0.5, blue supress */
} spillsupress_instance_t;

static int f0r_init()
{
  return 1;
}

static void f0r_deinit()
{ /* no initialization required */ }

static void f0r_get_plugin_info(f0r_plugin_info_t* spillsupress_info)
{

}

static void f0r_get_param_info(f0r_param_info_t* info, int param_index)
{
  switch(param_index)
  {
  case 0:
    info->name = "supresstype";
    info->type = F0R_PARAM_DOUBLE;
    info->explanation = "Defines if green or blue screen spill suppress is applied";
    break;
  }
}

static f0r_instance_t f0r_construct(unsigned int width, unsigned int height)
{
	spillsupress_instance_t* inst = (spillsupress_instance_t*)calloc(1, sizeof(*inst));
	inst->width = width; 
  inst->height = height;
	inst->supress_type = 0.0; // default supress type is green supress
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
  spillsupress_instance_t* inst = (spillsupress_instance_t*)instance;

  switch(param_index)
  {
  case 0:
    inst->supress_type = *((double*)param);
    break;
  }
}

static void f0r_get_param_value(f0r_instance_t instance,
                         f0r_param_t param, int param_index)
{
  assert(instance);
  spillsupress_instance_t* inst = (spillsupress_instance_t*)instance;
  
  switch(param_index)
  {
  case 0:
    *((double*)param) = inst->supress_type;
    break;
  }
}

static void f0r_update(f0r_instance_t instance, double time,
                const uint32_t* inframe, uint32_t* outframe)
{
  assert(instance);
  spillsupress_instance_t* inst = (spillsupress_instance_t*)instance;
  unsigned int len = inst->width * inst->height;

  if (inst->supress_type > 0.5)
  {
    blue_limited_by_green(len, inframe, outframe);
  }
  else
  {
    green_limited_by_blue(len, inframe, outframe);
  }
}


//==================================================================================================
//export
filter_dest(spillsupress,
	F0R_PLUGIN_TYPE_FILTER,
	F0R_COLOR_MODEL_RGBA8888,
	0,
	1,
	1,
	f0r_update,
	NULL);

