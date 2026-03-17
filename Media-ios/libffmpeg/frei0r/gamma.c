#include <stdlib.h>
#include <math.h>
#include <assert.h>

#include "frei0r.h"
#include "frei0r_math.h"

#define MAX_GAMMA 4.0

typedef struct gamma_instance
{
  unsigned int width;
  unsigned int height;
  double gamma; /* the gamma value [0, 1] */
  uint8_t lut[256]; /* look-up table */
} gamma_instance_t;

/* Updates the look-up-table. */
static void update_lut(gamma_instance_t *inst)
{
  int i;
  uint8_t *lut = inst->lut;
  double inv_gamma = 1.0 / (inst->gamma * MAX_GAMMA); /* set gamma in the range [0,MAX_GAMMA] and take its inverse */

  lut[0] = 0;
  for (i=1; i<256; ++i)
    lut[i] = CLAMP0255( ROUND(255.0 * pow( (double)i / 255.0, inv_gamma ) ) );
}

static int f0r_init()
{
  return 1;
}

static void f0r_deinit()
{ /* no initialization required */ }

static void f0r_get_plugin_info(f0r_plugin_info_t* gamma_info)
{

}

static void f0r_get_param_info(f0r_param_info_t* info, int param_index)
{
  switch(param_index)
  {
  case 0:
    info->name = "Gamma";
    info->type = F0R_PARAM_DOUBLE;
    info->explanation = "The gamma value";
    break;
  }
}

static f0r_instance_t f0r_construct(unsigned int width, unsigned int height)
{
  gamma_instance_t* inst = (gamma_instance_t*)calloc(1, sizeof(*inst));
  inst->width = width; inst->height = height;
  /* init look-up-table */
  inst->gamma = 1.0;
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
  gamma_instance_t* inst = (gamma_instance_t*)instance;

  switch(param_index)
  {
    double val;
  case 0:
    /* gamma */
    val = *((double*)param);
    if (val != inst->gamma)
    {
      inst->gamma = val;
      update_lut(inst);
    }
    break;
  }
}

static void f0r_get_param_value(f0r_instance_t instance,
                         f0r_param_t param, int param_index)
{
  assert(instance);
  gamma_instance_t* inst = (gamma_instance_t*)instance;
  
  switch(param_index)
  {
  case 0:
    *((double*)param) = inst->gamma;
    break;
  }
}

static void f0r_update(f0r_instance_t instance, double time,
                const uint32_t* inframe, uint32_t* outframe)
{
  assert(instance);
  gamma_instance_t* inst = (gamma_instance_t*)instance;
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
filter_dest(gamma,
	F0R_PLUGIN_TYPE_FILTER,
	F0R_COLOR_MODEL_RGBA8888,
	0,
	2,
	1,
	f0r_update,
	NULL);

