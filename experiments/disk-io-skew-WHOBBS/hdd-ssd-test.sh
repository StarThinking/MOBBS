#! /bin/bash

counter=3
disk_size=32G
numjobs=32
runtime=120

function iosize-test() {
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
			fio -filename=$filename -direct=1 -rw=rand$workload -random_distribution=zipf:1.5 -bs=$size -size=$disk_size -numjobs=$numjobs -runtime=$runtime -group_reporting --name=test > $dirname/$size.log$i
		done
	done
	echo "$dirname finish"
}

function skew-ran-test() {
	workload=$1
	filename=$2
	dirname=$3
	mkdir $dirname

	echo "$dirname start"
	for skew in 1.25 1.5 1.75 2.0 2.25 2.5 2.75 3.0 3.25 3.5 3.75 4.0
	do
		echo "testing skew $skew"
		for((i=1; i<=$counter; i++))
		do
			echo "counter $i"
			fio -filename=$filename -direct=1 -rw=rand$workload -random_distribution=zipf:$skew -bs=4k -size=$disk_size -numjobs=$numjobs -runtime=$runtime -group_reporting --name=test > $dirname/$skew.log$i
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
	for skew in 1.25 1.5 1.75 2.0 2.25 2.5 2.75 3.0 3.25 3.5 3.75 4.0
	do
		echo "testing skew $skew"
		for((i=1; i<=$counter; i++))
		do
			echo "counter $i"
			fio -filename=$filename -direct=1 -rw=rand$workload -percentage_random=1  -random_distribution=zipf:$skew -bs=4k -size=$disk_size -numjobs=$numjobs -runtime=$runtime -group_reporting --name=test > $dirname/$skew.log$i
		done
	done
	echo "$dirname finish"
}

#skew-seq-test write /dev/vdb hdd-skew-seq-write-4k
skew-seq-test write /dev/vda ssd-skew-seq-write-4k

#iosize-test write /dev/vdb hdd-iosize-write-zipf1.5-ran
#iosize-test write /dev/vdc ssd-iosize-write-zipf1.5-ran

#skew-ran-test write /dev/vdb hdd-skew-ran-write-4k
skew-ran-test write /dev/vda ssd-skew-ran-write-4k

#iosize-test read /dev/vdb hdd-iosize-read-zipf1.5-ran
#iosize-test read /dev/vdc ssd-iosize-read-zipf1.5-ran



