import matplotlib.pyplot as plt
import scipy.signal as signal
import test_input_3 as test_input
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


    def affinity(self, other, other_offset=0):
        mean_size = (self.size + other.size) / 2
        distance = abs(self.center - (other.center + other_offset))

        # affinity_perfect = 0.5
        # affinity_max = 2.0
        affinity_perfect = 1.0
        affinity_max = 5.0

        # distance < mean_size*affinity_perfect -> 1
        # distance > mean_size*affinity_max -> 0

        if distance < mean_size*affinity_perfect:
            w = 1
        elif distance > mean_size*affinity_max:
            w = 0
        else:
            w = 1 - ((distance - mean_size*affinity_perfect) / (mean_size*(affinity_max - affinity_perfect)))
        # winner weights
        # return max(self.ampl, other.ampl) * w # +-
        # return other.ampl * w # +
        # return (self.ampl + other.ampl) * other.size * w # +
        # return w
    
        # loser weights
        return self.size*other.size * w # -
        # return (self.ampl + other.ampl) * w # -
        # return other.size * w # -
        # return (self.power + other.power) * w # -+
    
    def __str__(self):
        return f"({self.start}..{self.end})"

def _correlate_objects(a_obj, b_obj, offset, debug=False):
    sum = 0
    time_delta = 16 * 50 # Throw away anything that is 50ms off

    # Object b is shifted with offset to the right
    for a in a_obj:
        _left = a.center - offset - time_delta
        _right = a.center - offset + time_delta
        left = bisect.bisect_left(b_obj, _left, key=lambda x: x.center)
        right = bisect.bisect_right(b_obj, _right, key=lambda x: x.center)
        if left >= len(b_obj):
            continue
        b_range = range(left, right)
        # print(a.center, b_range)

        # b_range = range(len(b_obj))
        for idx in b_range:
            b = b_obj[idx]
            # print(a.center, b.center + offset)
            aff = a.affinity(b, offset)
            sum = sum + aff
            if debug and aff > 0:
                # print(a.center, b.center + offset)
                print(f"Match {aff} between {a} and {b} + {offset}")
    return sum

def correlate_objects(a_obj, b_obj, offsets):
    output = []
    for off in offsets:
        output.append(_correlate_objects(a_obj, b_obj, off))
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

input = test_input.series2
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

b_offset = 10100
print(f"Offset {b_offset/ms} ms")
_correlate_objects(a_obj, b_obj, b_offset, True)
# exit(1)
cx_values = range(correlate_interval)
c = correlate_objects(a_obj, b_obj, cx_values)
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
