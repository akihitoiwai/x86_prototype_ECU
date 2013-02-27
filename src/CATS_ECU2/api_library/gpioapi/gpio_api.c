/**********************************************************************************
 * �t�@�C����      : gpio_api.c
 * �V�X�e����      : x86 �v���g�^�C�v�d�b�t OPE-RA Ver2.0
 * �T�u�V�X�e����  : GPIO API
 * �v���O������    : 
 * CPU TYPE        : 
 * �o�[�W����      : Ver1.01
 * ���W���[���\��  : gpio_api.h			API Header
 * --------------------------------------------------------------------------------
 * �쐬��          : 
 * �쐬����        : 
 * �쐬���t        : 2009�N10��26�� �V�K�쐬
 * �X�V����        : 2010�N06��10��
 **********************************************************************************/
#include <stdio.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include <signal.h>
#include <string.h>

#include "gpio_api.h"
#include "/usr/src/CATS_ECU2/kernel_driver/cats_ioctl.h"

#define CATSDRV_PATH "/dev/catsdrv6"

#define MASK24BIT 0xFFFFFF
#define MASK16BIT 0xFFFF

typedef struct gpio_callback_tbl {
	int fasync_use;
	void (* gpio_callback_fnc) (void);
} GPIO_CALLBACK_TBL;

GPIO_CALLBACK_TBL gpio_table = { 0, NULL };

int g_gpio_fd = 0;								/* GPIO �@�\�̃t�@�C���f�B�X�N���v�^ */

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 	�@�\		:		api_simple_ioctl
//	���� 		:		unsigned int ioctl command
//	�߂�l		:		int(0-OK)
//	�@�\����	: 		�p�����^�����h�n�b�s�k�̋@�\�ł��B
static inline int api_simple_ioctl( unsigned int cmd )
{
	int ret;

	if(g_gpio_fd == 0)							/* �t�@�C���f�B�X�N���v�^���擾�̏ꍇ */
	{
		g_gpio_fd =open(CATSDRV_PATH,O_RDWR);	/* �擾 */
		if( g_gpio_fd < 0 )						/* �擾���s���� */
		{
			return API_NG;
		}
	}

	ret = ioctl(g_gpio_fd,cmd);

	if(ret<0){
		if(-errno==CATS_ERR_BUSY) {
			return API_BUSY;
		}

		return API_NG;
	}

	return API_OK;
}

//------------------------------------------------------------------------------
// 	�@�\		:		api_arg_ioctl
//	���� 		:		unsigned int	- �R�}���h
//			:		int * 		- �R�}���h�p�����^
//	�߂�l		:		int(0-OK)
//	�@�\����	: 		�p�����^�݂�h�n�b�s�k�̋@�\�ł��B
static inline int api_arg_ioctl ( unsigned int cmd, int *arg )
{
	int ret;

	if(g_gpio_fd == 0)							/* �t�@�C���f�B�X�N���v�^���擾�̏ꍇ */
	{
		g_gpio_fd =open(CATSDRV_PATH,O_RDWR);	/* �擾 */
		if( g_gpio_fd < 0 )						/* �擾���s���� */
		{
			return API_NG;
		}
	}

	ret = ioctl(g_gpio_fd, cmd, arg);

	if(ret<0){
		if(-errno==CATS_ERR_BUSY) {
			return API_BUSY;
		}

		return API_NG;
	}

	return API_OK;
}


//------------------------------------------------------------------------------
// 	�@�\		:		FncinitGPIOOutput
//	���� 		:		����
//	�߂�l		:		int(0-OK)
//	�@�\����	: 		�f�o�h�n�o�͂̏����@�\�ł��B
int FncInitGPIOOutput ( void )
{
	return api_simple_ioctl(IOCTL_CATS_FncinitGPIOOutput);
}

