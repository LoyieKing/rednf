// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 PvpResultType/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r PvpResultType（G2-4 PvP 结果类型，0x84 字节）
// 逐函数对照 docs/class_func_reports/PvpResultType.md 与 ORIG 反汇编实现；
// AddNewResult 使用 AverageIncremental<int,long long>（ORIG 085dfec6）。
// 2026-08-28 HdrFieldG：删除本 TU 局部类定义，改用权威 PvpResultType.h
// （原局部 m_fieldN 名 → 权威字段名映射：m_field0→m_totalWinCount、
//  m_field4→m_totalLoseCount、m_field8→m_expPoint、m_fieldc→m_oldExpPoint、
//  m_field14→m_pvpGrade、m_field5c→m_todayWinCount、m_field60→m_todayLoseCount、
//  m_field64→m_todayPlayCount、m_field68→m_nextResetTime；其余同名）。
#include "PvpResultType.h"

// ---- AverageIncremental（mangling _Z18AverageIncrementalIixET_T0_S1_S1_）----
template <typename T, typename T2>
T AverageIncremental(T2 a, T2 b, T2 count)
{
    return (T)((a * count + b) / (count + 1));
}

#include "PvpResultType/PvpResultType.inc"
{
}

#include "PvpResultType/dtor_PvpResultType.inc"
{
}

#include "PvpResultType/Set.inc"
{
    *this = other;
    if (m_expPoint < 0)
    {
        m_expPoint = 0;
        m_pvpGrade = 0;
    }
}

#include "PvpResultType/Get.inc"
{
    out = *this;
}

#include "PvpResultType/AddNewResult.inc"
{
    if (m_count == 0)
    {
        unsigned savedWinPoint = m_winPoint;
        Set(other);
        // N76 封顶：此 store ORIG 4.1 值先（mov -0x1c,%edx 后 mov 0x8,%eax），
        // 4.4 恒 LHS 地址先载，probe 全试不可翻（负例表 N76）。
        m_winPoint = savedWinPoint;
    }
    else
    {
        m_avg[0] = AverageIncremental<int, long long>(
            (long long)m_avg[0], (long long)other.m_avg[0], (long long)m_count);
        m_avg[1] = AverageIncremental<int, long long>(
            (long long)m_avg[1], (long long)other.m_avg[1], (long long)m_count);
        m_avg[2] = AverageIncremental<int, long long>(
            (long long)m_avg[2], (long long)other.m_avg[2], (long long)m_count);
        m_avg[3] = AverageIncremental<int, long long>(
            (long long)m_avg[3], (long long)other.m_avg[3], (long long)m_count);
        m_avg[4] = AverageIncremental<int, long long>(
            (long long)m_avg[4], (long long)other.m_avg[4], (long long)m_count);
        m_avg[5] = AverageIncremental<int, long long>(
            (long long)m_avg[5], (long long)other.m_avg[5], (long long)m_count);
        m_avg[6] = AverageIncremental<int, long long>(
            (long long)m_avg[6], (long long)other.m_avg[6], (long long)m_count);
        m_avg[7] = AverageIncremental<int, long long>(
            (long long)m_avg[7], (long long)other.m_avg[7], (long long)m_count);
        m_avg[8] = AverageIncremental<int, long long>(
            (long long)m_avg[8], (long long)other.m_avg[8], (long long)m_count);
        m_avg[9] = AverageIncremental<int, long long>(
            (long long)m_avg[9], (long long)other.m_avg[9], (long long)m_count);
        m_avg[10] = AverageIncremental<int, long long>(
            (long long)m_avg[10], (long long)other.m_avg[10], (long long)m_count);
        m_avg[11] = AverageIncremental<int, long long>(
            (long long)m_avg[11], (long long)other.m_avg[11], (long long)m_count);
        m_avg[12] = AverageIncremental<int, long long>(
            (long long)m_avg[12], (long long)other.m_avg[12], (long long)m_count);
        m_avg[13] = AverageIncremental<int, long long>(
            (long long)m_avg[13], (long long)other.m_avg[13], (long long)m_count);
        m_avg[14] = AverageIncremental<int, long long>(
            (long long)m_avg[14], (long long)other.m_avg[14], (long long)m_count);
    }
    ++m_count;
}

#include "PvpResultType/Clear.inc"
{
    m_totalWinCount = 0;
    m_totalLoseCount = 0;
    m_expPoint = 0;
    m_pvpGrade = 0;
    m_field10 = -1;
    m_avg[0] = 0;
    m_avg[1] = 0;
    m_avg[2] = 0;
    m_avg[3] = 0;
    m_avg[4] = 0;
    m_avg[5] = 0;
    m_avg[6] = 0;
    m_avg[7] = 0;
    m_avg[8] = 0;
    m_avg[9] = 0;
    m_avg[10] = 0;
    m_avg[11] = 0;
    m_avg[12] = 0;
    m_avg[13] = 0;
    m_avg[14] = 0;
    m_count = 0;
    m_winPoint = 0;
    m_todayWinCount = 0;
    m_field6c = 0;
    m_field70 = 0;
    m_field74 = 0;
    m_field78 = 0;
    m_todayLoseCount = 0;
    m_todayPlayCount = 0;
    m_field7c = 0;
    m_field80 = 0;
}

// [L10-2 P210②] GetTotalScore 的 out-of-line 定义迁往 PvP_Room.cpp（置于
// SetPVPResult 之前）：4.4 只对「本 TU 内定义且先于 caller」的 callee 精确继承
// outgoing 帧（ORIG 0x085dc034 `sub $0x4`；跨 TU 仅声明时保守 `sub $0x14`）。
// 全仓唯一消费者 = PvP_Room::SetPVPResult（grep 实证），迁移零副作用；
// ORIG 中 GetTotalScore @0x085d5efe 与 PvP_Room 段（0x085d6012..）地址相邻，
// 本身即同 TU 旁证。
// ============================================================================
// PvpResultSet（G2-4 PvP 结果集合，PvpResultType 派生薄封装）
// 三个薄封装定义原在 PvpResultSet.cpp——ORIG outgoing sub $0x4 为 4.1 对同 TU
// callee（PvpResultType C1/D1/Clear）的精确分配；跨 TU 时 4.4 保守分配 0x18。
// 迁入本 TU（include PvpResultSet.h）后 unit-at-a-time 已知 callee，恢复 0x4。
// ============================================================================
#include "PvpResultSet.h"

#include "PvpResultType/PvpResultSet.inc"
{
}

#include "PvpResultType/dtor_PvpResultSet.inc"
{
}

#include "PvpResultType/Clear_1.inc"
{
    PvpResultType::Clear();
}
