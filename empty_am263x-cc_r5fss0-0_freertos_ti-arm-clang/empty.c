#include <stdint.h>
#include <drivers/gpio.h>
#include <kernel/dpl/AddrTranslateP.h>
#include <kernel/dpl/ClockP.h>
#include <kernel/dpl/DebugP.h>

#include "ti_drivers_config.h"
#include "ti_drivers_open_close.h"
#include "ti_board_open_close.h"


void empty_main(void *args)
{
    uint32_t loopcnt = 5U;
    uint32_t delayUs = 2000000U;
    uint32_t gpioBaseAddr;
    uint32_t pinNum;

    (void)args;

    Drivers_open();

    Board_driversOpen();

    gpioBaseAddr =
        (uint32_t)AddrTranslateP_getLocalAddr(GPIO_LED_BASE_ADDR);

    pinNum = GPIO_LED_PIN;

    GPIO_setDirMode(gpioBaseAddr,
                    pinNum,
                    GPIO_LED_DIR);

#if defined(AMP_FREERTOS_A53)

    DebugP_log("GPIO LED Blink Test Started on a53_core%d ...\r\n",
               Armv8_getCoreId());

#else

    DebugP_log("GPIO LED Blink Test Started ...\r\n");

#endif

    DebugP_log("LED will Blink for %d seconds ...\r\n",
               (loopcnt * delayUs * 2U) / 1000000U);

    while (loopcnt > 0U)
    {
        GPIO_pinWriteHigh(gpioBaseAddr, pinNum);
        ClockP_usleep(delayUs);

        GPIO_pinWriteLow(gpioBaseAddr, pinNum);
        ClockP_usleep(delayUs);

        loopcnt--;
    }

    DebugP_log("GPIO LED Blink Test Passed!!\r\n");
    DebugP_log("All tests have passed!!\r\n");

    Board_driversClose();

    Drivers_close();
}
