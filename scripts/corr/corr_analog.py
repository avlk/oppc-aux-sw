import matplotlib.pyplot as plt
import scipy.signal as signal
from test_input_2 import *


def block_dc(input):
    output = []
    base = 8192.0
    pole = (base - 1)/base #0.9999
    gain = 1 / (1-pole)

    print("pole: ", pole)
    prev_dx = sum(input)/len(input)
    prev_iy = 0
    for dx in input:
        # differentiator
        dy = dx - prev_dx
        prev_dx = dx
        # integrator
        ix = dy
        y = pole*prev_iy + (1-pole)*ix
        prev_iy = y
        output.append(y*gain)
    return output

def clip(val, min, max):
    if val < min:
        return 0
    if val > max:
        return max
    return val 

def binary(val, level):
    if val >= level:
        return 1
    return 0

a = block_dc(raw_a)
b = block_dc(raw_b)
a = [clip(x, 10, 50) for x in a]
b = [clip(x, 10, 50) for x in b]
# a = [binary(x, 20) for x in a]
# b = [binary(x, 20) for x in b]
# a = raw_a
# b = raw_b

c = signal.correlate(a, b, mode='valid')

a_len = len(a)
b_len = len(b)
c_len = len(c)
ax_values = range(a_len)
# b_offset = 173*16
b_offset = a_len - 8344

print(f"Offset {b_offset/16} ms")
bx_values = range(a_len - b_len - b_offset, a_len - b_offset)
cx_values = range(a_len - c_len, a_len)

fig, axis = plt.subplots(2, 1, tight_layout=True)
axis[0].set_title(f"Sensor input")
axis[0].set_ylabel("Values", color='C0')
axis[0].grid(True)
axis[0].set(xlabel="Time, samples", xlim=(0, a_len))

axis[0].plot(ax_values, a, "red")
axis[0].plot(bx_values, b, "blue")
#axis[0].plot(ax_values, a_dc, "violet")
#axis[0].plot(bx_values, b_dc, "cyan")

axis[1].set_title(f"Correlation")
axis[1].set_ylabel("C. values", color='C0')
axis[1].set(xlabel="Offset, samples", xlim=(0, a_len))
axis[1].plot(cx_values, c, "black")

plt.show()
