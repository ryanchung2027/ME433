/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "hardware/adc.h"

#define FLAG_VALUE 123
#define FLAG_READ_ADC 0
#define FLAG_LED_ON 1
#define FLAG_LED_OFF 2
#define FLAG_READY 3

float voltage;

void core1_entry() {
    // Init ADC
    adc_init();
    adc_gpio_init(26); // A0 is GPIO26
    adc_select_input(0); // Select ADC channel 0 (A0)

    // Init LED
    gpio_init(15);
    gpio_set_dir(15, GPIO_OUT);
    gpio_put(15, 0);

     while (true) {
        uint32_t cmd = multicore_fifo_pop_blocking();

        if (cmd == FLAG_READ_ADC) {
            uint16_t read = adc_read();
            voltage = (3.3f * read) / 4095.0f;
            multicore_fifo_push_blocking(FLAG_READY);
        } else if (cmd == FLAG_LED_ON) {
            gpio_put(15, 1);
        } else if (cmd == FLAG_LED_OFF) {
            gpio_put(15, 0);
        }
    }
}

int main() {
    stdio_init_all();
    printf("Hello, multicore!\n");

    /// \tag::setup_multicore[]

    while (!stdio_usb_connected()) {
        sleep_ms(100);
    }

    multicore_launch_core1(core1_entry);

    char entry;
    while (1) {
        printf("Enter a command\n0: read voltage\n1: turn on LED\n2: turn off LED\n");
        scanf("%c", &entry);
        printf("You entered %c\n", entry);
        
        if (entry == '0') {
            multicore_fifo_push_blocking(FLAG_READ_ADC);
            multicore_fifo_pop_blocking(); // wait for core 1
            printf("Voltage on A0 is %.2f V\n", voltage);
        } else if (entry == '1') {
            multicore_fifo_push_blocking(FLAG_LED_ON);
        } else if (entry == '2') {
            multicore_fifo_push_blocking(FLAG_LED_OFF);
        } else {
            printf("Invalid command. Try again.\n");
        }
    }
}
