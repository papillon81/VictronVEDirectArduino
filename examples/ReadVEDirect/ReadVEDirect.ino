/******************************************************************
 VEDirect Arduino

 Copyright 2018, 2019, Brendan McLearie
 Distributed under MIT license - see LICENSE.txt

 See README.md

 File: ReadVEDirect.ino / ReadVEDirect.cpp
 - Provides example use of the VEDirect library
******************************************************************/

#include "Arduino.h"
#include "VEDirect.h"

VEDirect MPPT(Serial1);

float MainBatteryVoltage = 0.f;
float PanelVoltage = 0.f;
float PanelPower = 0.f;
float BatteryCurrent = 0.f;
float LoadCurrent = 0.f;


void setup()
{
	Serial.begin(19200);

	Serial.print("MPPT connection... ");
	if(MPPT.begin())
		Serial.println("OK");
	else
	{
		Serial.println("Failed");
		delay(10000);
		setup();
	}
}


void loop()
{

	MPPT.update();

	if(MPPT.available())
	{
		MainBatteryVoltage = MPPT.read(VE_VOLTAGE)*0.001;
		PanelVoltage = MPPT.read(VE_PANEL_VOLTAGE)*0.001;
		PanelPower = MPPT.read(VE_PANEL_POWER);
		BatteryCurrent =MPPT.read(VE_CURRENT)*0.001;
		LoadCurrent = MPPT.read(VE_BATTERY_LOAD_CURRENT)*0.001;

		Serial.println("#####");
		Serial.print("Battery : "); Serial.print(MainBatteryVoltage, 2); Serial.println(" V");
		Serial.print("Panel : "); Serial.print(PanelVoltage, 2); Serial.println(" V");
		Serial.print("Panel : "); Serial.print(PanelPower, 2); Serial.println(" W");
		Serial.print("Battery Current : "); Serial.print(BatteryCurrent, 2); Serial.println(" A");
		Serial.print("Load : "); Serial.print(LoadCurrent, 2); Serial.println(" A");
		Serial.println("#####"); Serial.println();

	}

	delay(10);
}
