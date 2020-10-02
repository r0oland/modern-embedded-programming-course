/*****************************************************************************
* Active Object pattern implementation based on uC/OS-II (uC/AO)
*
*                    Q u a n t u m  L e a P s
*                    ------------------------
*                    Modern Embedded Software
*
* Copyright (C) 2020 Quantum Leaps, LLC. All rights reserved.
*
* SPDX-License-Identifier: APACHE-2.0
*
* This software is distributed by Quantum Leaps, LLC under the terms of
* Apache License Version 2.0, which is the same license used for uC/OS-II RTOS.
* The text of the license is available at: www.apache.org/licenses/LICENSE-2.0.
*
* Contact information:
* <www.state-machine.com>
* <info@state-machine.com>
*****************************************************************************/
#include "uc_ao.h" /* uC/AO interface */

static char const this_module[] = "uc_ao"; /* this module name for Q_ASSERT() */

/*..........................................................................*/
void Active_ctor(Active * const me, DispatchHandler dispatch) {
    me->dispatch = dispatch; /* attach the dispatch handler for the "me" AO */
}

/*..........................................................................*/
/* Event-loop thread function for all Active Objects (uC/OS-II task signature) */
static void Active_eventLoop(void *pdata) {
    Active *me = (Active *)pdata; /* the AO instance "me" */
    static Event const initEvt = { INIT_SIG };

    Q_ASSERT(me); /* Active object must be provided */

    /* initialize the AO */
    (*me->dispatch)(me, &initEvt);

    for (;;) {   /* for-ever "superloop" */
        Event *e; /* pointer to event object ("message") */
        INT8U err; /* uC/OS-II error status */

        /* wait for any event and receive it into object 'e' */
        e = OSQPend(me->queue, 0U, &err); /* BLOCKING! */
        Q_ASSERT(err == 0U);

        /* dispatch event to the active object 'me' */
        (*me->dispatch)(me, e); /* NO BLOCKING! */
    }
}

/*..........................................................................*/
void Active_start(Active * const me,
                  uint8_t prio,       /* priority (1-based) */
                  Event **queueSto,
                  uint32_t queueLen,
                  void *stackSto,
                  uint32_t stackSize,
                  uint16_t opt)
{
    INT8U err;
    OS_STK *stk_sto = stackSto;
    INT32U stk_depth = (stackSize / sizeof(OS_STK));

    /* precondition */
    Q_ASSERT((0 < prio) && (prio < OS_LOWEST_PRIO - 2U));

    me->queue = OSQCreate((void **)queueSto, queueLen);
    Q_ASSERT(me->queue); /* queue must be created */

    me->thread = OS_LOWEST_PRIO - 2U - prio; /* uC/OS-II priority */
    err = OSTaskCreateExt(
              &Active_eventLoop,        /* the thread function */
              me,                       /* the 'pdata' parameter */
#if OS_STK_GROWTH
              &stk_sto[stk_depth - 1],  /* ptos */
#else
              stk_sto,                  /* ptos */
#endif
              me->thread,               /* uC/OS-II priority */
              prio,                     /* task ID -- the unique AO priority */
              stk_sto,                  /* pbos */
              stk_depth,                /* stack depth */
              (void *)0,                /* pext */
              opt);                     /* task options */
    Q_ASSERT(err == 0U); /* thread must be created */
}

/*..........................................................................*/
void Active_post(Active * const me, Event const * const e) {
    OSQPost(me->queue, (void *)e);
}
