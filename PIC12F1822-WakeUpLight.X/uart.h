#ifndef UART_H
#define	UART_H


void UART_SetUp();
void UART_Write(uint8_t txData);
char getch(void);
void putch(char txData);


#endif	/* UART_H */

