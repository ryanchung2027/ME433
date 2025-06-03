#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "ssd1306.h"
#include "font.h"

// I2C defines
// This example will use I2C0 on GPIO8 (SDA) and GPIO9 (SCL) running at 400KHz.
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define I2C_PORT i2c0
#define I2C_SDA 8
#define I2C_SCL 9
#define HEARTBEAT_INTERVAL 500



int main()
{
    stdio_init_all();


    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
    
    // I2C Initialisation. Using it at 400Khz.
    i2c_init(I2C_PORT, 400*1000);

    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);

    // initialize screen
    ssd1306_setup();

    while (true) {
        gpio_put(PICO_DEFAULT_LED_PIN, 1);
        ssd1306_drawPixel(0, 0, 1);
        ssd1306_update();
        sleep_ms(1000);
        gpio_put(PICO_DEFAULT_LED_PIN, 0);
        ssd1306_clear();
        ssd1306_update();
        sleep_ms(1000);
    }
}
