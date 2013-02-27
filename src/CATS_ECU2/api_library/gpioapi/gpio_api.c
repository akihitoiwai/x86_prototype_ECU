/**********************************************************************************
 * ファイル名      : gpio_api.c
 * システム名      : x86 プロトタイプＥＣＵ OPE-RA Ver2.0
 * サブシステム名  : GPIO API
 * プログラム名    : 
 * CPU TYPE        : 
 * バージョン      : Ver1.01
 * モジュール構成  : gpio_api.h			API Header
 * --------------------------------------------------------------------------------
 * 作成者          : 
 * 作成部署        : 
 * 作成日付        : 2009年10月26日 新規作成
 * 更新履歴        : 2010年06月10日
 **********************************************************************************/
#include <stdio.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include <signal.h>
#include <string.h>

#include "gpio_api.h"
#include "/usr/src/CATS_ECU2/kernel_driver/cats_ioctl.h"

#define CATSDRV_PATH "/dev/catsdrv6"

#define MASK24BIT 0xFFFFFF
#define MASK16BIT 0xFFFF

typedef struct gpio_callback_tbl {
	int fasync_use;
	void (* gpio_callback_fnc) (void);
} GPIO_CALLBACK_TBL;

GPIO_CALLBACK_TBL gpio_table = { 0, NULL };

int g_gpio_fd = 0;								/* GPIO 機能のファイルディスクリプタ */

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 	機能		:		api_simple_ioctl
//	引数 		:		unsigned int ioctl command
//	戻り値		:		int(0-OK)
//	機能説明	: 		パラメタ無しＩＯＣＴＬの機能です。
static inline int api_simple_ioctl( unsigned int cmd )
{
	int ret;

	if(g_gpio_fd == 0)							/* ファイルディスクリプタ未取得の場合 */
	{
		g_gpio_fd =open(CATSDRV_PATH,O_RDWR);	/* 取得 */
		if( g_gpio_fd < 0 )						/* 取得失敗した */
		{
			return API_NG;
		}
	}

	ret = ioctl(g_gpio_fd,cmd);

	if(ret<0){
		if(-errno==CATS_ERR_BUSY) {
			return API_BUSY;
		}

		return API_NG;
	}

	return API_OK;
}

//------------------------------------------------------------------------------
// 	機能		:		api_arg_ioctl
//	引数 		:		unsigned int	- コマンド
//			:		int * 		- コマンドパラメタ
//	戻り値		:		int(0-OK)
//	機能説明	: 		パラメタ在りＩＯＣＴＬの機能です。
static inline int api_arg_ioctl ( unsigned int cmd, int *arg )
{
	int ret;

	if(g_gpio_fd == 0)							/* ファイルディスクリプタ未取得の場合 */
	{
		g_gpio_fd =open(CATSDRV_PATH,O_RDWR);	/* 取得 */
		if( g_gpio_fd < 0 )						/* 取得失敗した */
		{
			return API_NG;
		}
	}

	ret = ioctl(g_gpio_fd, cmd, arg);

	if(ret<0){
		if(-errno==CATS_ERR_BUSY) {
			return API_BUSY;
		}

		return API_NG;
	}

	return API_OK;
}


//------------------------------------------------------------------------------
// 	機能		:		FncinitGPIOOutput
//	引数 		:		無し
//	戻り値		:		int(0-OK)
//	機能説明	: 		ＧＰＩＯ出力の初期機能です。
int FncInitGPIOOutput ( void )
{
	return api_simple_ioctl(IOCTL_CATS_FncinitGPIOOutput);
}

//------------------------------------------------------------------------------
// 	機能		:		FncinitGPIOInput
//	引数 		:		無し
//	戻り値		:		int 
//	機能説明	: 		ＧＰＩＯ入力の初期機能です。
int FncInitGPIOInput ( void )
{
	return api_simple_ioctl(IOCTL_CATS_FncinitGPIOInput);
}

//------------------------------------------------------------------------------
// 	機能		:		FncGPIOSet
//	引数 		:		unsigned int - チャネル
//			:		unsigned int - output value
//	戻り値		:		int(0-OK)
//	機能説明	: 		チャネルのＧＰＩＯ出力を設定する機能です。
int FncGPIOSet ( unsigned int ch, unsigned int val )
{
	int ioctl_arg;

	//check for invalid arguments
	if((ch>23)||(ch<0)) return API_NG;
	if((val>1)||(val<0)) return API_NG;

	ioctl_arg = val << 31 | 1 << ch;

	return api_arg_ioctl(IOCTL_CATS_FncGPIOSet,&ioctl_arg);
}


//------------------------------------------------------------------------------
// 	機能		:		FncGPIOSetAll
//	引数		:		unsigned int - output value
//	戻り値		:		int(0-OK)
//	機能説明	: 		２４チャネル分のＧＰＩＯ出力を設定する
int FncGPIOSetAll( unsigned int val )
{
	int ioctl_arg;

	ioctl_arg = val & MASK24BIT;			/* 24Bit MASK */

	return api_arg_ioctl(IOCTL_CATS_FncGPIOSetAll,&ioctl_arg);

}


