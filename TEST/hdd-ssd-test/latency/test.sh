#! /bin/bash

counter=5
disk_size=64G
runtime=120

function ran-test() {
	workload=$1
	size=$2
	filename=$3
	dirname=$4
	mkdir $dirname

	echo "$dirname start"
	for jobs in 1 2 4 6 8 10 12 14 16 18 20 22 24 26 28 30 32 34 36 38 40 
	do
		echo "testing jobs $jobs"
		for((i=1; i<=$counter; i++))
		do
			echo "counter $i"
			fio -filename=$filename -direct=1 -rw=rand$workload -bs=$size -size=$disk_size -numjobs=$jobs -runtime=$runtime -group_reporting --name=test > $dirname/$jobs.log$i
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
			fio -filename=$filename -direct=1 -rw=rand$workload -percentage_random=1 -bs=$size -size=$disk_size -numjobs=$numjobs -runtime=$runtime -group_reporting --name=test > $dirname/$size.log$i
		done
	done
	echo "$dirname finish"
}

ran-test write 4k /dev/vdb ran-write-hdd





