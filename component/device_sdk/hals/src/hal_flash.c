/**
 *  Create by hainh2002
 *
 *  email: hai.nh200802@gmail.com
 *
 */
#include "hal_flash.h"
#include <stdio.h>
#include <stdlib.h>

hal_flash_t *hal_flash_init(hal_flash_proc_t *_proc,void* _handle){

    hal_flash_t* _this = malloc(sizeof(hal_flash_t));

    _this->handle = _handle;
    _this->proc = _proc;
    return _this;

}

void hal_flash_deinit(hal_flash_t *_this){

    if(_this->proc->close != NULL)
        _this->proc->close(_this);
    free(_this);
}