#ifndef __ADXL_H_
#define __ADXL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

typedef struct ADXL {
    GPIO_TypeDef *port;
    uint16_t pin;
    SPI_HandleTypeDef *spi;
} adxl_t;

adxl_t ADXL_Init(GPIO_TypeDef *csPort, uint16_t csPin, SPI_HandleTypeDef *hspi);
void ADXL_Write(uint8_t address, uint8_t value);
void ADXL_Read(uint8_t address, uint8_t *buf, size_t size);

#ifdef __cplusplus
}
#endif

#endif