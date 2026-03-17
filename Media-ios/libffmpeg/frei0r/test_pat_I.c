#include <stdlib.h>
#include <assert.h>
#include <math.h>

#include "frei0r.h"

typedef struct
	{
	float r;
	float g;
	float b;
	float a;
	} float_rgba;


//----------------------------------------------------------
static void draw_rectangle(float *sl, int w, int h, int x, int y, int wr, int hr, float gray)
{
int i,j;
int zx,kx,zy,ky;

zx=x;  if (zx<0) zx=0;
zy=y;  if (zy<0) zy=0;
kx=x+wr;  if (kx>w) kx=w;
ky=y+hr;  if (ky>h) ky=h;
for (i=zy;i<ky;i++)
	for (j=zx;j<kx;j++)
		sl[w*i+j]=gray;

}

//----------------------------------------------------
//pravokotna pika
static void pika_p(float *sl, int w, int h, float size, float amp)
{
int i;

for (i=0;i<w*h;i++) sl[i]=0.5-amp/2.0;	//background
draw_rectangle(sl, w, h, w/2-size/2, h/2-size/2, size, size, 0.5+amp/2.0);

}

//----------------------------------------------------
//okrogla pika  (raised cos)
static void pika_o(float *sl, int w, int h, float size, float amp)
{
int i,j;
float x,y,r,g;

for (i=0;i<w*h;i++) sl[i]=0.5-amp/2.0;	//background

for (i=0;i<size;i++)
	for (j=0;j<size;j++)
		{
		x=(float)j-size/2.0+0.5;
		y=(float)i-size/2.0+0.5;
		r=sqrtf(x*x+y*y);
		if (r>size/2.0) r=size/2.0;
		g=0.5+amp/2.0*cosf(r/size*2.0*PI);
		sl[(i+h/2-(int)size/2)*w+j+w/2-(int)size/2]=g;
		}

}

//----------------------------------------------------
//crta pravokotna
static void crta_p(float *sl, int w, int h, float size, float amp, float tilt)
{
int i,j;
float d,st,ct;

st=sinf(tilt);
ct=cosf(tilt);
for (i=0;i<h;i++)
	for (j=0;j<w;j++)
		{
		d=(i-h/2)*ct+(j-w/2)*st;
		if (fabsf(d)>size/2.0)
			{
			sl[i*w+j]=0.5-amp/2.0;
			}
		else
			{
			sl[i*w+j]=0.5+amp/2.0;
			}
		}

}

//----------------------------------------------------
//crta   raised cos
static void crta(float *sl, int w, int h, float size, float amp, float tilt)
{
int i,j;
float d,st,ct,g;

if (size==0.0) return;
st=sinf(tilt);
ct=cosf(tilt);
for (i=0;i<h;i++)
	for (j=0;j<w;j++)
		{
		d=(i-h/2)*ct+(j-w/2)*st;
		if (fabsf(d)>size/2.0)
			{
			sl[i*w+j]=0.5-amp/2.0;
			}
		else
			{
			if (d>size/2.0) d=size/2.0;
			g=0.5+amp/2.0*cosf(d/size*2.0*PI);
			sl[i*w+j]=g;
			}
		}

}

//----------------------------------------------------
//crta step  raised cos, oz. pravokotna, ce das size=1
static void crta_s(float *sl, int w, int h, float size, float amp, float tilt)
{
int i,j;
float d,st,ct,g;

if (size==0.0) return;
st=sinf(tilt);
ct=cosf(tilt);
for (i=0;i<h;i++)
	for (j=0;j<w;j++)
		{
		d=(i-h/2)*ct+(j-w/2)*st;
		if (fabsf(d)>size/2.0)
			{
			if (d>0.0)
				sl[i*w+j]=0.5-amp/2.0;
			else
				sl[i*w+j]=0.5+amp/2.0;
			}
		else
			{
			if (d>size/2.0) d=size/2.0;
			g=0.5-amp/2.0*sinf(d/size*PI);
			sl[i*w+j]=g;
			}
		}

}

