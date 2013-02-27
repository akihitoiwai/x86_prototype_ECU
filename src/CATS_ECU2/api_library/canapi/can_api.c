/**********************************************************************************
 * ファイル名      : can_api.c
 * システム名      : x86 プロトタイプＥＣＵ OPE-RA Ver2.0
 * サブシステム名  : CAN API
 * プログラム名    : 
 * CPU TYPE        : 
 * バージョン      : Ver1.01
 * モジュール構成  : can_api.h			API Header
 * --------------------------------------------------------------------------------
 * 作成者          : 
 * 作成部署        : 
 * 作成日付        : 2009年09月23日 新規作成
 * 更新履歴        : 2010年02月15日
 **********************************************************************************/
#include "can_api.h"
#include "errno.h"

#define CATSDRV_PATH "/dev/catsdrv0"


//-----------------------------------------------------------------//
/*!
	@brief	CAN ドライバー、コンテキストの作成
	@return CAN_CONTEXT 構造体のポインター
*/
//-----------------------------------------------------------------//
CAN_CONTEXT *CAN_create()
{
	CAN_CONTEXT *con;
	int		fd;
	int		ret;

	fd = open(CATSDRV_PATH, O_RDWR);
	if(fd < 0) {
		fprintf(stderr, "Can't open can-device file: '%s'\n", CATSDRV_PATH);
		return NULL;
	}

	con = (CAN_CONTEXT *)malloc(sizeof(CAN_CONTEXT));
	con->fd = fd;

	ret = sem_init(&con->sem_obj, 0, 1);
	if(ret != 0) {
		close(fd);
		free(con);
		con = NULL;
	}

	return con;
}


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
int CAN_read_register(CAN_CONTEXT *con, int device, int adr, int *data)
{
	int			 ret;
	CAN_REGISTER reg;

	sem_wait(&con->sem_obj);
	reg.device = device;
	reg.address = adr;
	reg.data = -1;
	ret = ioctl(con->fd, IOCTL_CATS_CAN_read_register, &reg);
	if(ret == 0) {
	    *data = reg.data;
	}
	sem_post(&con->sem_obj);
	return ret;
}


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
int CAN_write_register(CAN_CONTEXT *con, int device, int adr, int data)
{
	int			 ret;
	CAN_REGISTER reg;

	sem_wait(&con->sem_obj);
	reg.device = device;
	reg.address = adr;
	reg.data = data;
	ret = ioctl(con->fd, IOCTL_CATS_CAN_write_register, &reg);
	sem_post(&con->sem_obj);
	return ret;
}


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
int CAN_init(CAN_CONTEXT *con, int device, int send_buff_size, int recv_buff_size)
{
	CAN_INIT	init;
	int			ret;

	sem_wait(&con->sem_obj);
	init.device = device;
	init.send_buff_size = send_buff_size;
	init.recv_buff_size = recv_buff_size;
	ret = ioctl(con->fd, IOCTL_CATS_CAN_init, &init);
	sem_post(&con->sem_obj);
	return ret;
}


//-----------------------------------------------------------------//
/*!
	@brief	CAN リセット
	@param[in]	con デバイス・コンテキスト
	@param[in]	device デバイス
	@param[in]	mode 構造体ポインター
	@return		正常なら「０」を返す
*/
//-----------------------------------------------------------------//
int CAN_reset(CAN_CONTEXT *con, int device, const CAN_MODE *mode)
{
	CAN_RESET	reset;
	int			ret;

	sem_wait(&con->sem_obj);
	reset.device = device;
	memcpy(&reset.mode, mode, sizeof(CAN_MODE));
	ret = ioctl(con->fd, IOCTL_CATS_CAN_reset, &reset);
	sem_post(&con->sem_obj);
	return ret;
}


//-----------------------------------------------------------------//
/*!
	@brief	CAN スタート
	@param[in]	con デバイス・コンテキスト
	@param[in]	device デバイス
	@return		正常なら「０」を返す
*/
//-----------------------------------------------------------------//
int CAN_start(CAN_CONTEXT *con, int device)
{
	int		dev = device;
	int		ret;

	sem_wait(&con->sem_obj);
	ret = ioctl(con->fd, IOCTL_CATS_CAN_start, &dev);
	sem_post(&con->sem_obj);
	return ret;
}


