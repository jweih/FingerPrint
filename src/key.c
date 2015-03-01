#include"key.h"


#undef  DBG_ON
#undef  FILE_NAME
#define DBG_ON  	(0x01)
#define FILE_NAME 	"key:"


static int key_fd;
static unsigned char key_val;

struct keys
{
	unsigned char key_value;
	void (*key_fun)(struct font_fun * fun);
};


/***************************************************************************
*功能描述	:	按键设备的初始化	
*返回值		:	0==ok  others==wrong	
*--------------------------------------------------------------------------
*2015/03/01		jweih 			创建
****************************************************************************/

int key_init(void)
{
	key_val = 0xFF;
	key_fd = open("/dev/buttons", O_RDWR);
	if(key_fd<0)
	{
		dbg_printf("open key device fail\n");
		return(1);
	}	
	return(0);
}



/***************************************************************************
*功能描述	:	key1响应
*参数fun		:	字体操作接口
*返回值		:	void	
*--------------------------------------------------------------------------
*2015/03/01		jweih 			创建
****************************************************************************/

static void key_key1_fun(struct font_fun * fun)
{
	dbg_printf("this is key1\n");
	fun->setcolor(fun->lcd->rgb(163,46,204),fun->lcd->rgb(0x00,0x00,0x00));
	fun->setsize(64,64);
	fun->show(fun->lcd,100,100,L"K1",10);
	sleep(1);
	fun->lcd->fillrect(80,100,90,90,fun->lcd->rgb(0x00,0x00,0x00));


}


/***************************************************************************
*功能描述	:	key2响应
*参数fun		:	字体操作接口
*返回值		:	void	
*--------------------------------------------------------------------------
*2015/03/01		jweih 			创建
****************************************************************************/

static void key_key2_fun(struct font_fun * fun)
{
	dbg_printf("this is key2\n");
	fun->setcolor(fun->lcd->rgb(34,14,240),fun->lcd->rgb(0x00,0x00,0x00));
	fun->setsize(64,64);
	fun->show(fun->lcd,100,100,L"K2",10);
	sleep(1);
	fun->lcd->fillrect(80,100,90,90,fun->lcd->rgb(0x00,0x00,0x00));
	
}


/***************************************************************************
*功能描述	:	key3响应
*参数fun		:	字体操作接口
*返回值		:	void	
*--------------------------------------------------------------------------
*2015/03/01		jweih 			创建
****************************************************************************/

static void key_key3_fun(struct font_fun * fun)
{
	dbg_printf("this is key3\n");
	fun->setcolor(fun->lcd->rgb(241,20,68),fun->lcd->rgb(0x00,0x00,0x00));
	fun->setsize(64,64);
	fun->show(fun->lcd,100,100,L"K3",10);
	sleep(1);
	fun->lcd->fillrect(80,100,90,90,fun->lcd->rgb(0x00,0x00,0x00));

}

/***************************************************************************
*功能描述	:	key4响应
*参数fun		:	字体操作接口
*返回值		:	void	
*--------------------------------------------------------------------------
*2015/03/01		jweih 			创建
****************************************************************************/

static void key_key4_fun(struct font_fun * fun)
{
	dbg_printf("this is key4\n");
	fun->setcolor(fun->lcd->rgb(26,236,20),fun->lcd->rgb(0x00,0x00,0x00));
	fun->setsize(64,64);
	fun->show(fun->lcd,100,100,L"K4",10);
	sleep(1);
	fun->lcd->fillrect(80,100,90,90,fun->lcd->rgb(0x00,0x00,0x00));

}

/***************************************************************************
*功能描述	:	key5响应
*参数fun		:	字体操作接口
*返回值		:	void	
*--------------------------------------------------------------------------
*2015/03/01		jweih 			创建
****************************************************************************/

static void key_key5_fun(struct font_fun * fun)
{
	dbg_printf("this is key5\n");
	fun->setcolor(fun->lcd->rgb(247,236,10),fun->lcd->rgb(0x00,0x00,0x00));
	fun->setsize(64,64);
	fun->show(fun->lcd,100,100,L"K5",10);
	sleep(1);
	fun->lcd->fillrect(80,100,90,90,fun->lcd->rgb(0x00,0x00,0x00));

}




struct keys keys_deal[]={
	{0x01,key_key1_fun},
	{0x02,key_key2_fun},
	{0x04,key_key3_fun},
	{0x08,key_key4_fun},
	{0x10,key_key5_fun},
};


/***************************************************************************
*功能描述	:	按键的轮询
*参数fun		:	字体操作接口
*返回值		:	void	
*--------------------------------------------------------------------------
*2015/03/01		jweih 			创建
****************************************************************************/
void key_handler(struct font_fun * fun)
{
	int i=0;
	read(key_fd,&key_val,1);
	for(i=0;i<sizeof(keys_deal)/sizeof(keys_deal[0]);++i)
	{
		if(key_val == keys_deal[i].key_value )
		keys_deal[i].key_fun(fun);
	}

	key_val = 0xFF;
	

}


/***************************************************************************
*功能描述	:	按键服务函数
*参数args	:	线程传递的参数
*返回值		:	(void*)0	
*--------------------------------------------------------------------------
*2015/03/01		jweih 			创建
****************************************************************************/

void * key_service(void *args)
{
		struct font_fun * fun = (struct font_fun *)args;
		while(1)
		{
			key_handler(fun);
		}

        return (void *) 0;
}



struct key_fun key_device_fun ={
	.init 		= key_init,
	.service 	= key_service,
};



struct key_fun * get_keydevice()
{
	return(&key_device_fun);
}







