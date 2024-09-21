/*Duong Quoc Thang's Project*/
#include "main.h"
#include "stdio.h"
#include "fonts.h"
#include "ssd1306.h"
#include "test.h"
#include "bitmap.h"
#include "horse_anim.h"
I2C_HandleTypeDef hi2c1;

TIM_HandleTypeDef htim1;

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_TIM1_Init(void);

#define DHT22_PORT GPIOB
#define DHT22_PIN GPIO_PIN_14

uint8_t hum1, hum2, tempC1, tempC2, SUM, CHECK;
uint32_t pMillis, cMillis;
float temp_Celsius = 0;
float temp_Fahrenheit = 0;
float Humidity = 0;
uint8_t hum_integral, hum_decimal, tempC_integral, tempC_decimal, tempF_integral, tempF_decimal;
char string[15];

void microDelay (uint16_t delay)
{
  __HAL_TIM_SET_COUNTER(&htim1, 0);
  while (__HAL_TIM_GET_COUNTER(&htim1) < delay);
}

uint8_t DHT22_Start (void)
{
  uint8_t Response = 0;
  GPIO_InitTypeDef GPIO_InitStructPrivate = {0};
  GPIO_InitStructPrivate.Pin = DHT22_PIN;
  GPIO_InitStructPrivate.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStructPrivate.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStructPrivate.Pull = GPIO_NOPULL;
  
  HAL_GPIO_Init(DHT22_PORT, &GPIO_InitStructPrivate);
  HAL_GPIO_WritePin (DHT22_PORT, DHT22_PIN, 0);
  microDelay (1300);
  
  HAL_GPIO_WritePin (DHT22_PORT, DHT22_PIN, 1);
  microDelay (30);
  
  GPIO_InitStructPrivate.Mode = GPIO_MODE_INPUT;
  GPIO_InitStructPrivate.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(DHT22_PORT, &GPIO_InitStructPrivate);
  microDelay (40);
  
  if (!(HAL_GPIO_ReadPin (DHT22_PORT, DHT22_PIN)))
  {
    microDelay (80);
    if ((HAL_GPIO_ReadPin (DHT22_PORT, DHT22_PIN))) Response = 1;
  }
  pMillis = HAL_GetTick();
  cMillis = HAL_GetTick();
  while ((HAL_GPIO_ReadPin (DHT22_PORT, DHT22_PIN)) && pMillis + 2 > cMillis)
  {
    cMillis = HAL_GetTick();
  }
  return Response;
}

