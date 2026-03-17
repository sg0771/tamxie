#include <cmath>
#include "frei0r.hpp"
#include "frei0r_math.h"

class Vignette : public frei0r::filter
{

public:

    f0r_param_double m_aspect; ///< Neutral value: 0.5
    f0r_param_double m_cc; ///< Neutral value: 0
    f0r_param_double m_soft; ///< Suggested value: 0.6

    Vignette(unsigned int width, unsigned int height) :
        m_width(width),
        m_height(height)
    {
        register_param(m_aspect, "aspect", "Aspect ratio");
        register_param(m_cc, "clearCenter", "Size of the unaffected center");
        register_param(m_soft, "soft", "Softness");

        // Suggested default values
        m_aspect = .5;
        m_cc = 0;
        m_soft = .6;

        m_initialized = width*height > 0;
        if (m_initialized) {
            m_vignette = new float[width*height];
            updateVignette();
        }
    }

    ~Vignette()
    {
        if (m_initialized) {
            delete[] m_vignette;
        }
    }

    virtual void update(double time,
	                    uint32_t* out,
                        const uint32_t* in)
    {
        std::copy(in, in + m_width*m_height, out);

        // Rebuild the vignette matrix if a parameter has changed
        if (m_prev_aspect != m_aspect
                || m_prev_cc != m_cc
                || m_prev_soft != m_soft) {
            updateVignette();
        }

        uint8_t *pixel = (uint8_t *) in;
        uint8_t *dest = (uint8_t *) out;

        // Darken the pixels by multiplying with the vignette's factor
        float *vignette = m_vignette;
        for (int i = 0; i < size; i++) {
            *dest++ = (char) (*vignette * *pixel++);
            *dest++ = (char) (*vignette * *pixel++);
            *dest++ = (char) (*vignette * *pixel++);
            *dest++ = *pixel++;
            vignette++;
        }

    }

private:
    f0r_param_double m_prev_aspect;
    f0r_param_double m_prev_cc;
    f0r_param_double m_prev_soft;

    float *m_vignette;
    bool m_initialized;

    unsigned int m_width;
    unsigned int m_height;

    void updateVignette()
    {
//        std::cout << "New settings: aspect = " << m_aspect << ", clear center = " << m_cc << ", soft = " << m_soft << std::endl;
        m_prev_aspect = m_aspect;
        m_prev_cc = m_cc;
        m_prev_soft = m_soft;

        float soft = 5*std::pow(float(1)-m_soft,2)+.01;
        float scaleX = 1;
        float scaleY = 1;

        // Distance from 0.5 (\in [0,0.5]) scaled to [0,1]
        float scale = std::fabs(m_aspect-.5)*2;
        // Map scale to [0,5] in a way that values near 0 can be adjusted more precisely
        scale = 1 + 4*std::pow(scale, 3);
        // Scale either x or y, depending on the aspect value being above or below 0.5
        if (m_aspect > 0.5) {
            scaleX = scale;
        } else {
            scaleY = scale;
        }
//        std::cout << "Used values: soft=" << soft << ", x=" << scaleX << ", y=" << scaleY << std::endl;

        int cx = m_width/2;
        int cy = m_height/2;
        float rmax = std::sqrt(std::pow(float(cx), 2) + std::pow(float(cy), 2));
        float r;

        for (int y = 0; y < m_height; y++) {
            for (int x = 0; x < m_width; x++) {

                // Euclidian distance to the center, normalized to [0,1]
                r = std::sqrt(std::pow(scaleX*(x-cx), 2) + std::pow(scaleY*(y-cy), 2))/rmax;

                // Subtract the clear center
                r -= m_cc;

                if (r <= 0) {
                    // Clear center: Do not modify the brightness here
                    m_vignette[y*m_width+x] = 1;
                } else {
                    r *= soft;
                    if (r > M_PI_2) {
                        m_vignette[y*m_width+x] = 0;
                    } else {
                        m_vignette[y*m_width+x] = std::pow(std::cos(r), 4);
                    }
                }

            }
        }


    }

};


static frei0r::construct<Vignette> plugin("Vignette",
                "Lens vignetting effect, applies natural vignetting",
                "Simon A. Eugster (Granjow)",
                0,2,
                F0R_COLOR_MODEL_RGBA8888);


filter_dest_cpp(vignette)