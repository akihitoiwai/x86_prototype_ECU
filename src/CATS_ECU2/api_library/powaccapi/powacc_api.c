/**********************************************************************************
 * ファイル名      : powacc_api.c
 * システム名      : x86 プロトタイプＥＣＵ OPE-RA Ver2.0
 * サブシステム名  : pow,acc API
 * プログラム名    : 
 * CPU TYPE        : 
 * バージョン      : Ver1.00
 * モジュール構成  : powacc_api.h			API Header
 * --------------------------------------------------------------------------------
 * 作成者          : 
 * 作成部署        : 
 * 作成日付        : 2009年09月23日 新規作成
 * 更新履歴        : 2009年12月11日
 **********************************************************************************/
#include <stdio.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include <signal.h>
#include <string.h>

#include "powacc_api.h"
#include "/usr/src/CATS_ECU2/kernel_driver/cats_ioctl.h"

#define CATSDRV_PATH "/dev/catsdrv7"

typedef struct volmoni_callback_tbl {
	int fasync_use;
	void (* powermoni_callback_fnc) (void);
	void (* accmoni_callback_fnc) (void);
} VOLMONI_CALLBACK_TBL;

VOLMONI_CALLBACK_TBL volmoni_table = { 0, NULL, NULL };

int g_volmoni_fd = 0;							/* 電圧監視機能のファイルディスクリプタ */

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 	機能			:		api_simple_ioctl
//	引数 			:		unsigned int ioctl command
//	戻り値			:		int(0-OK)
//	機能説明		: 		パラメタ無しＩＯＣＴＬの機能です。
static inline int api_simple_ioctl ( unsigned int cmd )
{
	int ret;

	if(g_volmoni_fd == 0)						/* ファイルディスクリプタ未取得の場合 */
	{
		g_volmoni_fd =open(CATSDRV_PATH,O_RDWR);	/* 取得 */
		if( g_volmoni_fd < 0 )						/* 取得失敗した */
		{
			return API_NG;
		}
	}

	ret = ioctl(g_volmoni_fd,cmd);

	if(ret<0){
		if(-errno==CATS_ERR_BUSY) {
			return API_BUSY;
		}

		return API_NG;
	}

	return API_OK;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 	機能			:		api_arg_ioctl
//	引数 			:		unsigned int 	- 	コマンド
//							int * 			-	コマンドパラメタ
//	戻り値			:		int(0-OK)
//	機能説明		: 		パラメタ在りＩＯＣＴＬの機能です。
static inline int api_arg_ioctl ( unsigned int cmd, int *arg )
{
	int ret;

	if(g_volmoni_fd == 0)						/* ファイルディスクリプタ未取得の場合 */
	{
		g_volmoni_fd =open(CATSDRV_PATH,O_RDWR);	/* 取得 */
		if( g_volmoni_fd < 0 )						/* 取得失敗した */
		{
			return API_NG;
		}
	}

	ret = ioctl(g_volmoni_fd, cmd, arg);

	if(ret<0){
		if(-errno==CATS_ERR_BUSY) {
			return API_BUSY;
		}

		return API_NG;
	}

	return API_OK;
}


//------------------------------------------------------------------------------
// 	機能			:		FncPowerMoniIntEnable
//	引数 			:		unsigned char 	-
//	戻り値			:		int(0-OK)
//	機能説明		:		電源電圧監視割り込みを無効/有効にする機能です。
int FncPowerMoniIntEnable ( unsigned char sts )
{
	int ioctl_arg = (int) sts;

	//check for invalid arguments
	if((ioctl_arg>1)||(ioctl_arg<0)) return API_NG;

	return api_arg_ioctl(IOCTL_CATS_FncPowerMoniIntEnable, &ioctl_arg);
}


//------------------------------------------------------------------------------
// 	機能			:		FncPowerMoniIntSts
//	引数 			:		unsigned int* 	-interrupt status
//	戻り値			:		int(0-OK)
//	機能説明		:		電源電圧監視割り込みステータスの読み込み機能です。
int FncPowerMoniIntSts ( unsigned int *sts )
{
	return api_arg_ioctl(IOCTL_CATS_FncPowerMoniIntSts,sts);
}


//------------------------------------------------------------------------------
// 	機能			:		FncPowerMoniIntClear
//	引数 			:		無し
//	戻り値			:		int(0-OK)
//	機能説明		:		電源電圧監視割り込みステータスのクリア機能です。
int FncPowerMoniIntClear ( void )
{
	return api_simple_ioctl(IOCTL_CATS_FncPowerMoniIntClear);
}


//------------------------------------------------------------------------------
// 	機能			:		FncAccMoniIntEnable
//	引数 			:		unsigned char　enable/disable
//	戻り値			:		int(0-OK)
//	機能説明		:		アクセサリー電圧監視割り込みの無効/有効にする機能です。
int FncAccMoniIntEnable ( unsigned char sts )
{
	int ioctl_arg = (int) sts;

	//check for invalid arguments
	if((ioctl_arg>1)||(ioctl_arg<0)) return API_NG;

	return api_arg_ioctl(IOCTL_CATS_FncAccMoniIntEnable, &ioctl_arg);
}


//------------------------------------------------------------------------------
// 	機能			:		FncAccMoniIntSts
//	引数 			:		unsigned int* 	-interrupt status
//	戻り値			:		int(0-OK)
//	機能説明		:		アクセサリー電圧監視割り込みステータスの読み込み機能です。
int FncAccMoniIntSts ( unsigned int *sts )
{
	return api_arg_ioctl(IOCTL_CATS_FncAccMoniIntSts,sts);
}


//------------------------------------------------------------------------------
// 	機能			:		FncAccMoniIntClear
//	引数 			:		無し
//	戻り値			:		int(0-OK)
//	機能説明		:		アクセサリー電圧監視割り込みステータスのクリア機能です。
int FncAccMoniIntClear ( void )
{
	return api_simple_ioctl(IOCTL_CATS_FncAccMoniIntClear);
}


/***********************************************************************************
* MODULE         : Volmoni_SigHandler
* ABSTRACT       : 電圧監視 シグナルハンドラ
* FUNCTION       : 
* ARGUMENT       : 
* NOTE           : 
* RETURN         : 正常終了で「０」を返す。
* CREATE         : 2009/12/ 8  新規作成 
* UPDATE         : 2009/12/10
***********************************************************************************/
void Volmoni_SigHandler(int signo)
{
	int ret;
	int ioctl_arg, tmp;

//	printf("Volmoni_SigHandler\n");

	/* 電圧監視割り込みチェック */
	ret = ioctl(g_volmoni_fd, IOCTL_CATS_CallBackPowAccGetIrq, &ioctl_arg);
//	printf("IOCTL_CATS_CallBackPowAccGetIrq = %x ret = %d\n",ioctl_arg ,ret );

	tmp = ioctl_arg;

	if(( tmp & POWERMONI_INT ) != 0 ){
//		printf("POW OFF Det!!\n");

		/* 割り込み要求解除 */
		ioctl_arg = POWERMONI_INT;
		ret = ioctl(g_volmoni_fd, IOCTL_CATS_CallBackPowAccClrIrq, &ioctl_arg);
//		printf("IOCTL_CATS_CallBackPowAccClrIrq = %x ret = %d\n",ioctl_arg ,ret );

		/* コールバック関数登録チェック */
		if(( *volmoni_table.powermoni_callback_fnc ) != NULL ){
			(*volmoni_table.powermoni_callback_fnc)();
		}
	}

	if(( tmp & ACCMONI_INT ) != 0 ){
//		printf("ACC OFF Det!!\n");

		/* 割り込み要求解除 */
		ioctl_arg = ACCMONI_INT;
		ret = ioctl(g_volmoni_fd, IOCTL_CATS_CallBackPowAccClrIrq, &ioctl_arg);
//		printf("IOCTL_CATS_CallBackPowAccClrIrq = %x ret = %d\n",ioctl_arg ,ret );

		/* コールバック関数登録チェック */
		if(( *volmoni_table.accmoni_callback_fnc ) != NULL ){
			(*volmoni_table.accmoni_callback_fnc)();
		}
	}
}


/***********************************************************************************
* MODULE         : FncVolMoniHandlerSet
* ABSTRACT       : 電圧監視割り込みのコールバック関数を登録する。
* FUNCTION       : 
* ARGUMENT       : 
* NOTE           : kind  :callback type
*                : *func :callback function (pointer)
* RETURN         : 正常終了で「０」を返す。
* CREATE         : 2009/12/ 8  新規作成 
* UPDATE         : 2009/12/10
***********************************************************************************/
int FncVolMoniHandlerSet ( unsigned char kind, void * func )
{
	int ret;
	int oflags;
	struct sigaction act;

	memset(&act ,0 ,sizeof( struct sigaction ));

	if(func == NULL)
	{
		return API_NG;
	}

	if(g_volmoni_fd == 0)							/* ファイルディスクリプタ未取得の場合 */
	{
		g_volmoni_fd =open(CATSDRV_PATH,O_RDWR);	/* 取得 */
		if( g_volmoni_fd < 0 )						/* 取得失敗した */
		{
			return API_NG;
		}
	}

	/* シグナルハンドラ登録 */
	if(volmoni_table.fasync_use == 0)
	{
		sigaction(SIGIO, NULL, &act);				/* 設定前の取得 */
		act.sa_handler = Volmoni_SigHandler;		/* ハンドラの設定 */
		sigemptyset(&act.sa_mask);					/* sa_mask の初期化 */
		sigaddset(&act.sa_mask, SIGIO);				/* シグナルハンドラ実行中はブロック */
		act.sa_flags = 0;							/* sa_flags はデフォルト */
		act.sa_restorer = NULL;						/* sa_restorer は未使用 */
		sigaction(SIGIO, &act, NULL);				/* シグナルハンドラ登録 */

		fcntl(g_volmoni_fd, F_SETOWN, getpid());
		oflags = fcntl(g_volmoni_fd, F_GETFL);
		fcntl(g_volmoni_fd, F_SETFL, oflags | FASYNC);
		volmoni_table.fasync_use = 1;
	}


	if( kind == CALLBACK_POWER)
	{
		volmoni_table.powermoni_callback_fnc = func;
		ret = API_OK;

	}else if( kind == CALLBACK_ACC)
	{
		volmoni_table.accmoni_callback_fnc = func;
		ret = API_OK;

	}else{
		ret = API_NG;
	}


	return ret;
}


/***********************************************************************************
* MODULE         : FncVolMoniHandlerClear
* ABSTRACT       : 電圧監視割り込みのコールバック関数を解除する。
* FUNCTION       : 
* ARGUMENT       : 
* NOTE           : kind  :callback type
* RETURN         : 正常終了で「０」を返す。
* CREATE         : 2009/12/ 8  新規作成 
* UPDATE         : 2009/12/ 9
***********************************************************************************/
int FncVolMoniHandlerClear ( unsigned char kind )
{
	int ret;

	if( kind == CALLBACK_POWER)
	{
		volmoni_table.powermoni_callback_fnc = NULL;
		ret = API_OK;

	}else if( kind == CALLBACK_ACC)
	{
		volmoni_table.accmoni_callback_fnc = NULL;
		ret = API_OK;

	}else{
		ret = API_NG;
	}

	return ret;
}

