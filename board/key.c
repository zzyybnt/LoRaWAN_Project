#include "key.h"
#include "gpio.h"
#include "usart.h"
#include "XPT2046.h"
#include "ST7789v.h"
#include "app.h"
// #include "lcd_app.h"

KEY App_Key;
KEY *App_Key_str = &App_Key;
extern DEVICE_MODE_T *Device_Mode_str;
extern uint8_t LCD_EN;
extern Pen_Holder Pen_Point;
GUI_Switch_t GUI_Now;

/**
 * @brief		MCU引脚中断回调函数
 * @details  	该函数包含开发板2个按键以及与lorawan模块相连4个引脚mode\wake\busy\stat的中断处理
 * @param    	GPIO_Pin:stm32直接调用，无需处理
 * @return   	无
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GET_KEY1_LEVEL == GPIO_PIN_RESET && GET_KEY2_LEVEL == GPIO_PIN_RESET)
        return;
    if (GET_KEY1_LEVEL == GPIO_PIN_RESET) // 右边按键K1开始通讯
    {
        App_Key.A_KEY1 = 1;
        HAL_Delay(200);
    }
    if (GET_KEY2_LEVEL == GPIO_PIN_RESET)
    {
        App_Key.A_KEY2 = 1;
        HAL_Delay(200);
    }
    KEY_DO(App_Key_str, Device_Mode_str);

    if (LCD_EN == 1)
    {
        if (HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_0) == 0)
        {
            HAL_NVIC_DisableIRQ(EXTI0_IRQn);

            if (Pen_Point.Key_Sta == 0)
            {
                Read_ADS2(&Pen_Point.X, &Pen_Point.Y);
                Pen_Point.Key_Sta = 1;

                Pen_Point.X0 = Pen_Point.xfac * Pen_Point.X + Pen_Point.xoff;
                Pen_Point.Y0 = Pen_Point.yfac * Pen_Point.Y + Pen_Point.yoff;

                Touch_Key(Pen_Point.X0, Pen_Point.Y0);
            }

            __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_0);
            delay_10ms(15); //???
            HAL_NVIC_EnableIRQ(EXTI0_IRQn);
        }
    }
}

/**
 * @brief		按键处理函数
 * @param    	key_temp：key1 or key2, Device_Sate_temp:设备功能
 * @return   	无
 */
void KEY_DO(KEY *key_temp, DEVICE_MODE_T *device_mode_temp)
{
    if (key_temp->A_KEY1 == 1)
    {
        key_temp->A_KEY1 = 0;
        switch ((uint8_t)*device_mode_temp)
        {
        case NO_MODE:
            *device_mode_temp = CMD_CONFIG_MODE;
            break;

        case CMD_CONFIG_MODE:
            *device_mode_temp = DATA_TRANSPORT_MODE;
            break;

        case DATA_TRANSPORT_MODE:
            *device_mode_temp = CMD_CONFIG_MODE;
            break;

        case PRO_TRAINING_MODE:
            *device_mode_temp = CMD_CONFIG_MODE;
            break;
        }
    }
    else if (key_temp->A_KEY2 == 1)
    {
        key_temp->A_KEY2 = 0;
        switch ((uint8_t)*device_mode_temp)
        {
        case NO_MODE:
            *device_mode_temp = PRO_TRAINING_MODE;
            break;

        case CMD_CONFIG_MODE:
            *device_mode_temp = PRO_TRAINING_MODE;
            break;

        case DATA_TRANSPORT_MODE:
            *device_mode_temp = PRO_TRAINING_MODE;
            break;

        default:
            break;
        }
    }
}

/**
 * @brief		触摸屏幕处理函数
 * @param    	Pen_Point.X0:屏幕上触摸的位置X坐标  Pen_Point.Y0:屏幕上触摸的位置Y坐标
 * @return   	无
 */
void Touch_Key(uint16_t x, uint16_t y)
{
    if (GUI_Now == MAIN_GUI)
    {
        if (y > 212 && y < 320)
        {
            GUI_Now = PARAM_CONFIG_GUI;
            Fn_ConfigfirstIn(0);
        }
    }
    else if (GUI_Now == PARAM_CONFIG_GUI)
    {
        if (y > (5 + 16) && y < (5 + 16 + 99))
        {
            if (x > 0 && x < 80)
                Fn_Config_select(0);
            else if (x > 80 && x < 160)
                Fn_Config_select(1);
            else if (x > 160 && x < 240)
                Fn_Config_select(2);
        }
        else if (y > (5 + 16 + 99) && y < (5 + 16 + 199))
        {
            if (x > 0 && x < 80)
                Fn_Config_select(3);
            else if (x > 80 && x < 160)
                Fn_Config_select(4);
            else if (x > 160 && x < 240)
                Fn_Config_select(5);
        }
        else if (y > (5 + 16 + 199) && y < (5 + 16 + 299))
        {
            if (x > 0 && x < 80)
                Fn_Config_select(6);
            else if (x > 80 && x < 160)
            {
                Pen_Point.Key_Sta = 0;
                GUI_Now = MAIN_GUI;
                Fn_MainfirstIn(0);
                return;
            }
            else if (x > 160 && x < 240)
                Fn_Config_select(8);
        }
        Fn_ConfigfirstIn(0);
    }
    Pen_Point.Key_Sta = 0;
    return;
}