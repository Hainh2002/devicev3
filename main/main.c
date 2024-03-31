/**
 *  Create by hainh2002
 *
 *  email: hai.nh200802@gmail.com
 *
 */
#include <stdio.h>
#include"driver/gpio.h"
#include"freertos/FreeRTOS.h"
#include"freertos/task.h"
#include"freertos/queue.h"
#include"esp_log.h"
#include"porting.h"

void app_main(void)
{
    gpio_config_t g_io;

    hal_io_t* led_pin = hal_io_init(&io_func, &g_io);
    hal_io_open(led_pin, 0, PIN_8, GPIO_MODE_INPUT_OUTPUT);

    while(1){
        hal_io_set_value(led_pin, 0);
        vTaskDelay(500/portTICK_RATE_MS);
        ESP_LOGI("main", "PIN 8 ON\n");

        hal_io_set_value(led_pin, 1);
        vTaskDelay(500/portTICK_RATE_MS);
        ESP_LOGI("main", "PIN 8 OFF\n");        
    }
    
}