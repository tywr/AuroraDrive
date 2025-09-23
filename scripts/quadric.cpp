#include <cmath>

#include "quadric.h"

Triode::Triode(
    float fs, float kp, float kp2, float kpg, float E, float Rp, float Rk,
    float Ck, float Rg
)
    : m_fs(fs), m_kp(kp), m_kp2(kp2), m_kpg(kpg), m_E(E), m_Rp(Rp), m_Rk(Rk),
      m_Ck(Ck), m_Rg(Rg), m_wCi_s(0), m_wCk_s(0), m_wCo_s(0)
{

    // Constants
    const float Ri = 1e6f;
    const float Ci = 100e-9f;
    const float Ro = 1e6f;
    const float Co = 10e-9f;

    // Initial voltages for state variables
    float k1 = m_kpg / (2.0f * m_kp2) + m_Rp / m_Rk + 1.0f;
    float k2 = k1 * (m_kp / m_kp2 + 2.0f * m_E) * m_kp2;
    float k3 = m_Rk * k2 + 1.0f;
    m_wCk_s = (k3 - (k1 > 0 ? 1.0f : -1.0f) * sqrtf(2.0f * k3 - 1.0f)) /
              (2.0f * m_Rk * k1 * k1 * m_kp2);
    m_wCo_s = m_E - m_Rp / m_Rk * m_wCk_s;

    // WDF element values
    const float wVi_R = 1e-6f;
    const float wCi_R = 1.0f / (2.0f * m_fs * Ci);
    const float wCk_R = 1.0f / (2.0f * m_fs * m_Ck);
    const float wCo_R = 1.0f / (2.0f * m_fs * Co);
    const float wsi_kl = wCi_R / (wCi_R + wVi_R);
    const float wsi_R = wCi_R + wVi_R;
    const float wpg_kt = wsi_R / (wsi_R + Ri);
    const float wsg_kl = m_Rg / (m_Rg + (wsi_R * Ri) / (wsi_R + Ri));
    const float wpk_kt = wCk_R / (m_Rk + wCk_R);
    const float wsp_kl = wCo_R / (wCo_R + Ro);
    const float wsp_R = wCo_R + Ro;
    const float wpp_kt = wsp_R / (wsp_R + m_Rp);

    // Storing triode model parameters as members
    m_wsg_R = m_Rg + (wsi_R * Ri) / (wsi_R + Ri);
    m_wpk_R = (m_Rk * wCk_R) / (m_Rk + wCk_R);
    m_wpp_R = (wsp_R * m_Rp) / (wsp_R + m_Rp);

    // Filter coefficients (precomputed)
    m_kTxCi = 1.0f - wpg_kt;
    m_kTCk = 1.0f - wpk_kt;
    m_kTCo = 1.0f - wpp_kt;
    m_kT0 = wpp_kt * m_E;
    m_kyT = 0.5f * (1.0f - wsp_kl);
    m_kyCo = -0.5f * (1.0f - wsp_kl) * (1.0f + wpp_kt);
    m_ky0 = 0.5f * (1.0f - wsp_kl) * wpp_kt * m_E;
    m_kCiT = wsi_kl * (1.0f - wsg_kl);
    m_kCixCi = wsi_kl * ((1.0f - wpg_kt) * (wsg_kl + 1.0f) - 2.0f);
    m_kCoCo = 1.0f - wsp_kl * (1.0f + wpp_kt);
    m_kCo0 = wsp_kl * wpp_kt * m_E;
    m_wpk_kt = wpk_kt;
    m_wsp_kl = wsp_kl;
}

float Triode::process(float vin)
{
    float xCi = vin + m_wCi_s;
    float wT_ag = m_kTxCi * xCi;
    float wT_ak = m_kTCk * m_wCk_s;
    float wT_ap = m_kTCo * m_wCo_s + m_kT0;

    float wT_bg, wT_bk, wT_bp;
    triode(wT_ag, wT_ak, wT_ap, wT_bg, wT_bk, wT_bp);

    float vout = m_kyT * wT_bp + m_kyCo * m_wCo_s + m_ky0;

    m_wCi_s = m_kCiT * wT_bg + m_kCixCi * xCi + m_wCi_s;
    m_wCk_s = wT_bk - m_wpk_kt * m_wCk_s;
    m_wCo_s = m_wsp_kl * wT_bp + m_kCoCo * m_wCo_s + m_kCo0;
    return vout;
}

void Triode::triode(
    float ag, float ak, float ap, float& bg, float& bk, float& bp
)
{
    float bk_bp = m_wpk_R / m_wpp_R;
    float k_eta = 1.0f / (bk_bp * (0.5f * m_kpg + m_kp2) + m_kp2);
    float k_delta = m_kp2 * k_eta * k_eta / (2.0f * m_wpp_R);
    float k_bp_s = k_eta * sqrtf(2.0f * m_kp2 / m_wpp_R);
    float bp_k = 1.0f / (m_wpp_R + m_wpk_R);
    float bp_ap_0 = bp_k * (m_wpk_R - m_wpp_R);
    float bp_ak_0 = bp_k * (2.0f * m_wpp_R);

    float v1 = 0.5f * ap;
    float v2 = ak + v1 * bk_bp;
    float alpha = m_kpg * (ag - v2) + m_kp;
    float beta = m_kp2 * (v1 - v2);
    float eta = k_eta * (2.0f * beta + alpha);
    float v3 = eta + k_delta;
    float delta = ap + v3;

    if (delta >= 0)
    {
        bp = k_bp_s * sqrtf(delta) - v3 - k_delta;
        float d = bk_bp * (ap - bp);
        bk = ak + d;
        float Vpk2 = ap + bp - ak - bk;

        if (m_kpg * (ag - ak - 0.5f * d) + m_kp2 * Vpk2 + m_kp < 0)
        {
            bp = ap;
            bk = ak;
        }
    }
    else
    {
        bp = ap;
        bk = ak;
    }

    float Vpk = bp - bk;
    if (Vpk < 0)
    {
        bp = bp_ap_0 * ap + bp_ak_0 * ak;
    }
    bg = ag;
}
