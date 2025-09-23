import numpy as np


class Triode:
    # Constants
    Ri = 1e6
    Ci = 100e-9
    Ro = 1e6
    Co = 10e-9

    def __init__(self, fs, kp, kp2, kpg, E, Rp, Rk, Ck, Rg):
        self.kp = kp
        self.kp2 = kp2
        self.kpg = kpg
        self.E = E
        self.Rp = Rp
        self.Rk = Rk
        self.Ck = Ck
        self.Rg = Rg
        self.wVi_R = 1e-6
        self.wCi_R = 1 / (2 * fs * self.Ci)
        self.wCk_R = 1 / (2 * fs * Ck)
        self.wCo_R = 1 / (2 * fs * self.Co)
        self.wsi_kl = self.wCi_R / (self.wCi_R + self.wVi_R)
        self.wsi_R = self.wCi_R + self.wVi_R
        self.wpg_kt = self.wsi_R / (self.wsi_R + self.Ri)
        self.wpg_R = (self.wsi_R * self.Ri) / (self.wsi_R + self.Ri)
        self.wsg_kl = Rg / (Rg + self.wpg_R)
        self.wsg_R = Rg + self.wpg_R
        self.wpk_kt = self.wCk_R / (Rk + self.wCk_R)
        self.wpk_R = (Rk * self.wCk_R) / (Rk + self.wCk_R)
        self.wsp_kl = self.wCo_R / (self.wCo_R + self.Ro)
        self.wsp_R = self.wCo_R + self.Ro
        self.wpp_kt = self.wsp_R / (self.wsp_R + Rp)
        self.wpp_R = (self.wsp_R * Rp) / (self.wsp_R + Rp)

        self.kTxCi = 1 - self.wpg_kt
        self.kTCk = 1 - self.wpk_kt
        self.kTCo = 1 - self.wpp_kt
        self.kT0 = self.wpp_kt * E
        self.kyT = 0.5 * (1 - self.wsp_kl)
        self.kyCo = -0.5 * (1 - self.wsp_kl) * (1 + self.wpp_kt)
        self.ky0 = 0.5 * (1 - self.wsp_kl) * self.wpp_kt * E
        self.kCiT = self.wsi_kl * (1 - self.wsg_kl)
        self.kCixCi = self.wsi_kl * ((1 - self.wpg_kt) * (self.wsg_kl + 1) - 2)
        self.kCoCo = 1 - self.wsp_kl * (1 + self.wpp_kt)
        self.kCo0 = self.wsp_kl * self.wpp_kt * E

        # triode parameters
        self.bk_bp = self.wpk_R / self.wpp_R
        self.k_eta = 1 / (self.bk_bp * (0.5 * self.kpg + self.kp2) + self.kp2)
        self.k_delta = self.kp2 * self.k_eta * self.k_eta / (self.wpp_R + self.wpp_R)
        self.k_bp_s = self.k_eta * np.sqrt((self.kp2 + self.kp2) / self.wpp_R)
        self.bp_k = 1 / (self.wpp_R + self.wpk_R)
        self.bp_ap_0 = self.bp_k * (self.wpk_R - self.wpp_R)
        self.bp_ak_0 = self.bp_k * (self.wpp_R + self.wpp_R)

    def triode(self, ag, ak, ap, kp, kp2, kpg):
        """
        Simulates the triode's non-linear behavior.
        Returns the reflected waves (bg, bk, bp).
        """

        v1 = 0.5 * ap
        v2 = ak + v1 * self.bk_bp
        alpha = kpg * (ag - v2) + kp
        beta = kp2 * (v1 - v2)
        eta = self.k_eta * (beta + beta + alpha)
        v3 = eta + self.k_delta
        delta = ap + v3

        if delta >= 0:
            bp = self.k_bp_s * np.sqrt(delta) - v3 - self.k_delta
            d = self.bk_bp * (ap - bp)
            bk = ak + d
            Vpk2 = ap + bp - ak - bk

            if kpg * (ag - ak - 0.5 * d) + kp2 * Vpk2 + kp < 0:
                bp = ap
                bk = ak
                Vpk = ap - ak
            else:
                Vpk = 0.5 * Vpk2
        else:
            bp = ap
            bk = ak
            Vpk = ap - ak

        if Vpk < 0:
            bp = self.bp_ap_0 * ap + self.bp_ak_0 * ak

        bg = ag
        return bg, bk, bp

    def process(self, y: np.array) -> np.ndarray:
        """Process an input signal array through the triode model."""

        k1 = self.kpg / (2 * self.kp2) + self.Rp / self.Rk + 1
        k2 = k1 * (self.kp / self.kp2 + 2 * self.E) * self.kp2
        k3 = self.Rk * k2 + 1
        sign_k1 = 1 if k1 >= 0 else -1
        Vk0 = (k3 - sign_k1 * np.sqrt(2 * k3 - 1)) / (2 * self.Rk * k1 * k1 * self.kp2)
        Vp0 = self.E - self.Rp / self.Rk * Vk0

        self.wCi_s = 0.0
        self.wCk_s = Vk0
        self.wCo_s = Vp0

        yout = np.zeros_like(y)
        for n, vin in enumerate(y):
            yout[n] = self.process_sample(vin)
        return yout

    def process_sample(self, vin):
        xCi = vin + self.wCi_s
        wT_ag = self.kTxCi * xCi
        wT_ak = self.kTCk * self.wCk_s
        wT_ap = self.kTCo * self.wCo_s + self.kT0

        wT_bg, wT_bk, wT_bp = self.triode(wT_ag, wT_ak, wT_ap, kp, kp2, kpg)

        vout = self.kyT * wT_bp + self.kyCo * self.wCo_s + self.ky0

        self.wCi_s = self.kCiT * wT_bg + self.kCixCi * xCi + self.wCi_s
        self.wCk_s = wT_bk - self.wpk_kt * self.wCk_s
        self.wCo_s = self.wsp_kl * wT_bp + self.kCoCo * self.wCo_s + self.kCo0

        return vout


if __name__ == "__main__":
    import matplotlib.pyplot as plt

    # Example usage
    fs = 44100
    freq = 100
    N = 3 * fs // freq
    t = np.linspace(0, 3 / freq, N)
    y = 2.5 * np.sin(2 * np.pi * freq * t)

    kp = 1.014e-5
    kp2 = 5.498e-8
    kpg = 1.076e-5

    E = 250.0
    Rp = 100e3
    Rk = 1e3
    Ck = 10e-6
    Rg = 100e3

    triode = Triode(fs=fs, kp=kp, kp2=kp2, kpg=kpg, E=E, Rp=Rp, Rk=Rk, Ck=Ck, Rg=Rg)

    yout = triode.process(y)

    plt.plot(t, y, label="Input")
    plt.plot(t, yout, label="Overdriven")
    plt.legend()
    plt.show()

    # Y = np.fft.fft(y)
    # Y_clipped = np.fft.fft(yout)
    # xf = np.fft.fftfreq(N, 1 / fs)
    #
    # Y_norm = 2 / N * np.abs(Y[0 : N // 2])
    # Y_clipped_norm = 2 / N * np.abs(Y_clipped[0 : N // 2])
    # xf_pos = xf[0 : N // 2]
    #
    # plt.plot(xf_pos, Y_norm, label="Input")
    # plt.plot(xf_pos, Y_clipped_norm, label="Overdrive")
    # plt.xlim(0, 8000)
    # plt.legend()
    # plt.show()
