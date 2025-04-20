#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"

#define PIN_CS 15

static inline void cs_select(uint cs_pin);
static inline void cs_deselect(uint cs_pin);

int main()
{
    stdio_init_all();

    spi_init(spi_default, 1000 * 1000); // the baud, or bits per second
    gpio_set_function(PICO_DEFAULT_SPI_RX_PIN, GPIO_FUNC_SPI);
    gpio_set_function(PICO_DEFAULT_SPI_SCK_PIN, GPIO_FUNC_SPI);
    gpio_set_function(PICO_DEFAULT_SPI_TX_PIN, GPIO_FUNC_SPI);

    int s;
    int t = 0;
    for (s = 0; s < 100; s++) { // generate 2 Hz sine wave
        sleep_ms(0.01);
        t=t+0.01;
        v = sin(t);
        data_write(0, v);
    }
}

void data_write(int channel, float voltage) {
    uint8_t data[2];
    int len = 2;
    data[0] = 0;
    data[0] = data[0] | (channel<<7);
    data[0] = data[0] | (0b111<<4);
    uint16_t v = voltage * 4095 / 3.3;
    data[0] = data[0] | (v<<2);

    cs_select(PIN_CS);
    spi_write_blocking(SPI_PORT, data, len); // where data is a uint8_t array with length len
    cs_deselect(PIN_CS);
}

static inline void cs_select(uint cs_pin) {
    asm volatile("nop \n nop \n nop"); // FIXME
    gpio_put(cs_pin, 0);
    asm volatile("nop \n nop \n nop"); // FIXME
}

static inline void cs_deselect(uint cs_pin) {
    asm volatile("nop \n nop \n nop"); // FIXME
    gpio_put(cs_pin, 1);
    asm volatile("nop \n nop \n nop"); // FIXME
}

float sin(float time) { // generate a value along a 2Hz sine wave centered around 1.65 and an amplitude of 1.65
    return 1.65*math.sin(((2*pi)/0.5)*time) + 1.65;
}