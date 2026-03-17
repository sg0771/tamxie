/* lenscorrection.c
 * Copyright (C) 2007 Richard Spindler
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
#include <math.h>
#include <assert.h>

#include "frei0r.h"
#include "frei0r_math.h"

typedef struct lenscorrection_instance
{
  unsigned int width;
  unsigned int height;
  double xcenter;
  double ycenter;
  double correctionnearcenter;
  double correctionnearedges;
  double brightness;
} lenscorrection_instance_t;


static int f0r_init()
{
  return 1;
}

static void f0r_deinit()
{ /* no initialization required */ }

static void f0r_get_plugin_info(f0r_plugin_info_t* lenscorrection_info)
{
}

static void f0r_get_param_info(f0r_param_info_t* info, int param_index)
{
	switch(param_index)
	{
		case 0:
			info->name = "X center";
			info->type = F0R_PARAM_DOUBLE;
			info->explanation = "";
			break;
		case 1:
			info->name = "Y center";
			info->type = F0R_PARAM_DOUBLE;
			info->explanation = "";
			break;
		case 2:
			info->name = "Correction near center";
			info->type = F0R_PARAM_DOUBLE;
			info->explanation = "";
			break;
		case 3:
			info->name = "Correction near edges";
			info->type = F0R_PARAM_DOUBLE;
			info->explanation = "";
			break;
		case 4:
			info->name = "Brightness";
			info->type = F0R_PARAM_DOUBLE;
			info->explanation = "";
			break;
	}
}

static f0r_instance_t f0r_construct(unsigned int width, unsigned int height)
{
  lenscorrection_instance_t* inst = (lenscorrection_instance_t*)calloc(1, sizeof(*inst));
  inst->width = width; inst->height = height;

  inst->xcenter = 0.5;
  inst->ycenter = 0.5;
  inst->correctionnearcenter = 0.5;
  inst->correctionnearedges = 0.5;
  inst->brightness = 0.5;
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
	lenscorrection_instance_t* inst = (lenscorrection_instance_t*)instance;

	switch(param_index)
	{
		double val;
		case 0:
			val = *((double*)param);
			inst->xcenter = val;
			break;
		case 1:
			val = *((double*)param);
			inst->ycenter = val;
			break;
		case 2:
			val = *((double*)param);
			inst->correctionnearcenter = val;
			break;
		case 3:
			val = *((double*)param);
			inst->correctionnearedges = val;
			break;
		case 4:
			val = *((double*)param);
			inst->brightness = val;
			break;
	}
}

static void f0r_get_param_value(f0r_instance_t instance,
		f0r_param_t param, int param_index)
{
	assert(instance);
	lenscorrection_instance_t* inst = (lenscorrection_instance_t*)instance;

	switch(param_index)
	{
		case 0:
			*((double*)param) = inst->xcenter;
			break;
		case 1:
			*((double*)param) = inst->ycenter;
			break;
		case 2:
			*((double*)param) = inst->correctionnearcenter;
			break;
		case 3:
			*((double*)param) = inst->correctionnearedges;
			break;
		case 4:
			*((double*)param) = inst->brightness;
			break;
	}
}

static void f0r_update(f0r_instance_t instance, double time,
		const uint32_t* inframe, uint32_t* outframe)
{
	//Algorithm fetched from Krita
	int x, y;
	assert(instance);
	lenscorrection_instance_t* inst = (lenscorrection_instance_t*)instance;

	double xcenter = inst->xcenter;
	double ycenter = inst->ycenter;
	double correctionnearcenter = inst->correctionnearcenter;
	double correctionnearedges = inst->correctionnearedges;
	/* double brightness = inst->brightness; */

	double normallise_radius_sq = 4.0 / (inst->width * inst->width + inst->height * inst->height );
	xcenter = inst->width * xcenter;
	ycenter = inst->height * ycenter;
	double mult_sq = ( correctionnearcenter - 0.5 );
	double mult_qd = ( correctionnearedges - 0.5);

	for ( y = 0; y < inst->height; y++ ) {
		for ( x = 0; x < inst->width; x++ ) {
			double off_x = x - xcenter;
			double off_y = y - ycenter;
			double radius_sq = ( (off_x * off_x) + (off_y * off_y) ) * normallise_radius_sq;

			double radius_mult = radius_sq * mult_sq + radius_sq * radius_sq * mult_qd;
			/* double mag = radius_mult; */
			radius_mult += 1.0;
			double srcX = xcenter + radius_mult * off_x;
			double srcY = ycenter + radius_mult * off_y;

			/* double brighten = 1.0 + mag * brightness; */
				// Disabled to avoid compiler warnings
			
			int sx;
			int sy;
			sx = srcX;
			sy = srcY;
			if ( sx < 0 || sy < 0 || sx >= inst->width || sy >= inst->height ) {
				outframe[x + y * inst->width] =	0x00000000;
				continue;
			}
			//FIXME: interpolate pixel!!
			outframe[x + y * inst->width] = inframe[sx + sy * inst->width];
		}
	}
}


//export
filter_dest(lenscorrection,
	F0R_PLUGIN_TYPE_FILTER,
	F0R_COLOR_MODEL_RGBA8888,
	0,
	2,
	5,
	f0r_update,
	NULL);

