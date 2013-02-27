/**********************************************************************************
 * ファイル名      : cats_adc.c
 * システム名      : x86 プロトタイプＥＣＵ OPE-RA Ver2.0
 * サブシステム名  : カーネルドライバ
 * プログラム名    : ＡＤＣ制御
 * バージョン      : Ver1.00
 * モジュール構成  : cats_fpga.c		PCI Access
 *                 : cats_adc.c			ADC Control
 * --------------------------------------------------------------------------------
 * 作成者          : 
 * 作成部署        : 
 * 作成日付        : 2009年08月24日 新規作成
 * 更新履歴        : 2010年01月28日
 **********************************************************************************/
#include <linux/pci.h>
#include <asm/uaccess.h>
#include <linux/delay.h>

#include "cats_fpga.h"
#include "cats_files.h"
#include "cats_ioctl.h"



/***********************************************************************************
* MODULE         : catsdrv_adc_open
* ABSTRACT       : CATS/FPGA デバイス・ファイル・[open] 操作
* FUNCTION       : 
* ARGUMENT       : 
* NOTE           : 
* RETURN         : 正常終了で「０」を返す。
* CREATE         : 2009/08/24  新規作成 
* UPDATE         : 
***********************************************************************************/
int catsdrv_adc_open( struct inode *inode, struct file *file )
{
#if 0
	printk(KERN_INFO "===== cats_module_adc: open\n");
	printk(KERN_INFO "===== cats_module_adc: Major %d Minor %d (pid %d)\n",
			imajor(inode),
			iminor(inode),
			current->pid );
#endif
	return 0;
}


/***********************************************************************************
* MODULE         : catsdrv_adc_release
* ABSTRACT       : CATS/FPGA デバイス・ファイル・[release]操作
* FUNCTION       : 
* ARGUMENT       : 
* NOTE           : 
* RETURN         : 正常終了で「０」を返す。
* CREATE         : 2009/08/24  新規作成 
* UPDATE         : 
***********************************************************************************/
int catsdrv_adc_release( struct inode *inode, struct file *file )
{
#if 0
	printk(KERN_INFO "===== cats_module_adc: release\n");
	printk(KERN_INFO "===== cats_module_adc: Major %d Minor %d (pid %d)\n",
			imajor(inode),
			iminor(inode),
			current->pid );

	printk("===== cats_module_adc: i_private =%p private_data =%p\n",
			inode->i_private,
			file->private_data );
#endif
	return 0;
}


