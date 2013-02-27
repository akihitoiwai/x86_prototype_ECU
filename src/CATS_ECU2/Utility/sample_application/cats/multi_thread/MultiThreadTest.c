/********************************************/
/*	FlexRay����M�e�X�g�v���O����			*/
/********************************************/
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include <signal.h>
#include "/usr/src/CATS_ECU2/kernel_driver/cats_ioctl.h"

pthread_t	thread1, thread2;

/*******************************************************************
*
* GPIO�����p��`
*
*******************************************************************/
#include "/usr/src/CATS_ECU2/api_library/gpioapi/gpio_api.h"

#define MASK16BIT 0xFFFF

extern int g_gpio_fd;
int count1;

/*******************************************************************
*
* FlexRay�����p��`
*
*******************************************************************/
#include "/usr/src/CATS_ECU2/kernel_driver/cats_flexray.h"

#define E_OK		0
#define E_NOT_OK	1

#define True		1
#define False		0

#define FR_POCSTATE_DEFAULT_CONFIG	0
#define FR_POCSTATE_READY			1
#define FR_POCSTATE_NORMAL_ACTIVE	2
#define FR_POCSTATE_NORMAL_PASSIVE	3
#define FR_POCSTATE_HALT			4
#define FR_POCSTATE_MONITOR_MODE	5
#define FR_POCSTATE_CONFIG			15

int set_cc_config_chi(int cc_index, char *chi_file_path);
int compare_write(char *str, unsigned long *addr, unsigned long *data);
int compare_wait(char *str, unsigned long *addr, unsigned long *data);
int switch_state_ready(int cc_index);
int send_wakeup(int cc_index);
int run(int cc_index);
int wait_normal_active(int cc_index);
int set_cmd(int cc_index, cFr_CHICmdVectType cFr_CHICmdVect);

char *FRDRV_PATH[2] = {
	"/dev/catsdrv1",
	"/dev/catsdrv2",
};

char *chi_file_path[2] = {
	"Ecu1_Controller1.chi",
	"Ecu1_Controller2.chi",
};

