#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "app.h"
#include "usart.h"
#include "gpio.h"
#include "lorawan_node_driver.h"
#include "hdc1000.h"
#include "opt3001.h"
#include "MPL3115.h"
#include "sensors_test.h"
#include "common.h"
#include "ST7789v.h"

extern DEVICE_MODE_T device_mode;
extern DEVICE_MODE_T *Device_Mode_str;
down_list_t *pphead = NULL;

uint16_t Tim3_Counter = 0;
uint8_t *DevEui = "009569000000F554";
uint8_t UP_DataBuf_str[28] = {0};
uint8_t UP_DataCnt = 0;
uint8_t UP_DataBuf[9] = {0xAA, 0xF5, 0x54, 0, 0, 0, 0, 0, 0x0F};
uint8_t UP_Data_Status[26] = {0};
uint8_t DN_Data_Buf[1024] = {0};
uint8_t Receice_Down_Data;

extern int Tim3_Sensors_Delay_Secend = 1;
extern int SenSors_Data_Buf_Num = 5;
int SensorsCnt = 0;
SensorsData_t SensorsData;
SENSOR_Type_t SensorType = LUX_SENSOR;

GUI_Switch_t GUI_Now;

//-----------------Users application--------------------------
void LoRaWAN_Func_Process(void)
{
    static DEVICE_MODE_T dev_stat = NO_MODE;

    uint16_t temper = 0;

    switch ((uint8_t)device_mode)
    {
    /* 指令模式 */
    case CMD_CONFIG_MODE:
    {
        /* 如果不是command Configuration function, 则进入if语句,只执行一次 */
        if (dev_stat != CMD_CONFIG_MODE)
        {
            dev_stat = CMD_CONFIG_MODE;
            debug_printf("\r\n[Command Mode]\r\n");
            LCD_Clear(WHITE);
            LCD_ShowString(5 + (8 * 8), 5, "[Command Mode]", BLUE);

            nodeGpioConfig(wake, wakeup);
            nodeGpioConfig(mode, command);
        }
        /* 等待usart2产生中断 */
        if (UART_TO_PC_RECEIVE_FLAG)
        {
            UART_TO_PC_RECEIVE_FLAG = 0;
            lpusart1_send_data(UART_TO_PC_RECEIVE_BUFFER, UART_TO_PC_RECEIVE_LENGTH);
        }
        /* 等待lpuart1产生中断 */
        if (UART_TO_LRM_RECEIVE_FLAG)
        {
            UART_TO_LRM_RECEIVE_FLAG = 0;
            usart2_send_data(UART_TO_LRM_RECEIVE_BUFFER, UART_TO_LRM_RECEIVE_LENGTH);
        }
    }
    break;

    /* 透传模式 */
    case DATA_TRANSPORT_MODE:
    {
        /* 如果不是data transport function,则进入if语句,只执行一次 */
        if (dev_stat != DATA_TRANSPORT_MODE)
        {
            dev_stat = DATA_TRANSPORT_MODE;
            debug_printf("\r\n[Transperant Mode]\r\n");
            LCD_Clear(WHITE);
            LCD_ShowString(5 + (6 * 8), 5, "[Transperant Mode]", BLUE);

            /* 模块入网判断 */
            if (nodeJoinNet(JOIN_TIME_120_SEC) == false)
            {
                return;
            }

            LCD_ShowString(5 + (6 * 8), 5 + (9 * 16), "Join seccussfully", BLUE);

            temper = HDC1000_Read_Temper() / 1000;

            nodeDataCommunicate((uint8_t *)&temper, sizeof(temper), &pphead);
        }

        /* 等待usart2产生中断 */
        if (UART_TO_PC_RECEIVE_FLAG && GET_BUSY_LEVEL) // Ensure BUSY is high before sending data
        {
            UART_TO_PC_RECEIVE_FLAG = 0;
            nodeDataCommunicate((uint8_t *)UART_TO_PC_RECEIVE_BUFFER, UART_TO_PC_RECEIVE_LENGTH, &pphead);
        }

        /* 如果模块正忙, 则发送数据无效，并给出警告信息 */
        else if (UART_TO_PC_RECEIVE_FLAG && (GET_BUSY_LEVEL == 0))
        {
            UART_TO_PC_RECEIVE_FLAG = 0;
            debug_printf("-. Warning: Don't send data now! Module is busy!\r\n");
        }

        /* 等待lpuart1产生中断 */
        if (UART_TO_LRM_RECEIVE_FLAG)
        {
            UART_TO_LRM_RECEIVE_FLAG = 0;
            usart2_send_data(UART_TO_LRM_RECEIVE_BUFFER, UART_TO_LRM_RECEIVE_LENGTH);
        }
    }
    break;

    /*工程模式*/
    case PRO_TRAINING_MODE:
    {
        /* 如果不是工程模式, 则进入if语句,只执行一次 */
        if (dev_stat != PRO_TRAINING_MODE)
        {
            dev_stat = PRO_TRAINING_MODE;
            debug_printf("\r\n[Project Mode]\r\n");
            GUI_Show(GUI_Now);

            SensorsCnt = 0;
            Receice_Down_Data = 0;
        }

        /* 你的实验代码位置 */
        if (Tim3_Counter == Tim3_Sensors_Delay_Secend * 100)
        {
            // 开始获取传感器数值
            if (SensorsCnt == 0)
            {
                memset(&SensorsData, 0, sizeof(SensorsData_t));
                // 初始化结构体变量
                SensorsData.Min.Lux_OPT3001 = 0x7fffffff;
                SensorsData.Min.Pressure_MPL3115 = 0x7fffffff;
                SensorsData.Min.Temper_HDC1000 = 0xffff;
                SensorsData.Min.Humidi_HDC1000 = 0xffff;
            }

            SensorsData.Data.Lux_OPT3001 = OPT3001_Get_Lux();
            SensorsData.Data.Pressure_MPL3115 = MPL3115_ReadPressure();
            SensorsData.Data.Temper_HDC1000 = HDC1000_Read_Temper();
            SensorsData.Data.Humidi_HDC1000 = HDC1000_Read_Humidi();

            // 求最大值
            // clang-format off
            SensorsData.Data.Lux_OPT3001        >=  SensorsData.Max.Lux_OPT3001 
                                                 ?  SensorsData.Max.Lux_OPT3001 = SensorsData.Data.Lux_OPT3001
                                                 :  NULL; 
            SensorsData.Data.Pressure_MPL3115   >=  SensorsData.Max.Pressure_MPL3115 
                                                 ?  SensorsData.Max.Pressure_MPL3115 = SensorsData.Data.Pressure_MPL3115
                                                 :  NULL; 
            SensorsData.Data.Temper_HDC1000     >=  SensorsData.Max.Temper_HDC1000 
                                                 ?  SensorsData.Max.Temper_HDC1000 = SensorsData.Data.Temper_HDC1000
                                                 :  NULL; 
            SensorsData.Data.Humidi_HDC1000     >=  SensorsData.Max.Humidi_HDC1000 
                                                 ?  SensorsData.Max.Humidi_HDC1000 = SensorsData.Data.Humidi_HDC1000 
                                                 :  NULL; 
            // 求最小值
            SensorsData.Data.Lux_OPT3001        <=  SensorsData.Min.Lux_OPT3001 
                                                 ?  SensorsData.Min.Lux_OPT3001 = SensorsData.Data.Lux_OPT3001
                                                 :  NULL; 
            SensorsData.Data.Pressure_MPL3115   <=  SensorsData.Min.Pressure_MPL3115 
                                                 ?  SensorsData.Min.Pressure_MPL3115 = SensorsData.Data.Pressure_MPL3115
                                                 :  NULL; 
            SensorsData.Data.Temper_HDC1000     <=  SensorsData.Min.Temper_HDC1000 
                                                 ?  SensorsData.Min.Temper_HDC1000 = SensorsData.Data.Temper_HDC1000
                                                 :  NULL; 
            SensorsData.Data.Humidi_HDC1000     <=  SensorsData.Min.Humidi_HDC1000 
                                                 ?  SensorsData.Min.Humidi_HDC1000 = SensorsData.Data.Humidi_HDC1000 
                                                 :  NULL;
            // clang-format on
            // 将数据累加进Average中暂存
            SensorsData.Average.Lux_OPT3001 += SensorsData.Data.Lux_OPT3001;
            SensorsData.Average.Pressure_MPL3115 += SensorsData.Data.Pressure_MPL3115;
            SensorsData.Average.Temper_HDC1000 += SensorsData.Data.Temper_HDC1000;
            SensorsData.Average.Humidi_HDC1000 += SensorsData.Data.Humidi_HDC1000;

            // 获取传感器数值结束，数据处理
            if (SensorsCnt == SenSors_Data_Buf_Num - 1)
            {
                SensorsData.Average.Lux_OPT3001 -= SensorsData.Max.Lux_OPT3001;
                SensorsData.Average.Pressure_MPL3115 -= SensorsData.Max.Pressure_MPL3115;
                SensorsData.Average.Temper_HDC1000 -= SensorsData.Max.Temper_HDC1000;
                SensorsData.Average.Humidi_HDC1000 -= SensorsData.Max.Humidi_HDC1000;

                SensorsData.Average.Lux_OPT3001 -= SensorsData.Min.Lux_OPT3001;
                SensorsData.Average.Pressure_MPL3115 -= SensorsData.Min.Pressure_MPL3115;
                SensorsData.Average.Temper_HDC1000 -= SensorsData.Min.Temper_HDC1000;
                SensorsData.Average.Humidi_HDC1000 -= SensorsData.Min.Humidi_HDC1000;

                SensorsData.Average.Lux_OPT3001 /= (SenSors_Data_Buf_Num - 2);
                SensorsData.Average.Pressure_MPL3115 /= (SenSors_Data_Buf_Num - 2);
                SensorsData.Average.Temper_HDC1000 /= (SenSors_Data_Buf_Num - 2);
                SensorsData.Average.Humidi_HDC1000 /= (SenSors_Data_Buf_Num - 2);

                debug_printf("\r\n平均数据:%f, %f, %d, %d\r\n",
                             SensorsData.Average.Lux_OPT3001,
                             SensorsData.Average.Pressure_MPL3115,
                             SensorsData.Average.Temper_HDC1000,
                             SensorsData.Average.Humidi_HDC1000);

                SensorsData.Average.Lux_OPT3001 > 300 ? HAL_GPIO_WritePin(GPIOD, GPIO_PIN_7, GPIO_PIN_RESET)
                                                      : HAL_GPIO_WritePin(GPIOD, GPIO_PIN_7, GPIO_PIN_SET);
                SensorsData.Average.Pressure_MPL3115 > 101000 ? HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_RESET)
                                                      : HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_SET);
                SensorsData.Average.Lux_OPT3001 > 30000 ? HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET)
                                                      : HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
                SensorsData.Average.Lux_OPT3001 > 60000 ? HAL_GPIO_WritePin(GPIOE, GPIO_PIN_0, GPIO_PIN_RESET)
                                                      : HAL_GPIO_WritePin(GPIOE, GPIO_PIN_0, GPIO_PIN_SET);

                int integerPart = 0;
                int decimalPart = 0;
                switch (SensorType)
                {
                case LUX_SENSOR:
                    integerPart = (int)SensorsData.Average.Lux_OPT3001;
                    decimalPart = (int)((SensorsData.Average.Lux_OPT3001 - integerPart) * 100);
                    break;
                case PRESSURE_SNESOR:
                    SensorsData.Average.Pressure_MPL3115 /= 1000;
                    integerPart = (int)SensorsData.Average.Pressure_MPL3115;
                    decimalPart = (int)((SensorsData.Average.Pressure_MPL3115 - integerPart) * 100);
                    break;
                case TEMPER_SENSOR:
                    integerPart = SensorsData.Average.Temper_HDC1000 / 1000;
                    decimalPart = (SensorsData.Average.Temper_HDC1000 % 1000) / 10;
                    break;
                case HUMIDI_SENSOR:
                    integerPart = SensorsData.Average.Humidi_HDC1000 / 1000;
                    decimalPart = (SensorsData.Average.Humidi_HDC1000 % 1000) / 10;
                    break;

                default:
                    break;
                }
                uint8_t integerPart_H = (integerPart & 0xFF00) >> 2;
                uint8_t integerPart_L = integerPart & 0x00FF;

                UP_DataBuf[3] = (uint8_t)SensorType;
                UP_DataBuf[4] = (uint8_t)UP_DataCnt;
                UP_DataBuf[5] = (uint8_t)integerPart_H;
                UP_DataBuf[6] = (uint8_t)integerPart_L;
                UP_DataBuf[7] = (uint8_t)decimalPart;

                sprintf(UP_DataBuf_str, "%02X%02X%02X%02X%02X%02X%02X%02X%02X",
                        UP_DataBuf[0], UP_DataBuf[1], UP_DataBuf[2], UP_DataBuf[3], UP_DataBuf[4],
                        UP_DataBuf[5], UP_DataBuf[6], UP_DataBuf[7], UP_DataBuf[8]);

                UP_DataCnt == 0xFF ? UP_DataCnt = 0 : UP_DataCnt++;

                // for (int i = 0; i < 9; i++)
                //     debug_printf("%02X ", UP_DataBuf[i]);

                execution_status_t comm_status = nodeDataCommunicate(UP_DataBuf, 9, &pphead);
                if (comm_status == NODE_COMM_SUCC)
                    sprintf(UP_Data_Status, "Comm Success.");
                else
                    sprintf(UP_Data_Status, "Comm Fail,err:0x%02X.", comm_status);

                GUI_Show(GUI_Now);
            }

            SensorsCnt == SenSors_Data_Buf_Num - 1 ? SensorsCnt = 0 : SensorsCnt++;
        }

        /* 等待usart2产生中断 */
        if (UART_TO_PC_RECEIVE_FLAG && GET_BUSY_LEVEL) // Ensure BUSY is high before sending data
        {
            UART_TO_PC_RECEIVE_FLAG = 0;
            nodeDataCommunicate((uint8_t *)UART_TO_PC_RECEIVE_BUFFER, UART_TO_PC_RECEIVE_LENGTH, &pphead);
        }

        /* 如果模块正忙, 则发送数据无效，并给出警告信息 */
        else if (UART_TO_PC_RECEIVE_FLAG && (GET_BUSY_LEVEL == 0))
        {
            UART_TO_PC_RECEIVE_FLAG = 0;
            debug_printf("-. Warning: Don't send data now! Module is busy!\r\n");
        }

        /* 等待lpuart1产生中断 */
        if (Receice_Down_Data)
        {
            Receice_Down_Data = 0;

            for (int n = 0; n < UART_TO_LRM_RECEIVE_LENGTH; n++)
            {
                sprintf((DN_Data_Buf + (n * 2)), "%02X", UART_TO_LRM_RECEIVE_BUFFER[n]);
            }

            usart2_send_data(UART_TO_LRM_RECEIVE_BUFFER, UART_TO_LRM_RECEIVE_LENGTH);
            GUI_Show(GUI_Now);
        }
    }
    break;

    default:
        break;
    }
}

