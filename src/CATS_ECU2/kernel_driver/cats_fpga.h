/**********************************************************************************
 * ファイル名      : cats_fpga.h
 * システム名      : x86 プロトタイプＥＣＵ OPE-RA Ver2.0
 * サブシステム名  : カーネルドライバ
 * プログラム名    : CATS FPGA 制御 (ヘッダー）
 * バージョン      : Ver1.00
 * --------------------------------------------------------------------------------
 * 作成者          : 
 * 作成部署        : 
 * 作成日付        : 2009年09月23日 新規作成
 * 更新履歴        : 2009年12月17日
 **********************************************************************************/
#ifndef CATS_FPGA_H
#define CATS_FPGA_H

#include <linux/cdev.h>

#define CATSFPGA_name		"catsfpga"

#define USE_PAGE_1

#define API_OK 0
#define API_NG 1
#define API_BUSY 2

/********************************************************************/
/* 							暫定置き場								*/
/********************************************************************/
#if 1
/* TZM FlexRay ドライバから移植 091130 */
/* structure for our flexcard device */
typedef struct catssignaldev
{
//	struct list_head list;			/* link anchor for list of devices */
//	struct irqinfo irinfo;			/* holds the irstatus info */
	int lock;						/* device in use? */

	unsigned int* irstatusreg;		/* interrupt status register address */
	unsigned int* irstatusregCC1;	/* interrupt status register address for CC1 */
	unsigned int* irstatusregCC2;	/* interrupt status register address for CC2 */
	unsigned int irstatus_dev;		/* interrupt status register temp for device only */
//	struct irqinfo irinfo_dev;		/* interrupt information temp for API only */

	struct pci_dev* pciDev;			/* PCI device */

	unsigned int* vBAR0;			/* virtual kernel address to access BAR0 */
	unsigned int* vBAR1;			/* virtual kernel address to access BAR1 */
//	struct flexres res;				/* FlexCard resources */

	/* For Kernel Version > 2.6.20.0 the 'struct pid* pid' has to be used! -> Makefile */
#ifdef USESTRUCTPID
	struct pid* pid; 				/* pid where SIGRTMIN+x should be sent */
#else
	int pid;						/* pid where SIGRTMIN+x should be sent */
#endif

	/* 2.4 kernel: There is no cdev structure.*/
#ifndef FCDV_LINUX_2_4
	struct cdev cdev;				/* Card associated with PID */
#endif
	int minor;						/* the associated minor number */
	struct fasync_struct* async_queue;	/* asynchronous reader */

	unsigned long ulFlags;			/* Flags to be saved for irq lock */
	spinlock_t irq_lock;			/* irq/ioctl spinlock */
} CATSSIGNALDEV;
#endif

extern struct catssignaldev PowaccDev;		/* POW,ACC 割り込み(シグナル)非同期通知用 */
extern struct catssignaldev GpioDev;		/* GPIO 割り込み(シグナル)非同期通知用 */
extern struct catssignaldev FlexrayDev[2];	/* FlexRay 割り込み(シグナル)非同期通知用 */



/********************************************************************/
/* 						デバイスファイル定義						*/
/********************************************************************/
#define DEVICE_FILE_CAN			0	/* CAN */
#define DEVICE_FILE_FLEXRAY0	1	/* FLEXRAY0 */
#define DEVICE_FILE_FLEXRAY1	2	/* FLEXRAY1 */
#define DEVICE_FILE_ADC			3	/* ADC */
#define DEVICE_FILE_DAC			4	/* DAC */
#define DEVICE_FILE_PWM			5	/* PWM */
#define DEVICE_FILE_GPIO			6	/* GPIO */
#define DEVICE_FILE_POW_ACC		7	/* POW,ACC */
#define DEVICE_FILE_CALLBACK	8	/* CALLBACK */
#define DEVICE_FILE_TEST			9	/* TEST */


/********************************************************************/
/* 			CATS ECU FPGA PCI Device Vendor & Device IDs			*/
/********************************************************************/
#define CATSFPGA_VENDOR_ID 0x1B3C
#define CATSFPGA_DEVICE_ID 0x0001