int catsdrv_fd[2] = {-1, -1};
int Received = False;
unsigned long TxData;
unsigned long RxData;
/*******************************************************************
�@�\ 			:	GPIO CALLBACK
����			:	����
�߂�l			:	����
�@�\����		:	�f�o�h�n�R�[���o�b�N�@�\
*******************************************************************/
void gpio_test_callback(void)
{
	int ret, tmp;
	int ioctl_arg;

//	printf("gpio callback\n");

	/* GPIO ���荞�݃`�F�b�N */
	ret = ioctl(g_gpio_fd, IOCTL_CATS_CallBackGPIOGetIrq, &ioctl_arg);
//	printf("IOCTL_CATS_CallBackGPIOGetIrq = %x ret = %d\n",ioctl_arg ,ret);

	if((ioctl_arg & GPIO_INT) != 0 )
	{
		/* ���荞�ݗv������ */
		ret = ioctl(g_gpio_fd, IOCTL_CATS_CallBackGPIOClrIrq, &ioctl_arg);
//		printf("IOCTL_CATS_CallBackGPIOClrIrq gpio_intreq = %x\n",ioctl_arg );
		if(ret == API_NG) return;

		/* GPIO ���荞�݃`�F�b�N */
		/* �P�U�`���l�����̂f�o�h�n���荞�ݏ�Ԃ��擾���� */
		ret = FncGPIOIntSts2( &tmp );
		printf("FncGPIOIntSts2 gpio_intsts = %x\n", tmp);
		if(ret == API_NG) return;

		/* ���荞�ݗv���L�� */
		if(tmp)
		{
			printf("\x1b[31mGPIO CALLBACK count = %d \x1b[0m \n",++count1);
//			printf("gpio_intsts = %x\n", tmp);

			/* ���荞�ݗv������ */
			ret = FncGPIOIntClear ( tmp );
			if(ret == API_NG) return;

			/* �P�U�`���l�����̂f�o�h�n���荞�ݏ�Ԃ��擾���� */
			ret = FncGPIOIntSts2( &tmp );
//			printf("FncGPIOIntSts2 gpio_intsts = %x\n", tmp);
			if(ret == API_NG) return;

			printf("\n");
		}
	}
}
/*******************************************************************
�@�\ 			:	FlexRay CALLBACK
����			:	����
�߂�l			:	����
�@�\����		:	FlexRay���荞�݃R�[���o�b�N
*******************************************************************/
void fr_test_callback(void)
{
	CFR_GETINTSTS_REG getintsts_reg;
	CFR_RECEIVE_REG receive_reg;
	CFR_RESETINT_REG resetint_reg;
	int cc_index, msg_index, i;

	for (cc_index = 0; cc_index < 2; cc_index++)
	{
		// ���荞�݂̗L�����`�F�b�N
		if (ioctl(catsdrv_fd[cc_index], IOCTL_CATS_FLEXRAY_GETINTSTS, &getintsts_reg) != 0)
		{
			printf("FlexRay%d Error: Get Int Status\n", cc_index);
			break;
		}

		// ��M���荞�݂Ȃ�
		if (!getintsts_reg.rxi)
		{
			continue;
		}

		for (msg_index = 1; msg_index <= 2; msg_index++)
		{
			// ��M�o�b�t�@�̃C���f�b�N�X���Z�b�g
			receive_reg.index = msg_index;

			// ��M�f�[�^�i�[��̃|�C���^���Z�b�g
			receive_reg.data = (char*)&RxData;

			// ��M�f�[�^�擾
			if (ioctl(catsdrv_fd[cc_index], IOCTL_CATS_FLEXRAY_RECEIVE, &receive_reg) != 0)
			{
				printf("FlexRay%d Error: Receive\n", cc_index);
				break;
			}

			// ��M�f�[�^����
			if (receive_reg.received)
			{
				Received = True;

				printf("FlexRay%d Rx: MSG=%d LEN=%d DATA=%08lx\n", cc_index, msg_index, receive_reg.length, RxData);
			}
		}

		// ���荞�݃N���A
		resetint_reg.txi = False;
		resetint_reg.rxi = True;

		if (ioctl(catsdrv_fd[cc_index], IOCTL_CATS_FLEXRAY_RESETINT, &resetint_reg) != 0)
		{
			printf("FlexRay%d Error: Reset Int\n", cc_index);
			break;
		}
	}
}
/*******************************************************************
�@�\ 			: 	main_callback
����			:	����
�߂�l			:	����
�@�\����		:	�f�o�h�n�R�[���o�b�N�@�\
*******************************************************************/
void main_callback(void)
{
	// GPIO�̃R�[���o�b�N
	gpio_test_callback();

	// FlexRay�̃R�[���o�b�N
	fr_test_callback();
}
/*******************************************************************
�@�\ 			: 	gpio_thread
����			:	����
�߂�l			:	����
�@�\����		:	
*******************************************************************/
void *gpio_thread(void *args)
{
	unsigned int ret,tmp,atmp[16],atmp2[16],i;
	unsigned int count,tmp_in,tmp_out;
	unsigned char c;
	int oflags;
	int infinity;

	infinity = 1;

	printf("GPIO TEST START   \n");

/********************************************************************/
/*							GPIO Init								*/
/********************************************************************/

	ret = FncInitGPIOOutput();						/* GPIO �o�͏����� */
	if(ret != API_OK)
	{
		printf("FncInitGPIOOutput ret = %d\n",ret);
		printf("GPIO TEST FAIL!\n");
		return;
	}

	ret = FncInitGPIOInput();						/* GPIO ���͏����� */
	if(ret != API_OK)
	{
		printf("FncinitGPIOInput ret = %d\n",ret);
		printf("GPIO TEST FAIL!\n");
		return;
	}


/********************************************************************/
/*						Setup interrupt								*/
/********************************************************************/
	fcntl(g_gpio_fd, F_SETOWN, getpid());
	oflags = fcntl(g_gpio_fd, F_GETFL);
	fcntl(g_gpio_fd, F_SETFL, oflags | FASYNC);

	atmp[0]  = 0xFFFF;									/* bit 0 - 15 ���荞�ݗL�� */
	atmp[1]  = 0xFFFF;									/* bit 0 - 15 Hi���x���Ŋ��荞�ݔ��� */

	ret = FncGPIOIntEnable2 ( atmp[0], atmp[1] );	/* ���荞�ݗL�������w�� */
	if(ret != API_OK)
	{
		printf("FncGPIOIntEnable2 ret = %d\n",ret);
		printf("GPIO TEST FAIL!\n");
		return;
	}


/********************************************************************/
/*							Loop Section							*/
/********************************************************************/

	while(infinity)
	{
		printf("GPIO TEST RUNNING \n");

		tmp_out = 1;

		for(i=1;i<25;i++)
		{
//			printf("i = %2d tmp_out = %6x  ",i ,tmp_out);
			ret = FncGPIOSetAll( tmp_out );			/* 24 bit GPIO OUT */
			if(ret != API_OK)
			{
				printf("FncGPIOSetAll ret = %d tmp_out = %x\n", ret, tmp_out);
				printf("GPIO TEST FAIL!\n");
				return;
			}
			tmp_out = tmp_out << 1;

			ret = FncGPIOGetAll( &tmp_in );			/* 16 bit GPIO IN */
			if(ret != API_OK)
			{
				printf("FncGPIOGetAll ret = %d val = %x\n", ret, tmp_in);
				printf("GPIO TEST FAIL!\n");
				return;
			}

			usleep(200000);							/* 200ms */
		}
	}


/********************************************************************/
/*			GPIO ���荞�ݖ����^�R�[���o�b�N����						*/
/********************************************************************/

	atmp[0]  = 0x0;									/* bit 0 - 15 ���荞�ݖ��� */
	atmp[1]  = 0xFFFF;								/* bit 0 - 15 Hi���x���Ŋ��荞�ݔ��� */

	ret = FncGPIOIntEnable2 ( atmp[0], atmp[1] );	/* ���荞�ݗL�������w�� */
	if(ret != API_OK)
	{
		printf("FncGPIOIntEnable2 ret = %d\n",ret);
		printf("GPIO TEST FAIL!\n");
		return;
	}

	ret = FncGpioHandlerClear();
	if(ret != API_OK)
	{
		printf("FncGpioHandlerClear() ret = %d\n",ret);
		printf("GPIO TEST FAIL!\n");
		return;
	}

	tmp = 0x0;
	ret = FncGPIOSetAll( tmp );						/* GPIO OUT */
	if(ret != API_OK)
	{
		printf("FncGPIOSetAll ret = %d tmp = %x\n", ret, tmp);
		printf("GPIO TEST FAIL!\n");
		return;
	}

	printf("GPIO TEST END\n");

	return NULL;
}
/*******************************************************************
�@�\ 			: 	fr_thread
����			:	����
�߂�l			:	����
�@�\����		:	
*******************************************************************/
void *fr_thread(void *args)
{
	int cc_index, oflags, wait, i;
	struct sigaction act;
	CFR_GETPOCSTATE_REG reg;

	///////////////////////////////////////////////////////////////////////
	// FlexRay�h���C�o�I�[�v��
	///////////////////////////////////////////////////////////////////////
	for (cc_index = 0; cc_index < 2; cc_index++)
	{
		catsdrv_fd[cc_index] = open(FRDRV_PATH[cc_index], O_RDWR);

		if(catsdrv_fd[cc_index] < 0)
		{
			printf("FlexRay%d Driver Open Error: %s\n", cc_index, FRDRV_PATH[cc_index]);
			goto ERROR;
		}

		fcntl(catsdrv_fd[cc_index], F_SETOWN, getpid());
		oflags = fcntl(catsdrv_fd[cc_index], F_GETFL);
		fcntl(catsdrv_fd[cc_index], F_SETFL, oflags | FASYNC);
	}

	///////////////////////////////////////////////////////////////////////
	// FlexRay�̃R���t�B�O���[�V�������s��
	///////////////////////////////////////////////////////////////////////
	for (cc_index = 0; cc_index < 2; cc_index++)
	{
		// CONFIG�R�}���h�����s����
		if (set_cmd(cc_index, CFR_CONFIG) != E_OK)
		{
			printf("FlexRay%d Error: CONFIG\n", cc_index);
			goto ERROR;
		}

		// FlexRay�R���g���[���̃R���t�B�O���[�V�������s��
		if (set_cc_config_chi(cc_index, chi_file_path[cc_index]) != E_OK)
		{
			printf("FlexRay%d Error: CC CONFIG\n", cc_index);
			goto ERROR;
		}
	}

	///////////////////////////////////////////////////////////////////////
	// FlexRay�̓������s��
	///////////////////////////////////////////////////////////////////////
	for (cc_index = 0; cc_index < 2; cc_index++)
	{
		// READY�R�}���h�����s����
		if (switch_state_ready(cc_index) != E_OK)
		{
			printf("FlexRay%d Error: READY\n", cc_index);
			goto ERROR;
		}

		// WAKEUP�R�}���h�����s����
		if (send_wakeup(cc_index) != E_OK)
		{
			printf("FlexRay%d Error: WAKEUP\n", cc_index);
			goto ERROR;
		}

		// ALLOW_COLDSTART�R�}���h�����s����
		if (set_cmd(cc_index, CFR_ALLOW_COLDSTART) != E_OK)
		{
			printf("FlexRay%d Error: ALLOW_COLDSTART\n", cc_index);
			goto ERROR;
		}

		// RUN�R�}���h�����s����
		if (set_cmd(cc_index, CFR_RUN) != E_OK)
		{
			printf("FlexRay%d Error: RUN\n", cc_index);
			goto ERROR;
		}
	}

	///////////////////////////////////////////////////////////////////////
	// NORMAL_ACTIVE�ɂȂ�̂�҂�
	///////////////////////////////////////////////////////////////////////
	for (cc_index = 0; cc_index < 2; cc_index++)
	{
		if (wait_normal_active(cc_index) != E_OK)
		{
			printf("FlexRay%d Error: NORMAL ACTIVE\n", cc_index);
			goto ERROR;
		}

		// ���荞�݂�������
		if (ioctl(catsdrv_fd[cc_index], IOCTL_CATS_FLEXRAY_ENABLEINT) != 0)
		{
			printf("FlexRay%d Error: ENABLE INT\n", cc_index);
			goto ERROR;
		}
	}

	///////////////////////////////////////////////////////////////////////
	// �����J�n
	///////////////////////////////////////////////////////////////////////
	printf("FlexRay Test Start\n");

	while (True)
	{
		Received = False;

		printf("FlexRay%d Tx: MSG=%d LEN=%d DATA=%08lx\n", 0, 0, sizeof(TxData), TxData);

		sleep(1);

		// ���M
		if (send_data(0, 0, &TxData, sizeof(TxData)) != E_OK)
		{
			printf("FlexRay%d Error: SEND DATA\n", 0);
			goto ERROR;
		}

		// ��M���荞�ݑ҂�
		for (wait = 0; wait < 10000 && !Received; wait++)
		{
			usleep(100);
		}

		// ��M���Ȃ���΃G���[
		if (!Received)
		{
			printf("FlexRay%d Error: Receive Timeout\n", 1);
			goto ERROR;
		}
		
		// ���M�f�[�^�Ǝ�M�f�[�^�̈�v���`�F�b�N
		if (TxData != RxData)
		{
			printf("FlexRay Test: NG (TxData=%02X RxData=%02X)\n", TxData, RxData);
			goto ERROR;
		}

		printf("FlexRay Test: OK\n");

		// ���M�f�[�^���X�V
		TxData++;
	}

ERROR:
	///////////////////////////////////////////////////////////////////////
	// �h���C�o�N���[�Y
	///////////////////////////////////////////////////////////////////////
	if (catsdrv_fd[0] > -1)
	{
		close(catsdrv_fd[0]);
	}

	if (catsdrv_fd[1] > -1)
	{
		close(catsdrv_fd[1]);
	}

	return NULL;
}
/*******************************************************************
�@�\ 		: 	main
����		:	����
�߂�l		:	����
�@�\����	:	
*******************************************************************/
int main(int argc, char *argv[])
{
	int oflags;
	struct sigaction act;

	///////////////////////////////////////////////////////////////////////
	// ���荞�ݒʒm�p�̃V�O�i����ݒ�
	///////////////////////////////////////////////////////////////////////
	memset(&act, 0, sizeof(struct sigaction));
	sigaction(SIGIO, NULL, &act);
	act.sa_handler = (__sighandler_t)main_callback;	// �R�[���o�b�N�֐��Z�b�g
	sigemptyset(&act.sa_mask);
	sigaddset(&act.sa_mask, SIGIO);
	act.sa_flags = 0;
	act.sa_restorer = NULL;
	sigaction(SIGIO, &act, NULL);

	///////////////////////////////////////////////////////////////////////
	// GPIO�����X���b�h�𐶐�
	///////////////////////////////////////////////////////////////////////
	pthread_create(&thread1, NULL, gpio_thread, (void *)NULL);

	///////////////////////////////////////////////////////////////////////
	// FlexRay�����X���b�h�𐶐�
	///////////////////////////////////////////////////////////////////////
	pthread_create(&thread2, NULL, fr_thread, (void *)NULL);

	///////////////////////////////////////////////////////////////////////
	// GPIO�����X���b�h�̏I����҂�
	///////////////////////////////////////////////////////////////////////
	pthread_join(thread1, NULL);

	///////////////////////////////////////////////////////////////////////
	// FlexRay�����X���b�h�̏I����҂�
	///////////////////////////////////////////////////////////////////////
	pthread_join(thread2, NULL);

	return 0;
}

