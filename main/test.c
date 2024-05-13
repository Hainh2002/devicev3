#include <stdio.h>
#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gptimer.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "sdkconfig.h"
#include "tm1637.h"
#include "sv_loadcell.h"
#include "bsp_io.h"
#include "sv_iot.h"
#include "hx711.h"
#include "esp_system.h"
#include "sv_display.h"
#include "config.h"

uint32_t get_tick_count(){
	return 0;
}
void app_main(void){
	sv_dp_t* g_dp = sv_dp_create(8,9);
	uint64_t  num = 0;
	while (1){
//		sv_dp_num(g_dp, num);
		tm1637_set_number(g_dp->m_lcd, num);
		num++;
		Delay(1000);
	}
}
