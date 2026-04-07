#include "bn008x_port.h"
#include "stm32f4xx_hal.h"
#include "spi.h"
#include "gpio.h"

// ===== Реализация функций =====

static int32_t stm32_spi_transfer(uint8_t *tx_data, uint8_t *rx_data, uint16_t len, uint32_t timeout) {
    HAL_StatusTypeDef status = HAL_SPI_TransmitReceive(&hspi1, tx_data, rx_data, len, timeout);
    return (status == HAL_OK) ? 0 : -1;
}

static void stm32_cs_select(uint8_t select) {
    if (select) {
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);
    } else {
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);
    }
}


static void stm32_delay_ms(uint32_t ms) {
    HAL_Delay(ms);
}

static uint32_t stm32_get_tick_ms(void) {
    return HAL_GetTick();
}

// Инициализация HAL структуры

void bn008x_hal_init_stm32(bn008x_hal_t *hal) {
    if (hal == NULL) return;
    
    hal->spi_transfer = stm32_spi_transfer;
    hal->cs_select = stm32_cs_select;
    hal->delay_ms = stm32_delay_ms;
    hal->get_tick_ms = stm32_get_tick_ms;
}
