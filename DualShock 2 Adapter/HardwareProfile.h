/************************************************************************
	HardwareProfile.h

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

#ifndef HARDWAREPROFILE_H
#define HARDWAREPROFILE_H

// USB stack hardware selection options ----------------------------------------------------------------

// (This section is the set of definitions required by the MCHPFSUSB framework.)

// Uncomment the following define if you wish to use the self-power sense feature 
// and define the port, pin and tris for the power sense pin below:
// #define USE_SELF_POWER_SENSE_IO
#define tris_self_power     TRISAbits.TRISA2
#if defined(USE_SELF_POWER_SENSE_IO)
	#define self_power          PORTAbits.RA2
#else
	#define self_power          1
#endif

// Uncomment the following define if you wish to use the bus-power sense feature 
// and define the port, pin and tris for the power sense pin below:
//#define USE_USB_BUS_SENSE_IO
#define tris_usb_bus_sense  TRISAbits.TRISA1
#if defined(USE_USB_BUS_SENSE_IO)
	#define USB_BUS_SENSE       PORTAbits.RA1
#else
	#define USB_BUS_SENSE       1
#endif

// Uncomment the following line to make the output HEX of this project work with the MCHPUSB Bootloader    
//#define PROGRAMMABLE_WITH_USB_MCHPUSB_BOOTLOADER

// Uncomment the following line to make the output HEX of this project work with the HID Bootloader
#define PROGRAMMABLE_WITH_USB_HID_BOOTLOADER		

// Application specific hardware definitions ------------------------------------------------------------

// Oscillator frequency (48Mhz with a 20Mhz external oscillator)
#define CLOCK_FREQ 48000000

// Device Vendor Indentifier (VID) (0x054C is Sony's VID)
#define USB_VID	0x054C

// Device Product Indentifier (PID) (0x0268 is the PID for a PlayStation 3 Controller)
#define USB_PID	0x0268

// Manufacturer string descriptor
#define MSDLENGTH	13
#define MSD		'M','O','D','M','A','N','.','S','T','U','D','I','O'

// Product String descriptor
#define PSDLENGTH	26
#define PSD		'P','L','A','Y','S','T','A','T','I','O','N','(','R',')','3',' ','C','o','n','t','r','o','l','l','e','r'

// Device serial number string descriptor
#define DSNLENGTH	5
#define DSN		'V','_','1','.','0'

// Common useful definitions
#define INPUT_PIN 1
#define OUTPUT_PIN 0
#define FLAG_FALSE 0
#define FLAG_TRUE 1

// Comment out the following line if you do not want the debug
// feature of the firmware (saves code and RAM space when off)
//
// Note: if you use this feature you must compile with the large
// memory model on (for 24-bit pointers) so that the sprintf()
// function will work correctly.  If you do not require debug it's
// recommended that you compile with the small memory model and 
// remove any references to <strings.h> and sprintf().
#define DEBUGON

// PIC to hardware pin mapping and control macros

// Led control macros
#define mStatusLED0				LATDbits.LATD7
#define mStatusLED0_on()		mStatusLED0 = 1;
#define mStatusLED0_off()		mStatusLED0 = 0;
#define mStatusLED0_Toggle()	mStatusLED0 = !mStatusLED0;
#define mStatusLED0_Get()       mStatusLED0

#define PS2_ATT         LATDbits.LATD5  // PS2 controller port
#define MOTORBOOST         LATDbits.LATD1  // Set to HIGH to disable shutdown pin on MAX1771 boost converter for rumble motors

// PS2 controller subroutine functions
void SPI_Init_Master();
unsigned char SPI_Write(unsigned char x);
void __delay_pc(unsigned char delaytime);
void __delay_ms(unsigned char delaytime);
void __delay_poll();
void ReadWrite(void);
void ShortPoll(void);
void EnterConfig(void);
void TurnOnAnalogMode(void);
void TurnOnPressureButtons(void);
void CheckPressureButtons(void);
void SetupVibrationMotors(void);
void Exitconfig(void);
void Poll(void);
void PS2_init(void);

#endif
