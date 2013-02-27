/**********************************************************************************
 * ファイル名      : cats_can.c
 * システム名      : x86 プロトタイプＥＣＵ OPE-RA Ver2.0
 * サブシステム名  : カーネルドライバ
 * プログラム名    : ＣＡＮ制御
 * バージョン      : Ver1.01
 * モジュール構成  : cats_fpga.c		PCI Access
 *                 : cats_can.c			CAN Control
 *                 : cats_sja1000.c		SJA1000 Control
 * --------------------------------------------------------------------------------
 * 作成者          : 
 * 作成部署        : 
 * 作成日付        : 2009年09月04日 新規作成
 * 更新履歴        : 2010年02月15日
 **********************************************************************************/
#include <linux/pci.h>
#include <asm/uaccess.h>

#include "cats_fpga.h"
#include "cats_files.h"
#include "cats_ioctl.h"
#include "cats_sja1000.h"


/***********************************************************************************
* MODULE         : catsdrv_can_open
* ABSTRACT       : CATS/FPGA デバイス・ファイル・[open] 操作
* FUNCTION       : 
* ARGUMENT       : 
* NOTE           : 
* RETURN         : 正常終了で「０」を返す。
* CREATE         : 2009/09/04  新規作成 
* UPDATE         : 
***********************************************************************************/
int catsdrv_can_open( struct inode *inode, struct file *file )
{
	sja1000_create();

#ifdef TASK_LET_USE
	spin_lock_init( &tasklet_spinlock0 );
	spin_lock_init( &tasklet_spinlock1 );
	spin_lock_init( &tasklet_spinlock2 );
	spin_lock_init( &tasklet_spinlock3 );

	intr_st_get = intr_st_put = 0;
#endif

#ifdef OPEN_CLOSE_INTR_INSTALL
	catsdrv_interrupt_install();
#endif

	return 0;
}


/***********************************************************************************
* MODULE         : catsdrv_can_release
* ABSTRACT       : CATS/FPGA デバイス・ファイル・[release]操作
* FUNCTION       : 
* ARGUMENT       : 
* NOTE           : 
* RETURN         : 正常終了で「０」を返す。
* CREATE         : 2009/09/04  新規作成 
* UPDATE         : 
***********************************************************************************/
int catsdrv_can_release( struct inode *inode, struct file *file )
{
	int i;

	for(i = 0; i < 4; ++i) {
		sja1000_stop(pci_adr_map, i);
		sja1000_deinit(pci_adr_map, i);
	}

#ifdef OPEN_CLOSE_INTR_INSTALL
	catsdrv_interrupt_uninstall();
#endif

#ifdef TASK_LET_USE
	tasklet_kill( &sja1000_intr_tasklet0 );
	tasklet_kill( &sja1000_intr_tasklet1 );
	tasklet_kill( &sja1000_intr_tasklet2 );
	tasklet_kill( &sja1000_intr_tasklet3 );
#endif

#ifdef OPEN_CLOSE_INTR_INSTALL
	sja1000_destroy();
#endif

	return 0;
}


