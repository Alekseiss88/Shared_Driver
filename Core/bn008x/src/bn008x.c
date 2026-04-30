#include "bn008x.h"
#include <string.h>
#include "stdio.h"
static bn008x_status_t send_shtp_packet(bn008x_t *dev, uint8_t channel, 
                                         const uint8_t *data, uint16_t data_len) {
    if (!dev || !dev->hal || !dev->hal->spi_transmit) {
        return BN008X_ERROR_INVALID_PARAM;
    }
    
    // Проверяем длину
    if (data_len > BN008X_MAX_PACKET_SIZE - 4) {
        return BN008X_ERROR_INVALID_PARAM;  // Слишком много данных
    }
    
    // Выделяем буфер для полного пакета (заголовок + данные)
    uint16_t total_len = data_len + 4;
    uint8_t tx_buffer[total_len];
    // Формируем SHTP заголовок (4 байта)
    tx_buffer[0] = total_len & 0xFF;          // Length LSB
    tx_buffer[1] = (total_len >> 8) & 0xFF;    // Length MSB
    tx_buffer[2] = channel;                     // Channel
    tx_buffer[3] = dev->tx_seq[channel]++;             // Sequence number (увеличиваем)
    
    // Копируем данные после заголовка
    for (uint16_t i = 0; i < data_len; i++) {
        tx_buffer[4 + i] = data[i];
    }
    
    // Захватываем мьютекс 
    /*if (dev->i2c_mutex) {
        xSemaphoreTake(dev->i2c_mutex, portMAX_DELAY);
    }*/
    
    // Отправляем по SPI
    dev->hal->gpio_write(dev->im_ports[BN008X_CS_PIN],0);
    uint32_t result = dev->hal->spi_transmit(dev->spi, tx_buffer, total_len, BN008X_SPI_TIMEOUT_MS);
    dev->hal->gpio_write(dev->im_ports[BN008X_CS_PIN],1);
    // Отдаём мьютекс
    /*if (dev->i2c_mutex) {
        xSemaphoreGive(dev->i2c_mutex);
    }*/
    
    return (result == 0) ? BN008X_OK : BN008X_ERROR_SPI;
}

