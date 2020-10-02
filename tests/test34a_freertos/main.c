/* Blinky/Button with RTOS (FreeRTOS) and blocking */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "bsp.h"
#include <stdbool.h>

/* The Blinky thread =======================================================*/
StackType_t stack_blinky[configMINIMAL_STACK_SIZE]; /* task stack */
StaticTask_t blinky; /* task object (FreeRTOS static alloc) */

QueueHandle_t BSP_queue_blinky; /* message queue handle */
StaticQueue_t queue_blinky;     /* queue object (FreeRTOS static alloc) */
Event *queue_blinky_sto[10];    /* queue storage for events */
uint16_t BSP_timout_blinky;     /* global timeout counter */
bool isLedOn;

enum { INITIAL_BLINK_TIME = (250U / portTICK_RATE_MS) };

/* data shared between tasks */
uint32_t volatile global_blink_time = INITIAL_BLINK_TIME;

void main_blinky(void *pvParameters) { /* task function */
    uint16_t tout;
    (void)pvParameters; /* unused parameter(s) */

    vTaskSuspendAll(); /* mutual exclusion: scheduler lock */
    tout = global_blink_time; /* extract volatile to local */
    xTaskResumeAll(); /* mutual exclusion: scheduler unlock */

    BSP_ledGreenOn();
    isLedOn = true;

    /* arm the time event */
    taskENTER_CRITICAL();
    BSP_timout_blinky = tout;
    taskEXIT_CRITICAL();

    while (1) { /* endless "superloop" */
        Event *e; /* pointer to event object ("message") */

        /* wait for any event and receive it into object 'e' */
        xQueueReceive(BSP_queue_blinky, &e, portMAX_DELAY); /* BLOCKING! */

         /* process the received event */
        switch (e->sig) {
            case TIMEOUT_SIG: {

                vTaskSuspendAll(); /* mutual exclusion: scheduler lock */
                tout = global_blink_time;
                xTaskResumeAll(); /* mutual exclusion: scheduler unlock */

                if (!isLedOn) {
                    BSP_ledGreenOn();
                    isLedOn = true;

                    /* arm the time event */
                    taskENTER_CRITICAL();
                    BSP_timout_blinky = tout;
                    taskEXIT_CRITICAL();
                }
                else {
                    BSP_ledGreenOff();
                    isLedOn = false;

                    /* arm the time event */
                    taskENTER_CRITICAL();
                    BSP_timout_blinky = tout * 3U;
                    taskEXIT_CRITICAL();
                }
                break;
            }
        }
    }
}

/* The Button thread =======================================================*/
StackType_t stack_button[configMINIMAL_STACK_SIZE]; /* task stack */
StaticTask_t button;     /* task object (FreeRTOS static alloc) */

QueueHandle_t BSP_queue_button; /* message queue handle */
StaticQueue_t queue_button;     /* queue object (FreeRTOS static alloc) */
Event *queue_button_sto[10];    /* queue storage for events */

void main_button(void *pvParameters) { /* task function */
    (void)pvParameters; /* unused parameter(s) */

    while (1) { /* endless "superloop" (event-loop) */
        Event *e; /* pointer to event object ("message") */

        /* wait for any event and receive it into object 'e' */
        xQueueReceive(BSP_queue_button, &e, portMAX_DELAY); /* BLOCKING! */

        /* process the received event */
        switch (e->sig) {
            case BUTTON_PRESSED_SIG: {
                BSP_ledBlueOn();

                /* update the blink time for the 'blink' thread */
                vTaskSuspendAll(); /* mutual exclusion: scheduler lock */
                global_blink_time >>= 1; /* shorten by factor of 2 */
                if (global_blink_time == 0U) {
                    global_blink_time = INITIAL_BLINK_TIME;
                }
                xTaskResumeAll(); /* mutual exclusion: scheduler unlock */

                break;
            }
            case BUTTON_RELEASED_SIG: {
                BSP_ledBlueOff();
                break;
            }
            default: {
                break;
            }
        }
    }
}

/* the main function =======================================================*/
int main() {

    BSP_init(); /* initialize the BSP */

    /* initialize event queues before using them */
    BSP_queue_blinky = xQueueCreateStatic(
        sizeof(queue_blinky_sto)/sizeof(queue_blinky_sto[0]),
        sizeof(Event *),     /* item size */
        (uint8_t *)queue_blinky_sto, /* queue storage - provided by user */
        &queue_blinky);      /* queue object (FreeRTOS static alloc) */
    BSP_queue_button = xQueueCreateStatic(
        sizeof(queue_button_sto)/sizeof(queue_button_sto[0]),
        sizeof(Event *),     /* item size */
        (uint8_t *)queue_button_sto, /* queue storage - provided by user */
        &queue_button);      /* queue object (FreeRTOS static alloc) */

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

