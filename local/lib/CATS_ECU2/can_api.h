/**********************************************************************************
 * �t�@�C����      : can_api.h
 * �V�X�e����      : x86 �v���g�^�C�v�d�b�t OPE-RA Ver2.0
 * �T�u�V�X�e����  : CAN API
 * �v���O������    : �w�b�_�[
 * CPU TYPE        : 
 * �o�[�W����      : Ver1.00
 * --------------------------------------------------------------------------------
 * �쐬��          : 
 * �쐬����        : 
 * �쐬���t        : 2009�N09��23�� �V�K�쐬
 * �X�V����        : 
 **********************************************************************************/
#ifndef CAN_API_H
#define CAN_API_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include "/usr/src/CATS_ECU2/kernel_driver/cats_ioctl.h"
#include "/usr/src/CATS_ECU2/kernel_driver/cats_sja1000.h"


//-----------------------------------------------------------------//
/*!
	@brief	CAN API �R���e�L�X�g�\����
*/
//-----------------------------------------------------------------//
typedef struct {
	int			fd;		///< �t�@�C�����ʎq
	CAN_LOG		log[2];	///< ���O�ۑ��p�\����
	sem_t		sem_obj;
} CAN_CONTEXT;


#ifdef __cplusplus
extern "C" {
#endif

//-----------------------------------------------------------------//
/*!
	@brief	CAN �h���C�o�[�A�R���e�L�X�g�̍쐬
	@return CAN_CONTEXT �\���̂̃|�C���^�[
*/
//-----------------------------------------------------------------//
CAN_CONTEXT *CAN_create();


//-----------------------------------------------------------------//
/*!
	@brief	CAN �f�o�C�X�̃��W�X�^�[�𒼐ړǂݏo���B
	@param[in]	con �f�o�C�X�E�R���e�L�X�g
	@param[in]	device �f�o�C�X
	@param[in]	adr	   SJA1000 �̃A�h���X
	@param[in]	data   �ǂݏo�����f�[�^�̊i�[��
	@return		����Ȃ�u�O�v��Ԃ�
 */
//-----------------------------------------------------------------//
int CAN_read_register(CAN_CONTEXT *con, int device, int adr, int *data);


//-----------------------------------------------------------------//
/*!
	@brief	CAN �f�o�C�X�̃��W�X�^�[�𒼐ڏ������ށB
	@param[in]	con �f�o�C�X�E�R���e�L�X�g
	@param[in]	device �f�o�C�X
	@param[in]	adr	   SJA1000 �̃A�h���X
	@param[in]	data   �������ރf�[�^
	@return		����Ȃ�u�O�v��Ԃ�
*/
//-----------------------------------------------------------------//
int CAN_write_register(CAN_CONTEXT *con, int device, int adr, int data);


//-----------------------------------------------------------------//
/*!
	@brief	CAN ������
	@param[in]	con �f�o�C�X�E�R���e�L�X�g
	@param[in]	device �f�o�C�X
	@param[in]	send_buff_size	���M�o�b�t�@�T�C�Y
	@param[in]	recv_buff_size	��M�o�b�t�@�T�C�Y
	@return		����Ȃ�u�O�v��Ԃ�
*/
//-----------------------------------------------------------------//
int CAN_init(CAN_CONTEXT *con, int device, int send_buff_size, int recv_buff_size);


//-----------------------------------------------------------------//
/*!
	@brief	CAN ���Z�b�g
	@param[in]	con �f�o�C�X�E�R���e�L�X�g
	@param[in]	device �f�o�C�X
	@param[in]	mode �\���̃|�C���^�[
	@return		����Ȃ�u�O�v��Ԃ�
*/
//-----------------------------------------------------------------//
int CAN_reset(CAN_CONTEXT *con, int device, const CAN_MODE *mode);


//-----------------------------------------------------------------//
/*!
	@brief	CAN �X�^�[�g
	@param[in]	con �f�o�C�X�E�R���e�L�X�g
	@param[in]	device �f�o�C�X
	@return		����Ȃ�u�O�v��Ԃ�
*/
//-----------------------------------------------------------------//
int CAN_start(CAN_CONTEXT *con, int device);


//-----------------------------------------------------------------//
/*!
	@brief	CAN �X�g�b�v@n
			��x�u��~�v�������ꍇ�A�ureset�v���āustart�v����K�v������B
	@param[in]	con �f�o�C�X�E�R���e�L�X�g
	@param[in]	device �f�o�C�X
	@return		����Ȃ�u�O�v��Ԃ�
*/
//-----------------------------------------------------------------//
int CAN_stop(CAN_CONTEXT *con, int device);


//-----------------------------------------------------------------//
/*!
	@brief	CAN �X���[�v
	@param[in]	con �f�o�C�X�E�R���e�L�X�g
	@param[in]	device �f�o�C�X
	@return		����Ȃ�u�O�v��Ԃ�
*/
//-----------------------------------------------------------------//
int CAN_sleep(CAN_CONTEXT *con, int device);


//-----------------------------------------------------------------//
/*!
	@brief	CAN ���O�擾
	@param[in]	con �f�o�C�X�E�R���e�L�X�g
	@param[in]	device �f�o�C�X
	@param[in]	log	   ���O�\����
	@return		����Ȃ�u�O�v��Ԃ�
*/
//-----------------------------------------------------------------//
int CAN_log(CAN_CONTEXT *con, int device, CAN_LOG *log);



//-----------------------------------------------------------------//
/*!
	@brief	CAN �Z���h�i���M�j
			���M�o�C�g�����u�O�v�̏ꍇ�A�����[�g�t���[���̑��M�ƂȂ�
	@param[in]	con �f�o�C�X�E�R���e�L�X�g
	@param[in]	device �f�o�C�X
	@param[in]	frame �\���̃|�C���^�[
	@return		����Ȃ�u�O�v��Ԃ�
*/
//-----------------------------------------------------------------//
int CAN_send_device(CAN_CONTEXT *con, int device, const CAN_FRAME *frame);


//-----------------------------------------------------------------//
/*!
	@brief	CAN ���V�[�u�i��M�j
	@param[in]	con �f�o�C�X�E�R���e�L�X�g
	@param[in]	device �f�o�C�X
	@param[in]	frame �\���̃|�C���^�[
	@return		���V�[�u�f�[�^������΁A�O�ȊO
*/
//-----------------------------------------------------------------//
int CAN_recv_device(CAN_CONTEXT *con, int device, CAN_FRAME *frame);


//-----------------------------------------------------------------//
/*!
	@brief	CAN �X�e�[�^�X���󂯎��
	@param[in]	con �f�o�C�X�E�R���e�L�X�g
	@param[in]	device �f�o�C�X
	@param[in]	status �\���̃|�C���^�[
	@return		����Ȃ�u�O�v��Ԃ�
*/
//-----------------------------------------------------------------//
int CAN_status(CAN_CONTEXT *con, int device, CAN_STATUS *status);


//-----------------------------------------------------------------//
/*!
	@brief	CAN �I������
	@param[in]	con �f�o�C�X�E�R���e�L�X�g
	@param[in]	device �f�o�C�X
	@return		����Ȃ�u�O�v��Ԃ�
*/
//-----------------------------------------------------------------//
int CAN_exit(CAN_CONTEXT *con, int device);



//-----------------------------------------------------------------//
/*!
	@brief	CAN �h���C�o�[�A�R���e�L�X�g�̔p��
	@param[in]	con �f�o�C�X�E�R���e�L�X�g
*/
//-----------------------------------------------------------------//
void CAN_destroy(CAN_CONTEXT *con);


//-----------------------------------------------------------------//
/*!
	@brief	CAN ���荞�݃��O�E�_���v
	@param[in]	con �f�o�C�X�E�R���e�L�X�g
	@param[in]	device �f�o�C�X
	@param[in]	output	�_���v�o�͐�̃X�g���[���|�C���^�[
*/
//-----------------------------------------------------------------//
void CAN_log_dump(CAN_CONTEXT *con, int device, FILE *output);


#ifdef __cplusplus
};
#endif

#endif	// CAN_API_H

