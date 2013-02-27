/**********************************************************************************
 * ファイル名      : cats_sja1000.c
 * システム名      : x86 プロトタイプＥＣＵ OPE-RA Ver2.0
 * サブシステム名  : カーネルドライバ
 * プログラム名    : ＣＡＮ制御／ＳＪＡ１０００デバイス制御
 * バージョン      : Ver1.01
 * モジュール構成  : cats_fpga.c		PCI Access
 *                 : cats_can.c			CAN Control
 * --------------------------------------------------------------------------------
 * 作成者          : 
 * 作成部署        : 
 * 作成日付        : 2009年09月10日 新規作成
 * 更新履歴        : 2010年02月23日
 **********************************************************************************/
#include <linux/pci.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/interrupt.h>
#include <linux/wait.h>
#include <linux/delay.h>
#include <linux/sched.h>
#include <linux/spinlock.h>
#include <asm/uaccess.h>
#include "cats_fpga.h"
#include "cats_sja1000.h"
#include "context.h"

// FPGA Read/Write delay time [ns]
#define FPGA_DELAY		3000

/// デバイス（SJA1000）の最大数
#define DEVICE_LIMIT	4

// for PeriCAN mode..
static const unsigned char SJA1000_speed_tbl[10][2] = {
	{ CAN_TIM0_10K,   CAN_TIM1_10K   },
	{ CAN_TIM0_20K,   CAN_TIM1_20K   },
	{ CAN_TIM0_50K,   CAN_TIM1_50K   },
	{ CAN_TIM0_100K,  CAN_TIM1_100K  },
	{ CAN_TIM0_125K,  CAN_TIM1_125K  },
	{ CAN_TIM0_250K,  CAN_TIM1_250K  },
	{ CAN_TIM0_500K,  CAN_TIM1_500K  },
	{ CAN_TIM0_800K,  CAN_TIM1_800K  },
	{ CAN_TIM0_1000K, CAN_TIM1_1000K }
};

static CAN_INTR_PAD *context = 0;

//-----------------------------------------------------------------//
/*!
	@brief	SJA1000 コンテキストの作成
*/
//-----------------------------------------------------------------//
void sja1000_create(void)
{
	size_t	size;

	size = sizeof(CAN_INTR_PAD) * DEVICE_LIMIT;
	if(context == 0) {
		context = (CAN_INTR_PAD *)kmalloc(size, GFP_KERNEL);
	}
	memset(context, 0, size);
}


//-----------------------------------------------------------------//
/*!
	@brief	SJA1000 コンテキストの破棄
*/
//-----------------------------------------------------------------//
void sja1000_destroy(void)
{
	kfree(context);
	context = 0;
}


//-----------------------------------------------------------------//
/*!
	@brief	SJA1000 ログの書き込み
	@param[in]	log_type		ログ種類
*/
//-----------------------------------------------------------------//
static void log_write(CAN_LOG *log, int log_type, int value)
{
	int	pos;

	pos = log->pos;
	log->buff[pos] = log_type | (value & LOG_VALUE_MASK);
	pos++;
	if(pos >= LOG_LIMIT) pos = 0;
	log->pos = pos;
	log->count++;
}


//-----------------------------------------------------------------//
/*!
	@brief	SJA1000 デバイスからの読み出し
	@param[in]	memptr	PCI バスのメモリーイメージ
	@param[in]	pad		送受信バッファ構造体のポインター
*/
//-----------------------------------------------------------------//
static void sja1000_transmit(int *memptr, int device, CAN_INTR_PAD *pad)
{
	CAN_FRAME	*frame;
	int			pos;

	if(pad->send_frame_get == pad->send_frame_put) return;

	frame = &pad->send_frame[pad->send_frame_get];
	if(frame->length) sja1000_send(memptr, device, frame);
	else sja1000_remoto_frame(memptr, device, frame);

	pos = pad->send_frame_get;
	pos++;
	if(pos >= pad->send_frame_max) {
		pos = 0;
	}
	pad->send_frame_get = pos;
}


//-----------------------------------------------------------------//
/*!
	@brief	SJA1000 デバイスからの読み出し
			CAN Read Control <CAN_RCTL>
			CAN controller  SJA1000 のレジスタにリードする際の制御を行います。
			MSB の "SJA Read" に 1 を立てて、該当する CAN のチャネルを設定して、
			このレジスタをライトすると、リード動作が行われ、
			リードが終了すると"SJA Read" をクリアして、"MFR Data" に
			リードデータを格納します。
			また、リードに関係なく、SJA1000からの割り込み信号をこのレジスタで
			表示します。

			Bit No.	Mode	Reset時	Description
			31	R/W	0	CAN Read
			30	RO	0	(Reserved)
			29:28	R/W	0	CAN select
			27	RO	-	CAN3_INT  CAN_INTピンの状態を示します。
			26	RO	-	CAN2_INT  CAN_INTピンの状態を示します。
			25	RO	-	CAN1_INT  CAN_INTピンの状態を示します。
			24	RO	-	CAN0_INT  CAN_INTピンの状態を示します。
			23:16	R/W	0	SJA1000 Address
			15:8	RO	0	(Reserved)
			7:0	RO	0	SJA1000 Data
	@param[in]	base	I/O 空間ベースアドレス
	@param[in]	dev		デバイス番号
	@param[in]	adr		レジスターアドレス
	@return SJA1000 のレジスター値
*/
//-----------------------------------------------------------------//
static inline int SJA1000_read(volatile int *base, int dev, int adr)
{
	int data;

#ifdef USE_PAGE_1
	data = *((char *)pcican_adr_map + (dev*0x80)+adr+0x1000);
#else
	base[CAN_RCTL] = (1 << 31) | ((dev & 3) << 28) | ((adr & 0xff) << 16);
	ndelay(FPGA_DELAY);
	data = base[CAN_RCTL] & 0xff;
	ndelay(FPGA_DELAY);
#endif
	return data;
}


