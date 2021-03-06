#! /bin/bash
#for i in 19 20 6 15 101 102;do scp /usr/local/etc/ceph/ceph.conf 10.0.0.$i:/usr/local/etc/ceph/ceph.conf;done
for i in 19 20 6 15 101 102;do scp /usr/local/etc/ceph/ceph.conf 10.0.0.$i:/usr/local/etc/ceph/ceph.conf;done

rm -rf /var/lib/ceph
mkdir -p /var/lib/ceph/mon/ceph-0

service ceph killall
ssh 10.0.0.19 'service ceph killall; umount /var/lib/ceph/osd/*; umount /srv/ceph/journals/*'
ssh 10.0.0.20 'service ceph killall; umount /var/lib/ceph/osd/*; umount /srv/ceph/journals/*'
ssh 10.0.0.6 'service ceph killall; umount /var/lib/ceph/osd/*; umount /srv/ceph/journals/*'
ssh 10.0.0.15 'service ceph killall; umount /var/lib/ceph/osd/*; umount /srv/ceph/journals/*'
ssh 10.0.0.101 'service ceph killall; umount /var/lib/ceph/osd/*; umount /srv/ceph/journals/*'
ssh 10.0.0.102 'service ceph killall; umount /var/lib/ceph/osd/*; umount /srv/ceph/journals/*'

ssh 10.0.0.19 mkfs.xfs /dev/sdd1 -f
ssh 10.0.0.19 mkfs.xfs /dev/sdf1 -f
ssh 10.0.0.19 mkfs.xfs /dev/sdg1 -f
ssh 10.0.0.19 mkfs.xfs /dev/sda1 -f
ssh 10.0.0.19 mkfs.xfs /dev/sdb1 -f
ssh 10.0.0.19 mkfs.xfs /dev/sdc1 -f
ssh 10.0.0.20 mkfs.xfs /dev/sdc1 -f
ssh 10.0.0.20 mkfs.xfs /dev/sdf1 -f
ssh 10.0.0.20 mkfs.xfs /dev/sdh1 -f
ssh 10.0.0.20 mkfs.xfs /dev/sda1 -f
ssh 10.0.0.20 mkfs.xfs /dev/sdb1 -f
ssh 10.0.0.20 mkfs.xfs /dev/sdg1 -f
ssh 10.0.0.6  mkfs.xfs /dev/sdb1 -f
ssh 10.0.0.6  mkfs.xfs /dev/sdc1 -f
ssh 10.0.0.6  mkfs.xfs /dev/sdd1 -f
ssh 10.0.0.15 mkfs.xfs /dev/sdb1 -f
ssh 10.0.0.15 mkfs.xfs /dev/sdc1 -f
ssh 10.0.0.15 mkfs.xfs /dev/sdd1 -f
ssh 10.0.0.101 mkfs.xfs /dev/sdb1 -f
ssh 10.0.0.101 mkfs.xfs /dev/sdc1 -f
ssh 10.0.0.101 mkfs.xfs /dev/sdd1 -f
ssh 10.0.0.102 mkfs.xfs /dev/sdb1 -f
ssh 10.0.0.102 mkfs.xfs /dev/sdc1 -f
ssh 10.0.0.102  mkfs.xfs /dev/sdd1 -f

