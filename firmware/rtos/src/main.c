/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "zlcr_beta_bsp.h"
#include "zlcr_core.h"
#include "FreeRTOS_CLI.h"
#include "string.h"
#include "arm_math.h"
#include "stdio.h"
#include "py/compile.h"
#include "py/runtime.h"
#include "py/repl.h"
#include "py/gc.h"
#include "py/mperrno.h"
#include "lib/utils/pyexec.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

I2S_HandleTypeDef hi2s3;
DMA_HandleTypeDef hdma_i2s3_ext_rx;
DMA_HandleTypeDef hdma_spi3_tx;

SPI_HandleTypeDef hspi2;

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart6;
DMA_HandleTypeDef hdma_usart1_tx;
DMA_HandleTypeDef hdma_usart1_rx;
DMA_HandleTypeDef hdma_usart6_rx;
DMA_HandleTypeDef hdma_usart6_tx;

osThreadId defaultTaskHandle;
osThreadId myTask02Handle;
/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/
static char *stack_top;
static char heap[2048];

void gc_collect(void)
{
    void *dummy;
    gc_collect_start();
    gc_collect_root(&dummy, ((mp_uint_t)stack_top - (mp_uint_t)&dummy) / sizeof(mp_uint_t));
    gc_collect_end();
    gc_dump_info();
}

mp_lexer_t *mp_lexer_new_from_file(const char *filename)
{
    mp_raise_OSError(MP_ENOENT);
}

mp_import_stat_t mp_import_stat(const char *path)
{
    return MP_IMPORT_STAT_NO_EXIST;
}

