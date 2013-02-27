/**********************************************************************************
 * ファイル名      : context.h
 * システム名      : x86 プロトタイプＥＣＵ OPE-RA Ver2.0
 * サブシステム名  : カーネルドライバ
 * プログラム名    : コンテキスト関係（ヘッダー）
 * バージョン      : Ver1.00
 * --------------------------------------------------------------------------------
 * 作成者          : 
 * 作成部署        : 
 * 作成日付        : 2009年09月23日 新規作成
 * 更新履歴        : 
 **********************************************************************************/
#ifndef CONTEXT_H
#define CONTEXT_H

#include "cats_sja1000.h"


#define BUFFER_LIMIT	256

typedef struct {
	int	   send_frame_max;
	int	   send_frame_put;
	int	   send_frame_get;

	CAN_FRAME	send_frame[BUFFER_LIMIT];
	volatile int	send_kick;
	volatile int	send_kick2;
	size_t		 	send_count;

	int	   recv_frame_max;
	int	   recv_frame_put;
	int	   recv_frame_get;
	CAN_FRAME	recv_frame[BUFFER_LIMIT];
	size_t		recv_count;

	spinlock_t	send_lock;
	spinlock_t	recv_lock;

	size_t		interrupt_count;
	size_t		busoff_error_count;
	size_t		arbitr_lost_count;
	size_t		passive_error_count;
	size_t		wakeup_count;
	size_t		overrun_error_count;
	size_t		error_count;

	size_t		recv_buff_overrun_count;
	size_t		send_buff_overrun_count;

	unsigned char	rx_error_count;
	unsigned char	tx_error_count;
	unsigned char	rx_message_count;
	unsigned char	temp;

	CAN_LOG		log;

} CAN_INTR_PAD;


#endif	// CONTEXT_H
