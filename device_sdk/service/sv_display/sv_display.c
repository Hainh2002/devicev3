#include "stdio.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "sv_display.h"
#include "config.h"
static tm1637_lcd_t* g_lcd;
static dp_cfg_t g_cfg = {
		.m_on_scr_time 		= DISPLAY_ON_SCREEN_TIME_MAX,
		.m_refresh_cycle 	= DISPLAY_REFR_CYCLE,
		.m_blink_time	 	= DISPLAY_BLINK_TIME,
		.m_sleep_mode		= DISPLAY_ON_SLEEP_MODE,
};

sv_dp_t* sv_dp_create(gpio_num_t _pin_data, gpio_num_t _pin_clk){
	sv_dp_t* _this = malloc(sizeof(sv_dp_t));
	g_lcd = tm1637_init(_pin_clk, _pin_data);
	_this->m_lcd = g_lcd;
	_this->m_cfg = &g_cfg;
	_this->m_brightness = 7;
	_this->m_state = DP_ON;
	tm1637_set_brightness(_this->m_lcd, _this->m_brightness);
	elapsed_timer_resetz(&_this->m_blink_timeout, _this->m_cfg->m_blink_time);
	elapsed_timer_resetz(&_this->m_on_src_timeout, _this->m_cfg->m_on_scr_time);
	return _this;
}

void sv_dp_process(sv_dp_t* _this){
	if (_this == NULL) return;

	switch (_this->m_state) {
	case DP_OFF:
		elapsed_timer_resetz(&_this->m_on_src_timeout, _this->m_cfg->m_on_scr_time);
		break;
	case DP_ON:
		if (!elapsed_timer_get_remain(&_this->m_on_src_timeout)){
			printf("OFF DISPLAY\n");
			sv_dp_raw_all(_this,CHAR_NULL,CHAR_0,CHAR_F,CHAR_F);
			Delay(300);
			sv_dp_clear_screen(_this);
			_this->m_state = DP_OFF;
		}
		break;
	case DP_ON_BLINK:
		if (!elapsed_timer_get_remain(&_this->m_blink_timeout)){
			if (!_this->m_brightness){
				tm1637_set_brightness(_this->m_lcd, 0);
				_this->m_brightness = 0;
			}
			else{
				tm1637_set_brightness(_this->m_lcd, 7);
				_this->m_brightness = 7;
			}
		}
		break;
	case DP_ON_SCROLL:
		break;
	}
}

void sv_dp_num(sv_dp_t* _this, int16_t _num){
	if (!_this) return;
	if (_this->m_state == DP_OFF) return;
	if (_num < 0){
		if (_num < -999) _num = -999;
		_num = 0 - _num;
		tm1637_set_number(_this->m_lcd, _num);
		if (_num < 10)
			tm1637_set_segment_raw(_this->m_lcd, 2, CHAR_NEG);
		else if (_num < 100)
			tm1637_set_segment_raw(_this->m_lcd, 1, CHAR_NEG);
		else if (_num < 1000)
			tm1637_set_segment_raw(_this->m_lcd, 0, CHAR_NEG);
	}else{
		tm1637_set_number(_this->m_lcd, _num);
	}
}

void sv_dp_raw(sv_dp_t* _this, CHARACTER_t _ch, uint8_t _seg){
	if (!_this) return;

}

void sv_dp_raw_all(sv_dp_t* _this,
					CHARACTER_t _ch1,
					CHARACTER_t _ch2,
					CHARACTER_t _ch3,
					CHARACTER_t _ch4){
	tm1637_set_segment_raw(_this->m_lcd, 3, _ch4);
	tm1637_set_segment_raw(_this->m_lcd, 2, _ch3);
	tm1637_set_segment_raw(_this->m_lcd, 1, _ch2);
	tm1637_set_segment_raw(_this->m_lcd, 0, _ch1);
}

void sv_dp_blink_on(sv_dp_t* _this){
	if (!_this) return;

}

void sv_dp_blink_off(sv_dp_t* _this){
	if (!_this) return;

}

void sv_dp_set_brightness(sv_dp_t* _this, uint8_t _level){
	if (!_this) return;

}

void sv_dp_draw_loading(sv_dp_t* _this){
	if (!_this) return;
	uint8_t seg_data[] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20};
	for (uint8_t x=0; x<6; ++x)
	{
		uint8_t v_seg_data = seg_data[x%6];
		tm1637_set_segment_raw(_this->m_lcd, 0, v_seg_data);
		tm1637_set_segment_raw(_this->m_lcd, 1, v_seg_data);
		tm1637_set_segment_raw(_this->m_lcd, 2, v_seg_data);
		tm1637_set_segment_raw(_this->m_lcd, 3, v_seg_data);
		Delay(35);
	}
}
