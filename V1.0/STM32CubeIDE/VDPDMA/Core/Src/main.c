/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2022 STMicroelectronics.
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
#include "comp.h"
#include "dma.h"
#include "spi.h"
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "vdp.h"
#include "math.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

#define BLANKING_LINES (28 + (260 - 249))
#define FRAME_LINES 261

#define FIRST_VISIBLE_LINE 40
#define VISIBLE_LINES 192
#define LAST_VISIBLE_LINE (FIRST_VISIBLE_LINE + VISIBLE_LINES - 1)
#define BOTTOM_BORDER 32

#define PIXELS_PER_LINE 256

#define RGB_BUF_LEN (VISIBLE_LINES * PIXELS_PER_LINE)
#ifdef PAR_TFT
	#define RGB_BUF_CHUNKS 1
#else
	#define RGB_BUF_CHUNKS 4
#endif

#define RGB_BUF_CHUNK_BYTES (RGB_BUF_LEN / RGB_BUF_CHUNKS * 2)

#define CAPTURE_STATE_SEEKING_SYNC 0
#define CAPTURE_STATE_FOUND_SYNC 1
#define CAPTURE_STATE_START_GRAB 2
#define CAPTURE_STATE_GRABBING 3
#define CAPTURE_STATE_GRABBED 4
#define CAPTURE_STATE_IDLE 5

#define DMA_CAPTURE_STATE_IDLE 0
#define DMA_STATE_START 1
#define DMA_STATE_RUNNING 2
#define DMA_STATE_CHUNK_COMPLETE 3

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
extern void mysetup();
extern int mymain(uint16_t);
extern void pushpixel(uint16_t);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

volatile uint16_t rgbBuf[RGB_BUF_LEN];
volatile uint32_t rgbBufIdx;
uint8_t rgbBufchunk;

volatile uint32_t adcbuf[512];
volatile int16_t line;
volatile uint8_t captureState;
volatile uint8_t frameReady;
volatile uint8_t dmaState;

volatile uint16_t pix;

int _write(int file, char *ptr, int len) {
	int DataIdx;
	for (DataIdx = 0; DataIdx < len; DataIdx++) {
		ITM_SendChar(*ptr++);
	}
	return len;
}

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
  MX_DMA_Init();
  MX_TIM8_Init();
  MX_TIM1_Init();
  MX_ADC1_Init();
  MX_ADC2_Init();
  MX_TIM3_Init();
  MX_TIM2_Init();
  MX_TIM4_Init();
  MX_COMP1_Init();
  MX_TIM5_Init();
  MX_TIM16_Init();
  MX_TIM20_Init();
  MX_SPI2_Init();
  /* USER CODE BEGIN 2 */
	GPIOB->MODER = (GPIOB->MODER & 0xFFFFFFCF) | 0x00000010;
	mysetup();

	initBitMap();
	initColorMaps(0.43/0.47, 1.72);

#ifndef PAR_TFT
	swapRGBBytes();
