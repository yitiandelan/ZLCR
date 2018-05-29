/**
 * @file    ZLCR.c
 * @author  yitiandelan
 * @date    2017-02-25
 * @brief   
 *
 * Copyright (c) 2017, yitiandlan, All Rights Reserved
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
#include "BSP.h"
#include "arm_math.h"
#include "stdarg.h"

#define NoKeyPress() ((uart1_laseNDTR == huart1.hdmarx->Instance->NDTR) & (uart6_laseNDTR == huart6.hdmarx->Instance->NDTR))
#define refreshKey()                                \
    uart1_laseNDTR = huart1.hdmarx->Instance->NDTR; \
    uart6_laseNDTR = huart6.hdmarx->Instance->NDTR

extern float32_t freq;
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart6;

float32_t ans[8];
uint8_t uart1_rxbuf[64];
uint8_t uart6_rxbuf[64];
uint8_t uart_fifo[64] = "info\n";
uint16_t uart1_laseNDTR, uart6_laseNDTR;

extern void DSP_Filter_Init(void);

int _write(int file, char *ptr, int len)
{
    //  print("ptr:%x,len:%d",ptr,len);
    HAL_DMA_Abort(huart1.hdmatx);
    huart1.gState = HAL_UART_STATE_READY;
    HAL_UART_Transmit_DMA(&huart1, (uint8_t *)ptr, len);

    // HAL_DMA_Abort(&hdma_usart6_tx);
    // huart6.gState = HAL_UART_STATE_READY;
    // HAL_UART_Transmit_DMA(&huart6, (uint8_t *)ptr, len);
    return len;
}

int fgetc(FILE *_File)
{
    static unsigned int uart1cnt = 64;
    static unsigned int uart6cnt = 64;
    char ch = '\0';
    for (;;)
    {
        if (uart1cnt != huart1.hdmarx->Instance->NDTR)
        {
            ch = *((char *)(huart1.hdmarx->Instance->M0AR + 64 - uart1cnt));
            uart1cnt = uart1cnt - 1;
            if (uart1cnt == 0)
                uart1cnt = 64;
            return ch;
        }
        else if (uart6cnt != huart6.hdmarx->Instance->NDTR)
        {
            ch = *((char *)(huart6.hdmarx->Instance->M0AR + 64 - uart6cnt));
            uart6cnt = uart6cnt - 1;
            if (uart6cnt == 0)
                uart6cnt = 64;
            return ch;
        }
    }
    return ch;
}

int readline(char *ptr, int len)
{
    int n = 0;
    for (; n < len; n++)
    {
        *ptr = (char)fgetc(0);
        //_write(0, ptr, 1);
        if (*(ptr++) == '\n')
            break;
    }
    return n;
}

void ZLCR_Init(void)
{
    DSP_Filter_Init();
    BSP_CODEC_Init();
    BSP_CODEC_Start();

    HAL_UART_Receive_DMA(&huart1, (uint8_t *)uart1_rxbuf, 64);
    HAL_UART_Transmit_DMA(&huart1, (uint8_t *)uart1_rxbuf, 0);
    HAL_UART_Receive_DMA(&huart6, (uint8_t *)uart6_rxbuf, 64);
    HAL_UART_Transmit_DMA(&huart6, (uint8_t *)uart6_rxbuf, 0);
}

void ZLCR_LOOP(void)
{
    refreshKey();

    if (!strncmp((char *)uart_fifo, "reboot\n", 7))
        HAL_NVIC_SystemReset();

    if (!strncmp((char *)uart_fifo, "freq\n", 5))
        printf("%f Hz\n", freq);

    if (!strncmp((char *)uart_fifo, "info\n", 5))
        printf("ZLCR rev.c 2017-02-25 14:52:44 by.yitiandelan\n");

    if (!strncmp((char *)uart_fifo, "zlcr\n", 5))
        for (; NoKeyPress(); printf("{\"FREQ\":%e,\"MAG\":%e,\"PHASE\":%e}\n", freq, ans[2], ans[3]))
            osDelay(10);
    else if (!strncmp((char *)uart_fifo, "zlcr -f ", 8))
        if (sscanf((char *)uart_fifo, "zlcr -f %f", &freq))
            for (BSP_Setfreq(freq); NoKeyPress(); printf("{\"FREQ\":%e,\"MAG\":%e,\"PHASE\":%e}\n", freq, ans[2], ans[3]))
                osDelay(10);

    if (!strncmp((char *)uart_fifo, "zlcr -raw\n", 10))
        for (; NoKeyPress(); printf("{\"FREQ\":%e,\"a\":%e,\"b\":%e,\"c\":%e,\"d\":%e}\n", freq, ans[4], ans[5], ans[6], ans[7]))
            osDelay(10);
    else if (!strncmp((char *)uart_fifo, "zlcr -raw -f ", 13))
        if (sscanf((char *)uart_fifo, "zlcr -raw -f %f", &freq))
            for (BSP_Setfreq(freq); NoKeyPress(); printf("{\"FREQ\":%e,\"a\":%e,\"b\":%e,\"c\":%e,\"d\":%e}\n", freq, ans[4], ans[5], ans[6], ans[7]))
                osDelay(10);

    readline((char *)uart_fifo, 64);
}
