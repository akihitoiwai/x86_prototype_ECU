/**********************************************************************************
 * �t�@�C����      : cats_fpga.c
 * �V�X�e����      : x86 �v���g�^�C�v�d�b�t OPE-RA Ver2.0
 * �T�u�V�X�e����  : �J�[�l���h���C�o
 * �v���O������    : �x�[�X��
 * �o�[�W����      : Ver1.04
 * ���W���[���\��  : cats_fpga.c		PCI Access
 *                 : cats_can.c			CAN Control
 *                 : cats_sja1000.c		SJA1000 Control
 *                 : cats_flexray.c		FlexRay Control
 *                 : cats_adc.c			ADC Control
 *                 : cats_dac.c			DAC Control
 *                 : cats_pwm			PWM Control
 *                 : cats_gpio.c		GPIO Control
 *                 : cats_powacc.c		POW,ACC Control
 *                 : cats_testmodule.c	FPGA TEST Module
 * --------------------------------------------------------------------------------
 * �쐬��          : 
 * �쐬����        : 
 * �쐬���t        : 2009�N07��14�� �V�K�쐬
 * �X�V����        : 2010�N06��29�� 
 **********************************************************************************/
#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/pci.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/interrupt.h>
#include <linux/wait.h>
#include <asm/uaccess.h>
#include <linux/delay.h>

#include "cats_fpga.h"
#include "cats_files.h"
#include "cats_ioctl.h"
#include "cats_sja1000.h"


/********************************************************************/
/* 							�b��u����								*/
/********************************************************************/


/********************************************************************/
/* 							�萔��`								*/
/********************************************************************/
// #define NO_PCIDEVICE								/* Define is Not FPGA */

#define DRIVER_VERSION 104							/* �J�[�l���h���C�o �o�[�W���� */
#define DEVICEFILE_MAXMINOR 10						/* �}�C�i�[�ԍ�(�ő吔) */
#define DEVICEFILE_MAJOR 248						/* ���W���[�ԍ� */

#define DRIVER_NAME "catsdrv"						/* �f�o�C�X�t�@�C���� */
MODULE_DESCRIPTION("CATS/ECU PCI Driver");			/* ���C�Z���X�`�� */
MODULE_LICENSE("GPL");


/********************************************************************/
/* 							�ϐ���`								*/
/********************************************************************/
spinlock_t isr_spinlock;							/* */
int cats_pci_irq;									/* ���荞�ݔԍ� */
int *pci_adr_map = 0;								/* PCI �A�h���X�}�b�v */
int *pci_flexray_adr_map = 0;							/* FlexRay �A�h���X�}�b�v */
int flexray_intsts[2] = {0, 0};							/* FlexRay���荞�ݏ�� */

int powacc_intirq = 0;								/* �d���Ď��@�\���荞�ݗv�� */
int gpio_intirq = 0;								/* GPIO ���荞�ݗv�� */
int powacc_intsts = 0;								/* �d���Ď��@�\���荞�ݏ�� */
int gpio_intsts = 0;								/* GPIO ���荞�ݏ�� */

struct catssignaldev PowaccDev;						/* POW,ACC ���荞��(�V�O�i��)�񓯊��ʒm�p */
struct catssignaldev GpioDev;						/* GPIO ���荞��(�V�O�i��)�񓯊��ʒm�p */
struct catssignaldev FlexrayDev[2];				/* FlexRay ���荞��(�V�O�i��)�񓯊��ʒm�p */

static const int CATSFPGA_version = DRIVER_VERSION;	/* �J�[�l���h���C�o �o�[�W���� */

static int catsdrv_minor_num = DEVICEFILE_MAXMINOR;	/* �}�C�i�[�ԍ�(�ő吔) */
static int catsdrv_major_num = DEVICEFILE_MAJOR;	/* ���W���[�ԍ� */
static dev_t catsdrv_dev;							/* used in creation of char device file */
static struct cdev catsdrv_cdev;					/* used in creation of char device file */

// CATS/FPGA PCI resource start 0 to 5.
static unsigned long cats_fpga_pci_resource_start[5]; /* */

