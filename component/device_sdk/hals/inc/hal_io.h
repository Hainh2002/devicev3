/**
 *  Create by hainh2002
 *
 *  email: hai.nh200802@gmail.com
 *
 */
#ifndef HAL_INCLUDE_HAL_IO_H
#define HAL_INCLUDE_HAL_IO_H
#include <stdint.h>
/**
 * @enum
 * @brief
 *
 */
enum HAL_IO_PORT {
	HAL_IO_PORT_00 = 0,
	HAL_IO_PORT_01,
	HAL_IO_PORT_02,
	HAL_IO_PORT_03,
	HAL_IO_PORT_04,
	HAL_IO_PORT_05,
	HAL_IO_PORT_06,
	HAL_IO_PORT_07,
	HAL_IO_PORT_08,
	HAL_IO_PORT_09,
	HAL_IO_PORT_10,
	HAL_IO_PORT_11,
	HAL_IO_PORT_12,
	HAL_IO_PORT_13,
	HAL_IO_PORT_14,
	HAL_IO_PORT_15
};
/**
 * @enum
 * @brief
 *
 */
enum HAL_IO_PIN {
	HAL_IO_PIN_00 = 0,  
	HAL_IO_PIN_01,  
	HAL_IO_PIN_02,  
	HAL_IO_PIN_03,  
	HAL_IO_PIN_04,  
	HAL_IO_PIN_05,  
	HAL_IO_PIN_06,  
	HAL_IO_PIN_07,  
	HAL_IO_PIN_08,  
	HAL_IO_PIN_09,  
	HAL_IO_PIN_10,  
	HAL_IO_PIN_11,  
	HAL_IO_PIN_12,  
	HAL_IO_PIN_13,  
	HAL_IO_PIN_14,  
	HAL_IO_PIN_15 
};

/**
 * @enum
 * @brief
 *
 */
enum HAL_IO_STATE {
	HAL_IO_OFF = 0,
	HAL_IO_ON
};
/**
 * @enum
 * @brief
 *
 */
enum HAL_IO_MODE {
	HAL_IO_DIR_INPUT = 0,
	HAL_IO_DIR_OUTPUT,
	HAL_IO_INPUT_PULL_UP,
	HAL_IO_INPUT_PULL_DOWN,
	HAL_IO_OUTPUT_PULL_UP,
	HAL_IO_OUTPUT_PULL_DOWN,
	HAL_IO_ANALOG,
	HAL_IO_IRQ,
	HAL_PERIPHERAL
};

typedef struct hal_io hal_io_t;

/**
 * @struct hal_io_proc
 * @brief
 *
 */
typedef struct hal_io_proc {
    int32_t (* open_fn_t)(hal_io_t *, uint8_t , uint8_t , uint8_t);
    int32_t (* close_fn_t)(hal_io_t *);
    int32_t (* set_value_fn_t)(hal_io_t *, uint8_t );
    uint8_t (* get_value_fn_t)(hal_io_t *);
} hal_io_proc_t;

struct hal_io {
    hal_io_proc_t *m_proc;
    void *m_handle;
    uint8_t	m_port;
    uint8_t m_pin;
    uint8_t m_mode;
};
/**
 * @fn hal_io_t hal_io_init*(hal_io_proc_t*, void*)
 * @brief
 *
 * @param m_proc
 * @param handle
 * @param pin
 * @return
 */
hal_io_t* hal_io_init(hal_io_proc_t *_proc, void* _handle);
/**
 * @fn void hal_io_deinit(hal_io_t*)
 * @brief
 *
 * @param _this
 */
void hal_io_deinit(hal_io_t *_this);
/**
 * @fn int32_t hal_io_open(hal_io_t*)
 * @brief
 *
 * @param _this
 * @return
 */
int32_t hal_io_open(hal_io_t *_this, uint8_t _port, uint8_t _pin, uint8_t _mode);
/**
 * @fn int32_t hal_io_close(hal_io_t*)
 * @brief Resets IOPORT registers
 *
 * @param _this
 * @return
 */
int32_t hal_io_close(hal_io_t *_this);
/**
 * @fn int32_t hal_io_set_value(hal_io_t*, uint8_t)
 * @brief
 *
 * @param _this
 * @param level
 * @return
 */
int32_t hal_io_set_value(hal_io_t *_this, uint8_t _value);
/**
 * @fn uint8_t hal_io_get_value(hal_io_t*)
 * @brief
 *
 * @param _this
 * @return
 */
uint8_t hal_io_get_value(hal_io_t *_this);
/**
 * @fn uint8_t hal_io_toggle(hal_io_t*)
 * @brief Toggle output    
 *
 * @param _this
 * @return
 */
int32_t hal_io_toggle(hal_io_t *_this);

#endif //HAL_IO_H
