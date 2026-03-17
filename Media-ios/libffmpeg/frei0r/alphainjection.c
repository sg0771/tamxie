#include <stdlib.h>
#include <assert.h>

#include "frei0r.h"

typedef struct alphainjection_instance
{
  unsigned int width;
  unsigned int height;
} alphainjection_instance_t;

static int f0r_init()
{
  return 1;
}

static void f0r_deinit()
{ /* no initialization required */ }

static void f0r_get_param_info(f0r_param_info_t* info, int param_index)
{
  /* no params */
}

static f0r_instance_t f0r_construct(unsigned int width, unsigned int height)
{
  alphainjection_instance_t* inst = (alphainjection_instance_t*)calloc(1, sizeof(*inst));
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
  alphainjection_instance_t* inst = (alphainjection_instance_t*)instance;
  unsigned int w = inst->width;
  unsigned int h = inst->height;
  unsigned int x,y;
  
  uint32_t* dst = outframe;
  const uint32_t* alpha = inframe1;
  const uint32_t* src = inframe2;
  for(y=0;y<h;++y)
      for(x=0;x<w;++x,++src) {
	  int tmpbw;
	  uint8_t* tmpc = (uint8_t*)alpha;
	  tmpbw = (tmpc[0] + tmpc[1] + tmpc[2]) / 3;

	  *dst++ = ( 0x00ffffff & (*src) ) | (tmpbw << 24); 
	  alpha++;
      }
}



//==================================================================================================
//export
static void f0r_get_plugin_info(f0r_plugin_info_t* alphainjectionInfo)
{

}

filter_dest(alphainjection,
	F0R_PLUGIN_TYPE_MIXER2,
	F0R_COLOR_MODEL_RGBA8888,
	0,
	9,
	0,
	NULL,
	f0r_update2);
