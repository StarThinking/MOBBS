#! /bin/bash

counter=3 
bs=1
power=2

for size in 1k 2k 4k 8k 16k 32k 64k 128k 256k 512k 1M 2M 4M 8M 16M
do
	iopss=`cat "$size"* | grep iops | awk 'BEGIN{FS=",|:|="}{print $7}'`
	sum=0
	for iops in $iopss
	do
		sum=`expr $sum + $iops`
	done
	avg=`expr $sum / $counter`
	bw=`expr $avg \* $bs` 
	bs=`expr $bs \* $power`
	echo "$size avg iops = $avg, bw = $bw KB"
done
