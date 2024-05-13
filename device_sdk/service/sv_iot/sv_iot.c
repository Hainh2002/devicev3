#include "sv_iot.h"
static char* TAG = "SV_IOT";

#define impl(x) (sv_iot_t*)(x)

/* static variables */
static esp_netif_t *s_sta_netif = NULL;
static int s_retry_num = 0;

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
static void wifi_start(){
	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
	ESP_ERROR_CHECK(esp_wifi_init(&cfg));

	esp_netif_inherent_config_t esp_netif_config = ESP_NETIF_INHERENT_DEFAULT_WIFI_STA();
	esp_netif_config.if_desc = "";
	esp_netif_config.route_prio = 128;
	s_sta_netif = esp_netif_create_wifi(WIFI_IF_STA, &esp_netif_config);
	esp_wifi_set_default_wifi_sta_handlers();

	ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
	ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
	ESP_ERROR_CHECK(esp_wifi_start());
}
static esp_err_t wifi_sta_do_connect(sv_iot_t* _this){
	wifi_config_t wifi_config = *_this->m_wifi->m_wifi_cfg;
    s_retry_num = 0;
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED, &handler_on_wifi_disconnect, (void*)_this));
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_STA_CONNECTED, &handler_on_wifi_connect, (void*)_this));
    ESP_LOGI(TAG, "Connecting to %s...", wifi_config.sta.ssid);
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    esp_err_t ret = esp_wifi_connect();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "WiFi connect failed! ret:%x", ret);
        return ret;
    }
    if (s_retry_num > CONFIG_WIFI_MAXIMUM_RETRY) {
            return ESP_FAIL;
    }
    return ESP_OK;
}
static esp_err_t wifi_connect(sv_iot_t* _this){
	ESP_LOGI(TAG, "Start wifi connect.");
	wifi_start();
	return wifi_sta_do_connect(_this);
}
static void wifi_sta_do_disconnect(){
	ESP_ERROR_CHECK(esp_event_handler_unregister(WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED, &handler_on_wifi_disconnect));
	ESP_ERROR_CHECK(esp_event_handler_unregister(WIFI_EVENT, WIFI_EVENT_STA_CONNECTED, &handler_on_wifi_connect));
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
    printf("\nTOPIC=%.*s\r\n", event->topic_len, _this->m_mqtt->m_rx->m_topic);
    printf("DATA=%.*s\r\n", event->data_len, _this->m_mqtt->m_rx->m_msg);
}

static void mqtt_event_handler(void *handler_args, 
                            esp_event_base_t base, 
                            int32_t event_id, 
                            void *event_data)
{
	esp_mqtt_event_handle_t event = event_data;
	esp_mqtt_client_handle_t client = event->client;
	int msg_id;
	switch ((esp_mqtt_event_id_t)event_id) {
	case MQTT_EVENT_CONNECTED:
		ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
		esp_mqtt_client_publish(client, "device", "online", 0, 0, 0);
		mqtt_set_state((sv_iot_t*)handler_args, ST_CONNECTED);
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
		printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
		printf("DATA=%.*s\r\n", event->data_len, event->data);
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
	if (_this->m_mqtt->m_state == ST_CONNECTED){
		if (_this->m_mqtt->m_tx->m_is_new_msg){
			esp_mqtt_client_publish(*_this->m_mqtt->m_handle,
                            _this->m_mqtt->m_tx->m_topic,
                            _this->m_mqtt->m_tx->m_msg, 0, 0, 0);
			_this->m_mqtt->m_tx->m_is_new_msg = 0;
		}
	}
}
sv_iot_t *sv_iot_create(){
    sv_iot_t* _this = malloc(sizeof(sv_iot_t*));

    _this->m_wifi       		= &g_wifi;
    _this->m_mqtt				= &g_mqtt;
    _this->m_mqtt->m_handle		= &g_mqtt_client_t;
    _this->m_mqtt->m_state = ST_INIT;

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
    if (!_this || !_topic || !_data) {
    	ESP_LOGE(TAG, "INVALID INPUT !!!");
    	return;
    }

    _this->m_mqtt->m_tx->m_is_new_msg = 1;
    strncpy(_this->m_mqtt->m_tx->m_topic, _topic, strlen(_topic));
    strncpy(_this->m_mqtt->m_tx->m_msg, _data, strlen(_data));
    printf("topic: %s , data: %s\n", _this->m_mqtt->m_tx->m_topic, _this->m_mqtt->m_tx->m_msg);
}

void sv_iot_process(sv_iot_t *_this){
	if (!_this) return;
	mqtt_process(_this);
}
