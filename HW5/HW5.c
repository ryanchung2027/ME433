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
    for (i = 0; i < 1000; i++) {
        f_add = f1+f2;
    }
    absolute_time_t t1 = get_absolute_time();
    uint64_t t = to_us_since_boot(t1);
    printf("t = %llu\n", t);
    f_sub = f1-f2;
    f_mult = f1*f2;
    f_div = f1/f2;
    printf("\nResults: \n%f+%f=%f \n%f-%f=%f \n%f*%f=%f \n%f/%f=%f\n", f1,f2,f_add, f1,f2,f_sub, f1,f2,f_mult, f1,f2,f_div);

    
    
}
