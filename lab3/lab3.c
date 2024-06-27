#include <lcom/lcf.h>

#include <lcom/lab3.h>
#include <lcom/timer.h>
#include "i8042.h"
#include "kbd.h"


#include <stdbool.h>
#include <stdint.h>

uint8_t scancode;
extern int count;          // utils.c
extern int global_counter; // timer.c

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/lab3/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab3/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int(kbd_test_scan)() {
  int ipc_status;       // Status of IPC
  message msg;          // Stores received message
  uint8_t IRQ;              // IRQ line number
  bool isMakeCode;      // Flag for make (true) or break (false) code
  bool second = false;  // Flag for second byte of scancode
  uint8_t bytes[2];     // Stores bytes of scancode

  // Subscribe to keyboard interrupts
  if(kbd_subscribe_int(&IRQ) != 0) {
    printf("Error (KBD subscribe)\n"); // Error subscribing
    return 1;
  }    

  // Main loop: continues until ESC_BREAKCODE is received
  while (scancode != ESC_BREAKCODE) {
    // Receive a message
    if (driver_receive(ANY, &msg, &ipc_status) != 0) {
      printf("Error (driver_receiver)"); // Error receiving message
      continue;
    }
    // Check if it's a notification and from hardware
    if (is_ipc_notify(ipc_status) && _ENDPOINT_P(msg.m_source) == HARDWARE) {
      // Check if the notification is for the subscribed interrupt
      if(msg.m_notify.interrupts & BIT(IRQ)) {
        kbc_ih(); // Handle keyboard interrupt
        isMakeCode = !(scancode & KBD_SCANCODE_BRK); // Determine make/break
        if (second) { // If second byte of scancode
          second = false; // Reset flag
          bytes[1] = scancode; // Store second byte
          kbd_print_scancode(isMakeCode, 2, bytes); // Print 2-byte scancode
        } else { // If first byte or single-byte scancode
          bytes[0] = scancode; // Store byte
          if (scancode == 0xE0) second = true; // Check for multi-byte
          else kbd_print_scancode(isMakeCode, 1, bytes); // Print scancode
        }
      }
    }
  }

  // Unsubscribe from keyboard interrupts
  if(kbd_unsubscribe_int() != 0) {
    printf("Error (KBD unsubscribe)\n"); // Error unsubscribing
    return 1;
  }

  // Print number of sys_inb calls (assuming 'count' tracks this)
  if (kbd_print_no_sysinb(count) != 0) {
    printf("Error (kbd_print_no_sysinb)"); // Error printing sys_inb call count
  }

  return 0; // Successfully processed keyboard input and exited
}


int(kbd_test_poll)() {
  bool isMakeCode;      // true -> make code; false -> break code
  bool second = false;  // true -> 2 bytes; false -> 1 byte
  uint8_t bytes[2];

  while (scancode != ESC_BREAKCODE) {
    if (read_output(&scancode) != 0) continue;
    if (scancode == 0x00) {
      continue;
    }
    isMakeCode = !(scancode & KBD_SCANCODE_BRK);
    if (second) {
            second = false;
            bytes[1] = scancode;
            kbd_print_scancode(isMakeCode, 2, bytes);
          }
          else {
            bytes[0] = scancode;
            if (scancode == 0xE0) second = true;
            else kbd_print_scancode(isMakeCode, 1, bytes);
          }
  }

  if (restoreKBD() != 0) return 1;
  kbd_print_no_sysinb(count);
  return 0;
}

int(kbd_test_timed_scan)(uint8_t n) {
  int ipc_status;       // Variable to store the status of IPC
  message message;      // Variable to store received message
  uint8_t IRQ;
  bool isMakeCode;      // true -> make code; false -> break code
  bool second = false;  // true -> 2 bytes; false -> 1 byte
  uint8_t bytes[2];
  uint8_t IRQ_timer;

  if(timer_subscribe_int(&IRQ_timer) != 0){
    printf("Error (Timmer subscribe)\n");
    return 1;
  }

  if(kbd_subscribe_int(&IRQ) != 0) {
    printf("Error (KBD subscribe)\n");
    return 1;
  }    

  while (scancode != ESC_BREAKCODE && (global_counter/60.0) < n) {
    if ( driver_receive(ANY, &message, &ipc_status) != 0 ) {
      printf("Error (driver_receiver)");
      continue;
    }
    // Check if the received message is a notification
    if (is_ipc_notify(ipc_status)) {
      // Check if the message is from hardware source
      if (_ENDPOINT_P(message.m_source) == HARDWARE) {
        if (message.m_notify.interrupts & IRQ_timer) {
            timer_int_handler();
        }
        // Check if the interrupt corresponding to irq_set occurred
        if(message.m_notify.interrupts & BIT(IRQ)) {
          kbc_ih();
          isMakeCode = !(scancode & KBD_SCANCODE_BRK);
          if (second) {
            second = false;
            bytes[1] = scancode;
            kbd_print_scancode(isMakeCode, 2, bytes);
          }
          else {
            bytes[0] = scancode;
            if (scancode == 0xE0) second = true;
            else kbd_print_scancode(isMakeCode, 1, bytes);
          }
          global_counter = 0;
        }
      }
    }
  }

  if(timer_unsubscribe_int() != 0) {
    printf("Error (timer unsubscribe)\n");
    return 1;
  }

  if(kbd_unsubscribe_int() != 0) {
    printf("Error (KBD unsubscribe)\n");
    return 1;
  }

  if (kbd_print_no_sysinb(count) != 0) {
    printf("Error (kbd_print_no_sysinb)");
  }

  return 0;
}

// Keyboard controller interrupt handler.
void (kbc_ih)() {
  // Attempt to read a scancode from the keyboard's output buffer.
  if (read_output(&scancode) != 0) {
    printf("Error (read_output)"); // Print an error if reading the output buffer fails.
  }
}
