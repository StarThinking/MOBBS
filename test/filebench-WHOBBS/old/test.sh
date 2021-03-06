#! /bin/bash
#set -x

echo "filebench test begin"

for device in vdb vdc
do 
	for fs in xfs ext4
	do
		echo "mkfs $fs ad mount"
		if [ $fs == xfs ]
		then mkfs.xfs -f /dev/$device
		else mkfs.ext4 /dev/$device			
		fi
		mount /dev/$device /mnt

		for workload in fileserver varmail webserver webproxy videoserver
		do
			echo "test $workload $device $fs"
			filebench -f /root/workloads/$workload.f > ./"$workload"-"$device"-"$fs".log
		done
		
		echo "umount"
		umount /mnt
	done
done

