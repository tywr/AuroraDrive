import os
import ctypes
import math

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


import matplotlib.pyplot as plt

x = np.linspace(0, 3, 10000)
y_toms917 = np.array([lib2.wrightomega_double(xi) for xi in x])
y_small = np.array([omega_small(xi) for xi in x])
y_4 = np.array([omega4(xi) for xi in x])
y_cured = np.array([omega_cured(xi) for xi in x])


# plt.plot(x, y_cured, label="omega_cured", linestyle="--")
# plt.plot(x, y_toms917, label="toms917")
# plt.plot(x, y_4, label="omega4", linestyle=":")
# plt.plot(x, y_small, label="taylor expansion 5", linestyle="--")
# plt.plot(x, y_toms917 - y_small, color="red", label="5th order expansion")
# plt.plot(x, y_toms917 - y_4, color="green", label="omega4", linestyle=":")
# plt.plot(x, y_cured, label="cured")
# plt.plot(
#     x,
#     10* (y_toms917 - y_cured),
#     color="green",
#     label="omega_cured",
#     linestyle="--",
# )
plt.legend()
plt.show()
