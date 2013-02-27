/**********************************************************************************
 * ファイル名      : pwm_api.h
 * システム名      : x86 プロトタイプＥＣＵ OPE-RA Ver2.0
 * サブシステム名  : PWM API
 * プログラム名    : ヘッダー
 * CPU TYPE        : 
 * バージョン      : Ver1.00
 * --------------------------------------------------------------------------------
 * 作成者          : 
 * 作成部署        : 
 * 作成日付        : 2009年10月30日 新規作成
 * 更新履歴        : 
 **********************************************************************************/
#ifndef	_CATS_PWM_API_H_
#define _CATS_PWM_API_H_

#define API_OK 0
#define API_NG 1
#define API_BUSY 2

//------------------------------------------------------------------------------
// 	機能			:		FncInitPWM
//	引数 			:		unsigned int *(pwm initialization data)
//	戻り値			:		int(0-OK)
int FncInitPWM ( unsigned int * );

//------------------------------------------------------------------------------
// 	機能			:		FncPWMEnable
//	引数 			:		unsigned int *(enable/disable array)
//	戻り値			:		int(0-OK)
int FncPWMEnable ( unsigned int * );

//------------------------------------------------------------------------------
// 	機能			:		FncPWMSet
//	引数 			:		unsigned int - チャネル
//							unsigned int - duty cycle
//	戻り値			:		int(0-OK)
int FncPWMSet ( unsigned int , unsigned int  );


#endif // _CATS_PWM_API_H_
