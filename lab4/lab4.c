// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>
#include <lcom/timer.h>

#include <stdint.h>
#include <stdio.h>

// Any header files included below this line should have been created by you
#include "mouse.h"
#include "i8042.h"

extern struct packet mpacket;
extern int global_counter;

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need/ it]
  lcf_trace_calls("/home/lcom/labs/lab4/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab4/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}


int (mouse_test_packet)(uint32_t cnt) {
  int ipc_status;
  message msg;
  uint8_t irq_set_mouse;

  if (mouse_data_report(ENABLE_CMD) != 0) return 1;
  if (mouse_subscribe_int(&irq_set_mouse) != 0) return 1;

  while (cnt > 0) {
    if (driver_receive(ANY, &msg, &ipc_status) != 0){
      printf("Error");
      continue;
    }

    if (is_ipc_notify(ipc_status)){
      switch(_ENDPOINT_P(msg.m_source)){
        case HARDWARE: 
          if (msg.m_notify.interrupts & irq_set_mouse){
            mouse_ih();
            if (mouse_output()) {
              create_mpacket();
              mouse_print_packet(&mpacket);
              cnt--;
            }
          }
          break;
      }
    }
  }
  if (mouse_data_report(DISABLE_CMD) != 0) return 1;
  if (mouse_unsubscribe_int() != 0) return 1;
  return 0;
}

int (mouse_test_async)(uint8_t idle_time) {
  int ipc_status;
  message msg;
  uint8_t irq_set_mouse, irq_set_timer;
  uint16_t freq = sys_hz();
  uint8_t seconds = 0;

  if (mouse_data_report(ENABLE_CMD) != 0) return 1;
  if (mouse_subscribe_int(&irq_set_mouse) != 0) return 1;
  if (timer_subscribe_int(&irq_set_timer) != 0) return 1;

  while (seconds < idle_time) {
    if (driver_receive(ANY, &msg, &ipc_status) != 0){
      printf("Error");
      continue;
    }

    if (is_ipc_notify(ipc_status)){
      switch(_ENDPOINT_P(msg.m_source)){
        case HARDWARE: 
          if (msg.m_notify.interrupts & irq_set_mouse){
            mouse_ih();
            if (mouse_output()) {
              create_mpacket();
              mouse_print_packet(&mpacket);
              global_counter = 0;
              seconds = 0;
            }
          }
          if (msg.m_notify.interrupts & irq_set_timer){
            timer_int_handler();
            if (global_counter % freq == 0) seconds++;
          }
          break;
      }
    }
  }

  if (mouse_data_report(DISABLE_CMD) != 0) return 1;
  if (mouse_unsubscribe_int() != 0) return 1;
  if (timer_unsubscribe_int() != 0) return 1;

  return 0;
}

int (mouse_test_gesture)(uint8_t x_len, uint8_t tolerance) {
  /* int ipc_status;
  message msg;
  uint8_t irq_set_mouse;

  if (mouse_data_report(ENABLE_CMD) != 0) return 1;
  if (mouse_subscribe_int(&irq_set_mouse) != 0) return 1;

  while (true) {
    if (driver_receive(ANY, &msg, &ipc_status) != 0){
      printf("Error");
      continue;
    }

    if (is_ipc_notify(ipc_status)){
      switch(_ENDPOINT_P(msg.m_source)){
        case HARDWARE: 
          if (msg.m_notify.interrupts & irq_set_mouse){
            mouse_ih();
            if (mouse_output()) {
              create_mpacket();
              mouse_print_packet(&mpacket);
              // TODO
            }
          }
          break;
      }
    }
  }

  if (mouse_data_report(DISABLE_CMD) != 0) return 1;
  if (mouse_unsubscribe_int() != 0) return 1;
  return 0; */

  /* To be completed */
  printf("%s: under construction\n", __func__);
  return 1;
}

int (mouse_test_remote)(uint16_t period, uint8_t cnt) {
    /* This year you need not implement this. */
    printf("%s(%u, %u): under construction\n", __func__, period, cnt);
    return 1;
}
