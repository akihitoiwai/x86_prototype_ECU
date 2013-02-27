/**********************************************************************************
 * ファイル名      : dac_api.c
 * システム名      : x86 プロトタイプＥＣＵ OPE-RA Ver2.0
 * サブシステム名  : DAC API
 * プログラム名    : 
 * CPU TYPE        : 
 * バージョン      : Ver1.00
 * モジュール構成  : dac_api.h			API Header
 * --------------------------------------------------------------------------------
 * 作成者          : 
 * 作成部署        : 
 * 作成日付        : 2009年10月21日 新規作成
 * 更新履歴        : 
 **********************************************************************************/
#include <stdio.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>

#include "dac_api.h"
#include "/usr/src/CATS_ECU2/kernel_driver/cats_ioctl.h"

#define CATSDRV_PATH "/dev/catsdrv4"


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


//------------------------------------------------------------------------------
// 	機能			:		FncInitDAC
//	引数 			:		無し
//	戻り値			:		int(0-OK)
//	機能説明		: 		ドライバのＤＡＣ初期機能です。
int FncInitDAC ( void )
{
	int ret;
	int catsdrv_fd;
	int ioctl_arg;
	int i;

	ret = api_simple_ioctl(IOCTL_CATS_FncInitDAC);	/* DAC デバイスを初期化 */

	// current implementation of initdac is dacset(zero) for all channels
	if(ret == API_OK){

		catsdrv_fd=open(CATSDRV_PATH,O_RDWR);
		if(catsdrv_fd<0){
			return API_NG;
		}

		for (i=0;i<8;i++) {
			ioctl_arg = (i << 16);

			ret = ioctl(catsdrv_fd,IOCTL_CATS_FncDACSet,&ioctl_arg);
			if(ret<0){
			close(catsdrv_fd);
			return API_NG;
			}
		}
		close(catsdrv_fd);
	}

	return ret;
}

//------------------------------------------------------------------------------
// 	機能			:		FncDACSet
//	引数 			:		unsigned int *(dac values)
//	戻り値			:		int(0-OK)
//	機能説明		: 		ＤＡＣの電圧レベルを設定する機能です。
int FncDACSet ( unsigned int *val )
{
	int ret;
	int catsdrv_fd;
	int ioctl_arg;
	int i;

	//check for invalid arguments
	for (i=0;i<8;i++) {
		if((val[i]>4095)||(val[i]<0)) return API_NG;
	}

	catsdrv_fd=open(CATSDRV_PATH,O_RDWR);
	if(catsdrv_fd<0){
		return API_NG;
	}

	for (i=0;i<8;i++) {
		ioctl_arg = (i << 16) | val[i];

		ret = ioctl(catsdrv_fd,IOCTL_CATS_FncDACSet,&ioctl_arg);
		if(ret<0){
			close(catsdrv_fd);
			return API_NG;
		}
	}
	close(catsdrv_fd);

	return API_OK;
}

//------------------------------------------------------------------------------
// 	機能			:		FncDACStop
//	引数 			:		無し
//	戻り値			:		int(0-OK)
//	機能説明		: 		ＤＡＣの停止機能です。
int FncDACStop ( void )
{
	int ret;
	int catsdrv_fd;
	int ioctl_arg;
	int i;

	catsdrv_fd=open(CATSDRV_PATH,O_RDWR);
	if(catsdrv_fd<0){
		return API_NG;
	}

	// current implementation of stopdac is dacset(zero) for all channels
	for (i=0;i<8;i++) {
		ioctl_arg = (i << 16);

		ret = ioctl(catsdrv_fd,IOCTL_CATS_FncDACSet,&ioctl_arg);
		if(ret<0){
			close(catsdrv_fd);
			return API_NG;
		}
	}
	close(catsdrv_fd);

	return api_simple_ioctl(IOCTL_CATS_FncDACStop);
}

