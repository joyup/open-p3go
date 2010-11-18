#include <string.h>
#include <stdio.h>
#include "jz4755.h"
#include "usb.h"
#include "udc.h"
#include "usb_boot.h"
#include "descriptor_pl3.h"

extern enum { 
	init,
	wait_hub_ready,
	hub_ready,
	p1_wait_reset,
	p1_wait_enumerate,
	p1_ready,
	p2_wait_reset,
	p2_wait_enumerate,
	p2_ready,
	p3_wait_reset,
	p3_wait_enumerate,
	p3_ready,
	p2_wait_disconnect,
	p4_wait_connect,
	p4_wait_reset,
	p4_wait_enumerate,
	p4_ready,
	p5_wait_reset,
	p5_wait_enumerate,
	p5_challenged,
	p5_responded,
	p3_wait_disconnect,
	p3_disconnected,
	p5_wait_disconnect,
	p5_disconnected,
	p4_wait_disconnect,
	p4_disconnected,
	p1_wait_disconnect,
	p1_disconnected,
	p6_wait_reset,
	p6_wait_enumerate,
	done,
} state;

extern u8 expire;
extern s8 port_cur;

u16 PL3_USB_GetDescriptor(u8 *buf, const void** const DescriptorAddress)
{
	USB_DeviceRequest *dreq = (USB_DeviceRequest *)buf;
	const u8  DescriptorType   = (dreq->wValue >> 8);
	const u8  DescriptorNumber = (dreq->wValue & 0xFF);
	const u16  wLength = dreq->wLength;

	void* Address = NULL;
	u16 Size = 0;

	switch (DescriptorType)
	{
	case DEVICE_DESCRIPTOR:
		switch (port_cur) {
		case 0:
			Address = (void *) HUB_Device_Descriptor;
			Size    = sizeof(HUB_Device_Descriptor);
			break;
		case 1:
			Address = (void *) port1_device_descriptor;
			Size    = sizeof(port1_device_descriptor);
			break;
		case 2:
			Address = (void *) port2_device_descriptor;
			Size    = sizeof(port2_device_descriptor);
			break;
		case 3:
			Address = (void *) port3_device_descriptor;
			Size    = sizeof(port3_device_descriptor);
			break;
		case 4:
			Address = (void *) port4_device_descriptor;
			Size    = sizeof(port4_device_descriptor);
			break;
		case 5:
			Address = (void *) port5_device_descriptor;
			Size    = sizeof(port5_device_descriptor);
			break;
		case 6:
			Address = (void *) port6_device_descriptor;
			Size    = sizeof(port6_device_descriptor);
			break;
		}
		break;
	case CONFIGURATION_DESCRIPTOR: 
		switch (port_cur) {
		case 0:
			Address = (void *) HUB_Config_Descriptor;
			Size    = sizeof(HUB_Config_Descriptor);
			break;
		case 1:
			// 4 configurations are the same.
			// For the initial 8-byte request, we give a different
			// length response than in the full request.
			if (DescriptorNumber < 4) {
				if (wLength == 8) {
					Address = (void *) port1_short_config_descriptor;
					Size    = sizeof(port1_short_config_descriptor);
				} else {
					Address = (void *) port1_config_descriptor;
					//Size    = sizeof(port1_config_descriptor);
					Size    = PORT1_DESC_LEN;
				}
				if (DescriptorNumber == 3 && wLength > 8) {
					state = p1_ready;
					expire = 10;
				}
			}
			break;
		case 2:
			// only 1 config
			Address = (void *) port2_config_descriptor;
			Size    = sizeof(port2_config_descriptor);
			state = p2_ready;
			expire = 15;
			break;
		case 3:
			// 2 configurations are the same
			Address = (void *) port3_config_descriptor;
			Size    = sizeof(port3_config_descriptor);
			if (DescriptorNumber == 1 && wLength > 8) {
				state = p3_ready;
				expire = 10;
			}
			break;
		case 4:
			// 3 configurations
			if (DescriptorNumber == 0) {
				Address = (void *) port4_config_descriptor_1;
				Size    = sizeof(port4_config_descriptor_1);
			} else if (DescriptorNumber == 1) {
				if (wLength == 8) {
					Address = (void *) port4_short_config_descriptor_2;
					Size    = sizeof(port4_short_config_descriptor_2);
				} else {
					Address = (void *) port4_config_descriptor_2;
					Size    = sizeof(port4_config_descriptor_2);
				}
			} else if (DescriptorNumber == 2) {
				Address = (void *) port4_config_descriptor_3;
				Size    = sizeof(port4_config_descriptor_3);
				if (wLength > 8) {
					state = p4_ready;
					expire = 20;  // longer seems to help this one?
				}
			}
			break;
		case 5:
			// 1 config
			Address = (void *) port5_config_descriptor_pl3;
			Size    = sizeof(port5_config_descriptor_pl3);
			break;
		case 6:
			// 1 config
			Address = (void *) port6_config_descriptor;
			Size    = sizeof(port6_config_descriptor);
			break;
		}
		break;
	case 0x29: // HUB descriptor (always to port 0 we'll assume)
		switch (port_cur) {
		case 0:
			Address = (void *) HUB_Hub_Descriptor;
			Size    = sizeof(HUB_Hub_Descriptor);
			break;
		}
		break;
	}
	
	*DescriptorAddress = Address;
	return Size;
}

