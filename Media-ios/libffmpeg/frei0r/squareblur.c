/* squareblur.c
 * Copyright (C) 2004--2005 Mathieu Guindon
 *                          Julien Keable
 *                          Jean-Sebastien Senecal (js@drone.ws)
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
#include "blur.h"

static int f0r_init()
{
  return 1;
}

static void f0r_deinit()
{ /* no initialization required */ }

static void f0r_get_plugin_info(f0r_plugin_info_t* squareblur_info)
{

}

static void f0r_get_param_info(f0r_param_info_t* info, int param_index)
{
  blur_get_param_info(info, param_index);
}

static f0r_instance_t f0r_construct(unsigned int width, unsigned int height)
{
  return blur_construct(width, height);
}

static void f0r_destruct(f0r_instance_t instance)
{
  blur_destruct(instance);
}

static void f0r_set_param_value(f0r_instance_t instance, 
                         f0r_param_t param, int param_index)
{
  blur_set_param_value(instance, param, param_index);
}

static void f0r_get_param_value(f0r_instance_t instance,
                         f0r_param_t param, int param_index)
{
  blur_get_param_value(instance, param, param_index);
}

static void f0r_update(f0r_instance_t instance, double time,
                const uint32_t* inframe, uint32_t* outframe)
{
  blur_update(instance, time, inframe, outframe);
}


//==================================================================================================
//export
filter_dest(squareblur,
	F0R_PLUGIN_TYPE_FILTER,
	F0R_COLOR_MODEL_RGBA8888,
	0,
	1,
	1,
	f0r_update,
	NULL);

