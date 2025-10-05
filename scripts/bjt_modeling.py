import numpy as np
import ctypes

lib = ctypes.CDLL("scripts/bin/omega.so")

lib.omega4_py.argtypes = [ctypes.c_float]
lib.omega4_py.restype = ctypes.c_float


def omega_small(x):
    """
    Approximates the Wright Omega function for small values of x using a
    5th-order Taylor series expansion.
    """
    c0 = 0.5671432904097838
    c1 = 0.3618963236098023
    c2 = 0.0736778463779836
    c3 = -0.0013437346889135
    c4 = -0.0016355437889344
    c5 = 0.0002166542734346

    return c0 + c1 * x + c2 * x**2 + c3 * x**3 + c4 * x**4 + c5 * x**5


def omega(x: float) -> float:
    if x > 1.5 or x < -1.5:
        return lib.omega4_py(x)
    else:
        print("Using small x approximation for omega")
        # return lib.omega4_py(x)
        return omega_small(x)


class BJT:
    buffer_size = 16

    def __init__(self, vp=9, vt=0.026, i_s=0.1e-15, beta_f=100, re=1_000):
        self.vp = vp
        self.vt = vt
        self.i_s = i_s
        self.beta_f = beta_f
        self.re = re
        self.k = np.log((self.i_s * self.re / self.vt) * (1 + 1 / self.beta_f))

    def process(self, input_signal: np.array):
        output = np.zeros_like(input_signal)
        for i in range(0, len(input_signal), self.buffer_size):
            buffer = input_signal[i : i + self.buffer_size]
            output[i : i + self.buffer_size] = self.process_buffer(buffer)
        return output

    def process_buffer(self, buffer: np.array):
        return np.array([self.process_sample(sample) for sample in buffer])

    def process_sample(self, vin):
        vref = self.vp / 2
        vin = vin + vref
        if vin >= self.vp:
            v_x = self.i_s * self.re * (1 + 1 / self.beta_f)
            v_out = self.vt * omega(((self.vp + v_x) / self.vt) + self.k) - v_x
            return v_out - vref

        v_x = self.i_s * self.re * (np.exp((vin - self.vp) / self.vt) + 1 / self.beta_f)
        v_out = self.vt * omega(((vin + v_x) / self.vt) + self.k) - v_x
        return v_out - vref


if __name__ == "__main__":
    import matplotlib.pyplot as plt

    frequency = 120
    sample_rate = 44100
    duration = 5 / frequency
    n_samples = int(duration * sample_rate)

    t = np.linspace(0, duration, n_samples, endpoint=False)
    y = 5 * np.sin(2 * np.pi * frequency * t)

    bjt = BJT()
    y_clipped = bjt.process(y)
    print(y_clipped[0])

    plt.plot(t, y, label="input")
    plt.plot(t, y_clipped, label="output")
    plt.legend()
    plt.show()