#endif

	setLeftWindow();

	HAL_TIM_RegisterCallback(&htim3, HAL_TIM_OC_DELAY_ELAPSED_CB_ID,
			VSyncCallback);

	HAL_SPI_RegisterCallback(&hspi2, HAL_SPI_TX_COMPLETE_CB_ID,
			SPIFullCallback);
	HAL_DMA_RegisterCallback(&hdma_tim20_ch1, HAL_DMA_XFER_CPLT_CB_ID,
			GPIODMAComplete);

	HAL_ADC_RegisterCallback(&hadc1, HAL_ADC_CONVERSION_COMPLETE_CB_ID,
			ADCCallback);
	HAL_ADC_RegisterCallback(&hadc1, HAL_ADC_CONVERSION_HALF_CB_ID,
			ADCCallback);

	HAL_COMP_Start(&hcomp1);
	HAL_TIM_Base_Start(&htim4); // hysnc
	HAL_TIM_Base_Start(&htim1); // sample clock
	HAL_TIM_Base_Start_IT(&htim3); // vsync

	HAL_TIM_Base_Start(&htim8); // Parallel TFT DMA line clock
	HAL_TIM_Base_Start(&htim20); // Parallel TFT DMA pixel clock

	//HAL_DMA_Start_IT(&hdma_tim20_ch1, (uint32_t)rgbBuf, (uint32_t)&GPIOC->ODR, RGB_BUF_LEN);
	__HAL_TIM_ENABLE_DMA(&htim20, TIM_DMA_CC1);

	if (HAL_TIM_OnePulse_Start(&htim4, TIM_CHANNEL_1) != HAL_OK) {
		/* Starting Error */
		Error_Handler();
	}
	if (HAL_TIM_OnePulse_Start(&htim1, TIM_CHANNEL_1) != HAL_OK) {
		/* Starting Error */
		Error_Handler();
	}
	if (HAL_TIM_OnePulse_Start_IT(&htim3, TIM_CHANNEL_1) != HAL_OK) {
		/* Starting Error */
		Error_Handler();
	}

	if (HAL_TIM_OnePulse_Start_IT(&htim20, TIM_CHANNEL_1) != HAL_OK) {
		/* Starting Error */
		Error_Handler();
	}
	HAL_TIM_OnePulse_Start(&htim8, TIM_CHANNEL_1);

	HAL_ADCEx_MultiModeStart_DMA(&hadc1, adcbuf, 512);

	line = BLANKING_LINES; // Line 0 is when we detect vsync at the end of a frame
	captureState = CAPTURE_STATE_SEEKING_SYNC;

	dmaState = DMA_CAPTURE_STATE_IDLE;

	rgbBufIdx = 0;
	rgbBufchunk = 0;
	pix = 0;
	frameReady = 0;

	GPIOB->MODER = (GPIOB->MODER & 0xFFFFFFCF) | 0x00000010;

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	while (1) {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

		switch (captureState) {

		case CAPTURE_STATE_SEEKING_SYNC:
			break;

		case CAPTURE_STATE_FOUND_SYNC:
			captureState = CAPTURE_STATE_START_GRAB;
			break;

		case CAPTURE_STATE_GRABBED:
			captureState = CAPTURE_STATE_IDLE;
			frameReady = 1;
			break;

		case CAPTURE_STATE_IDLE:

			if (frameReady == 0) {
				captureState = CAPTURE_STATE_START_GRAB;
				pix=0;
			}

			break;

		default:
			break;
		}

		switch (dmaState) {

		case DMA_CAPTURE_STATE_IDLE:
			if (frameReady == 1) {
				dmaState = DMA_STATE_START;
			}
			break;

		case DMA_STATE_START:

#ifndef PAR_TFT
			while(SPI2->SR & SPI_SR_BSY) {
				__NOP();
			}
			vsync();
			while(SPI2->SR & SPI_SR_BSY) {
				__NOP();
			}
#else
			vsync();
#endif

			rgbBufchunk = 0;
			rgbBufIdx = 0;
			dmaState = DMA_STATE_RUNNING;
#ifndef PAR_TFT
			HAL_SPI_Transmit_DMA(&hspi2, (uint8_t *)&rgbBuf, RGB_BUF_CHUNK_BYTES);
#else
			TIM20->DIER &= ~ TIM_DIER_CC1DE;
			TIM20->DIER |= TIM_DIER_CC1DE;
			HAL_DMA_Start_IT(&hdma_tim20_ch1, (uint32_t)rgbBuf, (uint32_t)&GPIOC->ODR, RGB_BUF_CHUNK_BYTES/2);

			GPIOB->MODER = (GPIOB->MODER & 0xFFFFFFCF) | 0x00000020;

			__HAL_TIM_ENABLE(&htim8);
#endif
			break;

		case DMA_STATE_RUNNING:
			break;

		case DMA_STATE_CHUNK_COMPLETE:
			rgbBufchunk++;

			if (rgbBufchunk == RGB_BUF_CHUNKS) {
				dmaState = DMA_CAPTURE_STATE_IDLE;
				frameReady = 0;
			} else {
				dmaState = DMA_STATE_RUNNING;
#ifndef PAR_TFT
				HAL_SPI_Transmit_DMA(&hspi2, (uint8_t *)&(rgbBuf[rgbBufchunk * RGB_BUF_LEN / RGB_BUF_CHUNKS]), RGB_BUF_CHUNK_BYTES);
#else
				TIM20->DIER &= ~ TIM_DIER_CC1DE;
				TIM20->DIER |= TIM_DIER_CC1DE;
				HAL_DMA_Start_IT(&hdma_tim20_ch1, (uint32_t)&rgbBuf[rgbBufchunk * (RGB_BUF_LEN / RGB_BUF_CHUNKS)], (uint32_t)&GPIOC->ODR, RGB_BUF_CHUNK_BYTES/2);
				GPIOB->MODER = (GPIOB->MODER & 0xFFFFFFCF) | 0x00000020;
				__HAL_TIM_ENABLE(&htim8);
#endif
				}
			break;

		default:
			break;

		}
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
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV1;
  RCC_OscInitStruct.PLL.PLLN = 32;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }

  /** Enables the Clock Security System
  */
  HAL_RCC_EnableCSS();
}