//-----------------------------------------------------------------//
/*!
	@brief	SJA1000 デバイスからへの書き込み
			CAN Write Control <CAN_WCTL>
			CAN controller SJA1000のレジスタにライトする際の制御を行います。
			MSB の "SJA Write"に１を立てて、CANのチャネルを設定して、
			このレジスタをライトすると、ライト動作が行われます。

			Bit No.	Mode	Reset時	Description
			31	R/W	0	CAN Write
			30	RO	0	(Reserved)
			29:28	R/W	0	CAN select
			27:24	RO	0	(Reserved)
			23:16	R/W	0	SJA1000 Address
			15:8	RO	0	(Reserved)
			7:0	R/W	0	SJA 1000 Data
	@param[in]	base	I/O 空間ベースアドレス
	@param[in]	dev		デバイス番号
	@param[in]	adr		レジスターアドレス
	@param[in]	data	書き込むデータ
*/
//-----------------------------------------------------------------//
static inline void SJA1000_write(volatile int *base, int dev, int adr, int data)
{
#ifdef USE_PAGE_1
	*((char *)pcican_adr_map + (dev*0x80)+adr+0x1000) = data;
	ndelay(FPGA_DELAY);
#else
	base[CAN_WCTL] = (1 << 31) | ((dev & 3) << 28) | ((adr & 0xff) << 16) | (data & 0xff);
	ndelay(FPGA_DELAY);
#endif

}


//-----------------------------------------------------------------//
/*!
	@brief	SJA1000 デバイスのビットをセット（ 1 にする）
	@param[in]	base	I/O 空間ベースアドレス
	@param[in]	dev	デバイス番号
	@param[in]	adr	デバイス内アドレス
	@param[in]	bits	セットするビット列
*/
//-----------------------------------------------------------------//
static inline void SJA1000_bitset(volatile int *base, int dev, int adr, int bits)
{
	int temp;

	temp  = SJA1000_read(base, dev, adr);
	temp |= bits;
	SJA1000_write(base, dev, adr, temp);
}


//-----------------------------------------------------------------//
/*!
	@brief	SJA1000 デバイスのビットをリセット（ 0 にする）
	@param[in]	base	I/O 空間ベースアドレス
	@param[in]	dev	デバイス番号
	@param[in]	adr	デバイス内アドレス
	@param[in]	bits	セットするビット列
*/
//-----------------------------------------------------------------//
static inline void SJA1000_bitres(volatile int *base, int dev, int adr, int bits)
{
	int temp;

	temp  = SJA1000_read(base, dev, adr);
	temp &= ~bits;
	SJA1000_write(base, dev, adr, temp);
}


//-----------------------------------------------------------------//
/*!
	@brief	SJA1000 デバイスの通信速度を設定する。
	@param[in]	device	デバイス番号 (0、1)
	@param[in]	baud	通信速度@n
						10, 20, 50, 100, 125, 250, 500, 800, 1000 [Kbits/sed]
	@param[in]	btr0	直接指定値（BTR0）
	@param[in]	btr1	直接指定値（BTR1）
*/
//-----------------------------------------------------------------//
static void SJA1000_set_boudrate(volatile int *base, int device, int baud, int btr0, int btr1)
{
	int i;

	switch(baud) {
    case    0: break;
	case   10: i = 0; break;
	case   20: i = 1; break;
	case   50: i = 2; break;
	case  100: i = 3; break;
	case  125: i = 4; break;
	case  250: i = 5; break;
	case  500: i = 6; break;
	case  800: i = 7; break;
	case 1000: i = 8; break;
	default  :
		i = 3;
    }

//	SJA1000_write(base, device, CAN_ADR_CLOCK_DIVIDER,
//			CAN_MODE_PELICAN | CAN_MODE_CLK | CAN_MODE_CBP);

	if(baud == 0) {
		SJA1000_write(base, device, CAN_ADR_BUS_TIMING_0, btr0);
		SJA1000_write(base, device, CAN_ADR_BUS_TIMING_1, btr1);
	} else {
		SJA1000_write(base, device, CAN_ADR_BUS_TIMING_0, SJA1000_speed_tbl[i][0]);
		SJA1000_write(base, device, CAN_ADR_BUS_TIMING_1, SJA1000_speed_tbl[i][1]);
	}
}


//-----------------------------------------------------------------//
/*!
	@brief	TJA1040 CAN Transceiver 制御。
			CAN Transceiver Pin Control <CAN_TCTL>
			CAN transceiver  TJA1040のピンの制御を行います。
			TJA1040は、STBがHighで"Standby Mode"、Lowで "Normal Mode"になります。
			詳しくは、TJA1040仕様書のTable４を参照して下さい。

			Bit No.	Mode	Reset時	Description
			31		RO	-	CAN3_RxD  CAN3_RxD ピンの状態を示します。
			30		RO	-	CAN2_RxD  CAN2_RxD ピンの状態を示します。
			29		RO	-	CAN1_RxD  CAN1_RxD ピンの状態を示します。
			28		RO	-	CAN0_RxD  CAN0_RxD ピンの状態を示します。
			27:4	RO	0	(Reserved)
			3		R/W	1	CAN3_STB    CAN3_STBピンの設定値
			2		R/W	1	CAN2_STB    CAN2_STBピンの設定値
			1		R/W	1	CAN1_STB    CAN1_STBピンの設定値
			0		R/W	1	CAN0_STB    CAN0_STBピンの設定値
	@param[in]	base	I/O 空間ベースアドレス
	@param[in]	dev	 デバイス番号 ( 0 to 3 )
	@param[in]	value	ビットの状態（ 0, 1 )
 */
