/**********************************************************************************
 * �t�@�C����      : cats_flexray.c
 * �V�X�e����      : x86 �v���g�^�C�v�d�b�t OPE-RA Ver2.0
 * �T�u�V�X�e����  : �J�[�l���h���C�o
 * �v���O������    : �e�������q��������
 * �o�[�W����      : Ver1.01
 * ���W���[���\��  : cats_fpga.c		PCI Access
 *                 : cats_flexray.c		FlexRay Control
 * --------------------------------------------------------------------------------
 * �쐬��          : 
 * �쐬����        : 
 * �쐬���t        : 2009�N08��24�� �V�K�쐬
 * �X�V����        : 2010�N06��29��
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
 * �ϐ���`
 **********************************************************************************/
int flexray_drv_open[2] = {0, 0};
unsigned long cc_base_offset[2];
int txrq[2][4];

/***********************************************************************************
* MODULE         : catsdrv_flexray0_fasync
* ABSTRACT       : CATS/FPGA �f�o�C�X�E�t�@�C���E[fasync] ����
* FUNCTION       : 
* ARGUMENT       : 
* NOTE           : 
* RETURN         : ����I���Łu�O�v��Ԃ��B
* CREATE         : 2009/12/ 2  �V�K�쐬 
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
* ABSTRACT       : CATS/FPGA �f�o�C�X�E�t�@�C���E[fasync] ����
* FUNCTION       : 
* ARGUMENT       : 
* NOTE           : 
* RETURN         : ����I���Łu�O�v��Ԃ��B
* CREATE         : 2009/12/ 2  �V�K�쐬 
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
* ABSTRACT       : CATS/FPGA �f�o�C�X�E�t�@�C���E[fasync] ����
* FUNCTION       : 
* ARGUMENT       : 
* NOTE           : 
* RETURN         : ����I���Łu�O�v��Ԃ��B
* CREATE         : 2009/12/ 2  �V�K�쐬 
* UPDATE         : 
***********************************************************************************/
int catsdrv_flexray_fasync( int cc_index, int fd, struct file *file, int mode )
{
	return fasync_helper(fd, file, mode, &FlexrayDev[cc_index].async_queue);
}
/***********************************************************************************
* MODULE         : catsdrv_flexray0_open
* ABSTRACT       : CATS/FPGA �f�o�C�X�E�t�@�C���E[open] ����
* FUNCTION       : 
* ARGUMENT       : 
* NOTE           : 
* RETURN         : ����I���Łu�O�v��Ԃ��B
* CREATE         : 2009/12/ 2  �V�K�쐬 
* UPDATE         : 2010/06/29
***********************************************************************************/
int catsdrv_flexray0_open( struct inode *inode, struct file *file )
{
	volatile unsigned long *vio = (volatile unsigned long*)pci_adr_map;
	unsigned	long flags;		/* �X�s�����b�N�p(���荞�ݏ�ԑޔ�p) */

#ifdef FLEXRAY_DRV_DEBUG
	printk(KERN_INFO "===== catsdrv_flexray0_open\n");
#endif
	// ���d�I�[�v���`�F�b�N
	if (flexray_drv_open[0] != 0)
	{
		// �I�[�v�����s
		return -1;
	}

	spin_lock_irqsave( &isr_spinlock, flags );		/* ���荞�݋֎~�J�n */

	// FlexRay0�̊��荞�݂��֎~����
	vio[INT_EN] &= ~(FR0_INT0|FR0_INT1|FR0_INT2);

	// FlexRay0�̊��荞�݃��x���𔽓]������
	vio[INT_LVL] |= FR0_INT0|FR0_INT1|FR0_INT2;

	spin_unlock_irqrestore( &isr_spinlock, flags );		/* ���荞�݋֎~�I�� */

	// FlexRay0�Ƀ��Z�b�g��������
	vio[MFR_WCTL] = 0x01;
	vio[MFR_WCTL] = 0x00;

	// ���W�X�^�̐擪�A�h���X���Z�b�g
	cc_base_offset[0] = CC0_BASE_OFFSET + (unsigned long)pci_flexray_adr_map;

	return catsdrv_flexray_open(0);
}
/***********************************************************************************
* MODULE         : catsdrv_flexray1_open
* ABSTRACT       : CATS/FPGA �f�o�C�X�E�t�@�C���E[open] ����
* FUNCTION       : 
* ARGUMENT       : 
* NOTE           : 
* RETURN         : ����I���Łu�O�v��Ԃ��B
* CREATE         : 2009/12/ 2  �V�K�쐬 
* UPDATE         : 2010/06/29
***********************************************************************************/
int catsdrv_flexray1_open( struct inode *inode, struct file *file )
{
	volatile unsigned long *vio = (volatile unsigned long*)pci_adr_map;
	unsigned	long flags;		/* �X�s�����b�N�p(���荞�ݏ�ԑޔ�p) */

#ifdef FLEXRAY_DRV_DEBUG
	printk(KERN_INFO "===== catsdrv_flexray1_open\n");
#endif
	// ���d�I�[�v���`�F�b�N
	if (flexray_drv_open[1] != 0)
	{
		// �I�[�v�����s
		return -1;
	}

	spin_lock_irqsave( &isr_spinlock, flags );		/* ���荞�݋֎~�J�n */

	// FlexRay1�̊��荞�݂��֎~����
	vio[INT_EN] &= ~(FR1_INT0|FR1_INT1|FR1_INT2);

	// FlexRay1�̊��荞�݃��x���𔽓]������
	vio[INT_LVL] |= FR1_INT0|FR1_INT1|FR1_INT2;

	spin_unlock_irqrestore( &isr_spinlock, flags );		/* ���荞�݋֎~�I�� */

	// FlexRay1�Ƀ��Z�b�g��������
	vio[MFR_WCTL] = 0x02;
	vio[MFR_WCTL] = 0x00;

	// ���W�X�^�̐擪�A�h���X���Z�b�g
	cc_base_offset[1] = CC1_BASE_OFFSET + (unsigned long)pci_flexray_adr_map;

	return catsdrv_flexray_open(1);
}
/***********************************************************************************
* MODULE         : catsdrv_flexray_open
* ABSTRACT       : CATS/FPGA �f�o�C�X�E�t�@�C���E[open] ����
* FUNCTION       : 
* ARGUMENT       : 
* NOTE           : 
* RETURN         : ����I���Łu�O�v��Ԃ��B
* CREATE         : 2009/12/ 2  �V�K�쐬 
* UPDATE         : 
***********************************************************************************/
int catsdrv_flexray_open( int cc_index )
{
	unsigned long ver;

#ifdef FLEXRAY_DRV_DEBUG
	printk(KERN_INFO "===== catsdrv_flexray_open %d %d\n", flexray_drv_open[0], flexray_drv_open[1]);
#endif

	// FlexRay�R���g���[���̃o�[�W���������Q�Ƃ��R���g���[�������ڂ���Ă��邩�ǂ����`�F�b�N
	ver = catsdrv_flexray_read32(cc_index, CUST0);

	// �Ő��̓}�X�N���Ĕ�r����
	if ((ver & 0xff00ff00) != (FLEXRAY_LSI_VER & 0xff00ff00))
	{
		// �I�[�v�����s
		return -1;
	}

	// �N���b�N�R���g���[�����W�X�^
	catsdrv_flexray_write32(cc_index, CCNT, FLEXRAY_INIT_CCNT);

	// PLL���b�N�A�b�v����600us�E�F�C�g
	udelay(600);

	// PLL���M����
	catsdrv_flexray_write32(cc_index, CCNT, FLEXRAY_INIT_CCNT | FLEXRAY_CCNT_PON);

	// �I�[�v���ς݃t���O�Z�b�g
	flexray_drv_open[cc_index] = 1;

	// �I�[�v������
	return 0;
}
/***********************************************************************************
* MODULE         : catsdrv_flexray0_release
* ABSTRACT       : CATS/FPGA �f�o�C�X�E�t�@�C���E[release]����
* FUNCTION       : 
* ARGUMENT       : 
* NOTE           : 
* RETURN         : ����I���Łu�O�v��Ԃ��B
* CREATE         : 2009/12/ 2  �V�K�쐬 
* UPDATE         : 2010/06/29
***********************************************************************************/
int catsdrv_flexray0_release( struct inode *inode, struct file *filp )
{
	volatile unsigned long *vio = (volatile unsigned long*)pci_adr_map;
	unsigned	long flags;		/* �X�s�����b�N�p(���荞�ݏ�ԑޔ�p) */

#ifdef FLEXRAY_DRV_DEBUG
	printk(KERN_INFO "===== catsdrv_flexray0_release\n");
#endif

	spin_lock_irqsave( &isr_spinlock, flags );		/* ���荞�݋֎~�J�n */

	// ���荞�݋֎~
	vio[INT_EN] &= ~(FR0_INT0|FR0_INT1|FR0_INT2);

	spin_unlock_irqrestore( &isr_spinlock, flags );		/* ���荞�݋֎~�I�� */

	catsdrv_flexray_release(0, filp);

	return 0;
}
/***********************************************************************************
* MODULE         : catsdrv_flexray1_release
* ABSTRACT       : CATS/FPGA �f�o�C�X�E�t�@�C���E[release]����
* FUNCTION       : 
* ARGUMENT       : 
* NOTE           : 
* RETURN         : ����I���Łu�O�v��Ԃ��B
* CREATE         : 2009/12/ 2  �V�K�쐬 
* UPDATE         : 2010/06/29
***********************************************************************************/
int catsdrv_flexray1_release( struct inode *inode, struct file *filp )
{
	volatile unsigned long *vio = (volatile unsigned long*)pci_adr_map;
	unsigned	long flags;		/* �X�s�����b�N�p(���荞�ݏ�ԑޔ�p) */

#ifdef FLEXRAY_DRV_DEBUG
	printk(KERN_INFO "===== catsdrv_flexray1_release\n");
#endif

	spin_lock_irqsave( &isr_spinlock, flags );		/* ���荞�݋֎~�J�n */

	// ���荞�݋֎~
	vio[INT_EN] &= ~(FR1_INT0|FR1_INT1|FR1_INT2);

	spin_unlock_irqrestore( &isr_spinlock, flags );		/* ���荞�݋֎~�I�� */

	catsdrv_flexray_release(1, filp);

	return 0;
}
/***********************************************************************************
* MODULE         : catsdrv_flexray_release
* ABSTRACT       : CATS/FPGA �f�o�C�X�E�t�@�C���E[release]����
* FUNCTION       : 
* ARGUMENT       : 
* NOTE           : 
* RETURN         : ����I���Łu�O�v��Ԃ��B
* CREATE         : 2009/12/ 2  �V�K�쐬 
* UPDATE         : 
***********************************************************************************/
int catsdrv_flexray_release( int cc_index, struct file *filp )
{
	filp->private_data = filp;

	catsdrv_flexray_fasync(cc_index, -1, filp, 0);

	// �I�[�v���ς݃t���O���Z�b�g
	flexray_drv_open[cc_index] = 0;

	return 0;
}
/***********************************************************************************
* MODULE         : catsdrv_flexray0_ioctl
* ABSTRACT       : CATS/FPGA ���W���[���E�t�@�C���E[ioctl]����
* FUNCTION       : 
* ARGUMENT       : 
* NOTE           : 
*                : @param[in]	inode
*                : @param[in]	filep
*                : @param[in]	cmd
*                : @param[in]	arg
*                : @return ����I���Łu�O�v��Ԃ��B
* RETURN         : ����I���Łu�O�v��Ԃ��B
* CREATE         : 2009/09/04  �V�K�쐬 
* UPDATE         : 
***********************************************************************************/
int catsdrv_flexray0_ioctl( struct inode *inode, struct file *filep, unsigned int cmd, unsigned long arg )
{
	return catsdrv_flexray_ioctl(0, cmd, arg);
}
/***********************************************************************************
* MODULE         : catsdrv_flexray1_ioctl
* ABSTRACT       : CATS/FPGA ���W���[���E�t�@�C���E[ioctl]����
* FUNCTION       : 
* ARGUMENT       : 
* NOTE           : 
*                : @param[in]	inode
*                : @param[in]	filep
*                : @param[in]	cmd
*                : @param[in]	arg
*                : @return ����I���Łu�O�v��Ԃ��B
* RETURN         : ����I���Łu�O�v��Ԃ��B
* CREATE         : 2009/09/04  �V�K�쐬 
* UPDATE         : 
***********************************************************************************/
int catsdrv_flexray1_ioctl( struct inode *inode, struct file *filep, unsigned int cmd, unsigned long arg )
{
	return catsdrv_flexray_ioctl(1, cmd, arg);
}
/***********************************************************************************
* MODULE         : catsdrv_flexray_ioctl
* ABSTRACT       : CATS/FPGA ���W���[���E�t�@�C���E[ioctl]����
* FUNCTION       : 
* ARGUMENT       : 
* NOTE           : 
*                : @param[in]	inode
*                : @param[in]	filep
*                : @param[in]	cmd
*                : @param[in]	arg
*                : @return ����I���Łu�O�v��Ԃ��B
* RETURN         : ����I���Łu�O�v��Ԃ��B
* CREATE         : 2009/09/04  �V�K�쐬 
* UPDATE         : 2010/06/29
***********************************************************************************/
int catsdrv_flexray_ioctl( int cc_index, unsigned int cmd, unsigned long arg )
{
	int size;
	int ret_val;
	unsigned	long flags;		/* �X�s�����b�N�p(���荞�ݏ�ԑޔ�p) */

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

				// �A�h���X�A�f�[�^���R�s�[(reg��arg)
				__copy_from_user(&reg, (const CFR_WRITE32_REG*)arg, sizeof(CFR_WRITE32_REG));

				// �f�[�^����������
				catsdrv_flexray_write32(cc_index, reg.addr, reg.data);
			}
			break;

		//----------------------------------------------------------------------
		// IOCTL_CATS_FLEXRAY_READ32
		//----------------------------------------------------------------------
		case IOCTL_CATS_FLEXRAY_READ32:
			{
				CFR_READ32_REG reg;

				// �A�h���X���R�s�[(reg��arg)
				__copy_from_user(&reg, (const CFR_READ32_REG*)arg, sizeof(CFR_READ32_REG));

				// �f�[�^��ǂݏo��
				reg.data = catsdrv_flexray_read32(cc_index, reg.addr);

				// �ǂݏo�����f�[�^���R�s�[(arg��reg)
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

				// �R�}���h���R�s�[(reg��arg)
				__copy_from_user(&reg, (const CFR_SETCMD_REG*)arg, sizeof(CFR_SETCMD_REG));

				// POC�r�W�[�X�e�[�^�X�����[�h
				succ1 = catsdrv_flexray_read32(cc_index, SUCC1);

				// POC�r�W�[�`�F�b�N
				if ((succ1 & FLEXRAY_SUCC1_PBSY) != 0)
				{
					reg.busy = 1;
				}
				else
				{
					reg.busy = 0;

					// �R�}���h����������
					catsdrv_flexray_setcmd(cc_index, reg.cmd);
				}

				// �r�W�[�t���O���R�s�[(arg��reg)
				ret_val = __copy_to_user((CFR_SETCMD_REG*)arg, &reg, sizeof(CFR_SETCMD_REG));
			}
			break;

		//----------------------------------------------------------------------
		// IOCTL_CATS_FLEXRAY_GETPOCSTATE
		//----------------------------------------------------------------------
		case IOCTL_CATS_FLEXRAY_GETPOCSTATE:
			{
				CFR_GETPOCSTATE_REG reg;

				// ��Ԃ��擾����
				catsdrv_flexray_getpocstate(cc_index, &reg);

				// �ǂݏo�����f�[�^���R�s�[(arg��reg)
				ret_val = __copy_to_user((CFR_GETPOCSTATE_REG*)arg, &reg, sizeof(CFR_GETPOCSTATE_REG));
			}
			break;

		//----------------------------------------------------------------------
		// IOCTL_CATS_FLEXRAY_ENABLEINT
		//----------------------------------------------------------------------
		case IOCTL_CATS_FLEXRAY_ENABLEINT:
			{
				volatile unsigned long *vio = (volatile unsigned long*)pci_adr_map;

				// ���荞�݃t���O���N���A����
				flexray_intsts[cc_index] = 0;

				// FLEXRAY�R���g���[���̊��荞�݂�������
				catsdrv_flexray_write32(cc_index, SIES, FLEXRAY_SIES_RXIE);

				spin_lock_irqsave( &isr_spinlock, flags );		/* ���荞�݋֎~�J�n */

				if (cc_index == 0)
				{
					// FPGA�̊��荞�݂��N���A����
					vio[INT_ST] |= FR0_INT1;

					// FPGA�̊��荞�݂�������
					vio[INT_EN] |= FR0_INT1;
				}
				else if (cc_index == 1)
				{
					// FPGA�̊��荞�݂��N���A����
					vio[INT_ST] |= FR1_INT1;

					// FPGA�̊��荞�݂�������
					vio[INT_EN] |= FR1_INT1;
				}

				spin_unlock_irqrestore( &isr_spinlock, flags );		/* ���荞�݋֎~�I�� */

			}
			break;

		//----------------------------------------------------------------------
		// IOCTL_CATS_FLEXRAY_DISABLEINT
		//----------------------------------------------------------------------
		case IOCTL_CATS_FLEXRAY_DISABLEINT:
			{
				volatile unsigned long *vio = (volatile unsigned long*)pci_adr_map;

				spin_lock_irqsave( &isr_spinlock, flags );		/* ���荞�݋֎~�J�n */

				// FPGA�̊��荞�݂��֎~����
				if (cc_index == 0)
				{
					vio[INT_EN] &= ~FR0_INT1;
				}
				else if (cc_index == 1)
				{
					vio[INT_EN] &= ~FR1_INT1;
				}

				spin_unlock_irqrestore( &isr_spinlock, flags );		/* ���荞�݋֎~�I�� */
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

				// ���荞�݃X�e�[�^�X���R�s�[
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

				// �R�}���h���R�s�[(reg��arg)
				__copy_from_user(&reg, (const CFR_RESETINT_REG*)arg, sizeof(CFR_RESETINT_REG));

				// ���荞�݃t���O���N���A����
				flexray_intsts[cc_index] = 0;

				if (reg.rxi)
				{
					// ��M���荞�݂��N���A����
					catsdrv_flexray_write32(cc_index, SIR, FLEXRAY_SIR_RXI);
				}

				if (reg.txi)
				{
					// TXRQn���W�X�^��n�����߂�
					txrq_index = reg.txbufno / 32;

					// TXRQn���W�X�^�̉��r�b�g�ڂ������߂�
					bit_no = reg.txbufno % 32;

					// ���M�v���t���O���Z�b�g
					txrq[cc_index][txrq_index] &= ~(1 << bit_no);

					// ���M�v�����S�ă��Z�b�g���ꂽ�瑗�M�������荞�݂��N���A����
					for (txrq_index = 0; txrq_index < 4; txrq_index++)
					{
						if (txrq[cc_index][txrq_index] != 0)
						{
							break;
						}
					}

					if (txrq_index == 4)
					{
						// ���M�������荞�݂��N���A����
						catsdrv_flexray_write32(cc_index, SIR, FLEXRAY_SIR_TXI);

						// ���M�������荞�݂��֎~����
						catsdrv_flexray_write32(cc_index, SIER, FLEXRAY_SIES_TXIE);
					}
				}

				spin_lock_irqsave( &isr_spinlock, flags );		/* ���荞�݋֎~�J�n */

				if (cc_index == 0)
				{
					// FPGA�̊��荞�݂��N���A����
					vio[INT_ST] |= FR0_INT1;

					// FPGA�̊��荞�݂�������
					vio[INT_EN] |= FR0_INT1;
				}
				else if (cc_index == 1)
				{
					// FPGA�̊��荞�݂��N���A����
					vio[INT_ST] |= FR1_INT1;

					// FPGA�̊��荞�݂�������
					vio[INT_EN] |= FR1_INT1;
				}

				spin_unlock_irqrestore( &isr_spinlock, flags );		/* ���荞�݋֎~�I�� */
			}
			break;

		//----------------------------------------------------------------------
		// IOCTL_CATS_FLEXRAY_SETWAKEUPCH
		//----------------------------------------------------------------------
		case IOCTL_CATS_FLEXRAY_SETWAKEUPCH:
			{
				CFR_SETWAKEUPCH_REG reg;
				unsigned long succ1;

				// �R�}���h���R�s�[(reg��arg)
				__copy_from_user(&reg, (const CFR_SETWAKEUPCH_REG*)arg, sizeof(CFR_SETWAKEUPCH_REG));

				// SUCC1���W�X�^��ǂݏo��
				succ1 = catsdrv_flexray_read32(cc_index, SUCC1);

				if (reg.wakeup_ch == 1)
				{
					// �`�����l��B����E�F�C�N�A�b�v�p�^�[���𑗐M����悤�ɐݒ�
					succ1 |= FLEXRAY_SUCC1_WUCS;
				}
				else
				{
					// �`�����l��A����E�F�C�N�A�b�v�p�^�[���𑗐M����悤�ɐݒ�
					succ1 &= ~FLEXRAY_SUCC1_WUCS;
				}

				// SUCC1���W�X�^�ɏ�������
				catsdrv_flexray_write32(cc_index, SUCC1, succ1);
			}
			break;

		//----------------------------------------------------------------------
		// IOCTL_CATS_FLEXRAY_GETNMVECTOR
		//----------------------------------------------------------------------
		case IOCTL_CATS_FLEXRAY_GETNMVECTOR:
			{
				CFR_GETNMVECTOR_REG reg;

				// NMV1�̓��e���R�s�[
				reg.nm_vector[0]  = catsdrv_flexray_read32(cc_index, NMV1);
				reg.nm_vector[1]  = catsdrv_flexray_read32(cc_index, NMV1) >> 8;
				reg.nm_vector[2]  = catsdrv_flexray_read32(cc_index, NMV1) >> 16;
				reg.nm_vector[3]  = catsdrv_flexray_read32(cc_index, NMV1) >> 24;
				// NMV2�̓��e���R�s�[
				reg.nm_vector[4]  = catsdrv_flexray_read32(cc_index, NMV2);
				reg.nm_vector[5]  = catsdrv_flexray_read32(cc_index, NMV2) >> 8;
				reg.nm_vector[6]  = catsdrv_flexray_read32(cc_index, NMV2) >> 16;
				reg.nm_vector[7]  = catsdrv_flexray_read32(cc_index, NMV2) >> 24;
				// NMV3�̓��e���R�s�[
				reg.nm_vector[8]  = catsdrv_flexray_read32(cc_index, NMV3);
				reg.nm_vector[9]  = catsdrv_flexray_read32(cc_index, NMV3) >> 8;
				reg.nm_vector[10] = catsdrv_flexray_read32(cc_index, NMV3) >> 16;
				reg.nm_vector[11] = catsdrv_flexray_read32(cc_index, NMV3) >> 24;

				// �ǂݏo�����f�[�^���R�s�[(arg��reg)
				ret_val = __copy_to_user((CFR_GETNMVECTOR_REG*)arg, &reg, sizeof(CFR_GETNMVECTOR_REG));
			}
			break;

		//----------------------------------------------------------------------
		// IOCTL_CATS_FLEXRAY_TRANSMIT
		//----------------------------------------------------------------------
		case IOCTL_CATS_FLEXRAY_TRANSMIT:
			{
				CFR_TRANSMIT_REG reg;

				// ���M�f�[�^���R�s�[(reg��arg)
				__copy_from_user(&reg, (const CFR_TRANSMIT_REG*)arg, sizeof(CFR_TRANSMIT_REG));

				// ���M
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

				// ��M�o�b�t�@�̃|�C���^�ƃC���f�b�N�X���R�s�[(reg��arg)
				__copy_from_user(&reg, (const CFR_RECEIVE_REG*)arg, sizeof(CFR_RECEIVE_REG));

				// NDATn���W�X�^��n�����߂�
				ndat_index = reg.index / 32;

				// NDATn���W�X�^�����[�h
				ndat = catsdrv_flexray_read32(cc_index, NDAT1 + ndat_index * 4);

				// NDATn���W�X�^�̉��r�b�g�ڂ������߂�
				bit_no = reg.index % 32;

				if ((ndat & (1 << bit_no)) != 0)
				{
					// ��M�f�[�^���擾
					catsdrv_flexray_receive(cc_index, reg.index, reg.data, &reg.length);

					reg.received = 1;
				}
				else
				{
					reg.length = 0;
					reg.received = 0;
				}

				// ��M�f�[�^���R�s�[(arg��reg)
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

	// READY�R�}���h�Ȃ�A�����b�N����
	if (cmd == CFR_READY)
	{
		// SUCC1���W�X�^�̓��e��ǂݏo��
		succ1 = catsdrv_flexray_read32(cc_index, SUCC1);

		// �A�����b�N�V�[�P���X�����s
		catsdrv_flexray_write32(cc_index, LCK, FLEXRAY_LCK_KEY1);
		catsdrv_flexray_write32(cc_index, LCK, FLEXRAY_LCK_KEY2);
	}

	// �R�}���h����������
	succ1 = (succ1 & 0xfffffff0) | (cmd & 0x0000000f);
	catsdrv_flexray_write32(cc_index, SUCC1, succ1);
}

