#include "sv_iot.h"
#include "cJSON.h"
#include "cJSON_Utils.h"

static char* TAG = "SV_IOT";

#define impl(x) ((sv_iot_t*)(x))

/* static variables */
static esp_netif_t *s_sta_netif = NULL;
static int s_retry_num = 0;
static uint8_t smart_config_done = 0;
static uint8_t got_ip_address	= 0;
static char topic[128];
static wifi_config_t g_wifi_cfg_t = {
		.sta = {
			.ssid 					= WIFI_SSID,
			.password 				= WIFI_PASS,
			.scan_method 			= WIFI_ALL_CHANNEL_SCAN,
			.sort_method 			= WIFI_CONNECT_AP_BY_SIGNAL,
			.threshold.rssi 		= -127,
			.threshold.authmode 	= WIFI_AUTH_OPEN,
		}
};
static wifi_t	g_wifi	= {
		.m_state = ST_INIT,
		.m_wifi_cfg = &g_wifi_cfg_t,
};
static esp_mqtt_client_config_t g_mqtt_cfg_t = {
		.broker.address.uri		 				= MQTT_BROKER,
		.credentials.username    				= MQTT_USERNAME,
		.credentials.authentication.password    = MQTT_PASSWORD,
		.broker.address.port           			= MQTT_PORT,
};

static mqtt_msg_t g_tx_buff = {
    .m_is_new_msg = 0,
    .m_topic = "",
    .m_msg = "",
};
static mqtt_msg_t g_rx_buff = {
    .m_is_new_msg = 0,
    .m_topic = "",
    .m_msg = "",
};
static esp_mqtt_client_handle_t g_mqtt_client_t;
static mqtt_client_t g_mqtt = {
		.m_handle		= &g_mqtt_client_t,
		.m_mqtt_cfg 	= &g_mqtt_cfg_t,
		.m_tx			= &g_tx_buff,
		.m_rx			= &g_rx_buff,
};
static char* mqtt_topic_path_build(sv_iot_t* _this, char* _main_topic, char* _sub_topic, char* _id);
/* wifi event cb */
static void wifi_set_state(sv_iot_t* _this, STATE_OF_CONNECTION _st){

	 _this->m_wifi->m_state = _st;
}
static void handler_on_wifi_disconnect(void *_arg, esp_event_base_t event_base,
                               int32_t event_id, void *event_data)
{
	wifi_set_state((sv_iot_t*)_arg, ST_DISCONNECTED);
    s_retry_num++;
    if (s_retry_num > CONFIG_WIFI_MAXIMUM_RETRY) {
        ESP_LOGI(TAG, "WiFi Connect failed %d times, stop reconnect.", s_retry_num);
        return;
    }
    ESP_LOGI(TAG, "Wi-Fi disconnected, trying to reconnect...");
    esp_err_t err = esp_wifi_connect();
    if (err == ESP_ERR_WIFI_NOT_STARTED) {
        return;
    }
    ESP_ERROR_CHECK(err);
}
static void handler_on_wifi_connect(void *_arg, esp_event_base_t event_base,
                            int32_t event_id, void *event_data)
{
	ESP_LOGI(TAG, "WIFI CONNECTED EVENT");
	wifi_set_state((sv_iot_t*)_arg, ST_CONNECTED);

}
static void handler_on_sta_start(void *_arg, esp_event_base_t event_base,
                            int32_t event_id, void *event_data)
{
	ESP_ERROR_CHECK( esp_smartconfig_set_type(SC_TYPE_ESPTOUCH) );
	smartconfig_start_config_t cfg = SMARTCONFIG_START_CONFIG_DEFAULT();
	ESP_ERROR_CHECK( esp_smartconfig_start(&cfg) );
	ESP_LOGI(TAG, "Smart config start .");
}
static void wifi_event_callback_handel(void *_arg, esp_event_base_t event_base,
                               int32_t event_id, void *event_data){
	if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
		handler_on_sta_start(_arg, event_base, event_id, event_data);
	} else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
		handler_on_wifi_disconnect(_arg, event_base, event_id, event_data);
		got_ip_address = 0;
	} else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_CONNECTED) {
		handler_on_wifi_connect(_arg, event_base, event_id, event_data);
	} else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
		ESP_LOGI(TAG, "Got IP address");
		got_ip_address = 1;
	} else if (event_base == SC_EVENT && event_id == SC_EVENT_SCAN_DONE) {
		ESP_LOGI(TAG, "Scan done");
	} else if (event_base == SC_EVENT && event_id == SC_EVENT_FOUND_CHANNEL) {
		ESP_LOGI(TAG, "Found channel");
	} else if (event_base == SC_EVENT && event_id == SC_EVENT_GOT_SSID_PSWD) {
		ESP_LOGI(TAG, "Got SSID and password");
		smartconfig_event_got_ssid_pswd_t *evt = (smartconfig_event_got_ssid_pswd_t *)event_data;
		wifi_config_t wifi_config;
		uint8_t ssid[33] = { 0 };
		uint8_t password[65] = { 0 };
		uint8_t rvd_data[33] = { 0 };

		bzero(&wifi_config, sizeof(wifi_config_t));
		memcpy(wifi_config.sta.ssid, evt->ssid, sizeof(wifi_config.sta.ssid));
		memcpy(wifi_config.sta.password, evt->password, sizeof(wifi_config.sta.password));
		wifi_config.sta.bssid_set = evt->bssid_set;
		if (wifi_config.sta.bssid_set == true) {
			memcpy(wifi_config.sta.bssid, evt->bssid, sizeof(wifi_config.sta.bssid));
		}

		memcpy(ssid, evt->ssid, sizeof(evt->ssid));
		memcpy(password, evt->password, sizeof(evt->password));
		ESP_LOGI(TAG, "SSID:%s", ssid);
		ESP_LOGI(TAG, "PASSWORD:%s", password);
		if (evt->type == SC_TYPE_ESPTOUCH_V2) {
			ESP_ERROR_CHECK( esp_smartconfig_get_rvd_data(rvd_data, sizeof(rvd_data)) );
			ESP_LOGI(TAG, "RVD_DATA:");
			for (int i=0; i<33; i++) {
				printf("%02x ", rvd_data[i]);
			}
			printf("\n");
		}

		ESP_ERROR_CHECK( esp_wifi_disconnect() );
		ESP_ERROR_CHECK( esp_wifi_set_config(WIFI_IF_STA, &wifi_config) );
		esp_wifi_connect();
	} else if (event_base == SC_EVENT && event_id == SC_EVENT_SEND_ACK_DONE) {
		ESP_LOGI(TAG, "Smart config DONE!");
		esp_smartconfig_stop();
    }
}