//------------------------------------------------------------------------------
// 	�@�\		:		FncinitGPIOInput
//	���� 		:		����
//	�߂�l		:		int 
//	�@�\����	: 		�f�o�h�n���͂̏����@�\�ł��B
int FncInitGPIOInput ( void )
{
	return api_simple_ioctl(IOCTL_CATS_FncinitGPIOInput);
}

//------------------------------------------------------------------------------
// 	�@�\		:		FncGPIOSet
//	���� 		:		unsigned int - �`���l��
//			:		unsigned int - output value
//	�߂�l		:		int(0-OK)
//	�@�\����	: 		�`���l���̂f�o�h�n�o�͂�ݒ肷��@�\�ł��B
int FncGPIOSet ( unsigned int ch, unsigned int val )
{
	int ioctl_arg;

	//check for invalid arguments
	if((ch>23)||(ch<0)) return API_NG;
	if((val>1)||(val<0)) return API_NG;

	ioctl_arg = val << 31 | 1 << ch;

	return api_arg_ioctl(IOCTL_CATS_FncGPIOSet,&ioctl_arg);
}


//------------------------------------------------------------------------------
// 	�@�\		:		FncGPIOSetAll
//	����		:		unsigned int - output value
//	�߂�l		:		int(0-OK)
//	�@�\����	: 		�Q�S�`���l�����̂f�o�h�n�o�͂�ݒ肷��
int FncGPIOSetAll( unsigned int val )
{
	int ioctl_arg;

	ioctl_arg = val & MASK24BIT;			/* 24Bit MASK */

	return api_arg_ioctl(IOCTL_CATS_FncGPIOSetAll,&ioctl_arg);

}


//------------------------------------------------------------------------------
// 	�@�\		:		FncGPIOGet
//	���� 		:		unsigned int	- �`���l��
//			:		unsigned int*	- input value
//	�߂�l		:		int(0-OK)
//	�@�\����	: 		�`���l���̂f�o�h�n���̓f�[�^�ǂݍ��݋@�\�ł��B
int FncGPIOGet ( unsigned int ch, unsigned int *val )
{
	int ret;
	int ioctl_arg;

	//check for invalid arguments
	if((ch>15)||(ch<0)) return API_NG;

	ret = api_arg_ioctl(IOCTL_CATS_FncGPIOGet,&ioctl_arg);

	*val = (ioctl_arg >> ch) & 0x1;

	return ret;
}


//------------------------------------------------------------------------------
// 	�@�\		:		FncGPIOGetAll
//	����		:		unsigned int - output value
//	�߂�l		:		int(0-OK)
//	�@�\����	: 		�P�U�`���l�����f�o�h�n���̓f�[�^�ǂݍ���
int FncGPIOGetAll( unsigned int *val )
{
	int ret;
	int ioctl_arg;

	ret = api_arg_ioctl(IOCTL_CATS_FncGPIOGetAll,&ioctl_arg);

	*val =  ioctl_arg & MASK16BIT;			/* 16Bit MASK */

	return ret;
}


//------------------------------------------------------------------------------
// 	�@�\		:		FncGPIOIntEnable
//	���� 		:		unsigned int*	- (enable/disable array)
//			:		unsigned int*	- (level array)
//	�߂�l		:		int(0-OK)
//	�@�\����	: 		�f�o�h�n���͊��荞�݂̖���/�L���ɂ���@�\�ł��B
int FncGPIOIntEnable ( unsigned int *sts, unsigned int *lvl )
{
	int ret;
	int ioctl_arg[2] = {0,0};
	int *ioctl_ptr;
	int i;

	ioctl_ptr = &ioctl_arg[0];

	for (i=0;i<16;i++) {
		//check for invalid arguments
		if((sts[i]>1)||(sts[i]<0)) return API_NG;
        ioctl_arg[0] = ioctl_arg[0] | sts[i]<<(15-i);
	}

	for (i=0;i<16;i++) {
		//check for invalid arguments
		if((lvl[i]>1)||(lvl[i]<0)) return API_NG;
        ioctl_arg[1] = ioctl_arg[1] | lvl[i]<<(15-i);
	}

	if(g_gpio_fd == 0)							/* �t�@�C���f�B�X�N���v�^���擾�̏ꍇ */
	{
		g_gpio_fd =open(CATSDRV_PATH,O_RDWR);	/* �擾 */
		if( g_gpio_fd < 0 )						/* �擾���s���� */
		{
			return API_NG;
		}
	}

	ret = ioctl(g_gpio_fd, IOCTL_CATS_FncGPIOIntEnable,&ioctl_ptr);

	if(ret<0){

		return API_NG;
	}

	return API_OK;
}


