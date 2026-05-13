/**
 * @file bn008x.c
 * @brief Реализация драйвера BNO08x (SPI, SHTP, инициализация, чтение данных)
 * @author Aleksei
 * @date 2025
 * @version 1.0
 *
 * @details Драйвер реализует протокол SHTP поверх SPI.
 *          Поддерживает инициализацию, пробуждение, чтение Product ID,
 *          а также кэширование данных для polling-режима.
 *
 */

#include "bn008x.h"
#include <string.h>
#include "stdio.h"

/**
 * @brief Отправка SHTP-пакета по SPI.
 *
 * Формирует 4-байтовый заголовок (длина LSB/MSB, канал, seq num),
 * добавляет данные и отправляет пакет целиком.
 *
 * @param[in] dev       Указатель на контекст драйвера
 * @param[in] channel   SHTP-канал (0..5)
 * @param[in] data      Указатель на полезные данные (Report ID + параметры)
 * @param[in] data_len  Длина полезных данных (байт)
 *
 * @retval BN008X_OK                  Пакет отправлен
 * @retval BN008X_ERROR_INVALID_PARAM Неверные параметры (NULL, слишком большой пакет)
 * @retval BN008X_ERROR_SPI           Ошибка SPI-передачи
 *
 * @note Увеличивает счётчик tx_seq[channel] после отправки.
 */
static bn008x_status_t send_shtp_packet(bn008x_t *dev, uint8_t channel, 
                                         const uint8_t *data, uint16_t data_len) {
    if (!dev || !dev->hal || !dev->hal->spi_transmit) {
        return BN008X_ERROR_INVALID_PARAM;
    }
    

    if (data_len > BN008X_MAX_PACKET_SIZE - 4) {
        return BN008X_ERROR_INVALID_PARAM;
    }
    
    uint16_t total_len = data_len + 4;
    uint8_t tx_buffer[total_len];
    
    tx_buffer[0] = total_len & 0xFF;
    tx_buffer[1] = (total_len >> 8) & 0xFF;
    tx_buffer[2] = channel;
    tx_buffer[3] = dev->tx_seq[channel]++;

    for (uint16_t i = 0; i < data_len; i++) {
        tx_buffer[4 + i] = data[i];
    }
    
    // Захватываем мьютекс 
    /*if (dev->i2c_mutex) {
        xSemaphoreTake(dev->i2c_mutex, portMAX_DELAY);
    }*/
    
    dev->hal->gpio_write(dev->im_ports[BN008X_CS_PIN],0);
    uint32_t result = dev->hal->spi_transmit(dev->spi, tx_buffer, total_len, BN008X_SPI_TIMEOUT_MS);
    dev->hal->gpio_write(dev->im_ports[BN008X_CS_PIN],1);
    // Отдаём мьютекс
    /*if (dev->i2c_mutex) {
        xSemaphoreGive(dev->i2c_mutex);
    }*/
    
    return (result == 0) ? BN008X_OK : BN008X_ERROR_SPI;
}
/**
 * @brief Чтение одного SHTP-пакета из SPI.
 *
 * Сначала читает 4-байтовый заголовок, затем данные (если есть).
 *
 * @param[in]      dev     Указатель на контекст драйвера
 * @param[out]     buffer  Указатель на буфер для приёма пакета (заголовок + данные)
 * @param[out]     len     Указатель на фактическую длину принятого пакета (байт)
 *
 * @retval BN008X_OK                  Пакет прочитан
 * @retval BN008X_ERROR_INVALID_PARAM Неверная длина пакета
 * @retval BN008X_ERROR_SPI           Ошибка SPI-приёма
 *
 * @note После чтения CS поднимается. Между вызовами read_response рекомендуется
 *       небольшая задержка (1 мс), чтобы датчик успел подготовить следующий пакет.
 */
