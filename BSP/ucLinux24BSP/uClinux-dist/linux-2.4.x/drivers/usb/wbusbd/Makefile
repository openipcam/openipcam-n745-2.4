#
 # Makefile for USB peripheral controller and gadget drivers
 # for kbuild 2.4
 #
 
O_TARGET	:= w90n745_usbd.o

obj-$(CONFIG_WINBOND_USBD_MASS) += w90n745_mass.o
obj-$(CONFIG_WINBOND_USBD_VCOM) += w90n745_vcom.o

#export-objs	:= w90n745_usb.o

include $(TOPDIR)/Rules.make
