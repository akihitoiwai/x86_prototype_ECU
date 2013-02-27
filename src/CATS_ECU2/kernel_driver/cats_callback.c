/**********************************************************************************
 * �t�@�C����      : cats_callback.c
 * �V�X�e����      : x86 �v���g�^�C�v�d�b�t OPE-RA Ver2.0
 * �T�u�V�X�e����  : �J�[�l���h���C�o
 * �v���O������    : ���荞�݃R�[���o�b�N
 * �o�[�W����      : Ver1.00
 * ���W���[���\��  : cats_fpga.c		PCI Access
 *                 : cats_callback.c	ADC Control
 * --------------------------------------------------------------------------------
 * �쐬��          : 
 * �쐬����        : 
 * �쐬���t        : 2009�N10��20�� �V�K�쐬
 * �X�V����        : 2009�N12�� 7��
 **********************************************************************************/
#include <linux/pci.h>
#include <asm/uaccess.h>

#include "cats_fpga.h"
#include "cats_files.h"
#include "cats_ioctl.h"



/***********************************************************************************
* MODULE         : catsdrv_callback_open
* ABSTRACT       : CATS/FPGA �f�o�C�X�E�t�@�C���E[open] ����
* FUNCTION       : 
* ARGUMENT       : 
* NOTE           : 
* RETURN         : ����I���Łu�O�v��Ԃ��B
* CREATE         : 2009/10/20  �V�K�쐬 
* UPDATE         : 
***********************************************************************************/
int catsdrv_callback_open( struct inode *inode, struct file *file )
{
#if 0
	printk(KERN_INFO "===== cats_module_callback: open\n");
	printk(KERN_INFO "===== cats_module_callback: Major %d Minor %d (pid %d)\n",
			imajor(inode),
			iminor(inode),
			current->pid );
#endif
	return 0;
}


/***********************************************************************************
* MODULE         : catsdrv_callback_release
* ABSTRACT       : CATS/FPGA �f�o�C�X�E�t�@�C���E[release]����
* FUNCTION       : 
* ARGUMENT       : 
* NOTE           : 
* RETURN         : ����I���Łu�O�v��Ԃ��B
* CREATE         : 2009/10/20  �V�K�쐬 
* UPDATE         : 
***********************************************************************************/
int catsdrv_callback_release( struct inode *inode, struct file *file )
{
#if 0
	printk(KERN_INFO "===== cats_module_callback: release\n");
	printk(KERN_INFO "===== cats_module_callback: Major %d Minor %d (pid %d)\n",
			imajor(inode),
			iminor(inode),
			current->pid );

	printk("===== cats_module_callback: i_private =%p private_data =%p\n",
			inode->i_private,
			file->private_data );
#endif
	return 0;
}


