#include "adxl.h"

adxl_t adxl = {
    .port = NULL, 
    .pin = 0, 
    .spi = NULL,
    .accel = A4G,
    .xstOn = 0.0,
    .xstOff = 0.0,
    .ystOn = 0.0,
    .ystOff = 0.0,
    .zstOn = 0.0,
    .zstOff = 0.0,
    .xst = 0.0,
    .yst = 0.0,
    .zst = 0.0
};

void ADXL_Write(uint8_t address, uint8_t value) {
    uint8_t data[2];

    if(adxl.port == NULL || adxl.pin == 0 || adxl.spi == NULL) {
        return;
    }

    data[0] = address | MULTIBYTE_EN;
    data[1] = value;

    HAL_GPIO_WritePin(adxl.port, adxl.pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(adxl.spi, data, sizeof(data), HAL_MAX_DELAY);
    HAL_GPIO_WritePin(adxl.port, adxl.pin, GPIO_PIN_SET);
}

void ADXL_Read(uint8_t address, uint8_t *buf, size_t size) {
    address |= READ_OP;
    address |= MULTIBYTE_EN;

    HAL_GPIO_WritePin(adxl.port, adxl.pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(adxl.spi, &address, 1, HAL_MAX_DELAY);
    HAL_SPI_Receive(adxl.spi, buf, size, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(adxl.port, adxl.pin, GPIO_PIN_SET);
}

adxl_t ADXL_Init(GPIO_TypeDef *csPort, uint16_t csPin, SPI_HandleTypeDef *hspi, accel_t accel) {
    adxl.port = csPort;
    adxl.pin = csPin;
    adxl.spi = hspi;
    adxl.accel = accel;

    ADXL_Write(DATA_FORMAT_ADDR, adxl.accel | MEAS_BIT);
    ADXL_Write(BW_RATE_ADDR, BW_1600HZ);
    ADXL_Write(POWER_CTL_ADDR, 0x08);

    return adxl;
}

void ADXL_SelfTest() {
    if(adxl.port == NULL || adxl.pin == 0 || adxl.spi == NULL) {
        return;
    }

    float avgX = 0.0;
    float avgY = 0.0;
    float avgZ = 0.0;
    uint8_t buf[6] = {0};

    // Read 10 ST off
    for(int i = 0; i < ST_COUNT; i++) {
        ADXL_Read(DATAX0_ADDR, buf, sizeof(buf));
        avgX += buf[0] | (buf[1] << 8);
        avgY += buf[2] | (buf[3] << 8);
        avgZ += buf[4] | (buf[5] << 8);
    }

    adxl.xstOff = avgX / ST_COUNT;
    adxl.ystOff = avgY / ST_COUNT;
    adxl.zstOff = avgZ / ST_COUNT;

    avgX = 0.0;
    avgY = 0.0;
    avgZ = 0.0;

    ADXL_Write(BW_RATE_ADDR, BW_1600HZ); // Set BW_RATE > 100Hz
    ADXL_Write(DATA_FORMAT_ADDR, 0b00001011); // Set range > 0x03
    ADXL_Write(DATA_FORMAT_ADDR, 0b10000000); // Set ST on

    // Read 10 ST on
    for(int j = 0; j < ST_COUNT + ST_SETTLE_COUNT; j++) {
        ADXL_Read(DATAX0_ADDR, buf, sizeof(buf));
        
        if(j < ST_SETTLE_COUNT) {
            continue;
        }
        
        avgX += buf[0] | buf[1] << 8;
        avgY += buf[2] | buf[3] << 8;
        avgZ += buf[4] | buf[5] << 8;
    }

    adxl.xstOn = avgX / ST_COUNT;
    adxl.ystOn = avgY / ST_COUNT;
    adxl.zstOn = avgZ / ST_COUNT;

    adxl.xst = adxl.xstOn - adxl.xstOff;
    adxl.yst = adxl.ystOn - adxl.ystOff;
    adxl.zst = adxl.zstOn - adxl.zstOff;
}