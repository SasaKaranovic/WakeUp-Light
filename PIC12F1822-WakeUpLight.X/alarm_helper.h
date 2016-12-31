#ifndef ALARM_HELPER_H
#define	ALARM_HELPER_H


#define RTC_BCD_TO_DEC(x) ((uint8_t) ((x>>4)*10) + (x&0x0f))


void RTC_FormatTime(uint8_t *raw, uint8_t *result);

#endif	/* ALARM_HELPER_H */

