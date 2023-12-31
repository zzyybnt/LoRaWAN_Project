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
#include "XPT2046.h"
#include "InFlash.h"

extern DEVICE_MODE_T device_mode;
extern DEVICE_MODE_T *Device_Mode_str;
down_list_t *pphead = NULL;

uint16_t Tim3_Counter = 0;
uint8_t *DevEui = "009569000000F554";
char UP_DataBuf_str[28] = {0};
uint8_t UP_DataCnt = 0;
uint8_t UP_DataBuf[9] = {0xAA, 0xF5, 0x54, 0, 0, 0, 0, 0, 0x0F};
char UP_Data_Status[26] = {0};
char DN_Data_Buf[1024] = {0};
uint8_t Receice_Down_Data;

extern int Tim3_Sensors_Delay_Secend = 2;
extern int SenSors_Data_Buf_Num = 3;
int SensorsCnt = 0;
SensorsData_t SensorsData;
SENSOR_Type_t SensorType = LUX_SENSOR;
uint8_t SensorType_str[10] = "Lux";

int Tim3_Sensors_Delay_Secend_temp;
int SenSors_Data_Buf_Num_temp;
SENSOR_Type_t SensorType_temp;

extern GUI_Switch_t GUI_Now;
uint8_t MainGui_first_In = 0;
uint8_t ConfigGui_first_In = 0;
uint8_t Config_Touch_Num = 9;
uint8_t Config_select_type = 0;

uint8_t PAGE = 0, TurnPageflag = 1, WriteFlashflag = 0;
uint8_t EndRead = 1, LPflag = 0;
uint8_t FlashBuffer[256], DecodeBuffer[20] = {0};

uint8_t Func_Process_first_In = 1;