//-----------------------------------------------------------------//
static inline void TJA1040_stb(volatile int *base, int device, int value)
{
	if(value) {
		base[CAN_TCTL] |=  (1 << device);
	} else {
		base[CAN_TCTL] &= ~(1 << device);
	}
}


//-----------------------------------------------------------------//
/*!
	@brief	TJA1040 CAN Transceiver 制御。
			CAN Transceiver Pin Control <CAN_TCTL>
			CAN transceiver  TJA1040のピンの制御を行います。
			TJA1040は、STBがHighで"Standby Mode"、Lowで "Normal Mode"になります。
			詳しくは、TJA1040仕様書のTable４を参照して下さい。

			Bit No.	Mode	Reset時	Description
			31		RO	-	CAN3_RxD  CAN3_RxD ピンの状態を示します。
			30		RO	-	CAN2_RxD  CAN2_RxD ピンの状態を示します。
			29		RO	-	CAN1_RxD  CAN1_RxD ピンの状態を示します。
			28		RO	-	CAN0_RxD  CAN0_RxD ピンの状態を示します。
			27:4	RO	0	(Reserved)
			3		R/W	1	CAN3_STB    CAN3_STBピンの設定値
			2		R/W	1	CAN2_STB    CAN2_STBピンの設定値
			1		R/W	1	CAN1_STB    CAN1_STBピンの設定値
			0		R/W	1	CAN0_STB    CAN0_STBピンの設定値

	@param[in]	device	デバイス番号 ( 0 or 1 )
	@return	「RxD」ピンの状態（ 0, 1 )
 */
//-----------------------------------------------------------------//
static inline int TJA1040_RxD(volatile int *base, int device)
{
	if(base[CAN_TCTL] & (1 << (device + 28))) return 1;
	else return 0;
}


//-----------------------------------------------------------------//
/*!
	@brief	SJA1000 デバイスを read
	@param[in]	memptr	PCI バスのメモリーイメージ
	@param[in]	device	デバイス番号
	@param[in]	address	値
	@return register's value
 */
//-----------------------------------------------------------------//
int sja1000_read(int *memptr, int device, int address)
{
	return SJA1000_read(memptr, device, address);
}


//-----------------------------------------------------------------//
/*!
	@brief	SJA1000 デバイスを write.
	@param[in]	memptr	PCI バスのメモリーイメージ
	@param[in]	device	デバイス番号
	@param[in]	address	値
	@param[in]	data	書き込むデータ
	@return register's value
 */
//-----------------------------------------------------------------//
int sja1000_write(int *memptr, int device, int address, int data)
{
	if(address == 0x100) {
		 TJA1040_stb(memptr, device, data);
	} else {
	  	 SJA1000_write(memptr, device, address, data);
	}
	return CASTCAN_SUCCEEDED;
}


//-----------------------------------------------------------------//
/*!
	@brief	SJA1000 操作関係の初期化@n
			ワークメモリーの初期化など
	@param[in]	memptr	PCI バスのメモリーイメージ
	@param[in]	init	CAN_INIT 構造体
	@return 正常なら「０」を返す。
*/
//-----------------------------------------------------------------//
int sja1000_init(int *memptr, CAN_INIT *init)
{
	int		device;
	int		tmp;
	CAN_INTR_PAD	*pad;
	volatile int	*vmemptr = (volatile int *)memptr;

	device = init->device;
	if(device >= DEVICE_LIMIT) {
		return CATSCAN_INVALID_DEVICENUM;
	}

	if(init->send_buff_size > BUFFER_LIMIT) {
		return CATSCAN_SEND_BUFFER_TOO_BIG;
	}

	if(init->recv_buff_size > BUFFER_LIMIT) {
		return CATSCAN_RECV_BUFFER_TOO_BIG;
	}

	printk(KERN_INFO "catsfpga.SJA1000_init: (%d) send buff:%d, recv buff: %d\n",
		device, init->send_buff_size, init->recv_buff_size);

	// コールバック構造体をコピー
	pad = &context[device];

	pad->send_frame_max = init->send_buff_size;
	pad->send_frame_get = 0;
	pad->send_frame_put = 0;
	pad->send_kick = 0;
	pad->send_count = 0;

	spin_lock_init(&pad->send_lock);

	pad->recv_frame_max = init->recv_buff_size;
	pad->recv_frame_get = 0;
	pad->recv_frame_put = 0;

	pad->recv_count = 0;

	spin_lock_init(&pad->recv_lock);

	pad->interrupt_count = 0;
	pad->busoff_error_count = 0;
	pad->arbitr_lost_count = 0;
	pad->passive_error_count = 0;
	pad->wakeup_count = 0;
	pad->overrun_error_count = 0;
	pad->error_count = 0;

	pad->recv_buff_overrun_count = 0;
	pad->send_buff_overrun_count = 0;

	pad->log.pos = 0;
	pad->log.count = 0;

// CAN Transceviver Pin Control
	TJA1040_stb(memptr, device, 0);		// Normal Mode

// FPGA/SJA1000 割り込み許可設定 CAN0(17), CAN1(18), CAN2(19), CAN3(20)
    tmp = vmemptr[INT_EN];
	tmp |= (1<<17) | (1<<18) | (1<<19) | (1<<20);
//	tmp |= 1 << (17 + device);
	vmemptr[INT_EN] = tmp;

#ifndef NDEBUG
	printk(KERN_INFO "catsfpga.interrupt enable: (%d), 0x%08X\n", device, tmp);
#endif

#ifndef NDEBUG
	tmp = vmemptr[INT_LVL];
	printk(KERN_INFO "catsfpga.interrupt level:  (%d), 0x%08X\n", device, tmp);
#endif

#if 0
	tmp = vmemptr[INT_ST];
	ndelay(FPGA_DELAY);
	printk(KERN_INFO "catsfpga.interrupt status: (%d), 0x%08X\n", device, tmp);
#endif

	return CASTCAN_SUCCEEDED;
}


