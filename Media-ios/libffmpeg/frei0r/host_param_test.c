#include <math.h>
#include "frei0r.h"
#include <stdlib.h>
#include <string.h>




typedef struct host_param_test_instance {
	double dvalue;
	boolean bvalue;
	f0r_param_color_t cvalue;
	f0r_param_position_t pvalue;
	char* svalue;
	int w, h;
} host_param_test_instance_t;

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
			info->name = "Double";
			info->type = F0R_PARAM_DOUBLE;
			info->explanation = "Explanation for Double";
			break;
		case 1:
			info->name = "Boolean";
			info->type = F0R_PARAM_BOOL;
			info->explanation = "Explanation for Boolean";
			break;
		case 2:
			info->name = "Color";
			info->type = F0R_PARAM_COLOR;
			info->explanation = "Explanation for Color";
			break;
		case 3:
			info->name = "Position";
			info->type = F0R_PARAM_POSITION;
			info->explanation = "Explanation for Position";
			break;
		case 4:
			info->name = "String";
			info->type = F0R_PARAM_STRING;
			info->explanation = "Explanation for String";
			break;
	}
}

static f0r_instance_t f0r_construct(unsigned int width, unsigned int height)
{
	host_param_test_instance_t* inst = (host_param_test_instance_t*)calloc(1, sizeof(*inst));
	inst->w = width;
	inst->h = height;

	inst->dvalue = 0.5;
	inst->bvalue = FALSE;
	inst->cvalue.r = 0.5;
	inst->cvalue.g = 0.5;
	inst->cvalue.b = 0.5;
	inst->pvalue.x = 0.0;
	inst->pvalue.y = 0.0;
	const char* sval = "Hello";
	inst->svalue = (char*)malloc( strlen(sval) + 1 );
	strcpy( inst->svalue, sval );
	return (f0r_instance_t)inst;
}
static void f0r_destruct(f0r_instance_t instance)
{
	host_param_test_instance_t* inst = (host_param_test_instance_t*)instance;
	free(inst->svalue);
	free(instance);
}
static void f0r_set_param_value(f0r_instance_t instance, 
                         f0r_param_t param, int param_index)
{
	host_param_test_instance_t* inst = (host_param_test_instance_t*)instance;
	switch ( param_index ) {
		case 0:
			inst->dvalue = *((double*)param);
			break;
		case 1:
			inst->bvalue = (*((double*)param)) >= 0.5;
			break;
		case 2:
			inst->cvalue = *((f0r_param_color_t*)param);
			break;
		case 3:
			inst->pvalue = *((f0r_param_position_t*)param);
			break;
		case 4:
		{
			char* sval = (*(char**)param);
			inst->svalue = (char*)realloc( inst->svalue, strlen(sval) + 1 );
			strcpy( inst->svalue, sval );
			break;
		}
	}
}
static void f0r_get_param_value(f0r_instance_t instance,
                         f0r_param_t param, int param_index)
{
	host_param_test_instance_t* inst = (host_param_test_instance_t*)instance;
	switch ( param_index ) {
		case 0:
			*((double*)param) = inst->dvalue;
			break;
		case 1:
			*((double*)param) = (double)inst->bvalue;
			break;
		case 2:
			*((f0r_param_color_t*)param) = inst->cvalue;
			break;
		case 3:
			*((f0r_param_position_t*)param) = inst->pvalue;
			break;
		case 4:
			*((char**)param) = inst->svalue;
			break;
	}
}
static void f0r_update(f0r_instance_t instance, double time,
                const uint32_t* inframe, uint32_t* outframe)
{
	host_param_test_instance_t* inst = (host_param_test_instance_t*)instance;

	uint32_t* dst = outframe;
	const uint32_t* src = inframe;

	int len = inst->w * inst->h;
	
	int i;
	for ( i = 0; i < len; i++ ) {
		*dst = *src;
		dst++;
		src++;
	}

}



//==================================================================================================
//export
filter_dest(host_param_test,
	F0R_PLUGIN_TYPE_FILTER,
	F0R_COLOR_MODEL_RGBA8888,
	0,
	1,
	5,
	f0r_update,
	NULL);