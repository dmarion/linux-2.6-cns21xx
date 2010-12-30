#
# Makefile for the linux kernel.
#

# Object file lists.

#demo_pcm_legerity_2phone-objs := demo_pcm_legerity_config_2phone.o
#demo_i2s-objs := demo_i2s_config.o
#demo_i2s_wm8772-objs := demo_i2s_wm8772_config.o

obj-y			:= timer.o irq.o counter.o pm.o mm.o
obj-m			:=
obj-n			:=
obj-			:=

obj-$(CONFIG_PCI) 				+= pci.o
obj-$(CONFIG_MACH_NSK330)			+= nsk330-setup.o 
