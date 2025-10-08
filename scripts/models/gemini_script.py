import numpy as np
import matplotlib.pyplot as plt

class MinimalModel:
    """
    A minimal, self-contained model to test the feedback solver logic.
    The complex transistors have been REPLACED by a simple, ideal amplifier.
    """
    def __init__(self, sample_rate, V_dd):
        self.sample_rate = sample_rate
        self.V_dd = V_dd

        # --- Use the confirmed circuit parameters ---
        self.r = 330e3
        self.c = 22e-12

        # --- State variables ---
        # Initialize to the expected DC bias point
        self.prev_vin = self.V_dd / 2.0
        self.prev_vout = self.V_dd / 2.0
        
        # --- Ideal Amplifier Parameters ---
        # This replaces your nmos/pmos models. We're using a
        # simple transconductance model: i_out = -gm * v_in
        self.g_m = 10.0  # A very high transconductance (gain)
        self.g_ds = 1e-6 # A very low output conductance (high output resistance)


    def solve_rc(self, vin: float) -> float:
        """
        The solver we have been debugging, now using the ideal amplifier.
        """
        # --- Pre-calculate constants for this time step ---
        g_r = 1.0 / self.r
        g_c = self.c * self.sample_rate
        i_c_prev = g_c * (self.prev_vout - self.prev_vin)

        # --- Use previous state as the initial guess ---
        vout = self.prev_vout 

        # --- Run solver for enough iterations to converge ---
        for _ in range(50):
            # --- THIS REPLACES YOUR TRANSISTORS ---
            # Ideal linear amplifier model:
            # The input to the gate is Vin, so the output current is -gm * (Vin - Vbias)
            # For the AC part, the current change is -gm * (change in Vin)
            # The KCL is i_n + i_p = 0. We model the *net* current from the transistors.
            # Vgs for the NMOS is Vin. Let's assume Vbias is Vdd/2.
            ids_net = -self.g_m * (vin - self.V_dd / 2.0)
            gds_net = self.g_ds # The combined output conductance
            # ------------------------------------
            
            # Currents from feedback components (same as your code)
            i_r = (vout - vin) * g_r
            i_c = g_c * (vout - vin) - i_c_prev
            
            # KCL Function
            f_x = ids_net + i_r + i_c

            # Derivative of KCL w.r.t vout
            f_prime_x = gds_net + g_r + g_c

            # Newton-Raphson Step
            if abs(f_prime_x) < 1e-9: break
            update_step = f_x / f_prime_x
            vout = vout - update_step # NOTE: I removed the clip, the solver should be stable
            
            if abs(update_step) < 1e-7: break

        # Update state for the next step
        self.prev_vout = vout
        self.prev_vin = vin
        return vout

# --- Main simulation script ---
sample_rate = 44100
dt = 1.0 / sample_rate
V_DD = 9.0

# Instantiate the minimal model
model = MinimalModel(sample_rate=sample_rate, V_dd=V_DD)

# High frequency to guarantee a visible oval
frequency = 40000 
max_t = 100 / frequency # Simulate for 100 cycles

t = np.arange(0, max_t, dt)
input_signal = V_DD/2.0 + 1.0 * np.sin(2 * np.pi * frequency * t)
output_signal = np.zeros_like(input_signal)

print("Running minimal test case...")
for i in range(len(t)):
    output_signal[i] = model.solve_rc(input_signal[i])
print("Done.")

# Plotting
plt.figure(figsize=(10, 6))
plt.plot(input_signal, output_signal, color="green", alpha=0.7)
plt.title('Minimal Test Case: Vout vs. Vin')
plt.xlabel('Input Voltage (V)')
plt.ylabel('Output Voltage (V)')
plt.grid(True)
plt.axis('equal') # Use equal scaling to see the ellipse shape clearly
plt.show()
