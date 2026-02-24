/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#include "tim.h"
#include "usart.h"
#include "gpio.h"

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

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
volatile uint8_t pritisnutaTipka = 0;

uint32_t buzzerStart = 0;
uint8_t buzzerActive = 0;


uint32_t zadnjeVrijemeTipke = 0;
extern TIM_HandleTypeDef htim6;
extern TIM_HandleTypeDef htim7;
uint32_t p=0;

extern TIM_HandleTypeDef htim8;

extern TIM_HandleTypeDef htim8;

/**
 * PlayTone - reproducira ton na TIM8_CH1 (PC6)
 * frekvencija: Hz (npr. 3000)
 * trajanje_ms: trajanje u milisekundama (npr. 20)
 */
void Click()
{
    HAL_TIM_PWM_Start(&htim8, TIM_CHANNEL_1);

    __HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_1, htim8.Init.Period / 2);

    for(volatile int i=0; i<5000; i++);  // <1ms – dovoljno za click

    HAL_TIM_PWM_Stop(&htim8, TIM_CHANNEL_1);
}








void PosKvadrant(int kvadrant)
{
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, GPIO_PIN_SET);
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_SET);

  switch (kvadrant) {
    case 0: HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET); break;
    case 1: HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET); break;
    case 2: HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, GPIO_PIN_RESET); break;
    case 3: HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_RESET); break;
  }
}
uint16_t ledPins[10] = {
  GPIO_PIN_0, GPIO_PIN_1, GPIO_PIN_4, GPIO_PIN_6, GPIO_PIN_7,
  GPIO_PIN_8, GPIO_PIN_9, GPIO_PIN_10, GPIO_PIN_11, GPIO_PIN_12
};

void PaliLed(int ledIndex, int trenKvadrant)
{
    for (int i = 0; i < 10; i++) {
        HAL_GPIO_WritePin(GPIOA, ledPins[i], GPIO_PIN_RESET);
    }

    if (ledIndex == 0 && trenKvadrant != 0) {
        return; // preskoči paljenje
    }

    HAL_GPIO_WritePin(GPIOA, ledPins[ledIndex], GPIO_PIN_SET);
}

char* Rulet_GetColor(uint8_t broj)
{
    if (broj == 0) return "ZELENA";

    switch(broj)
    {
        case 1: case 3: case 5: case 7: case 9:
        case 12: case 14: case 16: case 18: case 19:
        case 21: case 23: case 25: case 27: case 30:
        case 32: case 34: case 36:
            return "CRVENA";

        default:
            return "CRNA";
    }
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM6)
    {
        p = 0;
        HAL_TIM_Base_Stop_IT(&htim6);
    }
}





