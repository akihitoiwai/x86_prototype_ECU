/**********************************************************************************
 * ファイル名      : main.c
 * システム名      : x86 プロトタイプＥＣＵ OPE-RA Ver2.0
 * サブシステム名  : 電圧監視機能サンプル
 * プログラム名    : 
 * バージョン      : Ver1.00
 * モジュール構成  : powacc_api			電圧監視機能API
 * --------------------------------------------------------------------------------
 * 作成者          : 
 * 作成部署        : 
 * 作成日付        : 2009年12月 8日 新規作成
 * 更新履歴        : 
 **********************************************************************************/
#include <stdio.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>


#include "/usr/src/CATS_ECU2/api_library/powaccapi/powacc_api.h"

#define CATSDRV_PATH "/dev/catsdrv7"

int count1,count2;


/*******************************************************************
機能 		: 	POW CALLBACK
引数		:	無し
戻り値		:	無し
機能説明	:	ユーザコールバック関数
*******************************************************************/
void powmoni_callback(void)
{
	int tmp,ret;

	printf("pow_callback count = %d\n",count1);
	count1++;

	/* 電源電圧監視割り込みステータスの読み込み　*/
	ret = FncPowerMoniIntSts( &tmp );
	printf("FncPowerMoniIntSts ret = %d val = %d\n",ret,tmp);
	if(ret == API_NG) return;

	/* 電源電圧監視割り込みステータスのクリア */
	ret = FncPowerMoniIntClear();
	printf("FncPowerMoniIntClear ret = %d\n",ret);
	if(ret == API_NG) return;

}


/*******************************************************************
機能 		: 	ACC CALLBACK
引数		:	無し
戻り値		:	無し
機能説明	:	ユーザコールバック関数
*******************************************************************/
void accmoni_callback(void)
{
	int tmp,ret;

	printf("acc_callback count = %d\n",count2);
	count2++;

	/* アクセサリー電圧監視割り込みのステータス読み込み　*/
	ret = FncAccMoniIntSts( &tmp );
	printf("FncAccMoniIntSts ret = %d val = %d\n",ret,tmp);
	if(ret == API_NG) return;

	/* アクセサリー電圧監視割り込みのクリア */
	ret = FncAccMoniIntClear();
	printf("FncAccMoniIntClear ret = %d\n",ret);
	if(ret == API_NG) return;

}


/*******************************************************************
機能 		: 	main
引数		:	無し
戻り値		:	無し
機能説明	:	
*******************************************************************/
int main(void)
{
	int tmp,ret;
	unsigned int count;

	count1 = 1;
	count2 = 1;

/********************************************************************/
/*			電圧監視割り込み有効／コールバック有効					*/
/********************************************************************/
	printf("Power,ACC Moniter API Test \nPID=%d\n\n",getpid());

#if 1
	ret = FncVolMoniHandlerSet( CALLBACK_POWER, &powmoni_callback );
	printf("FncVolMoniHandlerSet( CALLBACK_POWER ) ret = %d\n",ret);
	if(ret == API_NG) return 0;

	ret = FncPowerMoniIntEnable( 1 );
	printf("FncPowerMoniIntEnable(1) ret = %d\n",ret);
	if(ret == API_NG) return 0;

	ret = FncPowerMoniIntSts( &tmp );
	printf("FncPowerMoniIntSts ret = %d val = %d\n",ret,tmp);
	if(ret == API_NG) return 0;
#endif

#if 1
	ret = FncVolMoniHandlerSet( CALLBACK_ACC, &accmoni_callback );
	printf("FncVolMoniHandlerSet( CALLBACK_ACC ) ret = %d\n",ret);
	if(ret == API_NG) return 0;

	ret = FncAccMoniIntEnable( 1 );
	printf("FncAccMoniIntEnable(1) ret = %d\n",ret);
	if(ret == API_NG) return 0;

	ret = FncAccMoniIntSts( &tmp );
	printf("FncAccMoniIntSts ret = %d val = %d\n",ret,tmp);
	if(ret == API_NG) return 0;
#endif

	printf("VoltageMoniter INT Disable!! \n");
	printf("CALL Back Enable!! \n");
	count = 1;
	while(1){
		usleep(1000000);
		printf("sample count = %d\n",count);
		count++;
		if (count > 10)
			break;
	}

/********************************************************************/
/*			電圧監視割り込み有効／コールバック無効					*/
/********************************************************************/
#if 1
	ret = FncVolMoniHandlerClear( CALLBACK_POWER );
	printf("FncVolMoniHandlerClear( CALLBACK_POWER ) ret = %d\n",ret);
	if(ret == API_NG) return 0;
#endif

#if 1
	ret = FncVolMoniHandlerClear( CALLBACK_ACC );
	printf("FncVolMoniHandlerClear( CALLBACK_ACC ) ret = %d\n",ret);
	if(ret == API_NG) return 0;
#endif

	printf("CALL Back Disable!! \n");
	count = 1;
	while(1){
		usleep(1000000);
		printf("sample count = %d\n",count);
		count++;
		if (count > 10)
			break;
	}

/********************************************************************/
/*			電圧監視割り込み無効／コールバック無効					*/
/********************************************************************/
#if 1
	ret = FncPowerMoniIntEnable( 0 );
	printf("FncPowerMoniIntEnable(0) ret = %d\n",ret);	if(ret == API_NG) return 0;

	ret = FncPowerMoniIntSts( &tmp );
	printf("FncPowerMoniIntSts ret = %d val = %d\n",ret,tmp);
	if(ret == API_NG) return 0;
#endif

#if 1
	ret = FncAccMoniIntEnable( 0 );
	printf("FncAccMoniIntEnable(0) ret = %d\n",ret);
	if(ret == API_NG) return 0;

	ret = FncAccMoniIntSts( &tmp );
	printf("FncAccMoniIntSts ret = %d val = %d\n",ret,tmp);
	if(ret == API_NG) return 0;
#endif

	printf("VoltageMoniter INT Disable!! \n");
	count = 1;
	while(1){
		usleep(1000000);
		printf("sample count = %d\n",count);
		count++;
		if (count > 5)
			break;
	}

	return 0;
}

