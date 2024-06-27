#ifndef _LCOM_KBD_H_
#define _LCOM_KBD_H_

int(kbd_subscribe_int)(uint8_t *IRQ);
int(kbd_unsubscribe_int)();
int (read_output)(uint8_t *output);
int (restoreKBD)();

#endif
