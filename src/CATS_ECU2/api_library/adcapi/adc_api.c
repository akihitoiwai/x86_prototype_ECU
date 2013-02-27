/**********************************************************************************
 * ファイル名      : adc_api.c
 * システム名      : x86 プロトタイプＥＣＵ OPE-RA Ver2.0
 * サブシステム名  : ADC API
 * プログラム名    : 
 * CPU TYPE        : 
 * バージョン      : Ver1.00
 * モジュール構成  : adc_api.h			API Header
 * --------------------------------------------------------------------------------
 * 作成者          : 
 * 作成部署        : 
 * 作成日付        : 2009年08月24日 新規作成
 * 更新履歴        : 2009年10月19日
 **********************************************************************************/
#include <stdio.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>

#include "adc_api.h"
#include "/usr/src/CATS_ECU2/kernel_driver/cats_ioctl.h"

#define CATSDRV_PATH "/dev/catsdrv3"


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 	機能			:		api_simple_ioctl
//	引数 			:		unsigned int ioctl command
//	戻り値			:		int(0-OK)
//	機能説明		: 		パラメタ無しＩＯＣＴＬの機能です。
static inline int api_simple_ioctl ( unsigned int cmd )
{
	int ret;
	int catsdrv_fd;

	catsdrv_fd=open(CATSDRV_PATH,O_RDWR);
	if(catsdrv_fd<0){
		return API_NG;
	}

	ret = ioctl(catsdrv_fd,cmd);
	close(catsdrv_fd);

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
	int catsdrv_fd;

	catsdrv_fd=open(CATSDRV_PATH,O_RDWR);
	if(catsdrv_fd<0){
		return API_NG;
	}

	ret = ioctl(catsdrv_fd, cmd, arg);
	close(catsdrv_fd);

	if(ret<0){
		if(-errno==CATS_ERR_BUSY) {
			return API_BUSY;
		}

		return API_NG;
	}

	return API_OK;
}


//------------------------------------------------------------------------------
//	機能			: 		FncInitADC
//	引数 			:		無し
//	戻り値			:		int(0-OK)
//	機能説明		: 		ドライバのＡＤＣ初期機能です。

int FncInitADC ( void )
{
	return api_simple_ioctl(IOCTL_CATS_FncInitADC);
}

//------------------------------------------------------------------------------
// 	機能			:		FncADCStartOneShot
//	引数 			:		unsigned int - チャネル
//	戻り値			:		int(0-OK)
//	機能説明		: 		ドライバのチャネルのワンショット始機能です。
int FncADCStartOneShot ( unsigned int ch )
{
	int ioctl_arg;

	//check for invalid arguments
	if((ch>16)||(ch<1)) return API_NG;

	ioctl_arg = ch - 1;

	return api_arg_ioctl(IOCTL_CATS_FncADCStartOneShot, &ioctl_arg);
}

//------------------------------------------------------------------------------
// 	機能			:		FncADCStartContinuous
//	引数 			:		無し
//	戻り値			:		int(0-OK)
//	機能説明		: 		ドライバの連続モード始機能です。
int FncADCStartContinuous ( void )
{
	return api_simple_ioctl(IOCTL_CATS_FncADCStartContinuous);
}

//------------------------------------------------------------------------------
// 	機能			:		FncADCStopContinuous
//	引数 			:		無し
//	戻り値			:		int(0-OK)
//	機能説明		: 		ドライバの連続モード停止機能です。
int FncADCStopContinuous ( void )
{
	return api_simple_ioctl(IOCTL_CATS_FncADCStopContinuous);
}

//------------------------------------------------------------------------------
// 	機能			:		FncADCGetOneShot
//	引数 			:		unsigned int 	- チャネル
//							unsigned int* value
//	戻り値			:		int(0-OK)
//	機能説明		: 		ワンショットのデータ読み込み機能です。
int FncADCGetOneShot ( unsigned int ch, unsigned int *val )
{
	int ret;
	int ioctl_arg;

	//check for invalid arguments
	if((ch>16)||(ch<1)) return API_NG;

	ioctl_arg = ch - 1;
	ret = api_arg_ioctl(IOCTL_CATS_FncADCGetOneShot, &ioctl_arg);

	*val = (unsigned int) ioctl_arg;

	// check for invalid values
	if(ret==API_OK)
		if((ioctl_arg>65535)||(ioctl_arg<0)) return API_NG;

	return ret;
}

//------------------------------------------------------------------------------
// 	機能			:		FncADCGetContinuous
//	引数 			:		unsigned int *value(array)
//	戻り値			:		int(0-OK)
//	機能説明		: 		連続モードのデータ読み込み機能です。
int FncADCGetContinuous ( unsigned int *val )
{
	int ret, ret_f;
	int catsdrv_fd;
	int ioctl_arg;
	int ch;

	catsdrv_fd=open(CATSDRV_PATH,O_RDWR);
	if(catsdrv_fd<0){
		return API_NG;
	}

	ret_f = API_OK;

	// current implementation of getcontinuous is getoneshot for all channels
	for(ch=0;ch<16;ch++){
		ioctl_arg = ch;
		ret = ioctl(catsdrv_fd,IOCTL_CATS_FncADCGetContinuous, &ioctl_arg);
		if(ret<0){
			if((-errno==CATS_ERR_BUSY)&&(ret_f!=API_NG)){
				ret_f = API_BUSY;
			}else{
				ret_f = API_NG;
			}
		}
		val[ch] = (int) ioctl_arg;
	}

	close(catsdrv_fd);

	// check for invalid values
	for(ch=0;ch<16;ch++){
		if((val[ch]>65535)||(val[ch]<0)) return API_NG;
	}

	return ret_f;
}

//------------------------------------------------------------------------------
// 	機能			:		FncDisableADC
//	引数 			:		無し
//	戻り値			:		int(0-OK)
//	機能説明		: 		ＡＤＣを無効にする機能です。
int FncDisableADC ( void )
{
	return api_simple_ioctl(IOCTL_CATS_FncDisableADC);
}