#ifdef USE_PAGE_1
int *pcican_adr_map;									/* */
#endif



/********************************************************************/
/* 			�f�o�C�X�t�@�C������Ɏg�p�����`						*/
/********************************************************************/
struct file_operations catsdrv_can_fops = {			/* CAN �@�\�V�X�e���R�[���n���h�� */
	.ioctl   = catsdrv_can_ioctl,
	.open    = catsdrv_can_open,
	.release = catsdrv_can_release,
};

struct file_operations catsdrv_flexray0_fops = {		/* FlexRay0 �@�\�V�X�e���R�[���n���h�� */
	.ioctl   = catsdrv_flexray0_ioctl,
	.open    = catsdrv_flexray0_open,
	.release = catsdrv_flexray0_release,
	.fasync  = catsdrv_flexray0_fasync,
};

struct file_operations catsdrv_flexray1_fops = {		/* FlexRay1 �@�\�V�X�e���R�[���n���h�� */
	.ioctl   = catsdrv_flexray1_ioctl,
	.open    = catsdrv_flexray1_open,
	.release = catsdrv_flexray1_release,
	.fasync  = catsdrv_flexray1_fasync,
};

struct file_operations catsdrv_adc_fops = {			/* ADC �@�\�V�X�e���R�[���n���h�� */
	.ioctl   = catsdrv_adc_ioctl,
	.open    = catsdrv_adc_open,
	.release = catsdrv_adc_release,
};

struct file_operations catsdrv_dac_fops = {			/* DAC �@�\�V�X�e���R�[���n���h�� */
	.ioctl   = catsdrv_dac_ioctl,
	.open    = catsdrv_dac_open,
	.release = catsdrv_dac_release,
};

struct file_operations catsdrv_pwm_fops = {			/* PWM �@�\�V�X�e���R�[���n���h�� */
	.ioctl   = catsdrv_pwm_ioctl,
	.open    = catsdrv_pwm_open,
	.release = catsdrv_pwm_release,
};

struct file_operations catsdrv_gpio_fops = {			/* GPIO �@�\�V�X�e���R�[���n���h�� */
	.ioctl   = catsdrv_gpio_ioctl,
	.open    = catsdrv_gpio_open,
	.release = catsdrv_gpio_release,
	.fasync  = catsdrv_gpio_fasync,
};

struct file_operations catsdrv_powacc_fops = {		/* POW,ACC �@�\�V�X�e���R�[���n���h�� */
	.ioctl   = catsdrv_powacc_ioctl,
	.open    = catsdrv_powacc_open,
	.release = catsdrv_powacc_release,
	.fasync  = catsdrv_powacc_fasync,
};

struct file_operations catsdrv_callback_fops = {		/* ���荞�݃R�[���o�b�N �@�\�V�X�e���R�[���n���h�� */
	.ioctl   = catsdrv_callback_ioctl,
	.open    = catsdrv_callback_open,
	.release = catsdrv_callback_release,
};

struct file_operations catsdrv_test_fops = {			/* ���� �@�\�V�X�e���R�[���n���h�� */
	.ioctl   = catsdrv_test_ioctl,
	.open    = catsdrv_test_open,
	.release = catsdrv_test_release,
};


/***********************************************************************************
* MODULE         : cats_isrwrap
* ABSTRACT       : ���W���[���E���荞�݃n���h���[(�r������p)
* FUNCTION       : 
* ARGUMENT       : 
* NOTE           : 
* RETURN         : 
* CREATE         : 2009/09/29  �V�K�쐬 
* UPDATE         : 
***********************************************************************************/
#ifdef KERNELMINOR18
irqreturn_t cats_isrwrap(int irq, void *dev_id, struct pt_regs *regs)
#else
irqreturn_t cats_isrwrap(int irq, void *dev_id)
#endif
{
	irqreturn_t		retirq;
	unsigned		long flags;
	int				tmp;
	volatile int	*vio = (volatile int *)pci_adr_map;

	spin_lock_irqsave( &isr_spinlock, flags );

//	printk("INT_STis1_and_CANstatusis0 Total =  %08X\n", INT_STis1_and_CANstatusis0);

	tmp = vio[INT_ST];
//	printk("vio[INT_ST] =  %08X\n", tmp);

	if(tmp != 0) {	//FPGA�̊��荞�ݎ킪���邩�H

#ifdef KERNELMINOR18
	retirq = catsfpga_isr( irq, dev_id, regs);
#else
	retirq = catsfpga_isr( irq, dev_id);
#endif
	}
	else
		retirq = IRQ_NONE;

	spin_unlock_irqrestore( &isr_spinlock, flags );
	return(retirq);
}


