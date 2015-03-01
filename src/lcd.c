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
*��������	:	lcd�豸�ĳ�ʼ��
*����ֵ		:	0==ok  others==wrong
*--------------------------------------------------------------------------
*2015/02/28		jweih 			����
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
*��������	:	RGB888ɫת����RGB565ɫ
*����red		:	��ɫֵ
*����green	:	��ɫֵ
*����blue	:	��ɫֵ
*����ֵ		:	ת���ɵ�RGB565ֵ
*--------------------------------------------------------------------------
*2015/02/28		jweih 			����
****************************************************************************/

inline unsigned short lcd_rgb(unsigned char red,unsigned char green,unsigned char blue)
{
	return(((red>>3)<<11)|((green>>2)<<5)|((blue>>3)<<0));	
}


/***************************************************************************
*��������	:	�������	
*����dstx	:	��ʼ������
*����dsty	:	��ʼ������
*����width	:	�����
*����height	:	��䳤��
*����color	:	�����ɫ
*����ֵ		:	void	
*--------------------------------------------------------------------------
*2015/02/28		jweih 			����
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
*��������	:	����
*����x		:	�����ں�����
*����y		:	������������
*����color	:	�����ɫ
*����ֵ		:	void	
*--------------------------------------------------------------------------
*2015/02/28		jweih 			����
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
*��������	:	��ʾͼƬ
*����buff	:	ͼƬ����
*����dstx	:	��ʼ������
*����dsty	:	��ʼ������
*����width	:	��ʾ���(һ���ʵ�ʵ�ͼƬ���һ��)
*����height	:	��ʾ�߶�(һ���ʵ�ʵ�ͼƬ�߶�һ��)
*����ֵ		:	void
*--------------------------------------------------------------------------
*2015/02/28		jweih 			����
*2015/03/01		jweih 			�Ż���ʾ�ٶ�
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
*��������		:	��������	
*����buff		:	��������
*����dstx		:	��ʼ������
*����dsty		:	��ʼ������
*����width		:	������
*����height		:	����߶�
*����forecolor	:	����ǰ��ɫ
*����backcolor	:	���屳��ɫ
*����ֵ			:	void	
*--------------------------------------------------------------------------
*2015/02/28		jweih 			����
*2015/02/28		jweih 			�޸�������ʾ
*2015/03/01		jweih 			�Ż���ʾ�ٶ�
****************************************************************************/


/*˼·:�ȿ���ԭ���Ĵ洢���򣬴���ʱ�򣬶Է�0x00�������õ���ɫ��0x00����ԭ����ȡ����ɫ�û�*/
/*������!!!!!!!!!!!!!!!!!!!!!!!*/

static void lcd_drawfont(unsigned char* buff, unsigned int dstx, unsigned int dsty, unsigned int width, unsigned int height,unsigned short forecolor,unsigned short backcolor)
{

	unsigned  int i = 0;
	unsigned int k1=0;
	for(k1=0;k1<width*height;k1+=2)
	{
		if(buff[k1] == 0x00) /*������*/
		//if(buff[k1] != 0xFF) /*ϸ����*/
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
*��������	:	�ͷ�lcd�豸
*����lcd		:	lcd���ܽӿ�
*����ֵ		:	void	
*--------------------------------------------------------------------------
*2015/02/28		jweih 			����
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


