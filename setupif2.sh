#!/bin/bash

sudo ip tuntap add dev tap0 mode tap
sudo ip tuntap add dev tap1 mode tap
sudo ip link add br0 type bridge
sudo ip link set tap0 master br0
sudo ip link set tap1 master br0
sudo ip addr flush dev tap0
sudo ip addr flush dev tap1
sudo ip link set tap0 up
sudo ip link set tap1 up
sudo ip addr add 192.168.1.1/24 dev br0
sudo link set br0 up

#setup in ~/.bashrc
#export PRECONFIGURED_TAPIF=tap0
#export PRECONFIGURED_TAPIF_R=tap1
