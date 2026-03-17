
#include <stdlib.h>
#include <assert.h>
#include <math.h>

#include "frei0r.h"

typedef struct uvmap_instance
{
  unsigned int width;
  unsigned int height;
} uvmap_instance_t;

static int f0r_init()
{
  return 1;
}

static void f0r_deinit()
{ /* no initialization required */ }

static void f0r_get_plugin_info(f0r_plugin_info_t* uvmapInfo)
{
}

static void f0r_get_param_info(f0r_param_info_t* info, int param_index)
{
  /* no params */
}

static f0r_instance_t f0r_construct(unsigned int width, unsigned int height)
{
  uvmap_instance_t* inst = (uvmap_instance_t*)calloc(1, sizeof(*inst));
  inst->width = width; inst->height = height;
  return (f0r_instance_t)inst;
}

static void f0r_destruct(f0r_instance_t instance)
{
  free(instance);
}

static void f0r_set_param_value(f0r_instance_t instance, 
			 f0r_param_t param, int param_index)
{ /* no params */ }

static void f0r_get_param_value(f0r_instance_t instance,
			 f0r_param_t param, int param_index)
{ /* no params */ }


static void f0r_update2(f0r_instance_t instance,
		 double time,
		 const uint32_t* inframe1,
		 const uint32_t* inframe2,
		 const uint32_t* inframe3,
		 uint32_t* outframe)
{
	assert(instance);
	uvmap_instance_t* inst = (uvmap_instance_t*)instance;
	unsigned int w = inst->width;
	unsigned int h = inst->height;
	unsigned int x,y;

	uint32_t* dst = outframe;
	const uint32_t* uvmap = inframe1;
	const uint32_t* src = inframe2;
	float fx, fy;
	long px, py;
	for( y = 0; y < h; ++y )
		for( x = 0; x < w; ++x ) {
			/* The coordinates start in the lower left corner:
			 *
			 * ^ +-------------+
			 * | |             |
			 * G |             |
			 *  0+-------------+
			 *   0  R ->
			 *
			 */
			uint8_t* tmpc = (uint8_t*)uvmap;
			fx = ((float)tmpc[0]) / 255.0;
			fy = ((float)tmpc[1]) / 255.0;
			fy = 1.0 - fy;

			px = lrintf( w * fx );
			py = lrintf( h * fy );
			if ( tmpc[2] > 128 ) {
				*dst++ = src[px+w*py];
			} else {
				*dst++ = 0x00000000;
			}
			uvmap++;
		}
}


//==================================================================================================
//export
filter_dest(uvmap,F0R_PLUGIN_TYPE_FILTER,F0R_COLOR_MODEL_RGBA8888,0,9,0,NULL,f0r_update2);