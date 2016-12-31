#include "main.h"

// Function Purpose: Configure I2C module
void i2c_init(void)
{

    //Work around
    //
    //Before configuring the module for I2C operation:
    //
    //Configure the SCL and SDA pins as outputs by clearing their corresponding TRIS bits.
    //Force SCL and SDA low by clearing the corresponding LAT bits.
    //While keeping the LAT bits clear, configure SCL and SDA as inputs by setting their TRIS bits.
    //
    //Once this is done, use the SSPCON1 and SSPCON2 registers to configure the proper I2C mode as before.
//    SDA_DIR = 0;		// Make SDA and
//    SCK_DIR = 0;		// SCK pins input
//
//    SCK = 0;
//    SDA = 0;
//
    SDA_DIR = 1;		// Make SDA and
    SCK_DIR = 1;		// SCK pins input

    //SSPADD  = ((_XTAL_FREQ/4)/I2C_SPEED) - 1;
    SSPADD  = 0xF0; //39dec
    //SSP1ADD = 0x27;

    // BF RCinprocess_TXcomplete; UA dontupdate; SMP endsample_disable; P stopbit_notdetected; S startbit_notdetected; R_nW write_noTX; CKE tx_on_idle_to_active; D_nA lastbyte_address;
    //SSP1STAT = 0x80;
    SSP1STATbits.SMP = 1;   //1 = Slew rate control disabled for standard speed mode (100 kHz and 1 MHz)
    SSP1STATbits.CKE = 1;   //1 = Enable input logic so that thresholds are compliant with SMbus specification


    // SSPEN enabled; WCOL no_collision; SSPOV no_overflow; CKP lo_hold; SSPM I2CMaster_FOSC/4_SSPxADD;
    SSP1CON1 = 0x24;
    SSP1CON1bits.SSPEN = 1;         //1 = Enables the serial port and configures the SDAx and SCLx pins as the source of the serial port pins(3)
    SSP1CON1bits.SSPM  = 0b1000;    //1000 = I2C Master mode, clock = FOSC / (4 * (SSPxADD+1))(4)

    // ACKSTAT received; RCEN disabled; RSEN disabled; ACKEN disabled; ACKDT acknowledge; SEN disabled; GCEN disabled; PEN disabled;
    //SSP1CON2 = 0x00;

    // BOEN disabled; AHEN disabled; SBCDE disabled; SDAHT 300nshold; ACKTIM ackseq; DHEN disabled; PCIE disabled; SCIE disabled;
    //SSP1CON3 = 0x08;
    SSP1CON3bits.SDAHT  = 1;    //1 = Minimum of 300 ns hold time on SDAx after the falling edge of SCLx
    SSP1CON3bits.AHEN   = 0;    //0 = Address holding is disabled
    SSP1CON3bits.DHEN   = 0;    //0 = Data holding is disabled

    // SSPBUF 0x0;
    SSP1BUF = 0x00;

    // SSPMSK 0x0;
    SSP1MSK = 0x00;

}


void i2c_start() {
    SSP1CON2bits.SEN = 1;
    while(SSP1CON2bits.SEN);
}

void i2c_restart() {
    SSP1CON2bits.RSEN = 1;
    while(SSP1CON2bits.RSEN);
}

void i2c_stop() {
    SSP1CON2bits.PEN = 1;
    while(SSP1CON2bits.PEN);
}

void i2c_waitIDLE() {
    while( SSP1STATbits.R_nW | SSP1CON2bits.SEN | SSP1CON2bits.RSEN |  SSP1CON2bits.PEN |  SSP1CON2bits.RCEN | SSP1CON2bits.ACKEN ) {};
    // | SSP1CON2bits.ACKEN
}

void i2c_sendACK() {
    SSP1CON2bits.ACKDT = 0;
    SSP1CON2bits.ACKEN = 1;
}

void i2c_sendNACK() {
    SSP1CON2bits.ACKDT = 1;
    SSP1CON2bits.ACKEN = 1;
}

unsigned char i2c_sendByte(unsigned char data) {
    SSP1BUF = data;
    i2c_waitIDLE();

    return (unsigned char)(!SSP1CON2bits.ACKSTAT);
}