uint8_t DHT22_Read (void)
{
  uint8_t x,y;
  for (x=0;x<8;x++)
  {
    pMillis = HAL_GetTick();
    cMillis = HAL_GetTick();
    while (!(HAL_GPIO_ReadPin (DHT22_PORT, DHT22_PIN)) && pMillis + 2 > cMillis)
    {
      cMillis = HAL_GetTick();
    }
    microDelay (40);
    if (!(HAL_GPIO_ReadPin (DHT22_PORT, DHT22_PIN)))   // if the pin is low
      y&= ~(1<<(7-x));
    else
      y|= (1<<(7-x));
    pMillis = HAL_GetTick();
    cMillis = HAL_GetTick();
    while ((HAL_GPIO_ReadPin (DHT22_PORT, DHT22_PIN)) && pMillis + 2 > cMillis)
    {  // wait for the pin to go low
      cMillis = HAL_GetTick();
    }
  }
  return y;
}
int main(void)
{
    SystemClock_Config();

  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_TIM1_Init();
    HAL_TIM_Base_Start(&htim1);
    SSD1306_Init();
    
  while (1)
  {
	  if(DHT22_Start())
	  	     {
	  	       hum1 = DHT22_Read();
	  	       hum2 = DHT22_Read();
	  	       tempC1 = DHT22_Read();
	  	       tempC2 = DHT22_Read();
	  	       SUM = DHT22_Read();
	  	       CHECK = hum1 + hum2 + tempC1 + tempC2;
	  	       if (CHECK == SUM)
	  	       {
	  	         if (tempC1>127)
	  	         {
	  	           temp_Celsius = (float)tempC2/10*(-1);
	  	         }
	  	         else
	  	         {
	  	           temp_Celsius = (float)((tempC1<<8)|tempC2)/10;
	  	         }

	  	    /*     temp_Fahrenheit = temp_Celsius * 9/5 + 32;  */

	  	         Humidity = (float) ((hum1<<8)|hum2)/10;

	  	         SSD1306_GotoXY (0, 0);
	  	         hum_integral = Humidity;
	  	         hum_decimal = Humidity*10-hum_integral*10;
	  	         sprintf(string," DA: %d.%d %%  ", hum_integral, hum_decimal);
	  	         SSD1306_Puts (string, &Font_11x18, 1);

	  	         SSD1306_GotoXY (0, 20);
	  	         if (temp_Celsius < 0)
	  	         {
	  	           tempC_integral = temp_Celsius *(-1);
	  	           tempC_decimal = temp_Celsius*(-10)-tempC_integral*10;
	  	           sprintf(string," ND: -%d.%d C   ", tempC_integral, tempC_decimal);
	  	         }
	  	         else
	  	         {
	  	           tempC_integral = temp_Celsius;
	  	           tempC_decimal = temp_Celsius*10-tempC_integral*10;
	  	           sprintf(string," ND: %d.%d C   ", tempC_integral, tempC_decimal);
	  	         }
	  	         SSD1306_Puts (string, &Font_11x18, 1);

	  	         SSD1306_GotoXY (0, 40);
	  	 /*        if(temp_Fahrenheit < 0)
	  	         {
	  	           tempF_integral = temp_Fahrenheit*(-1);
	  	           tempF_decimal = temp_Fahrenheit*(-10)-tempF_integral*10;
	  	           sprintf(string,"-%d.%d F   ", tempF_integral, tempF_decimal);
	  	         }
	  	         else
	  	         {
	  	           tempF_integral = temp_Fahrenheit;
	  	           tempF_decimal = temp_Fahrenheit*10-tempF_integral*10;
	  	           sprintf(string,"%d.%d F   ", tempF_integral, tempF_decimal);
	  	         }
	  	         SSD1306_Puts (string, &Font_11x18, 1);
	  	         SSD1306_UpdateScreen(); */
	  	       }

	  	     if (temp_Celsius >38 )
	  	    	     {
	  	    	  	   HAL_GPIO_TogglePin(L_GPIO_Port, L_Pin);
	  	    	  	   HAL_GPIO_WritePin(L_GPIO_Port,L_Pin,SET);			//đèn tắt
	  	    	      }
			else 
			     {
				   HAL_GPIO_TogglePin(L_GPIO_Port, L_Pin);
	  	  	  	   HAL_GPIO_WritePin(L_GPIO_Port,L_Pin,RESET);
			     }			  	    	  	         
	  	   if (Humidity > 65)
	  	  	      {
	  	  	  	    HAL_GPIO_TogglePin(F_GPIO_Port, F_Pin);
	  	  	  	    HAL_GPIO_WritePin(F_GPIO_Port,F_Pin,SET);    			//QUAT TAT
			      }
			else
			      {
				    HAL_GPIO_TogglePin(F_GPIO_Port, F_Pin);
	  	    	  	    HAL_GPIO_WritePin(F_GPIO_Port,F_Pin,RESET);			// quạt chạy
	  	     	      }
	  	     HAL_Delay(500);
	    }
	  }
  
}

void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 72;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

static void MX_I2C1_Init(void)
{

  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 400000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
}


static void MX_TIM1_Init(void)
{
  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 71;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 65535;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  
  if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
}

static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  HAL_GPIO_WritePin(L_GPIO_Port, L_Pin, GPIO_PIN_SET);

  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);

  HAL_GPIO_WritePin(F_GPIO_Port, F_Pin, GPIO_PIN_SET);

  GPIO_InitStruct.Pin = L_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(L_GPIO_Port, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = GPIO_PIN_14;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = F_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(F_GPIO_Port, &GPIO_InitStruct);

}

void Error_Handler(void)
{}

#ifdef  USE_FULL_ASSERT

void assert_failed(uint8_t *file, uint32_t line)
{}
#endif 