//------------------------------------------------------------------------------
// 	�@�\		:		FncGPIOIntEnable2
//	���� 		:		unsigned int	- (enable/disable)
//			:		unsigned int	- (level)
//	�߂�l		:		int(0-OK)
//	�@�\����	: 		�f�o�h�n���͊��荞�݂̖���/�L���ɂ���@�\�ł��B
int FncGPIOIntEnable2( unsigned int sts, unsigned int lvl )
{
	int ret;
	int ioctl_arg[2] = {0,0};
	int *ioctl_ptr;

	if(g_gpio_fd == 0)							/* �t�@�C���f�B�X�N���v�^���擾�̏ꍇ */
	{
		g_gpio_fd =open(CATSDRV_PATH,O_RDWR);	/* �擾 */
		if( g_gpio_fd < 0 )						/* �擾���s���� */
		{
			return API_NG;
		}
	}

	ioctl_arg[0] = sts & MASK16BIT;
	ioctl_arg[1] = lvl & MASK16BIT;

	ioctl_ptr = &ioctl_arg[0];

	ret = ioctl(g_gpio_fd, IOCTL_CATS_FncGPIOIntEnable,&ioctl_ptr);

	if(ret<0){

		return API_NG;
	}

	return API_OK;
}


//------------------------------------------------------------------------------
// 	�@�\		:		FncGPIOIntSts
//	���� 		:		unsigned int*	- (status array)
//	�߂�l		:		int(0-OK)
//	�@�\����	: 		�f�o�h�n���荞�݂̃X�e�[�^�X�ǂݍ��݋@�\�ł��B
int FncGPIOIntSts ( unsigned int *val )
{
	int ret;
	int ioctl_arg;
	int i;

	ret = api_arg_ioctl(IOCTL_CATS_FncGPIOIntSts,&ioctl_arg);

	for(i=0;i<16;i++){
	    val[(15-i)] = (ioctl_arg >> i) & 0x1;
	}

	return ret;
}


//------------------------------------------------------------------------------
// 	�@�\		:		FncGPIOIntSts2
//	����		:		unsigned int*	- (status array)
//	�߂�l		:		int(0-OK)
//	�@�\����	: 		�P�U�`���l�����̂f�o�h�n���荞�ݏ�Ԃ��擾����
int FncGPIOIntSts2 ( unsigned int *val )
{
	int ret;
	int ioctl_arg;

	ret = api_arg_ioctl(IOCTL_CATS_FncGPIOIntSts,&ioctl_arg);

	*val =  ioctl_arg & MASK16BIT;			/* 16Bit MASK */

	return ret;
}

//------------------------------------------------------------------------------
// 	�@�\		:		FncGPIOIntClear
//	����	 	:		unsigned int	- �f�o�h�n���荞��
//	�߂�l		:		int(0-OK)
//	�@�\����	: 		�f�o�h�n���荞�݂̃N���A�@�\�ł��B
int FncGPIOIntClear ( unsigned int val )
{
	return api_arg_ioctl(IOCTL_CATS_FncGPIOIntClear, &val);
}


