#!/usr/bin/bash

for i in `seq 2 20`; do
	sudo perf stat -e frontend_retired.itlb_miss ./ctx_switch -M 10000 -n $i
done
