#!/bin/sh

#get hardware configure
sudo dmidecode1 -q 
dmesg

#get software configure
cat /proc/cmdline
cat /proc/config.gz
uname -a

#get runtime info
cat /proc/meminfo
cat /proc/interrupts
ps -eLf
lsof
ifconfig
vmstat
iostat
