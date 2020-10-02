#include "FreeRTOS.h"  /* FreeRTOS master include file, see NOTE4 */
#include "task.h"      /* FreeRTOS task  management */
#include "semphr.h"    /* FreeRTOS semaphore management */
#include "bsp.h"

StaticSemaphore_t SW1_sema;

portSTACK_TYPE stack_blinky1[80];
StaticTask_t blinky1;
void main_blinky1(void *par) {
    while (1) {
        uint32_t volatile i;
        for (i = 1500U; i != 0U; --i) {
            BSP_ledGreenOn();
            BSP_ledGreenOff();
        }
        vTaskDelay(1U); /* block for 1 tick */
    }
}

portSTACK_TYPE stack_blinky2[80];
StaticTask_t blinky2;
void main_blinky2(void *par) {
    while (1) {
        uint32_t volatile i;

        xSemaphoreTake((SemaphoreHandle_t)&SW1_sema, portMAX_DELAY);

        for (i = 3*1500U; i != 0U; --i) {
            BSP_ledBlueOn();
            BSP_ledBlueOff();
        }
        //vTaskDelay(50);
    }
}

portSTACK_TYPE stack_blinky3[80];
StaticTask_t blinky3;
void main_blinky3(void *par) {
    while (1) {
        BSP_ledRedOn();
        vTaskDelay(BSP_TICKS_PER_SEC / 3U);
        BSP_ledRedOff();
        vTaskDelay(BSP_TICKS_PER_SEC * 3U / 5U);
    }
}

int main() {
    TaskHandle_t th;

    BSP_init();

    /* initialize the SW1 semaphore as binary, signaling semaphore */
    xSemaphoreCreateBinaryStatic(&SW1_sema);

    /* initialize and start blinky1 thread */
    th = xTaskCreateStatic(
              &main_blinky1,    /* the task function */
              "T1" ,            /* the name of the task */
              (uint16_t)sizeof(stack_blinky1)/sizeof(portSTACK_TYPE), /* stack size */
              (void *)&blinky1, /* the 'pvParameters' parameter */
              (UBaseType_t)(5 + tskIDLE_PRIORITY),  /* FreeRTOS priority */
              (StackType_t *)stack_blinky1, /* stack storage */
              &blinky1);        /* task buffer */

    /* initialize and start blinky2 thread */
    th = xTaskCreateStatic(
              &main_blinky2,    /* the task function */
              "T2" ,            /* the name of the task */
              (uint16_t)sizeof(stack_blinky2)/sizeof(portSTACK_TYPE), /* stack size */
              (void *)&blinky2, /* the 'pvParameters' parameter */
              (UBaseType_t)(2 + tskIDLE_PRIORITY),  /* FreeRTOS priority */
              (StackType_t *)stack_blinky2, /* stack storage */
              &blinky2);        /* task buffer */

    /* initialize and start blinky3 thread */
    //QXThread_ctor(&blinky3, &main_blinky3, 0);
    //QXTHREAD_START(&blinky3,
    //               1U, /* priority */
    //               (void *)0, 0, /* message queue (not used) */
    //               stack_blinky3, sizeof(stack_blinky3), /* stack */
    //               (void *)0); /* extra parameter (not used) */

    /* configure and start interrupts */
    BSP_start();

    /* transfer control to the RTOS to run the threads */
    vTaskStartScheduler();

    //return 0;
}