static bn008x_status_t read_response(bn008x_t *dev, uint8_t *buffer, uint16_t *len) {
    uint8_t header[4];
    /*if (dev->i2c_mutex) {
        xSemaphoreTake(dev->i2c_mutex, portMAX_DELAY);
    }*/
    
    // Читаем заголовок shtp
    dev->hal->gpio_write(dev->im_ports[BN008X_CS_PIN],0);
    int32_t result = dev->hal->spi_receive(dev->spi, header, 4, BN008X_SPI_TIMEOUT_MS);
    dev->hal->gpio_write(dev->im_ports[BN008X_CS_PIN],1);
    /*if (result != 0) {
        if (dev->i2c_mutex) xSemaphoreGive(dev->i2c_mutex);
        return BN008X_ERROR_I2C;
    }*/
    
    uint16_t packet_len = header[0] | (header[1] << 8);
    
    if (packet_len > BN008X_MAX_PACKET_SIZE) {
        //if (dev->i2c_mutex) xSemaphoreGive(dev->i2c_mutex);
        return BN008X_ERROR_INVALID_PARAM;
    }
    
    // Составляем полный пакет
    buffer[0] = header[0];
    buffer[1] = header[1];
    buffer[2] = header[2];
    buffer[3] = header[3];
    if (packet_len > 4) {
        result = dev->hal->spi_receive(dev->spi, &buffer[4], packet_len - 4, BN008X_SPI_TIMEOUT_MS);
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

static bn008x_status_t send_simple_command(bn008x_t *dev, uint8_t channel, uint8_t report_id) {
    uint8_t data[2] = {report_id};
    return send_shtp_packet(dev, channel, data, 2);
}

/*static bn008x_status_t send_command_request(bn008x_t *dev, uint8_t cmd_id,
                                             const uint8_t *params, uint8_t params_len) {
    uint8_t data[2 + params_len];
    
    data[0] = BN008X_RID_COMMAND_REQUEST;  
    data[1] = cmd_id;                        
    
    for (uint8_t i = 0; i < params_len; i++) {
        data[2 + i] = params[i];
    }
    
    return send_shtp_packet(dev, BN008X_CHANNEL_CONTROL, data, sizeof(data));
}*/

static bn008x_status_t bn008x_Wake(bn008x_t *dev)
{
  uint32_t start_ms;

  if (dev->hal->gpio_read(dev->im_ports[BN008X_INT_PIN]) == GPIO_PIN_RESET)
  {
    return BN008X_OK;
  }

  dev->hal->gpio_write(dev->im_ports[BN008X_WAKE_PIN], 0);
  dev->hal->delay_ms(BN008X_RESET_DELAY_MS);
  dev->hal->gpio_write(dev->im_ports[BN008X_WAKE_PIN], 1);

  start_ms = dev->hal->get_tick_ms();
  while (dev->hal->gpio_read(dev->im_ports[BN008X_INT_PIN]) == GPIO_PIN_SET)
  {
    if ((dev->hal->get_tick_ms() - start_ms) > BN008X_RESET_DELAY_MS)
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
    uint8_t buffer[BN008X_MAX_PACKET_SIZE];
    uint16_t len;
    status = bn008x_hardreset(dev);
    if(status!=BN008X_OK){
    	return status;
    }
    status = bn008x_reset(dev);
    if(status!=BN008X_OK){
    	return status;
    }
    status = bn008x_Wake(dev);
    if(status!=BN008X_OK){
    	return status;
    }
    // Пробуем прочитать IDRequest
    status = send_simple_command(dev, BN008X_CHANNEL_CONTROL, BN008X_RID_PRODUCT_ID_REQUEST);
    if (status != BN008X_OK) {
        return status;
    }
    
    // Читаем ответ
    uint32_t ms = dev->hal->get_tick_ms();
    while(dev->hal->gpio_read(dev->im_ports[BN008X_INT_PIN])==GPIO_PIN_SET){
    	if(dev->hal->get_tick_ms()-ms>BN008X_SPI_TIMEOUT_MS){
    		return BN008X_ERROR_TIMEOUT;
    	}
    }
    uint8_t drain_count=0;
    uint8_t ready=1;
    for(int i=0; i<3; i++){
    	if(ready==1 || dev->hal->gpio_read(dev->im_ports[BN008X_INT_PIN])==GPIO_PIN_RESET){
    		ready=0;
			while (drain_count < BNO08X_RX_DRAIN_MAX_PACKETS)
				  {
					bn008x_status_t read_status = read_response(dev, buffer, &len);
					if(read_status == BN008X_OK){
						if(len>0){
							uint8_t channel = buffer[2];
							uint8_t report_id = buffer[4];
							if (channel == BN008X_CHANNEL_CONTROL || report_id == BN008X_RID_PRODUCT_ID_RESPONSE) {
									return BN008X_OK;
							}
						}
					}
					drain_count++;
					if(dev->hal->gpio_read(dev->im_ports[BN008X_INT_PIN])==GPIO_PIN_RESET){
						break;
					}
				  }
    	}
    	if(dev->hal->gpio_read(dev->im_ports[BN008X_INT_PIN])==GPIO_PIN_RESET){
    		ready=1;
    	}
    	dev->hal->delay_ms(1);
    }
    
    // Сброс и инициализация
    dev->initialized = 1;
    return BN008X_OK;
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
    
    // Если вдруг не сработало, запускаем инициализацию
    /*if (status != BN008X_OK) {
        uint8_t init_cmd[8] = {
            BN008X_RID_COMMAND_REQUEST,  
            dev->tx_seq[2]++,
            BN008X_CMD_INITIALIZATION,     
            0x01 //all sensor hub
        };
        status = send_shtp_packet(dev, BN008X_CHANNEL_CONTROL, init_cmd, 8);
    }
    
    if (status != BN008X_OK) {
        return status;  // Оба варианта не сработали
    }
    
    // Ждем некоторое время
    dev->hal->delay_ms(BN008X_RESET_DELAY_MS);
    
    // Очистка буфера
    uint8_t buffer[32];
    uint16_t len;
    (void)read_response(dev, buffer, &len);  
    
    // Сбрасывем флаг (опционально)
    //dev->initialized = 0;
    memset(&dev->tx_seq, 0, sizeof(dev->tx_seq));  // Сбрасываем счётчик
    */
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
	  HAL_Delay(BN008X_RESET_DELAY_MS);
	  dev->hal->gpio_write(dev->im_ports[BN008X_RESET_PIN],GPIO_PIN_SET);

	  start_ms = dev->hal->get_tick_ms();
	  while (dev->hal->gpio_read(dev->im_ports[BN008X_INT_PIN])==GPIO_PIN_SET)
	  {
		if ((dev->hal->get_tick_ms() - start_ms) > BN008X_RESET_DELAY_MS)
		{
		  return BN008X_ERROR_TIMEOUT;
		}
	  }

	  /* Keep PS0/WAKE high through first H_INTN assertion to lock SPI mode. */
	  return BN008X_OK;
}
