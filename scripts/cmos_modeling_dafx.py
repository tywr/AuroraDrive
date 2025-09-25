import numpy as np
import matplotlib.pyplot as plt

def model_dafx_cmos(vin: float) -> float:
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
    
    # --- Model Parameters from Table 1 of the DAFx Paper ---
    V_dd = 5.0      # Supply Voltage
    V_tn = 1.4      # NMOS Threshold Voltage
    V_tp = -1.4     # PMOS Threshold Voltage
    K_n = 2.4e-3    # NMOS Transconductance
    K_p = 1.2e-3    # PMOS Transconductance
    lambda_n = 0.04 # NMOS Channel-Length Modulation
    lambda_p = 0.05 # PMOS Channel-Length Modulation

    # --- Helper functions for NMOS and PMOS currents and their derivatives ---
    
    def _nmos_ids_and_deriv(vgs, vds):
        """Calculates NMOS I_ds and its derivative w.r.t. vds (g_ds)."""
        if vgs <= V_tn:  # Cut-off
            return 0.0, 0.0
        
        v_ov = vgs - V_tn
        channel_mod = (1 + lambda_n * vds)
        
        if vds < v_ov:  # Triode region
            ids = K_n * (v_ov * vds - 0.5 * vds**2) * channel_mod
            # Derivative of the whole expression w.r.t. vds
            gds = K_n * (
                (v_ov - vds) * channel_mod + 
                (v_ov * vds - 0.5 * vds**2) * lambda_n
            )
            return ids, gds
        else:  # Saturation region
            ids = 0.5 * K_n * v_ov**2 * channel_mod
            gds = 0.5 * K_n * v_ov**2 * lambda_n
            return ids, gds

    def _pmos_ids_and_deriv(vsg, vsd):
        """Calculates PMOS I_sd and its derivative w.r.t. vsd (g_sd)."""
        if vsg <= abs(V_tp):  # Cut-off
            return 0.0, 0.0
        
        v_ov = vsg - abs(V_tp)
        channel_mod = (1 + lambda_p * vsd)
        
        if vsd < v_ov:  # Triode region
            isd = K_p * (v_ov * vsd - 0.5 * vsd**2) * channel_mod
            gsd = K_p * (
                (v_ov - vsd) * channel_mod +
                (v_ov * vsd - 0.5 * vsd**2) * lambda_p
            )
            return isd, gsd
        else:  # Saturation region
            isd = 0.5 * K_p * v_ov**2 * channel_mod
            gsd = 0.5 * K_p * v_ov**2 * lambda_p
            return isd, gsd

    # --- Newton-Raphson Solver ---
    # We want to solve the KCL equation for vout:
    # f(vout) = i_n(vin, vout) - i_p(vin, vout) = 0
    
    # A good initial guess is the middle of the supply rails
    vout = V_dd / 2.0

    # Iterate a fixed number of times for stability and real-time safety
    for _ in range(10):
        # Map circuit voltages to MOSFET terminal voltages
        # NMOS:
        vgs_n = vin
        vds_n = vout
        # PMOS:
        vsg_p = V_dd - vin
        vsd_p = V_dd - vout

        # Get currents and their derivatives
        ids_n, gds_n = _nmos_ids_and_deriv(vgs_n, vds_n)
        isd_p, gsd_p = _pmos_ids_and_deriv(vsg_p, vsd_p)

        # The function we want to find the root of (KCL)
        f_x = ids_n - isd_p
        
        # The derivative of the function w.r.t. vout
        # d(ids_n)/d(vout) = gds_n
        # d(isd_p)/d(vout) = d(isd_p)/d(vsd_p) * d(vsd_p)/d(vout) = gsd_p * (-1) = -gsd_p
        f_prime_x = gds_n - (-gsd_p)
        
        # Newton-Raphson update step
        vout = vout - f_x / (f_prime_x + 1e-9) # Add epsilon for stability
        
        # Clamp vout to the supply rails for stability during iteration
        vout = np.clip(vout, 0, V_dd)

    return vout

# --- Demonstration of the function ---
if __name__ == "__main__":
    print("Demonstrating the DAFx 2020 CMOS Inverter Model.")
    
    # Generate a range of input voltages from 0 to V_dd to see the transfer curve
    input_voltages = np.linspace(0, 5.0, 500)
    
    # Calculate the output voltage for each input voltage
    output_voltages = np.array([model_dafx_cmos(v) for v in input_voltages])
    
    # --- Plotting the DC Transfer Curve ---
    plt.figure(figsize=(10, 6))
    plt.plot(input_voltages, output_voltages, label='CMOS Inverter Output V(out)')
    plt.plot(input_voltages, input_voltages, 'r--', label='V(out) = V(in)', alpha=0.5)

    # Add lines for the supply rails
    plt.axhline(y=5.0, color='gray', linestyle='--', label='Vdd (5V)')
    plt.axhline(y=0.0, color='gray', linestyle='--')

    plt.title('DC Transfer Curve of the DAFx 2020 CMOS Inverter Model')
    plt.xlabel('Input Voltage (Vin)')
    plt.ylabel('Output Voltage (Vout)')
    plt.grid(True)
    plt.legend()
    plt.show()

    # --- Plotting the effect on a sine wave ---
    SAMPLE_RATE = 48000
    duration = 0.03
    frequency = 100
    t = np.linspace(0., duration, int(SAMPLE_RATE * duration), endpoint=False)
    
    # Input sine wave centered around Vdd/2 with large amplitude to show clipping
    input_sine = 2.5 + 3.0 * np.sin(2. * np.pi * frequency * t) 
    
    output_sine = np.array([model_dafx_cmos(s) for s in input_sine])

    plt.figure(figsize=(12, 7))
    plt.plot(t, input_sine, label='Input Signal (Vin)')
    plt.plot(t, output_sine, label='Output Signal (Vout)')
    plt.title('CMOS Inverter Processing a Sine Wave')
    plt.xlabel('Time (s)')
    plt.ylabel('Voltage (V)')
    plt.grid(True)
    plt.legend()
    plt.show()
