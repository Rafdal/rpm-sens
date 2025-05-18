#ifndef RPM_SENSORS_H
#define RPM_SENSORS_H

#include <Arduino.h>

enum {RPM_SENSOR_MILLIS, RPM_SENSOR_MICROS};

typedef struct RPM_SENSOR_Data {
    unsigned long period;         // Time difference
    uint16_t dps;                 // Degrees per second
    uint16_t rpm;                 // Revolutions per minute
    uint8_t pulses_last_read;     // Pulses in the last read
    RPM_SENSOR_Data() {
        period = 0;
        dps = 0;
        rpm = 0;
        pulses_last_read = 0;
    }
} RPM_Data_t;

void RPM_sensors_begin(uint8_t S1_PIN, uint8_t S2_PIN, uint8_t time_res = RPM_SENSOR_MILLIS);

void RPM_sensors_read(RPM_Data_t *s1_data, RPM_Data_t *s2_data);

void RPM_sensors_dump_data(Stream &stream);

void RPM_sensors_run(unsigned long ms = millis());

#endif // RPM_SENSORS_H