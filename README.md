# VictronVEDirectArduino
Fork from winginitau/VictronVEDirectArduino to change to a non blocking reading approach.

Lightweight Arduino library to read data using the VE.Direct protocol from Victron Energy devices

Built as part of a larger project, now making it available separately in case others find it useful.

Setup:
 - An Arduino(ish) board 
 - A Victron Energy device that sends serial data using the text version of the VE.Direct protocol
 - For BMV products : A 5v to 3.3v serial converter (BMV is 3.3v - don't plug it directly into a 5v Arduino!)
 - Plugged into the Arduino on a serial port (eg Serial1, Serial2, Serial3 etc)
 - See also: https://www.victronenergy.com/live/vedirect_protocol:faq
 - Distributed under an MIT license - see LICENCE.txt

Provides:
 - Read access to VE.Direct statistics and settings
 - Defaults set to read Volts, Power, Current, State of Charge (SOC), Alarm
 - Easily extendible by adding labels for any other stats and settings of interest
 - A diagnostic "full dump" of everything coming from the device  

### Usage example for MPPT:
```C
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

	if(MPPT.available()) // Is true when a full TextFrame has been read from VE.Device and so new data are available
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

```

