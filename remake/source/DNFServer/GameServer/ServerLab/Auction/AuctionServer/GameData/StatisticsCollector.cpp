// StatisticsCollector.cpp
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

#include "StatisticsCollector.h"
#include "Script.h"
#include "TraceLog.h"
#include "Threads.h"
#include "NSLDBThread.h"
#include "WorkThread.h"
#include "ServiceFactory.h"
#include "ServiceError.h"

using namespace nsl;

const int StatisticsCollector::MAX_STATISTICS_LOG_FILE_SIZE;

// GlobalInstance<StatisticsCollector>::create() 显式特化（强符号）：
// ORIG 中该函数是弱符号，链接期胜出的是非 STATISTICS_STDATA_57 TU 的实例
// （分配 0x19a4 = [55] 布局 sizeof）。我们的链接顺序让 HandlerFor_GA_/GP_JPN.o
// （[57] 布局，分配 0x19d4）胜出，导致与 ORIG 差 0x30 字节。
// 在本 TU（无宏，sizeof=0x19a4）提供强符号特化，覆盖各 TU 的弱实例，
// 使最终二进制按 ORIG 的 0x19a4 分配。函数体与 GlobalInstance.h 内联定义一致，
// 代码生成与 ORIG 逐条一致（仅尾随对齐 nop 属 §3.3 伪影）。
namespace nsl {

template <>
void GlobalInstance<StatisticsCollector>::create()
{
    if (m_p == NULL)
    {
        if (m_p == NULL)
        {
            m_p = new StatisticsCollector;
        }
    }
    else
    {
        return;
    }
}

} // namespace nsl

extern "C" int __xstat(int ver, const char* path, struct stat* buf);

extern "C" __attribute__((weak)) int stat(const char* __path, struct stat* __statbuf)
{
    return __xstat(3, __path, __statbuf);
}

StatisticsCollector::StatisticsCollector()
{
    mpLogFile = NULL;
    mpDayFileName[0] = '\0';
    mpSecFileName[0] = '\0';
    strncpy(mpStLogDir, "statistics", 0x400);
    strncpy(mpStOldLogDir, "statistics/old_statistics", 0x400);
    for (unsigned int i = 0; i < 3; i++)
    {
        switch (i)
        {
        case 0:
            strncpy(mpStatisticsTags[i], "REGIST_ITEM  ", 0x200);
            break;
        case 1:
            strncpy(mpStatisticsTags[i], "REGIST_CANCEL", 0x200);
            break;
        case 2:
            strncpy(mpStatisticsTags[i], "BIDDING      ", 0x200);
            break;
        }
    }
    strncpy(LINE_STR,
            "----------------------------------------------------------------------------\n", 0x200);
    LINE_LEN = strlen(LINE_STR);
    DataInitialization(true);
    DataInitialization(false);
    SetTimeToNow();
    mLastLoggingDay = GetDay();
}

StatisticsCollector::~StatisticsCollector()
{
    if (mpLogFile != NULL)
    {
        fclose(mpLogFile);
    }
}

// 2026-08-11 复检修正：DataInitialization 移到 StData::reset/toString/
// SetLogFileName 之前——GCC 4.4 -O0 的栈槽分配受 TU 内前置函数的伪寄存器
// 编号影响；原顺序（reset/toString/SetLogFileName 在前）使本函数槽位为
// -0x14/-0x8/-0x4（帧 0x18），与 ORIG 的 -0x1c/-0x10/-0xc（帧 0x38）不符。
void StatisticsCollector::DataInitialization(bool dayDateInit)
{
    if (dayDateInit)
    {
        for (unsigned int i = 0; i < 3; i++)
        {
            mStDataPerDay[i].reset();
        }
    }
    else
    {
        for (unsigned int i = 0; i < 3; i++)
        {
            mStDataPerSec[i].reset();
        }
        // 原版 else 路径落在 leave 前有 1 字节 nop（day 路径 jmp 直接到 leave）
        __asm__ __volatile__("nop");
    }
}

void StatisticsCollector::StData::reset()
{
    tryCnt = 0;
    failCnt = 0;
    for (unsigned int i = 0; i < 0x37; i++)
    {
        becauseCnt[i] = 0;
    }
}

