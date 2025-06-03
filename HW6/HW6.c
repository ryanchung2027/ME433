#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

// I2C defines
// This example will use I2C0 on GPIO16 (SDA) and GPIO17 (SCL) running at 400KHz.
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define I2C_PORT i2c0
#define I2C_SDA 16
#define I2C_SCL 17
#define ADDR 0b0100000
#define HEARTBEAT_INTERVAL 500
#define OLAT 0x0A
#define GPIO 0x09

void setPin(unsigned char addr, unsigned char reg, unsigned char val);
unsigned char readPin(unsigned char addr, unsigned char reg);

int main()
{
    stdio_init_all();

    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
    // I2C Initialisation. Using it at 400Khz.
    i2c_init(I2C_PORT, 400*1000);
    
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);

    // set direction of GP pins
    uint8_t IODIR_reg = 0x00;
    uint8_t IODIR_val = 0b0000001;
    setPin(ADDR, IODIR_reg, IODIR_val);

    // Track time for heartbeat
    absolute_time_t last_heartbeat_time = get_absolute_time();
    bool heartbeat_state = false;
    bool LED_toggle = false;
    while (true) {
        uint8_t read = readPin(ADDR, GPIO);
        read = (read && 1) ;
        if (read == 0) {
            uint8_t GP7 = 1 << 7;
            setPin(ADDR, OLAT, GP7);
        } else {
            setPin(ADDR, OLAT, 0);
        }

        absolute_time_t now = get_absolute_time();
        if (absolute_time_diff_us(last_heartbeat_time, now) >= HEARTBEAT_INTERVAL * 1000) {
            heartbeat_state = !heartbeat_state;
            gpio_put(PICO_DEFAULT_LED_PIN, heartbeat_state);
            last_heartbeat_time = now;
        }
    }
}

void setPin(unsigned char addr, unsigned char reg, unsigned char val) {
    uint8_t buf[2];
    buf[0] = reg;
    buf[1] = val;
    i2c_write_blocking(i2c_default, addr, buf, 2, false);
}

unsigned char readPin(unsigned char addr, unsigned char reg) {
    uint8_t buf;
    i2c_write_blocking(i2c_default, addr, &reg, 1, true);  // true to keep master control of bus
    i2c_read_blocking(i2c_default, addr, &buf, 1, false);  // false - finished with bus
    return buf;
}