//-----------------------------------------------------------------//
/*!
	@brief	CAN ストップ@n
			一度「停止」させた場合、「reset」して「start」する必要がある。
	@param[in]	con デバイス・コンテキスト
	@param[in]	device デバイス
	@return		正常なら「０」を返す
*/
//-----------------------------------------------------------------//
int CAN_stop(CAN_CONTEXT *con, int device)
{
	int		dev = device;
	int		ret;

	sem_wait(&con->sem_obj);
	ret = ioctl(con->fd, IOCTL_CATS_CAN_stop, &dev);
	sem_post(&con->sem_obj);
	return ret;
}


//-----------------------------------------------------------------//
/*!
	@brief	CAN スリープ
	@param[in]	con デバイス・コンテキスト
	@param[in]	device デバイス
	@return		正常なら「０」を返す
*/
//-----------------------------------------------------------------//
int CAN_sleep(CAN_CONTEXT *con, int device)
{
	int		dev = device;
	int		ret;
	sem_wait(&con->sem_obj);
	ret = ioctl(con->fd, IOCTL_CATS_CAN_sleep, &dev);
	sem_post(&con->sem_obj);
	return ret;
}


//-----------------------------------------------------------------//
/*!
	@brief	CAN ログ取得
	@param[in]	con デバイス・コンテキスト
	@param[in]	device デバイス
	@param[in]	log	   ログ構造体
	@return		正常なら「０」を返す
*/
//-----------------------------------------------------------------//
int CAN_log(CAN_CONTEXT *con, int device, CAN_LOG *log)
{
	CAN_INTOP	iop;
	int			ret;

	sem_wait(&con->sem_obj);
	iop.device = device;
	ret = ioctl(con->fd, IOCTL_CATS_CAN_intop, &iop);
	if(ret == 0) {
		memcpy(log, &iop.log, sizeof(CAN_LOG));
	}
	sem_post(&con->sem_obj);
	return ret;
}


//-----------------------------------------------------------------//
/*!
	@brief	CAN センド（割り込みによる送信）
			送信バイト数が「０」の場合、リモートフレームの送信となる
	@param[in]	con デバイス・コンテキスト
	@param[in]	device デバイス
	@param[in]	frame 構造体ポインター
	@return		正常なら「０」を返す
*/
//-----------------------------------------------------------------//
int CAN_send_device(CAN_CONTEXT *con, int device, const CAN_FRAME *frame)
{
	int			ret;
	CAN_SEND	send;

	sem_wait(&con->sem_obj);
	send.device = device;
	memcpy(&send.frame, frame, sizeof(CAN_FRAME));
	ret = ioctl(con->fd, IOCTL_CATS_CAN_send_buff, &send);

	if( ret == 0)
	{											/* 成功 */
		ret = CASTCAN_SUCCEEDED;
//		printf("CAN_send_device:CASTCAN_SUCCEEDED ");

	}else if( ( ret == -1 )&&( errno == ENXIO ) )
	{											/* デバイス指定が範囲外 */
		ret = CATSCAN_INVALID_DEVICENUM;
//		printf("CAN_send_device:CATSCAN_INVALID_DEVICENUM ");

	}else if( ( ret == -1 )&&( errno == EOVERFLOW ) )
	{											/* 送信バッファが満杯 */
		ret = CATSCAN_SEND_BUFFER_OVERFLOW;
//		printf("CAN_send_device:CATSCAN_SEND_BUFFER_OVERFLOW ");
	}

	sem_post(&con->sem_obj);
	return ret;
}



