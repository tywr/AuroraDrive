#include "omega.h"

float log2f_approx(float x)
{
    union
    {
        int i;
        float f;
    } v;
    v.f = x;
    int ex = v.i & 0x7f800000;
    int e = (ex >> 23) - 127;
    v.i = (v.i - ex) | 0x3f800000;
    return (float)e - 2.213475204444817f +
           v.f * (3.148297929334117f +
                  v.f * (-1.098865286222744f + v.f * 0.1640425613334452f));
}

float logf_approx(float x)
{
    return 0.693147180559945f * log2f_approx(x);
}

float pow2f_approx(float x)
{
    if (x < -126.f)
        return 0.f;
    union
    {
        int i;
        float f;
    } v;
    int xi = (int)x;
    int l = x < 0.0f ? xi - 1 : xi;
    float f = x - (float)l;
    v.i = (l + 127) << 23;
    return v.f *
           (1.0f + f * (0.6931471805599453f +
                        f * (0.2274112777602189f + f * 0.07944154167983575f)));
}

float expf_approx(float x)
{
    return pow2f_approx(1.442695040888963f * x);
}

float omega1(float x)
{
    return x > 0.f ? x : 0.f;
}

float omega2(float x)
{
    const float x1 = -3.684303659906469f;
    const float x2 = 1.972967391708859f;
    const float a = 9.451797158780131e-3f;
    const float b = 1.126446405111627e-1f;
    const float c = 4.451353886588814e-1f;
    const float d = 5.836596684310648e-1f;
    return x < x1 ? 0.f : (x > x2 ? x : d + x * (c + x * (b + x * a)));
}

float omega3(float x)
{
    const float x1 = -3.341459552768620f;
    const float x2 = 8.f;
    const float a = -1.314293149877800e-3f;
    const float b = 4.775931364975583e-2f;
    const float c = 3.631952663804445e-1f;
    const float d = 6.313183464296682e-1f;
    return x < x1
               ? 0.f
               : (x < x2 ? d + x * (c + x * (b + x * a)) : x - logf_approx(x));
}

float omega4(float x)
{
    const float y = omega3(x);
    return y - (y - expf_approx(x - y)) / (y + 1.f);
}