ssh 10.0.0.19 mkfs.xfs /dev/sde1 -f
ssh 10.0.0.19 mkfs.xfs /dev/sde2 -f
ssh 10.0.0.19 mkfs.xfs /dev/sde3 -f
ssh 10.0.0.19 mkfs.xfs /dev/sda2 -f
ssh 10.0.0.19 mkfs.xfs /dev/sdb2 -f
ssh 10.0.0.19 mkfs.xfs /dev/sdc2 -f
ssh 10.0.0.20 mkfs.xfs /dev/sde1 -f
ssh 10.0.0.20 mkfs.xfs /dev/sde2 -f
ssh 10.0.0.20 mkfs.xfs /dev/sde3 -f
ssh 10.0.0.20 mkfs.xfs /dev/sda2 -f
ssh 10.0.0.20 mkfs.xfs /dev/sdb2 -f
ssh 10.0.0.20 mkfs.xfs /dev/sdg2 -f
ssh 10.0.0.6  mkfs.xfs /dev/sde1 -f
ssh 10.0.0.6  mkfs.xfs /dev/sde2 -f
ssh 10.0.0.6  mkfs.xfs /dev/sde3 -f
ssh 10.0.0.15 mkfs.xfs /dev/sde1 -f
ssh 10.0.0.15 mkfs.xfs /dev/sde2 -f
ssh 10.0.0.15 mkfs.xfs /dev/sde3 -f
ssh 10.0.0.101 mkfs.xfs /dev/sde1 -f
ssh 10.0.0.101 mkfs.xfs /dev/sde2 -f
ssh 10.0.0.101 mkfs.xfs /dev/sde3 -f
ssh 10.0.0.102 mkfs.xfs /dev/sde1 -f
ssh 10.0.0.102 mkfs.xfs /dev/sde2 -f
ssh 10.0.0.102 mkfs.xfs /dev/sde3 -f

for i in 19 20 6 15 101 102; do ssh 10.0.0.$i rm -rf /var/lib/ceph; done

for i in 0 6 12 18 20 22;do ssh 10.0.0.19 mkdir -p /var/lib/ceph/osd/ceph-$i;done
for i in 1 7 13 19 21 23;do ssh 10.0.0.20 mkdir -p /var/lib/ceph/osd/ceph-$i;done
for i in 2 8 14;do ssh 10.0.0.6 mkdir -p /var/lib/ceph/osd/ceph-$i;done
for i in 3 9 15;do ssh 10.0.0.15 mkdir -p /var/lib/ceph/osd/ceph-$i;done
for i in 4 10 16;do ssh 10.0.0.101 mkdir -p /var/lib/ceph/osd/ceph-$i;done
for i in 5 11 17;do ssh 10.0.0.102 mkdir -p /var/lib/ceph/osd/ceph-$i;done

ssh 10.0.0.19 mount /dev/sdd1 /var/lib/ceph/osd/ceph-0
ssh 10.0.0.19 mount /dev/sdf1 /var/lib/ceph/osd/ceph-6
ssh 10.0.0.19 mount /dev/sdg1 /var/lib/ceph/osd/ceph-12
ssh 10.0.0.19 mount /dev/sda1 /var/lib/ceph/osd/ceph-18
ssh 10.0.0.19 mount /dev/sdb1 /var/lib/ceph/osd/ceph-20
ssh 10.0.0.19 mount /dev/sdc1 /var/lib/ceph/osd/ceph-22
ssh 10.0.0.20 mount /dev/sdc1 /var/lib/ceph/osd/ceph-1
ssh 10.0.0.20 mount /dev/sdf1 /var/lib/ceph/osd/ceph-7
ssh 10.0.0.20 mount /dev/sdh1 /var/lib/ceph/osd/ceph-13
ssh 10.0.0.20 mount /dev/sda1 /var/lib/ceph/osd/ceph-19
ssh 10.0.0.20 mount /dev/sdb1 /var/lib/ceph/osd/ceph-21
ssh 10.0.0.20 mount /dev/sdg1 /var/lib/ceph/osd/ceph-23
ssh 10.0.0.6  mount /dev/sdb1 /var/lib/ceph/osd/ceph-2
ssh 10.0.0.6  mount /dev/sdc1 /var/lib/ceph/osd/ceph-8
ssh 10.0.0.6  mount /dev/sdd1 /var/lib/ceph/osd/ceph-14
ssh 10.0.0.15 mount /dev/sdb1 /var/lib/ceph/osd/ceph-3
ssh 10.0.0.15 mount /dev/sdc1 /var/lib/ceph/osd/ceph-9
ssh 10.0.0.15 mount /dev/sdd1 /var/lib/ceph/osd/ceph-15
ssh 10.0.0.101 mount /dev/sdb1 /var/lib/ceph/osd/ceph-4
ssh 10.0.0.101 mount /dev/sdc1 /var/lib/ceph/osd/ceph-10
ssh 10.0.0.101 mount /dev/sdd1 /var/lib/ceph/osd/ceph-16
ssh 10.0.0.102 mount /dev/sdb1 /var/lib/ceph/osd/ceph-5
ssh 10.0.0.102 mount /dev/sdc1 /var/lib/ceph/osd/ceph-11
ssh 10.0.0.102 mount /dev/sdd1 /var/lib/ceph/osd/ceph-17

