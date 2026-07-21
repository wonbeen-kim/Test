#include <stdint.h>
#include <drivers/gpio.h>
#include <kernel/dpl/AddrTranslateP.h>
#include <kernel/dpl/ClockP.h>
#include <kernel/dpl/DebugP.h>

#include "ti_drivers_config.h"
#include "ti_drivers_open_close.h"
#include "ti_board_open_close.h"

/*
 * 사용자 애플리케이션의 시작 함수
 *
 * FreeRTOS의 Main Task에서 호출된다.
 * SysConfig에서 설정한 GPIO를 출력으로 구성하고,
 * LED를 1초 간격으로 5회 점멸한 후 종료한다.
 *
 * args:
 * - Task 호출 시 전달되는 인자
 * - 현재 예제에서는 사용하지 않는다.
 */
void empty_main(void *args)
{
    uint32_t loopcnt = 5U;
    uint32_t delayUs = 2000000U;
    uint32_t gpioBaseAddr;
    uint32_t pinNum;

    /* 현재 예제에서는 전달 인자를 사용하지 않는다. */
    (void)args;

    /* SysConfig에서 생성된 드라이버를 초기화한다. */
    Drivers_open();

    /* 보드에 연결된 장치를 초기화한다. */
    Board_driversOpen();

    /* SysConfig의 GPIO 주소를 현재 코어에서 접근 가능한 주소로 변환한다. */
    gpioBaseAddr =
        (uint32_t)AddrTranslateP_getLocalAddr(GPIO_LED_BASE_ADDR);

    /* SysConfig에서 생성한 LED GPIO 핀 번호를 가져온다. */
    pinNum = GPIO_LED_PIN;

    /* LED가 연결된 GPIO 핀을 출력 모드로 설정한다. */
    GPIO_setDirMode(gpioBaseAddr,
                    pinNum,
                    GPIO_LED_DIR);

#if defined(AMP_FREERTOS_A53)

    /* A53 환경에서는 현재 실행 중인 코어 번호를 함께 출력한다. */
    DebugP_log("GPIO LED Blink Test Started on a53_core%d ...\r\n",
               Armv8_getCoreId());

#else

    /* R5F 등 A53 이외의 환경에서 테스트 시작을 출력한다. */
    DebugP_log("GPIO LED Blink Test Started ...\r\n");

#endif

    DebugP_log("LED will Blink for %d seconds ...\r\n",
               (loopcnt * delayUs * 2U) / 1000000U);

    /* 지정한 횟수만큼 LED를 ON/OFF 한다. */
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

    /* 보드 드라이버에서 사용한 자원을 정리한다. */
    Board_driversClose();

    /* 주변장치 드라이버에서 사용한 자원을 정리한다. */
    Drivers_close();
}
