// ============================================================================
// df_game_r 还原 —— StatisticsNatType（G2-4 批次，统计域）。
// 依据 docs/class_func_reports/StatisticsNatType.md + ORIG 反汇编。
// 布局：+0x00 map<NAT_TYPE_PEER_TO_PEER, RESULT_COUNT*>。
// NAT_TYPE_PEER_TO_PEER(0x8)/RESULT_COUNT(0x8)/SIG_NAT_TYPE(0x14) 见下。
// 类定义统一到权威头 GameWorld.h（StatisticsNatType/嵌套 key/value），
// 本 TU 仅保留方法定义。
// ============================================================================

#include <map>
// ============================================================================
// Lane33 判词表（2026-09-10；本 TU 两件 DIFF 均已闭合，剩余 diff 仅
// _Z41__static_initialization_and_destruction_0ii = N123 结构性豁免）：
//   StatisticsNatType::IncreaseNatType 0x0860c86c —— AE（78/78 指令、size 0xfc 全等）
//     ① 帧 +0x10 差（0x64 vs 0x54）真因 = `insert(std::make_pair(key, rc))`：
//        4.4 额外发 `_ZSt9make_pairIR…` + `pair<const K,V>::C1IS1_S4_EEOS_IT_T0_E`
//        （move 转换 ctor）两调用并多占 pair<K,V> 临时槽 ⇒ 改 value_type 直构
//        `m_counts.insert(value_type(key, rc))` 后复原 ORIG 的
//        `_ZNSt4pairIK…EEC1IRS1_RS4_EEOT_OT0_`（K&/V& 转发 ctor，P192 配套）。
//     ② 尾部计数 ORIG 单加载形 `mov -0x30(%ebp),%eax; mov (%eax),%edx; add $0x1,%edx;
//        mov %edx,(%eax)` ⇒ 源写前缀 `++rc->m_countA;`（`+= 1` 时 4.4 发
//        `mov slot,%eax; mov slot,%edx; mov (%edx),%edx` 双加载 +1 条）。
//   StatisticsNatType::UpdateNatType 0x0860c968 —— AE（161/161、jcc 目标逐位 0 差）
//     ① 迭代器【先声明后赋值】（ORIG 入口 iterator::C1Ev @0x0860c970 + begin 经
//        sret 临时 -0x2c 拷贝进 it；P50/P314）。
//     ② null 检查落 ORIG 空 then 占位岛（0x0860cb43 nop）+ body 尾 jmp 0xcb44 ⇒ 源写
//        负条件 `if (it->second == 0) goto L_inc; { ... } L_inc:;`（P223；标签置复合块
//        外以合法跨声明），4.4 即产 `test; sete %al; test %al,%al; jne <岛>`。
//     ③ it->second 每处重读（ORIG 条件 3 次 + 写侧 4 次 ptEv，无 rc 缓存槽；P183）。
//     ④ 循环尾后置 `it++`（ORIG ppEi + sret 临时 -0x14；P28）。
//   SIG_NAT_TYPE 写侧已由 TU 局部视图 cast（SIG_NAT_TYPE_VIEW）改为 SigTypes_E.h
//   权威字段直写；ORIG 写序/偏移表：
//     +0x00 m_natType      ← it->first.m_a          0x0860caa8
//     +0x04 m_natTypeOther ← it->first.m_b          0x0860caaf
//     +0x08 m_failCnt      ← it->second->m_countB   0x0860cac3（unsigned 异型 ⇒ eax→edx 中转）
//     +0x0c m_successCnt   ← it->second->m_countA   0x0860cadc（同上）
//     +0x10 m_time         ← now                    0x0860caf4（time_t 同型 ⇒ LHS-first 直存）
// ============================================================================
#include <string.h>
#include <time.h>

#include "GlobalData.h"
#include "MsgQueueMgr.h"
#include "CStreamGuard.h"
#include "GameWorld.h"
#include "SigTypes_E.h"

// SIG_NAT_TYPE：权威定义已字段化于 SigTypes_E.h（原 char m_pad[0x14] 占位移除；
// ORIG GetInBuffer 符号 _ZN11CStreamGuard11GetInBufferI12SIG_NAT_TYPEEPT_v @0x08610f6e
// 使用该权威类型）。字段语义见 UpdateNatType 判词（对齐/偏移 static_assert 在头内）。

// ORIG 08610262：参数对按升序存储（m_a <= m_b）
StatisticsNatType::NAT_TYPE_PEER_TO_PEER::NAT_TYPE_PEER_TO_PEER(int a, int b)
{
    if (a > b)
    {
        m_a = b;
        m_b = a;
    }
    else
    {
        m_a = a;
        m_b = b;
    }
}

