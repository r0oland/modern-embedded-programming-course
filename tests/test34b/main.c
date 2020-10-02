/* Blinky/Button with RTOS (uC/OS-II) and blocking */
#include "uc_ao.h"  /* uC/AO API */
#include "bsp.h"

static char const this_module[] = "main"; /* this module name for Q_ASSERT() */

/* The Blinky thread =========================================================*/
OS_STK blinky_stack[100]; /* task stack */

enum { INITIAL_BLINK_TIME = (OS_TICKS_PER_SEC / 4) };

/* data shared between tasks */
INT32U volatile shared_blink_time = INITIAL_BLINK_TIME;
OS_EVENT *shared_blink_time_mutex;

void main_blinky(void *pdata) { /* task function */
    (void)pdata; /* unused parameter(s) */

    while (1) { /* endless "superloop" */
        INT8U err;
        INT32U bt; /* local copy of shared_blink_time */

        OSMutexPend(shared_blink_time_mutex, 0, &err); /* mutual exclusion */
        Q_ASSERT(err == 0U);
        bt = shared_blink_time;
        OSMutexPost(shared_blink_time_mutex); /* mutual exclusion */

        BSP_ledGreenOn();
        OSTimeDly(bt);       /* BLOCKING! */
        BSP_ledGreenOff();
        OSTimeDly(bt * 3U);  /* BLOCKING! */
    }
}

/* The Button AO ============================================================*/
typedef struct {
    Active super; /* inherit Active base class */
    /* add private data for the AO... */
} Button;
static void Button_dispatch(Button * const me, Event const * const e) {
    switch (e->sig) {
        case INIT_SIG: {
            BSP_ledBlueOff();
            break;
        }
        case BUTTON_PRESSED_SIG: {
            INT8U err; /* uC/OS-II error status */
            BSP_ledBlueOn();

            /* update the blink time for the 'blink' thread */
            OSMutexPend(shared_blink_time_mutex, 0, &err); /* mutual exclusion */
            Q_ASSERT(err == 0);
            shared_blink_time >>= 1; /* shorten the blink time by factor of 2 */
            if (shared_blink_time == 0U) {
                shared_blink_time = INITIAL_BLINK_TIME;
            }
            OSMutexPost(shared_blink_time_mutex); /* mutual exclusion */

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
void Button_ctor(Button * const me) {
    Active_ctor(&me->super, (DispatchHandler)&Button_dispatch);
}
OS_STK button_stack[100]; /* task stack */
static Event *button_queue[10];
static Button button;
Active *AO_Button = &button.super;

/* the main function =========================================================*/
int main() {
    INT8U err;

    BSP_init(); /* initialize the BSP */
    OSInit();   /* initialize uC/OS-II */

    /* initialize the RTOS objects before using them */
    shared_blink_time_mutex = OSMutexCreate(OS_LOWEST_PRIO - 5, &err);
    Q_ASSERT(err == 0U);

    /* create uC/OS-II task, see NOTE1 */
    err = OSTaskCreateExt(&main_blinky, /* the task function */
          (void *)0,      /* the 'pdata' parameter (not used) */
          &blinky_stack[(sizeof(blinky_stack) / sizeof(OS_STK)) - 1], /* ptos */
          OS_LOWEST_PRIO - 4, /* uC/OS-II task priority */
          OS_LOWEST_PRIO - 4, /* unique priority is used as the task ID */
          blinky_stack,   /* pbos */
          (INT32U)(sizeof(blinky_stack)/sizeof(OS_STK)),/* stack depth */
          (void *)0,      /* pext */
          0U);            /* task options */
    Q_ASSERT(err == 0U);

    /* create AO and start it */
    Button_ctor(&button);
    Active_start(AO_Button,
                 1U,
                 button_queue,
                 sizeof(button_queue)/sizeof(button_queue[0]),
                 button_stack,
                 sizeof(button_stack),
                 0U);

    BSP_start(); /* configure and start the interrupts */

    OSStart(); /* start the uC/OS-II scheduler... */
    return 0; /* NOTE: the scheduler does NOT return */
}

/*******************************************************************************
* NOTE1:
* The call to uC/OS-II API OSTaskCreateExt() assumes that the pointer to the
* top-of-stack (ptos) is at the end of the provided stack memory. This is
* correct only for CPUs with downward-growing stack, but must be changed for
* CPUs with upward-growing stack.
*/
