/**
 * @file    DSP.c
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
#include "arm_math.h"
//#include "math.h"

extern float32_t ans[];
extern float32_t freq;
extern float32_t ADCGAIN[];

arm_biquad_casd_df1_inst_f32 S3[4];
arm_biquad_casd_df1_inst_f32 S4[4];
arm_biquad_casd_df1_inst_f32 S5[4];

float32_t S3buf[4][4*8];
float32_t S4buf[4][4*8];
float32_t S5buf[4][4*8];

float32_t iir1in [4][1024];
float32_t iir2in [4][32];
float32_t iir3in [4][32];
float32_t iir1buf[4][1024];
float32_t iir2buf[4][32];
float32_t iir3buf[4][32];

const float32_t iir1Av = 0.010199968335142896 * 0.0073290089176873902 * 0.0032983271056692275 * 0.00044678855322144309;
const float32_t iir2Av = 0.010199968335142896 * 0.0073290089176873902 * 0.0032983271056692275 * 0.00044678855322144309;
const float32_t iir3Av = 0.000047027154480564627 * 0.000046796105596234438 * 0.000046620796472273122 * 0.000046526466536911244;

const float32_t iir1[20] = {
  1.0f, -1.9822157452246196f, 1.0f, 1.9945972464232222, -0.99477864525879522,
  1.0f, -1.9752977979523887f, 1.0f, 1.98497899263019, -0.98516003528928342,
  1.0f, -1.9450917084336921f, 1.0f, 1.9776016079380723, -0.97778271344447154,
  1.0f, -1.5943028297031059f, 1.0f, 1.9735891833580808, -0.97377044420984371
};

const float32_t iir2[20] = {
  1.0f, -1.9822157452246196f, 1.0f, 1.9945972464232222, -0.99477864525879522,
  1.0f, -1.9752977979523887f, 1.0f, 1.98497899263019, -0.98516003528928342,
  1.0f, -1.9450917084336921f, 1.0f, 1.9776016079380723, -0.97778271344447154,
  1.0f, -1.5943028297031059f, 1.0f, 1.9735891833580808, -0.97377044420984371
};

const float32_t iir3[20] = {
  1.0f, 2.0f, 1.0f, 1.9944677391903753, -0.99465584780829763,
  1.0f, 2.0f, 1.0f, 1.9846687294254335, -0.98485591384781845,
  1.0f, 2.0f, 1.0f, 1.9772336975595128, -0.97742018074540182,
  1.0f, 2.0f, 1.0f, 1.9732330724951805, -0.9734191783613283
};

void DSP_Filter_Init(void)
{
  uint16_t i;
  
  for(i=0;i<4;i++)
  {
    arm_biquad_cascade_df1_init_f32(&S3[i], 4, (float32_t *)&iir1, (float32_t *)&S3buf[i]);
    arm_biquad_cascade_df1_init_f32(&S4[i], 4, (float32_t *)&iir2, (float32_t *)&S4buf[i]);
    arm_biquad_cascade_df1_init_f32(&S5[i], 4, (float32_t *)&iir3, (float32_t *)&S5buf[i]);
  }
}

void DSP_I2S_Callback(float* pt1, float* pt2, float* pt3, float* pt4, uint16_t offset)
{
  uint16_t i;
  
  pt1 += offset;  pt2 += offset;
  pt3 += offset;  pt4 += offset;
  
  arm_mult_f32(pt1, pt3, iir1in[0]+offset, 512);
  arm_mult_f32(pt1, pt4, iir1in[1]+offset, 512);
  arm_mult_f32(pt2, pt3, iir1in[2]+offset, 512);
  arm_mult_f32(pt2, pt4, iir1in[3]+offset, 512);
  
  for(i=0; i<4; i++)
    arm_biquad_cascade_df1_f32(&S3[i], iir1in[i]+offset, iir1buf[i]+offset, 512);
  
  pt1 = iir1buf[0]+offset;  pt2 = iir1buf[1]+offset;
  pt3 = iir1buf[2]+offset;  pt4 = iir1buf[3]+offset;
  
  for(i=(offset>>5);i<(offset>>5)+16;i++)
  {    
    iir2in[0][i] =  *pt1;  iir2in[1][i] =  *pt2;
    iir2in[2][i] = -*pt3;  iir2in[3][i] = -*pt4;
    pt1 += 32;  pt2 += 32;  pt3 += 32;  pt4 += 32;
  }
  
  for(i=0;i<4;i++)
    arm_biquad_cascade_df1_f32(&S4[i], &iir2in[i][offset>>5], &iir2buf[i][offset>>5], 16);
}

void DSP_I2S_CpltCallback(void)
{
  static uint16_t n=0;
  uint16_t i;
  
  for(i=0;i<4;i++)
  {
    iir3in[i][n] = iir2Av * iir2buf[i][0];
    if(n==31)
      arm_biquad_cascade_df1_f32(&S5[i], &iir3in[i][0], &iir3buf[i][0], 32);
  }
  ans[4] = ((freq > 48)?(iir2Av * iir2buf[0][0]):(iir3Av * iir3buf[0][n])) / ADCGAIN[2];
  ans[5] = ((freq > 48)?(iir2Av * iir2buf[1][0]):(iir3Av * iir3buf[1][n])) / ADCGAIN[2];
  ans[6] = ((freq > 48)?(iir2Av * iir2buf[2][0]):(iir3Av * iir3buf[2][n])) / ADCGAIN[3];
  ans[7] = ((freq > 48)?(iir2Av * iir2buf[3][0]):(iir3Av * iir3buf[3][n])) / ADCGAIN[3];
  
  ans[0] =  (ans[4]*ans[6] + ans[5]*ans[7]) / (ans[6]*ans[6] + ans[7]*ans[7]);
  ans[1] = -(ans[5]*ans[6] - ans[4]*ans[7]) / (ans[6]*ans[6] + ans[7]*ans[7]);
  if(ans[0]<=0) ans[0] = 1e-7;
  ans[2] = sqrtf(ans[0] * ans[0] + ans[1] * ans[1]);
  ans[3] = atanf(ans[1] / ans[0]);
  
  n = (n+1)&0x001f;
}