/* USER CODE BEGIN 4 */

void ADCCallback(ADC_HandleTypeDef *hadc) {
	register uint8_t y;
	register uint8_t ry;
	register uint16_t bits;
	register uint8_t b;
	register uint16_t pixel = pix;
	register uint16_t i = PIXELS_PER_LINE;
	line++;
	if (line > FRAME_LINES)
		line = 1;

	if (captureState != CAPTURE_STATE_GRABBING)
		return;

	if (line > FIRST_VISIBLE_LINE && line < (LAST_VISIBLE_LINE + 2)) { // don't capture the previous line - it was part of frame blanking

		while (i > 0) {

			y = (((uint16_t*) adcbuf)[pixel] & 0xff) >> (8 - ADC_RESOLUTION_BITS);
			ry = (((uint16_t*) adcbuf)[pixel] >> (16 - ADC_RESOLUTION_BITS));

			bits = YMap[y] & RYMap[ry];

			b = bitMap[bits >> 8];
			if(b == 0) {
				b = bitMap[bits & 0xff];
			} else {
				b+=8;
			}

			if (rgbBufIdx < RGB_BUF_LEN) {

				rgbBuf[rgbBufIdx] = rgbMap[b];
				rgbBufIdx++;
			}
			pixel++;
			i--;
		}
	}

	if (pixel >= 512) {
		pixel = 0;
	}

	pix = pixel;
}

void VSyncCallback(TIM_HandleTypeDef *htim) {

	line = LAST_VISIBLE_LINE + BOTTOM_BORDER;
	pix = 0;

	switch (captureState) {
	case CAPTURE_STATE_SEEKING_SYNC:
		captureState = CAPTURE_STATE_FOUND_SYNC;
		break;

	case CAPTURE_STATE_START_GRAB:
		captureState = CAPTURE_STATE_GRABBING;
		rgbBufIdx = 0;
		HAL_ADCEx_MultiModeStart_DMA(&hadc1, adcbuf, 512);
		break;

	case CAPTURE_STATE_GRABBING:
		HAL_ADCEx_MultiModeStop_DMA(&hadc1);
		captureState = CAPTURE_STATE_GRABBED;

	default:
		break;

	}
}

void SPIFullCallback(SPI_HandleTypeDef *spi) {
	dmaState = DMA_STATE_CHUNK_COMPLETE;
}

void GPIODMAComplete(DMA_HandleTypeDef *dma) {
	dmaState = DMA_STATE_CHUNK_COMPLETE;
	__HAL_TIM_DISABLE(&htim8);
	GPIOB->MODER = (GPIOB->MODER & 0xFFFFFFCF) | 0x00000010;
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
	while (1) {
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
