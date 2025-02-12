import matplotlib.pyplot as plt
import scipy.signal as signal
from test_input_2 import *

class RegisteredEvent:
    def __init__(self, start, end, power):
        self.start = start
        self.end = end
        self.size = end - start
        self.center = (start + end) / 2
        self.power = power


    def affinity(self, other, other_offset=0):
        mean_size = (self.size + other.size) / 2
        distance = abs(self.center - (other.center + other_offset))

        affinity_perfect = 0.5
        affinity_max = 2.0

        # distance < mean_size*affinity_perfect -> 1
        # distance > mean_size*affinity_max -> 0

        if distance < mean_size*affinity_perfect:
            w = 1
        elif distance > mean_size*affinity_max:
            w = 0
        else:
            w = 1 - ((distance - mean_size*affinity_perfect) / (mean_size*(affinity_max - affinity_perfect)))
        # winner weights
        # return self.size*other.size * w # 0, 1
        # return (self.power + other.power) * w # 0, 1
        return other.size * w # 0, 1
        
        # loser weights
        # return max(self.power, other.power) * w # 0, -
        # return other.power * w #  -, 1
        # return (self.power + other.power) * other.size * w # -, 1
    
    def __str__(self):
        return f"({self.start}..{self.end})"

def _correlate_objects(a_obj, b_obj, offset, debug=False):
    sum = 0

    # Object b is shifted with offset to the right
    for a in a_obj:
        for b in b_obj:
            aff = a.affinity(b, offset)
            sum = sum + aff
            if debug and aff > 0:
                print(f"Match {aff} between {a} and {b} + {offset}")
    return sum

def correlate_objects(a_obj, b_obj, offsets):
    output = []
    for off in offsets:
        output.append(_correlate_objects(a_obj, b_obj, off))
    return output

def block_dc(input):
    output = []
    base = 8192.0
    pole = (base - 1)/base #0.9999
    gain = 1 / (1-pole)

    prev_dx = sum(input) // len(input)
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

def block_dc_int32(input):
    output = []
    base = 32768.0
    pole = 0.9999
    A = int(base*(1.0 - pole))
    print("base:", base, "pole:", pole)
    acc = 0
    prev_x = (sum(input) // len(input)) * base
    prev_y = 0
    for dx in input:
        acc = acc - prev_x
        prev_x = int(dx * base)
        acc = acc + prev_x
        acc -= A*prev_y
        prev_y = acc // base
        output.append(prev_y)
    return output

def median_binary(input, thr, size=5, len_thr=5):
    output = []
    objects = []
    in_obj = False
    obj_start = None
    obj_power = 0
    history = [0] * size
    sum = 0

    for index in range(len(input)):
        x = input[index]
        v = x > thr
        sum = sum - history.pop(0) + v
        history.append(v)

        v_med = sum > (size // 2)
        output.append(v_med)
        if v_med:
            if not in_obj:
                in_obj = True
                obj_start = index
                obj_power = x
            else:
                obj_power = obj_power + x
        else:
            if in_obj:
                in_obj = False
                obj_len = index - obj_start
                if obj_len >= len_thr:
                    objects.append(RegisteredEvent(obj_start, obj_start + obj_len, obj_power))

    return output, objects    

def binary_objdet(input, thr, size=5, len_thr=5):
    output = []
    objects = []
    in_obj = False
    obj_start = None
    obj_power = 0

    for index in range(len(input)):
        x = input[index]
        v = x > thr
        output.append(v)
        if v:
            if not in_obj:
                in_obj = True
                obj_start = index
                obj_power = x
            else:
                obj_power = max(obj_power, x)
        else:
            if in_obj:
                in_obj = False
                obj_len = index - obj_start
                if obj_len >= len_thr:
                    objects.append(RegisteredEvent(obj_start, obj_start + obj_len, obj_power))

    return output, objects    



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

a_ac = block_dc_int32(raw_a)
b_ac = block_dc_int32(raw_b)
print([int(x) for x in b_ac])
# a = [clip(x, 5, 50) for x in a_ac]
# b = [clip(x, 5, 50) for x in b_ac]
# a = [binary(x, 20) for x in a_ac]
# b = [binary(x, 20) for x in b_ac]
# a = signal.medfilt(a_ac, 5)
# b = signal.medfilt(b_ac, 5)
# a, a_obj = median_binary(a_ac, thr=5, len_thr=10)
# b, b_obj = median_binary(b_ac, thr=5, len_thr=10)
a, a_obj = binary_objdet(a_ac, thr=8, len_thr=10)
b, b_obj = binary_objdet(b_ac, thr=8, len_thr=10)
# a = raw_a
# b = raw_b

print("A objects:", len(a_obj))
print([str(x) for x in a_obj])
print("B objects:", len(b_obj))
print([str(x) for x in b_obj])

# evt1 = RegisteredEvent(100, 110, 1)
# for c in range(80, 130, 1):
#     evt2 = RegisteredEvent(c, c + 10)
#     print(f"{evt1.center} <--> {evt2.center} ==> {evt2.affinity(evt1)}")

a_len = len(a)
b_len = len(b)
ax_values = range(a_len)
b_offset = 5507 # 5295 # 5153 # 1636 
#2152 # 5295 
#a_len - 8344

_correlate_objects(a_obj, b_obj, b_offset, True)
c = correlate_objects(a_obj, b_obj, range(a_len-b_len))
cx_values = range(a_len - b_len)

print(f"Offset {b_offset/16} ms")
bx_values = range(b_offset, b_len + b_offset)

fig, axis = plt.subplots(3, 1, tight_layout=True, sharex=True)
axis[0].set_title(f"Sensor input")
axis[0].set_ylabel("Values", color='C0')
axis[0].grid(True)
axis[0].set(xlabel="Time, samples", xlim=(0, a_len))

axis[0].plot(ax_values, a, "red")
axis[0].plot(bx_values, b, "blue")

axis[1].set_title(f"Binary")
axis[1].set_ylabel("B. values", color='C0')
axis[1].set(xlabel="Offset, samples", xlim=(0, a_len))
axis[1].plot(ax_values, a_ac, "violet")
axis[1].plot(bx_values, b_ac, "cyan")

axis[2].set_title(f"Correlation")
axis[2].set_ylabel("C. values", color='C0')
axis[2].set(xlabel="Offset, samples", xlim=(0, a_len))
axis[2].plot(cx_values, c, "violet")

plt.show()
