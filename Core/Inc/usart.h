/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usart.h
  * @brief   This file contains all the function prototypes for
  *          the usart.c file
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
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USART_H__
#define __USART_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */
extern uint8_t Receice_Down_Data;
/* USER CODE END Includes */

extern UART_HandleTypeDef hlpuart1;

extern UART_HandleTypeDef huart1;

extern UART_HandleTypeDef huart2;

/* USER CODE BEGIN Private defines */


    /** 与模块相连接的串口发送函数，按参数中指定的长度发送串口数据 */
#define UART_TO_MODULE_WRITE_DATA(buffer, size)		lpusart1_send_data(buffer, size)

    /** 与模块相连接的串口发送函数，不指定长度，遇\0结束 */
#define UART_TO_LRM_WRITE_STRING(buffer)			lpusart1_send_string(buffer)

    /** 与模块相连接的串口接收缓冲，目前串口接收数据仅支持中断加BUFFER的形式，不支持FIFO形式 */
#define UART_TO_LRM_RECEIVE_BUFFER					LPUsart1_RX.RX_Buf

    /** 与模块相连接的串口接收到数据的标致 */
#define UART_TO_LRM_RECEIVE_FLAG					LPUsart1_RX.receive_flag

    /** 与模块相连接的串口本次接收到数据长度 */
#define UART_TO_LRM_RECEIVE_LENGTH					LPUsart1_RX.rx_len

    /*==========================END OF LPUART1 CONFIG================================*/

    /** 与模块相连接的串口发送函数，按参数中指定的长度发送串口数据 */
#define UART_TO_PC_WRITE_DATA(buffer, size)			usart2_send_data(buffer, size)

    /** 与模块相连接的串口发送函数，不指定长度，遇\0结束 */
#define UART_TO_PC_WRITE_STRING(buffer)				usart2_send_string(buffer)

    /** 与模块相连接的串口接收缓冲，目前串口接收数据仅支持中断加BUFFER的形式，不支持FIFO形式 */
#define UART_TO_PC_RECEIVE_BUFFER					Usart2_RX.RX_Buf

    /** 与模块相连接的串口接收到数据的标致 */
#define UART_TO_PC_RECEIVE_FLAG						Usart2_RX.receive_flag

    /** 与模块相连接的串口本次接收到数据长度 */
#define UART_TO_PC_RECEIVE_LENGTH					Usart2_RX.rx_len

    /*==========================END OF USART2 CONFIG================================*/
#define RECEIVELEN 1024
#define USART_DMA_SENDING 1
#define USART_DMA_SENDOVER 0

typedef struct
{
		uint8_t receive_flag;
		uint8_t dmaSend_flag;
		uint16_t rx_len;
		uint8_t RX_Buf[RECEIVELEN];
} usart_recv_t;

extern usart_recv_t Usart1_RX;
extern usart_recv_t Usart2_RX;
extern usart_recv_t LPUsart1_RX;
void usart2_send_byte(uint8_t data);
void usart2_send_data(uint8_t *pdata, uint16_t length);
void usart2_send_string(uint8_t *str);
void usart2_send_onenumber(uint8_t data);
void usart2_send_numbers(uint8_t data);
void USART2_Clear_IT(void);
void usart2_receive_idle(void);
void lpusart1_send_byte(uint8_t data);
void lpusart1_send_data(uint8_t *pdata, uint16_t length);
void lpusart1_send_string(uint8_t *pdata);
void lpusart1_receive_idle(void);
void LPUART1_Clear_IT(void);
void lpuart1_reconfig(uint32_t baudrate);
void PrepareLPUARTToStopMode(void);
void debug_printf(char *fmt, ...);
void Usart1Receive_IDLE(void);
void Usart1SendData(uint8_t *pdata, uint16_t Length);
void UART1_SendByte(uint8_t data);
void USART1_SendString(uint8_t *str);
void Clear_UART1_IT(void);
/* USER CODE END Private defines */

void MX_LPUART1_UART_Init(void);
void MX_USART1_UART_Init(void);
void MX_USART2_UART_Init(void);

/* USER CODE BEGIN Prototypes */

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __USART_H__ */

