/************************************************************************
	main.c

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

#ifndef MAIN_C
#define MAIN_C

// Global includes
// Note: string.h is required for sprintf commands for debug
#include <string.h>

// Local includes
#include "HardwareProfile.h"
#include "debug.h"
#include "delays.h"

// Microchip Application Library includes
// (expects V2.9a of the USB library from "Microchip Solutions v2011-07-14")
//
// The library location must be set in:
// Project -> Build Options Project -> Directories -> Include search path
// in order for the project to compile.
#include "./usb_device.c"
#include "./HID Device Driver/usb_function_hid.c"
//#include "./usb_device_local.h"
#include "./USB/usb.h"
#include "./USB/usb_function_hid.h"

// Ensure we have the correct target PIC device family
#if !defined(__18F4550) && !defined(__18F2550)
	#error "This firmware only supports either the PIC18F4550 or PIC18F2550 microcontrollers."
#endif

// Define the globals for the USB data in the USB RAM of the PIC18F4550
#pragma udata
#pragma udata USB_VARIABLES=0x500
unsigned char ReceivedDataBuffer[64];
unsigned char ToSendDataBuffer[64];
#pragma udata

// Define initial reports to be sent back to the host in the ROM of the PIC18F4550
const rom unsigned char Report01[64] = {
    0x00, 0x01, 0x04, 0x00, 0x07, 0x0C, 0x01, 0x02, 0x18, 0x18, 0x18, 0x18, 0x09, 0x0A, 0x10, 0x11,
    0x12, 0x13, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x02, 0x02, 0x02, 0x02, 0x00, 0x00, 0x00, 0x04,
    0x04, 0x04, 0x04, 0x00, 0x00, 0x04, 0x00, 0x01, 0x02, 0x07, 0x00, 0x17, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
const rom unsigned char ReportF2[64] = {
    0xF2, 0xFF, 0xFF, 0x00, 0x00, 0x06, 0xF5, 0x48, 0xE2, 0x49, 0x00, 0x03, 0x50, 0x81, 0xD8, 0x01,
    0x8A, 0x13, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x02, 0x02, 0x02, 0x02, 0x00, 0x00, 0x00, 0x04,
    0x04, 0x04, 0x04, 0x00, 0x00, 0x04, 0x00, 0x01, 0x02, 0x07, 0x00, 0x17, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
const rom unsigned char ReportF5[64] = {
    0x01, 0x00, 0x00, 0x23, 0x06, 0x7C, 0xB9, 0x0B, 0xE2, 0x49, 0x00, 0x03, 0x50, 0x81, 0xD8, 0x01,
    0x8A, 0x13, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x02, 0x02, 0x02, 0x02, 0x00, 0x00, 0x00, 0x04,
    0x04, 0x04, 0x04, 0x00, 0x00, 0x04, 0x00, 0x01, 0x02, 0x07, 0x00, 0x17, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
const rom unsigned char ReportEF[64] = {
    0x00, 0xEF, 0x04, 0x00, 0x07, 0x03, 0x01, 0xA0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x02, 0x00, 0x01, 0x8E, 0x02, 0x00, 0x01, 0x8E, 0x01, 0xFF, 0x01, 0x8D, 0x01, 0xF8, 0x00,
    0x76, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
const rom unsigned char ReportF8[64] = {
    0x00, 0x01, 0x00, 0x00, 0x07, 0x03, 0x01, 0xB0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x02, 0x6D, 0x02, 0x6E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
const rom unsigned char ReportF7[64] = {
    0x02, 0x01, 0xD4, 0x02, 0xE6, 0x01, 0xF1, 0xFF, 0x14, 0x33, 0x02, 0x02, 0xD3, 0x01, 0xE5, 0x00,
    0x00, 0x02, 0x02, 0x02, 0x00, 0x03, 0x00, 0x00, 0x02, 0x00, 0x00, 0x02, 0x62, 0x01, 0x02, 0x01,
    0x5E, 0x00, 0x32, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

USB_HANDLE USBOutHandle = 0;
USB_HANDLE USBInHandle = 0;
BOOL blinkStatusValid = FLAG_TRUE;
BOOL transmit = FLAG_FALSE;  // Flag for whether PS2 controller to start sending packets, might not be needed

unsigned char PS2_READ;     // Variable for initializing SPI mode when starting device and for checking that the controller is in pressure sensitive mode
unsigned char SmallMotorSet;     // Variable for verifying the small motor is initialized properly, as occasionally SetupVibrationMotors() will fail
unsigned char LargeMotorSet;     // Variable for verifying the large motor is initialized properly, as occasionally SetupVibrationMotors() will fail

#pragma config PLLDIV   = 5         // 20Mhz external oscillator
#pragma config CPUDIV   = OSC1_PLL2   
#pragma config USBDIV   = 2         // Clock source from 96MHz PLL/2
#pragma config FOSC     = HSPLL_HS
#pragma config FCMEN    = OFF
#pragma config IESO     = OFF
#pragma config PWRT     = OFF
#pragma config BOR      = ON
#pragma config BORV     = 3
#pragma config VREGEN   = ON
#pragma config WDT      = OFF
#pragma config WDTPS    = 32768
#pragma config MCLRE    = ON
#pragma config LPT1OSC  = OFF
#pragma config PBADEN   = OFF
// #pragma config CCP2MX   = ON
#pragma config STVREN   = ON
#pragma config LVP      = OFF
// #pragma config ICPRT    = OFF
#pragma config XINST    = OFF
#pragma config CP0      = OFF
#pragma config CP1      = OFF
// #pragma config CP2      = OFF
// #pragma config CP3      = OFF
#pragma config CPB      = OFF
// #pragma config CPD      = OFF
#pragma config WRT0     = OFF
#pragma config WRT1     = OFF
// #pragma config WRT2     = OFF
// #pragma config WRT3     = OFF
#pragma config WRTB     = OFF
#pragma config WRTC     = OFF
// #pragma config WRTD     = OFF
#pragma config EBTR0    = OFF
#pragma config EBTR1    = OFF
// #pragma config EBTR2    = OFF
// #pragma config EBTR3    = OFF
#pragma config EBTRB    = OFF

// Private function prototypes
static void initialisePic(void);
void processUsbCommands(void);
void applicationInit(void);
void USBCBSendResume(void);
void highPriorityISRCode();
void lowPriorityISRCode();

// Remap vectors for compatibilty with Microchip USB boot loaders
#if defined(PROGRAMMABLE_WITH_USB_HID_BOOTLOADER)
	#define REMAPPED_RESET_VECTOR_ADDRESS			0x1000
	#define REMAPPED_HIGH_INTERRUPT_VECTOR_ADDRESS	0x1008
	#define REMAPPED_LOW_INTERRUPT_VECTOR_ADDRESS	0x1018
#elif defined(PROGRAMMABLE_WITH_USB_MCHPUSB_BOOTLOADER)	
	#define REMAPPED_RESET_VECTOR_ADDRESS			0x800
	#define REMAPPED_HIGH_INTERRUPT_VECTOR_ADDRESS	0x808
	#define REMAPPED_LOW_INTERRUPT_VECTOR_ADDRESS	0x818
#else	
	#define REMAPPED_RESET_VECTOR_ADDRESS			0x00
	#define REMAPPED_HIGH_INTERRUPT_VECTOR_ADDRESS	0x08
	#define REMAPPED_LOW_INTERRUPT_VECTOR_ADDRESS	0x18
#endif

#if defined(PROGRAMMABLE_WITH_USB_HID_BOOTLOADER) || defined(PROGRAMMABLE_WITH_USB_MCHPUSB_BOOTLOADER)
	extern void _startup (void);
	#pragma code REMAPPED_RESET_VECTOR = REMAPPED_RESET_VECTOR_ADDRESS
	void _reset (void)
	{
	    _asm goto _startup _endasm
	}
#endif

#pragma code REMAPPED_HIGH_INTERRUPT_VECTOR = REMAPPED_HIGH_INTERRUPT_VECTOR_ADDRESS
void Remapped_High_ISR (void)
{
     _asm goto highPriorityISRCode _endasm
}

#pragma code REMAPPED_LOW_INTERRUPT_VECTOR = REMAPPED_LOW_INTERRUPT_VECTOR_ADDRESS
void Remapped_Low_ISR (void)
{
     _asm goto lowPriorityISRCode _endasm
}

#if defined(PROGRAMMABLE_WITH_USB_HID_BOOTLOADER) || defined(PROGRAMMABLE_WITH_USB_MCHPUSB_BOOTLOADER)
#pragma code HIGH_INTERRUPT_VECTOR = 0x08
void High_ISR (void)
{
     _asm goto REMAPPED_HIGH_INTERRUPT_VECTOR_ADDRESS _endasm
}

#pragma code LOW_INTERRUPT_VECTOR = 0x18
void Low_ISR (void)
{
     _asm goto REMAPPED_LOW_INTERRUPT_VECTOR_ADDRESS _endasm
}
#endif

#pragma code

// High-priority ISR handling function
#pragma interrupt highPriorityISRCode
void highPriorityISRCode()
{
	// Application specific high-priority ISR code goes here
	
	#if defined(USB_INTERRUPT)
		//Perform USB device tasks
		USBDeviceTasks();
	#endif

}

// Low-priority ISR handling function
#pragma interruptlow lowPriorityISRCode
void lowPriorityISRCode()
{
	// Application specific low-priority ISR code goes here
    
    if(INTCONbits.TMR0IF){  // Checking the Timer0 interrupt bit
        mStatusLED0_Toggle();
        INTCONbits.TMR0IF = 0;    // Clearing the Timer0 interrupt bit
        
        // Clearing Timer0 (need to make sure TMR0H written to before TMR0L)
        TMR0H = 0b00000000;
        TMR0L = 0b00000000;
    }
    
}

// String for creating debug messages
char debugString[64];

// Main program entry point
void main(void)
{   
	// Initialise and configure the PIC ready to go
    initialisePic();

	// If we are running in interrupt mode attempt to attach the USB device
    #if defined(USB_INTERRUPT)
        USBDeviceAttach();
    #endif
	
	// Initialise the debug log functions
    debugInitialise();
	
	sprintf(debugString, "Sony Dualshock 2 Controller Adapter");
	debugOut(debugString);
	
	sprintf(debugString, "USB Device Initialised.");
	debugOut(debugString);
    
    SPI_Init_Master();		// Initialize SPI communication
    PS2_READ = SSPBUF;      // Copy PS2 controller output to PSX_READ
    T2CONbits.TMR2ON = 0;              // Turning Timer2 Off
	// Main processing loop
    while(1)
    {
        #if defined(USB_POLLING)
			// If we are in polling mode the USB device tasks must be processed here
			// (otherwise the interrupt is performing this task)
	        USBDeviceTasks();
        #endif
    	
    	// Process USB Commands
        processUsbCommands();
        
        if (PS2_READ != 0x9E)
        {
            INTCONbits.GIE = 0; // Disabling global interrupts, so no interrupt interferes with the timing here
            PS2_init();
        }
        else
        {
            INTCONbits.GIE = 1; // Enabling global interrupts
        }
        Poll();
        __delay_poll();
    }
}

// Initialise the PIC
static void initialisePic(void)
{
    // PIC port set up --------------------------------------------------------

	// Default all pins to digital
    ADCON1 = 0x0F;
    
    RCONbits.IPEN = 1;  // Enabling priority levels on interrupts
    INTCONbits.GIE = 1; // Enabling global interrupts
    INTCONbits.GIEL = 1; // Enabling global interrupts
    INTCONbits.GIEH = 1; // Enabling global interrupts
    
    // Writing to T0CON Timer0 control register
    T0CONbits.TMR0ON = 1;   // Turning Timer0 on
    T0CONbits.T08BIT = 0;     // 16-bit read/write mode enabled
    T0CONbits.T0CS = 0;   // Internal clock (FOSC/4)
    T0CONbits.PSA = 0;  //Timer0 prescaler is assigned, clock comes from prescaler output
    T0CONbits.T0PS0 = 1;  // Timer0 prescale valve set to 1:256
    T0CONbits.T0PS1 = 1;  // Timer0 prescale valve set to 1:256
    T0CONbits.T0PS2 = 1;  // Timer0 prescale valve set to 1:256

    // Clearing Timer0 (need to make sure TMR0H written to before TMR0L)
    TMR0H = 0b00000000;
    TMR0L = 0b00000000;
    
    // Setting Timer0 interrupt priority bit to low-priority
    INTCON2bits.TMR0IP = 0;
    
    // Clearing the Timer0 interrupt bit
    INTCONbits.TMR0IF = 0;
    
    // Enabling Timer0 interrupt
    INTCONbits.TMR0IE = 1;

	// Configure ports as inputs (1) or outputs(0)
	TRISA = 0b00100000;
	TRISB = 0b00000001;
	TRISC = 0b00000000;
#if defined(__18F4550)
	TRISD = 0b00000000;
	TRISE = 0b00000000;
#endif

	// Clear all ports
	PORTA = 0b00000000;
	PORTB = 0b00000000;
	PORTC = 0b00000000;
#if defined(__18F4550)
	PORTD = 0b00000000;
	PORTE = 0b00000000;
#endif

	// If you have a VBUS sense pin (for self-powered devices when you
	// want to detect if the USB host is connected) you have to specify
	// your input pin in HardwareProfile.h
    #if defined(USE_USB_BUS_SENSE_IO)
    	tris_usb_bus_sense = INPUT_PIN;
    #endif
    
    // In the case of a device which can be both self-powered and bus-powered
    // the device must respond correctly to a GetStatus (device) request and
    // tell the host how it is currently powered.
    //
    // To do this you must device a pin which is high when self powered and low
    // when bus powered and define this in HardwareProfile.h
    #if defined(USE_SELF_POWER_SENSE_IO)
    	tris_self_power = INPUT_PIN;
    #endif

    // Application specific initialisation
    applicationInit();
    
    // Initialise the USB device
    USBDeviceInit();
}

// Application specific device initialisation
void applicationInit(void)
{
    // Initialize the variable holding the USB handle for the last transmission
    USBOutHandle = 0;
    USBInHandle = 0;
}

unsigned char reverse(unsigned char b) {
   b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
   b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
   b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
   return b;
}
// Process USB commands
void processUsbCommands(void)
{   
    // Check if we are in the configured state; otherwise just return
    if((USBDeviceState < CONFIGURED_STATE) || (USBSuspendControl == 1))
    {
	    // We are not configured
	    return;
	}
    
    if (transmit)
    {
        USBInHandle = HIDTxPacket(HID_EP,(BYTE*)&ToSendDataBuffer,49);
        
        // Check if data was received from the host.
        //if(!HIDRxHandleBusy(USBOutHandle))
        //{   
            // Re-arm the OUT endpoint for the next packet
            USBOutHandle = HIDRxPacket(HID_EP,(BYTE*)&ReceivedDataBuffer,49);
        //}
    }
}

// USB Callback handling routines -----------------------------------------------------------

// Call back that is invoked when a USB suspend is detected
void USBCBSuspend(void)
{
}

// This call back is invoked when a wakeup from USB suspend is detected.
void USBCBWakeFromSuspend(void)
{
}

// The USB host sends out a SOF packet to full-speed devices every 1 ms.
void USBCB_SOF_Handler(void)
{
    // No need to clear UIRbits.SOFIF to 0 here. Callback caller is already doing that.
}

// The purpose of this callback is mainly for debugging during development.
// Check UEIR to see which error causes the interrupt.
void USBCBErrorHandler(void)
{
    // No need to clear UEIR to 0 here.
    // Callback caller is already doing that.
}

// Check other requests callback
void USBCBCheckOtherReq(void)
{
    // Handling DsHidMini driver device to host transfer setup reports
    if((SetupPkt.bmRequestType == 0xA1) && (SetupPkt.bRequest == 0x01))
    {
        switch(SetupPkt.wValue)
        {
            case 0x0301:
                USBEP0SendROMPtr((BYTE*)&Report01, 64, USB_EP0_ROM);
                break;

            case 0x03F2:
                USBEP0SendROMPtr((BYTE*)&ReportF2, 64, USB_EP0_ROM);
                break;

            case 0x03F5:
                USBEP0SendROMPtr((BYTE*)&ReportF5, 64, USB_EP0_ROM);
                break;

            case 0x03EF:
                USBEP0SendROMPtr((BYTE*)&ReportEF, 64, USB_EP0_ROM);
                break;
                
            case 0x03F8:
                USBEP0SendROMPtr((BYTE*)&ReportF8, 64, USB_EP0_ROM);
                break;

            case 0x03F7:
                USBEP0SendROMPtr((BYTE*)&ReportF7, 64, USB_EP0_ROM);
                break;

            default:
                break;
        }
    }

    // Handling DsHidMini driver host to device transfer setup reports
    if ((SetupPkt.bmRequestType == 0x21) && (SetupPkt.bRequest == 0x09))
    {
        switch(SetupPkt.wValue)
        {
            case 0x03F4:
                USBEP0Receive((BYTE*)&ReceivedDataBuffer, SetupPkt.wLength, NULL);
                transmit = FLAG_TRUE;
                break;
                
            case 0x03F5:
                USBEP0Receive((BYTE*)&ReceivedDataBuffer, SetupPkt.wLength, NULL);
                break;
                
            default:
                break;
        }
    }
    
    USBCheckHIDRequest();
}

// Callback function is called when a SETUP, bRequest: SET_DESCRIPTOR request arrives.
void USBCBStdSetDscHandler(void)
{
    // You must claim session ownership if supporting this request
}

//This function is called when the device becomes initialized
void USBCBInitEP(void)
{
    // Enable the HID endpoint
    USBEnableEndpoint(HID_EP,USB_IN_ENABLED|USB_OUT_ENABLED|USB_HANDSHAKE_ENABLED|USB_DISALLOW_SETUP);
    
    // Re-arm the OUT endpoint for the next packet
    USBOutHandle = HIDRxPacket(HID_EP,(BYTE*)&ReceivedDataBuffer,49);
}

// Send resume call-back
void USBCBSendResume(void)
{
    static WORD delay_count;
    
    // Verify that the host has armed us to perform remote wakeup.
    if(USBGetRemoteWakeupStatus() == FLAG_TRUE) 
    {
        // Verify that the USB bus is suspended (before we send remote wakeup signalling).
        if(USBIsBusSuspended() == FLAG_TRUE)
        {
            USBMaskInterrupts();
            
            // Bring the clock speed up to normal running state
            USBCBWakeFromSuspend();
            USBSuspendControl = 0; 
            USBBusIsSuspended = FLAG_FALSE;

            // Section 7.1.7.7 of the USB 2.0 specifications indicates a USB
            // device must continuously see 5ms+ of idle on the bus, before it sends
            // remote wakeup signalling.  One way to be certain that this parameter
            // gets met, is to add a 2ms+ blocking delay here (2ms plus at 
            // least 3ms from bus idle to USBIsBusSuspended() == FLAG_TRUE, yeilds
            // 5ms+ total delay since start of idle).
            delay_count = 3600U;        
            do
            {
                delay_count--;
            } while(delay_count);
            
            // Start RESUME signaling for 1-13 ms
            USBResumeControl = 1;
            delay_count = 1800U;
            do
            {
                delay_count--;
            } while(delay_count);
            USBResumeControl = 0;

            USBUnmaskInterrupts();
        }
    }
}

// USB callback function handler
BOOL USER_USB_CALLBACK_EVENT_HANDLER(USB_EVENT event, void *pdata, WORD size)
{
    switch(event)
    {
        case EVENT_TRANSFER:
            // Application callback tasks and functions go here
            break;
        case EVENT_SOF:
            USBCB_SOF_Handler();
            break;
        case EVENT_SUSPEND:
            USBCBSuspend();
            break;
        case EVENT_RESUME:
            USBCBWakeFromSuspend();
            break;
        case EVENT_CONFIGURED: 
            USBCBInitEP();
            break;
        case EVENT_SET_DESCRIPTOR:
            USBCBStdSetDscHandler();
            break;
        case EVENT_EP0_REQUEST:
            USBCBCheckOtherReq();
            break;
        case EVENT_BUS_ERROR:
            USBCBErrorHandler();
            break;
        default:
            break;
    }      
    return FLAG_TRUE; 
}

// One program count delay function
void __delay_pc(unsigned char delaytime)
{
    auto unsigned char i;
    for (i = 0; i < delaytime; i++) {
        Nop();
    }
    
}

// Millisecond delay function
void __delay_ms(unsigned char delaytime)
{
    Delay1KTCYx(delaytime * 12);
}

// Time delay function between polls, getting this time right can be fussy
void __delay_poll()
{
    Delay1KTCYx(39);
}

void SPI_Init_Master()
{
    // Configuring registers for SPI communication
    SSPSTAT=0b00000000;	// Data change on rising edge of clock , buffer full (BF) status set to 0
    SSPCON1=0b00110011; // SPI master mode, serial enable, idle state high for clock, SPI clock to be dependent on TMR2 = PR2

    // Clearing Timer2 and PR2 registers
    TMR2 = 0b00000000;
    PR2 = 0b00001111;
    
    // Clearing the SPI transmission and reception completed interrupt flag
    PIR1bits.SSPIF=0;

    // Disable the ADC channel which are on for multiplexed pin when used as an input 
    ADCON0=0;   // This is for de-multiplexing the SCL and SDI from analog pins
    ADCON1=0x0F;    // This makes all pins as digital I/O
}

unsigned char SPI_Write(unsigned char x)
{
    unsigned char data_flush;
    TMR2 = 0;   // Clearing Timer2
    SSPBUF=x;   // Copy data in SSBUF to transmit
    T2CONbits.TMR2ON = 1;

    while(!PIR1bits.SSPIF); // Wait for complete 1 byte transmission
    PIR1bits.SSPIF=0;   // Clear SSPIF flag
    //T2CONbits.TMR2ON = 0;
    data_flush=SSPBUF;  //Flush the data
    __delay_pc(29);
    return data_flush;
}

/*------------------------------------------------------------------------------
                      PS2 CONTROLLER SUBROUTINES
 -------------------------------------------------------------------------------*/

