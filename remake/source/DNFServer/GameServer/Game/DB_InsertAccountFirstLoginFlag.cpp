// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 DB_InsertAccountFirstLoginFlag/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r 还原 —— DB_InsertAccountFirstLoginFlag（账号首次登录标记 DB 请求，DBThread 派发）。
// 逐函数对照 docs/class_func_reports/DB_InsertAccountFirstLoginFlag/。
// 语义：makeRequest（0x0843e79a）向 DB 队列发一条请求，表头 0x25e + 5 参数；
// dispatch（0x0843e636）读流并据此向事件表执行 "insert into ...(m_id,server_id,
// charac_no, occ_date) values(...)"。
#include "DB_InsertAccountFirstLoginFlag.h"

#include "CStreamGuard.h"
#include "StreamPool.h"
#include "MsgQueueMgr.h"
#include "GlobalData.h"
#include "DBMgr.h"
#include "MySQL.h"
#include "CDataManager.h"
#include "CEventScriptMng.h"
#include "SyncScriptDeps.h"         // toTString(const std::string&)
#include "CInventory.h"
#include "Stream.h"

#include <cstdio>

// ============================================================================
// DB_InsertAccountFirstLoginFlag::makeRequest @ 0x0843e79a
// mangled _ZN30DB_InsertAccountFirstLoginFlag11makeRequestEijiji
// ============================================================================
#include "DB_InsertAccountFirstLoginFlag/makeRequest.inc"
                                                 unsigned int p4, int p5)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x9503), true);
    guard.operator*()->operator<<(0x25e);
    guard.operator*()->operator<<(p1);
    guard.operator*()->operator<<(p2);
    guard.operator*()->operator<<(p3);
    guard.operator*()->operator<<(p4);
    guard.operator*()->operator<<(p5);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

// ============================================================================
// DB_InsertAccountFirstLoginFlag::dispatch @ 0x0843e636
// mangled _ZN30DB_InsertAccountFirstLoginFlag8dispatchEiiP6Stream
// ============================================================================
// 逐条对照 ORIG 0x0843e636..0x0843e799：槽图 db(-0x24)/m_id(-0x28)/characNo(-0x2c)/
// serverID(-0x30)/eventType(-0x34 未初始化)/sm(-0x20)/ent(-0x1c)；读取序
// m_id,serverID,characNo,eventType（lea 序 0x0843e674/e686/e698/e6aa）；条件形
// `if(A||B){...}else{return 0}`（ORIG 0x0843e6db je / 0x0843e6e5 jne 双落点）；
// set_query 中间值全部表达式内联（ORIG edi/esi/ebx 寄存器直传，无栈槽往返 ⇒ P0）；
// 尾形 `if(!exec)return 0; return 1;`（P101，ORIG 0x0843e77f xor/test/je）。
#include "DB_InsertAccountFirstLoginFlag/dispatch.inc"
                                               Stream* s)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)9,
                                                  (ENUM_SERVER_GROUP)0);
    unsigned int m_id = 0;       // -0x28：第 1 次 >>（unsigned int&）
    unsigned int characNo = 0;   // -0x2c：第 3 次 >>（unsigned int&）
    int          serverID = 0;   // -0x30：第 2 次 >>（int&）
    int          eventType;      // -0x34：第 4 次 >>（ORIG 无清零，照抄 P171）
    *s >> m_id;
    *s >> serverID;
    *s >> characNo;
    *s >> eventType;

    EventClassify::CEventScriptMng* sm = static_cast<EventClassify::CEventScriptMng*>(
        G_CDataManager()->get_event_script_mng());
    EventClassify::CEventEntity* ent = 0;
    if (eventType == 0x191 || eventType == 0x192)
    {
        ent = sm->get_event_entity(eventType);
        if (ent == 0)
            return 0;
    }
    else
        return 0;

    // ORIG 0x0843e744：toTString(const std::string&) 转换表名（get_db_table_name
    // 返回 &m_dbTableName，调用方按 std::string& 解释）；实参表达式内联（P0）。
    db->set_query("inSert into %s(m_id,server_id, charac_no, occ_date) "
                  "values(%s,%d, %u, now())",
                  toTString(ent->get_db_table_name()),
                  NumberToString(m_id, 0), serverID, characNo);
    if (!db->exec(true))
        return 0;
    return 1;
}
