/**
 * @file bn008x.h
 * @brief Публичный API драйвера BNO08x (SPI, SHTP)
 * @author Aleksei
 * @date 2025
 * @version 1.0
 *
 * @details Драйвер обеспечивает инициализацию датчика BNO08x,
 *          чтение данных (гироскоп, акселерометр, кватернион, температура)
 *          и управление режимами. Работает через SPI и протокол SHTP.
 *
 */
#ifndef BN008X_H
#define BN008X_H

#include "bn008x_types.h"
#include "bn008x_port.h"
#include "bn008x_regs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Контекст (состояние) драйвера BNO08x.
 *
 * Содержит все данные, необходимые для работы с одним экземпляром датчика.
 */
typedef struct {
    const bn008x_hal_t *hal;        /**< Указатель на HAL-абстракцию (GPIO, SPI, задержки) */
    uint8_t im_ports[5];			/**< Таблица соответствия абстрактных пинов реальным (порты и номера) */
    void *spi;						/**< Указатель на хендл SPI (SPI_HandleTypeDef) */
    void *mutex;                    /**< Мьютекс для FreeRTOS (может быть NULL) */
    void *sem_data_ready;           /**< Семафор для пробуждения задачи по прерыванию (может быть NULL) */
    uint8_t initialized;            /**< Флаг: 1 – датчик инициализирован, 0 – нет */
    uint8_t tx_seq[6];              /**< Sequence number для каждого SHTP-канала (0..5) */
    struct {
    	bn008x_gyro_t gyro;		  /**< Последние данные гироскопа (кэш) */
		bn008x_accel_t accel;     /**< Последние данные акселерометра (кэш) */
		bn008x_quaternion_t quat; /**< Последний кватернион (кэш) */
		bn008x_temp_t temp;       /**< Последняя температура (кэш) */
		uint32_t timestamp;       /**< Время последнего обновления кэша в миллисекундах */
	} cache;                      /**< Кэш данных для polling-режима */
} bn008x_t;

/**
 * @brief Полная инициализация датчика BNO08x.
 *
 * Выполняет:
 * - инициализацию полей структуры,
 * - аппаратный сброс (hardreset),
 * - мягкий сброс (soft reset),
 * - пробуждение датчика,
 * - отправку Product ID Request,
 * - чтение и проверку Product ID Response.
 *
 * @param[in,out] dev        Указатель на структуру драйвера
 * @param[in]     hal        HAL-абстракция (GPIO, SPI, задержки)
 * @param[in]     spi        Указатель на хендл SPI (SPI_HandleTypeDef)
 * @param[in]     num_wake   Абстрактный номер пина WAKE (PS0)
 * @param[in]     num_int    Абстрактный номер пина INT
 * @param[in]     num_cs     Абстрактный номер пина CS
 * @param[in]     num_ps1    Абстрактный номер пина PS1
 * @param[in]     num_reset  Абстрактный номер пина RESET
 *
 * @retval BN008X_OK                   Датчик инициализирован
 * @retval BN008X_ERROR_INVALID_PARAM  dev или hal == NULL
 * @retval BN008X_ERROR_SPI            Ошибка SPI-передачи/приёма
 * @retval BN008X_ERROR_TIMEOUT        Таймаут ожидания
 * @retval BN008X_ERROR_NOT_INITIALIZED Датчик не ответил Product ID Response
 *
 * @note Функция блокирующая. Может занять до 200 мс.
 * @note Перед вызовом необходимо инициализировать SPI и GPIO.
 */
bn008x_status_t bn008x_init(bn008x_t *dev, const bn008x_hal_t *hal, SPI_HandleTypeDef* spi, uint8_t num_wake, uint8_t num_int, uint8_t num_cs, uint8_t num_ps1, uint8_t num_reset);
/**
 * @brief Мягкий сброс датчика (Soft Reset) через канал Executable.
 *
 * @param[in] dev Указатель на структуру драйвера
 *
 * @retval BN008X_OK                  Сброс выполнен
 * @retval BN008X_ERROR_INVALID_PARAM dev == NULL или hal == NULL
 * @retval BN008X_ERROR_SPI           Ошибка SPI-передачи
 */
bn008x_status_t bn008x_reset(bn008x_t *dev);
/**
 * @brief Установка пинов PS1 и WAKE в HIGH для выбора SPI-интерфейса.
 *
 * Должна вызываться до аппаратного сброса.
 *
 * @param[in] dev Указатель на структуру драйвера
 *
 * @retval BN008X_OK                  Успех
 * @retval BN008X_ERROR_INVALID_PARAM dev == NULL или hal == NULL
 */
bn008x_status_t bn008x_SetProtocolSPI(bn008x_t *dev);
/**
 * @brief Аппаратный сброс датчика (RESET).
 *
 * Выполняет:
 * - установку пинов для SPI-режима,
 * - импульс RESET,
 * - ожидание подъёма INT.
 *
 * @param[in] dev Указатель на структуру драйвера
 *
 * @retval BN008X_OK                  Датчик сброшен и вышел из bootloader
 * @retval BN008X_ERROR_INVALID_PARAM dev == NULL или hal == NULL
 * @retval BN008X_ERROR_TIMEOUT       INT не появился после сброса
 */
bn008x_status_t bn008x_hardreset(bn008x_t *dev);

// Настройки
bn008x_status_t bn008x_set_sample_rate(bn008x_t *dev, uint16_t rate_hz);
bn008x_status_t bn008x_set_gyro_range(bn008x_t *dev, uint16_t range_dps);
bn008x_status_t bn008x_set_accel_range(bn008x_t *dev, uint16_t range_g);
bn008x_status_t bn008x_set_mode(bn008x_t *dev, bn008x_mode_t mode);

// Чтение данных с датчиков
bn008x_status_t bn008x_read_gyro(bn008x_t *dev, bn008x_gyro_t *gyro);
bn008x_status_t bn008x_read_accel(bn008x_t *dev, bn008x_accel_t *accel);
bn008x_status_t bn008x_read_quaternion(bn008x_t *dev, bn008x_quaternion_t *quat);
bn008x_status_t bn008x_read_temperature(bn008x_t *dev, bn008x_temp_t *temp);

// Прерывания
void bn008x_isr_handler(bn008x_t *dev);
bn008x_status_t bn008x_wait_data_ready(bn008x_t *dev, uint32_t timeout_ms);

#ifdef __cplusplus
}
#endif

#endif
