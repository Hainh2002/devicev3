#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/gptimer.h"
#include "esp_log.h"
#include "sdkconfig.h"
#include "esp_system.h"

//#include "bsp_timer.h"
static uint64_t sys_time = 0;
static uint64_t sys_tick = 10;

static	gptimer_handle_t gptimer = NULL;
static gptimer_event_callbacks_t cbs;

static void timer_callback(){

	return;
}
static bool IRAM_ATTR timer0_cb_handle(gptimer_handle_t timer,
											const gptimer_alarm_event_data_t *edata,
											void *user_data)
{
	uint8_t* flag = (uint8_t*)user_data;
	*flag = 1;
	timer_callback();
	return true;
}


void app_main(){
	uint8_t*	timer_data = malloc(sizeof(uint8_t));
	gptimer_config_t timer_config = {
			.clk_src = GPTIMER_CLK_SRC_DEFAULT,
			.direction = GPTIMER_COUNT_UP,
			.resolution_hz = 1000000, // 1MHz, 1 tick=1us
		};
	ESP_ERROR_CHECK(gptimer_new_timer(&timer_config, &gptimer));

	cbs.on_alarm = timer0_cb_handle;
	ESP_ERROR_CHECK(gptimer_register_event_callbacks(gptimer, &cbs, timer_data));
	ESP_ERROR_CHECK(gptimer_enable(gptimer));
	gptimer_alarm_config_t alarm_config = {
		.reload_count = 0,
		.alarm_count = 200 * 1000,
		.flags.auto_reload_on_alarm = true,
	};
	ESP_ERROR_CHECK(gptimer_set_alarm_action(gptimer, &alarm_config));
	ESP_ERROR_CHECK(gptimer_start(gptimer));

	while(1){

	}
}



