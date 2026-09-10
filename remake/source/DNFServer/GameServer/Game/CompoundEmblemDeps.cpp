// ============================================================================
// CompoundEmblem 依赖还原（df_game_r ORIG）
//   - CCompoundEmblemStatistic         纹章合成统计（尺寸 0x1c = 7 int）
//   - getCompoundEmblemStatistic()     单例访问器（_Z26getCompoundEmblemStatisticv）
//   - getAvatarEmblemTypeIdxForCompountEmblem(...)  纹章位图→槽位计数
//                                          （_Z39getAvatarEmblemTypeIdxForCompountEmblemtPiRi）
//   - LineFunc(int, const char*, int, unsigned)     包分发断言日志
//                                          （_Z8LineFunciPKcij）
//
// 参照报告：
//   docs/class_func_reports/_lt_global_gt_/getCompoundEmblemStatistic.md
//   docs/class_func_reports/_lt_global_gt_/LineFunc.md
//   docs/class_func_reports/Dispatcher_CompoundEmblem/_selectJewelTypeIndex.md
//   docs/class_func_reports/TimerUpdateCompoundEmblemStatics/dispatch_sig.md
// ============================================================================

#include "CompoundEmblemDeps.h"
#include "LogManager.h"

// ----------------------------------------------------------------------------
// CCompoundEmblemStatistic —— 纹章合成统计（size 0x1c = 7 int，偏移 +0）
// ORIG 反汇编：
//   ctor 0x861078c / dtor 0x8610792：仅 push/mov/pop/ret（空体）。
//   increaseCount 0x81933b4：idx<0 早退；6<idx 早退；否则 m_data[idx]++。
//   getGradeCount 0x863bcea：return m_data[idx]（非 const）。
//   getTotalCount 0x863bcf8：求和 m_data[0..6]。
//   clear 0x863bd2e：循环 m_data[0..6]=0。
// ----------------------------------------------------------------------------
CCompoundEmblemStatistic::CCompoundEmblemStatistic()
{
}

CCompoundEmblemStatistic::~CCompoundEmblemStatistic()
{
}

void CCompoundEmblemStatistic::increaseCount(int idx)
{
    if (idx < 0) return;
    if (6 < idx) return;
    ++m_data[idx];
}

int CCompoundEmblemStatistic::getGradeCount(int idx)
{
    return m_data[idx];
}

int CCompoundEmblemStatistic::getTotalCount()
{
    int total = 0;
    for (int i = 0; i <= 6; ++i)
        total += m_data[i];
    return total;
}

void CCompoundEmblemStatistic::clear()
{
    for (int i = 0; i <= 6; ++i)
        m_data[i] = 0;
}

// ----------------------------------------------------------------------------
// getCompoundEmblemStatistic —— 单例访问器（_Z26getCompoundEmblemStatisticv）
// ORIG 0x860fd6f：函数局部 static 实例 + __cxa_guard_acquire/release + __cxa_atexit。
// C++ 函数局部 static 自动生成同样的守卫/atexit 注册，符号完全一致。
// ----------------------------------------------------------------------------
CCompoundEmblemStatistic* getCompoundEmblemStatistic()
{
    static CCompoundEmblemStatistic instance;
    return &instance;
}

// ----------------------------------------------------------------------------
// getAvatarEmblemTypeIdxForCompountEmblem —— 纹章位图→槽位计数
// ORIG 0x822a9e5（_Z39getAvatarEmblemTypeIdxForCompountEmblemtPiRi）：
//   type 为 16 位位图；位 0..4 分别对应 counts[0..4]。
//   每命中一位：++counts[i] 且 ++*total。任一命中返回 1，否则返回 -1。
// ----------------------------------------------------------------------------
int getAvatarEmblemTypeIdxForCompountEmblem(unsigned short type, int* counts, int& total)
{
    bool any = false;
    if (type & 0x0001)
    {
        ++counts[0];
        ++total;
        any = true;
    }
    if (type & 0x0002)
    {
        ++counts[1];
        ++total;
        any = true;
    }
    if (type & 0x0004)
    {
        ++counts[2];
        ++total;
        any = true;
    }
    if (type & 0x0008)
    {
        ++counts[3];
        ++total;
        any = true;
    }
    if (type & 0x0010)
    {
        ++counts[4];
        ++total;
        any = true;
    }
    return any ? 1 : -1;
}

// ----------------------------------------------------------------------------
// LineFunc —— 包分发断言日志（_Z8LineFunciPKcij）
// ORIG 0x85908d7：
//   return LogManager::logFormat(1,"PacketDispatcher.cpp",
//       __PRETTY_FUNCTION__, 0x221,
//       "return __LINE__:func(%s) line(%d) errNo(%d) mid(%u)",
//       func, line, errNo, mid);
//   return line;
// ----------------------------------------------------------------------------
int LineFunc(int line, const char* func, int errNo, memberIdentificationNumber_t mid)
{
    LogManager::logFormat(
        1, "PacketDispatcher.cpp",
        __PRETTY_FUNCTION__, 0x221,
        "return __LINE__:func(%s) line(%d) errNo(%d) mid(%u)",
        func, line, errNo, mid);
    return line;
}
