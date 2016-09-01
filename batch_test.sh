#!/bin/bash

NCPU=`nproc`
TIME=3

echo "use pthread_spinlock"

i=0
while [ $i -lt $NCPU ]; do
	let i=i+1
	timeout -s 2 $TIME ./amdahl -b -p 99 -s 1 -l 0 -q $i
done

echo "use mcs lock"

i=0
while [ $i -lt $NCPU ]; do
	let i=i+1
	timeout -s 2 $TIME ./amdahl -b -p 99 -s 1 -l 1 -q $i
done
