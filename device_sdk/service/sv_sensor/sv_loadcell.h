#ifndef LOADCELL_H
#define LOADCELL_H

#include<stdint.h>
#include<stdlib.h>
#include"hx711.h"
#include"sv_btn.h"
#include"config.h"


enum SV_LC_EVENT {
    SV_LC_POWER_ON,
    SV_LC_POWER_OFF,
    SV_LC_TARED,
    SV_LC_CALIBED
};

typedef struct lc_event_cb_fn {
    void (*on_event)(uint8_t _event, void* _arg);
    void (*update_data)(int32_t _mass, void* _arg);
}lc_event_cb_fn_t; 

typedef struct {
    hx711_t             *m_adc;
    sv_btn_t            *m_btn;
    int32_t             m_mass;
    int32_t             m_calib_val;
    int32_t             m_offset_val;
    uint8_t				m_is_tared;
    uint8_t             m_state;
    lc_event_cb_fn_t    *cb_fn;
    void*               event_arg;   
} sv_loadcell_t;

sv_loadcell_t *sv_lc_create();
int8_t sv_lc_destroy(sv_loadcell_t*);
int8_t sv_lc_reg_event(sv_loadcell_t*, lc_event_cb_fn_t*, void*);
int8_t sv_lc_calib(sv_loadcell_t*);
int8_t sv_lc_tare(sv_loadcell_t*);
int8_t sv_lc_get_mass(sv_loadcell_t* , int32_t *);
void sv_lc_process(sv_loadcell_t *);
#endif // LOADCELL_H
