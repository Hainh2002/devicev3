#include"sv_loadcell.h"

#define impl(x) ((loadcell_t*)(x))

loadcell_t *sv_lc_create(void* _if){
    void *_this = malloc(sizeof(loadcell_t*));
    if (_this == NULL)
        return NULL;
    impl(_this)->interface = (HX711*)_if;
    impl(_this)->mass = 0;
    impl(_this)->calib_val = 0;
    impl(_this)->event = 0;
    impl(_this)->cb_fn = NULL;
    impl(_this)->event_arg = NULL;
    return impl(_this);
}

int8_t sv_lc_destroy(loadcell_t* _this){
    if (_this == NULL) 
        return -1;
    _this->interface = NULL;
    _this->mass = 0;
    _this->calib_val = 0;
    _this->event = 0;
    _this->cb_fn = NULL;
    _this->event_arg = NULL;
    free(_this);
}

int8_t sv_lc_reg_event(loadcell_t* _this, lc_event_cb_fn_t cb_fn, void* _arg){
    if (_this == NULL)
        return -1;
    _this->cb_fn = cb_fn;
    _this->event_arg = _arg;
    return 0;
}

int8_t sv_lc_calib(loadcell_t* _this){
    if (_this == NULL)
        return -1;
    _this->interface->set_scale(LC_CALIB_VALUE);
    return 0;
}

int8_t sv_lc_tare(loadcell_t* _this){
    if (_this == NULL)
        return -1;
    _this->cb_fn(SV_LC_TARED, (void*) _this);
    _this->interface->tare();
    return 0;
}

int8_t sv_lc_get_mass(loadcell_t* _this, int32_t *_mass){
    if (_this == NULL)
        return -1;
    if (!_this->interface->is_ready()){
        _this->cb_fn(SV_LC_INIT_FAIL,(void*) _this);
        return -1;
    }
    int32_t mass;
    mass = (int32_t) _this->interface->get_units(LC_SAMPLE_NUM);
    *_mass = mass;
    return 0;
}

