#include "diod_clipper.h"
#include "dsp/maths/omega.h"
#include "dsp/maths/toms917.h"

float DiodClipper::omegaFunction(float x)
{
    // Use different omega approximations for small and large values
    if (x > 0.5)
    {
        return omega4(x);
    }
    else
    {
        return static_cast<double> wrightomega_double(x);
    }
}
