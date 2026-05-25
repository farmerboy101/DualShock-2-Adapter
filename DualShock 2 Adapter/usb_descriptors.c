/************************************************************************
	usb_descriptors.c

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

    This projects framework was taken from https://www.waitingforfriday.com/?p=451 and modified by https://modman.studio to build a Sony DualShock 2 controller USB adapter.
    Thank you to Simon Inns at https://www.waitingforfriday.com as well as a bunch of other people whose projects were used as references to create this.

************************************************************************/

#ifndef USB_DESCRIPTORS_C
#define USB_DESCRIPTORS_C

// Local includes
#include "HardwareProfile.h"

// Microchip Application Library includes
#include "./USB/usb.h"
#include "./USB/usb_function_hid.h"

// Device Descriptor
ROM USB_DEVICE_DESCRIPTOR device_dsc=
{
    0x12,    				// Size of this descriptor in bytes
    USB_DESCRIPTOR_DEVICE,  // DEVICE descriptor type
    0x0200,                 // USB Spec Release Number in BCD format
    0x00,                   // Class Code
    0x00,                   // Subclass code
    0x00,                   // Protocol code
    USB_EP0_BUFF_SIZE,      // Max packet size for EP0, see usb_config.h
    USB_VID,				// Vendor ID
    USB_PID,				// Product ID
    0x0100,                 // Device release number in BCD format
    0x01,                   // Manufacturer string index
    0x02,                   // Product string index
    0x00,                   // Device serial number string index
    0x01                    // Number of possible configurations
};

// Configuration 1 Descriptor
ROM BYTE configDescriptor1[]=
{
    // Configuration Descriptor
    0x09,							// Size of this descriptor in bytes (sizeof(USB_CFG_DSC))
    USB_DESCRIPTOR_CONFIGURATION,	// CONFIGURATION descriptor type
    0x29,0x00,						// Total length of data for this cfg
    0x01,								// Number of interfaces in this cfg
    0x01,								// Index value of this configuration
    0x00,								// Configuration string index
    _DEFAULT,                       // Attributes, see usb_device.h
    100,								// Max power consumption (2X mA)
							
    // Interface Descriptor
    0x09,							// Size of this descriptor in bytes (sizeof(USB_INTF_DSC))
    USB_DESCRIPTOR_INTERFACE,		// INTERFACE descriptor type
    0x00,								// Interface Number
    0x00,								// Alternate Setting Number
    0x02,								// Number of endpoints in this intf
    HID_INTF,						// Class code
    0x00,								// Subclass code
    0x00,								// Protocol code
    0x00,								// Interface string index

    // HID Class-Specific Descriptor
    0x09,							// Size of this descriptor in bytes (sizeof(USB_HID_DSC)+3)
    DSC_HID,						// HID descriptor type
    0x11, 0x01,						// HID Spec Release Number in BCD format (1.11)
    0x00,							// Country Code (0x00 for Not supported)
    HID_NUM_OF_DSC,					// Number of class descriptors, see usbcfg.h
    DSC_RPT,						// Report descriptor type
    HID_RPT01_SIZE,0x00,			// Size of the report descriptor (sizeof(hid_rpt01))
    
    // Endpoint Descriptor
    0x07,							// sizeof(USB_EP_DSC)
    USB_DESCRIPTOR_ENDPOINT,		// Endpoint Descriptor
    HID_EP | _EP_IN,				// Endpoint Address
    _INTERRUPT,						// Attributes
    0x40,0x00,						// size
    0x01,							// Interval

    // Endpoint Descriptor
    0x07,							// sizeof(USB_EP_DSC)
    USB_DESCRIPTOR_ENDPOINT,		// Endpoint Descriptor
    HID_EP | _EP_OUT,				// EndpointAddress
    _INTERRUPT,						// Attributes
    0x40,0x00,						// size
    0x01							// Interval
};

// Language code string descriptor
ROM struct{BYTE bLength;BYTE bDscType;WORD string[1];}sd000={
sizeof(sd000),USB_DESCRIPTOR_STRING,{0x0409
}};

// Manufacturer string descriptor (set in HardwareProfile.h)
ROM struct{BYTE bLength;BYTE bDscType;WORD string[MSDLENGTH];}sd001={
sizeof(sd001),USB_DESCRIPTOR_STRING,
{MSD}};

// Product string descriptor (set in HardwareProfile.h)
ROM struct{BYTE bLength;BYTE bDscType;WORD string[PSDLENGTH];}sd002={
sizeof(sd002),USB_DESCRIPTOR_STRING,
{PSD}};

// Device serial number string descriptor
ROM struct{BYTE bLength;BYTE bDscType;WORD string[DSNLENGTH];}sd003={
sizeof(sd003),USB_DESCRIPTOR_STRING,
{DSN}};

