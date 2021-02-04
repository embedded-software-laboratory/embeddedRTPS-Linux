#!/bin/bash

if [ -z $1 ]
then
    echo "Use name of networkinterface as first argument! (e.g. eth0)"
    exit 1;
fi

sudo ip tuntap add dev tap0 mode tap
sudo ip link set tap0 up
sudo ip link add br0 type bridge
sudo ip link set tap0 master br0
sudo ip link set $1 down
sudo ip addr flush dev $1
sudo ip link set dev $1 up
sudo ip link set $1 master br0
sudo ip link set dev br0 up
sudo dhclient br0