// Short poll
void ShortPoll(void)
{   
    PS2_ATT=0;
	__delay_pc(21); // Using 21 roughly delays by 16us
    SPI_Write(0x80);
    SPI_Write(0x42);
    SPI_Write(0x00);
    SPI_Write(0x00);
    SPI_Write(0x00);
    __delay_pc(21); // 16 us delay
	PS2_ATT=1;
}

// To enter the Dualshock 2 controllers config so that the PIC18F4550 can enable both analog mode and pressure sensitive buttons
void EnterConfig(void)
{
    PS2_ATT=0;
	__delay_pc(21); // 16 us delay
    SPI_Write(0x80);
    SPI_Write(0xC2);
    PS2_READ = SPI_Write(0x00);
    SPI_Write(0x80);
    SPI_Write(0x00);
    __delay_pc(21); // 16 us delay
	PS2_ATT=1;
}

// Turns on analog mode
void TurnOnAnalogMode(void)
{
    PS2_ATT=0;
	__delay_pc(21); // 16 us delay
    SPI_Write(0x80);
    SPI_Write(0x22);
    SPI_Write(0x00); // Why is this here?
    SPI_Write(0x80);
    SPI_Write(0xC0);    // Lock on to analogue mode (user cannot toggle digital & analogue)
    SPI_Write(0x00);
    SPI_Write(0x00);
    SPI_Write(0x00);
    SPI_Write(0x00);
    __delay_pc(21); // 16 us delay
	PS2_ATT=1;
}

