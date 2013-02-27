/**********************************************************************************
 * ファイル名      : main.c
 * システム名      : x86 プロトタイプＥＣＵ OPE-RA Ver2.0
 * サブシステム名  : TEST 機能試験
 * プログラム名    : 
 * バージョン      : Ver1.00
 * --------------------------------------------------------------------------------
 * 作成者          : 
 * 作成部署        : 
 * 作成日付        : 2009年11月12日 新規作成
 * 更新履歴        : 
 **********************************************************************************/

#include <stdio.h>

#include "/usr/src/CATS_ECU2/api_library/testapi/test_api.h"


/*******************************************************************
機能 		: 	main
引数			:	無し
戻り値		:	無し
機能説明		:	
*******************************************************************/
int main(void)
{
	unsigned int ret,tmp;

/********************************************************************/
/*							FncTEST								*/
/********************************************************************/
	printf("FncTEST test1\n");

	ret = FncTESTGetFPGAVer( &tmp );				/* FPGAバージョン取得 */
	printf("FncTESTGetFPGAVer ret = %d val = %x\n", ret, tmp);

	ret = FncTESTReadScratch( &tmp );				/* スクラッチレジスタから読み込み */
	printf("FncTESTReadScratch ret = %d val = %x\n", ret, tmp);

	tmp++;

	ret = FncTESTWriteScratch( tmp );				/* スクラッチレジスタへ書き込み */
	printf("FncTESTWriteScratch ret = %d\n", ret);

	ret = FncTESTReadScratch( &tmp );				/* スクラッチレジスタから読み込み */
	printf("FncTESTReadScratch ret = %d val = %x\n", ret, tmp);

	printf("\n");


	return 0;
}