static void wifi_start(sv_iot_t* _this){
/*	esp_netif_inherent_config_t esp_netif_config = ESP_NETIF_INHERENT_DEFAULT_WIFI_STA();
	esp_netif_config.if_desc = "";
	esp_netif_config.route_prio = 128;
	s_sta_netif = esp_netif_create_wifi(WIFI_IF_STA, &esp_netif_config);
	esp_wifi_set_default_wifi_sta_handlers();*/

    esp_netif_t *sta_netif = esp_netif_create_default_wifi_sta();
	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
	ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_callback_handel, (void*)_this);
    esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_callback_handel, (void*)_this);
    esp_event_handler_register(SC_EVENT, ESP_EVENT_ANY_ID, &wifi_event_callback_handel, (void*) _this);

	ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
	ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
	ESP_ERROR_CHECK(esp_wifi_start());
}
static esp_err_t wifi_sta_do_connect(sv_iot_t* _this){
	wifi_config_t wifi_config = *_this->m_wifi->m_wifi_cfg;
    s_retry_num = 0;


/*
    ESP_LOGI(TAG, "Connecting to %s...", wifi_config.sta.ssid);
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));

    esp_err_t ret = esp_wifi_connect();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "WiFi connect failed! ret:%x", ret);
        return ret;
    }
    if (s_retry_num > CONFIG_WIFI_MAXIMUM_RETRY) {
            return ESP_FAIL;
    }*/
    return ESP_OK;
}
static esp_err_t wifi_connect(sv_iot_t* _this){
	ESP_LOGI(TAG, "Start wifi connect.");
	wifi_start(_this);
	return wifi_sta_do_connect(_this);
}
static void wifi_sta_do_disconnect(){
//	ESP_ERROR_CHECK(esp_event_handler_unregister(WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED, &handler_on_wifi_disconnect));
//	ESP_ERROR_CHECK(esp_event_handler_unregister(WIFI_EVENT, WIFI_EVENT_STA_CONNECTED, &handler_on_wifi_connect));
	esp_wifi_disconnect();
}
static void wifi_stop(void)
{
    esp_err_t err = esp_wifi_stop();
    if (err == ESP_ERR_WIFI_NOT_INIT) {
        return;
    }
    ESP_ERROR_CHECK(err);
    ESP_ERROR_CHECK(esp_wifi_deinit());
    ESP_ERROR_CHECK(esp_wifi_clear_default_wifi_driver_and_handlers(s_sta_netif));
    esp_netif_destroy(s_sta_netif);
    s_sta_netif = NULL;
}
static void wifi_shutdown(void)
{
    wifi_sta_do_disconnect();
    wifi_stop();
//    g_wifi.m_state = WIFI_ST_DISCONNECTED;
}

