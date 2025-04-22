#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "math.h"

#define SPI_PORT spi0
#define PIN_CS 15
#define PI 3.1415926535

void data_write(int channel, float voltage);
static inline void cs_select(uint cs_pin);
static inline void cs_deselect(uint cs_pin);
float sin_val(float time);
float triangle(float time);

int main() {
    stdio_init_all();

    spi_init(spi_default, 1000 * 1000); // the baud, or bits per second
    gpio_set_function(PICO_DEFAULT_SPI_RX_PIN, GPIO_FUNC_SPI);
    gpio_set_function(PICO_DEFAULT_SPI_SCK_PIN, GPIO_FUNC_SPI);
    gpio_set_function(PICO_DEFAULT_SPI_TX_PIN, GPIO_FUNC_SPI);
    gpio_init(PIN_CS);
    gpio_put(PIN_CS,1); // initialize GP15 to high
    gpio_set_dir(PIN_CS, GPIO_OUT); // set GP15 to be output pin for chip select
    
    float t = 0;
    while(1) {
        if (t == 1.0) {
            t = 0;
            printf("overflow accounted for\r\n");
        }
        float vs = sin_val(t); // sine wave
        data_write(0, vs);
        float vt = triangle(t); // triangle wave
        data_write(1, vt);
        t = t + 0.001;
        sleep_ms(1);
    }
}

void data_write(int channel, float voltage) {
    uint8_t data[2];
    int len = 2;
    data[0] = 0;
    data[0] = data[0] | (channel<<7);
    data[0] = data[0] | (0b111<<4);
    uint16_t v = (voltage/3.3) * 1023;
    data[0] = data[0] | (v>>6);
    data[1] = data[1] | (v<<2);

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

float sin_val(float time) { // generate a value along a 2Hz sine wave centered around 1.65 and an amplitude of 1.65
    return 1.65*sin(((2*PI)/0.5)*time) + 1.65;
}

float triangle(float time) { // generate a value along a 1 Hz triangle wave with min and 0 and max at 3.3
    return 3.3*fabs(fmod(time, 2.0)-1);
}