//-----------------Users application--------------------------
void LoRaWAN_Func_Process(void)
{
    static DEVICE_MODE_T dev_stat = NO_MODE;

    uint16_t temper = 0;

    if (Func_Process_first_In == 1)
    {
        Func_Process_first_In = 0;
        DecodeFlashData();
    }

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

            MainGui_first_In = 1;
        }

        if (GUI_Now == MAIN_GUI && MainGui_first_In)
        {
            SensorsCnt = 0;
            Receice_Down_Data = 0;
            GUI_Show(GUI_Now);
            Tim3_Sensors_Delay_Secend_temp = Tim3_Sensors_Delay_Secend;
            SenSors_Data_Buf_Num_temp = SenSors_Data_Buf_Num;
            SensorType_temp = SensorType;
            MainGui_first_In = 0;
        }

        if (GUI_Now == PARAM_CONFIG_GUI && ConfigGui_first_In)
        {
            GUI_Show(GUI_Now);
            ConfigGui_first_In = 0;
        }

        /* 你的实验代码位置 */
        if (Tim3_Counter == Tim3_Sensors_Delay_Secend * 100 && GUI_Now != PARAM_CONFIG_GUI)
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

                FlashBuffer[0] = 0XF5;
                FlashBuffer[1] = 0X54;
                FlashBuffer[2] = (uint8_t)SensorType;
                FlashBuffer[3] = (uint8_t)UP_DataCnt;
                FlashBuffer[4] = (uint8_t)integerPart_H;
                FlashBuffer[5] = (uint8_t)integerPart_L;
                FlashBuffer[6] = (uint8_t)decimalPart;

                WriteFlash(FlashBuffer, (uint8_t *)ADDR_FLASH_PAGE_256, 256);
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

            if (GUI_Now == PARAM_CONFIG_GUI)
                break;

            memset(&FlashBuffer[7], 0, 256 - 7);

            for (int n = 0; n < UART_TO_LRM_RECEIVE_LENGTH; n++)
            {
                sprintf((DN_Data_Buf + (n * 2)), "%02X", UART_TO_LRM_RECEIVE_BUFFER[n]);
            }

            strcpy(&FlashBuffer[7], DN_Data_Buf);
            WriteFlash(FlashBuffer, (uint8_t *)ADDR_FLASH_PAGE_256, 256);
            
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
        LCD_ShowString(5 + (5 * 8), 5 + (3 * 16), (uint8_t *)UP_DataBuf_str, BLACK);
        LCD_ShowString(5 + (0 * 8), 5 + (4 * 16), "[STATUS]:", BLUE);
        LCD_ShowString(5 + (9 * 8), 5 + (4 * 16), (uint8_t *)UP_Data_Status, RED);
        LCD_ShowString(5 + (0 * 8), 5 + (6 * 16), "[DN]:", BLUE);
        LCD_ShowString(5 + (5 * 8), 5 + (6 * 16), (uint8_t *)DN_Data_Buf, BLACK);
        LCD_Fill(0, 212, 240, 320, BLACK);
        LCD_ShowString(12, 212 + 45, "Press to Enter Param Config", WHITE);
        break;
    case PARAM_CONFIG_GUI:
        switch (Config_Touch_Num)
        {
        case 0:
            Config_select_type = 0;
            break;
        case 1:
            Config_select_type = 1;
            break;
        case 2:
            Config_select_type = 2;
            break;
        case 3:
            Fn_Config_set(3);
            break;
        case 5:
            Fn_Config_set(5);
            break;
        case 6:
            Fn_Config_set(6);
            break;
        case 8:
            Fn_Config_set(8);
            break;
        default:
            break;
        }
        Config_Touch_Num = 9; // 重置触摸标志位

        if (SensorType_temp == LUX_SENSOR)
            sprintf(SensorType_str, "Lux");
        else if (SensorType_temp == PRESSURE_SNESOR)
            sprintf(SensorType_str, "Pressure");
        else if (SensorType_temp == TEMPER_SENSOR)
            sprintf(SensorType_str, "Temper");
        else if (SensorType_temp == HUMIDI_SENSOR)
            sprintf(SensorType_str, "Humidi");

        LCD_Clear(WHITE);
        LCD_ShowString(5 + (8 * 8), 5 + (0 * 16), "[Project Mode]", BLUE);
        LCD_DrawRectangle(1, 5 + 16, 240 - 1, 5 + 16 + 99, BLUE);
        LCD_DrawRectangle(1, 5 + 16 + 99, 240 - 1, 5 + 16 + 99 * 2, BLUE);
        LCD_DrawRectangle(1, 5 + 16 + 99 * 2, 240 - 1, 5 + 16 + 99 * 3, BLUE);
        LCD_DrawLine(80, 5 + 16, 80, 5 + 16 + 99 * 3, BLUE);
        LCD_DrawLine(160, 5 + 16, 160, 5 + 16 + 99 * 3, BLUE);

        LCD_ShowString(1 + 8, 5 + 16 + 20, "Sample", BLUE);
        LCD_ShowString(1 + 8, 5 + 16 + 36, "Interval", BLUE);
        LCD_ShowNum(1 + 24, 5 + 16 + 52, Tim3_Sensors_Delay_Secend_temp, 2, BLACK);
        LCD_ShowString(1 + 8 + 8, 5 + 16 + 68, "seconds", BLACK);

        LCD_ShowString(1 + 8 + 80 - 2, 5 + 16 + 20, "Number of", BLUE);
        LCD_ShowString(1 + 8 + 80, 5 + 16 + 36, "Sampling", BLUE);
        LCD_ShowNum(1 + 24 + 80, 5 + 16 + 52, SenSors_Data_Buf_Num_temp, 2, BLACK);
        LCD_ShowString(1 + 8 + 80 + 8, 5 + 16 + 68, "times", BLACK);

        LCD_ShowString(1 + 8 + 160, 5 + 16 + 20, "Sensor", BLUE);
        LCD_ShowString(1 + 8 + 160, 5 + 16 + 36, "Type", BLUE);
        LCD_ShowNum(1 + 24 + 160, 5 + 16 + 52, SensorType_temp, 2, BLACK);
        LCD_ShowString(1 + 8 + 160 + 4, 5 + 16 + 68, SensorType_str, BLACK);

        LCD_ShowString(1 + 32, 5 + 16 + 36 + 99, "+1", BLUE);
        LCD_ShowString(1 + 32, 5 + 16 + 36 + 199, "-1", BLUE);

        LCD_ShowString(1 + 16 + 80, 5 + 120, "Upload", BLUE);
        LCD_ShowString(1 + 24 + 80, 5 + 16 + 120, "once", BLUE);
        LCD_ShowString(1 + 20 + 80, 5 + 16 + 136, "every", BLUE);
        LCD_ShowNum(1 + 24 + 80, 5 + 16 + 152, Tim3_Sensors_Delay_Secend_temp * SenSors_Data_Buf_Num_temp, 3, RED);
        LCD_ShowString(1 + 8 + 80 + 8, 5 + 16 + 168, "seconds", BLACK);

        LCD_ShowString(1 + 24 + 160, 5 + 32 + 120, "Save", BLUE);
        LCD_ShowString(1 + 20 + 160, 5 + 16 + 236, "Reset", BLUE);
        LCD_ShowString(1 + 16 + 80, 5 + 16 + 236, "Return", RED);

        switch (Config_select_type)
        {
        case 0:
            Draw_Circle(40, 5 + 16 + 10, 5, RED);
            break;
        case 1:
            Draw_Circle(40 + 80, 5 + 16 + 10, 5, RED);
            break;
        case 2:
            Draw_Circle(40 + 160, 5 + 16 + 10, 5, RED);
            break;
        default:
            break;
        }

        break;
    default:
        break;
    }
}

void Fn_MainfirstIn(int a)
{
    MainGui_first_In = 1;
}
void Fn_ConfigfirstIn(int a)
{
    ConfigGui_first_In = 1;
}

void Fn_Config_select(uint8_t SelectNum)
{
    Config_Touch_Num = SelectNum;
}

