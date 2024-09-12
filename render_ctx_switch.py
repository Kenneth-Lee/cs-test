#!/usr/bin/python3

import sys
import re
import matplotlib.pyplot as plt

num="([1234567890.,]+)"
re_cpu_core=re.compile(num+".*cpu_core")
re_time_elapsed=re.compile(num+".*time elapsed")

def match_line(a, rec, line):
    data = rec.findall(line)
    if data and len(data)==1:
        d = float(data[0].replace(',', ''))
        a.append(d)
        return True
    return False

def render_one_result(ax, fn):
    core_counter=[]
    time_counter=[]
    ave_core_counter=[]
    x=[]

    with open(fn) as f:
        lines=f.readlines()

    for line in lines:
        if match_line(core_counter, re_cpu_core, line):
            continue
        else:
            match_line(time_counter, re_time_elapsed, line)

    if len(core_counter) != len(time_counter):
        print(f"format error, core/time counter is {core_counter}/{time_counter}")
        sys.exit(-1)

    for i in range(len(core_counter)):
        ave_core_counter.append(core_counter[i]/time_counter[i]) # assume no zero in time_counter
        x.append(i+2)

    l = ax.plot(x, ave_core_counter, label=fn)
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
