/**********************************************************************************
 * ファイル名      : cats_flexray.h
 * システム名      : x86 プロトタイプＥＣＵ OPE-RA Ver2.0
 * サブシステム名  : カーネルドライバ
 * プログラム名    : FlexRay 制御関数
 * バージョン      : Ver1.00
 * --------------------------------------------------------------------------------
 * 作成者          : 
 * 作成部署        : 
 * 作成日付        : 2009年09月23日 新規作成
 * 更新履歴        : 
 **********************************************************************************/
#ifndef CATS_FLEXRAY_H
#define CATS_FLEXRAY_H

typedef enum {
	CFR_COMMAND_NOT_ACCEPTED = 0,
	CFR_CONFIG,
	CFR_READY,
	CFR_WAKEUP,
	CFR_RUN,
	CFR_ALL_SLOTS,
	CFR_HALT,
	CFR_FREEZE,
	CFR_SEND_MTS,
	CFR_ALLOW_COLDSTART,
	CFR_RESET_STATUS_INDICATORS,
	CFR_MONITOR_MODE,
	CFR_CLEAR_RAMS
} cFr_CHICmdVectType;

typedef struct {
	unsigned long	addr;
	unsigned long	data;
} CFR_WRITE32_REG;

typedef struct {
	unsigned long	addr;
	unsigned long	data;
} CFR_READ32_REG;

typedef struct {
	cFr_CHICmdVectType	cmd;
	int					busy;
} CFR_SETCMD_REG;

typedef struct {
	int				coldstart_noise;
	int				halt_request;
	int				freeze;
	unsigned char	slot_mode;
	unsigned char	wakeup_status;
	unsigned char	error_mode;
	unsigned char	poc_status;
} CFR_GETPOCSTATE_REG;

typedef struct {
	int		wakeup_ch;
} CFR_SETWAKEUPCH_REG;

typedef struct {
	int		index;
	char	*data;
	int		length;
	int		int_enable;
} CFR_TRANSMIT_REG;

typedef struct {
	int		received;
	int		index;
	char	*data;
	int		length;
} CFR_RECEIVE_REG;

typedef struct {
	int				rxi;
	int				txi;
	int				txbufno;
	unsigned long	err_int_reg;
	unsigned long	sts_int_reg;
} CFR_GETINTSTS_REG;

typedef struct {
	int				rxi;
	int				txi;
	int				txbufno;
} CFR_RESETINT_REG;

typedef struct {
	unsigned char	nm_vector[12];
} CFR_GETNMVECTOR_REG;

#define FLEXRAY_CCSV_RCA	19
#define FLEXRAY_CCSV_WSV	16
#define FLEXRAY_CCSV_CSI	14
#define FLEXRAY_CCSV_CSAI	13
#define FLEXRAY_CCSV_CSNI	12
#define FLEXRAY_CCSV_SLM	8
#define FLEXRAY_CCSV_HRQ	7
#define FLEXRAY_CCSV_FSI	6
#define FLEXRAY_CCSV_POC	0

#define FLEXRAY_CCEV_PTAC	8
#define FLEXRAY_CCEV_ERRM	6
#define FLEXRAY_CCEV_CCFC	0

#define CC0_BASE_OFFSET		0x00000000
#define CC1_BASE_OFFSET		0x00000800
#define FLEXRAY_LSI_VER		0x04007900		// バージョン番号(<JEDEC IDコード><LSI版数><LSI識別番号><FlexRay IP版数>(版数はマスクする))
#define FLEXRAY_INIT_CCNT	0x00000001		// クロックコントロールレジスタの初期値(4MHzの20逓倍)
#define FLEXRAY_CCNT_PON	0x00000002		// PLL発信許可ビット
#define FLEXRAY_SUCC1_PBSY	0x00000080		// POCビジー状態
#define FLEXRAY_SUCC1_WUCS	0x00200000		// ウェイクアップパターン送信チャンネル選択
#define FLEXRAY_LCK_KEY1	0x000000CE		// ロックレジスタ(アンロックシーケンス1)
#define FLEXRAY_LCK_KEY2	0x00000031		// ロックレジスタ(アンロックシーケンス2)
#define FLEXRAY_IBCM_STXRH	0x00000004		// 送信要求フラグホストの設定
#define FLEXRAY_IBCM_LDSH	0x00000002		// データセクションホストのロード
#define FLEXRAY_IBCR_IBSYH	0x00008000		// インプットバッファホストビジー状態
#define FLEXRAY_IBCR_IBRH	0x0000007f		// インプットバッファホスト転送リクエスト
#define FLEXRAY_OBCM_RDSS	0x00000002		// データセクションシャドウの読み出し
#define FLEXRAY_OBCM_RHSS	0x00000001		// ヘッダセクションシャドウの読み出し
#define FLEXRAY_OBCR_REQ	0x00000200		// メッセージRAM転送要求
#define FLEXRAY_OBCR_OBSYS	0x00008000		// アウトプットバッファシャドウビジー
#define FLEXRAY_OBCR_VIEW	0x00000100		// シャドウバッファ・ホストバッファ入れ替え
#define FLEXRAY_SIR_RXI		0x00000010		// 受信
#define FLEXRAY_SIR_TXI		0x00000008		// 送信完了

