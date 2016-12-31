#include "main.h"


void UART_SetUp() {
    // Set the EUSART module to the options selected in the user interface.

    // ABDOVF no_overflow; SCKP Non-Inverted; BRG16 16bit_generator; WUE disabled; ABDEN disabled; 
    BAUDCON = 0x08;

    // SPEN enabled; RX9 8-bit; CREN disabled; ADDEN disabled; SREN disabled; 
    RCSTA = 0x80;

    // TX9 8-bit; TX9D 0; SENDB sync_break_complete; TXEN enabled; SYNC asynchronous; BRGH hi_speed; CSRC slave; 
    TXSTA = 0x24;

    // Baud Rate = 57600; SP1BRGL 34; 
    SPBRGL = 0x22;

    // Baud Rate = 57600; SP1BRGH 0; 
    SPBRGH = 0x00;
    
    //Setup UART pins
    TRISAbits.TRISA4    = 0;
    LATAbits.LATA4      = 0;
    
    APFCONbits.TXCKSEL  = 1;
    APFCONbits.RXDTSEL  = 1;
}


void UART_Write(uint8_t txData)
{
    while(0 == PIR1bits.TXIF)
    {
    }

    TXREG = txData;    // Write the data byte to the USART.
}

void putch(char txData)
{
    UART_Write(txData);
}