#ifndef __BSP_H__
#define __BSP_H__

/* system clock tick [Hz] */
#define BSP_TICKS_PER_SEC 1000U

void BSP_init(void);

enum {
    BSP_LED_RED = 1U,
    BSP_LED_BLUE,
    BSP_LED_GREEN,
    BSP_LED_EXT1,
    BSP_LED_EXT2,
    /* ... */
};
void BSP_ledOn(uint8_t num);
void BSP_ledOff(uint8_t num);

void BSP_ledRedOn(void);
void BSP_ledRedOff(void);

void BSP_ledBlueOn(void);
void BSP_ledBlueOff(void);
void BSP_ledBlueToggle(void);

void BSP_ledGreenOn(void);
void BSP_ledGreenOff(void);
void BSP_ledGreenToggle(void);

void BSP_sendMorseCode(uint32_t bitmask);

extern QXSemaphore SW1_sema;

#endif // __BSP_H__