#define FLEXRAY_SIES_RXIE		0x00000010		// 受信
#define FLEXRAY_SIES_TXIE		0x00000008		// 送信完了

/**********************************************************/
/* CC Register Map                                        */
/**********************************************************/
/* Customer Registers */
#define CUST0           0x0000
#define CUST1           0x0004
#define CUST2           0x0008
#define CUST3           0x000C

/* Clock Control */
#define CCNT			CUST1

/* Special Registers */
#define TEST1           0x0010
#define TEST2           0x0014
/*** reserved(1) */
#define LCK             0x001C

/* Interrupt Registers */
#define EIR             0x0020
#define SIR             0x0024
#define EILS            0x0028
#define SILS            0x002C
#define EIES            0x0030
#define EIER            0x0034
#define SIES            0x0038
#define SIER            0x003C
#define ILE             0x0040
#define T0C             0x0044
#define T1C             0x0048
#define STPW            0x004C
/*** reserved(12) */

/* CC Control Registers */
#define SUCC1           0x0080
#define SUCC2           0x0084
#define SUCC3           0x0088
#define NEMC            0x008C
#define PRTC1           0x0090
#define PRTC2           0x0094
#define MHDC            0x0098
/*** reserved(1) */
#define GTUC1           0x00A0
#define GTUC2           0x00A4
#define GTUC3           0x00A8
#define GTUC4           0x00AC
#define GTUC5           0x00B0
#define GTUC6           0x00B4
#define GTUC7           0x00B8
#define GTUC8           0x00BC
#define GTUC9           0x00C0
#define GTUC10          0x00C4
#define GTUC11          0x00C8
#define BGSC            0x00CC
/*** reserved(12) */

/* CC Status Registers */
#define CCSV            0x0100
#define CCEV            0x0104
/*** reserved(2) */
#define SCV             0x0110
#define MTCCV           0x0114
#define RCV             0x0118
#define OCV             0x011C
#define SFS             0x0120
#define SWNIT           0x0124
#define ACS             0x0128
/*** reserved(1) */
#define ESID1           0x0130
#define ESID2           0x0134
#define ESID3           0x0138
#define ESID4           0x013C
#define ESID5           0x0140
#define ESID6           0x0144
#define ESID7           0x0148
#define ESID8           0x014C
#define ESID9           0x0150
#define ESID10          0x0154
#define ESID11          0x0158
#define ESID12          0x015C
#define ESID13          0x0160
#define ESID14          0x0164
#define ESID15          0x0168
/*** reserved(1) */
#define OSID1           0x0170
#define OSID2           0x0174
#define OSID3           0x0178
#define OSID4           0x017C
#define OSID5           0x0180
#define OSID6           0x0184
#define OSID7           0x0188
#define OSID8           0x018C
#define OSID9           0x0190
#define OSID10          0x0194
#define OSID11          0x0198
#define OSID12          0x019C
#define OSID13          0x01A0
#define OSID14          0x01A4
#define OSID15          0x01A8
/*** reserved(1) */
#define NMV1            0x01B0
#define NMV2            0x01B4
#define NMV3            0x01B8
/*** reserved(81) */

/* Message Buffer Control Registers */
#define MRC             0x0300
#define FRF             0x0304
#define FRFM            0x0308
/*** reserved(1) */

/* Message Buffer Status Registers */
#define MHDS            0x0310
/*** reserved(3) */
#define TXRQ1           0x0320
#define TXRQ2           0x0324
/*** reserved(2) */
#define NDAT1           0x0330
#define NDAT2           0x0334
#define NDAT3           0x0338
#define NDAT4           0x033C
#define MBSC1           0x0340
#define MBSC2           0x0344
/*** reserved(46) */

