import os
import numpy as np
import matplotlib.pyplot as plt
import ctypes
import math
from cmos_modeling_llama import ModelLlama
from scipy.signal import butter, filtfilt


lib = ctypes.CDLL("scripts/bin/cmos.so")

lib.CMOS_create.argtypes = []
lib.CMOS_create.restype = ctypes.c_void_p

lib.CMOS_process_sample.argtypes = [ctypes.c_void_p, ctypes.c_float]
lib.CMOS_process_sample.restype = ctypes.c_float

lib.CMOS_destroy.argtypes = [ctypes.c_void_p]
lib.CMOS_destroy.restype = None


class PyCMOS:
    def __init__(self, fs, c, r, i_s, v_t):
        """
        Constructor: Calls the C function to create a Diode instance.
        """
        self._instance = lib.CMOS_create()
        print(f"C++ CMOS object created at memory address: {self._instance}")

    def process_sample(self, vin):
        """
        Calls the C function to process a single sample.
        """
        return lib.CMOS_process_sample(self._instance, ctypes.c_float(vin))

    def __del__(self):
        """
        Destructor: Called when the Python object is garbage collected.
        This is where we clean up the C++ memory.
        """
        print(f"Destroying C++ CMOS object at memory address: {self._instance}")
        lib.CMOS_destroy(self._instance)


if __name__ == "__main__":
    sample_rate = 44_100
    frequency = 440
    duration = 5 / frequency

    # t = np.linspace(0, duration, int(sample_rate * duration), endpoint=False)
    # input = 8 * np.sin(2 * np.pi * frequency * t)
    #
    # b, a = butter(1, 20 / (2 * sample_rate), btype="highpass", analog=False)
    cmos = PyCMOS(sample_rate, 1e-8, 2200, 200e-9, 0.02585)
    # output = [cmos.process_sample(sample) for sample in input]
    #
    # plt.plot(t, input, label="Input Signal", color="blue")
    # plt.plot(t, output, label="Output Signal (C++)", color="orange")

    input = np.linspace(-4.5, 4.5, 10000)
    output = [cmos.process_sample(sample) for sample in input]
    plt.plot(input, output)

    # plt.legend()
    # plt.plot(t, output_2, label="Output Signal (Python)")
    plt.show()
