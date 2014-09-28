#!/bin/bash

# Build Script for Y301-A2

# set toolchain, clean, then make.
export ARCH=arm
export SUBARCH=arm
export CCOMPILER=$HOME/android/prebuilt/linux-x86/toolchain/arm-cortex_a15-linux-gnueabihf-linaro_4.8.3-2014.04/bin/arm-gnueabi-

rm -r out
make clean
make mrproper

echo    "##########################"
echo -e "# Pick Kernel Type       #"
echo -e "#  1. Stock Kernel       #"
echo -e "#  2. Custom Kernel      #"
echo    "##########################"

read kernel

if [ "$kernel" != "2" ]; then
	make hw_msm8930_defconfig
else
	make y301a2_defconfig
fi

make ARCH=arm CROSS_COMPILE=$CCOMPILER -j`grep 'processor' /proc/cpuinfo | wc -l`

# copy kernel and modules to out if not failed
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
	
	if [ "$kernel" != "2" ]; then
		cp -r ../ramdisk/stock/boot.img-ramdisk .
	else
		cp -r ../ramdisk/custom/boot.img-ramdisk .
	fi
	
	cd boot.img-ramdisk
	cd wifi
	mv dhd_4330 dhd_4330.ko
	cd ..
	find . | cpio -o -H newc | gzip > ../ramdisk.gz
	cd ..
	./mkbootimg --kernel zImage --ramdisk ramdisk.gz --base 0x80200000 --ramdisk_offset 0x02000000 --cmdline "androidboot.hardware=qcom user_debug=31 msm_rtb.filter=0x3F ehci-hcd.park=3 maxcpus=2" --pagesize 2048 -o boot.img	
	
	zip -r kernel.zip META-INF boot.img
	cd ..
	
	echo "#########################################################"
	echo "#                      COMPLETED                        #"
	echo "#########################################################"
else
	echo "#########################################################"
	echo "#                       FAILED :(                       #"
	echo "#########################################################"
fi


