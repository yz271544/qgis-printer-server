#!/bin/bash

# add `display-setup-script=/opt/anbao/running/xhost.sh` to /etc/lightdm/lightdm.conf.d/logout.conf
# the logout.conf content is:
# [SeatDefaults]
# session-cleanup-script=/usr/bin/python3 /usr/hedron/hook.py
# display-setup-script=/opt/anbao/running/xhost.sh
xhost +local:*
xhost +SI:localuser:root
xhost +SI:localuser:jingwei