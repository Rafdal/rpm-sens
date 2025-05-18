#include <Arduino.h>

#include <RPM_sensors.h>

#include "pin_defs.h"	// Definiciones de pines

unsigned long last_ms = 0;

void setup()
{
	Serial.begin(1000000);
	while (!Serial) ;

	RPM_sensors_begin(S1_A1, S2_A2, RPM_SENSOR_MICROS);
}

void loop()
{
	if (millis() - last_ms >= 1000UL) {
		last_ms = millis();

        RPM_Data_t s1, s2;
		RPM_sensors_read(&s1, &s2);
		Serial.print("S1: ");
		Serial.print(s1.dps);
		Serial.print("dps, ");
		Serial.print(s1.rpm);
		Serial.println("rpm, ");
	}

	RPM_sensors_run();
}