mp_obj_t mp_builtin_open(size_t n_args, const mp_obj_t *args, mp_map_t *kwargs)
{
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(mp_builtin_open_obj, 1, mp_builtin_open);

void nlr_jump_fail(void *val)
{
    for (;;)
    {
    }
}

void NORETURN __fatal_error(const char *msg)
{
    for (;;)
    {
    }
}

int mp_hal_stdin_rx_chr(void)
{
    unsigned char c = 0;
    for (; ZLCR_Beta_BSP_REPL_GetChar((signed char *)&c) == 0;)
    {
        osDelay(1);
    }
    return c;
}

void mp_hal_stdout_tx_strn(const char *str, mp_uint_t len)
{
    ZLCR_Beta_BSP_REPL_PutString(str, len);
}

/* Const messages output by the command console. */
static const char *const pcWelcomeMessage = "\r\n";
static const char *const pcEndOfOutputMessage = "\r\n> ";
static const char *const pcNewLine = "\r\n";
static const char *const pcHelpMessage =
    "\r\n\
zlcr: Open-Source-Hardware L-C-R Meter\r\n\
usage: zlcr [option]\r\n\r\n\
option:\r\n\
  -f FREQ       Set test frequency (default: 1000.0)\r\n\
  -o FORMAT     Set output format (default: JSON); FORMAT is JSON:RAW:NULL\r\n\
  -h, --help    Print help message and exit\r\n\
  -v, --version Print version and exit\r\n";
static const char *const pcVersionMessage = "{\"HW\": \"ZLCR BETA REV.C\", \"FW\": \"v0.1.1\", \"API\": 2}\r\n";

static BaseType_t prvZLCRCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
    const char *pcParameter;
    BaseType_t xParameterStringLength;
    static UBaseType_t uxParameterNumber = 0;
    BaseType_t xReturn = pdPASS;
    static size_t temp = 0;
    static char mode = 0;
    float f, ans[4];
    signed char cRxedChar;

    pcWriteBuffer[0] = 0x00;

    if (uxParameterNumber == 0)
    {
        uxParameterNumber++;
    }
    else
    {
        pcParameter = FreeRTOS_CLIGetParameter(
            pcCommandString,        /* The command string itself. */
            uxParameterNumber,      /* Return the next parameter. */
            &xParameterStringLength /* Store the parameter string length. */
        );

        if (pcParameter != NULL)
        {
            if (!strncmp(pcParameter, "-h", 2) || !strncmp(pcParameter, "--help", 6))
            {
                strncpy(pcWriteBuffer, (char *)(pcHelpMessage + temp), xWriteBufferLen - 1);
                temp += (xWriteBufferLen - 1);

                if (temp >= strlen(pcHelpMessage))
                {
                    xReturn = pdFALSE;
                }
            }
            else if (!strncmp(pcParameter, "-v", 2) || !strncmp(pcParameter, "--version", 9))
            {
                strcpy(pcWriteBuffer, pcVersionMessage);
                xReturn = pdFALSE;
            }
            else if (!strncmp(pcParameter, "-f", 2))
            {
                if (sscanf(pcParameter, "-f %f", &f))
                {
                    ZLCR_Core_SetFreq(&f);
                    uxParameterNumber += 2;
                }
                else
                {
                    xReturn = pdFALSE;
                }
            }
            else if (!strncmp(pcParameter, "-o", 2))
            {
                if (!strncmp(pcParameter, "-o JSON", 7))
                {
                    mode = 0;
                }
                else if (!strncmp(pcParameter, "-o RAW", 6))
                {
                    mode = 1;
                }
                else if (!strncmp(pcParameter, "-o NULL", 7))
                {
                    mode = 2;
                }
                uxParameterNumber += 2;
            }
            else
            {
                xReturn = pdFALSE;
            }
        }
        else
        {
            ZLCR_Core_GetFreq(&f);

            if (ZLCR_Core_GetData(&ans[0]))
            {
                osDelay(5);
            }
            else if (mode == 0)
            {
                float t[4];

                t[0] = (ans[0] * ans[2] + ans[1] * ans[3]) / (ans[2] * ans[2] + ans[3] * ans[3]);
                t[1] = (ans[1] * ans[2] - ans[0] * ans[3]) / (ans[2] * ans[2] + ans[3] * ans[3]);

                t[0] = (t[0] <= 0) ? 1e-7 : t[0];

                t[2] = sqrtf(t[0] * t[0] + t[1] * t[1]);
                t[3] = atanf(-1.f * t[1] / t[0]);

                snprintf(pcWriteBuffer, xWriteBufferLen, "{\"FREQ\":%e,\"MAG\":%e,\"PHASE\":%e}\r\n", f, t[2], t[3]);
            }
            else if (mode == 1)
            {
                snprintf(pcWriteBuffer, xWriteBufferLen, "{\"FREQ\":%e,\"a\":%e,\"b\":%e,\"c\":%e,\"d\":%e}\r\n", f, ans[0], ans[1], ans[2], ans[3]);
            }
            else if (mode == 2)
            {
                xReturn = pdFALSE;
            }

            /* press CTRL+C to exit */
            if (ZLCR_Beta_BSP_REPL_GetChar(&cRxedChar))
            {
                if (cRxedChar == 0x03)
                {
                    xReturn = pdFALSE;
                }
            }
        }
    }

    if (xReturn == pdFALSE)
    {
        uxParameterNumber = 0;
        temp = 0;
        mode = 0;
    }

    return xReturn;
}

static BaseType_t prvRebootCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
    HAL_NVIC_SystemReset();
    return pdPASS;
}

static BaseType_t prvPythonCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
    int stack_dummy;
    stack_top = (char *)&stack_dummy;
    gc_init(heap, heap + sizeof(heap));
    mp_init();
    pyexec_friendly_repl();
    mp_deinit();
    *pcWriteBuffer = '\0';
    return pdFALSE;
}

static const CLI_Command_Definition_t xZLCRCmd = {
    "zlcr", /* The command string to type. */
    "zlcr:\r\n Open-Source-Hardware L-C-R Meter\r\n\r\n",
    prvZLCRCommand, /* The function to run. */
    -1              /* No parameters are expected. */
};

static const CLI_Command_Definition_t xRebootCmd = {
    "reboot", /* The command string to type. */
    "reboot:\r\n Reboot the system.\r\n\r\n",
    prvRebootCommand, /* The function to run. */
    -1                /* No parameters are expected. */
};

