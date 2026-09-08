// df_game_r Game/ CDungeonEntranceLog（批3：从 GameStubs.cpp 迁移）。
// 布局依据 ORIG IncrementDungeonEntrance(0x860e1ee)/Decrement(0x860e324)：
// 对象即 std::map<int, STDungeonInoutCnt>（find 的 this 为对象首址）。
#ifndef DNF_CDUNGEONENTRANCELOG_H
#define DNF_CDUNGEONENTRANCELOG_H

#include <map>

// pair<int,STDungeonInoutCnt> 中 value 起始 +0x4；ORIG 访问偏移
// +0x4/+0x8/+0xc/+0x10 → 结构内 4 个 int（STDungeonInoutCnt::ctor 清零）。
struct STDungeonInoutCnt
{
    STDungeonInoutCnt() : m_enter(0), m_partyEnter(0), m_total(0), m_curInside(0) {}

    int m_enter;       // +0x00 单人进入累计
    int m_partyEnter;  // +0x04 组队进入累计
    int m_total;       // +0x08 进入总次数
    int m_curInside;   // +0x0c 当前在副本内人数（Decrement 递减）
};

class CDungeonEntranceLog
{
public:
    void DecrementDungeonEntrance(int a, bool b);   // ORIG 0x860e324
    void IncrementDungeonEntrance(int a, bool b);   // ORIG 0x860e1ee
    int InsertDungeonEntrance();                     // ORIG 0x860e59c（尾 mov %ebx,%eax ⇒ int 返回 1）
    int InsertDungeonEntrance_Hour();                // ORIG 0x860e38e（尾 mov %ebx,%eax ⇒ int 返回 1）

    std::map<int, STDungeonInoutCnt> m_inoutMap;
};

CDungeonEntranceLog* GetInstanceDungeonEntranceLog();

#endif  // DNF_CDUNGEONENTRANCELOG_H
