/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                        opensource.org/licenses/BSD-3-Clause
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "dma.h"
#include "eth.h"
#include "gpio.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "usb_otg.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
// #include <string>
#include <stdio.h>
#include <string.h>
#include <string>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

#define SPI_BUFFER_LENGTH 10
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

DMA_BUFFER char _char_bufferDMA1[200];                      // DMA Buffer
DMA_BUFFER char _char_bufferDMA2[200];                      // DMA Buffer2
DMA_BUFFER char _spi_buffer_TX[SPI_BUFFER_LENGTH];          // DMA Buffer2
DMA_BUFFER volatile char _spi_buffer_RX[SPI_BUFFER_LENGTH]; // DMA Buffer2

uint8_t _spi_rx_flag = 0;
uint8_t _print_tx_flag = 0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void) {
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
    MX_ETH_Init();
    MX_USART3_UART_Init();
    MX_USB_OTG_FS_PCD_Init();
    MX_TIM2_Init();
    MX_TIM5_Init();
    MX_USART2_Init();
    MX_SPI1_Init();
    /* USER CODE BEGIN 2 */

    // callbacks for HAL_UART_Transmit_IT function

    // start timers
    HAL_TIM_Base_Start(&htim2);
    HAL_TIM_Base_Start(&htim5);

    HAL_SPI_Receive_DMA(&hspi1, (uint8_t *)_spi_buffer_RX, SPI_BUFFER_LENGTH);

    /* USER CODE END 2 */

    /* Infinite loop */
    /* USER CODE BEGIN WHILE */
    while (1) {

        static uint32_t timerMicros = 0;
        static uint32_t timerMillis = 0;

        // once every second
        if (__HAL_TIM_GetCounter(&htim5) / 2 - timerMillis > 1000) {

            timerMicros = __HAL_TIM_GetCounter(&htim2);
            timerMillis = __HAL_TIM_GetCounter(&htim5) / 2;

            // prepare first string
            std::string output = "Timer1: ";
            output.append(std::to_string(timerMicros));
            output.append(", Timer2: ");
            output.append(std::to_string(timerMillis));
            output.append("\r\n");
            strcpy(_char_bufferDMA1, output.data());

            // prepare second string
            strcpy(_char_bufferDMA2, "DMA 2 online\r\n");

            print((uint8_t *)_char_bufferDMA1, output.size());

            // print((uint8_t *)_char_bufferDMA2, strlen(_char_bufferDMA2));

            // test for callbacks
            HAL_USART_Transmit_DMA(&husart2, (uint8_t *)_char_bufferDMA2, strlen(_char_bufferDMA2));

            // HAL_Delay(100);
            // send SPI
            memset(_spi_buffer_TX, 0b01010101, SPI_BUFFER_LENGTH);

            // print((uint8_t *)_spi_buffer_TX, SPI_BUFFER_LENGTH);

            HAL_SPI_Transmit_DMA(&hspi1, (uint8_t *)_spi_buffer_TX, SPI_BUFFER_LENGTH);
        }

        if (_spi_rx_flag) {
            _spi_rx_flag = 0;
            if (_spi_buffer_RX[0] == 0b11001100) {
                HAL_GPIO_TogglePin(LD3_GPIO_Port, LD3_Pin);
            }
        }

        /* USER CODE END WHILE */

        /* USER CODE BEGIN 3 */
    }
    /* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void) {
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
    RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

    /** Supply configuration update enable
     */
    HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);
    /** Configure the main internal regulator output voltage
     */
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    while (!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {
    }
    /** Initializes the RCC Oscillators according to the specified parameters
     * in the RCC_OscInitTypeDef structure.
     */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM = 1;
    RCC_OscInitStruct.PLL.PLLN = 96;
    RCC_OscInitStruct.PLL.PLLP = 2;
    RCC_OscInitStruct.PLL.PLLQ = 16;
    RCC_OscInitStruct.PLL.PLLR = 2;
    RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_3;
    RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
    RCC_OscInitStruct.PLL.PLLFRACN = 0;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
        Error_Handler();
    }
    /** Initializes the CPU, AHB and APB buses clocks
     */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2 |
                                  RCC_CLOCKTYPE_D3PCLK1 | RCC_CLOCKTYPE_D1PCLK1;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV4;
    RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV1;
    RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK) {
        Error_Handler();
    }
    PeriphClkInitStruct.PeriphClockSelection =
        RCC_PERIPHCLK_USART3 | RCC_PERIPHCLK_USART2 | RCC_PERIPHCLK_SPI1 | RCC_PERIPHCLK_USB;
    PeriphClkInitStruct.Spi123ClockSelection = RCC_SPI123CLKSOURCE_PLL;
    PeriphClkInitStruct.Usart234578ClockSelection = RCC_USART234578CLKSOURCE_D2PCLK1;
    PeriphClkInitStruct.UsbClockSelection = RCC_USBCLKSOURCE_PLL;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK) {
        Error_Handler();
    }
    /** Enable USB Voltage detector
     */
    HAL_PWREx_EnableUSBVoltageDetector();
}

/* USER CODE BEGIN 4 */

void print(uint8_t *str, uint16_t size) {
    while (_print_tx_flag == 1) {
    }
    _print_tx_flag = 1;
    HAL_UART_Transmit_DMA(&huart3, str, size);
}

// example callbacks for DMA transfer of data
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance == huart3.Instance) {
        // toggle pin to show callback is working
        _print_tx_flag = 0;
        HAL_GPIO_TogglePin(LD1_GPIO_Port, LD1_Pin);
    }
}

void HAL_UART_TxHalfCpltCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance == huart3.Instance) {
        // do sth
    }
}

void HAL_USART_TxCpltCallback(USART_HandleTypeDef *husart) {
    if (husart->Instance == husart2.Instance) {

        // do sth
    }
}

void HAL_USART_RxCpltCallback(USART_HandleTypeDef *husart) {
    if (husart->Instance == husart2.Instance) {
        // do sth
    }
}

void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi) {
    if (hspi->Instance == hspi1.Instance) {
        _spi_rx_flag = 1;
    }
}

void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi) {
    if (hspi->Instance == hspi1.Instance) {
        // toggle pin to show callback is working
        HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
    }
}
/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void) {
    /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state */

    /* USER CODE END Error_Handler_Debug */
}

#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line) {
    /* USER CODE BEGIN 6 */
    /* User can add his own implementation to report the file name and line number,
       tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
    /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
