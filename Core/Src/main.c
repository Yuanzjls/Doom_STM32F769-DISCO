/* USER CODE BEGIN Header */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/

#include <main.h>
#include <string.h>
#include <ff.h>
#include <stm32f769i_discovery_audio.h>
#include "usbh_def.h"

//#include <stdio.h>



//#include "MainTask.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
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

extern LTDC_HandleTypeDef hltdc_discovery;
static DMA2D_HandleTypeDef   hdma2d;
extern DSI_HandleTypeDef hdsi_discovery;
DMA2D_HandleTypeDef Dma2dHandle;
DSI_VidCfgTypeDef hdsivideo_handle;
DSI_CmdCfgTypeDef CmdCfg;
DSI_LPCmdTypeDef LPCmd;
DSI_PLLInitTypeDef dsiPllInit;
SAI_HandleTypeDef SaiHandle;
DMA_HandleTypeDef            hSaiDma;

FATFS 	FatFs;
char 	Path[4];

USBH_HandleTypeDef hUSBHost[5], *phUSBHost = NULL;

uint8_t host_state;
osThreadId thread_id;


static void USBThread(void const *argument);
static void DoomThread(void const *argument);
static void USBH_UserProcess(USBH_HandleTypeDef *phost, uint8_t id);
static void HUB_Process(void);





/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/



RTC_HandleTypeDef hrtc;
UART_HandleTypeDef UartHandle;
DMA_HandleTypeDef DMAUartHandle;

/* USER CODE BEGIN PV */
/*GIF Decode Varible part*/



/* USER CODE END PV */



/* Private function prototypes -----------------------------------------------*/
 void SystemClock_Config(void);
static void MX_RTC_Init(void);

/* USER CODE BEGIN PFP */
/* USER CODE END PFP */
void vApplicationStackOverflowHook(xTaskHandle *pxTask, signed char *pcTaskName )
{
	xTaskHandle *bad_task_handle;
	signed char *bad_task_name;

    bad_task_handle = pxTask;     // this seems to give me the crashed task handle
    bad_task_name = pcTaskName;     // this seems to give me a pointer to the name of the crashed task
    //mLED_3_On();   // a LED is lit when the task has crashed
    taskDISABLE_INTERRUPTS();
    for( ;; );
    taskEXIT_CRITICAL();
}








/*gif call back functions end */
/* USER CODE END 0 */







//higher number, higher priority
static void AppTaskCreate(void)
{



}
static void MPU_Config(void)
{
  MPU_Region_InitTypeDef MPU_InitStruct;

  /* Disable the MPU */
  HAL_MPU_Disable();

  /* Configure the MPU as Normal Non Cacheable for the SRAM1 and SRAM2 */
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.BaseAddress = 0x20020000;
  MPU_InitStruct.Size = MPU_REGION_SIZE_512KB;
  MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_SHAREABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER1;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL1;
  MPU_InitStruct.SubRegionDisable = 0x00;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_ENABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);

  /* Configure the MPU attributes as Non Cacheable and Non Bufferable for SDRAM */
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.BaseAddress = 0xC0000000;
  MPU_InitStruct.Size = MPU_REGION_SIZE_256MB;
  MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_NOT_SHAREABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER2;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
  MPU_InitStruct.SubRegionDisable = 0x00;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_ENABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);

  /* Configure the MPU attributes as WT for SDRAM */
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.BaseAddress = 0xC0000000;
  MPU_InitStruct.Size = MPU_REGION_SIZE_8MB;
  MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_CACHEABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_NOT_SHAREABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER3;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
  MPU_InitStruct.SubRegionDisable = 0x00;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_ENABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);

  /* Configure the MPU attributes as strongly ordred for QSPI (unused zone) */
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.BaseAddress = 0x90000000;
  MPU_InitStruct.Size = MPU_REGION_SIZE_256MB;
  MPU_InitStruct.AccessPermission = MPU_REGION_PRIV_RO;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_NOT_SHAREABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER4;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
  MPU_InitStruct.SubRegionDisable = 0x00;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_ENABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);

  /* Configure the MPU attributes as WT for QSPI (used 16Mbytes) */
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.BaseAddress = 0x90000000;
  MPU_InitStruct.Size = MPU_REGION_SIZE_16MB;
  MPU_InitStruct.AccessPermission = MPU_REGION_PRIV_RO;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_CACHEABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_NOT_SHAREABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER5;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
  MPU_InitStruct.SubRegionDisable = 0x00;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_ENABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);

  /* Enable the MPU */
  HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);
}



