#include <stdlib.h>
#include <assert.h>

#include "frei0r.h"

typedef struct rgb_instance
{
  unsigned int width;
  unsigned int height;
} rgb_instance_t;

static int f0r_init()
{
  return 1;
}

static void f0r_deinit()
{ /* no initialization required */ }

static void f0r_get_plugin_info(f0r_plugin_info_t* rgbInfo)
{

}

static void f0r_get_param_info(f0r_param_info_t* info, int param_index)
{
  /* no params */
}

static f0r_instance_t f0r_construct(unsigned int width, unsigned int height)
{
  rgb_instance_t* inst = (rgb_instance_t*)calloc(1, sizeof(*inst));
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

static void f0r_update(f0r_instance_t instance, double time,
		const uint32_t* inframe, uint32_t* outframe)
{
  assert(instance);
  rgb_instance_t* inst = (rgb_instance_t*)instance;
  unsigned int w = inst->width;
  unsigned int h = inst->height;
  unsigned int x,y;
  
  uint32_t* dst = outframe;
  const uint32_t* src = inframe;
  for(y=0;y<h;++y)
      for(x=0;x<w;++x,++src) {
	  *dst++ = ( 0xff00ff00 & (*src) ) | ( (0x0000ff00 & (*src)) << 8 ) | ( (0x0000ff00 & (*src)) >> 8 ) ; 
      }
}



//==================================================================================================
//export
filter_dest(G,
	F0R_PLUGIN_TYPE_FILTER,
	F0R_COLOR_MODEL_RGBA8888,
	0,
	9,
	0,
	f0r_update,
	NULL);