/***********************************************************************************
* MODULE         : catsfpga_isr
* ABSTRACT       : ���W���[���E���荞�݃n���h���[
* FUNCTION       : 
* ARGUMENT       : 
* NOTE           : 
* RETURN         : 
* CREATE         : 2009/08/19  �V�K�쐬 
* UPDATE         : 2010/06/10
***********************************************************************************/
#ifdef KERNELMINOR18
irqreturn_t catsfpga_isr(int irq, void *dev_id, struct pt_regs *regs)
#else
irqreturn_t catsfpga_isr(int irq, void *dev_id)
#endif
{
	volatile int	*vio = (volatile int *)pci_adr_map;
	int		tmp;
	int		status;
	int		i;

	tmp = vio[INT_ST];

//	printk(KERN_INFO "===== cats_module: INT Detect!! INT_ST=%x\n",tmp );

	if(tmp == 0) {
		return IRQ_NONE;
	}

/********************************************************************/
/*				�d���Ď��A�f�o�h�n���荞�ݗv���ۑ�					*/
/********************************************************************/
															/* ���L�̗v�����N���A����ꍇ�� */
															/* ���荞�݋֎~��ԂɂĎ��{���鎖 */

	powacc_intirq   |= tmp & MASK_POWACCINT_STS;		/* �d���Ď��@�\���荞�ݗv���X�V */
	gpio_intirq     |= tmp & MASK_GPIOINT_STS;		/* GPIO ���荞�ݗv���X�V */

															/* ���荞�ݏ�ԎQ�Ƃ`�o�h�p */
	powacc_intsts |= powacc_intirq;						/* �d���Ď��@�\���荞�ݏ�ԍX�V */
	gpio_intsts |= gpio_intirq;							/* GPIO ���荞�ݏ�ԍX�V */


	if(powacc_intirq){									/* ���荞�ݗL��H */
		if (PowaccDev.async_queue)						/* �v���Z�XID ���o�^�ς݂��H */
		{
//			printk(KERN_INFO "===== cats_module: ISR POWACC SIGNAL Send!!\n");
//			printk(KERN_INFO "===== cats_module: powacc_intirq = %x\n",powacc_intirq);
			kill_fasync(&PowaccDev.async_queue, SIGIO, POLL_IN);
		}
	}


	if(gpio_intirq){										/* ���荞�ݗL��H */
		if (GpioDev.async_queue)							/* �v���Z�XID ���o�^�ς݂��H */
		{
//			printk(KERN_INFO "===== cats_module: ISR GPIO SIGNAL Send!!\n");
//			printk(KERN_INFO "===== cats_module: gpio_intirq = %x\n",gpio_intirq);
			kill_fasync(&GpioDev.async_queue, SIGIO, POLL_IN);
		}
	}

/********************************************************************/
/*						�b�`�m���荞�ݏ���							*/
/********************************************************************/
//	wake_up_interruptible(&int_queue);

	// ���荞�݂̃f�o�C�X�ԍ��𓾂�B
	for(i = 0; i < 4; ++i) {
		if(tmp & (1 << (17 + i))) {
			status = sja1000_get_interrupt_status(pci_adr_map, i);
			if(status) {
#ifdef TASK_LET_USE
				intr_st[intr_st_put & INTR_ST_MASK].device = i;
				intr_st[intr_st_put & INTR_ST_MASK].status = status;
			 	intr_st_put++;
				if(i == 0) tasklet_schedule( &sja1000_intr_tasklet0 );
				else if(i == 1) tasklet_schedule( &sja1000_intr_tasklet1 );
				else if(i == 2) tasklet_schedule( &sja1000_intr_tasklet2 );
				else if(i == 3) tasklet_schedule( &sja1000_intr_tasklet3 );
#else
				sja1000_tasklet_core(pci_adr_map, i, status);
#endif
			}
//			else {
//				INT_STis1_and_CANstatusis0++;
//				printk("found INT_STis1_and_CANstatusis0 %08X\n", INT_STis1_and_CANstatusis0);
//			}
		}
	}

/********************************************************************/
/*						FlexRay���荞�ݏ���						*/
/********************************************************************/
	if ((vio[INT_EN] & FR0_INT1) && (tmp & FR0_INT1))			/* ���荞�ݗL��H */
	{
		if (FlexrayDev[0].async_queue)								/* �v���Z�XID ���o�^�ς݂��H */
		{
//			printk(KERN_INFO "===== cats_module: ISR FR0 SIGNAL Send!!\n");
			kill_fasync(&FlexrayDev[0].async_queue, SIGIO, POLL_IN);
			tmp &= ~FR0_INT1;
			vio[INT_EN] &= ~FR0_INT1;
			flexray_intsts[0] = 1;
		}
	}

	if((vio[INT_EN] & FR1_INT1) && (tmp & FR1_INT1))				/* ���荞�ݗL��H */
	{
		if (FlexrayDev[1].async_queue)								/* �v���Z�XID ���o�^�ς݂��H */
		{
//			printk(KERN_INFO "===== cats_module: ISR FR1 SIGNAL Send!!\n");
			kill_fasync(&FlexrayDev[1].async_queue, SIGIO, POLL_IN);
			tmp &= ~FR1_INT1;
			vio[INT_EN] &= ~FR1_INT1;
			flexray_intsts[1] = 1;
		}
	}

/********************************************************************/
/*					FPGA���荞�ݗv���N���A							*/
/********************************************************************/
	vio[INT_ST] = tmp;
	ndelay(500);

	return IRQ_HANDLED;
}


