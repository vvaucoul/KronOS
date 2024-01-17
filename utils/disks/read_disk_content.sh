#!/bin/bash

# Colors
RESET="\033[0m"
BOLD="\033[1m"
DIM="\033[2m"
RED="\033[1;31m"
GREEN="\033[1;32m"
YELLOW="\033[1;33m"
WHITE="\033[1;37m"

if command -v qemu-img >/dev/null 2>&1; then
    echo -n ""
else
    printf "$WHITE    - qemu-img not found, please install it.$RESET\n"
    exit 1
fi

hdd_path="../../isodir/boot"
hdd_filename=$(grep -oP 'disk_filename="\K[^"]+' ../../mk-files/.config)

printf "$WHITE    - Reading disk content...\n"
printf "$WHITE    \t- Disk path: $GREEN%s\n" $hdd_path
printf "$WHITE    \t- Disk filename: $GREEN%s\n$RESET" $hdd_filename

qemu-img info $hdd_path/$hdd_filename >.hdd_output.log
dd if=$hdd_path/$hdd_filename of=.sector.bin bs=512 count=1 

cat .sector.bin >>.hdd_output.log
cat .sector.bin | hexdump -C >.sector.hex

printf "$WHITE    - Disk content read. $RESET$DIM(log: .hdd_output.log)\n$RESET"
