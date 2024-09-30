#include "pico/stdlib.h"
#include "pico/time.h"
#include "hardware/uart.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <math.h>

#define ULTRA_SENSOR_TRIGGER 1
#define ULTRA_SENSOR_ECHO 0
#define ULTRA_SENSOR_ALPHA 0.2f
#define ULTRA_ENTER_DISTANCE_CM 15
#define ULTRA_EXIT_DISTANCE_CM 20
#define SPEAKER_UART uart1
#define SPEAKER_UART_BAUDRATE 9600
#define SPEAKER_TX 4
#define SPEAKER_RX 5
#define DIODE_PIN 25
#define MAX_STEALING_TIME_MS 6000

static void _speaker_exec_cmd(uint8_t cmd, uint16_t param)
{
    uint8_t buffer[10];

    buffer[0] = 0x7E;
    buffer[1] = 0xFF;
    buffer[2] = 0x06;
    buffer[3] = cmd;
    buffer[4] = 0x00;
    buffer[5] = param >> 8;
    buffer[6] = param & 0xFF;

    uint16_t checksum = 0;

    for (size_t i = 0; i < 6; i++)
    {
        checksum -= buffer[1 + i];
    }

    buffer[7] = checksum >> 8;
    buffer[8] = checksum & 0xFF;
    buffer[9] = 0xEF;

    uart_write_blocking(SPEAKER_UART, buffer, sizeof(buffer));

    printf("Successfully sent command to speaker!\n");

    sleep_ms(100);
}

static void _speaker_set_normal(void)
{
    _speaker_exec_cmd(0x03, 1);
    _speaker_exec_cmd(0x0D, 0);
    _speaker_exec_cmd(0x19, 0);

    printf("Set normal mode of speaker\n");
}

static void _speaker_set_special(void)
{
    _speaker_exec_cmd(0x03, 2);
    _speaker_exec_cmd(0x0D, 0);
    _speaker_exec_cmd(0x19, 1);

    printf("Set special mode of speaker\n");
}

static void _speaker_init(void)
{
    uart_init(SPEAKER_UART, SPEAKER_UART_BAUDRATE);
    gpio_set_function(SPEAKER_TX, GPIO_FUNC_UART);
    gpio_set_function(SPEAKER_RX, GPIO_FUNC_UART);

    _speaker_exec_cmd(0x06, 30);

    _speaker_set_normal();

    printf("Speaker initialized!\n");
}

static void _ultra_sensor_init(void)
{
    gpio_init(ULTRA_SENSOR_TRIGGER);
    gpio_set_dir(ULTRA_SENSOR_TRIGGER, GPIO_OUT);
    gpio_put(ULTRA_SENSOR_TRIGGER, 0);

    gpio_init(ULTRA_SENSOR_ECHO);
    gpio_set_dir(ULTRA_SENSOR_ECHO, GPIO_IN);

    printf("Ulatrsonic sensor ready!\n");
}

static float _ultra_sensor_read(void)
{
    gpio_put(ULTRA_SENSOR_TRIGGER, 1);
    sleep_us(20);
    gpio_put(ULTRA_SENSOR_TRIGGER, 0);

    uint32_t on = 0, off = 0;

    while (gpio_get(ULTRA_SENSOR_ECHO) == 0)
    {
        off = to_us_since_boot(get_absolute_time());
    }

    while (gpio_get(ULTRA_SENSOR_ECHO) == 1)
    {
        on = to_us_since_boot(get_absolute_time());
    }

    uint32_t diff = on - off;
    float distance = (diff * 0.0343f) / 2.0f;

    return distance;
}

int main()
{
    stdio_init_all();
    sleep_ms(1000);

    printf("Started!\n");

    _speaker_init();
    _ultra_sensor_init();

    gpio_init(DIODE_PIN);
    gpio_set_dir(DIODE_PIN, GPIO_OUT);
    gpio_put(DIODE_PIN, 0);

    float lastDist = 0;
    bool isSpecial = false;
    bool isNormal = true;
    uint32_t stealingTime = 0;

    while (true)
    {
        float dist = _ultra_sensor_read();
        dist = lastDist == 0 ? dist : ULTRA_SENSOR_ALPHA * dist + (1 - ULTRA_SENSOR_ALPHA) * lastDist;
        lastDist = dist;

        if (dist <= ULTRA_ENTER_DISTANCE_CM)
        {
            if (!isSpecial)
            {
                _speaker_set_special();

                isSpecial = true;
                isNormal = false;

                stealingTime = to_ms_since_boot(get_absolute_time());
            }
        }
        else if (dist >= ULTRA_EXIT_DISTANCE_CM)
        {
            if (!isNormal)
            {
                _speaker_set_normal();

                isNormal = true;
                isSpecial = false;
            }
        }

        if (isSpecial)
        {
            if (to_ms_since_boot(get_absolute_time()) - stealingTime >= MAX_STEALING_TIME_MS)
            {
                printf("Finished stealing!\n");

                gpio_put(DIODE_PIN, 1);

                return 0;
            }
        }

        sleep_ms(50);
    }
}