#! /bin/bash

counter=20
disk_size=32G
numjobs=32
runtime=300
size=4k

function ran-test() {
	workload=$1
	filename=$2
	dirname=$3
	mkdir $dirname

	echo "$dirname start"
	echo "testing size $size"
	for((i=1; i<=$counter; i++))
	do
		echo "counter $i"
		fio -filename=$filename -direct=1 -rw=rand$workload -bs=$size -size=$disk_size -numjobs=$numjobs -runtime=$runtime -group_reporting --name=test > $dirname/$size.log$i
	done
	echo "$dirname finish"
}

function seq-test() {
	workload=$1
	filename=$2
	dirname=$3
	mkdir $dirname

	echo "$dirname start"
	echo "testing size $size"
	for((i=1; i<=$counter; i++))
	do
		echo "counter $i"
		fio -filename=$filename -direct=1 -rw=rand$workload -percentage_random=1 -bs=$size -size=$disk_size -numjobs=$numjobs -runtime=$runtime -group_reporting --name=test > $dirname/$size.log$i
	done
	echo "$dirname finish"
}

ran-test write /dev/vda ran-write
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

#seq-test read /dev/vdb seq-read-ssd
#cp analyze.sh seq-read-ssd/analyze.sh
#seq-test read /dev/vdc seq-read-hdd
#cp analyze.sh seq-read-hdd/analyze.sh
