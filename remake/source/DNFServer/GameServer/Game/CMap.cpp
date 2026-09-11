// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CMap/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
 #include <algorithm>
 #include <new>

 // cElection<int,V,C> 的唯一实现归属地图/选举 TU（声明点 PvPTypes.h，ORIG 布局）。
 // cParty.cpp 的 cElection<int,4,4>（m_padElection@+0xd8）与 PvP_Room.cpp 的
 // cElection<int,8,8>（+0x12c）经本 TU 提供真实符号。
 #include "PvPTypes.h"
// GetElected 共同语义（ORIG 0x85dff78 / 0x85c03a8）：对票矩阵按列求和，
// 返回票数最大列下标；全零票返回 -1。以宏展开避免提前隐式实例化。
#define CELECTION_GET_ELECTED(V, C)                                          \
    do                                                                       \
    {                                                                        \
        int tally[C] = { 0 };                                                \
        int total = 0;                                                       \
        for (int i = 0; i <= (V) - 1; ++i)                                   \
        {                                                                    \
            for (int j = 0; j <= (C) - 1; ++j)                               \
                tally[j] += m_voters[i].m_votes[j];                          \
            total += tally[i];                                               \
        }                                                                    \
        int best = -1;                                                       \
        int bestVal = 0;                                                     \
        for (int i = 0; i <= (C) - 1; ++i)                                   \
        {                                                                    \
            if (tally[i] > bestVal)                                          \
            {                                                                \
                bestVal = tally[i];                                          \
                best = i;                                                    \
            }                                                                \
        }                                                                    \
        return best;                                                         \
    } while (0)
#include "CMap/Init.inc"
#include "CMap/cVoter.inc"
template <> cElection<int, 8, 8>::cElection();
template <> void cElection<int, 8, 8>::Init();
template <> void cElection<int, 8, 8>::Vote(int voter, int candidate);
template <> void cElection<int, 8, 8>::CancelVoter(int voter);
template <> void cElection<int, 8, 8>::CancelCandidate(int candidate);
template <> int cElection<int, 8, 8>::GetElected() const;
#include "CMap/Init_1.inc"
#include "CMap/cVoter_1.inc"
template <> cElection<int, 4, 4>::cElection();
template <> void cElection<int, 4, 4>::Init();
template <> void cElection<int, 4, 4>::Vote(int voter, int candidate);
template <> void cElection<int, 4, 4>::CancelVoter(int voter);
template <> void cElection<int, 4, 4>::CancelCandidate(int candidate);
template <> int cElection<int, 4, 4>::GetElected() const;


#include "CMap/Init_2.inc"
{
    std::fill(m_votes, m_votes + 8, 0);   // ORIG 0x85e0304
}
#include "CMap/cVoter_2.inc"
template <> cElection<int, 8, 8>::cElection()
{
    // 成员数组 m_voters[8] 的 cVoter 构造由编译器在 ctor 体前自动生成
    // （ORIG 0x85dff3a 亦仅此一段倒序构造 + Init；NEW 曾多出的 placement
    // new 循环属双构造冗余，已删除以对齐 ORIG）。
    Init();                               // ORIG 0x85e02d2
}
template <> void cElection<int, 8, 8>::Init()
{
    for (int i = 0; i <= 7; ++i)
        m_voters[i].Init();
}
template <> void cElection<int, 8, 8>::Vote(int voter, int candidate)
{
    m_voters[voter].m_votes[candidate] = 1;
}
template <> void cElection<int, 8, 8>::CancelVoter(int voter)
{
    m_voters[voter].Init();
}
template <> void cElection<int, 8, 8>::CancelCandidate(int candidate)
{
    for (int i = 0; i <= 7; ++i)
        m_voters[i].m_votes[candidate] = 0;
}
template <> int cElection<int, 8, 8>::GetElected() const   // ORIG 0x85dff78
{
    CELECTION_GET_ELECTED(8, 8);
}

#include "CMap/Init_3.inc"
{
    std::fill(m_votes, m_votes + 4, 0);   // ORIG 0x82beb62
}
#include "CMap/cVoter_3.inc"
template <> cElection<int, 4, 4>::cElection()
{
    // ORIG 0x082be?? P167 calls=2：仅「数组元素 cVoter::C1 循环 + Init()」。
    // 原 placement-new 循环产生 EXTRA cVoter C1/_ZnwjPv/_ZdlPvS_/Unwind_Resume
    // （双构造冗余），与 8,8 版同样删除（成员数组构造由编译器自动生成）。
    Init();
}
template <> void cElection<int, 4, 4>::Init()
{
    for (int i = 0; i <= 3; ++i)
        m_voters[i].Init();
}
template <> void cElection<int, 4, 4>::Vote(int voter, int candidate)
{
    m_voters[voter].m_votes[candidate] = 1;   // ORIG 0x85c038c
}
template <> void cElection<int, 4, 4>::CancelVoter(int voter)
{
    m_voters[voter].Init();
}
template <> void cElection<int, 4, 4>::CancelCandidate(int candidate)  // ORIG 0x85c047c
{
    for (int i = 0; i <= 3; ++i)
        m_voters[i].m_votes[candidate] = 0;
}
template <> int cElection<int, 4, 4>::GetElected() const   // ORIG 0x85c03a8
{
    CELECTION_GET_ELECTED(4, 4);
}
 // df_game_r Game/ CMap 实现（G2 地图链）。
 // 逻辑依据 docs/class_func_reports/CMap.md；成员访问统一使用语义字段。
 #include <cstring>
 #include "CMap.h"
 #include "CDataManager.h"   // G_CDataManager()->m_stageTimeLine（set_map）
// isInArea / makePathArea / isInPathArea —— ORIG 全局函数（mangled _Z8isInAreaiiP7MapArea
// _Z12makePathAreaP7MapAreaPA2_KiS3_ _Z12isInPathAreaiiPA2_KiS1_）。
// 语义（报告 08891c18 / 0889203b / 08891f20）：
//   makePathArea(areas, area1(坐标), area2(宽高))：宽/高取自 area2（≤0 用默认值），
//   坐标取 area1 并裁剪到区域内；isInArea 为严格半开区间 [x0,x0+w)×[y0,y0+h)。
// 默认为第 0/1 区 w=0x78 h=0x6a；第 2/3 区 w=0x8c h=0x28。
// 实现地点：CMap.cpp（本域）；MapArea 权威定义见 CVillageMonsterMgr.h。

#include "CMap/isInArea.inc"


#include "CMap/makePathArea.inc"


#include "CMap/isInPathArea.inc"

// _mapItem / _mapMonster 生命周期 —— 唯一定义点（声明见 CMap.h）。
// ============================================================================

#include "CMap/_mapItem.inc"

// ORIG 无 _mapItem dtor 符号：成员全为 int，平凡析构由编译器内联生成，勿显式定义。

#include "CMap/_mapMonster.inc"


#include "CMap/_mapMonster_1.inc"


#include "CMap/dtor__mapMonster.inc"


#include "CMap/CMap.inc"


#include "CMap/dtor_CMap.inc"


#include "CMap/destroy.inc"


#include "CMap/destory.inc"


#include "CMap/checkFitInPathObject.inc"


#include "CMap/insert_item.inc"


#include "CMap/insert_monster.inc"


#include "CMap/insert_condition_monster.inc"


#include "CMap/box_item_index.inc"


#include "CMap/CheckEntrance.inc"

#include "CMap/set_map.inc"

