#ifndef GAME_CCLIENT_CRASH_DOWN_LOG_STATISTIC_H
#define GAME_CCLIENT_CRASH_DOWN_LOG_STATISTIC_H

#include <cstddef>
#include <map>
#include <string>
#include <vector>

class CClientCrashDownLogStatistic
{
public:
    struct stClientCrashDownLog_t
    {
        int m_occTime;
        std::map<int, int> m_reasonCount;
        stClientCrashDownLog_t();
        stClientCrashDownLog_t(const stClientCrashDownLog_t& other);
        stClientCrashDownLog_t& operator=(const stClientCrashDownLog_t& other);
        ~stClientCrashDownLog_t();
    };

    CClientCrashDownLogStatistic();
    ~CClientCrashDownLogStatistic();
    void Init();
    void RegisterClientCrashDown(int reason);
    int CarryClientCrashDown();
    void MakeClientCrashDownQuery(std::vector<std::string>& queries, int queryPrefixLength);
    int UpdateClientCrashDown();

    std::map<int, int> m_reasonCount;                 // +0x00
    std::vector<stClientCrashDownLog_t> m_logRecords; // +0x18
};
CClientCrashDownLogStatistic* GetInstanceClientCrashDownLogStatistic();

static_assert(sizeof(std::map<int, int>) == 0x18, "ORIG i386 map layout");
static_assert(sizeof(CClientCrashDownLogStatistic::stClientCrashDownLog_t) == 0x1c, "ORIG crash log record layout");
static_assert(offsetof(CClientCrashDownLogStatistic, m_logRecords) == 0x18, "ORIG vector offset");

#endif
