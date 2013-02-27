/**********************************************************************************
 * ファイル名      : cats_sja1000.h
 * システム名      : x86 プロトタイプＥＣＵ OPE-RA Ver2.0
 * サブシステム名  : カーネルドライバ
 * プログラム名    : SAJ1000 制御関数
 * バージョン      : Ver1.00
 * --------------------------------------------------------------------------------
 * 作成者          : 
 * 作成部署        : 
 * 作成日付        : 2009年09月23日 新規作成
 * 更新履歴        : 
 **********************************************************************************/
#ifndef CATS_SJA1000_H
#define CATS_SJA1000_H

#define LOG_LIMIT       256

#define LOG_TYPE_MASK   0xf000
#define LOG_VALUE_MASK  0x0fff

/// エラーコード
#define CASTCAN_SUCCEEDED              0  //! 成功
#define CATSCAN_INVALID_DEVICENUM     -1  //! デバイス番号が不正
#define CATSCAN_SEND_BUFFER_OVERFLOW  -2  //! 送信バッファがオーバーフロー
#define CATSCAN_SEND_BUFFER_TOO_BIG   -2  //! 送信バッファサイズが大きすぎる
#define CATSCAN_RECV_BUFFER_TOO_BIG   -3  //! 受信バッファサイズが大きすぎる


/// 割り込みログの種類
enum {
     LOG_TYPE_NULL = 0x0000,
     LOG_TYPE_INTI = 0x1000,    // 割り込み (in)
     LOG_TYPE_INTO = 0x2000,    // 割り込み (out)
     LOG_TYPE_SEND = 0x3000,    // 送信完了割り込み
     LOG_TYPE_RECV = 0x4000,    // 受信完了割り込み
     LOG_TYPE_BUSOFF = 0x5000,  // BUSOFF 割り込み
     LOG_TYPE_LOST = 0x6000,    // Arbitr lost
     LOG_TYPE_PASSIVE = 0x7000, // Passive エラー
     LOG_TYPE_WAKEUP = 0x8000,  // Wakeup 割り込み
     LOG_TYPE_OVERRUN = 0xA000, // Overrun エラー
     LOG_TYPE_ERROR = 0xB000,   // エラー    
};


/// SJA1000 レジスター PeriCAN mode..
#define CAN_ADR_MODE            0
#define CAN_ADR_COMMAND         1
#define CAN_ADR_STATUS          2
#define CAN_ADR_INTERRUPT       3
#define CAN_ADR_INTERRUPT_ENABLE    4

#define CAN_ADR_BUS_TIMING_0    6
#define CAN_ADR_BUS_TIMING_1    7
#define CAN_ADR_OUTPUT_CONTROL  8

#define CAN_ADR_ARBITRATION_LOST_CAPTURE 11     //for dummy read
#define CAN_ADR_ERROR_CODE_CAPTURE 12
#define CAN_ADR_ERROR_WARNING_LIMIT 13
#define CAN_ADR_RX_ERROR_COUNT  14
#define CAN_ADR_TX_ERROR_COUNT  15


// Read register by...
#define CAN_ADR_TX_FRAME_INFO   16
#define CAN_ADR_TX_IDENTIFIRE1  17
#define CAN_ADR_TX_IDENTIFIRE2  18
#define CAN_ADR_TX_STD_DATA1    19
#define CAN_ADR_TX_STD_DATA2    20
#define CAN_ADR_TX_STD_DATA3    21
#define CAN_ADR_TX_STD_DATA4    22
#define CAN_ADR_TX_STD_DATA5    23
#define CAN_ADR_TX_STD_DATA6    24
#define CAN_ADR_TX_STD_DATA7    25
#define CAN_ADR_TX_STD_DATA8    26

#define CAN_ADR_TX_IDENTIFIRE3  19
#define CAN_ADR_TX_IDENTIFIRE4  20
#define CAN_ADR_TX_EXT_DATA1    21
#define CAN_ADR_TX_EXT_DATA2    22
#define CAN_ADR_TX_EXT_DATA3    23
#define CAN_ADR_TX_EXT_DATA4    24
#define CAN_ADR_TX_EXT_DATA5    25
#define CAN_ADR_TX_EXT_DATA6    26
#define CAN_ADR_TX_EXT_DATA7    27
#define CAN_ADR_TX_EXT_DATA8    28