/***********************************************************************************
* MODULE         : catsdrv_callback_ioctl
* ABSTRACT       : CATS/FPGA ���W���[���E�t�@�C���E[ioctl]����
* FUNCTION       : 
* ARGUMENT       : 
* NOTE           : 
*                : @param[in]	inode
*                : @param[in]	filep
*                : @param[in]	cmd
*                : @param[in]	arg
* RETURN         : ����I���Łu�O�v��Ԃ��B
* CREATE         : 2009/10/20  �V�K�쐬 
* UPDATE         : 2009/10/29
***********************************************************************************/
int catsdrv_callback_ioctl( struct inode *inode, struct file *filep,
				   unsigned int cmd, unsigned long arg )
{
	int size;
	int ret_val;
	volatile int *vio;

#if 0
	unsigned	long flags;		/* �X�s�����b�N�p(���荞�ݏ�ԑޔ�p) */
	int ret;
#endif

#if 0
	printk(KERN_INFO "===== cats_module_callback: ioctl start\n");
	printk(KERN_INFO "===== cats_module_callback: Major %d Minor %d (pid %d)\n",
			imajor(inode),
			iminor(inode),
			current->pid );
#endif

	ret_val = 0;
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
#if 0
		/********************************************************************/
		/* 					�d���Ď����荞�ݗv���`�F�b�N					*/
		/********************************************************************/
		case IOCTL_CATS_CallBackPowAccGetIrq:
//			printk(KERN_INFO "===== cats_module_callback: IOCTL_CATS_CallBackPowAccGetIrq intirq is %x\n\n",powacc_intirq);
		{
			spin_lock_irqsave( &isr_spinlock, flags );			/* ���荞�݋֎~�J�n */

			ret = __put_user(powacc_intirq, (int __user *)arg);

			spin_unlock_irqrestore( &isr_spinlock, flags );		/* ���荞�݋֎~�I�� */

			return 0;
		}

		/********************************************************************/
		/* 					�d���Ď����荞�ݗv���N���A						*/
		/********************************************************************/
		case IOCTL_CATS_CallBackPowAccClrIrq:
//			printk(KERN_INFO "===== cats_module_callback: IOCTL_CATS_CallBackPowAccClrIrq\n\n");
		{

			int ioctl_arg;

			ret = __get_user(ioctl_arg, (int __user *)arg);

			spin_lock_irqsave( &isr_spinlock, flags );			/* ���荞�݋֎~�J�n */

			if( ioctl_arg == V9OFF_INTERRUPT ){
				powacc_intirq = powacc_intirq & ~(V9OFF_INTERRUPT);

			}else if( ioctl_arg == ACC_INTERRUPT ){
				powacc_intirq = powacc_intirq & ~(ACC_INTERRUPT);

			}

			spin_unlock_irqrestore( &isr_spinlock, flags );		/* ���荞�݋֎~�I�� */


//			printk(KERN_INFO "===== cats_module_callback: IOCTL_CATS_CallBackPowAccClrIrq Val is %x\n\n",ioctl_arg);
			return 0;
		}

		/********************************************************************/
		/* 					GPIO ���荞�ݗv���`�F�b�N						*/
		/********************************************************************/
		case IOCTL_CATS_CallBackGPIOGetIrq:
//			printk(KERN_INFO "===== cats_module_callback: IOCTL_CATS_CallBackGPIOGetIrq intirq is %x\n\n",gpio_intirq);
		{
			spin_lock_irqsave( &isr_spinlock, flags );			/* ���荞�݋֎~�J�n */

			ret = __put_user(gpio_intirq, (int __user *)arg);

			spin_unlock_irqrestore( &isr_spinlock, flags );		/* ���荞�݋֎~�I�� */

			return 0;
		}


		/********************************************************************/
		/* 					GPIO ���荞�ݗv���N���A							*/
		/********************************************************************/
		case IOCTL_CATS_CallBackGPIOClrIrq:
//			printk(KERN_INFO "===== cats_module_callback: IOCTL_CATS_CallBackGPIOClrIrq\n\n");
		{
			int ioctl_arg;

			ret = __get_user(ioctl_arg, (int __user *)arg);

			spin_lock_irqsave( &isr_spinlock, flags );			/* ���荞�݋֎~�J�n */
//			printk(KERN_INFO "===== cats_module_callback: Before gpio_intirq = %x\n",gpio_intirq);

            ioctl_arg = ioctl_arg & MASK_GPIOINT_STS;

			gpio_intirq = gpio_intirq & ioctl_arg;

//			printk(KERN_INFO "===== cats_module_callback: After gpio_intirq = %x\n",gpio_intirq);
			spin_unlock_irqrestore( &isr_spinlock, flags );		/* ���荞�݋֎~�I�� */

//			printk(KERN_INFO "===== cats_module_callback: IOCTL_CATS_CallBackGPIOClrIrq Val is %x\n\n",ioctl_arg);
			return 0;
		}
#endif

		//----------------------------------------------------------------------
		// ???
		//----------------------------------------------------------------------
		default:
			return -ENOTTY;
	}

	return -ENOTTY;
}

