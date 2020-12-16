#!/bin/bash

sudo ip tuntap add dev tap0 mode tap
sudo ifconfig tap0 up
sudo ifconfig tap0 192.168.1.1/24