void StatisticsCollector::StData::toString(char* buf, int len)
{
    sprintf(buf, "%8d Try, %8d Fail", tryCnt, failCnt);
    if (failCnt != 0)
    {
        // 原版：memcpy(buf+strlen(buf), "\n", 2)（含 NUL；rodata 紧挨 format 串）
        // __builtin_strcat 在 4.4 -O0 降为该 memcpy 形态（先铺 n/src 再 strlen）
        __builtin_strcat(buf, "\n");
        char tmp[256];
        for (unsigned int i = 0; i < 0x37; i++)
        {
            if (becauseCnt[i] != 0)
            {
                sprintf(tmp, "                  %s # %d\n", nsl::GetErrorStr(i), becauseCnt[i]);
                strncat(buf, tmp, len);
            }
        }
    }
}

bool StatisticsCollector::SetLogFileName(const char* logDir, int serviceId)
{
    mServiceId = serviceId;
    sprintf(mpDayFileName, "%s/%s/DaySt_%d.log", logDir, mpStLogDir, serviceId);
    sprintf(mpSecFileName, "%s/%s/SecSt_%d.log", logDir, mpStLogDir, serviceId);
    return makeLogDir();
}

void StatisticsCollector::SetTimeToNow()
{
    time(&mNowTimeT);
    mpNowTm = localtime(&mNowTimeT);
}

bool StatisticsCollector::makeLogDir()
{
    char logdname[1024];
    struct stat st;
    sprintf(logdname, "%s/%s", G_Script()->findCharValue(0, 1), mpStLogDir);
    if (stat(logdname, &st) < 0)
    {
        if (errno != ENOENT)
        {
            G_TraceLog()->sysLog(7, "StatisticsCollector::makeLogDir() stat() errmsg[%s(%d)] %s", strerror(errno), errno, logdname);
            return false;
        }
        if (G_TraceLog()->MakeDir(logdname) < 0)
        {
            G_TraceLog()->sysLog(7, "StatisticsCollector::makeLogDir() mkdir() errmsg[%s(%d)] %s", strerror(errno), errno, logdname);
            return false;
        }
    }
    sprintf(logdname, "%s/%s", G_Script()->findCharValue(0, 1), mpStOldLogDir);
    if (stat(logdname, &st) < 0)
    {
        if (errno != ENOENT)
        {
            G_TraceLog()->sysLog(7, "StatisticsCollector::makeLogDir() stat() errmsg[%s(%d)] %s", strerror(errno), errno, logdname);
            return false;
        }
        if (G_TraceLog()->MakeDir(logdname) < 0)
        {
            G_TraceLog()->sysLog(7, "StatisticsCollector::makeLogDir() mkdir() errmsg[%s(%d)] %s", strerror(errno), errno, logdname);
            return false;
        }
    }
    return true;
}

bool StatisticsCollector::backupLogFile(bool isDayLog)
{
    if (makeLogDir())
    {
        char renfname[1024];
        if (isDayLog)
        {
            sprintf(renfname, "%s/%s/DaySt_%d#%02d%02d%02d.log", G_Script()->findCharValue(0, 1), mpStOldLogDir, mServiceId, GetYear(), GetMonth(), GetDay());
            if (rename(mpDayFileName, renfname) < 0)
            {
                G_TraceLog()->sysLog(7, "backupLogFile rename() errmsg[%s(%d)] %s", strerror(errno), errno, renfname);
                return false;
            }
        }
        else
        {
            sprintf(renfname, "%s/%s/SecSt_%d#%02d%02d%02d-%02d%02d.log", G_Script()->findCharValue(0, 1), mpStOldLogDir, mServiceId, GetYear(), GetMonth(), GetDay(), GetHour(), GetMin());
            if (rename(mpSecFileName, renfname) < 0)
            {
                G_TraceLog()->sysLog(7, "backupLogFile2 rename() errmsg[%s(%d)] %s", strerror(errno), errno, renfname);
                return false;
            }
        }
    }
    return false;
}

