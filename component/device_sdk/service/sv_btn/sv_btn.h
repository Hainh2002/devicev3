#ifndef SV_BTN_H
#define SV_BTN_H

#include<stdint.h>

enum SV_BTN_EVENT {
    SV_BTN_TAP,
    SV_BTN_DOUBLE_TAP,
    SV_BTN_HOLD
};

typedef uint8_t* (BTN)(uint8_t);

typedef void (*btn_event_cb_fn_t)(uint8_t, void*);

typedef struct sv_btn {
    BTN *interface;
    uint8_t state;
    uint8_t event;
    btn_event_cb_fn_t cb_fn;
    void* event_arg;
} sv_btn_t;

sv_btn_t* sv_btn_create(uint8_t _btn_num, void* _if);


#endif