/**********************************************************************************
 * �t�@�C����      : main.c
 * �V�X�e����      : x86 �v���g�^�C�v�d�b�t OPE-RA Ver2.0
 * �T�u�V�X�e����  : TEST �@�\����
 * �v���O������    : 
 * �o�[�W����      : Ver1.00
 * --------------------------------------------------------------------------------
 * �쐬��          : 
 * �쐬����        : 
 * �쐬���t        : 2009�N11��12�� �V�K�쐬
 * �X�V����        : 
 **********************************************************************************/

#include <stdio.h>

#include "/usr/src/CATS_ECU2/api_library/testapi/test_api.h"


/*******************************************************************
�@�\ 		: 	main
����			:	����
�߂�l		:	����
�@�\����		:	
*******************************************************************/
int main(void)
{
	unsigned int ret,tmp;

/********************************************************************/
/*							FncTEST								*/
/********************************************************************/
	printf("FncTEST test1\n");

	ret = FncTESTGetFPGAVer( &tmp );				/* FPGA�o�[�W�����擾 */
	printf("FncTESTGetFPGAVer ret = %d val = %x\n", ret, tmp);

	ret = FncTESTReadScratch( &tmp );				/* �X�N���b�`���W�X�^����ǂݍ��� */
	printf("FncTESTReadScratch ret = %d val = %x\n", ret, tmp);

	tmp++;

	ret = FncTESTWriteScratch( tmp );				/* �X�N���b�`���W�X�^�֏������� */
	printf("FncTESTWriteScratch ret = %d\n", ret);

	ret = FncTESTReadScratch( &tmp );				/* �X�N���b�`���W�X�^����ǂݍ��� */
	printf("FncTESTReadScratch ret = %d val = %x\n", ret, tmp);

	printf("\n");


	return 0;
}
