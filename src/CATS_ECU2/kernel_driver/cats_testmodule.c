/**********************************************************************************
 * �t�@�C����      : cats_testmodule.c
 * �V�X�e����      : x86 �v���g�^�C�v�d�b�t OPE-RA Ver2.0
 * �T�u�V�X�e����  : �J�[�l���h���C�o
 * �v���O������    : �e�X�g���W���[��
 * �o�[�W����      : Ver1.00
 * ���W���[���\��  : cats_fpga.c		PCI Access
 *                 : cats_testmodule.c	FPGA TEST Module
 * --------------------------------------------------------------------------------
 * �쐬��          : 
 * �쐬����        : 
 * �쐬���t        : 2009�N11��11�� �V�K�쐬
 * �X�V����        : 
 **********************************************************************************/
#include <linux/pci.h>
#include <asm/uaccess.h>

#include "cats_fpga.h"
#include "cats_files.h"
#include "cats_ioctl.h"


/***********************************************************************************
* MODULE         : catsdrv_test_open
* ABSTRACT       : CATS/FPGA �f�o�C�X�E�t�@�C���E[open] ����
* FUNCTION       : 
* ARGUMENT       : 
* NOTE           : 
* RETURN         : ����I���Łu�O�v��Ԃ��B
* CREATE         : 2009/11/11  �V�K�쐬 
* UPDATE         : 
***********************************************************************************/
int catsdrv_test_open( struct inode *inode, struct file *file )
{
#if 0
	printk(KERN_INFO "===== cats_module_test: open\n");
	printk(KERN_INFO "===== cats_module_test: Major %d Minor %d (pid %d)\n",
			imajor(inode),
			iminor(inode),
			current->pid );

#endif
	return 0;
}


/***********************************************************************************
* MODULE         : catsdrv_test_release
* ABSTRACT       : CATS/FPGA �f�o�C�X�E�t�@�C���E[release]����
* FUNCTION       : 
* ARGUMENT       : 
* NOTE           : 
* RETURN         : ����I���Łu�O�v��Ԃ��B
* CREATE         : 2009/11/11  �V�K�쐬 
* UPDATE         : 
***********************************************************************************/
int catsdrv_test_release( struct inode *inode, struct file *file )
{
#if 0
	printk(KERN_INFO "===== cats_module_test: release\n");
	printk(KERN_INFO "===== cats_module_test: Major %d Minor %d (pid %d)\n",
			imajor(inode),
			iminor(inode),
			current->pid );

#endif
	return 0;
}


/***********************************************************************************
* MODULE         : catsdrv_test_ioctl
* ABSTRACT       : CATS/FPGA ���W���[���E�t�@�C���E[ioctl]����
* FUNCTION       : 
* ARGUMENT       : 
* NOTE           : 
*                : @param[in]	inode
*                : @param[in]	filep
*                : @param[in]	cmd
*                : @param[in]	arg
*                : @return ����I���Łu�O�v��Ԃ��B
* RETURN         : ����I���Łu�O�v��Ԃ��B
* CREATE         : 2009/11/11  �V�K�쐬 
* UPDATE         : 
***********************************************************************************/
int catsdrv_test_ioctl( struct inode *inode, struct file *filep,
				   unsigned int cmd, unsigned long arg )
{
	int ioctl_arg,ret,tmp;
	int size;
	volatile int *vio;

#if 0
	printk(KERN_INFO "===== cats_module_test: ioctl start\n");
	printk(KERN_INFO "===== cats_module_test: Major %d Minor %d (pid %d)\n",
			imajor(inode),
			iminor(inode),
			current->pid );
#endif
	vio = (volatile int *)pci_adr_map;

	if( _IOC_TYPE( cmd ) != IOCTL_MAGIC_CATSDRV ) {
		printk(KERN_WARNING "\n\rIOCTL Op > Op IOCTL_MAGIC_DRV_CAM\n\r");
		return -EINVAL;
	}

	if( _IOC_NR( cmd ) >= IOCTL_DRV_CATSDRV_MAX ) {
		printk(KERN_WARNING "\n\rIOCTL Op > Op count\n\r" );
		return -EINVAL;
	}

	size = _IOC_SIZE( cmd );
	if( size ) {
		int err = 0;

		if( _IOC_DIR( cmd ) & _IOC_WRITE ) {
			err = !access_ok( VERIFY_READ, ( void *)arg, size );
			if( err ) {
				printk(KERN_WARNING "\ndrv_cam : IOCTL Verify read failed" );
				return err;
			}
		}

		if( _IOC_DIR( cmd ) & _IOC_READ ) {
			err = !access_ok( VERIFY_WRITE, ( void *)arg, size );
			if( err ) {
				printk(KERN_WARNING "\ndrv_cam : IOCTL Verify write failed" );
				return err;
			}
		}
	}


	switch ( cmd ) {
		/***********************************************************************
		* MODULE : FncTESTReadScratch
		* NOTE   : �X�N���b�`���W�X�^�̓ǂݍ��݋@�\
		************************************************************************/
		case IOCTL_CATS_TestScratchReg_R:
//			printk(KERN_INFO "IOCTL_CATS_TestScratchReg_R\n");
		{
			tmp = 0;

			tmp = vio[SCRATCH];

			ret = __put_user(tmp, (int __user *)arg);
			return 0;
		}

		/************************************************************************
		* MODULE : FncTESTWriteScratch
		* NOTE   : �X�N���b�`���W�X�^�̏������݋@�\
		************************************************************************/
		case IOCTL_CATS_TestScratchReg_W:
//			printk(KERN_INFO "IOCTL_CATS_TestScratchReg_W\n");
		{
			ioctl_arg = 0;

			ret = __get_user(ioctl_arg, (int __user *)arg);

			vio[SCRATCH] = ioctl_arg;
			return 0;
		}

		/*************************************************************************
		* MODULE : FncTESTGetFPGAVer
		* NOTE   : FPGA�o�[�W�����擾�@�\
		**************************************************************************/
		case IOCTL_CATS_TestVersionReadReg:
//			printk(KERN_INFO "IOCTL_CATS_TestVersionReadReg\n");
		{
			tmp = vio[VERSION];
			ret = __put_user(tmp, (int __user *)arg);
			return 0;
		}

		//----------------------------------------------------------------------
		// ???
		//----------------------------------------------------------------------
		default:
			return -ENOTTY;
	}

	return -ENOTTY;
}


