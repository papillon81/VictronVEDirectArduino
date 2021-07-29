/******************************************************************
 VEDirect Arduino

 Forked from Brendan McLearie VEDirect work
 Modified by Théo Hyvon

 See README.md

 File: VEDirect.cpp
 - Class / enums / API

 Updates:
 - 2021-01-29:
	- Changing reading method to a non-blocking approach based on state machine presented on VE FAQ
	 --> https://www.victronenergy.com/live/vedirect_protocol:faq#framehandler_reference_implementation

	- Adding MPPT and Phoenix labels
 - 2019-07-14:
	- Rewrite of read - cleaner.
	- Target labels extendible with enum and PROGMEM strings
	- Retired copy_raw_to_serial0() code - use VE_DUMP on read
	- Added some tunable parameters see #defines
******************************************************************/

#include "VEDirect.h"

VEDirect::VEDirect(HardwareSerial& port): mVESerial(port)
	// Initialise the serial port that the
	// VE.Direct device is connected to and
	// store it for later use.
{
	mState = Idle;
	mRX_Pointer = 0;
	mCurrentLabel = VE_DUMP;
	mChecksum = 0;
	mNewData = false;
}

uint8_t VEDirect::begin(uint8_t rxPin, uint8_t txPin)
{
	// Check connection with the serial port
	mVESerial.begin(VED_BAUD_RATE, SERIAL_8N1, rxPin, txPin);
	if (mVESerial)
	{
		delay(1500);
		if(mVESerial.available())
		{
			mVESerial.flush();
			return 1;
		}

		return 0;
	}
	return 0;
}

bool VEDirect::available()
{
	return mNewData;
}

int32_t VEDirect::read(uint8_t Label)
{

	int32_t value;

	mNewData = false;

	if(Label >= VE_LAST_LABEL)
		return -1;

	if (mData[Label][0] == 'O')
	{
		//ON OFF type
		if (mData[Label][1] == 'N')
			value = 1;	// ON

		else
			value = 0;	// OFF
	}

	else
	{
		sscanf(mData[Label], "%ld", &value);
	}

	return value;
}

void VEDirect::update()
{

	while(mVESerial.available())
	{
		uint8_t inbyte = mVESerial.read();

		if(inbyte == ':')
			mState = Idle;

		switch(mState)
		{
			case Idle:
				/* wait for \n of the start of an record */
				switch(inbyte)
				{
					case '\n':
						mState = Rec_Begin;
						break;

					case '\r': /* Skip */
					default:
						break;
				}
				break;

			case Rec_Begin:
				mRX_Pointer = 0;
				mRX_Buffer[mRX_Pointer] = inbyte;
				mRX_Pointer ++;

				mState = Rec_Name;
				break;

			case Rec_Name:
				// The record name is being received, terminated by a \t
				switch(inbyte)
				{
					case '\t':
						if (mRX_Pointer < VED_LINE_SIZE)
						{
							mRX_Buffer[mRX_Pointer] = 0; /* Zero terminate */

							for(uint8_t i = 0; i < VE_LAST_LABEL; i++)
							{

								if (strcmp(mRX_Buffer, ved_labels[i]) == 0)
								{
									mCurrentLabel = i;
									break;
								}
							}

							if(mCurrentLabel == VE_CHECKSUM) // If label is checksum change state to checksum verification
								mState = Checksum;
							else if(mCurrentLabel == VE_LAST_LABEL) // Label hasn't been found, going back to Idle State
							{
								mState = Idle;
								Serial.println("##### Framing Error : Unknown label #####");
							}

							else
							{
								mState = Rec_Value;
								mRX_Pointer = 0;
							}

						}

						else	// Buffer overflow going back to Idle
						{
							mState = Idle;
							Serial.println("##### Framing Error : Overflow Tab #####");
						}

						break;

					default:
						// add byte to name, and check overflow
						if (mRX_Pointer< VED_LINE_SIZE)
						{
							mRX_Buffer[mRX_Pointer] = inbyte;
							mRX_Pointer++;
						}
						else	// Buffer overflow going back to Idle
						{
							mState = Idle;
							Serial.println("##### Framing Error : Overflow Name #####");
							Serial.println("Dump : ");
							Serial.println(mRX_Buffer);
							Serial.println("#####");
						}

						break;
				}
				break;

			case Rec_Value:
				// The record value is being received.  The \r indicates a new record.
				switch(inbyte)
				{
					case '\n':
						// forward record
						if (mRX_Pointer < VED_MAX_VALUE_SIZE)
						{
							mRX_Buffer[mRX_Pointer] = 0; // make zero ended

							if(mCurrentLabel < VE_LAST_LABEL)	// Is selected Label correct ?
							{
								strcpy(mData[mCurrentLabel], mRX_Buffer);
							}
						}

						mState = Rec_Begin;
						break;

					case '\r': /* Skip */
						break;

					default:
						// add byte to name, and check overflow
						if (mRX_Pointer< VED_LINE_SIZE)
						{
							mRX_Buffer[mRX_Pointer] = inbyte;
							mRX_Pointer++;

						}
						else	// Buffer overflow going back to Idle
						{
							mState = Idle;
							Serial.println("##### Framing Error : Overflow Value #####");
						}
						break;
				}
				break;

			case Checksum:
				mChecksum = 0;
				mState = Idle;
				mNewData = true;

				break;
		}
	}

}

