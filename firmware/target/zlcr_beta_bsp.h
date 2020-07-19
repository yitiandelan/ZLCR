/**
 * @file    BSP.h
 * @author  yitiandelan
 * @date    2017-02-25
 * @brief   
 *
 * Copyright (c) 2016-2017, yitiandlan, All Rights Reserved
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

#define I2C_ADDRESS  (0x18<<1)

extern int16_t I2S_ADCBuf[];
extern int16_t I2S_DACBuf[];
extern float freq;
extern float ADCGAIN[];

void BSP_CODEC_Init(void);
void BSP_CODEC_Start(void);
void BSP_Setfreq(float f);
void BSP_SetGAIN(float GAIN1, float GAIN2);
void Single_WriteI2C(uint8_t REG_Address,uint8_t REG_data);
uint8_t Single_ReadI2C(uint8_t REG_Address);

