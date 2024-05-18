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

#include "cJSON.h"
#include "cJSON_Utils.h"

#define TAG "MAIN"

typedef struct main_app {
	sv_iot_t* 		m_iot;
	sv_btn_t* 		m_btn;
	sv_loadcell_t* 	m_lc;
	sv_dp_t* 		m_dp;
    int32_t 		m_mass;
}main_app_t;

#define impl(x)		((main_app_t*)(x))

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

char* data_buffer;
char* topic_buffer;
uint32_t get_tick_count(){
	return sys_time;
}
void btn_on_event_handle(uint8_t _event, void*_arg){
    switch (_event) {
        case SV_BTN_TAP:
        	if (impl(_arg)->m_dp->m_state == DP_ON)
        		sv_lc_tare(impl(_arg)->m_lc);
            break;
        case SV_BTN_HOLD:
            break;
    }
    sv_dp_btn_event_cb(impl(_arg)->m_dp, _event);
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
    ((main_app_t*)(_arg))->m_mass = _mass;
}

lc_event_cb_fn_t lc_cb = {
    .on_event = lc_on_event_handle,
    .update_data = lc_update_data_handle
};

static bool IRAM_ATTR timer0_cb_handle (gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_data){
	sys_time+=SYS_TICK_MS;
	sv_btn_process(impl(user_data)->m_btn);
	return true;
}
static char* app_status_data_build(main_app_t* _this);
static char* app_volume_data_build(main_app_t* _this);
static char* app_topic_path_build(main_app_t* _this, char* _main_topic, char* _sub_topic, char* _id);
uint8_t online_flag = 1;
void app_main(){
	nvs_flash_init();
	main_app_t* ibme_app = malloc(sizeof(main_app_t));

	/* Create services */
	g_iot = sv_iot_create();
	sv_iot_init(g_iot);
    g_lc = sv_lc_create();
    g_btn = sv_btn_create(BTN_PIN);
    g_dp = sv_dp_create(LCD_DIO_PIN, LCD_CLK_PIN);

    /* Init device main app */
    ibme_app->m_iot  	= g_iot;
    ibme_app->m_btn  	= g_btn;
    ibme_app->m_dp		= g_dp;
    ibme_app->m_lc		= g_lc;

    /* Register services event */
    sv_lc_reg_event(g_lc, &lc_cb,  (void*)ibme_app);
    sv_btn_reg_event(g_btn, btn_on_event_handle, (void*)ibme_app);

    /* Timer */
	uint64_t* timer_data = malloc(sizeof(uint8_t));
	*timer_data = 0;
	gptimer_config_t timer_config = {
			.clk_src = GPTIMER_CLK_SRC_DEFAULT,
			.direction = GPTIMER_COUNT_UP,
			.resolution_hz = 1000000, // 1MHz, 1 tick=1us
		};
	ESP_ERROR_CHECK(gptimer_new_timer(&timer_config, &gptimer));
	cbs.on_alarm = timer0_cb_handle;
	ESP_ERROR_CHECK(gptimer_register_event_callbacks(gptimer, &cbs, ibme_app));
	ESP_ERROR_CHECK(gptimer_enable(gptimer));
	gptimer_alarm_config_t alarm_config = {
		.reload_count = 0,
		.alarm_count = SYS_TICK_MS * 1000,
		.flags.auto_reload_on_alarm = true,
	};
	ESP_ERROR_CHECK(gptimer_set_alarm_action(gptimer, &alarm_config));
	ESP_ERROR_CHECK(gptimer_start(gptimer));

	Delay(1000);

    sv_lc_process(g_lc);
    sv_lc_tare(g_lc);

    publish_data_timeout = sys_time;

    while(1){
    	if (ibme_app->m_iot->m_mqtt->m_state == ST_CONNECTED){
    		if (online_flag){
    			online_flag = 0;
    			data_buffer = app_status_data_build(ibme_app);
				topic_buffer = app_topic_path_build(ibme_app, MQTT_STATUS_TOPIC, "", DEVICE_ID);
				sv_iot_mqtt_build_data(g_iot, topic_buffer, data_buffer);
    		}
    		if (sys_time - lc_sv_time > 100){
				sv_lc_process(ibme_app->m_lc);
				lc_sv_time = sys_time;
				sv_dp_num(ibme_app->m_dp, ibme_app->m_mass);

			}
			if (sys_time - lcd_sv_time >100){

			}
			if (sys_time - publish_data_timeout > 1000){
				data_buffer = app_volume_data_build(ibme_app);
				topic_buffer = app_topic_path_build(ibme_app, MQTT_DATA_TOPIC, "", DEVICE_ID);
				sv_iot_mqtt_build_data(g_iot, topic_buffer, data_buffer);
				publish_data_timeout = sys_time;
			}
    	}else{
    		if (online_flag == 0)
    			online_flag = 1;
    		sv_dp_draw_loading(ibme_app->m_dp);
    	}
		sv_dp_process(ibme_app->m_dp);
		sv_iot_process(ibme_app->m_iot);
    }
}
static char* app_status_data_build(main_app_t* _this){
	char* data_buff;
	cJSON *device = cJSON_CreateObject();
	cJSON_AddStringToObject(device, "id", DEVICE_ID);
	cJSON_AddStringToObject(device, "firmware_version", FIRMWARE_VERSION);
	cJSON_AddStringToObject(device, "hardware_version", HARDWARE_VERSION);
	data_buff = cJSON_Print(device);
	cJSON_Delete(device);
	return data_buff;
}

static char* app_volume_data_build(main_app_t* _this){
	char* data_buff;
	cJSON *device = cJSON_CreateObject();
	cJSON_AddStringToObject(device, "id", DEVICE_ID);
	cJSON_AddNumberToObject(device, "volume", _this->m_mass);
	data_buff = cJSON_Print(device);
	cJSON_Delete(device);
	return data_buff;
}
static char topic[128];
static char* app_topic_path_build(main_app_t* _this, char* _main_topic, char* _sub_topic, char* _id){

	memset(topic, '\0',128);
	strcat(topic, _main_topic);
	strcat(topic, _sub_topic);
	strcat(topic, _id);
//	printf("%s\n",topic);
	return topic;
}
