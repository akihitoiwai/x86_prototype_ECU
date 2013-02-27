/**********************************************************************************
 * ファイル名      : cats_flexray.c
 * システム名      : x86 プロトタイプＥＣＵ OPE-RA Ver2.0
 * サブシステム名  : カーネルドライバ
 * プログラム名    : ＦｌｅｘＲａｙ制御
 * バージョン      : Ver1.01
 * モジュール構成  : cats_fpga.c		PCI Access
 *                 : cats_flexray.c		FlexRay Control
 * --------------------------------------------------------------------------------
 * 作成者          : 
 * 作成部署        : 
 * 作成日付        : 2009年08月24日 新規作成
 * 更新履歴        : 2010年06月29日
 **********************************************************************************/
#include <linux/pci.h>
#include <asm/uaccess.h>
#include <linux/fs.h>
#include <linux/delay.h>

#include "cats_fpga.h"
#include "cats_files.h"
#include "cats_ioctl.h"
#include "cats_flexray.h"

//#define FLEXRAY_DRV_DEBUG

int catsdrv_flexray_fasync( int cc_index, int fd, struct file *file, int mode );
int catsdrv_flexray_open( int cc_index );
int catsdrv_flexray_release( int cc_index, struct file *filp );
int catsdrv_flexray_ioctl( int cc_index, unsigned int cmd, unsigned long arg );
void catsdrv_flexray_setcmd(int cc_index, int cmd);
void catsdrv_flexray_getpocstate(int cc_index, CFR_GETPOCSTATE_REG *reg);
void catsdrv_flexray_transmit(int cc_index, unsigned long index, unsigned char *data, int length, int int_enable);
int catsdrv_flexray_gettxbuf(int cc_index);
void catsdrv_flexray_receive(int cc_index, unsigned long index, unsigned char *data, int *length);
unsigned long catsdrv_flexray_read32(int cc_index, unsigned long addr);
void catsdrv_flexray_write32(int cc_index, unsigned long addr, unsigned long data);

/**********************************************************************************
 * 変数定義
 **********************************************************************************/
int flexray_drv_open[2] = {0, 0};
unsigned long cc_base_offset[2];
int txrq[2][4];

