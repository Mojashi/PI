#!/bin/sh
yum -y update
yum -y install htop tmux gcc-c++ gmp gmp-devel boost boost-devel
make