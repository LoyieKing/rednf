// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CDungeonExpStatistic/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
#include "CDungeonExpStatistic.h"

#include "AdditionalDungeonInfo.h"
#include "CStreamGuard.h"
#include "GlobalData.h"
#include "MsgQueueMgr.h"
#include "StreamPool.h"

// ============================================================================
// AdditionalDungeonInfo::~AdditionalDungeonInfo —— ORIG D1/D2 0x08a6887c（W，空体）。
// 非多态 POD（8 字节两 int，无 vtable/typeinfo）；属主 TU 提供真实空析构，
// 供 vector<AdditionalDungeonInfo>（QuestScript::m_field16c）析构实例化引用。
// ============================================================================


#include "CDungeonExpStatistic/InsertDatabase.inc"
{
    // [B24D判词 N] 语义正确：m_map 判空早返+逐记录构包+DB insert 全链；
    //   P167 调用点差集 31/31 CLEAR；diff=地址/EH lowering。
    if (m_map.size() == 0)
        return;
    // e1/e2/e3 按 ORIG 栈槽分配序（gcc4.1 由低到高 e3/e2/e1）倒序声明，
    // gcc4.4 按声明序从高地址往下分配，倒排后槽位与 ORIG 对齐。
    stDungeonExp e3;
    stDungeonExp e2;
    stDungeonExp e1;
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("Statistics.cpp", 0x59c),
                       true);
    **guard << 0x116;
    **guard << -1;
    **guard << (int)m_map.size();
    // b28d-61 [P212/N98：4.4 槽按声明序先声明深]：ORIG it@-0x38（begin 先调用）、
    // endIt@-0x3c；旧形 endIt 先声明致两槽互换（NEAR 9）。it 提前声明（仍直构，
    // 非先声明后赋值，不触发 P50 默认 ctor 弱实例）后槽位与 ORIG 一致。
    std::map<int, stDungeonExp>::iterator it = m_map.begin();
    std::map<int, stDungeonExp>::iterator endIt = m_map.end();
    for (; it != endIt; ++it)
    {
        **guard << it->first;
        e1 = it->second;
        **guard << e1.m_exp;
        e2 = it->second;
        **guard << e2.m_count;
        e3 = it->second;
        **guard << e3.m_clearTimes;
    }
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::LOGDB_Q, guard);
    m_map.clear();
}
