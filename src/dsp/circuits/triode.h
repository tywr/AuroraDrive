// triode.h
// Single-file header-only implementation of the Triode class.
// To use as a library, just #include this file.
// To run the example, compile this file directly: g++ triode.h -o triode_sim
// -std=c++17

#pragma once
#include <cmath>
#include <iostream>
#include <vector>

struct TriodeWaves
{
    float bg;
    float bk;
    float bp;
};

class Triode
{
  public:
    // Constructor
    Triode(
        float fs, float kp, float kp2, float kpg, float E, float Ci, float Co,
        float Ck, float Ri, float Ro, float Rp, float Rk, float Rg
    );
    void initializeState();

    // Processes a block of samples (a vector)
    // std::vector<float> process(const std::vector<float>& input);

    // Processes a single sample
    float processSample(float inputSample);

  private:
    // --- State Variables ---
    float wCi_s;
    float wCk_s;
    float wCo_s;

    // --- Input Parameters (stored for use in methods) ---
    float kp, kp2, kpg, E;

    // --- Pre-calculated Coefficients ---
    float wpg_kt, wpk_kt, wsp_kl, wpp_kt;
    float kTxCi, kTCk, kTCo, kT0;
    float kyT, kyCo, ky0;
    float kCiT, kCixCi;
    float kCoCo, kCo0;
    float bk_bp, k_eta, k_delta, k_bp_s;
    float bp_ap_0, bp_ak_0;

    // --- Circuit Parameters (needed for initialization) ---
    float Rp_val, Rk_val;

    TriodeWaves triode(float ag, float ak, float ap);
};

// --- IMPLEMENTATION ---
// Note: All method implementations are marked 'inline' because they are in a
// header file.

inline Triode::Triode(
    float fs, float kp, float kp2, float kpg, float E, float Ci, float Co,
    float Ck, float Ri, float Ro, float Rp, float Rk, float Rg
)
{
    // Store parameters that are needed later
    this->kp = kp;
    this->kp2 = kp2;
    this->kpg = kpg;
    this->E = E;
    this->Rp_val = Rp;
    this->Rk_val = Rk;

    float wVi_R = 1e-6;
    float wCi_R = 1.0 / (2.0 * fs * Ci);
    float wCk_R = 1.0 / (2.0 * fs * Ck);
    float wCo_R = 1.0 / (2.0 * fs * Co);
    float wsi_kl = wCi_R / (wCi_R + wVi_R);
    float wsi_R = wCi_R + wVi_R;
    wpg_kt = wsi_R / (wsi_R + Ri);
    float wpg_R = (wsi_R * Ri) / (wsi_R + Ri);
    float wsg_kl = Rg / (Rg + wpg_R);
    wpk_kt = wCk_R / (Rk + wCk_R);
    float wpk_R = (Rk * wCk_R) / (Rk + wCk_R);
    wsp_kl = wCo_R / (wCo_R + Ro);
    float wsp_R = wCo_R + Ro;
    wpp_kt = wsp_R / (wsp_R + Rp);
    float wpp_R = (wsp_R * Rp) / (wsp_R + Rp);

    kTxCi = 1.0 - wpg_kt;
    kTCk = 1.0 - wpk_kt;
    kTCo = 1.0 - wpp_kt;
    kT0 = wpp_kt * E;
    kyT = 0.5 * (1.0 - wsp_kl);
    kyCo = -0.5 * (1.0 - wsp_kl) * (1.0 + wpp_kt);
    ky0 = 0.5 * (1.0 - wsp_kl) * wpp_kt * E;
    kCiT = wsi_kl * (1.0 - wsg_kl);
    kCixCi = wsi_kl * ((1.0 - wpg_kt) * (wsg_kl + 1.0) - 2.0);
    kCoCo = 1.0 - wsp_kl * (1.0 + wpp_kt);
    kCo0 = wsp_kl * wpp_kt * E;

    // Triode parameters
    bk_bp = wpk_R / wpp_R;
    k_eta = 1.0 / (bk_bp * (0.5 * kpg + kp2) + kp2);
    k_delta = kp2 * k_eta * k_eta / (wpp_R + wpp_R);
    k_bp_s = k_eta * std::sqrt((kp2 + kp2) / wpp_R);
    bp_ap_0 = (1.0 / (wpp_R + wpk_R)) * (wpk_R - wpp_R);
    bp_ak_0 = (1.0 / (wpp_R + wpk_R)) * (wpp_R + wpp_R);

    // Initialize state variables to zero
    wCi_s = 0.0;
    wCk_s = 0.0;
    wCo_s = 0.0;
}

inline void Triode::initializeState()
{
    float k1 = kpg / (2.0 * kp2) + Rp_val / Rk_val + 1.0;
    float k2 = k1 * (kp / kp2 + 2.0 * E) * kp2;
    float k3 = Rk_val * k2 + 1.0;
    float sign_k1 = (k1 >= 0) ? 1.0 : -1.0;
    float Vk0 = (k3 - sign_k1 * std::sqrt(2.0 * k3 - 1.0)) /
                (2.0 * Rk_val * k1 * k1 * kp2);
    float Vp0 = E - Rp_val / Rk_val * Vk0;

    wCi_s = 0.0;
    wCk_s = Vk0;
    wCo_s = Vp0;
}

// inline std::vector<float> Triode::process(const std::vector<float>& input)
// {
//     initializeState(); // Set initial DC operating point
//     std::vector<float> output(input.size());
//     for (size_t n = 0; n < input.size(); ++n)
//     {
//         output[n] = processSample(input[n]);
//     }
//     return output;
// }

inline float Triode::processSample(float inputSample)
{
    float xCi = inputSample + wCi_s;
    float wT_ag = kTxCi * xCi;
    float wT_ak = kTCk * wCk_s;
    float wT_ap = kTCo * wCo_s + kT0;

    TriodeWaves waves = triode(wT_ag, wT_ak, wT_ap);

    float vout = kyT * waves.bp + kyCo * wCo_s + ky0;

    wCi_s = kCiT * waves.bg + kCixCi * xCi + wCi_s;
    wCk_s = waves.bk - wpk_kt * wCk_s;
    wCo_s = wsp_kl * waves.bp + kCoCo * wCo_s + kCo0;

    return vout;
}

inline TriodeWaves Triode::triode(float ag, float ak, float ap)
{
    float bp, bk;
    float v1 = 0.5 * ap;
    float v2 = ak + v1 * bk_bp;
    float alpha = kpg * (ag - v2) + kp;
    float beta = kp2 * (v1 - v2);
    float eta = k_eta * (beta + beta + alpha);
    float v3 = eta + k_delta;
    float delta = ap + v3;
    float Vpk;

    if (delta >= 0)
    {
        bp = k_bp_s * std::sqrt(delta) - v3 - k_delta;
        float d = bk_bp * (ap - bp);
        bk = ak + d;
        float Vpk2 = ap + bp - ak - bk;

        if (kpg * (ag - ak - 0.5 * d) + kp2 * Vpk2 + kp < 0)
        {
            bp = ap;
            bk = ak;
            Vpk = ap - ak;
        }
        else
        {
            Vpk = 0.5 * Vpk2;
        }
    }
    else
    {
        bp = ap;
        bk = ak;
        Vpk = ap - ak;
    }

    if (Vpk < 0)
    {
        bp = bp_ap_0 * ap + bp_ak_0 * ak;
    }

    float bg = ag;
    return {bg, bk, bp};
}
