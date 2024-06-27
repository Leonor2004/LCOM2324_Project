// definition of macros for the i8042
// valores tirados do ppt da aula teórica e do 8042 functional description from the IBM Technical Reference Manual

#ifndef _LCOM_I8042_H_
#define _LCOM_I8042_H_

// adresses
#define IN_BUF_COMMANDS 0x64
#define IN_BUF_ARGUMENTS 0x60
#define OUT_BUF 0x60
#define ESC_BREAKCODE 0x81
#define READ_COMMAND_BACK 0x20
#define WRITE_COMAND_BACK 0x60
#define ST_REG 0x64

// status-register bit definitions
#define OBF BIT(0)
#define IBF BIT(1)
#define AUX BIT(5)
#define TIMEOUT_ERROR BIT(6)
#define PARITY_ERROR BIT(7)

#define KEYBOARD_IRQ 1
#define KBD_SCANCODE_BRK BIT(7)

#endif /* _LCOM_I8042_H */
