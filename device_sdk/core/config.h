#ifndef CONFIG_H
#define CONFIG_H

#include <stdint.h>
/* DEVICE INFORMATION*/
#define DEVICE_ID                   "D001"
#define FIRMWARE_VERSION            0x000001
#define HARDWARE_VERSION            0x000101

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
//#define PHONE 						""
#ifdef	PHONE
#define WIFI_SSID                   "IBME"
#define WIFI_PASS                   "20203898"
#else
#define WIFI_SSID                   "TP-Link_7ACA"
#define WIFI_PASS                   "39406316"
#endif

#define MQTT_BROKER                 "mqtt://0.tcp.ap.ngrok.io"
#define MQTT_PORT                   11573
#define MQTT_USERNAME               "mqtt_username"
#define MQTT_PASSWORD               "mqtt_password"
#define MQTT_CLIENT_ID              DEVICE_ID
#define MQTT_STATUS_TOPIC           "ibme/device/online/"
#define MQTT_DATA_TOPIC             "ibme/device/data/"
#define MQTT_CMD_TOPIC              "ibme/device/config/"
#define MQTT_CMD_SUB_TOPIC_UPDATE   "/update"
#define MQTT_CMD_SUB_TOPIC_RESPONE  "/respone"
#define MQTT_CMD_SUB_TOPIC_STATUS   "/status"             

/*SYSTEM CONFIG*/
#define SYS_TICK_MS                 1
#define Delay(x)					vTaskDelay(x/portTICK_PERIOD_MS)
#endif
