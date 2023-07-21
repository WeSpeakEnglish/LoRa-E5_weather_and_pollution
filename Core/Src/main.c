/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2021 STMicroelectronics.
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
#include "i2c.h"
#include "app_lorawan.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "string.h"
#include "NRF24L01.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define RX_BUFFER_SIZE 32
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

uint8_t aRXBufferUser[RX_BUFFER_SIZE];

uint8_t mainBuffer[RX_BUFFER_SIZE];

uint8_t receivedFlag = 1;
uint32_t PM2_5;
float temp;
float humidity;
char PM_measure_flag = 1;
static int counter = 0;

extern DMA_HandleTypeDef hdma_usart2_rx;

uint8_t RxAddress[] = {0x00,0xDD,0xCC,0xBB,0xAA}; //ADDR receiving
uint8_t TxAddress[] = {0xEE,0xDD,0xCC,0xBB,0xAA}; //ADDR to which transmitting
uint8_t TxData[] = "Hello World\n";

uint8_t RxData[32];
uint8_t data[50];

uint8_t SHT40_cmd = 0xFD;
uint8_t SHT40_dataRX[6];
uint16_t temp_hword; // teporarly temperature half word
uint16_t th_hword;   // teporarly humidy half  word


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void MeasurePM_sens(void);
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
//	static uint16_t count = 1;

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
  MX_GPIO_Init();
  MX_LoRaWAN_Init();
  MX_USART2_UART_Init();
  MX_I2C2_Init();
  MX_TIM16_Init();
  MX_TIM17_Init();
  MX_SPI2_Init();
  /* USER CODE BEGIN 2 */

  UART2_SET =0;

  NRF24_Init();

  NRF24_RxMode(RxAddress, 10);


   NRF24_ReadAll(data);

  F1_QueueIni(); // init Function queue

  HAL_UART_Receive_IT(&huart2, aRXBufferUser, RX_BUFFER_SIZE);


  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_RESET);

  HAL_TIM_Base_Start_IT(&htim16);


  while (1)
  {
	HAL_I2C_Master_Transmit(&hi2c2, (uint16_t)(0x44 << 1),(uint8_t*)&SHT40_cmd, 1, 100);


    /* USER CODE END WHILE */
    MX_LoRaWAN_Process();

    /* USER CODE BEGIN 3 */


    F1_pull()();


    HAL_I2C_Master_Receive(&hi2c2, (uint16_t)(0x44 << 1),SHT40_dataRX, 6, 100);
    temp_hword = SHT40_dataRX[0] * 256 + SHT40_dataRX[1];
    th_hword = SHT40_dataRX[3] * 256 + SHT40_dataRX[4];
    temp  = -45.0 + 175.0 * (float)temp_hword/(float)65535.0;
    humidity = -6.0 + 125.0 * (float)th_hword/(float)65535.0;
    if(UART2_SET)MeasurePM_sens();
    if (isDataAvailable(2) == 1)
   	  {
 //  		  NRF24_Receive(RxData);
//   		  HAL_UART_Transmit(&huart2, RxData, strlen((char *)RxData), 1000);
   	  }

    HAL_GPIO_TogglePin(LED2_GPIO_Port, LED2_Pin);///DBG

   //
  }


  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure LSE Drive Capability
  */
  HAL_PWR_EnableBkUpAccess();
  __HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_LOW);

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_HSE
                              |RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS_PWR;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.HSEDiv = RCC_HSE_DIV1;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure the SYSCLKSource, HCLK, PCLK1 and PCLK2 clocks dividers
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK3|RCC_CLOCKTYPE_HCLK
                              |RCC_CLOCKTYPE_SYSCLK|RCC_CLOCKTYPE_PCLK1
                              |RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSE;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.AHBCLK3Divider = RCC_SYSCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */



void EnablePM_sens(void){
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_RESET);

}

void DisablePM_sens(void){
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_SET);


}

void MeasurePM_sens(void){

	       HAL_UART_Receive_IT(&huart2, aRXBufferUser, RX_BUFFER_SIZE);


}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
   if (htim == &htim16)
   {
	  counter++;
      switch(counter){
      case 820:
    	  F1_push(EnablePM_sens);

    	  break;
      case 5:
     	  PM_measure_flag = 1;
    	//  F1_push(MeasurePM_sens);
    	  break;
      case 10:
    	//  F1_push(DisablePM_sens);
    	  PM_measure_flag = 0;
      }

      counter %= 20;
   }
}




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
