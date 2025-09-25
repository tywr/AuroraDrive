import os
import ctypes

import numpy as np


class CustomClipper:
    # State variables
    buffer_size = 16

    def __init__(
        self,
        sample_rate,
        asymetric: bool = False,
        r: float = 1000e3,
        vdd: float = 9,
        delta: float = 0.06,
        p_vt_coef: list = [-0.25610349392710086, 0.27051216771368214],
        p_alpha_coef: list = [
            -0.0003577445606469842,
            -0.0008620153809796321,
            -0.00016848836814836602,
            -1.0800821774906936e-5,
        ],
        n_vt_coef: list = [1.208306917691355, 0.3139084341943607],
        n_alpha_coef: list = [0.020662094888127674, -0.0017181795239085821],
    ):
        self.sample_rate = sample_rate
        self.is_asymetric = asymetric
        self.r = r
        self.vdd = vdd
        self.p_alpha_coef = p_alpha_coef
        self.p_vt_coef = p_vt_coef
        self.n_alpha_coef = n_alpha_coef
        self.n_vt_coef = n_vt_coef
        self.delta = delta
        self.prev_v = 1

    def process(self, input_signal: np.array):
        output = np.zeros_like(input_signal)
        for i in range(0, len(input_signal), self.buffer_size):
            buffer = input_signal[i : i + self.buffer_size]
            output[i : i + self.buffer_size] = self.process_buffer(buffer)
        return output

    def process_buffer(self, buffer: np.array):
        return np.array([self.process_sample(sample) for sample in buffer])

    def process_sample(self, s: float):
        vin = s  # Input sample value

        # p-channel
        vgs = vin - self.vdd
        vds = self.prev_v - self.vdd
        vt = self.p_vt_coef[1] * vgs + self.p_vt_coef[0]
        alpha = (
            self.p_alpha_coef[3] * vgs**3
            + self.p_alpha_coef[2] * vgs**2
            + self.p_alpha_coef[1] * vgs
            + self.p_alpha_coef[0]
        )
        if vgs >= vt:
            iout_p = 0
        if vds >= (vgs - vt) and vgs < vt:
            iout_p = -alpha * (vgs - vt - vds / 2) * vds * (1 - self.delta)
        else:
            iout_p = (-alpha / 2) * (vgs - vt**2) * (1 - self.delta)

        # n-channel
        vgs = vin
        vds = self.prev_v
        vt = self.n_vt_coef[1] * vgs + self.n_vt_coef[0]
        alpha = self.n_alpha_coef[1] * vgs + self.n_alpha_coef[0]
        if vgs <= vt:
            iout_n = 0
        elif vds <= (vgs - vt) and vgs > vt:
            iout_n = alpha * (vgs - vt - (vds / 2)) * vds
        else:
            iout_n = (alpha / 2) * (vgs - vt**2)

        vout = (iout_n + iout_p) * 1000
        return vout


if __name__ == "__main__":
    import matplotlib.pyplot as plt

    sample_rate = 96000
    for frequency in [120 * 2**i for i in range(1)]:
        duration = 2 / frequency
        n_samples = int(duration * sample_rate)

        t = np.linspace(0, duration, n_samples, endpoint=False)
        y = 4.5 + 5 * np.sin(2 * np.pi * frequency * t)

        clipper = CustomClipper(sample_rate)
        y_clipped = clipper.process(y)

        # plt.plot(
        #     y,
        #     y_clipped,
        #     label=f"{frequency} Hz",
        #     alpha=0.5,
        #     color=plt.cm.viridis(frequency / 4000),
        # )
        # plt.plot(
        #     t,
        #     y,
        #     label=f"{frequency} Hz",
        #     alpha=0.5,
        #     color=plt.cm.viridis(frequency / 4000),
        # )
        plt.plot(
            t,
            y_clipped,
            label=f"{frequency} Hz",
            alpha=0.5,
            color=plt.cm.viridis(frequency / 4000),
        )
    plt.legend()
    plt.show()

    # Y = np.fft.fft(y)
    # Y_clipped = np.fft.fft(y_clipped)
    # xf = np.fft.fftfreq(n_samples, 1 / sample_rate)
    #
    # Y_norm = 2 / n_samples * np.abs(Y[0 : n_samples // 2])
    # Y_clipped_norm = 2 / n_samples * np.abs(Y_clipped[0 : n_samples // 2])
    # xf_pos = xf[0 : n_samples // 2]
    #
    # plt.plot(xf_pos, Y_norm)
    # plt.plot(xf_pos, Y_clipped_norm)
    # plt.xlim(0, 8000)
    # plt.show()