/***********************************************************************************
* MODULE         : catsdrv_adc_ioctl
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
* CREATE         : 2009/08/24  新規作成 
* UPDATE         : 
***********************************************************************************/
int catsdrv_adc_ioctl( struct inode *inode, struct file *filep,
				   unsigned int cmd, unsigned long arg )
{
	int ret;
	int size;
	volatile int *vio;

#if 0
	printk(KERN_INFO "===== cats_module_adc: ioctl start\n");
	printk(KERN_INFO "===== cats_module_adc: Major %d Minor %d (pid %d)\n",
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
		// FncInitADC
		//
		// int FncInitADC ( void );
		//
		case IOCTL_CATS_FncInitADC:
//			printk(KERN_INFO "===== cats_module_adc: IOCTL_CATS_FncInitADC\n\n");
		{
			vio[ADC_CTL] = 0;			//stop ADC
			udelay(64);					//wait for stop ADC

			return 0;
		}

		//----------------------------------------------------------------------
		// FncADCStartOneShot
		//
		// int FncADCStartOneShot ( int );
		//
		case IOCTL_CATS_FncADCStartOneShot:
//			printk(KERN_INFO "===== cats_module_adc: IOCTL_CATS_FncADCStartOneShot\n\n");
		{
			int ioctl_arg;
			int tmp;

			ret = __get_user(ioctl_arg, (int __user *)arg);
			tmp = vio[ADC_CTL];
			tmp = tmp & 0xF8000000;

			// if device is busy, return busy error code
			if((tmp & ADC_BUSY)==ADC_BUSY)
			return CATS_ERR_BUSY;

			// applied same to one shot
			if((ioctl_arg>>3)==0)
				tmp = tmp & ~(ADC0_SHUTDOWN);
			else
				tmp = tmp & ~(ADC1_SHUTDOWN);

			// set to one shot mode
			vio[ADC_CTL] = tmp | ADC_READ_ENABLE | ADC_ONESHOT_MODE
							 | ioctl_arg;

			return 0;
		}

		//----------------------------------------------------------------------
		// FncADCStartContinuous
		//
		// int FncADCStartContinuous ( void );
		//
		case IOCTL_CATS_FncADCStartContinuous:
//			printk(KERN_INFO "===== cats_module_adc: IOCTL_CATS_FncADCStartContinuous\n\n");
		{
			int tmp;

			tmp = vio[ADC_CTL];

			tmp = tmp & ~(ADC0_SHUTDOWN | ADC1_SHUTDOWN);

			// set to countinuous mode
			tmp = tmp | ADC_READ_ENABLE;
			vio[ADC_CTL] = tmp & ~(ADC_ONESHOT_MODE);

			return 0;
		}

		//----------------------------------------------------------------------
		// FncADCStopContinuous
		//
		// int FncADCStopContinuous ( void );
		//
		case IOCTL_CATS_FncADCStopContinuous:
//			printk(KERN_INFO "===== cats_module_adc: IOCTL_CATS_FncADCStopContinuous\n\n");
		{
			vio[ADC_CTL] = 0;			//stop ADC
			udelay(64);					//wait for stop ADC

			return 0;
		}

		//----------------------------------------------------------------------
		// FncADCGetOneShot
		//
		// int FncADCGetOneShot ( int ch, unsigned int *val );
		//
		// ioctl_arg = ch - 1;
		//
		case IOCTL_CATS_FncADCGetOneShot:
//			printk(KERN_INFO "===== cats_module_adc: IOCTL_CATS_FncADCGetOneShot\n\n");
		{
			int ioctl_arg;
			int tmp = 0;
			unsigned int timeout = 0;

			ret = __get_user(ioctl_arg, (int __user *)arg);

			// if device is busy, return busy error code
			if((vio[ADC_CTL] & ADC_BUSY)==ADC_BUSY){
//				printk(KERN_WARNING "catsfpga: CATS_ERR_BUSY\n");
				return CATS_ERR_BUSY;
			}

			// wait for data valid
			while((tmp & ADC_DATA_VALID)!=ADC_DATA_VALID){
				tmp = vio[(ioctl_arg + 1)];
				// timeout, in case data never becomes valid
				timeout++;
				if(timeout>105000){
					tmp = 0x10000;
					ret = __put_user(tmp, (int __user *)arg);
					return -1;
				}
			}

			// ADC is only 16bits
			tmp = tmp & 0xffff;

			ret = __put_user(tmp, (int __user *)arg);

			return 0;
		}

		//----------------------------------------------------------------------
		// IOCTL_CATS_FncADCGetContinuous
		//
		// int FncADCGetContinuous ( int *val );
		//
		// ioctl_arg = ch - 1;
		//
		case IOCTL_CATS_FncADCGetContinuous:
//			printk(KERN_INFO "===== cats_module_adc: IOCTL_CATS_FncADCGetContinuous\n\n");
		{
			int ioctl_arg;
			int tmp = 0;
			unsigned int timeout = 0;

			ret = __get_user(ioctl_arg, (int __user *)arg);

			// wait for data valid
			while((tmp & ADC_DATA_VALID)!=ADC_DATA_VALID){
				tmp = vio[(ioctl_arg + 1)];
				// timeout, in case data never becomes valid
				timeout++;
				if(timeout>105000){
					tmp = 0x10000;
					ret = __put_user(tmp, (int __user *)arg);
					return -1;
				}
			}

			// ADC is only 16bits
			tmp = tmp & 0xffff;

			ret = __put_user(tmp, (int __user *)arg);

			return 0;
		}

		//----------------------------------------------------------------------
		// FncDisableADC
		//
		// int FncDisableADC ( void );
		//
		case IOCTL_CATS_FncDisableADC:
//			printk(KERN_INFO "===== cats_module_adc: IOCTL_CATS_FncDisableADC\n\n");
		{
			vio[ADC_CTL] = 0;			//stop ADC
			udelay(64);					//wait for stop ADC

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