unsigned char i2c_readByte() {
    SSP1CON2bits.RCEN = 1;
    while (SSP1CON2bits.RCEN == 1);
    return SSP1BUF;
}



/**
  Prototype:        uint8_t I2C1_ReadFrame(uint8_t i2c1SlaveAddress, uint8_t *i2c1ReadPointer, uint8_t i2c1FrameLength)
  Input:            i2c1SlaveAddress : Address of slave sending data.
                    *i2c1ReadPointer : Pointer to starting location in file register where data is written.
                    i2c1FrameLength : Number of bytes to receive.
  Output:           none
  Description:      This function is used to read from the I2C bus and store into the file register from
                    the starting location passed as an argument. This is a blocking function and will wait until
                    all the data is received.
  Usage:            I2C1_ReadFrame(i2c1SlaveAddress, (char *)i2c1ReadPointer, i2c1FrameLength);
*/
uint8_t I2C_ReadFrame(uint8_t i2c1SlaveAddress, uint8_t *i2c1ReadPointer, uint8_t i2c1FrameLength)
{
    if(SSP1STATbits.S)
    {
        return I2C1_BUS_BUSY;
    }

    // initiate start condition
    SSP1CON2bits.SEN = 1;
    while (SSP1CON2bits.SEN)
    {
    }

    //check for bus collision
    if(PIR2bits.BCL1IF)
    {
        PIR2bits.BCL1IF = 0;
        return I2C1_BUS_COLLISION;
    }

    // send slave address with Read/Write bit set
    SSP1BUF = i2c1SlaveAddress | 0x01;
    while ((SSP1STATbits.BF || SSP1STATbits.R_nW) && !PIR2bits.BCL1IF);

    //check for bus collision
    if(PIR2bits.BCL1IF)
    {
        PIR2bits.BCL1IF = 0;
        return I2C1_BUS_COLLISION;
    }

    // check for acknowledgement status
    if (SSP1CON2bits.ACKSTAT)
    {
        SSP1CON2bits.PEN = 1;
        while(SSP1CON2bits.PEN)
		{
		}
        return I2C1_ACK_NOT_RECEIVED;
    }


    while (i2c1FrameLength)
    {
        // receive byte of data
        SSP1CON2bits.RCEN = 1;
        while(SSP1CON2bits.RCEN)
	{
	}
        *i2c1ReadPointer++ = SSP1BUF;

        // set acknowledgement status
        if(i2c1FrameLength == 1)
        {
            SSP1CON2bits.ACKDT = 1;
        }
        else
        {
            SSP1CON2bits.ACKDT = 0;
        }
        // send acknowledgement
        SSP1CON2bits.ACKEN = 1;
        while (SSP1CON2bits.ACKEN)
	{
	}
        i2c1FrameLength--;

        //check for bus collision
        if (PIR2bits.BCL1IF)
        {
            PIR2bits.BCL1IF = 0;
            return I2C1_BUS_COLLISION;
        }

    }
    // initiate stop condition
    SSP1CON2bits.PEN = 1;
    while (SSP1CON2bits.PEN)
    {
    }

    //check for bus collision
    if (PIR2bits.BCL1IF)
    {
        PIR2bits.BCL1IF=0;
        return I2C1_BUS_COLLISION;
    }
    return I2C1_SUCCESS;
}

