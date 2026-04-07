#include "bn008x.h"
#include <string.h>
#include "stdio.h"
static bn008x_status_t send_shtp_packet(bn008x_t *dev, uint8_t channel, 
                                         const uint8_t *data, uint16_t data_len) {
    if (!dev || !dev->hal || !dev->hal->spi_transfer) {
        return BN008X_ERROR_INVALID_PARAM;
    }
    
    // Проверяем длину
    if (data_len > BN008X_MAX_PACKET_SIZE - 4) {
        return BN008X_ERROR_INVALID_PARAM;  // Слишком много данных
    }
    
    // Выделяем буфер для полного пакета (заголовок + данные)
    uint16_t total_len = data_len + 4;
    uint8_t tx_buffer[total_len];
    uint8_t rx_buffer[total_len];
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
    dev->hal->cs_select(1);
    uint32_t result = dev->hal->spi_transfer(tx_buffer, rx_buffer, total_len, BN008X_SPI_TIMEOUT_MS);
    dev->hal->cs_select(0);
    // Отдаём мьютекс
    /*if (dev->i2c_mutex) {
        xSemaphoreGive(dev->i2c_mutex);
    }*/
    
    return (result == 0) ? BN008X_OK : BN008X_ERROR_SPI;
}

static bn008x_status_t read_response(bn008x_t *dev, uint8_t *buffer, uint16_t *len) {
    uint8_t header[4];
    uint8_t mock[4];
    memset(mock, 0xFF, 4);
    /*if (dev->i2c_mutex) {
        xSemaphoreTake(dev->i2c_mutex, portMAX_DELAY);
    }*/
    
    // Читаем заголовок shtp
    dev->hal->cs_select(1);
    int32_t result = dev->hal->spi_transfer(mock, header, 4, BN008X_SPI_TIMEOUT_MS);
    dev->hal->cs_select(0);
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
    	uint8_t mock[packet_len-4];
    	memset(mock, 0xFF, packet_len-4);
        result = dev->hal->spi_transfer(mock, &buffer[4], packet_len - 4, BN008X_SPI_TIMEOUT_MS);
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

static bn008x_status_t send_command_request(bn008x_t *dev, uint8_t cmd_id, 
                                             const uint8_t *params, uint8_t params_len) {
    uint8_t data[2 + params_len];
    
    data[0] = BN008X_RID_COMMAND_REQUEST;  
    data[1] = cmd_id;                        
    
    for (uint8_t i = 0; i < params_len; i++) {
        data[2 + i] = params[i];
    }
    
    return send_shtp_packet(dev, BN008X_CHANNEL_CONTROL, data, sizeof(data));
}

bn008x_status_t bn008x_init(bn008x_t *dev, const bn008x_hal_t *hal, uint8_t dev_addr) {
    if (!dev || !hal) {
        return BN008X_ERROR_INVALID_PARAM;
    }

    // Сохраняем данные в структуре
    dev->hal = hal;
    dev->dev_addr = dev_addr;
    dev->mutex = NULL;
    dev->sem_data_ready = NULL;
    dev->initialized = 0;
    
    // Очищаем кэш
    memset(&dev->tx_seq, 0, sizeof(dev->tx_seq));
    memset(&dev->cache, 0, sizeof(dev->cache));
    
    bn008x_status_t status;
    uint8_t buffer[32];
    uint16_t len;
    
    // Пробуем прочитать IDRequest
    status = send_simple_command(dev, BN008X_CHANNEL_CONTROL, BN008X_RID_PRODUCT_ID_REQUEST);
    if (status != BN008X_OK) {
        return status;
    }
    
    // Ждём немного
    dev->hal->delay_ms(10);
    
    // Читаем ответ
    status = read_response(dev, buffer, &len);
    if (status != BN008X_OK) {
        return status;
    }
    
    // Проверяем, что это правильный ответ
    uint8_t channel = buffer[2];
    uint8_t report_id = buffer[4];
    printf("%i check\n", channel);
    printf("%i check\n", report_id);
    if (channel != BN008X_CHANNEL_CONTROL || report_id != BN008X_RID_PRODUCT_ID_RESPONSE) {
        return BN008X_ERROR;  
    }
    
    // Сброс и инициализация
    dev->initialized = 1;
    return bn008x_reset(dev);
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
    if (status != BN008X_OK) {
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
    
    return BN008X_OK;
}
