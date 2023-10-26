/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usart.c
  * @brief   This file provides code for the configuration
  *          of the USART instances.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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
#include "usart.h"

/* USER CODE BEGIN 0 */
#include <stdarg.h>
usart_recv_t Usart1_RX;
usart_recv_t Usart2_RX;
usart_recv_t LPUsart1_RX;
/* USER CODE END 0 */

UART_HandleTypeDef hlpuart1;
UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;
DMA_HandleTypeDef hdma_lpuart_rx;
DMA_HandleTypeDef hdma_usart1_rx;
DMA_HandleTypeDef hdma_usart2_rx;

/* LPUART1 init function */

void MX_LPUART1_UART_Init(void)
{

  /* USER CODE BEGIN LPUART1_Init 0 */

  /* USER CODE END LPUART1_Init 0 */

  /* USER CODE BEGIN LPUART1_Init 1 */

  /* USER CODE END LPUART1_Init 1 */
  hlpuart1.Instance = LPUART1;
  hlpuart1.Init.BaudRate = 9600;
  hlpuart1.Init.WordLength = UART_WORDLENGTH_8B;
  hlpuart1.Init.StopBits = UART_STOPBITS_1;
  hlpuart1.Init.Parity = UART_PARITY_NONE;
  hlpuart1.Init.Mode = UART_MODE_TX_RX;
  hlpuart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  hlpuart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  hlpuart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&hlpuart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN LPUART1_Init 2 */

  /* USER CODE END LPUART1_Init 2 */

}
/* USART1 init function */

void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 9600;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}
/* USART2 init function */

