// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CDungeonEntranceLog/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r Game/ CDungeonEntranceLog 实现（批3：从 GameStubs.cpp 迁移）。
// 语义依据 ORIG IncrementDungeonEntrance 0x860e1ee / DecrementDungeonEntrance 0x860e324 /
// InsertDungeonEntrance 0x860e59c / InsertDungeonEntrance_Hour 0x860e38e。
#include "CDungeonEntranceLog.h"

#include "GlobalData.h"
#include "CStreamGuard.h"
#include "StreamPool.h"
#include "MsgQueueMgr.h"

#include "CDungeonEntranceLog/IncrementDungeonEntrance.inc"
{
    // [b28d-59] AE（ORIG 0x860e1ee）：指令序列逐条一致，.o 层仅 call 重定位标签
    // 未解析的显示差（AE 口径归零）；95/95。
    // ORIG 0x860e1ee：存在路径 isParty 仅 ++m_partyEnter（不加 total/curInside）；
    // 插入路径 isParty 时 ++m_partyEnter 且 ++m_total/++m_curInside（不对称，忠实复刻）。
    std::map<int, STDungeonInoutCnt>::iterator it = m_inoutMap.find(dungeonIdx);
    if (it != m_inoutMap.end())
    {
        if (isParty)
        {
            it->second.m_partyEnter += 1;
        }
        else
        {
            it->second.m_enter += 1;
            it->second.m_total += 1;
            it->second.m_curInside += 1;
        }
    }
    else
    {
        STDungeonInoutCnt cnt;
        if (isParty)
        {
            cnt.m_partyEnter += 1;
        }
        else
        {
            cnt.m_enter += 1;
        }
        cnt.m_total += 1;
        cnt.m_curInside += 1;
        m_inoutMap.insert(std::make_pair(dungeonIdx, cnt));
    }
}

#include "CDungeonEntranceLog/DecrementDungeonEntrance.inc"
{
    std::map<int, STDungeonInoutCnt>::iterator it = m_inoutMap.find(dungeonIdx);
    if (it != m_inoutMap.end())
        it->second.m_curInside -= 1;   // ORIG 节点 +0x10 -1（未命中不动）
}

#include "CDungeonEntranceLog/InsertDungeonEntrance.inc"
{
    // [b28d-59] 封 N82附/N98（ORIG 0x860e59c）：已还原 int 返回 1（尾 mov %ebx,%eax）、
    // 具名 nSize 槽 -0xc、早退 `cmpl $0,-0xc; jg body; mov $1,%ebx; jmp end`（125/125
    // 指令数全等）。残差=槽位相对序：4.4 把编译器临时 end() sret 放浅槽 -0x10、具名
    // guard/it 下沉（-0x18/-0x1c），4.1 为 guard -0x14 / it -0x18 / end 临时 -0x1c
    // （声明序）；源码不可控编译器临时槽（P232/N98），语义等价。
    // 语义：map 非空时把 (key, m_enter, m_partyEnter) 写入 Statistics DB 流
    //（msg 0xe0, LOGDB_Q=4），随后清空 map。
    int nSize = m_inoutMap.size();
    if (nSize <= 0)
        return 1;
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("Statistics.cpp", 0x4da), true);
    **guard << 0xe0;
    **guard << -1;
    **guard << nSize;
    // [b28d-59] it 提到 for 外（ORIG 槽序 guard -0x14 / it -0x18 / end 临时 -0x1c）。
    std::map<int, STDungeonInoutCnt>::iterator it = m_inoutMap.begin();
    for (; it != m_inoutMap.end(); ++it)
    {
        **guard << it->first;
        **guard << it->second.m_enter;        // 节点 +0x4
        **guard << it->second.m_partyEnter;   // 节点 +0x8
    }
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::LOGDB_Q, guard);
    m_inoutMap.clear();
    return 1;
}

#include "CDungeonEntranceLog/InsertDungeonEntrance_Hour.inc"
{
    // [b28d-59] 封 N82附/N98（ORIG 0x860e38e）：与 InsertDungeonEntrance 同构——已还原
    // int 返回 1、具名 nSize 槽 -0xc、早退 `jg body; mov $1,%ebx`；残差同件（4.4 编译器
    // 临时 end() sret 占浅槽致 guard/it 下沉），语义等价。语义：map 非空时把
    // (key, m_total, m_curInside) 写入 Statistics DB 流（msg 0xe1, LOGDB_Q=4），
    // 随后归零 m_total（ORIG 清节点 +0xc）。
    int nSize = m_inoutMap.size();
    if (nSize <= 0)
        return 1;
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("Statistics.cpp", 0x4b5), true);
    **guard << 0xe1;
    **guard << -1;
    **guard << nSize;
    // [b28d-59] it 提到 for 外（同 InsertDungeonEntrance 的 ORIG 槽序）。
    std::map<int, STDungeonInoutCnt>::iterator it = m_inoutMap.begin();
    for (; it != m_inoutMap.end(); ++it)
    {
        **guard << it->first;
        **guard << it->second.m_total;       // 节点 +0xc
        **guard << it->second.m_curInside;   // 节点 +0x10
    }
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::LOGDB_Q, guard);
    for (std::map<int, STDungeonInoutCnt>::iterator it = m_inoutMap.begin();
         it != m_inoutMap.end(); ++it)
        it->second.m_total = 0;
    return 1;
}

CDungeonEntranceLog* GetInstanceDungeonEntranceLog()
{
    // ORIG _Z29GetInstanceDungeonEntranceLogv：guard 单例。
    static CDungeonEntranceLog instance;
    return &instance;
}