//-----------------------------------------------------------------//
/*!
	@brief	SJA1000 デバイスをリセットする。@n
			ボーレートの設定など。
	@param[in]	memptr	PCI バスのメモリーイメージ
	@param[in]	device	デバイス番号
	@param[in]	mode	モード設定構造体のポインター
	@return 正常なら「０」を返す。
 */
//-----------------------------------------------------------------//
int sja1000_reset(int *memptr, int device, CAN_MODE *mode)
{
	int ret, i;

	if(device >= DEVICE_LIMIT) {
		return CATSCAN_INVALID_DEVICENUM;
	}

// printk(KERN_INFO "catsfpga.SJA1000_reset device=%d, baud=%d\n", device, baud);
// RESET / アクセプト・フィルターを有効
	SJA1000_write(memptr, device, CAN_ADR_MODE, CAN_RESET_REQUEST | CAN_ACC_FILT_MASK);

	// RESET delay device.
	udelay(100);

	ret = SJA1000_read(memptr, device, CAN_ADR_STATUS);

	ret = SJA1000_read(memptr, device, CAN_ADR_MODE);
	if((ret & CAN_RESET_REQUEST) == 0) {
		printk(KERN_INFO "catsfpga.SJA1000_reset: 'can't reset device' (%d)\n", device);
		return 1;
	}

	// turn into PeliCAN - mode with Clock Divide, CBP
	// Turn on 'CBP' bit -> RX1 input cancel. (only RX0 input)
	SJA1000_write(memptr, device, CAN_ADR_CLOCK_DIVIDER,
			CAN_CDR_PELICAN | CAN_CDR_CLK_OFF | CAN_CDR_CBP);

	// error counter(TX/RX) reset.
	SJA1000_write(memptr, device, CAN_ADR_RX_ERROR_COUNT, 0);
	SJA1000_write(memptr, device, CAN_ADR_TX_ERROR_COUNT, 0);

	// アクセプトフィルター設定（RESET 時のみ設定できる）
	SJA1000_write(memptr, device, CAN_ADR_ACR0, mode->accept_code[0]);
	SJA1000_write(memptr, device, CAN_ADR_ACR1, mode->accept_code[1]);
	SJA1000_write(memptr, device, CAN_ADR_ACR2, mode->accept_code[2]);
	SJA1000_write(memptr, device, CAN_ADR_ACR3, mode->accept_code[3]);

	SJA1000_write(memptr, device, CAN_ADR_AMR0, mode->accept_mask[0]);
	SJA1000_write(memptr, device, CAN_ADR_AMR1, mode->accept_mask[1]);
	SJA1000_write(memptr, device, CAN_ADR_AMR2, mode->accept_mask[2]);
	SJA1000_write(memptr, device, CAN_ADR_AMR3, mode->accept_mask[3]);

// FIFO をクリア
   for(i = 0; i < 64; ++i) {
       SJA1000_write(memptr, device, CAN_ADR_FIFO_TOP + i, 0x00);
   }
// TX buffer をクリア
   for(i = 0; i < 15; ++i) {
       SJA1000_write(memptr, device, CAN_ADR_TX_BUFFER_TOP + i, 0x00);
   }

	SJA1000_set_boudrate(memptr, device, mode->baud_rate, mode->BTR0, mode->BTR1);

	// output controle
	// TXD:0 ---> TX0:vss, TX1:vcc (push pull mode)
	// TXD:1 ---> TX0:vcc, TX1:vss (push pull mode)
	// OCMODE bits: normal output mode.
	SJA1000_write(memptr, device, CAN_ADR_OUTPUT_CONTROL,
 	   (CAN_OCMODE1 | CAN_OCTP0 | CAN_OCTN0));
// 	   (CAN_OCMODE1 | CAN_OCTP0 | CAN_OCTN0 | CAN_OCPOL0));

	{
	int *acr = (int *)mode->accept_code;
	int *amr = (int *)mode->accept_mask;
	printk(KERN_INFO "catsfpga.SJA1000_reset: (%d) baud = %d, ACR: %08X, AMR: %08X\n",
					 device, mode->baud_rate, *acr, *amr);
	}


	return CASTCAN_SUCCEEDED;
}


//-----------------------------------------------------------------//
/*!
	@brief	SJA1000 デバイスを開始する。
	@param[in]	memptr	PCI バスのメモリーイメージ
	@param[in]	device	デバイス番号
	@return 正常なら「０」を返す。
*/
//-----------------------------------------------------------------//
int sja1000_start(int *memptr, int device)
{
	int		i;

	if(device >= DEVICE_LIMIT) {
		return CATSCAN_INVALID_DEVICENUM;
	}

	// Interrupts Status リセット
	SJA1000_read(memptr, device, CAN_ADR_INTERRUPT);

	SJA1000_write(memptr, device, CAN_ADR_INTERRUPT_ENABLE, (
		CAN_TRANSMIT_INT_ENABLE |
		CAN_RECEIVE_INT_ENABLE |
		CAN_OVERRUN_INT_ENABLE |
 		CAN_WAKEUP_INT_ENABLE |
		CAN_ERROR_INT_ENABLE |
		CAN_ERROR_PASSIVE_INT_ENABLE |
		CAN_ARBITR_LOST_INT_ENABLE
		));

	// 受信バッファをクリア
	for(i = 0; i < 16; ++i) {
		SJA1000_write(memptr, device, CAN_ADR_COMMAND, CAN_RELEASE_RECEIVE_BUFFER);
	}

	// リセット解除（アクセプト・フィルター有効）
	SJA1000_write(memptr, device, CAN_ADR_MODE, CAN_ACC_FILT_MASK);

	printk(KERN_INFO "catsfpga.SJA1000_start: (%d)\n", device);

	return CASTCAN_SUCCEEDED;
}


