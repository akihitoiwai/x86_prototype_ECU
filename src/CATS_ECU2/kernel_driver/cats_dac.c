/**********************************************************************************
 * ファイル名      : cats_dac.c
 * システム名      : x86 プロトタイプＥＣＵ OPE-RA Ver2.0
 * サブシステム名  : カーネルドライバ
 * プログラム名    : ＤＡＣ制御
 * バージョン      : Ver1.00
 * モジュール構成  : cats_fpga.c		PCI Access
 *                 : cats_dac.c			DAC Control
 * --------------------------------------------------------------------------------
 * 作成者          : 
 * 作成部署        : 
 * 作成日付        : 2009年10月21日 新規作成
 * 更新履歴        : 
 **********************************************************************************/
#include <linux/pci.h>
#include <asm/uaccess.h>

#include "cats_fpga.h"
#include "cats_files.h"
#include "cats_ioctl.h"



/***********************************************************************************
* MODULE         : catsdrv_dac_open
* ABSTRACT       : CATS/FPGA デバイス・ファイル・[open] 操作
* FUNCTION       : 
* ARGUMENT       : 
* NOTE           : 
* RETURN         : 正常終了で「０」を返す。
* CREATE         : 2009/10/21  新規作成 
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
* ABSTRACT       : CATS/FPGA デバイス・ファイル・[release]操作
* FUNCTION       : 
* ARGUMENT       : 
* NOTE           : 
* RETURN         : 正常終了で「０」を返す。
* CREATE         : 2009/10/21  新規作成 
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
* ABSTRACT       : CATS/FPGA モジュール・ファイル・[ioctl]操作
* FUNCTION       : 
* ARGUMENT       : 
* NOTE           : 
*                : @param[in]	inode
*                : @param[in]	filep
*                : @param[in]	cmd
*                : @param[in]	arg
*                : @return 正常終了で「０」を返す。
* RETURN         : 正常終了で「０」を返す。
* CREATE         : 2009/10/21  新規作成 
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


