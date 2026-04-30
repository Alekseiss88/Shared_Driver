#ifndef BN008X_REGS_H
#define BN008X_REGS_H

// I2C адрес
#define BN008X_I2C_ADDR         0x4A

//Каналы
#define BN008X_CHANNEL_COMMAND      0   /**< SHTP command channel */
#define BN008X_CHANNEL_EXECUTABLE   1   /**< Executable channel */
#define BN008X_CHANNEL_CONTROL      2   /**< Sensor hub control channel */
#define BN008X_CHANNEL_DATA         3   /**< Input sensor reports (non-wake) */
#define BN008X_CHANNEL_WAKE         4   /**< Wake input sensor reports */
#define BN008X_CHANNEL_GYRO_RV      5   /**< Gyro rotation vector */

// Идентификаторы отчётов
// Report IDs для control channel (channel 2)
#define BN008X_RID_COMMAND_RESPONSE     0xF1  // BNO - host
#define BN008X_RID_COMMAND_REQUEST      0xF2  // host -BNO
#define BN008X_RID_FRS_READ_RESPONSE    0xF3  // BNO - host
#define BN008X_RID_FRS_READ_REQUEST     0xF4  // host - BNO
#define BN008X_RID_FRS_WRITE_RESPONSE   0xF5  // BNO - host
#define BN008X_RID_FRS_WRITE_DATA       0xF6  // host - BNO
#define BN008X_RID_FRS_WRITE_REQUEST    0xF7  // host - BNO
#define BN008X_RID_PRODUCT_ID_RESPONSE  0xF8  // BNO - host
#define BN008X_RID_PRODUCT_ID_REQUEST   0xF9  // host - BNO
#define BN008X_RID_GET_FEATURE_RESPONSE 0xFC  // BNO - host
#define BN008X_RID_SET_FEATURE_COMMAND  0xFD  // host - BNO
#define BN008X_RID_GET_FEATURE_REQUEST  0xFE  // host - BNO

// Идентификаторы команд
#define BN008X_CMD_REPORT_ERRORS        0x01    /**< Report errors from queue */
#define BN008X_CMD_COUNTER              0x02    /**< Counter commands */
#define BN008X_CMD_TARE                 0x03    /**< Tare operations */
#define BN008X_CMD_INITIALIZATION       0x04    /**< Reinitialize sensor hub */
#define BN008X_CMD_SAVE_DCD             0x06    /**< Save Dynamic Calibration Data */
#define BN008X_CMD_ME_CALIBRATION       0x07    /**< ME Calibration commands */
#define BN008X_CMD_CONFIGURE_PERIODIC_DCD_SAVE 0x09 /**< Configure periodic DCD save */
#define BN008X_CMD_GET_OSCILLATOR_TYPE  0x0A    /**< Get oscillator type */
#define BN008X_CMD_CLEAR_DCD_AND_RESET  0x0B    /**< Clear DCD and reset */
#define BN008X_CMD_SIMPLE_CALIBRATION   0x0C    /**< Simple calibration */
#define BN008X_CMD_BOOTLOADER           0x0D    /**< Bootloader commands */
#define BN008X_CMD_INTERACTIVE_CALIBRATION 0x0E /**< Interactive calibration */
#define BN008X_CMD_WHEEL_REQUEST        0x0F    /**< Wheel encoder data */

//Команды bootloader
#define BN008X_BOOT_MODE_RESET          0x00    /**< Reset to bootloader mode */
#define BN008X_BOOT_MODE_UPGRADE        0x01    /**< Upgrade application */
#define BN008X_BOOT_MODE_VALIDATE       0x02    /**< Validate application image */
#define BN008X_BOOT_MODE_LAUNCH         0x03    /**< Launch application */

// Размеры данных
#define BN008X_ACCEL_DATA_SIZE          6   /**< Акселерометр: 3 оси × 2 байта */
#define BN008X_GYRO_DATA_SIZE           6   /**< Гироскоп: 3 оси × 2 байта */
#define BN008X_MAG_DATA_SIZE            6   /**< Магнитометр: 3 оси × 2 байта */
#define BN008X_QUATERNION_DATA_SIZE     8   /**< Кватернион: 4 компоненты × 2 байта */
#define BN008X_TEMPERATURE_DATA_SIZE    2   /**< Температура: 2 байта */
#define BN008X_MAX_PACKET_SIZE          256

#define BN008X_MAX_PACKET_SIZE          256     /**< Максимальный размер пакета */
#define BN008X_RESET_DELAY_MS           3     /**< Задержка после сброса (мс) */
#define BN008X_SPI_TIMEOUT_MS           10     /**< Таймаут I2C по умолчанию (мс) */
#define BN008X_MAX_SENSORS              50      /**< Максимальное количество сенсоров */
#define BN008X_DEFAULT_SAMPLE_RATE_HZ   100     /**< Частота опроса по умолчанию (Гц) */

#define BN008X_EXEC_CMD_RESET           0x01
#endif