static void mqtt_set_state(sv_iot_t* _this, STATE_OF_CONNECTION _state){
	_this->m_mqtt->m_state = _state;
	printf("MQTT SET STATE : %d\n", _state);
}
static void mqtt_recv_data_handle(sv_iot_t* _this, void* _data){
    if (!_this) return ;
    esp_mqtt_event_handle_t event = _data;
    _this->m_mqtt->m_rx->m_is_new_msg = 1;
    strncpy(_this->m_mqtt->m_rx->m_topic, event->topic, event->topic_len);
    strncpy(_this->m_mqtt->m_rx->m_msg, event->data, event->data_len);
    printf("\nRX TOPIC=%.*s\r\n", event->topic_len, _this->m_mqtt->m_rx->m_topic);
    printf("RX DATA=%.*s\r\n", event->data_len, _this->m_mqtt->m_rx->m_msg);


}

static void mqtt_event_handler(void *handler_args, 
                            esp_event_base_t base, 
                            int32_t event_id, 
                            void *event_data)
{
	esp_mqtt_event_handle_t event = event_data;
	esp_mqtt_client_handle_t client = event->client;
	int msg_id;
	char* topic_buffer ;
	switch ((esp_mqtt_event_id_t)event_id) {
	case MQTT_EVENT_CONNECTED:
		ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
		mqtt_set_state(impl(handler_args), ST_CONNECTED);

		topic_buffer = mqtt_topic_path_build(impl(handler_args), MQTT_CMD_TOPIC, MQTT_CMD_SUB_TOPIC_UPDATE, DEVICE_ID);
		sv_iot_mqtt_sub(impl(handler_args), topic_buffer);
		printf("SUB: %s\n",topic_buffer);
		topic_buffer = NULL;

		topic_buffer = mqtt_topic_path_build(impl(handler_args), MQTT_SHUTDOWN_TOPIC, MQTT_CMD_SUB_TOPIC_UPDATE, DEVICE_ID);
		sv_iot_mqtt_sub(impl(handler_args), topic_buffer);
		printf("SUB: %s\n",topic_buffer);
		topic_buffer = NULL;

		break;
	case MQTT_EVENT_DISCONNECTED:
		ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
		mqtt_set_state((sv_iot_t*)handler_args, ST_DISCONNECTED);
		break;
	case MQTT_EVENT_SUBSCRIBED:
		ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
		break;
	case MQTT_EVENT_UNSUBSCRIBED:
		ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
		break;
	case MQTT_EVENT_PUBLISHED:
		ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
		break;
	case MQTT_EVENT_DATA:
		ESP_LOGI(TAG, "MQTT_EVENT_DATA");
		mqtt_recv_data_handle((sv_iot_t*)handler_args, event_data);
		break;
	case MQTT_EVENT_ERROR:
		ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
		break;
	default:
		ESP_LOGI(TAG, "Other event id:%d", event->event_id);
		break;
	}
}

