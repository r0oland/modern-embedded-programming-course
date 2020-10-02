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

extern SemaphoreHandle_t BSP_semaPress;   /* global semaphore handle */
extern SemaphoreHandle_t BSP_semaRelease; /* global semaphore handle */

#endif // __BSP_H__