static const CLI_Command_Definition_t xPythonCmd = {
    "python", /* The command string to type. */
    "python:\r\n Launch MicroPython REPL\r\n\r\n",
    prvPythonCommand, /* The function to run. */
    -1                /* No parameters are expected. */
};
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_I2C1_Init(void);
static void MX_I2S3_Init(void);
static void MX_SPI2_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_USART6_UART_Init(void);
void StartDefaultTask(void const *argument);
void SysTask(void const *argument);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
    /* USER CODE BEGIN 1 */

    /* USER CODE END 1 */

    /* MCU Configuration--------------------------------------------------------*/

    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    HAL_Init();

    /* USER CODE BEGIN Init */

    /* USER CODE END Init */

    /* Configure the system clock */
    SystemClock_Config();

    /* USER CODE BEGIN SysInit */

    /* USER CODE END SysInit */

    /* Initialize all configured peripherals */
    MX_GPIO_Init();
    MX_DMA_Init();
    MX_I2C1_Init();
    MX_I2S3_Init();
    MX_SPI2_Init();
    MX_USART1_UART_Init();
    MX_USART6_UART_Init();
    /* USER CODE BEGIN 2 */
    ZLCR_Beta_BSP_Init();
    ZLCR_Core_Init();
    /* USER CODE END 2 */

    /* USER CODE BEGIN RTOS_MUTEX */
    /* add mutexes, ... */
    /* USER CODE END RTOS_MUTEX */

    /* USER CODE BEGIN RTOS_SEMAPHORES */
    /* add semaphores, ... */
    /* USER CODE END RTOS_SEMAPHORES */

    /* USER CODE BEGIN RTOS_TIMERS */
    /* start timers, add new ones, ... */
    /* USER CODE END RTOS_TIMERS */

    /* USER CODE BEGIN RTOS_QUEUES */
    /* add queues, ... */
    /* USER CODE END RTOS_QUEUES */

    /* Create the thread(s) */
    /* definition and creation of defaultTask */
    osThreadDef(defaultTask, StartDefaultTask, osPriorityIdle, 0, 128);
    defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

    /* definition and creation of myTask02 */
    osThreadDef(myTask02, SysTask, osPriorityNormal, 0, 512);
    myTask02Handle = osThreadCreate(osThread(myTask02), NULL);

    /* USER CODE BEGIN RTOS_THREADS */
    /* add threads, ... */
    /* USER CODE END RTOS_THREADS */

    /* Start scheduler */
    osKernelStart();

    /* We should never get here as control is now taken by the scheduler */
    /* Infinite loop */
    /* USER CODE BEGIN WHILE */
    while (1)
    {
        /* USER CODE END WHILE */

        /* USER CODE BEGIN 3 */
    }
    /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
    RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

    /** Configure the main internal regulator output voltage
  */
    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
    /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM = 4;
    RCC_OscInitStruct.PLL.PLLN = 100;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLQ = 4;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }
    /** Initializes the CPU, AHB and APB buses clocks
  */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
    {
        Error_Handler();
    }
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_I2S;
    PeriphClkInitStruct.PLLI2S.PLLI2SN = 192;
    PeriphClkInitStruct.PLLI2S.PLLI2SM = 4;
    PeriphClkInitStruct.PLLI2S.PLLI2SR = 2;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
        Error_Handler();
    }
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{
    /* USER CODE BEGIN I2C1_Init 0 */

    /* USER CODE END I2C1_Init 0 */

    /* USER CODE BEGIN I2C1_Init 1 */

    /* USER CODE END I2C1_Init 1 */
    hi2c1.Instance = I2C1;
    hi2c1.Init.ClockSpeed = 100000;
    hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
    hi2c1.Init.OwnAddress1 = 0;
    hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    hi2c1.Init.OwnAddress2 = 0;
    hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
    if (HAL_I2C_Init(&hi2c1) != HAL_OK)
    {
        Error_Handler();
    }
    /* USER CODE BEGIN I2C1_Init 2 */

    /* USER CODE END I2C1_Init 2 */
}

