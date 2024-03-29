/**
 * Create by hainh2002 
 * 
 * hal_flash.h 
 * 
*/
#ifndef HAL_INCLUDE_HAL_FLASH_H_
#define HAL_INCLUDE_HAL_FLASH_H_

#include <stdint.h>
#include <stdio.h>
typedef struct hal_flash hal_flash_t;
/**
 * @struct hal_flash_proc
 * @brief
 *
 */
typedef struct hal_flash_proc
{
    /* data */
    int32_t (*write)(hal_flash_t *_this, uint32_t _addr, void *_data, size_t _size);
    int32_t (*read)(hal_flash_t *_this, uint32_t _addr, void *_data, size_t _size);
    int32_t (*erase)(hal_flash_t *_this, uint32_t _addr, size_t _size);
    int32_t (*open)(hal_flash_t *_this);
    int32_t (*close)(hal_flash_t *_this);
} hal_flash_proc_t;

/**
 * @struct hal_flash
 * @brief
 *
 */
struct hal_flash
{
    /* data */
    hal_flash_proc_t *proc;
    void *handle;
};
/**
 * @fn hal_flash_t hal_flash_init*(hal_flash_proc_t*, void*)
 * @brief
 *
 * @param m_proc
 * @param handle
 * @return
 */
hal_flash_t* hal_flash_init(hal_flash_proc_t *_proc, void *_handle);
/**
 * @fn void hal_flash_deinit(hal_flash_t*)
 * @brief
 *
 * @param _this
 */
void hal_flash_deinit(hal_flash_t *_this);
/**
 * @fn int32_t hal_flash_write(hal_flash_t*, uint32_t, void*, size_t)
 * @brief
 *
 * @param _this
 * @param addr
 * @param data
 * @param size
 * @return
 */
static inline int32_t hal_flash_write(hal_flash_t *_this, uint32_t _addr, void *_data, size_t _size)
{
    return _this->proc->write (_this, _addr, _data, _size);
}
/**
 * @fn int32_t hal_flash_read(hal_flash_t*, uint32_t, void*, size_t)
 * @brief
 *
 * @param _this
 * @param addr
 * @param data
 * @param size
 * @return
 */
static inline int32_t hal_flash_read(hal_flash_t *_this, uint32_t _addr, void *_data, size_t _size)
{
    return _this->proc->read (_this, _addr, _data, _size);
}
/**
 * @fn int32_t hal_flash_erase(hal_flash_t*, uint32_t, size_t)
 * @brief
 *
 * @param _this
 * @param addr
 * @param size
 * @return
 */
static inline int32_t hal_flash_erase(hal_flash_t *_this, uint32_t _addr, size_t _size)
{
    return _this->proc->erase (_this, _addr, _size);
}
/**
 * @fn int32_t hal_flash_open(hal_flash_t*)
 * @brief
 *
 * @param _this
 * @return
 */
static inline int32_t hal_flash_open(hal_flash_t *_this)
{
    return _this->proc->open (_this);
}
/**
 * @fn int32_t hal_flash_close(hal_flash_t*)
 * @brief
 *
 * @param _this
 * @return
 */
static inline int32_t hal_flash_close(hal_flash_t *_this)
{
    return _this->proc->close (_this);
}

#endif /* hal_INCLUDE_HAL_FLASH_H_ */