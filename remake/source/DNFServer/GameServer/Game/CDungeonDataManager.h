// CDungeonDataManager 批次独占（仅 ORIG 符号，无桥）。
// 布局/行为证据：
// - C1/C2 W 0x08486bb4：仅 map C1 + movb $0,0x18 → 非虚类（无 vptr 写入），
//   成员 std::map<DungeonKey,CDungeonNode> @ +0x00（0x18）+ char @ +0x18；
// - D1/D2 W 0x08486bda：仅调 map D1 → 编译器隐式析构（无用户析构，P12）；
// - 7 方法：docs/class_func_reports/CDungeonDataManager/ 全部报告；
//   map 迭代器解引用取 CDungeonNode 均为 add $0xc ⇒ DungeonKey sizeof 0xc
//   （第三字段 @+0x08，CDungeonNode.h:26 m_field8，由 OrigDungeonNode 落实）；
// - GetInstanceDungeonDataManager T 0x084849b4：P20 函数局部 static 单例。
// 返回类型按 Ghidra 报告：Update* 为 bool（mov $0/$1 直返），Insert*/UpdateDungeonDataStatistic 为 int。
#ifndef DNF_GAME_CDUNGEONDATAMANAGER_H_
#define DNF_GAME_CDUNGEONDATAMANAGER_H_

#include <map>

#include "CDungeonNode.h"  // struct DungeonKey（0x0c）/ class CDungeonNode（0x90）

struct SIG_DUNGEON_DATA_STATISTIC;  // 完整定义 SigTypes_B.h（仅指针传递，前向足够）

class CDungeonDataManager
{
public:
    CDungeonDataManager()              // W 0x08486bb4（C1/C2）
        : m_dungeonMap(), m_flag(0)
    {
    }
    // 无用户析构：ORIG D1/D2 W 0x08486bda 仅调 map D1，编译器隐式生成（P12）

    bool UpdateSkill(SIG_DUNGEON_DATA_STATISTIC* stat);        // T 0x084859d8
    bool UpdateMonster(SIG_DUNGEON_DATA_STATISTIC* stat);      // T 0x08485a5a
    int InsertSkillKey(int skillIndex, int job);               // T 0x0848589a
    bool UpdateCharacter(SIG_DUNGEON_DATA_STATISTIC* stat);    // T 0x08485956
    bool InsertDungeonKey(DungeonKey& key);                    // T 0x084856ac
    int InsertMonsterKey(int monsterIndex);                    // T 0x0848580a
    int UpdateDungeonDataStatistic();                          // T 0x08485adc

    std::map<DungeonKey, CDungeonNode> m_dungeonMap;  // +0x00（0x18）
    char m_flag;                                      // +0x18（C1 写 0；无语义证据）
};

CDungeonDataManager& GetInstanceDungeonDataManager();  // T 0x084849b4

#endif // DNF_GAME_CDUNGEONDATAMANAGER_H_
