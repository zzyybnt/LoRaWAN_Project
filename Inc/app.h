#ifndef __APP_H
#define __APP_H
#include "stdint.h"

/* 软件版本，升级时修改该信息即可 */
#define CODE_VERSION "V1.0.3"
/* 软件版本，升级时修改该信息即可 */
#define PRINT_CODE_VERSION_INFO(format, ...) debug_printf("******** ---Based on LoRaWAN sensor data transmission experiment " format "_%s %s --- ********\r\n", ##__VA_ARGS__, __DATE__, __TIME__)

/** 串口接收内容长度最大值 512字节 */
#define USART_RECV_LEN_MAX 512

/** 入网超时时间：120s */
#define JOIN_TIME_120_SEC 120

typedef enum {
    MAIN_GUI = 0,
    PARAM_CONFIG_GUI,

} GUI_Switch_t;

typedef enum {
    LUX_SENSOR = 0x01,
    PRESSURE_SNESOR,
    TEMPER_SENSOR,
    HUMIDI_SENSOR,
} SENSOR_Type_t;

typedef struct
{
    struct
    {
        float Lux_OPT3001;
        float Pressure_MPL3115;
        uint16_t Temper_HDC1000;
        uint16_t Humidi_HDC1000;
    } Data;
    struct
    {
        float Lux_OPT3001;
        float Pressure_MPL3115;
        uint16_t Temper_HDC1000;
        uint16_t Humidi_HDC1000;
    } Max;
    struct
    {
        float Lux_OPT3001;
        float Pressure_MPL3115;
        uint16_t Temper_HDC1000;
        uint16_t Humidi_HDC1000;
    } Min;
    struct
    {
        float Lux_OPT3001;
        float Pressure_MPL3115;
        uint32_t Temper_HDC1000;
        uint32_t Humidi_HDC1000;
    } Average;

} SensorsData_t;

typedef struct
{
    char upcnt[10];
    char ackcnt[10];
    char toa[10];
    char nbt[10];

    char ch[10];
    char sf[10];
    char pwr[10];
    char per[10];

    char rssi[10];
    char snr[10];

} DEBUG;

void LoRaWAN_Func_Process(void);
void DecodeFlashData(void);
void LoRaWAN_Borad_Info_Print(void);
void GUI_Show(GUI_Switch_t GUI);
void Fn_MainfirstIn(int a);
void Fn_ConfigfirstIn(int a);
void Fn_Config_select(uint8_t SelectNum);
void Fn_Config_set(uint8_t config_num);

#endif
