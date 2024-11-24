#include <hardware/adc.h>
#include "temp_sense.h"

#define ADC_CHANNEL_TEMPSENSOR 4

void temperature_setup(void)
{
   adc_init();
   adc_select_input(ADC_CHANNEL_TEMPSENSOR);
   adc_set_temp_sensor_enabled(true);
}

float temperature_poll(void)
{
    adc_select_input(ADC_CHANNEL_TEMPSENSOR);
    uint32_t raw32 = adc_read();
    const uint32_t bits = 12;

    // Scale raw reading to 16 bit value using a Taylor expansion (for 8 <= bits <= 16)
    uint16_t raw16 = raw32 << (16 - bits) | raw32 >> (2 * bits - 16);

    // ref https://github.com/raspberrypi/pico-micropython-examples/blob/master/adc/temperature.py
    const float conversion_factor = 3.3 / (65535);
    float reading = raw16 * conversion_factor;

    // The temperature sensor measures the Vbe voltage of a biased bipolar diode, connected to the fifth ADC channel
    // Typically, Vbe = 0.706V at 27 degrees C, with a slope of -1.721mV (0.001721) per degree.
    float deg_c = 27 - (reading - 0.706) / 0.001721;
    return deg_c;
}
