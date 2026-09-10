// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 DB_LinkMisc4/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r 还原 —— LinkMisc4 批次 DB 请求 TU（4 个 makeRequest）。
// 逐函数对照 docs/class_func_reports/{DB_EventGrowthWeaponReset,
// DB_EventUpdateGrowthWeaponTimepiece,DB_ReqChangeCharacName,
// DB_TestTradeRestrict}/makeRequest.md。语义：向 DB 队列（QUEUE_IDX = 2 = DB_Q）
// 发一条 CStreamGuard 请求；表头值、字段写入顺序、Acquire 行号常量均取自报告。
// 声明唯一来源为各自同名头文件。
#include <cstring>

#include "CStreamGuard.h"
#include "StreamPool.h"
#include "MsgQueueMgr.h"
#include "GlobalData.h"
#include "CEnvironment.h"
#include "SigTypes.h"
#include "DB_EventGrowthWeaponReset.h"
#include "DB_EventUpdateGrowthWeaponTimepiece.h"
#include "DB_ReqChangeCharacName.h"
#include "DB_TestTradeRestrict.h"
#include "DBDispatcher.h"
#include "DBMgr.h"
#include "MySQL.h"
#include <map>

// ============================================================================
// DB_EventGrowthWeaponReset::makeRequest @ 0x08449980
// mangled _ZN25DB_EventGrowthWeaponReset11makeRequestEj
// Acquire("DBThread.cpp", 0xb95e)；<<0x316(int) → <<-1 → <<accId(u32 0x080e2292)
// ============================================================================
#include "DB_LinkMisc4/makeRequest.inc"
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0xb95e), true);
    guard.operator*()->operator<<(0x316);
    guard.operator*()->operator<<(-1);
    guard.operator*()->operator<<(accId);       // operator<<(unsigned int) 0x080e2292
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

// ============================================================================
// DB_EventUpdateGrowthWeaponTimepiece::makeRequest @ 0x084497ea
// mangled _ZN35DB_EventUpdateGrowthWeaponTimepiece11makeRequestEjsi
// Acquire("DBThread.cpp", 0xb941)；<<0x315 → <<-1 → <<accId(u32) →
// <<timepiece(short 0x0822ad66) → <<value(int 0x080c8c56)
// ============================================================================
#include "DB_LinkMisc4/makeRequest_1.inc"
    unsigned int accId, short timepiece, int value)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0xb941), true);
    guard.operator*()->operator<<(0x315);
    guard.operator*()->operator<<(-1);
    guard.operator*()->operator<<(accId);       // operator<<(unsigned int) 0x080e2292
    guard.operator*()->operator<<(timepiece);   // operator<<(short) 0x0822ad66
    guard.operator*()->operator<<(value);       // operator<<(int) 0x080c8c56
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

