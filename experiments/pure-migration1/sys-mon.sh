#!/bin/bash

get_cpu()
{
	cat /proc/stat | grep '^cpu ' | awk '{idle=$5; total=$2+$3+$4+$5+$6+$7} END {printf "%s %f", idle, total}'
}

watch_cpu()
{
	begin_t=`get_cpu`
	sleep $1
	end_t=`get_cpu`
	echo $begin_t $end_t | awk '{idle=$3-$1; total=$4-$2} END {percent=100-(idle*100/total);print percent}'

}

watch_mem()
{
	total=`cat /proc/meminfo | grep "MemTotal" | awk '{print $2}'`
	free=`cat /proc/meminfo |grep "MemFree"|awk '{print $2}'`
	echo $free $total | awk '{percent=100-$1/$2*100; print percent}'
}

ifstat > network.info &
while true
do
	watch_cpu 1m >> cpu.info
	watch_mem >> memory.info
done
