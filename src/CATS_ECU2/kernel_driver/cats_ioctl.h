/**********************************************************************************
 * ファイル名      : cats_ioctl.h
 * システム名      : x86 プロトタイプＥＣＵ OPE-RA Ver2.0
 * サブシステム名  : カーネルドライバ
 * プログラム名    : FPGA レジスタ定義
 * バージョン      : Ver1.00
 * --------------------------------------------------------------------------------
 * 作成者          : 
 * 作成部署        : 
 * 作成日付        : 2009年09月23日 新規作成
 * 更新履歴        : 2009年12月17日
 **********************************************************************************/
#ifndef	_CATS_IOCTL_H_
#define _CATS_IOCTL_H_

#define IOCTL_MAGIC_CATSDRV		'c'

#define IOCTL_CATS_FncInitADC				_IO( IOCTL_MAGIC_CATSDRV, 0 )
#define IOCTL_CATS_FncADCStartOneShot		_IOW( IOCTL_MAGIC_CATSDRV, 1, int )
#define IOCTL_CATS_FncADCStartContinuous	_IO( IOCTL_MAGIC_CATSDRV, 2 )
#define IOCTL_CATS_FncADCStopContinuous		_IO( IOCTL_MAGIC_CATSDRV, 3 )
#define IOCTL_CATS_FncADCGetOneShot			_IOWR( IOCTL_MAGIC_CATSDRV, 4, int )
#define IOCTL_CATS_FncADCGetContinuous		_IOWR( IOCTL_MAGIC_CATSDRV, 5, int )
#define IOCTL_CATS_FncDisableADC			_IO( IOCTL_MAGIC_CATSDRV, 6 )

#define IOCTL_CATS_FncInitDAC				_IO( IOCTL_MAGIC_CATSDRV, 7 )
#define IOCTL_CATS_FncDACSet				_IOW( IOCTL_MAGIC_CATSDRV, 8, int )
#define IOCTL_CATS_FncDACStop				_IO( IOCTL_MAGIC_CATSDRV, 9 )

#define IOCTL_CATS_FncInitPWM				_IOW( IOCTL_MAGIC_CATSDRV, 10, int )
#define IOCTL_CATS_FncPWMEnable				_IOW( IOCTL_MAGIC_CATSDRV, 11, int )
#define IOCTL_CATS_FncPWMSet				_IOW( IOCTL_MAGIC_CATSDRV, 12, int )

#define IOCTL_CATS_FncinitGPIOOutput		_IO( IOCTL_MAGIC_CATSDRV, 13 )
#define IOCTL_CATS_FncinitGPIOInput			_IO( IOCTL_MAGIC_CATSDRV, 14 )
#define IOCTL_CATS_FncGPIOSet				_IOW( IOCTL_MAGIC_CATSDRV, 15, int )
#define IOCTL_CATS_FncGPIOGet				_IOR( IOCTL_MAGIC_CATSDRV, 16, int )
#define IOCTL_CATS_FncGPIOIntEnable			_IO( IOCTL_MAGIC_CATSDRV, 17 )
#define IOCTL_CATS_FncGPIOIntSts			_IOR( IOCTL_MAGIC_CATSDRV, 18, int )
#define IOCTL_CATS_FncGPIOIntClear			_IOW( IOCTL_MAGIC_CATSDRV, 19, int )

#define IOCTL_CATS_FncPowerMoniIntEnable	_IOW( IOCTL_MAGIC_CATSDRV, 20, int )
#define IOCTL_CATS_FncPowerMoniIntSts		_IOW( IOCTL_MAGIC_CATSDRV, 21, int )
#define IOCTL_CATS_FncPowerMoniIntClear	_IO( IOCTL_MAGIC_CATSDRV, 22 )
#define IOCTL_CATS_FncAccMoniIntEnable		_IOW( IOCTL_MAGIC_CATSDRV, 23, int )
#define IOCTL_CATS_FncAccMoniIntSts		_IOW( IOCTL_MAGIC_CATSDRV, 24, int )
#define IOCTL_CATS_FncAccMoniIntClear		_IO( IOCTL_MAGIC_CATSDRV, 25 )

#define IOCTL_CATS_DebugFunc				_IOR( IOCTL_MAGIC_CATSDRV, 26, int )
#define IOCTL_CATS_DebugFunc2				_IOW( IOCTL_MAGIC_CATSDRV, 27, int )
#define IOCTL_CATS_DebugFunc3				_IOW( IOCTL_MAGIC_CATSDRV, 28, int )