/********************************************************************/
/* 			ISR �Ɏg�p�����`										*/
/********************************************************************/
typedef struct catsfpga_obj {
	u32 usrs;
	u32 io_usrs;
	spinlock_t irqlock;
	struct semaphore lock;
} catsfpga_obj;

static catsfpga_obj catsfpga_device = {
	.usrs    = 0,
	.io_usrs = 0,
	.irqlock = SPIN_LOCK_UNLOCKED
};


/***********************************************************************************
* MODULE         : get_pci_resource_start
* ABSTRACT       : PCI �f�o�C�X�̃��\�[�X�A�h���X�𓾂�B
* FUNCTION       : 
* ARGUMENT       : [in]	no ���\�[�X�̔ԍ��i0 to 4�j
* NOTE           : 
* RETURN         : PCI �f�o�C�X�������[�|�C���^�[
* CREATE         : 2009/07/24  �V�K�쐬 
* UPDATE         : 
***********************************************************************************/
unsigned long get_pci_resource_start(int no)
{
	return cats_fpga_pci_resource_start[no];
}


/***********************************************************************************
* MODULE         : catsfpga_device_probe
* ABSTRACT       : �f�o�C�X�E�v���[�u
* FUNCTION       : 
* ARGUMENT       : 
* NOTE           : 
* RETURN         : ����I���Łu�O�v��Ԃ��B
* CREATE         : 2009/07/24  �V�K�쐬 
* UPDATE         : 
***********************************************************************************/
int catsfpga_device_probe(struct pci_dev *dev, const struct pci_device_id *id)
{
	int ret;

	printk(KERN_INFO "===== cats_module: PCI device_probe pci_enable_device\n");
	// enable PCI device
	ret = pci_enable_device(dev);
	if (ret < 0)
	{
		printk(KERN_INFO "===== cats_module: PCI unable to initialize PCI device\n");
		return ret;
	}

	printk(KERN_INFO "===== cats_module: PCI device_probe pci_request_regions\n");
	// request regions
	ret = pci_request_regions(dev, CATSFPGA_name);
	if (ret < 0)
	{
		printk(KERN_INFO "===== cats_module: PCI unable to reserve PCI resources\n");
		pci_disable_device(dev);
		return ret;
	}

	// CATS/FPGA brige address map registers
	cats_fpga_pci_resource_start[0] = pci_resource_start(dev, 0);
	cats_fpga_pci_resource_start[1] = pci_resource_start(dev, 1);
	cats_fpga_pci_resource_start[2] = pci_resource_start(dev, 2);
	cats_fpga_pci_resource_start[3] = pci_resource_start(dev, 3);
	cats_fpga_pci_resource_start[4] = pci_resource_start(dev, 4);
	pci_adr_map = ioremap_nocache(get_pci_resource_start(0), 0xff);
	pci_flexray_adr_map = ioremap_nocache(get_pci_resource_start(1), 0x1200);

#ifdef USE_PAGE_1
	pcican_adr_map = ioremap_nocache(get_pci_resource_start(1), 0x1200);
#endif

	cats_pci_irq = dev->irq;


//#ifdef USE_PAGE_1
//	printk(KERN_INFO "===== cats_module: PCI device_probe successful:\n> %x %x\n",pci_adr_map,pcican_adr_map);
//#else
	printk(KERN_INFO "===== cats_module: PCI device_probe successful.\n");
//#endif

	return 0;
}


