#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

#include "frei0r.h"
#include "frei0r_math.h"

typedef struct contrast0r_instance
{
  unsigned int width;
  unsigned int height;
  int contrast; /* the contrast [-256, 256] */
  uint8_t lut[256]; /* look-up table */
} contrast0r_instance_t;

/* Updates the look-up-table. */
static void update_lut(contrast0r_instance_t *inst)
{
  int i;
  uint8_t *lut = inst->lut;
  int contrast = inst->contrast;
  for (i=0; i<128; ++i)
    lut[i] = CLAMP0255(i - (((128 - i)*contrast)>>8));
  for (i=128; i<256; ++i)
    lut[i] = CLAMP0255(i + (((i - 128)*contrast)>>8));
}

static int f0r_init()
{
  return 1;
}

static void f0r_deinit()
{ /* no initialization required */ }

static void f0r_get_plugin_info(f0r_plugin_info_t* contrast0r_info)
{
}

static void f0r_get_param_info(f0r_param_info_t* info, int param_index)
{
  switch(param_index)
  {
  case 0:
    info->name = "Contrast";
    info->type = F0R_PARAM_DOUBLE;
    info->explanation = "The contrast value";
    break;
  }
}

static f0r_instance_t f0r_construct(unsigned int width, unsigned int height)
{
  contrast0r_instance_t* inst = (contrast0r_instance_t*)calloc(1, sizeof(*inst));
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
  contrast0r_instance_t* inst = (contrast0r_instance_t*)instance;
  switch(param_index)
  {
    int val;
  case 0:
    /* constrast */
    val = (int) ((*((double*)param) - 0.5) * 512.0); /* remap to [-256, 256] */
    if (val != inst->contrast)
    {
      inst->contrast = val;
      update_lut(inst);
    }
    break;
  }
}

static void f0r_get_param_value(f0r_instance_t instance,
                         f0r_param_t param, int param_index)
{
  assert(instance);
  contrast0r_instance_t* inst = (contrast0r_instance_t*)instance;
  
  switch(param_index)
  {
  case 0:
    *((double*)param) = (double) ( (inst->contrast + 256.0) / 512.0 );
    break;
  }
}

static void f0r_update(f0r_instance_t instance, double time,
                const uint32_t* inframe, uint32_t* outframe)
{
  assert(instance);
  contrast0r_instance_t* inst = (contrast0r_instance_t*)instance;
  unsigned int len = inst->width * inst->height;
  
  uint8_t* lut = inst->lut;
  uint8_t* dst = (uint8_t*)outframe;
  const uint8_t* src = (uint8_t*)inframe;
  while (len--)
  {
    *dst++ = lut[*src++];
    *dst++ = lut[*src++];
    *dst++ = lut[*src++];
    *dst++ = *src++; // copy alpha
  }
}


//==================================================================================================
//export
filter_dest(contrast0r,
	F0R_PLUGIN_TYPE_FILTER,
	F0R_COLOR_MODEL_RGBA8888,
	0,
	2,
	1,
	f0r_update,
	NULL);