// CAN SJA1000 device controle
#define IOCTL_CATS_CAN_read_register  		_IOWR( IOCTL_MAGIC_CATSDRV, 29, int )
#define IOCTL_CATS_CAN_write_register		_IOWR( IOCTL_MAGIC_CATSDRV, 30, int )
#define IOCTL_CATS_CAN_init					_IOR(  IOCTL_MAGIC_CATSDRV, 31, int )
#define IOCTL_CATS_CAN_reset				_IOW(  IOCTL_MAGIC_CATSDRV, 32, int )
#define IOCTL_CATS_CAN_start				_IOR(  IOCTL_MAGIC_CATSDRV, 33, int )
#define IOCTL_CATS_CAN_stop					_IOR(  IOCTL_MAGIC_CATSDRV, 34, int )
#define IOCTL_CATS_CAN_sleep				_IOR(  IOCTL_MAGIC_CATSDRV, 35, int )
#define IOCTL_CATS_CAN_scan					_IOWR( IOCTL_MAGIC_CATSDRV, 36, int )
#define IOCTL_CATS_CAN_remoto_frame			_IOWR( IOCTL_MAGIC_CATSDRV, 37, int )
#define IOCTL_CATS_CAN_send					_IOWR( IOCTL_MAGIC_CATSDRV, 38, int )
#define IOCTL_CATS_CAN_recv					_IOWR( IOCTL_MAGIC_CATSDRV, 39, int )
#define IOCTL_CATS_CAN_send_buff			_IOWR( IOCTL_MAGIC_CATSDRV, 40, int )
#define IOCTL_CATS_CAN_recv_buff			_IOWR( IOCTL_MAGIC_CATSDRV, 41, int )
#define IOCTL_CATS_CAN_exit					_IOR(  IOCTL_MAGIC_CATSDRV, 42, int )
#define IOCTL_CATS_CAN_intop				_IOWR( IOCTL_MAGIC_CATSDRV, 43, int )

// FPGA test mode
#define IOCTL_CATS_TestScratchReg_R		_IOWR( IOCTL_MAGIC_CATSDRV, 44, int )
#define IOCTL_CATS_TestScratchReg_W		_IOWR( IOCTL_MAGIC_CATSDRV, 45, int )
#define IOCTL_CATS_TestVersionReadReg		_IOWR( IOCTL_MAGIC_CATSDRV, 46, int )

#define IOCTL_CATS_CallBackPowAccGetIrq		_IOR( IOCTL_MAGIC_CATSDRV, 47, int )
#define IOCTL_CATS_CallBackPowAccClrIrq		_IOW( IOCTL_MAGIC_CATSDRV, 48, int )

#define IOCTL_CATS_CallBackGPIOGetIrq		_IOR( IOCTL_MAGIC_CATSDRV, 49, int )
#define IOCTL_CATS_CallBackGPIOClrIrq		_IOW( IOCTL_MAGIC_CATSDRV, 50, int )

#define IOCTL_CATS_FncGPIOSetAll			_IOW( IOCTL_MAGIC_CATSDRV, 51, int )
#define IOCTL_CATS_FncGPIOGetAll			_IOR( IOCTL_MAGIC_CATSDRV, 52, int )

#define IOCTL_CATS_FLEXRAY_WRITE32		_IOW( IOCTL_MAGIC_CATSDRV, 53, int )
#define IOCTL_CATS_FLEXRAY_READ32			_IOWR( IOCTL_MAGIC_CATSDRV, 54, int )
#define IOCTL_CATS_FLEXRAY_SETCMD			_IOWR( IOCTL_MAGIC_CATSDRV, 55, int )
#define IOCTL_CATS_FLEXRAY_GETPOCSTATE		_IOR( IOCTL_MAGIC_CATSDRV, 56, int )
#define IOCTL_CATS_FLEXRAY_ENABLEINT		_IO( IOCTL_MAGIC_CATSDRV, 57 )
#define IOCTL_CATS_FLEXRAY_DISABLEINT		_IO( IOCTL_MAGIC_CATSDRV, 58 )
#define IOCTL_CATS_FLEXRAY_GETINTSTS		_IOR( IOCTL_MAGIC_CATSDRV, 59, int )
#define IOCTL_CATS_FLEXRAY_RESETINT		_IOW( IOCTL_MAGIC_CATSDRV, 60, int )
#define IOCTL_CATS_FLEXRAY_SETWAKEUPCH		_IOW( IOCTL_MAGIC_CATSDRV, 61, int )
#define IOCTL_CATS_FLEXRAY_GETNMVECTOR		_IOR( IOCTL_MAGIC_CATSDRV, 62, int )
#define IOCTL_CATS_FLEXRAY_TRANSMIT		_IOW( IOCTL_MAGIC_CATSDRV, 63, int )
#define IOCTL_CATS_FLEXRAY_RECEIVE		_IOWR( IOCTL_MAGIC_CATSDRV, 64, int )

#define IOCTL_DRV_CATSDRV_MAX   65


#define CATS_ERR_BUSY -344

#endif // _CATS_IOCTL_H_
