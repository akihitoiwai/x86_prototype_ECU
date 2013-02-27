/**********************************************************************************
 * �t�@�C����      : main.c
 * �V�X�e����      : x86 �v���g�^�C�v�d�b�t OPE-RA Ver2.0
 * �T�u�V�X�e����  : �d���Ď��@�\�T���v��
 * �v���O������    : 
 * �o�[�W����      : Ver1.00
 * ���W���[���\��  : powacc_api			�d���Ď��@�\API
 * --------------------------------------------------------------------------------
 * �쐬��          : 
 * �쐬����        : 
 * �쐬���t        : 2009�N12�� 8�� �V�K�쐬
 * �X�V����        : 
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
�@�\ 		: 	POW CALLBACK
����		:	����
�߂�l		:	����
�@�\����	:	���[�U�R�[���o�b�N�֐�
*******************************************************************/
void powmoni_callback(void)
{
	int tmp,ret;

	printf("pow_callback count = %d\n",count1);
	count1++;

	/* �d���d���Ď����荞�݃X�e�[�^�X�̓ǂݍ��݁@*/
	ret = FncPowerMoniIntSts( &tmp );
	printf("FncPowerMoniIntSts ret = %d val = %d\n",ret,tmp);
	if(ret == API_NG) return;

	/* �d���d���Ď����荞�݃X�e�[�^�X�̃N���A */
	ret = FncPowerMoniIntClear();
	printf("FncPowerMoniIntClear ret = %d\n",ret);
	if(ret == API_NG) return;

}


/*******************************************************************
�@�\ 		: 	ACC CALLBACK
����		:	����
�߂�l		:	����
�@�\����	:	���[�U�R�[���o�b�N�֐�
*******************************************************************/
void accmoni_callback(void)
{
	int tmp,ret;

	printf("acc_callback count = %d\n",count2);
	count2++;

	/* �A�N�Z�T���[�d���Ď����荞�݂̃X�e�[�^�X�ǂݍ��݁@*/
	ret = FncAccMoniIntSts( &tmp );
	printf("FncAccMoniIntSts ret = %d val = %d\n",ret,tmp);
	if(ret == API_NG) return;

	/* �A�N�Z�T���[�d���Ď����荞�݂̃N���A */
	ret = FncAccMoniIntClear();
	printf("FncAccMoniIntClear ret = %d\n",ret);
	if(ret == API_NG) return;

}


/*******************************************************************
�@�\ 		: 	main
����		:	����
�߂�l		:	����
�@�\����	:	
*******************************************************************/
int main(void)
{
	int tmp,ret;
	unsigned int count;

	count1 = 1;
	count2 = 1;

/********************************************************************/
/*			�d���Ď����荞�ݗL���^�R�[���o�b�N�L��					*/
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
/*			�d���Ď����荞�ݗL���^�R�[���o�b�N����					*/
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
/*			�d���Ď����荞�ݖ����^�R�[���o�b�N����					*/
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

