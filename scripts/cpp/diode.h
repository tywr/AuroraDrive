#pragma once
#include "omega.h"
#include <cmath>

class Diode
{
  public:
    Diode(float fs, float c, float r, float i_s, float v_t);
    float omega(float x);
    float processSample(float);

  private:
    // State variables
    float prev_v;
    float prev_p;

    // Main parameters
    float fs;
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
};

Diode::Diode(float t_fs, float t_c, float t_r, float t_i_s, float t_v_t)
{
    fs = t_fs;
    c = t_c;
    r = t_r;
    i_s = t_i_s;
    v_t = t_v_t;

    // Pre-calculate fixed values
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

    // Initialize state variables
    prev_v = 1.0f;
    prev_p = k6 * prev_v;
}

float Diode::omega(float x)
{
    if ((x > 1.5f) || (x < -1.5f))
    {
        return omega4(x);
    }
    else
    {
        const float c0 = 0.5671432904097838f;
        const float c1 = 0.3618963236098023f;
        const float c2 = 0.0736778463779836f;
        const float c3 = -0.0013437346889135f;
        const float c4 = -0.0016355437889344f;
        const float c5 = 0.0002166542734346f;

        return c0 + x * (c1 + x * (c2 + x * (c3 + x * (c4 + x * c5))));
    }
}

float Diode::processSample(float vin)
{
    float q = k1 * vin - prev_p;

    float r_sign = 0.0f;
    if (q > 0.0f)
        r_sign = 1.0f;
    else if (q < 0.0f)
        r_sign = -1.0f;

    float w = k2 * q + k3 * r_sign;
    float vout = w - v_t * r_sign * omega(k4 * r_sign * w + k5);

    float p = k6 * vout - a1 * prev_p;

    // Update state for the next sample
    prev_p = p;
    prev_v = vout;

    return vout;
}