// ORIG 08610294：两个计数清零
StatisticsNatType::RESULT_COUNT::RESULT_COUNT()
{
    m_countA = 0;
    m_countB = 0;
}

StatisticsNatType::StatisticsNatType()
    : m_counts()
{
}

StatisticsNatType::~StatisticsNatType()
{
    std::map<NAT_TYPE_PEER_TO_PEER, RESULT_COUNT*>::iterator it = m_counts.begin();
    while (it != m_counts.end())
    {
        RESULT_COUNT* rc = it->second;
        if (rc != 0)
        {
            delete rc;
            rc = 0;
        }
        it++;
    }
}

void StatisticsNatType::Reset()
{
    std::map<NAT_TYPE_PEER_TO_PEER, RESULT_COUNT*>::iterator it = m_counts.begin();
    while (it != m_counts.end())
    {
        RESULT_COUNT* rc = it->second;
        if (rc != 0)
        {
            rc->m_countB = 0;
            rc->m_countA = 0;
        }
        it++;
    }
}

void StatisticsNatType::IncreaseNatType(int natA, int natB, unsigned char count)
{
    NAT_TYPE_PEER_TO_PEER key(natA, natB);
    std::map<NAT_TYPE_PEER_TO_PEER, RESULT_COUNT*>::iterator it = m_counts.find(key);

    RESULT_COUNT* rc = 0;
    if (it != m_counts.end())
    {
        rc = it->second;
    }
    else
    {
        rc = new RESULT_COUNT();
        // ORIG 0x0860c924 段：pair 转发 ctor `_ZNSt4pairIK…EEC1IRS1_RS4_EEOT_OT0_`
        // （K&/V& 直构）⇒ 源码为 value_type(key, rc) 直构；make_pair 会多出
        // `_ZSt9make_pairIR…E` + move 转换 ctor 两次调用（P192 配套）。
        m_counts.insert(std::map<NAT_TYPE_PEER_TO_PEER, RESULT_COUNT*>::value_type(key, rc));
    }

    if (count != 0)
    {
        ++rc->m_countA;
    }
    else
    {
        ++rc->m_countB;
    }
}

void StatisticsNatType::UpdateNatType()
{
    // ORIG 0860c968：入口先 _Rb_tree_iterator::C1Ev（-0x1c）⇒ 迭代器【先声明后赋值】
    // （P50/P314 复活形）；begin() 经 sret 临时 -0x2c 再拷贝进 it。
    std::map<NAT_TYPE_PEER_TO_PEER, RESULT_COUNT*>::iterator it;
    time_t now = time(0);
    it = m_counts.begin();
    // ORIG 条件块在尾部（0x0860cb61：end() -0x18 + operator!=），增量段 0x0860cb44
    // 为【后置】operator++（ppEi，sret 临时 -0x14）⇒ while + it++（P28）；
    // it->second 每处重读（ORIG 无 rc 缓存槽：条件 3 次 + 写字段 2 次 ptEv）。
    while (it != m_counts.end())
    {
        // ORIG 0x0860c9b5..0x0860cb44：null 检查落「空 then 占位岛」（0xcb43 nop）+
        // body 尾 `jmp <inc>`（P223 空块 nop 中继岛；标签置复合块外以合法跨越声明）。
        if (it->second == 0) goto L_inc;
        {
            if (it->second->m_countB > 0 || it->second->m_countA > 0)
            {
                CStreamGuard guard(
                    GlobalData::s_stream_pool->Acquire("Statistics.cpp", 0x1c7), true);

                **guard << 0x83;
                **guard << -1;
                SIG_NAT_TYPE* sig = guard->GetInBuffer<SIG_NAT_TYPE>();
                memset(sig, 0, sizeof(SIG_NAT_TYPE));
                sig->m_natType = it->first.m_a;        // ORIG 0x0860caa8（+0x00 ← m_a）
                sig->m_natTypeOther = it->first.m_b;   // ORIG 0x0860caaf（+0x04 ← m_b）
                sig->m_failCnt = it->second->m_countB;   // ORIG 0x0860cac3（+0x08）
                sig->m_successCnt = it->second->m_countA;// ORIG 0x0860cadc（+0x0c）
                sig->m_time = now;                     // ORIG 0x0860caf4（+0x10，同型直存）
                GlobalData::s_msgq_mgr->put(MsgQueueMgr::LOGDB_Q, guard);
            }
        }
    L_inc:;
        it++;
    }
    Reset();
}
