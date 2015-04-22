#!/bin/bash

ifstat > network.log &

count=0
while [ $count -lt 59 ]
do
	ps -e -o 'pid,comm,pcpu,rsz' | grep Monitor >> cpu_mem.log
	sleep 5s
	count=`expr ${count} + 1`
done

kill %1

