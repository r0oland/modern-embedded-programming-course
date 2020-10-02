#ifndef __BSP_H__
#define __BSP_H__

/* system clock tick [Hz] */
#define BSP_TICKS_PER_SEC 1000U

void BSP_init(void);
void BSP_start(void);

void BSP_ledRedOn(void);
void BSP_ledRedOff(void);

void BSP_ledBlueOn(void);
void BSP_ledBlueOff(void);
void BSP_ledBlueToggle(void);

void BSP_ledGreenOn(void);
void BSP_ledGreenOff(void);
void BSP_ledGreenToggle(void);

/*---------------------------------------------------------------------------*/
/* Event facilities... */

typedef uint16_t Signal; /* event signal */

/* Event base class */
typedef struct {
    Signal sig; /* event signal */
    /* event parameters added in subclasses of Event */
} Event;

enum EventSignals {
    TIMEOUT_SIG,
    BUTTON_PRESSED_SIG,
    BUTTON_RELEASED_SIG,
};

/* Event queues... */
extern QueueHandle_t BSP_queue_blinky; /* global message queue handle */
extern QueueHandle_t BSP_queue_button; /* global message queue handle */

extern uint16_t BSP_timout_blinky;     /* global timeout counter */

#endif // __BSP_H__
