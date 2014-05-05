#! /bin/bash
set -x

filebench_dir=/root/filebench-whobbs
dirname=`date +%Y%m%d%H%M%S`

echo "filebench test begin"

function before() {
  echo "before"
  rm /usr/local/libvirt/var/log/libvirt/qemu/Ubuntu.log
  touch /usr/local/libvirt/var/log/libvirt/qemu/Ubuntu.log
  /root/WHOBBS/6h+6s/create_pool.sh
  sleep 10
  rbd -p hdd-pool create volume --size 65536
  sleep 30
  rbd -p hdd-pool map volume
  sleep 10
  dd if=/dev/zero of=/dev/rbd1 bs=4M count=16384
  sleep 10
  virsh start Ubuntu
  sleep 60
}

function after() {
  name=$1
  echo "after"
  sleep 60
  scp -r 192.168.122.53:$filebench_dir/tmp-result/* ./result/$dirname
  sleep 30
  virsh destroy Ubuntu
  sleep 30
  cp /usr/local/libvirt/var/log/libvirt/qemu/Ubuntu.log ./result/"$dirname"/$name".log"
  sleep 5
  rbd unmap /dev/rbd1
  sleep 30
}

mkdir ./result/$dirname

/root/WHOBBS/6h+6s/create_pool.sh
sleep 10
rbd -p hdd-pool create volume --size 65536
sleep 10
virsh start Ubuntu
sleep 60
scp -r ../filebench-whobbs 192.168.122.53:~
sleep 10
ssh 192.168.122.53 "cd $filebench_dir; rm -rf tmp-result; mkdir tmp-result"
sleep 30
virsh destroy Ubuntu

if true;then
for workload in webserver fileserver varmail videoserver
do 
  for fs in xfs ext4
  do
  echo "test $workload $fs"
  before
  if [ $fs = xfs ] 
  then
    ssh 192.168.122.53 "mkfs.xfs -f /dev/vdb"
  else
    ssh 192.168.122.53 "mkfs.ext4 /dev/vdb"
  fi
  ssh 192.168.122.53 "mount /dev/vdb /mnt"
  sleep 10
  ssh 192.168.122.53 "filebench -f $filebench_dir/$workload.f > $filebench_dir/tmp-result/filebench-$workload-$fs.log"
  after "qemu"-$workload-$fs
  done
done

fi
