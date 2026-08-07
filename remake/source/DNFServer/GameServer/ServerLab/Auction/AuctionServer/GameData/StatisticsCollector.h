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

    struct StData
    {
        unsigned int tryCnt;
        unsigned int failCnt;
        unsigned int becauseCnt[55];

        static bool isValidErrorNo(int err_no)
        {
            if ((err_no < 0) || (0x37 < err_no))
            {
                return false;
            }
            return true;
        }
        void reset();
        void toString(char* buf, int len);
    };

    StatisticsCollector();
    virtual ~StatisticsCollector();
    void IncTryCnt(int kind);
    void IncFailCnt(int kind, int error_no);
    void LoggingPerSec();
    bool SetLogFileName(char* logDir, int serviceId);
    bool DataInitialization(bool dayDateInit);
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
        return mpNowTm->tm_year + 1900;
    }
    bool makeLogDir();
    bool backupLogFile(bool isDayLog);

    StData mStDataPerDay[3];
    StData mStDataPerSec[3];
    FILE* mpLogFile;
    int mServiceId;
    char mpDayFileName[1024];
    char mpSecFileName[1024];
    char mpStLogDir[1024];
    char mpStOldLogDir[1024];
    char mpStatisticsTags[3][512];
    char LINE_STR[512];
    size_t LINE_LEN;
    time_t mNowTimeT;
    tm* mpNowTm;
    int mLastLoggingDay;
};

inline StatisticsCollector* G_StatisticsCollector()
{
    return nsl::GlobalInstance<StatisticsCollector>::inst_ptr();
}

#endif // AUCTION_STATISTICSCOLLECTOR_H_
