/**
 * @file    zlcr_beta_bsp.h
 * @author  TIANLAN <yitiandelan@outlook.com>
 * @date    2020-07-29
 * @brief   
 *
 * Copyright (c) 2016-2020, TIANLAN.tech
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
#ifndef _ZLCR_BETA_BSP_H
#define _ZLCR_BETA_BSP_H

#define I2C_ADDRESS (0x18 << 1)
#define ZLCR_Beta_BSP_Delay(t) osDelay(t)

void ZLCR_Beta_BSP_Init(void);
void ZLCR_Beta_BSP_REPL_PutString(const char *pcString, unsigned short usStringLength);
void ZLCR_Beta_BSP_REPL_PutChar(signed char cOutChar);
unsigned int ZLCR_Beta_BSP_REPL_GetChar(signed char *pcRxedChar);
void ZLCR_Beta_BSP_I2C_Write(char REG_Address, char REG_data);

#endif