static void mqtt_process(sv_iot_t *_this)
{
	if (_this->m_mqtt->m_state == ST_CONNECTED ){
		if (_this->m_mqtt->m_tx->m_is_new_msg && _this->m_denied == 0){
			esp_mqtt_client_publish(*_this->m_mqtt->m_handle,
									_this->m_mqtt->m_tx->m_topic,
									_this->m_mqtt->m_tx->m_msg, 0, 0, 0);
			_this->m_mqtt->m_tx->m_is_new_msg = 0;
			_this->m_mqtt->m_tx->m_topic[0] = '\0';
			_this->m_mqtt->m_tx->m_msg[0] = '\0';
		}
		if (_this->m_mqtt->m_rx->m_is_new_msg){
			char* topic = _this->m_mqtt->m_rx->m_topic;
			char* data = _this->m_mqtt->m_rx->m_msg;
			if (!strcmp(topic, "ibme/device/shutdown/update/D001")){
				cJSON *obj_data = cJSON_Parse(data);
				if (!obj_data) return;
				cJSON *obj_shutdown = cJSON_GetObjectItemCaseSensitive(obj_data, "shutdown");
				if (!obj_shutdown) return;
				cJSON *obj_new_status 	= cJSON_GetObjectItemCaseSensitive(obj_shutdown, "new_status");
				if (cJSON_IsNumber(obj_new_status)){
					if (obj_new_status->valuedouble == 0 && _this->m_denied == 1){
						_this->m_denied =0;
					}else if (obj_new_status->valuedouble == 1 &&  _this->m_denied == 0){
						_this->m_denied =1;
					}
				}else{
					return;
				}

				char* data_buff;
				cJSON *device = cJSON_CreateObject();
				cJSON_AddStringToObject(device, "id", DEVICE_ID);
				cJSON *shutdown = cJSON_CreateObject();
				cJSON_AddItemToObject(shutdown, "new", obj_new_status);
				cJSON_AddItemToObject(device, "shutdown", shutdown);
				data_buff = cJSON_Print(device);
				cJSON_Delete(device);
				esp_mqtt_client_publish(*_this->m_mqtt->m_handle,
										"ibme/device/shutdown/response/D001",
										data_buff, 0, 0, 0);

				device = cJSON_CreateObject();
				cJSON_AddStringToObject(device, "id", DEVICE_ID);
				cJSON_AddItemToObject(device, "shutdown", shutdown);
				cJSON_AddItemToObject(shutdown, "status", obj_new_status);
//				cJSON_AddItemToObject(device, "shutdown", shutdown);
//				data_buff = cJSON_Print(device);
				cJSON_Delete(device);
//
//				esp_mqtt_client_publish(*_this->m_mqtt->m_handle,
//										"ibme/device/shutdown/status/D001",
//										data_buff, 0, 0, 0);
			}
			_this->m_mqtt->m_rx->m_is_new_msg = 0;
		}
	}
}
sv_iot_t *sv_iot_create(){
    sv_iot_t* _this = malloc(sizeof(sv_iot_t*));

    _this->m_wifi       		= &g_wifi;
    _this->m_mqtt				= &g_mqtt;
    _this->m_mqtt->m_handle		= &g_mqtt_client_t;
    _this->m_mqtt->m_state = ST_INIT;
    _this->m_denied				= 0;
    return _this;
}
void sv_iot_init(sv_iot_t* _this){
	ESP_ERROR_CHECK(esp_netif_init());
	ESP_ERROR_CHECK(esp_event_loop_create_default());
	wifi_connect(_this);
	*_this->m_mqtt->m_handle = esp_mqtt_client_init(_this->m_mqtt->m_mqtt_cfg);
	esp_mqtt_client_register_event(*_this->m_mqtt->m_handle,
													ESP_EVENT_ANY_ID,
													mqtt_event_handler,
													(void*)_this);
	esp_mqtt_client_start(*_this->m_mqtt->m_handle);
	printf("SV IOT INITED\n");
}
void sv_iot_mqtt_sub(sv_iot_t* _this, const char* _topic){
    if (!_this || !_topic) return;
    esp_mqtt_client_subscribe(*_this->m_mqtt->m_handle, _topic, 0);
}
void sv_iot_mqtt_build_data(sv_iot_t* _this, const char* _topic, const char* _data){
    if (!_this) return ;
    if(!_topic || !_data) {
    	ESP_LOGE(TAG, "INVALID INPUT !!!");
    	return;
    }
    if (_this->m_mqtt->m_tx->m_is_new_msg) {
    	ESP_LOGE(TAG, "MQTT DATA TX IS BUSY !!!");
    	return;
    }
    _this->m_mqtt->m_tx->m_is_new_msg = 1;
    memset(_this->m_mqtt->m_tx->m_topic, '\0', MQTT_TOPIC_SIZE);
    memset(_this->m_mqtt->m_tx->m_msg, '\0', MQTT_DATA_SIZE);
    strncpy(_this->m_mqtt->m_tx->m_topic, _topic, strlen(_topic));
    strncpy(_this->m_mqtt->m_tx->m_msg, _data, strlen(_data));
//    printf("topic: %s , data: %s\n", _this->m_mqtt->m_tx->m_topic, _this->m_mqtt->m_tx->m_msg);
}

static char* mqtt_topic_path_build(sv_iot_t* _this, char* _main_topic, char* _sub_topic, char* _id){

	memset(topic, '\0',128);
	strcat(topic, _main_topic);
	strcat(topic, _sub_topic);
	strcat(topic, _id);
//	printf("%s\n",topic);
	return topic;
}

void sv_iot_process(sv_iot_t *_this){
	if (!_this) return;
	mqtt_process(_this);
}

