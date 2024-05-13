#ifndef SV_BTN_H
#define SV_BTN_H

#include<stdint.h>
#include<stdlib.h>

#define BTN_MARGIN_PRESS        20
#define BTN_TAP_MS              60
#define BTN_DB_TAP_MS           250
#define BTN_HOLD_MS             900

enum SV_BTN_EVENT {
    SV_BTN_RELEASE = 0,
    SV_BTN_TAP,
    SV_BTN_DOUBLE_TAP,
    SV_BTN_HOLD
};

typedef void (*btn_event_cb_fn_t)(uint8_t _event, void* _arg);

typedef struct sv_btn {
    uint8_t m_state;
    uint8_t m_state_old;
    btn_event_cb_fn_t cb_fn;
    void* event_arg;
} sv_btn_t;

sv_btn_t* sv_btn_create(uint8_t);
int8_t sv_btn_reg_event(sv_btn_t *_this, btn_event_cb_fn_t _cb_fn, void* _arg);
int8_t sv_btn_process(sv_btn_t *_this);

#endif
