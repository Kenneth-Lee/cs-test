#!/usr/bin/bash

for i in `seq 0 100`; do
	#sudo perf stat --cpu 2 -e itlb_misses.walk_completed ./ctx_switch -l 1 -M 10000 -n $i
	sudo perf stat --cpu 2 -e itlb_misses.walk_completed ./ctx_switch -l 1 -M 10000 -i $i
done