// ============================================================================
// DB_ReqChangeCharacName::makeRequest @ 0x08426590
// mangled _ZN22DB_ReqChangeCharacName11makeRequestEiiPKci
// Acquire("DBThread.cpp", 0x64de)；<<0xc3(int) → <<accId(int) →
// SIG_CHANGE_CHARAC_NAME 权威布局见 SigTypes_A.h（总 0x30），按报告偏移写入。
// ============================================================================
// [SweepDBLoadEtcMain 本波] 旧 [b28d-L1]「N7/P168 封顶」**作废**，DIFF→AE：
//   ① +0x2c 改走 SigTypes_A.h 匿名 union `m_group` 视角（完整类型成员直访问，
//      复现 ORIG 08426655 `mov 0x378(%eax),%eax; mov %eax,%edx; mov -0xc(%ebp),%eax;
//      mov %dl,0x2c(%eax)`；旧局部尾视图 cast 恒产 `mov %al,0x2c(%ebx)` + 指针
//      %ebx 跨 call 缓存，探针 /tmp/p211p.cpp f1↔f3）；
//   ② +0x24 改走 `m_arg24u` unsigned 视角（P211 档③ 异型 ⇒ RHS-first+edx 中转，
//      复现 ORIG 0842663c `mov 0x14(%ebp),%edx; mov -0xc(%ebp),%eax; mov %edx,0x24(%eax)`；
//      探针 /tmp/p211q.cpp f1(int←int LHS)↔f2(uint←int RHS)）。
// 两处 union 视角均 sizeof 0x30 / 偏移 / 对齐 / mangled 零变化。
#include "DB_LinkMisc4/makeRequest_2.inc"
                                         char const* name, int flag)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x64de), true);
    guard.operator*()->operator<<(0xc3);
    guard.operator*()->operator<<(accId);       // operator<<(int) 0x080c8c56

    // ORIG 请求体 = SIG_CHANGE_CHARAC_NAME（SigTypes_A.h，总 0x30）：
    // +0x00 m_characNo ← characIdx / +0x04 m_name strncpy 0x1d /
    // +0x24 m_arg24 ← flag / +0x2c m_arg2c 低字节 ← 渠道组
    // （CEnvironment+0x378 = m_serverEnvir.m_serverGroup，ORIG mov %dl,0x2c）。
    SIG_CHANGE_CHARAC_NAME* body =
        guard.operator->()->GetInBuffer<SIG_CHANGE_CHARAC_NAME>();
    *reinterpret_cast<int*>(body) = characIdx;
    strncpy(body->m_name, name, 0x1d);
    // +0x24：走 SigTypes_A.h 匿名 union 的 unsigned 视角（P211 档③：字段与 RHS
    // 异型 ⇒ RHS-first+edx 中转），复现 ORIG 0842663c `mov 0x14(%ebp),%edx;
    // mov -0xc(%ebp),%eax; mov %edx,0x24(%eax)`；同字段 _sendResult 侧用 int
    // 视角保持 LHS-first（ORIG 0842673a），探针 /tmp/p211q.cpp f1↔f2。
    body->m_arg24u = flag;
    // +0x2c 低字节：走 SigTypes_A.h 的匿名 union `m_group` 视角（P235），完整
    // 类型成员直访问复现 ORIG 08426655 `mov %dl,0x2c(%eax)`（指针 cast 视图形
    // 恒产 `mov %al,0x2c(%ebx)` 并丢 edx 中转，探针 /tmp/p211p.cpp f1↔f3）。
    body->m_group =
        static_cast<char>(G_CEnvironment()->m_serverEnvir.m_serverGroup);

    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

// ============================================================================
// DB_TestTradeRestrict::makeRequest @ 0x0844a132
// mangled _ZN20DB_TestTradeRestrict11makeRequestEj
// Acquire("DBThread.cpp", 0xba73)；<<0x31c(int) → <<-1 → <<accId(u32)
// ============================================================================
#include "DB_LinkMisc4/makeRequest_3.inc"
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0xba73), true);
    guard.operator*()->operator<<(0x31c);
    guard.operator*()->operator<<(-1);
    guard.operator*()->operator<<(accId);       // operator<<(unsigned int) 0x080e2292
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

// ============================================================================
// DB_EventGrowthWeaponReset::dispatch @ 0x084498fe
// mangled _ZN25DB_EventGrowthWeaponReset8dispatchEiiP6Stream
// GetDBHandle(DB_HANDLE_9, 0) → Stream>>m_id(u32) →
// set_query("deLete from event_1207_growthweapon where m_id = %u", m_id) →
// exec(true) 成功返回 true。
// ============================================================================
#include "DB_LinkMisc4/dispatch.inc"
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_9, E_SERVER_GROUP_NONE);
    unsigned int m_id = 0;
    *stream >> m_id;
    db->set_query("deLete from event_1207_growthweapon where m_id = %u", m_id);
    if (!db->exec(true))
        return false;
    return true;
}

// ============================================================================
// DB_EventUpdateGrowthWeaponTimepiece::dispatch @ 0x084496f4
// mangled _ZN35DB_EventUpdateGrowthWeaponTimepiece8dispatchEiiP6Stream
// GetDBHandle(DB_HANDLE_9, 0) → >>m_id(u32) → >>timepiece(short) →
// >>infinityweapon_id(int)；id==0 时 UPDATE 不带 reward_occ_date，
// 否则带 reward_occ_date = now()；exec 成功返回 true。
// ============================================================================
#include "DB_LinkMisc4/dispatch_1.inc"
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_9, E_SERVER_GROUP_NONE);
    unsigned int m_id = 0;
    short timepiece = 0;
    int infinityweapon_id = 0;
    *stream >> m_id;
    *stream >> timepiece;
    *stream >> infinityweapon_id;
    if (infinityweapon_id != 0)
    {
        db->set_query("upDate event_1207_growthweapon set  reward_occ_date = now(), "
                      "timepiece = %d, infinityweapon_id = %d  where m_id = %u",
                      (int)timepiece, infinityweapon_id, m_id);
    }
    else
    {
        db->set_query("upDate event_1207_growthweapon set  timepiece = %d, "
                      "infinityweapon_id = %d  where m_id = %u",
                      (int)timepiece, infinityweapon_id, m_id);
    }
    if (!db->exec(true))
        return false;
    return true;
}

