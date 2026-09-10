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

PvpResultType::PvpResultType()
{
}

PvpResultType::~PvpResultType()
{
}

void PvpResultType::Set(const PvpResultType& other)
{
    *this = other;
    if (m_expPoint < 0)
    {
        m_expPoint = 0;
        m_pvpGrade = 0;
    }
}

void PvpResultType::Get(PvpResultType& out) const
{
    out = *this;
}

void PvpResultType::AddNewResult(const PvpResultType& other)
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

void PvpResultType::Clear()
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

PvpResultSet::PvpResultSet()
{
}

PvpResultSet::~PvpResultSet()
{
}

void PvpResultSet::Clear()
{
    PvpResultType::Clear();
}
