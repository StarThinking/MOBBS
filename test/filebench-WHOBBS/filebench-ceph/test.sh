! /bin/bash
#set -x

echo "filebench test begin"
mkdir result

for device in vdb vdc
do 
	for fs in xfs ext4
	do
		echo "mkfs $fs and mount"
		if [ $fs == xfs ]
		then mkfs.xfs -f /dev/$device
		else mkfs.ext4 /dev/$device			
		fi
		mount /dev/$device /mnt

		for workload in fileserver varmail webserver videoserver
		do
			echo "test $workload $device $fs"
			filebench -f ./$workload.f > ./result/"$workload"-"$device"-"$fs".log
		done
		
		echo "umount"
		umount /mnt
	done
done

