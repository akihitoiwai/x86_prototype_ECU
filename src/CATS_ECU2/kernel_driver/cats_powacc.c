/**********************************************************************************
 * ファイル名      : cats_powacc.c
 * システム名      : x86 プロトタイプＥＣＵ OPE-RA Ver2.0
 * サブシステム名  : カーネルドライバ
 * プログラム名    : 電源、ＡＣＣ電圧監視制御
 * バージョン      : Ver1.00
 * モジュール構成  : cats_fpga.c		PCI Access
 *                 : cats_powacc.c		POW,ACC Control
 * --------------------------------------------------------------------------------
 * 作成者          : 
 * 作成部署        : 
 * 作成日付        : 2009年08月26日 新規作成
 * 更新履歴        : 2009年12月10日
 **********************************************************************************/
#include <linux/pci.h>
#include <asm/uaccess.h>
#include <linux/fs.h>

#include "cats_fpga.h"
#include "cats_files.h"
#include "cats_ioctl.h"


/***********************************************************************************
* MODULE         : catsdrv_powacc_fasync
* ABSTRACT       : CATS/FPGA デバイス・ファイル・[fasync] 操作
* FUNCTION       : 
* ARGUMENT       : 
* NOTE           : 
* RETURN         : 正常終了で「０」を返す。
* CREATE         : 2009/11/16  新規作成 
* UPDATE         : 2009/12/ 4
***********************************************************************************/
int catsdrv_powacc_fasync(int fd, struct file *file, int mode)
{
//	printk(KERN_INFO "===== cats_module_powacc: fasync\n");

	return fasync_helper(fd, file, mode, &PowaccDev.async_queue);
}


/***********************************************************************************
* MODULE         : catsdrv_powacc_open
* ABSTRACT       : CATS/FPGA デバイス・ファイル・[open] 操作
* FUNCTION       : 
* ARGUMENT       : 
* NOTE           : 
* RETURN         : 正常終了で「０」を返す。
* CREATE         : 2009/08/26  新規作成 
* UPDATE         : 2009/12/ 1
***********************************************************************************/
int catsdrv_powacc_open( struct inode *inode, struct file *file )
{
	inode->i_private = inode;
	file->private_data = file;

#if 0
	printk(KERN_INFO "===== cats_module_powacc: open\n");
	printk(KERN_INFO "===== cats_module_powacc: Major %d Minor %d (pid %d)\n",
			imajor(inode),
			iminor(inode),
			current->pid );

	printk("===== cats_module_powacc: i_private =%p private_data =%p\n",
			inode->i_private,
			file->private_data );
#endif
	return 0;
}

/***********************************************************************************
* MODULE         : catsdrv_powacc_release
* ABSTRACT       : CATS/FPGA デバイス・ファイル・[release]操作
* FUNCTION       : 
* ARGUMENT       : 
* NOTE           : 
* RETURN         : 正常終了で「０」を返す。
* CREATE         : 2009/08/26  新規作成 
* UPDATE         : 2009/12/ 1
***********************************************************************************/
int catsdrv_powacc_release( struct inode *inode, struct file *filp )
{
//	struct flexdev *pPowaccDev;

	inode->i_private = inode;
	filp->private_data = filp;

#if 0
	printk(KERN_INFO "===== cats_module_powacc: release\n");
	printk(KERN_INFO "===== cats_module_powacc: Major %d Minor %d (pid %d)\n",
			imajor(inode),
			iminor(inode),
			current->pid );

	printk("===== cats_module_powacc: i_private =%p private_data =%p\n",
			inode->i_private,
			filp->private_data );
#endif

	catsdrv_powacc_fasync(-1, filp, 0);

	return 0;
}


