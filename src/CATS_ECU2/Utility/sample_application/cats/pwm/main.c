/*******************************************************************************

	main.c

	sample application / basic usage of the APIs


*******************************************************************************/

#include <stdio.h>

#include "/usr/src/CATS_ECU2/api_library/pwmapi/pwm_api.h"



/*******************************************************************
�@�\ 			: 	main
����			:	����
�߂�l			:	����
�@�\����		:	
*******************************************************************/
int main(void)
{
	unsigned int ret,tmp,atmp[16],atmp2[16],i;
	unsigned char c;

	printf("PWM TEST Start\n");


/********************************************************************/
/*	���d�l															*/
/*	PWM �̕���\���u1ms�v											*/
/*	�p���X�������u100ms�v											*/
/*	Hi ���Ԃ��u�f���[�e�B�[�� 50%�v									*/
/*  																*/
/*  �N���b�N���Ƃ� PCI-BUS Clock 33.3333Mhz �� 32 = 1.04Mhz			*/
/*  �ł��邽�߁A�덷��������B										*/
/********************************************************************/


#if 0
/********************************************************************/
/*	���p�����[�^�@													*/
/*	�N���b�N��������u0xA(1/1024)�v�ɐݒ�B							*/
/*	�p���X�������u0x64(100)�v�ɐݒ�B								*/
/*	�N���b�N�����덷�ׁ̈A������98ms �ƂȂ�܂��B					*/
/********************************************************************/
	atmp[0] = 0xAAAAAAAA;	/* �N���b�N������(PWM0�`PWM7) */
								/* PWM0 = 0xA 1/1024 */
								/* PWM1 = 0xA 1/1024 */
								/* PWM2 = 0xA 1/1024 */
								/* PWM3 = 0xA 1/1024 */
								/* PWM4 = 0xA 1/1024 */
								/* PWM5 = 0xA 1/1024 */
								/* PWM6 = 0xA 1/1024 */
								/* PWM7 = 0xA 1/1024 */

	atmp[1] = 0x64646464;	/* �o�̓p���X����(PWM0�`PWM3) */
								/* PWM0 = 0x64 100 */
								/* PWM1 = 0x64 100 */
								/* PWM2 = 0x64 100 */
								/* PWM3 = 0x64 100 */

	atmp[2] = 0x64646464;	/* �o�̓p���X����(PWM4�`PWM7) */
								/* PWM4 = 0x64 100 */
								/* PWM5 = 0x64 100 */
								/* PWM6 = 0x64 100 */
								/* PWM7 = 0x64 100 */

#else
/********************************************************************/
/*	���p�����[�^�A													*/
/*	�N���b�N��������u0xA(1/1024)�v�ɐݒ�B							*/
/*	�p���X�������u0x66(102)�v�ɐݒ�B								*/
/*	�N���b�N�����덷���l���������ʁA������100ms �ƂȂ�܂��B		*/
/********************************************************************/
	atmp[0] = 0xAAAAAAAA;	/* �N���b�N������(PWM0�`PWM7) */
								/* PWM0 = 0xA 1/1024 */
								/* PWM1 = 0xA 1/1024 */
								/* PWM2 = 0xA 1/1024 */
								/* PWM3 = 0xA 1/1024 */
								/* PWM4 = 0xA 1/1024 */
								/* PWM5 = 0xA 1/1024 */
								/* PWM6 = 0xA 1/1024 */
								/* PWM7 = 0xA 1/1024 */

	atmp[1] = 0x66666666;	/* �o�̓p���X����(PWM0�`PWM3) */
								/* PWM0 = 0x66 102 */
								/* PWM1 = 0x66 102 */
								/* PWM2 = 0x66 102 */
								/* PWM3 = 0x66 102 */

	atmp[2] = 0x66666666;	/* �o�̓p���X����(PWM4�`PWM7) */
								/* PWM4 = 0x66 102 */
								/* PWM5 = 0x66 102 */
								/* PWM6 = 0x66 102 */
								/* PWM7 = 0x66 102 */
#endif

	ret = FncInitPWM ( atmp );
	printf("FncInitPWM ret = %d\n",ret);

	ret = FncPWMSet ( 1, 10 );	/* 10% (Hi����) */
	ret = FncPWMSet ( 2, 20 );	/* 20% (Hi����) */
	ret = FncPWMSet ( 3, 30 );	/* 30% (Hi����) */
	ret = FncPWMSet ( 4, 40 );	/* 40% (Hi����) */

	ret = FncPWMSet ( 5, 50 );	/* 50% (Hi����) */
	ret = FncPWMSet ( 6, 90 );	/* 90% (Hi����) */
	ret = FncPWMSet ( 7, 100 );	/* 100% (Hi����) */
	ret = FncPWMSet ( 8, 102 );	/* 102% (Hi����)�N���b�N�����덷���l�� */

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
