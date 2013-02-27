/**********************************************************************************
 * ファイル名      : main.c
 * システム名      : x86 プロトタイプＥＣＵ OPE-RA Ver2.0
 * サブシステム名  : ADC サンプル
 * バージョン      : Ver1.00
 * --------------------------------------------------------------------------------
 * 作成者          : 
 * 作成部署        : 
 * 作成日付        : 2009年09月30日 新規作成
 * 更新履歴        : 
 **********************************************************************************/
#include <stdio.h>
#include "/usr/src/CATS_ECU2/api_library/adcapi/adc_api.h"


/***********************************************************************************
* MODULE         : main
* ABSTRACT       : 
* FUNCTION       : 
* ARGUMENT       : 
* NOTE           : Ch1～Ch16を One Shot モードで取得表示する。
*                : Ch1～Ch16を Continue モードで１０回取得表示する
* RETURN         : 
* CREATE         : 2009/09/30 
* UPDATE         : 
***********************************************************************************/
int main(void)
{
	unsigned int ret,tmp,atmp[16],atmp2[16],i,j,ch;
	unsigned char c;


	// ADC One Shot test
	printf("ADC One Shot test Ch1 to Ch16\n");

	ret = FncInitADC();
	printf("FncInitADC ret = %d\n",ret);

	for(ch=1;ch<16;ch++)
	{
		usleep(125000);
		ret = FncADCStartOneShot ( ch );
		if( ret != 0) printf("FncADCStartOneShot ret = %d\n",ret);

		if(ret==API_BUSY) while(FncADCStartOneShot ( ch )==API_BUSY);

		usleep(125000);
		ret = FncADCGetOneShot ( ch, &tmp );
		if(ret==API_BUSY) while(FncADCGetOneShot ( ch, &tmp )==API_BUSY);
		printf("FncADCGetOneShot ch%d val = 0x%x = %fV\n"
				,ch
				,tmp
				,tmp*0.000076295 );

	}
	ret = FncDisableADC();
	printf("FncDisableADC ret = %d\n\n",ret);


	// ADC Continuous test
	printf("ADC Continuous test\n");

	ret = FncInitADC();
	printf("FncInitADC %d\n",ret);

	ret = FncADCStartContinuous();
	printf("FncADCStartContinuous %d\n",ret);

	for(j=1;j<11;j++){
		usleep(250000);
		printf("\nLOOP %d\n",j);

		ret = FncADCGetContinuous (atmp );
		printf("FncADCGetContinuous ret = %d\n",ret);
		for(i=0;i<16;i++) printf("FncADCGetContinuous ch%d val = 0x%x = %fV\n"
				,i
				,atmp[i]
				,atmp[i]*0.000076295 );
	}

	ret = FncADCStopContinuous();
	printf("FncADCStopContinuous %d\n",ret);

	ret = FncDisableADC();
	printf("FncDisableADC %d\n",ret);

	return 0;
}
