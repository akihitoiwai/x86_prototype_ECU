/**********************************************************************************
 * ファイル名      : can_api.h
 * システム名      : x86 プロトタイプＥＣＵ OPE-RA Ver2.0
 * サブシステム名  : CAN API
 * プログラム名    : ヘッダー
 * CPU TYPE        : 
 * バージョン      : Ver1.00
 * --------------------------------------------------------------------------------
 * 作成者          : 
 * 作成部署        : 
 * 作成日付        : 2009年09月23日 新規作成
 * 更新履歴        : 
 **********************************************************************************/
#ifndef CAN_API_H
#define CAN_API_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include "/usr/src/CATS_ECU2/kernel_driver/cats_ioctl.h"
#include "/usr/src/CATS_ECU2/kernel_driver/cats_sja1000.h"


//-----------------------------------------------------------------//
/*!
	@brief	CAN API コンテキスト構造体
*/
//-----------------------------------------------------------------//
typedef struct {
	int			fd;		///< ファイル識別子
	CAN_LOG		log[2];	///< ログ保存用構造体
	sem_t		sem_obj;
} CAN_CONTEXT;


#ifdef __cplusplus
extern "C" {
#endif

//-----------------------------------------------------------------//
/*!
	@brief	CAN ドライバー、コンテキストの作成
	@return CAN_CONTEXT 構造体のポインター
*/
//-----------------------------------------------------------------//
CAN_CONTEXT *CAN_create();


//-----------------------------------------------------------------//
/*!
	@brief	CAN デバイスのレジスターを直接読み出す。
	@param[in]	con デバイス・コンテキスト
	@param[in]	device デバイス
	@param[in]	adr	   SJA1000 のアドレス
	@param[in]	data   読み出したデータの格納先
	@return		正常なら「０」を返す
 */
//-----------------------------------------------------------------//
int CAN_read_register(CAN_CONTEXT *con, int device, int adr, int *data);


//-----------------------------------------------------------------//
/*!
	@brief	CAN デバイスのレジスターを直接書き込む。
	@param[in]	con デバイス・コンテキスト
	@param[in]	device デバイス
	@param[in]	adr	   SJA1000 のアドレス
	@param[in]	data   書き込むデータ
	@return		正常なら「０」を返す
*/
//-----------------------------------------------------------------//
int CAN_write_register(CAN_CONTEXT *con, int device, int adr, int data);


//-----------------------------------------------------------------//
/*!
	@brief	CAN 初期化
	@param[in]	con デバイス・コンテキスト
	@param[in]	device デバイス
	@param[in]	send_buff_size	送信バッファサイズ
	@param[in]	recv_buff_size	受信バッファサイズ
	@return		正常なら「０」を返す
*/
//-----------------------------------------------------------------//
int CAN_init(CAN_CONTEXT *con, int device, int send_buff_size, int recv_buff_size);


//-----------------------------------------------------------------//
/*!
	@brief	CAN リセット
	@param[in]	con デバイス・コンテキスト
	@param[in]	device デバイス
	@param[in]	mode 構造体ポインター
	@return		正常なら「０」を返す
*/
//-----------------------------------------------------------------//
int CAN_reset(CAN_CONTEXT *con, int device, const CAN_MODE *mode);


//-----------------------------------------------------------------//
/*!
	@brief	CAN スタート
	@param[in]	con デバイス・コンテキスト
	@param[in]	device デバイス
	@return		正常なら「０」を返す
*/
//-----------------------------------------------------------------//
int CAN_start(CAN_CONTEXT *con, int device);


//-----------------------------------------------------------------//
/*!
	@brief	CAN ストップ@n
			一度「停止」させた場合、「reset」して「start」する必要がある。
	@param[in]	con デバイス・コンテキスト
	@param[in]	device デバイス
	@return		正常なら「０」を返す
*/
//-----------------------------------------------------------------//
int CAN_stop(CAN_CONTEXT *con, int device);


//-----------------------------------------------------------------//
/*!
	@brief	CAN スリープ
	@param[in]	con デバイス・コンテキスト
	@param[in]	device デバイス
	@return		正常なら「０」を返す
*/
//-----------------------------------------------------------------//
int CAN_sleep(CAN_CONTEXT *con, int device);


//-----------------------------------------------------------------//
/*!
	@brief	CAN ログ取得
	@param[in]	con デバイス・コンテキスト
	@param[in]	device デバイス
	@param[in]	log	   ログ構造体
	@return		正常なら「０」を返す
*/
//-----------------------------------------------------------------//
int CAN_log(CAN_CONTEXT *con, int device, CAN_LOG *log);



//-----------------------------------------------------------------//
/*!
	@brief	CAN センド（送信）
			送信バイト数が「０」の場合、リモートフレームの送信となる
	@param[in]	con デバイス・コンテキスト
	@param[in]	device デバイス
	@param[in]	frame 構造体ポインター
	@return		正常なら「０」を返す
*/
//-----------------------------------------------------------------//
int CAN_send_device(CAN_CONTEXT *con, int device, const CAN_FRAME *frame);


//-----------------------------------------------------------------//
/*!
	@brief	CAN レシーブ（受信）
	@param[in]	con デバイス・コンテキスト
	@param[in]	device デバイス
	@param[in]	frame 構造体ポインター
	@return		レシーブデータがあれば、０以外
*/
//-----------------------------------------------------------------//
int CAN_recv_device(CAN_CONTEXT *con, int device, CAN_FRAME *frame);


//-----------------------------------------------------------------//
/*!
	@brief	CAN ステータスを受け取る
	@param[in]	con デバイス・コンテキスト
	@param[in]	device デバイス
	@param[in]	status 構造体ポインター
	@return		正常なら「０」を返す
*/
//-----------------------------------------------------------------//
int CAN_status(CAN_CONTEXT *con, int device, CAN_STATUS *status);


//-----------------------------------------------------------------//
/*!
	@brief	CAN 終了処理
	@param[in]	con デバイス・コンテキスト
	@param[in]	device デバイス
	@return		正常なら「０」を返す
*/
//-----------------------------------------------------------------//
int CAN_exit(CAN_CONTEXT *con, int device);



//-----------------------------------------------------------------//
/*!
	@brief	CAN ドライバー、コンテキストの廃棄
	@param[in]	con デバイス・コンテキスト
*/
//-----------------------------------------------------------------//
void CAN_destroy(CAN_CONTEXT *con);


//-----------------------------------------------------------------//
/*!
	@brief	CAN 割り込みログ・ダンプ
	@param[in]	con デバイス・コンテキスト
	@param[in]	device デバイス
	@param[in]	output	ダンプ出力先のストリームポインター
*/
//-----------------------------------------------------------------//
void CAN_log_dump(CAN_CONTEXT *con, int device, FILE *output);


#ifdef __cplusplus
};
#endif

#endif	// CAN_API_H

