/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "dma.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include <stdio.h>

#ifdef __GNUC__
    #define _PUTCHAR int __io_putchar(int ch)
#else
    #define _PUTCHAR int fputc(int ch, FILE *f)
#endif

#define ADC_SAMPLES 10
#define ADC_CHANNELS 2
#define ADC_RESOLUTION 4095

_PUTCHAR {
  HAL_UART_Transmit(&huart2, (uint8_t *) &ch, 1, HAL_MAX_DELAY);
  return 1;
}

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void processADCBuf(uint16_t *buffer);
void calibrateTempSensor();

typedef struct {
  float vdda;
  float vref;
  float temp;
  uint16_t vrefAvg;
  uint16_t tempAvg;
} temp_t;

float ta;
float tb;
temp_t tempVal;
uint16_t adcBuffer[ADC_SAMPLES * ADC_CHANNELS * 2] = {0};

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_DMA_Init();
  MX_GPIO_Init();
  MX_ADC1_Init();
  MX_TIM3_Init();
  MX_USART2_UART_Init();
  
  calibrateTempSensor();
  HAL_TIM_Base_Start_IT(&htim3);
  HAL_ADC_Start_DMA(&hadc1, (uint32_t *) adcBuffer, ADC_SAMPLES * ADC_CHANNELS * 2);

  uint32_t now = 0;
  uint32_t then = 0;

  while (1)
  {
    now = HAL_GetTick();
    if(now - then == 1000) {
      printf("(Temp,vRef,tempAvg,vRevAfg,vdda)=(%03.2f,%01.2f,%d,%d,%01.2f)\r\n", tempVal.temp, tempVal.vref, tempVal.tempAvg, tempVal.vrefAvg, tempVal.vdda);
      then = now;
    }
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

void calibrateTempSensor() {
  float x1 = (float) *TEMPSENSOR_CAL1_ADDR;
  float x2 = (float) *TEMPSENSOR_CAL2_ADDR;
  float y1 = (float) TEMPSENSOR_CAL1_TEMP;
  float y2 = (float) TEMPSENSOR_CAL2_TEMP;

  ta = (float) ((y2 - y1) / (x2 - x1));
  tb = (float) ((x2 * y1 - x1 * y2) / (x2 - x1));
}

void processADCBuf(uint16_t *buffer) {
  uint32_t sum1 = 0;
  uint32_t sum2 = 0;

  for(int i = 0; i < ADC_SAMPLES; i++) {
    sum1 += buffer[i * 2];
    sum2 += buffer[1 + i * 2];
  }

  tempVal.tempAvg = sum1 / ADC_SAMPLES;
  tempVal.vrefAvg = sum2 / ADC_SAMPLES;
  tempVal.vdda = (float) VREFINT_CAL_VREF * (float) *VREFINT_CAL_ADDR / tempVal.vrefAvg / 1000;
  tempVal.vref = (float) tempVal.vdda / ADC_RESOLUTION * tempVal.vrefAvg;

  tempVal.temp = (float) ta * tempVal.tempAvg + tb;
}

void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef *hadc) {
  if(hadc->Instance == ADC1) {
    processADCBuf(&adcBuffer[0]);
  }
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc) {
  if(hadc->Instance == ADC1) {
    processADCBuf(&adcBuffer[ADC_CHANNELS * ADC_SAMPLES]);
  }
}
  
/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 96;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
