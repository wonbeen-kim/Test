#include <stdlib.h>
#include <kernel/dpl/DebugP.h>
#include "ti_drivers_config.h"
#include "ti_board_config.h"
#include "FreeRTOS.h"
#include "task.h"

/* Main Task를 최고 우선순위로 설정 */
#define MAIN_TASK_PRI  (configMAX_PRIORITIES-1)

/* Main Task가 사용할 Stack 크기 (16KB) */
#define MAIN_TASK_SIZE (16384U/sizeof(configSTACK_DEPTH_TYPE))

/* Main Task의 Stack 메모리 (32Byte 정렬) */
StackType_t gMainTaskStack[MAIN_TASK_SIZE] __attribute__((aligned(32)));

/* Main Task의 Task Control Block(TCB) */
StaticTask_t gMainTaskObj;

/* Main Task를 제어하기 위한 Handle */
TaskHandle_t gMainTask;

/* 사용자 애플리케이션 함수 */
void empty_main(void *args);

/*
 * FreeRTOS에서 가장 먼저 실행되는 Main Task
 *
 * 1. 사용자 애플리케이션 실행
 * 2. 실행이 끝나면 자기 자신(Task)을 삭제
 */

void freertos_main(void *args)
{
    empty_main(NULL);

    /* 현재 Task 종료 */
    vTaskDelete(NULL);
}

int main(void)
{
    /* SoC(System on Chip) 관련 모듈 초기화 */
    System_init();

    /* Board 관련 초기화 */
    Board_init();

    /*
     * 최고 우선순위의 Main Task 생성
     * Main Task는 필요한 다른 Task를 생성한 후
     * 자신의 역할이 끝나면 삭제되는 것을 권장한다.
     */
    gMainTask = xTaskCreateStatic(
                    freertos_main,   /* Task에서 실행할 함수 */
                    "freertos_main", /* 디버깅을 위한 Task 이름 */
                    MAIN_TASK_SIZE,  /* Task Stack 크기 (StackType_t 단위) */
                    NULL,            /* Task 전달 인자 (사용하지 않음) */
                    MAIN_TASK_PRI,   /* Task 우선순위 (0: 최저, configMAX_PRIORITIES-1: 최고) */
                    gMainTaskStack,  /* Task Stack 시작 주소 */
                    &gMainTaskObj ); /* 정적으로 할당한 Task Control Block(TCB) */

    /* Task 생성 성공 여부 확인 */
    configASSERT(gMainTask != NULL);

    /* FreeRTOS Scheduler 시작 */
    vTaskStartScheduler();

    /*
     * 일반적으로 이 코드는 실행되지 않는다.
     *
     * vTaskStartScheduler()가 반환되는 경우는
     * Idle Task 또는 Timer Task를 생성할 Heap 메모리가
     * 부족한 경우뿐이다.
     */
    DebugP_assertNoLog(0);
    Board_deinit();
    System_deinit();

    return 0;
}
