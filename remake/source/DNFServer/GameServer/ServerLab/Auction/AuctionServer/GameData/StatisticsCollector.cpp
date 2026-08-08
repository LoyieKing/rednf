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

StatisticsCollector::StatisticsCollector()
{
    mpLogFile = NULL;
    mpDayFileName[0] = '\0';
    mpSecFileName[0] = '\0';
    strncpy(mpStLogDir, "statistics", 0x400);
    strncpy(mpStOldLogDir, "statistics/old_statistics", 0x400);
    for (unsigned int i = 0; i < 3; i++)
    {
        if (i == 1)
        {
            strncpy(mpStatisticsTags[1], "REGIST_CANCEL", 0x200);
        }
        else if (i == 0)
        {
            strncpy(mpStatisticsTags[0], "REGIST_ITEM  ", 0x200);
        }
        else if (i == 2)
        {
            strncpy(mpStatisticsTags[2], "BIDDING      ", 0x200);
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
        strcat(buf, "\n");
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

void StatisticsCollector::IncTryCnt(int kind)
{
    if ((kind < 0) || (2 < kind))
    {
        G_TraceLog()->sysLog(7, "StatisticsCollector::IncTryCnt() failed. kind(%d)", kind);
    }
    else
    {
        mStDataPerDay[kind].tryCnt = mStDataPerDay[kind].tryCnt + 1;
        mStDataPerSec[kind].tryCnt = mStDataPerSec[kind].tryCnt + 1;
    }
}

void StatisticsCollector::IncFailCnt(int kind, int error_no)
{
    if ((kind < 0) || (2 < kind))
    {
        G_TraceLog()->sysLog(7, "StatisticsCollector::IncFailCnt() failed. kind(%d), error_no(%d)", kind, error_no);
    }
    else
    {
        if (StData::isValidErrorNo(error_no))
        {
            mStDataPerDay[kind].failCnt = mStDataPerDay[kind].failCnt + 1;
            mStDataPerDay[kind].becauseCnt[error_no] = mStDataPerDay[kind].becauseCnt[error_no] + 1;
            mStDataPerSec[kind].failCnt = mStDataPerSec[kind].failCnt + 1;
            mStDataPerSec[kind].becauseCnt[error_no] = mStDataPerSec[kind].becauseCnt[error_no] + 1;
        }
        else
        {
            G_TraceLog()->sysLog(7, "StatisticsCollector::IncFailCnt() failed. kind(%d), error_no(%d)", kind, error_no);
        }
    }
}

bool StatisticsCollector::SetLogFileName(const char* logDir, int serviceId)
{
    mServiceId = serviceId;
    sprintf(mpDayFileName, "%s/%s/DaySt_%d.log", logDir, mpStLogDir, serviceId);
    sprintf(mpSecFileName, "%s/%s/SecSt_%d.log", logDir, mpStLogDir, serviceId);
    bool bRet = makeLogDir();
    return bRet;
}

bool StatisticsCollector::DataInitialization(bool dayDateInit)
{
    if (dayDateInit)
    {
        for (int i = 0; i < 3; i++)
        {
            mStDataPerDay[i].reset();
        }
    }
    else
    {
        for (int i = 0; i < 3; i++)
        {
            mStDataPerSec[i].reset();
        }
    }
    return true;
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
    char* logdir = G_Script()->findCharValue(0, 1);
    sprintf(logdname, "%s/%s", logdir, mpStLogDir);
    if (stat(logdname, &st) < 0)
    {
        if (errno != 2)
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
    sprintf(logdname, "%s/%s", logdir, mpStOldLogDir);
    if (stat(logdname, &st) < 0)
    {
        if (errno != 2)
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
        char renfname[1036];
        if (isDayLog)
        {
            sprintf(renfname, "%s/%s/DaySt_%d#%02d%02d%02d.log", G_Script()->findCharValue(0, 1), mpStOldLogDir, mServiceId, GetYear(), GetMonth(), GetDay());
            if (rename(mpDayFileName, renfname) < 0)
            {
                G_TraceLog()->sysLog(7, "backupLogFile rename() errmsg[%s(%d)] %s", strerror(errno), errno, renfname);
            }
        }
        else
        {
            sprintf(renfname, "%s/%s/SecSt_%d#%02d%02d%02d-%02d%02d.log", G_Script()->findCharValue(0, 1), mpStOldLogDir, mServiceId, GetYear(), GetMonth(), GetDay(), GetHour(), GetMin());
            if (rename(mpSecFileName, renfname) < 0)
            {
                G_TraceLog()->sysLog(7, "backupLogFile2 rename() errmsg[%s(%d)] %s", strerror(errno), errno, renfname);
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
            if (errno != 2)
            {
                G_TraceLog()->sysLog(7, "StatisticsCollector::LoggingPerSec(), stat() errmsg[%s(%d)] [%s]", strerror(errno), errno, mpSecFileName);
                return;
            }
        }
        else if (0x1388000 < (unsigned int)st.st_size)
        {
            backupLogFile(false);
        }
        if (stat(mpDayFileName, &st) < 0)
        {
            if (errno != 2)
            {
                G_TraceLog()->sysLog(7, "StatisticsCollector::LoggingPerSec(), stat() errmsg[%s(%d)] [%s]", strerror(errno), errno, mpDayFileName);
                return;
            }
        }
        else if (0x1388000 < (unsigned int)st.st_size)
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
            NSLDBThread* pDBThread = pApp->super_Threads.getDBThread(0);
            int dbQueueSize = pDBThread->mQueueSize;
            size_t dbTr = pApp->super_Threads.getDBThread(0)->GetTransactionCntPerSec();
            WorkThread* pWorkThread = pApp->super_Threads.getWorkThread(0);
            size_t workQueueSize = pWorkThread->GetQueueSizeNoLock();
            size_t workTr = pWorkThread->GetTransactionCntPerSec();
            char szLog[512];
            char data_str[0x1000];
            size_t szLogLen;
            size_t write_len;
            sprintf(szLog, "[%02d%02d%02d_%02d:%02d:%02d] net_tr[%4d,%4d] db_tr[%4d,%4d]\n",
                    GetYear(), GetMonth(), GetDay(), GetHour(), GetMin(), GetSec(),
                    (int)workTr, (int)workQueueSize, (int)dbTr, dbQueueSize);
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
                        G_TraceLog()->sysLog(8, "StatisticsCollector::LoggingPerSec(), fwrite2 failed = %s", strerror(errno));
                        fclose(mpLogFile);
                        mpLogFile = NULL;
                        return;
                    }
                }
                for (unsigned int i = 0; i < 3; i++)
                {
                    mStDataPerDay[i].toString(data_str, 0x1000);
                    sprintf(szLog, "[%02d%02d%02d] [%s] %s\n", GetYear(), GetMonth(), GetDay(), mpStatisticsTags[i], data_str);
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
    }
}