//----------------------------------------------------
//crta step  linear ramp, oz. pravokotna, ce das size=1
void crta_r(float *sl, int w, int h, float size, float amp, float tilt)
{
int i,j;
float d,st,ct,g;

if (size==0.0) return;
st=sinf(tilt);
ct=cosf(tilt);
for (i=0;i<h;i++)
	for (j=0;j<w;j++)
		{
		d=(i-h/2)*ct+(j-w/2)*st;
		if (fabsf(d)>size/2.0)
			{
			if (d>0.0)
				sl[i*w+j]=0.5-amp/2.0;
			else
				sl[i*w+j]=0.5+amp/2.0;
			}
		else
			{
			if (d>size/2.0) d=size/2.0;
			g = 0.5-amp*(d/size);
			sl[i*w+j]=g;
			}
		}

}

//-----------------------------------------------------
//converts the internal monochrome float image into
//Frei0r rgba8888 color
//ch selects the channel   0=all  1=R  2=G  3=B
//sets alpha to opaque
static void float2color(float *sl, uint32_t* outframe, int w , int h, int ch)
{
int i,ri,gi,bi;
uint32_t p;
float r,g,b;

switch (ch)
	{
	case 0:		//all (gray)
		for (i=0;i<w*h;i++)
			{
			p=(uint32_t)(255.0*sl[i]) & 0xFF;
			outframe[i] = (p<<16)+(p<<8)+p+0xFF000000;
			}
		break;
	case 1:		//R
		for (i=0;i<w*h;i++)
			{
			p=(uint32_t)(255.0*sl[i]) & 0xFF;
			outframe[i] = p+0xFF000000;
			}
		break;
	case 2:		//G
		for (i=0;i<w*h;i++)
			{
			p=(uint32_t)(255.0*sl[i]) & 0xFF;
			outframe[i] = (p<<8)+0xFF000000;
			}
		break;
	case 3:		//B
		for (i=0;i<w*h;i++)
			{
			p=(uint32_t)(255.0*sl[i]) & 0xFF;
			outframe[i] = (p<<16)+0xFF000000;
			}
		break;
	case 4:		//ccir rec 601  R-Y   on 50 gray
		for (i=0;i<w*h;i++)
			{
			r=sl[i];
			b=0.5;
			g=(0.5-0.299*r-0.114*b)/0.587;
			ri=(int)(255.0*r);
			gi=(int)(255.0*g);
			bi=(int)(255.0*b);
			outframe[i] = (bi<<16)+(gi<<8)+ri+0xFF000000;
			}
		break;
	case 5:		//ccir rec 601  B-Y   on 50% gray
		for (i=0;i<w*h;i++)
			{
			b=sl[i];
			r=0.5;
			g=(0.5-0.299*r-0.114*b)/0.587;
			ri=(int)(255.0*r);
			gi=(int)(255.0*g);
			bi=(int)(255.0*b);
			outframe[i] = (bi<<16)+(gi<<8)+ri+0xFF000000;
			}
		break;
	case 6:		//ccir rec 709  R-Y   on 50 gray
		for (i=0;i<w*h;i++)
			{
			r=sl[i];
			b=0.5;
			g=(0.5-0.2126*r-0.0722*b)/0.7152;
			ri=(int)(255.0*r);
			gi=(int)(255.0*g);
			bi=(int)(255.0*b);
			outframe[i] = (bi<<16)+(gi<<8)+ri+0xFF000000;
			}
		break;
	case 7:		//ccir rec 709  B-Y   on 50% gray
		for (i=0;i<w*h;i++)
			{
			b=sl[i];
			r=0.5;
			g=(0.5-0.2126*r-0.0722*b)/0.7152;
			ri=(int)(255.0*r);
			gi=(int)(255.0*g);
			bi=(int)(255.0*b);
			outframe[i] = (bi<<16)+(gi<<8)+ri+0xFF000000;
			}
		break;
	default:
		break;
	}

}

