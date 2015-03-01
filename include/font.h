#ifndef _FONT_H
#define _FONT_H

#include"common.h"




struct font_config
{
  FT_Library    library;
  FT_Face       face;
  FT_Matrix     matrix;
  unsigned char char_width;
  unsigned char char_height;
  unsigned short forecolor;
  unsigned short backcolor;

};


struct font_fun
{
	void (*init)(void);
	void (*free)(struct font_fun * font);
	void (*setsize)(unsigned char width,unsigned char height);
	void (*setcolor)(unsigned short fore,unsigned short back);
	void (*show)(struct lcd_fun * lcd,unsigned int x,unsigned int y,wchar_t *chinese_str,unsigned char offset);
	void (*clearlcd)(struct lcd_fun * lcd,unsigned int dstx, unsigned int dsty, unsigned int width, unsigned int height,unsigned short color);
	struct lcd_fun * lcd;
};


struct font_fun * get_fontdevice(struct lcd_fun * lcd);

#endif /*_FONT_H*/