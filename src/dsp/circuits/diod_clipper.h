#include <cmath>

class DiodClipper
{
  public:
    DiodClipper(
        double C, double R, double Is, double Vt, double b0, double b1,
        double a1
    )
        : i_C(C), i_R(R), i_Is(Is), i_Vt(Vt), i_b0(b0), i_b1(b1), i_a1(a1),
          k1(1.0 / (C * R)), k2((C * R) / (C * R + b0 + 1)),
          k3((Is * R) / (C * R + b0 + 1)), k4(1.0 / Vt),
          k5(std::log((Is * R) / (C * R + b0 + 1) * Vt)), k6(b1 - a1 * b0)
    {
        b0 = 2 / Ts a1 = 2 / Ts
    }

    float omegaFunction(float x);
    float processSample(float vin, float sample_rate);

  private:
    // Internal State
    float prev_p;
    float prev_v;

    // Circuit parameters
    const double i_C;
    const double i_R;
    const double i_Is;
    const double i_Vt;

    // Discretization coefficients
    const double i_b0;
    const double i_b1;
    const double i_a1;

    const double k1;
    const double k2;
    const double k3;
    const double k4;
    const double k5;
    const double k6;
}
