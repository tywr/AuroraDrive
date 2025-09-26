#include <vector>

class Triode
{
  public:
    Triode(
        float fs, float kp, float kp2, float kpg, float E, float Rp, float Rk,
        float Ck, float Rg
    );

    float process(float);

  private:
    // Helper function for the triode model, now a private method
    void triode(float ag, float ak, float ap, float& bg, float& bk, float& bp);

    // Circuit parameters and constants
    float m_fs, m_kp, m_kp2, m_kpg, m_E, m_Rp, m_Rk, m_Ck, m_Rg;

    // WDF element values
    float m_wsg_R, m_wpk_R, m_wpp_R;

    // Filter states
    float m_wCi_s;
    float m_wCk_s;
    float m_wCo_s;

    // Precomputed coefficients
    float m_kTxCi, m_kTCk, m_kTCo, m_kT0;
    float m_kyT, m_kyCo, m_ky0;
    float m_kCiT, m_kCixCi;
    float m_kCoCo, m_kCo0;
    float m_wpk_kt, m_wsp_kl;
};
