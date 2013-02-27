/**********************************************************************************
 * ファイル名      : pwm_api.c
 * システム名      : x86 プロトタイプＥＣＵ OPE-RA Ver2.0
 * サブシステム名  : PWM API
 * プログラム名    : 
 * CPU TYPE        : 
 * バージョン      : Ver1.00
 * モジュール構成  : pwm_api.h			API Header
 * --------------------------------------------------------------------------------
 * 作成者          : 
 * 作成部署        : 
 * 作成日付        : 2009年10月30日 新規作成
 * 更新履歴        : 
 **********************************************************************************/
#include <stdio.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>

#include "pwm_api.h"
#include "/usr/src/CATS_ECU2/kernel_driver/cats_ioctl.h"

#define CATSDRV_PATH "/dev/catsdrv5"


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
// 	機能			:		FncInitPWM
//	引数 			:		unsigned int *(pwm initialization data)
//	戻り値			:		int(0-OK)
//	機能説明		: 		ドライバのＰＷＭ初期機能です。
int FncInitPWM ( unsigned int *val )
{
	int ret;
	int catsdrv_fd;

	catsdrv_fd=open(CATSDRV_PATH,O_RDWR);
	if(catsdrv_fd<0){
		return API_NG;
	}
													/* PWM を初期化 */
	ret = ioctl(catsdrv_fd, IOCTL_CATS_FncInitPWM,&val);
	close(catsdrv_fd);

	if(ret<0){

		return API_NG;
	}

	return API_OK;
}

//------------------------------------------------------------------------------
// 	機能			:		FncPWMEnable
//	引数 			:		unsigned int *(enable/disable array)
//	戻り値			:		int(0-OK)
//	機能説明		: 		ＰＷＭの有効/無効にするを設定する機能です。
int FncPWMEnable ( unsigned int *val )
{
	int ioctl_arg = 0;
	int i;

	for (i=0;i<8;i++) {
		//check for invalid arguments
		if((val[i]>1)||(val[i]<0)) return API_NG;
        ioctl_arg = ioctl_arg | (val[i]<<(7-i));
	}

	return api_arg_ioctl(IOCTL_CATS_FncPWMEnable,&ioctl_arg);
}

//------------------------------------------------------------------------------
// 	機能			:		FncPWMSet
//	引数 			:		unsigned int - チャネル
//							unsigned int - duty cycle
//	戻り値			:		int(0-OK)
//	機能説明		: 		ＰＷＭチャネルのデューティサイクルを設定する機能です。
int FncPWMSet ( unsigned int ch, unsigned int val )
{
	int ret;
	int catsdrv_fd;
	int ioctl_arg[2] = {0,0};
	int *ioctl_ptr;

	ioctl_ptr = &ioctl_arg[0];

	//check for invalid arguments
	if((ch>8)||(ch<1)) return API_NG;
	if((val>0xff)||(val<0)) return API_NG;

	ioctl_arg[0] = (ch -1);
	ioctl_arg[1] = val;

	catsdrv_fd=open(CATSDRV_PATH,O_RDWR);
	if(catsdrv_fd<0){
		return API_NG;
	}

	ret = ioctl(catsdrv_fd, IOCTL_CATS_FncPWMSet,&ioctl_ptr);
	close(catsdrv_fd);

	if(ret<0){

		return API_NG;
	}

	return API_OK;
}


