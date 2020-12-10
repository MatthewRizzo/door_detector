#!/usr/bin/bash
# File: Helps install the needed linux packages
echo "------------Running Script for Linux Package Management------------"

# need to have sudo permissions
if [ "$EUID" -ne 0 ]; then
    echo "Please run as 'sudo'"
    exit
fi

apt update -y

apt install -y \
    cmake \
    gcc-7 g++-7 \

apt upgrade -y

echo "------------Completed Script for Linux Package Management------------"