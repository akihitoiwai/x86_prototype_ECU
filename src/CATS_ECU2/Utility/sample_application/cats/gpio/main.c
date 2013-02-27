/**********************************************************************************
 * �t�@�C����      : main.c
 * �V�X�e����      : x86 �v���g�^�C�v�d�b�t OPE-RA Ver2.0
 * �T�u�V�X�e����  : GPIO �T���v��
 * �v���O������    : 
 * �o�[�W����      : Ver1.01
 * --------------------------------------------------------------------------------
 * �쐬��          : 
 * �쐬����        : 
 * �쐬���t        : 2009�N10��23�� �V�K�쐬
 * �X�V����        : 2010�N06��10��
 **********************************************************************************/

#include <stdio.h>

#include "/usr/src/CATS_ECU2/api_library/gpioapi/gpio_api.h"

#define MASK16BIT 0xFFFF
int count1;

/*******************************************************************
�@�\ 	: 	GPIO CALLBACK
����		:	����
�߂�l	:	����
�@�\����	:	�f�o�h�n�R�[���o�b�N�@�\
*******************************************************************/
void gpio_callback(void)
{
	int ret, tmp;

	/* �P�U�`���l�����̂f�o�h�n���荞�ݏ�Ԃ��擾���� */
	ret = FncGPIOIntSts2( &tmp );
//	printf("FncGPIOIntSts2 gpio_intsts = %x\n", tmp);
	if(ret == API_NG) return;

	/* GPIO ���荞�݃`�F�b�N */
	if(tmp)
	{
		printf("GPIO CALLBACK count = %d\n", count1++);
//		printf("gpio_intsts = %x\n", tmp);

		/* ���荞�ݗv������ */
		ret = FncGPIOIntClear ( tmp );
		printf("FncGPIOIntClear gpio_intsts = %x\n", tmp);
		if(ret == API_NG) return;

		/* �P�U�`���l�����̂f�o�h�n���荞�ݏ�Ԃ��擾���� */
		ret = FncGPIOIntSts2( &tmp );
		printf("FncGPIOIntSts2 gpio_intsts = %x\n", tmp);
		if(ret == API_NG) return;

		printf("\n");
	}
}


/*******************************************************************
�@�\ 	: 	main
����		:	����
�߂�l	:	����
�@�\����	:	
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
	ret = FncInitGPIOOutput();						/* GPIO �o�͏����� */
	printf("FncInitGPIOOutput ret = %d\n",ret);

	ret = FncInitGPIOInput();						/* GPIO ���͏����� */
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

	ret = FncInitGPIOOutput();						/* GPIO �o�͏����� */
	printf("FncInitGPIOOutput ret = %d\n",ret);

	ret = FncInitGPIOInput();						/* GPIO ���͏����� */
	printf("FncinitGPIOInput ret = %d\n",ret);

	atmp[0]  = 0x0F;								/* bit 0 - 3 ���荞�ݗL�� */
	atmp[1]  = 0x0F;								/* bit 0 - 3 Hi ���x���Ŋ��荞�ݔ��� */

	ret = FncGPIOIntEnable2 ( atmp[0], atmp[1] );	/* ���荞�ݗL�������w�� */
	printf("FncGPIOIntEnable2 ret = %d\n",ret);

	printf("[blocking scanf]\n"); scanf("%c",&c);
#endif


#if 1
/********************************************************************/
/*			GPIO ���荞�ݗL���^�R�[���o�b�N�L��						*/
/********************************************************************/
	printf("GPIO callback API Test \nPID=%d\n\n",getpid());


	ret = FncInitGPIOOutput();						/* GPIO �o�͏����� */
	printf("FncInitGPIOOutput ret = %d\n",ret);

	ret = FncInitGPIOInput();						/* GPIO ���͏����� */
	printf("FncinitGPIOInput ret = %d\n",ret);


	ret = FncGpioHandlerSet( &gpio_callback );		/* ���荞�݃n���h���o�^ */
	printf("FncGpioHandlerSet ret = %d\n",ret);


	atmp[0]  = 0xFFFF;								/* bit 0 - 15 ���荞�ݗL�� */
	atmp[1]  = 0xFFFF;								/* bit 0 - 15 Hi���x���Ŋ��荞�ݔ��� */

	ret = FncGPIOIntEnable2 ( atmp[0], atmp[1] );	/* ���荞�ݗL�������w�� */
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
/*			GPIO ���荞�ݗL���^�R�[���o�b�N����						*/
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
/*			GPIO ���荞�ݖ����^�R�[���o�b�N����						*/
/********************************************************************/
	atmp[0]  = 0x0;									/* bit 0 - 15 ���荞�ݖ��� */
	atmp[1]  = 0xFFFF;								/* bit 0 - 15 Hi���x���Ŋ��荞�ݔ��� */

	ret = FncGPIOIntEnable2 ( atmp[0], atmp[1] );	/* ���荞�ݗL�������w�� */
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
