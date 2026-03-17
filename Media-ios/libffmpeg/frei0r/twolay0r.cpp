#include "frei0r.hpp"

#include <algorithm>
#include <vector>
#include <utility>
#include <cassert>

#include <iostream>

class twolay0r : public frei0r::filter
{
  static uint8_t grey(unsigned int value)
  {
    uint8_t* rgba = reinterpret_cast<uint8_t*>(&value);
    uint8_t gw= (rgba[0] + rgba[1] + 2*rgba[2])/4;
    return gw;
  }
  
  struct histogram
  {
    histogram()
      : hist(256)
    {
      std::fill(hist.begin(),hist.end(),0);
    }
    
    void operator()(uint32_t value)
    {
      ++hist[grey(value)];
    }
    
    std::vector<unsigned int> hist;
  };

public:
  twolay0r(unsigned int width, unsigned int height)
  {
  }

  virtual void update(double time,
                      uint32_t* out,
                      const uint32_t* in)
  {
    histogram h;
    
    // create histogramm
    for (const unsigned int* i=in; i != in + (width*height);++i)
      h(*i);

    // calc th
    int th=127;
    int th_old=0;
    
    while (th!=th_old)
      {
	th_old=th;
	// calc low
	double num = 0;
	double val = 0;
	for (int i= (int)(th-1); i!= -1; --i)
	  {
	    num += h.hist[i];
	    val += h.hist[i]*i;
	  }
	uint8_t low = static_cast<uint8_t>(val/num);
	
	// clac hi
	num = 0;
	val = 0;
	for (unsigned int i=th;i!=256;++i)
	  {
	    num += h.hist[i];
	    val += h.hist[i]*i;
	  }
	uint8_t hi = static_cast<uint8_t>(val/num);

	th = (low + hi) / 2;
      }


    
    // create b/w image with the th value
    {
      uint32_t* outpixel= out;
      const uint32_t* pixel=in;
      while(pixel != in+(width*height))
	{
	  if ( grey(*pixel)<th )
	    *outpixel=0xFF000000;
	  else
	    *outpixel=0xFFFFFFFF;
	  ++outpixel;
	  ++pixel;
	}
    }  
  }
};


static frei0r::construct<twolay0r> plugin("Twolay0r",
				  "dynamic thresholding",
				  "Martin Bayer",
				  0,2);


filter_dest_cpp(twolay0r)
