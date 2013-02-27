/**********************************************************************************
 * ファイル名      : main.cpp
 * システム名      : x86 プロトタイプＥＣＵ OPE-RA Ver2.0
 * サブシステム名  : CAN サンプル
 * バージョン      : Ver1.01
 * --------------------------------------------------------------------------------
 * 作成者          : 
 * 作成部署        : 
 * 作成日付        : 2009年09月30日 新規作成
 * 更新履歴        : 2010年02月15日
 **********************************************************************************/
#include <iostream>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#include "/usr/src/CATS_ECU2/api_library/canapi/can_api.h"




int get_integer_rand(int min, int max)
{
	int		a;

	a = rand() % (max - min + 1);
	return a + min;
}


void dump_register(CAN_CONTEXT *cont, int device, int start, int count)
{
	int i;
	int data;
	for(i = 0; i < count; ++i) {
		  CAN_read_register(cont, device, start + i, &data);
		  if((i & 3)==0) {
		  		 printf("(%d)Adr: 0x%02X:-   ", device, i + start);
		  }
		  printf("0x%02X", data);
		  if((i & 3)==3) {
		  		printf("\n");
		  } else {
		  	printf(" . ");
		  }
	}
	if((i & 3)!=3) {
  		printf("\n");
	}
}


void list_frame(const char* name, int count, int device, CAN_FRAME& frame)
{
	printf("%s - %05d.(%d) ID: %d, DLC: %d, type: %d, length: %d\n",
				 name, count, device, frame.id, frame.dlc, frame.type, frame.length);
	printf("(%d) data:", device);
    for(int i = 0; i < frame.length; ++i) {
		  printf(" 0x%02X", frame.data[i]);
	}
	printf("\n");
}


void list_status(CAN_CONTEXT *cont, int device)
{
	CAN_STATUS	st;

	CAN_status(cont, device, &st);
	printf("(%d) Error:         %d\n", device, st.error_count);
	printf("(%d) Overrun Error: %d\n", device, st.overrun_error_count);
	printf("(%d) Send count:    %d\n", device, st.send_count);
	printf("(%d) Recv count:    %d\n", device, st.recv_count);
	printf("(%d) Send buff Overrun: %d\n", device, st.send_buff_overrun_count);
	printf("(%d) Recv buff Overrun: %d\n", device, st.recv_buff_overrun_count);
	printf("(%d) Send buff count: %d\n", device, st.send_buff_count);
	printf("(%d) Recv buff count: %d\n", device, st.recv_buff_count);
}


void create_send_frame(int f_type, CAN_FRAME& send_frame)
{
	send_frame.id = 999;
	send_frame.dlc = get_integer_rand(1, 8);
	send_frame.length = send_frame.dlc;
	send_frame.type = f_type;
	for(int i = 0; i < send_frame.length; ++i) {
		send_frame.data[i] = get_integer_rand(0, 255);
	}
}


// レシーブバッファにフレームが何個あるか調べる。
int scan_recv_buff(CAN_CONTEXT *cont, int device)
{
	CAN_STATUS	st;

	CAN_status(cont, device, &st);

	return st.recv_buff_count;
}


static bool frame_cmp(int idx, CAN_FRAME& a, CAN_FRAME& b)
{
	if(a.length != b.length) {
		printf("(%d) Data Length missmatch: %d -- %d\n", idx, a.length, b.length);
		return false;
	}

	int l = 0;
	for(int i = 0; i < a.length; ++i) {
		if(a.data[i] != b.data[i]) break;
		l++;
	}
	if(l != a.length) {
		 printf("(%d) Data missmatch: \n", idx);
		 return false;
	}
	return true;
}


/***********************************************************************************
* MODULE         : main
* ABSTRACT       : 
* FUNCTION       : 
* ARGUMENT       : 
* NOTE           : 
* RETURN         : 
* CREATE         : 2009/09/30 
* UPDATE         : 2010/02/15
***********************************************************************************/
int main(int argc, char *argv[])
{
	CAN_CONTEXT		*cont;
	time_t			tt;

	time(&tt);
	srand(tt);

	cont = CAN_create();
	if(cont == NULL) return -1;

	CAN_init(cont, 0, 128, 128);
	CAN_init(cont, 1, 128, 128);


	CAN_MODE	mode;
	mode.baud_rate = 500;		// ボーレート
	for(int i = 0; i < 4; ++i) {
		mode.accept_code[i] = 0x00;
		mode.accept_mask[i] = 0xff;
	}
	CAN_reset(cont, 0, &mode); 
	CAN_reset(cont, 1, &mode); 

	CAN_start(cont, 0);
	CAN_start(cont, 1);

//	dump_register(cont, 0, 0, 32);
//	dump_register(cont, 1, 0, 32);

	int	send_device = 0;
	int	recv_device = 1;
	int frame_type = 0;			// 0: std, 1:ext
	int mini_block_max = 10;

	int	send_count = 0;
	int	recv_count = 0;
	int total_send_count = 0;
	int total_recv_count = 0;
	int loop_count = 500;

	int total_error = 0;
	int ret_val = 0;

	for(int loop = 0; loop < loop_count; ++loop) {
		send_count = 0;
		recv_count = 0;
// 送信
		int num = get_integer_rand(1, mini_block_max);

//		printf("(%d) Send frame: %d\n", loop, num);
		CAN_FRAME	send_frame[mini_block_max];
		for(int i = 0; i < num; ++i) {
			create_send_frame(frame_type, send_frame[i]);

			do{	/* 送信エラーの時はリトライする */
				ret_val = CAN_send_device(cont, send_device, &send_frame[i]);
				if(ret_val != CASTCAN_SUCCEEDED)
				{
					printf("CAN_send_device ret =%d\n" ,ret_val);
				}
			}while(ret_val != CASTCAN_SUCCEEDED);

//			list_frame("S", send_count, send_device, send_frame[i]);
			send_count++;
			total_send_count++;
		}
//		list_status(cont, send_device);
//		list_status(cont, recv_device);

// 受信
		CAN_FRAME	recv_frame[mini_block_max];
//		printf("(%d) Recv frame: %d\n", loop, num);
		for(int i = 0; i < num; ++i) {
			int lpc = 0;
			int f;
			while((f = scan_recv_buff(cont, recv_device)) == 0) {
				usleep(100);
				lpc++;
				if(lpc > 100) break;
			}

			if(f) {
				CAN_recv_device(cont, recv_device, &recv_frame[i]);
//				list_frame("R", recv_count, recv_device, recv_frame[i]);
				recv_count++;
				total_recv_count++;
			}
		}
		if(send_count != recv_count) {
			printf("recv error ! %d/%d\n", recv_count, send_count);
			total_error++;
		} else {
		    for(int i = 0; i < num; ++i) {
				if(frame_cmp(i, send_frame[i], recv_frame[i]) == false) {
					total_error++; 
				}
			}
		}

		if((loop % 50) == 0) printf("loop: %d\n", loop);

//		printf("\n");

   	}

	if(total_error) {
		list_status(cont, send_device);
		list_status(cont, recv_device);
		printf("Total error: %d\n", total_error);
	} else {
	  	printf("Total send/recv count: %d/%d\n", total_send_count, total_recv_count);
	}

	CAN_exit(cont, 0);
	CAN_exit(cont, 1);

	CAN_destroy(cont);

	return 0;
}

/* end of file "main.cpp" */

