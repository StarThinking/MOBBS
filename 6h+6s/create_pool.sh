#! /bin/bash

# delete pools
ceph osd pool delete hdd-pool hdd-pool --yes-i-really-really-mean-it
ceph osd pool delete ssd-pool ssd-pool --yes-i-really-really-mean-it

# create pools
ceph osd pool create hdd-pool 256 256
ceph osd pool create ssd-pool 256 256

# set rule of pools
ceph osd pool set hdd-pool crush_ruleset 3
ceph osd pool set ssd-pool crush_ruleset 4


