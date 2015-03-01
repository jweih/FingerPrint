#include"lcd.h"


#undef  DBG_ON
#undef  FILE_NAME
#define DBG_ON  	(0x01)
#define FILE_NAME 	"lcd:"


#define		fbdev   "/dev/fb0" 
#define		lcd_width	(240u)
#define		lcd_height	(320u)


static lcd_struct_t *lcd_device = NULL;


/***************************************************************************
*功能描述		:	lcd设备的初始化
*返回值		:	0==ok  others==wrong
*--------------------------------------------------------------------------
*2015/02/28		jweih 			创建
****************************************************************************/

static unsigned int lcd_init(void)
{

	int ret = -1;

	if(NULL == lcd_device )
	{
		lcd_device = (lcd_struct_t*)malloc(sizeof(*lcd_device));
		if(NULL == lcd_device )
		{
			dbg_printf("malloc lcd device fail \n");
			return(1);
		}
		memset(lcd_device, 0, sizeof(*lcd_device));
	}

	lcd_device->m_fbfd = open(fbdev, O_RDWR);

	if(-1 == lcd_device->m_fbfd)
	{
		dbg_printf("open lcd device fail \n");
		return(2);	
	}

	ret = ioctl(lcd_device->m_fbfd, FBIOGET_FSCREENINFO, &lcd_device->m_fix);
	if(ret<0)
	{

		dbg_printf("get lcd device fix para fail \n");
		close(lcd_device->m_fbfd);
		return(3);	

	}
	
	ret = ioctl(lcd_device->m_fbfd, FBIOGET_VSCREENINFO, &lcd_device->m_var);
	if(ret<0)
	{

		dbg_printf("get lcd device var para fail \n");
		close(lcd_device->m_fbfd);
		return(4);	
	}

	lcd_device->m_bufvpage1paddr = (unsigned char*)((lcd_device->m_fix).smem_start);
	lcd_device->m_bufvpage2paddr = lcd_device->m_bufvpage1paddr + ((lcd_device->m_fix).smem_len) / 2;

	lcd_device->m_bufvpage1 = (unsigned char*)mmap(NULL, (lcd_device->m_fix).smem_len, PROT_READ | PROT_WRITE, MAP_SHARED, lcd_device->m_fbfd, 0);
	if (lcd_device->m_bufvpage1 == (void *)-1)
	{
		dbg_printf("lcd device mmap fail \n");
		close(lcd_device->m_fbfd);
		return(5);
	}
	lcd_device->m_bufvpage2 = lcd_device->m_bufvpage1 + ((lcd_device->m_fix).smem_len)/2;

	return(0);
	
}



/***************************************************************************
*功能描述		:	RGB888色转换成RGB565色
*参数red		:	红色值
*参数green	:	绿色值
*参数blue	:	蓝色值
*返回值		:	转换成的RGB565值
*--------------------------------------------------------------------------
*2015/02/28		jweih 			创建
****************************************************************************/

inline unsigned short lcd_rgb(unsigned char red,unsigned char green,unsigned char blue)
{
	return(((red>>3)<<11)|((green>>2)<<5)|((blue>>3)<<0));	
}


/***************************************************************************
*功能描述		:	矩形填充	
*参数dstx	:	起始横坐标
*参数dsty	:	起始纵坐标
*参数width	:	填充宽度
*参数height	:	填充长度
*参数color	:	填充颜色
*返回值		:	void	
*--------------------------------------------------------------------------
*2015/02/28		jweih 			创建
****************************************************************************/

static void lcd_fillrect(unsigned int dstx, unsigned int dsty, unsigned int width, unsigned int height,unsigned short color)
{


	unsigned int i=0;
	unsigned short color_buffer[lcd_width];
	if(dstx+width>lcd_width || dsty+height>lcd_height)
	{
		dbg_printf("the pos out of limit \n");
		return;
	}
	for(i=0;i<width;++i)
	color_buffer[i] = color; 	

	for(i = 0; i < height; i ++)
	{
		memcpy(lcd_device->m_bufvpage1 + ((dsty + i) * lcd_device->m_var.xres + dstx)*2, color_buffer, width*2);	
	}
	ioctl(lcd_device->m_fbfd, FBIOPAN_DISPLAY, &lcd_device->m_var);
}


/***************************************************************************
*功能描述		:	画点
*参数x		:	点所在横坐标
*参数y		:	点所在纵坐标
*参数color	:	点的颜色
*返回值		:	void	
*--------------------------------------------------------------------------
*2015/02/28		jweih 			创建
****************************************************************************/

