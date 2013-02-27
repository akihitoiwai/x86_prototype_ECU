/**********************************************************************************
 * �t�@�C����      : test_api.h
 * �V�X�e����      : x86 �v���g�^�C�v�d�b�t OPE-RA Ver2.0
 * �T�u�V�X�e����  : TEST API
 * �v���O������    : �w�b�_�[
 * CPU TYPE        : 
 * �o�[�W����      : Ver1.00
 * --------------------------------------------------------------------------------
 * �쐬��          : 
 * �쐬����        : 
 * �쐬���t        : 2009�N11��11�� �V�K�쐬
 * �X�V����        : 
 **********************************************************************************/
#ifndef	_CATS_TEST_API_H_
#define _CATS_TEST_API_H_

#define API_OK 0
#define API_NG 1
#define API_BUSY 2

/***********************************************************************************
* MODULE         : FncTESTReadScratch
* NOTE           : �X�N���b�`���W�X�^�̓ǂݍ��݋@�\�ł��B
***********************************************************************************/
int FncTESTReadScratch ( unsigned int * );

/***********************************************************************************
* MODULE         : FncTESTWriteScratch
* NOTE           : �X�N���b�`���W�X�^�̏������݋@�\�ł��B
***********************************************************************************/
int FncTESTWriteScratch ( unsigned int );

/***********************************************************************************
* MODULE         : FncTESTGetFPGAVer
* NOTE           : FPGA�o�[�W�����擾�@�\�ł��B
***********************************************************************************/
int FncTESTGetFPGAVer ( unsigned int * );


#endif // _CATS_TEST_API_H_