import numpy as np
import matplotlib.pyplot as plt


class ModelLlama:
    """
    Models a single CMOS inverter unit based on the DAFx 2020 paper_21.

    This function implements the extended Shichman-Hodges model described
    in the paper "Real-Time Black-Box Modelling with Recurrent Neural
    Networks for Audio Distortion Effects". It solves the non-linear
    Kirchhoff's Current Law equation for the inverter's output node
    using the Newton-Raphson method.

    Args:
        vin: The input voltage to the CMOS inverter gate.

    Returns:
        The corresponding output voltage from the inverter.
    """

    def __init__(self, V_dd=9.0, delta=0.06):
        self.V_dd = V_dd
        self.delta = 0.06

    def nmos(self, vgs, vds):
        """Calculates NMOS I_ds and its derivative w.r.t. vds (g_ds)."""
        vt_coef = [1.208306917691355, 0.3139084341943607]
        alpha_coef = [0.020662094888127674, -0.0017181795239085821]

        alpha = alpha_coef[1] * vgs + alpha_coef[0]
        vt = vt_coef[1] * vgs + vt_coef[0]

        if vgs <= vt:
            return 0.0, 0.0, vt, alpha

        if vds < vgs - vt and vgs > vt:
            ids = alpha * (vgs - vt - vds / 2) * vds
            gds = alpha * (vgs - vt) - alpha * vds  # Derivative d(Ids)/d(Vds)
            return ids, gds, vt, alpha

        ids = 0.5 * alpha * (vgs - vt) ** 2
        gds = 0.0
        return ids, gds, vt, alpha

    def pmos(self, vgs, vds):
        """Calculates PMOS I_sd and its derivative w.r.t. vsd (g_sd)."""
        vt_coef = [-0.25610349392710086, 0.27051216771368214]
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
            return 0.0, 0.0, vt, alpha

        if vds >= vgs - vt and vgs < vt:
            ids = -alpha * (vgs - vt - vds / 2) * vds * (1 - self.delta * vds)
            gds = -alpha * (
                3 * self.delta * vds**2 / 2
                - (2 * self.delta * (vgs - vt) + 1) * vds
                + vgs
                - vt
            )
            return ids, gds, vt, alpha

        ids = -0.5 * alpha * ((vgs - vt) ** 2) * (1 - self.delta * vds)
        gds = 0.5 * alpha * self.delta * (vgs - vt) ** 2
        return ids, gds, vt, alpha

    def solve(self, vin: float) -> float:
        # if vin < 1.5:
        #     return 0

        vout = self.V_dd / 2.0

        # Iterate a fixed number of times for stability and real-time safety
        for _ in range(10):
            # NMOS:
            vgs_n = vin
            vds_n = vout

            # PMOS:
            vgs_p = vin - self.V_dd
            vds_p = vout - self.V_dd

            # Get currents and their derivatives
            ids_n, gds_n, _, _ = self.nmos(vgs_n, vds_n)
            ids_p, gds_p, _, _ = self.pmos(vgs_p, vds_p)

            # The function we want to find the root of (KCL)
            f_x = ids_n + ids_p

            # The derivative of the function w.r.t. vout
            # d(ids_n)/d(vout) = gds_n
            # d(isd_p)/d(vout) = d(isd_p)/d(vsd_p) * d(vsd_p)/d(vout) = gsd_p * (-1) = -gsd_p
            f_prime_x = gds_n + gds_p

            # Newton-Raphson update step
            vout = vout - f_x / (f_prime_x + 1e-9)  # Add epsilon for stability

            # Clamp vout to the supply rails for stability during iteration
            vout = np.clip(vout, 0, self.V_dd)

        return vout


