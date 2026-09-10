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
    // ORIG 0x080c8776：data(-0x10)/i(-0xc) 具名；循环条件每轮重读 data+4
    // （无 count 具名局部）；记录基址 data+i*0x18 每字段重算（无 entry 指针
    // 缓存，P183）；make_pair 两实参为记录内 int&/SIG& 左值。
    char* data = pData;
    int i = 0;
    while (i < *reinterpret_cast<int*>(data + 4) && i < 1000)
    {
        // ORIG：make_pair<int&,SIG&>(记录.m_key, 记录.m_data) → pair<int,SIG>
        // 拷贝 → map::insert(value_type const&)（三段调用与 ORIG 展开一致）。
        m_map.insert(std::make_pair(
            *reinterpret_cast<int*>(data + i * 0x18 + 8),
            *reinterpret_cast<SIG_BEST_CLEAR_TIME_DIFFICULTY*>(data + i * 0x18 +
                                                               0xc)));
        ++i;
    }
    return true;
}

// ORIG 0x080c883c：按 map 顺序填 entries（i>0x3e7 截断），写 count=i。
// （characNo@+0 不由本函数写，由 _saveData 填。）
void BestClearTime::getData(char* pData) const
{
    // ORIG 0x080c883c 结构：data(-0x10)/i(-0xc) 具名 + begin 具名迭代器(-0x18)；
    // 循环体先 i>0x3e7 直接 return（不写 count）；逐字段重算 i*0x18（无 entry
    // 指针缓存，P183）；first 一次 ptEv、second 一次取值后连读 5 个 int。
    char* data = pData;
    int i = 0;
    std::map<int, SIG_BEST_CLEAR_TIME_DIFFICULTY>::const_iterator it =
        m_map.begin();
    while (it != m_map.end())
    {
        if (i > 0x3e7)
            return;
        *reinterpret_cast<int*>(data + i * 0x18 + 0x8) = it->first;
        const SIG_BEST_CLEAR_TIME_DIFFICULTY& d = it->second;
        *reinterpret_cast<int*>(data + i * 0x18 + 0xc) = d.m_time[0];
        *reinterpret_cast<int*>(data + i * 0x18 + 0x10) = d.m_time[1];
        *reinterpret_cast<int*>(data + i * 0x18 + 0x14) = d.m_time[2];
        *reinterpret_cast<int*>(data + i * 0x18 + 0x18) = d.m_time[3];
        *reinterpret_cast<int*>(data + i * 0x18 + 0x1c) = d.m_time[4];
        ++it;
        ++i;
    }
    *reinterpret_cast<int*>(data + 4) = i;
}

// ORIG 0x080c8616：BigStreamPool 序列化 → put DB_Q（QUEUE_IDX 2）。
bool BestClearTime::_saveData(CUser* pUser)
{
    // ORIG 无 stream 具名槽（P0/P259：Acquire 结果 %eax 直传 CStreamGuard C1
    // 第二实参；旧形 +1 store/load 且 guard 槽 -0x14→-0x1c）。
    CStreamGuard guard(
        GlobalData::s_big_stream_pool->Acquire("BestClearTime.cpp", 0x16),
        true);
    // ORIG 0x080c8616：`(*guard)->operator<<(x)`（operator* de() + operator<<）
    // 与 `guard->GetInBuffer`（operator-> pt()）标识调用形态，勿改为直接成员调用
    // （直接 `guard << x` 会省略 ORIG 的 de() 调用）。
    (*guard)->operator<<((int)0x300);
    (*guard)->operator<<((int)pUser->GetUID());

    SIG_BEST_CLEAR_TIME* sig = guard->GetInBuffer<SIG_BEST_CLEAR_TIME>();
    // ORIG：`cmpl $0,sig; jne body; mov $0,%ebx; jmp 尾部` + 尾 `mov $1,%ebx`
    // ⇒ 返回值跨析构保活于 ebx（无 bool 槽）：早退形（旧 `bool ok` 形落槽
    // -0x9 并令 guard 槽 -0x14→-0x18）。
    if (sig == 0)
    {
        return false;
    }
    memset(sig, 0, sizeof(SIG_BEST_CLEAR_TIME));
    // ORIG 虚调 vtable slot2 = getData(char*)。
    getData(reinterpret_cast<char*>(sig));
    *reinterpret_cast<int*>(reinterpret_cast<char*>(sig)) =
        pUser->get_charac_no(-1);   // sig->characNo @+0
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    return true;
}