// Write register by ...
#define CAN_ADR_RX_FRAME_INFO   16
#define CAN_ADR_RX_IDENTIFIRE1  17
#define CAN_ADR_RX_IDENTIFIRE2  18
#define CAN_ADR_RX_STD_DATA1    19
#define CAN_ADR_RX_STD_DATA2    20
#define CAN_ADR_RX_STD_DATA3    21
#define CAN_ADR_RX_STD_DATA4    22
#define CAN_ADR_RX_STD_DATA5    23
#define CAN_ADR_RX_STD_DATA6    24
#define CAN_ADR_RX_STD_DATA7    25
#define CAN_ADR_RX_STD_DATA8    26

#define CAN_ADR_RX_IDENTIFIRE3  19
#define CAN_ADR_RX_IDENTIFIRE4  20
#define CAN_ADR_RX_EXT_DATA1    21
#define CAN_ADR_RX_EXT_DATA2    22
#define CAN_ADR_RX_EXT_DATA3    23
#define CAN_ADR_RX_EXT_DATA4    24
#define CAN_ADR_RX_EXT_DATA5    25
#define CAN_ADR_RX_EXT_DATA6    26
#define CAN_ADR_RX_EXT_DATA7    27
#define CAN_ADR_RX_EXT_DATA8    28

#define CAN_ADR_RX_MESSAGE_COUNTER      29
#define CAN_ADR_RX_BUFFER_START_ADR     30

#define CAN_ADR_CLOCK_DIVIDER   31

// Acceptance code register (By RESET)
#define CAN_ADR_ACR0            16
#define CAN_ADR_ACR1            17
#define CAN_ADR_ACR2            18
#define CAN_ADR_ACR3            19

// Acceptance mask (By RESET)
#define CAN_ADR_AMR0            20
#define CAN_ADR_AMR1            21
#define CAN_ADR_AMR2            22
#define CAN_ADR_AMR3            23

// FIFO (internal RAM) at 64 Bytes
#define CAN_ADR_FIFO_TOP        64

// TX Buffer (internal RAM) at 15 Bytes
#define CAN_ADR_TX_BUFFER_TOP   96

// SJA1000 の発信周波数設定
// クリスタルの発信周波数の半分
//#define CAN_SYSCLK    8
#define CAN_SYSCLK  12

/*--- Mode Register -------- PeliCAN -------------------*/

#  define CAN_SLEEP_MODE        0x10    /* Sleep Mode */
#  define CAN_ACC_FILT_MASK     0x08    /* Acceptance Filter Mask */
#  define CAN_SELF_TEST_MODE    0x04    /* Self test mode */
#  define CAN_LISTEN_ONLY_MODE  0x02    /* Listen only mode */
#  define CAN_RESET_REQUEST     0x01    /* reset mode */

/*--- Interrupt enable Reg -----------------------------*/
#define CAN_ERROR_BUSOFF_INT_ENABLE     (1<<7)
#define CAN_ARBITR_LOST_INT_ENABLE      (1<<6)
#define CAN_ERROR_PASSIVE_INT_ENABLE    (1<<5)
#define CAN_WAKEUP_INT_ENABLE           (1<<4)
#define CAN_OVERRUN_INT_ENABLE          (1<<3)
#define CAN_ERROR_INT_ENABLE            (1<<2)
#define CAN_TRANSMIT_INT_ENABLE         (1<<1)
#define CAN_RECEIVE_INT_ENABLE          (1<<0)

/*--- Frame information register -----------------------*/
#define CAN_EFF             0x80    // extended frame

/*--- Remote Request ---------------------------------*/
/*    Notes:
 *    Basic CAN: RTR is Bit 4 in TXDES1.
 *    Peli  CAN: RTR is Bit 6 in frameinfo.
 */
#define CAN_RTR             0x40    // remoto frame

#define CAN_SFF             0x00    // standard fame format


/*--- Command Register ------------------------------------*/
 
#define CAN_GOTO_SLEEP                  (1<<4)
#define CAN_CLEAR_OVERRUN_STATUS        (1<<3)
#define CAN_RELEASE_RECEIVE_BUFFER      (1<<2)
#define CAN_ABORT_TRANSMISSION          (1<<1)
#define CAN_TRANSMISSION_REQUEST        (1<<0)

/*--- Status Register --------------------------------*/
 
#define CAN_BUS_STATUS                      (1<<7)
#define CAN_ERROR_STATUS                    (1<<6)
#define CAN_TRANSMIT_STATUS                 (1<<5)
#define CAN_RECEIVE_STATUS                  (1<<4)
#define CAN_TRANSMISSION_COMPLETE_STATUS    (1<<3)
#define CAN_TRANSMIT_BUFFER_ACCESS          (1<<2)
#define CAN_DATA_OVERRUN                    (1<<1)
#define CAN_RECEIVE_BUFFER_STATUS           (1<<0)

