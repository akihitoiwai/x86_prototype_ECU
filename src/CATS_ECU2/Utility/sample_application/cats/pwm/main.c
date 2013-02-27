/*******************************************************************************

	main.c

	sample application / basic usage of the APIs


*******************************************************************************/

#include <stdio.h>

#include "/usr/src/CATS_ECU2/api_library/pwmapi/pwm_api.h"



/*******************************************************************
機能 			: 	main
引数			:	無し
戻り値			:	無し
機能説明		:	
*******************************************************************/
int main(void)
{
	unsigned int ret,tmp,atmp[16],atmp2[16],i;
	unsigned char c;

	printf("PWM TEST Start\n");


/********************************************************************/
/*	■仕様															*/
/*	PWM の分解能を「1ms」											*/
/*	パルス周期を「100ms」											*/
/*	Hi 時間を「デューティー比 50%」									*/
/*  																*/
/*  クロック源とは PCI-BUS Clock 33.3333Mhz ÷ 32 = 1.04Mhz			*/
/*  であるため、誤差が生じる。										*/
/********************************************************************/


#if 0
/********************************************************************/
/*	●パラメータ①													*/
/*	クロック分周比を「0xA(1/1024)」に設定。							*/
/*	パルス周期を「0x64(100)」に設定。								*/
/*	クロック分周誤差の為、周期が98ms となります。					*/
/********************************************************************/
	atmp[0] = 0xAAAAAAAA;	/* クロック分周比(PWM0～PWM7) */
								/* PWM0 = 0xA 1/1024 */
								/* PWM1 = 0xA 1/1024 */
								/* PWM2 = 0xA 1/1024 */
								/* PWM3 = 0xA 1/1024 */
								/* PWM4 = 0xA 1/1024 */
								/* PWM5 = 0xA 1/1024 */
								/* PWM6 = 0xA 1/1024 */
								/* PWM7 = 0xA 1/1024 */

	atmp[1] = 0x64646464;	/* 出力パルス周期(PWM0～PWM3) */
								/* PWM0 = 0x64 100 */
								/* PWM1 = 0x64 100 */
								/* PWM2 = 0x64 100 */
								/* PWM3 = 0x64 100 */

	atmp[2] = 0x64646464;	/* 出力パルス周期(PWM4～PWM7) */
								/* PWM4 = 0x64 100 */
								/* PWM5 = 0x64 100 */
								/* PWM6 = 0x64 100 */
								/* PWM7 = 0x64 100 */

#else
/********************************************************************/
/*	●パラメータ②													*/
/*	クロック分周比を「0xA(1/1024)」に設定。							*/
/*	パルス周期を「0x66(102)」に設定。								*/
/*	クロック分周誤差を考慮した結果、周期が100ms となります。		*/
/********************************************************************/
	atmp[0] = 0xAAAAAAAA;	/* クロック分周比(PWM0～PWM7) */
								/* PWM0 = 0xA 1/1024 */
								/* PWM1 = 0xA 1/1024 */
								/* PWM2 = 0xA 1/1024 */
								/* PWM3 = 0xA 1/1024 */
								/* PWM4 = 0xA 1/1024 */
								/* PWM5 = 0xA 1/1024 */
								/* PWM6 = 0xA 1/1024 */
								/* PWM7 = 0xA 1/1024 */

	atmp[1] = 0x66666666;	/* 出力パルス周期(PWM0～PWM3) */
								/* PWM0 = 0x66 102 */
								/* PWM1 = 0x66 102 */
								/* PWM2 = 0x66 102 */
								/* PWM3 = 0x66 102 */

	atmp[2] = 0x66666666;	/* 出力パルス周期(PWM4～PWM7) */
								/* PWM4 = 0x66 102 */
								/* PWM5 = 0x66 102 */
								/* PWM6 = 0x66 102 */
								/* PWM7 = 0x66 102 */
#endif

	ret = FncInitPWM ( atmp );
	printf("FncInitPWM ret = %d\n",ret);

	ret = FncPWMSet ( 1, 10 );	/* 10% (Hi期間) */
	ret = FncPWMSet ( 2, 20 );	/* 20% (Hi期間) */
	ret = FncPWMSet ( 3, 30 );	/* 30% (Hi期間) */
	ret = FncPWMSet ( 4, 40 );	/* 40% (Hi期間) */

	ret = FncPWMSet ( 5, 50 );	/* 50% (Hi期間) */
	ret = FncPWMSet ( 6, 90 );	/* 90% (Hi期間) */
	ret = FncPWMSet ( 7, 100 );	/* 100% (Hi期間) */
	ret = FncPWMSet ( 8, 102 );	/* 102% (Hi期間)クロック分周誤差を考慮 */

	printf("FncPWMSet ret = %d\n",ret);

	atmp[0] = 0x1; 				/* PWM7 Enable */
	atmp[1] = 0x1; 				/* PWM6 Enable */
	atmp[2] = 0x1; 				/* PWM5 Enable */
	atmp[3] = 0x1; 				/* PWM4 Enable */
	atmp[4] = 0x1; 				/* PWM3 Enable */
	atmp[5] = 0x1; 				/* PWM2 Enable */
	atmp[6] = 0x1; 				/* PWM1 Enable */
	atmp[7] = 0x1; 				/* PWM0 Enable */
	ret = FncPWMEnable ( atmp );
	printf("FncPWMEnable ret = %d\n",ret);


	scanf("%c",&c);

	atmp[0] = 0x0;				/* PWM7 Disable */
	atmp[1] = 0x0;				/* PWM6 Disable */
	atmp[2] = 0x0;				/* PWM5 Disable */
	atmp[3] = 0x0;				/* PWM4 Disable */
	atmp[4] = 0x0;				/* PWM3 Disable */
	atmp[5] = 0x0;				/* PWM2 Disable */
	atmp[6] = 0x0;				/* PWM1 Disable */
	atmp[7] = 0x0;				/* PWM0 Disable */
	ret = FncPWMEnable ( atmp );
	printf("FncPWMEnable ret = %d\n",ret);

	printf("PWM TEST End\n");

	return 0;
}
