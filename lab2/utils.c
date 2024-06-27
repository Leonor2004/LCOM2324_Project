#include <lcom/lcf.h>

#include <stdint.h>

// This function is intended to retrieve the least significant byte (LSB)
// of a 16-bit unsigned integer 'val' and store it in the memory location
// pointed to by the pointer 'lsb'.
int(util_get_LSB)(uint16_t val, uint8_t *lsb) {

    if (lsb == NULL) return 1;

    *lsb = 0xff;  // Set the value pointed to by 'lsb' to 0xff (possibly as a default or initialization) 0x11111111
    *lsb = *lsb & val;  // Retrieve the least significant byte of 'val' by performing a bitwise AND operation
                        // between the byte pointed to by 'lsb' and 'val'. Store the result in the memory
                        // location pointed to by 'lsb'.
    return 0;
}

// This function is intended to retrieve the most significant byte (MSB)
// of a 16-bit unsigned integer 'val' and store it in the memory location
// pointed to by the pointer 'msb'.
int(util_get_MSB)(uint16_t val, uint8_t *msb) {

    if (msb == NULL) return 1;

    *msb = (uint8_t)(val >> 8); // Shift the bits of 'val' 8 positions to the right to get the most significant byte,
                                // then cast the result to an unsigned 8-bit integer and store it in the memory
                                // location pointed to by 'msb'.
    return 0;
}

// This function is intended to read a byte from an I/O port specified by 'port'
// and store the 8lsb of the result in the memory location pointed to by the pointer 'value'.
int (util_sys_inb)(int port, uint8_t *value) {

  if (value == NULL) return 1;

  uint32_t aux;            // Declare an unsigned 32-bit integer 'aux' to temporarily store the read value
  sys_inb(port, &aux);     // Call the system function sys_inb() to read a byte from the specified I/O port 'port'.
                           // Store the result in the memory location pointed to by 'aux'.

  *value = aux & 0xff;     // Extract the least significant byte of the value stored in 'aux' by performing
                           // a bitwise AND operation with 0xff. Store the result in the memory location
                           // pointed to by 'value'.
  return 0;
}
