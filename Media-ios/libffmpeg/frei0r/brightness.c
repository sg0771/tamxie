#include <stdlib.h>
#include <assert.h>

#include "frei0r.h"
#include "frei0r_math.h"

typedef struct brightness_instance
{
  unsigned int width;
  unsigned int height;
  int brightness; /* the brightness [-256, 256] */
  uint8_t lut[256]; /* look-up table */
} brightness_instance_t;

/* Updates the look-up-table. */
static void update_lut(brightness_instance_t *inst)
{
  int i;
  uint8_t *lut = inst->lut;
  int brightness = inst->brightness;

  if (brightness < 0)
  {
    for (i=0; i<256; ++i)
      lut[i] = CLAMP0255((i * (256 + brightness))>>8);
  }
  else
  {
    for (i=0; i<256; ++i)
      lut[i] = CLAMP0255(i + (((256 - i) * brightness)>>8));
  }
}

static int f0r_init()
{
  return 1;
}

static void f0r_deinit()
{ /* no initialization required */ }



static void f0r_get_param_info(f0r_param_info_t* info, int param_index)
{
  switch(param_index)
  {
  case 0:
    info->name = "Brightness";
    info->type = F0R_PARAM_DOUBLE;
    info->explanation = "The brightness value";
    break;
  }
}

static f0r_instance_t f0r_construct(unsigned int width, unsigned int height)
{
  brightness_instance_t* inst = (brightness_instance_t*)calloc(1, sizeof(*inst));
  inst->width = width; inst->height = height;
  /* init look-up-table */
  update_lut(inst);
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
  brightness_instance_t* inst = (brightness_instance_t*)instance;

  switch(param_index)
  {
    int val;
  case 0:
    /* constrast */
    val = (int) ((*((double*)param) - 0.5) * 512.0); /* remap to [-256, 256] */
    if (val != inst->brightness)
    {
      inst->brightness = val;
      update_lut(inst);
    }
    break;
  }
}

static void f0r_get_param_value(f0r_instance_t instance,
                         f0r_param_t param, int param_index)
{
  assert(instance);
  brightness_instance_t* inst = (brightness_instance_t*)instance;
  
  switch(param_index)
  {
  case 0:
    *((double*)param) = (double) ( (inst->brightness + 256.0) / 512.0 );
    break;
  }
}

static void f0r_update(f0r_instance_t instance, double time,
                const uint32_t* inframe, uint32_t* outframe)
{
  assert(instance);
  brightness_instance_t* inst = (brightness_instance_t*)instance;
  unsigned int len = inst->width * inst->height;
  
  uint8_t* lut = inst->lut;
  uint8_t* dst = (uint8_t*)outframe;
  const uint8_t* src = (uint8_t*)inframe;
  while (len--)
  {
    *dst++ = lut[*src++];
    *dst++ = lut[*src++];
    *dst++ = lut[*src++];
    *dst++ = *src++;// copy alpha
  }
}

//==================================================================================================
//export
static void f0r_get_plugin_info(f0r_plugin_info_t* brightness_info)
{

}

filter_dest(brightness,
	F0R_PLUGIN_TYPE_FILTER,
	F0R_COLOR_MODEL_RGBA8888,
	0,
	2,
	1,
	f0r_update,
	NULL);