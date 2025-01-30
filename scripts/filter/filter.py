import math
import numpy as np
import matplotlib.pyplot as plt
import scipy.signal as signal
from filter_lib import *

# This work is inspired by:
# https://tomverbeure.github.io/2020/09/30/Moving-Average-and-CIC-Filters.html
# https://tomverbeure.github.io/2020/12/20/Design-of-a-Multi-Stage-PDM-to-PCM-Decimation-Pipeline.html
# https://www.gibbard.me/cic_filters/cic_filters_ipython.html
# https://wirelesspi.com/cascaded-integrator-comb-cic-filters-a-staircase-of-dsp/
# https://www.dsprelated.com/showarticle/1337.php

def cic_decimated_mirrors(w, h, fs, decimation):
    range_size = len(w) // decimation

    output = []
    start = 0
    reverse = False
    n_range = 0
    while start < len(w):
        _w = w[start:start + range_size]
        _h = h[start:start + range_size]

        if reverse == True:
            _w = _w[::-1] - n_range*(fs / decimation/2)
            reverse = False
        else:
            _w = _w - n_range*(fs / decimation/2)
            reverse = True
        n_range += 1
        start += range_size
        output.append([_w, _h])
    return output

# Compensation function of CIC transfer function
# w = relative frequency, where 0.5 = nyquist frequency
# R = decimation factor
# M = CIC order
# N = Differential delay (e.g. D / R)
def cic_comp_function(R,M,N, w):
    if w == 0:
        return 1.0
    return ((M*R)**N)*(np.abs((np.sin(w/(2.*R))) / (np.sin((w*M)/2.)) ) **N)

def print_c_array(vals, value_format="{}", vals_per_line=4):
    vals_str = list(map(lambda v: value_format.format(v), vals))
    print("{")
    while len(vals_str):
        to_print = vals_str[:vals_per_line]
        vals_str = vals_str[vals_per_line:]
        print("    ", ", ".join(to_print) + ",")
    print("};")

f_s = 240000

# Optimal values:
# R=6 M=4 fir_downsample=3 passband=4000
# R=8 M=4 fir_downsample=2 passband=4000
# R=5 M=4 fir_downsample=3 passband=5000

CIC_R=5
CIC_M=4
cic_coeff = cic_filter(CIC_R, CIC_M)
w, h = signal.freqz(cic_coeff, fs=f_s, worN=1024)
cic_in = [1] + [0]*31
cic_out = signal.lfilter(cic_coeff, [1], cic_in)
print("CIC in", cic_in)
print("CIC out", cic_out)

# Design a lowpass filter so that it's stopband starts after 1/2 Niquist
# for 2-fold decimation afterwards
fir_order = 127
fir_downsample = 3
fs_fir = f_s / CIC_R
f_fir_niquist = fs_fir/2
f_fir_passband = 5000.0
f_fir_stopband = f_fir_niquist / fir_downsample
print("FIR passband", f_fir_passband)
print("FIR stopband", f_fir_stopband)
print("Final FS after decimation", fs_fir/2)

fir_passband_ripple = 0.3
N_FIR = fir_find_optimal_N(fs_fir, f_fir_passband, f_fir_stopband, fir_passband_ripple, 75, Nmin=3, Nstep=2, verbose=False)
print("FIR optimal N", N_FIR)
(fir_taps, fir_w, fir_h, fir_Rpb, fir_Rsb, fir_Hpb_min, fir_Hpb_max, fir_Hsb_max) = fir_calc_filter(
            fs_fir, f_fir_passband, f_fir_stopband, fir_passband_ripple, 75, N_FIR)
print("FIR taps")
print_c_array(fir_taps, value_format="{:.6f}")

fig, ax = plt.subplots(tight_layout=True)
ax.set_title(f"Frequency Response of CIC Filter (R={CIC_R}, M={CIC_M}) and FIR Filter (N={N_FIR}, D={fir_downsample})")
ax.axvline(f_s/2, color='black', linestyle=':', linewidth=0.8)
ax.set_ylabel("Amplitude in dB", color='C0')
ax.grid(True)
ax.set(xlabel="Frequency, relative", xlim=(0, f_s/2), ylim=(-120,6))

plt.plot([f_fir_passband,f_fir_passband],[-100,1], '--', label=f"FIR Passband")
plt.plot([f_fir_stopband,f_fir_stopband],[-100,-40], '--', label=f"FIR Stopband")
plt.plot([f_fir_niquist,f_fir_niquist],[-100,-1], '--', label=f"FIR Niquist")
plt.plot([fs_fir, fs_fir],[-100,-1], '--', label=f"FIR Fs")

ax.plot(w, 20 * np.log10(abs(h)), "red")
lobes = cic_decimated_mirrors(w, h, f_s, CIC_R)
for n in range(len(lobes)):
    color = f"C{n}"
    _w = lobes[n][0]
    _h = lobes[n][1]
    ax.plot(_w, 20 * np.log10(abs(_h)), color)

#ax.plot(fir_w, 20 * np.log10(abs(fir_h)), "C7")
ax.plot(fir_w/(2*np.pi)*fs_fir, 20 * np.log10(abs(fir_h)), "C7")


plt.show()
