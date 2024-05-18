#ifndef SV_DISPLAY_H
#define SV_DISPLAY_H

#include <stdint.h>

#include "esp_log.h"
#include "driver/gpio.h"

#include "tm1637.h"
#include "sm_elapsed_timer.h"
#include "sv_btn.h"

#define DISPLAY_ON_SLEEP_MODE				(0)
#define DISPLAY_ON_SCREEN_TIME_MAX			(25*1000)
#define DISPLAY_BLINK_TIME					(500)
#define DISPLAY_REFR_CYCLE					(100)
/*			 _a_
		   f|   |b
			|_g_|
		   e|   |c
			|_d_| .x
 */
typedef enum {  // XGFE DCBA
	CHAR_0 = 0x3f, CHAR_1 = 0x06, CHAR_2 = 0x5b, CHAR_3 = 0x4f, CHAR_4 = 0x66,
	CHAR_5 = 0x6d, CHAR_6 = 0x7d, CHAR_7 = 0x07, CHAR_8 = 0x7f,	CHAR_9 = 0x6f,
	CHAR_A = 0x77, CHAR_b = 0x7c, CHAR_C = 0x39, CHAR_c = 0x59,	CHAR_d = 0x5e,
	CHAR_E = 0x79, CHAR_F = 0x71, CHAR_H = 0x76, CHAR_h = 0x74,	CHAR_N = 0x37,
	CHAR_o = 0x5c, CHAR_I = 0x06, CHAR_i = 0x04, CHAR_L = 0x38, CHAR_NEG = 0x40,

	CHAR_NULL = 0x00,
} CHARACTER_t;

enum {
	DP_OFF,
	DP_ON,
	DP_ON_BLINK,
	DP_ON_SCROLL,
};
typedef struct dp_cfg{
	int32_t 	m_on_scr_time;
	int32_t	m_refresh_cycle;
	int32_t 	m_blink_time;
	int8_t		m_sleep_mode;
}dp_cfg_t;
typedef struct sv_dp {
	tm1637_lcd_t 	*m_lcd;
	dp_cfg_t		*m_cfg;
	uint8_t 		m_state;
	uint8_t         m_brightness;
	CHARACTER_t		m_seg[4];
	elapsed_timer_t m_blink_timeout;
	elapsed_timer_t m_on_src_timeout;
}sv_dp_t;

sv_dp_t* sv_dp_create(gpio_num_t _pin_data, gpio_num_t _pin_clk);
void sv_dp_process(sv_dp_t* _this);
void sv_dp_num(sv_dp_t* _this, int16_t _num);
void sv_dp_raw(sv_dp_t* _this, CHARACTER_t _ch, uint8_t _seg);
void sv_dp_raw_all(sv_dp_t* _this,
					CHARACTER_t _ch1,
					CHARACTER_t _ch2,
					CHARACTER_t _ch3,
					CHARACTER_t _ch4);

void sv_dp_blink_on(sv_dp_t* _this);
void sv_dp_blink_off(sv_dp_t* _this);
void sv_dp_set_brightness(sv_dp_t* _this, uint8_t _level);
static inline void sv_dp_clear_screen(sv_dp_t* _this){
	sv_dp_raw_all(_this, CHAR_NULL, CHAR_NULL, CHAR_NULL, CHAR_NULL);
}

static inline void sv_dp_btn_event_cb(sv_dp_t* _this, uint8_t _event){
	 switch (_event) {
	        case SV_BTN_TAP:
	        	if (_this->m_state == DP_OFF)
	        		_this->m_state = DP_ON;
	            break;
	        case SV_BTN_HOLD:
	        	if (_this->m_state == DP_ON)
	        	{
	        		_this->m_state = DP_OFF;
	        		sv_dp_clear_screen(_this);
	        	}
	            break;
	    }
}
void sv_dp_draw_loading(sv_dp_t* _this);
#endif //SV_DISPLAY_H
