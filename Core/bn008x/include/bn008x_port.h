#ifndef BN008X_PORT_H
#define BN008X_PORT_H

#include <stdint.h>

// HAL-функции, которые должен предоставить пользователь
typedef struct {
    // Передача по SPI
    int32_t (*spi_transfer)(uint8_t *tx_data, uint8_t *rx_data, uint16_t len, uint32_t timeout);
    
    void (*cs_select)(uint8_t select);
    
    void (*gpio_write)(uint8_t pin, uint8_t state);
    uint8_t (*gpio_read)(uint8_t pin);

    // Задержка в мс
    void (*delay_ms)(uint32_t ms);
    
    // Получить текущее время в мс
    uint32_t (*get_tick_ms)(void);
} bn008x_hal_t;

void bn008x_hal_init_stm32(bn008x_hal_t *hal);
#endif
