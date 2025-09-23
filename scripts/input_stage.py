from sympy import symbols, I, solve

R1, R2, C1 = symbols("R1 R2 C1")
V_in, V_out = symbols("V_in V_out")
T, z = symbols("T z")
s = symbols("s")  # Complex frequency

Z_parallel = R2 / (s * C1 * R2 + 1)

eq = V_in * Z_parallel / (R1 + Z_parallel) - V_out

# Solve for the output voltage Vo in terms of Vi
solution = solve(eq, V_out)

bilinear_transform = (2 / T) * (1 - z ** (-1)) / (1 + z ** (-1))
print(bilinear_transform)

# Get the final transfer function
transfer_function = solution[0]

values = {
    R1: 1,
    C1: 1,
    R2: 1,
    T: 1,
}
print(transfer_function.subs(s, bilinear_transform).subs(values).simplify())
