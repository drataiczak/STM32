#ifndef __ADXL_H_
#define __ADXL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

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

#define DEVID_RESET         0b11100101
#define BW_RATE_RESET       0b00001010
#define INT_SOURCE_RESET    0b00000010

#define BW_3_125HZ  0b0110
#define BW_6_25HZ   0b0111
#define BW_12_5HZ   0b1000
#define BW_25HZ     0b1001
#define BW_50HZ     0b1010
#define BW_100HZ    0b1011
#define BW_200HZ    0b1100
#define BW_400HZ    0b1101
#define BW_800HZ    0b1110
#define BW_1600HZ   0b1111

#define MULTIBYTE_EN 0x40
#define READ_OP 0x80
#define MEAS_BIT 0x8

#define ACC_2G_SCALER (0.0039)
#define ACC_4G_SCALER (0.0078)
#define ACC_8G_SCALER (0.0156)
#define ACC_16G_SCALER (0.312)

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
    GPIO_TypeDef *port;
    uint16_t pin;
    SPI_HandleTypeDef *spi;
    accel_t accel;
    float xstOn;
    float xstOff;
    float ystOn;
    float ystOff;
    float zstOn;
    float zstOff;
    float xst;
    float yst;
    float zst;
} adxl_t;

adxl_t ADXL_Init(GPIO_TypeDef *csPort, uint16_t csPin, SPI_HandleTypeDef *hspi, accel_t accel);
void ADXL_Write(uint8_t address, uint8_t value);
void ADXL_Read(uint8_t address, uint8_t *buf, size_t size);
void ADXL_SelfTest();
float ADXL_ApplyCalibOffset(axis_t axis, float val);

#ifdef __cplusplus
}
#endif

#endif