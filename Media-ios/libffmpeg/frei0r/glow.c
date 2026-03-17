#include <math.h>
#include "frei0r.h"
#include <stdlib.h>
#include "blur.h"

typedef struct glow_instance {
	double blur;
	int w, h;
	uint32_t* blurred;
	f0r_instance_t* blur_instance;
} glow_instance_t;

static int f0r_init()
{
  return 1;
}
static void f0r_deinit()
{ /* empty */ }

static void f0r_get_plugin_info( f0r_plugin_info_t* info )
{

}
static void f0r_get_param_info( f0r_param_info_t* info, int param_index )
{
	switch ( param_index ) {
		case 0:
			info->name = "Blur";
			info->type = F0R_PARAM_DOUBLE;
			info->explanation = "Blur of the glow";
			break;
	}
}

static f0r_instance_t f0r_construct(unsigned int width, unsigned int height)
{
	glow_instance_t* inst = (glow_instance_t*)calloc(1, sizeof(*inst));
	inst->w = width;
	inst->h = height;
	inst->blurred = (uint32_t*)malloc( width * height * sizeof(uint32_t) );
	inst->blur_instance = (f0r_instance_t *)blur_construct( width, height );
	blur_set_param_value(inst->blur_instance, &inst->blur, 0 );
	return (f0r_instance_t)inst;
}
static void f0r_destruct(f0r_instance_t instance)
{
	glow_instance_t* inst = (glow_instance_t*)instance;
	blur_destruct(inst->blur_instance);
	free(inst->blurred);
	free(instance);
}
static void f0r_set_param_value(f0r_instance_t instance, 
                         f0r_param_t param, int param_index)
{
	glow_instance_t* inst = (glow_instance_t*)instance;
	switch ( param_index ) {
		case 0:
			inst->blur = (*((double*)param)) / 20.0;
			blur_set_param_value(inst->blur_instance, &inst->blur, 0 );
			break;
	}
}
static void f0r_get_param_value(f0r_instance_t instance,
                         f0r_param_t param, int param_index)
{
	glow_instance_t* inst = (glow_instance_t*)instance;
	switch ( param_index ) {
		case 0:
			*((double*)param) = inst->blur * 20.0;
			break;
	}
}
static void f0r_update(f0r_instance_t instance, double time,
                const uint32_t* inframe, uint32_t* outframe)
{
	glow_instance_t* inst = (glow_instance_t*)instance;

	uint8_t* dst = (uint8_t*)outframe;
	const uint8_t* src = (uint8_t*)inframe;
	const uint8_t* blur = (uint8_t*)inst->blurred;

	int len = inst->w * inst->h * 4;

	blur_update(inst->blur_instance, 0.0, inframe, inst->blurred );
	
	int i;
	for ( i = 0; i < len; i++ ) {
		*dst = 255 - ( ( 255 - *src ) * ( 255 - *blur ) ) / 255;
		// 1 - ( ( 1 - A ) * ( 1 - B ) )
		dst++;
		src++;
		blur++;
	}

}



//==================================================================================================
//export
filter_dest(glow,
	F0R_PLUGIN_TYPE_FILTER,
	F0R_COLOR_MODEL_RGBA8888,
	0,
	1,
	1,
	f0r_update,
	NULL);