// ============================================================================
// df_game_r 还原 —— StatisticsShusiaService（G2-4 批次，统计域）。
// 依据 docs/class_func_reports/StatisticsShusiaService.md + ORIG 反汇编。
// 布局：+0x00 Mutex | +0x18 map<int,int> | +0x30 map<int,int>。
// ============================================================================

#include <map>

#include "GameTypes.h"

class StatisticsShusiaService
{
public:
    StatisticsShusiaService();
    ~StatisticsShusiaService();

    void reset();
    void incDungeonFailure(int dungeonIdx);
    void incServiceUsage(int serviceIdx);
    int getDungeonFailure(int dungeonIdx);
    int getServiceUSage(int serviceIdx);

    Mutex m_mutex;              // +0x00
    std::map<int, int> m_dungeonFailure;  // +0x18
    std::map<int, int> m_serviceUsage;    // +0x30
};

StatisticsShusiaService::StatisticsShusiaService()
    : m_mutex(), m_dungeonFailure(), m_serviceUsage()
{
}

StatisticsShusiaService::~StatisticsShusiaService() {}

void StatisticsShusiaService::reset()
{
    Guard<Mutex> guard(&m_mutex);
    m_dungeonFailure.clear();
    m_serviceUsage.clear();
}

void StatisticsShusiaService::incDungeonFailure(int dungeonIdx)
{
    Guard<Mutex> guard(&m_mutex);
    std::map<int, int>::iterator it = m_dungeonFailure.find(dungeonIdx);
    if (it == m_dungeonFailure.end())
    {
        m_dungeonFailure.insert(std::make_pair(dungeonIdx, 1));
    }
    else
    {
        it->second += 1;
    }
}

void StatisticsShusiaService::incServiceUsage(int serviceIdx)
{
    Guard<Mutex> guard(&m_mutex);
    std::map<int, int>::iterator it = m_serviceUsage.find(serviceIdx);
    if (it == m_serviceUsage.end())
    {
        m_serviceUsage.insert(std::make_pair(serviceIdx, 1));
    }
    else
    {
        it->second += 1;
    }
}

int StatisticsShusiaService::getDungeonFailure(int dungeonIdx)
{
    Guard<Mutex> guard(&m_mutex);
    std::map<int, int>::iterator it = m_dungeonFailure.find(dungeonIdx);
    if (it != m_dungeonFailure.end())
    {
        return it->second;
    }
    return 0;
}

int StatisticsShusiaService::getServiceUSage(int serviceIdx)
{
    Guard<Mutex> guard(&m_mutex);
    std::map<int, int>::iterator it = m_serviceUsage.find(serviceIdx);
    if (it != m_serviceUsage.end())
    {
        return it->second;
    }
    return 0;
}
