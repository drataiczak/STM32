#include "main.h"
#include "adc.h"
#include "dma.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

#define ADC_SAMPLES 10
#define ADC_CHANNELS 2
#define ADC_RES 4095
#define ADC_INTREF 1.21

#ifdef __GNUC__
    #define _PUTCHAR int __io_putchar(int ch)
#else
    #define _PUTCHAR int fputc(int ch, FILE *f)
#endif

_PUTCHAR {
  HAL_UART_Transmit(&huart2, (uint8_t *) &ch, 1, HAL_MAX_DELAY);
}

typedef struct TempPair {
  float temp;
  float vref;
} temp_t;

/* Create dma buffer */
uint16_t buf[ADC_SAMPLES * ADC_CHANNELS * 2] = {0};
temp_t temp;

void SystemClock_Config(void);
temp_t procAdcBuf(uint16_t *buf);

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();

  /* Initialize all configured peripherals */
  MX_DMA_Init();
  MX_GPIO_Init();
  MX_ADC1_Init();
  MX_USART2_UART_Init();
  MX_TIM3_Init();

  HAL_TIM_Base_Start_IT(&htim3);
  HAL_ADC_Start_DMA(&hadc1, (uint32_t *) buf, sizeof(buf));

  uint32_t curr = 0;
  uint32_t prev = 0;
  while (1)
  {
    curr = HAL_GetTick();
    if(curr % 1000 == 0 && curr != prev) {
      printf("Temperature: %f; Vref: %f\r\n", temp.temp, temp.vref);
      prev = curr;
    }
  }

}

temp_t procAdcBuf(uint16_t *buf) {
  uint32_t sum1 = 0;
  uint32_t sum2 = 0;
  float tempCalc = 0.0;
  float vrefCalc = 0.0;
  float vrefVal = 0.0;
  float tempVal = 0.0;
  float tempValCalc = 0.0;

  for(int i = 0; i < ADC_SAMPLES; i++) {
    sum1 += buf[i * 2]; // 0, 2, 4, 6, 8, ...
    sum2 += buf[1 + i * 2]; // 1, 3, 5, 7, 9, ...
    printf("Found temp %d with vref %d\r\n", buf[i * 2], buf[1 + i * 2]);
  }

  // Temp = ((Vsense - V25) / avgSlope) + 25
  vrefCalc = (float) (sum2 / ADC_SAMPLES);
  tempCalc = (float) ((((sum1 / ADC_SAMPLES - 0.76) / 2.5) + 25));

  vrefVal = ADC_RES * ADC_INTREF / vrefCalc;
  tempVal = tempCalc * vrefVal / ADC_RES;

  tempValCalc =  ((tempVal - 0.76) / 2.5) + 25;
  printf("Calculated temp, vref over %d samples: %03.1f, %03.1f\r\n", ADC_SAMPLES, tempValCalc, vrefVal);

  temp.temp = tempCalc;
  temp.vref = vrefCalc;

  return temp;
}

void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef *hadc) {
  procAdcBuf(&buf[0]);
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc) {
  procAdcBuf(&buf[ADC_SAMPLES * 2]);
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

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  __disable_irq();
  while (1)
  {
  }
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

}
#endif /* USE_FULL_ASSERT */
