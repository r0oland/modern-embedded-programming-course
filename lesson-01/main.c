#include <stdint.h>
#include <stdbool.h>

int main() {
  *((uint32_t *)0x400FE608U) = 0x20U; // enable clock to GPIOF
  *((uint32_t *)0x40025400U) = 0x0EU; // enable digital I/O on PF1, PF2, PF3
  *((uint32_t *)0x4002551CU) = 0x0EU; // enable LED on PF1, PF2, PF3

  while (true) {
    *((uint32_t *)0x400253FCU) = 0x0EU; // turn on LED on PF1, PF2, PF3
    for (int i = 0; i < 1000000; i++) {
    }
    *((uint32_t *)0x400253FCU) = 0x00U; // turn off LED on PF1, PF2, PF3
    for (int i = 0; i < 1000000; i++) {
    }
  }

  return 0;
}
