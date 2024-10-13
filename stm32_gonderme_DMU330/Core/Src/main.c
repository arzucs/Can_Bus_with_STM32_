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
#include "stdio.h"
#include "stdlib.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
CAN_FilterTypeDef sFilterConfig;
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;

CAN_HandleTypeDef hcan1;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_CAN1_Init(void);
static void MX_ADC1_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
//CAN_TxHeaderTypeDef pTxHeader;
//CAN_RxHeaderTypeDef pRxHeader;

//uint32_t TxData[8];
//uint32_t RxData[8];
//uint32_t TxMailBox;
uint32_t amp1_buff1[8];
uint32_t amp1_buff2[8];

typedef struct {
    CAN_TxHeaderTypeDef pTxHeader; // Mesaj başlığı
    CAN_RxHeaderTypeDef RxHeader;

    uint8_t TxData[8];            // Veri
    uint32_t TxMailbox;           // Mailbox ID
    uint8_t RxData[8];            // Alınan veri
    uint16_t feedback_amp;        // Geri bildirim verisi
} CAN_MessageTypeDef;

CAN_MessageTypeDef amp1;

//CAN_TxHeaderTypeDef pTxHeader; // Mesaj başlığı
//unsigned char TxData[8];      // Veri
//uint32_t TxMailbox;          // Mailbox ID
//uint32_t RxData[8];         // Alınan veri
//uint16_t feedback_amp;     // Geri bildirim verisi

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
  MX_CAN1_Init();
  MX_ADC1_Init();
  /* USER CODE BEGIN 2 */
	HAL_CAN_Start(&hcan1);

    // Activate interrupt
    HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING);
//	  // set filter parameter
  if (HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING) != HAL_OK)
    {
      Error_Handler();
    }
    sFilterConfig.FilterBank = 0;
    sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
    sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
    sFilterConfig.FilterIdHigh = 0x0000;
    sFilterConfig.FilterIdLow = 0x0000 ;
    sFilterConfig.FilterMaskIdHigh =0x0000; // 0xFFF0- 0x0150 dersek yine aynı şeyi yapar
    sFilterConfig.FilterMaskIdLow = 0x0000;
    sFilterConfig.FilterFIFOAssignment = CAN_RX_FIFO0;
    sFilterConfig.FilterActivation = ENABLE;
  //  HAL_CAN_GetRxMessage(&hcan1, CAN_RX_FIFO0 , &pRxHeader, RxData);
    HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING);
    if (HAL_CAN_ConfigFilter(&hcan1, &sFilterConfig) != HAL_OK)
    {
      Error_Handler();
    }

//    amp1.RxHeader.StdId=0x0000;
//    amp1.RxHeader.DLC=1;
//    amp1.RxHeader.IDE=CAN_ID_STD;
//    amp1.RxHeader.RTR=CAN_RTR_DATA;

    amp1.pTxHeader.StdId= 0x0000;// 0x07E1
    amp1.pTxHeader.DLC=8;
    amp1.pTxHeader.IDE=CAN_ID_STD;
    amp1.pTxHeader.RTR = CAN_RTR_DATA;

	HAL_CAN_ConfigFilter(&hcan1, &sFilterConfig);


//////*******************************************************
	/// dmu okuma
//void Read_Adc(){
//    HAL_ADC_Start(&hadc1);  // ADC'yi başlat
//
//    if(HAL_ADC_PollForConversion(&hadc1, 100000) == HAL_OK){
//    	TxData[0] = HAL_ADC_GetValue(&hadc1);
//    	TxData[1] =
//    	HAL_CAN_AddTxMessage(&hcan1, &pTxHeader, TxData, &TxMailBox);
//    }
//    HAL_ADC_Stop(&hadc1);

//////*******************************************************

	void Read_Adc()
	{
	    HAL_ADC_Start(&hadc1);  // ADC'yi başlat

	    if(HAL_ADC_PollForConversion(&hadc1, 100000) == HAL_OK)
	    {
	    	amp1.TxData[0] = HAL_ADC_GetValue(&hadc1);
//	    	HAL_ADC_GetValue(&hadc1);

//	        amp1.TxData[0]=0x48;
	        amp1.TxData[1]=0x00;
	        amp1.TxData[2]=0x21;
	        amp1.TxData[3]=0x01;
	        amp1.TxData[4]=0x20;
	        amp1.TxData[5]=0x12;
	     	amp1.TxData[6]=0x00;
	        amp1.TxData[7]=0x30;
	        HAL_CAN_AddTxMessage(&hcan1, &amp1.pTxHeader, amp1.TxData, &amp1.TxMailbox);
	        HAL_Delay(1);

	        if(amp1.RxData[5] != 0xFF)
	        {
	        sprintf(amp1_buff1, "%d", amp1.RxData[4]);
	        sprintf(amp1_buff2, "%d", amp1.RxData[5]);
	       	amp1.feedback_amp= atoi(amp1_buff2)*256 + atoi(amp1_buff1);  //atoi (ASCII to Integer), bir C kütüphane fonksiyonudur ve bir C string'ini (char dizisini) tamsayı (int) veri türüne dönüştürmek için kullanılır.
	        }else{
	        sprintf(amp1_buff1, "%d", amp1.RxData[4]);
	        sprintf(amp1_buff2, "%d", amp1.RxData[5]);
	        amp1.feedback_amp = 65536 - (atoi(amp1_buff2)*256 + atoi(amp1_buff1));
	        }
	        HAL_Delay(1);

	    }
}
//	    HAL_ADC_Stop(&hadc1);