int set_cc_config_chi(int cc_index, char *path)
{
	FILE *fd;
	char str[256];
	unsigned long addr, data;
	CFR_WRITE32_REG wreg;
	CFR_READ32_REG rreg;
	int wait, ret;

	// CHI�t�@�C�����J��
	fd = fopen(path, "r");
	if (fd == NULL)
	{
		printf("CHI File Open Error: %s\n", path);
		return E_NOT_OK;
	}

	ret = E_OK;
	while (NULL != fgets(str, 255, fd))
	{
		// "WRITE32"�Ɣ�r
		if (compare_write(str, &addr, &data))
		{
			// "WRITE32"�ƈ�v�����烉�C�g�����s
			wreg.addr = addr;
			wreg.data = data;
			ret = ioctl(catsdrv_fd[cc_index], IOCTL_CATS_FLEXRAY_WRITE32, &wreg);
		}
		// "WAIT_TILL_CLEARED32"�Ɣ�r
		else if (compare_wait(str, &addr, &data))
		{
			// "WAIT_TILL_CLEARED32"�ƈ�v������w��̃r�b�g���N���A�����܂ŃE�F�C�g�����s
			rreg.addr = addr;
			for (wait = 1000; wait > 0; wait--)
			{
				ret = ioctl(catsdrv_fd[cc_index], IOCTL_CATS_FLEXRAY_READ32, &rreg);

				// �w��̃r�b�g���N���A����Ă���?
				if ((rreg.data & data) == 0)
				{
					break;
				}

				usleep(1);
			}

			// "WAIT_TILL_CLEARED32"�Ŏw��̃r�b�g���N���A����Ȃ�������G���[�Ŕ�����
			if (wait == 0)
			{
				ret = E_NOT_OK;
				break;
			}
		}
	}

	fclose(fd);

	return ret;
}

