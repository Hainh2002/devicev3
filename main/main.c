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
#include"bsp_io.h"
#include"tm1637.h"

const gpio_num_t LED_CLK = CONFIG_TM1637_CLK_PIN;
const gpio_num_t LED_DTA = CONFIG_TM1637_DIO_PIN;

void app_main(void)
{
    bsp_init();
    tm1637_lcd_t * lcd = tm1637_init( LED_CLK, LED_DTA);

    while(1){
        bsp_led_write(0);
        vTaskDelay(100/portTICK_RATE_MS);
        bsp_led_write(1);
        vTaskDelay(100/portTICK_RATE_MS);
        ESP_LOGI("MAIN", "BTN: %d\n", bsp_btn_read());
        // Test segment control
		uint8_t seg_data[] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20};
		for (uint8_t x=0; x<32; ++x)
		{
			uint8_t v_seg_data = seg_data[x%6];
			tm1637_set_segment_raw(lcd, 0, v_seg_data);
			tm1637_set_segment_raw(lcd, 1, v_seg_data);
			tm1637_set_segment_raw(lcd, 2, v_seg_data);
			tm1637_set_segment_raw(lcd, 3, v_seg_data);
			vTaskDelay(100 / portTICK_PERIOD_MS);
		}

		// Test brightness
		for (int x=0; x<=9999; x++) {
			tm1637_set_number(lcd, x);
			vTaskDelay(50 / portTICK_PERIOD_MS);
		}

		// uint8_t v_seg_data = seg_data[x%6];
		tm1637_set_segment_raw(lcd, 0, 0x76);
		tm1637_set_segment_raw(lcd, 1, 0x77);
		tm1637_set_segment_raw(lcd, 2, 0x30);
		// tm1637_set_segment_raw(lcd, 3, 0x3E);
		vTaskDelay(3000 / portTICK_PERIOD_MS);
    }
    
}