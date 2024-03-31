/**
 *  Create by hainh2002
 *
 *  email: hai.nh200802@gmail.com
 *
 */
#include "hal_io.h"
#include <stdio.h>
#include <stdlib.h>

hal_io_t* hal_io_init(hal_io_proc_t *_proc, void* _handle){
    static hal_io_t _this;
    _this.m_proc = _proc;
    _this.m_handle = _handle;
    return &_this;
}

void hal_io_deinit(hal_io_t *_this){
    if(_this->m_proc->close_fn_t != NULL)
        _this->m_proc->close_fn_t(_this);
    free(_this);
}

int32_t hal_io_open(hal_io_t *_this,
                       uint8_t _port,
                       uint8_t _pin,
                       uint8_t _mode){
    return _this->m_proc->open_fn_t(_this, _port, _pin, _mode);
}

int32_t hal_io_close(hal_io_t *_this){
    return _this->m_proc->close_fn_t(_this);
}

int32_t hal_io_set_value(hal_io_t *_this, uint8_t _value){
    return _this->m_proc->set_value_fn_t(_this,_value);
}

uint8_t hal_io_get_value(hal_io_t *_this){
    return _this->m_proc->get_value_fn_t(_this);
}

int32_t hal_io_toggle(hal_io_t *_this){
    uint8_t st = _this->m_proc->get_value_fn_t(_this);
    if (st == 1) st = 0;
    else st = 1;
    return _this->m_proc->set_value_fn_t(_this, st);
}
