#include"sv_btn.h"
#include"driver/gpio.h"
#include"esp_log.h"
#include"config.h"

static struct st_cnt {
    int64_t release_cnt;
    int64_t press_cnt;
    int64_t tap_cnt;
    int64_t db_tap_cnt;
    int64_t hold_cnt;
}btn_st_cnt;

sv_btn_t* sv_btn_create(uint8_t _pin){
    sv_btn_t *_this = malloc(sizeof(sv_btn_t));
    if (_this == NULL)
        return NULL;
    _this->m_state = 1;
    _this->m_state_old = 1;
    btn_st_cnt.tap_cnt = 0;
    btn_st_cnt.press_cnt = 0;
    btn_st_cnt.release_cnt =0;
	gpio_set_direction(_pin, GPIO_MODE_INPUT);
    return _this;
}

int8_t sv_btn_reg_event(sv_btn_t *_this, btn_event_cb_fn_t _cb_fn, void* _arg){
    if (_this == NULL)
        return -1;
    _this->cb_fn = _cb_fn;
    _this->event_arg = _arg;
    return 0;
}

int8_t sv_btn_process(sv_btn_t *_this){
    if (_this == NULL)
        return -1;

    int btn_st ;
    btn_st = gpio_get_level(BTN_PIN); // 1: Release, 0: Press
    if (btn_st == 0) {
        btn_st_cnt.press_cnt += SYS_TICK_MS;
        if (btn_st_cnt.press_cnt > BTN_HOLD_MS && btn_st_cnt.release_cnt == BTN_MARGIN_PRESS){
            _this->m_state_old = _this->m_state;
            _this->m_state = SV_BTN_HOLD;
            btn_st_cnt.release_cnt = 0;
            _this->cb_fn(_this->m_state, _this->event_arg);
        }
        _this->m_state = SV_BTN_RELEASE;
    }
    else {
        btn_st_cnt.release_cnt += SYS_TICK_MS;
        if (btn_st_cnt.release_cnt > BTN_MARGIN_PRESS){
            btn_st_cnt.release_cnt = BTN_MARGIN_PRESS;
            if (btn_st_cnt.press_cnt > BTN_TAP_MS && btn_st_cnt.press_cnt < BTN_HOLD_MS){
                _this->m_state_old = _this->m_state;
                _this->m_state = SV_BTN_TAP;
                btn_st_cnt.press_cnt = 0;
                _this->cb_fn(SV_BTN_TAP, _this->event_arg);
            }else{
                  _this->m_state = SV_BTN_RELEASE;
                  btn_st_cnt.press_cnt = 0;
            }    
        }
    }

    return 0;
}
   
