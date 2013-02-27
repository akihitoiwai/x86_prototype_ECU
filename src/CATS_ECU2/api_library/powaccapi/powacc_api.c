/**********************************************************************************
 * �t�@�C����      : powacc_api.c
 * �V�X�e����      : x86 �v���g�^�C�v�d�b�t OPE-RA Ver2.0
 * �T�u�V�X�e����  : pow,acc API
 * �v���O������    : 
 * CPU TYPE        : 
 * �o�[�W����      : Ver1.00
 * ���W���[���\��  : powacc_api.h			API Header
 * --------------------------------------------------------------------------------
 * �쐬��          : 
 * �쐬����        : 
 * �쐬���t        : 2009�N09��23�� �V�K�쐬
 * �X�V����        : 2009�N12��11��
 **********************************************************************************/
#include <stdio.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include <signal.h>
#include <string.h>

#include "powacc_api.h"
#include "/usr/src/CATS_ECU2/kernel_driver/cats_ioctl.h"

#define CATSDRV_PATH "/dev/catsdrv7"

typedef struct volmoni_callback_tbl {
	int fasync_use;
	void (* powermoni_callback_fnc) (void);
	void (* accmoni_callback_fnc) (void);
} VOLMONI_CALLBACK_TBL;

VOLMONI_CALLBACK_TBL volmoni_table = { 0, NULL, NULL };