//    amp1.TxData[0]=0x48;
//    amp1.TxData[1]=0x00;
//    amp1.TxData[2]=0x21;
//    amp1.TxData[3]=0x01;
//    amp1.TxData[4]=0x00;
//    amp1.TxData[5]=0x00;
// 	amp1.TxData[6]=0x00;
//    amp1.TxData[7]=0x00;
//    HAL_CAN_AddTxMessage(&hcan1, &amp1.TxHeader, amp1.TxData, &amp1.TxMailbox);
//    HAL_Delay(1);
//
//    amp2.TxData[0]=0x48;
//    amp2.TxData[1]=0x00;
//    amp2.TxData[2]=0x21;
//    amp2.TxData[3]=0x02;
//    amp2.TxData[4]=0x00;
//   	amp2.TxData[5]=0x00;
//    amp2.TxData[6]=0x00;
//    amp2.TxData[7]=0x00;
//    HAL_CAN_AddTxMessage(&hcan1, &amp2.TxHeader, amp2.TxData, &amp2.TxMailbox);
//
//    if(amp1.RxData[5] != 0xFF)
//    {
//    sprintf(amp1_buff1, "%d", amp1.RxData[4]);
//    sprintf(amp1_buff2, "%d", amp1.RxData[5]);
//   	amp1.feedback_amp= atoi(amp1_buff2)*256 + atoi(amp1_buff1);  //atoi (ASCII to Integer), bir C kütüphane fonksiyonudur ve bir C string'ini (char dizisini) tamsayı (int) veri türüne dönüştürmek için kullanılır.
//    }else{
//    sprintf(amp1_buff1, "%d", amp1.RxData[4]);
//    sprintf(amp1_buff2, "%d", amp1.RxData[5]);
//    amp1.feedback_amp = 65536 - (atoi(amp1_buff2)*256 + atoi(amp1_buff1));
//    }
//    HAL_Delay(1);
//
//    if(amp2.RxData[5] != 0xFF)
//    {
//    sprintf(amp2_buff1, "%d", amp2.RxData[4]);
//    sprintf(amp2_buff2, "%d", amp2.RxData[5]);
//    amp2.feedback_amp = atoi(amp2_buff2)*256 + atoi(amp2_buff1);
//    }else{
//    sprintf(amp2_buff1, "%d", amp2.RxData[4]);
//    sprintf(amp2_buff2, "%d", amp2.RxData[5]);
//    amp2.feedback_amp= 65536 - (atoi(amp2_buff2)*256 + atoi(amp2_buff1));
//    }

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
//	  ADC_val = HAL_ADC_GetValue(&hadc1);
//	  HAL_ADC_PollForConversion(&hadc1, ADC_val); // ADC dönüşümünü
	  Read_Adc();
//	  HAL_ADC_Start(&hadc1);
////	  HAL_ADC_PollForConversion(&hadc1 ,100);
//	  ADC_Val = HAL_ADC_GetValue(&hadc1);

//      TxData[0]=0x48;
//      TxData[1]=0x00;
//      TxData[2]=0x21;
//      TxData[3]=0x01;
//      TxData[4]=0x20;
//      TxData[5]=0xFF;
//   	  TxData[6]=0x00;
//      TxData[7]=0x00;
//      HAL_CAN_AddTxMessage(&hcan1, &amp1.pTxHeader, TxData, &TxMailbox);
//      HAL_Delay(1);
//	    // Test: Set GPIO pin high
//	    HAL_GPIO_WritePin(GPIOF, GPIO_PIN_0, GPIO_PIN_SET);
//
//	    // Get ADC value
//	    HAL_ADC_Start(&hadc1);
//	    HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
//	    ADC_Val = HAL_ADC_GetValue(&hadc1);
//
//	    // Test: Set GPIO pin low
//	    HAL_GPIO_WritePin(GPIOF, GPIO_PIN_0, GPIO_PIN_RESET);

	  //***************************
	  // gelen veri ile toggle yapma
//      TxData=HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_13);
//      if(TxData==1){
//    	  HAL_GPIO_TogglePin(GPIOF, GPIO_PIN_0);
//      }else{
//    	  HAL_GPIO_WritePin(GPIOF, GPIO_PIN_0, GPIO_PIN_RESET);
//      }
	  //***************************
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

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */

  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.ScanConvMode = ENABLE;
  hadc1.Init.ContinuousConvMode = ENABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 2;
  hadc1.Init.DMAContinuousRequests = DISABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_0;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_144CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Rank = 2;
  sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief CAN1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_CAN1_Init(void)
{

  /* USER CODE BEGIN CAN1_Init 0 */

  /* USER CODE END CAN1_Init 0 */

  /* USER CODE BEGIN CAN1_Init 1 */

  /* USER CODE END CAN1_Init 1 */
  hcan1.Instance = CAN1;
  hcan1.Init.Prescaler = 12;
  hcan1.Init.Mode = CAN_MODE_NORMAL;
  hcan1.Init.SyncJumpWidth = CAN_SJW_1TQ;
  hcan1.Init.TimeSeg1 = CAN_BS1_11TQ;
  hcan1.Init.TimeSeg2 = CAN_BS2_2TQ;
  hcan1.Init.TimeTriggeredMode = DISABLE;
  hcan1.Init.AutoBusOff = DISABLE;
  hcan1.Init.AutoWakeUp = DISABLE;
  hcan1.Init.AutoRetransmission = DISABLE;
  hcan1.Init.ReceiveFifoLocked = DISABLE;
  hcan1.Init.TransmitFifoPriority = DISABLE;
  if (HAL_CAN_Init(&hcan1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN CAN1_Init 2 */

  /* USER CODE END CAN1_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOF, GPIO_PIN_0|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15, GPIO_PIN_RESET);

  /*Configure GPIO pins : PF0 PF13 PF14 PF15 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan1)
{


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