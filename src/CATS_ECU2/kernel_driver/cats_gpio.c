/**********************************************************************************
 * ファイル名      : cats_gpio.c
 * システム名      : x86 プロトタイプＥＣＵ OPE-RA Ver2.0
 * サブシステム名  : カーネルドライバ
 * プログラム名    : ＧＰＩＯ制御
 * バージョン      : Ver1.01
 * モジュール構成  : cats_fpga.c		PCI Access
 *                 : cats_gpio.c		GPIO Control
 * --------------------------------------------------------------------------------
 * 作成者          : 
 * 作成部署        : 
 * 作成日付        : 2009年10月26日 新規作成
 * 更新履歴        : 2010年06月10日
 **********************************************************************************/
#include <linux/pci.h>
#include <asm/uaccess.h>

#include "cats_fpga.h"
#include "cats_files.h"
#include "cats_ioctl.h"


/***********************************************************************************
* MODULE         : catsdrv_gpio_fasync
* ABSTRACT       : CATS/FPGA デバイス・ファイル・[fasync] 操作
* FUNCTION       : 
* ARGUMENT       : 
* NOTE           : 
* RETURN         : 正常終了で「０」を返す。
* CREATE         : 2009/12/10  新規作成 
* UPDATE         : 
***********************************************************************************/
int catsdrv_gpio_fasync(int fd, struct file *file, int mode)
{
//	printk(KERN_INFO "===== cats_module_gpio: fasync\n");

	return fasync_helper(fd, file, mode, &GpioDev.async_queue);
}


/***********************************************************************************
* MODULE         : catsdrv_gpio_open
* ABSTRACT       : CATS/FPGA デバイス・ファイル・[open] 操作
* FUNCTION       : 
* ARGUMENT       : 
* NOTE           : 
* RETURN         : 正常終了で「０」を返す。
* CREATE         : 2009年10月26日  新規作成 
* UPDATE         : 2009年12月10日
***********************************************************************************/
int catsdrv_gpio_open( struct inode *inode, struct file *file )
{
	inode->i_private = inode;
	file->private_data = file;

#if 0
	printk(KERN_INFO "===== cats_module_gpio: open\n");
	printk(KERN_INFO "===== cats_module_gpio: Major %d Minor %d (pid %d)\n",
			imajor(inode),
			iminor(inode),
			current->pid );
#endif
	return 0;
}


/***********************************************************************************
* MODULE         : catsdrv_gpio_release
* ABSTRACT       : CATS/FPGA デバイス・ファイル・[release]操作
* FUNCTION       : 
* ARGUMENT       : 
* NOTE           : 
* RETURN         : 正常終了で「０」を返す。
* CREATE         : 2009年10月26日  新規作成 
* UPDATE         : 2009年12月10日
***********************************************************************************/
int catsdrv_gpio_release( struct inode *inode, struct file *filp )
{
	inode->i_private = inode;
	filp->private_data = filp;

#if 0
	printk(KERN_INFO "===== cats_module_gpio: release\n");
	printk(KERN_INFO "===== cats_module_gpio: Major %d Minor %d (pid %d)\n",
			imajor(inode),
			iminor(inode),
			current->pid );

	printk("===== cats_module_gpio: i_private =%p private_data =%p\n",
			inode->i_private,
			filp->private_data );
#endif

	catsdrv_gpio_fasync(-1, filp, 0);

	return 0;
}


