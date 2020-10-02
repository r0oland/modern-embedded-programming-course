/* Blinky/Button with RTOS (FreeRTOS) and blocking */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "bsp.h"

/* The Blinky thread =======================================================*/
StackType_t stack_blinky[configMINIMAL_STACK_SIZE]; /* task stack */
StaticTask_t blinky; /* task object (FreeRTOS static alloc) */

enum { INITIAL_BLINK_TIME = (250U / portTICK_RATE_MS) };

/* data shared between tasks */
uint32_t volatile shared_blink_time = INITIAL_BLINK_TIME;
SemaphoreHandle_t shared_blink_time_mutex;
StaticSemaphore_t shared_blink_time_mutex_obj;

void main_blinky(void *pvParameters) { /* task function */
    (void)pvParameters; /* unused parameter(s) */

    while (1) { /* endless "superloop" */
        uint32_t bt; /* local copy of shared_blink_time */

        xSemaphoreTake(shared_blink_time_mutex,
                       portMAX_DELAY ); /* mutual exclusion */
        bt = shared_blink_time;
        xSemaphoreGive(shared_blink_time_mutex); /* mutual exclusion */

        BSP_ledGreenOn();
        vTaskDelay(bt);       /* BLOCKING! */
        BSP_ledGreenOff();
        vTaskDelay(bt * 3U);  /* BLOCKING! */
    }
}

/* The Button thread =======================================================*/
StackType_t stack_button[configMINIMAL_STACK_SIZE]; /* task stack */
StaticTask_t button; /* task object (FreeRTOS static alloc) */

void main_button(void *pvParameters) { /* task function */
    (void)pvParameters; /* unused parameter(s) */

    while (1) { /* endless "superloop" */
        /* wait on the button-press semaphore (BLOCK indefinitely) */
        xSemaphoreTake(BSP_semaPress, portMAX_DELAY); /* BLOCKING! */
        BSP_ledBlueOn();

        /* update the blink time for the 'blink' thread */
        xSemaphoreTake(shared_blink_time_mutex,
                       portMAX_DELAY ); /* mutual exclusion */
        shared_blink_time >>= 1; /* shorten the blink time by factor of 2 */
        if (shared_blink_time == 0U) {
            shared_blink_time = INITIAL_BLINK_TIME;
        }
        xSemaphoreGive(shared_blink_time_mutex); /* mutual exclusion */

        /* wait on the button-release semaphore (BLOCK indefinitely) */
        xSemaphoreTake(BSP_semaRelease, portMAX_DELAY); /* BLOCKING! */
        BSP_ledBlueOff();
    }
}

SemaphoreHandle_t BSP_semaPress;   /* global semaphore handle */
SemaphoreHandle_t BSP_semaRelease; /* global semaphore handle */

/* control-blocks for statically allocated FreeRTOS semaphores */
static StaticSemaphore_t semaPress;   /* semaphore object (FreeRTOS static alloc) */
static StaticSemaphore_t semaRelease; /* semaphore object (FreeRTOS static alloc) */

/* the main function =======================================================*/
int main() {

    BSP_init(); /* initialize the BSP */

    /* initialize the semaphores before using them */
    BSP_semaPress   = xSemaphoreCreateBinaryStatic(&semaPress);
    BSP_semaRelease = xSemaphoreCreateBinaryStatic(&semaRelease);
    shared_blink_time_mutex =
        xSemaphoreCreateMutexStatic(&shared_blink_time_mutex_obj);

    /* statically create FreeRTOS task */
    xTaskCreateStatic(
              &main_blinky,       /* the thread function */
              "blinky" ,          /* the name of the task */
              sizeof(stack_blinky)/sizeof(portSTACK_TYPE), /* stack length */
              (void *)0,          /* the 'pvParameters' parameter */
              2U + tskIDLE_PRIORITY, /* FreeRTOS priority */
              stack_blinky,       /* private stack storage */
              &blinky);           /* task object (FreeRTOS static alloc) */

    /* statically create FreeRTOS task */
    xTaskCreateStatic(
              &main_button,       /* the thread function */
              "button" ,          /* the name of the task */
              sizeof(stack_button)/sizeof(portSTACK_TYPE), /* stack length */
              (void *)0,          /* the 'pvParameters' parameter */
              1U + tskIDLE_PRIORITY, /* FreeRTOS priority */
              stack_button,       /* private stack storage */
              &button);           /* task object (FreeRTOS static alloc) */

    BSP_start(); /* configure and start interrupts */

    vTaskStartScheduler(); /* start the FreeRTOS scheduler... */
    return 0; /* NOTE: the scheduler does NOT return */
}

