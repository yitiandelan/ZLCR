#include "stm32f4xx_hal.h"
#include "arm_math.h"

int16_t output[4][1024];
float32_t fir1buf[4][512];
float32_t fir2buf[4][128];
float32_t iir1buf[4][128];

float32_t fir1[16] = {
0.0004494543537, 0.003041580785,  0.01127499714,  0.02948645875,  0.05986463279,
0.09895656258,   0.1367274523,    0.1601988673,   0.1601988673,   0.1367274523,
0.09895656258,   0.05986463279,   0.02948645875,  0.01127499714,  0.003041580785,
0.0004494543537
};

float32_t fir2[36] = {
  8.807687118e-05,0.0002881827822, 0.000719233416,  0.00151371886, 0.002835062565,
   0.004860690329,  0.00776010938,  0.01166700851,  0.01664892584,  0.02267931961,
    0.02961743623,  0.03720088676,  0.04505435377,   0.0527154617,  0.05967601016,
    0.06543384492,  0.06954857707,  0.07169309258,  0.07169309258,  0.06954857707,
    0.06543384492,  0.05967601016,   0.0527154617,  0.04505435377,  0.03720088676,
    0.02961743623,  0.02267931961,  0.01664892584,  0.01166700851,  0.00776010938,
   0.004860690329, 0.002835062565,  0.00151371886, 0.000719233416,0.0002881827822,
  8.807687118e-05
};

float32_t iir1Av = 0.0000010098701256537921 * 0.0000010087732844571611;
float32_t iir1[10] = {
	1.0f, 2.0f, 1.0f, 1.9984582833030753f, -0.99846232278357772f,
	1.0f, 2.0f, 1.0f, 1.9962877157031513f, -0.99629175079628907f
};

arm_fir_instance_f32 S1[4];
arm_fir_instance_f32 S2[4];
arm_biquad_casd_df1_inst_f32 S3[4];

float32_t S1buf[4][16+2-1];
float32_t S2buf[4][36+4-1];
float32_t S3buf[4][4*4];


void DSP_FIR_Init(void)
{
	arm_fir_init_f32(&S1[0], 16, (float32_t *)&fir1[0], (float32_t *)&S1buf[0], 8);
	arm_fir_init_f32(&S1[1], 16, (float32_t *)&fir1[0], (float32_t *)&S1buf[1], 8);
	arm_fir_init_f32(&S1[2], 16, (float32_t *)&fir1[0], (float32_t *)&S1buf[2], 8);
	arm_fir_init_f32(&S1[3], 16, (float32_t *)&fir1[0], (float32_t *)&S1buf[3], 8);
	
	arm_fir_init_f32(&S2[0], 36, (float32_t *)&fir2[0], (float32_t *)&S2buf[0], 8);
	arm_fir_init_f32(&S2[1], 36, (float32_t *)&fir2[0], (float32_t *)&S2buf[1], 8);
	arm_fir_init_f32(&S2[2], 36, (float32_t *)&fir2[0], (float32_t *)&S2buf[2], 8);
	arm_fir_init_f32(&S2[3], 36, (float32_t *)&fir2[0], (float32_t *)&S2buf[3], 8);
}

void DSP_IIR_Init(void)
{
	arm_biquad_cascade_df1_init_f32(&S3[0], 2, (float32_t *)&iir1, (float32_t *)&S3buf[0]);
	arm_biquad_cascade_df1_init_f32(&S3[1], 2, (float32_t *)&iir1, (float32_t *)&S3buf[1]);
	arm_biquad_cascade_df1_init_f32(&S3[2], 2, (float32_t *)&iir1, (float32_t *)&S3buf[2]);
	arm_biquad_cascade_df1_init_f32(&S3[3], 2, (float32_t *)&iir1, (float32_t *)&S3buf[3]);
}

void fir1_f32(
const arm_fir_instance_f32 * S,
q15_t * pSrc,
float32_t * pDst,
uint32_t blockSize)
{
	uint16_t i,n;
	arm_matrix_instance_f32 Coeffs;
	arm_matrix_instance_f32 State;
	arm_matrix_instance_f32 Dst;
	float32_t * p1;
	float32_t * p2;
	
	Coeffs.numRows = 1;
	Coeffs.numCols = 16;
	Coeffs.pData = S->pCoeffs;
	State.numRows = 16;
	State.numCols = 1;
	State.pData = S->pState;
	Dst.numRows = 1;
	Dst.numCols = 16;
	Dst.pData = pDst;
	
	for(i=0;i<512;i+=2)
	{
		p1 = S->pState;
		p2 = p1+2;
		for(n=0;n<16-2;n+=7)
		{
			*p1++ = *p2++;
			*p1++ = *p2++;
			*p1++ = *p2++;
			*p1++ = *p2++;
			*p1++ = *p2++;
			*p1++ = *p2++;
			*p1++ = *p2++;
		}
		*p1++ = *(pSrc+i);
		*p1++ = *(pSrc+i+1);
//		arm_copy_f32(S->pState+2, S->pState, 16-2);
//		arm_q15_to_float( pSrc+i, S->pState+16-2, 2);
		arm_mat_mult_f32( &Coeffs, &State, &Dst);
		Dst.pData ++;
	}
}

void fir2_f32(
const arm_fir_instance_f32 * S,
float32_t * pSrc,
float32_t * pDst,
uint32_t blockSize)
{
	uint16_t i,n;
	arm_matrix_instance_f32 Coeffs;
	arm_matrix_instance_f32 State;
	arm_matrix_instance_f32 Dst;
	float32_t * p1;
	float32_t * p2;
	
	Coeffs.numRows = 1;
	Coeffs.numCols = 36;
	Coeffs.pData = S->pCoeffs;
	State.numRows = 36;
	State.numCols = 1;
	State.pData = S->pState;
	Dst.numRows = 1;
	Dst.numCols = 36;
	Dst.pData = pDst;
	
	for(i=0;i<256;i+=4)
	{
		p1 = S->pState;
		p2 = p1+4;
		for(n=0;n<36-4;n+=4)
		{
			*p1++ = *p2++;
			*p1++ = *p2++;
			*p1++ = *p2++;
			*p1++ = *p2++;
		}
		*p1++ = *(pSrc+i);
		*p1++ = *(pSrc+i+1);
		*p1++ = *(pSrc+i+2);
		*p1++ = *(pSrc+i+3);
//		arm_copy_f32(S->pState+4, S->pState, 36-4);
//		arm_copy_f32(pSrc+i, S->pState+36-4, 4);
		arm_mat_mult_f32( &Coeffs, &State, &Dst);
		Dst.pData ++;
	}
}

void DSP_I2S_Callback(int16_t* pt1, int16_t* pt2, int16_t* pt3, int16_t* pt4, uint16_t offset)
{
	uint16_t i;
	
	pt1 += offset;
	pt2 += offset;
	pt3 += offset;
	pt4 += offset;
	
	arm_mult_q15(pt1, pt3, output[0]+offset, 512);
	arm_mult_q15(pt1, pt4, output[1]+offset, 512);
	arm_mult_q15(pt2, pt3, output[2]+offset, 512);
	arm_mult_q15(pt2, pt4, output[3]+offset, 512);
	
	for(i=0;i<4;i++)
	{
		fir1_f32(&S1[i], output[i]+offset, fir1buf[i]+(offset>>1), 2);
		fir2_f32(&S2[i], fir1buf[i]+(offset>>1), fir2buf[i]+(offset>>3), 4);
		arm_biquad_cascade_df1_f32(&S3[i], fir2buf[i]+(offset>>3), iir1buf[i]+(offset>>3), 64);
	}
}