/********************************************************************/
/* 				FPGA 制御レジスタ オフセット定義					*/
/********************************************************************/
// CATS ECU FPGA Registers
// 32 bits 単位のアドレスなので、実際のアドレス（バイト単位）の 1/4
#define ADC_CTL    0  // R/W ADC Control
#define ADC_DATA0  1  // RO  ADC Channel 0 Data
#define ADC_DATA1  2  // RO  ADC Channel 1 Data
#define ADC_DATA2  3  // RO  ADC Channel 2 Data
#define ADC_DATA3  4  // RO  ADC Channel 3 Data
#define ADC_DATA4  5  // RO  ADC Channel 4 Data
#define ADC_DATA5  6  // RO  ADC Channel 5 Data
#define ADC_DATA6  7  // RO  ADC Channel 6 Data
#define ADC_DATA7  8  // RO  ADC Channel 7 Data
#define ADC_DATA8  9  // RO  ADC Channel 8 Data
#define ADC_DATA9  10 // RO  ADC Channel 9 Data
#define ADC_DATA10 11 // RO  ADC Channel 10 Data
#define ADC_DATA11 12 // RO  ADC Channel 11 Data
#define ADC_DATA12 13 // RO  ADC Channel 12 Data
#define ADC_DATA13 14 // RO  ADC Channel 13 Data
#define ADC_DATA14 15 // RO  ADC Channel 14 Data
#define ADC_DATA15 16 // RO  ADC Channel 15 Data
#define DAC_CTL    17 // R/W DAC Control & Data
#define MFR_WCTL   18 // R/W MFR Write Control
#define MFR_RCTL   19 // R/W MFR Read Control
#define CAN_WCTL   20 // R/W CAN controller SJA1000 Write Control
#define CAN_RCTL   21 // R/W CAN controller SJA1000 Read Control
#define CAN_TCTL   22 // R/W CAN transceiver TJA1041A pin Control
#define PWM_EN     24 // R/W PWM Enable
#define PWM_DIV    25 // R/W PWM0-7 Clock Divide setting
#define PWM_CYCL   26 // R/W PWM0-3 Cycle Count
#define PWM_CYCU   27 // R/W PWM4-7 Cycle Count
#define PWM_HWCL   28 // R/W PWM0-3 High Width Count
#define PWM_HWCU   29 // R/W PWM4-7 High Width Count
#define GPIO_PLS   31 // R/W
#define GPIO_OUT   32 // R/W GPIO Output data
#define GPIO_IN    33 // RO  GPIO Input data
#define INT_EN     34 // R/W Interrupt Enable
#define INT_LVL    35 // R/W Interrupt Level
#define INT_ST     36 // R/W Interrupt Status
#define FPGA_ST    37 // RO  FPGA Status
#define SCRATCH    38 // R/W Scratch
#define VERSION    39 // RO  FPGA Version



/********************************************************************/
/* 					FPGA 制御レジスタ ビット定義					*/
/********************************************************************/
#define ADC_READ_ENABLE  (1<<31) // ADC_CTL : 1 = enable
#define ADC_ONESHOT_MODE (1<<30) // ADC_CTL : 1 = one shot mode
#define ADC1_SHUTDOWN    (1<<29) // ADC_CTL : 1 = shutdown
#define ADC0_SHUTDOWN    (1<<28) // ADC_CTL : 1 = shutdown
#define ADC_BUSY         (1<<27) // ADC_CTL : 1 = data reading

#define ADC_DATA_VALID   (1<<31) // ADC_DATA# : 1 = valid

#define DAC_READ_ENABLE  (1<<31) // DAC_CTL : 1 = enable

#define V9OFF_INTERRUPT  (1<<22) // INT_EN : 1 = enable
                                 // INT_ST : 1 = int pending

#define ACC_INTERRUPT    (1<<21) // INT_EN : 1 = enable
                                 // INT_ST : 1 = int pending

#define FR1_INT2         (1<<27)
#define FR1_INT1         (1<<26)
#define FR1_INT0         (1<<25)
#define FR0_INT2         (1<<24)
#define FR0_INT1         (1<<23)
#define FR0_INT0         (1<<16)

#define MASK_POWACCINT_STS	0x600000;	/* V9OFF(POW),ACC 割り込み要因マスク */
#define MASK_GPIOINT_STS	0xFFFF;		/* GPIO 割り込み要因マスク */
#define MASK_FLEXRAY1INT_STS	0x0E000000;	/* FlexRay1 割り込み要因マスク */
#define MASK_FLEXRAY0INT_STS	0x01810000;	/* FlexRay0 割り込み要因マスク */


/********************************************************************/
/* 							外部宣言								*/
/********************************************************************/
extern int powacc_intirq;						/* 電圧監視機能割り込み要求 */
extern int gpio_intirq;							/* GPIO 割り込み要求 */
extern int powacc_intsts;						/* 電圧監視機能割り込み状態 */
extern int gpio_intsts;							/* GPIO 割り込み状態 */
extern int flexray_intsts[2];						/* FlexRay割り込み状態 */

extern int *pci_adr_map;
extern int *pci_flexray_adr_map;

#ifdef USE_PAGE_1
extern int *pcican_adr_map;
#endif

//-----------------------------------------------------------------//
/*!
	@brief	デバイス・プローブ@n
			called when a pci device if the given ids (vendor, device..) matched
	@param[in]	dev
	@param[in]	id
*/
//-----------------------------------------------------------------//
int catsfpga_device_probe(struct pci_dev *dev, const struct pci_device_id *id);


//-----------------------------------------------------------------//
/*!
	@brief	CATS/FPGA / モジュール削除
	@param[in]	dev
*/
//-----------------------------------------------------------------//
void catsfpga_device_remove(struct pci_dev *dev);


//-----------------------------------------------------------------//
/*!
	@brief	CATS/FPGA モジュール・初期化@n
			init_module
*/
//-----------------------------------------------------------------//
int init_module_CATSFPGA(void);


//-----------------------------------------------------------------//
/*!
	@brief	CATS/FPGA モジュール・クリーンアップ@n
			cleanup_module
*/
//-----------------------------------------------------------------//
void cleanup_module_CATSFPGA(void);


//-----------------------------------------------------------------//
/*!
	@brief	PCI デバイスのリソースアドレスを得る。
	@param[in]	no リソースの番号（0 to 4）
	@return	PCI デバイスメモリーポインター
*/
//-----------------------------------------------------------------//
unsigned long get_pci_resource_start(int no);


#endif // CATS_FPGA_H