//-----------------------------------------------------
//stretch [0...1] to parameter range [min...max] linear
static float map_value_forward(double v, float min, float max)
{
return min+(max-min)*v;
}

//-----------------------------------------------------
//collapse from parameter range [min...max] to [0...1] linear
static double map_value_backward(float v, float min, float max)
{
return (v-min)/(max-min);
}

//-----------------------------------------------------
//stretch [0...1] to parameter range [min...max] logarithmic
//min and max must be positive!
static float map_value_forward_log(double v, float min, float max)
{
float sr,k;

sr=sqrtf(min*max);
k=2.0*log(max/sr);
return sr*expf(k*(v-0.5));
}

//-----------------------------------------------------
//collapse from parameter range [min...max] to [0...1] logarithmic
//min and max must be positive!
static double map_value_backward_log(float v, float min, float max)
{
float sr,k;

sr=sqrtf(min*max);
k=2.0*log(max/sr);
return logf(v/sr)/k+0.5;
}

//**************************************************
//obligatory frei0r stuff follows

//------------------------------------------------
//this structure holds an instance of the test_pat_I plugin
typedef struct
{
  unsigned int w;
  unsigned int h;

  int type;
  int chan;
  float amp;
  float pw;
  float tilt;
  int neg;

  float *sl;

} tp_inst_t;

//----------------------------------------------------
static int f0r_init()
{
  return 1;
}

//--------------------------------------------------
static void f0r_deinit()
{ /* no initialization required */ }

//--------------------------------------------------
static void f0r_get_plugin_info(f0r_plugin_info_t* tp_info)
{
}

//--------------------------------------------------
static void f0r_get_param_info(f0r_param_info_t* info, int param_index)
{
  switch (param_index)
    {
    case 0:
      info->name        = "Type";
      info->type        = F0R_PARAM_DOUBLE;
      info->explanation = "Type of test pattern"; break;
    case 1:
      info->name	="Channel";
      info->type	= F0R_PARAM_DOUBLE;
      info->explanation = "Into which color channel to draw";
      break;
    case 2:
      info->name	= "Amplitude";
      info->type	= F0R_PARAM_DOUBLE;
      info->explanation = "Amplitude (contrast) of the pattern";
      break;
    case 3:
      info->name        = "Width";
      info->type        = F0R_PARAM_DOUBLE;
      info->explanation = "Width of impulse";
      break;
    case 4:
      info->name	= "Tilt";
      info->type	= F0R_PARAM_DOUBLE;
      info->explanation = "Angle of step function";
      break;
    case 5:
      info->name	= "Negative";
      info->type	= F0R_PARAM_BOOL;
      info->explanation = "Change polarity of impulse/step";
      break;
    }
}

//--------------------------------------------------
static f0r_instance_t f0r_construct(unsigned int width, unsigned int height)
{
  tp_inst_t* inst = calloc(1, sizeof(*inst));
  inst->w  = width; 
  inst->h = height;

  inst->type=0;
  inst->chan=0;
  inst->amp=0.8;
  inst->pw=5.0;
  inst->tilt=0.0;
  inst->neg=0;

  inst->sl=(float*)calloc(width*height,sizeof(float));

  pika_p(inst->sl, inst->w, inst->h, inst->pw, inst->amp);

  return (f0r_instance_t)inst;
}

//--------------------------------------------------
static void f0r_destruct(f0r_instance_t instance)
{
  tp_inst_t* inst = (tp_inst_t*)instance;

  free(inst->sl);
  free(inst);
}

