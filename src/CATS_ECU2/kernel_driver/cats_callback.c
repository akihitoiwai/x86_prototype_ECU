/**********************************************************************************
 * ファイル名      : cats_callback.c
 * システム名      : x86 プロトタイプＥＣＵ OPE-RA Ver2.0
 * サブシステム名  : カーネルドライバ
 * プログラム名    : 割り込みコールバック
 * バージョン      : Ver1.00
 * モジュール構成  : cats_fpga.c		PCI Access
 *                 : cats_callback.c	ADC Control
 * --------------------------------------------------------------------------------
 * 作成者          : 
 * 作成部署        : 
 * 作成日付        : 2009年10月20日 新規作成
 * 更新履歴        : 2009年12月 7日
 **********************************************************************************/
#include <linux/pci.h>
#include <asm/uaccess.h>

#include "cats_fpga.h"
#include "cats_files.h"
#include "cats_ioctl.h"



/***********************************************************************************
* MODULE         : catsdrv_callback_open
* ABSTRACT       : CATS/FPGA デバイス・ファイル・[open] 操作
* FUNCTION       : 
* ARGUMENT       : 
* NOTE           : 
* RETURN         : 正常終了で「０」を返す。
* CREATE         : 2009/10/20  新規作成 
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
* ABSTRACT       : CATS/FPGA デバイス・ファイル・[release]操作
* FUNCTION       : 
* ARGUMENT       : 
* NOTE           : 
* RETURN         : 正常終了で「０」を返す。
* CREATE         : 2009/10/20  新規作成 
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
* ABSTRACT       : CATS/FPGA モジュール・ファイル・[ioctl]操作
* FUNCTION       : 
* ARGUMENT       : 
* NOTE           : 
*                : @param[in]	inode
*                : @param[in]	filep
*                : @param[in]	cmd
*                : @param[in]	arg
* RETURN         : 正常終了で「０」を返す。
* CREATE         : 2009/10/20  新規作成 
* UPDATE         : 2009/10/29
***********************************************************************************/
int catsdrv_callback_ioctl( struct inode *inode, struct file *filep,
				   unsigned int cmd, unsigned long arg )
{
	int size;
	int ret_val;
	volatile int *vio;

#if 0
	unsigned	long flags;		/* スピンロック用(割り込み状態退避用) */
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
		/* 					電圧監視割り込み要求チェック					*/
		/********************************************************************/
		case IOCTL_CATS_CallBackPowAccGetIrq:
//			printk(KERN_INFO "===== cats_module_callback: IOCTL_CATS_CallBackPowAccGetIrq intirq is %x\n\n",powacc_intirq);
		{
			spin_lock_irqsave( &isr_spinlock, flags );			/* 割り込み禁止開始 */

			ret = __put_user(powacc_intirq, (int __user *)arg);

			spin_unlock_irqrestore( &isr_spinlock, flags );		/* 割り込み禁止終了 */

			return 0;
		}

		/********************************************************************/
		/* 					電圧監視割り込み要求クリア						*/
		/********************************************************************/
		case IOCTL_CATS_CallBackPowAccClrIrq:
//			printk(KERN_INFO "===== cats_module_callback: IOCTL_CATS_CallBackPowAccClrIrq\n\n");
		{

			int ioctl_arg;

			ret = __get_user(ioctl_arg, (int __user *)arg);

			spin_lock_irqsave( &isr_spinlock, flags );			/* 割り込み禁止開始 */

			if( ioctl_arg == V9OFF_INTERRUPT ){
				powacc_intirq = powacc_intirq & ~(V9OFF_INTERRUPT);

			}else if( ioctl_arg == ACC_INTERRUPT ){
				powacc_intirq = powacc_intirq & ~(ACC_INTERRUPT);

			}

			spin_unlock_irqrestore( &isr_spinlock, flags );		/* 割り込み禁止終了 */


//			printk(KERN_INFO "===== cats_module_callback: IOCTL_CATS_CallBackPowAccClrIrq Val is %x\n\n",ioctl_arg);
			return 0;
		}

		/********************************************************************/
		/* 					GPIO 割り込み要求チェック						*/
		/********************************************************************/
		case IOCTL_CATS_CallBackGPIOGetIrq:
//			printk(KERN_INFO "===== cats_module_callback: IOCTL_CATS_CallBackGPIOGetIrq intirq is %x\n\n",gpio_intirq);
		{
			spin_lock_irqsave( &isr_spinlock, flags );			/* 割り込み禁止開始 */

			ret = __put_user(gpio_intirq, (int __user *)arg);

			spin_unlock_irqrestore( &isr_spinlock, flags );		/* 割り込み禁止終了 */

			return 0;
		}


		/********************************************************************/
		/* 					GPIO 割り込み要求クリア							*/
		/********************************************************************/
		case IOCTL_CATS_CallBackGPIOClrIrq:
//			printk(KERN_INFO "===== cats_module_callback: IOCTL_CATS_CallBackGPIOClrIrq\n\n");
		{
			int ioctl_arg;

			ret = __get_user(ioctl_arg, (int __user *)arg);

			spin_lock_irqsave( &isr_spinlock, flags );			/* 割り込み禁止開始 */
//			printk(KERN_INFO "===== cats_module_callback: Before gpio_intirq = %x\n",gpio_intirq);

            ioctl_arg = ioctl_arg & MASK_GPIOINT_STS;

			gpio_intirq = gpio_intirq & ioctl_arg;

//			printk(KERN_INFO "===== cats_module_callback: After gpio_intirq = %x\n",gpio_intirq);
			spin_unlock_irqrestore( &isr_spinlock, flags );		/* 割り込み禁止終了 */

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

