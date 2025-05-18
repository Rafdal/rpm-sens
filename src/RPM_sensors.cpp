
#include "RPM_sensors.h"

// https://developer.arm.com/documentation/ddi0419/c/System-Level-Architecture/System-Level-Programmers--Model/Registers/The-special-purpose-mask-register--PRIMASK?lang=en
// https://developer.arm.com/documentation/ddi0439/b/Programmers-Model/Instruction-set-summary/Cortex-M4-instructions
static inline void enable_irqs()  {  __ASM volatile ("cpsie i" : : : "memory"); }
static inline void disable_irqs() {  __ASM volatile ("cpsid i" : : : "memory"); }

static void interruptHandler_S1();
static void interruptHandler_S2();

#include "rpm_data_buffer.h"

// Variables para almacenar tiempos
static volatile RPM_DATA_BUFFER_t _s1_data;
static volatile RPM_DATA_BUFFER_t _s2_data;
static unsigned long last_run_ms = 0;

static volatile uint8_t _time_res = RPM_SENSOR_MILLIS; // Resolución de tiempo por defecto

static void calculate_angular_velocity(RPM_Data_t *data);

void RPM_sensors_begin(uint8_t S1_PIN, uint8_t S2_PIN, uint8_t time_res)
{
    _time_res = time_res;
    pinMode(S1_PIN, INPUT); // Pin de interrupción
	pinMode(S2_PIN, INPUT); // Pin de interrupción

    attachInterrupt(digitalPinToInterrupt(S1_PIN), interruptHandler_S1, FALLING);
    attachInterrupt(digitalPinToInterrupt(S2_PIN), interruptHandler_S2, FALLING);
}

void RPM_sensors_read(RPM_Data_t *s1, RPM_Data_t *s2)
{
    disable_irqs();
    uint8_t s1_pulses = _s1_data.pulses;
    uint8_t s2_pulses = _s2_data.pulses;
    if (_s1_data.size)
    {
        s1->period = _s1_data.get_mean_delta_ms();
        calculate_angular_velocity(s1);
    }
    if (_s2_data.size)
    {
        s2->period = _s2_data.get_mean_delta_ms();
        calculate_angular_velocity(s2);
    }
    s1->pulses_last_read = _s1_data.last_pulses;
    s2->pulses_last_read = _s2_data.last_pulses;
    enable_irqs();

}

void RPM_sensors_dump_data(Stream &stream)
{
    RPM_Data_t s1, s2;
    RPM_sensors_read(&s1, &s2);
    stream.print("S1: ");
    stream.print(s1.period);
    stream.print("xs, ");
    stream.print(s1.dps);
    stream.print("dps, ");
    stream.print(s1.rpm);
    stream.print("rpm, ");
    stream.print(s1.pulses_last_read);
    stream.println();

    stream.print("S2: ");
    stream.print(s2.period);
    stream.print("xs, ");
    stream.print(s2.dps);
    stream.print("dps, ");
    stream.print(s2.rpm);
    stream.print("rpm, ");
    stream.print(s2.pulses_last_read);
    stream.println();
}

void RPM_sensors_run(unsigned long ms)
{
    if (ms - last_run_ms < 2000) return; // Ejecutar cada segundo
    last_run_ms = ms;

    disable_irqs();
    _s1_data.cycle_pulses();
    _s2_data.cycle_pulses();
    enable_irqs();
}

static void interruptHandler_S1()
{
    if (_time_res == RPM_SENSOR_MICROS)
        _s1_data.call( micros() );
    else
        _s1_data.call( millis() );
}

static void interruptHandler_S2()
{
    if (_time_res == RPM_SENSOR_MICROS)
        _s2_data.call( micros() );
    else
        _s2_data.call( millis() );
}

static void calculate_angular_velocity(RPM_Data_t *data)
{
    if (data->period <= 10)
    {
        data->dps = (uint16_t)9999;
        data->rpm = (uint16_t)9999;
        return;
    }

    if (_time_res == RPM_SENSOR_MICROS)
    {
        data->dps = (uint16_t)(360000000UL / data->period);
        data->rpm = (uint16_t)(60000000UL / data->period);
    }
    else
    {
        data->dps = (uint16_t)(360000UL / data->period);
        data->rpm = (uint16_t)(60000UL / data->period);
    }
}