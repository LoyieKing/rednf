// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CLevelDungeonPlayStatistic/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
#include "CLevelDungeonPlayStatistic.h"

#include "CStreamGuard.h"
#include "GlobalData.h"
#include "MsgQueueMgr.h"
#include "StreamPool.h"

// [b28c-32] verdict=DIFF：N18a（ORIG it->second 成员读发 8 字节两段载入，4.4 恒折单条）。
#include "CLevelDungeonPlayStatistic/InsertDatabase.inc"
{
    if (m_levelPlayMap.size() == 0)
        return;
    // ORIG 栈槽（低→高）：end@-0x18、it@-0x14、guard@-0x10；
    // gcc4.4 按声明序从高地址往下分配，故声明序 guard → it → endIt。
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("Statistics.cpp", 0x569),
                       true);
    **guard << 0x107;
    **guard << -1;
    **guard << (int)m_levelPlayMap.size();
    std::map<int, stDungeonPlayInfo>::iterator it = m_levelPlayMap.begin();
    std::map<int, stDungeonPlayInfo>::iterator endIt = m_levelPlayMap.end();
    while (it != endIt)
    {
        // pair 起始即 key；value（stDungeonPlayInfo）@+4，故 value+4=m_userCount、
        // value+8=m_playCount（ORIG 经 iterator::operator-> 读 +0/+4/+8）。
        **guard << it->first;
        **guard << it->second.m_userCount;
        **guard << it->second.m_playCount;
        ++it;
    }
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::LOGDB_Q, guard);
    m_levelPlayMap.clear();
}
