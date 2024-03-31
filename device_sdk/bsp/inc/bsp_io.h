#ifndef BSP_INC_BSP_IO_H
#define BSP_INC_BSP_IO_H

#include"driver/gpio.h"
#include"porting.h"
#include"hal_io.h"

static gpio_config_t g_cfg_io = {
    .intr_type = 0,
    .mode = GPIO_MODE_INPUT_OUTPUT,
    .pull_down_en = 0,
    .pull_up_en = 0,
    .pin_bit_mask = ((1ULL << PIN_8) |
                    (1ULL << PIN_3))
};

hal_io_t* g_led;
hal_io_t* g_btn;


static inline void bsp_init(){
    g_led = hal_io_init(&io_func, &g_cfg_io);
    g_btn = hal_io_init(&io_func, &g_cfg_io);

    hal_io_open(g_led, 0, PIN_8, 0);
    hal_io_open(g_btn, 0, PIN_3, 0);

    // gpio_config(&g_cfg_io);
}

static inline void bsp_led_write(uint8_t _st){
    hal_io_set_value(g_led, _st);
}

static inline uint8_t bsp_btn_read(){
    return (uint8_t) hal_io_get_value(g_btn);
}


#endif