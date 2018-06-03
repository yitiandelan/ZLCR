/**
 * @file    ZLCR_beta_core.c
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

#include "ZLCR_beta_core.h"
#include "arm_math.h"

float ZLCR_prv_freq = 1000.0f;  //f = fs / 2^22 * freqREG
unsigned int ZLCR_prv_tick1;    //freqREG
unsigned int ZLCR_prv_sum1;     //phaseREG

void ZLCR_Init(void)
{
    ZLCR_Setfreq(&ZLCR_prv_freq);
}

void ZLCR_DeInit(void);

void ZLCR_IDLE(void)
{

}

void ZLCR_ISR(unsigned short *txbuf, unsigned short *rxbuf, unsigned short offset, unsigned short size)
{

}

void ZLCR_Setfreq(float *freq)
{
    float f;
    f =  (*freq > 90000.0f) ? 90000.0f : (*freq < 0.0f) ? 0.0f : *freq;
    ZLCR_prv_tick1 = 22906.5f * f;
    ZLCR_prv_freq = ZLCR_prv_tick1 / 22906.5f;
}

void ZLCR_Getfreq(float *freq)
{
    *freq = ZLCR_prv_freq;
}
