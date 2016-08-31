#!/bin/bash

NCPU=`nproc`
TIME=2

i=0
while [ $i -lt $NCPU ]; do
	let i=i+1
	timeout -s 2 $TIME ./amdahl -b -p 99 -s 1 -q $i
done
