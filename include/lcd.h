#ifndef _LCD_H
#define _LCD_H

#include"common.h"



typedef struct rgb
{
	unsigned char red;
	unsigned char green;
	unsigned char blue;
}rgb_t;

typedef struct lcd_struct
{

	int m_fbfd;
	struct fb_fix_screeninfo m_fix;
	struct fb_var_screeninfo m_var;
	unsigned char* m_bufvpage1;
	unsigned char* m_bufvpage2;
	unsigned char* m_bufvpage1paddr;
	unsigned char* m_bufvpage2paddr;
	
}lcd_struct_t;


struct lcd_fun
{
	unsigned int (*init)(void);
	unsigned short (*rgb)(unsigned char red,unsigned char green,unsigned char blue);
	void (*drawpoint)(unsigned int x,unsigned int y, unsigned short color);
	void (*drawbuffer)(unsigned char* buff, unsigned int dstx, unsigned int dsty, unsigned int width, unsigned int height);
	void (*drawfont)(unsigned char* buff, unsigned int dstx, unsigned int dsty, unsigned int width, unsigned int height,unsigned short forecolor,unsigned short backcolor);
	void (*fillrect)(unsigned int dstx, unsigned int dsty, unsigned int width, unsigned int height,unsigned short color);
	void (*drawbmp)(unsigned char* buff, unsigned int dstx, unsigned int dsty, unsigned int width, unsigned int height);
	void (*free_lcd)(struct lcd_fun * lcd);
};


struct lcd_fun * get_lcddevice();


#endif   /*_LCD_H*/

