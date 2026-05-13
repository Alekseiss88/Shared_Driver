/**
 * @file bn008x_regs.h
 * @brief Регистры, константы и идентификаторы протокола SHTP для BNO08x
 * @author Aleksei
 * @date 2025
 * @version 1.0
 *
 * @details Этот файл содержит все внутренние константы драйвера:
 *          - I2C адрес (не используется в SPI, оставлен для совместимости)
 *          - SHTP-каналы
 *          - Report ID для команд и ответов
 *          - Идентификаторы команд Command Request
 *          - Команды bootloader
 *          - Размеры данных сенсоров
 *          - Константы таймаутов и задержек
 *
 * @warning Данный файл является внутренним для драйвера и не должен
 *          изменяться пользователем.
 *
 * @see SH-2 Reference Manual
 */
#ifndef BN008X_REGS_H
#define BN008X_REGS_H

/**
 * @brief I2C адрес датчика (рабочий режим).
 * @note Не используется в SPI-версии драйвера, оставлен для совместимости.
 */
#define BN008X_I2C_ADDR         0x4A

/**
 * @name SHTP-каналы
 * @{
 */
#define BN008X_CHANNEL_COMMAND      0   /**< SHTP command channel (резерв) */
#define BN008X_CHANNEL_EXECUTABLE   1   /**< Executable channel (Soft Reset) */
#define BN008X_CHANNEL_CONTROL      2   /**< Sensor hub control channel (команды) */
#define BN008X_CHANNEL_DATA         3   /**< Input sensor reports (данные сенсоров) */
#define BN008X_CHANNEL_WAKE         4   /**< Wake input sensor reports (для wake-up сенсоров) */
#define BN008X_CHANNEL_GYRO_RV      5   /**< Gyro rotation vector (специальный канал) */
/** @} */

/**
 * @name Report IDs для control channel (канал 2)
 * @{
 */
#define BN008X_RID_COMMAND_RESPONSE     0xF1  /**< Ответ на Command Request */
#define BN008X_RID_COMMAND_REQUEST      0xF2  /**< Запрос команды */
#define BN008X_RID_FRS_READ_RESPONSE    0xF3  /**< Ответ на чтение FRS */
#define BN008X_RID_FRS_READ_REQUEST     0xF4  /**< Запрос чтения FRS */
#define BN008X_RID_FRS_WRITE_RESPONSE   0xF5  /**< Ответ на запись в FRS */
#define BN008X_RID_FRS_WRITE_DATA       0xF6  /**< Данные для записи в FRS */
#define BN008X_RID_FRS_WRITE_REQUEST    0xF7  /**< Запрос записи в FRS */
#define BN008X_RID_PRODUCT_ID_RESPONSE  0xF8  /**< Ответ с Product ID */
#define BN008X_RID_PRODUCT_ID_REQUEST   0xF9  /**< Запрос Product ID */
#define BN008X_RID_GET_FEATURE_RESPONSE 0xFC  /**< Ответ Get Feature */
#define BN008X_RID_SET_FEATURE_COMMAND  0xFD  /**< Команда Set Feature (включение сенсоров) */
#define BN008X_RID_GET_FEATURE_REQUEST  0xFE  /**< Запрос Get Feature */
/** @} */

/**
 * @name Идентификаторы команд (Command Request 0xF2)
 * @{
 */
#define BN008X_CMD_REPORT_ERRORS        0x01    /**< Чтение очереди ошибок */
#define BN008X_CMD_COUNTER              0x02    /**< Команды счётчиков */
#define BN008X_CMD_TARE                 0x03    /**< Обнуление ориентации (tare) */
#define BN008X_CMD_INITIALIZATION       0x04    /**< Реинициализация сенсорного хаба */
#define BN008X_CMD_SAVE_DCD             0x06    /**< Сохранение Dynamic Calibration Data */
#define BN008X_CMD_ME_CALIBRATION       0x07    /**< Калибровка ME */
#define BN008X_CMD_CONFIGURE_PERIODIC_DCD_SAVE 0x09 /**< Автосохранение DCD */
#define BN008X_CMD_GET_OSCILLATOR_TYPE  0x0A    /**< Определение типа осциллятора */
#define BN008X_CMD_CLEAR_DCD_AND_RESET  0x0B    /**< Сброс DCD и перезагрузка */
#define BN008X_CMD_SIMPLE_CALIBRATION   0x0C    /**< Простая калибровка (180°) */
#define BN008X_CMD_BOOTLOADER           0x0D    /**< Команды загрузчика */
#define BN008X_CMD_INTERACTIVE_CALIBRATION 0x0E /**< Интерактивная калибровка */
#define BN008X_CMD_WHEEL_REQUEST        0x0F    /**< Данные от энкодеров колёс */
/** @} */

/**
 * @name Режимы работы bootloader (команда 0x0D)
 * @{
 */
#define BN008X_BOOT_MODE_RESET          0x00    /**< Переход в bootloader */
#define BN008X_BOOT_MODE_UPGRADE        0x01    /**< Обновление прошивки */
#define BN008X_BOOT_MODE_VALIDATE       0x02    /**< Проверка образа */
#define BN008X_BOOT_MODE_LAUNCH         0x03    /**< Запуск приложения */

/**
 * @name Размеры данных (в байтах) для парсинга отчётов
 * @{
 */
#define BN008X_ACCEL_DATA_SIZE          6   /**< Акселерометр: 3 оси × 2 байта */
#define BN008X_GYRO_DATA_SIZE           6   /**< Гироскоп: 3 оси × 2 байта */
#define BN008X_MAG_DATA_SIZE            6   /**< Магнитометр: 3 оси × 2 байта */
#define BN008X_QUATERNION_DATA_SIZE     8   /**< Кватернион: 4 компоненты × 2 байта */
#define BN008X_TEMPERATURE_DATA_SIZE    2   /**< Температура: 2 байта */
/** @} */

/**
 * @name Таймауты и задержки
 * @{
 */
#define BN008X_MAX_PACKET_SIZE          512     /**< Максимальный размер SHTP-пакета */
#define BN008X_RESET_DELAY_MS           200     /**< Таймаут после аппаратного сброса (мс) */
#define BN008X_SPI_TIMEOUT_MS           20      /**< Таймаут SPI-операции (мс) */
#define BN008X_INT_TIMEOUT_MS           20      /**< Таймаут ожидания INT (мс) */
#define BN008X_WAKE_DELAY_MS            20      /**< Таймаут пробуждения (мс) */
#define BN008X_RESET_PULSE_DELAY_MS     2       /**< Длительность импульса RESET (мс) */
#define BNO08X_RX_DRAIN_MAX_PACKETS     8       /**< Максимум пакетов за один цикл чтения */
#define BN008X_NUM_ATTEMPTS             3       /**< Количество попыток инициализации */
/** @} */
/**
 * @name Прочие константы
 * @{
 */
#define BN008X_EXEC_CMD_RESET           0x01    /**< Команда Soft Reset для канала Executable */
/** @} */
#endif
