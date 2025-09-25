import ctypes

import numpy as np

lib = ctypes.CDLL("scripts/omega.so")
lib2 = ctypes.CDLL("scripts/toms917.so")

lib.omega4.argtypes = [ctypes.c_float]
lib.omega4.restype = ctypes.c_float

lib2.wrightomega_double.argtypes = [ctypes.c_double]
lib2.wrightomega_double.restype = ctypes.c_double


def omega4(x):
    return lib.omega4(ctypes.c_float(x))


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


def omega_cured(x):
    if x < 1.5:
        return omega_small(x)
    else:
        return omega4(x)


class AsymetricSingleDiodeClipper:
    # Circuit parameters from your schematic
    r = 6650.0
    c = 220e-9  # 220nF
    i_s = 2.5e-9
    n = 1.7
    v_t = 0.02585

    def __init__(self, sample_rate):
        self.sample_rate = sample_rate
        self.dt = 1.0 / sample_rate

        # State variables to store the previous sample's values
        self.vin_prev = 0.0
        self.vout_prev = 0.0

    def I_d(self, v):
        """Calculates diode current using Shockley equation."""
        return self.i_s * (np.exp(v / (self.n * self.v_t)) - 1)

    def I_d_prime(self, v):
        """Calculates the derivative of the diode current."""
        return (self.i_s / (self.n * self.v_t)) * np.exp(v / (self.n * self.v_t))

    def process(self, input_signal: np.ndarray):
        output = np.zeros_like(input_signal)
        for i, s in enumerate(input_signal):
            output[i] = self.process_sample(s)
        return output

    def process_sample(self, vin_n: float):
        # History term from the capacitor's state
        v_hist = self.vout_prev + self.r * self.I_d(self.vout_prev) - self.vin_prev

        # A very good initial guess is the previous output sample
        vout_n = self.vout_prev

        # Iterate a fixed number of times for real-time safety
        for _ in range(8):
            id_n = self.I_d(vout_n)
            id_prime_n = self.I_d_prime(vout_n)

            # The function f(vout_n) = 0 we want to solve
            f_x = (self.c / self.dt) * (vout_n + self.r * id_n - vin_n - v_hist) + id_n

            # The derivative f'(vout_n)
            f_prime_x = (self.c / self.dt) * (1.0 + self.r * id_prime_n) + id_prime_n

            # Newton-Raphson update
            vout_n = vout_n - f_x / (f_prime_x + 1e-9)  # Add epsilon for safety

        # CRUCIAL: Update state variables for the next sample
        self.vin_prev = vin_n
        self.vout_prev = vout_n

        return vout_n


if __name__ == "__main__":
    import matplotlib.pyplot as plt

    frequency = 120
    sample_rate = 44100
    duration = 3 / frequency
    n_samples = int(duration * sample_rate)

    t = np.linspace(0, duration, n_samples, endpoint=False)
    y = 10 * np.sin(2 * np.pi * frequency * t)

    diod_clipper = AsymetricSingleDiodeClipper(sample_rate)
    y_clipped = diod_clipper.process(y)

    plt.plot(t, y)
    plt.plot(t, y_clipped)
    plt.show()

    Y = np.fft.fft(y)
    Y_clipped = np.fft.fft(y_clipped)
    xf = np.fft.fftfreq(n_samples, 1 / sample_rate)

    Y_norm = 2 / n_samples * np.abs(Y[0 : n_samples // 2])
    Y_clipped_norm = 2 / n_samples * np.abs(Y_clipped[0 : n_samples // 2])
    xf_pos = xf[0 : n_samples // 2]

    plt.plot(xf_pos, Y_norm)
    plt.plot(xf_pos, Y_clipped_norm)
    plt.xlim(0, 8000)
    plt.show()
