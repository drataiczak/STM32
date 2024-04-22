#include "adxl.h"

adxl_t adxl = {
    .port = NULL, .pin = 0, .spi = NULL
};

void ADXL_Write(uint8_t address, uint8_t value) {
    uint8_t data[2];

    if(adxl.port == NULL || adxl.pin == 0 || adxl.spi == NULL) {
        return;
    }

    data[0] = address | 0x40;
    data[1] = value;

    HAL_GPIO_WritePin(adxl.port, adxl.pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(adxl.spi, data, 2, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(adxl.port, adxl.pin, GPIO_PIN_SET);
}

void ADXL_Read(uint8_t address, uint8_t *buf, size_t size) {
    address |= 0x80;
    address |= 0x40;

    HAL_GPIO_WritePin(adxl.port, adxl.pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(adxl.spi, &address, 1, HAL_MAX_DELAY);
    HAL_SPI_Receive(adxl.spi, buf, size, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(adxl.port, adxl.pin, GPIO_PIN_SET);

    
}

adxl_t ADXL_Init(GPIO_TypeDef *csPort, uint16_t csPin, SPI_HandleTypeDef *hspi) {
    adxl.port = csPort;
    adxl.pin = csPin;
    adxl.spi = hspi;

    ADXL_Write(0x31, 0x01);
    ADXL_Write(0x2D, 0x00);
    ADXL_Write(0x2D, 0x08);

    return adxl;
}