// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CActionPointManager/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r Game/ APSystem::CActionPointManager 实现（批3：从 GameStubs.cpp 迁移）。
// 类声明位于 CUser.h（APSystem::CActionPointManager）。本 TU 是这些符号的唯一权威定义。
// ORIG：GM_Set @0x8121b3e（T）、Reset @0x8122af4（W）、GetActionPoint @0x8694bfc（W）。
#include "CUser.h"
#include <string.h>

namespace APSystem
{

// _ActionGroupIndex / CUserProc 权威声明见 CUserProc.h（本 TU 本地镜像已删）。

// ORIG 0x8694bfc（W，内联）：mov 0x8(%ebp),%eax; add $0x8,%eax; ret。
// 返回嵌入的 _SIG_LOAD_ACTION_POINT 首地址；具体返回类型仍按现有 ABI
// 以字节指针暴露，避免把 packed 包误当成普通对齐对象。
#include "CActionPointManager/GetActionPoint.inc"
{
    return reinterpret_cast<unsigned char*>(
        const_cast<_SIG_LOAD_ACTION_POINT*>(&m_sig));
}

// ORIG 0x8121b3e：m_flag00=1；m_sig.m_apSum=0；m_prevApSum=0；
// 先把所有已领条目的五个日期槽取负标记；再按行/槽扫描，在行动点总数
// 未达目标 value 前逐项恢复、累加并发包通知。_ActionPoint 已按 ORIG
// 0xb 字节 packed，具名成员访问仍保持 +0x00/+0x02/+0x06 的真实布局。
#include "CActionPointManager/GM_Set.inc"
{
    // b28d-L3: DIFF/N82 — ORIG 0x082... push ebx + sub 0x44 vs 我方 sub 0x58（帧差/callee-saved 缺失，N82）
    m_flag00 = 1;
    m_sig.m_apSum = 0;
    m_prevApSum = 0;
    for (int row = 0; row <= 299; ++row)
    {
        _ActionPoint& rec = m_sig.m_actionPoints[row];
        for (int slot = 0; slot <= 4; ++slot)
        {
            const signed char entry =
                static_cast<signed char>(rec.m_days[slot]);
            if (entry > 0)
            {
                rec.m_days[slot] = static_cast<unsigned char>(-entry);
            }
        }
    }
    for (int row = 0;
         row <= 299 && m_sig.m_apSum < value; ++row)
    {
        _ActionPoint& rec = m_sig.m_actionPoints[row];
        for (int slot = 0;
             slot <= 4 && m_sig.m_apSum < value; ++slot)
        {
            const signed char entry =
                static_cast<signed char>(rec.m_days[slot]);
            if (entry < 0)
            {
                m_prevApSum = m_sig.m_apSum;
                const unsigned char restored =
                    static_cast<unsigned char>(-entry);
                rec.m_days[slot] = restored;
                m_sig.m_apSum += static_cast<signed char>(restored);
                const _ActionGroupIndex group =
                    static_cast<_ActionGroupIndex>(rec.m_groupIndex);
                CUserProc::SendActionClear(user, rec.m_actionNo, group, true,
                                           ENUM_PACKETCLASS_0);
                CUserProc::CheckAndSendToMessageAllUser(
                    m_prevApSum, m_sig.m_apSum, user.getCurCharacName());
            }
        }
    }
}

// ORIG 0x08122af4（W）：m_flag00=0；m_prevApSum=0；
// memset(&m_sig, 0, sizeof(_SIG_LOAD_ACTION_POINT))。无负标记扫描循环。
#include "CActionPointManager/Reset.inc"
{
    m_flag00 = 0;
    m_prevApSum = 0;
    memset(&m_sig, 0, sizeof(m_sig));
}

} // namespace APSystem
