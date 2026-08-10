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
        // ORIG 二进制存在双版本头（ODR 违背，需按 TU 还原）：
        //   StatisticsCollector.cpp 等多数 TU：becauseCnt[55]（228B）
        //   HandlerFor_GA_/HandlerFor_GP_JPN（内含内联 IncTryCnt/IncFailCnt）：
        //     becauseCnt[57]（236B，DWARF byte_size 236）
        // 用 STATISTICS_STDATA_57 宏按 TU 切换；228/236 分别对应机器码
        // imul 0xe4/0xec、sec 基址 0x2b0/0x2c8。运行时分配 0x19a4 覆盖两版。
        // 注意：isValidErrorNo 允许 error_no=55，[55] 时数组下标 55 越界——
        // 这是原版 228B-TU 自身的潜在缺陷，为保持与原版一致而保留。
#ifdef STATISTICS_STDATA_57
        unsigned int becauseCnt[57];
#else
        unsigned int becauseCnt[55];
#endif

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
#ifdef STATISTICS_STDATA_57
    inline void IncTryCnt(int kind)
    {
        if ((kind < 0) || (2 < kind))
        {
            nsl::G_TraceLog()->sysLog(7, "StatisticsCollector::IncTryCnt() failed. kind(%d)", kind);
        }
        else
        {
            // 用后置 ++ 复现 ORIG 的单装载寄存器流（x++ 与 x=x+1 的 GIMPLE 形态不同）
            mStDataPerDay[kind].tryCnt++;
            mStDataPerSec[kind].tryCnt++;
        }
    }

    inline void IncFailCnt(int kind, int error_no)
    {
        if ((kind < 0) || (2 < kind))
        {
            nsl::G_TraceLog()->sysLog(7, "StatisticsCollector::IncFailCnt() failed. kind(%d), error_no(%d)", kind, error_no);
        }
        else
        {
            if (StData::isValidErrorNo(error_no))
            {
                // 同上：++ 形态与 ORIG 反汇编逐指令一致
                mStDataPerDay[kind].failCnt++;
                mStDataPerDay[kind].becauseCnt[error_no]++;
                mStDataPerSec[kind].failCnt++;
                mStDataPerSec[kind].becauseCnt[error_no]++;
            }
            else
            {
                // ORIG：无效 error_no 时先自增 becauseCnt[56]（[57] 版最后一个桶）再打日志
                mStDataPerDay[kind].becauseCnt[56]++;
                mStDataPerSec[kind].becauseCnt[56]++;
                nsl::G_TraceLog()->sysLog(7, "StatisticsCollector::IncFailCnt() failed. kind(%d), error_no(%d)", kind, error_no);
            }
        }
    }
#else
    void IncTryCnt(int kind);
    void IncFailCnt(int kind, int error_no);
#endif
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

// GlobalInstance<StatisticsCollector>::create() 显式特化声明（定义见
// StatisticsCollector.cpp）：ORIG 中 create() 是弱符号，链接期胜出的是
// 非 STATISTICS_STDATA_57 TU 的实例（new 分配 0x19a4）。我们构建的链接顺序
// 会让 HandlerFor_GA_/GP_JPN.o（[57] 布局，0x19d4）的弱实例胜出；
// 此声明使所有 TU 改用本强符号特化，分配尺寸与 ORIG 一致。
namespace nsl {
template <> void GlobalInstance<StatisticsCollector>::create();
}

inline StatisticsCollector* G_StatisticsCollector()
{
    return nsl::GlobalInstance<StatisticsCollector>::inst_ptr();
}

#endif // AUCTION_STATISTICSCOLLECTOR_H_
