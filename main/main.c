#include <stdio.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"

void app_main(void)
{
    gpio_config_t io_conf = {
    .mode = GPIO_MODE_INPUT_OUTPUT,
    .pull_down_en = 0,
    .pull_up_en = 0,
    .intr_type = GPIO_INTR_DISABLE,
    .pin_bit_mask = 1ULL << GPIO_NUM_8
    };

    gpio_config(&io_conf);
    while(1){
        gpio_set_level(GPIO_NUM_8,0);
        vTaskDelay(1000/portTICK_RATE_MS);
        ESP_LOGI("main", "PIN 8 ON\n");
        gpio_set_level(GPIO_NUM_8,1);
        vTaskDelay(1000/portTICK_RATE_MS);
        ESP_LOGI("main", "PIN 8 OFF\n");        
    }
    
}