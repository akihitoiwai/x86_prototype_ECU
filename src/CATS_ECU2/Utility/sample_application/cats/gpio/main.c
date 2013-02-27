/**********************************************************************************
 * ファイル名      : main.c
 * システム名      : x86 プロトタイプＥＣＵ OPE-RA Ver2.0
 * サブシステム名  : GPIO サンプル
 * プログラム名    : 
 * バージョン      : Ver1.01
 * --------------------------------------------------------------------------------
 * 作成者          : 
 * 作成部署        : 
 * 作成日付        : 2009年10月23日 新規作成
 * 更新履歴        : 2010年06月10日
 **********************************************************************************/

#include <stdio.h>

#include "/usr/src/CATS_ECU2/api_library/gpioapi/gpio_api.h"

#define MASK16BIT 0xFFFF
int count1;

/*******************************************************************
機能 	: 	GPIO CALLBACK
引数		:	無し
戻り値	:	無し
機能説明	:	ＧＰＩＯコールバック機能
*******************************************************************/
void gpio_callback(void)
{
	int ret, tmp;

	/* １６チャネル分のＧＰＩＯ割り込み状態を取得する */
	ret = FncGPIOIntSts2( &tmp );
//	printf("FncGPIOIntSts2 gpio_intsts = %x\n", tmp);
	if(ret == API_NG) return;

	/* GPIO 割り込みチェック */
	if(tmp)
	{
		printf("GPIO CALLBACK count = %d\n", count1++);
//		printf("gpio_intsts = %x\n", tmp);

		/* 割り込み要求解除 */
		ret = FncGPIOIntClear ( tmp );
		printf("FncGPIOIntClear gpio_intsts = %x\n", tmp);
		if(ret == API_NG) return;

		/* １６チャネル分のＧＰＩＯ割り込み状態を取得する */
		ret = FncGPIOIntSts2( &tmp );
		printf("FncGPIOIntSts2 gpio_intsts = %x\n", tmp);
		if(ret == API_NG) return;

		printf("\n");
	}
}


/*******************************************************************
機能 	: 	main
引数		:	無し
戻り値	:	無し
機能説明	:	
*******************************************************************/
int main(void)
{
	unsigned int ret,tmp,atmp[16],atmp2[16],i;
	unsigned int count;
	unsigned char c;

#if 0
/********************************************************************/
/*							GPIO test1								*/
/********************************************************************/
	ret = FncInitGPIOOutput();						/* GPIO 出力初期化 */
	printf("FncInitGPIOOutput ret = %d\n",ret);

	ret = FncInitGPIOInput();						/* GPIO 入力初期化 */
	printf("FncinitGPIOInput ret = %d\n",ret);

	tmp = 0xA5A5A5;
	ret = FncGPIOSetAll( tmp );						/* 24 bit GPIO OUT */
	printf("FncGPIOSetAll ret = %d\n", ret);

	ret = FncGPIOGetAll( &tmp );					/* 16 bit GPIO IN */
	printf("FncGPIOGetAll ret = %d val = %x\n", ret, tmp);

	ret = FncGPIOIntSts2( &tmp );					/* 16 bit GPIO INT STS READ */
	printf("FncGPIOIntSts2 ret = %d val = %x\n", ret, tmp);

	printf("[blocking scanf]\n"); scanf("%c",&c);
#endif


#if 0
/********************************************************************/
/*						Setup interrupt								*/
/********************************************************************/
	printf("[blocking scanf]\n"); scanf("%c",&c);
	printf("Setup interrupt\n");

	ret = FncInitGPIOOutput();						/* GPIO 出力初期化 */
	printf("FncInitGPIOOutput ret = %d\n",ret);

	ret = FncInitGPIOInput();						/* GPIO 入力初期化 */
	printf("FncinitGPIOInput ret = %d\n",ret);

	atmp[0]  = 0x0F;								/* bit 0 - 3 割り込み有効 */
	atmp[1]  = 0x0F;								/* bit 0 - 3 Hi レベルで割り込み発生 */

	ret = FncGPIOIntEnable2 ( atmp[0], atmp[1] );	/* 割り込み有効無効指定 */
	printf("FncGPIOIntEnable2 ret = %d\n",ret);

	printf("[blocking scanf]\n"); scanf("%c",&c);
#endif


#if 1
/********************************************************************/
/*			GPIO 割り込み有効／コールバック有効						*/
/********************************************************************/
	printf("GPIO callback API Test \nPID=%d\n\n",getpid());


	ret = FncInitGPIOOutput();						/* GPIO 出力初期化 */
	printf("FncInitGPIOOutput ret = %d\n",ret);

	ret = FncInitGPIOInput();						/* GPIO 入力初期化 */
	printf("FncinitGPIOInput ret = %d\n",ret);


	ret = FncGpioHandlerSet( &gpio_callback );		/* 割り込みハンドラ登録 */
	printf("FncGpioHandlerSet ret = %d\n",ret);


	atmp[0]  = 0xFFFF;								/* bit 0 - 15 割り込み有効 */
	atmp[1]  = 0xFFFF;								/* bit 0 - 15 Hiレベルで割り込み発生 */

	ret = FncGPIOIntEnable2 ( atmp[0], atmp[1] );	/* 割り込み有効無効指定 */
	printf("FncGPIOIntEnable2 ret = %d\n",ret);


	tmp = 0x000001;
	ret = FncGPIOSetAll( tmp );						/* GPIO OUT */
	printf("FncGPIOSetAll ret = %d\n", ret);


	printf("GPIO INT,CALL Back Enable!! \n");
	count = 1;
	while(1){
		usleep(1000000);
		printf("sample count = %d\n",count);
		count++;
		if (count > 5)
			break;
	}
#endif

#if 1
/********************************************************************/
/*			GPIO 割り込み有効／コールバック無効						*/
/********************************************************************/
	ret = FncGpioHandlerClear();
	printf("FncGpioHandlerClear() ret = %d\n",ret);
	if(ret == API_NG) return 0;

	printf("CALL Back Disable!! \n");
	count = 1;
	while(1){
		usleep(1000000);
		printf("sample count = %d\n",count);
		count++;
		if (count > 5)
			break;
	}
#endif

#if 1
/********************************************************************/
/*			GPIO 割り込み無効／コールバック無効						*/
/********************************************************************/
	atmp[0]  = 0x0;									/* bit 0 - 15 割り込み無効 */
	atmp[1]  = 0xFFFF;								/* bit 0 - 15 Hiレベルで割り込み発生 */

	ret = FncGPIOIntEnable2 ( atmp[0], atmp[1] );	/* 割り込み有効無効指定 */
	printf("FncGPIOIntEnable2 ret = %d\n",ret);

	printf("GPIO INT Disable!! \n");
	count = 1;
	while(1){
		usleep(1000000);
		printf("sample count = %d\n",count);
		count++;
		if (count > 5)
			break;
	}
#endif
	return 0;
}
