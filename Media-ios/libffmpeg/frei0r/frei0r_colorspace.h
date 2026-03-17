#ifndef INCLUDED_FREI0R_COLORSPACE_H
#define INCLUDED_FREI0R_COLORSPACE_H

#include "frei0r_math.h"
#include <stdlib.h>
#include <math.h>

inline void rgb_to_hsv_int (int *red,int *green,int *blue )
{
  double  r, g, b;
  double  h, s, v;
  double  min;
  double  delta;

  r = *red;
  g = *green;
  b = *blue;

  if (r > g)
  {
    v = MAX (r, b);
    min = MIN (g, b);
  }
  else
  {
    v = MAX (g, b);
    min = MIN (r, b);
  }

  delta = v - min;

  if (v == 0.0)
    s = 0.0;
  else
    s = delta / v;

  if (s == 0.0)
    h = 0.0;
  else
  {
    if (r == v)
      h = 60.0 * (g - b) / delta;
    else if (g == v)
      h = 120 + 60.0 * (b - r) / delta;
    else
      h = 240 + 60.0 * (r - g) / delta;

    if (h < 0.0)
      h += 360.0;
    if (h > 360.0)
      h -= 360.0;
  }

  *red   = ROUND (h);
  *green = ROUND (s * 255.0);
  *blue  = ROUND (v);  
}

inline void hsv_to_rgb_int (int *hue,int *saturation,int *value){
  double h, s, v, h_temp;
  double f, p, q, t;
  int i;

  if (*saturation == 0)
  {
    *hue        = *value;
    *saturation = *value;
    //    *value      = *value;
  }
  else
  {
    h = *hue;
    s = *saturation / 255.0;
    v = *value      / 255.0;

    if (h == 360)
      h_temp = 0;
    else
      h_temp = h;

    h_temp = h_temp / 60.0;
    i = (int) floor (h_temp);
    f = h_temp - i;
    p = v * (1.0 - s);
    q = v * (1.0 - (s * f));
    t = v * (1.0 - (s * (1.0 - f)));

    switch (i)
    {
    case 0:
      *hue        = ROUND (v * 255.0);
      *saturation = ROUND (t * 255.0);
      *value      = ROUND (p * 255.0);
      break;

    case 1:
      *hue        = ROUND (q * 255.0);
      *saturation = ROUND (v * 255.0);
      *value      = ROUND (p * 255.0);
      break;

    case 2:
      *hue        = ROUND (p * 255.0);
      *saturation = ROUND (v * 255.0);
      *value      = ROUND (t * 255.0);
      break;

    case 3:
      *hue        = ROUND (p * 255.0);
      *saturation = ROUND (q * 255.0);
      *value      = ROUND (v * 255.0);
      break;

    case 4:
      *hue        = ROUND (t * 255.0);
      *saturation = ROUND (p * 255.0);
      *value      = ROUND (v * 255.0);
      break;

    case 5:
      *hue        = ROUND (v * 255.0);
      *saturation = ROUND (p * 255.0);
      *value      = ROUND (q * 255.0);
      break;
    }
  }
}

inline void rgb_to_hsl_int (unsigned int *red, unsigned int *green,unsigned int *blue)
{
  unsigned int r, g, b;
  double h, s, l;
  unsigned int    min, max;
  unsigned int    delta;

  r = *red;
  g = *green;
  b = *blue;

  if (r > g)
  {
    max = MAX (r, b);
    min = MIN (g, b);
  }
  else
  {
    max = MAX (g, b);
    min = MIN (r, b);
  }

  l = (max + min) / 2.0;

  if (max == min)
  {
    s = 0.0;
    h = 0.0;
  }
  else
  {
    delta = (max - min);

    if (l < 128)
      s = 255 * (double) delta / (double) (max + min);
    else
      s = 255 * (double) delta / (double) (511 - max - min);

    if (r == max)
      h = (g - b) / (double) delta;
    else if (g == max)
      h = 2 + (b - r) / (double) delta;
    else
      h = 4 + (r - g) / (double) delta;

    h = h * 42.5;

    if (h < 0)
      h += 255;
    else if (h > 255)
      h -= 255;
  }

  *red   = ROUND (h);
  *green = ROUND (s);
  *blue  = ROUND (l);
}

inline int
hsl_value_int (double n1,
               double n2,
               double hue)
{
  double value;

  if (hue > 255)
    hue -= 255;
  else if (hue < 0)
    hue += 255;

  if (hue < 42.5)
    value = n1 + (n2 - n1) * (hue / 42.5);
  else if (hue < 127.5)
    value = n2;
  else if (hue < 170)
    value = n1 + (n2 - n1) * ((170 - hue) / 42.5);
  else
    value = n1;

  return ROUND (value * 255.0);
}

inline void hsl_to_rgb_int (unsigned int *hue, unsigned int *saturation, unsigned int *lightness)
{
  double h, s, l;

  h = *hue;
  s = *saturation;
  l = *lightness;

  if (s == 0)
  {
    /*  achromatic case  */
    *hue        = (int)l;
    *lightness  = (int)l;
    *saturation = (int)l;
  }
  else
  {
    double m1, m2;

    if (l < 128)
      m2 = (l * (255 + s)) / 65025.0;
    else
      m2 = (l + s - (l * s) / 255.0) / 255.0;

    m1 = (l / 127.5) - m2;

    /*  chromatic case  */
    *hue        = hsl_value_int (m1, m2, h + 85);
    *saturation = hsl_value_int (m1, m2, h);
    *lightness  = hsl_value_int (m1, m2, h - 85);
  }
}

inline void gimp_rgb_to_cmyk_int (int *red, int *green, int *blue,  int *pullout)
{
  int c, m, y;

  c = 255 - *red;
  m = 255 - *green;
  y = 255 - *blue;

  if (*pullout == 0)
    {
      *red   = c;
      *green = m;
      *blue  = y;
    }
  else
    {
      int k = 255;

      if (c < k)  k = c;
      if (m < k)  k = m;
      if (y < k)  k = y;

      k = (k * CLAMP (*pullout, 0, 100)) / 100;

      *red   = ((c - k) << 8) / (256 - k);
      *green = ((m - k) << 8) / (256 - k);
      *blue  = ((y - k) << 8) / (256 - k);
      *pullout = k;
    }
}

inline void cmyk_to_rgb_int (int *cyan, int *magenta, int *yellow, int *black){
  int c, m, y, k;
  c = *cyan;
  m = *magenta;
  y = *yellow;
  k = *black;
  if (k)
    {
      c = ((c * (256 - k)) >> 8) + k;
      m = ((m * (256 - k)) >> 8) + k;
      y = ((y * (256 - k)) >> 8) + k;
    }
  *cyan    = 255 - c;
  *magenta = 255 - m;
  *yellow  = 255 - y;
}

#endif
