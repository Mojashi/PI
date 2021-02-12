#!/bin/sh
sudo yum -y update
sudo yum -y install htop tmux gcc-c++ gmp gmp-devel boost boost-devel
make