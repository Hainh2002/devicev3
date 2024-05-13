#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "freertos/queue.h"
#include "driver/gptimer.h"
#include "esp_wifi.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_log.h"
#include "mqtt_client.h"
#include "esp_tls.h"
#include "esp_log.h"
#include "sdkconfig.h"

#include "tm1637.h"
#include "sv_loadcell.h"
#include "sv_display.h"
#include "sv_iot.h"
#include "hx711.h"





#define TAG "MAIN"

typedef struct main_app {
    int32_t m_mass;
}main_app_t;

main_app_t ibme_app;
int32_t volume = 0;
uint64_t sys_time = 0;
uint64_t btn_sv_time = 0;
uint64_t lc_sv_time = 10;
uint64_t lcd_sv_time = 20;
uint64_t publish_data_timeout;

sv_iot_t* g_iot;
sv_btn_t* g_btn;
sv_loadcell_t *g_lc;
sv_dp_t* g_dp;

static	gptimer_handle_t gptimer = NULL;
static gptimer_event_callbacks_t cbs;

uint32_t get_tick_count(){
	return sys_time;
}
void btn_on_event_handle(uint8_t _event, void*_arg){
    switch (_event) {
        case SV_BTN_TAP:
            sv_lc_tare((sv_loadcell_t*)_arg);
            break;
        case SV_BTN_HOLD:
            break;
    }
}

void lc_on_event_handle(uint8_t _event, void*_arg){
    switch (_event)  {
        case SV_LC_TARED:
            break;
        case SV_LC_CALIBED:
            break;
    }    
}

void lc_update_data_handle(int32_t _mass, void*_arg){
//    ((main_app_t*)(_arg))->m_mass = _mass;
    volume = _mass;
}

lc_event_cb_fn_t lc_cb = {
    .on_event = lc_on_event_handle,
    .update_data = lc_update_data_handle
};

static void timer_callback();
static bool IRAM_ATTR timer0_cb_handle (gptimer_handle_t timer,
											const gptimer_alarm_event_data_t *edata,
											void *user_data)
{
	sys_time+=SYS_TICK_MS;
	sv_btn_process(g_btn);
	return true;
}

char* int_to_char_with_sign(int32_t num);
char* uint_to_char(int32_t num);
void app_main(){
	nvs_flash_init();
	g_iot = sv_iot_create();

	sv_iot_init(g_iot);

    g_lc = sv_lc_create();

    sv_lc_reg_event(g_lc, &lc_cb, NULL);

    g_btn = sv_btn_create(BTN_PIN);
    sv_btn_reg_event(g_btn, btn_on_event_handle, g_lc);

    g_dp = sv_dp_create(LCD_DIO_PIN, LCD_CLK_PIN);

	uint64_t* timer_data = malloc(sizeof(uint8_t));
	*timer_data = 0;
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
		.alarm_count = SYS_TICK_MS * 1000,
		.flags.auto_reload_on_alarm = true,
	};
	ESP_ERROR_CHECK(gptimer_set_alarm_action(gptimer, &alarm_config));
	ESP_ERROR_CHECK(gptimer_start(gptimer));

    vTaskDelay(1000/portTICK_PERIOD_MS);
    sv_lc_process(g_lc);
    sv_lc_tare(g_lc);

    publish_data_timeout = sys_time;
    char* data_buff = "";
    while(1){
    	if (g_iot->m_mqtt->m_state == ST_CONNECTED){
    		if (sys_time - lc_sv_time > 100){
				sv_lc_process(g_lc);
				lc_sv_time = sys_time;
				printf("volume: %d\n", volume);
			}
			if (sys_time - lcd_sv_time >100){
				sv_dp_num(g_dp, volume);
				lcd_sv_time = sys_time;
			}
			if (sys_time - publish_data_timeout > 2000){
				data_buff = uint_to_char(volume);
				printf("DATA SEND: %s\n", data_buff);
				sv_iot_mqtt_build_data(g_iot, "topictest", data_buff);
				publish_data_timeout = sys_time;
				sv_iot_process(g_iot);
			}
    	}
    }
}

char* uint_to_char(int32_t num) {
    int32_t length = snprintf(NULL, 0, "%d", num);
    char *str = (char*)malloc((length + 1) * sizeof(char));
    snprintf(str, length + 1, "%d", num);
    return str;
}
char* int_to_char_with_sign(int32_t num) {
    bool is_negative = false;
    if (num < 0) {
        is_negative = true;
        num = -num;
    }
    char *str = uint_to_char(num);
    int length = snprintf(NULL, 0, "%s", str);
    if (is_negative) {
        char *temp = (char*)malloc((snprintf(NULL, 0, "-%s", str) + 1) * sizeof(char));
        snprintf(temp, length + 2, "-%s", str);
        free(str);
        return temp;
    }
    return str;
}
