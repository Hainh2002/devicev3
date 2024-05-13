#include"sv_loadcell.h"
#include "esp_log.h"
static int32_t cal_mass(sv_loadcell_t*_this);

static hx711_t g_adc24 = {
    .dout = GPIO_NUM_3,
    .pd_sck = GPIO_NUM_2,
    .gain = HX711_GAIN_A_128
};

sv_loadcell_t *sv_lc_create(){
    sv_loadcell_t *_this = malloc(sizeof(sv_loadcell_t));
    if (_this == NULL)
        return NULL;
    _this->m_adc = &g_adc24;
    hx711_init(_this->m_adc);
    _this->m_mass = 0;
    _this->m_calib_val = 0;
    _this->m_offset_val = 0;
    _this->m_is_tared = 0;
    _this->m_state = 0;
    return _this;
}

int8_t sv_lc_reg_event(sv_loadcell_t* _this, lc_event_cb_fn_t *cb_fn, void* _arg){
    if (_this == NULL)
        return -1;
    _this->cb_fn = cb_fn;
    _this->event_arg = _arg;
    return 0;
}

int8_t sv_lc_calib(sv_loadcell_t* _this){
    if (_this == NULL)
        return -1;
    
    _this->m_calib_val = LC_CALIB_VALUE;
    _this->cb_fn->on_event(SV_LC_CALIBED, _this->event_arg);
    return 0;
}

int8_t sv_lc_tare(sv_loadcell_t* _this){
    if (_this == NULL)
        return -1;
    _this->m_offset_val += _this->m_mass;
    _this->cb_fn->on_event(SV_LC_TARED, _this->event_arg);
    return 0;
}

int8_t sv_lc_get_mass(sv_loadcell_t* _this, int32_t *_mass){
    if (_this == NULL)
        return -1;
    *_mass = _this->m_mass;
    return 0;
}

static int32_t cal_mass(sv_loadcell_t*_this){
    int32_t raw_data = 0;
    int32_t mass = 0;
    hx711_read_data(_this->m_adc,&raw_data);
    if (_this->m_calib_val == 0)
        sv_lc_calib(_this);
    mass = raw_data / _this->m_calib_val;
   	mass = mass - _this->m_offset_val;
    return mass;
} 

void sv_lc_process(sv_loadcell_t *_this){
    if (_this == NULL) return;
    _this->m_mass = cal_mass(_this);
    _this->cb_fn->update_data(_this->m_mass, _this->event_arg);
}

