#!/bin/bash

sudo ip tuntap add dev tap0 mode tap
sudo ip link set tap0 up
sudo ip addr add 192.168.1.1/24 dev tap0