// Class specific descriptor - HID 
ROM struct{BYTE report[HID_RPT01_SIZE];}hid_rpt01={
{
    0x05, 0x01,        // Usage Page (Generic Desktop Ctrls)
    0x09, 0x04,        // Usage (Joystick)
    0xA1, 0x01,        // Collection (Physical)
    0xA1, 0x02,        //   Collection (Application)
    0x85, 0x01,        //     Report ID (1)
    0x75, 0x08,        //     Report Size (8)
    0x95, 0x01,        //     Report Count (1)
    0x15, 0x00,        //     Logical Minimum (0)
    0x26, 0xFF, 0x00,  //     Logical Maximum (255)
    0x81, 0x03,        //     Input (Const,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
                       //     NOTE: reserved byte
    0x75, 0x01,        //     Report Size (1)
    0x95, 0x13,        //     Report Count (19)
    0x15, 0x00,        //     Logical Minimum (0)
    0x25, 0x01,        //     Logical Maximum (1)
    0x35, 0x00,        //     Physical Minimum (0)
    0x45, 0x01,        //     Physical Maximum (1)
    0x05, 0x09,        //     Usage Page (Button)
    0x19, 0x01,        //     Usage Minimum (0x01)
    0x29, 0x13,        //     Usage Maximum (0x13)
    0x81, 0x02,        //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x75, 0x01,        //     Report Size (1)
    0x95, 0x0D,        //     Report Count (13)
    0x06, 0x00, 0xFF,  //     Usage Page (Vendor Defined 0xFF00)
    0x81, 0x03,        //     Input (Const,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
                       //     NOTE: 32 bit integer, where 0:18 are buttons and 19:31 are reserved
    0x15, 0x00,        //     Logical Minimum (0)
    0x26, 0xFF, 0x00,  //     Logical Maximum (255)
    0x05, 0x01,        //     Usage Page (Generic Desktop Ctrls)
    0x09, 0x01,        //     Usage (Pointer)
    0xA1, 0x00,        //     Collection (Undefined)
    0x75, 0x08,        //       Report Size (8)
    0x95, 0x04,        //       Report Count (4)
    0x35, 0x00,        //       Physical Minimum (0)
    0x46, 0xFF, 0x00,  //       Physical Maximum (255)
    0x09, 0x30,        //       Usage (X)
    0x09, 0x31,        //       Usage (Y)
    0x09, 0x32,        //       Usage (Z)
    0x09, 0x35,        //       Usage (Rz)
    0x81, 0x02,        //       Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
                       //       NOTE: four joysticks
    0xC0,              //     End Collection
    0x05, 0x01,        //     Usage Page (Generic Desktop Ctrls)
    0x75, 0x08,        //     Report Size (8)
    0x95, 0x27,        //     Report Count (39)
    0x09, 0x01,        //     Usage (Pointer)
    0x81, 0x02,        //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x75, 0x08,        //     Report Size (8)
    0x95, 0x30,        //     Report Count (48)
    0x09, 0x01,        //     Usage (Pointer)
    0x91, 0x02,        //     Output (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
    0x75, 0x08,        //     Report Size (8)
    0x95, 0x30,        //     Report Count (48)
    0x09, 0x01,        //     Usage (Pointer)
    0xB1, 0x02,        //     Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
    0xC0,              //   End Collection
    0xA1, 0x02,        //   Collection (Application)
    0x85, 0x02,        //     Report ID (2)
    0x75, 0x08,        //     Report Size (8)
    0x95, 0x30,        //     Report Count (48)
    0x09, 0x01,        //     Usage (Pointer)
    0xB1, 0x02,        //     Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
    0xC0,              //   End Collection
    0xA1, 0x02,        //   Collection (Application)
    0x85, 0xEE,        //     Report ID (238)
    0x75, 0x08,        //     Report Size (8)
    0x95, 0x30,        //     Report Count (48)
    0x09, 0x01,        //     Usage (Pointer)
    0xB1, 0x02,        //     Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
    0xC0,              //   End Collection
    0xA1, 0x02,        //   Collection (Application)
    0x85, 0xEF,        //     Report ID (239)
    0x75, 0x08,        //     Report Size (8)
    0x95, 0x30,        //     Report Count (48)
    0x09, 0x01,        //     Usage (Pointer)
    0xB1, 0x02,        //     Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
    0xC0,              //   End Collection
    0xC0}              // End Collection
};                  


// Array of configuration descriptors
ROM BYTE *ROM USB_CD_Ptr[]=
{
    (ROM BYTE *ROM)&configDescriptor1
};

// Array of string descriptors
ROM BYTE *ROM USB_SD_Ptr[]=
{
    (ROM BYTE *ROM)&sd000,
    (ROM BYTE *ROM)&sd001,
    (ROM BYTE *ROM)&sd002,
    (ROM BYTE *ROM)&sd003
};

#endif
