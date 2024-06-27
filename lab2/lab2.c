#include <lcom/lcf.h>
#include <lcom/lab2.h>

#include <stdbool.h>
#include <stdint.h>

extern int global_counter;
  
int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/lab2/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab2/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

// This function tests reading and displaying the configuration of a timer specified by 'timer'.
// It reads the configuration byte of the specified timer, then displays the configuration
// based on the requested field 'field'.
int(timer_test_read_config)(uint8_t timer, enum timer_status_field field) {

  uint8_t config;                            // Declare a variable 'config' to store the configuration byte
  // Call 'timer_get_conf' function to read the configuration of the specified timer
  if (timer_get_conf(timer, &config) != 0) return 1;            
                                             // and store it in the variable 'config'
  // Call 'timer_display_conf' function to display the configuration based on the requested field
  if (timer_display_conf(timer,config,field) != 0) return 1;  
  return 0;
}

// This function tests the time base by setting the frequency of a timer specified by 'timer' to the value 'freq'.
int(timer_test_time_base)(uint8_t timer, uint32_t freq) {
    
    // Set the frequency of the specified timer to the desired frequency 'freq'
    if (timer_set_frequency(timer, freq) != 0) return 1;
    return 0;                                // Return 0 to indicate successful completion of testing the time base
}


int(timer_test_int)(uint8_t time) {
  int ipc_status;   // Variable to store the status of IPC
  message msg;      // Variable to store received message
  uint8_t irq_set;  // Variable to store the IRQ set for the timer interrupt

  // Subscribe to the timer interrupt
  if(timer_subscribe_int(&irq_set) != 0){
    printf("error subscribing\n");
    return 1;
  }

  // Loop until the specified time is reached (specified by the run command)
  while(global_counter < time * 60){

    // Receive messages from the driver
    if(driver_receive(ANY, &msg, &ipc_status) != 0){
      printf("error receiving the message\n");
      return 1;
    }

    // Check if the received message is a notification
    if (is_ipc_notify(ipc_status)) {
      // Check if the message is from hardware source
      if (_ENDPOINT_P(msg.m_source) == HARDWARE) {
        // Check if the interrupt corresponding to irq_set occurred
        if (msg.m_notify.interrupts & irq_set) {
          // Handle the timer interrupt (increment the global counter)
          timer_int_handler();
          // Print elapsed time every second
          if (global_counter % 60 == 0) {
            timer_print_elapsed_time();
          }
        }
      }
    }
  }

  // Unsubscribe from the timer interrupt
  if(timer_unsubscribe_int() != 0){
    printf("error unsubscribing\n");
    return 1;
  }

  return 0;
}