static bn008x_status_t read_response(bn008x_t *dev, uint8_t *buffer, uint16_t *len) {
    uint8_t header[4]={};
    /*if (dev->i2c_mutex) {
        xSemaphoreTake(dev->i2c_mutex, portMAX_DELAY);
    }*/
    
    dev->hal->gpio_write(dev->im_ports[BN008X_CS_PIN],0);
    int32_t result = dev->hal->spi_receive(dev->spi, header, 4, BN008X_SPI_TIMEOUT_MS);

    /*if (result != 0) {
        if (dev->i2c_mutex) xSemaphoreGive(dev->i2c_mutex);
        return BN008X_ERROR_I2C;
    }*/
    
    uint16_t packet_len = header[0] | (header[1] << 8);
    packet_len &= 0x7FFFU;
    if (packet_len > BN008X_MAX_PACKET_SIZE) {
        //if (dev->i2c_mutex) xSemaphoreGive(dev->i2c_mutex);
        dev->hal->gpio_write(dev->im_ports[BN008X_CS_PIN],1);
        return BN008X_ERROR_INVALID_PARAM;
    }
    
    buffer[0] = header[0];
    buffer[1] = header[1];
    buffer[2] = header[2];
    buffer[3] = header[3];
    if (packet_len > 4) {
        result = dev->hal->spi_receive(dev->spi, &buffer[4], packet_len - 4, BN008X_SPI_TIMEOUT_MS);
        dev->hal->gpio_write(dev->im_ports[BN008X_CS_PIN],1);
        if (result != 0) {
            //if (dev->i2c_mutex) xSemaphoreGive(dev->i2c_mutex);
            return BN008X_ERROR_SPI;
        }
    }
    
    /*if (dev->i2c_mutex) {
        xSemaphoreGive(dev->i2c_mutex);
    }*/
    
    *len = packet_len;
    return BN008X_OK;
}
/**
 * @brief Отправка простой команды (только Report ID).
 *
 * Формирует двухбайтовый массив: {report_id, 0x00} и вызывает send_shtp_packet.
 *
 * @param[in] dev       Указатель на контекст драйвера
 * @param[in] channel   SHTP-канал
 * @param[in] report_id Report ID команды
 *
 * @return Статус выполнения (пробрасывает статус send_shtp_packet)
 */
static bn008x_status_t send_simple_command(bn008x_t *dev, uint8_t channel, uint8_t report_id) {
    uint8_t data[2] = {report_id};
    return send_shtp_packet(dev, channel, data, 2);
}
/**
 * @brief Пробуждение датчика через PS0/WAKE пин.
 *
 * Если INT уже активен (LOW), сразу возвращает успех.
 * Иначе дёргает WAKE пином и ждёт, пока INT не станет LOW.
 *
 * @param[in] dev Указатель на контекст драйвера
 *
 * @retval BN008X_OK          Датчик проснулся
 * @retval BN008X_ERROR_TIMEOUT Таймаут ожидания INT
 */
static bn008x_status_t bn008x_Wake(bn008x_t *dev)
{
  uint32_t start_ms;

  if (dev->hal->gpio_read(dev->im_ports[BN008X_INT_PIN]) == GPIO_PIN_RESET)
  {
    return BN008X_OK;
  }

  dev->hal->gpio_write(dev->im_ports[BN008X_WAKE_PIN], 0);
  dev->hal->delay_ms(BN008X_RESET_PULSE_DELAY_MS);
  dev->hal->gpio_write(dev->im_ports[BN008X_WAKE_PIN], 1);

  start_ms = dev->hal->get_tick_ms();
  while (dev->hal->gpio_read(dev->im_ports[BN008X_INT_PIN]) == GPIO_PIN_SET)
  {
    if ((dev->hal->get_tick_ms() - start_ms) > BN008X_WAKE_DELAY_MS)
    {
      return BN008X_ERROR_TIMEOUT;
    }
  }
  return BN008X_OK;
}

