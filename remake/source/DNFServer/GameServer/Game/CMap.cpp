// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CMap/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
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
// 先声明全部显式特化，避免定义顺序触发隐式实例化在前（GCC 4.4）。
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

// ORIG 返回单字节：isInPathArea 调用点 0x08891f20 段为 `call; test %al,%al; je`
// （4.4 对 int 返回 callee 恒先 `test %eax; setne %al` 物化）⇒ 返回 bool（P3/P9）。
#include "CMap/isInArea.inc"
}

// ORIG 0x08891c18。P0/P335 复核（2026-09-09）：ORIG 入口 0834e1c20..0834e1c35 死存
// 4 个常量局部（-0x1c=0x78、-0x18=0x6a、-0x14=0x8c、-0x10=0x14），全函数从未读取
// ⇒ 原码有 4 个具名局部（赋值后未用）；且各默认值分支为 `movl $imm,disp(%eax)`
// 直接落字段、then 分支亦直接落字段（0834e070/0834e08b），非三元物化
// ⇒ 源码是 if/else 语句而非 `?:`（原重建用 ?: 使 8 处字段赋值退化为 eax 物化 + join store）。
#include "CMap/makePathArea.inc"
}

#include "CMap/isInPathArea.inc"
}
// _mapItem / _mapMonster 生命周期 —— 唯一定义点（声明见 CMap.h）。
// ============================================================================

// _mapItem ctor —— ORIG 0x0815088e：仅清零 +0x04/+0x08/+0x0c，不初始化 m_objectIdx。
#include "CMap/_mapItem.inc"
}
// ORIG 无 _mapItem dtor 符号：成员全为 int，平凡析构由编译器内联生成，勿显式定义。

#include "CMap/_mapMonster.inc"
}

// _mapMonster 拷贝构造 —— ORIG 0x0813d708：逐成员拷贝 +0x00..+0x30
// （不含 +0x31 pad），再拷贝构造 +0x34 vector。
// [b28d-24] DIFF 残7: ORIG 连续成员 store 用 %ecx 缓存 this（P29 初始化列表），4.4 逐条重载 0x8(%ebp)。ORIG 0x0813d708
#include "CMap/_mapMonster_1.inc"
}

// _mapMonster dtor —— ORIG 0x081398c6：仅析构 +0x34 vector，无其它动作。
#include "CMap/dtor__mapMonster.inc"


#include "CMap/CMap.inc"
}

#include "CMap/dtor_CMap.inc"
}

#include "CMap/destroy.inc"
}

#include "CMap/destory.inc"
}

#include "CMap/checkFitInPathObject.inc"
}

#include "CMap/insert_item.inc"
}

#include "CMap/insert_monster.inc"
}

#include "CMap/insert_condition_monster.inc"
}

#include "CMap/box_item_index.inc"
}

#include "CMap/CheckEntrance.inc"
}
// ORIG 0x0834e18a。P167 调用点差集（修复 2026-09-09）：ORIG 有
// STAssignMonster::operator=(aSERKS_) ×2 与 STAssignMonster::Clear() ×1，重建曾全缺。
// 证据（报告 docs/class_func_reports/CMap/set_map.md）：
//   ① -0x2b4 的 monster 只在两循环之前构造一次（0834e281 STAssignMonster::C1），
//      两循环内均为 `monster = *it`（0834e2fc / 0834e488 aSERKS_），迭代器槽 -0x240 复用
//      ⇒ 源码是循环外声明 monster/it，non-const iterator（P96/P220）。
//   ② boss 判定读写 monster+0x24（-0x290）而非 +0x14：0834e337 `mov -0x290; cmp $3`
//      → 0834e342 `movl $1,-0x290` + 0834e34c `movb $1,-0x260`（+0x54）。
//   ③ SPOBJ 分支 0834e658 Clear()（call 0836c4fa）→ m_0=action->m_4、m_8=action->m_8
//      （0834e678 `mov %eax,-0x2ac` = +0x8）、m_4=0（0834e67e `movb $0,-0x2b0`）。
//   ④ 内层 action 用索引 + 缓存指针（0834e5db operator[] 一次，结果 -0xc 复用），
//      外层两处循环有 list::empty() 预检查（0834e57e / 0834e82f）。
#include "CMap/set_map.inc"
}