/*--- Interrupt Register -----------------------------------*/
 
#define CAN_WAKEUP_INT                  (1<<4)
#define CAN_OVERRUN_INT                 (1<<3)
#define CAN_ERROR_INT                   (1<<2)
#define CAN_TRANSMIT_INT                (1<<1)
#define CAN_RECEIVE_INT                 (1<<0)

/*--- Output Control Register -----------------------------------------*/
/*
 *  7   6   5   4   3   2   1   0
 *  OCTP1   OCTN1   OCPOL1  OCTP0   OCTN0   OCPOL0  OCMODE1 OCMODE0
 *  ----------------------  ----------------------  ---------------
 *      TX1 Output          TX0 Output        programmable
 *    Driver Control      Driver Control      output functions
 *
 *  MODE
 *  OCMODE1 OCMODE0
 *    0   1 Normal Mode; TX0, TX1 bit sequenze TXData
 *    1   1 Normal Mode; TX0 bit sequenze, TX1 busclock TXCLK
 *    0   0 Biphase Mode
 *    1   0 Test Mode; TX0 bit sequenze, TX1 COMPOUT
 *
 *  In normal Mode Voltage Output Levels depend on 
 *  Driver Characteristic: OCTPx, OCTNx
 *  and programmed Output Polarity: OCPOLx
 *
 *  Driver Characteristic
 *  OCTPx   OCTNx
 *    0  0  always Floating Outputs,
 *    0  1  Pull Down
 *    1  0  Pull Up
 *    1  1  Push Pull
 */
 
/*--- Output control register --------------------------------*/

#define CAN_OCTP1           (1<<7)
#define CAN_OCTN1           (1<<6)
#define CAN_OCPOL1          (1<<5)
#define CAN_OCTP0           (1<<4)
#define CAN_OCTN0           (1<<3)
#define CAN_OCPOL0          (1<<2)
#define CAN_OCMODE1         (1<<1)
#define CAN_OCMODE0         (1<<0)

/*--- Clock Divider Register ---------------------------------*/
#define CAN_CDR_PELICAN     (0x80)
#define CAN_CDR_CLK_OFF     (0x08)
#define CAN_CDR_CLK         (0x07)      /* CLK-out = Fclk   */
#define CAN_CDR_CBP         (0x40)      /* RX0: active, RX1: unused */ 

/* the base address register array */
/* extern unsigned int Base[]; */

/*---------- Timing values */
/* generated bit rate table by
 * http://www.port.de/engl/canprod/sv_req_form.html
 */

#if CAN_SYSCLK == 8
/* these timings are valid for clock 8MHz */
#  define CAN_TIM0_10K        49
#  define CAN_TIM1_10K      0x1c
#  define CAN_TIM0_20K        24    
#  define CAN_TIM1_20K      0x1c
#  define CAN_TIM0_40K      0x89    /* Old Bit Timing Standard of port */
#  define CAN_TIM1_40K      0xEB    /* Old Bit Timing Standard of port */
#  define CAN_TIM0_50K         9
#  define CAN_TIM1_50K      0x1c
#  define CAN_TIM0_100K        4    /* sp 87%, 16 abtastungen, sjw 1 */
#  define CAN_TIM1_100K     0x1c
#  define CAN_TIM0_125K        3
#  define CAN_TIM1_125K     0x1c
#  define CAN_TIM0_250K        1
#  define CAN_TIM1_250K     0x1c
#  define CAN_TIM0_500K        0
#  define CAN_TIM1_500K     0x1c
#  define CAN_TIM0_800K        0
#  define CAN_TIM1_800K     0x16
#  define CAN_TIM0_1000K       0
#  define CAN_TIM1_1000K    0x14

#define CAN_SYSCLK_is_ok            1
#endif