bn008x_status_t bn008x_init(bn008x_t *dev, const bn008x_hal_t *hal, SPI_HandleTypeDef* spi, uint8_t num_wake, uint8_t num_int, uint8_t num_cs, uint8_t num_ps1, uint8_t num_reset) {
    if (!dev || !hal) {
        return BN008X_ERROR_INVALID_PARAM;
    }

    // Сохраняем данные в структуре
    dev->hal = hal;
    dev->mutex = NULL;
    dev->sem_data_ready = NULL;
    dev->initialized = 0;
    dev->im_ports[BN008X_WAKE_PIN]=num_wake;
    dev->im_ports[BN008X_INT_PIN]=num_int;
    dev->im_ports[BN008X_CS_PIN]=num_cs;
    dev->im_ports[BN008X_PS1_PIN]=num_ps1;
    dev->im_ports[BN008X_RESET_PIN]=num_reset;
    dev->spi=spi;
    // Очищаем кэш
    memset(&dev->tx_seq, 0, sizeof(dev->tx_seq));
    memset(&dev->cache, 0, sizeof(dev->cache));
    
    bn008x_status_t status;
    uint8_t buffer[BN008X_MAX_PACKET_SIZE]={};
    uint16_t len;
    status = bn008x_hardreset(dev);
    if(status!=BN008X_OK){
    	return status;
    }
    status = bn008x_reset(dev);
    if(status!=BN008X_OK){
    	return status;
    }
    
	for(int i=0; i<BN008X_NUM_ATTEMPTS; i++){
		status = bn008x_Wake(dev);
		if (status != BN008X_OK) continue;

		status = send_simple_command(dev, BN008X_CHANNEL_CONTROL, BN008X_RID_PRODUCT_ID_REQUEST);
		if (status != BN008X_OK) continue;

		uint32_t start = dev->hal->get_tick_ms();
		while (dev->hal->gpio_read(dev->im_ports[BN008X_INT_PIN]) == GPIO_PIN_SET) {
			if (dev->hal->get_tick_ms() - start > BN008X_INT_TIMEOUT_MS) {
				break;
			}
		}

		uint8_t drain_count = 0;
		while (drain_count < BNO08X_RX_DRAIN_MAX_PACKETS) {
			bn008x_status_t read_status = read_response(dev, buffer, &len);
			if (read_status == BN008X_OK && len > 0) {
				uint8_t channel = buffer[2];
				uint8_t report_id = buffer[4];
				if (channel == BN008X_CHANNEL_CONTROL && report_id == BN008X_RID_PRODUCT_ID_RESPONSE) {
					dev->initialized = 1;
					return BN008X_OK;
				}
				dev->hal->delay_ms(1);//Важная задержка! Без нее не успевает подняться CS
			} else if (read_status != BN008X_OK) {
				break;
			}
			drain_count++;
			if (dev->hal->gpio_read(dev->im_ports[BN008X_INT_PIN]) != GPIO_PIN_RESET) break;
		}
		dev->hal->delay_ms(100);
	}
	return BN008X_ERROR_NOT_INITIALIZED;
}

bn008x_status_t bn008x_SetProtocolSPI(bn008x_t *dev)
{
	if(!dev || !dev->hal){
		return BN008X_ERROR_INVALID_PARAM;
	}
	dev->hal->gpio_write(dev->im_ports[BN008X_PS1_PIN], 1);
	dev->hal->gpio_write(dev->im_ports[BN008X_WAKE_PIN], 1);
	return BN008X_OK;
}

bn008x_status_t bn008x_reset(bn008x_t *dev) {
    if (!dev || !dev->hal) {
        return BN008X_ERROR_INVALID_PARAM;
    }
    
    bn008x_status_t status;
    
    // Пытаемся сделать обычный ресет
    uint8_t exec_cmd[1] = {BN008X_EXEC_CMD_RESET};  
    status = send_shtp_packet(dev, BN008X_CHANNEL_EXECUTABLE, exec_cmd, 1);
    
    memset(&dev->tx_seq, 0, sizeof(dev->tx_seq));
    return status;
}

bn008x_status_t bn008x_hardreset(bn008x_t *dev){
		if (!dev || !dev->hal) {
	        return BN008X_ERROR_INVALID_PARAM;
	    }
	  uint32_t start_ms;

	  bn008x_SetProtocolSPI(dev);

	  dev->hal->gpio_write(dev->im_ports[BN008X_RESET_PIN],GPIO_PIN_RESET);
	  dev->hal->delay_ms(BN008X_RESET_PULSE_DELAY_MS);
	  dev->hal->gpio_write(dev->im_ports[BN008X_RESET_PIN],GPIO_PIN_SET);

	  start_ms = dev->hal->get_tick_ms();
	  while (dev->hal->gpio_read(dev->im_ports[BN008X_INT_PIN])==GPIO_PIN_SET)
	  {
		if ((dev->hal->get_tick_ms() - start_ms) > BN008X_RESET_DELAY_MS)
		{
		  return BN008X_ERROR_TIMEOUT;
		}
	  }

	  return BN008X_OK;
}
