#ifndef _KEY_H
#define _KEY_H

#include"common.h"


struct key_fun
{
	int (*init)(void);
	void * (*service)(void *args);
};



struct key_fun * get_keydevice();



#endif /*_KEY_H*/