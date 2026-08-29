// df_game_r Game/ CLevelDungeonPlayStatistic（等级副本游玩统计）。
// 唯一声明点。ORIG 符号：
//   _ZN26CLevelDungeonPlayStatistic24IncreaseLevelDungeonPlayEii T 0x0860ecc6
// 报告：docs/class_func_reports/CLevelDungeonPlayStatistic/IncreaseLevelDungeonPlay.md
// GlobalData::s_statisticMgr（CStatisticMgr 首基类即本类，调用以基类指针直达）。
#ifndef DNF_GAME_CLEVELDUNGEONPLAYSTATISTIC_H_
#define DNF_GAME_CLEVELDUNGEONPLAYSTATISTIC_H_

#include <map>

class CLevelDungeonPlayStatistic
{
public:
    // ORIG IncreaseLevelDungeonPlay 0x860ecc6 写入节点 value+4/+8 两 int。
    struct stDungeonPlayInfo
    {
        int m_userCount;   // +0x00（每次调用 +1）
        int m_playCount;   // +0x04（+= playCount）
        stDungeonPlayInfo() : m_userCount(0), m_playCount(0) {}
    };
    void IncreaseLevelDungeonPlay(int level, int playCount);  // ORIG T 0x0860ecc6

    // ORIG 0x0860edf4 T（_ZN26CLevelDungeonPlayStatistic14InsertDatabaseEv）：
    // 统计表空直接返回；否则 s_stream_pool 取流（Statistics.cpp:0x569），
    // 写消息头 0x107、-1、表 size，遍历每节点输出 (level, m_userCount,
    // m_playCount) 三字段，put(LOGDB_Q) 入库后 clear 统计表。
    void InsertDatabase();

    std::map<int, stDungeonPlayInfo> m_levelPlayMap;   // +0x00（sizeof(map)==0x18）
};

#endif  // DNF_GAME_CLEVELDUNGEONPLAYSTATISTIC_H_
