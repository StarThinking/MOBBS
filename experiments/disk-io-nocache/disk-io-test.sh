#! /bin/bash

counter=3
disk_size=64G
numjobs=32
runtime=120

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
			fio -filename=$filename -direct=1 -rw=rand$workload -bs=$size -size=$disk_size -numjobs=$numjobs -runtime=$runtime -group_reporting --name=test > $dirname/$size.log$i
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

#ran-test write /dev/vdb ran-write-ssd
#cp analyze.sh ran-write-ssd/analyze.sh
#ran-test write /dev/vdc ran-write-hdd
#cp analyze.sh ran-write-hdd/analyze.sh

#seq-test write /dev/vdb seq-write-ssd
#cp analyze.sh seq-write-ssd/analyze.sh
#seq-test write /dev/vdc seq-write-hdd
#cp analyze.sh seq-write-hdd/analyze.sh

#ran-test read /dev/vdb ran-read-ssd
#cp analyze.sh ran-read-ssd/analyze.sh
#ran-test read /dev/vdc ran-read-hdd
#cp analyze.sh ran-read-hdd/analyze.sh

seq-test read /dev/vdb seq-read-ssd
cp analyze.sh seq-read-ssd/analyze.sh
seq-test read /dev/vdc seq-read-hdd
cp analyze.sh seq-read-hdd/analyze.sh
