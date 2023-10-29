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
#include "lptim.h"
#include "spi.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "ZE27_O3.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

volatile uint8_t aRXBufferUser[ZE27_RX_BUFFER_SIZE];

uint8_t receivedFlag = 1;
uint32_t PM2_5;
uint32_t PM1;
uint32_t PM10;
uint16_t OzonePPB;
uint8_t extBattery;
float temp;
float humidity;
uint8_t PM_measure_flag = 1;
volatile int counter = 0;
uint8_t pmSensStatus = 0x00;

const uint8_t SHT40_cmd = 0xFD;
const uint8_t SHT40_addr = 0x44;
volatile uint8_t SHT40_dataRX[6] = {0,0,0,0,0,0};
uint16_t temp_hword; // teporarly temperature half word
uint16_t th_hword;   // teporarly humidy half  word

const uint8_t J5_SSP_addr = 0x33;
const uint8_t J5_SSP_cmd_status = 0x26;
volatile uint8_t J5_SSP_dataRX[12]= {0,0,0,0,0,0,0,0,0,0,0,0};

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void MeasurePM_sens(void);
void MeasureOzone(void);
/* USER CODE END 0 */

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
  MX_GPIO_Init();
  MX_LoRaWAN_Init();
  MX_USART2_UART_Init();
  MX_I2C2_Init();
  MX_SPI2_Init();
  MX_LPTIM1_Init();
  /* USER CODE BEGIN 2 */

  F1_QueueIni(); // init Function queue
  F2_QueueIni();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_RESET);
  //HAL_TIM_Base_Start_IT(&htim16);
  HAL_LPTIM_TimeOut_Start_IT(&hlptim1,  256, 0);


  while (1)
  {

    /* USER CODE END WHILE */
    MX_LoRaWAN_Process();

    /* USER CODE BEGIN 3 */

    F1_pull()();
    F2_pull()();

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
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_SET);
}

void DisablePM_sens(void){
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_RESET);
}

void MeasurePM_sens(void){
	HAL_I2C_Mem_Read(&hi2c2, J5_SSP_addr << 1, 0x00, 1, J5_SSP_dataRX, 12, 1000);
	HAL_I2C_Mem_Read(&hi2c2, J5_SSP_addr << 1, 0x26, 1, &pmSensStatus, 1, 1000);
 if(!pmSensStatus){
	PM1 = J5_SSP_dataRX[0] + (J5_SSP_dataRX[1] << 8) + (J5_SSP_dataRX[2] << 16) +  (J5_SSP_dataRX[3] << 24);
	PM2_5 = J5_SSP_dataRX[4] + (J5_SSP_dataRX[5] << 8) + (J5_SSP_dataRX[6] << 16) +  (J5_SSP_dataRX[7] << 24);
	PM10 = J5_SSP_dataRX[8] + (J5_SSP_dataRX[9] << 8) + (J5_SSP_dataRX[10] << 16) +  (J5_SSP_dataRX[11] << 24);
 }
}
void MeasureOzone(void){
	uint16_t RxLen;
	HAL_UART_Receive_IT(&huart2, (uint8_t *) aRXBufferUser, ZE27_RX_BUFFER_SIZE);
	HAL_UARTEx_ReceiveToIdle(&huart2, (uint8_t *) aRXBufferUser, ZE27_RX_BUFFER_SIZE, &RxLen, 1000);
	OzonePPB = ZE27_parsePPB(aRXBufferUser);
}

void MeasureTempHum(void){

    HAL_I2C_Master_Receive(&hi2c2, (uint16_t)(SHT40_addr << 1),SHT40_dataRX, 6, 100);
    temp_hword = SHT40_dataRX[0] * 256 + SHT40_dataRX[1];
    th_hword = SHT40_dataRX[3] * 256 + SHT40_dataRX[4];
    temp  = -45.0 + 175.0 * (float)temp_hword/(float)65535.0;
    humidity = -6.0 + 125.0 * (float)th_hword/(float)65535.0;
    HAL_I2C_Master_Transmit(&hi2c2, (uint16_t)(SHT40_addr << 1),(uint8_t*)&SHT40_cmd, 1, 100);
}

 void HAL_LPTIM_CompareMatchCallback(LPTIM_HandleTypeDef *hlptim)
{
	static char firstStart = 1;
   if (hlptim == &hlptim1)
   {

      switch(counter){
      case 10:
    	  if(firstStart)
    		  F2_push(EnablePM_sens);
    	  break;
      case 40:
    	  if(firstStart){
    		  F2_push(MeasureTempHum);
    		  F1_push(MeasurePM_sens);
    		  F1_push(MeasureOzone);
    		  F2_push(DisablePM_sens);
    		  firstStart = 0;
    	  }
    	  break;
      case 559:
    	  F2_push(EnablePM_sens);

    	  break;
      case 595:
    	  F2_push(MeasureTempHum);
    	  break;
      case 596:
    	  F2_push(MeasureTempHum);
    	  F1_push(MeasureOzone);
    	  break;

      case 597:
    	  F1_push(MeasurePM_sens);
    	  break;
      case 599:
    	  F2_push(DisablePM_sens);
      }
      counter++;
      if(counter % 10 == 8)HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_RESET);///DBG
      if(counter % 10 == 9)HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_SET);///DBG
      counter %= 600;
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