#if CAN_SYSCLK == 10
/* these timings are valid for clock 10MHz */
/* 20 Mhz cristal */
#  define CAN_TIM0_10K      0x31
#  define CAN_TIM1_10K      0x2f
#  define CAN_TIM0_20K      0x18
#  define CAN_TIM1_20K      0x2f
#  define CAN_TIM0_50K      0x18
#  define CAN_TIM1_50K      0x05
#  define CAN_TIM0_100K     0x04
#  define CAN_TIM1_100K     0x2f
#  define CAN_TIM0_125K     0x04
#  define CAN_TIM1_125K     0x1c
#  define CAN_TIM0_250K     0x04
#  define CAN_TIM1_250K     0x05
#  define CAN_TIM0_500K     0x00
#  define CAN_TIM1_500K     0x2f
#  define CAN_TIM0_800K     0x00
#  define CAN_TIM1_800K     0x00
#  define CAN_TIM0_1000K    0x00
#  define CAN_TIM1_1000K    0x07

#define CAN_SYSCLK_is_ok            1
#endif

#if CAN_SYSCLK == 12
/* these timings are valid for clock 12MHz */
/* 24 Mhz cristal */
#  define CAN_TIM0_10K      0x31
#  define CAN_TIM1_10K      0x6f
#  define CAN_TIM0_20K      0x18
#  define CAN_TIM1_20K      0x6f
#  define CAN_TIM0_50K      0x0e
#  define CAN_TIM1_50K      0x1c
#  define CAN_TIM0_100K     0x05
#  define CAN_TIM1_100K     0x2f
#  define CAN_TIM0_125K     0x05
#  define CAN_TIM1_125K     0x1c
#  define CAN_TIM0_250K     0x02
#  define CAN_TIM1_250K     0x1c
#  define CAN_TIM0_500K     0x00
#  define CAN_TIM1_500K     0x6f
#  define CAN_TIM0_800K     0x00
#  define CAN_TIM1_800K     0x1b
#  define CAN_TIM0_1000K    0x00
#  define CAN_TIM1_1000K    0x09

#define CAN_SYSCLK_is_ok            1
#endif

#ifndef CAN_SYSCLK_is_ok
#  error Please specify a valid CAN_SYSCLK value (i.e. 8, 10) or define new parameters
#endif


//-----------------------------------------------------------------//
/*!
    @brief  CAN register 構造体
*/
//-----------------------------------------------------------------//
typedef struct {
    int    device;
    int    address;
    int    data;
} CAN_REGISTER;


//-----------------------------------------------------------------//
/*!
    @brief  CAN モード構造体@n
            baud_rate: 10, 20, 50, 100, 125, 250, 500, 800, 1000 [Kbits/sed]@n
            baud_rate が「0」の場合には、BTR0、BTR1 の値が直接使われる。
*/
//-----------------------------------------------------------------//
typedef struct {
    int        baud_rate;               ///< ボーレート
    unsigned char BTR0;                 ///< BTR0 レジスター直接値
    unsigned char BTR1;                 ///< BTR1 レジスター直接値
    unsigned char accept_code[4];       ///< アクセプト・フィルター
    unsigned char accept_mask[4];       ///< アクセプト・フィルター・マスク
} CAN_MODE;


//-----------------------------------------------------------------//
/*!
    @brief  CAN ステータス構造体
*/
//-----------------------------------------------------------------//
typedef struct {
    size_t      recv_count;
    size_t      send_count;

    size_t      interrupt_count;
    size_t      busoff_error_count;
    size_t      arbitr_lost_count;
    size_t      passive_error_count;
    size_t      wakeup_count;
    size_t      overrun_error_count;
    size_t      error_count;

    size_t      recv_buff_overrun_count;
    size_t      send_buff_overrun_count;

    int         recv_buff_count;
    int         send_buff_count;

    unsigned char   rx_error_count;     ///< RX エラー・カウント
    unsigned char   tx_error_count;     ///< TX エラー・カウント
    unsigned char   rx_message_count;   ///< RX メッセージ・カウント
    char        tmep;
} CAN_STATUS;


//-----------------------------------------------------------------//
/*!
    @brief  CAN フレーム構造体
*/
//-----------------------------------------------------------------//
typedef struct {
    unsigned int    id;         ///< Identifier (standard frame: 11 Bits、extended frame: 29 Bits)
    unsigned char   dlc;        ///< Data Length Code
    unsigned char   length;     ///< データ長
    unsigned char   type;       ///< フレームタイプ（0: standard frame、1: extended frame)
    unsigned char   data[8];
    short    frame_id;
} CAN_FRAME;


//-----------------------------------------------------------------//
/*!
    @brief  CAN init 構造体
*/
//-----------------------------------------------------------------//
typedef struct {
    int    device;
    int    recv_buff_size;
    int    send_buff_size;
} CAN_INIT;


//-----------------------------------------------------------------//
/*!
    @brief  CAN reset 構造体
*/
//-----------------------------------------------------------------//
typedef struct {
    int         device;
    CAN_MODE    mode;
} CAN_RESET;


