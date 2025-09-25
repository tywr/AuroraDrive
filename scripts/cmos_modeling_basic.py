import numpy as np
import matplotlib.pyplot as plt
from scipy.optimize import brentq

# --- Define Parameters for both transistors ---

# NMOS Parameters
k_prime_n = 110e-6
W_L_n = 2
V_th_n = 0.7
lambda_n = 0.05

# PMOS Parameters (k'p is typically lower than k'n)
k_prime_p = 50e-6
W_L_p = 4 # PMOS is often wider to match current
V_th_p = -0.8 # Threshold for PMOS is negative
lambda_p = 0.06

# --- Transistor Models ---

def id_nmos(vgs, vds):
    """Calculates drain current for NMOS."""
    if vgs <= V_th_n:
        return 0.0 # Cut-off
    
    vov = vgs - V_th_n
    if vds >= vov: # Saturation
        return 0.5 * k_prime_n * W_L_n * vov**2 * (1 + lambda_n * vds)
    else: # Triode
        return k_prime_n * W_L_n * (vov * vds - 0.5 * vds**2) * (1 + lambda_n * vds)

def id_pmos(vsg, vsd):
    """Calculates drain current for PMOS using Vsg and Vsd (positive values)."""
    if vsg <= abs(V_th_p):
        return 0.0 # Cut-off
        
    vov = vsg - abs(V_th_p)
    if vsd >= vov: # Saturation
        return 0.5 * k_prime_p * W_L_p * vov**2 * (1 + lambda_p * vsd)
    else: # Triode
        return k_prime_p * W_L_p * (vov * vsd - 0.5 * vsd**2) * (1 + lambda_p * vsd)

# --- The Solver Function ---

def find_vout(vin, vdd):
    """
    Finds Vout for a given Vin by finding the root of the current difference equation.
    """
    # Define the function whose root we want to find
    # f(vout) = id_nmos - id_pmos
    def current_difference(vout):
        # Calculate NMOS current
        i_n = id_nmos(vgs=vin, vds=vout)
        
        # Calculate PMOS current
        vsg_p = vdd - vin
        vsd_p = vdd - vout
        i_p = id_pmos(vsg=vsg_p, vsd=vsd_p)
        
        return i_n - i_p

    # Use a numerical solver to find vout in the range [0, vdd]
    # where the current difference is zero.
    try:
        vout_solution = brentq(current_difference, 0, vdd)
        return vout_solution
    except ValueError:
        # Solver fails if Vin is 0 or Vdd, handle these edge cases
        if vin < V_th_n:
            return vdd
        elif vin > vdd - abs(V_th_p):
            return 0
        return np.nan # Should not happen in the middle

# --- Main Application: Generate the Voltage Transfer Characteristic (VTC) ---

VDD = 5.0  # Set the supply voltage

# Create a sweep of input voltages
vin_range = np.linspace(0, VDD, 100)
vout_range = [find_vout(vin, VDD) for vin in vin_range]

# Plot the result
plt.figure(figsize=(10, 7))
plt.plot(vin_range, vout_range, label='VTC ($V_{out}$ vs $V_{in}$)')
plt.plot([0, VDD], [0, VDD], 'k--', label='$V_{out} = V_{in}$') # Helper line
plt.title('CMOS Inverter Voltage Transfer Characteristic (VTC)', fontsize=16)
plt.xlabel('$V_{in}$ (V)', fontsize=14)
plt.ylabel('$V_{out}$ (V)', fontsize=14)
plt.grid(True)
plt.legend()
plt.show()

# Example: Get a single value for Vout
vin_test = 2.5
vout_test = find_vout(vin_test, VDD)
print(f"For Vin = {vin_test:.2f}V, the calculated Vout is {vout_test:.2f}V")
