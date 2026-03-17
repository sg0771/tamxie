#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <string.h>

#include "frei0r.h"
#include "matrix.h"

typedef struct hueshift0r_instance
{
  unsigned int width;
  unsigned int height;
  int hueshift; /* the shift [0, 360] */
  float mat[4][4];
} hueshift0r_instance_t;

/* Updates the shift matrix. */
void update_mat(hueshift0r_instance_t *inst)
{
  identmat((float*)inst->mat);
  huerotatemat(inst->mat, (float)inst->hueshift);
}

static int f0r_init()
{
  return 1;
}

static void f0r_deinit()
{ /* no initialization required */ }

static void f0r_get_plugin_info(f0r_plugin_info_t* info)
{
}

static void f0r_get_param_info(f0r_param_info_t* info, int param_index)
{
  switch(param_index)
  {
  case 0:
    info->name = "Hue";
    info->type = F0R_PARAM_DOUBLE;
    info->explanation = "The shift value";
    break;
  }
}

static f0r_instance_t f0r_construct(unsigned int width, unsigned int height)
{
  hueshift0r_instance_t* inst = (hueshift0r_instance_t*)calloc(1, sizeof(*inst));
  inst->width = width; inst->height = height;
  /* init transformation matrix */
  inst->hueshift = 0;
  update_mat(inst);
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
  hueshift0r_instance_t* inst = (hueshift0r_instance_t*)instance;

  switch(param_index)
  {
    int val;
  case 0:
    /* constrast */
    val = (int) (*((double*)param) * 360.0); /* remap to [0, 360] */
    if (val != inst->hueshift)
    {
      inst->hueshift = val;
      update_mat(inst);
    }
    break;
  }
}

static void f0r_get_param_value(f0r_instance_t instance,
                         f0r_param_t param, int param_index)
{
  assert(instance);
  hueshift0r_instance_t* inst = (hueshift0r_instance_t*)instance;
  
  switch(param_index)
  {
  case 0:
    *((double*)param) = (double) (inst->hueshift / 360.0);
    break;
  }
}

static void f0r_update(f0r_instance_t instance, double time,
                const uint32_t* inframe, uint32_t* outframe)
{
  assert(instance);
  hueshift0r_instance_t* inst = (hueshift0r_instance_t*)instance;
  unsigned int len = inst->width * inst->height;
  
  memcpy(outframe, inframe, len*sizeof(uint32_t));
  applymatrix((unsigned long*)outframe, inst->mat, len);
}



//==================================================================================================
//export
filter_dest(hueshift0r,
	F0R_PLUGIN_TYPE_FILTER,
	F0R_COLOR_MODEL_RGBA8888,
	0,
	3,
	1,
	f0r_update,
	NULL);