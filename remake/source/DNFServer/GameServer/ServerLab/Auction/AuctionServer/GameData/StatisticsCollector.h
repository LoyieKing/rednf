#ifndef AUCTION_STATISTICSCOLLECTOR_H_
#define AUCTION_STATISTICSCOLLECTOR_H_

#include <stdio.h>
#include <time.h>

#include "GlobalInstance.h"

class StatisticsCollector
{
public:
    enum EnumData
    {
        ENUM_REGIST_ITEM_STATISTICS = 0,
        ENUM_REGIST_CANCEL_STATISTICS = 1,
        ENUM_BIDDING_STATISTICS = 2,
        ENUM_STATISTICS_END_ = 3
    };

    static const int MAX_STATISTICS_LOG_FILE_SIZE = 0x1388000;

    struct StData
    {
        unsigned int tryCnt;
        unsigned int failCnt;
        // ORIG 二进制存在双版本头：多数 TU（ctor/getter/LoggingPerSec/DataInitialization
        // 等）用 becauseCnt[55]（228B），仅 IncTryCnt/IncFailCnt 所在 TU 用 [57]（236B）。
        // 本实现跟随多数 + 运行时分配 0x19a4（228B + 尾部 48B 填充）。
        // 注意：isValidErrorNo 允许 error_no=55，[55] 时数组下标 55 越界——
        // 这是原版 228B-TU 自身的潜在缺陷，为保持与原版一致而保留。
        unsigned int becauseCnt[55];

        static bool isValidErrorNo(int err_no)
        {
            // ORIG：正区间 then（js/jg 直跳尾部 false 块），勿用 De Morgan 反写
            if ((err_no >= 0) && (err_no <= 0x37))
            {
                return true;
            }
            return false;
        }
        void reset();
        void toString(char* buf, int len);
    };

    StatisticsCollector();
    virtual ~StatisticsCollector();
    void IncTryCnt(int kind);
    void IncFailCnt(int kind, int error_no);
    void LoggingPerSec();
    bool SetLogFileName(const char* logDir, int serviceId);
    void DataInitialization(bool dayDateInit);
    void SetTimeToNow();
    int GetSec()
    {
        return mpNowTm->tm_sec;
    }
    int GetMin()
    {
        return mpNowTm->tm_min;
    }
    int GetHour()
    {
        return mpNowTm->tm_hour;
    }
    int GetDay()
    {
        return mpNowTm->tm_mday;
    }
    int GetMonth()
    {
        return mpNowTm->tm_mon + 1;
    }
    int GetYear()
    {
        // 原版：两位数年（tm_year % 100），配合 %02d 格式
        return mpNowTm->tm_year % 100;
    }
    bool makeLogDir();
    bool backupLogFile(bool isDayLog);

    StData mStDataPerDay[3];
    StData mStDataPerSec[3];
    FILE* mpLogFile;
    int mServiceId;
    char mpDayFileName[1024];
    char mpSecFileName[1024];
    char mpStLogDir[512];
    char mpStOldLogDir[512];
    char mpStatisticsTags[3][512];
    char LINE_STR[512];
    size_t LINE_LEN;
    time_t mNowTimeT;
    tm* mpNowTm;
    int mLastLoggingDay;
    char unused_tail[48];  // 228B-StData 布局 + 48B 尾部 = 0x19a4（运行时分配大小）
};

inline StatisticsCollector* G_StatisticsCollector()
{
    return nsl::GlobalInstance<StatisticsCollector>::inst_ptr();
}

#endif // AUCTION_STATISTICSCOLLECTOR_H_
