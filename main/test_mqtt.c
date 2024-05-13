/*
 * test_mqtt.c
 *
 *  Created on: Apr 20, 2024
 *      Author: DELL 5425
 */
#include <stdio.h>
#include <stdint.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "sdkconfig.h"
#include "esp_system.h"
#include "esp_timer.h"
#include "mqtt_client.h"

#include "tm1637.h"
#include "sv_loadcell.h"
#include "bsp_io.h"
#include "sv_iot.h"
#include "hx711.h"

sv_iot_t* g_iot;
const char* TAG = "TEST_MQTT";

void app_main(){
	nvs_flash_init();
	g_iot = sv_iot_create();
	sv_iot_init(g_iot);

	while(1){

		sv_iot_process(g_iot);
	}
}


