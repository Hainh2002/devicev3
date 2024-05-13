#ifndef SV_IOT_H
#define SV_IOT_H
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "esp_wifi.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"


#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"
#include "mqtt_client.h"

#include "esp_log.h"
#include "config.h"

#define MQTT_TOPIC_SIZE					128
#define MQTT_DATA_SIZE					128

typedef enum {
    SV_IOT_EVENT_WIFI_CONNECETED,
    SV_IOT_EVENT_WIFI_DISCONNECTED,
    SV_IOT_EVENT_WIFI_RECFG_SUCC,
    SV_IOT_EVENT_WIFI_RECFG_FAIL,
    SV_IOT_EVENT_MQTT_CONNECTED,
    SV_IOT_EVENT_MQTT_DISCONNECTED,
}SV_IOT_EVENT;

typedef enum {
	ST_INIT = 0,
	ST_DISCONNECTED,
	ST_CONNECTED,
	ST_RECONNECT,
	ST_UPDATE_CFG,
	ST_NULL,
} STATE_OF_CONNECTION;
typedef struct wifi_sta {
	uint8_t 			m_state;
	wifi_config_t 		*m_wifi_cfg;
} wifi_t;

typedef struct mqtt_msg
{
    int8_t     m_is_new_msg;
    char       m_topic[MQTT_TOPIC_SIZE];
    char       m_msg[MQTT_DATA_SIZE];
} mqtt_msg_t;

typedef struct {
	esp_mqtt_client_config_t   *m_mqtt_cfg;
    esp_mqtt_client_handle_t   *m_handle;
    uint8_t					   m_state;
    mqtt_msg_t                 *m_tx;
    mqtt_msg_t                 *m_rx;
} mqtt_client_t;

typedef struct event_cb {
	void (*on_wifi_connection_cb)(uint8_t, void*);
	void (*on_mqtt_connection_cb)(uint8_t, void*);
	void (*on_update_iot_cfg)(uint8_t, void*, void*, void*);
}sv_iot_event_cb_t;

typedef struct sv_iot {
	wifi_t						*m_wifi;
    mqtt_client_t              	*m_mqtt;
    sv_iot_event_cb_t          	*m_cb;
    void                        *m_event_arg;
}sv_iot_t;


sv_iot_t *sv_iot_create();
void sv_iot_init(sv_iot_t* _this);
void sv_iot_mqtt_sub(sv_iot_t* _this, const char* _topic);
void sv_iot_mqtt_build_data(sv_iot_t* _this, const char* _topic, const char* _data);
void sv_iot_process(sv_iot_t *_this);


#endif
