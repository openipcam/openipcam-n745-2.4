/*
 * rtc710.h
 * PC34 Lsshi 2005-4-19 14:11
 */
 
#define INIRRESET	0x1357	//init value

#define AERPOWERON	0xA965 // RTC access enable
#define AERPOWEROFF	0x0000 // RTC access disable

//Mask Value
#define RTCSET		0x10000
#define AERRWENB	0x10000 // check if rtc r/w register enable

#define HR24		0x0001 // 24 hr
#define LEAPYEAR	0x0001
#define TICKENB		0x80

#define TICKINTENB	0x0002
#define ALARMINTENB	0x0001

#define RTC_SECONDS			(((_TLR>>4)&0x07)*10+(_TLR&0x0F))
#define RTC_MINUTES			(((_TLR>>12)&0x07)*10+((_TLR>>8)&0x0F))
#define RTC_HOURS			(((_TLR>>20)&0x07)*10+((_TLR>>16)&0x0F))

#define RTC_DAY_OF_MONTH	(((_CLR>>4)&0x03)*10+(_CLR&0x0F))
#define RTC_MONTH			(((_CLR>>12)&0x01)*10+((_CLR>>8)&0x0F))
#define RTC_YEAR			(((_CLR>>20)&0x0F)*10+((_CLR>>16)&0x0F))

//Alarm time
#define RTC_SECONDS_ALARM	(((_TAR>>4)&0x07)*10+(_TAR&0x0F))
#define RTC_MINUTES_ALARM	(((_TAR>>12)&0x07)*10+((_TAR>>8)&0x0F))
#define RTC_HOURS_ALARM		(((_TAR>>20)&0x07)*10+((_TAR>>16)&0x0F))

#define RTC_DAY_OF_MONTH_ALARM	(((_CAR>>4)&0x03)*10+(_CAR&0x0F))
#define RTC_MONTH_ALARM			(((_CAR>>12)&0x01)*10+((_CAR>>8)&0x0F))
#define RTC_YEAR_ALARM			(((_CAR>>20)&0x0F)*10+((_CAR>>16)&0x0F))


#define RTC_DAYOFWEEK			(_DWR&0x07)


#define RTC_TIME_SCALE	0x10  /* Time scale selection	*/
#define TIME24			1
#define TIME12			0



#define CMOS_READ(reg)		DWORD_READ(reg)
#define CMOS_WRITE(val,reg)	DWORD_WRITE(reg,val)

#ifndef BCD_TO_BIN
#define BCD_TO_BIN(val) ((val)=((val)&15) + ((val)>>4)*10)
#endif

#ifndef BIN_TO_BCD
#define BIN_TO_BCD(val) ((val)=(((val)/10)<<4) + (val)%10)
#endif
