#ifndef _LCOM_MOUSE_H_
#define _LCOM_MOUSE_H_

void (mouse_ih)();
bool (mouse_output)();
int (mouse_subscribe_int)(uint8_t *irq_set);
int (mouse_unsubscribe_int)();
void (create_mpacket)();
int (mouse_data_report)(uint8_t command);
int (write_command)(uint8_t port, uint8_t cmd);

#endif
