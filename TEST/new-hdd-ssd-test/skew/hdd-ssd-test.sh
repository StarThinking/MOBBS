#! /bin/bash

counter=3

function skew-ran-test() {
	workload=$1
	filename=$2
	dirname=$3
	mkdir $dirname

	echo "$dirname start"
	for size in 1k 2k 4k 8k 16k 32k 64k 128k 256k 512k 1M 2M 4M 8M 
	do
		echo "testing size $size"
		for((i=1; i<=$counter; i++))
		do
			echo "counter $i"
			fio -filename=$filename -direct=1 -rw=rand$workload -random_distribution=zipf:1.5 -bs=$size -size=128G -numjobs=30 -runtime=120 -group_reporting --name=test > $dirname/$size.log$i
		done
	done
	echo "$dirname finish"
}

function skew-seq-test() {
	workload=$1
	filename=$2
	dirname=$3
	mkdir $dirname

	echo "$dirname start"
	for skew in 1.5 2.0 2.5 3.0 3.5 4.0
	do
		echo "testing skew $skew"
		for((i=1; i<=$counter; i++))
		do
			echo "counter $i"
			fio -filename=$filename -direct=1 -rw=rand$workload -percentage_random=1  -random_distribution=zipf:$skew -bs=4k -size=128G -numjobs=30 -runtime=120 -group_reporting --name=test > $dirname/$skew.log$i
		done
	done
	echo "$dirname finish"
}

skew-ran-test read /dev/vdb hdd-iosize-ran-read-zipf1.5
skew-ran-test read /dev/vdc ssd-iosize-ran-read-zipf1.5