//--------------------------------------------------
static void f0r_set_param_value(f0r_instance_t instance, f0r_param_t param, int param_index)
{
  tp_inst_t* inst = (tp_inst_t*)instance;

  f0r_param_double* p = (f0r_param_double*) param;

  int chg,tmpi;
  float tmpf;

  chg=0;
  switch (param_index)
    {
    case 0:	//type
      tmpf=*((double*)p);
      if (tmpf>=1.0)
        tmpi=(int)tmpf;
      else
        tmpi = map_value_forward(tmpf, 0.0, 5.9999);
      if ((tmpi<0)||(tmpi>5.0)) break;
      if (inst->type != tmpi) chg=1;
      inst->type = tmpi;
      break;
    case 1:	//channel
      tmpf=*((double*)p);
      if (tmpf>=1.0)
        tmpi=(int)tmpf;
      else
        tmpi = map_value_forward(tmpf, 0.0, 7.9999);
      if ((tmpi<0)||(tmpi>7.0)) break;
      if (inst->chan != tmpi) chg=1;
      inst->chan = tmpi;
    case 2:	//amplitude
      tmpf = map_value_forward(*((double*)p), 0.0, 1.0);
      if (inst->amp != tmpf) chg=1;
      inst->amp = tmpf;
      break;
    case 3:	//width
      tmpf = map_value_forward(*((double*)p), 1.0, 100.0);
      if (inst->pw != tmpf) chg=1;
      inst->pw = tmpf;
      break;
    case 4:	//tilt
      tmpf = map_value_forward(*((double*)p), -PI/2.0, PI/2.0);
      if (inst->tilt != tmpf) chg=1;
      inst->tilt = tmpf;
      break;
    case 5:	//negative
      tmpi = map_value_forward(*((double*)p), 0.0, 1.0);
      if (inst->neg != tmpi) chg=1;
      inst->neg = tmpi;
      break;
    }

  if (chg==0) return;

  switch (inst->type)
    {
    case 0:		 //
      pika_p(inst->sl, inst->w, inst->h, inst->pw, inst->amp);
      break;
    case 1:		 //
      pika_o(inst->sl, inst->w, inst->h, inst->pw, inst->amp);
      break;
    case 2:		 //
      crta_p(inst->sl, inst->w, inst->h, inst->pw, inst->amp, inst->tilt);
      break;
    case 3:		 //
      crta(inst->sl, inst->w, inst->h, inst->pw, inst->amp, inst->tilt);
      break;
    case 4:		//
      crta_s(inst->sl, inst->w, inst->h, inst->pw, inst->amp, inst->tilt);
      break;
    case 5:		//
      crta_r(inst->sl, inst->w, inst->h, inst->pw, inst->amp, inst->tilt);
      break;
    default:
      break;
    }

}

//-------------------------------------------------
static void f0r_get_param_value(f0r_instance_t instance, f0r_param_t param, int param_index)
{
  tp_inst_t* inst = (tp_inst_t*)instance;

  f0r_param_double* p = (f0r_param_double*) param;

  switch (param_index)
    {
    case 0:	//type
      *p = map_value_backward(inst->type, 0.0, 5.9999);
      break;
    case 1:	//channel
      *p = map_value_backward(inst->chan, 0.0, 7.9999);
      break;
    case 2:	//amplitude
      *p = map_value_backward(inst->amp, 0.0, 1.0);
      break;
    case 3:	//width
      *p = map_value_backward(inst->pw, 1.0, 100.0);
      break;
    case 4:	//tilt
      *p = map_value_backward(inst->tilt, -PI/2.0, PI/2.0);
      break;
    case 5:	//negative
      *p = map_value_backward(inst->neg, 0.0, 1.0);
      break;
    }
}

//---------------------------------------------------
static void f0r_update(f0r_instance_t instance, double time, const uint32_t* inframe, uint32_t* outframe)
{

  assert(instance);
  tp_inst_t* inst = (tp_inst_t*)instance;

  float2color(inst->sl, outframe, inst->w , inst->h, inst->chan);

}


//==================================================================================================
//export
filter_dest(test_pat_I,
	F0R_PLUGIN_TYPE_SOURCE,
	F0R_COLOR_MODEL_RGBA8888,
	0,
	2,
	6,
	f0r_update,
	NULL);
