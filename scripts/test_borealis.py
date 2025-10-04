import os
import numpy as np
import matplotlib.pyplot as plt
import ctypes
import math

from diode_modeling import DiodeClipper


lib = ctypes.CDLL("scripts/bin/diode.so")

lib.Diode_create.argtypes = [
    ctypes.c_float,
    ctypes.c_float,
    ctypes.c_float,
    ctypes.c_float,
    ctypes.c_float,
]
lib.Diode_create.restype = (
    ctypes.c_void_p
)  # The return is an opaque pointer to our Diode object

# C++: float Diode_process_sample(Diode* diode_instance, float inputVoltage)
lib.Diode_process_sample.argtypes = [ctypes.c_void_p, ctypes.c_float]
lib.Diode_process_sample.restype = ctypes.c_float

# C++: void Diode_destroy(Diode* diode_instance)
lib.Diode_destroy.argtypes = [ctypes.c_void_p]
lib.Diode_destroy.restype = None


class PyDiode:
    def __init__(self, fs, c, r, i_s, v_t):
        """
        Constructor: Calls the C function to create a Diode instance.
        """
        self._instance = lib.Diode_create(
            ctypes.c_float(fs),
            ctypes.c_float(c),
            ctypes.c_float(r),
            ctypes.c_float(i_s),
            ctypes.c_float(v_t),
        )
        print(f"C++ Diode object created at memory address: {self._instance}")

    def process_sample(self, vin):
        """
        Calls the C function to process a single sample.
        """
        return lib.Diode_process_sample(self._instance, ctypes.c_float(vin))

    def __del__(self):
        """
        Destructor: Called when the Python object is garbage collected.
        This is where we clean up the C++ memory.
        """
        print(f"Destroying C++ Diode object at memory address: {self._instance}")
        lib.Diode_destroy(self._instance)


if __name__ == "__main__":
    sample_rate = 44_100
    frequency = 440
    duration = 3 / frequency
    t = np.linspace(0, duration, int(sample_rate * duration), endpoint=False)
    input = np.sin(2 * np.pi * frequency * t)

    diode = PyDiode(sample_rate, 1e-8, 2200, 200e-9, 0.02585)
    diode_clipper = DiodeClipper(sample_rate)
    # output = [diode.process_sample(sample) for sample in input]
    output_2 = [diode_clipper.process_sample(sample) for sample in input]

    plt.plot(t, input, label="Input Signal")
    # plt.plot(t, output, label="Output Signal")
    plt.plot(t, output_2, label="Output Signal (Python)")
    plt.show()