// Turns on pressure sensitive buttons
void TurnOnPressureButtons(void)
{
    PS2_ATT=0;
	__delay_pc(21); // 16 us delay
    SPI_Write(0x80);
    SPI_Write(0xF2);
    SPI_Write(0x00);
    SPI_Write(0xFF);
    SPI_Write(0xFF);
    SPI_Write(0xC0);
    SPI_Write(0x00);
    SPI_Write(0x00);
    SPI_Write(0x00);
    __delay_pc(21); // 16 us delay
	PS2_ATT=1;
}

// Checks that buttons are pressure sensitive
void CheckPressureButtons(void)
{
    PS2_ATT=0;
	__delay_pc(21); // 16 us delay
    SPI_Write(0x80);
    SPI_Write(0x82);
    SPI_Write(0x00);
    SPI_Write(0x5A);
    SPI_Write(0x5A);
    SPI_Write(0x5A);
    SPI_Write(0x5A);
    SPI_Write(0x5A);
    SPI_Write(0x5A);
    __delay_pc(21); // 16 us delay
	PS2_ATT=1;
}

// For configurating the which bytes from the PIC18F4550 to the Dualshock 2 controller with the Poll() command will enable the vibration motors)
void SetupVibrationMotors(void)
{
    PS2_ATT=0;
	__delay_pc(21); // 16 us delay
    SPI_Write(0x80);
    SPI_Write(0xB2);
    SPI_Write(0x00);
    SmallMotorSet = SPI_Write(0x00);    //Maps the corresponding byte in 0x42 to control the small motor
    LargeMotorSet = SPI_Write(0x80);    //Maps the corresponding byte in 0x42 to control the large motor
    SPI_Write(0xFF);
    SPI_Write(0xFF);
    SPI_Write(0xFF);
    SPI_Write(0xFF);
    __delay_pc(21); // 16 us delay
	PS2_ATT=1;
}

