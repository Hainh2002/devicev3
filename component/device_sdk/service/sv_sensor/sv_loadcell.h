#ifndef LOADCELL_H
#define LOADCELL_H

#include<stdint.h>
#include<stdlib.h>
#include"HX711/HX711.h"
#include"config.h"


enum SV_LC_EVENT {
    SV_LC_INIT_SUCC,
    SV_LC_INIT_FAIL,
    SV_LC_POWER_ON,
    SV_LC_POWER_OFF,
    SV_LC_TARED,
};

typedef struct loadcell sv_loadcell_t;

typedef void (*lc_event_cb_fn_t) (uint8_t, void* _arg);

typedef struct loadcell {
    HX711               *interface;
    int32_t             mass;
    int32_t             calib_val;
    uint8_t             event;
    lc_event_cb_fn_t    cb_fn;
    void*               event_arg;   
} loadcell_t;

loadcell_t *sv_lc_create(void*);
int8_t sv_lc_destroy(loadcell_t*);
int8_t sv_lc_reg_event(loadcell_t*, lc_event_cb_fn_t, void*);
int8_t sv_lc_calib(loadcell_t*);
int8_t sv_lc_tare(loadcell_t*);
int8_t sv_lc_get_mass(loadcell_t*);

#endif // LOADCELL_H