static void lcd_drawpoint(unsigned int x,unsigned int y, unsigned short color)
{
	unsigned short *lcd_dest=NULL;
	if(x>lcd_width || y>lcd_height)
	{
		dbg_printf("the pos out of limit \n");
		return;
	}
	lcd_dest = (unsigned short *)(lcd_device->m_bufvpage1 + (y * ((lcd_device->m_var).xres)+ x)*2);
	*lcd_dest = color;
	ioctl(lcd_device->m_fbfd, FBIOPAN_DISPLAY, &lcd_device->m_var);
}


/***************************************************************************
*功能描述		:	显示图片
*参数buff	:	图片数据
*参数dstx	:	起始横坐标
*参数dsty	:	起始纵坐标
*参数width	:	显示宽度(一般和实际的图片宽度一致)
*参数height	:	显示高度(一般和实际的图片高度一致)
*返回值		:	void
*--------------------------------------------------------------------------
*2015/02/28		jweih 			创建
*2015/03/01		jweih 			优化显示速度
****************************************************************************/

static void lcd_drawbmp(unsigned char* buff, unsigned int dstx, unsigned int dsty, unsigned int width, unsigned int height)
{

	unsigned int i=0;
	if(NULL == buff )
	{
		dbg_printf("buff is null \n");
		return;	
	}
	
	if(dstx+width>lcd_width || dsty+height>lcd_height)
	{
		dbg_printf("the pos out of limit \n");
		return;
	}
	
	for(i = 0; i < height; i ++)
	{
		memcpy(lcd_device->m_bufvpage1 + ((dsty + i) * lcd_device->m_var.xres + dstx)*2, (buff + i * width*2), width*2);	
	}
	ioctl(lcd_device->m_fbfd, FBIOPAN_DISPLAY, &lcd_device->m_var);
}



/***************************************************************************
*功能描述			:	绘制字体	
*参数buff		:	字体数据
*参数dstx		:	起始横坐标
*参数dsty		:	起始纵坐标
*参数width		:	字体宽度
*参数height		:	字体高度
*参数forecolor	:	字体前景色
*参数backcolor	:	字体背景色
*返回值			:	void	
*--------------------------------------------------------------------------
*2015/02/28		jweih 			创建
*2015/02/28		jweih 			修复坐标显示
*2015/03/01		jweih 			优化显示速度
****************************************************************************/


/*思路:先拷贝原来的存储区域，打点的时候，对非0x00的用设置的颜色，0x00的用原来提取的颜色置换*/
/*待调整!!!!!!!!!!!!!!!!!!!!!!!*/

static void lcd_drawfont(unsigned char* buff, unsigned int dstx, unsigned int dsty, unsigned int width, unsigned int height,unsigned short forecolor,unsigned short backcolor)
{

	unsigned  int i = 0;
	unsigned int k1=0;
	for(k1=0;k1<width*height;k1+=2)
	{
		if(buff[k1] == 0x00) /*粗字体*/
		//if(buff[k1] != 0xFF) /*细字体*/
		{
			buff[k1] = backcolor&0xFF;
			buff[k1+1] = (backcolor>>8)&0xFF;
		}
		else 
		{
			buff[k1] = forecolor&0xFF;
			buff[k1+1] = (forecolor>>8)&0xFF;
		}	
	}
	for(i = 0; i < height; i ++)
	{
		memcpy(lcd_device->m_bufvpage1 + ((dsty + i) * lcd_device->m_var.xres + dstx)*2, (buff + i * width), width);
	}
	
	ioctl(lcd_device->m_fbfd, FBIOPAN_DISPLAY, &lcd_device->m_var);


}



/***************************************************************************
*功能描述		:	释放lcd设备
*参数lcd		:	lcd功能接口
*返回值		:	void	
*--------------------------------------------------------------------------
*2015/02/28		jweih 			创建
****************************************************************************/

static void  lcd_free(struct lcd_fun * lcd)
{
	if(NULL != lcd && NULL != lcd_device)
	{
		memset(lcd_device->m_bufvpage1,0x0000,lcd_device->m_var.xres * lcd_device->m_var.yres * 2);
		ioctl(lcd_device->m_fbfd, FBIOPAN_DISPLAY, &lcd_device->m_var);
		munmap(lcd_device->m_bufvpage1, lcd_device->m_fix.smem_len);
		lcd_device->m_bufvpage1 = lcd_device->m_bufvpage2 = (void*)0;
		close(lcd_device->m_fbfd);
		free(lcd_device);
		lcd_device = NULL;

	}

}


struct lcd_fun lcd_device_fun ={
	
	.init		= lcd_init,
	.rgb		= lcd_rgb,
	.drawpoint	= lcd_drawpoint,
	.drawbuffer	= lcd_drawbmp,
	.drawfont	= lcd_drawfont,
	.fillrect	= lcd_fillrect,
	.drawbmp	= lcd_drawbmp,
	.free_lcd 	= lcd_free,
};



struct lcd_fun * get_lcddevice()
{
	return(&lcd_device_fun);
}