/**
  Prototype:        uint8_t I2C1_WriteFrame(uint8_t i2c1SlaveAddress, uint8_t *i2c1WritePointer, uint8_t i2c1FrameLength)
  Input:            i2c1SlaveAddress : Address of slave receiving data.
		    *i2c1WritePointer : Pointer to starting location in file register from where data is read.
                    i2c1FrameLength : Number of bytes to send.
  Output:           none
  Description:      This function is used to write into the I2C bus. This is a blocking function and will wait until
                    all the data is send.
  Usage:            I2C1_WriteFrame(i2c1SlaveAddress, (char *)i2c1WritePointer, i2c1FrameLength);
*/
uint8_t I2C_WriteFrame(uint8_t i2c1SlaveAddress, uint8_t *i2c1WritePointer, uint8_t i2c1FrameLength)
{
   if (SSP1STATbits.S)
   {
      return I2C1_BUS_BUSY;
   }

    // initiate start condition
    SSP1CON2bits.SEN = 1;
    while (SSP1CON2bits.SEN)
    {
    }

    //check for bus collision
    if (PIR2bits.BCL1IF)
    {
        PIR2bits.BCL1IF = 0;
        return I2C1_BUS_COLLISION;
    }

    //write address into the buffer
    SSP1BUF = i2c1SlaveAddress;
    while (SSP1STATbits.BF || SSP1STATbits.R_nW)
    {
    }

    //Check for acknowledgement status
    if (SSP1CON2bits.ACKSTAT )
    {
        SSP1CON2bits.PEN = 1;
        while (SSP1CON2bits.PEN)
	{
	}
        return I2C1_ACK_NOT_RECEIVED;
    }

    while (i2c1FrameLength)
    {
        //write byte into the buffer
        SSP1BUF = *i2c1WritePointer++;
        while (SSP1STATbits.BF || SSP1STATbits.R_nW)
	{
	}

        //Check for acknowledgement status
        if ( SSP1CON2bits.ACKSTAT )
        {
            SSP1CON2bits.PEN = 1;
            while (SSP1CON2bits.PEN)
            {
            }
            return I2C1_ACK_NOT_RECEIVED;
        }

        // check for bus collision
        if (PIR2bits.BCL1IF)
        {
            PIR2bits.BCL1IF = 0;
            return I2C1_BUS_COLLISION;
        }
        i2c1FrameLength-- ;
    }

    //initiate stop condition
    SSP1CON2bits.PEN = 1;
    while (SSP1CON2bits.PEN)
    {
    }

    //check for bus collision
    if(PIR2bits.BCL1IF)
    {
        PIR2bits.BCL1IF = 0;
        return I2C1_BUS_COLLISION;
    }
    return I2C1_SUCCESS;
}

/**
  Prototype:        uint8_t I2C1_WriteByte(uint8_t i2c1SlaveAddress, uint8_t i2c1Data)
  Input:            i2c1SlaveAddress : Address of slave receiving data.
		    i2c1Data : data to be send.
  Output:           none
  Description:      This function is used to write into the I2C bus. This is a blocking function and will wait until
                    the data byte is send.
  Usage:            I2C1_WriteByte(i2c1SlaveAddress, i2c1Data);
*/
uint8_t I2C_WriteByte(uint8_t i2c1SlaveAddress, uint8_t i2c1Data)
{
   if (SSP1STATbits.S)
   {
       return I2C1_BUS_BUSY;
   }

    // initiate start condition
    SSP1CON2bits.SEN = 1;
    while (SSP1CON2bits.SEN)
    {
    }

    //check for bus collision
    if (PIR2bits.BCL1IF)
    {
        PIR2bits.BCL1IF = 0;
        return I2C1_BUS_COLLISION;
    }

    //write address into the buffer
    SSP1BUF = i2c1SlaveAddress;
    while (SSP1STATbits.BF || SSP1STATbits.R_nW)
    {
    }

    //Check for acknowledgement status
    if (SSP1CON2bits.ACKSTAT )
    {
        SSP1CON2bits.PEN = 1;
        while (SSP1CON2bits.PEN)
	{
	}
        return I2C1_ACK_NOT_RECEIVED;
    }

    //write byte into the buffer
    SSP1BUF = i2c1Data;
    while (SSP1STATbits.BF || SSP1STATbits.R_nW)
    {
    }

    //Check for acknowledgement status
    if ( SSP1CON2bits.ACKSTAT )
    {
        SSP1CON2bits.PEN = 1;
        while (SSP1CON2bits.PEN)
	{
	}
        return I2C1_ACK_NOT_RECEIVED;
    }

    // check for bus collision
    if (PIR2bits.BCL1IF)
    {
        PIR2bits.BCL1IF = 0;
        return I2C1_BUS_COLLISION;
    }

    //initiate stop condition
    SSP1CON2bits.PEN = 1;
    while (SSP1CON2bits.PEN)
    {
    }

    //check for bus collision
    if(PIR2bits.BCL1IF)
    {
        PIR2bits.BCL1IF = 0;
        return I2C1_BUS_COLLISION;
    }
    return I2C1_SUCCESS;
}