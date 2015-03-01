#include"common.h"


#undef  DBG_ON
#undef  FILE_NAME
#define DBG_ON  	(0x01)
#define FILE_NAME 	"main:"


/***************************************************************************
*功能描述	:	main	
*--------------------------------------------------------------------------
*2015/02/28		jweih 			构建
*2015/02/29		jweih 			调整
*2015/03/01		jweih 			调整
****************************************************************************/

int main(void)
{

	unsigned int i =0;
	int ret = 0;
	debug_siginit();


	struct lcd_fun * lcd = get_lcddevice();
	ret = lcd->init();
	if(ret != 0 )
	{
		dbg_printf("lcd init fail\n");
		return(1);
	}

	
	struct font_fun * font = get_fontdevice(lcd);
	ret=font->init();
	if(ret != 0 )
	{
		dbg_printf("font init fail\n");
		return(2);
	}

	font->setcolor(lcd->rgb(33,45,222),lcd->rgb(0x00,0x00,0x00));
	font->setsize(64,64);
	

	threadpool_t *tpool;
	task_t	key_task,wiegand_task,rtc_task;
	struct thread_fun * thread= get_thread();
	ret = thread->init(&tpool);
	if(ret != 0 )
	{
		dbg_printf("font init fail\n");
		return(2);
	}
	

	struct key_fun * key=get_keydevice();
	ret = key->init();
	if(0x00==ret)
	{
		key_task.task = key->service;
		key_task.args = (void *)font;
		thread->add_task(tpool, &key_task);

	}

	struct wiegand_fun * wiegand=get_wieganddevice();
	ret = wiegand->init();
	if(0x00==ret)
	{
		wiegand_task.task = wiegand->service;
		wiegand_task.args = (void *)font;
		thread->add_task(tpool, &wiegand_task);
	}



	struct rtc_fun * rtc = get_rtcdevice();
	ret = rtc->init();
	if(0x00==ret)
	{
		rtc_task.task = rtc->service;
		rtc_task.args = (void *)font;
		thread->add_task(tpool, &rtc_task);
	}
	
	thread->run(tpool);

	for(; ;)
	{

	}
	thread->exit(tpool);
	lcd->free_lcd(lcd);
	return(0);

}