for i in 19 20 6 15 101 102; do ssh 10.0.0.$i rm -rf /srv/ceph/journals; done

for i in 0 6 12 18 20 22;do ssh 10.0.0.19 mkdir -p /srv/ceph/journals/osd$i;done
for i in 1 7 13 19 21 23;do ssh 10.0.0.20 mkdir -p /srv/ceph/journals/osd$i;done
for i in 2 8 14;do ssh 10.0.0.6 mkdir -p /srv/ceph/journals/osd$i;done
for i in 3 9 15;do ssh 10.0.0.15 mkdir -p /srv/ceph/journals/osd$i;done
for i in 4 10 16;do ssh 10.0.0.101 mkdir -p /srv/ceph/journals/osd$i;done
for i in 5 11 17;do ssh 10.0.0.102 mkdir -p /srv/ceph/journals/osd$i;done

ssh 10.0.0.19 mount /dev/sde1 /srv/ceph/journals/osd0
ssh 10.0.0.19 mount /dev/sde2 /srv/ceph/journals/osd6
ssh 10.0.0.19 mount /dev/sde3 /srv/ceph/journals/osd12
ssh 10.0.0.19 mount /dev/sda2 /srv/ceph/journals/osd18
ssh 10.0.0.19 mount /dev/sdb2 /srv/ceph/journals/osd20
ssh 10.0.0.19 mount /dev/sdc2 /srv/ceph/journals/osd22
ssh 10.0.0.20 mount /dev/sde1 /srv/ceph/journals/osd1
ssh 10.0.0.20 mount /dev/sde2 /srv/ceph/journals/osd7
ssh 10.0.0.20 mount /dev/sde3 /srv/ceph/journals/osd13
ssh 10.0.0.20 mount /dev/sda2 /srv/ceph/journals/osd19
ssh 10.0.0.20 mount /dev/sdb2 /srv/ceph/journals/osd21
ssh 10.0.0.20 mount /dev/sdg2 /srv/ceph/journals/osd23
ssh 10.0.0.6  mount /dev/sde1 /srv/ceph/journals/osd2
ssh 10.0.0.6  mount /dev/sde2 /srv/ceph/journals/osd8
ssh 10.0.0.6  mount /dev/sde3 /srv/ceph/journals/osd14
ssh 10.0.0.15 mount /dev/sde1 /srv/ceph/journals/osd3
ssh 10.0.0.15 mount /dev/sde2 /srv/ceph/journals/osd9
ssh 10.0.0.15 mount /dev/sde3 /srv/ceph/journals/osd15
ssh 10.0.0.101 mount /dev/sde1 /srv/ceph/journals/osd4
ssh 10.0.0.101 mount /dev/sde2 /srv/ceph/journals/osd10
ssh 10.0.0.101 mount /dev/sde3 /srv/ceph/journals/osd16
ssh 10.0.0.102 mount /dev/sde1 /srv/ceph/journals/osd5
ssh 10.0.0.102 mount /dev/sde2 /srv/ceph/journals/osd11
ssh 10.0.0.102 mount /dev/sde3 /srv/ceph/journals/osd17

mkcephfs -a -c /usr/local/etc/ceph/ceph.conf 