// Exit config
void Exitconfig(void)
{
    PS2_ATT=0;
	__delay_pc(21); // 16 us delay
    SPI_Write(0x80);
    SPI_Write(0xC2);
    SPI_Write(0x00);
    SPI_Write(0x00);
    SPI_Write(0x5A);
    SPI_Write(0x5A);
    SPI_Write(0x5A);
    SPI_Write(0x5A);
    SPI_Write(0x5A);
    __delay_pc(21); // 16 us delay
	PS2_ATT=1;
}

//Poll
void Poll(void)
{
    /*
    Output report info of Sony Dualshock 3 controller, which we're mimicking for this to be compatible with the DsHidMini driver so that it functions with this adapter.
    Provided by https://eleccelerator.com/wiki/index.php/DualShock_3
    
    Byte 0 is the report ID
    Byte 1 is reserved
    Byte 2, 3, 4, and 5 contain button info. Active high (1 means pressed, 0 means released)
    Byte 2: Bit 7 - D Left, Bit 6 - D Down, Bit 5 - D Right, Bit 4 - D Up, Bit 3 - Start, Bit 2 - R3, Bit 1 - L3, Bit 0 - Select
    Byte 3: Bit 7 - Square, Bit 6 - X, Bit 5 - Circle, Bit 4 - Triangle, Bit 3 - R1, Bit 2 - L1, Bit 1 - R2, Bit 0 - L2
    Byte 4: All undefined except for Bit 0 - PS
    Byte 5: All undefined
    Byte 6 is left stick X axis; byte 7 is left stick Y axis; byte 8 is right stick X axis; byte 9 is right stick Y axis; top left is 0
    Byte 14 to byte 25 contains analog button data, 8 bits each, 0 is released and FF is fully pressed
    Byte 14: Analog D Up
    Byte 15: Analog D Right
    Byte 16: Analog D Down
    Byte 17: Analog D Left
    Byte 18: Analog Left Trigger
    Byte 19: Analog Right Trigger
    Byte 20: Analog Left Shoulder
    Byte 21: Analog Right Shoulder
    Byte 22: Analog Triangle
    Byte 23: Analog Circle
    Byte 24: Analog Cross
    Byte 25: Analog Square
    Byte 40 and 41: accelerometer X axis, little endian 10 bit unsigned
    Byte 42 and 43: accelerometer Y axis, little endian 10 bit unsigned
    Byte 44 and 45: accelerometer Z axis, little endian 10 bit unsigned
    Byte 46 and 47: gyroscope, little endian 10 bit unsigned
    
    Input report info
    Byte 3: Small Motor Strength
    Byte 5: Large Motor Strength
    */
    MOTORBOOST = !(ReceivedDataBuffer[3] | ReceivedDataBuffer[5]);
    PS2_ATT=0;
	__delay_pc(21); // 16 us delay
    SPI_Write(0x80);
    PS2_READ = SPI_Write(0x42);
    SPI_Write(0x00);
    ToSendDataBuffer[2] = reverse(SPI_Write(0xFF * ReceivedDataBuffer[3])) ^ 0xF;
    ToSendDataBuffer[3] = reverse(SPI_Write(ReceivedDataBuffer[5]));
    ToSendDataBuffer[8] = reverse(SPI_Write(0x00));  // RightJoyX
    ToSendDataBuffer[9] = reverse(SPI_Write(0x00));  // RightJoyY
    ToSendDataBuffer[6] = reverse(SPI_Write(0x00));  // LeftJoyX
    ToSendDataBuffer[7] = reverse(SPI_Write(0x00));  // LeftJoyY
    ToSendDataBuffer[15] = reverse(SPI_Write(0x00));  // DPAD Right
    ToSendDataBuffer[17] = reverse(SPI_Write(0x00));  // DPAD Left
    ToSendDataBuffer[14] = reverse(SPI_Write(0x00));  // DPAD Up
    ToSendDataBuffer[16] = reverse(SPI_Write(0x00));  // DPAD Down
    ToSendDataBuffer[22] = reverse(SPI_Write(0x00)); // Triangle
    ToSendDataBuffer[23] = reverse(SPI_Write(0x00)); // Circle
    ToSendDataBuffer[24] = reverse(SPI_Write(0x00)); // Cross
    ToSendDataBuffer[25] = reverse(SPI_Write(0x00)); // Square
    ToSendDataBuffer[20] = reverse(SPI_Write(0x00)); // L1
    ToSendDataBuffer[21] = reverse(SPI_Write(0x00)); // R1
    ToSendDataBuffer[18] = reverse(SPI_Write(0x00)); // L2
    ToSendDataBuffer[19] = reverse(SPI_Write(0x00)); // R2
    __delay_pc(21); // 16 us delay
	PS2_ATT=1;
}

//Initialize
void PS2_init(void)
{
    ToSendDataBuffer[0] = 0x01; // Setting the report ID to send to the host to 0x01
    EnterConfig();
    if (PS2_READ != 0x5A)
    {
        return;
    }
    __delay_pc(21); // 16 us delay
    TurnOnAnalogMode();
    __delay_pc(21); // 16 us delay
    
    resetmotor: // Occasionally the vibration motor won't set, so SetupVibrationMotors() runs at least twice, one to set and the next one to check
            
    SetupVibrationMotors();
    __delay_pc(21); // 16 us delay
    if (SmallMotorSet != 0x00 | LargeMotorSet != 0x80)
    {
        goto resetmotor;
    }
    SetupVibrationMotors();
    __delay_pc(21); // 16 us delay
    
    TurnOnPressureButtons();
    __delay_pc(21); // 16 us delay
    Exitconfig();
    __delay_ms(5);
    ReceivedDataBuffer[3] = 0x00;   // Ensuring vibration motors start off
    ReceivedDataBuffer[5] = 0x00;   // Ensuring vibration motors start off
    Poll(); // First poll
    __delay_poll();
}

#endif
