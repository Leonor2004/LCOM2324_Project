// definition of macros for the i8042
// valores tirados do ppt da aula teórica e do 8042 functional description from the IBM Technical Reference Manual

#ifndef _LCOM_I8042_H_
#define _LCOM_I8042_H_

// adresses
#define IN_BUF_COMMANDS             0x64
#define IN_BUF_ARGUMENTS            0x60
#define OUT_BUF                     0x60
#define ESC_BREAKCODE               0x81
#define READ_COMMAND_BACK           0x20
#define WRITE_COMAND_BACK           0x60
#define ST_REG                      0x64

// status-register bit definitions
#define OBF                         BIT(0)
#define IBF                         BIT(1)
#define AUX                         BIT(5)  // mouse data
#define TIMEOUT_ERROR               BIT(6)
#define PARITY_ERROR                BIT(7)

#define KEYBOARD_IRQ                1
#define MOUSE_IRQ                   12
#define KBD_SCANCODE_BRK            BIT(7)

#define MAX_ATTEMPS                 10

#define MOUSE_LB                    BIT(0)
#define MOUSE_RB                    BIT(1)
#define MOUSE_MB                    BIT(2)
#define FIRST_BYTE                  BIT(3)
#define MSB_X_DELTA                 BIT(4)
#define MSB_Y_DELTA                 BIT(5)
#define X_OVFL                      BIT(6)
#define Y_OVFL                      BIT(7)
#define DISABLE_CMD                 0xF5
#define ENABLE_CMD                  0xF4
#define SET_STREAM_MODE             0xEA
#define READ_DATA                   0xEB
#define WRITE_BYTE_TO_AUX_DEVICE    0xD4

#define ACK                         0xFA    
#define NACK                        0xFE
#define ERROR                       0xFC

#endif /* _LCOM_I8042_H */
