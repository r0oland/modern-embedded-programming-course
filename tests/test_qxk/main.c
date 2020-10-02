#include "qpc.h"  /* QP/C framework / QXK RTOS */
#include "bsp.h"  /* Board Support Package */

/* The threads ===============================================================*/
static QXThread thread_blinky1;  /* Thread object for the Blinky thread */
static uint32_t stack_blinky1[40]; /* stack for the Blinky thread */
static void main_blinky1(QXThread * const me) { /* thread function definition */
    (void)me;
    while (1) {
        BSP_ledGreenOn();
        QXThread_delay(BSP_TICKS_PER_SEC / 4U );  /* BLOCK */
        BSP_ledGreenOff();
        QXThread_delay(BSP_TICKS_PER_SEC * 3U / 4U);  /* BLOCK */
    }
}

static QXThread thread_blinky2;  /* Thread object for the Blinky thread */
static uint32_t stack_blinky2[40]; /* stack for the Blinky thread */
static void main_blinky2(QXThread * const me) { /* thread function definition */
    (void)me;
    while (1) {
        BSP_ledBlueOn();
        QXThread_delay(BSP_TICKS_PER_SEC / 2U );  /* BLOCK */
        BSP_ledBlueOff();
        QXThread_delay(BSP_TICKS_PER_SEC / 3U);  /* BLOCK */
    }
}

/* the main function =======================================================*/
int main() {
    QF_init(); /* initialize the framework/RTOS */
    BSP_init(); /* initialize the BSP */

    /* start the blinky1 thread */
    QXThread_ctor(&thread_blinky1, &main_blinky1, 0U);
    QXTHREAD_START(&thread_blinky1,      /* Thread to start */
                  (uint_fast8_t)1,       /* priority of the thread */
                  (QEvt *)0,             /* message queue storage (not used) */
                  0,                     /* queue length [events] (not used) */
                  stack_blinky1,         /* stack storage */
                  sizeof(stack_blinky1), /* stack size [bytes] */
                  (QEvt *)0);            /* initialization event (not used) */

    /* start the blinky2 thread */
    QXThread_ctor(&thread_blinky2, &main_blinky2, 0U);
    QXTHREAD_START(&thread_blinky2,      /* Thread to start */
                  (uint_fast8_t)2,       /* priority of the thread */
                  (QEvt *)0,             /* message queue storage (not used) */
                  0,                     /* queue length [events] (not used) */
                  stack_blinky2,         /* stack storage */
                  sizeof(stack_blinky2), /* stack size [bytes] */
                  (QEvt *)0);            /* initialization event (not used) */

    return QF_run(); /* run the application */
}
