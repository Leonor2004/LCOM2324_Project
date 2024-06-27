#include <lcom/lcf.h>

#include <lcom/lab3.h>
#include "kbd.h"
#include "i8042.h"

int hook_kbc = 1;

// Subscribes to keyboard interrupts.
int(kbd_subscribe_int)(uint8_t *IRQ) {
  if(IRQ == NULL) { // Check if the IRQ pointer is NULL to avoid dereferencing it.
    printf("Bit_no is a null pointer\n"); // Print error message if IRQ is NULL.
    return 1; // Return failure.
  }
  *IRQ = hook_kbc; // Store the hook ID for the keyboard controller in *IRQ.
  // Set the keyboard interrupt policy to re-enable interrupts and mark them as exclusive to this handler.
  sys_irqsetpolicy(KEYBOARD_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &hook_kbc);
  return 0; // Return success.
}

// Unsubscribes to keyboard interrupts.
int(kbd_unsubscribe_int)() {
  // Remove the interrupt subscription policy for the keyboard controller.
  sys_irqrmpolicy(&hook_kbc);
  return 0; // Return success.
}


int (read_output)(uint8_t *scancode) {
  uint8_t st = 0;

  if (util_sys_inb(ST_REG, &st) != 0) {
    printf("Error (getting stats)");
    return 1;
  }

  if (OBF & st) {
    if (util_sys_inb(OUT_BUF, scancode) != 0) {
      printf("Error (reading buffer)");
      return 1;
    }

    if ((((st & (PARITY_ERROR | TIMEOUT_ERROR)) == 0) && ((st & AUX) == 0))) {
      return 0;
    }
  }
  return 1;
}

int (restoreKBD)() {
  uint8_t cmd;
  if (sys_outb(IN_BUF_COMMANDS, READ_COMMAND_BACK) != 0) return 1;
  if (util_sys_inb(OUT_BUF, &cmd) != 0) return 1;
  cmd |= BIT(0);
  if (sys_outb(IN_BUF_COMMANDS, WRITE_COMAND_BACK) != 0) return 1;
  if (sys_outb(IN_BUF_ARGUMENTS, cmd) != 0) return 1;
  return 0;
}