//------------------------------------------------------------------------------
// 	機能		:		FncGPIOGet
//	引数 		:		unsigned int	- チャネル
//			:		unsigned int*	- input value
//	戻り値		:		int(0-OK)
//	機能説明	: 		チャネルのＧＰＩＯ入力データ読み込み機能です。
int FncGPIOGet ( unsigned int ch, unsigned int *val )
{
	int ret;
	int ioctl_arg;

	//check for invalid arguments
	if((ch>15)||(ch<0)) return API_NG;

	ret = api_arg_ioctl(IOCTL_CATS_FncGPIOGet,&ioctl_arg);

	*val = (ioctl_arg >> ch) & 0x1;

	return ret;
}


//------------------------------------------------------------------------------
// 	機能		:		FncGPIOGetAll
//	引数		:		unsigned int - output value
//	戻り値		:		int(0-OK)
//	機能説明	: 		１６チャネル分ＧＰＩＯ入力データ読み込む
int FncGPIOGetAll( unsigned int *val )
{
	int ret;
	int ioctl_arg;

	ret = api_arg_ioctl(IOCTL_CATS_FncGPIOGetAll,&ioctl_arg);

	*val =  ioctl_arg & MASK16BIT;			/* 16Bit MASK */

	return ret;
}


//------------------------------------------------------------------------------
// 	機能		:		FncGPIOIntEnable
//	引数 		:		unsigned int*	- (enable/disable array)
//			:		unsigned int*	- (level array)
//	戻り値		:		int(0-OK)
//	機能説明	: 		ＧＰＩＯ入力割り込みの無効/有効にする機能です。
int FncGPIOIntEnable ( unsigned int *sts, unsigned int *lvl )
{
	int ret;
	int ioctl_arg[2] = {0,0};
	int *ioctl_ptr;
	int i;

	ioctl_ptr = &ioctl_arg[0];

	for (i=0;i<16;i++) {
		//check for invalid arguments
		if((sts[i]>1)||(sts[i]<0)) return API_NG;
        ioctl_arg[0] = ioctl_arg[0] | sts[i]<<(15-i);
	}

	for (i=0;i<16;i++) {
		//check for invalid arguments
		if((lvl[i]>1)||(lvl[i]<0)) return API_NG;
        ioctl_arg[1] = ioctl_arg[1] | lvl[i]<<(15-i);
	}

	if(g_gpio_fd == 0)							/* ファイルディスクリプタ未取得の場合 */
	{
		g_gpio_fd =open(CATSDRV_PATH,O_RDWR);	/* 取得 */
		if( g_gpio_fd < 0 )						/* 取得失敗した */
		{
			return API_NG;
		}
	}

	ret = ioctl(g_gpio_fd, IOCTL_CATS_FncGPIOIntEnable,&ioctl_ptr);

	if(ret<0){

		return API_NG;
	}

	return API_OK;
}


//------------------------------------------------------------------------------
// 	機能		:		FncGPIOIntEnable2
//	引数 		:		unsigned int	- (enable/disable)
//			:		unsigned int	- (level)
//	戻り値		:		int(0-OK)
//	機能説明	: 		ＧＰＩＯ入力割り込みの無効/有効にする機能です。
int FncGPIOIntEnable2( unsigned int sts, unsigned int lvl )
{
	int ret;
	int ioctl_arg[2] = {0,0};
	int *ioctl_ptr;

	if(g_gpio_fd == 0)							/* ファイルディスクリプタ未取得の場合 */
	{
		g_gpio_fd =open(CATSDRV_PATH,O_RDWR);	/* 取得 */
		if( g_gpio_fd < 0 )						/* 取得失敗した */
		{
			return API_NG;
		}
	}

	ioctl_arg[0] = sts & MASK16BIT;
	ioctl_arg[1] = lvl & MASK16BIT;

	ioctl_ptr = &ioctl_arg[0];

	ret = ioctl(g_gpio_fd, IOCTL_CATS_FncGPIOIntEnable,&ioctl_ptr);

	if(ret<0){

		return API_NG;
	}

	return API_OK;
}


//------------------------------------------------------------------------------
// 	機能		:		FncGPIOIntSts
//	引数 		:		unsigned int*	- (status array)
//	戻り値		:		int(0-OK)
//	機能説明	: 		ＧＰＩＯ割り込みのステータス読み込み機能です。
int FncGPIOIntSts ( unsigned int *val )
{
	int ret;
	int ioctl_arg;
	int i;

	ret = api_arg_ioctl(IOCTL_CATS_FncGPIOIntSts,&ioctl_arg);

	for(i=0;i<16;i++){
	    val[(15-i)] = (ioctl_arg >> i) & 0x1;
	}

	return ret;
}


