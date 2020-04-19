#!/bin/bash

kernel_file=$1
raw_kernel_file=$2
boot_sect=$3
output=$4
size=$(stat --printf='%s' ${raw_kernel_file})
sectors=$((size/512+1)) # Size of the file
bss_size=$(/usr/local/i386elfgcc/bin/i386-elf-size --format=SysV ${kernel_file} | grep .bss | awk -F '  +' '{print $2}')
bss_start=$(/usr/local/i386elfgcc/bin/i386-elf-size --format=SysV ${kernel_file} | grep .bss | awk -F '  +' '{print $3}')
kernel_end=$((bss_start+bss_size))

echo "Updating ${boot_sect} to read $sectors and start usable RAM at ${kernel_end}"

sed "s/<KERNEL_IMAGE_SECTORS>/${sectors}/g" ${boot_sect} > ${output}
sed -i "s/<KERNEL_END_ADDRESS>/${kernel_end}/g" ${output}
