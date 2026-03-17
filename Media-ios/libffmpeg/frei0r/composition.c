
#include <stdlib.h>
#include <assert.h>

#include "frei0r.h"
#include "frei0r_math.h"

typedef struct composition_instance
{
  unsigned int width;
  unsigned int height;
} composition_instance_t;

static int f0r_init()
{
  return 1;
}

static void f0r_deinit()
{ /* no initialization required */ }

static void f0r_get_plugin_info(f0r_plugin_info_t* compositionInfo)
{

}

static void f0r_get_param_info(f0r_param_info_t* info, int param_index)
{
  /* no params */
}

static f0r_instance_t f0r_construct(unsigned int width, unsigned int height)
{
  composition_instance_t* inst = (composition_instance_t*)calloc(1, sizeof(*inst));
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
  composition_instance_t* inst = (composition_instance_t*)instance;
  unsigned int w = inst->width;
  unsigned int h = inst->height;

  uint8_t *ps1, *ps2, *pd, *pd_end;
  ps1 = (uint8_t *)inframe2;
  ps2 = (uint8_t *)inframe1;
  pd = (uint8_t *)outframe;
  pd_end = pd + ( w * h * 4 );
  while ( pd < pd_end ) {
	  pd[0] = ( ( ( ps1[0] - ps2[0] ) * 255 * ps1[3] ) >> 16 ) + ps2[0];
	  pd[1] = ( ( ( ps1[1] - ps2[1] ) * 255 * ps1[3] ) >> 16 ) + ps2[1];
	  pd[2] = ( ( ( ps1[2] - ps2[2] ) * 255 * ps1[3] ) >> 16 ) + ps2[2];
	  pd[3] = CLAMP0255( ps1[3] + ps2[3] );
	  ps1 += 4;
	  ps2 += 4;
	  pd += 4;
  }
}



//==================================================================================================
//export
filter_dest(composition,
	F0R_PLUGIN_TYPE_MIXER2,
	F0R_COLOR_MODEL_RGBA8888,
	0,
	9,
	0,
	NULL,
	f0r_update2);