/***********************************************************************************
* MODULE         : catsdrv_flexray0_fasync
* ABSTRACT       : CATS/FPGA デバイス・ファイル・[fasync] 操作
* FUNCTION       : 
* ARGUMENT       : 
* NOTE           : 
* RETURN         : 正常終了で「０」を返す。
* CREATE         : 2009/12/ 2  新規作成 
* UPDATE         : 
***********************************************************************************/
int catsdrv_flexray0_fasync( int fd, struct file *file, int mode )
{
#ifdef FLEXRAY_DRV_DEBUG
	printk(KERN_INFO "===== catsdrv_flexray0_fasync\n");
#endif

	return catsdrv_flexray_fasync(0, fd, file, mode);
}
/***********************************************************************************
* MODULE         : catsdrv_flexray1_fasync
* ABSTRACT       : CATS/FPGA デバイス・ファイル・[fasync] 操作
* FUNCTION       : 
* ARGUMENT       : 
* NOTE           : 
* RETURN         : 正常終了で「０」を返す。
* CREATE         : 2009/12/ 2  新規作成 
* UPDATE         : 
***********************************************************************************/
int catsdrv_flexray1_fasync( int fd, struct file *file, int mode )
{
#ifdef FLEXRAY_DRV_DEBUG
	printk(KERN_INFO "===== catsdrv_flexray1_fasync\n");
#endif

	return catsdrv_flexray_fasync(1, fd, file, mode);
}
/***********************************************************************************
* MODULE         : catsdrv_flexray_fasync
* ABSTRACT       : CATS/FPGA デバイス・ファイル・[fasync] 操作
* FUNCTION       : 
* ARGUMENT       : 
* NOTE           : 
* RETURN         : 正常終了で「０」を返す。
* CREATE         : 2009/12/ 2  新規作成 
* UPDATE         : 
***********************************************************************************/
int catsdrv_flexray_fasync( int cc_index, int fd, struct file *file, int mode )
{
	return fasync_helper(fd, file, mode, &FlexrayDev[cc_index].async_queue);
}
/***********************************************************************************
* MODULE         : catsdrv_flexray0_open
* ABSTRACT       : CATS/FPGA デバイス・ファイル・[open] 操作
* FUNCTION       : 
* ARGUMENT       : 
* NOTE           : 
* RETURN         : 正常終了で「０」を返す。
* CREATE         : 2009/12/ 2  新規作成 
* UPDATE         : 2010/06/29
***********************************************************************************/
int catsdrv_flexray0_open( struct inode *inode, struct file *file )
{
	volatile unsigned long *vio = (volatile unsigned long*)pci_adr_map;
	unsigned	long flags;		/* スピンロック用(割り込み状態退避用) */

#ifdef FLEXRAY_DRV_DEBUG
	printk(KERN_INFO "===== catsdrv_flexray0_open\n");
#endif
	// 多重オープンチェック
	if (flexray_drv_open[0] != 0)
	{
		// オープン失敗
		return -1;
	}

	spin_lock_irqsave( &isr_spinlock, flags );		/* 割り込み禁止開始 */

	// FlexRay0の割り込みを禁止する
	vio[INT_EN] &= ~(FR0_INT0|FR0_INT1|FR0_INT2);

	// FlexRay0の割り込みレベルを反転させる
	vio[INT_LVL] |= FR0_INT0|FR0_INT1|FR0_INT2;

	spin_unlock_irqrestore( &isr_spinlock, flags );		/* 割り込み禁止終了 */

	// FlexRay0にリセットをかける
	vio[MFR_WCTL] = 0x01;
	vio[MFR_WCTL] = 0x00;

	// レジスタの先頭アドレスをセット
	cc_base_offset[0] = CC0_BASE_OFFSET + (unsigned long)pci_flexray_adr_map;

	return catsdrv_flexray_open(0);
}
/***********************************************************************************
* MODULE         : catsdrv_flexray1_open
* ABSTRACT       : CATS/FPGA デバイス・ファイル・[open] 操作
* FUNCTION       : 
* ARGUMENT       : 
* NOTE           : 
* RETURN         : 正常終了で「０」を返す。
* CREATE         : 2009/12/ 2  新規作成 
* UPDATE         : 2010/06/29
***********************************************************************************/
int catsdrv_flexray1_open( struct inode *inode, struct file *file )
{
	volatile unsigned long *vio = (volatile unsigned long*)pci_adr_map;
	unsigned	long flags;		/* スピンロック用(割り込み状態退避用) */

#ifdef FLEXRAY_DRV_DEBUG
	printk(KERN_INFO "===== catsdrv_flexray1_open\n");
#endif
	// 多重オープンチェック
	if (flexray_drv_open[1] != 0)
	{
		// オープン失敗
		return -1;
	}

	spin_lock_irqsave( &isr_spinlock, flags );		/* 割り込み禁止開始 */

	// FlexRay1の割り込みを禁止する
	vio[INT_EN] &= ~(FR1_INT0|FR1_INT1|FR1_INT2);

	// FlexRay1の割り込みレベルを反転させる
	vio[INT_LVL] |= FR1_INT0|FR1_INT1|FR1_INT2;

	spin_unlock_irqrestore( &isr_spinlock, flags );		/* 割り込み禁止終了 */

	// FlexRay1にリセットをかける
	vio[MFR_WCTL] = 0x02;
	vio[MFR_WCTL] = 0x00;

	// レジスタの先頭アドレスをセット
	cc_base_offset[1] = CC1_BASE_OFFSET + (unsigned long)pci_flexray_adr_map;

	return catsdrv_flexray_open(1);
}
/***********************************************************************************
* MODULE         : catsdrv_flexray_open
* ABSTRACT       : CATS/FPGA デバイス・ファイル・[open] 操作
* FUNCTION       : 
* ARGUMENT       : 
* NOTE           : 
* RETURN         : 正常終了で「０」を返す。
* CREATE         : 2009/12/ 2  新規作成 
* UPDATE         : 
***********************************************************************************/
int catsdrv_flexray_open( int cc_index )
{
	unsigned long ver;

#ifdef FLEXRAY_DRV_DEBUG
	printk(KERN_INFO "===== catsdrv_flexray_open %d %d\n", flexray_drv_open[0], flexray_drv_open[1]);
#endif

	// FlexRayコントローラのバージョン情報を参照しコントローラが搭載されているかどうかチェック
	ver = catsdrv_flexray_read32(cc_index, CUST0);

	// 版数はマスクして比較する
	if ((ver & 0xff00ff00) != (FLEXRAY_LSI_VER & 0xff00ff00))
	{
		// オープン失敗
		return -1;
	}

	// クロックコントロールレジスタ
	catsdrv_flexray_write32(cc_index, CCNT, FLEXRAY_INIT_CCNT);

	// PLLロックアップ時間600usウェイト
	udelay(600);

	// PLL発信許可
	catsdrv_flexray_write32(cc_index, CCNT, FLEXRAY_INIT_CCNT | FLEXRAY_CCNT_PON);

	// オープン済みフラグセット
	flexray_drv_open[cc_index] = 1;

	// オープン成功
	return 0;
}
/***********************************************************************************
* MODULE         : catsdrv_flexray0_release
* ABSTRACT       : CATS/FPGA デバイス・ファイル・[release]操作
* FUNCTION       : 
* ARGUMENT       : 
* NOTE           : 
* RETURN         : 正常終了で「０」を返す。
* CREATE         : 2009/12/ 2  新規作成 
* UPDATE         : 2010/06/29
***********************************************************************************/
int catsdrv_flexray0_release( struct inode *inode, struct file *filp )
{
	volatile unsigned long *vio = (volatile unsigned long*)pci_adr_map;
	unsigned	long flags;		/* スピンロック用(割り込み状態退避用) */

#ifdef FLEXRAY_DRV_DEBUG
	printk(KERN_INFO "===== catsdrv_flexray0_release\n");
#endif

	spin_lock_irqsave( &isr_spinlock, flags );		/* 割り込み禁止開始 */

	// 割り込み禁止
	vio[INT_EN] &= ~(FR0_INT0|FR0_INT1|FR0_INT2);

	spin_unlock_irqrestore( &isr_spinlock, flags );		/* 割り込み禁止終了 */

	catsdrv_flexray_release(0, filp);

	return 0;
}
/***********************************************************************************
* MODULE         : catsdrv_flexray1_release
* ABSTRACT       : CATS/FPGA デバイス・ファイル・[release]操作
* FUNCTION       : 
* ARGUMENT       : 
* NOTE           : 
* RETURN         : 正常終了で「０」を返す。
* CREATE         : 2009/12/ 2  新規作成 
* UPDATE         : 2010/06/29
***********************************************************************************/
int catsdrv_flexray1_release( struct inode *inode, struct file *filp )
{
	volatile unsigned long *vio = (volatile unsigned long*)pci_adr_map;
	unsigned	long flags;		/* スピンロック用(割り込み状態退避用) */

#ifdef FLEXRAY_DRV_DEBUG
	printk(KERN_INFO "===== catsdrv_flexray1_release\n");
#endif

	spin_lock_irqsave( &isr_spinlock, flags );		/* 割り込み禁止開始 */

	// 割り込み禁止
	vio[INT_EN] &= ~(FR1_INT0|FR1_INT1|FR1_INT2);

	spin_unlock_irqrestore( &isr_spinlock, flags );		/* 割り込み禁止終了 */

	catsdrv_flexray_release(1, filp);

	return 0;
}
/***********************************************************************************
* MODULE         : catsdrv_flexray_release
* ABSTRACT       : CATS/FPGA デバイス・ファイル・[release]操作
* FUNCTION       : 
* ARGUMENT       : 
* NOTE           : 
* RETURN         : 正常終了で「０」を返す。
* CREATE         : 2009/12/ 2  新規作成 
* UPDATE         : 
***********************************************************************************/
int catsdrv_flexray_release( int cc_index, struct file *filp )
{
	filp->private_data = filp;

	catsdrv_flexray_fasync(cc_index, -1, filp, 0);

	// オープン済みフラグリセット
	flexray_drv_open[cc_index] = 0;

	return 0;
}
/***********************************************************************************
* MODULE         : catsdrv_flexray0_ioctl
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
* UPDATE         : 
***********************************************************************************/
int catsdrv_flexray0_ioctl( struct inode *inode, struct file *filep, unsigned int cmd, unsigned long arg )
{
	return catsdrv_flexray_ioctl(0, cmd, arg);
}
/***********************************************************************************
* MODULE         : catsdrv_flexray1_ioctl
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
* UPDATE         : 
***********************************************************************************/
int catsdrv_flexray1_ioctl( struct inode *inode, struct file *filep, unsigned int cmd, unsigned long arg )
{
	return catsdrv_flexray_ioctl(1, cmd, arg);
}
/***********************************************************************************
* MODULE         : catsdrv_flexray_ioctl
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
* UPDATE         : 2010/06/29
***********************************************************************************/
int catsdrv_flexray_ioctl( int cc_index, unsigned int cmd, unsigned long arg )
{
	int size;
	int ret_val;
	unsigned	long flags;		/* スピンロック用(割り込み状態退避用) */

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

	switch (cmd)
	{

		//----------------------------------------------------------------------
		// IOCTL_CATS_FLEXRAY_WRITE32
		//----------------------------------------------------------------------
		case IOCTL_CATS_FLEXRAY_WRITE32:
			{
				CFR_WRITE32_REG reg;

				// アドレス、データをコピー(reg←arg)
				__copy_from_user(&reg, (const CFR_WRITE32_REG*)arg, sizeof(CFR_WRITE32_REG));

				// データを書き込む
				catsdrv_flexray_write32(cc_index, reg.addr, reg.data);
			}
			break;

		//----------------------------------------------------------------------
		// IOCTL_CATS_FLEXRAY_READ32
		//----------------------------------------------------------------------
		case IOCTL_CATS_FLEXRAY_READ32:
			{
				CFR_READ32_REG reg;

				// アドレスをコピー(reg←arg)
				__copy_from_user(&reg, (const CFR_READ32_REG*)arg, sizeof(CFR_READ32_REG));

				// データを読み出す
				reg.data = catsdrv_flexray_read32(cc_index, reg.addr);

				// 読み出したデータをコピー(arg←reg)
				ret_val = __copy_to_user((CFR_READ32_REG*)arg, &reg, sizeof(CFR_READ32_REG));
			}
			break;

		//----------------------------------------------------------------------
		// IOCTL_CATS_FLEXRAY_SETCMD
		//----------------------------------------------------------------------
		case IOCTL_CATS_FLEXRAY_SETCMD:
			{
				CFR_SETCMD_REG reg;
				unsigned long succ1;

				// コマンドをコピー(reg←arg)
				__copy_from_user(&reg, (const CFR_SETCMD_REG*)arg, sizeof(CFR_SETCMD_REG));

				// POCビジーステータスをリード
				succ1 = catsdrv_flexray_read32(cc_index, SUCC1);

				// POCビジーチェック
				if ((succ1 & FLEXRAY_SUCC1_PBSY) != 0)
				{
					reg.busy = 1;
				}
				else
				{
					reg.busy = 0;

					// コマンドを書き込む
					catsdrv_flexray_setcmd(cc_index, reg.cmd);
				}

				// ビジーフラグをコピー(arg←reg)
				ret_val = __copy_to_user((CFR_SETCMD_REG*)arg, &reg, sizeof(CFR_SETCMD_REG));
			}
			break;

		//----------------------------------------------------------------------
		// IOCTL_CATS_FLEXRAY_GETPOCSTATE
		//----------------------------------------------------------------------
		case IOCTL_CATS_FLEXRAY_GETPOCSTATE:
			{
				CFR_GETPOCSTATE_REG reg;

				// 状態を取得する
				catsdrv_flexray_getpocstate(cc_index, &reg);

				// 読み出したデータをコピー(arg←reg)
				ret_val = __copy_to_user((CFR_GETPOCSTATE_REG*)arg, &reg, sizeof(CFR_GETPOCSTATE_REG));
			}
			break;

		//----------------------------------------------------------------------
		// IOCTL_CATS_FLEXRAY_ENABLEINT
		//----------------------------------------------------------------------
		case IOCTL_CATS_FLEXRAY_ENABLEINT:
			{
				volatile unsigned long *vio = (volatile unsigned long*)pci_adr_map;

				// 割り込みフラグをクリアする
				flexray_intsts[cc_index] = 0;

				// FLEXRAYコントローラの割り込みを許可する
				catsdrv_flexray_write32(cc_index, SIES, FLEXRAY_SIES_RXIE);

				spin_lock_irqsave( &isr_spinlock, flags );		/* 割り込み禁止開始 */

				if (cc_index == 0)
				{
					// FPGAの割り込みをクリアする
					vio[INT_ST] |= FR0_INT1;

					// FPGAの割り込みを許可する
					vio[INT_EN] |= FR0_INT1;
				}
				else if (cc_index == 1)
				{
					// FPGAの割り込みをクリアする
					vio[INT_ST] |= FR1_INT1;

					// FPGAの割り込みを許可する
					vio[INT_EN] |= FR1_INT1;
				}

				spin_unlock_irqrestore( &isr_spinlock, flags );		/* 割り込み禁止終了 */

			}
			break;

		//----------------------------------------------------------------------
		// IOCTL_CATS_FLEXRAY_DISABLEINT
		//----------------------------------------------------------------------
		case IOCTL_CATS_FLEXRAY_DISABLEINT:
			{
				volatile unsigned long *vio = (volatile unsigned long*)pci_adr_map;

				spin_lock_irqsave( &isr_spinlock, flags );		/* 割り込み禁止開始 */

				// FPGAの割り込みを禁止する
				if (cc_index == 0)
				{
					vio[INT_EN] &= ~FR0_INT1;
				}
				else if (cc_index == 1)
				{
					vio[INT_EN] &= ~FR1_INT1;
				}

				spin_unlock_irqrestore( &isr_spinlock, flags );		/* 割り込み禁止終了 */
			}
			break;

		//----------------------------------------------------------------------
		// IOCTL_CATS_FLEXRAY_GETINTSTS
		//----------------------------------------------------------------------
		case IOCTL_CATS_FLEXRAY_GETINTSTS:
			{
				CFR_GETINTSTS_REG reg;
				unsigned long sies;

				reg.txi = 0;
				reg.rxi = 0;

				reg.err_int_reg = catsdrv_flexray_read32(cc_index, EIR);
				reg.sts_int_reg = catsdrv_flexray_read32(cc_index, SIR);

				sies = catsdrv_flexray_read32(cc_index, SIES);

				if (flexray_intsts[cc_index] != 0)
				{
					if ((sies & FLEXRAY_SIES_RXIE) && (reg.sts_int_reg & FLEXRAY_SIR_RXI))
					{
						reg.rxi = 1;
					}

					if ((sies & FLEXRAY_SIES_TXIE) && (reg.sts_int_reg & FLEXRAY_SIR_TXI))
					{
						reg.txi = 1;

						reg.txbufno = catsdrv_flexray_gettxbuf(cc_index);
					}
				}

				// 割り込みステータスをコピー
				ret_val = __copy_to_user((CFR_GETINTSTS_REG*)arg, &reg, sizeof(CFR_GETINTSTS_REG));
			}
			break;

		//----------------------------------------------------------------------
		// IOCTL_CATS_FLEXRAY_RESETINT
		//----------------------------------------------------------------------
		case IOCTL_CATS_FLEXRAY_RESETINT:
			{
				CFR_RESETINT_REG reg;
				volatile unsigned long *vio = (volatile unsigned long*)pci_adr_map;
				int txrq_index, bit_no;

				// コマンドをコピー(reg←arg)
				__copy_from_user(&reg, (const CFR_RESETINT_REG*)arg, sizeof(CFR_RESETINT_REG));

				// 割り込みフラグをクリアする
				flexray_intsts[cc_index] = 0;

				if (reg.rxi)
				{
					// 受信割り込みをクリアする
					catsdrv_flexray_write32(cc_index, SIR, FLEXRAY_SIR_RXI);
				}

				if (reg.txi)
				{
					// TXRQnレジスタのnを求める
					txrq_index = reg.txbufno / 32;

					// TXRQnレジスタの何ビット目かを求める
					bit_no = reg.txbufno % 32;

					// 送信要求フラグリセット
					txrq[cc_index][txrq_index] &= ~(1 << bit_no);

					// 送信要求が全てリセットされたら送信完了割り込みをクリアする
					for (txrq_index = 0; txrq_index < 4; txrq_index++)
					{
						if (txrq[cc_index][txrq_index] != 0)
						{
							break;
						}
					}

					if (txrq_index == 4)
					{
						// 送信完了割り込みをクリアする
						catsdrv_flexray_write32(cc_index, SIR, FLEXRAY_SIR_TXI);

						// 送信完了割り込みを禁止する
						catsdrv_flexray_write32(cc_index, SIER, FLEXRAY_SIES_TXIE);
					}
				}

				spin_lock_irqsave( &isr_spinlock, flags );		/* 割り込み禁止開始 */

				if (cc_index == 0)
				{
					// FPGAの割り込みをクリアする
					vio[INT_ST] |= FR0_INT1;

					// FPGAの割り込みを許可する
					vio[INT_EN] |= FR0_INT1;
				}
				else if (cc_index == 1)
				{
					// FPGAの割り込みをクリアする
					vio[INT_ST] |= FR1_INT1;

					// FPGAの割り込みを許可する
					vio[INT_EN] |= FR1_INT1;
				}

				spin_unlock_irqrestore( &isr_spinlock, flags );		/* 割り込み禁止終了 */
			}
			break;

		//----------------------------------------------------------------------
		// IOCTL_CATS_FLEXRAY_SETWAKEUPCH
		//----------------------------------------------------------------------
		case IOCTL_CATS_FLEXRAY_SETWAKEUPCH:
			{
				CFR_SETWAKEUPCH_REG reg;
				unsigned long succ1;

				// コマンドをコピー(reg←arg)
				__copy_from_user(&reg, (const CFR_SETWAKEUPCH_REG*)arg, sizeof(CFR_SETWAKEUPCH_REG));

				// SUCC1レジスタを読み出す
				succ1 = catsdrv_flexray_read32(cc_index, SUCC1);

				if (reg.wakeup_ch == 1)
				{
					// チャンネルBからウェイクアップパターンを送信するように設定
					succ1 |= FLEXRAY_SUCC1_WUCS;
				}
				else
				{
					// チャンネルAからウェイクアップパターンを送信するように設定
					succ1 &= ~FLEXRAY_SUCC1_WUCS;
				}

				// SUCC1レジスタに書き込む
				catsdrv_flexray_write32(cc_index, SUCC1, succ1);
			}
			break;

		//----------------------------------------------------------------------
		// IOCTL_CATS_FLEXRAY_GETNMVECTOR
		//----------------------------------------------------------------------
		case IOCTL_CATS_FLEXRAY_GETNMVECTOR:
			{
				CFR_GETNMVECTOR_REG reg;

				// NMV1の内容をコピー
				reg.nm_vector[0]  = catsdrv_flexray_read32(cc_index, NMV1);
				reg.nm_vector[1]  = catsdrv_flexray_read32(cc_index, NMV1) >> 8;
				reg.nm_vector[2]  = catsdrv_flexray_read32(cc_index, NMV1) >> 16;
				reg.nm_vector[3]  = catsdrv_flexray_read32(cc_index, NMV1) >> 24;
				// NMV2の内容をコピー
				reg.nm_vector[4]  = catsdrv_flexray_read32(cc_index, NMV2);
				reg.nm_vector[5]  = catsdrv_flexray_read32(cc_index, NMV2) >> 8;
				reg.nm_vector[6]  = catsdrv_flexray_read32(cc_index, NMV2) >> 16;
				reg.nm_vector[7]  = catsdrv_flexray_read32(cc_index, NMV2) >> 24;
				// NMV3の内容をコピー
				reg.nm_vector[8]  = catsdrv_flexray_read32(cc_index, NMV3);
				reg.nm_vector[9]  = catsdrv_flexray_read32(cc_index, NMV3) >> 8;
				reg.nm_vector[10] = catsdrv_flexray_read32(cc_index, NMV3) >> 16;
				reg.nm_vector[11] = catsdrv_flexray_read32(cc_index, NMV3) >> 24;

				// 読み出したデータをコピー(arg←reg)
				ret_val = __copy_to_user((CFR_GETNMVECTOR_REG*)arg, &reg, sizeof(CFR_GETNMVECTOR_REG));
			}
			break;

		//----------------------------------------------------------------------
		// IOCTL_CATS_FLEXRAY_TRANSMIT
		//----------------------------------------------------------------------
		case IOCTL_CATS_FLEXRAY_TRANSMIT:
			{
				CFR_TRANSMIT_REG reg;

				// 送信データをコピー(reg←arg)
				__copy_from_user(&reg, (const CFR_TRANSMIT_REG*)arg, sizeof(CFR_TRANSMIT_REG));

				// 送信
				catsdrv_flexray_transmit(cc_index, reg.index, reg.data, reg.length, reg.int_enable);
			}
			break;

		//----------------------------------------------------------------------
		// IOCTL_CATS_FLEXRAY_RECEIVE
		//----------------------------------------------------------------------
		case IOCTL_CATS_FLEXRAY_RECEIVE:
			{
				CFR_RECEIVE_REG reg;
				unsigned long ndat;
				int ndat_index, bit_no;

				// 受信バッファのポインタとインデックスをコピー(reg←arg)
				__copy_from_user(&reg, (const CFR_RECEIVE_REG*)arg, sizeof(CFR_RECEIVE_REG));

				// NDATnレジスタのnを求める
				ndat_index = reg.index / 32;

				// NDATnレジスタをリード
				ndat = catsdrv_flexray_read32(cc_index, NDAT1 + ndat_index * 4);

				// NDATnレジスタの何ビット目かを求める
				bit_no = reg.index % 32;

				if ((ndat & (1 << bit_no)) != 0)
				{
					// 受信データを取得
					catsdrv_flexray_receive(cc_index, reg.index, reg.data, &reg.length);

					reg.received = 1;
				}
				else
				{
					reg.length = 0;
					reg.received = 0;
				}

				// 受信データをコピー(arg←reg)
				ret_val = __copy_to_user((CFR_RECEIVE_REG*)arg, &reg, sizeof(CFR_RECEIVE_REG));
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

void catsdrv_flexray_setcmd(int cc_index, int cmd)
{
	unsigned long succ1 = 0;

	// READYコマンドならアンロックする
	if (cmd == CFR_READY)
	{
		// SUCC1レジスタの内容を読み出す
		succ1 = catsdrv_flexray_read32(cc_index, SUCC1);

		// アンロックシーケンスを実行
		catsdrv_flexray_write32(cc_index, LCK, FLEXRAY_LCK_KEY1);
		catsdrv_flexray_write32(cc_index, LCK, FLEXRAY_LCK_KEY2);
	}

	// コマンドを書き込む
	succ1 = (succ1 & 0xfffffff0) | (cmd & 0x0000000f);
	catsdrv_flexray_write32(cc_index, SUCC1, succ1);
}

void catsdrv_flexray_getpocstate(int cc_index, CFR_GETPOCSTATE_REG *reg)
{
	unsigned long ccsv;
	unsigned long ccev;

	// CCSVレジスタの内容を読み出す
	ccsv = catsdrv_flexray_read32(cc_index, CCSV);

	// CCEVレジスタの内容を読み出す
	ccev = catsdrv_flexray_read32(cc_index, CCEV);

	// Coldstart Noise Indicator
	if (((ccsv >> FLEXRAY_CCSV_CSNI) & 0x01) != 0)
		reg->coldstart_noise = 1;
	else
		reg->coldstart_noise = 0;

	// Halt Request
	if (((ccsv >> FLEXRAY_CCSV_HRQ) & 0x01) != 0)
		reg->halt_request = 1;
	else
		reg->halt_request = 0;

	// Freeze Status Indicator
	if (((ccsv >> FLEXRAY_CCSV_FSI) & 1) != 0)
		reg->freeze = 1;
	else
		reg->freeze = 0;

	reg->slot_mode     = (ccsv >> FLEXRAY_CCSV_SLM)  & 0x03;	// Slot Mode
	reg->wakeup_status = (ccsv >> FLEXRAY_CCSV_WSV)  & 0x07;	// Wakeup Stauts
	reg->error_mode    = (ccev >> FLEXRAY_CCEV_ERRM) & 0x03;	// Error Mode
	reg->poc_status    = (ccsv >> FLEXRAY_CCSV_POC)  & 0x3f;	// Protocol Operation Control Status
}

void catsdrv_flexray_transmit(int cc_index, unsigned long index, unsigned char *data, int length, int int_enable)
{
	unsigned long ibcr, data32;
	int txrq_index, bit_no, wait, i;

	for (i = 0; i < length; i += 4)
	{
		data32  = data[i + 3] << 24;
		data32 |= data[i + 2] << 16;
		data32 |= data[i + 1] << 8;
		data32 |= data[i + 0];

		// 4バイト分データ書き込み
		catsdrv_flexray_write32(cc_index, WRDS1 + i, data32);
	}

	// インプットバッファホストが空きになるまで待つ
	for (wait = 0; wait < 1000; wait++)
	{
		ibcr = catsdrv_flexray_read32(cc_index, IBCR_E);

		if ((ibcr & FLEXRAY_IBCR_IBSYH) == 0)
		{
			break;
		}

		udelay(1);
	}

	// データセクションをインプットバッファからメッセージRAMへ転送する
	catsdrv_flexray_write32(cc_index, IBCM, FLEXRAY_IBCM_STXRH | FLEXRAY_IBCM_LDSH);

	// メッセージバッファ番号を選択する
	catsdrv_flexray_write32(cc_index, IBCR_E, index);

	if (int_enable)
	{
		// TXRQnレジスタのnを求める
		txrq_index = index / 32;

		// TXRQnレジスタの何ビット目かを求める
		bit_no = index % 32;

		// 送信要求フラグセット
		txrq[cc_index][txrq_index] |= 1 << bit_no;

		// 送信完了割り込みを許可する
		catsdrv_flexray_write32(cc_index, SIES, FLEXRAY_SIES_TXIE);
	}
}

int catsdrv_flexray_gettxbuf(int cc_index)
{
	unsigned long txrq_reg, txrq_data;
	int txrq_index, bit_no, buf_no;

	txrq_reg = TXRQ1;

	buf_no = 0;

	// TXRQのレジスタ数分ループ
	for (txrq_index = 0; txrq_index < 4; txrq_index++)
	{
		txrq_data = catsdrv_flexray_read32(cc_index, txrq_reg);

		// 32ビット分ループ
		for (bit_no = 0; bit_no < 32; bit_no++)
		{
			// 送信要求があって送信済みになっている
			if ((((txrq[cc_index][txrq_index] >> bit_no) & 0x01) != 0) &&
				(((txrq_data                  >> bit_no) & 0x01) == 0))
			{
				 return buf_no;
			}

			buf_no++;
		}

		txrq_reg += 4;
	}

	 return -1;
}

void catsdrv_flexray_receive(int cc_index, unsigned long index, unsigned char *data, int *length)
{
	unsigned long obcm, obcr, rdhs2, plr, data32;
	int wait, i;

	// データセクションをメッセージRAMからアウトプットバッファへ転送する
	// ヘッダセクションをメッセージRAMからアウトプットバッファへ転送する
	obcm = catsdrv_flexray_read32(cc_index, OBCM);
	catsdrv_flexray_write32(cc_index, OBCM, obcm | FLEXRAY_OBCM_RDSS | FLEXRAY_OBCM_RHSS);

	// メッセージRAMからアウトプットバッファシャドウへ転送するメッセージバッファ番号を設定する
	catsdrv_flexray_write32(cc_index, OBCR, index & 0x007F);

	// メッセージRAMからアウトプットバッファシャドウへ転送する
	obcr = catsdrv_flexray_read32(cc_index, OBCR);
	catsdrv_flexray_write32(cc_index, OBCR, obcr | FLEXRAY_OBCR_REQ);

	// アウトプットバッファシャドウが空きになるまで待つ
	for (wait = 0; wait < 1000; wait++)
	{
		obcr = catsdrv_flexray_read32(cc_index, OBCR);

		if ((obcr & FLEXRAY_OBCR_OBSYS) == 0)
		{
			break;
		}

		udelay(1);
	}

	// アウトプットバッファシャドウとアウトプットバッファホストを入れ替える
	obcr = catsdrv_flexray_read32(cc_index, OBCR);
	catsdrv_flexray_write32(cc_index, OBCR, obcr | FLEXRAY_OBCR_VIEW);

	// データ長を取得
	rdhs2 = catsdrv_flexray_read32(cc_index, RDHS2);
	plr = (rdhs2 >> 24) & 0x7f;
	*length = plr * 2;

	// 受信データを取り出す
	for (i = 0; i < plr * 2; i += 4)
	{
		// 4バイト分データ読み出し
		data32 = catsdrv_flexray_read32(cc_index, RDDS1 + i);

		data[i + 3] = data32 >> 24;
		data[i + 2] = data32 >> 16;
		data[i + 1] = data32 >> 8;
		data[i + 0] = data32;
	}
}

unsigned long catsdrv_flexray_read32(int cc_index, unsigned long addr)
{
	unsigned long data;

	udelay(3);

	data = *(volatile unsigned long*)(cc_base_offset[cc_index] + addr);

#ifdef FLEXRAY_DRV_DEBUG
	printk(KERN_INFO "Read: addr=%08lx data=%08lx\n", cc_base_offset[cc_index] + addr, data);
#endif

	return data;
}

void catsdrv_flexray_write32(int cc_index, unsigned long addr, unsigned long data)
{

#ifdef FLEXRAY_DRV_DEBUG
	printk(KERN_INFO "Write: addr=%08lx data=%08lx\n", cc_base_offset[cc_index] + addr, data);
#endif

	udelay(3);

	*(volatile unsigned long*)(cc_base_offset[cc_index] + addr) = data;
}