int compare_write(char *str, unsigned long *addr, unsigned long *data)
{
	char work[255];
	char write_str[] = "WRITE32";
	int i;

	// "WRITE32(0x00000000, 0x00000000); /* XXXX */"��'('��')'��'x'��','���󔒂ɒu��������
	// "WRITE32 0x00000000  0x00000000 ; /* XXXX */"�ɂ���
	for (i = 0; i < 255 && str[i] != '\0'; i++)
	{
		if ((str[i] == '(') || (str[i] == ')') || (str[i] == ','))
			str[i] = ' ';
	}

	// "WRITE32 0x00000000  0x00000000 ; /* XXXX */"�𕪉�����
	work[0] = '\0';
	sscanf(str, "%s %x %x", work, data, addr);

	// �ŏ��̕������"WRITE32"?
	if (strcmp(write_str, work))
	{
		// �s��v�Ń��^�[��
		return False;
	}
	CFR_GETPOCSTATE_REG reg;

	return True;
}

int compare_wait(char *str, unsigned long *addr, unsigned long *data)
{
	char work[255];
	char wait_str[]  = "WAIT_TILL_CLEARED32";
	int i;

	// "WAIT_TILL_CLEARED32(0x00000000, 0x00000000); /* XXXX */"��'('��')'��'x'��','���󔒂ɒu��������
	// "WAIT_TILL_CLEARED32 0x00000000  0x00000000 ; /* XXXX */"�ɂ���
	for (i = 0; i < 255 && str[i] != '\0'; i++)
	{
		if ((str[i] == '(') || (str[i] == ')') || (str[i] == ','))
			str[i] = ' ';
	}

	// "WAIT_TILL_CLEARED32 0x00000000  0x00000000 ; /* XXXX */"�𕪉�����
	work[0] = '\0';
	sscanf(str, "%s %x %x", work, data, addr);

	// �ŏ��̕������"WAIT_TILL_CLEARED32"?
	if (strcmp(wait_str, work))
	{
		// �s��v�Ń��^�[��
		return False;
	}

	return True;
}

