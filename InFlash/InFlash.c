#include "InFlash.h"
#include <stdio.h>


uint8_t EraseFlash(uint32_t page, uint32_t end_Add)
{
    uint32_t UserStartSector;
    uint32_t SectorError;
    FLASH_EraseInitTypeDef pEraseInit;
 
    // 解锁内部FLASH
    HAL_FLASH_Unlock();
 
    // 获取起始地址扇区号
    pEraseInit.TypeErase = FLASH_TYPEERASE_PAGES;
		pEraseInit.Page = page;
		pEraseInit.Banks = FLASH_BANK_2;
		pEraseInit.NbPages = 1;
 
    if (HAL_FLASHEx_Erase(&pEraseInit, &SectorError) != HAL_OK)
        return (HAL_ERROR);

    return (HAL_OK);
}

uint8_t WriteFlash(uint8_t *src, uint8_t *dest, uint32_t Len)
{
		uint32_t i = 0;

		if (EraseFlash((uint32_t)dest, (uint32_t)(dest + Len)) == HAL_OK)
		{
			for(i = 0; i < Len; i+=8)
			{
					// 电源电压在2.7-3.6V范围时支持FLASH写入操作
					if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, (uint32_t)(dest+i), *(uint64_t*)(src+i)) == HAL_OK)
					{
							// 检查写入是否有效
							if(*(uint32_t *)(src + i) != *(uint32_t*)(dest+i))
							{
									// 校验失败，返回错误代码2
									return 2;
							}
					}
					else
					{
							// 写入失败，返回错误代码1
							return HAL_ERROR;
					}
			}
			return HAL_OK;
		}
		else
			return HAL_ERROR;
}

uint8_t ReadFlash (uint8_t *src, uint8_t *dest, uint32_t Len)
{
    uint32_t i = 0;
 
    for(i = 0; i < Len; i++)
        dest[i] = *(__IO uint8_t*)(src+i);
    return HAL_OK;
}
