#!/bin/bash

export CROSS_COMPILE=arm-linux-gnueabi-
export ARCH=arm
make mrproper 
make at91sam9g45oem_nf_android_defconfig



