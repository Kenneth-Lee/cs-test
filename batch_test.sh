#!/bin/bash

NCPU=`nproc`
TIME=3

echo "98:2"
echo "use pthread_spinlock"

i=0
while [ $i -lt $NCPU ]; do
	let i=i+1
	timeout -s 2 $TIME ./amdahl -b -p 98 -s 2 -l 0 -q $i
done

echo "use mcs lock"

i=0
while [ $i -lt $NCPU ]; do
	let i=i+1
	timeout -s 2 $TIME ./amdahl -b -p 98 -s 2 -l 1 -q $i
done


echo "97:3"
i=0
while [ $i -lt $NCPU ]; do
	let i=i+1
	timeout -s 2 $TIME ./amdahl -b -p 97 -s 3 -l 0 -q $i
done

echo "use mcs lock"

i=0
while [ $i -lt $NCPU ]; do
	let i=i+1
	timeout -s 2 $TIME ./amdahl -b -p 97 -s 3 -l 1 -q $i
done

echo "90:10"
i=0
while [ $i -lt $NCPU ]; do
	let i=i+1
	timeout -s 2 $TIME ./amdahl -b -p 90 -s 10 -l 0 -q $i
done

echo "use mcs lock"

i=0
while [ $i -lt $NCPU ]; do
	let i=i+1
	timeout -s 2 $TIME ./amdahl -b -p 90 -s 10 -l 1 -q $i
done
