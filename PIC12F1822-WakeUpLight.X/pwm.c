#include "main.h"

void TMR2_Initialize(void)
{
    // Set TMR2 to the options selected in the User Interface

    // T2CKPS 1:64; T2OUTPS 1:1; TMR2ON off; 
    T2CON = 0x03;

    // PR2 155; 
    PR2 = 0x9B;

    // TMR2 0; 
    TMR2 = 0x00;

    // Clearing IF flag.
    PIR1bits.TMR2IF = 0;

    // Start TMR2
    TMR2_StartTimer();
}

void TMR2_StartTimer(void)
{
    // Start the Timer by writing to TMRxON bit
    T2CONbits.TMR2ON = 1;
}

void TMR2_StopTimer(void)
{
    // Stop the Timer by writing to TMRxON bit
    T2CONbits.TMR2ON = 0;
}

uint8_t TMR2_ReadTimer(void)
{
    uint8_t readVal;

    readVal = TMR2;

    return readVal;
}

void TMR2_WriteTimer(uint8_t timerVal)
{
    // Write to the Timer2 register
    TMR2 = timerVal;
}

void TMR2_LoadPeriodRegister(uint8_t periodVal)
{
   PR2 = periodVal;
}

void EPWM_Initialize (void)
{
    // Set the PWM to the options selected in MPLAB(c) Code Configurator
    
    // CCP1M P1A: active high; P1B: active high; DC1B 3; P1M single; 
    CCP1CON = 0x3C;
    
    // CCP1ASE operating; PSS1BD low; PSS1AC low; CCP1AS disabled; 
    ECCP1AS = 0x00;
    
    // P1RSEN automatic_restart; P1DC 0; 
    PWM1CON = 0x80;
    
    // STR1B P1B_to_CCP1M; STR1A P1A_to_port; STR1SYNC start_at_begin; 
    PSTR1CON = 0x02;
    
    // CCPR1L 155; 
    CCPR1L = 0x9B;
    
    // CCPR1H 0; 
    CCPR1H = 0x00;
    
    EPWM_LoadDutyValue(0);
    
}

void EPWM_LoadDutyValue(uint16_t dutyValue)
{
   // Writing to 8 MSBs of pwm duty cycle in CCPRL register
    CCPR1L = ((dutyValue & 0x03FC)>>2);
    
   // Writing to 2 LSBs of pwm duty cycle in CCPCON register
    CCP1CON = (CCP1CON & 0xCF) | ((dutyValue & 0x0003)<<4);
}