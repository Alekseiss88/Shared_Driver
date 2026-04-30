#ifndef BN008X_H
#define BN008X_H

#include "bn008x_types.h"
#include "bn008x_port.h"
#include "bn008x_regs.h"

#ifdef __cplusplus
extern "C" {
#endif

// Структура драйвера
typedef struct {
    const bn008x_hal_t *hal;      // HAL-функции
    uint8_t im_ports[5];			//Таблица соотвествия портов датчика и портов в файле port.c
    void *spi;						//spi
    void *mutex;                    // Мьютекс для I2C
    void *sem_data_ready;           // Семафор для прерываний
    uint8_t initialized;            // Флаг инициализации
    uint8_t tx_seq[6];                   /**< Sequence number для отправляемых пакетов, номер 
    в массиве соответсвует номеру канала   */
    struct {
        bn008x_gyro_t gyro;           /**< Последние данные гироскопа */
        bn008x_accel_t accel;         /**< Последние данные акселерометра */
        bn008x_quaternion_t quat;     /**< Последний кватернион */
        bn008x_temp_t temp;           /**< Последняя температура */
        uint32_t timestamp;            /**< Время последнего обновления кэша (мс) */
    } cache;
} bn008x_t;

// Начальные функции
bn008x_status_t bn008x_init(bn008x_t *dev, const bn008x_hal_t *hal, SPI_HandleTypeDef* spi, uint8_t num_wake, uint8_t num_int, uint8_t num_cs, uint8_t num_ps1, uint8_t num_reset);
bn008x_status_t bn008x_reset(bn008x_t *dev);
bn008x_status_t bn008x_SetProtocolSPI(bn008x_t *dev);
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
