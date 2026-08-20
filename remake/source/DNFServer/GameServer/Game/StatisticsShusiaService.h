#ifndef DNF_STATISTICS_SHUSIA_SERVICE_H
#define DNF_STATISTICS_SHUSIA_SERVICE_H

#include <map>
#include "GameTypes.h"

// df_game_r 布局（32 位）：Mutex +0x00（sizeof 0x18），
// m_dungeonFailure +0x18，m_serviceUsage +0x30；sizeof 0x48。
// 两张 map 均由本类持有，访问必须通过 m_mutex 保护。
class StatisticsShusiaService
{
public:
    StatisticsShusiaService();
    ~StatisticsShusiaService();
    void reset();
    void incDungeonFailure(int dungeon);
    void incServiceUsage(int service);
    int getDungeonFailure(int dungeon);
    int getServiceUSage(int service);
private:
    Mutex m_mutex;                         // +0x00
    std::map<int, int> m_dungeonFailure;   // +0x18
    std::map<int, int> m_serviceUsage;     // +0x30
};

#endif
