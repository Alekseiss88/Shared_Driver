#ifndef BN008X_TYPES_H
#define BN008X_TYPES_H

#include <stdint.h>

// Статусы возврата
typedef enum {
    BN008X_OK = 0,
    BN008X_ERROR,
    BN008X_ERROR_TIMEOUT,
    BN008X_ERROR_INVALID_PARAM,
    BN008X_ERROR_NOT_INITIALIZED,
    BN008X_ERROR_SPI
} bn008x_status_t;

// Режимы работы
typedef enum {
    BN008X_MODE_SLEEP = 0,
    BN008X_MODE_STANDBY,
    BN008X_MODE_NORMAL,
    BN008X_MODE_HIGH_PERFORMANCE
} bn008x_mode_t;

// Данные гироскопа
typedef struct {
    float x;
    float y;
    float z;
} bn008x_gyro_t;

// Данные акселерометра
typedef struct {
    float x;
    float y;
    float z;
} bn008x_accel_t;

// Кватернион
typedef struct {
    float q0;  // w
    float q1;  // x
    float q2;  // y
    float q3;  // z
} bn008x_quaternion_t;

// Температура
typedef struct {
    float temp;
} bn008x_temp_t;

typedef enum {
    /* Акселерометры и гравитация */
    BN008X_SENSOR_RAW_ACCELEROMETER = 0x14,        /**< Сырой акселерометр */
    BN008X_SENSOR_ACCELEROMETER = 0x01,            /**< Калиброванный акселерометр */
    BN008X_SENSOR_LINEAR_ACCELERATION = 0x04,      /**< Линейное ускорение (без гравитации) */
    BN008X_SENSOR_GRAVITY = 0x06,                   /**< Вектор гравитации */
    
    /* Гироскопы */
    BN008X_SENSOR_RAW_GYROSCOPE = 0x15,             /**< Сырой гироскоп */
    BN008X_SENSOR_GYROSCOPE_CALIBRATED = 0x02,      /**< Калиброванный гироскоп */
    BN008X_SENSOR_GYROSCOPE_UNCALIBRATED = 0x07,    /**< Некалиброванный гироскоп */
    
    /* Магнитометры */
    BN008X_SENSOR_RAW_MAGNETOMETER = 0x16,          /**< Сырой магнитометр */
    BN008X_SENSOR_MAGNETIC_FIELD_CALIBRATED = 0x03, /**< Калиброванное магнитное поле */
    BN008X_SENSOR_MAGNETIC_FIELD_UNCALIBRATED = 0x0F, /**< Некалиброванное магнитное поле */
    
    /* Векторы поворота (кватернионы) */
    BN008X_SENSOR_ROTATION_VECTOR = 0x05,           /**< Rotation vector (кватернион) */
    BN008X_SENSOR_GAME_ROTATION_VECTOR = 0x08,      /**< Game rotation vector */
    BN008X_SENSOR_GEOMAGNETIC_ROTATION_VECTOR = 0x09, /**< Geomagnetic rotation vector */
    BN008X_SENSOR_ARVR_STABILIZED_RV = 0x28,        /**< AR/VR stabilized rotation vector */
    BN008X_SENSOR_ARVR_STABILIZED_GAME_RV = 0x29,   /**< AR/VR stabilized game rotation vector */
    BN008X_SENSOR_GYRO_INTEGRATED_RV = 0x2A,        /**< Gyro-integrated rotation vector */
    
    /* Окружающая среда */
    BN008X_SENSOR_PRESSURE = 0x0A,                   /**< Давление */
    BN008X_SENSOR_AMBIENT_LIGHT = 0x0B,              /**< Освещённость */
    BN008X_SENSOR_HUMIDITY = 0x0C,                    /**< Влажность */
    BN008X_SENSOR_PROXIMITY = 0x0D,                   /**< Приближение */
    BN008X_SENSOR_TEMPERATURE = 0x0E,                 /**< Температура */
    
    /* Детекторы событий */
    BN008X_SENSOR_TAP_DETECTOR = 0x10,                /**< Детектор касания */
    BN008X_SENSOR_STEP_DETECTOR = 0x18,               /**< Детектор шага */
    BN008X_SENSOR_STEP_COUNTER = 0x11,                /**< Счётчик шагов */
    BN008X_SENSOR_SIGNIFICANT_MOTION = 0x12,          /**< Значительное движение */
    BN008X_SENSOR_STABILITY_CLASSIFIER = 0x13,        /**< Классификатор стабильности */
    BN008X_SENSOR_SHAKE_DETECTOR = 0x19,              /**< Детектор встряхивания */
    BN008X_SENSOR_FLIP_DETECTOR = 0x1A,               /**< Детектор переворота */
    BN008X_SENSOR_PICKUP_DETECTOR = 0x1B,             /**< Детектор подъёма */
    BN008X_SENSOR_STABILITY_DETECTOR = 0x1C,          /**< Детектор стабильности */
    BN008X_SENSOR_PERSONAL_ACTIVITY_CLASSIFIER = 0x1E, /**< Классификатор активности */
    BN008X_SENSOR_SLEEP_DETECTOR = 0x1F,              /**< Детектор сна */
    BN008X_SENSOR_TILT_DETECTOR = 0x20,                /**< Детектор наклона */
    BN008X_SENSOR_POCKET_DETECTOR = 0x21,              /**< Детектор кармана */
    BN008X_SENSOR_CIRCLE_DETECTOR = 0x22,              /**< Детектор кругового движения */
    BN008X_SENSOR_HEART_RATE_MONITOR = 0x23            /**< Монитор сердечного ритма */
} bn008x_sensor_t;
#endif
