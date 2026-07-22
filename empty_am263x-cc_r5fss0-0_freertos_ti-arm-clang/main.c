#include <stdlib.h>

#include <kernel/dpl/DebugP.h>

#include "ti_drivers_config.h"
#include "ti_board_config.h"

#include "FreeRTOS.h"
#include "task.h"

#define MAIN_TASK_PRI   (configMAX_PRIORITIES - 1)

#define MAIN_TASK_SIZE  (16384U / sizeof(configSTACK_DEPTH_TYPE))

StackType_t gMainTaskStack[MAIN_TASK_SIZE]
    __attribute__((aligned(32)));

StaticTask_t gMainTaskObj;

TaskHandle_t gMainTask;

void empty_main(void *args);

void freertos_main(void *args)
{
    (void)args;

empty_main(NULL);


vTaskDelete(NULL);

}

int main(void)
{

System_init();
Board_init();


gMainTask =
    xTaskCreateStatic(
        freertos_main,
        "freertos_main",
        MAIN_TASK_SIZE,
        NULL,
        MAIN_TASK_PRI,
        gMainTaskStack,
        &gMainTaskObj);


configASSERT(gMainTask != NULL);

vTaskStartScheduler();

DebugP_assertNoLog(0);

Board_deinit();

System_deinit();

return 0;

}
