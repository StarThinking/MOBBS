#! /bin/bash

ceph osd pool create hdd-pool 256 256
ceph osd pool create ssd-pool 256 256

ceph osd pool set hdd-pool crush_ruleset 3
ceph osd pool set ssd-pool crush_ruleset 4

