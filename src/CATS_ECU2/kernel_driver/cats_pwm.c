/**********************************************************************************
 * �t�@�C����      : cats_pwm.c
 * �V�X�e����      : x86 �v���g�^�C�v�d�b�t OPE-RA Ver2.0�p�J�[�l���h���C�o
 * �T�u�V�X�e����  : PWM �@�\
 * �v���O������    : 
 * CPU TYPE        : 
 * �o�[�W����      : Ver1.00
 * ���W���[���\��  : cats_fpga.c		PCI Access
 *                 : cats_pwm			PWM Control
 * --------------------------------------------------------------------------------
 * �쐬��          : 
 * �쐬����        : 
 * �쐬���t        : 2009�N10��30�� �V�K�쐬
 * �X�V����        : 
 **********************************************************************************/
 
#include <linux/pci.h>
#include <asm/uaccess.h>

#include "cats_fpga.h"
#include "cats_files.h"
#include "cats_ioctl.h"



/***********************************************************************************
* MODULE         : catsdrv_pwm_open
* ABSTRACT       : CATS/FPGA �f�o�C�X�E�t�@�C���E[open] ����
* FUNCTION       : 
* ARGUMENT       : 
* NOTE           : 
* RETURN         : ����I���Łu�O�v��Ԃ��B
* CREATE         : 2009�N10��30��  �V�K�쐬 
* UPDATE         : 
***********************************************************************************/
int catsdrv_pwm_open( struct inode *inode, struct file *file )
{
#if 0
	printk(KERN_INFO "===== cats_module_pwm: open\n");
	printk(KERN_INFO "===== cats_module_pwm: Major %d Minor %d (pid %d)\n",
			imajor(inode),
			iminor(inode),
			current->pid );
#endif
	return 0;
}


/***********************************************************************************
* MODULE         : catsdrv_pwm_release
* ABSTRACT       : CATS/FPGA �f�o�C�X�E�t�@�C���E[release]����
* FUNCTION       : 
* ARGUMENT       : 
* NOTE           : 
* RETURN         : ����I���Łu�O�v��Ԃ��B
* CREATE         : 2009�N10��30��  �V�K�쐬 
* UPDATE         : 
***********************************************************************************/
int catsdrv_pwm_release( struct inode *inode, struct file *file )
{
#if 0
	printk(KERN_INFO "===== cats_module_pwm: release\n");
	printk(KERN_INFO "===== cats_module_pwm: Major %d Minor %d (pid %d)\n",
			imajor(inode),
			iminor(inode),
			current->pid );

	printk("===== cats_module_pwm: i_private =%p private_data =%p\n",
			inode->i_private,
			file->private_data );
#endif
	return 0;
}


/***********************************************************************************
* MODULE         : catsdrv_pwm_ioctl
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
* CREATE         : 2009�N10��30��  �V�K�쐬 
* UPDATE         : 
***********************************************************************************/
int catsdrv_pwm_ioctl( struct inode *inode, struct file *filep,
				   unsigned int cmd, unsigned long arg )
{
	int ret;
	int size;
	volatile int *vio;

#if 0
	printk(KERN_INFO "===== cats_module_pwm: ioctl start\n");
	printk(KERN_INFO "===== cats_module_pwm: Major %d Minor %d (pid %d)\n",
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
		// FncInitPWM
		//
		// int FncInitPWM (int *val );
		//
		// ioctl_arg = val
		//
		case IOCTL_CATS_FncInitPWM:
//			printk(KERN_INFO "IOCTL_CATS_FncInitPWM\n");
		{
			int *ioctl_arg;

			ret = __get_user(ioctl_arg, (int * __user *)arg);

			// set PWM registers
			vio[PWM_DIV] = ioctl_arg[0];
			vio[PWM_CYCL] = ioctl_arg[1];
			vio[PWM_CYCU] = ioctl_arg[2];

//			printk(KERN_INFO "PWM_DIV = %x, PWM_CYCL = %x, PWM_CYCU = %x\n",
//								ioctl_arg[0],ioctl_arg[1],	ioctl_arg[2]);
			return 0;
		}

		//----------------------------------------------------------------------
		// FncPWMEnable
		//
		// int FncPWMEnable ( int *val );
		//
		// ioctl_arg = ioctl_arg | val[i]<<i;
		//
		case IOCTL_CATS_FncPWMEnable:
//			printk(KERN_INFO "IOCTL_CATS_FncPWMEnable\n");
		{
			int ioctl_arg;

			ret = __get_user(ioctl_arg, (int __user *)arg);

			// set PWM_en bits
			vio[PWM_EN] = ioctl_arg;

//			printk(KERN_INFO "PWM_EN = %x\n",ioctl_arg);

			return 0;
		}

		//----------------------------------------------------------------------
		// FncPWMSet
		//
		// int FncPWMSet (int ch, int val );
		//
		// ioctl_arg[0] = ch ;
		// ioctl_arg[1] = val;
		//
		case IOCTL_CATS_FncPWMSet:
//			printk(KERN_INFO "IOCTL_CATS_FncPWMSet\n");
		{
			int *ioctl_arg;
			int tmp;

			ret = __get_user(ioctl_arg, (int * __user *)arg);

			// set ch bits of PWM_HWC(L/U), preserve other values 
			if(ioctl_arg[0]<4){
				// for channels 1-4 (0-3)
				tmp = vio[PWM_HWCL];
				tmp = tmp & ~(0xff << (ioctl_arg[0] * 8));
				vio[PWM_HWCL] = tmp | (ioctl_arg[1] << (ioctl_arg[0] * 8));
			} else {
				// for channels 5-8 (4-7)
				tmp = vio[PWM_HWCU];
				tmp = tmp & ~(0xff << ((ioctl_arg[0] - 4) * 8));
				vio[PWM_HWCU] = tmp | (ioctl_arg[1] << ((ioctl_arg[0] - 4) * 8));
			}

//			printk(KERN_INFO "PWM_HWCL = %x, PWM_HWCU = %x\n",
//							vio[PWM_HWCL],vio[PWM_HWCU]);
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

