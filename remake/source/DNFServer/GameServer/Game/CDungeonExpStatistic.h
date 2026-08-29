// df_game_r Game/ CDungeonExpStatistic（地下城经验统计）。
// 唯一权威声明点（替换 PartyClosureC2.cpp 的局部占位建模，布局保持一致）。
// ORIG 符号：
//   _ZN20CDungeonExpStatistic18IncreaseDungeonExpEiii T 0x0860efca
//   _ZN20CDungeonExpStatistic14InsertDatabaseEv  T 0x0860f110
// 报告：docs/class_func_reports/CDungeonExpStatistic/
// GlobalData::s_statisticMgr（CStatisticMgr）子对象位于 +0x18，本体即 map。
#ifndef DNF_GAME_CDUNGEONEXPSTATISTIC_H_
#define DNF_GAME_CDUNGEONEXPSTATISTIC_H_

#include <map>

class CDungeonExpStatistic
{
public:
    CDungeonExpStatistic() {}  // ORIG W 0x082aa88a（map 默认构造）
    struct stDungeonExp
    {
        int m_exp;          // +0x00（+= 参数 exp）
        int m_count;        // +0x04（+= 参数 count）
        int m_clearTimes;   // +0x08（每次 clear +1）
    };

    // ORIG 0x0860efca：level ∉ [1,200] 仅 trace；命中累加，未命中 insert。
    void IncreaseDungeonExp(int level, int exp, int count);

    // ORIG 0x0860f110：表空直接返回；否则 pool 流 0x59c、msg 0x116，
    // 写 -1 + 表 size，遍历输出 (level, m_exp, m_count, m_clearTimes)，
    // put(LOGDB_Q) 入库后清空统计表。
    void InsertDatabase();

    std::map<int, stDungeonExp> m_map;   // +0x00（子对象起始即 map；CStatisticMgr+0x18）
};

#endif  // DNF_GAME_CDUNGEONEXPSTATISTIC_H_
