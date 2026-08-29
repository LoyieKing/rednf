// df_game_r 还原 —— CLog（全局日志单例）。
// 布局依据 ORIG C1 0x0854e558 / D1 0x0854e69c / initiate 0x0854e88c /
// log* 系列报告（docs/class_func_reports/CLog/），sizeof 0x358
// （createInstance new(0x358) 实证）：
//   +0x000..+0x0ef  10×Mutex（0x18/个，C1 循环构造）
//   +0x0f0..+0x10f  8×FILE* 句柄（.log/.error/.debug/.money/.history/.cri/.init/.snap）
//   +0x110..+0x20f  m_initFile[0x100]（initiate strcpy 目录路径）
//   +0x210..+0x24f  8×TimeLog_（initiate 逐个 setTime，与句柄一一对应）
//   +0x250         m_flag（initiate 第 3 参）
//   +0x254         m_initFlag（initiate 置 1）
//   +0x258..+0x357 m_logFile[0x100]（initiate 日志名前缀，缺省 "Log"）
#ifndef DNF_GAME_CLOG_H_
#define DNF_GAME_CLOG_H_

#include <cstdio>

#include "GameTypes.h"  // Mutex
#include "TimeLog_.h"   // TimeLog_

class CLog
{
public:
    static CLog* instance();          // ORIG 0x0817dfec（纯 getter，10 字节）
    static void destroyInstance();    // ORIG W 0x82a3cfd
    static void createInstance();     // ORIG W 0x82a3cb3（new(0x358)）
    void initiate(char* name, char* path, int flag);  // ORIG T 0x0854e88c
    unsigned char logNotice(const char* fmt, ...);    // ORIG T 0x0854f162
    unsigned char logError(const char* fmt, ...);     // ORIG T 0x0854f1c2
    int logDebug(const char* fmt, ...);               // ORIG T 0x0854f40a（return 1）
    unsigned char logConsole(const char* fmt, ...);   // ORIG T 0x0854f414（bool 语义）
    unsigned char logMoney(const char* fmt, ...);     // ORIG T 0x0854f282
    unsigned char logCritical(const char* fmt, ...);  // ORIG T 0x0854f222
    unsigned char logSnapShot(const char* fmt, ...);  // ORIG T 0x0854f4c0
    unsigned char logInit(bool bForceOpen, bool bAppend,
                          const char* fmt, ...);      // ORIG T 0x0854f520
    int logHistory(const char* fmt, char* vaArgs);    // ORIG T 0x0854f2e2

private:
    CLog();                          // ORIG T 0x0854e558
    ~CLog();                         // ORIG T 0x0854e69c

    static CLog* this_;              // ORIG 单例指针 _ZN4CLog5this_E @0x0940bd60

    // ORIG T 0x0854ec70：按日期拼文件名开日志；fopen 失败回退 stdout。
    void openLogFile(TimeLog_ t, FILE*& out, char* ext, int flag);
    // ORIG T 0x0854f594：跨日/跨小时（flag>0）重开文件；恒返回 true。
    // bool——调用侧（logToFile 0x854f020/logHistory 0x854f377）以 xor $0x1 取反（P22）。
    bool checkDay(TimeLog_ t, TimeLog_* last, FILE*& out, char* ext, int flag);
    // ORIG T 0x0854edb0：vsprintf + 可选时间戳前缀 + fprintf/printf + fflush。
    int logToFile(FILE*& out, TimeLog_& last, bool bPrint, char* ext,
                  char const* fmt, char* va, bool bTimestamp);
    // ORIG T 0x0854ef56：init 专用（memset 缓冲 + 格式串变量化）。
    int logToInitFile(FILE*& out, TimeLog_& last, char* ext,
                      char const* fmt, char* va, bool bPrint, bool bTimestamp);
    // ORIG T 0x0854f692：time()+localtime_r 填充 TimeLog_。
    void getTime(TimeLog_* t);

public:
    Mutex m_mutexes[10];             // +0x000（0x18/个）
    FILE* m_logFileNotice;           // +0x0f0（".log"）
    FILE* m_logFileError;            // +0x0f4（".error"）
    FILE* m_logFileDebug;            // +0x0f8（".debug"）
    FILE* m_logFileMoney;            // +0x0fc（".money"）
    FILE* m_logFileHistory;          // +0x100（".history"）
    FILE* m_logFileCri;              // +0x104（".cri"）
    FILE* m_logFileInit;             // +0x108（".init"）
    FILE* m_logFileSnap;             // +0x10c（".snap"）
    char m_initFile[0x100];          // +0x110
    TimeLog_ m_logTime[8];           // +0x210（8×8）
    int m_flag;                      // +0x250
    int m_initFlag;                  // +0x254
    char m_logFile[0x100];           // +0x258
};                                   // sizeof == 0x358

#endif