/***********************************************************************************
* MODULE         : catsfpga_device_remove
* ABSTRACT       : CATS/FPGA / ���W���[���폜
* FUNCTION       : 
* ARGUMENT       : [in]	dev	  PCI �f�o�C�X�\����
* NOTE           : 
* RETURN         : 
* CREATE         : 2009/07/24  �V�K�쐬 
* UPDATE         : 2009/12/17
***********************************************************************************/
void catsfpga_device_remove(struct pci_dev *dev)
{

	iounmap(pci_adr_map);
	iounmap(pci_flexray_adr_map);

#ifdef USE_PAGE_1
	iounmap(pcican_adr_map);
#endif

	pci_adr_map = 0;
	pci_flexray_adr_map = 0;

	pci_release_regions(dev);
	pci_disable_device(dev);
	printk(KERN_INFO "===== cats_module: PCI device removed\n\n");
}


/********************************************************************/
/* 			PCI �f�o�C�X�v���[�u�Ɏg�p�����`						*/
/********************************************************************/
static struct pci_device_id catsfpga_pci_tbl[] = {
	{	CATSFPGA_VENDOR_ID,							// 0x1B3C (CATS)
		CATSFPGA_DEVICE_ID,							// 0x0001
		PCI_ANY_ID,
		PCI_ANY_ID,
	},{
		0,
	}
};

MODULE_DEVICE_TABLE(pci, catsfpga_pci_tbl);


/********************************************************************/
/* 			PCI �f�o�C�X�o�^�Ɏg�p�����`							*/
/********************************************************************/
struct pci_driver pci_driver_CATSFPGA = {
	.name     = CATSFPGA_name,
	.id_table = catsfpga_pci_tbl,
	.probe    = catsfpga_device_probe,
	.remove   = catsfpga_device_remove
};


