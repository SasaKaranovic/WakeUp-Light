#include "main.h"


void RTC_FormatTime(uint8_t *raw, uint8_t *result)
{
    //Seconds
//    result[0] = (raw[0] >> 4) * 10;
//    result[0] += (raw[0] & 0x0f);
    result[0] = RTC_BCD_TO_DEC(raw[0]);
    
    //Minutes
//    result[1] = (raw[1] >> 4) * 10;
//    result[1] += (raw[1] & 0x0f);
    result[1] = RTC_BCD_TO_DEC(raw[1]);
    
    //Hours
//    result[2] = (raw[2] >> 4) * 10;
//    result[2] += (raw[2] & 0x0f);
    result[2] = RTC_BCD_TO_DEC(raw[2]);
    
    //Day
    result[3] = raw[3];
    
    //Date
//    result[4] = (raw[4] >> 4) * 10;
//    result[4] += (raw[4] & 0x0f);
    result[4] = RTC_BCD_TO_DEC(raw[4]);
    
    //Month
//    result[5] = (raw[5] >> 4) * 10;
//    result[5] += (raw[5] & 0x0f);
    result[5] = RTC_BCD_TO_DEC(raw[5]);
    
    //Year
//    result[6] = (raw[6] >> 4) * 10;
//    result[6] += (raw[6] & 0x0f);
    result[6] = RTC_BCD_TO_DEC(raw[6]);
    
}
