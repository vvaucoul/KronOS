#!/bin/bash

make fclean
rm -rf libkfs kernel isodir kfs.iso
cp -r ~/Delivery/PostCommonCore/KFS/kfs-1/* .
make re && make fclean && sh ./push.sh
