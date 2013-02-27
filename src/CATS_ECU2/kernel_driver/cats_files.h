/**********************************************************************************
 * ファイル名      : cats_files.h
 * システム名      : x86 プロトタイプＥＣＵ OPE-RA Ver2.0
 * サブシステム名  : カーネルドライバ
 * プログラム名    : ファイル操作 (ヘッダー）
 * バージョン      : Ver1.00
 * --------------------------------------------------------------------------------
 * 作成者          : 
 * 作成部署        : 
 * 作成日付        : 2009年09月23日 新規作成
 * 更新履歴        : 2009年12月11日
 **********************************************************************************/
#ifndef	CATS_FILES_H
#define CATS_FILES_H

#include <linux/fs.h>
#include <linux/interrupt.h>

extern spinlock_t isr_spinlock;

// open/release(close) 時に割り込み設定する場合。
// #define OPEN_CLOSE_INTR_INSTALL


/********************************************************************/
/* 			システムコールハンドラ宣言								*/
/********************************************************************/
/* CAN 機能システムコールハンドラ */
int catsdrv_can_open( struct inode *, struct file * );
int catsdrv_can_release( struct inode *, struct file * );
int catsdrv_can_ioctl( struct inode *, struct file *,
				   unsigned int , unsigned long );

/* FlexRay0 機能システムコールハンドラ */
int catsdrv_flexray0_open( struct inode *, struct file * );
int catsdrv_flexray0_release( struct inode *, struct file * );
int catsdrv_flexray0_ioctl( struct inode *, struct file *,
				   unsigned int , unsigned long );
int catsdrv_flexray0_fasync(int fd, struct file *file, int mode);


/* FlexRay1 機能システムコールハンドラ */
int catsdrv_flexray1_open( struct inode *, struct file * );
int catsdrv_flexray1_release( struct inode *, struct file * );
int catsdrv_flexray1_ioctl( struct inode *, struct file *,
				   unsigned int , unsigned long );
int catsdrv_flexray1_fasync(int fd, struct file *file, int mode);


/* ADC 機能システムコールハンドラ */
int catsdrv_adc_open( struct inode *, struct file * );
int catsdrv_adc_release( struct inode *, struct file * );
int catsdrv_adc_ioctl( struct inode *, struct file *,
				   unsigned int , unsigned long );

/* DAC 機能システムコールハンドラ */
int catsdrv_dac_open( struct inode *, struct file * );
int catsdrv_dac_release( struct inode *, struct file * );
int catsdrv_dac_ioctl( struct inode *, struct file *,
				   unsigned int , unsigned long );

/* PWM 機能システムコールハンドラ */
int catsdrv_pwm_open( struct inode *, struct file * );
int catsdrv_pwm_release( struct inode *, struct file * );
int catsdrv_pwm_ioctl( struct inode *, struct file *,
				   unsigned int , unsigned long );

/* GPIO 機能システムコールハンドラ */
int catsdrv_gpio_open( struct inode *, struct file * );
int catsdrv_gpio_release( struct inode *, struct file * );
int catsdrv_gpio_ioctl( struct inode *, struct file *,
				   unsigned int , unsigned long );
int catsdrv_gpio_fasync(int fd, struct file *file, int mode);

/* POW,ACC 機能システムコールハンドラ */
int catsdrv_powacc_open( struct inode *, struct file * );
int catsdrv_powacc_release( struct inode *, struct file * );
int catsdrv_powacc_ioctl( struct inode *, struct file *,
				   unsigned int , unsigned long );
int catsdrv_powacc_fasync(int fd, struct file *file, int mode);


/* 割り込みコールバック 機能システムコールハンドラ */
int catsdrv_callback_open( struct inode *, struct file * );
int catsdrv_callback_release( struct inode *, struct file * );
int catsdrv_callback_ioctl( struct inode *, struct file *,
				   unsigned int , unsigned long );

/* 試験 機能システムコールハンドラ */
int catsdrv_test_open( struct inode *, struct file * );
int catsdrv_test_release( struct inode *, struct file * );
int catsdrv_test_ioctl( struct inode *, struct file *,
				   unsigned int , unsigned long );

//-----------------------------------------------------------------//
/*!
	@brief	SJA1000 タスクレット処理
	@param[in]	param	パラメーター
 */
//-----------------------------------------------------------------//
void sja1000_tasklet(unsigned long param);


//-----------------------------------------------------------------//
/*!
	@brief	CATS/FPGA モジュール・割り込みハンドラー(排他制御用)
			irqreturn_t cats_isrwrap(int irq, void *dev_id, struct pt_regs *regs)
*/
//-----------------------------------------------------------------//
#ifdef KERNELMINOR18
irqreturn_t cats_isrwrap(int irq, void *dev_id, struct pt_regs *regs);
#else
irqreturn_t cats_isrwrap(int irq, void *dev_id);
#endif


//-----------------------------------------------------------------//
/*!
	@brief	CATS/FPGA モジュール・割り込みハンドラー
			割り込み関数、プロトタイプの間違い。
			irqreturn_t catsfpga_isr(int irq, void *dev_id, struct pt_regs *regs)
*/
//-----------------------------------------------------------------//
#ifdef KERNELMINOR18
irqreturn_t catsfpga_isr(int irq, void *dev_id, struct pt_regs *regs);
#else
irqreturn_t catsfpga_isr(int irq, void *dev_id);
#endif


//-----------------------------------------------------------------//
/*!
	@brief	割り込みハンドラー設定
	@return 正常終了で「０」を返す。
*/
//-----------------------------------------------------------------//
int catsdrv_interrupt_install(void);


//-----------------------------------------------------------------//
/*!
	@brief	割り込みハンドラー解除
*/
//-----------------------------------------------------------------//
void catsdrv_interrupt_uninstall(void);


//-----------------------------------------------------------------//
/*!
	@brief	CATS/FPGA モジュール・ファイル・[open] 操作
	@param[in]	inode
	@param[in]	filep
	@return 正常終了で「０」を返す。
*/
//-----------------------------------------------------------------//
int catsdrv_open( struct inode *inode, struct file * filep );


//-----------------------------------------------------------------//
/*!
	@brief	CATS/FPGA モジュール・ファイル・[release]操作
	@param[in]	inode
	@param[in]	filep
	@return 正常終了で「０」を返す。
*/
//-----------------------------------------------------------------//
int catsdrv_release( struct inode *inode, struct file * filep );


//-----------------------------------------------------------------//
/*!
	@brief	CATS/FPGA モジュール・ファイル・[ioctl]操作
	@param[in]	inode
	@param[in]	filep
	@param[in]	cmd
	@param[in]	arg
	@return 正常終了で「０」を返す。
*/
//-----------------------------------------------------------------//
int catsdrv_ioctl( struct inode *inode, struct file *filep,
				   unsigned int cmd, unsigned long arg );


#endif // CATS_FILES_H