void StatisticsCollector::LoggingPerSec()
{
    if ((mpDayFileName[0] == '\0') || (mpSecFileName[0] == '\0'))
    {
        G_TraceLog()->sysLog(7, "StatisticsCollector::LoggingPerSec(), you must call SetLogFileName() first.");
    }
    else
    {
        SetTimeToNow();
        struct stat st;
        if (stat(mpSecFileName, &st) < 0)
        {
            if (errno != ENOENT)
            {
                G_TraceLog()->sysLog(7, "StatisticsCollector::LoggingPerSec(), stat() errmsg[%s(%d)] [%s]", strerror(errno), errno, mpSecFileName);
                return;
            }
        }
        else if (MAX_STATISTICS_LOG_FILE_SIZE < (unsigned int)st.st_size)
        {
            backupLogFile(false);
        }
        if (stat(mpDayFileName, &st) < 0)
        {
            if (errno != ENOENT)
            {
                G_TraceLog()->sysLog(7, "StatisticsCollector::LoggingPerSec(), stat() errmsg[%s(%d)] [%s]", strerror(errno), errno, mpDayFileName);
                return;
            }
        }
        else if (MAX_STATISTICS_LOG_FILE_SIZE < (unsigned int)st.st_size)
        {
            backupLogFile(true);
        }
        mpLogFile = fopen(mpSecFileName, "a+");
        if (mpLogFile == NULL)
        {
            G_TraceLog()->sysLog(8, "StatisticsCollector::LoggingPerSec(), mpSecFileName fopen failed = %s", mpSecFileName);
        }
        else
        {
            static char szLog[0x1000];
            static char data_str[0x1000];
            static size_t szLogLen;
            static size_t write_len;
            // 全部作为 sprintf 实参（RTL 右到左），避免命名局部量打乱栈槽/寄存器
            sprintf(szLog, "[%02d%02d%02d_%02d:%02d:%02d] net_tr[%4d,%4d] db_tr[%4d,%4d]\n",
                    GetYear(), GetMonth(), GetDay(), GetHour(), GetMin(), GetSec(),
                    pApp->super_Threads.getWorkThread(0)->GetTransactionCntPerSec(),
                    pApp->super_Threads.getWorkThread(0)->GetQueueSizeNoLock(),
                    pApp->super_Threads.getDBThread(0)->GetTransactionCntPerSec(),
                    pApp->super_Threads.getDBThread(0)->mQueueSize);
            szLogLen = strlen(szLog);
            write_len = fwrite(szLog, 1, szLogLen, mpLogFile);
            if (write_len < szLogLen)
            {
                G_TraceLog()->sysLog(8, "StatisticsCollector::LoggingPerSec(), fwrite1 failed = %s", strerror(errno));
                fclose(mpLogFile);
                mpLogFile = NULL;
            }
            else
            {
                for (unsigned int i = 0; i < 3; i++)
                {
                    mStDataPerSec[i].toString(data_str, 0x1000);
                    sprintf(szLog, "                  [%s] %s\n", mpStatisticsTags[i], data_str);
                    szLogLen = strlen(szLog);
                    mStDataPerSec[i].reset();
                    write_len = fwrite(szLog, 1, szLogLen, mpLogFile);
                    if (write_len < szLogLen)
                    {
                        G_TraceLog()->sysLog(8, "StatisticsCollector::LoggingPerSec(), fwrite1 failed = %s", strerror(errno));
                        fclose(mpLogFile);
                        mpLogFile = NULL;
                        return;
                    }
                }
                if (mpLogFile != NULL)
                {
                    fclose(mpLogFile);
                }
                mpLogFile = NULL;
                if (mLastLoggingDay != GetDay())
                {
                    mpLogFile = fopen(mpDayFileName, "a+");
                    if (mpLogFile == NULL)
                    {
                        G_TraceLog()->sysLog(8, "StatisticsCollector::LoggingPerSec(), mpDayFileName fopen failed = %s", mpDayFileName);
                    }
                    else
                    {
                        for (unsigned int i = 0; i < 3; i++)
                        {
                            // 原版 day 循环 toString 使用 mStDataPerSec（偏移 0x2b0），reset 用 mStDataPerDay
                            mStDataPerSec[i].toString(data_str, 0x1000);
                            sprintf(szLog, "[%02d%02d%02d] [%s] %s\n", GetYear(), GetMonth(), GetDay(),
                                    mpStatisticsTags[i], data_str);
                            szLogLen = strlen(szLog);
                            mStDataPerDay[i].reset();
                            write_len = fwrite(szLog, 1, szLogLen, mpLogFile);
                            if (write_len < szLogLen)
                            {
                                G_TraceLog()->sysLog(8, "StatisticsCollector::LoggingPerSec(), fwrite3 failed = %s", strerror(errno));
                                fclose(mpLogFile);
                                mpLogFile = NULL;
                                return;
                            }
                        }
                        if (mpLogFile != NULL)
                        {
                            fclose(mpLogFile);
                        }
                        mpLogFile = NULL;
                        mLastLoggingDay = GetDay();
                        DataInitialization(true);
                    }
                }
                else
                {
                    // 原版：day 未变化时 je 落到 leave 前的 nop（body 结束 jmp 跳过该 nop）
                    __asm__ __volatile__("nop");
                }
            }
        }
    }
}
