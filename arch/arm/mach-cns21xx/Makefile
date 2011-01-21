# Common
obj-y						:= core.o time.o irq.o mm.o gpio.o uart.o 

# Features
obj-$(CONFIG_ARCH_CNS21XX_SPI)			+= spi.o
obj-$(CONFIG_ARCH_CNS21XX_USB)			+= usb.o
obj-$(CONFIG_ARCH_CNS21XX_NIC)			+= nic.o

# Devices
obj-$(CONFIG_MACH_NSK330)			+= device-nsk330.o
