/**********************************************************************************
 * �t�@�C����      : cats_dac.c
 * �V�X�e����      : x86 �v���g�^�C�v�d�b�t OPE-RA Ver2.0
 * �T�u�V�X�e����  : �J�[�l���h���C�o
 * �v���O������    : �c�`�b����
 * �o�[�W����      : Ver1.00
 * ���W���[���\��  : cats_fpga.c		PCI Access
 *                 : cats_dac.c			DAC Control
 * --------------------------------------------------------------------------------
 * �쐬��          : 
 * �쐬����        : 
 * �쐬���t        : 2009�N10��21�� �V�K�쐬
 * �X�V����        : 
 **********************************************************************************/
#include <linux/pci.h>
#include <asm/uaccess.h>

#include "cats_fpga.h"
#include "cats_files.h"
#include "cats_ioctl.h"



/***********************************************************************************
* MODULE         : catsdrv_dac_open
* ABSTRACT       : CATS/FPGA �f�o�C�X�E�t�@�C���E[open] ����
* FUNCTION       : 
* ARGUMENT       : 
* NOTE           : 
* RETURN         : ����I���Łu�O�v��Ԃ��B
* CREATE         : 2009/10/21  �V�K�쐬 
* UPDATE         : 
***********************************************************************************/
int catsdrv_dac_open( struct inode *inode, struct file *file )
{
#if 0
	printk(KERN_INFO "===== cats_module_dac: open\n");
	printk(KERN_INFO "===== cats_module_dac: Major %d Minor %d (pid %d)\n",
			imajor(inode),
			iminor(inode),
			current->pid );
#endif
	return 0;
}


/***********************************************************************************
* MODULE         : catsdrv_dac_release
* ABSTRACT       : CATS/FPGA �f�o�C�X�E�t�@�C���E[release]����
* FUNCTION       : 
* ARGUMENT       : 
* NOTE           : 
* RETURN         : ����I���Łu�O�v��Ԃ��B
* CREATE         : 2009/10/21  �V�K�쐬 
* UPDATE         : 
***********************************************************************************/
int catsdrv_dac_release( struct inode *inode, struct file *file )
{
#if 0
	printk(KERN_INFO "===== cats_module_dac: release\n");
	printk(KERN_INFO "===== cats_module_dac: Major %d Minor %d (pid %d)\n",
			imajor(inode),
			iminor(inode),
			current->pid );

	printk("===== cats_module_dac: i_private =%p private_data =%p\n",
			inode->i_private,
			file->private_data );
#endif
	return 0;
}


/***********************************************************************************
* MODULE         : catsdrv_dac_ioctl
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
* CREATE         : 2009/10/21  �V�K�쐬 
* UPDATE         : 
***********************************************************************************/
int catsdrv_dac_ioctl( struct inode *inode, struct file *filep,
				   unsigned int cmd, unsigned long arg )
{
	int ret;
	int size;
	volatile int *vio;
#if 0
	printk(KERN_INFO "===== cats_module_dac: ioctl start\n");
	printk(KERN_INFO "===== cats_module_dac: Major %d Minor %d (pid %d)\n",
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
		//----------------------------------------------------------------------
		// FncInitDAC
		//
		// int FncInitDAC ( void );
		//
		case IOCTL_CATS_FncInitDAC:
//			printk(KERN_INFO "IOCTL_CATS_FncInitDAC\n");
		{
			// enable read_en
			vio[DAC_CTL] = DAC_READ_ENABLE;

			return 0;
		}

		//----------------------------------------------------------------------
		// FncDACSet
		//
		// int FncDACSet ( int *val );
		//
		// ioctl_arg = (i << 16) | val[i];
		//
		case IOCTL_CATS_FncDACSet:
//			printk(KERN_INFO "IOCTL_CATS_FncDACSet\n");
		{
			int ioctl_arg;

			ret = __get_user(ioctl_arg, (int __user *)arg);

			// set DAC control and data register
			vio[DAC_CTL] = DAC_READ_ENABLE | ioctl_arg;

			return 0;
		}

		//----------------------------------------------------------------------
		// FncDACStop
		//
		// int FncDACStop ( void );
		//
		case IOCTL_CATS_FncDACStop:
//			printk(KERN_INFO "IOCTL_CATS_FncDACStop\n");
		{
			int tmp;

			tmp = vio[DAC_CTL];

			// disable read_en
			vio[DAC_CTL] = tmp & ~(DAC_READ_ENABLE);

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


