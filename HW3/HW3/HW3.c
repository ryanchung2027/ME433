#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"

// define GPIO pins
#define GPIO_LED_PIN 19
#define GPIO_WATCH_PIN 15

// initialize count variable
int count = 0;

int pico_led_init(void);
void pico_set_led(bool led_on);

// interrupt delcaration
void gpio_callback(uint gpio, uint32_t events) {
    if (!gpio_get(GPIO_LED_PIN)) {
        pico_set_led(true);
    } else {
        pico_set_led(false);
    }
    count++;
    printf("The button has been pressed %d times\n", count);
}


int main()
{
    stdio_init_all();
    
    while (!stdio_usb_connected()) {
        sleep_ms(100);
    }
    printf("Start!\n");
 
    while (1) {
        char message[100];
        scanf("%s", message);
        printf("message: %s\r\n",message);
        sleep_ms(50);
    }
}
