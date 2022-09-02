#include "intrinsics.h"

int main() {
  int counter = 0;
  while (counter < 21) {
    ++counter;
    if ((counter & 1) != 0) {
      /* do something when the counter is odd */
      __no_operation();
      asm("NOP");
    } else {
      /* do something when the counter is even */
      __iar_builtin_no_operation();
      asm("NOP");
    }
  }

  return 0;
}