/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */
#include "main.h"
#include <stdbool.h>
#include <stdlib.h>
#include "lcd.h"


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
  MX_USART2_UART_Init();
  MX_I2C1_Init();
  MX_TIM6_Init();
  MX_TIM7_Init();
  MX_TIM8_Init();
  /* USER CODE BEGIN 2 */
  HAL_Delay(200);
  MX_I2C1_Init();
  LCD_Init();
  HAL_Delay(100);
  HAL_NVIC_SetPriority(EXTI1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI1_IRQn);

  HAL_TIM_Base_Start_IT(&htim6);
  HAL_TIM_Base_Start_IT(&htim7);


  if (HAL_I2C_IsDeviceReady(&hi2c1, 0x27 << 1, 3, 100) == HAL_OK)
  {
      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET); // LCD FOUND
  }
  else
  {
      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET); // LCD NOT FOUND
  }


  LCD_Init();
  HAL_Delay(500);


  LCD_Clear();
  LCD_SetCursor(0, 0);
  LCD_SendString("HELLO");
  LCD_SetCursor(1, 0);
  LCD_SendString("LCD TEST");

  uint8_t broj;   // broj na kojem je LED stala (0–36)



  int trenLed = rand() % 10;
  int trenKvartal = rand() % 4;
  bool CCW = true;

  PosKvadrant(trenKvartal);
  if (!(trenLed == 0 && trenKvartal != 0)) {
      PaliLed(trenLed,trenKvartal);
  }

  int rouletteNumbers[4][10] = {
      { 0, 32, 15, 19, 4, 21, 2, 25, 17, 34 },    // Kvadrant 0
      { -1, 6, 27, 13, 34, 11, 30, 8, 23, 10 },   // Kvadrant 1
      { -1, 5, 24, 16, 33, 1, 20, 14, 31,  9 },   // Kvadrant 2
      { -1, 22, 18, 29, 7, 28, 12, 25, 3, 26 }    // Kvadrant 3
  };*/
  int rouletteNumbers[4][10] = {
        { 26, 0, 32, 15, 19, 4, 21, 2, 25, 17 },    // Kvadrant 0
        { -1, 34, 6, 27, 13, 36, 11, 30, 8, 23 },   // Kvadrant 1
        { -1, 10, 5, 24, 16, 33, 1, 20, 14,  31 },   // Kvadrant 2
        { -1, 9, 22, 18, 29, 7, 28, 12, 25, 3 }    // Kvadrant 3
    };


  //LightLed(currentLed, currentQuarter);
  //Click();
  /*
  HAL_TIM_PWM_Start(&htim8, TIM_CHANNEL_1);
  __HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_1, htim8.Init.Period / 2);
  HAL_Delay(200);
  __HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_1, 0);
*/
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {


	  if (pritisnutaTipka)
	  {

		  pritisnutaTipka=0;
		  HAL_TIM_Base_Stop(&htim7);
	    //uint32_t startTime = HAL_GetTick();
	    //while (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_1) == GPIO_PIN_RESET);
	    //uint32_t pressDuration = HAL_GetTick() - startTime;
		  __HAL_TIM_SET_COUNTER(&htim7, 0);      // reset brojača
		  HAL_TIM_Base_Start(&htim7);            // pokreni timer

		  while (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_1) == GPIO_PIN_RESET);

		  HAL_TIM_Base_Stop(&htim7);             // zaustavi timer
		  uint32_t duzPritiska = __HAL_TIM_GET_COUNTER(&htim7);  // očitaj vrijeme


		  /*

		  LCD_SetCursor(0, 0);
		 char buf[16];
		  sprintf(buf, "Vrijeme: %4d ms", duzPritiska);
		  LCD_Clear();
		  LCD_SendString(buf);
*///>=
	    int steps = 37 + rand() % 18;
	    if (duzPritiska > 5000) steps *= 1.5;
	    //if (duzPritiska > 5000) steps *= 2.1;

	    for (int step = 0; step < steps; step++) {
	      // 1. Ugasi sve LED-ice
	      for (int i = 0; i < 10; i++) {
	        HAL_GPIO_WritePin(GPIOA, ledPins[i], GPIO_PIN_RESET);
	      }

	      // 2. Ugasi sve kvartale
	      HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_4, GPIO_PIN_SET);

	      // 3. Mali delay za stabilizaciju signala
	      HAL_Delay(1);

	      // 4. Postavi aktivni kvartal
	      PosKvadrant(trenKvartal);



	      // 5. Upali SAMO aktivnu LED-icu
	      if (!(trenLed == 0 && trenKvartal != 0)) {

	          HAL_GPIO_WritePin(GPIOA, ledPins[trenLed], GPIO_PIN_SET);

	      }
	      else if (1) {

	          trenLed++;

	          if (trenLed > 9) {
	              trenLed = 0;
	              trenKvartal = (trenKvartal + 1) % 4;
	          }

	          HAL_GPIO_WritePin(GPIOA, ledPins[trenLed], GPIO_PIN_SET);
	      }


	      HAL_Delay(40 + step * 5);

	        trenLed++;
	        if (trenLed > 9) {
	          trenLed = 0;
	          trenKvartal = (trenKvartal + 1) % 4;
	        }

	    }

	    int finalLed = trenLed;

	    while (rouletteNumbers[trenKvartal][finalLed] == -1) {

	        finalLed++;

	        if (finalLed > 9) {
	            finalLed = 0;
	            trenKvartal = (trenKvartal + 1) % 4;
	        }
	    }

	    int resultNumber = rouletteNumbers[trenKvartal][finalLed];

	    uint8_t rezultat = resultNumber;

	    LCD_Clear();

	    LCD_SetCursor(0, 0);          // red 0, kolona 0
	    LCD_SendString("Broj: ");

	    char buffer[10];               // dovoljno za 0–36 + null
	    sprintf(buffer, " %2d", resultNumber);

	    LCD_SendString(buffer);

	    // 3. Drugi red — boja
	    LCD_SetCursor(1, 0);          // red 1, kolona 0
	    LCD_SendString(Rulet_GetColor(resultNumber));
	    sprintf(buffer, " %d", Rulet_GetColor(resultNumber));
	    //LCD_SendString("||");
	    //sprintf(buffer, " K %2d L %2d", trenKvartal, finalLed);
	  }




    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

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
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 52;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
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
