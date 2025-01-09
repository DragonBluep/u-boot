#!/bin/bash

[[ $(grep -c 'CONFIG_ARCH_MEDIATEK=y' '.config') -ne 0 ]] && arch=aarch64
[[ $(grep -c 'CONFIG_ARCH_MTMIPS=y' '.config') -ne 0 ]] && arch=mipsel

if [ -z "$arch" ] ; then
	echo "Unsupported ARCH!"
	exit
fi

# toolchain path
if [ -d ../openwrt*/staging_dir/toolchain-${arch}*/bin ]; then
	Toolchain=$(cd ../openwrt*/staging_dir/toolchain-${arch}*/bin; pwd)/${arch}-openwrt-linux-
elif [ -d ../openwrt*/toolchain-${arch}*/bin ]; then
	Toolchain=$(cd ../openwrt*/toolchain-${arch}*/bin; pwd)/${arch}-openwrt-linux-
else
	echo "can not find the toolchain"
	exit
fi
Staging=${Toolchain%/toolchain-*}

echo "CROSS_COMPILE=${Toolchain}"
echo "STAGING_DIR=${Toolchain%/toolchain-*}"

thread=$(cat /proc/cpuinfo| grep "processor"| wc -l)

make -j ${thread} CROSS_COMPILE=${Toolchain} STAGING_DIR=${Staging}
# add board name here
# Boards=( \
# 	)

# if [ ! -d "./bin" ]; then
# 	mkdir ./bin
# fi

# for Board in ${Boards[@]}
# do
# 	echo "Build ${Board}"
# 	make clean
# 	rm .config*
# 	make ${Board}_defconfig
# 	make CROSS_COMPILE=${Toolchain} STAGING_DIR=${Staging}
# 	if [ ! -d "./bin/$Board" ]; then
# 		mkdir ./bin/$Board
# 	else
# 		rm ./bin/$Board/*
# 	fi
# 	mv ./u-boot.img ./bin/$Board/u-boot.img
# 	mv ./u-boot-mt7621.bin ./bin/$Board/u-boot-mt7621.bin
# done

# tar -acvf u-boot-mt7621.tar.xz ./bin/*
