/**********************************************************************************
 * ファイル名      : test_api.c
 * システム名      : x86 プロトタイプＥＣＵ OPE-RA Ver2.0
 * サブシステム名  : TEST API
 * プログラム名    : 
 * CPU TYPE        : 
 * バージョン      : Ver1.00
 * モジュール構成  : test_api.h			API Header
 * --------------------------------------------------------------------------------
 * 作成者          : 
 * 作成部署        : 
 * 作成日付        : 2009年11月11日 新規作成
 * 更新履歴        : 
 **********************************************************************************/
#include <stdio.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>

#include "test_api.h"
#include "/usr/src/CATS_ECU2/kernel_driver/cats_ioctl.h"

#define CATSDRV_PATH "/dev/catsdrv9"


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


/***********************************************************************************
* MODULE         : FncTESTReadScratch
* ABSTRACT       : 
* FUNCTION       : 
* ARGUMENT       : 
* NOTE           : スクラッチレジスタの読み込み機能です。
* RETURN         : 
* CREATE         : 2009/11/11  新規作成 
* UPDATE         : 
***********************************************************************************/
int FncTESTReadScratch ( unsigned int *val )
{
	int ret;
	int ioctl_arg;

	ret = api_arg_ioctl(IOCTL_CATS_TestScratchReg_R,&ioctl_arg);

	*val =  ioctl_arg;

	return ret;
}


/***********************************************************************************
* MODULE         : FncTESTWriteScratch
* ABSTRACT       : 
* FUNCTION       : 
* ARGUMENT       : 
* NOTE           : スクラッチレジスタの書き込み機能です。
* RETURN         : 
* CREATE         : 2009/11/11  新規作成 
* UPDATE         : 
***********************************************************************************/
int FncTESTWriteScratch ( unsigned int val )
{
	return api_arg_ioctl(IOCTL_CATS_TestScratchReg_W, &val);
}


/***********************************************************************************
* MODULE         : FncTESTGetFPGAVer
* ABSTRACT       : 
* FUNCTION       : 
* ARGUMENT       : 
* NOTE           : FPGAバージョン取得機能です。
* RETURN         : 
* CREATE         : 2009/11/11  新規作成 
* UPDATE         : 
***********************************************************************************/
int FncTESTGetFPGAVer ( unsigned int *val )
{
	int ret;
	int ioctl_arg;

	ret = api_arg_ioctl(IOCTL_CATS_TestVersionReadReg,&ioctl_arg);

	*val =  ioctl_arg;

	return ret;
}


