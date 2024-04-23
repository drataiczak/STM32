#ifndef __ADXL_H_
#define __ADXL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f4xx_hal.h"

#define I2C_ADDR (0x1D << 1)
#define I2C_ADDR_ALT 0x53
#define I2C_WRITE 0x3A
#define I2C_READ 0x3B
#define I2C_WRITE_AL 0xA6
#define I2C_READ_ALT 0xA7

#define DEVID_ADDR          0x00
#define THRESH_TAP_ADDR     0x1D
#define OFSX_ADDR           0x1E
#define OFSY_ADDR           0x1F
#define OFSZ_ADDR           0x20
#define DUR_ADDR            0x21
#define LATENT_ADDR         0x22
#define WINDOW_ADDR         0x23
#define THRESH_ACT_ADDR     0x24
#define THRESH_INACT_ADDR   0x25
#define TIME_INACT_ADDR     0x26
#define ACT_INACT_CTL_ADDR  0x27
#define THRESH_FF_ADDR      0x28
#define TIME_FF_ADDR        0x29
#define TAP_AXES_ADDR       0x2A
#define ACT_TAP_STATUS_ADDR 0x2B
#define BW_RATE_ADDR        0x2C
#define POWER_CTL_ADDR      0x2D
#define INT_ENABLE_ADDR     0x2E
#define INT_MAP_ADDR        0x2F
#define INT_SOURCE_ADDR     0x30
#define DATA_FORMAT_ADDR    0x31
#define DATAX0_ADDR         0x32
#define DATAX1_ADDR         0x33
#define DATAY0_ADDR         0x34
#define DATAY1_ADDR         0x35
#define DATAZ0_ADDR         0x36
#define DATAZ1_ADDR         0x37
#define FIFO_CTL_ADDR       0x38
#define FIFO_STATUS_ADDR    0x39

#define DEVID_RESET         0xE5
#define BW_RATE_RESET       0x0A
#define INT_SOURCE_RESET    0x02

#define BW_3_125HZ  0x06
#define BW_6_25HZ   0x07
#define BW_12_5HZ   0x08
#define BW_25HZ     0x09
#define BW_50HZ     0x0A
#define BW_100HZ    0x0B
#define BW_200HZ    0x0C
#define BW_400HZ    0x0D
#define BW_800HZ    0x0E
#define BW_1600HZ   0x0F

#define MULTIBYTE_EN 0x40
#define READ_OP 0x80
#define MEAS_BIT 0x8

#define ACC_2G_SCALER ((float) 1/256)
#define ACC_4G_SCALER ((float) 1/128)
#define ACC_8G_SCALER ((float) 1/64)
#define ACC_16G_SCALER ((float) 1/32)

#define RESOLUTION_10B 1024
#define RESOLUTION_11B 2048
#define RESOLUTION_12B 4096
#define RESOLUTION_13B 8192

#define ST_COUNT 10
#define ST_SETTLE_COUNT 4

typedef enum Accel {
    A2G = 0,
    A4G = 1,
    A8G = 2,
    A16G = 3
} accel_t;

typedef enum Axis {
    X = 0,
    Y = 1,
    Z = 2
} axis_t;

typedef struct ADXL {
    uint8_t devid;
    I2C_HandleTypeDef *hi2c;
    accel_t accel;
    float accelData[3];
    float scaler;
} adxl_t;

uint8_t ADXL_Init(adxl_t *device, I2C_HandleTypeDef *hi2c, accel_t accel);
HAL_StatusTypeDef ADXL_Write(adxl_t *dev, uint8_t reg, uint8_t value);
HAL_StatusTypeDef ADXL_ReadMulti(adxl_t *dev, uint8_t reg, uint8_t *buf, size_t size);
HAL_StatusTypeDef ADXL_ReadSingle(adxl_t *dev, uint8_t reg, uint8_t *buf);
HAL_StatusTypeDef ADXL_ReadAccel(adxl_t *dev);
#ifdef __cplusplus
}
#endif

#endif