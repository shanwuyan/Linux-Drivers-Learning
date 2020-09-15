#!/bin/bash

obj-m += shanwuyan.o

KDIR := /home/topeet/Android/itop4412_kernel_4_14_2_bsp/linux-4.14.2_iTop-4412_scp #这里要对应你的Linux内核目录

PWD ?= $(shell pwd)

all:
	make -C $(KDIR) M=$(PWD) modules

clean:
	make -C $(KDIR) M=$(PWD) clean