void catsdrv_flexray_getpocstate(int cc_index, CFR_GETPOCSTATE_REG *reg)
{
	unsigned long ccsv;
	unsigned long ccev;

	// CCSV���W�X�^�̓��e��ǂݏo��
	ccsv = catsdrv_flexray_read32(cc_index, CCSV);

	// CCEV���W�X�^�̓��e��ǂݏo��
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

		// 4�o�C�g���f�[�^��������
		catsdrv_flexray_write32(cc_index, WRDS1 + i, data32);
	}

	// �C���v�b�g�o�b�t�@�z�X�g���󂫂ɂȂ�܂ő҂�
	for (wait = 0; wait < 1000; wait++)
	{
		ibcr = catsdrv_flexray_read32(cc_index, IBCR_E);

		if ((ibcr & FLEXRAY_IBCR_IBSYH) == 0)
		{
			break;
		}

		udelay(1);
	}

	// �f�[�^�Z�N�V�������C���v�b�g�o�b�t�@���烁�b�Z�[�WRAM�֓]������
	catsdrv_flexray_write32(cc_index, IBCM, FLEXRAY_IBCM_STXRH | FLEXRAY_IBCM_LDSH);

	// ���b�Z�[�W�o�b�t�@�ԍ���I������
	catsdrv_flexray_write32(cc_index, IBCR_E, index);

	if (int_enable)
	{
		// TXRQn���W�X�^��n�����߂�
		txrq_index = index / 32;

		// TXRQn���W�X�^�̉��r�b�g�ڂ������߂�
		bit_no = index % 32;

		// ���M�v���t���O�Z�b�g
		txrq[cc_index][txrq_index] |= 1 << bit_no;

		// ���M�������荞�݂�������
		catsdrv_flexray_write32(cc_index, SIES, FLEXRAY_SIES_TXIE);
	}
}

