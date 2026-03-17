#ifndef INCLUDED_FREI0R_H
#define INCLUDED_FREI0R_H

#include <inttypes.h>

#if defined(_MSC_VER)
#define _USE_MATH_DEFINES
#endif /* _MSC_VER */

typedef int boolean;

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif


#ifndef M_PI
#define M_PI       3.14159265358979323846
#endif

#ifndef M_PI_2
#define M_PI_2     1.57079632679489661923
#endif

#ifndef PI
#define PI         3.14159265358979
#endif


#ifndef pixelScale
#define pixelScale 255.9
#endif


#define FREI0R_MAJOR_VERSION 1
#define FREI0R_MINOR_VERSION 2

/** one input and one output */
#define F0R_PLUGIN_TYPE_FILTER 0

/** just one output */
#define F0R_PLUGIN_TYPE_SOURCE 1

/** two inputs and one output */
#define F0R_PLUGIN_TYPE_MIXER2 2

/** three inputs and one output */
#define F0R_PLUGIN_TYPE_MIXER3 3

#define F0R_COLOR_MODEL_BGRA8888 0
#define F0R_COLOR_MODEL_RGBA8888 1
#define F0R_COLOR_MODEL_PACKED32 2

#define F0R_PARAM_BOOL      0
#define F0R_PARAM_DOUBLE    1
#define F0R_PARAM_COLOR     2
#define F0R_PARAM_POSITION  3
#define F0R_PARAM_STRING  4

typedef double f0r_param_bool;
typedef double f0r_param_double;
typedef void* f0r_instance_t;
typedef void* f0r_param_t;
typedef char* f0r_param_string;




#ifdef __cplusplus
extern "C"{
#endif

typedef struct f0r_param_color {
	float r;
	float g;
	float b;
} f0r_param_color_t;

typedef struct f0r_param_position {
	double x;
	double y;
} f0r_param_position_t;

typedef struct f0r_param_info {
	const char* name;
	int type;
	const char* explanation;
} f0r_param_info_t;

typedef struct f0r_plugin_info_t f0r_plugin_info_t;
//add by tam
//plugin in the static library
typedef f0r_instance_t(*f0r_construct_f)(unsigned int width, unsigned int height);
typedef void(*f0r_destruct_f)(f0r_instance_t instance);

typedef void(*f0r_deinit_f)();
typedef int(*f0r_init_f)();

typedef void(*f0r_get_plugin_info_f)(f0r_plugin_info_t *info);
typedef void(*f0r_get_param_info_f)(f0r_param_info_t *info, int param_index);

typedef void(*f0r_update_f)(f0r_instance_t instance, double time, const uint32_t *inframe, uint32_t *outframe);
typedef void(*f0r_update2_f)(f0r_instance_t instance, double time, const uint32_t *inframe1, const uint32_t *inframe2, const uint32_t *inframe3, uint32_t *outframe);

typedef void(*f0r_set_param_value_f)(f0r_instance_t instance, f0r_param_t param, int param_index);
typedef void(*f0r_get_param_value_f)(f0r_instance_t instance, f0r_param_t param, int param_index);

struct f0r_plugin_info_t {

	//add by Tam
	f0r_construct_f       f_construct;
	f0r_destruct_f        f_destruct;
	f0r_init_f            f_init;
	f0r_deinit_f          f_deinit;
	f0r_get_plugin_info_f f_get_plugin_info;
	f0r_get_param_info_f  f_get_param_info;
	f0r_get_param_value_f f_get_param_value;
	f0r_set_param_value_f f_set_param_value;
	f0r_update_f          f_update;
	f0r_update2_f         f_update2;

	const char* name;    /**< The (short) name of the plugin                   */
	const char* author;  /**< The plugin author                                */
	const char* explanation; /**< An optional explanation string               */

	int plugin_type;    
	int color_model;     /**< The color model used                             */
	int frei0r_version;  /**< The frei0r major version this plugin is built for*/
	int major_version;   /**< The major version of the plugin                  */
	int minor_version;   /**< The minor version of the plugin                  */
	int num_params;      /**< The number of parameters of the plugin           */

};

#define  filter_dest(val,_type, _model,_major_version, _minor_version, _num_params, func_update, func_update2)  f0r_plugin_info_t _##val##Info={ \
.name = #val,  \
.author = "Tam.Xie",\
.explanation = "Frei0r filter build static by Tam.Xie filter_desc",\
.plugin_type = _type, \
.color_model = _model,\
.frei0r_version = FREI0R_MAJOR_VERSION, \
.major_version = _major_version, \
.minor_version = _minor_version,\
.num_params = _num_params,\
.f_construct = f0r_construct,\
.f_destruct = f0r_destruct, \
.f_init = f0r_init, \
.f_deinit = f0r_deinit, \
.f_get_plugin_info = f0r_get_plugin_info, \
.f_get_param_info = f0r_get_param_info, \
.f_get_param_value = f0r_set_param_value, \
.f_set_param_value = f0r_get_param_value,  \
.f_update  = func_update, \
.f_update2 = func_update2 }; 

#ifdef __cplusplus
};
#endif



#endif
