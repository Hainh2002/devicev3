#include "hx711.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "sdkconfig.h"
#include "tm1637.h"
#include "sv_loadcell.h"
#define MAINTAG "MAIN"


typedef struct main_app {
    int32_t m_mass;
}main_app_t;

main_app_t ibme_app;

#define app_impl(x) ((main_app_t*)(x))

void lc_on_event_handle(uint8_t _event, void*_arg){
    ESP_LOGI(MAINTAG, "EVENT NUM : %d", _event);
}

void lc_update_data_handle(int32_t _mass, void*_arg){
    ESP_LOGI(MAINTAG, "NEW DATA: %d", _mass);
    ibme_app.m_mass = _mass;

}

int app_main(){
    tm1637_lcd_t *display = tm1637_init(GPIO_NUM_9, GPIO_NUM_8);

    int32_t volume = 0;
    sv_loadcell_t *g_lc = sv_lc_create();
    lc_event_cb_fn_t lc_cb = {
        .on_event = lc_on_event_handle,
        .update_data = lc_update_data_handle
    };

    sv_lc_reg_event(g_lc, &lc_cb, &ibme_app);

    vTaskDelay(1000/portTICK_PERIOD_MS);
    
    sv_lc_tare(g_lc);

    while(1){
        sv_lc_process(g_lc);
        volume = ibme_app.m_mass;
        ESP_LOGI(MAINTAG, "%d", volume);

        tm1637_set_brightness(display, 7);
        tm1637_set_number(display, volume);

        vTaskDelay(100/portTICK_PERIOD_MS);
    }
    
}