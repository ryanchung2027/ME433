#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "ssd1306.h"
#include "font.h"
#include "hardware/adc.h"

// I2C defines
// This example will use I2C0 on GPIO8 (SDA) and GPIO9 (SCL) running at 400KHz.
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define I2C_PORT i2c0
#define I2C_SDA 8
#define I2C_SCL 9
#define HEARTBEAT_INTERVAL 500

void drawLetter(unsigned char x, unsigned char y, char c);
void drawMessage(unsigned char x, unsigned char y, char *m);

int main()
{
    stdio_init_all();


    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
    
    // I2C Initialisation. Using it at 400Khz.
    i2c_init(I2C_PORT, 400*1000);

    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);

    adc_init(); // init the adc module
    adc_gpio_init(26); // set ADC0 pin to be adc input instead of GPIO
    adc_select_input(0); // select to read from ADC0

    // initialize screen
    ssd1306_setup();

    
    
    while (true) {
        unsigned int t1 = to_us_since_boot(get_absolute_time());
        uint16_t result = adc_read();
        float voltage = 0.0008 * result;
        char message[50], fps_display[10];
        sprintf(message, "ADC: %.2f", voltage); 
        drawMessage(0,0,message); // draw starting at x=20,y=10  
        ssd1306_update();
        unsigned int t2 = to_us_since_boot(get_absolute_time());
        unsigned int t_diff = t2-t1; 
        int fps = 1000000 / t_diff;

        sprintf(fps_display, "FPS: %d", fps);
        drawMessage(0,24,fps_display);
        ssd1306_update();
    }
}


void drawLetter(unsigned char x, unsigned char y, char c) {
    int row, col, i;
    row = c-0x20;
    col = 0;
    for (col = 0; col < 5; col++) {
        char byte = ASCII[row][col];
        for (i = 0; i < 8; i++) {
            char on_or_off = (byte >> i) & 0b1;

            ssd1306_drawPixel(x+col, y+i, on_or_off);
        }
    }
}

void drawMessage(unsigned char x, unsigned char y, char *m) {
    int i = 0;
    while (m[i] != '\0') {
        drawLetter(x+i*5, y, m[i]);
        i++;
    }
}