int g_volmoni_fd = 0;							/* �d���Ď��@�\�̃t�@�C���f�B�X�N���v�^ */

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 	�@�\			:		api_simple_ioctl
//	���� 			:		unsigned int ioctl command
//	�߂�l			:		int(0-OK)
//	�@�\����		: 		�p�����^�����h�n�b�s�k�̋@�\�ł��B
static inline int api_simple_ioctl ( unsigned int cmd )
{
	int ret;

	if(g_volmoni_fd == 0)						/* �t�@�C���f�B�X�N���v�^���擾�̏ꍇ */
	{
		g_volmoni_fd =open(CATSDRV_PATH,O_RDWR);	/* �擾 */
		if( g_volmoni_fd < 0 )						/* �擾���s���� */
		{
			return API_NG;
		}
	}

	ret = ioctl(g_volmoni_fd,cmd);

	if(ret<0){
		if(-errno==CATS_ERR_BUSY) {
			return API_BUSY;
		}

		return API_NG;
	}

	return API_OK;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 	�@�\			:		api_arg_ioctl
//	���� 			:		unsigned int 	- 	�R�}���h
//							int * 			-	�R�}���h�p�����^
//	�߂�l			:		int(0-OK)
//	�@�\����		: 		�p�����^�݂�h�n�b�s�k�̋@�\�ł��B
static inline int api_arg_ioctl ( unsigned int cmd, int *arg )
{
	int ret;

	if(g_volmoni_fd == 0)						/* �t�@�C���f�B�X�N���v�^���擾�̏ꍇ */
	{
		g_volmoni_fd =open(CATSDRV_PATH,O_RDWR);	/* �擾 */
		if( g_volmoni_fd < 0 )						/* �擾���s���� */
		{
			return API_NG;
		}
	}

	ret = ioctl(g_volmoni_fd, cmd, arg);

	if(ret<0){
		if(-errno==CATS_ERR_BUSY) {
			return API_BUSY;
		}

		return API_NG;
	}

	return API_OK;
}


//------------------------------------------------------------------------------
// 	�@�\			:		FncPowerMoniIntEnable
//	���� 			:		unsigned char 	-
//	�߂�l			:		int(0-OK)
//	�@�\����		:		�d���d���Ď����荞�݂𖳌�/�L���ɂ���@�\�ł��B
int FncPowerMoniIntEnable ( unsigned char sts )
{
	int ioctl_arg = (int) sts;

	//check for invalid arguments
	if((ioctl_arg>1)||(ioctl_arg<0)) return API_NG;

	return api_arg_ioctl(IOCTL_CATS_FncPowerMoniIntEnable, &ioctl_arg);
}


//------------------------------------------------------------------------------
// 	�@�\			:		FncPowerMoniIntSts
//	���� 			:		unsigned int* 	-interrupt status
//	�߂�l			:		int(0-OK)
//	�@�\����		:		�d���d���Ď����荞�݃X�e�[�^�X�̓ǂݍ��݋@�\�ł��B
int FncPowerMoniIntSts ( unsigned int *sts )
{
	return api_arg_ioctl(IOCTL_CATS_FncPowerMoniIntSts,sts);
}


//------------------------------------------------------------------------------
// 	�@�\			:		FncPowerMoniIntClear
//	���� 			:		����
//	�߂�l			:		int(0-OK)
//	�@�\����		:		�d���d���Ď����荞�݃X�e�[�^�X�̃N���A�@�\�ł��B
int FncPowerMoniIntClear ( void )
{
	return api_simple_ioctl(IOCTL_CATS_FncPowerMoniIntClear);
}


//------------------------------------------------------------------------------
// 	�@�\			:		FncAccMoniIntEnable
//	���� 			:		unsigned char�@enable/disable
//	�߂�l			:		int(0-OK)
//	�@�\����		:		�A�N�Z�T���[�d���Ď����荞�݂̖���/�L���ɂ���@�\�ł��B
int FncAccMoniIntEnable ( unsigned char sts )
{
	int ioctl_arg = (int) sts;

	//check for invalid arguments
	if((ioctl_arg>1)||(ioctl_arg<0)) return API_NG;

	return api_arg_ioctl(IOCTL_CATS_FncAccMoniIntEnable, &ioctl_arg);
}


//------------------------------------------------------------------------------
// 	�@�\			:		FncAccMoniIntSts
//	���� 			:		unsigned int* 	-interrupt status
//	�߂�l			:		int(0-OK)
//	�@�\����		:		�A�N�Z�T���[�d���Ď����荞�݃X�e�[�^�X�̓ǂݍ��݋@�\�ł��B
int FncAccMoniIntSts ( unsigned int *sts )
{
	return api_arg_ioctl(IOCTL_CATS_FncAccMoniIntSts,sts);
}


//------------------------------------------------------------------------------
// 	�@�\			:		FncAccMoniIntClear
//	���� 			:		����
//	�߂�l			:		int(0-OK)
//	�@�\����		:		�A�N�Z�T���[�d���Ď����荞�݃X�e�[�^�X�̃N���A�@�\�ł��B
int FncAccMoniIntClear ( void )
{
	return api_simple_ioctl(IOCTL_CATS_FncAccMoniIntClear);
}


/***********************************************************************************
* MODULE         : Volmoni_SigHandler
* ABSTRACT       : �d���Ď� �V�O�i���n���h��
* FUNCTION       : 
* ARGUMENT       : 
* NOTE           : 
* RETURN         : ����I���Łu�O�v��Ԃ��B
* CREATE         : 2009/12/ 8  �V�K�쐬 
* UPDATE         : 2009/12/10
***********************************************************************************/
void Volmoni_SigHandler(int signo)
{
	int ret;
	int ioctl_arg, tmp;

//	printf("Volmoni_SigHandler\n");

	/* �d���Ď����荞�݃`�F�b�N */
	ret = ioctl(g_volmoni_fd, IOCTL_CATS_CallBackPowAccGetIrq, &ioctl_arg);
//	printf("IOCTL_CATS_CallBackPowAccGetIrq = %x ret = %d\n",ioctl_arg ,ret );

	tmp = ioctl_arg;

	if(( tmp & POWERMONI_INT ) != 0 ){
//		printf("POW OFF Det!!\n");

		/* ���荞�ݗv������ */
		ioctl_arg = POWERMONI_INT;
		ret = ioctl(g_volmoni_fd, IOCTL_CATS_CallBackPowAccClrIrq, &ioctl_arg);
//		printf("IOCTL_CATS_CallBackPowAccClrIrq = %x ret = %d\n",ioctl_arg ,ret );

		/* �R�[���o�b�N�֐��o�^�`�F�b�N */
		if(( *volmoni_table.powermoni_callback_fnc ) != NULL ){
			(*volmoni_table.powermoni_callback_fnc)();
		}
	}

	if(( tmp & ACCMONI_INT ) != 0 ){
//		printf("ACC OFF Det!!\n");

		/* ���荞�ݗv������ */
		ioctl_arg = ACCMONI_INT;
		ret = ioctl(g_volmoni_fd, IOCTL_CATS_CallBackPowAccClrIrq, &ioctl_arg);
//		printf("IOCTL_CATS_CallBackPowAccClrIrq = %x ret = %d\n",ioctl_arg ,ret );

		/* �R�[���o�b�N�֐��o�^�`�F�b�N */
		if(( *volmoni_table.accmoni_callback_fnc ) != NULL ){
			(*volmoni_table.accmoni_callback_fnc)();
		}
	}
}


/***********************************************************************************
* MODULE         : FncVolMoniHandlerSet
* ABSTRACT       : �d���Ď����荞�݂̃R�[���o�b�N�֐���o�^����B
* FUNCTION       : 
* ARGUMENT       : 
* NOTE           : kind  :callback type
*                : *func :callback function (pointer)
* RETURN         : ����I���Łu�O�v��Ԃ��B
* CREATE         : 2009/12/ 8  �V�K�쐬 
* UPDATE         : 2009/12/10
***********************************************************************************/
int FncVolMoniHandlerSet ( unsigned char kind, void * func )
{
	int ret;
	int oflags;
	struct sigaction act;

	memset(&act ,0 ,sizeof( struct sigaction ));

	if(func == NULL)
	{
		return API_NG;
	}

	if(g_volmoni_fd == 0)							/* �t�@�C���f�B�X�N���v�^���擾�̏ꍇ */
	{
		g_volmoni_fd =open(CATSDRV_PATH,O_RDWR);	/* �擾 */
		if( g_volmoni_fd < 0 )						/* �擾���s���� */
		{
			return API_NG;
		}
	}

	/* �V�O�i���n���h���o�^ */
	if(volmoni_table.fasync_use == 0)
	{
		sigaction(SIGIO, NULL, &act);				/* �ݒ�O�̎擾 */
		act.sa_handler = Volmoni_SigHandler;		/* �n���h���̐ݒ� */
		sigemptyset(&act.sa_mask);					/* sa_mask �̏����� */
		sigaddset(&act.sa_mask, SIGIO);				/* �V�O�i���n���h�����s���̓u���b�N */
		act.sa_flags = 0;							/* sa_flags �̓f�t�H���g */
		act.sa_restorer = NULL;						/* sa_restorer �͖��g�p */
		sigaction(SIGIO, &act, NULL);				/* �V�O�i���n���h���o�^ */

		fcntl(g_volmoni_fd, F_SETOWN, getpid());
		oflags = fcntl(g_volmoni_fd, F_GETFL);
		fcntl(g_volmoni_fd, F_SETFL, oflags | FASYNC);
		volmoni_table.fasync_use = 1;
	}


	if( kind == CALLBACK_POWER)
	{
		volmoni_table.powermoni_callback_fnc = func;
		ret = API_OK;

	}else if( kind == CALLBACK_ACC)
	{
		volmoni_table.accmoni_callback_fnc = func;
		ret = API_OK;

	}else{
		ret = API_NG;
	}


	return ret;
}


/***********************************************************************************
* MODULE         : FncVolMoniHandlerClear
* ABSTRACT       : �d���Ď����荞�݂̃R�[���o�b�N�֐�����������B
* FUNCTION       : 
* ARGUMENT       : 
* NOTE           : kind  :callback type
* RETURN         : ����I���Łu�O�v��Ԃ��B
* CREATE         : 2009/12/ 8  �V�K�쐬 
* UPDATE         : 2009/12/ 9
***********************************************************************************/
int FncVolMoniHandlerClear ( unsigned char kind )
{
	int ret;

	if( kind == CALLBACK_POWER)
	{
		volmoni_table.powermoni_callback_fnc = NULL;
		ret = API_OK;

	}else if( kind == CALLBACK_ACC)
	{
		volmoni_table.accmoni_callback_fnc = NULL;
		ret = API_OK;

	}else{
		ret = API_NG;
	}

	return ret;
}

