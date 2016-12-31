#ifndef PWM_H
#define	PWM_H

#define PWM_MAX_VALUE   623

void TMR2_Initialize(void);
void TMR2_StartTimer(void);
void TMR2_StopTimer(void);
uint8_t TMR2_ReadTimer(void);
void TMR2_WriteTimer(uint8_t timerVal);
void TMR2_LoadPeriodRegister(uint8_t periodVal);
void EPWM_Initialize (void);
void EPWM_LoadDutyValue(uint16_t dutyValue);


#endif	/* PWM_H */