//-----------------------------------------------------------------//
/*!
    @brief  CAN scan 構造体
*/
//-----------------------------------------------------------------//
typedef struct {
    int    device;
    CAN_STATUS      status;
} CAN_SCAN;


//-----------------------------------------------------------------//
/*!
    @brief  CAN send 構造体
*/
//-----------------------------------------------------------------//
typedef struct {
    int    device;
    CAN_FRAME frame;
} CAN_SEND;


//-----------------------------------------------------------------//
/*!
    @brief  CAN recv 構造体
*/
//-----------------------------------------------------------------//
typedef struct {
    int    device;
    CAN_FRAME frame;
} CAN_RECV;


//-----------------------------------------------------------------//
/*!
    @brief  CAN ログ構造体
*/
//-----------------------------------------------------------------//
typedef struct {
    size_t count;
    int    pos;
    unsigned short buff[LOG_LIMIT];
} CAN_LOG;


//-----------------------------------------------------------------//
/*!
    @brief  CAN 割り込みオペコード構造体
*/
//-----------------------------------------------------------------//
typedef struct {
    int        device;
    CAN_LOG    log;
} CAN_INTOP;


//-----------------------------------------------------------------//
/*!
    @brief  SJA1000 コンテキストの作成
*/
//-----------------------------------------------------------------//
void sja1000_create(void);


//-----------------------------------------------------------------//
/*!
    @brief  SJA1000 コンテキストの破棄
*/
//-----------------------------------------------------------------//
void sja1000_destroy(void);


//-----------------------------------------------------------------//
/*!
    @brief  SJA1000 デバイスを read
    @param[in]  memptr  PCI バスのメモリーイメージ
    @param[in]  device  デバイス番号
    @param[in]  address 値
    @return register's value
 */
//-----------------------------------------------------------------//
int sja1000_read(int *memptr, int device, int address);


//-----------------------------------------------------------------//
/*!
    @brief  SJA1000 デバイスを write.
    @param[in]  memptr  PCI バスのメモリーイメージ
    @param[in]  device  デバイス番号
    @param[in]  address 値
    @param[in]  data    書き込むデータ
    @return register's value
 */
//-----------------------------------------------------------------//
int sja1000_write(int *memptr, int device, int address, int data);


//-----------------------------------------------------------------//
/*!
    @brief  SJA1000 デバイスをテストする。@n
            装置テスト用 API
    @param[in]  memptr  PCI バスのメモリーイメージ
    @param[in]  device  デバイス番号
    @param[in]  value   値
    @return 正常なら「０」を返す。
 */
//-----------------------------------------------------------------//
int sja1000_test(int *memptr, int device, int value);


//-----------------------------------------------------------------//
/*!
    @brief  SJA1000 操作関係の初期化@n
            ワークメモリーの初期化など
    @param[in]  memptr  PCI バスのメモリーイメージ
    @param[in]  init    CAN_INIT 構造体
    @return 正常なら「０」を返す。
*/
//-----------------------------------------------------------------//
int sja1000_init(int *memptr, CAN_INIT *init);


//-----------------------------------------------------------------//
/*!
    @brief  SJA1000 デバイスをリセットする。
    @param[in]  memptr  PCI バスのメモリーイメージ
    @param[in]  device  デバイス番号
    @param[in]  mode    モード設定構造体のポインター
    @return 正常なら「０」を返す。
 */
//-----------------------------------------------------------------//
int sja1000_reset(int *memptr, int device, CAN_MODE *mode);


//-----------------------------------------------------------------//
/*!
    @brief  SJA1000 デバイスを開始する。
    @param[in]  memptr  PCI バスのメモリーイメージ
    @param[in]  device  デバイス番号
    @return 正常なら「０」を返す。
*/
//-----------------------------------------------------------------//
int sja1000_start(int *memptr, int device);


//-----------------------------------------------------------------//
/*!
    @brief  SJA1000 デバイスを停止する。
    @param[in]  memptr  PCI バスのメモリーイメージ
    @param[in]  device  デバイス番号
    @return 正常なら「０」を返す。
*/
//-----------------------------------------------------------------//
int sja1000_stop(int *memptr, int device);


//-----------------------------------------------------------------//
/*!
    @brief  SJA1000 デバイスを Sleep モードにする。
    @param[in]  memptr  PCI バスのメモリーイメージ
    @param[in]  device  デバイス番号
    @return 正常なら「０」を返す。
*/
//-----------------------------------------------------------------//
int sja1000_sleep(int *memptr, int device);


