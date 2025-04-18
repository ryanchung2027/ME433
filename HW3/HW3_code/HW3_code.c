#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"

// define GPIO pins
#define GPIO_LED_PIN 19
#define GPIO_WATCH_PIN 15

int numSamp;
volatile bool interrupt_flag = false;
int results[100];
float voltage[100];

int pico_led_init(void);
void pico_set_led(bool led_on);

// interrupt delcaration
void gpio_callback(uint gpio, uint32_t events) {
    pico_set_led(false);                            // turn off led
    interrupt_flag = true;
}


int main()
{
    stdio_init_all();
    int rc = pico_led_init();
    hard_assert(rc == PICO_OK);
    gpio_init(GPIO_WATCH_PIN);
    gpio_set_irq_enabled_with_callback(GPIO_WATCH_PIN, GPIO_IRQ_EDGE_FALL, true, &gpio_callback);
    adc_init(); // init the adc module
    adc_gpio_init(26); // set ADC0 pin to be adc input instead of GPIO
    adc_select_input(0); // select to read from ADC0
    
    while (!stdio_usb_connected()) {
        sleep_ms(100);
    }

    pico_set_led(true);

    while (true) {
        if (interrupt_flag) {
            printf("Enter number of samples to take (1-100): \r\n"); // prompt user
            scanf("%d", &numSamp);
            int i;
            for (i = 0; i < numSamp; i++) {
                sleep_ms(10);                 // wait 10ms (100 Hz)
                uint16_t result = adc_read(); // read adc value
                results[i] = result;
            }
            for (i = 0; i < numSamp; i++) {
                voltage[i] = 0.0008 * results[i];
                printf("%.2f\r\n", voltage[i]);
            }
        }
    }
}

// Perform led initialisation
int pico_led_init(void) {
    #if defined(GPIO_LED_PIN)
        // A device like Pico that uses a GPIO for the LED will define GPIO_LED_PIN
        // so we can use normal GPIO functionality to turn the led on and off
        gpio_init(GPIO_LED_PIN);
        gpio_set_dir(GPIO_LED_PIN, GPIO_OUT);
        return PICO_OK;
    #elif defined(CYW43_WL_GPIO_LED_PIN)
        // For Pico W devices we need to initialise the driver etc
        return cyw43_arch_init();
    #endif
    }

// Turn the led on or off
void pico_set_led(bool led_on) {
#if defined(GPIO_LED_PIN)
    // Just set the GPIO on or off
    gpio_put(GPIO_LED_PIN, led_on);
#elif defined(CYW43_WL_GPIO_LED_PIN)
    // Ask the wifi "driver" to set the GPIO on or off
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, led_on);
#endif
}
