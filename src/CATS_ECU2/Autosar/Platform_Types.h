/* Platform_Types.h


  環境依存部の定義

*/


/* Windows VisualStudioの場合 */
#ifdef WIN32

#pragma warning (disable : 4996) /* libcの安全性 */
#pragma warning (disable : 4214) /* 非標準の拡張 */
#pragma warning (disable : 4100) /* 未使用の変数 */

#define usleep(x) Sleep(x/1000)

#ifdef _DEBUG
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include "windows.h"
#define DebugOut(x) { OutputDebugString(x); }
#define DebugOut1(x, p1) { char buf[4096]; sprintf(buf, x, p1); OutputDebugString(buf); }
#define DebugOut2(x, p1, p2) { char buf[4096]; sprintf(buf, x, p1, p2); OutputDebugString(buf); }
#define DebugOut3(x, p1, p2, p3) { char buf[4096]; sprintf(buf, x, p1, p2, p3); OutputDebugString(buf); }
#define DebugOut4(x, p1, p2, p3, p4) { char buf[4096]; sprintf(buf, x, p1, p2, p3, p4); OutputDebugString(buf); }

#else

#define DebugOut __noop
#define DebugOut1 __noop
#define DebugOut2 __noop
#define DebugOut3 __noop
#define DebugOut4 __noop

#endif
#endif

/* gccの場合 */
#ifdef __GNUC__
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <memory.h>

#ifdef _DEBUG

#define DebugOut(x) { printf(x); }
#define DebugOut1(x, p1) { printf(x, p1); }
#define DebugOut2(x, p1, p2) { printf(x, p1, p2); }
#define DebugOut3(x, p1, p2, p3) { printf(x, p1, p2, p3); }
#define DebugOut4(x, p1, p2, p3, p4) { printf(x, p1, p2, p3, p4); }

#else

#define DebugOut 0&&
#define DebugOut1 0&&
#define DebugOut2 0&&
#define DebugOut3 0&&
#define DebugOut4 0&&

#endif
#endif
