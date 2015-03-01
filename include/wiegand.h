#ifndef _WIEGAND_H
#define   _WIEGAND_H

#include"common.h"



struct wiegand_fun
{
	int (*init)(void);
	void * (*service)(void *args);
};


struct wiegand_fun * get_wieganddevice();



#endif  /*_WIEGAND_H*/

