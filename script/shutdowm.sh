#!/bin/bash

ssh 10.0.0.20 "killall -e Storage"
ssh 10.0.0.20 "killall -e Monitor"
ssh 10.0.0.21 "killall -e Storage"
ssh 10.0.0.21 "killall -e Monitor"

killall -e Monitor
