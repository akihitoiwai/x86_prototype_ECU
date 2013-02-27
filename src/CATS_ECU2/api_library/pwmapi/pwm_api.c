/**********************************************************************************
 * �t�@�C����      : pwm_api.c
 * �V�X�e����      : x86 �v���g�^�C�v�d�b�t OPE-RA Ver2.0
 * �T�u�V�X�e����  : PWM API
 * �v���O������    : 
 * CPU TYPE        : 
 * �o�[�W����      : Ver1.00
 * ���W���[���\��  : pwm_api.h			API Header
 * --------------------------------------------------------------------------------
 * �쐬��          : 
 * �쐬����        : 
 * �쐬���t        : 2009�N10��30�� �V�K�쐬
 * �X�V����        : 
 **********************************************************************************/
#include <stdio.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>

#include "pwm_api.h"
#include "/usr/src/CATS_ECU2/kernel_driver/cats_ioctl.h"

#define CATSDRV_PATH "/dev/catsdrv5"


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
// 	�@�\			:		FncInitPWM
//	���� 			:		unsigned int *(pwm initialization data)
//	�߂�l			:		int(0-OK)
//	�@�\����		: 		�h���C�o�̂o�v�l�����@�\�ł��B
int FncInitPWM ( unsigned int *val )
{
	int ret;
	int catsdrv_fd;

	catsdrv_fd=open(CATSDRV_PATH,O_RDWR);
	if(catsdrv_fd<0){
		return API_NG;
	}
													/* PWM �������� */
	ret = ioctl(catsdrv_fd, IOCTL_CATS_FncInitPWM,&val);
	close(catsdrv_fd);

	if(ret<0){

		return API_NG;
	}

	return API_OK;
}

//------------------------------------------------------------------------------
// 	�@�\			:		FncPWMEnable
//	���� 			:		unsigned int *(enable/disable array)
//	�߂�l			:		int(0-OK)
//	�@�\����		: 		�o�v�l�̗L��/�����ɂ����ݒ肷��@�\�ł��B
int FncPWMEnable ( unsigned int *val )
{
	int ioctl_arg = 0;
	int i;

	for (i=0;i<8;i++) {
		//check for invalid arguments
		if((val[i]>1)||(val[i]<0)) return API_NG;
        ioctl_arg = ioctl_arg | (val[i]<<(7-i));
	}

	return api_arg_ioctl(IOCTL_CATS_FncPWMEnable,&ioctl_arg);
}

//------------------------------------------------------------------------------
// 	�@�\			:		FncPWMSet
//	���� 			:		unsigned int - �`���l��
//							unsigned int - duty cycle
//	�߂�l			:		int(0-OK)
//	�@�\����		: 		�o�v�l�`���l���̃f���[�e�B�T�C�N����ݒ肷��@�\�ł��B
int FncPWMSet ( unsigned int ch, unsigned int val )
{
	int ret;
	int catsdrv_fd;
	int ioctl_arg[2] = {0,0};
	int *ioctl_ptr;

	ioctl_ptr = &ioctl_arg[0];

	//check for invalid arguments
	if((ch>8)||(ch<1)) return API_NG;
	if((val>0xff)||(val<0)) return API_NG;

	ioctl_arg[0] = (ch -1);
	ioctl_arg[1] = val;

	catsdrv_fd=open(CATSDRV_PATH,O_RDWR);
	if(catsdrv_fd<0){
		return API_NG;
	}

	ret = ioctl(catsdrv_fd, IOCTL_CATS_FncPWMSet,&ioctl_ptr);
	close(catsdrv_fd);

	if(ret<0){

		return API_NG;
	}

	return API_OK;
}


