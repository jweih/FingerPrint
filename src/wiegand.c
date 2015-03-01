#include"wiegand.h"

#undef  DBG_ON
#undef  FILE_NAME
#define DBG_ON  	(0x01)
#define FILE_NAME 	"wiegand:"


static int wiegand_fd;
static unsigned int wigand_value[3];


/***************************************************************************
*��������	:	Τ���豸��ʼ��	
*����ֵ		:	0==ok	others==wrong
*--------------------------------------------------------------------------
*2015/03/01		jweih 			����
****************************************************************************/

static int wiegand_init(void)
{

	wiegand_fd = open("/dev/wiegand_in", O_RDWR);
	if(wiegand_fd<0)
	{
		dbg_printf("open wiegand device fail\n");
		return(1);
	}
	memset((char*)wigand_value,0,sizeof(wigand_value));
	return(0);
}


/***************************************************************************
*��������	:	Τ������	
*����fun		:	�����豸�ӿ�
*����ֵ		:	void	
*--------------------------------------------------------------------------
*2015/03/01		jweih 			����
****************************************************************************/

static void wiegand_handler(struct font_fun * fun)
{
	int i=0;
	read(wiegand_fd,&wigand_value[0],2);
	fun->setcolor(fun->lcd->rgb(247,236,10),fun->lcd->rgb(0x00,0x00,0x00));
	fun->setsize(64,64);
	fun->show(fun->lcd,100,100,L"打卡",3);
	sleep(1);
	fun->lcd->fillrect(80,100,100,120,fun->lcd->rgb(0x00,0x00,0x00));



}


/***************************************************************************
*��������	:	Τ��������
*����args	:	�̴߳��ݵĲ���
*����ֵ		:	(void*)0	
*--------------------------------------------------------------------------
*2015/03/01		jweih 			����
****************************************************************************/

void * wiegand_service(void *args)
{
		struct font_fun * fun = (struct font_fun *)args;
		while(1)
		{
			wiegand_handler(fun);
		}

        return (void *) 0;
}


struct wiegand_fun wiegand_device_fun ={
	.init 		= wiegand_init,
	.service 	= wiegand_service,
};


struct wiegand_fun * get_wieganddevice()
{
	return(&wiegand_device_fun);
}




