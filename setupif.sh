#!/bin/bash

sudo ip tuntap add dev tap0 mode tap
sudo ip tuntap add dev tap1 mode tap
sudo brctl addbr br0
sudo brctl addif br0 tap0 tap1
sudo ifconfig tap0 up
sudo ifconfig tap1 up
sudo ifconfig br0 up

#setup in ~/.bashrc
#export PRECONFIGURED_TAPIF=tap0
#export PRECONFIGURED_TAPIF_R=tap1