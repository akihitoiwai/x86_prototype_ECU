/**********************************************************************************
 * �t�@�C����      : adc_api.c
 * �V�X�e����      : x86 �v���g�^�C�v�d�b�t OPE-RA Ver2.0
 * �T�u�V�X�e����  : ADC API
 * �v���O������    : 
 * CPU TYPE        : 
 * �o�[�W����      : Ver1.00
 * ���W���[���\��  : adc_api.h			API Header
 * --------------------------------------------------------------------------------
 * �쐬��          : 
 * �쐬����        : 
 * �쐬���t        : 2009�N08��24�� �V�K�쐬
 * �X�V����        : 2009�N10��19��
 **********************************************************************************/
#include <stdio.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>

#include "adc_api.h"
#include "/usr/src/CATS_ECU2/kernel_driver/cats_ioctl.h"

#define CATSDRV_PATH "/dev/catsdrv3"


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 	�@�\			:		api_simple_ioctl
//	���� 			:		unsigned int ioctl command
//	�߂�l			:		int(0-OK)
//	�@�\����		: 		�p�����^�����h�n�b�s�k�̋@�\�ł��B
static inline int api_simple_ioctl ( unsigned int cmd )
{
	int ret;
	int catsdrv_fd;

	catsdrv_fd=open(CATSDRV_PATH,O_RDWR);
	if(catsdrv_fd<0){
		return API_NG;
	}

	ret = ioctl(catsdrv_fd,cmd);
	close(catsdrv_fd);

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
	int catsdrv_fd;

	catsdrv_fd=open(CATSDRV_PATH,O_RDWR);
	if(catsdrv_fd<0){
		return API_NG;
	}

	ret = ioctl(catsdrv_fd, cmd, arg);
	close(catsdrv_fd);

	if(ret<0){
		if(-errno==CATS_ERR_BUSY) {
			return API_BUSY;
		}

		return API_NG;
	}

	return API_OK;
}


//------------------------------------------------------------------------------
//	�@�\			: 		FncInitADC
//	���� 			:		����
//	�߂�l			:		int(0-OK)
//	�@�\����		: 		�h���C�o�̂`�c�b�����@�\�ł��B

int FncInitADC ( void )
{
	return api_simple_ioctl(IOCTL_CATS_FncInitADC);
}

//------------------------------------------------------------------------------
// 	�@�\			:		FncADCStartOneShot
//	���� 			:		unsigned int - �`���l��
//	�߂�l			:		int(0-OK)
//	�@�\����		: 		�h���C�o�̃`���l���̃����V���b�g�n�@�\�ł��B
int FncADCStartOneShot ( unsigned int ch )
{
	int ioctl_arg;

	//check for invalid arguments
	if((ch>16)||(ch<1)) return API_NG;

	ioctl_arg = ch - 1;

	return api_arg_ioctl(IOCTL_CATS_FncADCStartOneShot, &ioctl_arg);
}

//------------------------------------------------------------------------------
// 	�@�\			:		FncADCStartContinuous
//	���� 			:		����
//	�߂�l			:		int(0-OK)
//	�@�\����		: 		�h���C�o�̘A�����[�h�n�@�\�ł��B
int FncADCStartContinuous ( void )
{
	return api_simple_ioctl(IOCTL_CATS_FncADCStartContinuous);
}

//------------------------------------------------------------------------------
// 	�@�\			:		FncADCStopContinuous
//	���� 			:		����
//	�߂�l			:		int(0-OK)
//	�@�\����		: 		�h���C�o�̘A�����[�h��~�@�\�ł��B
int FncADCStopContinuous ( void )
{
	return api_simple_ioctl(IOCTL_CATS_FncADCStopContinuous);
}

//------------------------------------------------------------------------------
// 	�@�\			:		FncADCGetOneShot
//	���� 			:		unsigned int 	- �`���l��
//							unsigned int* value
//	�߂�l			:		int(0-OK)
//	�@�\����		: 		�����V���b�g�̃f�[�^�ǂݍ��݋@�\�ł��B
int FncADCGetOneShot ( unsigned int ch, unsigned int *val )
{
	int ret;
	int ioctl_arg;

	//check for invalid arguments
	if((ch>16)||(ch<1)) return API_NG;

	ioctl_arg = ch - 1;
	ret = api_arg_ioctl(IOCTL_CATS_FncADCGetOneShot, &ioctl_arg);

	*val = (unsigned int) ioctl_arg;

	// check for invalid values
	if(ret==API_OK)
		if((ioctl_arg>65535)||(ioctl_arg<0)) return API_NG;

	return ret;
}

//------------------------------------------------------------------------------
// 	�@�\			:		FncADCGetContinuous
//	���� 			:		unsigned int *value(array)
//	�߂�l			:		int(0-OK)
//	�@�\����		: 		�A�����[�h�̃f�[�^�ǂݍ��݋@�\�ł��B
int FncADCGetContinuous ( unsigned int *val )
{
	int ret, ret_f;
	int catsdrv_fd;
	int ioctl_arg;
	int ch;

	catsdrv_fd=open(CATSDRV_PATH,O_RDWR);
	if(catsdrv_fd<0){
		return API_NG;
	}

	ret_f = API_OK;

	// current implementation of getcontinuous is getoneshot for all channels
	for(ch=0;ch<16;ch++){
		ioctl_arg = ch;
		ret = ioctl(catsdrv_fd,IOCTL_CATS_FncADCGetContinuous, &ioctl_arg);
		if(ret<0){
			if((-errno==CATS_ERR_BUSY)&&(ret_f!=API_NG)){
				ret_f = API_BUSY;
			}else{
				ret_f = API_NG;
			}
		}
		val[ch] = (int) ioctl_arg;
	}

	close(catsdrv_fd);

	// check for invalid values
	for(ch=0;ch<16;ch++){
		if((val[ch]>65535)||(val[ch]<0)) return API_NG;
	}

	return ret_f;
}

//------------------------------------------------------------------------------
// 	�@�\			:		FncDisableADC
//	���� 			:		����
//	�߂�l			:		int(0-OK)
//	�@�\����		: 		�`�c�b�𖳌��ɂ���@�\�ł��B
int FncDisableADC ( void )
{
	return api_simple_ioctl(IOCTL_CATS_FncDisableADC);
}


