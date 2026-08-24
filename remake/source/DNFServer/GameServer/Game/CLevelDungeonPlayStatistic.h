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

    // ORIG 0x0860ecc6：level ∉ [1,200] 仅 trace；否则 find(level)，
    // 命中 → m_playCount += playCount、++m_userCount；未命中 → insert 新节点
    // （m_userCount=1，m_playCount=playCount）。
    void IncreaseLevelDungeonPlay(int level, int playCount);

    std::map<int, stDungeonPlayInfo> m_levelPlayMap;   // +0x00（sizeof(map)==0x18）
};

#endif  // DNF_GAME_CLEVELDUNGEONPLAYSTATISTIC_H_
