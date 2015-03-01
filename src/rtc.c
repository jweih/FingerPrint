#include"rtc.h"

#undef  DBG_ON
#undef  FILE_NAME
#define DBG_ON  	(0x01)
#define FILE_NAME 	"rtc:"



#define RTC_IOC_MAGIC 		'm'
#define RTC_CONFIG_TIME 	_IOW(RTC_IOC_MAGIC, 11, struct rtc_time)

#define RTC_WRITE_RAM 		_IOWR(RTC_IOC_MAGIC, 12, struct rtc_ram)
#define RTC_READ_RAM 		_IOWR(RTC_IOC_MAGIC, 13, struct rtc_ram)




static int rtc_fd;
struct rtc_time time_value={0x03,0x03,0x0B,0x03,0x03,0x03,0x03,};



/***************************************************************************
*功能描述	:	设置时间
*参数time	:	时间结构体
*返回值		:	0==ok  others==wrong
*--------------------------------------------------------------------------
*2015/03/01		jweih 			创建
****************************************************************************/
int rtc_configtime(struct rtc_time * time)
{
	/*待增加时间界限检测*/
	if(NULL == time)
	{
		dbg_printf("time is null \n");
		return(-1);
	}
	ioctl(rtc_fd,RTC_CONFIG_TIME, time);

	return(0);
}




/***************************************************************************
*功能描述	:	初始化rtc设备	
*返回值		:	0==ok  others==wrong
*--------------------------------------------------------------------------
*2015/03/01		jweih 			创建
****************************************************************************/
int  rtc_init(void)
{

	rtc_fd = open("/dev/rtc", O_RDWR);
	if(rtc_fd<0)
	{
		dbg_printf("open rtc device fail\n");
		return(1);
	}
//	memset((char*)time,0,sizeof(time));
	rtc_configtime(&time_value);
	
	return(0);
}



/***************************************************************************
*功能描述	:	bcd码转十进制	
*参数val		:	待转换的bcd码
*返回值		:	转换后的十进制形式的bcd码
*--------------------------------------------------------------------------
*2015/03/01		jweih 			创建
****************************************************************************/

unsigned int rtc_bcd2hex(unsigned char val)
{
    unsigned char i;
    i= val&0x0f;
    val >>= 4;
    val &= 0x0f;
    val *= 10;
    i += val;
    
    return i;
}



/***************************************************************************
*功能描述	:	读取rtc时间值	
*参数fun	:	字体操作接口
*返回值		:	void
*--------------------------------------------------------------------------
*2015/03/01		jweih 			创建
****************************************************************************/
void rtc_handler(struct font_fun * fun)
{

	wchar_t str_time[6];

	unsigned int hour = 0;
	unsigned int minute = 0;
	read(rtc_fd,&time_value, sizeof(time_value));
	
	hour = rtc_bcd2hex(time_value.hour);
	minute = rtc_bcd2hex(time_value.minute);

	if(hour<10)
	{
		sprintf((char*)&str_time[0], "%d" , 0);
		sprintf((char*)&str_time[1], "%d" , hour);
	}
	else
	{
		sprintf((char*)&str_time[0], "%d" , hour/10);
		sprintf((char*)&str_time[1], "%d" , hour%10);
	}
	str_time[2]=' ';
	str_time[3]=':';
	if(minute<10)
	{
		sprintf((char*)&str_time[4], "%d" , 0);
		sprintf((char*)&str_time[5], "%d" , minute);
	}
	else
	{
		sprintf((char*)&str_time[4], "%d" , minute/10);
		sprintf((char*)&str_time[5], "%d" , minute%10);
	}

	
	fun->show(fun->lcd,50,180,(wchar_t *)str_time,10);

#if 0
	printf("time.second = %d\n",rtc_bcd2hex(time_value.second));
	printf("time.minute= %d\n",rtc_bcd2hex(time_value.minute));
	printf("time.hour = %d\n",rtc_bcd2hex(time_value.hour));
	printf("time.day = %d\n",rtc_bcd2hex(time_value.day));
	printf("time.week = %d\n",rtc_bcd2hex(time_value.week));
	printf("time.year = %d\n",rtc_bcd2hex(time_value.year));
	printf("time.month = %d\n",rtc_bcd2hex(time_value.month));
#endif
	sleep(30);


}


/***************************************************************************
*功能描述	:	rtc服务函数
*参数args	:	线程传递的参数
*返回值		:	(void*)0	
*--------------------------------------------------------------------------
*2015/03/01		jweih 			创建
****************************************************************************/

void * rtc_service(void *args)
{
		struct font_fun * fun = (struct font_fun *)args;
		while(1)
		{
			rtc_handler(fun);
		}

        return (void *)0;
}



struct rtc_fun rtc_device_fun ={
	.init 		= rtc_init,
	.service 	= rtc_service,
};


struct rtc_fun * get_rtcdevice()
{
	return(&rtc_device_fun);
}









