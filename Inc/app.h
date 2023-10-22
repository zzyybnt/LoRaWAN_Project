#ifndef __APP_H
#define __APP_H
#include "stdint.h"

/* �����汾������ʱ�޸ĸ���Ϣ���� */
#define CODE_VERSION "V1.0.3"
/* �����汾������ʱ�޸ĸ���Ϣ���� */
#define PRINT_CODE_VERSION_INFO(format, ...) debug_printf("******** ---Based on LoRaWAN sensor data transmission experiment " format "_%s %s --- ********\r\n", ##__VA_ARGS__, __DATE__, __TIME__)

/** ���ڽ������ݳ������ֵ 512�ֽ� */
#define USART_RECV_LEN_MAX 512

/** ������ʱʱ�䣺120s */
#define JOIN_TIME_120_SEC 120

typedef struct
{
    float Lux_OPT3001;
    float Pressure_MPL3115;
    uint16_t Temper_HDC1000;
    uint16_t Humidi_HDC1000;
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
void LoRaWAN_Borad_Info_Print(void);

#endif