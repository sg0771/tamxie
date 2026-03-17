
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "frei0r.h"

typedef struct threshold0r_instance
{
  unsigned int width;
  unsigned int height;
  uint8_t threshold; /* the threshold [0, 255] */
  uint8_t lut[256]; /* look-up table */
} threshold0r_instance_t;

/* Updates the look-up-table. */
static void update_lut(threshold0r_instance_t *inst)
{
  int i;
  uint8_t *lut = inst->lut;
  uint8_t thresh = inst->threshold;
  if (thresh == 0xff)
    memset(lut, 0x00, 256*sizeof(uint8_t));
  else if (thresh == 0x00)
    memset(lut, 0xff, 256*sizeof(uint8_t));
  else
  {
    for (i=0; i<thresh; ++i)
      lut[i] = 0x00;
    for (i=thresh; i<256; ++i)
      lut[i] = 0xff;
  }
}

static int f0r_init()
{
  return 1;
}

static void f0r_deinit()
{ /* no initialization required */ }

static void f0r_get_plugin_info(f0r_plugin_info_t* threshold0r_info)
{
}

static void f0r_get_param_info(f0r_param_info_t* info, int param_index)
{
  switch(param_index)
  {
  case 0:
    info->name = "Threshold";
    info->type = F0R_PARAM_DOUBLE;
    info->explanation = "The threshold";
    break;
  }
}

static f0r_instance_t f0r_construct(unsigned int width, unsigned int height)
{
  threshold0r_instance_t* inst = (threshold0r_instance_t*)calloc(1, sizeof(*inst));
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
  threshold0r_instance_t* inst = (threshold0r_instance_t*)instance;

  switch(param_index)
  {
    uint8_t val;
  case 0:
    /* threshold */
    val = (uint8_t) (255.0 * *((double*)param));
    if (val != inst->threshold)
    {
      inst->threshold = val;
      update_lut(inst);
    }
    break;
  }
}

static void f0r_get_param_value(f0r_instance_t instance,
                         f0r_param_t param, int param_index)
{
  assert(instance);
  threshold0r_instance_t* inst = (threshold0r_instance_t*)instance;
  
  switch(param_index)
  {
  case 0:
    *((double*)param) = (double)(inst->threshold) / 255.0;
    break;
  }
}

static void f0r_update(f0r_instance_t instance, double time,
                const uint32_t* inframe, uint32_t* outframe)
{
  assert(instance);
  threshold0r_instance_t* inst = (threshold0r_instance_t*)instance;
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
filter_dest(threshold0r,
	F0R_PLUGIN_TYPE_FILTER,
	F0R_COLOR_MODEL_RGBA8888,
	0,
	2,
	1,
	f0r_update,
	NULL);

