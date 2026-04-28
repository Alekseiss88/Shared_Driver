#include "bn008x_port.h"

//здесь в таблицах указывается соответствие между реальным портом и пином и абстрактным, заданный пользователем
static const uint8_t pin_map[] = {PS1_Pin, WAKE_Pin, RESET_Pin, INT_Pin, CS_PIN_Pin};

static GPIO_TypeDef* const ports[] = {PS1_GPIO_Port, WAKE_GPIO_Port, RESET_GPIO_Port, INT_GPIO_Port, CS_PIN_GPIO_Port};
// ===== Реализация функций =====

static int32_t stm32_spi_transfer(SPI_HandleTypeDef* spi, uint8_t *tx_data, uint8_t *rx_data, uint16_t len, uint32_t timeout) {
    HAL_StatusTypeDef status = HAL_SPI_TransmitReceive(spi, tx_data, rx_data, len, timeout);
    return (status == HAL_OK) ? 0 : -1;
}

static void stm32_gpio_write(uint8_t pin, uint8_t state) {
    HAL_GPIO_WritePin(ports[pin], pin_map[pin], state ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

static uint8_t stm32_gpio_read(uint8_t pin) {
    return (HAL_GPIO_ReadPin(ports[pin], pin_map[pin]) == GPIO_PIN_SET) ? 1 : 0;
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
    hal->gpio_write = stm32_gpio_write;
    hal->gpio_read = stm32_gpio_read;
    hal->delay_ms = stm32_delay_ms;
    hal->get_tick_ms = stm32_get_tick_ms;
}
