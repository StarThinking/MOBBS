#! /bin/bash

counter=1

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
	parts=6
	workload=$1
	filenames[1]=$2
	filenames[2]=$3
	filenames[3]=$4
	filenames[4]=$5
	filenames[5]=$6
	filenames[6]=$7
	dirname=$8
	mkdir $dirname

	echo "$dirname start"
	#for size in 1k 2k 4k 8k 16k 32k 64k 128k 256k 512k 1M 2M 4M 8M 16M 
	for size in 4k
	do
		echo "testing size $size"
		for((i=1; i<=$counter; i++))
		do
			echo "counter $i"
			for((j=1; j<=$parts; j++))
			do
				echo "start ${filenames[$j]}"
				fio -filename=${filenames[$j]} -direct=1 -rw=$workload -bs=$size -size=21G -numjobs=5 -runtime=120 -group_reporting --name=test > $dirname/$size.part$j.log$i &
				tids[$j]=$!
			done
		
			for((k=1; k<=$parts; k++))
			do
				wait ${tids[$k]}
				echo "finish ${filenames[$k]}"
			done
		done
	done
	echo "$dirname finish"
}

#ran-test read /dev/vdb ran-read-hdd
#ran-test write /dev/vdb ran-write-hdd
#ran-test read /dev/vdb ran-read-ssd
#ran-test write /dev/vdb ran-write-ssd

#seq-test read /dev/vdb /dev/vdc /dev/vdd /dev/vde /dev/vdf /dev/vdg seq-read-ssd
seq-test write /dev/vdb /dev/vdc /dev/vdd /dev/vde /dev/vdf /dev/vdg seq-write-ssd-4k



