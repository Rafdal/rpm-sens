#ifndef RPM_DATA_BUFFER_H
#define RPM_DATA_BUFFER_H

#define RPM_DATA_CIRCULAR_BUFFER_SIZE 16

class RPM_DATA_BUFFER_t 
{
public:
    uint8_t last_pulses = 0; // Pulses in the last second
    uint8_t pulses = 0;
    uint8_t size = 0;  // Size of the circular buffer

    inline void cycle_pulses() volatile
    {
        last_pulses = pulses;
        pulses = 0; // Reset the pulse count
    }

    inline void call(unsigned long ms) volatile
    {
        pulses++;
        push_delta_ms(ms - timestamp_ms);
        timestamp_ms = ms; // Actualiza el tiempo de la última interrupción
    }
    
    inline void push_delta_ms(unsigned long delta) volatile
    {
        delta_ms_buf[head] = delta;
        head = (head + 1) % RPM_DATA_CIRCULAR_BUFFER_SIZE;
        if (size < RPM_DATA_CIRCULAR_BUFFER_SIZE)
            size++;
        else
            tail = (tail + 1) % RPM_DATA_CIRCULAR_BUFFER_SIZE;
    }
    inline unsigned long get_mean_delta_ms() volatile
    {
        if (size == 0)
            return 0;
        unsigned long sum = 0;
        uint8_t tmp_size = size;
        while (size > 0)
            sum += pop_delta_ms();
        return sum / tmp_size;
    }            
    
private:
    inline unsigned long pop_delta_ms() volatile
    {
        if (size == 0)
            return 0;
        unsigned long delta = delta_ms_buf[tail];
        tail = (tail + 1) % RPM_DATA_CIRCULAR_BUFFER_SIZE;
        size--;
        return delta;
    }
    unsigned long timestamp_ms = 0; // Timestamp in milliseconds
        
    unsigned long delta_ms_buf[RPM_DATA_CIRCULAR_BUFFER_SIZE];     // Time difference in milliseconds
    uint8_t head = 0;
    uint8_t tail = 0;
};

#endif // RPM_DATA_BUFFER_H