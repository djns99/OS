#!/bin/bash

sudo apt install build-essential bison flex libgmp3-dev libmpc-dev libmpfr-dev texinfo

export CC=/usr/bin/gcc
export LD=/usr/bin/gcc
export PREFIX="/usr/local/i386elfgcc"
export TARGET=i386-elf
export PATH="$PREFIX/bin:$PATH"

mkdir /tmp/src
cd /tmp/src
curl -O http://ftp.gnu.org/gnu/binutils/binutils-2.34.tar.gz
tar xf binutils-2.34.tar.gz
mkdir binutils-build
cd binutils-build
../binutils-2.34/configure --target=$TARGET --enable-interwork --enable-multilib --disable-nls --disable-werror --prefix=$PREFIX
sudo make all install

cd /tmp/src
curl -O ftp://ftp.gnu.org/gnu/gcc/gcc-9.3.0/gcc-9.3.0.tar.gz
tar xf gcc-9.3.0.tar.gz
mkdir gcc-build
cd gcc-build
../gcc-9.3.0/configure --target=$TARGET --prefix="$PREFIX" --disable-nls --disable-libssp --enable-languages=c --without-headers
sudo make all-gcc 
sudo make all-target-libgcc 
sudo make install-gcc 
sudo make install-target-libgcc 