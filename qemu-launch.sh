#!/bin/bash

killall qemu-system-i386 || true
set -e
make -C cmake-build-debug

if [ "$#" == "0" ]; then
  nohup qemu-system-i386 -fda ./cmake-build-debug/bin/kernel-image.bin &
elif [ "$1" == "monitor" ]; then
  nohup qemu-system-i386 -fda ./cmake-build-debug/bin/kernel-image.bin -monitor telnet:127.0.0.1:1235,server,nowait &
elif [ "$1" == "gdb" ]; then
  nohup qemu-system-i386 -fda ./cmake-build-debug/bin/kernel-image.bin -monitor telnet:127.0.0.1:1235,server &
  sleep 1
  echo "stop" | telnet 127.0.0.1 1235 || true
  echo "gdbserver" | telnet 127.0.0.1 1235 || true
else
  nohup qemu-system-i386 -fda ./cmake-build-debug/bin/kernel-image.bin $@ &
fi