void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};
  if(uartHandle->Instance==LPUART1)
  {
  /* USER CODE BEGIN LPUART1_MspInit 0 */

  /* USER CODE END LPUART1_MspInit 0 */

  /** Initializes the peripherals clock
  */
    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_LPUART1;
    PeriphClkInit.Lpuart1ClockSelection = RCC_LPUART1CLKSOURCE_HSI;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
    {
      Error_Handler();
    }

    /* LPUART1 clock enable */
    __HAL_RCC_LPUART1_CLK_ENABLE();

    __HAL_RCC_GPIOC_CLK_ENABLE();
    /**LPUART1 GPIO Configuration
    PC0     ------> LPUART1_RX
    PC1     ------> LPUART1_TX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF8_LPUART1;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    /* LPUART1 DMA Init */
    /* LPUART_RX Init */
    hdma_lpuart_rx.Instance = DMA2_Channel7;
    hdma_lpuart_rx.Init.Request = DMA_REQUEST_4;
    hdma_lpuart_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_lpuart_rx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_lpuart_rx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_lpuart_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_lpuart_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_lpuart_rx.Init.Mode = DMA_NORMAL;
    hdma_lpuart_rx.Init.Priority = DMA_PRIORITY_LOW;
    if (HAL_DMA_Init(&hdma_lpuart_rx) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(uartHandle,hdmarx,hdma_lpuart_rx);

    /* LPUART1 interrupt Init */
    HAL_NVIC_SetPriority(LPUART1_IRQn, 1, 0);
    HAL_NVIC_EnableIRQ(LPUART1_IRQn);
  /* USER CODE BEGIN LPUART1_MspInit 1 */

  /* USER CODE END LPUART1_MspInit 1 */
  }
  else if(uartHandle->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspInit 0 */

  /* USER CODE END USART1_MspInit 0 */

  /** Initializes the peripherals clock
  */
    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1;
    PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
    {
      Error_Handler();
    }

    /* USART1 clock enable */
    __HAL_RCC_USART1_CLK_ENABLE();

    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**USART1 GPIO Configuration
    PB6     ------> USART1_TX
    PB7     ------> USART1_RX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* USART1 DMA Init */
    /* USART1_RX Init */
    hdma_usart1_rx.Instance = DMA1_Channel5;
    hdma_usart1_rx.Init.Request = DMA_REQUEST_2;
    hdma_usart1_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_usart1_rx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_usart1_rx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_usart1_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_usart1_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_usart1_rx.Init.Mode = DMA_NORMAL;
    hdma_usart1_rx.Init.Priority = DMA_PRIORITY_LOW;
    if (HAL_DMA_Init(&hdma_usart1_rx) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(uartHandle,hdmarx,hdma_usart1_rx);

    /* USART1 interrupt Init */
    HAL_NVIC_SetPriority(USART1_IRQn, 1, 0);
    HAL_NVIC_EnableIRQ(USART1_IRQn);
  /* USER CODE BEGIN USART1_MspInit 1 */

  /* USER CODE END USART1_MspInit 1 */
  }
  else if(uartHandle->Instance==USART2)
  {
  /* USER CODE BEGIN USART2_MspInit 0 */

  /* USER CODE END USART2_MspInit 0 */

  /** Initializes the peripherals clock
  */
    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART2;
    PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
    {
      Error_Handler();
    }

    /* USART2 clock enable */
    __HAL_RCC_USART2_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**USART2 GPIO Configuration
    PA2     ------> USART2_TX
    PA3     ------> USART2_RX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_3;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* USART2 DMA Init */
    /* USART2_RX Init */
    hdma_usart2_rx.Instance = DMA1_Channel6;
    hdma_usart2_rx.Init.Request = DMA_REQUEST_2;
    hdma_usart2_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_usart2_rx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_usart2_rx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_usart2_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_usart2_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_usart2_rx.Init.Mode = DMA_NORMAL;
    hdma_usart2_rx.Init.Priority = DMA_PRIORITY_LOW;
    if (HAL_DMA_Init(&hdma_usart2_rx) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(uartHandle,hdmarx,hdma_usart2_rx);

    /* USART2 interrupt Init */
    HAL_NVIC_SetPriority(USART2_IRQn, 1, 0);
    HAL_NVIC_EnableIRQ(USART2_IRQn);
  /* USER CODE BEGIN USART2_MspInit 1 */

  /* USER CODE END USART2_MspInit 1 */
  }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle)
{

  if(uartHandle->Instance==LPUART1)
  {
  /* USER CODE BEGIN LPUART1_MspDeInit 0 */

  /* USER CODE END LPUART1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_LPUART1_CLK_DISABLE();

    /**LPUART1 GPIO Configuration
    PC0     ------> LPUART1_RX
    PC1     ------> LPUART1_TX
    */
    HAL_GPIO_DeInit(GPIOC, GPIO_PIN_0|GPIO_PIN_1);

    /* LPUART1 DMA DeInit */
    HAL_DMA_DeInit(uartHandle->hdmarx);

    /* LPUART1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(LPUART1_IRQn);
  /* USER CODE BEGIN LPUART1_MspDeInit 1 */

  /* USER CODE END LPUART1_MspDeInit 1 */
  }
  else if(uartHandle->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspDeInit 0 */

  /* USER CODE END USART1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART1_CLK_DISABLE();

    /**USART1 GPIO Configuration
    PB6     ------> USART1_TX
    PB7     ------> USART1_RX
    */
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_6|GPIO_PIN_7);

    /* USART1 DMA DeInit */
    HAL_DMA_DeInit(uartHandle->hdmarx);

    /* USART1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(USART1_IRQn);
  /* USER CODE BEGIN USART1_MspDeInit 1 */

  /* USER CODE END USART1_MspDeInit 1 */
  }
  else if(uartHandle->Instance==USART2)
  {
  /* USER CODE BEGIN USART2_MspDeInit 0 */

  /* USER CODE END USART2_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART2_CLK_DISABLE();

    /**USART2 GPIO Configuration
    PA2     ------> USART2_TX
    PA3     ------> USART2_RX
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_2|GPIO_PIN_3);

    /* USART2 DMA DeInit */
    HAL_DMA_DeInit(uartHandle->hdmarx);

    /* USART2 interrupt Deinit */
    HAL_NVIC_DisableIRQ(USART2_IRQn);
  /* USER CODE BEGIN USART2_MspDeInit 1 */

  /* USER CODE END USART2_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */
//--------------------UART2---------------------------------
void MX_LPUART1_Init(uint32_t baudrate)
{

    hlpuart1.Instance = LPUART1;
    hlpuart1.Init.BaudRate = baudrate;
    hlpuart1.Init.WordLength = UART_WORDLENGTH_8B;
    hlpuart1.Init.StopBits = UART_STOPBITS_1;
    hlpuart1.Init.Parity = UART_PARITY_NONE;
    hlpuart1.Init.Mode = UART_MODE_TX_RX;
    hlpuart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    hlpuart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
    hlpuart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
    if (HAL_UART_Init(&hlpuart1) != HAL_OK)
    {
        Error_Handler();
    }

}
/* USART1 init function */

void MX_USART1_Init(uint32_t baudrate)
{

    GPIO_InitTypeDef GPIO_InitStruct;

    __HAL_RCC_GPIOB_CLK_ENABLE();

    GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
//    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
//    GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);


//    huart1.Instance = USART1;
//    huart1.Init.BaudRate = baudrate;
//    huart1.Init.WordLength = UART_WORDLENGTH_8B;
//    huart1.Init.StopBits = UART_STOPBITS_1;
//    huart1.Init.Parity = UART_PARITY_NONE;
//    huart1.Init.Mode = UART_MODE_TX_RX;
//    huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
//    huart1.Init.OverSampling = UART_OVERSAMPLING_16;
//    huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
//    huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
//    if (HAL_UART_Init(&huart1) != HAL_OK)
//    {
//        Error_Handler();
//    }

}
/* USART2 init function */

void MX_USART2_Init(uint32_t baudrate)
{

    huart2.Instance = USART2;
    huart2.Init.BaudRate = baudrate;
    huart2.Init.WordLength = UART_WORDLENGTH_8B;
    huart2.Init.StopBits = UART_STOPBITS_1;
    huart2.Init.Parity = UART_PARITY_NONE;
    huart2.Init.Mode = UART_MODE_TX_RX;
    huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart2.Init.OverSampling = UART_OVERSAMPLING_16;
    huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
    huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
    if (HAL_UART_Init(&huart2) != HAL_OK)
    {
        Error_Handler();
    }
}

void usart2_send_byte(uint8_t data)
{
    while((USART2->ISR&UART_FLAG_TXE)!=UART_FLAG_TXE);//?????????
    USART2->TDR = data;
    while((USART2->ISR&UART_FLAG_TC)!=UART_FLAG_TC);//?????????
}

void usart2_send_data(uint8_t *pdata, uint16_t length)
{
    uint32_t i = 0;

    for (i = 0; i < length; i++)
    {
        usart2_send_byte(pdata[i]);
    }
}

void usart2_send_string(uint8_t *str)
{
    while((*str)!=0)
    {
        while((USART2->ISR&UART_FLAG_TXE)!=UART_FLAG_TXE);//????????
        USART2->TDR = *str++;
        while((USART2->ISR&UART_FLAG_TC)!=UART_FLAG_TC);//?????????
    }
}

void usart2_send_onenumber(uint8_t data)
{
    uint8_t tmpl = data & 0x0f;
    while((USART2->ISR&UART_FLAG_TXE)!=UART_FLAG_TXE);//????????
    USART2->TDR = (tmpl+'0');
    while((USART2->ISR&UART_FLAG_TC)!=UART_FLAG_TC);//?????????
}

void usart2_send_numbers(uint8_t data)
{
    uint8_t i = 0;
    uint8_t tmph = 0,tmpm = 0,tmpl = 0;
    tmph = data/100;
    usart2_send_onenumber(tmph);
    tmpm = data%100/10;
    usart2_send_onenumber(tmpm);
    tmpl = data%10;
    usart2_send_onenumber(tmpl);
}

void USART2_Clear_IT(void)
{
    __HAL_UART_CLEAR_FLAG(&huart2,UART_FLAG_IDLE);               //清除空闲中断标志
    HAL_Delay(2);
    __HAL_UART_CLEAR_FLAG(&huart2,UART_FLAG_TC);                 //清除发送标志
    HAL_UART_Receive_DMA(&huart2, Usart2_RX.RX_Buf, RECEIVELEN); //开启DMA接收
    __HAL_UART_ENABLE_IT(&huart2, UART_IT_IDLE);                 //使能空闲中断
    __HAL_UART_CLEAR_FLAG(&huart2,UART_FLAG_IDLE);               //清除空闲中断标志
}

//串口接收空闲中断
void usart2_receive_idle(void)
{
    uint32_t temp;

    if((__HAL_UART_GET_FLAG(&huart2,UART_FLAG_IDLE) != RESET))
    {
        __HAL_UART_CLEAR_IDLEFLAG(&huart2);
        HAL_UART_DMAStop(&huart2);
        temp = huart2.hdmarx->Instance->CNDTR;
        Usart2_RX.rx_len = RECEIVELEN - temp;
        Usart2_RX.receive_flag = 1;
        HAL_UART_Receive_DMA(&huart2,Usart2_RX.RX_Buf,RECEIVELEN);
    }
}

//--------------------UART1---------------------------------

void Usart1Receive_IDLE(void)
{
    uint32_t temp;

    if((__HAL_UART_GET_FLAG(&huart1,UART_FLAG_IDLE) != RESET))
    {
        __HAL_UART_CLEAR_IDLEFLAG(&huart1);
        HAL_UART_DMAStop(&huart1);
        temp = huart1.hdmarx->Instance->CNDTR;
        Usart1_RX.rx_len =  RECEIVELEN - temp;
        Usart1_RX.receive_flag=1;
        HAL_UART_Receive_DMA(&huart1,Usart1_RX.RX_Buf,RECEIVELEN);
    }
}

void Usart1SendData(uint8_t *pdata, uint16_t Length)
{
    uint32_t i = 0;

    for (i = 0; i < Length; i++)
    {
        UART1_SendByte(pdata[i]);
    }
}

void UART1_SendByte(uint8_t data)
{
    while((USART1->ISR&UART_FLAG_TXE)!=UART_FLAG_TXE);//?????????
    USART1->TDR = data;
}

void USART1_SendString(uint8_t *str)
{
    while((*str)!=0)
    {
        while((USART1->ISR&UART_FLAG_TXE)!=UART_FLAG_TXE);//?????????
        USART1->TDR = *str++;
    }
}


void Clear_UART1_IT(void)
{
    __HAL_UART_CLEAR_FLAG(&huart1,UART_FLAG_IDLE);               //????????
    HAL_Delay(2);
    __HAL_UART_CLEAR_FLAG(&huart1,UART_FLAG_TC);                 //??????
    HAL_UART_Receive_DMA(&huart1, Usart1_RX.RX_Buf, RECEIVELEN); //??DMA??
    __HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE);                 //??????
    __HAL_UART_CLEAR_FLAG(&huart1,UART_FLAG_IDLE);               //????????
}


//--------------------LPUART1---------------------------------

void lpusart1_send_byte(uint8_t data)
{
    while((LPUART1->ISR&UART_FLAG_TXE)!=UART_FLAG_TXE);//?????????
    LPUART1->TDR = data;
    while((LPUART1->ISR&UART_FLAG_TC)!=UART_FLAG_TC);//?????????
}

void lpusart1_send_data(uint8_t *pdata, uint16_t length)
{
    uint32_t i = 0;

    for (i = 0; i < length; i++)
    {
        lpusart1_send_byte(pdata[i]);
    }
}

void lpusart1_send_string(uint8_t *pdata)
{
    while((*pdata)!=0)
    {
        lpusart1_send_byte(*(pdata++));
    }
}

void lpusart1_receive_idle(void)
{
    static uint32_t temp = 0 ;

    if((__HAL_UART_GET_FLAG(&hlpuart1,UART_FLAG_IDLE) != RESET))
    {
//        SET_BIT(LPUART1->ICR , UART_FLAG_IDLE);
        __HAL_UART_CLEAR_IDLEFLAG(&hlpuart1);
        HAL_UART_DMAStop(&hlpuart1);
        temp = hlpuart1.hdmarx->Instance->CNDTR;
        LPUsart1_RX.rx_len =  RECEIVELEN - temp;
        LPUsart1_RX.receive_flag = 1;
        Receice_Down_Data = 1;
        HAL_UART_Receive_DMA(&hlpuart1,LPUsart1_RX.RX_Buf,RECEIVELEN);
    }
}

void LPUART1_Clear_IT(void)
{
    __HAL_UART_CLEAR_FLAG(&hlpuart1,UART_CLEAR_IDLEF);               //????????
    HAL_Delay(2);
    __HAL_UART_CLEAR_FLAG(&hlpuart1,UART_CLEAR_TCF);                 //??????
    HAL_UART_Receive_DMA(&hlpuart1, LPUsart1_RX.RX_Buf, RECEIVELEN); //??DMA??
    __HAL_UART_ENABLE_IT(&hlpuart1, UART_IT_IDLE);                 //??????
    __HAL_UART_CLEAR_FLAG(&hlpuart1,UART_CLEAR_IDLEF);               //????????
}

//------------------------------------------------------------------------------------------------

void PrepareLPUARTToStopMode(void)
{

    /* Clear OVERRUN flag */
    WRITE_REG(LPUART1->ICR, USART_ICR_ORECF);

    /* Configure LPUART1 transfer interrupts : */
    /* Clear WUF flag and enable the UART Wake Up from stop mode Interrupt */
    WRITE_REG(LPUART1->ICR, USART_ICR_WUCF);
    SET_BIT(LPUART1->CR3, USART_CR3_WUFIE);

    /* Enable Wake Up From Stop */
    SET_BIT(LPUART1->CR1, USART_CR1_UESM);
}

//-----------------------------------------------------------------------------------------
void lpuart1_reconfig(uint32_t baudrate)
{
    //	HAL_NVIC_DisableIRQ(LPUART1_IRQn);
    //	HAL_UART_MspDeInit(&hlpuart1);
    MX_LPUART1_Init(baudrate);
}

void debug_printf(char *fmt, ...)
{
    uint8_t buf[1024];
    va_list va_args;

    // Start the varargs processing.
    va_start(va_args, fmt);

    vsnprintf((char *)buf, sizeof(buf), fmt, va_args);

    // End the varargs processing.
    va_end(va_args);

    /*
    * ?????????,??????
    */
    //    Serial_PutString(buf);
    usart2_send_string(buf);
}
/* USER CODE END 1 */
