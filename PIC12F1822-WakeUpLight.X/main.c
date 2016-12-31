/*
 __    __      _                         __ _       _     _   
/ / /\ \ \__ _| | _____  /\ /\ _ __     / /(_) __ _| |__ | |_ 
\ \/  \/ / _` | |/ / _ \/ / \ \ '_ \   / / | |/ _` | '_ \| __|
 \  /\  / (_| |   <  __/\ \_/ / |_) | / /__| | (_| | | | | |_ 
  \/  \/ \__,_|_|\_\___| \___/| .__/  \____/_|\__, |_| |_|\__|
                              |_|             |___/           

http://sasakaranovic.com/projects/diy-wake-up-light-wake-up-easily-and-well-rested/
https://github.com/coldkeyboard/WakeUp-Light
Version 1.0
License: GPL-3.0

*/
#include "main.h"


volatile uint16_t PWM_Duty = 0;

void main(void) 
{
    SystemInit();
    __delay_ms(2000);
    
    printf("System init complete\r\n");
    uint8_t TimeStamp[7] = {0};
    uint8_t buff[10] = {0};
    uint8_t result = 0;

    
    I2C_WriteByte(I2C_DEVICE_ADDRESS, 0);
    result = I2C_ReadFrame(I2C_DEVICE_ADDRESS, buff, 7);
    
 
    while(1)
    {
        I2C_WriteByte(I2C_DEVICE_ADDRESS, 0);
        result = I2C_ReadFrame(I2C_DEVICE_ADDRESS, buff, 7);
        RTC_FormatTime(buff, TimeStamp);

		//printf is very greedy, but we have time and resources in this case
        printf("%2d %2d %2d %2d %2d %2d %2d \r\n", TimeStamp[0], TimeStamp[1], TimeStamp[2], TimeStamp[3], TimeStamp[4], TimeStamp[5], TimeStamp[6]);

		
		//Check if current time is 07:00 AM
		//I am ignoring date/day of the week because I want it to run every day at 7AM.
        if(TimeStamp[2] == 7 && TimeStamp[1] == 0) 
		{
            while(PWM_Duty < PWM_MAX_VALUE) 
			{
                EPWM_LoadDutyValue(PWM_Duty);
                PWM_Duty++;
                __delay_ms(4330);	// ~ 45 (min) * 60 (sec) / 623 (pwm max)
            }
            
			//When we reach PWM_MAX_VALUE, hang on with maximum brightnes until user presses "stop" button
			//which will actually reset the micro
            while(1);
        }
    
		//Instead we could go to low power mode but since this device is running from wall adapter
		//power consumption is not critical part, so just wait 2sec and repeat
        __delay_ms(2000);
    }//while
    
    
}//main


void SystemInit() 
{
    OSCCON = 0x70;
    OSCTUNE = 0x00;   
    ANSELA = 0x00;
    WPUA = 0x39;
    OPTION_REGbits.nWPUEN = 1;

    TRISAbits.TRISA0    = 0;
    LATAbits.LATA0      = 0;
    
    TRISAbits.TRISA1    = 1;
    TRISAbits.TRISA2    = 1;
    
    APFCON = 0xC4;
    
    UART_SetUp();
    EPWM_Initialize();
    TMR2_Initialize();
    i2c_init();
    
}
