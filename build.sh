#!/bin/bash

# Build Script for Y301-A2

# set toolchain, clean, then make.
export ARCH=arm
export SUBARCH=arm
export CCOMPILER=$HOME/android/prebuilt/linux-x86/toolchain/arm-cortex_a15-linux-gnueabihf-linaro_4.8.3-2014.04/bin/arm-gnueabi-

# cleanup before building
rm -r out
make clean
make mrproper

# choose defconfig then build by number of cores available
make y301a2_defconfig
make ARCH=arm CROSS_COMPILE=$CCOMPILER -j`grep 'processor' /proc/cpuinfo | wc -l`

# copy kernel and modules to out then make bootable or fail.
if [ -e arch/arm/boot/zImage ]; then
	echo "#########################################################"
	echo "#          Copying zImage and Modules to Out            #"
	echo "#########################################################"
	mkdir -p ./out
	mkdir -p ./out/modules/
	cd out
	cp -f ../arch/arm/boot/zImage .
	cd ..
	cp -r `find -iname '*.ko'` ./out/modules/
	cd out
	cp -r ../extra/mkbootimg .
	cp -r ../extra/META-INF .
	cp -r ../extra/system .
	cp -r ../ramdisk/ramdisk .
	mv ramdisk ramdisk.gz
	./mkbootimg --kernel zImage --ramdisk ramdisk.gz --base 0x80200000 --ramdisk_offset 0x02000000 --cmdline "androidboot.hardware=qcom user_debug=31 msm_rtb.filter=0x3F ehci-hcd.park=3 maxcpus=2" --pagesize 2048 -o boot.img	
	
	zip -r kernel.zip system META-INF boot.img
	cd ..
	
	echo "#########################################################"
	echo "#                      COMPLETED                        #"
	echo "#########################################################"
else
	echo "#########################################################"
	echo "#                       FAILED                          #"
	echo "#########################################################"
fi


