// df_game_r BestClearTime 实现（2026-08-19）。
// 逐函数对照 docs/class_func_reports/BestClearTime.md 与 ORIG objdump：
//   ctor 0x80c851c / dtor 0x80c8596 / _reset 0x80c8758 /
//   setBestClearTime 0x80c8928 / getBestClearTime 0x80c8ac0 /
//   setNewRecord 0x80c8b44 / checkNewRecord 0x80c8b5c。
// loadData/_saveData/getData 未在本 TU 实现（后续批次，见头文件注释）。
#include "BestClearTime.h"

// ============================================================================
// 构造 / 析构 / 重置
// ============================================================================

BestClearTime::BestClearTime()
{
    // ORIG 0x80c851c：基类 CData 构造后，map 默认构造并立即 clear，
    // newRecord 清零。
    m_map.clear();
    m_bNewRecord = false;
}

BestClearTime::~BestClearTime()
{
}

void BestClearTime::_reset()
{
    // ORIG 0x80c8758：map.clear() + newRecord = false。
    m_map.clear();
    m_bNewRecord = false;
}

// ============================================================================
// 最佳通关时间读写
// ============================================================================

void BestClearTime::setBestClearTime(int dungeonIdx, int difficulty, int time)
{
    // ORIG 0x80c8928。
    // 难度只允许 0..4；时间为负按 0 处理。
    if (difficulty > 4)
        return;
    if (time < 0)
        time = 0;

    std::map<int, SIG_BEST_CLEAR_TIME_DIFFICULTY>::iterator it =
        m_map.find(dungeonIdx);
    if (it == m_map.end())
    {
        // 无该副本记录：以全零 SIG_BEST_CLEAR_TIME_DIFFICULTY 插入，
        // 再写入当前难度的时间。
        SIG_BEST_CLEAR_TIME_DIFFICULTY tmp;
        tmp.m_time[difficulty] = time;
        m_map.insert(std::make_pair(dungeonIdx, tmp));
        setNewRecord(true);
        alter();
        return;
    }

    if (it->second.m_time[difficulty] == 0)
    {
        // 该难度尚无记录：直接写入。
        it->second.m_time[difficulty] = time;
        setNewRecord(true);
        alter();
    }
    else if (it->second.m_time[difficulty] > time)
    {
        // 已有记录且新时间更短：覆盖。
        it->second.m_time[difficulty] = time;
        setNewRecord(true);
        alter();
    }
}

int BestClearTime::getBestClearTime(int dungeonIdx, int difficulty)
{
    // ORIG 0x80c8ac0：find 未命中或难度 > 4 返回 0，否则返回记录值。
    std::map<int, SIG_BEST_CLEAR_TIME_DIFFICULTY>::iterator it =
        m_map.find(dungeonIdx);
    if (it == m_map.end() || difficulty > 4)
        return 0;
    return it->second.m_time[difficulty];
}

// ============================================================================
// 新纪录标志
// ============================================================================

void BestClearTime::setNewRecord(bool bNew)
{
    // ORIG 0x80c8b44：写 this+0x20。
    m_bNewRecord = bNew;
}

bool BestClearTime::checkNewRecord()
{
    // ORIG 0x80c8b5c：读 this+0x20。
    return m_bNewRecord;
}
