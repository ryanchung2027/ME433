#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "math.h"

// SPI Defines
// We are going to use SPI 0, and allocate it to the following GPIO pins
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define SPI_PORT spi0
#define PIN_MISO 16
#define PIN_CS   15
#define PIN_SCK  18
#define PIN_MOSI 19
#define PI 3.14159

union FloatInt {
    float f;
    uint32_t i;
};

void spi_ram_init(); 
void ram_data_write(uint16_t addr, float voltage); 
float ram_data_read(uint16_t addr); 
void data_write(int channel, float voltage);
static inline void cs_select(uint cs_pin);
static inline void cs_deselect(uint cs_pin);
float sin_val(float time);


int main()
{
    stdio_init_all();

    // SPI initialisation. This example will use SPI at 1MHz.
    spi_init(SPI_PORT, 1000*1000);
    gpio_set_function(PIN_MISO, GPIO_FUNC_SPI);
    gpio_set_function(PIN_CS,   GPIO_FUNC_SIO);
    gpio_set_function(PIN_SCK,  GPIO_FUNC_SPI);
    gpio_set_function(PIN_MOSI, GPIO_FUNC_SPI);
    
    // Chip select is active-low, so we'll initialise it to a driven-high state
    gpio_put(PIN_CS,1); // initialize GP15 to high
    gpio_set_dir(PIN_CS, GPIO_OUT); // set GP15 to be output pin for chip select

    spi_ram_init();

    // load 1000 floats into ERAM
    float t = 0; int i = 0;
    for (i = 0; i < 1000; i++) {
        float v = sin_val(t); // sine wave
        ram_data_write(i * 4, v);
        t = t + 0.001;
    }

    // read from ERAM into DAC
    int a = 0;
    while(1) {
        float v_read = ram_data_read(a);
        data_write(0, v_read);
        a = a +  4;
        if (a > 4000) {
            a = 0;
        }
    }


    // // CODE FOR MATH AND TIMING 
    // while (!stdio_usb_connected()) {}
    // sleep_ms(100);

    // volatile float f1, f2; int i = 0;
    // printf("Enter two floats to use:");
    // scanf("%f %f", &f1, &f2);
    // volatile float f_add, f_sub, f_mult, f_div;

    // // run addition 1000 times
    // absolute_time_t t1_init = get_absolute_time();
    // for (i = 0; i < 1000; i++) {
    //     f_add = f1+f2;
    // }
    // absolute_time_t t1_add = get_absolute_time();                   // get time for all addition operations
    
    // // run subtraction 1000 times
    // for (i = 0; i < 1000; i++) {
    //     f_sub = f1-f2;
    // }
    // absolute_time_t t1_sub = get_absolute_time();                   // get time for all subtraction operations

    // // run multiplcation 1000 times
    // for (i = 0; i < 1000; i++) {
    //     f_mult = f1*f2;
    // }
    // absolute_time_t t1_mult = get_absolute_time();                   // get time for all multiplication operations
    
    // // run divison 1000 times
    // for (i = 0; i < 1000; i++) {
    //     f_div = f1/f2;
    // }
    // absolute_time_t t1_div = get_absolute_time();                   // get time for all division operations

    // // find time it takes for each operation to work
    // uint64_t t_init = to_us_since_boot(t1_init);                 // convert initial time to ms
    // uint64_t t_add = to_us_since_boot(t1_add);
    // int cycles_add = (t_add - t_init) / (6.667 / 1000 * 1000);   // convert to cycles for one operation by dividing by 6.667 ns and 1000 cycles
    // uint64_t t_sub = to_us_since_boot(t1_sub);
    // int cycles_sub = (t_sub - t_add) / (6.667 / 1000 * 1000);    // convert to cycles for one operation by dividing by 6.667 ns and 1000 cycles
    // uint64_t t_mult = to_us_since_boot(t1_mult);
    // int cycles_mult = (t_mult - t_sub) / (6.667 / 1000 * 1000);  // convert to cycles for one operation by dividing by 6.667 ns and 1000 cycles
    // uint64_t t_div = to_us_since_boot(t1_div);
    // int cycles_div = (t_div - t_mult) / (6.667 / 1000 * 1000);   // convert to cycles for one operation by dividing by 6.667 ns and 1000 cycles    
    

    // printf("\nResults: \n%f+%f=%f \n%f-%f=%f \n%f*%f=%f \n%f/%f=%f\n", f1,f2,f_add, f1,f2,f_sub, f1,f2,f_mult, f1,f2,f_div);
    // printf("\nCycles for addition = %d", cycles_add);
    // printf("\nCycles for subtraction = %d", cycles_sub);
    // printf("\nCycles for multiplication = %d", cycles_mult);
    // printf("\nCycles for division = %d", cycles_div);
}

void spi_ram_init() {
    uint8_t buf[2];
    buf[0] = 0b1;
    buf[1] = 0b01000000;    // sequential mode
    spi_write_blocking(spi_default, buf, 2);
}

void ram_data_write(uint16_t addr, float voltage) {
    uint8_t buf[7];
    buf[0] = 0b00000010;
    buf[1] = addr >> 8;
    buf[2] = addr & 0xFF;

    union FloatInt v;
    v.f = voltage;

    buf[3] = v.i >> 24;
    buf[4] = (v.i >> 16) & 0xFF;
    buf[5] = (v.i >> 8) & 0xFF;
    buf[6] = v.i & 0xFF;

    spi_write_blocking(spi_default, buf, 7);
}


float ram_data_read(uint16_t addr) {
    uint8_t write[7], read[7];
    write[0] = 0b00000011;
    write[1] = addr >> 8;
    write[2] = addr & 0xFF;
    
    spi_write_read_blocking(spi_default, write, read, 7);

    union FloatInt v;
    v.i = v.i | read[3] << 24;
    v.i = v.i | read[4] << 16;
    v.i = v.i | read[5] << 8;
    v.i = v.i | read[6];

    return v.f;
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