//-----------------------------------------------------------------//
/*!
    @brief  SJA1000 各種ステータスを受け取る
    @param[in]  memptr  PCI バスのメモリーイメージ
    @param[in]  device  デバイス番号
    @param[in]  status  ステータス構造体のポインター
    @return 正常なら「０」以外を返す。
*/
//-----------------------------------------------------------------//
int sja1000_status(int *memptr, int device, CAN_STATUS *status);


//-----------------------------------------------------------------//
/*!
    @brief  SJA1000 リモート・フレーム・メッセージを送信バッファ@n
            にセットして送信要求
    @param[in]  memptr  PCI バスのメモリーイメージ
    @param[in]  device  デバイス番号
    @param[in]  cf      CAN_FRAME 構造体
    @return 正常なら「０」を返す。
*/
//-----------------------------------------------------------------//
int sja1000_remoto_frame(int *memptr, int device, const CAN_FRAME *cf);


//-----------------------------------------------------------------//
/*!
    @brief  SJA1000 メッセージを送信レジスターにセットして送信要求
    @param[in]  memptr  PCI バスのメモリーイメージ
    @param[in]  device  デバイス番号
    @param[in]  cf      CAN_FRAME 構造体
    @return 正常なら「０」を返す。
*/
//-----------------------------------------------------------------//
int sja1000_send(int *memptr, int device, const CAN_FRAME *cf);


//-----------------------------------------------------------------//
/*!
    @brief  SJA1000 受信メッセージレジスターを読み込む。
            受信バッファのデータを受け取ったら、「Release Receive Buffer」
            ビットを立てる。
    @param[in]  memptr  PCI バスのメモリーイメージ
    @param[in]  device  デバイス番号
    @param[in]  cf      CAN_FRAME 構造体
    @return 正常なら「０」を返す。
*/
//-----------------------------------------------------------------//
int sja1000_recv(int *memptr, int device, CAN_FRAME *cf);


//-----------------------------------------------------------------//
/*!
    @brief  SJA1000 送信バッファメッセージを書き込む。
    @param[in]  memptr  PCI バスのメモリーイメージ
    @param[in]  send    送信構造体
    @return 正常なら「０」を返す。
*/
//-----------------------------------------------------------------//
int sja1000_send_buff(int *memptr, CAN_SEND *send);


//-----------------------------------------------------------------//
/*!
    @brief  SJA1000 受信バッファメッセージを読み込む。
    @param[in]  memptr  PCI バスのメモリーイメージ
    @param[in]  recv    受信構造体
    @return 正常なら「０」を返す。
*/
//-----------------------------------------------------------------//
int sja1000_recv_buff(int *memptr, CAN_RECV *recv);


//-----------------------------------------------------------------//
/*!
    @brief  SJA1000 の割り込みステータスを得る、@n
            ステータスをクリアする。
    @param[in]  memptr  PCI バスのメモリーイメージ
    @param[in]  device  デバイス番号
    @return     割り込みステータス・フラグ
 */
//-----------------------------------------------------------------//
int sja1000_get_interrupt_status(int *memptr, int device);


//-----------------------------------------------------------------//
/*!
    @brief  SJA1000 タスクレット処理
    @param[in]  memptr  PCI バスのメモリーイメージ
    @param[in]  device  デバイス番号
    @param[in]  status  割り込みステータス・フラグ
 */
//-----------------------------------------------------------------//
void sja1000_tasklet_core(int *memptr, int device, int status);


//-----------------------------------------------------------------//
/*!
    @brief  SJA1000 終了処理
    @param[in]  memptr  PCI バスのメモリーイメージ
    @param[in]  device  デバイス番号
    @return 正常なら「０」を返す。
 */
//-----------------------------------------------------------------//
int sja1000_exit(int *memptr, int device);


//-----------------------------------------------------------------//
/*!
    @brief  SJA1000 ログ取得
    @param[in]  memptr  PCI バスのメモリーイメージ
    @param[in]  device  デバイス番号
    @param[in]  log     ログ構造体ポインター
    @return 正常なら「０」を返す。
 */
//-----------------------------------------------------------------//
int sja1000_log(int *memptr, int device, CAN_LOG *log);

int sja1000_get_ringinfo(int *memptr, int device, int *ret);
int sja1000_deinit(int *memptr, int device);

#endif  // CATS_SJA1000_H
