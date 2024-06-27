#include <lcom/lcf.h>

#include <lcom/lab4.h>
#include "kbc.h"
#include "i8042.h"

int (read_output)(uint8_t port, uint8_t *scancode, uint8_t mouse) {
  uint8_t st = 0;

  if (util_sys_inb(ST_REG, &st) != 0) {
    printf("Error (getting stats)");
    return 1;
  }

  if (OBF & st) {
    if (util_sys_inb(port, scancode) != 0) {
      printf("Error (reading buffer)");
      return 1;
    }

    if (((st & (PARITY_ERROR | TIMEOUT_ERROR)) != 0)) {
      return 1;
    }

    if (mouse && ((st & AUX) == 0)) return 1;
    if (!mouse && ((st & AUX) != 0)) return 1;

    return 0;
  }
  return 1;
}