/* Input Buffer */
#define WRDS1           0x0400
#define WRDS2           0x0404
#define WRDS3           0x0408
#define WRDS4           0x040C
#define WRDS5           0x0410
#define WRDS6           0x0414
#define WRDS7           0x0418
#define WRDS8           0x041C
#define WRDS9           0x0420
#define WRDS10          0x0424
#define WRDS11          0x0428
#define WRDS12          0x042C
#define WRDS13          0x0430
#define WRDS14          0x0434
#define WRDS15          0x0438
#define WRDS16          0x043C
#define WRDS17          0x0440
#define WRDS18          0x0444
#define WRDS19          0x0448
#define WRDS20          0x044C
#define WRDS21          0x0450
#define WRDS22          0x0454
#define WRDS23          0x0458
#define WRDS24          0x045C
#define WRDS25          0x0460
#define WRDS26          0x0464
#define WRDS27          0x0468
#define WRDS28          0x046C
#define WRDS29          0x0470
#define WRDS30          0x0474
#define WRDS31          0x0478
#define WRDS32          0x047C
#define WRDS33          0x0480
#define WRDS34          0x0484
#define WRDS35          0x0488
#define WRDS36          0x048C
#define WRDS37          0x0490
#define WRDS38          0x0494
#define WRDS39          0x0498
#define WRDS40          0x049C
#define WRDS41          0x04A0
#define WRDS42          0x04A4
#define WRDS43          0x04A8
#define WRDS44          0x04AC
#define WRDS45          0x04B0
#define WRDS46          0x04B4
#define WRDS47          0x04B8
#define WRDS48          0x04BC
#define WRDS49          0x04C0
#define WRDS50          0x04C4
#define WRDS51          0x04C8
#define WRDS52          0x04CC
#define WRDS53          0x04D0
#define WRDS54          0x04D4
#define WRDS55          0x04D8
#define WRDS56          0x04DC
#define WRDS57          0x04E0
#define WRDS58          0x04E4
#define WRDS59          0x04E8
#define WRDS60          0x04EC
#define WRDS61          0x04F0
#define WRDS62          0x04F4
#define WRDS63          0x04F8
#define WRDS64          0x04FC
#define WRHS1           0x0500
#define WRHS2           0x0504
#define WRHS3           0x0508
/*** reserved(1) */
#define IBCM            0x0510
#define IBCR_E          0x0514
/*** reserved(58) */

/* Output Buffer */
#define RDDS1           0x0600
#define RDDS2           0x0604
#define RDDS3           0x0608
#define RDDS4           0x060C
#define RDDS5           0x0610
#define RDDS6           0x0614
#define RDDS7           0x0618
#define RDDS8           0x061C
#define RDDS9           0x0620
#define RDDS10          0x0624
#define RDDS11          0x0628
#define RDDS12          0x062C
#define RDDS13          0x0630
#define RDDS14          0x0634
#define RDDS15          0x0638
#define RDDS16          0x063C
#define RDDS17          0x0640
#define RDDS18          0x0644
#define RDDS19          0x0648
#define RDDS20          0x064C
#define RDDS21          0x0650
#define RDDS22          0x0654
#define RDDS23          0x0658
#define RDDS24          0x065C
#define RDDS25          0x0660
#define RDDS26          0x0664
#define RDDS27          0x0668
#define RDDS28          0x066C
#define RDDS29          0x0670
#define RDDS30          0x0674
#define RDDS31          0x0678
#define RDDS32          0x067C
#define RDDS33          0x0680
#define RDDS34          0x0684
#define RDDS35          0x0688
#define RDDS36          0x068C
#define RDDS37          0x0690
#define RDDS38          0x0694
#define RDDS39          0x0698
#define RDDS40          0x069C
#define RDDS41          0x06A0
#define RDDS42          0x06A4
#define RDDS43          0x06A8
#define RDDS44          0x06AC
#define RDDS45          0x06B0
#define RDDS46          0x06B4
#define RDDS47          0x06B8
#define RDDS48          0x06BC
#define RDDS49          0x06C0
#define RDDS50          0x06C4
#define RDDS51          0x06C8
#define RDDS52          0x06CC
#define RDDS53          0x06D0
#define RDDS54          0x06D4
#define RDDS55          0x06D8
#define RDDS56          0x06DC
#define RDDS57          0x06E0
#define RDDS58          0x06E4
#define RDDS59          0x06E8
#define RDDS60          0x06EC
#define RDDS61          0x06F0
#define RDDS62          0x06F4
#define RDDS63          0x06F8
#define RDDS64          0x06FC
#define RDHS1           0x0700
#define RDHS2           0x0704
#define RDHS3           0x0708
#define MBS             0x070C
#define OBCM            0x0710
#define OBCR            0x0714
/*** reserved(58) */

#endif	// CATS_FLEXRAY_H