/***********************************************************************************
* MODULE         : catsdrv_gpio_ioctl
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
* CREATE         : 2009年10月26日  新規作成 
* UPDATE         : 2009年10月27日
***********************************************************************************/
int catsdrv_gpio_ioctl( struct inode *inode, struct file *filep,
				   unsigned int cmd, unsigned long arg )
{
	int ret;
	int size;
	volatile int *vio;
	unsigned	long flags;		/* スピンロック用(割り込み状態退避用) */

#if 0
	printk(KERN_INFO "===== cats_module_gpio: ioctl start\n");
	printk(KERN_INFO "===== cats_module_gpio: Major %d Minor %d (pid %d)\n",
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
		// FncinitGPIOOutput
		//
		// int FncinitGPIOOutput ( void );
		//
		case IOCTL_CATS_FncinitGPIOOutput:
//			printk(KERN_INFO "IOCTL_CATS_FncinitGPIOOutput\n");
		{
			// set all outputs to zero
			vio[GPIO_OUT] = 0;

			return 0;
		}

		//----------------------------------------------------------------------
		// FncinitGPIOInput
		//
		// int FncinitGPIOInput ( void );
		//
		case IOCTL_CATS_FncinitGPIOInput:
//			printk(KERN_INFO "IOCTL_CATS_FncinitGPIOInput\n");
		{
			int tmp;

			spin_lock_irqsave( &isr_spinlock, flags );			/* 割り込み禁止開始 */

			// disable interrupts
			tmp = vio[INT_EN];
			vio[INT_EN] = tmp & 0xffff0000;
			tmp = vio[INT_LVL];
			vio[INT_LVL] = tmp & 0xffff0000;

			spin_unlock_irqrestore( &isr_spinlock, flags );		/* 割り込み禁止終了 */

			return 0;
		}

		//----------------------------------------------------------------------
		// FncGPIOSet
		//
		// int FncGPIOSet ( int ch, int val );
		//
		// ioctl_arg = val << 31 | 1 << ch;
		//
		case IOCTL_CATS_FncGPIOSet:
//			printk(KERN_INFO "IOCTL_CATS_FncGPIOSet\n");
		{
			int ioctl_arg;
			int tmp;

			ret = __get_user(ioctl_arg, (int __user *)arg);

			// set gpio output
			tmp = vio[GPIO_OUT];

			if((ioctl_arg & (1<<31))==(1<<31))
				// set bit to 1
				vio[GPIO_OUT] = tmp | (ioctl_arg & 0xffffff);
			else
				// set bit to 0
				vio[GPIO_OUT] = tmp & ~(ioctl_arg);

			return 0;
		}

		//------------------------------------------------------------------------------
		// 	機能			:		FncGPIOSetAll
		//	引数					unsigned int - output value
		//	戻り値			:		int(0-OK)
		//	機能説明		: 		２４チャネル分のＧＰＩＯ出力を設定する
		case IOCTL_CATS_FncGPIOSetAll:
//			printk(KERN_INFO "IOCTL_CATS_FncGPIOSetAll\n");
		{
			int ioctl_arg;

			ret = __get_user(ioctl_arg, (int __user *)arg);
//			printk(KERN_INFO "IOCTL_CATS_FncGPIOSetAll ret = %d val = %x\n", ret ,ioctl_arg);

			vio[GPIO_OUT] = ioctl_arg & 0xFFFFFF;

			return 0;
		}

		//----------------------------------------------------------------------
		// FncGPIOGet
		//
		// int FncGPIOGet ( int ch, int *val );
		//
		case IOCTL_CATS_FncGPIOGet:
//			printk(KERN_INFO "IOCTL_CATS_FncGPIOGet\n");
		{
			int tmp;

			// get gpio status
			tmp = vio[GPIO_IN];

			ret = __put_user(tmp, (int __user *)arg);

			return 0;
		}

		//------------------------------------------------------------------------------
		// 	機能			:		FncGPIOGetAll
		//	引数					unsigned int - output value
		//	戻り値			:		int(0-OK)
		//	機能説明		: 		１６チャネル分ＧＰＩＯ入力データ読み込む
		case IOCTL_CATS_FncGPIOGetAll:
//			printk(KERN_INFO "IOCTL_CATS_FncGPIOGetAll\n");
		{
			int tmp;

			tmp = vio[GPIO_IN] & 0xFFFF;

			ret = __put_user(tmp, (int __user *)arg);

			return 0;
		}

		//----------------------------------------------------------------------
		// FncGPIOIntEnable
		//
		// int FncGPIOIntEnable ( int *sts, int *lvl );
		//
		// ioctl_arg[0] = ioctl_arg[0] | sts[i]<<(15-i);
		// ioctl_arg[1] = ioctl_arg[1] | lvl[i]<<(15-i);
		//
		case IOCTL_CATS_FncGPIOIntEnable:
//			printk(KERN_INFO "IOCTL_CATS_FncGPIOIntEnable\n");
//			printk(KERN_INFO "Before INT_EN = %x, INT_LVL = %x\n",vio[INT_EN],vio[INT_LVL]);
		{
			int *ioctl_arg;
			int tmp;

			ret = __get_user(ioctl_arg, (int * __user *)arg);

			spin_lock_irqsave( &isr_spinlock, flags );			/* 割り込み禁止開始 */

			// set INT_en register (preserve can & mfr)
			tmp = vio[INT_EN];
			tmp = tmp & 0xffff0000;
			vio[INT_EN] = tmp | ioctl_arg[0];

			// set INT_lvl register (preserve can & mfr)
			tmp = vio[INT_LVL];
			tmp = tmp & 0xffff0000;
			vio[INT_LVL] = tmp | ioctl_arg[1];


//			printk(KERN_INFO "After  INT_EN = %x, INT_LVL = %x\n",vio[INT_EN],vio[INT_LVL]);

			spin_unlock_irqrestore( &isr_spinlock, flags );		/* 割り込み禁止終了 */

			return 0;
		}

		//----------------------------------------------------------------------
		// FncGPIOIntSts
		//
		// int FncGPIOIntSts ( int *val );
		//
		case IOCTL_CATS_FncGPIOIntSts:
//			printk(KERN_INFO "IOCTL_CATS_FncGPIOIntSts\n");
//			printk(KERN_INFO "gpio_intsts = %x\n",gpio_intsts);
		{
			int tmp;

			spin_lock_irqsave( &isr_spinlock, flags );			/* 割り込み禁止開始 */

			// get interrupt status
			tmp = gpio_intsts;
			ret = __put_user(tmp, (int __user *)arg);

			spin_unlock_irqrestore( &isr_spinlock, flags );		/* 割り込み禁止終了 */

			return 0;
		}

		//----------------------------------------------------------------------
		// FncGPIOIntClear
		//
		// int FncGPIOIntClear ( unsigned int val );
		//
		case IOCTL_CATS_FncGPIOIntClear:
//			printk(KERN_INFO "IOCTL_CATS_FncGPIOIntClear\n");
//			printk(KERN_INFO "gpio_intsts = %x ioctl_arg = %x\n",int_status,
//								ioctl_arg);
		{
			int ioctl_arg;

			ret = __get_user(ioctl_arg, (int __user *)arg);

			spin_lock_irqsave( &isr_spinlock, flags );			/* 割り込み禁止開始 */
//			printk(KERN_INFO "===== cats_module_gpio: Before gpio_intsts = %x\n", gpio_intsts);

			ioctl_arg = ~ioctl_arg;									/* 指定要因のみクリア */
			gpio_intsts &= ioctl_arg;

//			printk(KERN_INFO "===== cats_module_gpio: After gpio_intsts = %x\n", gpio_intsts);

			spin_unlock_irqrestore( &isr_spinlock, flags );		/* 割り込み禁止終了 */

			return 0;

		}

		/********************************************************************/
		/* 					GPIO 割り込み要求チェック						*/
		/********************************************************************/
		case IOCTL_CATS_CallBackGPIOGetIrq:
//			printk(KERN_INFO "===== cats_module_gpio: IOCTL_CATS_CallBackGPIOGetIrq intirq is %x\n\n",gpio_intirq);
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
//			printk(KERN_INFO "===== cats_module_gpio: IOCTL_CATS_CallBackGPIOClrIrq\n\n");
		{
			int ioctl_arg;

			ret = __get_user(ioctl_arg, (int __user *)arg);

			spin_lock_irqsave( &isr_spinlock, flags );			/* 割り込み禁止開始 */
//			printk(KERN_INFO "===== cats_module_gpio: Before gpio_intirq = %x\n",gpio_intirq);

			ioctl_arg = ~ioctl_arg;									/* 指定要因のみクリア */
			gpio_intirq &= ioctl_arg;

//			printk(KERN_INFO "===== cats_module_gpio: After gpio_intirq = %x\n",gpio_intirq);
			spin_unlock_irqrestore( &isr_spinlock, flags );		/* 割り込み禁止終了 */

//			printk(KERN_INFO "===== cats_module_gpio: IOCTL_CATS_CallBackGPIOClrIrq Val is %x\n\n",ioctl_arg);
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