/**
  * @brief I2S3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2S3_Init(void)
{
    /* USER CODE BEGIN I2S3_Init 0 */

    /* USER CODE END I2S3_Init 0 */

    /* USER CODE BEGIN I2S3_Init 1 */

    /* USER CODE END I2S3_Init 1 */
    hi2s3.Instance = SPI3;
    hi2s3.Init.Mode = I2S_MODE_MASTER_TX;
    hi2s3.Init.Standard = I2S_STANDARD_PHILIPS;
    hi2s3.Init.DataFormat = I2S_DATAFORMAT_16B;
    hi2s3.Init.MCLKOutput = I2S_MCLKOUTPUT_ENABLE;
    hi2s3.Init.AudioFreq = I2S_AUDIOFREQ_192K;
    hi2s3.Init.CPOL = I2S_CPOL_LOW;
    hi2s3.Init.ClockSource = I2S_CLOCK_PLL;
    hi2s3.Init.FullDuplexMode = I2S_FULLDUPLEXMODE_ENABLE;
    if (HAL_I2S_Init(&hi2s3) != HAL_OK)
    {
        Error_Handler();
    }
    /* USER CODE BEGIN I2S3_Init 2 */

    /* USER CODE END I2S3_Init 2 */
}

/**
  * @brief SPI2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI2_Init(void)
{
    /* USER CODE BEGIN SPI2_Init 0 */

    /* USER CODE END SPI2_Init 0 */

    /* USER CODE BEGIN SPI2_Init 1 */

    /* USER CODE END SPI2_Init 1 */
    /* SPI2 parameter configuration*/
    hspi2.Instance = SPI2;
    hspi2.Init.Mode = SPI_MODE_MASTER;
    hspi2.Init.Direction = SPI_DIRECTION_2LINES;
    hspi2.Init.DataSize = SPI_DATASIZE_8BIT;
    hspi2.Init.CLKPolarity = SPI_POLARITY_LOW;
    hspi2.Init.CLKPhase = SPI_PHASE_1EDGE;
    hspi2.Init.NSS = SPI_NSS_SOFT;
    hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
    hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
    hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
    hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    hspi2.Init.CRCPolynomial = 10;
    if (HAL_SPI_Init(&hspi2) != HAL_OK)
    {
        Error_Handler();
    }
    /* USER CODE BEGIN SPI2_Init 2 */

    /* USER CODE END SPI2_Init 2 */
}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

    /* USER CODE BEGIN USART1_Init 0 */

    /* USER CODE END USART1_Init 0 */

    /* USER CODE BEGIN USART1_Init 1 */

    /* USER CODE END USART1_Init 1 */
    huart1.Instance = USART1;
    huart1.Init.BaudRate = 115200;
    huart1.Init.WordLength = UART_WORDLENGTH_8B;
    huart1.Init.StopBits = UART_STOPBITS_1;
    huart1.Init.Parity = UART_PARITY_NONE;
    huart1.Init.Mode = UART_MODE_TX_RX;
    huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart1.Init.OverSampling = UART_OVERSAMPLING_16;
    if (HAL_UART_Init(&huart1) != HAL_OK)
    {
        Error_Handler();
    }
    /* USER CODE BEGIN USART1_Init 2 */

    /* USER CODE END USART1_Init 2 */
}

/**
  * @brief USART6 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART6_UART_Init(void)
{

    /* USER CODE BEGIN USART6_Init 0 */

    /* USER CODE END USART6_Init 0 */

    /* USER CODE BEGIN USART6_Init 1 */

    /* USER CODE END USART6_Init 1 */
    huart6.Instance = USART6;
    huart6.Init.BaudRate = 115200;
    huart6.Init.WordLength = UART_WORDLENGTH_8B;
    huart6.Init.StopBits = UART_STOPBITS_1;
    huart6.Init.Parity = UART_PARITY_NONE;
    huart6.Init.Mode = UART_MODE_TX_RX;
    huart6.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart6.Init.OverSampling = UART_OVERSAMPLING_16;
    if (HAL_UART_Init(&huart6) != HAL_OK)
    {
        Error_Handler();
    }
    /* USER CODE BEGIN USART6_Init 2 */

    /* USER CODE END USART6_Init 2 */
}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

    /* DMA controller clock enable */
    __HAL_RCC_DMA2_CLK_ENABLE();
    __HAL_RCC_DMA1_CLK_ENABLE();

    /* DMA interrupt init */
    /* DMA1_Stream0_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(DMA1_Stream0_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DMA1_Stream0_IRQn);
    /* DMA1_Stream7_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(DMA1_Stream7_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DMA1_Stream7_IRQn);
    /* DMA2_Stream1_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(DMA2_Stream1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DMA2_Stream1_IRQn);
    /* DMA2_Stream2_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(DMA2_Stream2_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DMA2_Stream2_IRQn);
    /* DMA2_Stream6_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(DMA2_Stream6_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DMA2_Stream6_IRQn);
    /* DMA2_Stream7_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(DMA2_Stream7_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DMA2_Stream7_IRQn);
}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOH_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);

    /*Configure GPIO pin : PC15 */
    GPIO_InitStruct.Pin = GPIO_PIN_15;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    /*Configure GPIO pin : PB12 */
    GPIO_InitStruct.Pin = GPIO_PIN_12;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used 
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void const *argument)
{
    /* USER CODE BEGIN 5 */
    /* Infinite loop */
    for (;;)
    {
        ZLCR_Core_IDLE();
    }
    /* USER CODE END 5 */
}

