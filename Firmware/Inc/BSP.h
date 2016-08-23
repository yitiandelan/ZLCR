#include "stm32f4xx_hal.h"

#define I2C_ADDRESS		(0x18<<1)

extern int16_t I2S_ADCBuf[];
extern int16_t I2S_DACBuf[];

extern const int16_t SinWave[];

void BSP_CODEC_Init(void);
void BSP_CODEC_Start(void);
void BSP_Setfreq(float f);

void Single_WriteI2C(uint8_t REG_Address,uint8_t REG_data);
uint8_t Single_ReadI2C(uint8_t REG_Address);

