#!/bin/bash

kernel_file=$1
boot_sect=$2
output=$3
size=$(stat --printf='%s' ${kernel_file})
sectors=$((size/512))
echo "Updating ${boot_sect} to read $sectors"

sed "s/<KERNEL_IMAGE_SECTORS>/${sectors}/g" ${boot_sect} > ${output}
