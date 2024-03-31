#ifndef PORTING_INC_PORTING_H_
#define PORTING_INC_PORTING_H_

// #include "hal_adc.h"
// #include "hal_delay.h"
// #include "hal_flash.h"
#include "hal_io.h"
// #include "hal_timer.h"

typedef enum {
    PIN_NC = -1,    /*!< Use to signal not connected to S/W */
    PIN_0 = 0,     /*!< GPIO0, input and output */
    PIN_1 = 1,     /*!< GPIO1, input and output */
    PIN_2 = 2,     /*!< GPIO2, input and output */
    PIN_3 = 3,     /*!< GPIO3, input and output */
    PIN_4 = 4,     /*!< GPIO4, input and output */
    PIN_5 = 5,     /*!< GPIO5, input and output */
    PIN_6 = 6,     /*!< GPIO6, input and output */
    PIN_7 = 7,     /*!< GPIO7, input and output */
    PIN_8 = 8,     /*!< GPIO8, input and output */
    PIN_9 = 9,     /*!< GPIO9, input and output */
    PIN_10 = 10,   /*!< GPIO10, input and output */
    PIN_11 = 11,   /*!< GPIO11, input and output */
    PIN_12 = 12,   /*!< GPIO12, input and output */
    PIN_13 = 13,   /*!< GPIO13, input and output */
    PIN_14 = 14,   /*!< GPIO14, input and output */
    PIN_15 = 15,   /*!< GPIO15, input and output */
    PIN_16 = 16,   /*!< GPIO16, input and output */
    PIN_17 = 17,   /*!< GPIO17, input and output */
    PIN_18 = 18,   /*!< GPIO18, input and output */
    PIN_19 = 19,   /*!< GPIO19, input and output */
    PIN_20 = 20,   /*!< GPIO20, input and output */
    PIN_21 = 21,   /*!< GPIO21, input and output */
    PIN_MAX,
} PIN_NUM;

// extern hal_adc_proc_t adc_func;
// extern hal_flash_proc_t flash_func;
extern hal_io_proc_t io_func;
// extern hal_timer_proc_t timer_func;


#endif /* PORTING_INC_PORTING_H_ */