/* USER CODE BEGIN Header_SysTask */
/**
* @brief Function implementing the myTask02 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_SysTask */
void SysTask(void const *argument)
{
    /* USER CODE BEGIN SysTask */
    signed char cRxedChar;
    uint8_t ucInputIndex = 0;
    char *pcOutputString;
    static char cInputString[64], cLastInputString[64];
    BaseType_t xReturned;

    FreeRTOS_CLIRegisterCommand(&xZLCRCmd);
    FreeRTOS_CLIRegisterCommand(&xPythonCmd);
    FreeRTOS_CLIRegisterCommand(&xRebootCmd);
    pcOutputString = FreeRTOS_CLIGetOutputBuffer();

    ZLCR_Beta_BSP_REPL_PutString(pcWelcomeMessage, (unsigned short)strlen(pcWelcomeMessage));
    ZLCR_Beta_BSP_REPL_PutString(pcEndOfOutputMessage, (unsigned short)strlen(pcEndOfOutputMessage));

    /* Infinite loop */
    for (;;)
    {
        for (; ZLCR_Beta_BSP_REPL_GetChar(&cRxedChar) != pdPASS;)
        {
            osDelay(10);
        }
        ZLCR_Beta_BSP_REPL_PutChar(cRxedChar);

        if (cRxedChar == '\n' || cRxedChar == '\r')
        {
            if (ucInputIndex != 0)
            {
                ZLCR_Beta_BSP_REPL_PutString(pcNewLine, (unsigned short)strlen(pcNewLine));

                for (xReturned = pdTRUE; xReturned != pdFALSE;)
                {
                    xReturned = FreeRTOS_CLIProcessCommand(cInputString, pcOutputString, configCOMMAND_INT_MAX_OUTPUT_SIZE);
                    ZLCR_Beta_BSP_REPL_PutString(pcOutputString, (unsigned short)strlen(pcOutputString));
                }
            }

            strcpy(cLastInputString, cInputString);
            ucInputIndex = 0;
            memset(cInputString, 0x00, 64);

            ZLCR_Beta_BSP_REPL_PutString(pcEndOfOutputMessage, (unsigned short)strlen(pcEndOfOutputMessage));
        }
        else if ((cRxedChar == '\b') || (cRxedChar == 0x7F))
        {
            if (ucInputIndex > 0)
            {
                ucInputIndex--;
                cInputString[ucInputIndex] = '\0';
            }
        }
        else if ((cRxedChar >= ' ') && (cRxedChar <= '~'))
        {
            if (ucInputIndex < 64)
            {
                cInputString[ucInputIndex] = cRxedChar;
                ucInputIndex++;
            }
        }
    }
    /* USER CODE END SysTask */
}

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM9 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    /* USER CODE BEGIN Callback 0 */

    /* USER CODE END Callback 0 */
    if (htim->Instance == TIM9)
    {
        HAL_IncTick();
    }
    /* USER CODE BEGIN Callback 1 */

    /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
    /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state */

    /* USER CODE END Error_Handler_Debug */
}

#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
    /* USER CODE BEGIN 6 */
    /* User can add his own implementation to report the file name and line number,
ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
    /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
