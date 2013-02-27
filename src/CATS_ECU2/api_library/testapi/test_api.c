/**********************************************************************************
 * �t�@�C����      : test_api.c
 * �V�X�e����      : x86 �v���g�^�C�v�d�b�t OPE-RA Ver2.0
 * �T�u�V�X�e����  : TEST API
 * �v���O������    : 
 * CPU TYPE        : 
 * �o�[�W����      : Ver1.00
 * ���W���[���\��  : test_api.h			API Header
 * --------------------------------------------------------------------------------
 * �쐬��          : 
 * �쐬����        : 
 * �쐬���t        : 2009�N11��11�� �V�K�쐬
 * �X�V����        : 
 **********************************************************************************/
#include <stdio.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>

#include "test_api.h"
#include "/usr/src/CATS_ECU2/kernel_driver/cats_ioctl.h"

#define CATSDRV_PATH "/dev/catsdrv9"


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


/***********************************************************************************
* MODULE         : FncTESTReadScratch
* ABSTRACT       : 
* FUNCTION       : 
* ARGUMENT       : 
* NOTE           : �X�N���b�`���W�X�^�̓ǂݍ��݋@�\�ł��B
* RETURN         : 
* CREATE         : 2009/11/11  �V�K�쐬 
* UPDATE         : 
***********************************************************************************/
int FncTESTReadScratch ( unsigned int *val )
{
	int ret;
	int ioctl_arg;

	ret = api_arg_ioctl(IOCTL_CATS_TestScratchReg_R,&ioctl_arg);

	*val =  ioctl_arg;

	return ret;
}


/***********************************************************************************
* MODULE         : FncTESTWriteScratch
* ABSTRACT       : 
* FUNCTION       : 
* ARGUMENT       : 
* NOTE           : �X�N���b�`���W�X�^�̏������݋@�\�ł��B
* RETURN         : 
* CREATE         : 2009/11/11  �V�K�쐬 
* UPDATE         : 
***********************************************************************************/
int FncTESTWriteScratch ( unsigned int val )
{
	return api_arg_ioctl(IOCTL_CATS_TestScratchReg_W, &val);
}


/***********************************************************************************
* MODULE         : FncTESTGetFPGAVer
* ABSTRACT       : 
* FUNCTION       : 
* ARGUMENT       : 
* NOTE           : FPGA�o�[�W�����擾�@�\�ł��B
* RETURN         : 
* CREATE         : 2009/11/11  �V�K�쐬 
* UPDATE         : 
***********************************************************************************/
int FncTESTGetFPGAVer ( unsigned int *val )
{
	int ret;
	int ioctl_arg;

	ret = api_arg_ioctl(IOCTL_CATS_TestVersionReadReg,&ioctl_arg);

	*val =  ioctl_arg;

	return ret;
}


