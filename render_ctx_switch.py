#!/usr/bin/python3

import sys
import re
import matplotlib.pyplot as plt
import numpy as np

num="([1234567890.,]+)"
re_cpu_core=re.compile(num+".*cpu_core")
re_time_elapsed=re.compile(num+".*time elapsed")

def match_line(line, rec):
    data = rec.findall(line)
    if data and len(data)==1:
        d = float(data[0].replace(',', ''))
        return d
    return None

def render_one_result(ax, fn):
    core_counter=np.array([], dtype=float)
    time_counter=np.array([], dtype=float)

    with open(fn) as f:
        lines=f.readlines()

    for line in lines:
        d = match_line(line, re_cpu_core)
        if d:
            core_counter = np.append(core_counter, d)
            continue
        d = match_line(line, re_time_elapsed)
        if d:
            time_counter = np.append(time_counter, d)
            continue

    length = np.size(core_counter)
    if length != np.size(time_counter):
        print(f"format error, core/time counter is {length}/{np.size(time_counter)}")
        sys.exit(-1)

    l = ax.plot(np.arange(2, 2+length), core_counter/time_counter, label=fn)
    return l

fig, ax=plt.subplots()
ax.set(xlabel="num_process", ylabel="ave itlb misses", title="tlbmiss with switch process number")
ax.grid()

ls = []
for fn in sys.argv[1:]:
    l = render_one_result(ax, fn)
    ls.append(l)

fig.legend()
plt.show()
