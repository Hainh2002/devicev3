#ifndef BSP_INC_BSP_IO_H
#define BSP_INC_BSP_IO_H

#include"driver/gpio.h"
#include"porting.h"
#include"hal_io.h"

/*static gpio_config_t g_cfg_io = {
    .intr_type = 0,
    .mode = GPIO_MODE_INPUT_OUTPUT,
    .pull_down_en = 0,
    .pull_up_en = 0,
    .pin_bit_mask = (1ULL << PIN_0) | (1ULL << PIN_1) | 
                    (1ULL << PIN_2) | (1ULL << PIN_3) |
                    (1ULL << PIN_4) | (1ULL << PIN_5) |
                    (1ULL << PIN_6) | (1ULL << PIN_7) |
                    (1ULL << PIN_8) | (1ULL << PIN_9) |
                    (1ULL << PIN_10)
};*/

static inline void bsp_init(){
//    gpio_config(&g_cfg_io);
}

#endif
