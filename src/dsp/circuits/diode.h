#include "../maths/omega.h"
#include <cmath>

inline float omega(float x)
{
    if (x > 1.5f)
    {
        return omega4(x);
    }
    else
    {
        float c0 = 0.5671432904097838;
        float c1 = 0.3618963236098023;
        float c2 = 0.0736778463779836;
        float c3 = -0.0013437346889135;
        float c4 = -0.0016355437889344;
        float c5 = 0.0002166542734346;

        return c0 + c1 * x + c2 * std::pow(x, 2.0f) + c3 * std::pow(x, 3.0f) +
               c4 * std::pow(x, 4.0f) + c5 * std::pow(x, 5.0f);
    }
}

class Diode
{
  public:
    Diode(float fs, float c, float r, float i_s, float v_t);
    void initializeState();
    float processSample(float);

  private:
    // State variables
    float prev_v;
    float prev_p;

    // Main parameters
    float c;
    float r;
    float i_s;
    float v_t;

    // Fixed variables for computation
    float b0;
    float b1;
    float a1;

    float crb_1;
    float k1;
    float k2;
    float k3;
    float k4;
    float k5;
    float k6;

}

inline Diode::Diode(float t_fs, float t_c, float t_r, float t_i_s, float t_v_t)
{
    fs = t_fs;
    c = t_c;
    r = t_r;
    i_s = t_i_s;
    v_t = t_v_t;

    b0 = 2.0f / fs;
    b1 = -2.0f / fs;
    a1 = -1.0f;

    crb_1 = c * r + b0 + 1;
    k1 = 1 / (c * r);
    k2 = (c * r) / crb_1;
    k3 = (i_s * r) / crb_1;
    k4 = 1 / v_t;
    k5 = std::log((i_s * r) / crb_1 * v_t);
    k6 = b1 - a1 * b0;

    prev_v = 1.0f;
    prev_p = k6 * prev_v;
}

inline void Diode::processSample(float vin)
{
    float q = k1 * vin - prev_p;
    float r = np.sign(q);
    float w = k2 * q + k3 * r;
    float vout = w - v_t * r * omega(k4 * r * w + k5);
    float p = k6 * vout - a1 * prev_p;
    prev_p = p;
    prev_v = vout;
    return vout;
}