//-----------------------------------------------------------------//
/*!
	@brief	SJA1000 デバイスを停止する。
	@param[in]	memptr	PCI バスのメモリーイメージ
	@param[in]	device	デバイス番号
	@return 正常なら「０」を返す。
*/
//-----------------------------------------------------------------//
int sja1000_stop(int *memptr, int device)
{
	if(device >= DEVICE_LIMIT) {
		return CATSCAN_INVALID_DEVICENUM;
	}


	SJA1000_write(memptr, device, CAN_ADR_MODE, CAN_RESET_REQUEST);

	SJA1000_write(memptr, device, CAN_ADR_INTERRUPT_ENABLE, 0x00);

	printk(KERN_INFO "catsfpga.SJA1000_stop: (%d)\n", device);

	return 0;
}


//-----------------------------------------------------------------//
/*!
	@brief	SJA1000 デバイスを Sleep モードにする。
	@param[in]	memptr	PCI バスのメモリーイメージ
	@param[in]	device	デバイス番号
	@return 正常なら「０」を返す。
*/
//-----------------------------------------------------------------//
int sja1000_sleep(int *memptr, int device)
{
	int i;

	if(device >= DEVICE_LIMIT) {
		return CATSCAN_INVALID_DEVICENUM;
	}

	// Interrupts Status リセット
	SJA1000_read(memptr, device, CAN_ADR_INTERRUPT);

	SJA1000_write(memptr, device, CAN_ADR_INTERRUPT_ENABLE, (
		CAN_TRANSMIT_INT_ENABLE |
		CAN_RECEIVE_INT_ENABLE |
		CAN_OVERRUN_INT_ENABLE |
 		CAN_WAKEUP_INT_ENABLE |
		CAN_ERROR_INT_ENABLE |
		CAN_ERROR_PASSIVE_INT_ENABLE |
		CAN_ARBITR_LOST_INT_ENABLE
		));

	// 受信バッファをクリア
	for(i = 0; i < 16; ++i) {
		SJA1000_write(memptr, device, CAN_ADR_COMMAND, CAN_RELEASE_RECEIVE_BUFFER);
	}

	// リセット解除（アクセプト・フィルター有効）
	SJA1000_write(memptr, device, CAN_ADR_MODE, CAN_ACC_FILT_MASK | CAN_SLEEP_MODE);

	printk(KERN_INFO "catsfpga.SJA1000_sleep: (%d)\n", device);

	return CASTCAN_SUCCEEDED;
}


//-----------------------------------------------------------------//
/*!
	@brief	SJA1000 各種ステータスを受け取る
	@param[in]	memptr	PCI バスのメモリーイメージ
	@param[in]	device	デバイス番号
	@param[in]	status	ステータス構造体のポインター
	@return 正常なら「０」以外を返す。
*/
//-----------------------------------------------------------------//
int sja1000_status(int *memptr, int device, CAN_STATUS *status)
{
	CAN_INTR_PAD	*pad;
	int		n;

	if(device >= DEVICE_LIMIT) {
		return CATSCAN_INVALID_DEVICENUM;
	}

	pad = &context[device];

	n = pad->recv_frame_put - pad->recv_frame_get;
	if(n < 0) n += pad->recv_frame_max;
	status->recv_buff_count = n;
	status->recv_count = pad->recv_count;

	n = pad->send_frame_put - pad->send_frame_get;
	if(n < 0) n += pad->send_frame_max;
	status->send_buff_count = n;
	status->send_count = pad->send_count;

	status->rx_error_count = pad->rx_error_count;
	status->tx_error_count = pad->tx_error_count;
	status->rx_message_count = pad->rx_message_count;

	status->interrupt_count = pad->interrupt_count;
	status->busoff_error_count  = pad->busoff_error_count;
	status->arbitr_lost_count   = pad->arbitr_lost_count;
	status->passive_error_count = pad->passive_error_count;
	status->wakeup_count        = pad->wakeup_count;
	status->overrun_error_count = pad->overrun_error_count;
	status->error_count         = pad->error_count;

	status->recv_buff_overrun_count = pad->recv_buff_overrun_count;
    status->send_buff_overrun_count = pad->send_buff_overrun_count;


	return CASTCAN_SUCCEEDED;
}