int switch_state_ready(int cc_index)
{
	CFR_GETPOCSTATE_REG reg;
	int wait;

	// READY��ԂɑJ�ڂ�����
	for (wait = 0; wait < 1000; wait++)
	{
		// POC�X�e�[�^�X���擾
		ioctl(catsdrv_fd[cc_index], IOCTL_CATS_FLEXRAY_GETPOCSTATE, &reg);
	
		if (reg.poc_status == FR_POCSTATE_READY)
		{
			return E_OK;
		}

		if (set_cmd(cc_index, CFR_READY) != E_OK)
		{
			return E_NOT_OK;
		}

		usleep(1);
	}

	return E_NOT_OK;
}

int send_wakeup(int cc_index)
{
	CFR_GETPOCSTATE_REG reg;
	int wait;

	// WAKEUP��ԂɑJ�ڂ�����
	if (set_cmd(cc_index, CFR_WAKEUP) != E_OK)
	{
		return E_NOT_OK;
	}

	// READY�ɂȂ�܂ő҂�
	for (wait = 0; wait < 1000; wait++)
	{
		// POC�X�e�[�^�X���擾
		ioctl(catsdrv_fd[cc_index], IOCTL_CATS_FLEXRAY_GETPOCSTATE, &reg);
	
		if (reg.poc_status == FR_POCSTATE_READY)
		{
			return E_OK;
		}

		usleep(1);
	}

	return E_NOT_OK;
}

