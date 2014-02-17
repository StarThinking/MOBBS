#! /bin/bash

counter=3

function ran-test() {
	workload=$1
	filename=$2
	dirname=$3
	mkdir $dirname

	echo "$dirname start"
	for size in 1k 2k 4k 8k 16k 32k 64k 128k 256k 512k 1M 2M 4M 8M 16M 
	do
		echo "testing size $size"
		for((i=1; i<=$counter; i++))
		do
			echo "counter $i"
			fio -filename=$filename -direct=1 -rw=rand$workload -bs=$size -size=128G -numjobs=30 -runtime=120 -group_reporting --name=test > $dirname/$size.log$i
		done
	done
	echo "$dirname finish"
}

function seq-test() {
	workload=$1
	filename=$2
	dirname=$3
	mkdir $dirname

	echo "$dirname start"
	for size in 1k 2k 4k 8k 16k 32k 64k 128k 256k 512k 1M 2M 4M 8M 16M 
	do
		echo "testing size $size"
		for((i=1; i<=$counter; i++))
		do
			echo "counter $i"
			fio -filename=$filename -direct=1 -rw=rand$workload -percentage_random=1 -bs=$size -size=128G -numjobs=30 -runtime=120 -group_reporting --name=test > $dirname/$size.log$i
		done
	done
	echo "$dirname finish"
}

#ran-test write /dev/vdb ran-write-hdd
#ran-test write /dev/vdc ran-write-ssd
ran-test write /dev/vdb ran-write-hdd-new
ran-test write /dev/vdc ran-write-ssd-new