//------------------------------------------------------------------------------
// 	機能		:		FncGPIOIntSts2
//	引数		:		unsigned int*	- (status array)
//	戻り値		:		int(0-OK)
//	機能説明	: 		１６チャネル分のＧＰＩＯ割り込み状態を取得する
int FncGPIOIntSts2 ( unsigned int *val )
{
	int ret;
	int ioctl_arg;

	ret = api_arg_ioctl(IOCTL_CATS_FncGPIOIntSts,&ioctl_arg);

	*val =  ioctl_arg & MASK16BIT;			/* 16Bit MASK */

	return ret;
}

//------------------------------------------------------------------------------
// 	機能		:		FncGPIOIntClear
//	引数	 	:		unsigned int	- ＧＰＩＯ割り込み
//	戻り値		:		int(0-OK)
//	機能説明	: 		ＧＰＩＯ割り込みのクリア機能です。
int FncGPIOIntClear ( unsigned int val )
{
	return api_arg_ioctl(IOCTL_CATS_FncGPIOIntClear, &val);
}


/***********************************************************************************
* MODULE         : Gpio_SigHandler
* ABSTRACT       : GPIO シグナルハンドラ
* FUNCTION       : 
* ARGUMENT       : 
* NOTE           : 
* RETURN         : 正常終了で「０」を返す。
* CREATE         : 2009/12/10  新規作成 
* UPDATE         : 2010/06/10
***********************************************************************************/
void Gpio_SigHandler(int signo)
{
	int ret;
	int ioctl_arg;

//	printf("gpio callback\n");

	/* GPIO 割り込みチェック */
	ret = ioctl(g_gpio_fd, IOCTL_CATS_CallBackGPIOGetIrq, &ioctl_arg);
//	printf("IOCTL_CATS_CallBackGPIOGetIrq = %x ret = %d\n",ioctl_arg ,ret );


	if(( ioctl_arg & GPIO_INT ) != 0 ){
//		printf("GPIO INT Det!! gpio_intreq = %x\n", ioctl_arg);

		/* 割り込み要求解除 */
		ret = ioctl(g_gpio_fd, IOCTL_CATS_CallBackGPIOClrIrq, &ioctl_arg);
//		printf("IOCTL_CATS_CallBackGPIOClrIrq gpio_intreq = %x\n",ioctl_arg );

		/* コールバック関数登録チェック */
		if(( *gpio_table.gpio_callback_fnc ) != NULL ){
			(*gpio_table.gpio_callback_fnc)();
		}
	}
}

/***********************************************************************************
* MODULE         : FncGpioHandlerSet
* ABSTRACT       : GPIO 割り込みのコールバック関数を登録する。
* FUNCTION       : 
* ARGUMENT       : 
* NOTE           : *func :callback function (pointer)
* RETURN         : 正常終了で「０」を返す。
* CREATE         : 2009/12/10  新規作成 
* UPDATE         : 
***********************************************************************************/
int FncGpioHandlerSet ( void * func )
{
	int ret;
	int oflags;
	struct sigaction act;

	memset(&act ,0 ,sizeof( struct sigaction ));

	if(func == NULL)
	{
		return API_NG;
	}

	if(g_gpio_fd == 0)							/* ファイルディスクリプタ未取得の場合 */
	{
		g_gpio_fd =open(CATSDRV_PATH,O_RDWR);	/* 取得 */
		if( g_gpio_fd < 0 )						/* 取得失敗した */
		{
			return API_NG;
		}
	}

	/* シグナルハンドラ登録 */
	if(gpio_table.fasync_use == 0)
	{
		sigaction(SIGIO, NULL, &act);			/* 設定前の取得 */
		act.sa_handler = Gpio_SigHandler;		/* ハンドラの設定 */
		sigemptyset(&act.sa_mask);				/* sa_mask の初期化 */
		sigaddset(&act.sa_mask, SIGIO);			/* シグナルハンドラ実行中はブロック */
		act.sa_flags = 0;						/* sa_flags はデフォルト */
		act.sa_restorer = NULL;					/* sa_restorer は未使用 */
		sigaction(SIGIO, &act, NULL);			/* シグナルハンドラ登録 */

		fcntl(g_gpio_fd, F_SETOWN, getpid());
		oflags = fcntl(g_gpio_fd, F_GETFL);
		fcntl(g_gpio_fd, F_SETFL, oflags | FASYNC);
		gpio_table.fasync_use = 1;
	}

	gpio_table.gpio_callback_fnc = func;
	ret = API_OK;

	return ret;
}


/***********************************************************************************
* MODULE         : FncGpioHandlerClear
* ABSTRACT       : GPIO 割り込みのコールバック関数を解除する。
* FUNCTION       : 
* ARGUMENT       : 
* NOTE           : 
* RETURN         : 正常終了で「０」を返す。
* CREATE         : 2009/12/10  新規作成 
* UPDATE         : 
***********************************************************************************/
int FncGpioHandlerClear ( void )
{
	int ret;

	gpio_table.gpio_callback_fnc = NULL;
	ret = API_OK;

	return ret;
}