// ============================================================================
// DB_TestTradeRestrict::dispatch @ 0x0844a0be
// mangled _ZN20DB_TestTradeRestrict8dispatchEiiP6Stream
// GetDBHandle(DB_HANDLE_3, 0) → >>charac_no(u32) → UPDATE trade_restrict=1 →
// exec 后恒返回 true。
// ============================================================================
#include "DB_LinkMisc4/dispatch_2.inc"
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_3, E_SERVER_GROUP_NONE);
    unsigned int charac_no = 0;
    *stream >> charac_no;
    db->set_query("upDate user_items set trade_restrict = 1 where charac_no = %u", charac_no);
    db->exec(true);
    return true;
}

// ============================================================================
// DB_ReqChangeCharacName::dispatch @ 0x084262ea
// mangled _ZN22DB_ReqChangeCharacName8dispatchEiiP6Stream
// 基类 DBDispatcher::dispatch 校验失败 → false；随后：
// GetDBHandle(DB_HANDLE_2,0) → GetOutBuffer<SIG_CHANGE_CHARAC_NAME>() 读请求体 →
// escape_string 名字 → env+0x68 m_dbConnInfo[DB_HANDLE_2].m_db(+0x42) 库名 →
// _hasSameCharacName 重名检查（!=0 直接 _sendResult 返回）→
// UPDATE charac_info；exec&&affected!=0 失败 → _sendResult(2) →
// GetDBHandle(DB_HANDLE_8,0) 读 guild_member.guild_id（失败/无行清零 m_arg28）→
// _sendResult(slot, 0)。
// ============================================================================
#include "DB_LinkMisc4/dispatch_3.inc"
{
    if (!DBDispatcher::dispatch(userIdx, slot, stream))
        return false;

    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2, E_SERVER_GROUP_NONE);
    SIG_CHANGE_CHARAC_NAME* sig = stream->GetOutBuffer<SIG_CHANGE_CHARAC_NAME>();

    char name_buf[60] = { '\0' };   // 转义缓冲区（ORIG rep stos 清零 60 字节）
    db->escape_string(name_buf, sig->m_name);

    // env+0x68 = m_dbConnInfo：ORIG 直访成员（经 G_CEnvironment()+0x68 做
    // map::operator[] this，无 accessor call），故禁 get_db_conn_info() accessor。
    typedef std::map<ENUM_DB_HANDLE_IDX, STDBConnInfo> DbConnMap;
    ENUM_DB_HANDLE_IDX connIdx = DB_HANDLE_2;   // ORIG 栈上局部初始值 2
    int ret = _hasSameCharacName(
        db,
        G_CEnvironment()->m_dbConnInfo[connIdx].m_db,
        name_buf);
    if (ret != 0)
    {
        _sendResult(sig, slot, ret);
        return true;
    }

    db->set_query("upDate charac_info set charac_name='%s' where charac_no=%d and delete_flag=0",
                  name_buf, sig->m_characNo);
    if (!(db->exec(true) && db->getAffectedRowCount() != 0))
    {
        _sendResult(sig, slot, 2);
        return true;
    }

    MySQL* guildDb = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_8, E_SERVER_GROUP_NONE);
    // ORIG movzbl 0x2c(%eax)（8 位读）；m_arg2c 为 uint 成员，用 P82 尾部视图折叠为单 disp。
    struct ChangeCharacNameByteView
    {
        char m_pad[0x2c];
        unsigned char m_serverId;
    };
    guildDb->set_query("seLect guild_id from guild_member where charac_no=%d and member_flag=1 "
                       "and server_id=%d",
                       sig->m_characNo,
                       reinterpret_cast<ChangeCharacNameByteView*>(sig)->m_serverId);
    sig->m_arg28 = 0;
    if (!(guildDb->exec(true) && guildDb->fetch()))
        sig->m_arg28 = 0;
    if (guildDb->get_n_rows() == 0)
        sig->m_arg28 = 0;
    else if (!guildDb->get_uint(0, sig->m_arg28))
        sig->m_arg28 = 0;
    _sendResult(sig, slot, 0);
    return true;
}

