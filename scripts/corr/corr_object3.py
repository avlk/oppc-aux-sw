import matplotlib.pyplot as plt
import scipy.signal as signal
import test_input_5 as test_input
import bisect
import random

class RegisteredEvent:
    def __init__(self, start, len, ampl, power):
        self.start = start
        self.end = start + len
        self.size = len
        self.center = start + len / 2
        self.ampl = ampl
        self.power = power


    def like(self, other):
        max_size = max(self.size, other.size)
        min_size = min(self.size, other.size)
        max_ampl = max(self.ampl, other.ampl)
        min_ampl = min(self.ampl, other.ampl)
        max_power = max(self.power, other.power)
        min_power = min(self.power, other.power)
    
        w = (max_size / min_size) < 2
    
        return w

    def __str__(self):
        return f"({self.start}..{self.end})"


def correlate_objects(a_obj, b_obj, max_delay, bins):
    output = [0] * bins
    for a in a_obj:
        left = bisect.bisect_left(b_obj, a.center, key=lambda x: x.center)
        if left >= len(b_obj):
            continue
        b_range = range(left, len(b_obj))
        for idx in b_range:
            b = b_obj[idx]
            delay = b.center - a.center
            if delay < 0:
                continue
            if delay > max_delay:
                break
            bin = int((delay * bins) // max_delay)
            # if b.like(a):
            output[bin] += 1
    return output

def load_obj(test_data, source, range_from=None, range_to=None):
    output = []
    if range_from and range_to:
        t = test_data[range_from:range_to]
    elif range_from:
        t = test_data[range_from:]
    elif range_to:
        t = test_data[:range_to]
    else:       
        t = test_data
    for x in t:
        if x['source'] == source:
            output.append(RegisteredEvent(x['start'], x['len'], x['ampl'], x['power']))
    return output

def obj_to_scatter(sequence: list[RegisteredEvent], offset=0, y=0):
    ox = []
    oy = []
    os = []
    for a in sequence:
        ox.append(a.center + offset)
        oy.append(y + random.randint(1,10))
        os.append(a.power / 100)
    return ox, oy, os

input = test_input.series4
print("Input objects:", len(input))

range_from = None
range_to = None
a_obj = load_obj(input, source=0, range_from=range_from, range_to=range_to)
b_obj = load_obj(input, source=1, range_from=range_from, range_to=range_to)
ts_first = input[0]['start']
ts_last  = input[-1]['start'] + input[-1]['len']
ms = 16
correlate_interval = 1000*ms

print("A objects:", len(a_obj))
# print([str(x) for x in a_obj])
print("B objects:", len(b_obj))
# print([str(x) for x in b_obj])

# evt1 = RegisteredEvent(100, 110, 1)
# for c in range(80, 130, 1):
#     evt2 = RegisteredEvent(c, c + 10)
#     print(f"{evt1.center} <--> {evt2.center} ==> {evt2.affinity(evt1)}")

b_offset = 830 # 2182 # 1442 # 1520
print(f"Offset {b_offset/ms} ms")
# exit(1)
cx_values = range(0, correlate_interval, 4*ms)
c = correlate_objects(a_obj, b_obj, correlate_interval, len(cx_values))
print("Done correlating")

xa, ya, sa = obj_to_scatter(a_obj)
xb, yb, sb = obj_to_scatter(b_obj, offset=b_offset, y=1)


fig, axis = plt.subplots(2, 1, tight_layout=True)

axis[0].scatter(xa, ya, sa, color="blue")
axis[0].scatter(xb, yb, sb, color="red")
axis[0].set_xlabel("Timestamp")
axis[0].set_ylabel("Amplitude")

axis[1].set_title(f"Correlation")
axis[1].set_ylabel("C. values", color='C0')
axis[1].set(xlabel="Offset, samples")
axis[1].plot(cx_values, c, "violet")

plt.show()
