/**
 * @file    ZLCR_beta_bsp.c
 * @author  TIANLAN <yitiandelan@outlook.com>
 * @date    2018-JUN-3
 * @brief   
 *
 * Copyright (c) 2016-2018, TIANLAN.tech
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "stm32f4xx_hal.h"
#include "cmsis_os.h"
#include "ZLCR_beta_bsp.h"

extern I2C_HandleTypeDef hi2c1;
extern I2S_HandleTypeDef hi2s3;
extern UART_HandleTypeDef huart1;

unsigned short I2S_ADCBuf[2048];
unsigned short I2S_DACBuf[2048];
char uart1_rxbuf[64];

const char ZLCR_BSP_CODEC_REG[][2] = {
    /* DAC Step */
    {0x00, 0x00}, //Select Page 0
    {0x0b, 0x81}, //NDAC = 1,Power up
    {0x0c, 0x82}, //MDAC = 2,Power up
    {0x0d, 0x00}, //DOSR = 128
    {0x0e, 0x80}, //DOSR LSB
    {0x3c, 0x01}, //PRB_P1

    /* ADC Step */
    {0x12, 0x81}, //NADC = 1,Power up
    {0x13, 0x82}, //MADC = 2,Power up
    {0x14, 0x80}, //AOSR = 128
    {0x3d, 0x01}, //PRB_R1

    /* Audio Interface */
    {0x1b, 0x00}, //16bit
    {0x55, 0x00}, //ADC Phase Adjust Register

    /* Power Step */
    {0x00, 0x01}, //Select Page 1
    {0x01, 0x08}, //Disabled weak connection of AVDD with DVDD
    {0x02, 0x01}, //Analog Block Power up,AVDD LDO Power up
    //{0x0a,0x3b},  //Input Vcom = 0.9v,Output Vcom = 1.65v
    {0x0a, 0x03},

    {0x03, 0x00}, //DAC PTM mode to PTM_P3/4
    {0x04, 0x00},
    {0x3d, 0x00}, //ADC PTM mode to PTM_R4

    //  {0x7b,0x01},  //REF settime to 40ms
    //  {0x14,0x25},  //HP settime  to

    /* Input Step */
    {0x34, 0x10}, //Route IN2L to LEFT_P with 10k
    {0x36, 0x10}, //Route IN2R to LEFT_M with 10k
    {0x37, 0x40}, //Route IN1R to RIGHT_P with 10k
    {0x39, 0x10}, //Route IN1L to RIGHT_M with 10k

    {0x3b, 0x00}, //Left  MicPGA not mute,gain to 0dB
    {0x3c, 0x00}, //Right MicPGA not mute,gain to 0dB

    /* Output Step */
    {0x0c, 0x08}, //Route Left  DAC to HPL
    {0x0d, 0x08}, //Route Right DAC to HPR
    {0x0e, 0x08}, //Route Left  DAC to LOL
    {0x0f, 0x08}, //Route Right DAC to LOR

    {0x10, 0x00}, //HPL gain  to 0dB
    {0x11, 0x00}, //HPR gain  to 0dB
    {0x12, 0x08}, //LOL gain  to 0dB
    {0x13, 0x08}, //LOR gain  to 0dB

    {0x16, 0x75}, //IN1L to HPL, MUTE
    {0x17, 0x75}, //IN1R to HPL, MUTE

    {0x09, 0x3c}, //LOL,LOR,HPL,HPR,Power up

    /* Initial ok */
    {0x00, 0x00}, //Select Page 0
    {0x3f, 0xd6}, //L&R DAC Power up
    {0x40, 0x00}, //L&R DAC not mute
    {0x51, 0xc0}, //L&R ADC Power up
    {0x52, 0x00}, //L&R ADC not mute, ADC Fine Gain Adjust
    {0x53, 0x00}, //Left  ADC Digital gain
    {0x54, 0x00}, //Right ADC Digital gain
};

void HAL_I2S_TxHalfCpltCallback(I2S_HandleTypeDef *hi2s)
{
}

void HAL_I2S_TxCpltCallback(I2S_HandleTypeDef *hi2s)
{
}

void ZLCR_BSP_Init(void)
{
    unsigned short size = sizeof(ZLCR_BSP_CODEC_REG) / sizeof(ZLCR_BSP_CODEC_REG[0][0]) / 2;
    unsigned short i;

    /* TLV320AIC3204 reset */
    ZLCR_BSP_I2C_Write(0x00, 0x00);
    ZLCR_BSP_I2C_Write(0x01, 0x01);

    ZLCR_BSP_Delay(5);

    /* TLV320AIC3204 init */
    for (i = 0; i < size; i++)
        ZLCR_BSP_I2C_Write(ZLCR_BSP_CODEC_REG[i][0], ZLCR_BSP_CODEC_REG[i][1]);

    /* TLV320AIC3204 start */
    HAL_I2SEx_TransmitReceive_DMA(&hi2s3, (unsigned short *)&I2S_DACBuf, (unsigned short *)&I2S_ADCBuf, 2048);

    /* uart start */
    HAL_UART_Receive_DMA(&huart1, (unsigned char *)uart1_rxbuf, 64);
    HAL_UART_Transmit_DMA(&huart1, (unsigned char *)uart1_rxbuf, 0);
}

void ZLCR_BSP_UART_PutString(const char *pcString, unsigned short usStringLength)
{
    HAL_DMA_Abort(huart1.hdmatx);
    huart1.gState = HAL_UART_STATE_READY;
    HAL_UART_Transmit_DMA(&huart1, (unsigned char *)pcString, usStringLength);
    for (; huart1.hdmatx->Instance->NDTR; ZLCR_BSP_Delay(1));
}

void ZLCR_BSP_UART_PutChar(signed char cOutChar)
{
    huart1.Instance->DR = cOutChar;
}

unsigned char ZLCR_BSP_UART_GetChar(signed char *pcRxedChar)
{
    static unsigned int uart1cnt = 64;

    if (uart1cnt != huart1.hdmarx->Instance->NDTR)
    {
        *pcRxedChar = *((char *)(huart1.hdmarx->Instance->M0AR + 64 - uart1cnt));
        uart1cnt = (uart1cnt > 1) ? uart1cnt - 1 : 64;
        return 1;
    }
    
    return 0;
}

void ZLCR_BSP_I2C_Write(char REG_Address, char REG_data)
{
    char txData[2] = {REG_Address, REG_data};
    while (HAL_I2C_Master_Transmit(&hi2c1, I2C_ADDRESS, (unsigned char *)&txData, 2, 100) != HAL_OK);
}
