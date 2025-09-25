import os
import ctypes

import numpy as np

lib = ctypes.CDLL("scripts/omega.so")
lib2 = ctypes.CDLL("scripts/toms917.so")

lib.omega4.argtypes = [ctypes.c_float]
lib.omega4.restype = ctypes.c_float

print(lib2.wrightomega_double)
lib2.wrightomega_double.argtypes = [ctypes.c_double]
lib2.wrightomega_double.restype = ctypes.c_double


def omega(x: float) -> float:
    if x > 0.5:
        return lib.omega4(x)
    else:
        return lib.wrightomega_double(x)


class DiodeClipper:
    # State variables
    buffer_size = 16

    # Diode parameters
    c = 1e-8
    r = 2_200
    i_s = 200e-9
    v_t = 0.02585

    # Discretization coefficients

    def __init__(self, sample_rate, asymetric: bool = False):
        self.sample_rate = sample_rate
        self.is_asymetric = asymetric

        self.b0 = 2 / sample_rate
        self.b1 = -2 / sample_rate
        self.a1 = -1

        crb_1 = self.c * self.r + self.b0 + 1
        self.k1 = 1 / (self.c * self.r)
        self.k2 = (self.c * self.r) / crb_1
        self.k3 = (self.i_s * self.r) / crb_1
        self.k4 = 1 / self.v_t
        self.k5 = np.log((self.i_s * self.r) / crb_1 * self.v_t)
        self.k6 = self.b1 - self.a1 * self.b0
        print(self.k6)

        self.prev_v = 1
        self.prev_p = self.k6 * self.prev_v

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
        if self.is_asymetric and vin < 0 or not self.is_asymetric:
            q = self.k1 * vin - self.prev_p
            r = np.sign(q)
            w = self.k2 * q + self.k3 * r
            vout = w - self.v_t * r * lib.omega4(self.k4 * r * w + self.k5)
            p = self.k6 * vout - self.a1 * self.prev_p
            self.prev_p = p
            return vout
        else:
            return vin


if __name__ == "__main__":
    import matplotlib.pyplot as plt

    sample_rate = 96000
    for frequency in [60 * 2**i for i in range(8)]:
        duration = 5 / frequency
        n_samples = int(duration * sample_rate)

        t = np.linspace(0, duration, n_samples, endpoint=False)
        y = np.sin(2 * np.pi * frequency * t)

        diod_clipper = DiodeClipper(sample_rate)
        y_clipped = diod_clipper.process(y)

        plt.plot(
            y,
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
