#include "stm32f4xx_hal.h"
#include "arm_math.h"

extern float32_t ans[8];

float32_t iir1in[4][1024];
float32_t iir1buf[4][1024];
float32_t iir2in[4][32];
float32_t iir2buf[4][32];

float32_t iir1Av = 0.010199968335142896 * 0.0073290089176873902 * 0.0032983271056692275 * 0.00044678855322144309;
float32_t iir2Av = 0.010199968335142896 * 0.0073290089176873902 * 0.0032983271056692275 * 0.00044678855322144309;

float32_t iir1[20] = {
	1.0f, -1.9822157452246196f, 1.0f, 1.9945972464232222, -0.99477864525879522,
	1.0f, -1.9752977979523887f, 1.0f, 1.98497899263019, -0.98516003528928342,
	1.0f, -1.9450917084336921f, 1.0f, 1.9776016079380723, -0.97778271344447154,
	1.0f, -1.5943028297031059f, 1.0f, 1.9735891833580808, -0.97377044420984371
};

float32_t iir2[20] = {
	1.0f, -1.9822157452246196f, 1.0f, 1.9945972464232222, -0.99477864525879522,
	1.0f, -1.9752977979523887f, 1.0f, 1.98497899263019, -0.98516003528928342,
	1.0f, -1.9450917084336921f, 1.0f, 1.9776016079380723, -0.97778271344447154,
	1.0f, -1.5943028297031059f, 1.0f, 1.9735891833580808, -0.97377044420984371
};

arm_biquad_casd_df1_inst_f32 S3[4];
arm_biquad_casd_df1_inst_f32 S4[4];
float32_t S3buf[4][4*8];
float32_t S4buf[4][4*8];

void DSP_IIR_Init(void)
{
	arm_biquad_cascade_df1_init_f32(&S3[0], 4, (float32_t *)&iir1, (float32_t *)&S3buf[0]);
	arm_biquad_cascade_df1_init_f32(&S3[1], 4, (float32_t *)&iir1, (float32_t *)&S3buf[1]);
	arm_biquad_cascade_df1_init_f32(&S3[2], 4, (float32_t *)&iir1, (float32_t *)&S3buf[2]);
	arm_biquad_cascade_df1_init_f32(&S3[3], 4, (float32_t *)&iir1, (float32_t *)&S3buf[3]);
	
	arm_biquad_cascade_df1_init_f32(&S4[0], 4, (float32_t *)&iir2, (float32_t *)&S4buf[0]);
	arm_biquad_cascade_df1_init_f32(&S4[1], 4, (float32_t *)&iir2, (float32_t *)&S4buf[1]);
	arm_biquad_cascade_df1_init_f32(&S4[2], 4, (float32_t *)&iir2, (float32_t *)&S4buf[2]);
	arm_biquad_cascade_df1_init_f32(&S4[3], 4, (float32_t *)&iir2, (float32_t *)&S4buf[3]);
}

void DSP_I2S_Callback(float* pt1, float* pt2, float* pt3, float* pt4, uint16_t offset)
{
	uint16_t i;
	
	pt1 += offset;
	pt2 += offset;
	pt3 += offset;
	pt4 += offset;
	
	arm_mult_f32(pt1, pt3, iir1in[0]+offset, 512);
	arm_mult_f32(pt1, pt4, iir1in[1]+offset, 512);
	arm_mult_f32(pt2, pt3, iir1in[2]+offset, 512);
	arm_mult_f32(pt2, pt4, iir1in[3]+offset, 512);
	
	for(i=0; i<4; i++)
		arm_biquad_cascade_df1_f32(&S3[i], iir1in[i]+offset, iir1buf[i]+offset, 512);
	
	pt1 = iir1buf[0] + offset;	pt2 = iir1buf[1] + offset;
	pt3 = iir1buf[2] + offset;	pt4 = iir1buf[3] + offset;
	
	for(i=0;i<16;i++)
	{		
		iir2in[0][i+(offset>>5)] = -iir2Av * (*pt1 * *pt3 + *pt2 * *pt4) / (*pt3 * *pt3 + *pt4 * *pt4);
		iir2in[1][i+(offset>>5)] =  iir2Av * (*pt1 * *pt4 - *pt2 * *pt3) / (*pt3 * *pt3 + *pt4 * *pt4);
		
		pt1 += 32;	pt2 += 32;
		pt3 += 32;	pt4 += 32;
	}
	if(!isnan(iir2in[0][0]))
	{
		arm_biquad_cascade_df1_f32(&S4[0], &iir2in[0][offset>>5], &iir2buf[0][offset>>5], 16);
		arm_biquad_cascade_df1_f32(&S4[1], &iir2in[1][offset>>5], &iir2buf[1][offset>>5], 16);
		
		ans[0] =  iir2buf[0][0];
		ans[1] = -iir2buf[1][0];
		ans[2] = sqrtf(ans[0]*ans[0] + ans[1]*ans[1]);
		ans[3] = atanf(ans[1] / ans[0]);
		
		ans[4] =  iir1buf[0][0];
		ans[5] =  iir1buf[1][0];
		ans[6] = -iir1buf[2][0];
		ans[7] = -iir1buf[3][0];
	}
}