//-----------------------------------------------------------------//
/*!
	@brief	CAN レシーブ（割り込みによる受信）
	@param[in]	con デバイス・コンテキスト
	@param[in]	device デバイス
	@param[in]	frame 構造体ポインター
	@return		レシーブデータがあれば、０以外
*/
//-----------------------------------------------------------------//
int CAN_recv_device(CAN_CONTEXT *con, int device, CAN_FRAME *frame)
{
	int			ret;
	CAN_RECV	recv;

	sem_wait(&con->sem_obj);
	recv.device = device;
	memset(&recv.frame, 0, sizeof(CAN_FRAME));
	ret = ioctl(con->fd, IOCTL_CATS_CAN_recv_buff, &recv);
	if(ret == 0) {
		memcpy(frame, &recv.frame, sizeof(CAN_FRAME));
		sem_post(&con->sem_obj);
		return 1;
   	} else {
		sem_post(&con->sem_obj);
		return 0;
	}
}


//-----------------------------------------------------------------//
/*!
	@brief	CAN ステータスを受け取る
	@param[in]	con デバイス・コンテキスト
	@param[in]	device デバイス
	@param[in]	status 構造体ポインター
	@return		正常なら「０」を返す
*/
//-----------------------------------------------------------------//
int CAN_status(CAN_CONTEXT *con, int device, CAN_STATUS *status)
{
	int		ret;
	CAN_SCAN	scan;

	sem_wait(&con->sem_obj);
	scan.device = device;
	memset(&scan.status, 0, sizeof(CAN_STATUS));
	ret = ioctl(con->fd, IOCTL_CATS_CAN_scan, &scan);
	if(ret == 0) {
		memcpy(status, &scan.status, sizeof(CAN_STATUS));
	}	
	sem_post(&con->sem_obj);
	return ret;
}


//-----------------------------------------------------------------//
/*!
	@brief	CAN 終了処理
	@param[in]	con デバイス・コンテキスト
	@param[in]	device デバイス
	@return		正常なら「０」を返す
*/
//-----------------------------------------------------------------//
int CAN_exit(CAN_CONTEXT *con, int device)
{
	int		dev = device;
	int		ret;

	sem_wait(&con->sem_obj);
	ret = ioctl(con->fd, IOCTL_CATS_CAN_exit, &dev);
	sem_post(&con->sem_obj);
	return ret;
}


//-----------------------------------------------------------------//
/*!
	@brief	CAN ドライバー、コンテキストの廃棄
	@param[in]	con デバイス・コンテキスト
*/
//-----------------------------------------------------------------//
void CAN_destroy(CAN_CONTEXT *con)
{
	if(con == NULL) return;

	close(con->fd);

	sem_destroy(&con->sem_obj);

	free(con);
}


static const char *type_keys[] = {
	  "NULL",
	  "INTI",
	  "INTO",
	  "SEND",
	  "RECV",
	  "BSOF",
	  "LOST",
	  "PSIV",
	  "WKUP",
	  "OVRN",
	  "ERRR"
};
static void log_dump_sub(FILE *out, int dno, CAN_LOG *log)
{
	int	i, pos, limit;
	unsigned short	op, val;

	if(log->count > LOG_LIMIT) limit = LOG_LIMIT;
	else limit = log->count;
	pos = log->pos - limit;
	if(pos < 0) pos += LOG_LIMIT;

	fprintf(out, "Device: %d, Total log: %d\n", dno, log->count);
	for(i = 0; i < limit; ++i) {
		op  = (log->buff[pos] & LOG_TYPE_MASK) >> 12;
		val = log->buff[pos] & LOG_VALUE_MASK;
		fprintf(out, " (%d)LOG-%d: '%s': 0x%03X (%d)\n",
			i, dno, type_keys[op], val, val);
		pos++;
		if(pos >= LOG_LIMIT) pos = 0;
	}
}


//-----------------------------------------------------------------//
/*!
	@brief	CAN 割り込みログ・ダンプ
	@param[in]	con デバイス・コンテキスト
	@param[in]	device デバイス
	@param[in]	output	ダンプ出力先のストリームポインター
*/
//-----------------------------------------------------------------//
void CAN_log_dump(CAN_CONTEXT *con, int device, FILE *output)
{
	CAN_log(con, device, &con->log[device]);
	log_dump_sub(output, device, &con->log[device]);
}

/* ----- End Of File ----- */
