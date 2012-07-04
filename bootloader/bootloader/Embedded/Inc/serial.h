#ifndef SERIAL_H
#define SERIAL_H
//-----------------------------------------------------------------------------------------
extern void sendchar( char *ch );
extern int serial_kbhit(void);
extern int serial_getchar(void);
extern int UART_Speed(int speed, int *divider);
void init_serial(unsigned int port, unsigned int baudRate);

//-----------------------------------------------------------------------------------------
#endif