void Fn_Config_set(uint8_t config_num)
{
    switch (config_num)
    {
    case 3:
        Config_select_type == 0 && Tim3_Sensors_Delay_Secend_temp < 10 ? Tim3_Sensors_Delay_Secend_temp++ : NULL;
        Config_select_type == 1 && SenSors_Data_Buf_Num_temp < 20 ? SenSors_Data_Buf_Num_temp++ : NULL;
        Config_select_type == 2 && SensorType_temp < 4 ? SensorType_temp++ : NULL;
        break;

    case 5:
        Tim3_Sensors_Delay_Secend = Tim3_Sensors_Delay_Secend_temp;
        SenSors_Data_Buf_Num = SenSors_Data_Buf_Num_temp;
        SensorType = SensorType_temp;
        break;
    case 6:
        Config_select_type == 0 && Tim3_Sensors_Delay_Secend_temp > 1 ? Tim3_Sensors_Delay_Secend_temp-- : NULL;
        Config_select_type == 1 && SenSors_Data_Buf_Num_temp > 3 ? SenSors_Data_Buf_Num_temp-- : NULL;
        Config_select_type == 2 && SensorType_temp > 1 ? SensorType_temp-- : NULL;
        break;
    case 8:
        Tim3_Sensors_Delay_Secend_temp = 1;
        SenSors_Data_Buf_Num_temp = 5;
        SensorType_temp = LUX_SENSOR;
        break;

    default:
        break;
    }
}

// 解析Flash数据
void DecodeFlashData(void)
{
    float data = 0;

    memset(FlashBuffer, 0, 256);
    if (ReadFlash((uint8_t *)ADDR_FLASH_PAGE_256, FlashBuffer, 256) == HAL_OK)
    {
        if (FlashBuffer[2] == LUX_SENSOR)
            sprintf(SensorType_str, "Lux");
        else if (FlashBuffer[2] == PRESSURE_SNESOR)
            sprintf(SensorType_str, "Pressure");
        else if (FlashBuffer[2] == TEMPER_SENSOR)
            sprintf(SensorType_str, "Temper");
        else if (FlashBuffer[2] == HUMIDI_SENSOR)
            sprintf(SensorType_str, "Humidi");

        LCD_Clear(WHITE);
        LCD_ShowString(76, 5 + (0 * 16), "[Last Data]", BLUE);
        LCD_ShowString(5 + (0 * 8), 5 + (1 * 16), "[DevEui]:", BLUE);
        sprintf((char *)DecodeBuffer, "009569000000%02X%02X", FlashBuffer[0], FlashBuffer[1]);
        LCD_ShowString(5 + (9 * 8), 5 + (1 * 16), DecodeBuffer, BLACK);
        memset(DecodeBuffer, 0, 20);

        LCD_ShowString(5 + (0 * 8), 5 + (3 * 16), "[Upload Counter]:", BLUE);
        sprintf((char *)DecodeBuffer, "%d", FlashBuffer[3]);
        LCD_ShowString(5 + (17 * 8), 5 + (3 * 16), DecodeBuffer, BLACK);
        memset(DecodeBuffer, 0, 20);

        LCD_ShowString(5 + (0 * 8), 5 + (4 * 16), "[Sensors Type]:", BLUE);
        sprintf((char *)DecodeBuffer, "0X%02X %s", FlashBuffer[2], SensorType_str);
        LCD_ShowString(5 + (15 * 8), 5 + (4 * 16), DecodeBuffer, BLACK);
        memset(DecodeBuffer, 0, 20);

        LCD_ShowString(5 + (0 * 8), 5 + (5 * 16), "[Sensors Data]:", BLUE);
        data = (float)FlashBuffer[4] * 256 + (float)FlashBuffer[5] + (float)FlashBuffer[6] / 100;
        switch (FlashBuffer[2])
        {
        case 1:
            sprintf((char *)DecodeBuffer, "%.2f lx", data);
            break; // 光照
        case 2:
            sprintf((char *)DecodeBuffer, "%.2f kPa", data);
            break; // 气压
        case 3:
            sprintf((char *)DecodeBuffer, "%.2f C", data);
            break; // 温度
        case 4:
            sprintf((char *)DecodeBuffer, "%.2f %%", data);
            break; // 湿度
        }
        LCD_ShowString(5 + (15 * 8), 5 + (5 * 16), DecodeBuffer, BLACK);
        memset(DecodeBuffer, 0, 20);
        LCD_ShowString(5 + (0 * 8), 5 + (6 * 16), "[Downloaw Data]:", BLUE);
        LCD_ShowString(5 + (16 * 8), 5 + (6 * 16), &FlashBuffer[7], BLACK);
    }
    memset(FlashBuffer, 0, 256);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM3)
    {
        // 用户代码
        Tim3_Counter == (Tim3_Sensors_Delay_Secend * 100) ? Tim3_Counter = 0 : Tim3_Counter++;
    }
}
