/*
 * main.cpp
 *
 *  Created on: Jul 15, 2024
 *      Author: youngbin
 */

#include "main.h"
#include <stdio.h>
#include <stdlib.h>
#include "mc.h"


#define RAM_START ((volatile unsgined int*)(0x20000000))
#define KEY_LEN 1024
#define SEC_BL_ADDR 0x0800C000


void SystemClock_Config(void);
static void MX_GPIO_Init(void);

void Jump_To_Sec_BL()
{

	uint32_t sec_bl_addr = SEC_BL_ADDR;

	uint32_t *sec_bl_vt = (uint32_t *)sec_bl_addr;

	SCB->VTOR = sec_bl_addr;

	__asm__ __volatile__
	(
			//__set_MSP(sec_bl_vt[0]);
			"ldr r0, [%0] \n\t"
			"msr MSP, r0 \n\t"

			"ldr r1, [%0, #4] \n\t"
			"blx r1 \n\t"
			:
			:"r"(sec_bl_vt)
			:"r0", "r1"

			);


	//__set_MSP(sec_bl_vt[0]);

	//(*(void (*)())((void*)sec_bl_vt[1]))();


	//sec_bl_entry();

}

int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/
  //MPU_Config();

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */



  //HAL_Init();
  //SystemClock_Config();
  //MX_GPIO_Init();



  //MX_USART3_UART_Init();

  /* USER CODE BEGIN Init */

  
  uint32_t* sig;
  sig = (uint32_t *)(0x0800C000 - 128 - 4 - 4 - 128);

  uint32_t* size;
  size = (uint32_t *)sig + 32; // 128 bytes

  uint32_t* ver;
  ver = (uint32_t *)size + 1; // 4bytes

  uint32_t* pub2_k;
  pub2_k = (uint32_t*)ver + 1; // 4bytes

  uint32_t* sec_bl;
  sec_bl = (uint32_t*)pub2_k + 32; // 128 bytes

  int RSA_key = 0x0;
  int RSA_sign_len = 128;                                                                                               

  int plain_len = *size;
  uint8_t* plain_data = (uint8_t *)sec_bl;

  if (!Verify_RSA1024(RSA_key, (uint8_t *)sig, RSA_sign_len, plain_data, &plain_len)){
    while(1); // Second bootloader is tampered with.
  }

  else // Verification Success
  {
    Jump_To_Sec_BL();
  }
	  
  /* USER CODE END Init */

  /* Configure the system clock */

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Supply configuration update enable
  */
  HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_DIV1;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV1;
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief USART3 Initialization Function
  * @param None
  * @retval None
  */


/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/* MPU Configuration */

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