/***********************************************************************************
* MODULE         : catsdrv_open
* ABSTRACT       : CATS/FPGA �f�o�C�X�E�t�@�C���E[open] ����
* FUNCTION       : 
* ARGUMENT       : 
* NOTE           : 
* RETURN         : ����I���Łu�O�v��Ԃ��B
* CREATE         : 2009/07/24  �V�K�쐬 
* UPDATE         : 2009/11/11
***********************************************************************************/
int catsdrv_open( struct inode *inode, struct file *file )
{
	inode->i_private = inode;
	file->private_data = file;

#if 0
	printk(KERN_INFO "===== cats_module: open, irq: %d\n", cats_pci_irq);
	printk(KERN_INFO "===== cats_module: Major %d Minor %d (pid %d)\n",
			imajor(inode),
			iminor(inode),
			current->pid );

	printk("===== cats_module: i_private =%p private_data =%p\n",
			inode->i_private,
			file->private_data );
#endif

	switch (iminor(inode)) {
		case DEVICE_FILE_CAN:						/* CAN */
			file->f_op = &catsdrv_can_fops;
			break;

		case DEVICE_FILE_FLEXRAY0:					/* FlexRay 0 */
			file->f_op = &catsdrv_flexray0_fops;
			break;

		case DEVICE_FILE_FLEXRAY1:					/* FlexRay 1 */
			file->f_op = &catsdrv_flexray1_fops;
			break;

		case DEVICE_FILE_ADC:						/* ADC */
			file->f_op = &catsdrv_adc_fops;
			break;

		case DEVICE_FILE_DAC:						/* DAC */
			file->f_op = &catsdrv_dac_fops;
			break;

		case DEVICE_FILE_PWM:						/* PWM */
			file->f_op = &catsdrv_pwm_fops;
			break;

		case DEVICE_FILE_GPIO:						/* GPIO */
			file->f_op = &catsdrv_gpio_fops;
			break;

		case DEVICE_FILE_POW_ACC:					/* POW,ACC */
			file->f_op = &catsdrv_powacc_fops;
			break;

		case DEVICE_FILE_CALLBACK:					/* CALLBACK */
			file->f_op = &catsdrv_callback_fops;
			break;

		case DEVICE_FILE_TEST:						/* TEST */
			file->f_op = &catsdrv_test_fops;
			break;

		default:
			return -ENXIO;
	}

	if (file->f_op && file->f_op->open)
		return file->f_op->open(inode, file);

	return 0;
}


/********************************************************************/
/* 			�f�o�C�X�t�@�C�� �V�X�e���R�[���֐���`					*/
/********************************************************************/
struct file_operations catsdrv_fops = {
	.open    = catsdrv_open,
};


/***********************************************************************************
* MODULE         : cats_module_init
* ABSTRACT       : �J�[�l���h���C�o�o�^
* FUNCTION       : 
* ARGUMENT       : 
* NOTE           : 
* RETURN         : 
* CREATE         : 2009/07/14  �V�K�쐬 
* UPDATE         : 2009/12/17  
***********************************************************************************/
static int cats_module_init(void)
{
	int ret;
	int alloc_ret;
	int major;
	int cdev_err;

	alloc_ret = 0;
	cdev_err = 0;

	printk(KERN_INFO "===== cats_module: load start\n");

/********************************************************************/
/* 		���\�[�X������												*/
/********************************************************************/
	memset(&FlexrayDev[0] ,0 ,sizeof( struct catssignaldev));
	memset(&FlexrayDev[1] ,0 ,sizeof( struct catssignaldev));
	memset(&PowaccDev   ,0 ,sizeof( struct catssignaldev));
	memset(&GpioDev     ,0 ,sizeof( struct catssignaldev));

/********************************************************************/
/* 		PCI �f�o�C�X�o�^											*/
/********************************************************************/
#ifndef NO_PCIDEVICE
	ret =  pci_register_driver(&pci_driver_CATSFPGA);
	if (ret < 0)
	{
		printk(KERN_INFO "===== cats_module: unable to register PCI device\n");
		return ret;
	}
	printk(KERN_INFO "===== cats_module: PCI Device Registered\n");

#endif
/********************************************************************/
/* 		�f�o�C�X�t�@�C���֘A�t��									*/
/********************************************************************/
	catsdrv_dev = MKDEV(catsdrv_major_num, 0);

	if( catsdrv_major_num == 0 ) {
		alloc_ret = alloc_chrdev_region(&catsdrv_dev, 0, catsdrv_minor_num, "catsdrv");
		if (alloc_ret)
			goto error;

	}else{
		alloc_ret = register_chrdev_region(catsdrv_dev, catsdrv_minor_num, "catsdrv");
		if (alloc_ret)
			goto error;
	}

		catsdrv_major_num = major = MAJOR(catsdrv_dev);

		// ���������蓖�ĂƏ�����
		catsdrv_cdev = *cdev_alloc();

		// cdev �\���̂̏������ƃV�X�e���R�[���̓o�^
		cdev_init(&catsdrv_cdev, &catsdrv_fops);
		catsdrv_cdev.owner = THIS_MODULE;

		// �J�[�l���ւ̓o�^
		cdev_err = cdev_add(&catsdrv_cdev, MKDEV(catsdrv_major_num, 0), catsdrv_minor_num);
		if (cdev_err) 
			goto error;

	printk(KERN_INFO "===== cats_module: cdev add end\n");
	printk(KERN_INFO "===== cats_module: driver(major %d) installed.\n", major);


/********************************************************************/
/* 		���荞�݃n���h���o�^										*/
/********************************************************************/
#ifndef NO_PCIDEVICE
	spin_lock_init( &isr_spinlock);						/* �X�s�����b�N�ϐ������� */

	ret = request_irq(	cats_pci_irq,					/* ���荞�ݔԍ� */
						cats_isrwrap,						/* ���荞�݃n���h�� */
						IRQF_DISABLED | IRQF_SHARED,	/* ���荞�ݏ������̓��[�J���v���Z�b�T�̊��荞�݂��֎~���� */
															/* IRQ �����L���� */
//						SA_INTERRUPT|SA_SHIRQ,
						CATSFPGA_name,					/* �f�o�C�X�� */
						&catsfpga_device);				/* */
	if(ret)
	{
		printk(KERN_INFO "===== cats_module: request_irq() failed (%d)\n", ret);
		return ret;
	}else{
		printk(KERN_INFO "===== cats_module: ISR Handler installed\n");
	}

#endif
/********************************************************************/
/* 		�J�[�l���h���C�o �o�[�W�����\��								*/
/********************************************************************/
	printk(KERN_INFO "===== cats_module: Kernel Driver Ver %d.%d.%d initialized.\n",
					 (CATSFPGA_version / 100) % 10,
					 (CATSFPGA_version / 10) % 10,
					 (CATSFPGA_version / 1) % 10);

	printk(KERN_INFO "===== cats_module: load end\n");

	return 0;

error:
	if (cdev_err == 0)
		cdev_del(&catsdrv_cdev);

	if (alloc_ret == 0)
		unregister_chrdev_region(catsdrv_dev, catsdrv_minor_num);

		printk(KERN_ALERT "===== cats_module: Device File init failed\n");
		printk(KERN_ALERT "===== cats_module: driver error removed.\n");

	return -1;
}