# --- Demonstration of the function ---
if __name__ == "__main__":
    print("Demonstrating the DAFx 2020 CMOS Inverter Model.")

    model = ModelLlama(V_dd=9.0, delta=0.06)
    # Vin = np.linspace(0, 9, 9)
    # plt.axhline(y=0, color="r", linestyle="-", alpha=0.1)
    # for vin in Vin:
    #     Vout = np.linspace(-5, 30, 1000)
    #     pmos = np.array([model.pmos(vin - 9, vout - 9)[0] for vout in Vout])
    #     nmos = np.array([model.nmos(vin, vout)[0] for vout in Vout])
    #     # fx = nmos - pmos
    #     # plt.plot(Vds, pmos, label=f"pmos vin={vin} V")
    #     # plt.plot(Vds, nmos, label=f"nmos vin={vin} V")
    #     plt.plot(Vout, nmos + pmos, label=f"$v_{{in}}$={vin}V")
    #     plt.ylabel("$f(x)$")
    #     plt.xlabel("$v_{out}$")
    #
    # plt.legend()
    # plt.show()

    # alpha-graph for p-channel
    # X = np.linspace(-6, -2.5, 500)
    # V_t = np.array([model.pmos(x, 0)[3] for x in X])
    # plt.plot(X, V_t)
    # plt.title("PMOS alpha vs. Vgs")
    # plt.show()

    # vt vs vgs graph for p-channel
    # X = np.linspace(-6, 2.5, 500)
    # V_t = np.array([model.pmos(x, 0)[2] for x in X])
    # plt.plot(X, V_t)
    # plt.title("PMOS Threshold Voltage vs. Vgs")
    # plt.show()

    # vt vs vgs graph for n-channel
    # X = np.linspace(2.5, 6, 500)
    # V_t = np.array([model.nmos(x, 0)[2] for x in X])
    # plt.plot(X, V_t)
    # plt.title("NMOS Threshold Voltage vs. Vgs")
    # plt.show()

    # id vs vds graph for p-channel
    # Vds = np.linspace(-9, 0, 500)
    # Vgs = np.linspace(-6, -2.5, 15)
    # ids_vgs = []
    # for vgs in Vgs:
    #     ids = np.array([model.pmos(vgs, vds)[0] for vds in Vds])
    #     ids_vgs.append(ids)
    # for i, vgs in enumerate(zip(Vgs, ids_vgs)):
    #     plt.plot(Vds, vgs[1], label=f"Vgs={vgs[0]:.2f} V", alpha=0.5)
    # plt.legend()
    # plt.show()

    # id vs vds graph for n-channel
    # Vds = np.linspace(0, 9, 500)
    # Vgs = np.linspace(2.5, 6, 15)
    # ids_vgs = []
    # for vgs in Vgs:
    #     ids = np.array([model.nmos(vgs, vds)[0] for vds in Vds])
    #     ids_vgs.append(ids)
    # for i, vgs in enumerate(zip(Vgs, ids_vgs)):
    #     plt.plot(Vds, vgs[1], label=f"Vgs={vgs[0]:.2f} V", alpha=0.5)
    # plt.legend()
    # plt.show()

    # Vout / Vin curve
    # vgs = np.linspace(0, 9, 10000)
    # vout = np.array([model.solve(v) for v in vgs])
    # plt.plot(vgs, vout)
    # plt.show()

    # # Vout / Vin curve
    input_voltages = np.linspace(0, 9, 10000)
    output_voltages = np.array([model.solve(v) for v in input_voltages])

    plt.figure(figsize=(10, 6))
    plt.plot(input_voltages, output_voltages, label="CMOS Inverter Output V(out)")
    plt.plot(input_voltages, input_voltages, "r--", label="V(out) = V(in)", alpha=0.5)
    plt.axhline(y=9, color="gray", linestyle="--", label="Vdd")
    plt.axhline(y=0.0, color="gray", linestyle="--")
    plt.title("DC Transfer Curve of the Red LLAMA")
    plt.xlabel("Input Voltage (Vin)")
    plt.ylabel("Output Voltage (Vout)")
    plt.grid(True)
    plt.legend()
    plt.show()

    # Sine wave processing
    # SAMPLE_RATE = 48000
    # duration = 0.02
    # frequency = 500
    # t = np.linspace(0.0, duration, int(SAMPLE_RATE * duration), endpoint=False)
    #
    # # Input sine wave centered around Vdd/2 with large amplitude to show clipping
    # input_sine = 4.5 + 5 * np.sin(2.0 * np.pi * frequency * t)
    #
    # output_sine = np.array([model_dafx_cmos(s) for s in input_sine])
    #
    # plt.figure(figsize=(12, 7))
    # plt.plot(t, input_sine, label="Input Signal (Vin)")
    # plt.plot(t, output_sine, label="Output Signal (Vout)")
    # plt.title("CMOS Inverter Processing a Sine Wave with Red LLAMA")
    # plt.xlabel("Time (s)")
    # plt.ylabel("Voltage (V)")
    # plt.grid(True)
    # plt.legend()
    # plt.show()
