/**********************************************************************************
 * �t�@�C����      : dac_api.c
 * �V�X�e����      : x86 �v���g�^�C�v�d�b�t OPE-RA Ver2.0
 * �T�u�V�X�e����  : DAC API
 * �v���O������    : 
 * CPU TYPE        : 
 * �o�[�W����      : Ver1.00
 * ���W���[���\��  : dac_api.h			API Header
 * --------------------------------------------------------------------------------
 * �쐬��          : 
 * �쐬����        : 
 * �쐬���t        : 2009�N10��21�� �V�K�쐬
 * �X�V����        : 
 **********************************************************************************/
#include <stdio.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>

#include "dac_api.h"
#include "/usr/src/CATS_ECU2/kernel_driver/cats_ioctl.h"

#define CATSDRV_PATH "/dev/catsdrv4"


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


//------------------------------------------------------------------------------
// 	�@�\			:		FncInitDAC
//	���� 			:		����
//	�߂�l			:		int(0-OK)
//	�@�\����		: 		�h���C�o�̂c�`�b�����@�\�ł��B
int FncInitDAC ( void )
{
	int ret;
	int catsdrv_fd;
	int ioctl_arg;
	int i;

	ret = api_simple_ioctl(IOCTL_CATS_FncInitDAC);	/* DAC �f�o�C�X�������� */

	// current implementation of initdac is dacset(zero) for all channels
	if(ret == API_OK){

		catsdrv_fd=open(CATSDRV_PATH,O_RDWR);
		if(catsdrv_fd<0){
			return API_NG;
		}

		for (i=0;i<8;i++) {
			ioctl_arg = (i << 16);

			ret = ioctl(catsdrv_fd,IOCTL_CATS_FncDACSet,&ioctl_arg);
			if(ret<0){
			close(catsdrv_fd);
			return API_NG;
			}
		}
		close(catsdrv_fd);
	}

	return ret;
}

//------------------------------------------------------------------------------
// 	�@�\			:		FncDACSet
//	���� 			:		unsigned int *(dac values)
//	�߂�l			:		int(0-OK)
//	�@�\����		: 		�c�`�b�̓d�����x����ݒ肷��@�\�ł��B
int FncDACSet ( unsigned int *val )
{
	int ret;
	int catsdrv_fd;
	int ioctl_arg;
	int i;

	//check for invalid arguments
	for (i=0;i<8;i++) {
		if((val[i]>4095)||(val[i]<0)) return API_NG;
	}

	catsdrv_fd=open(CATSDRV_PATH,O_RDWR);
	if(catsdrv_fd<0){
		return API_NG;
	}

	for (i=0;i<8;i++) {
		ioctl_arg = (i << 16) | val[i];

		ret = ioctl(catsdrv_fd,IOCTL_CATS_FncDACSet,&ioctl_arg);
		if(ret<0){
			close(catsdrv_fd);
			return API_NG;
		}
	}
	close(catsdrv_fd);

	return API_OK;
}

//------------------------------------------------------------------------------
// 	�@�\			:		FncDACStop
//	���� 			:		����
//	�߂�l			:		int(0-OK)
//	�@�\����		: 		�c�`�b�̒�~�@�\�ł��B
int FncDACStop ( void )
{
	int ret;
	int catsdrv_fd;
	int ioctl_arg;
	int i;

	catsdrv_fd=open(CATSDRV_PATH,O_RDWR);
	if(catsdrv_fd<0){
		return API_NG;
	}

	// current implementation of stopdac is dacset(zero) for all channels
	for (i=0;i<8;i++) {
		ioctl_arg = (i << 16);

		ret = ioctl(catsdrv_fd,IOCTL_CATS_FncDACSet,&ioctl_arg);
		if(ret<0){
			close(catsdrv_fd);
			return API_NG;
		}
	}
	close(catsdrv_fd);

	return api_simple_ioctl(IOCTL_CATS_FncDACStop);
}

