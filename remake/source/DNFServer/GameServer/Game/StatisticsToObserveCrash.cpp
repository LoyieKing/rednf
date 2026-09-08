// ============================================================================
// df_game_r 还原 —— StatisticsToObserveCrash（G2-4 批次，统计域）。
// 依据 docs/class_func_reports/StatisticsToObserveCrash.md + ORIG 反汇编。
// 布局：+0x00 int | +0x04 Mutex | +0x1c/0x34/0x4c 三个 map<int,int>。
// ============================================================================

#include <map>

#include "GameTypes.h"

class StatisticsToObserveCrash
{
public:
    StatisticsToObserveCrash();
    ~StatisticsToObserveCrash();

    void reset();
    int getAbnormalDownUserCount();
    void setAbnormalDownUserCount(int count);
    void incAbnormalDownUserCount();
    void incUserLastCrashPacketID(int userIdx);
    int getUserLastCrashPacketID(int userIdx);
    void incUserLastCrashDungeonID(int userIdx);
    int getUserLastCrashDungeonID(int userIdx);
    void incUserLastCrashPlayStatus(int userIdx);
    int getUserLastCrashPlayStatus(int userIdx);

    int m_abnormalDownUserCount;        // +0x00
    Mutex m_mutex;                      // +0x04
    std::map<int, int> m_crashPacketID;   // +0x1c
    std::map<int, int> m_crashDungeonID;  // +0x34
    std::map<int, int> m_crashPlayStatus; // +0x4c
};

StatisticsToObserveCrash::StatisticsToObserveCrash()
    : m_mutex(), m_crashPacketID(), m_crashDungeonID(), m_crashPlayStatus()
{
    m_abnormalDownUserCount = 0;
}

StatisticsToObserveCrash::~StatisticsToObserveCrash() {}

void StatisticsToObserveCrash::reset()
{
    Guard<Mutex> guard(&m_mutex);
    m_abnormalDownUserCount = 0;
    m_crashPacketID.clear();
    m_crashDungeonID.clear();
    m_crashPlayStatus.clear();
}

int StatisticsToObserveCrash::getAbnormalDownUserCount()
{
    Guard<Mutex> guard(&m_mutex);
    return m_abnormalDownUserCount;
}

void StatisticsToObserveCrash::setAbnormalDownUserCount(int count)
{
    Guard<Mutex> guard(&m_mutex);
    m_abnormalDownUserCount = count;
}

void StatisticsToObserveCrash::incAbnormalDownUserCount()
{
    Guard<Mutex> guard(&m_mutex);
    m_abnormalDownUserCount += 1;
}

// [b28d-62] AE：value_type 直构 + `(*it).second`（P167：去 make_pair 转换 ctor
//   与 ptEv）。ORIG 0x0860bb88
void StatisticsToObserveCrash::incUserLastCrashPacketID(int userIdx)
{
    Guard<Mutex> guard(&m_mutex);
    std::map<int, int>::iterator it = m_crashPacketID.find(userIdx);
    if (it == m_crashPacketID.end())
    {
        // [b28d-62] P167：ORIG 直接构造 value_type（pair<int&,int> 模板 ctor），
        //   非 make_pair（多一对 make_pair+转换 ctor call）。
        m_crashPacketID.insert(std::map<int, int>::value_type(userIdx, 1));
    }
    else
    {
        // [b28d-62] P167：ORIG 走 operator*（deEv），非 operator->（ptEv）。
        (*it).second += 1;
    }
}

int StatisticsToObserveCrash::getUserLastCrashPacketID(int userIdx)
{
    Guard<Mutex> guard(&m_mutex);
    std::map<int, int>::iterator it = m_crashPacketID.find(userIdx);
    if (it != m_crashPacketID.end())
    {
        return (*it).second;   // ORIG 用 operator*（de）
    }
    return 0;
}

// [b28d-62] AE：同 incUserLastCrashPacketID。ORIG 0x0860bd2e
void StatisticsToObserveCrash::incUserLastCrashDungeonID(int userIdx)
{
    Guard<Mutex> guard(&m_mutex);
    std::map<int, int>::iterator it = m_crashDungeonID.find(userIdx);
    if (it == m_crashDungeonID.end())
    {
        m_crashDungeonID.insert(std::map<int, int>::value_type(userIdx, 1));
    }
    else
    {
        (*it).second += 1;
    }
}

int StatisticsToObserveCrash::getUserLastCrashDungeonID(int userIdx)
{
    Guard<Mutex> guard(&m_mutex);
    std::map<int, int>::iterator it = m_crashDungeonID.find(userIdx);
    if (it != m_crashDungeonID.end())
    {
        return (*it).second;   // ORIG 用 operator*（de）
    }
    return 0;
}

// [b28d-62] AE：同 incUserLastCrashPacketID。ORIG 0x0860bed4
void StatisticsToObserveCrash::incUserLastCrashPlayStatus(int userIdx)
{
    Guard<Mutex> guard(&m_mutex);
    std::map<int, int>::iterator it = m_crashPlayStatus.find(userIdx);
    if (it == m_crashPlayStatus.end())
    {
        m_crashPlayStatus.insert(std::map<int, int>::value_type(userIdx, 1));
    }
    else
    {
        (*it).second += 1;
    }
}

int StatisticsToObserveCrash::getUserLastCrashPlayStatus(int userIdx)
{
    Guard<Mutex> guard(&m_mutex);
    std::map<int, int>::iterator it = m_crashPlayStatus.find(userIdx);
    if (it != m_crashPlayStatus.end())
    {
        return (*it).second;   // ORIG 用 operator*（de）
    }
    return 0;
}
