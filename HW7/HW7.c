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

    // initialize screen
    ssd1306_setup();

    
    char message[50]; 
    sprintf(message, "Hello world!"); 
    drawMessage(20,10,message); // draw starting at x=20,y=10  
    ssd1306_update();
    // while (true) {
    //     gpio_put(PICO_DEFAULT_LED_PIN, 1);
    //     drawChar(64, 16, 'L');
    //     ssd1306_update();
    //     sleep_ms(1000);
    //     gpio_put(PICO_DEFAULT_LED_PIN, 0);
    //     ssd1306_clear();
    //     ssd1306_update();
    //     sleep_ms(1000);
    // }
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