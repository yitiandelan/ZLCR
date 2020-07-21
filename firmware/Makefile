STM_COMMON = lib

# Sources
SRCS =  main.c
SRCS += freertos.c
SRCS += stm32f4xx_hal_msp.c
SRCS += stm32f4xx_it.c
SRCS += system_stm32f4xx.c
SRCS += FreeRTOS_CLI.c
SRCS += zlcr_beta_bsp.c
SRCS += zlcr_beta_core.c

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
SRCS += arm_copy_f32.c

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
SRCS += ./rtos/gcc/startup_stm32f411xe.s

# Project name
PROJ_NAME = zlcr_beta
OUTPATH = ./build

CC = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy
SIZE = arm-none-eabi-size

CFLAGS  = -g -O3 -Wall
CFLAGS += -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16
CFLAGS += -u_printf_float -u_scanf_float
CFLAGS += -D USE_HAL_DRIVER -D STM32F411xE -D ARM_MATH_CM4 -D __FPU_PRESENT=1U
CFLAGS += -T ./rtos/gcc/STM32F411CEUx_FLASH.ld

LIBS = -lm -lc -lnosys

vpath %.c  ./core
vpath %.c  ./target
vpath %.c  ./rtos/src
vpath %.c  $(STM_COMMON)/stm32f4/Src
vpath %.c  $(STM_COMMON)/cmsis/CMSIS/DSP_Lib/Source/FilteringFunctions
vpath %.c  $(STM_COMMON)/cmsis/CMSIS/DSP_Lib/Source/BasicMathFunctions
vpath %.c  $(STM_COMMON)/cmsis/CMSIS/DSP_Lib/Source/SupportFunctions
vpath %.c  $(STM_COMMON)/freertos/FreeRTOS/Source
vpath %.c  $(STM_COMMON)/freertos/FreeRTOS/Source/portable/MemMang
vpath %.c  $(STM_COMMON)/freertos/FreeRTOS/Source/portable/GCC/ARM_CM4F
vpath %.c  $(STM_COMMON)/freertos/FreeRTOS-Plus/Source/FreeRTOS-Plus-CLI

# Includes
CFLAGS += -I ./core
CFLAGS += -I ./target
CFLAGS += -I ./rtos/inc
CFLAGS += -I $(STM_COMMON)/stm32f4/Inc
CFLAGS += -I $(STM_COMMON)/cmsis/CMSIS/Include
CFLAGS += -I $(STM_COMMON)/freertos/FreeRTOS/Source/include
CFLAGS += -I $(STM_COMMON)/freertos/FreeRTOS/Source/portable/GCC/ARM_CM4F
CFLAGS += -I $(STM_COMMON)/freertos/FreeRTOS-Plus/Source/FreeRTOS-Plus-CLI

OBJS = $(SRCS:.c=.o)

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