//-----------------------------------------------------------------//
/*!
	@brief	SJA1000 メッセージを送信バッファにセットして送信要求
	@param[in]	memptr	PCI バスのメモリーイメージ
	@param[in]	device	デバイス番号
	@param[in]	cf		CAN_FRAME 構造体
	@return 正常なら「０」を返す。
*/
//-----------------------------------------------------------------//
int sja1000_remoto_frame(int *memptr, int device, const CAN_FRAME *cf)
{
	if(device >= DEVICE_LIMIT) {
		return CATSCAN_INVALID_DEVICENUM;
	}

//	extended frame ID (29Bits)
//	---xxxxx xxxyyyyy yyyzzzzz zzzwwwww
//	standard frame ID (11Bits)
//	-------- -------- -----xxx xxxxxyyy
	if(cf->type) {
		SJA1000_write(memptr, device, CAN_ADR_TX_FRAME_INFO, CAN_EFF | CAN_RTR | cf->dlc);
		SJA1000_write(memptr, device, CAN_ADR_TX_IDENTIFIRE1, cf->id >> 21);
		SJA1000_write(memptr, device, CAN_ADR_TX_IDENTIFIRE2, cf->id >> 13);
		SJA1000_write(memptr, device, CAN_ADR_TX_IDENTIFIRE3, cf->id >> 5);
		SJA1000_write(memptr, device, CAN_ADR_TX_IDENTIFIRE4, ((cf->id & 0x1f) << 5));
	} else {
		SJA1000_write(memptr, device, CAN_ADR_TX_FRAME_INFO, CAN_SFF | CAN_RTR | cf->dlc);
		SJA1000_write(memptr, device, CAN_ADR_TX_IDENTIFIRE1, cf->id >> 3);
		SJA1000_write(memptr, device, CAN_ADR_TX_IDENTIFIRE2, ((cf->id & 0x7) << 5));
	}

	// 送信要求をセット
	SJA1000_write(memptr, device, CAN_ADR_COMMAND, CAN_TRANSMISSION_REQUEST);

	return CASTCAN_SUCCEEDED;
}


//-----------------------------------------------------------------//
/*!
	@brief	SJA1000 メッセージを送信レジスターにセットして送信要求
	@param[in]	memptr	PCI バスのメモリーイメージ
	@param[in]	device	デバイス番号
	@param[in]	cf		CAN_FRAME 構造体
	@return 正常なら「０」を返す。
*/
//-----------------------------------------------------------------//
int sja1000_send(int *memptr, int device, const CAN_FRAME *cf)
{
	int i, cmd, len;

	if(device >= DEVICE_LIMIT) {
		return CATSCAN_INVALID_DEVICENUM;
	}

	if( cf->type ) {
		SJA1000_write(memptr, device, CAN_ADR_TX_FRAME_INFO, CAN_EFF | (cf->dlc & 0x0f));
    	SJA1000_write(memptr, device, CAN_ADR_TX_IDENTIFIRE1, cf->id >> 21);
    	SJA1000_write(memptr, device, CAN_ADR_TX_IDENTIFIRE2, cf->id >> 13);
    	SJA1000_write(memptr, device, CAN_ADR_TX_IDENTIFIRE3, cf->id >> 5);
    	SJA1000_write(memptr, device, CAN_ADR_TX_IDENTIFIRE4, ((cf->id & 0x1f) << 3));
    	cmd = CAN_ADR_TX_EXT_DATA1;
	} else {
	  	SJA1000_write(memptr, device, CAN_ADR_TX_FRAME_INFO, CAN_SFF | (cf->dlc & 0x0f));
		SJA1000_write(memptr, device, CAN_ADR_TX_IDENTIFIRE1, cf->id >> 3);
 		SJA1000_write(memptr, device, CAN_ADR_TX_IDENTIFIRE2, ((cf->id & 0x7) << 5));
		cmd = CAN_ADR_TX_STD_DATA1;
	}

	if(cf->length > 8) len = 8; else len = cf->length;
	for(i = 0; i < len; ++i) {
  		SJA1000_write(memptr, device, cmd + i, cf->data[i]);
	}

  // 送信要求をセット
  	SJA1000_write(memptr, device, CAN_ADR_COMMAND, CAN_TRANSMISSION_REQUEST);
  //printk("CanSend%d: %x %d %s\n", device, cf->id, cf->length, cf->data);
    return CASTCAN_SUCCEEDED;
}


//-----------------------------------------------------------------//
/*!
	@brief	SJA1000 受信メッセージレジスターを読み込む。
			受信バッファのデータを受け取ったら、「Release Receive Buffer」
			ビットを立てる。
	@param[in]	memptr	PCI バスのメモリーイメージ
	@param[in]	device	デバイス番号
	@param[in]	cf		CAN_FRAME 構造体
	@return 正常なら「０」を返す。
*/
//-----------------------------------------------------------------//
int sja1000_recv(int *memptr, int device, CAN_FRAME *cf)
{
	int i, cmd;

	if(device >= DEVICE_LIMIT) {
		return CATSCAN_INVALID_DEVICENUM;
	}

	cmd = SJA1000_read(memptr, device, CAN_ADR_RX_FRAME_INFO);
	cf->dlc = cmd & 0xf;
	if(cmd & CAN_EFF) {
		cf->type = 1;
		i  =  SJA1000_read(memptr, device, CAN_ADR_RX_IDENTIFIRE1) << 21;
		i |=  SJA1000_read(memptr, device, CAN_ADR_RX_IDENTIFIRE2) << 13;
		i |=  SJA1000_read(memptr, device, CAN_ADR_RX_IDENTIFIRE3) << 5;
		i |= (SJA1000_read(memptr, device, CAN_ADR_RX_IDENTIFIRE4) >> 3) & 0x1f;
		cmd = CAN_ADR_RX_EXT_DATA1;
	} else {
		cf->type = 0;
		i  =  SJA1000_read(memptr, device, CAN_ADR_RX_IDENTIFIRE1) << 3;
		i |= (SJA1000_read(memptr, device, CAN_ADR_RX_IDENTIFIRE2) >> 5) & 0x7;
		cmd = CAN_ADR_RX_STD_DATA1;
	}
	cf->id = i;
	if(cf->dlc < 8) cf->length = cf->dlc; else cf->length = 8;
	for(i = 0; i < cf->length; ++i) {
	  cf->data[i] = SJA1000_read(memptr, device, cmd + i);
	}

	// 受信バッファをリリース
	SJA1000_write(memptr, device, CAN_ADR_COMMAND, CAN_RELEASE_RECEIVE_BUFFER);

	//printk("CanRecv%d: %x %d %s\n", device, cf->id, cf->length, cf->data);
	return CASTCAN_SUCCEEDED;
}


