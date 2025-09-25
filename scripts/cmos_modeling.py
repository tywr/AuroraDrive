import numpy as np


class CMOSInverterStage:
    """
    Solves the static, non-linear behavior of a CMOS inverter with feedback.
    This class has no 'memory' (state); it calculates Vout based on Vin instantly.
    """

    def __init__(self, sample_rate):
        self.buffer_size = 16

        # --- Circuit & MOSFET Parameters ---
        self.r_f = 1e6  # 1M feedback resistor
        self.v_dd = 9.0  # 9V power supply

        # Typical parameters for a CD4000 series inverter's MOSFETs
        self.delta = 0.06

    def _nmos_ids_and_deriv(self, vgs, vds):
        """Calculates NMOS drain-source current AND its derivative w.r.t. vds."""
        vt_coef = [1.208306917691355, 0.3139084341943607]
        alpha_coef = [0.020662094888127674, -0.0017181795239085821]

        alpha = alpha_coef[1] * vgs + alpha_coef[0]
        vt = vt_coef[1] * vgs + vt_coef[0]

        if vgs <= vt:  # Cut-off
            return 0.0, 0.0

        if vds < vgs - vt and vgs > vt:
            ids = alpha * (vgs - vt - vds / 2) * vds
            gds = alpha * (vgs - vt) - alpha * vds  # Derivative d(Ids)/d(Vds)
            return ids, gds
        else:
            ids = 0.5 * alpha * (vgs - vt) ** 2
            gds = 0.0
            return ids, gds

    def _pmos_ids_and_deriv(self, vgs, vds):
        """Calculates PMOS drain-source current AND its derivative w.r.t. vds."""
        vt_coef = [0.25610349392710086, -0.27051216771368214]
        alpha_coef = [
            -0.0003577445606469842,
            -0.0008620153809796321,
            -0.00016848836814836602,
            -1.0800821774906936e-5,
        ]
        alpha = (
            alpha_coef[3] * vgs**3
            + alpha_coef[2] * vgs**2
            + alpha_coef[1] * vgs
            + alpha_coef[0]
        )
        vt = vt_coef[1] * vgs + vt_coef[0]

        if vgs >= vt:
            return 0.0, 0.0

        if vds > vgs - vt and vgs < vt:  # Triode region
            ids = -alpha * (vgs - vt - vds / 2) * vds * (1 - self.delta * vds)
            gds = -alpha * (
                3 * self.delta * vds**2 / 2
                - (2 * self.delta * (vgs - vt) + 1) * vds
                + vgs
                - vt
            )
            return ids, gds

        else:  # Saturation region
            ids = -0.5 * alpha * ((vgs - vt) ** 2) * (1 - self.delta * vds)
            gds = 0.5 * alpha * self.delta * (vgs - vt) ** 2
            return ids, gds

    def process(self, input_signal: np.array):
        output = np.zeros_like(input_signal)
        for i in range(0, len(input_signal), self.buffer_size):
            buffer = input_signal[i : i + self.buffer_size]
            output[i : i + self.buffer_size] = self.process_buffer(buffer)
        return output

    def process_buffer(self, buffer: np.array):
        return np.array([self.process_sample(sample) for sample in buffer])

    def process_sample(self, vin: float):
        # We need to solve the KCL equation for the output node voltage (vout)
        # f(vout) = (vin - vout)/r_f - I_ds_n(vin, vout) + I_ds_p(vin, vout) = 0

        # A good initial guess is the middle of the supply rails
        vout = self.v_dd / 2.0

        # Newton-Raphson solver with a fixed number of iterations
        for _ in range(8):
            # NMOS voltages
            vgs_n, vds_n = vin, vout
            # PMOS voltages
            vgs_p, vds_p = vin - self.v_dd, vout - self.v_dd

            # Get currents and derivatives
            ids_n, gds_n = self._nmos_ids_and_deriv(vgs_n, vds_n)
            ids_p, gds_p = self._pmos_ids_and_deriv(vgs_p, vds_p)

            # KCL equation: f(x) = 0
            f_x = (vin - vout) / self.r_f - ids_n + ids_p

            # Derivative of KCL equation: f'(x)
            f_prime_x = -1.0 / self.r_f - gds_n + gds_p

            # Newton-Raphson update step
            vout = vout - f_x / (f_prime_x + 1e-9)

        return vout


if __name__ == "__main__":
    import matplotlib.pyplot as plt

    frequency = 120
    sample_rate = 44100
    duration = 5 / frequency
    n_samples = int(duration * sample_rate)

    t = np.linspace(0, duration, n_samples, endpoint=False)
    y = 4.5 + 5 * np.sin(2 * np.pi * frequency * t)

    cmos_clipper = CMOSInverterStage(sample_rate)
    y_clipped = cmos_clipper.process(y)

    plt.plot(t, y)
    plt.plot(t, y_clipped)
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
