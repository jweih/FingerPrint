#include"common.h"


#undef  DBG_ON
#undef  FILE_NAME
#define DBG_ON  	(0x01)
#define FILE_NAME 	"debug:"


static struct sigaction sig_action;


 
/***************************************************************************
*功能描述:	信号处理函数
*参数sig	:	信号值
*返回值	:	void
*--------------------------------------------------------------------------
*2015/02/27		jweih 			创建
****************************************************************************/
static void  debug_sighandler(int sig)
{
    size_t  i = 0;
	size_t count = 0;
    void *tracePtrs[16];
	
#ifdef PRINT_DEBUG

    count = backtrace(tracePtrs, 16);
    char **funcNames = backtrace_symbols(tracePtrs, count);
    for(i = 0; i < count; ++i)
        dbg_printf("%s\n", funcNames[i]);
    free(funcNames);
    fflush(stderr);
    fflush(stdout);

#else

	int fd = open("err.log", O_CREAT | O_WRONLY);
	if(-1==fd)
	{
		dbg_printf("open err.log fail!\n");
		return;
	}
	count = backtrace(tracePtrs, 16);
	backtrace_symbols_fd(tracePtrs, count, fd);
	close(fd);

#endif
    
    exit(1);
}




/***************************************************************************
*功能描述:	信号初始化
*返回值	:	void
*--------------------------------------------------------------------------
*2015/02/27		jweih 			创建
****************************************************************************/

void debug_siginit(void)
{

	sig_action.sa_handler = (void (*)(int))debug_sighandler;
	sigemptyset(&sig_action.sa_mask);
	sig_action.sa_flags = SA_RESTART | SA_SIGINFO;
	sigaction(SIGSEGV, &sig_action, NULL);
	sigaction(SIGFPE, &sig_action, NULL);
	sigaction(SIGBUS, &sig_action, NULL);
	sigaction(SIGABRT, &sig_action, NULL);
	sigaction(SIGINT, &sig_action, NULL);	
	sigaction(SIGTERM, &sig_action, NULL);
}

















