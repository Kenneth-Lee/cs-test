#!/usr/bin/bash

for i in `seq 2 20`; do
	sudo perf stat -e itlb_misses.walk_completed ./ctx_switch -M 10000 -n $i
done