int catsdrv_flexray_gettxbuf(int cc_index)
{
	unsigned long txrq_reg, txrq_data;
	int txrq_index, bit_no, buf_no;

	txrq_reg = TXRQ1;

	buf_no = 0;

	// TXRQ�̃��W�X�^�������[�v
	for (txrq_index = 0; txrq_index < 4; txrq_index++)
	{
		txrq_data = catsdrv_flexray_read32(cc_index, txrq_reg);

		// 32�r�b�g�����[�v
		for (bit_no = 0; bit_no < 32; bit_no++)
		{
			// ���M�v���������đ��M�ς݂ɂȂ��Ă���
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

	// �f�[�^�Z�N�V���������b�Z�[�WRAM����A�E�g�v�b�g�o�b�t�@�֓]������
	// �w�b�_�Z�N�V���������b�Z�[�WRAM����A�E�g�v�b�g�o�b�t�@�֓]������
	obcm = catsdrv_flexray_read32(cc_index, OBCM);
	catsdrv_flexray_write32(cc_index, OBCM, obcm | FLEXRAY_OBCM_RDSS | FLEXRAY_OBCM_RHSS);

	// ���b�Z�[�WRAM����A�E�g�v�b�g�o�b�t�@�V���h�E�֓]�����郁�b�Z�[�W�o�b�t�@�ԍ���ݒ肷��
	catsdrv_flexray_write32(cc_index, OBCR, index & 0x007F);

	// ���b�Z�[�WRAM����A�E�g�v�b�g�o�b�t�@�V���h�E�֓]������
	obcr = catsdrv_flexray_read32(cc_index, OBCR);
	catsdrv_flexray_write32(cc_index, OBCR, obcr | FLEXRAY_OBCR_REQ);

	// �A�E�g�v�b�g�o�b�t�@�V���h�E���󂫂ɂȂ�܂ő҂�
	for (wait = 0; wait < 1000; wait++)
	{
		obcr = catsdrv_flexray_read32(cc_index, OBCR);

		if ((obcr & FLEXRAY_OBCR_OBSYS) == 0)
		{
			break;
		}

		udelay(1);
	}

	// �A�E�g�v�b�g�o�b�t�@�V���h�E�ƃA�E�g�v�b�g�o�b�t�@�z�X�g�����ւ���
	obcr = catsdrv_flexray_read32(cc_index, OBCR);
	catsdrv_flexray_write32(cc_index, OBCR, obcr | FLEXRAY_OBCR_VIEW);

	// �f�[�^�����擾
	rdhs2 = catsdrv_flexray_read32(cc_index, RDHS2);
	plr = (rdhs2 >> 24) & 0x7f;
	*length = plr * 2;

	// ��M�f�[�^�����o��
	for (i = 0; i < plr * 2; i += 4)
	{
		// 4�o�C�g���f�[�^�ǂݏo��
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

