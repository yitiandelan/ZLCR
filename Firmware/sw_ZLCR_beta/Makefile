STM_COMMON = ../sw_Lib

# Sources
SRCS =  main.c
SRCS += ZLCR.c
SRCS += BSP.c
SRCS += DSP.c
SRCS += freertos.c
SRCS += stm32f4xx_hal_msp.c
SRCS += stm32f4xx_it.c
SRCS += system_stm32f4xx.c

# CMSIS OS
SRCS += cmsis_os.c
SRCS += event_groups.c
SRCS += port.c
SRCS += timers.c
SRCS += tasks.c
SRCS += croutine.c
SRCS += heap_4.c
SRCS += list.c
SRCS += queue.c

# DSP Library
SRCS += arm_mult_f32.c
SRCS += arm_biquad_cascade_df1_init_f32.c
SRCS += arm_biquad_cascade_df1_f32.c
SRCS += arm_fir_init_f32.c
SRCS += arm_fir_f32.c
SRCS += arm_mult_f32.c

# Library code
SRCS += stm32f4xx_hal.c
SRCS += stm32f4xx_hal_i2c.c
SRCS += stm32f4xx_hal_i2s_ex.c
SRCS += stm32f4xx_hal_rcc_ex.c
SRCS += stm32f4xx_hal_rcc.c
SRCS += stm32f4xx_hal_uart.c
SRCS += stm32f4xx_hal_spi.c
SRCS += stm32f4xx_hal_tim.c
SRCS += stm32f4xx_hal_flash_ramfunc.c
SRCS += stm32f4xx_hal_dma.c
SRCS += stm32f4xx_hal_cortex.c
SRCS += stm32f4xx_hal_flash.c
SRCS += stm32f4xx_hal_pwr.c
SRCS += stm32f4xx_hal_pwr_ex.c
SRCS += stm32f4xx_hal_gpio.c
SRCS += stm32f4xx_hal_i2c_ex.c
SRCS += stm32f4xx_hal_tim_ex.c
SRCS += stm32f4xx_hal_flash_ex.c
SRCS += stm32f4xx_hal_dma_ex.c
SRCS += stm32f4xx_hal_i2s.c

# add startup file to build
SRCS += ./GCC/startup_stm32f411xe.c

# Project name
PROJ_NAME = 100k_ZLCR_beta
OUTPATH = ./GCC/build

CC = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy
SIZE = arm-none-eabi-size


# Choose debug or release...
# CFLAGS =  -g -O2 -Wall -std=c99 -lm -lnosys -specs=nosys.specs -u _printf_float -u _scanf_float
# CFLAGS =  --specs=nano.specs -u_printf_float -u_scanf_float --specs=nosys.specs
CFLAGS  = -g -O0 -Wall
CFLAGS += -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16
CFLAGS += -specs=nano.specs -u_printf_float -u_scanf_float -specs=nosys.specs
# CFLAGS += -ffunction-sections -fdata-sections
CFLAGS += -DUSE_HAL_DRIVER -DSTM32F411xE -DARM_MATH_CM4 -D__FPU_PRESENT=1U
CFLAGS += -T./GCC/STM32F411CE_flash.lds

LIBS  = -lm -lc -lnosys

vpath %.c  ./Src
vpath %.c  $(STM_COMMON)/CMSIS/Device/ST/STM32F4xx/Source/Templates
vpath %.c  $(STM_COMMON)/STM32F4xx_HAL_Driver/Src
vpath %.c  $(STM_COMMON)/Middlewares/Third_Party/FreeRTOS/Source
vpath %.c  $(STM_COMMON)/Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS
vpath %.c  $(STM_COMMON)/Middlewares/Third_Party/FreeRTOS/Source/portable/MemMang
vpath %.c  $(STM_COMMON)/Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F
vpath %.c  $(STM_COMMON)/CMSIS/DSP_Lib/Source/FilteringFunctions
vpath %.c  $(STM_COMMON)/CMSIS/DSP_Lib/Source/BasicMathFunctions

# Includes
CFLAGS += -I./Inc
CFLAGS += -I$(STM_COMMON)/CMSIS/Include
CFLAGS += -I$(STM_COMMON)/CMSIS/Device/ST/STM32F4xx/Include
CFLAGS += -I$(STM_COMMON)/STM32F4xx_HAL_Driver/Inc
CFLAGS += -I$(STM_COMMON)/STM32F4xx_HAL_Driver/Inc/Legacy
CFLAGS += -I$(STM_COMMON)/Middlewares/Third_Party/FreeRTOS/Source/include
CFLAGS += -I$(STM_COMMON)/Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS
CFLAGS += -I$(STM_COMMON)/Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F

OBJS = $(SRCS:.c=.o)


.PHONY: lib proj dir_tree

all:  dir_tree proj

dir_tree:
	mkdir -p $(OUTPATH)

proj: $(OUTPATH)/$(PROJ_NAME).elf
	$(SIZE) $(OUTPATH)/$(PROJ_NAME).elf

$(OUTPATH)/$(PROJ_NAME).elf: $(SRCS)
	$(CC) $(CFLAGS) $^ -o $@ $(LIBPATHS) $(LIBS)
	$(OBJCOPY) -O ihex $(OUTPATH)/$(PROJ_NAME).elf $(OUTPATH)/$(PROJ_NAME).hex
	$(OBJCOPY) -O binary $(OUTPATH)/$(PROJ_NAME).elf $(OUTPATH)/$(PROJ_NAME).bin

clean:
	find . -name \*.o -type f -delete
	find . -name \*.lst -type f -delete
	rm -f $(OUTPATH)/$(PROJ_NAME).elf
	rm -f $(OUTPATH)/$(PROJ_NAME).hex
	rm -f $(OUTPATH)/$(PROJ_NAME).bin
