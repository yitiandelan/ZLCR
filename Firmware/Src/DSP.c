#include "stm32f4xx_hal.h"
#include "arm_math.h"

float32_t iir1in[4][1024];
float32_t iir1buf[4][1024];
float32_t iir2in[4][32];
float32_t iir2buf[4][32];

float32_t iir1Av = 0.00000070160903126781204 * 0.00000070118566080715445 * 0.00000070086197210963915 * 0.00000070068691765119882;
float32_t iir2Av = 0.0000024658877448044323 * 0.0000024631000438563838 * 0.0000024609706844175688 * 0.0000024598198180999213;

float32_t iir1[20] = {
	1.0f, 2.0f, 1.0f, 1.9993436535529137, -0.99934645998903859,
	1.0f, 2.0f, 1.0f, 1.9981371938212198, -0.99813999856386271,
	1.0f, 2.0f, 1.0f, 1.9972147927199473, -0.99721759616783578,
	1.0f, 2.0f, 1.0f, 1.9967159479147769, -0.99671875066244753
};

float32_t iir2[20] = {
	1.0f, 2.0f, 1.0f, 1.9987651006367555, -0.99877496418773482,
	1.0f, 2.0f, 1.0f, 1.9965054846515138, -0.99651533705168915,
	1.0f, 2.0f, 1.0f, 1.9947794979994522, -0.99478934188219004,
	1.0f, 2.0f, 1.0f, 1.9938466447355279, -0.99385648401480042
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
	uint16_t i, n;
	
	pt1 += offset;
	pt2 += offset;
	pt3 += offset;
	pt4 += offset;
	
	arm_mult_f32(pt1, pt3, iir1in[0]+offset, 512);
	arm_mult_f32(pt1, pt4, iir1in[1]+offset, 512);
	arm_mult_f32(pt2, pt3, iir1in[2]+offset, 512);
	arm_mult_f32(pt2, pt4, iir1in[3]+offset, 512);
	
	for(i=0; i<4; i++)
	{
		arm_biquad_cascade_df1_f32(&S3[i], iir1in[i]+offset, iir1buf[i]+offset, 512);
		for(n=0; n<16; n++)
			iir2in[i][n + (offset>>5)] = iir1buf[i][(n<<5) + offset] * iir2Av;
		arm_biquad_cascade_df1_f32(&S4[i], iir2in[i]+(offset>>5), iir2buf[i]+(offset>>5), 16);
	}
}