int wait_normal_active(int cc_index)
{
	CFR_GETPOCSTATE_REG reg;
	int wait;

	// NORMAL_ACTIVE�ɂȂ�܂ő҂�
	for (wait = 0; wait < 1000; wait++)
	{
		// POC�X�e�[�^�X���擾
		ioctl(catsdrv_fd[cc_index], IOCTL_CATS_FLEXRAY_GETPOCSTATE, &reg);
	
		if (reg.poc_status == FR_POCSTATE_NORMAL_ACTIVE)
		{
			return E_OK;
		}

		usleep(1);
	}

	return E_NOT_OK;
}

int send_data(int cc_index, int msg_buf_index, char *data, int length)
{
	CFR_TRANSMIT_REG reg;
	int ret;

	reg.index  = msg_buf_index;
	reg.data   = (unsigned char*)data;
	reg.length = length;
	reg.int_enable = False;

	// ���M����
	ret = ioctl(catsdrv_fd[cc_index], IOCTL_CATS_FLEXRAY_TRANSMIT, &reg);

	if (ret != 0)
	{
		return E_NOT_OK;
	}

	return E_OK;
}

int set_cmd(int cc_index, cFr_CHICmdVectType cFr_CHICmdVect)
{
	CFR_SETCMD_REG reg;
	int wait, ret;

	// �R�}���h���Z�b�g
	reg.cmd = cFr_CHICmdVect;

	for (wait = 0; wait < 1000; wait++)
	{
		ret = ioctl(catsdrv_fd[cc_index], IOCTL_CATS_FLEXRAY_SETCMD, &reg);

		if (ret != 0)
		{
			return E_NOT_OK;
		}

		// �r�W�[�łȂ���΃R�}���h�������ݐ���
		if (!reg.busy)
		{
			return E_OK;
		}

		usleep(1);
	}

	// �r�W�[�̂܂܂Ȃ�G���[
	return E_NOT_OK;
}

