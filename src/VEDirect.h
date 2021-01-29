/******************************************************************
 VEDirect Arduino

 Copyright 2018, 2019, Brendan McLearie
 Distributed under MIT license - see LICENSE.txt

 See README.md

 File: VEDirect.h
 - Class / enums / API

 Updates:
 - 2019-07-14:
  	  - Rewrite of read - cleaner.
  	  - Target labels extendible with enum and PROGMEM strings
  	  - Retired copy_raw_to_serial0() code - use VE_DUMP on read
  	  - Added some tunable parameters see #defines
******************************************************************/

#ifndef VEDIRECT_H_
#define VEDIRECT_H_

#include <Arduino.h>

// Tunable parameters - defaults tested on mega2560 R3
#define VED_LINE_SIZE 20		 // Seems to be plenty. VE.Direct protocol could change

#define VED_MAX_LABEL_SIZE 10	 // Max length of value
#define VED_MAX_VALUE_SIZE 10	 // Max length of value

#define VED_MAX_READ_LOOPS 60000 // How many read loops to be considered a read time-out
#define VED_MAX_READ_LINES 50	 // How many lines to read looking for a value
								 // before giving up. Also determines lines for diag dump
#define VED_RX_BUFFER_SIZE	50   // RX buffer size by line

#define VED_BAUD_RATE 19200

// Extend this and ved_labels[] for needed inclusions
enum VE_DIRECT_DATA {
	VE_DUMP = 0,
	VE_SOC,
	VE_VOLTAGE,
	VE_POWER,
	VE_CURRENT,
	VE_ALARM,
	VE_PANEL_VOLTAGE,
	VE_PANEL_POWER,
	VE_BATTERY_LOAD_CURRENT,
	VE_LOAD_STATE,
	VE_RELAY_STATE,
	VE_ALARM_REASON,
	VE_YIELD_TOTAL,
	VE_YIELD_TODAY,
	VE_MAX_DAILY_POWER,
	VE_YIELD_YESTERDAY,
	VE_MAX_YESTERDAY_POWER,
	VE_ERROR_CODE,
	VE_STATE_OF_OPERATION,
	VE_FW,
	VE_PID,
	VE_DAY_SEQUENCE_NUMBER,
	VE_DEVICE_MODE,
	VE_AC_OUTPUT_VOLTAGE,
	VE_AC_OUTPUT_CURRENT,
	VE_WARNING_REASON,
	VE_CHECKSUM,
	VE_LAST_LABEL
};

const char ved_labels[VE_LAST_LABEL][VED_MAX_LABEL_SIZE] PROGMEM = {
		"Dump",	// a string that won't match any label
		"SOC",
		"V",
		"P",
		"I",
		"Alarm",
		"VPV",
		"PPV",
		"IL",
		"LOAD",
		"Relay",
		"AR",
		"H19",
		"H20",
		"H21",
		"H23",
		"ERR",
		"CS",
		"FW",
		"PID",
		"HSDS",
		"MODE",
		"AC_OUT_V",
		"AC_OUT_I",
		"WARN",
		"Checksum"
};

class VEDirect
{
	public:
		VEDirect(HardwareSerial& port);
		virtual ~VEDirect();
		uint8_t begin();
		void update();

		int32_t read(uint8_t Label);

		bool available();


	private:
		HardwareSerial& mVESerial;

		bool mNewData;

		enum States{Idle, Rec_Begin, Rec_Name, Rec_Value, Checksum};

		uint8_t mCurrentLabel;

		char mData[VE_LAST_LABEL][VED_MAX_VALUE_SIZE];
		char mRX_Buffer[VED_LINE_SIZE];
		uint8_t mRX_Pointer;
		uint8_t mState;
		uint16_t mChecksum;

};

#endif /* VEDIRECT_H_ */
