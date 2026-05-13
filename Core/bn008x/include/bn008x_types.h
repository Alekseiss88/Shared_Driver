/**
 * @file bn008x_types.h
 * @brief Публичные типы данных, перечисления и структуры для драйвера BNO08x
 * @author Aleksei
 * @date 2025
 * @version 1.0
 *
 * @details Этот файл содержит все публичные типы данных, которые используются
 *          в API драйвера BNO08x: статусы возврата, абстрактные номера пинов,
 *          режимы работы, структуры данных сенсоров и идентификаторы сенсоров.
 *
 * @note Все типы и перечисления доступны пользователю драйвера.
 */

#ifndef BN008X_TYPES_H
#define BN008X_TYPES_H

#include <stdint.h>

/**
 * @brief Коды возврата функций драйвера.
 */
typedef enum {
	BN008X_OK = 0,                  /**< Успешное выполнение операции */
	BN008X_ERROR,                   /**< Общая ошибка */
	BN008X_ERROR_TIMEOUT,           /**< Превышено время ожидания */
	BN008X_ERROR_INVALID_PARAM,     /**< Неверный параметр (NULL, вне диапазона) */
	BN008X_ERROR_NOT_INITIALIZED,   /**< Драйвер не инициализирован */
	BN008X_ERROR_SPI                /**< Ошибка SPI (таймаут, NACK и т.д.) */
} bn008x_status_t;

/**
 * @brief Абстрактные идентификаторы GPIO-пинов BNO08x.
 *
 * Используются для обращения к пинам через HAL-абстракцию.
 * Пользователь должен сопоставить эти номера с реальными пинами
 * своей платформы в реализации порта.
 */
typedef enum {
	BN008X_RESET_PIN = 0,   /**< Пин RESET (аппаратный сброс) */
	BN008X_WAKE_PIN,        /**< Пин WAKE (PS0, пробуждение) */
	BN008X_INT_PIN,         /**< Пин INT (прерывание, данные готовы) */
	BN008X_PS1_PIN,         /**< Пин PS1 (выбор интерфейса SPI) */
	BN008X_CS_PIN           /**< Пин CS (Chip Select, выбор SPI-устройства) */
} bn008x_gpio_pin_t;

/**
 * @brief Режимы питания и производительности BNO08x.
 */
typedef enum {
	BN008X_MODE_SLEEP = 0,          /**< Режим сна (минимальное потребление) */
	BN008X_MODE_STANDBY,            /**< Дежурный режим */
	BN008X_MODE_NORMAL,             /**< Обычный режим */
	BN008X_MODE_HIGH_PERFORMANCE    /**< Высокая производительность */
} bn008x_mode_t;

/**
 * @brief Данные гироскопа (3 оси).
 */
typedef struct {
	float x;    /**< Значение по оси X (рад/с или °/с) */
	float y;    /**< Значение по оси Y (рад/с или °/с) */
	float z;    /**< Значение по оси Z (рад/с или °/с) */
} bn008x_gyro_t;

/**
 * @brief Данные акселерометра (3 оси).
 */
typedef struct {
	float x;    /**< Значение по оси X (м/с² или g) */
	float y;    /**< Значение по оси Y (м/с² или g) */
	float z;    /**< Значение по оси Z (м/с² или g) */
} bn008x_accel_t;

/**
 * @brief Кватернион (ориентация в пространстве).
 *
 * Представляет собой нормализованный кватернион: q0² + q1² + q2² + q3² = 1.
 */
typedef struct {
	float q0;   /**< Скалярная компонента (w) */
	float q1;   /**< Векторная компонента (x) */
	float q2;   /**< Векторная компонента (y) */
	float q3;   /**< Векторная компонента (z) */
} bn008x_quaternion_t;

/**
 * @brief Температура датчика.
 */
typedef struct {
    float temp;  /**< Температура в градусах Цельсия */
} bn008x_temp_t;

/**
 * @brief Идентификаторы сенсоров для включения через Set Feature Command.
 *
 * Значения соответствуют Report ID из SH-2 Reference Manual.
 */
