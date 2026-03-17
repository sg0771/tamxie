
#include "frei0r.hpp"
#include "frei0r_math.h"

#define NBYTES 4
#define ALPHA 3

class addition : public frei0r::mixer2
{
public:
  addition(unsigned int width, unsigned int height)
  {
    // initialize look-up table
    for (int i = 0; i < 256; i++)
      add_lut[i] = i;
  	for (int i = 256; i <= 510; i++)
      add_lut[i] = 255;
  }

  void update(double time,
              uint32_t* out,
              const uint32_t* in1,
              const uint32_t* in2)
  {
    const uint8_t *A = reinterpret_cast<const uint8_t*>(in1);
    const uint8_t *B = reinterpret_cast<const uint8_t*>(in2);
    uint8_t *D = reinterpret_cast<uint8_t*>(out);
    uint32_t sizeCounter = size;
            
    uint32_t b;
  
    while (sizeCounter--)
      {
        for (b = 0; b < ALPHA; b++)
          D[b] = add_lut[A[b] + B[b]];
        
        D[ALPHA] = MIN(A[ALPHA], B[ALPHA]);
        A += NBYTES;
        B += NBYTES;
        D += NBYTES;
      }
  }
  
private:
  static uint8_t add_lut[511]; // look-up table storing values to do a quick MAX of two values when you know you add two uint8_ts
};

uint8_t addition::add_lut[511];

static frei0r::construct<addition> plugin_addition("addition",
                                  "Perform an RGB[A] addition operation of the pixel sources.",
                                  "Jean-Sebastien Senecal",
                                  0,
									2,
                                  F0R_COLOR_MODEL_RGBA8888);



filter_dest_cpp(addition)