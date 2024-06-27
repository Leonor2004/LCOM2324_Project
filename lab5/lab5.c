// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>

#include <lcom/lab5.h>

#include <stdint.h>
#include <stdio.h>

#include "vg.h"
#include "kbd.h"
#include "i8042.h"

// Any header files included below this line should have been created by you

uint8_t scancode;

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/lab5/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab5/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int(video_test_init)(uint16_t mode, uint8_t delay) {
  if(enter_video_mode(mode) != 0) return 1;
  sleep(delay);
  if (vg_exit() != 0) return 1;
  return 0;
}

int(video_test_rectangle)(uint16_t mode, uint16_t x, uint16_t y,
                          uint16_t width, uint16_t height, uint32_t color) {

  if(_vg_init(mode) != 0) return 1;

  vg_draw_rectangle(x, y, width, height, (color & (BIT(bitsPerPixel()) - 1)));

  if (isESC() != 0) return 1;

  if (vg_exit() != 0) return 1;
  
  return 0;
}

int(video_test_pattern)(uint16_t mode, uint8_t no_rectangles, uint32_t first, uint8_t step) {
  if(_vg_init(mode) != 0) return 1;
  
  if (mode == 0x105) vg_draw_pattern(true, no_rectangles, first, step);
  else vg_draw_pattern(false, no_rectangles, first, step);

  if (isESC() != 0) return 1;

  if (vg_exit() != 0) return 1;

  return 0;
}

int(isESC)() {
  int ipc_status;       // Status of IPC
  message msg;          // Stores received message
  uint8_t IRQ;          // IRQ line number
  bool isMakeCode;      // Flag for make (true) or break (false) code
  bool second = false;  // Flag for second byte of scancode
  uint8_t bytes[2];     // Stores bytes of scancode

  if(kbd_subscribe_int(&IRQ) != 0) return 1; 

  while (scancode != ESC_BREAKCODE) {
    if (driver_receive(ANY, &msg, &ipc_status) != 0) {
      continue;
    }
    if (is_ipc_notify(ipc_status) && _ENDPOINT_P(msg.m_source) == HARDWARE) {
      if(msg.m_notify.interrupts & BIT(IRQ)) {
        if (read_output(&scancode) != 0) {
          printf("Error (read_output)");
        }
        isMakeCode = !(scancode & KBD_SCANCODE_BRK);
        if (second) {
          second = false;
          bytes[1] = scancode;
        } 
        else {
          bytes[0] = scancode;
          if (scancode == 0xE0) second = true;
        }
      }
    }
  }

  if(kbd_unsubscribe_int() != 0) return 1;
  return 0;
}

int(video_test_xpm)(xpm_map_t xpm, uint16_t x, uint16_t y) {
  /* To be completed */
  printf("%s(%8p, %u, %u): under construction\n", __func__, xpm, x, y);

  return 1;
}

int(video_test_move)(xpm_map_t xpm, uint16_t xi, uint16_t yi, uint16_t xf, uint16_t yf,
                     int16_t speed, uint8_t fr_rate) {
  /* To be completed */
  printf("%s(%8p, %u, %u, %u, %u, %d, %u): under construction\n",
         __func__, xpm, xi, yi, xf, yf, speed, fr_rate);

  return 1;
}

int(video_test_controller)() {
  /* To be completed */
  printf("%s(): under construction\n", __func__);

  return 1;
}
