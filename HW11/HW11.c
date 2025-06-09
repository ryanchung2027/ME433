#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "bsp/board_api.h"
#include "tusb.h"
#include "hardware/gpio.h"
#include "usb_descriptors.h"


#define UP_BUTTON    18
#define DOWN_BUTTON  17
#define LEFT_BUTTON  14
#define RIGHT_BUTTON 16
#define MODE_BUTTON  15


static int8_t delta_x = 0;
static int8_t delta_y = 0;
static bool remote_mode = false;
static absolute_time_t remote_start_time;

typedef struct {
    bool pressed;
    absolute_time_t press_time;
} button_state;

button_state up_btn, down_btn, left_btn, right_btn;


void update_regular_deltas(void);
void update_remote_deltas(void);
void hid_task(void);


int main(void) {
    // Initialize buttons
    gpio_init(UP_BUTTON);
    gpio_set_dir(UP_BUTTON, GPIO_IN);
    gpio_init(DOWN_BUTTON);
    gpio_set_dir(DOWN_BUTTON, GPIO_IN);
    gpio_init(LEFT_BUTTON);
    gpio_set_dir(LEFT_BUTTON, GPIO_IN);
    gpio_init(RIGHT_BUTTON);
    gpio_set_dir(RIGHT_BUTTON, GPIO_IN);
    gpio_init(MODE_BUTTON);
    gpio_set_dir(MODE_BUTTON, GPIO_IN);

    // Initialize mode LED
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);

    board_init();
    tud_init(BOARD_TUD_RHPORT);

    // Wait for USB enumeration
    if (board_init_after_tusb) {
        board_init_after_tusb();
    }

    while (1) {
        tud_task();

        // Mode toggle
        if (!gpio_get(MODE_BUTTON)) {
            sleep_ms(30); // Debounce
            if (!gpio_get(MODE_BUTTON)) {
                remote_mode = !remote_mode;
                gpio_put(PICO_DEFAULT_LED_PIN, remote_mode);
                remote_start_time = get_absolute_time();

                // Reset deltas when switching modes
                delta_x = 0;
                delta_y = 0;

                // Wait for release
                while (!gpio_get(MODE_BUTTON)) {}
                sleep_ms(30);
            }
        }

        // Update deltas based on mode
        if (remote_mode) {
            update_remote_deltas();
        } else {
            update_regular_deltas();
        }

        hid_task();
    }
}


void update_regular_deltas(void) {
    // Check each button
    struct {
        uint pin;
        int8_t *delta;
        int8_t direction; // +1 or -1
        button_state *state;
        const char *name;
    } buttons[] = {
        { UP_BUTTON, &delta_y, -1, &up_btn, "UP" },
        { DOWN_BUTTON, &delta_y,  1, &down_btn, "DOWN" },
        { LEFT_BUTTON, &delta_x, -1, &left_btn, "LEFT" },
        { RIGHT_BUTTON, &delta_x, 1, &right_btn, "RIGHT" },
    };

    for (int i = 0; i < 4; i++) {
        bool pressed = (gpio_get(buttons[i].pin) == 0);

        if (pressed && !buttons[i].state->pressed) {
        // Just pressed
        *(buttons[i].delta) = buttons[i].direction;
        buttons[i].state->press_time = get_absolute_time();
        buttons[i].state->pressed = true;
        } else if (pressed && buttons[i].state->pressed) {
        // Held down — acceleration
        int elapsed = absolute_time_diff_us(buttons[i].state->press_time, get_absolute_time()) / 1000;
        int8_t level = 1;

        if (elapsed > 1500) level = 4;
        else if (elapsed > 1000) level = 3;
        else if (elapsed > 500) level = 2;

        *(buttons[i].delta) = buttons[i].direction * level;
        } else if (!pressed && buttons[i].state->pressed) {
        // Released
        *(buttons[i].delta) = 0;
        buttons[i].state->pressed = false;
        }
    }
}

void update_remote_deltas(void) {
    int elapsed_ms = to_ms_since_boot(get_absolute_time()) - to_ms_since_boot(remote_start_time);
    float angle = (elapsed_ms / 1000.0f); // Radians per second

    delta_x = (int8_t)(3 * cosf(angle));
    delta_y = (int8_t)(3 * sinf(angle));
 }

// USB HID reporting
void send_hid_report(uint8_t report_id) {
    if (!tud_hid_ready()) return;

    switch (report_id) {
        case REPORT_ID_MOUSE:
        tud_hid_mouse_report(REPORT_ID_MOUSE, 0x00, delta_x, delta_y, 0, 0);
        break;
        default:break;
    }
}

void hid_task(void) {
    const uint32_t interval_ms = 10;
    static uint32_t last_ms = 0;
    if (board_millis() - last_ms < interval_ms) return;
    last_ms += interval_ms;

    send_hid_report(REPORT_ID_MOUSE);
}

    // USB callbacks (unchanged from default examples)
    void tud_mount_cb(void);
    void tud_umount_cb(void);
    void tud_suspend_cb(bool remote);
    void tud_resume_cb(void);
    void tud_hid_set_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t const *buffer, uint16_t bufsize) { (void)instance; (void)report_id; (void)report_type; (void)buffer; (void)bufsize; }
    uint16_t tud_hid_get_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t *buffer, uint16_t reqlen) { (void)instance; (void)report_id; (void)report_type; (void)buffer; (void)reqlen; return 0; }