//-----------------------------------------------------------------//
/*!
	@brief	SJA1000 送信バッファへメッセージを書き込む。
	@param[in]	memptr	PCI バスのメモリーイメージ
	@param[in]	send	送信構造体
*/
//-----------------------------------------------------------------//
int sja1000_send_buff(int *memptr, CAN_SEND *send)
{
	CAN_INTR_PAD	*pad;
	CAN_FRAME		*frame;
	int				pos;
    int             newpos;
	int				device;
	unsigned long	flags;
//	volatile int	sync;

	device = send->device;

	if(device >= DEVICE_LIMIT) {
		return CATSCAN_INVALID_DEVICENUM;
	}
	pad = &context[device];

	spin_lock_irqsave(&pad->send_lock, flags);
    
    // 次の位置の確認
    newpos = (pad->send_frame_put + 2) % pad->send_frame_max;
    if (newpos == pad->send_frame_get) {
      // 送信バッファが満杯
      pad->send_buff_overrun_count++;
      spin_unlock_irqrestore(&pad->send_lock, flags);
      return CATSCAN_SEND_BUFFER_OVERFLOW; // エラー
    }

	pos = pad->send_frame_put;
	frame = &pad->send_frame[pos];
	memcpy(frame, &send->frame, sizeof(CAN_FRAME));

    // 次のposへ
    pad->send_frame_put = (pos + 1) % pad->send_frame_max;

	if(pad->send_kick == 0) {
		pad->send_kick = 1;
		sja1000_transmit(memptr, send->device, pad);
	}

	spin_unlock_irqrestore(&pad->send_lock, flags);
	return CASTCAN_SUCCEEDED;
}


//-----------------------------------------------------------------//
/*!
	@brief	SJA1000 受信バッファメッセージを読み込む。
	@param[in]	memptr	PCI バスのメモリーイメージ
	@param[in]	recv	受信構造体
*/
//-----------------------------------------------------------------//
int sja1000_recv_buff(int *memptr, CAN_RECV *recv)
{
	CAN_INTR_PAD	*pad;
	CAN_FRAME		*frame;
	int				pos;
	int				device;
	unsigned long	flags;

	device = recv->device;

	if(device >= DEVICE_LIMIT) {
		return CATSCAN_INVALID_DEVICENUM;
	}

	pad = &context[device];

	spin_lock_irqsave(&pad->recv_lock, flags);
	if(pad->recv_frame_get == pad->recv_frame_put) {
		spin_unlock_irqrestore(&pad->recv_lock, flags);
		return 1;
	}

	pos = pad->recv_frame_get;
	frame = &pad->recv_frame[pos];
	memcpy(&recv->frame, frame, sizeof(CAN_FRAME));
	pos++;
	if(pos >= pad->recv_frame_max) {
		pos = 0;
	}
	pad->recv_frame_get = pos;
	spin_unlock_irqrestore(&pad->recv_lock, flags);

	return CASTCAN_SUCCEEDED;
}


//-----------------------------------------------------------------//
/*!
	@brief	SJA1000 の割り込みステータスを得る、@n
			ステータスをクリアする。
	@param[in]	memptr	PCI バスのメモリーイメージ
	@param[in]	device	デバイス番号
	@return		割り込みステータス・フラグ
 */
//-----------------------------------------------------------------//
int sja1000_get_interrupt_status(int *memptr, int device)
{
	// CAN_ADR_INTERRUPT を読み出すと割り込み要因は全てクリアされる！
	return SJA1000_read(memptr, device, CAN_ADR_INTERRUPT);
}


//-----------------------------------------------------------------//
/*!
	@brief	SJA1000 タスクレット処理
	@param[in]	memptr	PCI バスのメモリーイメージ
	@param[in]	device	デバイス番号
	@param[in]	status	割り込みステータス・フラグ
 */