/**
 * @brief   开发板版本信息和按键使用说明信息打印
 * @details 上电所有灯会短暂亮100ms
 * @param   无
 * @return  无
 */
void LoRaWAN_Borad_Info_Print(void)
{
    debug_printf("\r\n\r\n");
    PRINT_CODE_VERSION_INFO("%s", CODE_VERSION);
    debug_printf("\r\n");
    debug_printf("-. Press Key1 to: \r\n");
    debug_printf("-.  - Enter command Mode\r\n");
    debug_printf("-.  - Enter Transparent Mode\r\n");
    debug_printf("-. Press Key2 to: \r\n");
    debug_printf("-.  - Enter Project Trainning Mode\r\n");
    LEDALL_ON;
    HAL_Delay(100);
    LEDALL_OFF;
}

void GUI_Show(GUI_Switch_t GUI)
{
    switch (GUI)
    {
    case MAIN_GUI:
        LCD_Clear(WHITE);
        LCD_ShowString(5 + (8 * 8), 5 + (0 * 16), "[Project Mode]", BLUE);
        LCD_ShowString(5 + (0 * 8), 5 + (1 * 16), "[DevEui]:", BLUE);
        LCD_ShowString(5 + (9 * 8), 5 + (1 * 16), DevEui, BLACK);
        LCD_ShowString(5 + (0 * 8), 5 + (3 * 16), "[UP]:", BLUE);
        LCD_ShowString(5 + (5 * 8), 5 + (3 * 16), UP_DataBuf_str, BLACK);
        LCD_ShowString(5 + (0 * 8), 5 + (4 * 16), "[STATUS]:", BLUE);
        LCD_ShowString(5 + (9 * 8), 5 + (4 * 16), UP_Data_Status, RED);
        LCD_ShowString(5 + (0 * 8), 5 + (6 * 16), "[DN]:", BLUE);
        LCD_ShowString(5 + (5 * 8), 5 + (6 * 16), DN_Data_Buf, BLACK);
        break;

    default:
        break;
    }
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM3)
    {
        // 用户代码
        Tim3_Counter == (Tim3_Sensors_Delay_Secend * 100) ? Tim3_Counter = 0 : Tim3_Counter++;
    }
}
