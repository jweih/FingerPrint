#ifndef _COMMON_H
#define _COMMON_H

#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <execinfo.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <linux/fb.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <math.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <sys/mman.h>
#include <sys/types.h>
#include <wchar.h>
#include <pthread.h>
#include <errno.h>

#include"debug.h"
#include"lcd.h"
#include"font.h"
#include"thread.h"
#include"key.h"
#include"rtc.h"
#include"wiegand.h"


#define PRINT_DEBUG	

#define DBG_ON  	(0x01)
#define FILE_NAME 	"common:"
#define dbg_printf(fmt,arg...) \
	do{if(DBG_ON)fprintf(stderr,FILE_NAME"%s(line=%d)->"fmt,__FUNCTION__,__LINE__,##arg);}while(0)


#endif /*_COMMON_H*/