//-----------------------------------------------------------------//
void sja1000_tasklet_core(int *memptr, int device, int status)
{
	CAN_INTR_PAD	*pad;
	CAN_FRAME		*frame;
	int				pos;
	int				dummy;	//dummy for Read 

// printk(KERN_INFO "catsfpga.SJA1000_tasklet.\n");
//
	pad = &context[device];

	log_write(&pad->log, LOG_TYPE_INTI, pad->interrupt_count);

	pad->rx_error_count = SJA1000_read(memptr, device, CAN_ADR_RX_ERROR_COUNT);
	pad->tx_error_count = SJA1000_read(memptr, device, CAN_ADR_TX_ERROR_COUNT);
	pad->rx_message_count = SJA1000_read(memptr, device, CAN_ADR_RX_MESSAGE_COUNTER);

	if(status & CAN_ERROR_BUSOFF_INT_ENABLE) {
		dummy = SJA1000_read(memptr, device, CAN_ADR_ERROR_CODE_CAPTURE);	//for dummy read ECC
//		printk(KERN_INFO "===== sja1000_tasklet_core: CAN_ERROR_BUSOFF_INT_ENABLE  status = %x\n",status);
		log_write(&pad->log, LOG_TYPE_BUSOFF, pad->busoff_error_count);
		pad->busoff_error_count++;
	}

	if(status & CAN_ARBITR_LOST_INT_ENABLE) {
		dummy = SJA1000_read(memptr, device, CAN_ADR_ARBITRATION_LOST_CAPTURE);	//for dummy read ALC
//		printk(KERN_INFO "===== sja1000_tasklet_core: CAN_ARBITR_LOST_INT_ENABLE  status = %x\n",status);
		log_write(&pad->log, LOG_TYPE_LOST, pad->arbitr_lost_count);
		pad->arbitr_lost_count++;
	}

	if(status & CAN_ERROR_PASSIVE_INT_ENABLE) {
//		printk(KERN_INFO "===== sja1000_tasklet_core: CAN_ERROR_PASSIVE_INT_ENABLE\n");
		log_write(&pad->log, LOG_TYPE_PASSIVE, pad->passive_error_count);
		pad->passive_error_count++;
	}

	if(status & CAN_WAKEUP_INT_ENABLE) {
//		printk(KERN_INFO "===== sja1000_tasklet_core: CAN_WAKEUP_INT_ENABLE\n");
		log_write(&pad->log, LOG_TYPE_WAKEUP, pad->wakeup_count);
		pad->wakeup_count++;
	}

	if(status & CAN_OVERRUN_INT_ENABLE) {
//		printk(KERN_INFO "===== sja1000_tasklet_core: CAN_OVERRUN_INT_ENABLE\n");
		log_write(&pad->log, LOG_TYPE_OVERRUN, pad->overrun_error_count);
		pad->overrun_error_count++;
		// オーバーラン・エラーフラグを除去
		SJA1000_write(memptr, device, CAN_ADR_COMMAND, CAN_CLEAR_OVERRUN_STATUS);
	}

	if(status & CAN_ERROR_INT_ENABLE) {
//		printk(KERN_INFO "===== sja1000_tasklet_core: CAN_ERROR_INT_ENABLE\n");
		log_write(&pad->log, LOG_TYPE_ERROR, pad->error_count);
		pad->error_count++;
	}

	if(status & CAN_TRANSMIT_INT_ENABLE) {
//		printk(KERN_INFO "===== sja1000_tasklet_core: CAN_TRANSMIT_INT_ENABLE\n");
		unsigned long flags;
		spin_lock_irqsave(&pad->send_lock, flags);

		pad->send_count++;

		pos = pad->send_frame_get;
		pos--;
		if(pos < 0) pos += pad->send_frame_max;
		frame = &pad->send_frame[pos];

		log_write(&pad->log, LOG_TYPE_SEND, frame->frame_id);

// printk(KERN_INFO "catsfpga.SJA1000_tasklet: transmit interrupt. (%d)\n",
// (pad->send_frame_put - pad->send_frame_get)
// );
		// 送るもんがあれば続けて送信
		if(pad->send_frame_get != pad->send_frame_put) {
			sja1000_transmit(memptr, device, pad);
		} else {
		    pad->send_kick = 0;
		}
		spin_unlock_irqrestore(&pad->send_lock, flags);
	}

	if(status & CAN_RECEIVE_INT_ENABLE) {
		unsigned long flags;
		spin_lock_irqsave(&pad->recv_lock, flags);
		pad->recv_count++;
// printk(KERN_INFO "catsfpga.SJA1000_tasklet: receive interrupt.\n");

		pos = pad->recv_frame_put;
		frame = &pad->recv_frame[pos];
		pos++;
		if(pos >= pad->recv_frame_max) {
			pos = 0;
		}
		if(pad->recv_frame_get == pos) pad->recv_buff_overrun_count++;
		else pad->recv_frame_put = pos;
		sja1000_recv(memptr, device, frame);
		spin_unlock_irqrestore(&pad->recv_lock, flags);

		log_write(&pad->log, LOG_TYPE_RECV, frame->frame_id);
	}

	log_write(&pad->log, LOG_TYPE_INTO, pad->interrupt_count);
	pad->interrupt_count++;
}


//-----------------------------------------------------------------//
/*!
	@brief	SJA1000 終了処理
	@param[in]	memptr	PCI バスのメモリーイメージ
	@param[in]	device	デバイス番号
 */
//-----------------------------------------------------------------//
int sja1000_exit(int *memptr, int device)
{
	if(device >= DEVICE_LIMIT) {
		return -1;
	}

	return CASTCAN_SUCCEEDED;
}


//-----------------------------------------------------------------//
/*!
	@brief	SJA1000 ログ取得
	@param[in]	memptr	PCI バスのメモリーイメージ
	@param[in]	device	デバイス番号
	@param[in]	log		ログ構造体ポインター
	@return 正常なら「０」を返す。
 */
//-----------------------------------------------------------------//
int sja1000_log(int *memptr, int device, CAN_LOG *log)
{
	CAN_INTR_PAD	*pad;

	if(device >= DEVICE_LIMIT) {
		return CATSCAN_INVALID_DEVICENUM;
	}

	pad = &context[device];
// printk(KERN_INFO "catsfpga.SJA1000_log: %d\n", pad->log.count);
	memcpy(log, &pad->log, sizeof(CAN_LOG));

	return 0;
}

/* for debug - sja1000_read_proc */
int sja1000_get_ringinfo(int *memptr, int device, int *ret)
{
	CAN_INTR_PAD	*pad;

	if(device >= DEVICE_LIMIT) {
		return CATSCAN_INVALID_DEVICENUM;
	}

	pad = &context[device];
	ret[0] = pad->send_frame_max;
	ret[1] = pad->send_frame_put;
	ret[2] = pad->send_frame_get;
	ret[3] = pad->send_kick;
	ret[4] = pad->recv_frame_max;
	ret[5] = pad->recv_frame_put;
	ret[6] = pad->recv_frame_get;
	return 0;
}

int sja1000_deinit(int *memptr, int device)
{
	volatile int *vmemptr = (volatile int *)memptr;
	int tmp = vmemptr[INT_EN];
	tmp &= ~(1 << (17 + device));
	vmemptr[INT_EN] = tmp;
	return 0;
}

/* ----- end of file "cats_sja1000.c" ----- */