typedef enum {
    /* ========== Акселерометры и гравитация ========== */
    BN008X_SENSOR_RAW_ACCELEROMETER = 0x14,      /**< Сырой акселерометр */
    BN008X_SENSOR_ACCELEROMETER = 0x01,          /**< Калиброванный акселерометр */
    BN008X_SENSOR_LINEAR_ACCELERATION = 0x04,    /**< Линейное ускорение (без гравитации) */
    BN008X_SENSOR_GRAVITY = 0x06,                /**< Вектор гравитации */

    /* ========== Гироскопы ========== */
    BN008X_SENSOR_RAW_GYROSCOPE = 0x15,          /**< Сырой гироскоп */
    BN008X_SENSOR_GYROSCOPE_CALIBRATED = 0x02,   /**< Калиброванный гироскоп */
    BN008X_SENSOR_GYROSCOPE_UNCALIBRATED = 0x07, /**< Некалиброванный гироскоп */

    /* ========== Магнитометры ========== */
    BN008X_SENSOR_RAW_MAGNETOMETER = 0x16,       /**< Сырой магнитометр */
    BN008X_SENSOR_MAGNETIC_FIELD_CALIBRATED = 0x03,   /**< Калиброванное магнитное поле */
    BN008X_SENSOR_MAGNETIC_FIELD_UNCALIBRATED = 0x0F, /**< Некалиброванное магнитное поле */

    /* ========== Векторы поворота (кватернионы) ========== */
    BN008X_SENSOR_ROTATION_VECTOR = 0x05,        /**< Rotation vector (основной кватернион) */
    BN008X_SENSOR_GAME_ROTATION_VECTOR = 0x08,   /**< Game rotation vector */
    BN008X_SENSOR_GEOMAGNETIC_ROTATION_VECTOR = 0x09, /**< Geomagnetic rotation vector */
    BN008X_SENSOR_ARVR_STABILIZED_RV = 0x28,     /**< AR/VR stabilized rotation vector */
    BN008X_SENSOR_ARVR_STABILIZED_GAME_RV = 0x29,/**< AR/VR stabilized game rotation vector */
    BN008X_SENSOR_GYRO_INTEGRATED_RV = 0x2A,     /**< Gyro-integrated rotation vector */

    /* ========== Окружающая среда ========== */
    BN008X_SENSOR_PRESSURE = 0x0A,               /**< Давление (гПа) */
    BN008X_SENSOR_AMBIENT_LIGHT = 0x0B,          /**< Освещённость (люкс) */
    BN008X_SENSOR_HUMIDITY = 0x0C,               /**< Влажность (% RH) */
    BN008X_SENSOR_PROXIMITY = 0x0D,              /**< Приближение (безразмерное) */
    BN008X_SENSOR_TEMPERATURE = 0x0E,            /**< Температура (°C) */

    /* ========== Детекторы событий ========== */
    BN008X_SENSOR_TAP_DETECTOR = 0x10,           /**< Детектор касания */
    BN008X_SENSOR_STEP_DETECTOR = 0x18,          /**< Детектор шага */
    BN008X_SENSOR_STEP_COUNTER = 0x11,           /**< Счётчик шагов */
    BN008X_SENSOR_SIGNIFICANT_MOTION = 0x12,     /**< Значительное движение */
    BN008X_SENSOR_STABILITY_CLASSIFIER = 0x13,   /**< Классификатор стабильности */
    BN008X_SENSOR_SHAKE_DETECTOR = 0x19,         /**< Детектор встряхивания */
    BN008X_SENSOR_FLIP_DETECTOR = 0x1A,          /**< Детектор переворота */
    BN008X_SENSOR_PICKUP_DETECTOR = 0x1B,        /**< Детектор подъёма */
    BN008X_SENSOR_STABILITY_DETECTOR = 0x1C,     /**< Детектор стабильности */
    BN008X_SENSOR_PERSONAL_ACTIVITY_CLASSIFIER = 0x1E, /**< Классификатор активности */
    BN008X_SENSOR_SLEEP_DETECTOR = 0x1F,         /**< Детектор сна */
    BN008X_SENSOR_TILT_DETECTOR = 0x20,          /**< Детектор наклона */
    BN008X_SENSOR_POCKET_DETECTOR = 0x21,        /**< Детектор кармана */
    BN008X_SENSOR_CIRCLE_DETECTOR = 0x22,        /**< Детектор кругового движения */
    BN008X_SENSOR_HEART_RATE_MONITOR = 0x23      /**< Монитор сердечного ритма */
} bn008x_sensor_t;

#ifdef __cplusplus
}
#endif

#endif
