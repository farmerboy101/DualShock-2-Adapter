/************************************************************************
	debug.h

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

#ifndef DEBUG_H
#define DEBUG_H

// Microchip Application Library includes
// (expects V2.9a of the USB library from "Microchip Solutions v2011-07-14")
#include "./USB/usb.h"
#include "./USB/usb_function_hid.h"

// Set the size of the debug information buffer in characters
#define DEBUGBUFFERSIZE 128

// Function prototypes
void debugInitialise(void);
void debugOut(char*);
void copyDebugToSendBuffer(BYTE* sendDataBuffer);

#endif