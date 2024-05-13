#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/gptimer.h"
#include "esp_log.h"

#include "bsp_timer.h"

typedef void (*timer_irq_callback_t)();
static bool IRAM_ATTR timer0_cb_handle(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_data)
{
//	(void*)user_data();
	return true;
}
static	gptimer_handle_t gptimer = NULL;
static gptimer_event_callbacks_t cbs;
void timer0_init(){

	gptimer_config_t timer_config = {
		.clk_src = GPTIMER_CLK_SRC_DEFAULT,
		.direction = GPTIMER_COUNT_UP,
		.resolution_hz = 1000000, // 1MHz, 1 tick=1us
	};
	ESP_ERROR_CHECK(gptimer_new_timer(&timer_config, &gptimer));

	cbs.on_alarm = timer0_cb_handle;
}
void timer0_set_irq_callback(void* _cb_handle){
	ESP_ERROR_CHECK(gptimer_register_event_callbacks(gptimer, &cbs, _cb_handle));
	ESP_ERROR_CHECK(gptimer_enable(gptimer));
	gptimer_alarm_config_t alarm_config = {
		.reload_count = 0,
		.alarm_count = 100 * 1000, // period = 1s
		.flags.auto_reload_on_alarm = true,
	};
	ESP_ERROR_CHECK(gptimer_set_alarm_action(gptimer, &alarm_config));
	ESP_ERROR_CHECK(gptimer_start(gptimer));
}