/***********************************************************************************
* MODULE         : cats_module_exit
* ABSTRACT       : �J�[�l���h���C�o�폜
* FUNCTION       : 
* ARGUMENT       : 
* NOTE           : 
* RETURN         : 
* CREATE         : 2009/07/14  �V�K�쐬 
* UPDATE         : 2009/08/19  
***********************************************************************************/
static void cats_module_exit(void)
{
	printk(KERN_INFO "===== cats_module: unload start\n");

/********************************************************************/
/* 		PCI �f�o�C�X�폜											*/
/********************************************************************/
#ifndef NO_PCIDEVICE
	pci_unregister_driver(&pci_driver_CATSFPGA);
	printk(KERN_INFO "===== cats_module: PCI Device Delete\n");

#endif
/********************************************************************/
/* 		�f�o�C�X�t�@�C���֘A�폜									*/
/********************************************************************/
	cdev_del(&catsdrv_cdev );						/* �J�[�l������h���C�o���폜 */
														/* ���W���[�ԍ����폜 */
	unregister_chrdev_region( catsdrv_dev, catsdrv_minor_num );
	printk(KERN_INFO "===== cats_module: Device File Delete\n");

/********************************************************************/
/* 		���荞�݃n���h������										*/
/********************************************************************/
#ifndef NO_PCIDEVICE
	free_irq(cats_pci_irq, &catsfpga_device);
	printk(KERN_INFO "===== cats_module: ISR Handler uninstalled\n");
#endif

	printk(KERN_INFO "===== cats_module: unload end\n");
}


/********************************************************************/
/* 						�J�[�l���h���C�o�o�^��`					*/
/********************************************************************/
module_init(cats_module_init);						/* �o�^�֐�( insmod ) */
module_exit(cats_module_exit);						/* �폜�֐�( rmmod ) */

