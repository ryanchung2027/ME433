#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"

// SPI Defines
// We are going to use SPI 0, and allocate it to the following GPIO pins
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define SPI_PORT spi0
#define PIN_CS   15



int main()
{
    stdio_init_all();

    // SPI initialisation. This example will use SPI at 1MHz.
    spi_init(SPI_PORT, 1000*1000);
    gpio_set_function(PICO_DEFAULT_SPI_RX_PIN, GPIO_FUNC_SPI);
    gpio_set_function(PICO_DEFAULT_SPI_SCK_PIN, GPIO_FUNC_SPI);
    gpio_set_function(PICO_DEFAULT_SPI_TX_PIN, GPIO_FUNC_SPI);
    
    // Chip select is active-low, so we'll initialise it to a driven-high state
    gpio_init(PIN_CS);
    gpio_put(PIN_CS,1); // initialize GP15 to high
    gpio_set_dir(PIN_CS, GPIO_OUT); // set GP15 to be output pin for chip select

    
    while (!stdio_usb_connected()) {}
    sleep_ms(100);

    volatile float f1, f2; int i = 0;
    printf("Enter two floats to use:");
    scanf("%f %f", &f1, &f2);
    volatile float f_add, f_sub, f_mult, f_div;

    // run addition 1000 times
    absolute_time_t t1_init = get_absolute_time();
    for (i = 0; i < 1000; i++) {
        f_add = f1+f2;
    }
    absolute_time_t t1_add = get_absolute_time();                   // get time for all addition operations
    
    // run subtraction 1000 times
    for (i = 0; i < 1000; i++) {
        f_sub = f1-f2;
    }
    absolute_time_t t1_sub = get_absolute_time();                   // get time for all subtraction operations

    // run multiplcation 1000 times
    for (i = 0; i < 1000; i++) {
        f_mult = f1*f2;
    }
    absolute_time_t t1_mult = get_absolute_time();                   // get time for all multiplication operations
    
    // run divison 1000 times
    for (i = 0; i < 1000; i++) {
        f_div = f1/f2;
    }
    absolute_time_t t1_div = get_absolute_time();                   // get time for all division operations

    // find time it takes for each operation to work
    uint64_t t_init = to_us_since_boot(t1_init);                 // convert initial time to ms
    uint64_t t_add = to_us_since_boot(t1_add);
    int cycles_add = (t_add - t_init) / (6.667 / 1000 * 1000);   // convert to cycles for one operation by dividing by 6.667 ns and 1000 cycles
    uint64_t t_sub = to_us_since_boot(t1_sub);
    int cycles_sub = (t_sub - t_add) / (6.667 / 1000 * 1000);    // convert to cycles for one operation by dividing by 6.667 ns and 1000 cycles
    uint64_t t_mult = to_us_since_boot(t1_mult);
    int cycles_mult = (t_mult - t_sub) / (6.667 / 1000 * 1000);  // convert to cycles for one operation by dividing by 6.667 ns and 1000 cycles
    uint64_t t_div = to_us_since_boot(t1_div);
    int cycles_div = (t_div - t_mult) / (6.667 / 1000 * 1000);   // convert to cycles for one operation by dividing by 6.667 ns and 1000 cycles    
    

    printf("\nResults: \n%f+%f=%f \n%f-%f=%f \n%f*%f=%f \n%f/%f=%f\n", f1,f2,f_add, f1,f2,f_sub, f1,f2,f_mult, f1,f2,f_div);
    printf("\nCycles for addition = %d", cycles_add);
    printf("\nCycles for subtraction = %d", cycles_sub);
    printf("\nCycles for multiplication = %d", cycles_mult);
    printf("\nCycles for division = %d", cycles_div);

    
    
}
