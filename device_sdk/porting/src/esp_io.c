#include "hal_io.h"
#include "porting.h"
#include "driver/gpio.h"

#define _impl(x) ((gpio_config_t*)(x))

static int32_t gpio_open(hal_io_t *_this, uint8_t _port, uint8_t _pin, uint8_t _mode);
static int32_t gpio_close(hal_io_t *_this);
static int32_t gpio_set_value(hal_io_t*, uint8_t);
static uint8_t gpio_get_value(hal_io_t*);

hal_io_proc_t io_func = {.close_fn_t = gpio_close, 
                        .open_fn_t = gpio_open,
		                .set_value_fn_t = gpio_set_value, 
                        .get_value_fn_t = gpio_get_value };

static int32_t gpio_open(hal_io_t *_this, uint8_t _port, uint8_t _pin, uint8_t _mode) {
	/*User code*/
    _this->m_port = _port;
    _this->m_pin = _pin;
    _this->m_mode = _mode;

    _impl(_this->m_handle)->intr_type = GPIO_INTR_DISABLE;
    _impl(_this->m_handle)->mode    = GPIO_MODE_INPUT_OUTPUT;
    _impl(_this->m_handle)->pin_bit_mask = 1ULL << (gpio_num_t)_pin;
    _impl(_this->m_handle)->pull_down_en = 0;
    _impl(_this->m_handle)->pull_up_en = 0;

    gpio_config(_impl(_this->m_handle));
    
	return 0;
}

static int32_t gpio_set_value(hal_io_t *_this, uint8_t _value) {
	/*User code*/
    return gpio_set_level((gpio_num_t) _this->m_pin, (uint32_t)_value);
	
}
static int32_t gpio_close(hal_io_t *_this) {
	/*User code*/
	return 0;
}

static uint8_t gpio_get_value(hal_io_t *_this) {
	/*User code*/
    gpio_get_level(_this->m_pin);
	return (uint8_t)gpio_get_level(_this->m_pin);
}