/***********************************************************************************
* MODULE         : Gpio_SigHandler
* ABSTRACT       : GPIO �V�O�i���n���h��
* FUNCTION       : 
* ARGUMENT       : 
* NOTE           : 
* RETURN         : ����I���Łu�O�v��Ԃ��B
* CREATE         : 2009/12/10  �V�K�쐬 
* UPDATE         : 2010/06/10
***********************************************************************************/
void Gpio_SigHandler(int signo)
{
	int ret;
	int ioctl_arg;

//	printf("gpio callback\n");

	/* GPIO ���荞�݃`�F�b�N */
	ret = ioctl(g_gpio_fd, IOCTL_CATS_CallBackGPIOGetIrq, &ioctl_arg);
//	printf("IOCTL_CATS_CallBackGPIOGetIrq = %x ret = %d\n",ioctl_arg ,ret );


	if(( ioctl_arg & GPIO_INT ) != 0 ){
//		printf("GPIO INT Det!! gpio_intreq = %x\n", ioctl_arg);

		/* ���荞�ݗv������ */
		ret = ioctl(g_gpio_fd, IOCTL_CATS_CallBackGPIOClrIrq, &ioctl_arg);
//		printf("IOCTL_CATS_CallBackGPIOClrIrq gpio_intreq = %x\n",ioctl_arg );

		/* �R�[���o�b�N�֐��o�^�`�F�b�N */
		if(( *gpio_table.gpio_callback_fnc ) != NULL ){
			(*gpio_table.gpio_callback_fnc)();
		}
	}
}

/***********************************************************************************
* MODULE         : FncGpioHandlerSet
* ABSTRACT       : GPIO ���荞�݂̃R�[���o�b�N�֐���o�^����B
* FUNCTION       : 
* ARGUMENT       : 
* NOTE           : *func :callback function (pointer)
* RETURN         : ����I���Łu�O�v��Ԃ��B
* CREATE         : 2009/12/10  �V�K�쐬 
* UPDATE         : 
***********************************************************************************/
int FncGpioHandlerSet ( void * func )
{
	int ret;
	int oflags;
	struct sigaction act;

	memset(&act ,0 ,sizeof( struct sigaction ));

	if(func == NULL)
	{
		return API_NG;
	}

	if(g_gpio_fd == 0)							/* �t�@�C���f�B�X�N���v�^���擾�̏ꍇ */
	{
		g_gpio_fd =open(CATSDRV_PATH,O_RDWR);	/* �擾 */
		if( g_gpio_fd < 0 )						/* �擾���s���� */
		{
			return API_NG;
		}
	}

	/* �V�O�i���n���h���o�^ */
	if(gpio_table.fasync_use == 0)
	{
		sigaction(SIGIO, NULL, &act);			/* �ݒ�O�̎擾 */
		act.sa_handler = Gpio_SigHandler;		/* �n���h���̐ݒ� */
		sigemptyset(&act.sa_mask);				/* sa_mask �̏����� */
		sigaddset(&act.sa_mask, SIGIO);			/* �V�O�i���n���h�����s���̓u���b�N */
		act.sa_flags = 0;						/* sa_flags �̓f�t�H���g */
		act.sa_restorer = NULL;					/* sa_restorer �͖��g�p */
		sigaction(SIGIO, &act, NULL);			/* �V�O�i���n���h���o�^ */

		fcntl(g_gpio_fd, F_SETOWN, getpid());
		oflags = fcntl(g_gpio_fd, F_GETFL);
		fcntl(g_gpio_fd, F_SETFL, oflags | FASYNC);
		gpio_table.fasync_use = 1;
	}

	gpio_table.gpio_callback_fnc = func;
	ret = API_OK;

	return ret;
}


/***********************************************************************************
* MODULE         : FncGpioHandlerClear
* ABSTRACT       : GPIO ���荞�݂̃R�[���o�b�N�֐�����������B
* FUNCTION       : 
* ARGUMENT       : 
* NOTE           : 
* RETURN         : ����I���Łu�O�v��Ԃ��B
* CREATE         : 2009/12/10  �V�K�쐬 
* UPDATE         : 
***********************************************************************************/
int FncGpioHandlerClear ( void )
{
	int ret;

	gpio_table.gpio_callback_fnc = NULL;
	ret = API_OK;

	return ret;
}

