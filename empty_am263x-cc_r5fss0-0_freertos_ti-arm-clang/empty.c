#include <stdint.h>

#include <drivers/gpio.h>
#include <drivers/rti.h>

#include <kernel/dpl/AddrTranslateP.h>
#include <kernel/dpl/DebugP.h>

#include "ti_drivers_config.h"
#include "ti_drivers_open_close.h"
#include "ti_board_open_close.h"

#define LED_ON           (0x01U)
#define LED_OFF          (0x00U)
#define LED_BLINK_COUNT  (10U)

volatile uint32_t gLedState;
volatile uint32_t gBlinkCount;

uint32_t gpioBaseAddr;
uint32_t pinNum;

void empty_main(void *args)
{
    (void)args;

Drivers_open();
Board_driversOpen();

DebugP_log("[RTI LED Blink Test] Starting ...\\r\\n");

gpioBaseAddr =
    (uint32_t)AddrTranslateP_getLocalAddr(GPIO_LED_BASE_ADDR);

pinNum = GPIO_LED_PIN;

gLedState = LED_ON;


gBlinkCount = 0U;

GPIO_setDirMode(gpioBaseAddr,
                pinNum,
                GPIO_LED_DIR);


GPIO_pinWriteHigh(gpioBaseAddr,
                  pinNum);


(void)RTI_counterEnable(CONFIG_RTI0_BASE_ADDR,
                        RTI_TMR_CNT_BLK_INDEX_0);

DebugP_log("[RTI LED Blink Test] Timer Started ...\\r\\n");


while (gBlinkCount < LED_BLINK_COUNT)
{

}


(void)RTI_counterDisable(CONFIG_RTI0_BASE_ADDR,
                         RTI_TMR_CNT_BLK_INDEX_0);

DebugP_log("[RTI LED Blink Test] Timer Stopped ...\\r\\n");

DebugP_log("GPIO LED Blink Test Passed!!\\r\\n");
DebugP_log("All tests have passed!!\\r\\n");


Board_driversClose();

Drivers_close();

}

void rtiEvent0(void)
{

if (gLedState == LED_ON)
{
    GPIO_pinWriteLow(gpioBaseAddr,
                     pinNum);

    gLedState = LED_OFF;
}

else
{
    GPIO_pinWriteHigh(gpioBaseAddr,
                      pinNum);

    gLedState = LED_ON;
}

gBlinkCount++;

}