// ============================================================================
// DB_ReqChangeCharacName::_hasSameCharacName @ 0x084267c2
// mangled _ZN22DB_ReqChangeCharacName18_hasSameCharacNameEP5MySQLPKcS3_
// set_query("seLect delete_flag from %s.charac_info where charac_name='%s'")；
// exec 失败 → 2；无行 → 0；fetch/get_int 失败 → 2；
// delete_flag==0 → 0x14，delete_flag!=0 → 0x18。
// ============================================================================
// [SweepDBLoadEtcMain 本波] 旧 [b28d-L1]「N93/P168a④ 封顶」**作废**，DIFF→AE：
//   ① 短路守卫改失败极性 `if (!(db->fetch() && db->get_int(0, delete_flag))) return 2;`
//      ⇒ 复现 ORIG 08426830/08426851 双 `xor $1,%eax; test %al,%al` 寄存器物化
//      与 08426864 `test %al,%al; je`（旧具名成功 bool 多 `movb %al,-0x9;
//      cmpb $0,-0x9` 栈化物化，P226/P316 同族）；
//   ② 尾部三目 `delete_flag==0?0x14:0x18` 改 if-return 链 ⇒ 复现 ORIG 08426874
//      `test %eax,%eax; je <0x14 块>`（三目恒反产 `jne <0x18>`，P47 镜像）。
#include "DB_LinkMisc4/_hasSameCharacName.inc"
{
    db->set_query("seLect delete_flag from %s.charac_info where charac_name='%s'", table, name);
    if (!db->exec(true))
        return 2;

    int delete_flag = 0;
    if (db->get_n_rows() != 0)
    {
        // ORIG 08426830/08426851 双 `xor $1,%eax; test %al,%al` 寄存器物化、
        // 汇聚后 `test %al,%al; je <成功>`，全程无 bool 槽 ⇒ 原码是失败极性
        // 短路守卫（P226/P316 语句表达式同族），非具名成功 bool（旧形多
        // `mov %al,-0x9; cmpb $0,-0x9` 两步且槽 -0x9）。
        if (!(db->fetch() && db->get_int(0, delete_flag)))
            return 2;
        // ORIG 08426874 `test %eax,%eax; je <0x14 块>`（==0 跳、!=0 fall-through
        // 到 0x18）⇒ 三目形恒反产 `jne <0x18>`（P47 镜像），改 if-return 链。
        if (delete_flag != 0)
            return 0x18;
        return 0x14;
    }
    return 0;
}

// ============================================================================
// DB_ReqChangeCharacName::_sendResult @ 0x084266a4
// mangled _ZN22DB_ReqChangeCharacName11_sendResultEPK22SIG_CHANGE_CHARAC_NAMEii
// Acquire("DBThread.cpp", 0x64f8) → <<0xc3 → <<slot →
// GetInBuffer<SIG_CHANGE_CHARAC_NAME>() 回填请求体（+0x28←sig+0x24、+0x24←result、
// +0x00←sig+0x00、+0x2c←sig+0x28、strncpy name 0x1d）→ put(DISPATCH_Q)。
// ============================================================================
// [SweepDBLoadEtcMain 本波] 旧 [b28d-L1]「N18a 封顶」**作废**，DIFF→AE：
//   读侧改 `out->m_arg28 = sig->m_arg24u;`（uint←uint 同型 ⇒ 单段
//   `mov 0x24(%eax),%edx`，复现 ORIG 0842672e；int 视角恒两段 regmove，
//   P211 档③ 同型化即消中转）。写侧 `out->m_arg24 = result;` 保持 int 视角
//   LHS-first（ORIG 0842673a）。
#include "DB_LinkMisc4/_sendResult.inc"
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x64f8), true);
    guard.operator*()->operator<<(0xc3);
    guard.operator*()->operator<<(slot);
    SIG_CHANGE_CHARAC_NAME* out = guard.operator->()->GetInBuffer<SIG_CHANGE_CHARAC_NAME>();
    // ORIG 0842672e `mov 0x24(%eax),%edx` 单段直落：源/目标同型 unsigned
    // （m_arg28 uint ← m_arg24u uint 视角）；int 视角恒产 `mov 0x24(%eax),%eax;
    // mov %eax,%edx` 两段（旧 N18a 判词作废，P211 档③ 同型化即消中转）。
    out->m_arg28 = sig->m_arg24u;
    out->m_arg24 = result;
    out->m_characNo = sig->m_characNo;
    out->m_arg2c = sig->m_arg28;
    strncpy(out->m_name, sig->m_name, 0x1d);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
}
