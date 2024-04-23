#include "adxl.h"

void _ADXL_SetScaler(adxl_t *device) {
    switch(device->accel) {
        case A2G:
            device->scaler = ACC_2G_SCALER;
            break;
        case A4G:
            device->scaler = ACC_4G_SCALER;
            break;
        case A8G:
            device->scaler = ACC_8G_SCALER;
            break;
        case A16G:
            device->scaler = ACC_16G_SCALER;
            break;
        default:
            device->scaler = ACC_4G_SCALER;
            break;
    }
}

HAL_StatusTypeDef ADXL_Write(adxl_t *dev, uint8_t reg, uint8_t value) {
    HAL_StatusTypeDef status = HAL_OK;

    status = HAL_I2C_Mem_Write(dev->hi2c, I2C_ADDR, reg, I2C_MEMADD_SIZE_8BIT, &value, 1, HAL_MAX_DELAY);
    return status;
}

HAL_StatusTypeDef ADXL_ReadSingle(adxl_t *dev, uint8_t reg, uint8_t *buf) {
    HAL_StatusTypeDef status = HAL_OK;
    
    status = HAL_I2C_Mem_Read(dev->hi2c, I2C_ADDR, reg, I2C_MEMADD_SIZE_8BIT, buf, 1, HAL_MAX_DELAY);

    return status;
}

HAL_StatusTypeDef ADXL_ReadMulti(adxl_t *dev, uint8_t reg, uint8_t *buf, size_t size) {
    HAL_StatusTypeDef status = HAL_OK;

    status = HAL_I2C_Mem_Read(dev->hi2c, I2C_ADDR, reg, I2C_MEMADD_SIZE_8BIT, buf, size, HAL_MAX_DELAY);

    return status;
}

uint8_t ADXL_Init(adxl_t *device, I2C_HandleTypeDef *hi2c, accel_t accel) {
    HAL_StatusTypeDef status = HAL_OK;
    uint8_t errStatus = 0;

    device->hi2c = hi2c;
    device->accelData[0] = 0.0f;
    device->accelData[1] = 0.0f;
    device->accelData[2] = 0.0f;
    device->accel = accel;

    _ADXL_SetScaler(device);

    // Read device ID
    status = ADXL_ReadSingle(device, DEVID_ADDR, &device->devid);
    errStatus += (status != HAL_OK);

    status = ADXL_Write(device, POWER_CTL_ADDR, 0);
    errStatus += (status != HAL_OK);

    status = ADXL_Write(device, POWER_CTL_ADDR, 0x08); // Sets measure bit
    errStatus += (status != HAL_OK);

    status = ADXL_Write(device, DATA_FORMAT_ADDR, device->accel);
    errStatus += (status != HAL_OK);

    return errStatus;
}

HAL_StatusTypeDef ADXL_ReadAccel(adxl_t *dev) {
    HAL_StatusTypeDef status = HAL_OK;
    uint8_t buf[6] = {0};
    int16_t rawAccel[3] = {0};

    status = ADXL_ReadMulti(dev, DATAX0_ADDR, buf, sizeof(buf));
    if(status == HAL_OK) {
        rawAccel[0] = buf[0] | buf[1] << 8;
        rawAccel[1] = buf[2] | buf[3] << 8;
        rawAccel[2] = buf[4] | buf[5] << 8;

        dev->accelData[0] = rawAccel[0] * dev->scaler;
        dev->accelData[1] = rawAccel[1] * dev->scaler;
        dev->accelData[2] = rawAccel[2] * dev->scaler;
    }

    return status;
}