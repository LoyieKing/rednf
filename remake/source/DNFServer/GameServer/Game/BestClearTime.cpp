// df_game_r BestClearTime 实现（2026-08-19）。
// 逐函数对照 docs/class_func_reports/BestClearTime.md 与 ORIG objdump：
//   ctor 0x80c851c / dtor 0x80c8596 / _reset 0x80c8758 /
//   setBestClearTime 0x80c8928 / getBestClearTime 0x80c8ac0 /
//   setNewRecord 0x80c8b44 / checkNewRecord 0x80c8b5c。
// loadData/_saveData/getData 已由本 TU 实现（StubS3A 批，ORIG 报告转写）。
//   loadData 0x80c8776 / _saveData 0x80c8616 / getData 0x80c883c。

#include "GlobalData.h"      // GlobalData::{s_big_stream_pool,s_msgq_mgr}
#include "CStreamGuard.h"
#include "CUser.h"           // CUser::{GetUID,get_charac_no}
#include "MsgQueueMgr.h"
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

// ============================================================================
// loadData / _saveData / getData（StubS3A 批）
// SIG_BEST_CLEAR_TIME（0x5dc8）：characNo@+0 / count@+4 /
//   entries[1000]@+8，每条 0x18 = key(4) + SIG_BEST_CLEAR_TIME_DIFFICULTY(0x14)。
// ============================================================================
struct SIG_BEST_CLEAR_TIME_ENTRY
{
    int m_key;                              // +0x00（dungeon idx）
    SIG_BEST_CLEAR_TIME_DIFFICULTY m_data;  // +0x04（5 难度时间）
};


// ORIG 0x080c8776：虚调 _reset() 后按 count（≤1000）逐条
// map::insert(value_type)（ORIG 为 make_pair 引用 → pair 拷贝 → insert 展开）。
bool BestClearTime::loadData(CUser* pUser, char* pData)
{
    (void)pUser;
    _reset();
    const int count = *reinterpret_cast<const int*>(pData + 4);
    for (int i = 0; i < count && i < 1000; ++i)
    {
        // ORIG：make_pair<int&,SIG&>(...) → pair<int,SIG> 拷贝 →
        // map::insert(value_type const&)（三段调用与 ORIG 展开一致）。
        char* entry = pData + 8 + i * 0x18;
        std::pair<int, SIG_BEST_CLEAR_TIME_DIFFICULTY> value(std::make_pair(
            *reinterpret_cast<int*>(entry),
            *reinterpret_cast<SIG_BEST_CLEAR_TIME_DIFFICULTY*>(entry + 4)));
        m_map.insert(value);
    }
    return true;
}

// ORIG 0x080c883c：按 map 顺序填 entries（i>0x3e7 截断），写 count=i。
// （characNo@+0 不由本函数写，由 _saveData 填。）
void BestClearTime::getData(char* pData) const
{
    std::map<int, SIG_BEST_CLEAR_TIME_DIFFICULTY>::const_iterator it =
        m_map.begin();
    int i = 0;
    for (;;)
    {
        if (i > 0x3e7)
            break;
        // ORIG：entries[i]（pData + i*0x18 + 8）：key + 5 个 int 直接 mov。
        int* entry = reinterpret_cast<int*>(pData + i * 0x18);
        entry[2] = it->first;
        entry[3] = it->second.m_time[0];
        entry[4] = it->second.m_time[1];
        entry[5] = it->second.m_time[2];
        entry[6] = it->second.m_time[3];
        entry[7] = it->second.m_time[4];
        ++i;
        ++it;
        if (!(it != m_map.end()))
            break;
    }
    *reinterpret_cast<int*>(pData + 4) = i;
}

// ORIG 0x080c8616：BigStreamPool 序列化 → put DB_Q（QUEUE_IDX 2）。
bool BestClearTime::_saveData(CUser* pUser)
{
    Stream* stream =
        GlobalData::s_big_stream_pool->Acquire("BestClearTime.cpp", 0x16);
    CStreamGuard guard(stream, true);
    // ORIG 0x080c8616：`(*guard)->operator<<(x)`（operator* de() + operator<<）
    // 与 `guard->GetInBuffer`（operator-> pt()）标识调用形态，勿改为直接成员调用
    // （直接 `guard << x` 会省略 ORIG 的 de() 调用）。
    (*guard)->operator<<((int)0x300);
    (*guard)->operator<<((int)pUser->GetUID());

    SIG_BEST_CLEAR_TIME* sig = guard->GetInBuffer<SIG_BEST_CLEAR_TIME>();
    bool ok = (sig != 0);
    if (ok)
    {
        memset(sig, 0, sizeof(SIG_BEST_CLEAR_TIME));
        // ORIG 虚调 vtable slot2 = getData(char*)。
        getData(reinterpret_cast<char*>(sig));
        *reinterpret_cast<int*>(reinterpret_cast<char*>(sig)) =
            pUser->get_charac_no(-1);   // sig->characNo @+0
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    }
    return ok;
}
