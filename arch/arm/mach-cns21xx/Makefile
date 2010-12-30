#
# Makefile for the linux kernel.
#

# Object file lists.

obj-y		:= core.o devices.o gpio.o irq.o mm.o time.o

# devices
obj-$(CONFIG_CNS21XX_DEV_GEC)		+= dev-gec.o
obj-$(CONFIG_CNS21XX_DEV_USB)		+= dev-usb.o
obj-$(CONFIG_CNS21XX_DEV_SPI_MASTER)	+= dev-spi-master.o

# machine specific files
obj-$(CONFIG_MACH_NS_K330)		+= mach-ns-k330.o
obj-$(CONFIG_MACH_NSB3AST)		+= mach-nsb3ast.o
