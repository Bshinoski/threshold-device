#ifndef BLUETOOTH_H
#define BLUETOOTH_H

void init_gpio_uart_pins(void);
void init_clock(void);
void init_uart(void);
void uart_send_char(char c);
void uart_send_string(const char *str);
void init_bluetooth(void);

#endif