/***********************************************************************************
* MODULE         : catsdrv_powacc_ioctl
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
* UPDATE         : 2009/10/27
***********************************************************************************/
int catsdrv_powacc_ioctl( struct inode *inode, struct file *filep,
				   unsigned int cmd, unsigned long arg )
{
	int ret;
	int size;
	int ret_val;
	volatile int *vio;
	unsigned	long flags;		/* スピンロック用(割り込み状態退避用) */

#if 0
	printk(KERN_INFO "===== cats_module_powacc: ioctl start\n");
	printk(KERN_INFO "===== cats_module_powacc: Major %d Minor %d (pid %d)\n",
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

		//----------------------------------------------------------------------
		// FncPowerMoniIntEnable
		//
		// int FncPowerMoniIntEnable ( unsigned char sts );
		//
		case IOCTL_CATS_FncPowerMoniIntEnable:
//			printk(KERN_INFO "===== cats_module_powacc: IOCTL_CATS_FncPowerMoniIntEnable\n\n");
		{
			int ioctl_arg;
			int tmp;

			ret = __get_user(ioctl_arg, (int __user *)arg);

			spin_lock_irqsave( &isr_spinlock, flags );			/* 割り込み禁止開始 */

			// set power interrupt level to 1
			tmp = vio[INT_LVL];
			vio[INT_LVL] = tmp | V9OFF_INTERRUPT;

			// enable/disable interrupt
			tmp = vio[INT_EN];

			if(ioctl_arg==0){
       	    		vio[INT_EN] = tmp & ~(V9OFF_INTERRUPT);
			}else if(ioctl_arg==1){
	           		vio[INT_EN] = tmp | V9OFF_INTERRUPT;
			}

			spin_unlock_irqrestore( &isr_spinlock, flags );		/* 割り込み禁止終了 */

			return 0;
		}

		//----------------------------------------------------------------------
		// FncPowerMoniIntSts
		//
		// int FncPowerMoniIntSts ( unsigned int *sts );
		//
		case IOCTL_CATS_FncPowerMoniIntSts:
//			printk(KERN_INFO "===== cats_module_powacc: IOCTL_CATS_FncPowerMoniIntSts\n\n");
		{
			int tmp;

			spin_lock_irqsave( &isr_spinlock, flags );			/* 割り込み禁止開始 */

			// get interrupt status
			if( (powacc_intsts & V9OFF_INTERRUPT) == 0 ){
				tmp = 0;
			}else{
				tmp = 1;
			}
			spin_unlock_irqrestore( &isr_spinlock, flags );		/* 割り込み禁止終了 */

			ret = __put_user(tmp, (int __user *)arg);


			return 0;
		}

		//----------------------------------------------------------------------
		// FncPowerMoniIntClear
		//
		// int FncPowerMoniIntClear ( void );
		//
		case IOCTL_CATS_FncPowerMoniIntClear:
//			printk(KERN_INFO "===== cats_module_powacc: IOCTL_CATS_FncPowerMoniIntClear\n\n");
		{
			spin_lock_irqsave( &isr_spinlock, flags );			/* 割り込み禁止開始 */

            powacc_intsts = powacc_intsts & ~(V9OFF_INTERRUPT);

			spin_unlock_irqrestore( &isr_spinlock, flags );		/* 割り込み禁止終了 */

			return 0;
		}

		//----------------------------------------------------------------------
		// FncAccMoniIntEnable
		//
		// int FncAccMoniIntEnable ( unsigned char sts );
		//
		case IOCTL_CATS_FncAccMoniIntEnable:
//			printk(KERN_INFO "===== cats_module_powacc: IOCTL_CATS_FncAccMoniIntEnable\n\n");
		{
			int ioctl_arg;
			int tmp;

			ret = __get_user(ioctl_arg, (int __user *)arg);

			spin_lock_irqsave( &isr_spinlock, flags );			/* 割り込み禁止開始 */

			// set power interrupt level to 1
			tmp = vio[INT_LVL];
			vio[INT_LVL] = tmp | ACC_INTERRUPT;

			// enable/disable interrupt
			tmp = vio[INT_EN];

			if(ioctl_arg==0){
    	       		vio[INT_EN] = tmp & ~(ACC_INTERRUPT);
			}else if(ioctl_arg==1){
    	        		vio[INT_EN] = tmp | ACC_INTERRUPT;
			}

			spin_unlock_irqrestore( &isr_spinlock, flags );		/* 割り込み禁止終了 */

			return 0;
		}

		//----------------------------------------------------------------------
		// FncAccMoniIntSts
		//
		// int FncAccMoniIntSts ( unsigned int *sts );
		//
		case IOCTL_CATS_FncAccMoniIntSts:
//			printk(KERN_INFO "===== cats_module_powacc: IOCTL_CATS_FncAccMoniIntSts\n\n");
		{
			int tmp;

			spin_lock_irqsave( &isr_spinlock, flags );			/* 割り込み禁止開始 */

			// get interrupt status
			if( (powacc_intsts & ACC_INTERRUPT) == 0 ){
				tmp = 0;
			}else{
				tmp = 1;
			}

			spin_unlock_irqrestore( &isr_spinlock, flags );		/* 割り込み禁止終了 */

			ret = __put_user(tmp, (int __user *)arg);

			return 0;
		}

		//----------------------------------------------------------------------
		// FncAccMoniIntClear
		//
		// int FncAccMoniIntClear ( void );
		//
		case IOCTL_CATS_FncAccMoniIntClear:
//			printk(KERN_INFO "===== cats_module_powacc: IOCTL_CATS_FncAccMoniIntClear\n\n");
		{
			spin_lock_irqsave( &isr_spinlock, flags );			/* 割り込み禁止開始 */

            powacc_intsts = powacc_intsts & ~(ACC_INTERRUPT);

			spin_unlock_irqrestore( &isr_spinlock, flags );		/* 割り込み禁止終了 */

			return 0;
		}

		/********************************************************************/
		/* 					電圧監視割り込み要求チェック					*/
		/********************************************************************/
		case IOCTL_CATS_CallBackPowAccGetIrq:
//			printk(KERN_INFO "===== cats_module_powacc: IOCTL_CATS_CallBackPowAccGetIrq intirq is %x\n\n",powacc_intirq);
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
//			printk(KERN_INFO "===== cats_module_powacc: IOCTL_CATS_CallBackPowAccClrIrq\n\n");
		{
			int ioctl_arg;

			ret = __get_user(ioctl_arg, (int __user *)arg);

			spin_lock_irqsave( &isr_spinlock, flags );			/* 割り込み禁止開始 */

			if( ioctl_arg == V9OFF_INTERRUPT ){
				powacc_intirq = powacc_intirq & ~(V9OFF_INTERRUPT);
//				printk(KERN_INFO "===== cats_module_powacc: V9OFF_INTERRUPT\n");

			}else if( ioctl_arg == ACC_INTERRUPT ){
				powacc_intirq = powacc_intirq & ~(ACC_INTERRUPT);
//				printk(KERN_INFO "===== cats_module_powacc: ACC_INTERRUPT\n");
			}

			spin_unlock_irqrestore( &isr_spinlock, flags );		/* 割り込み禁止終了 */
//			printk(KERN_INFO "===== cats_module_powacc: IOCTL_CATS_CallBackPowAccClrIrq Val is %x\n\n",ioctl_arg);
//			printk(KERN_INFO "===== cats_module_powacc: IOCTL_CATS_CallBackPowAccClrIrq powacc_intirq is %x\n\n",powacc_intirq);
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

