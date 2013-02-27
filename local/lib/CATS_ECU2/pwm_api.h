/**********************************************************************************
 * �t�@�C����      : pwm_api.h
 * �V�X�e����      : x86 �v���g�^�C�v�d�b�t OPE-RA Ver2.0
 * �T�u�V�X�e����  : PWM API
 * �v���O������    : �w�b�_�[
 * CPU TYPE        : 
 * �o�[�W����      : Ver1.00
 * --------------------------------------------------------------------------------
 * �쐬��          : 
 * �쐬����        : 
 * �쐬���t        : 2009�N10��30�� �V�K�쐬
 * �X�V����        : 
 **********************************************************************************/
#ifndef	_CATS_PWM_API_H_
#define _CATS_PWM_API_H_

#define API_OK 0
#define API_NG 1
#define API_BUSY 2

//------------------------------------------------------------------------------
// 	�@�\			:		FncInitPWM
//	���� 			:		unsigned int *(pwm initialization data)
//	�߂�l			:		int(0-OK)
int FncInitPWM ( unsigned int * );

//------------------------------------------------------------------------------
// 	�@�\			:		FncPWMEnable
//	���� 			:		unsigned int *(enable/disable array)
//	�߂�l			:		int(0-OK)
int FncPWMEnable ( unsigned int * );

//------------------------------------------------------------------------------
// 	�@�\			:		FncPWMSet
//	���� 			:		unsigned int - �`���l��
//							unsigned int - duty cycle
//	�߂�l			:		int(0-OK)
int FncPWMSet ( unsigned int , unsigned int  );


#endif // _CATS_PWM_API_H_
