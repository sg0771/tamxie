#include <stdlib.h>
#include <assert.h>

#include "frei0r.h"
#include "frei0r_math.h"

#define GIMP_RGB_LUMINANCE_RED    (0.2126)
#define GIMP_RGB_LUMINANCE_GREEN  (0.7152)
#define GIMP_RGB_LUMINANCE_BLUE   (0.0722)

#define GIMP_RGB_LUMINANCE(r,g,b) ((r) * GIMP_RGB_LUMINANCE_RED   + \
                                   (g) * GIMP_RGB_LUMINANCE_GREEN + \
                                   (b) * GIMP_RGB_LUMINANCE_BLUE)

typedef struct colorize_instance
{
  unsigned int width;
  unsigned int height;
  double hue;
  double saturation;
  double lightness;
} colorize_instance_t;

typedef struct _GimpRGB  GimpRGB;
typedef struct _GimpHSL  GimpHSL;

struct _GimpRGB
{
  double r, g, b, a;
};

struct _GimpHSL
{
  double h, s, l, a;
};

static inline double
gimp_hsl_value (double n1,
                double n2,
                double hue)
{
  double val;

  if (hue > 6.0)
    hue -= 6.0;
  else if (hue < 0.0)
    hue += 6.0;

  if (hue < 1.0)
    val = n1 + (n2 - n1) * hue;
  else if (hue < 3.0)
    val = n2;
  else if (hue < 4.0)
    val = n1 + (n2 - n1) * (4.0 - hue);
  else
    val = n1;

  return val;
}

static inline void
gimp_hsl_to_rgb (const GimpHSL *hsl,
                 GimpRGB       *rgb)
{
  if (hsl->s == 0)
    {
      /*  achromatic case  */
      rgb->r = hsl->l;
      rgb->g = hsl->l;
      rgb->b = hsl->l;
    }
  else
    {
      double m1, m2;

      if (hsl->l <= 0.5)
        m2 = hsl->l * (1.0 + hsl->s);
      else
        m2 = hsl->l + hsl->s - hsl->l * hsl->s;

      m1 = 2.0 * hsl->l - m2;

      rgb->r = gimp_hsl_value (m1, m2, hsl->h * 6.0 + 2.0);
      rgb->g = gimp_hsl_value (m1, m2, hsl->h * 6.0);
      rgb->b = gimp_hsl_value (m1, m2, hsl->h * 6.0 - 2.0);
    }

  rgb->a = hsl->a;
}


static int f0r_init()
{
  return 1;
}

static void f0r_deinit()
{ /* no initialization required */ }

static void f0r_get_plugin_info(f0r_plugin_info_t* colorize_info)
{
}

static void f0r_get_param_info(f0r_param_info_t* info, int param_index)
{
  switch(param_index)
  {
  case 0:
    info->name = "hue"; 
    info->type = F0R_PARAM_DOUBLE;
    info->explanation = "Color shade of the colorized image";
    break;
  case 1:
    info->name = "saturation"; 
    info->type = F0R_PARAM_DOUBLE;
    info->explanation = "Amount of color in the colorized image";
    break;
  case 2:
    info->name = "lightness"; 
    info->type = F0R_PARAM_DOUBLE;
    info->explanation = "Lightness of the colorized image";
    break;
  }
}

static f0r_instance_t f0r_construct(unsigned int width, unsigned int height)
{
	colorize_instance_t* inst = (colorize_instance_t*)calloc(1, sizeof(*inst));
	inst->width = width; 
  inst->height = height;
	inst->hue = 0.5;
	inst->saturation = 0.5;
	inst->lightness = 0.5;
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
  colorize_instance_t* inst = (colorize_instance_t*)instance;

  switch(param_index)
  {
  case 0:
    inst->hue = *((double*)param);
    break;
  case 1:
    inst->saturation = *((double*)param);
    break;
  case 2:
    inst->lightness = *((double*)param);
    break;
  }
}

static void f0r_get_param_value(f0r_instance_t instance,
                         f0r_param_t param, int param_index)
{
  assert(instance);
  colorize_instance_t* inst = (colorize_instance_t*)instance;
  
  switch(param_index)
  {
  case 0:
    *((double*)param) = inst->hue;
    break;
  case 1:
    *((double*)param) = inst->saturation;
    break;
  case 2:
    *((double*)param) = inst->lightness;
    break;
  }
}

static void f0r_update(f0r_instance_t instance, double time,
                const uint32_t* inframe, uint32_t* outframe)
{
  assert(instance);
  colorize_instance_t* inst = (colorize_instance_t*)instance;
  unsigned int len = inst->width * inst->height;
  
  GimpHSL hsl;
  GimpRGB rgb;

  hsl.h = inst->hue;
  hsl.s = inst->saturation;

  double lightness = inst->lightness - 0.5;

  uint8_t* dst = (uint8_t*)outframe;
  const uint8_t* src = (uint8_t*)inframe;
  double lum, r, g, b = 0;
  while (len--)
  {
    r = *src++ / 255.0;
    g = *src++ / 255.0;
    b = *src++ / 255.0;

    lum = GIMP_RGB_LUMINANCE (r, g, b);

    if (lightness > 0)
    {
      lum = lum * (1.0 - lightness);
      lum += 1.0 - (1.0 - lightness);
    }
    else if (lightness < 0)
    {
      lum = lum * (lightness + 1.0);
    }

    hsl.l = lum;
    gimp_hsl_to_rgb (&hsl, &rgb);

    *dst++ = (uint8_t) (rgb.r * 255.0);
    *dst++ = (uint8_t) (rgb.g * 255.0);
    *dst++ = (uint8_t) (rgb.b * 255.0);
    *dst++ = *src++;//copy alpha
  }
}


//==================================================================================================
//export
filter_dest(colorize,
	F0R_PLUGIN_TYPE_FILTER,
	F0R_COLOR_MODEL_RGBA8888,
	0,
	1,
	3,
	f0r_update,
	NULL);