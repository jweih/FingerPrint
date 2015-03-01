
#include"font.h"


#undef  DBG_ON
#undef  FILE_NAME
#define DBG_ON  	(0x01)
#define FILE_NAME 	"font:"

#define		font_pos_width		(240u)
#define		font_pos_height		(320u)

#define  font_file	"/usr/fzlth.ttf"

static struct font_config  *font_para = NULL;


/***************************************************************************
*��������:	�������豸
*����ֵ	:	0==ok	others==wrong
*--------------------------------------------------------------------------
*2015/02/28		jweih 			����
****************************************************************************/
static int  font_open(void)
{

	FT_Error  ret=0;
	ret = FT_Init_FreeType(&font_para->library);
	if(ret)
	{
		dbg_printf("FreeType init fail! \n");
		return(1);		
	}
	ret = FT_New_Face(font_para->library, font_file, 0, &font_para->face);
	if(ret)
	{
		dbg_printf("FT_New_Face fail! \n");
		return(2);		
	}

	ret = FT_Select_Charmap(font_para->face, FT_ENCODING_UNICODE);
	if(ret)
	{
		dbg_printf("FT_Select_Charmap fail! \n");
		return(3);		
	}

	ret=FT_Set_Pixel_Sizes(font_para->face, font_para->char_width, font_para->char_height);
	if(ret)
	{
		dbg_printf("FT_Set_Pixel_Sizes fail! \n");
		return(4);		
	}

	return(0);


}

/***************************************************************************
*��������:	��ʼ��������Լ����ò���
*--------------------------------------------------------------------------
*2015/02/28		jweih 			����
****************************************************************************/

static void font_init(void)
{
	font_para = (struct font_config *)malloc(sizeof(*font_para));
	if(NULL == font_para )
	{
		dbg_printf("malloc is fail \n");
		return;	

	}
	memset(font_para,0,sizeof(*font_para));
	font_para->char_width 		= 64;
	font_para->char_height 		= 64;

	font_para->matrix.xx 		= (FT_Fixed)( -1 * 0x10000L );
	font_para->matrix.xy 		= (FT_Fixed)( 0 * 0x10000L );
	font_para->matrix.yx 		= (FT_Fixed)( 0 * 0x10000L );
	font_para->matrix.yy 		= (FT_Fixed)( -1 * 0x10000L );

	font_para->forecolor  		= 0xFFFF;
	font_para->backcolor 		= 0x0000;
	
	font_open();

}



/***************************************************************************
*��������	:	�������豸
*����font	:	��������ӿڽṹ��
*--------------------------------------------------------------------------
*2015/02/28		jweih 			����
****************************************************************************/

void font_free(struct font_fun * font)
{
	if(NULL != font_para && NULL != font)
	{
  		FT_Done_Face    ( font_para->face );
  		FT_Done_FreeType( font_para->library );	
		free(font_para);
		font_para = NULL;
	}


}


/***************************************************************************
*��������	:	����������ʾ����	
*����width	:	������
*����height	:	����߶�
*����ֵ		:	void
*--------------------------------------------------------------------------
*2015/02/28		jweih 			����
****************************************************************************/


void font_setsize(unsigned char width,unsigned char height)
{

	FT_Error  ret=0;
	if(NULL == font_para )return;
	if( width==font_para->char_width && height==font_para->char_height)return;
	ret=FT_Set_Pixel_Sizes(font_para->face, width, height);
	if(ret)
	{
		dbg_printf("FT_Set_Pixel_Sizes fail! \n");
		return;		
	}
	font_para->char_width = width;
	font_para->char_height= height;
}

/***************************************************************************
*��������	:	����������ʾ��ɫ
*����fore	:	ǰ��ɫ
*����back	:	����ɫ
*����ֵ		:	void	
*--------------------------------------------------------------------------
*2015/02/28		jweih 			����
****************************************************************************/

void font_setcolor(unsigned short fore,unsigned short back)
{
	if(NULL == font_para )return;
	if( fore==font_para->forecolor && back==font_para->backcolor)return;

	font_para->forecolor = fore;
	font_para->backcolor = back;
	
}


/***************************************************************************
*��������	:	������λͼ
*����bitmap	:	��������ɵ�����λͼ����
*����x		:	������ʾ������
*����y		:	������ʾ������
*����lcd		:	lcd�豸�����ӿ�
*����ֵ		:	void
*--------------------------------------------------------------------------
*2015/02/28		jweih 			����
****************************************************************************/

inline void font_bitmap( FT_Bitmap*  bitmap,FT_Int x, FT_Int  y,struct lcd_fun * lcd)                        
{

	if(NULL != lcd)
		lcd->drawfont(bitmap->buffer,x,y,bitmap->width,bitmap->rows,font_para->forecolor,font_para->backcolor);

}





/***************************************************************************
*��������		:	��ʾ�ַ���(����+����+accsii)	
*����lcd			:	lcd�豸���ܽӿ�
*����x			:	�ַ�����ʼ��ʾ�ĺ�����
*����y			:	�ַ�����ʼ��ʾ��������
*����chinese_str	:	Ҫ��ʾ���ַ���
*����offset		:	�ַ�֮��ļ��
*����ֵ			:	void
*--------------------------------------------------------------------------
*2015/02/28		jweih 			����
****************************************************************************/

static void font_show(struct lcd_fun * lcd,unsigned int x,unsigned int y,wchar_t *chinese_str,unsigned char offset)
{

	FT_GlyphSlot  slot;
	FT_Vector     pen;                   
	FT_Error      error;
	int           n;
 
	if(NULL == font_para )return;
	if(x>font_pos_width || y>font_pos_height )return;
	if(NULL == chinese_str )return;

	slot = font_para->face->glyph;
	pen.x = (-x) * 64;
	pen.y = (-24+font_pos_height-y) * 64;

	for(n = 0; n < wcslen(chinese_str); n++ )
	{
	    FT_Set_Transform( font_para->face, &font_para->matrix, &pen );
	    error = FT_Load_Char( font_para->face, chinese_str[n], FT_LOAD_RENDER );
	    if ( error )
	      continue;           
	  	font_bitmap( &slot->bitmap,slot->bitmap_left+font_para->char_width/2-5,font_pos_height-slot->bitmap_top,lcd);
	    pen.x += slot->advance.x/2+(-offset) * 64; 
	    pen.y += slot->advance.y;
	}

}


/***************************************************************************
*��������	:	���ƾ��λ�����
*����lcd		:	lcd�豸���ܽӿ�
*����dstx	:	��ʼ������
*����dsty	:	��ʼ������
*����width	:	���ƵĿ��
*����height	:	���Ƶĸ߶�
*����color	:	���Ƶ���ɫ
*����ֵ		:	void
*--------------------------------------------------------------------------
*2015/02/28		jweih 			����
****************************************************************************/

static void font_clearlcd(struct lcd_fun * lcd,unsigned int dstx, unsigned int dsty, unsigned int width, unsigned int height,unsigned short color)
{
	if(NULL != lcd)
	   lcd->fillrect(dstx,dsty,width,height,color);

}


struct font_fun font_device_fun ={
	.init = font_init,
	.free = font_free,
	.setsize = font_setsize,
	.setcolor = font_setcolor,
	.show = font_show,
	.clearlcd = font_clearlcd,

};


struct font_fun * get_fontdevice(struct lcd_fun * lcd)
{
	font_device_fun.lcd = (struct lcd_fun * )malloc(sizeof(struct lcd_fun));
	memcpy(font_device_fun.lcd,lcd,sizeof(struct lcd_fun));
	
	return(&font_device_fun);
}



