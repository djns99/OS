#!/bin/bash

kernel_file=$1
raw_kernel_file=$2
boot_sect=$3
output=$4
size=$(stat --printf='%s' ${raw_kernel_file})
sectors=$((size/512+1)) # Size of the file
echo "Updating ${boot_sect} to read $sectors"

sed "s/<KERNEL_IMAGE_SECTORS>/${sectors}/g" ${boot_sect} > ${output}