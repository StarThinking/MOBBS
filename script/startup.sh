#!/bin/bash

ssh 10.0.0.20 "killall -e Storage"
ssh 10.0.0.20 "killall -e Monitor"
ssh 10.0.0.21 "killall -e Storage"
ssh 10.0.0.21 "killall -e Monitor"

ssh 10.0.0.20 "rm monitor.log"
ssh 10.0.0.20 "rm storage.log"
ssh 10.0.0.21 "rm monitor.log"
ssh 10.0.0.21 "rm storage.log"

ssh 10.0.0.20 "nohup /root/git/MOBBS/monitor/Monitor > monitor.log &"
ssh 10.0.0.21 "nohup /root/git/MOBBS/monitor/Monitor > monitor.log &"
ssh 10.0.0.20 "nohup /root/git/MOBBS/storage/Storage > storage.log &"
ssh 10.0.0.21 "nohup /root/git/MOBBS/storage/Storage > storage.log &"

killall -e Monitor
nohup ../monitor/Monitor > /root/monitor.log &
