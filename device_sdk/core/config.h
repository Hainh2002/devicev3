#ifndef CONFIG_H
#define CONFIG_H

#include <stdint.h>
/* DEVICE INFORMATION*/
#define DEVICE_ID                   "D001"
#define FIRMWARE_VERSION            "1.0.2"
#define HARDWARE_VERSION            "2.0.1"

/*LOADCELL CONFIGURATION PARAMETERS*/
#define LC_CALIB_VALUE              400
#define LC_DOUT_PIN                 0
#define LC_CLK_PIN                  0
#define LC_SAMPLE_NUM               1

/* DISPLAY CONFIGURATION PARAMETERS*/
#define LCD_CLK_PIN					9
#define LCD_DIO_PIN					8

/* BUTTON CONFIGURATION PARAMETERS*/
#define BTN_PIN						10

/* NETWORK CONFIGURATION PARAMETERS*/

#define SMARTCONFIG_ENABLE			(1)
#if !SMARTCONFIG_ENABLE
#define WIFI_SSID                   "IBME"
#define WIFI_PASS                   "20203898"
#endif

#define FREE_BROKER					1
#if FREE_BROKER
#define MQTT_BROKER                 "mqtt://103.1.238.175"
#define MQTT_PORT                   1885
#define MQTT_USERNAME               "test"
#define MQTT_PASSWORD               "testadmin"
#else
#define MQTT_BROKER                 "mqtt://0.tcp.ap.ngrok.io"
#define MQTT_PORT                   19639
#define MQTT_USERNAME               "mqtt_username"
#define MQTT_PASSWORD               "mqtt_password"
#endif
#define MQTT_CLIENT_ID              DEVICE_ID
#define MQTT_STATUS_TOPIC           "ibme/device/online/"
#define MQTT_DATA_TOPIC             "ibme/device/data/"
#define MQTT_CMD_TOPIC              "ibme/device/config/"
#define MQTT_SHUTDOWN_TOPIC         "ibme/device/shutdown/"
#define MQTT_CMD_SUB_TOPIC_UPDATE   "update/"
#define MQTT_CMD_SUB_TOPIC_RESPONE  "respone/"
#define MQTT_CMD_SUB_TOPIC_STATUS   "status/"

/*SYSTEM CONFIG*/
#define SYS_TICK_MS                 1
#define Delay(x)					vTaskDelay(x/portTICK_PERIOD_MS)
#endif