int main(void)
{
  /* USER CODE BEGIN 1 */
  /* USER CODE END 1 */
	

	uint8_t SD_state = MSD_OK, res;

	
	uint32_t AlphaInvertConfig;


	MPU_Config();
	SCB_InvalidateICache();

	  /* Enable branch prediction */
	  SCB->CCR |= (1 <<18);
	  __DSB();

	  /* Invalidate I-Cache : ICIALLU register*/
	  SCB_InvalidateICache();

	  /* Enable I-Cache */
	  SCB_EnableICache();

	  SCB_InvalidateDCache();
	  SCB_EnableDCache();

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
  /* Enable the CRC Module */
  __HAL_RCC_CRC_CLK_ENABLE();
	  MX_RTC_Init();
	  //MPU_Config();


	  /*##-1- JPEG Initialization ################################################*/
	  /* Init The JPEG Color Look Up Tables used for YCbCr to RGB conversion   */

	  /* Init the HAL JPEG driver */
	   //JPEG_Handle.Instance = JPEG;


		UartHandle.Instance        = USARTx;
		UartHandle.Init.BaudRate   = 115200;
		UartHandle.Init.WordLength = UART_WORDLENGTH_9B;
		UartHandle.Init.StopBits   = UART_STOPBITS_1;
		UartHandle.Init.Parity     = UART_PARITY_ODD;
		UartHandle.Init.HwFlowCtl  = UART_HWCONTROL_NONE;
		UartHandle.Init.Mode       = UART_MODE_TX_RX;
		UartHandle.Init.OverSampling = UART_OVERSAMPLING_16;
		if (HAL_UART_Init(&UartHandle) != HAL_OK)
		{
		  /* Initialization Error */
		  Error_Handler();
		}


  /* USER CODE BEGIN 2 */
	   // gif decode callback function set
	    /* Initializes the SDRAM device */
	    BSP_SDRAM_Init();
		BSP_SD_Init();
		BSP_LED_Init(LED1);
		BSP_LED_Init(LED2);
		//BSP_LED_Init(LED3);




		memset(&hUSBHost[0], 0, sizeof(USBH_HandleTypeDef));

		  hUSBHost[0].valid   = 1;
		  hUSBHost[0].address = USBH_DEVICE_ADDRESS;
		  hUSBHost[0].Pipes   = USBH_malloc(sizeof(uint32_t) * USBH_MAX_PIPES_NBR);



		  /* USB task */
		  osThreadDef(USB_Thread, USBThread, osPriorityLow, 0, 8 * configMINIMAL_STACK_SIZE);
		  osThreadCreate(osThread(USB_Thread), NULL);

		  /* Doom task */
		  osThreadDef(DOOM_Thread, DoomThread, osPriorityRealtime, 0, 16 * configMINIMAL_STACK_SIZE);
		  thread_id = osThreadCreate(osThread(DOOM_Thread), NULL);

		  //osThreadSuspend (thread_id);

    vTaskStartScheduler();

    while(1);
    


    /* USER CODE BEGIN 3 */
  /* USER CODE END 3 */
}
static void HUB_Process(void)
{
	static uint8_t current_port = -1;

	if(phUSBHost != NULL && phUSBHost->valid == 1)
	{
		USBH_Process(phUSBHost);

		if(phUSBHost->busy)
			return;
	}

	for( ;; )
	{
		current_port++;

		if(current_port > MAX_HUB_PORTS)
			current_port = 0;

		if(hUSBHost[current_port].valid)
		{
			phUSBHost = &hUSBHost[current_port];
			USBH_LL_SetupEP0(phUSBHost);

			if(phUSBHost->valid == 3)
			{
				phUSBHost->valid = 1;
				phUSBHost->busy  = 1;
			}

			break;
		}
	}
}
static void USBThread(void const * argument)
{
	  /* Init Host Library */
	  USBH_Init(&hUSBHost[0], USBH_UserProcess, 0);

	  /* Add Supported Class */
	  USBH_RegisterClass(&hUSBHost[0], USBH_HID_CLASS);
	  USBH_RegisterClass(&hUSBHost[0], USBH_HUB_CLASS);

	  /* Start Host Process */
	  USBH_Start(&hUSBHost[0]);
	for( ;; )
	{
	    /* USB Host Background task */
		HUB_Process();
	}
}

static void DoomThread(void const * argument)
{
	f_mount(&FatFs, (TCHAR const*)"", 1);
	D_DoomMain ();
}
/**
  * @brief  User Process
  * @param  phost: Host Handle
  * @param  id: Host Library user message ID
  * @retval None
  */
static void USBH_UserProcess(USBH_HandleTypeDef *phost, uint8_t id)
{
  	switch(id)
  	{
  		case HOST_USER_SELECT_CONFIGURATION:
    			break;

 	 	case HOST_USER_DISCONNECTION:
 	 			host_state = HOST_USER_DISCONNECTION;
    			break;

  		case HOST_USER_CLASS_ACTIVE:
  				osDelay (100);

  				host_state = HOST_USER_CLASS_ACTIVE;

  				osThreadResume (thread_id);
    			break;

  		case HOST_USER_CONNECTION:
  				host_state = HOST_USER_CONNECTION;
    			break;

  		default:
    			break;
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
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  /**Configure LSE Drive Capability
  */
  HAL_PWR_EnableBkUpAccess();
  __HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_LOW);
  /**Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /**Initializes the CPU, AHB and APB busses clocks
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE|RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 432;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /**Activate the Over-Drive mode
  */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
    Error_Handler();
  }
  /**Initializes the CPU, AHB and APB busses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_7) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_LTDC|RCC_PERIPHCLK_RTC;
  PeriphClkInitStruct.PLLSAI.PLLSAIN = 384;
  PeriphClkInitStruct.PLLSAI.PLLSAIR = 7;
  PeriphClkInitStruct.PLLSAI.PLLSAIQ = 2;
  PeriphClkInitStruct.PLLSAI.PLLSAIP = RCC_PLLSAIP_DIV2;
  PeriphClkInitStruct.PLLSAIDivQ = 1;
  PeriphClkInitStruct.PLLSAIDivR = RCC_PLLSAIDIVR_2;
  PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
}




/**
  * @brief RTC Initialization Function
  * @param None
  * @retval None
  */
static void MX_RTC_Init(void)
{

  /* USER CODE BEGIN RTC_Init 0 */
  /* USER CODE END RTC_Init 0 */

  /* USER CODE BEGIN RTC_Init 1 */
  /* USER CODE END RTC_Init 1 */
  /**Initialize RTC Only 
  */
  hrtc.Instance = RTC;
  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv = 127;
  hrtc.Init.SynchPrediv = 255;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RTC_Init 2 */
  /* USER CODE END RTC_Init 2 */

}





/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
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
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
