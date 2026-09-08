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
void DB_EventGrowthWeaponReset::makeRequest(unsigned int accId)
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
void DB_EventUpdateGrowthWeaponTimepiece::makeRequest(
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
// [b28d-L1] N7/P168 封顶：ORIG 0x08426590 尾部 `mov -0xc(%ebp),%eax; mov %eax,%edx; mov -0xc(%ebp),%eax; mov %dl,0x2c(%eax)`（body 双读 + edx 中转）；4.4 恒用 %ebx 缓存（多 push/pop）。P167 CLEAR（14/14）语义等价；其余为字符串/全局 reloc 归一。
void DB_ReqChangeCharacName::makeRequest(int accId, int characIdx,
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
    body->m_arg24 = flag;
    // +0x2c 仅写低字节：ORIG 为 char 成员 disp 折叠（mov %dl,0x2c），
    // 用局部尾部视图模拟（m_arg2c 在共享头为 uint，直写会变 32 位 store）。
    struct ChangeCharacNameTail
    {
        char m_pad[0x2c];
        char m_group;
    };
    reinterpret_cast<ChangeCharacNameTail*>(body)->m_group =
        static_cast<char>(G_CEnvironment()->m_serverEnvir.m_serverGroup);

    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

// ============================================================================
// DB_TestTradeRestrict::makeRequest @ 0x0844a132
// mangled _ZN20DB_TestTradeRestrict11makeRequestEj
// Acquire("DBThread.cpp", 0xba73)；<<0x31c(int) → <<-1 → <<accId(u32)
// ============================================================================
void DB_TestTradeRestrict::makeRequest(unsigned int accId)
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
bool DB_EventGrowthWeaponReset::dispatch(int userIdx, int slot, Stream* stream)
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
bool DB_EventUpdateGrowthWeaponTimepiece::dispatch(int userIdx, int slot, Stream* stream)
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
bool DB_TestTradeRestrict::dispatch(int userIdx, int slot, Stream* stream)
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
bool DB_ReqChangeCharacName::dispatch(int userIdx, int slot, Stream* stream)
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
// [b28d-L1] N93/P168a④ 封顶：ORIG 0x084267c2 用 `xor $1,%eax; test %al,%al` 寄存器物化 + 槽 -0xc；4.4 恒 `movb %al,-0x9; cmpb $0,-0x9` 栈化物化且槽 -0x10（P168a④ 实证：无绑定不可达）。P167 CLEAR（5/5）语义等价。
int DB_ReqChangeCharacName::_hasSameCharacName(MySQL* db, char const* table, char const* name)
{
    db->set_query("seLect delete_flag from %s.charac_info where charac_name='%s'", table, name);
    if (!db->exec(true))
        return 2;

    	    int delete_flag = 0;
    if (db->get_n_rows() != 0)
    {
        bool ok = db->fetch() && db->get_int(0, delete_flag);
        if (ok)
            return delete_flag == 0 ? 0x14 : 0x18;
        return 2;
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
// [b28d-L1] N18a 封顶：ORIG 0x084266a4 段 `mov 0x24(%eax),%edx`（单段直落 %edx）；4.4 恒 `mov 0x24(%eax),%eax; mov %eax,%edx`（+1 regmove）。P167 CLEAR（13/13）语义等价。
void DB_ReqChangeCharacName::_sendResult(SIG_CHANGE_CHARAC_NAME const* sig, int slot, int result)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x64f8), true);
    guard.operator*()->operator<<(0xc3);
    guard.operator*()->operator<<(slot);
    SIG_CHANGE_CHARAC_NAME* out = guard.operator->()->GetInBuffer<SIG_CHANGE_CHARAC_NAME>();
    out->m_arg28 = sig->m_arg24;
    out->m_arg24 = result;
    out->m_characNo = sig->m_characNo;
    out->m_arg2c = sig->m_arg28;
    strncpy(out->m_name, sig->m_name, 0x1d);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
}
