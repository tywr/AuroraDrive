import numpy as np
from scipy.signal import butter, filtfilt


class Triode:
    def __init__(self, fs, kp, kp2, kpg, E, Ci, Co, Ck, Ri, Ro, Rp, Rk, Rg):
        self.kp = kp
        self.kp2 = kp2
        self.kpg = kpg
        self.E = E
        self.Ci = Ci
        self.Co = Co
        self.Ck = Ck
        self.Ri = Ri
        self.Ro = Ro
        self.Rp = Rp
        self.Rk = Rk
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

        return -vout * 2 / 27


if __name__ == "__main__":
    import matplotlib.pyplot as plt

    # Example usage
    fs = 44100

    kp = 1.014e-5
    kp2 = 5.498e-8
    kpg = 1.076e-5

    E = 250
    Ri = 1e6
    Rg = 20e3
    Ck = 10e-6
    Co = 10e-9
    Rp = 100e3
    Ro = 1e6
    Rk = 1e3
    Ci = 100e-9

    triode = Triode(
        fs=fs,
        kp=kp,
        kp2=kp2,
        kpg=kpg,
        E=E,
        Ci=Ci,
        Co=Co,
        Ck=Ck,
        Ri=Ri,
        Ro=Ro,
        Rp=Rp,
        Rk=Rk,
        Rg=Rg,
    )
    triode2 = Triode(
        fs=fs,
        kp=kp,
        kp2=kp2,
        kpg=kpg,
        E=E,
        Ci=Ci,
        Co=Co,
        Ck=Ck,
        Ri=Ri,
        Ro=Ro,
        Rp=Rp,
        Rk=Rk,
        Rg=Rg,
    )

    freq = 440
    T = 30 / freq
    N = int(T * fs)
    t = np.linspace(0, T, N)
    y = 0.251189 * np.sin(2 * np.pi * freq * t)

    input_gain = 1
    drive_gain = 7.94

    yout = triode.process(y * input_gain)
    b, a = butter(1, 20, btype="highpass", analog=False, fs=fs)
    yout_hpf = filtfilt(b, a, yout)

    yout2 = triode2.process(yout_hpf * drive_gain)
    b, a = butter(1, 20, btype="highpass", analog=False, fs=fs)
    yout2_hpf = filtfilt(b, a, yout2)


    start = N // 4
    end = 3 * N // 4

    t = t[start:end]
    y = y[start:end]
    yout_hpf = yout_hpf[start:end]
    yout2_hpf = yout2_hpf[start:end]

    plt.plot(t, y, label="Input Signal", alpha=1)
    plt.plot(t, yout_hpf, label="Output Signal", alpha=0.6)
    plt.plot(t, yout2_hpf, label="Output Signal", alpha=0.6)
    plt.show()


    n = len(y)
    Y = np.fft.fft(y)
    Yout = np.fft.fft(yout_hpf)
    Yout2 = np.fft.fft(yout2_hpf)
    xf = np.fft.fftfreq(n, 1 / fs)
    xf_pos = xf[: n // 2]
    Y_abs = np.abs(Y[: n // 2])  # Get magnitudes of the positive frequencies
    Yout_abs = np.abs(Yout[: n // 2])
    Yout2_abs = np.abs(Yout2[: n // 2])
    Y_norm = Y_abs / n
    Yout_norm = Yout_abs / n
    Yout2_norm = Yout2_abs / n
    Y_norm[1:] = Y_norm[1:] * 2
    Yout_norm[1:] = Yout_norm[1:] * 2
    Yout2_norm[1:] = Yout2_norm[1:] * 2

    plt.figure(figsize=(12, 6))
    plt.plot(xf_pos, Y_norm, label="Input Spectrum", alpha=1)
    plt.plot(xf_pos, Yout_norm, label="Output Spectrum", alpha=0.5)
    plt.plot(xf_pos, Yout2_norm, label="Output Spectrum 2", alpha=0.5)
    plt.title("Single-Sided Amplitude Spectrum of the Signal")
    plt.xlabel("Frequency (Hz)")
    plt.ylabel("Amplitude")
    plt.grid(True)
    plt.legend()
    plt.show()
