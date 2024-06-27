#include <lcom/lcf.h>
#include <stdint.h>


#include "i8254.h"

int timer_hook_id = 0;
int global_counter = 0;


// This function sets the frequency of a timer specified by 'timer' to the value 'freq'.
// It calculates the appropriate values for the timer to generate the desired frequency
// and writes them to the timer's control registers and data ports.
int (timer_set_frequency)(uint8_t timer, uint32_t freq) {

    if (freq > TIMER_FREQ || freq < 19 || timer < 0 || timer > 2) // Check if the frequency is within the acceptable range and if the timer number is valid
        return 1; 

    uint8_t cw;                                      // Control word to be used for timer configuration

    
    if (timer_get_conf(timer, &cw) != 0)             // Read the current configuration of the timer and return error if reading configuration fails
        return 1; 

    // Set the read-back command and control word to configure the timer to read LSB followed by MSB
    cw = (cw & 0xF) | TIMER_LSB_MSB;                 // Preserve 4 least significant bits (which can change the timer operation mode)

    // Calculate the initial value of the timer counter to generate a square wave with the desired frequency.
    uint32_t squareWaveFreq = TIMER_FREQ / freq;
    

    // Obtain the least significant byte (LSB) and most significant byte (MSB) of 'squareWaveFreq'.
    uint8_t lsb, msb;
    if (util_get_LSB(squareWaveFreq, &lsb) != 0) return 1;
    if (util_get_MSB(squareWaveFreq, &msb) != 0) return 1;

    uint8_t port;
    // Update the control word based on the type of timer
    switch (timer) {
        case 0:
            cw = cw | TIMER_SEL0;
            port = TIMER_0; // 0x40
            break;
        case 1:
            cw = cw | TIMER_SEL1;
            port = TIMER_1; // 0x41
            break;
        case 2:
            cw = cw | TIMER_SEL2;
            port = TIMER_2; // 0x42
            break;
        default:
            return 1; // Return error for invalid timer number
    }

    // Write the updated control word to the timer control register
    if (sys_outb(TIMER_CTRL, cw) != 0)
        return 1; // Return error if writing control word fails

    // Write the initial timer value (LSB followed by MSB) to the respective port
    if (sys_outb(port, lsb) != 0)
        return 1;
    if (sys_outb(port, msb) != 0)
        return 1;

    // Return 0 to indicate successful configuration
    return 0;
}


// Subscribes to timer interrupts, setting a bit number for the interrupt line.
int (timer_subscribe_int)(uint8_t *bit_no) {
  if (bit_no == NULL){ // Check for NULL pointer to avoid dereferencing it.
    printf("bit_no is NULL\n"); // Error message for NULL pointer.
    return 1; // Return error code.
  }

  *bit_no = BIT(timer_hook_id); // Assign interrupt line bit to *bit_no using BIT macro.

  // Attempt to set interrupt policy. Return error if fails.
  if(sys_irqsetpolicy(TIMER0_IRQ, IRQ_REENABLE, &timer_hook_id) != 0){
    printf("error setting the policy\n"); // Error message for setting policy failure.
    return 1; // Return error code.
  }
  return 0; // Success.
}

// Unsubscribes from timer interrupts, removing the interrupt policy.
int (timer_unsubscribe_int)() {
  // Attempt to remove interrupt policy. Return error if fails.
  if(sys_irqrmpolicy(&timer_hook_id) != 0){
    printf("error removing the policy\n"); // Error message for removing policy failure.
    return 1; // Return error code.
  }
  return 0; // Success.
}

// Handles timer interrupts by incrementing a global counter.
void (timer_int_handler)() {
  global_counter++; // Increment global counter on each timer interrupt.
}



// This function is intended to get the configuration of a timer specified by 'timer'.
// It reads the configuration byte of the specified timer and stores it in the memory
// location pointed to by the pointer 'st'.
int (timer_get_conf)(uint8_t timer, uint8_t *st) {

  uint8_t cw = TIMER_RB_CMD|TIMER_RB_SEL(timer)|TIMER_RB_COUNT_;    // Create a control word 'cw' to be written to the control register.
                                                                    // The control word combines the read-back command, the timer selection bits,
                                                                    // and the read/write mode (count or status) for the specified timer.

  sys_outb(TIMER_CTRL,cw);                                          // Write the control word 'cw' to the control register.

  if (util_sys_inb(timer + 0x40, st) != 0) return 1;                // Call the function util_sys_inb() to read a byte from the timer's register,
                                                                    // located at the base address of the timer plus 0x40 (to select the timer's status register).
                                                                    // The read byte is stored in the memory location pointed to by 'st'.
  return 0;
}

// This function is intended to display the configuration of a timer
// specified by 'timer', based on the requested field 'field'.
// 'st' represents the status byte of the timer.
int (timer_display_conf)(uint8_t timer, uint8_t st, enum timer_status_field field) {

  union timer_status_field_val conf;       // Declare a union 'conf' of type 'timer_status_field_val'

  // If all fields are requested, store the entire status byte 'st' into the 'byte' field of 'conf'
  if (field == tsf_all) conf.byte = st;   

  // If the requested field is 'tsf_initial', extract the initialization mode bits from 'st' and store them in 'in_mode' field of 'conf'
  if (field == tsf_initial) conf.in_mode = (st & TIMER_LSB_MSB) >> 4;

  // If the requested field is 'tsf_mode', extract the counting mode bits from 'st' and store them in 'count_mode' field of 'conf'
  if (field == tsf_mode) conf.count_mode = (st & (BIT(3)|BIT(2)|BIT(1))) >> 1;

  // If the requested field is 'tsf_base', extract the BCD mode bit from 'st' and store it in 'bcd' field of 'conf'
  if (field == tsf_base) conf.bcd = st & TIMER_BCD;
  
  if (timer_print_config(timer, field, conf) != 0) return 1;  // Call 'timer_print_config' function to display the configuration

  return 0;
}
