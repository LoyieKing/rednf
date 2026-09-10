// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 BestClearTime/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
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

#include "BestClearTime/BestClearTime.inc"
{
    // ORIG 0x80c851c：基类 CData 构造后，map 默认构造并立即 clear，
    // newRecord 清零。
    m_map.clear();
    m_bNewRecord = false;
}

#include "BestClearTime/dtor_BestClearTime.inc"
{
}

#include "BestClearTime/_reset.inc"
{
    // ORIG 0x80c8758：map.clear() + newRecord = false。
    m_map.clear();
    m_bNewRecord = false;
}

// ============================================================================
// 最佳通关时间读写
// ============================================================================

#include "BestClearTime/setBestClearTime.inc"
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

#include "BestClearTime/getBestClearTime.inc"
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

#include "BestClearTime/setNewRecord.inc"
{
    // ORIG 0x80c8b44：写 this+0x20。
    m_bNewRecord = bNew;
}

#include "BestClearTime/checkNewRecord.inc"
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

// 线格式整体视图：characNo@+0 / count@+4 / entries[1000]@+8（每条 0x18）。
// 用结构体数组下标访问（ORIG 的 i*0x18 强度归约 + 成员 disp 折叠）。
struct SIG_BEST_CLEAR_TIME_VIEW
{
    int m_characNo;                                 // +0x00
    int m_count;                                    // +0x04
    SIG_BEST_CLEAR_TIME_ENTRY m_entries[1000];      // +0x08
};


// ORIG 0x080c8776：虚调 _reset() 后按 count（≤1000）逐条
// map::insert(value_type)（ORIG 为 make_pair 引用 → pair 拷贝 → insert 展开）。
#include "BestClearTime/loadData.inc"
{
    (void)pUser;
    _reset();
    // ORIG 0x080c8776：data(-0x10)/i(-0xc) 具名；循环条件每轮重读 count
    // （无 count 具名局部）；记录经结构体数组下标访问（ORIG 的 3i*8 强度
    // 归约 + `lea 0x8/0xc(%eax)` 成员 disp）；make_pair 两实参为记录内
    // int&/SIG& 左值。
    SIG_BEST_CLEAR_TIME_VIEW* data =
        reinterpret_cast<SIG_BEST_CLEAR_TIME_VIEW*>(pData);
    int i = 0;
    while (i < data->m_count && i < 1000)
    {
        // ORIG：make_pair<int&,SIG&>(记录.m_key, 记录.m_data) → pair<int,SIG>
        // 拷贝 → map::insert(value_type const&)（三段调用与 ORIG 展开一致）。
        m_map.insert(std::make_pair(data->m_entries[i].m_key,
                                    data->m_entries[i].m_data));
        ++i;
    }
    return true;
}

// ORIG 0x080c883c：按 map 顺序填 entries（i>0x3e7 截断），写 count=i。
// （characNo@+0 不由本函数写，由 _saveData 填。）
#include "BestClearTime/getData.inc"
{
    // ORIG 0x080c883c 结构：data(-0x10)/i(-0xc) 具名 + begin 具名迭代器(-0x18)；
    // 循环体先 i>0x3e7 直接 return（不写 count）；逐字段重算 i*0x18（无 entry
    // 指针缓存，P183）；first 一次 ptEv、second 一次取值后连读 5 个 int。
    SIG_BEST_CLEAR_TIME_VIEW* data =
        reinterpret_cast<SIG_BEST_CLEAR_TIME_VIEW*>(pData);
    int i = 0;
    std::map<int, SIG_BEST_CLEAR_TIME_DIFFICULTY>::const_iterator it =
        m_map.begin();
    while (it != m_map.end())
    {
        if (i > 0x3e7)
            return;
        // [M3S4 实测-4形 · 76=74+2 封顶] key 直赋 13 条（lea+add+store）vs ORIG
        // 11 条（索引折叠 `mov %ecx,0x8(%eax,%edx,1)`）：①显式 `(char*)data+i*0x18+8`
        // 同 13；②`(char*)data+(8+i*0x18)` 同 13（树序不变产物）；③4 字节聚合拷贝
        // （局部 KeyCell）更劣（提前物化地址 + 多 1 mov）⇒ 跨版本 codegen 差
        // （ORIG 4.1 索引折叠 vs 4.4 标量 store 恒 lea+add）不可翻。
        // 本函数最大改善：删 `const SIG& d = it->second` 具名引用 + 5 条逐字段赋值
        // ⇒ 单条 `m_data = it->second` 聚合拷贝（113→76，m_data 段与 ORIG 逐条恒等）。
        data->m_entries[i].m_key = it->first;
        // ORIG 0x080c88a7-0x080c88d2：第二次 operator-> 后**连读源 5 个 int**
        // 直写目标 +0xc..+0x1c（无具名引用槽）⇒ 整体结构体赋值形。
        data->m_entries[i].m_data = it->second;
        ++it;
        ++i;
    }
    data->m_count = i;
}

// ORIG 0x080c8616：BigStreamPool 序列化 → put DB_Q（QUEUE_IDX 2）。
#include "BestClearTime/_saveData.inc"
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
    // ORIG store 为 RHS-first + eax 中转（`mov %eax,%edx; mov -0xc(%ebp),%eax;
    // mov %edx,(%eax)`）⇒ 目标字段与 RHS 异型（P211 档③）⇒ unsigned 视角。
    // [M3S4 实测-2形 · 87=88-1 封顶] ①unsigned 视角（本形）与 ②具名成员
    // `sig->m_field0 = pUser->get_charac_no(-1)`（SigTypes_A.h:489 int 字段）读数
    // 与产物逐行相同 ⇒ 残差 1 条在 ORIG 独有的槽重载 `mov -0xc(%ebp),%eax;
    // mov %edx,(%eax)`：4.4 改用 callee-saved %ebx 保活（call 前 `mov -0xc(%ebp),
    // %ebx`，call 后 `mov %eax,(%ebx)`）⇒ 跨版本 regalloc 差，无源码形可翻。
    *reinterpret_cast<unsigned int*>(reinterpret_cast<char*>(sig)) =
        pUser->get_charac_no(-1);   // sig->characNo @+0
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    return true;
}
