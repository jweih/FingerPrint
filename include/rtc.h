#ifndef _RTC_H
#define _RTC_H

#include"common.h"

/*unsigned char*/
struct rtc_time
{
	char second;
	char minute;
	char hour;
	char week;
	char day;
	char month;
	char year;
};


struct rtc_ram
{
	unsigned int addres;
	unsigned char data;

};


struct rtc_fun
{
	int (*init)(void);
	void * (*service)(void *args);
};


struct rtc_fun * get_rtcdevice();


#endif /*_RTC_H*/