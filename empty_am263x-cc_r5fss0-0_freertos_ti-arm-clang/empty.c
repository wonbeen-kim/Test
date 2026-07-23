#include <stdint.h>
#include <stdio.h>

#include <drivers/gpio.h>
#include <drivers/rti.h>
#include <drivers/adc.h>
#include <drivers/uart.h>

#include <kernel/dpl/AddrTranslateP.h>
#include <kernel/dpl/DebugP.h>

#include "ti_drivers_config.h"
#include "ti_drivers_open_close.h"
#include "ti_board_open_close.h"


#define LED_ON              (0x01U)
#define LED_OFF             (0x00U)
#define LED_BLINK_COUNT     (10U)

#define ADC_TIMEOUT_COUNT   (1000000U)
#define UART_TX_BUFFER_SIZE (64U)


volatile uint32_t gLedState;
volatile uint32_t gBlinkCount;
volatile uint32_t gAdcRequest;

uint32_t gAdcValue;

uint32_t gpioBaseAddr;
uint32_t pinNum;

const uint32_t adcBaseAddr = CONFIG_ADC0_BASE_ADDR;


void uartPrintAdcValue(uint32_t adcValue)
{
    char uartTxBuffer[UART_TX_BUFFER_SIZE];

    int32_t messageLength;

    UART_Transaction uartTransaction;


    messageLength = snprintf(
        uartTxBuffer,
        UART_TX_BUFFER_SIZE,
        "[ADC Test] Result : %u\r\n",
        adcValue
    );


    if (messageLength > 0)
    {
        UART_Transaction_init(&uartTransaction);

        uartTransaction.buf =
            (void *)uartTxBuffer;

        uartTransaction.count =
            (uint32_t)messageLength;


        UART_write(
            gUartHandle[CONFIG_UART_CONSOLE],
            &uartTransaction
        );
    }
}


void empty_main(void *args)
{
    uint32_t timeout;

    (void)args;


    Drivers_open();
    Board_driversOpen();


    DebugP_log("[ADC Test] Starting ...\r\n");

    ADC_setInterruptSource(
        adcBaseAddr,
        ADC_INT_NUMBER1,
        ADC_SOC_NUMBER0
    );


    ADC_clearInterruptStatus(
        adcBaseAddr,
        ADC_INT_NUMBER1
    );


    DebugP_log(
        "[RTI LED Blink Test] Starting ...\r\n"
    );


    gpioBaseAddr =
        (uint32_t)AddrTranslateP_getLocalAddr(
            GPIO_LED_BASE_ADDR
        );


    pinNum = GPIO_LED_PIN;


    gLedState   = LED_ON;
    gBlinkCount = 0U;
    gAdcRequest = 0U;


    GPIO_setDirMode(
        gpioBaseAddr,
        pinNum,
        GPIO_LED_DIR
    );


    GPIO_pinWriteHigh(
        gpioBaseAddr,
        pinNum
    );


    (void)RTI_counterEnable(
        CONFIG_RTI0_BASE_ADDR,
        RTI_TMR_CNT_BLK_INDEX_0
    );


    DebugP_log(
        "[RTI LED Blink Test] Timer Started ...\r\n"
    );


    while (gBlinkCount < LED_BLINK_COUNT)
    {
        if (gAdcRequest == 1U)
        {

            gAdcRequest = 0U;


            ADC_clearInterruptStatus(
                adcBaseAddr,
                ADC_INT_NUMBER1
            );


            ADC_forceSOC(
                adcBaseAddr,
                ADC_SOC_NUMBER0
            );


            timeout = ADC_TIMEOUT_COUNT;


            while ((ADC_getInterruptStatus(
                        adcBaseAddr,
                        ADC_INT_NUMBER1
                    ) == false) &&
                   (timeout > 0U))
            {
                timeout--;
            }


            if (timeout == 0U)
            {
                DebugP_log(
                    "[ADC Test] Conversion timeout\r\n"
                );
            }
            else
            {

                gAdcValue = ADC_readResult(
                    CONFIG_ADC0_RESULT_BASE_ADDR,
                    ADC_SOC_NUMBER0
                );


                ADC_clearInterruptStatus(
                    adcBaseAddr,
                    ADC_INT_NUMBER1
                );


                uartPrintAdcValue(
                    gAdcValue
                );
            }
        }
    }



    (void)RTI_counterDisable(
        CONFIG_RTI0_BASE_ADDR,
        RTI_TMR_CNT_BLK_INDEX_0
    );


    DebugP_log(
        "[RTI LED Blink Test] Timer Stopped ...\r\n"
    );

    DebugP_log(
        "[RTI LED Blink Test] Passed!!\r\n"
    );


    Board_driversClose();
    Drivers_close();
}


void rtiEvent0(void)
{

    if (gLedState == LED_ON)
    {
        GPIO_pinWriteLow(
            gpioBaseAddr,
            pinNum
        );

        gLedState = LED_OFF;
    }
    else
    {
        GPIO_pinWriteHigh(
            gpioBaseAddr,
            pinNum
        );

        gLedState = LED_ON;
    }



    gBlinkCount++;


    gAdcRequest = 1U;
}
