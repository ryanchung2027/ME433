#include <stdio.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/pio.h"
#include "ws2812.pio.h"

#define PWM_PIN 17
#define LED_PIN 15
#define NUM_PIXELS 4

// link three 8bit colors together
typedef struct {
    unsigned char r;
    unsigned char g;
    unsigned char b;
} wsColor; 

void servo_pwm_init();
void servo_set_angle();
void ws2812_init(PIO pio, int sm, uint pin);
void put_pixel(PIO pio, int sm, uint32_t pixel_grb);
uint32_t urgb_u32(uint8_t r, uint8_t g, uint8_t b);
wsColor HSBtoRGB(float hue, float sat, float brightness);

int main()
{
    stdio_init_all();

    // initialize PWM
    servo_pwm_init();

    PIO pio = pio0;
    int sm = 0;
    ws2812_init(pio, sm, LED_PIN);

    while (true) {
        for (int t = 0; t < 10000; t += 20) {
            float hue_base = (t / 5000.0f) * 360.0f;
            for (int i = 0; i < NUM_PIXELS; i++) {
                float hue = fmodf(hue_base + i * (360.0f / NUM_PIXELS), 360.0f);
                wsColor c = HSBtoRGB(hue, 1.0f, 0.3f);
                put_pixel(pio, sm, urgb_u32(c.r, c.g, c.b));
            }

            int angle;
            if (t <= 5000)
                angle = (t * 180) / 5000;
            else
                angle = 180 - ((t - 5000) * 180 / 5000);
            servo_set_angle(angle);

            sleep_ms(20);
        }
    }
}

void servo_pwm_init() {
    gpio_set_function(PWM_PIN, GPIO_FUNC_PWM); // Set Pin GP17 to be PWM
    uint slice_num = pwm_gpio_to_slice_num(PWM_PIN); // Get PWM slice number
    float div = 60; // must be between 1-255
    pwm_set_clkdiv(slice_num, div); // divider
    uint16_t wrap = 50000; // when to rollover, must be less than 65535
    pwm_set_wrap(slice_num, wrap);
    pwm_set_enabled(slice_num, true); // turn on the PWM
}


void servo_set_angle(int angle) {
    float min_duty = 0.5f / 20.0f; // 2.5%
    float max_duty = 2.5f / 20.0f; // 12.5%
    float duty_frac = min_duty + (max_duty - min_duty) * (angle / 180.0f);
    uint level = (uint)(duty_frac * 50000);
    pwm_set_gpio_level(PWM_PIN, level);
}

void ws2812_init(PIO pio, int sm, uint pin) {
    uint offset = pio_add_program(pio, &ws2812_program);
    ws2812_program_init(pio, sm, offset, pin, 800000, false);
}

void put_pixel(PIO pio, int sm, uint32_t pixel_grb) {
    pio_sm_put_blocking(pio, sm, pixel_grb << 8u);
}

uint32_t urgb_u32(uint8_t r, uint8_t g, uint8_t b) {
    return ((uint32_t)g << 16) | ((uint32_t)r << 8) | b;
}

// adapted from https://forum.arduino.cc/index.php?topic=8498.0
// hue is a number from 0 to 360 that describes a color on the color wheel
// sat is the saturation level, from 0 to 1, where 1 is full color and 0 is gray
// brightness sets the maximum brightness, from 0 to 1
wsColor HSBtoRGB(float hue, float sat, float brightness) {
    float red = 0.0;
    float green = 0.0;
    float blue = 0.0;

    if (sat == 0.0) {
        red = brightness;
        green = brightness;
        blue = brightness;
    } else {
        if (hue == 360.0) {
            hue = 0;
        }

        int slice = hue / 60.0;
        float hue_frac = (hue / 60.0) - slice;

        float aa = brightness * (1.0 - sat);
        float bb = brightness * (1.0 - sat * hue_frac);
        float cc = brightness * (1.0 - sat * (1.0 - hue_frac));

        switch (slice) {
            case 0:
                red = brightness;
                green = cc;
                blue = aa;
                break;
            case 1:
                red = bb;
                green = brightness;
                blue = aa;
                break;
            case 2:
                red = aa;
                green = brightness;
                blue = cc;
                break;
            case 3:
                red = aa;
                green = bb;
                blue = brightness;
                break;
            case 4:
                red = cc;
                green = aa;
                blue = brightness;
                break;
            case 5:
                red = brightness;
                green = aa;
                blue = bb;
                break;
            default:
                red = 0.0;
                green = 0.0;
                blue = 0.0;
                break;
        }
    }

    unsigned char ired = red * 255.0;
    unsigned char igreen = green * 255.0;
    unsigned char iblue = blue * 255.0;

    wsColor c;
    c.r = ired;
    c.g = igreen;
    c.b = iblue;
    return c;
}