/***********************************************************************************
* MODULE         : catsdrv_can_ioctl
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
* CREATE         : 2009/09/04  新規作成 
* UPDATE         : 2010/02/15
***********************************************************************************/
int catsdrv_can_ioctl( struct inode *inode, struct file *filep,
				   unsigned int cmd, unsigned long arg )
{
//	int ret;
	int size;
	int ret_val;
//	volatile int *vio = (volatile int *)pci_adr_map;

	ret_val = 0;

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
		// IOCTL_CATS_CAN_read_register
		//----------------------------------------------------------------------
		case IOCTL_CATS_CAN_read_register:
			{
				CAN_REGISTER reg;

				if( !access_ok(VERIFY_READ, (void *)arg, sizeof(CAN_REGISTER))) {
					ret_val = -ENOTTY;
					break;
      			}

				if( !access_ok(VERIFY_WRITE, (void *)arg, sizeof(CAN_REGISTER))) {
					ret_val = -ENOTTY;
					break;
				}

				__copy_from_user((void *)&reg,
					(const CAN_REGISTER *)arg, sizeof(CAN_REGISTER));
				reg.data = sja1000_read(pci_adr_map, reg.device, reg.address);
				ret_val = __copy_to_user((CAN_REGISTER *)arg,
					(const void *)&reg, sizeof(CAN_REGISTER));
			}
			break;

		//----------------------------------------------------------------------
		// IOCTL_CATS_CAN_write_register
		//----------------------------------------------------------------------
		case IOCTL_CATS_CAN_write_register:
			{
				CAN_REGISTER reg;

				if( !access_ok(VERIFY_READ, (void *)arg, sizeof(CAN_REGISTER))) {
					ret_val = -ENOTTY;
					break;
      			}

				if( !access_ok(VERIFY_WRITE, (void *)arg, sizeof(CAN_REGISTER))) {
					ret_val = -ENOTTY;
					break;
      			}

				__copy_from_user((void *)&reg,
					(const CAN_REGISTER *)arg, sizeof(CAN_REGISTER));
				sja1000_write(pci_adr_map, reg.device, reg.address, reg.data);
			}
			break;


		//----------------------------------------------------------------------
		// IOCTL_CATS_CAN_init
		//----------------------------------------------------------------------
		case IOCTL_CATS_CAN_init:
			{
				CAN_INIT	init;

				if( !access_ok(VERIFY_READ, (void *)arg, sizeof(CAN_INIT))) {
					ret_val = -ENOTTY;
					break;
      			}

				__copy_from_user((void *)&init, (const CAN_INIT *)arg, sizeof(CAN_INIT));
				if(sja1000_init(pci_adr_map, &init)) {
					ret_val = -ENOTTY;
				}
			}
			break;


		//----------------------------------------------------------------------
		// IOCTL_CATS_CAN_reset
		//----------------------------------------------------------------------
		case IOCTL_CATS_CAN_reset:
			{
				CAN_RESET	reset;

				if( !access_ok(VERIFY_READ, (void *)arg, sizeof(CAN_RESET))) {
					ret_val = -ENOTTY;
					break;
      			}

				__copy_from_user((void *)&reset,
					(const CAN_RESET *)arg, sizeof(CAN_RESET));
				if(sja1000_reset(pci_adr_map, reset.device, &reset.mode)) {
					ret_val = -ENOTTY;
				}
			}
			break;


		//----------------------------------------------------------------------
		// IOCTL_CATS_CAN_start
		//----------------------------------------------------------------------
		case IOCTL_CATS_CAN_start:
			{
				int	device;
				__get_user(device, (int __user *)arg);
				if(sja1000_start(pci_adr_map, device)) {
					ret_val = -ENOTTY;
				}
			}
			break;


		//----------------------------------------------------------------------
		// IOCTL_CATS_CAN_stop
		//----------------------------------------------------------------------
		case IOCTL_CATS_CAN_stop:
			{
				int device;
				__get_user(device, (int __user *)arg);
				if(sja1000_stop(pci_adr_map, device)) {
					ret_val = -ENOTTY;
				}
			}
			break;


		//----------------------------------------------------------------------
		// IOCTL_CATS_CAN_sleep
		//----------------------------------------------------------------------
		case IOCTL_CATS_CAN_sleep:
			{
				int device;
				__get_user(device, (int __user *)arg);
				if(sja1000_sleep(pci_adr_map, device)) {
					ret_val = -ENOTTY;
				}
			}
			break;


		//----------------------------------------------------------------------
		// IOCTL_CATS_CAN_scan:
		//----------------------------------------------------------------------
		case IOCTL_CATS_CAN_scan:
			{
				CAN_SCAN	scan;

				if( !access_ok(VERIFY_READ, (void *)arg, sizeof(CAN_SCAN))) {
					ret_val = -ENOTTY;
					break;
				}

      			if( !access_ok(VERIFY_WRITE, (void *)arg, sizeof(CAN_SCAN))) {
					ret_val = -ENOTTY;
					break;
      			}

				__copy_from_user((void *)&scan, (const CAN_SCAN *)arg, sizeof(CAN_SCAN));
				if(sja1000_status(pci_adr_map, scan.device, &scan.status)) {
					ret_val = -ENOTTY;
				} else {
				  	ret_val = __copy_to_user((CAN_SCAN *)arg,
							  	  (const void *)&scan, sizeof(CAN_SCAN));
				}
			}
			break;


		//----------------------------------------------------------------------
		// IOCTL_CATS_CAN_remoto_frame
		//----------------------------------------------------------------------
		case IOCTL_CATS_CAN_remoto_frame:
			{
				CAN_SEND	send;

				if( !access_ok(VERIFY_READ, (void *)arg, sizeof(CAN_SEND))) {
					ret_val = -ENOTTY;
					break;
      			}

				__copy_from_user((void *)&send, (const CAN_SEND *)arg, sizeof(CAN_SEND));
				if(sja1000_remoto_frame(pci_adr_map, send.device, &send.frame)) {
					ret_val = -ENOTTY;
				}
			}
			break;


		//----------------------------------------------------------------------
		// IOCTL_CATS_CAN_send
		//----------------------------------------------------------------------
		case IOCTL_CATS_CAN_send:
			{
				CAN_SEND	send;

				if( !access_ok(VERIFY_READ, (void *)arg, sizeof(CAN_SEND))) {
					ret_val = -ENOTTY;
					break;
      			}

				__copy_from_user((void *)&send, (const CAN_SEND *)arg, sizeof(CAN_SEND));
				if(sja1000_send(pci_adr_map, send.device, &send.frame)) {
					ret_val = -ENOTTY;
				}
			}
			break;


		//----------------------------------------------------------------------
		// IOCTL_CATS_CAN_recv
		//----------------------------------------------------------------------
		case IOCTL_CATS_CAN_recv:
			{
				CAN_RECV	recv;

				if( !access_ok(VERIFY_READ, (void *)arg, sizeof(CAN_RECV))) {
					ret_val = -ENOTTY;
					break;
      			}

				if( !access_ok(VERIFY_WRITE, (void *)arg, sizeof(CAN_RECV))) {
					ret_val = -ENOTTY;
					break;
      			}

				__copy_from_user((void *)&recv, (const CAN_RECV *)arg, sizeof(CAN_RECV));
				if(sja1000_recv(pci_adr_map, recv.device, &recv.frame)) {
					ret_val = -ENOTTY;
				} else {
			  	  	ret_val = __copy_to_user((CAN_RECV *)arg,
							  	  (const void *)&recv, sizeof(CAN_RECV));
				}
			}
			break;


		//----------------------------------------------------------------------
		// IOCTL_CATS_CAN_recv_buff
		//----------------------------------------------------------------------
		case IOCTL_CATS_CAN_recv_buff:
			{
				CAN_RECV	recv;

				if( !access_ok(VERIFY_READ, (void *)arg, sizeof(CAN_RECV))) {
					ret_val = -ENOTTY;
					break;
      			}

				if( !access_ok(VERIFY_WRITE, (void *)arg, sizeof(CAN_RECV))) {
					ret_val = -ENOTTY;
					break;
      			}

				__copy_from_user((void *)&recv, (const CAN_RECV *)arg, sizeof(CAN_RECV));
				if(sja1000_recv_buff(pci_adr_map, &recv)) {
					ret_val = -ENOTTY;
				} else {
				  	ret_val = __copy_to_user((CAN_RECV *)arg,
							  	  (const void *)&recv, sizeof(CAN_RECV));
				}
			}
			break;


		//----------------------------------------------------------------------
		// IOCTL_CATS_CAN_send_buff
		//----------------------------------------------------------------------
		case IOCTL_CATS_CAN_send_buff:
			{
				CAN_SEND	send;
				int 		ret;

				if( !access_ok(VERIFY_READ, (void *)arg, sizeof(CAN_SEND))) {
					ret_val = -ENOTTY;
					break;
      			}

				if( !access_ok(VERIFY_WRITE, (void *)arg, sizeof(CAN_SEND))) {
					ret_val = -ENOTTY;
					break;
      			}

				__copy_from_user((void *)&send, (const CAN_SEND *)arg, sizeof(CAN_SEND));

				/* 送信バッファへメッセージを書き込む */
				ret = sja1000_send_buff(pci_adr_map, &send);

				if( ret == CASTCAN_SUCCEEDED )							/* 成功 */
				{
					break;

				}else if( ret == CATSCAN_INVALID_DEVICENUM )
				{
					ret_val = -ENXIO;									/* デバイス指定が範囲外 */
					break;

				}else if( ret == CATSCAN_SEND_BUFFER_OVERFLOW )
				{
					ret_val = -EOVERFLOW;								/* 送信バッファが満杯 */
					break;
				}
			}
			break;


		//----------------------------------------------------------------------
		// IOCTL_CATS_CAN_exit
		//----------------------------------------------------------------------
		case IOCTL_CATS_CAN_exit:
			{
				int device;
				__get_user(device, (int __user *)arg);
				if(sja1000_exit(pci_adr_map, device)) {
					ret_val = -ENOTTY;
				}
			}
			break;;


		//----------------------------------------------------------------------
		// IOCTL_CATS_CAN_intop
		//----------------------------------------------------------------------
		case IOCTL_CATS_CAN_intop:
			{
				CAN_INTOP	iop;
				if( !access_ok(VERIFY_READ, (void *)arg, sizeof(CAN_INTOP))) {
					ret_val = -ENOTTY;
					break;
      			}

				if( !access_ok(VERIFY_WRITE, (void *)arg, sizeof(CAN_INTOP))) {
					ret_val = -ENOTTY;
					break;
      			}

				__copy_from_user((void *)&iop,
					(const CAN_INTOP *)arg, sizeof(CAN_INTOP));
				if(sja1000_log(pci_adr_map, iop.device, &iop.log)) {
					ret_val = -ENOTTY;
					break;
				}
				ret_val = __copy_to_user((CAN_INTOP *)arg,
						  	  (const void *)&iop, sizeof(CAN_INTOP));
			}
			break;


		//----------------------------------------------------------------------
		// ???
		//----------------------------------------------------------------------
		default:
			ret_val = -ENOTTY;
			break;
	}

	return ret_val;

}


