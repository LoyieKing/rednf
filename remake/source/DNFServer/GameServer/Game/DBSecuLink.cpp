// ============================================================================
// DBSecuLink.cpp —— DBSecuLink 批（DB_Secu / 公会 DB makeRequest + Secu_DataControl）
// 独占 TU，覆盖 ORIG 符号（nm /home/wangyilei/下载/df_game_r 实证，全部 T）：
//   T 0x0843cc06 _ZN17DB_CheckJoinGuild11makeRequestEiP20SIG_CHECK_JOIN_GUILD
//   T 0x0843df0e _ZN16DB_DenyJoinGuild11makeRequestEijj
//   T 0x0843dd7c _ZN16DB_GuildJoinList11makeRequestEij17ENUM_SERVER_GROUP
//   T 0x0842b57a _ZN20DB_SecurityCardIssue11makeRequestEijPKcS1_S1_S1_S1_
//   T 0x0842bb5c _ZN23DB_SecurityCardActivate11makeRequestEijPKcb
//   T 0x08286a30 _ZN16Secu_DataControl14SetControlDataEP5CUserhtt
// 引用方：Dispatchers.cpp extern "C" asm 声明 / DspD3_Integ_check.cpp 直调。
// 包构建约定同 W7LinkDB.cpp（自 docs/class_func_reports/<类>/makeRequest.md
// 逐指令转写）：StreamPool::Acquire("DBThread.cpp", 行号) → CStreamGuard(stream,
// true) → guard << opcode [/ << 参数] [→ GetInBuffer<SIG_*> 填 payload] →
// MsgQueueMgr::put(DB_Q, guard)。
// ============================================================================

#include "GlobalData.h"
#include "StreamPool.h"
#include "Stream.h"
#include "MsgQueueMgr.h"
#include "CStreamGuard.h"
#include "SigTypes.h"
#include "Secu_DataControl.h"
#include "CUser.h"
#include "CHackAnalyzer.h"
#include "CMCAPManager.h"
#include "CEnvironment.h"        // ENUM_SERVER_GROUP
#include "GameTypes.h"           // cMyTrace
#include "CInventory.h"          // NumberToString(unsigned int, int) 声明
#include "DBDispatcher.h"      // DBDispatcher 基类（virtual dispatch）
#include "DBMgr.h"             // DBMgr::GetDBHandle
#include "MySQL.h"             // MySQL 查询接口
#include <time.h>              // time / localtime_r

#include <string.h>
#include <algorithm>

// ---- DB 类声明（W8LT_DBTask.cpp 仅空壳 ctor，未声明 makeRequest/dispatch；
//      局部声明与 ORIG mangled / W7LinkDB.cpp 模式一致；dispatch 为 ORIG
//      vtable slot2 虚重写，见各函数注释）----
class DB_CheckJoinGuild : public DBDispatcher
{
public:
    DB_CheckJoinGuild() {}
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG T 0x0843c98e
    static void makeRequest(int uid, SIG_CHECK_JOIN_GUILD* sig);   // _ZN...EiP20SIG...
};

class DB_DenyJoinGuild : public DBDispatcher
{
public:
    DB_DenyJoinGuild() {}
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG T 0x0843de6c
    static void makeRequest(int uid, unsigned int guildKey, unsigned int applicant);
                                                                   // _ZN...Eijj
};

class DB_GuildJoinList : public DBDispatcher
{
public:
    DB_GuildJoinList() {}
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG T 0x0843d87c
    static void makeRequest(int uid, unsigned int guildKey, ENUM_SERVER_GROUP group);
                                                                   // _ZN...Eij17ENUM...
};

class DB_SecurityCardIssue : public DBDispatcher
{
public:
    DB_SecurityCardIssue() {}
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG T 0x0842af94
    void _sendResult(int param_1, char const* param_2, char const* param_3,
                     char const* param_4, char const* param_5,
                     ENUM_ERROR error);                            // OrigDBInter19.cpp
    static void makeRequest(int uid, unsigned int accId,
                            char const* certKey, char const* serverKey,
                            char const* phone, char const* sha1Hex,
                            char const* cardData);                 // _ZN...EijPKcS1_S1_S1_S1_
};

class DB_SecurityCardActivate : public DBDispatcher
{
public:
    DB_SecurityCardActivate() {}
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG T 0x0842b86c
    static void makeRequest(int uid, unsigned int accId,
                            char const* certKey, bool activated);  // _ZN...EijPKcb
};

// ---- ORIG 全局枚举（saveSecurityCardHistory 形参域；modify_type 列 0/1）----
enum ENUM_SECURITYCARD_MOD_TYPE
{
    ENUM_SECURITYCARD_MOD_TYPE_NONE = 0,
    ENUM_SECURITYCARD_MOD_TYPE_ACTIVATE = 1,
};

// SIG_SECURITY_CARD_ACTIVATE 载荷视图（ORIG dispatch @0x842b86c 偏移实证）：
// +0x00 m_id | +0x04 cert_key[6] | +0x0a 激活结果回写 | +0x0b 卡状态标志。
struct SecuActivateView
{
    unsigned int m_id;        // +0x00
    char m_certKey[6];        // +0x04（strncpy 6）
    unsigned char m_field0a;  // +0x0a（dispatch 回写 ok）
    unsigned char m_field0b;  // +0x0b（history 模式选择）
};
static_assert(sizeof(SecuActivateView) == 0xc, "SecuActivateView ABI drift");

// SIG_SECURITY_CARD_ISSUE 载荷视图（ORIG dispatch @0x842af94 偏移实证）：
// +0x00 m_id | +0x04 server_key[0x21] | +0x25 serial[0xc] | +0x31 cert_key[6] |
// +0x37 card[0x27] | +0x5e q_pos[5] | +0x63 phone[0x29] | +0x8c error。
struct SecuIssueView
{
    unsigned int m_id;        // +0x00
    char m_serverKey[0x21];   // +0x04
    char m_serial[0xc];       // +0x25
    char m_certKey[6];        // +0x31
    char m_card[0x27];        // +0x37
    char m_qPos[5];           // +0x5e
    char m_phone[0x29];       // +0x63
    ENUM_ERROR m_error;       // +0x8c
};
static_assert(sizeof(SecuIssueView) == 0x90, "SecuIssueView ABI drift");

struct GuildJoinListEntry
{
    unsigned int m_characNo;   // +0x00（get_uint col0）
    char m_name[0x1e];         // +0x04（get_str col0）
    char m_job;                // +0x22（get_byte col2）
    char m_growType;           // +0x23（get_byte col3）
    char m_sex;                // +0x24（get_byte col4）
    char m_lev;                // +0x25（get_byte col1）
    char m_memo[0x1a];         // +0x26（get_str col1）
    int m_remainTime;          // +0x40（now - occ_time）
};
static_assert(sizeof(GuildJoinListEntry) == 0x44, "GuildJoinListEntry ABI drift");

struct GuildJoinListView
{
    int m_count;                       // +0x00
    GuildJoinListEntry m_entry[0x32];  // +0x04
};
static_assert(sizeof(GuildJoinListView) == 0xd4c, "GuildJoinListView ABI drift");
// SIG_CHECK_JOIN_GUILD +0x18 server_id 字段视图（ORIG dispatch @0x843c98e
// 单 disp 读 `mov 0x18(%eax),%eax` 实证；P1 组件视图折叠）。
struct CheckJoinGuildView
{
    char m_pad[0x18];
    int m_serverId;   // +0x18
};

// ---- 跨 TU 引用（定义在其它权威 TU；本 TU 仅声明，U 合规）----

// ---- 跨 TU 引用（定义在其它权威 TU；本 TU 仅声明，U 合规）----
void generateSecurityCardQuestion(char* out);             // OrigDBInter20.cpp
class DB_CancelRestingUserRestrict                        // MouseRegisterEJ.cpp
{
public:
    static void makeRequest(unsigned int uid, int param2);  // _ZN...Eji
};
class DB_CheckSecuReward                                  // MouseRegisterEJ.cpp
{
public:
    static void makeRequest(int param1, unsigned int uid, int param3);  // _ZN...Eiji
};

namespace {

void put_db_request(CStreamGuard& guard)
{
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

}  // namespace

// ----------------------------------------------------------------------------
// DB_CheckJoinGuild::makeRequest(int uid, SIG_CHECK_JOIN_GUILD* sig)
// ORIG @ 0x0843cc06（报告 DB_CheckJoinGuild/makeRequest.md）：
// Acquire("DBThread.cpp", 0x911a) → guard(stream,true) → << 0x216（op<<(int)）|
// << uid（op<<(int)）→ GetInBuffer<SIG_CHECK_JOIN_GUILD>()（memcpy 0x1c 字节，
// 无 memset）→ put(DB_Q)。无 -1 槽。
// ----------------------------------------------------------------------------
void DB_CheckJoinGuild::makeRequest(int uid, SIG_CHECK_JOIN_GUILD* sig)
{
    Stream* stream = GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x911a);
    CStreamGuard guard(stream, true);

    guard << 0x216;                     // op<<(int)
    guard << uid;                       // op<<(int)

    {
        SIG_CHECK_JOIN_GUILD* p = guard.GetInBuffer<SIG_CHECK_JOIN_GUILD>();
        memcpy(p, sig, 0x1c);           // sizeof(SIG_CHECK_JOIN_GUILD) == 0x1c
    }

    put_db_request(guard);
}

// ----------------------------------------------------------------------------
// DB_DenyJoinGuild::makeRequest(uint uid, uint guildKey, uint applicant)
// ORIG @ 0x0843df0e（报告 DB_DenyJoinGuild/makeRequest.md）：
// Acquire("DBThread.cpp", 0x92b4) → << 0x21c | << uid(int) | << guildKey(uint) |
// << applicant(uint) → put(DB_Q)。无 GetInBuffer。
// ----------------------------------------------------------------------------
void DB_DenyJoinGuild::makeRequest(int uid, unsigned int guildKey,
                                   unsigned int applicant)
{
    Stream* stream = GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x92b4);
    CStreamGuard guard(stream, true);

    guard << 0x21c;                     // op<<(int)
    guard << static_cast<int>(uid);     // op<<(int)
    guard << guildKey;                  // op<<(unsigned int)
    guard << applicant;                 // op<<(unsigned int)

    put_db_request(guard);
}

// ----------------------------------------------------------------------------
// DB_GuildJoinList::makeRequest(int uid, uint guildKey, ENUM_SERVER_GROUP group)
// ORIG @ 0x0843dd7c（报告 DB_GuildJoinList/makeRequest.md）：
// Acquire("DBThread.cpp", 0x9299) → << 0x21a | << uid(int) | << guildKey(uint) |
// << group（op<<(int)，参数槽按 int）→ put(DB_Q)。无 GetInBuffer。
// ----------------------------------------------------------------------------
void DB_GuildJoinList::makeRequest(int uid, unsigned int guildKey,
                                   ENUM_SERVER_GROUP group)
{
    Stream* stream = GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x9299);
    CStreamGuard guard(stream, true);

    guard << 0x21a;                     // op<<(int)
    guard << uid;                       // op<<(int)
    guard << guildKey;                  // op<<(unsigned int)
    guard << static_cast<int>(group);   // op<<(int)

    put_db_request(guard);
}

// ----------------------------------------------------------------------------
// DB_SecurityCardIssue::makeRequest(uint uid, uint accId, certKey, serverKey,
//                                   phone, sha1Hex, cardData)
// ORIG @ 0x0842b57a（报告 DB_SecurityCardIssue/makeRequest.md）：
// Acquire("DBThread.cpp", 0x6e0a) → << 0xfd（op<<(int)）| << uid(int) →
// GetInBuffer<SIG_SECURITY_CARD_ISSUE>()（0x90 字节，无 memset）填：
//   +0x00 = accId（uint）；strncpy(+0x31, certKey, 6)；strncpy(+0x04, serverKey,
//   0x21)；strncpy(+0x25, phone, 0xc)；strncpy(+0x37, cardData, 0x27)；
//   strncpy(+0x63, sha1Hex, 0x29)。→ put(DB_Q)。
// 注意 ORIG 汇编 strncpy 次序与反编译一致（cardData@+0x37 先于 sha1Hex@+0x63）。
// ----------------------------------------------------------------------------
void DB_SecurityCardIssue::makeRequest(int uid, unsigned int accId,
                                       char const* certKey, char const* serverKey,
                                       char const* phone, char const* sha1Hex,
                                       char const* cardData)
{
    Stream* stream = GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x6e0a);
    CStreamGuard guard(stream, true);

    guard << 0xfd;                      // op<<(int)
    guard << uid;                       // op<<(int)

    {
        unsigned char* p = reinterpret_cast<unsigned char*>(
            guard.GetInBuffer<SIG_SECURITY_CARD_ISSUE>());
        *reinterpret_cast<unsigned int*>(p + 0x00) = accId;
        strncpy(reinterpret_cast<char*>(p + 0x31), certKey, 6);
        strncpy(reinterpret_cast<char*>(p + 0x04), serverKey, 0x21);
        strncpy(reinterpret_cast<char*>(p + 0x25), phone, 0xc);
        strncpy(reinterpret_cast<char*>(p + 0x37), cardData, 0x27);
        strncpy(reinterpret_cast<char*>(p + 0x63), sha1Hex, 0x29);
    }

    put_db_request(guard);
}

// ----------------------------------------------------------------------------
// DB_SecurityCardActivate::makeRequest(uint uid, uint accId, certKey, activated)
// ORIG @ 0x0842bb5c（报告 DB_SecurityCardActivate/makeRequest.md）：
// Acquire("DBThread.cpp", 0x6e99) → << 0xff（op<<(int)）| << uid(int) →
// GetInBuffer<SIG_SECURITY_CARD_ACTIVATE>()（0xc 字节，无 memset）填：
//   +0x00 = accId（uint）；strncpy(+0x04, certKey, 6)；+0x0b(byte) = activated。
// → put(DB_Q)。
// ----------------------------------------------------------------------------
void DB_SecurityCardActivate::makeRequest(int uid, unsigned int accId,
                                          char const* certKey, bool activated)
{
    Stream* stream = GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x6e99);
    CStreamGuard guard(stream, true);

    guard << 0xff;                      // op<<(int)
    guard << uid;                       // op<<(int)
    {
        unsigned char* p = reinterpret_cast<unsigned char*>(
            guard.GetInBuffer<SIG_SECURITY_CARD_ACTIVATE>());
        *reinterpret_cast<unsigned int*>(p + 0x00) = accId;
        strncpy(reinterpret_cast<char*>(p + 0x04), certKey, 6);
        p[0x0b] = static_cast<unsigned char>(activated ? 1 : 0);
    }

    put_db_request(guard);
}

// ----------------------------------------------------------------------------
// Secu_DataControl::SetControlData(CUser* user, uchar type, ushort v1, ushort v2)
// ORIG @ 0x08286a30（报告 Secu_DataControl/SetControlData.md 全文转写）。
// 四分支（type 0..3），type==1/2/3 均要求 user->isGMUser()：
//   type 0（Hacktype 注入）：100 < v1 <= 0xfa0 且 v2 < 10 时 trace
//     "[SecuCommand] (mid:%s) Hacktype : %d     Cnt : %d "（line 0x1e）并循环
//     v2 次 getHackAnalyzer()->addServerHackCnt(user, v1, 1, 0, 0)；
//     结束后（无论条件）addServerHackCnt(user, 0x262, 1, 0, 0)。
//   type 1（CleanPad 点数）：v = clamp(v1, 0, 10000)（max/min int）；
//     trace "…CleanPad Set Point : %d"（line 0x32）；
//     m_mcap.setPoint((ushort)v)；若 getPoint() > 1000 →
//     setStdDropRate(100 - getPoint()/100)。
//   type 2（CleanPad Open）：trace "…CleanPad Open!"（line 0x40）；
//     reqHumanCertify4ClearMap(false)。
//   type 3（DropRate）：v = clamp(v1, 0, 100)；trace "…DropRate : %d"
//     （line 0x4a）；setStdDropRate(v)。
// cMyTrace 构造 (name=__PRETTY_FUNCTION__, line, 0)；mid = NumberToString(
// get_acc_id(), 0)。
// ----------------------------------------------------------------------------
void Secu_DataControl::SetControlData(CUser* user, unsigned char type,
                                      unsigned short v1, unsigned short v2)
{
    if (user == 0)
        return;

    switch (type)
    {
    case 0:  // Hacktype 注入
    {
        unsigned int hackType = static_cast<unsigned int>(v1);
        unsigned int cnt = static_cast<unsigned int>(v2);

        if (hackType > 100 && hackType <= 0xfa0 && cnt < 10)
        {
            char* mid = NumberToString(user->get_acc_id(), 0);
            cMyTrace trace(__PRETTY_FUNCTION__, 0x1e, 0);
            trace("[SecuCommand] (mid:%s) Hacktype : %d     Cnt : %d ",
                  mid, static_cast<int>(hackType), static_cast<int>(cnt));

            for (unsigned int i = 0; i < cnt; ++i)
            {
                user->getHackAnalyzer()->addServerHackCnt(
                    user, static_cast<WongWork::ENUM_HACKTYPE>(hackType), 1, 0, 0);
            }
        }

        user->getHackAnalyzer()->addServerHackCnt(
            user, static_cast<WongWork::ENUM_HACKTYPE>(0x262), 1, 0, 0);
        break;
    }

    case 1:  // CleanPad Set Point（仅 GM）
    {
        if (!user->isGMUser())
            break;

        int v = static_cast<int>(v1);
        v = std::max(v, 0);
        v = std::min(v, 10000);

        char* mid = NumberToString(user->get_acc_id(), 0);
        {
            cMyTrace trace(__PRETTY_FUNCTION__, 0x32, 0);
            trace("[SecuCommand] (mid:%s) CleanPad Set Point : %d", mid, v);
        }

        user->m_mcap.setPoint(static_cast<unsigned short>(v));

        unsigned int point = user->m_mcap.getPoint();
        if (point > 1000)
            user->setStdDropRate(100 - static_cast<int>(point / 100));
        break;
    }

    case 2:  // CleanPad Open（仅 GM）
    {
        if (!user->isGMUser())
            break;

        char* mid = NumberToString(user->get_acc_id(), 0);
        cMyTrace trace(__PRETTY_FUNCTION__, 0x40, 0);
        trace("[SecuCommand] (mid:%s) CleanPad Open!", mid);

        user->reqHumanCertify4ClearMap(false);
        break;
    }

    case 3:  // DropRate（仅 GM）
    {
        if (!user->isGMUser())
            break;

        int v = static_cast<int>(v1);
        v = std::max(v, 0);
        v = std::min(v, 100);

        char* mid = NumberToString(user->get_acc_id(), 0);
        cMyTrace trace(__PRETTY_FUNCTION__, 0x4a, 0);
        trace("[SecuCommand] (mid:%s) DropRate : %d", mid, v);

        user->setStdDropRate(v);
        break;
    }

    default:
        break;
    }
}
// ============================================================================
// SecurityCard 批自由函数（ORIG T 段；与 DB_SecurityCardIssue/Activate::dispatch
// 同属 DB_Secu 批，本 TU 独占）。
// ============================================================================

// ----------------------------------------------------------------------------
// saveSecurityCardHistory(MySQL*, uint m_id, ENUM_SECURITYCARD_MOD_TYPE mod)
// ORIG @ 0x0842ae3f：set_query("inSert into member_security_card_history(
// occ_time,m_id,modify_type) values(now(),%s,%d)", NumberToString(m_id,0), mod)
// → exec(true)。void。
// ----------------------------------------------------------------------------
void saveSecurityCardHistory(MySQL* db, unsigned int m_id,
                             ENUM_SECURITYCARD_MOD_TYPE mod)
{
    db->set_query(
        "inSert into member_security_card_history(occ_time,m_id,modify_type) "
        "values(now(),%s,%d)",
        NumberToString(m_id, 0), static_cast<int>(mod));
    db->exec(true);
}

// ----------------------------------------------------------------------------
// turnOnSecurityFlag(MySQL*, uint m_id) —— ORIG @ 0x0842ae90：
// char buf[0x14] 清零后 memcpy 13 字节 "member_login"（含 \0）；set_query(
// "upDate %s set security_flag=1 where m_id=%s and security_flag=0", buf,
// NumberToString(m_id,0)) → exec(true)。void。
// ----------------------------------------------------------------------------
void turnOnSecurityFlag(MySQL* db, unsigned int m_id)
{
    char table[0x14];
    memset(table, 0, 0x14);
    memcpy(table, "member_login", 0xd);

    db->set_query(
        "upDate %s set security_flag=1 where m_id=%s and security_flag=0",
        table, NumberToString(m_id, 0));
    db->exec(true);
}

// ----------------------------------------------------------------------------
// turnOffSecurityFlag(MySQL*, uint m_id) —— ORIG @ 0x0842af12：与 turnOn 同构，
// 仅 SQL 的 security_flag 目标值相反。
// ----------------------------------------------------------------------------
void turnOffSecurityFlag(MySQL* db, unsigned int m_id)
{
    char table[0x14];
    memset(table, 0, 0x14);
    memcpy(table, "member_login", 0xd);

    db->set_query(
        "upDate %s set security_flag=0 where m_id=%s and security_flag=1",
        table, NumberToString(m_id, 0));
    db->exec(true);
}

// ============================================================================
// DB_DenyJoinGuild::dispatch —— ORIG @ 0x0843de6c（0xa1，最短）：
// GetDBHandle(DB_HANDLE_8, NONE) → *stream >> guildKey(uint) >>
// applicant(int) → set_query("deLete from guild_join_list where
// guild_id=%d and charac_no=%d", guildKey, applicant) → exec(true)。
// 失败返 0，成功返 1（无基类 dispatch 检查、无回包）。
// ============================================================================
bool DB_DenyJoinGuild::dispatch(int userIdx, int slot, Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_8,
                                                  E_SERVER_GROUP_NONE);
    unsigned int guildKey = 0;
    int applicant = 0;
    *stream >> guildKey;
    *stream >> applicant;

    db->set_query(
        "deLete from guild_join_list where guild_id=%d and charac_no=%d",
        guildKey, applicant);
    if (!db->exec(true))
        return 0;
    return 1;
}

// ============================================================================
// DB_CheckJoinGuild::dispatch —— ORIG @ 0x0843c98e（0x278）：
// GetDBHandle(DB_HANDLE_8, NONE) → GetOutBuffer<SIG_CHECK_JOIN_GUILD> →
// escape_string(buf, sig) → set_query("seLect guild_name,master_name,lev,
// member_count from guild_info where server_id=%d and expire_flag=0 and
// guild_name='%s'", *(int*)(sig+0x18), buf) → exec 失败返 0 →
// Acquire("DBThread.cpp",0x90e6) → guard << 0x216 << slot →
// GetInBuffer<SIG_CHECK_JOIN_GUILD_DB_REPLY>：无行 → m_result=0x15；
// 有行 → fetch 失败返 0；m_result=0 → get_str(0,guildName,0x17) /
// get_str(1,name,0x1e) / get_int(2,+0x3c) / get_int(3,+0x40)（任一失败返 0）
// → put(DISPATCH_Q) → 返 1。
// ============================================================================
bool DB_CheckJoinGuild::dispatch(int userIdx, int slot, Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_8,
                                                  E_SERVER_GROUP_NONE);
    SIG_CHECK_JOIN_GUILD* sig = stream->GetOutBuffer<SIG_CHECK_JOIN_GUILD>();
    int unused = 0;              // ORIG -0x10 槽（dead local，P31）
    char nameBuf[0x2f];          // escape_string 中转（尺寸按 ORIG 帧反推）

    db->escape_string(nameBuf, reinterpret_cast<const char*>(sig));
    db->set_query(
        "seLect guild_name,master_name,lev,member_count from guild_info "
        "where server_id=%d and expire_flag=0 and guild_name='%s'",
        reinterpret_cast<CheckJoinGuildView*>(sig)->m_serverId, nameBuf);
    if (!db->exec(true))
        return 0;

    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x90e6), true);
    guard.operator*()->operator<<(0x216);
    guard.operator*()->operator<<(slot);

    SIG_CHECK_JOIN_GUILD_DB_REPLY* reply =
        guard->GetInBuffer<SIG_CHECK_JOIN_GUILD_DB_REPLY>();
    if (db->get_n_rows() == 0)
    {
        reply->m_result = 0x15;
    }
    else
    {
        if (!db->fetch())
            return 0;
        reply->m_result = 0;
        if (!db->get_str(0, reply->m_guildName, 0x17))
            return 0;
        if (!db->get_str(1, reply->m_name, 0x1e))
            return 0;
        if (!db->get_int(2, reply->m_arg3c))
            return 0;
        if (!db->get_int(3, reply->m_arg40))
            return 0;
    }

    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    return 1;
}

// DB_SecurityCardActivate::dispatch —— ORIG @ 0x0842b86c（0x2f0）：
// GetOutBuffer<SIG_SECURITY_CARD_ACTIVATE> → GetDBHandle(DB_HANDLE_6) →
// set_query(update apply_flag/cert_flag) → exec → ok = (affected == 1)，
// 为真则 turnOnSecurityFlag → DB_CancelRestingUserRestrict::makeRequest(m_id,
// slot) → sig->m_field0b 非零选 mod=1 否则 0 写 history → GetDBHandle(
// DB_HANDLE_1)：ok 时 update member_security_grade，失败或无行则 insert
// （insert 失败返 0）→ DB_CheckSecuReward::makeRequest(slot, m_id, 0) →
// Acquire("DBThread.cpp",0x6e89) → guard << 0xff << slot →
// GetInBuffer<SIG_SECURITY_CARD_ACTIVATE> 回写 +0xa=ok → put → 返 1。
// ============================================================================
bool DB_SecurityCardActivate::dispatch(int userIdx, int slot, Stream* stream)
{
    SecuActivateView* sig = reinterpret_cast<SecuActivateView*>(
        stream->GetOutBuffer<SIG_SECURITY_CARD_ACTIVATE>());
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6,
                                                  E_SERVER_GROUP_NONE);
    db->set_query(
        "upDate member_security_card set apply_flag=1, cert_flag=1, "
        "cert_cnt = 0 where m_id=%s and cert_key='%s'",
        NumberToString(sig->m_id, 0), sig->m_certKey);
    db->exec(true);

    bool ok = false;   // -0x21（回写 +0x0a）
    if (db->getAffectedRowCount() == 1)
    {
        ok = true;
        turnOnSecurityFlag(db, sig->m_id);
    }

    DB_CancelRestingUserRestrict::makeRequest(sig->m_id, slot);

    if (sig->m_field0b != 0)
        saveSecurityCardHistory(db, sig->m_id,
                                ENUM_SECURITYCARD_MOD_TYPE_ACTIVATE);
    else
        saveSecurityCardHistory(db, sig->m_id, ENUM_SECURITYCARD_MOD_TYPE_NONE);

    MySQL* db2 = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_1,
                                                   E_SERVER_GROUP_NONE);
    if (ok)
    {
        db2->set_query(
            "upDate member_security_grade set security_card_reg=now() "
            "where m_id=%s",
            NumberToString(sig->m_id, 0));
        if (!db2->exec(true) || db2->getAffectedRowCount() == 0)
        {
            db2->set_query(
                "inSert into member_security_grade(m_id, security_card_reg) "
                "values(%s, now())",
                NumberToString(sig->m_id, 0));
            if (!db2->exec(true))
                return 0;
        }
    }

    DB_CheckSecuReward::makeRequest(slot, sig->m_id, 0);

    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x6e89), true);
    guard.operator*()->operator<<(0xff);
    guard.operator*()->operator<<(slot);
    SecuActivateView* reply = reinterpret_cast<SecuActivateView*>(
        guard->GetInBuffer<SIG_SECURITY_CARD_ACTIVATE>());
    reply->m_field0a = ok;
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    return 1;
}

// ============================================================================
// DB_SecurityCardIssue::dispatch —— ORIG @ 0x0842af94（0x47c）：
// 基类 DBDispatcher::dispatch 放行（失败返 0）→ GetOutBuffer<
// SIG_SECURITY_CARD_ISSUE> → GetDBHandle(DB_HANDLE_6)。try：
//   查询 phone apply_flag：exec 失败或行数>4 → throw (ENUM_ERROR)0x77；
//   查 re_issue_cnt/last_issue：exec 失败 → throw 1；
//   char q[5] 清零 → generateSecurityCardQuestion(q) → strncpy(sig+0x5e,q,5)；
//   无行 → insert（exec 失败 throw 1）；有行 → fetch 失败 throw 1；
//     get_uint(0,nIssue) / get_uint(1,lastTime) → now=getCurSec() →
//     localtime_r 两次比较 tm_yday：同日 ++nIssue 超 3 → throw 0x78，
//     异日归零 → update（exec 结果丢弃）；
//   turnOffSecurityFlag(db, m_id) → _sendResult(this, slot, sig+0x25,
//     sig+0x04, sig+0x37, sig+0x5e, 0)。
// catch (ENUM_ERROR& e) → _sendResult(this, slot, 0,0,0,0, e)。返 1。
// ============================================================================
bool DB_SecurityCardIssue::dispatch(int userIdx, int slot, Stream* stream)
{
    if (!DBDispatcher::dispatch(userIdx, slot, stream))
        return 0;

    SecuIssueView* sig = reinterpret_cast<SecuIssueView*>(
        stream->GetOutBuffer<SIG_SECURITY_CARD_ISSUE>());
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6,
                                                  E_SERVER_GROUP_NONE);
    try
    {
        db->set_query(
            "seLect apply_flag from member_security_card where phone='%s' "
            "and apply_flag=1",
            sig->m_phone);
        if (!db->exec(true) || (unsigned int)db->get_n_rows() > 4)
            throw static_cast<ENUM_ERROR>(0x77);

        db->set_query(
            "seLect re_issue_cnt,unix_timestamp(last_issue_time) from "
            "member_security_card where m_id=%s",
            NumberToString(sig->m_id, 0));
        if (!db->exec(true))
            throw static_cast<ENUM_ERROR>(1);

        char q[5] = { '\0' };
        generateSecurityCardQuestion(q);
        strncpy(sig->m_qPos, q, 5);

        if (db->get_n_rows() == 0)
        {
            db->set_query(
                "inSert into member_security_card(m_id,occ_time,phone,"
                "cert_key,server_key,card,fail_cnt,re_issue_cnt,"
                "last_issue_time,validity_time,apply_flag,q_pos) "
                "values(%s,now(),'%s','%s','%s','%s',0,0,now(),0,0,'%s')",
                NumberToString(sig->m_id, 0), sig->m_phone, sig->m_certKey,
                sig->m_serverKey, sig->m_card, q);
            db->exec(true);   // ORIG 无检查（结果丢弃）
        }
        else
        {
            unsigned int nIssue = 0;   // ORIG 声明/初始化先于 fetch
            unsigned int lastTime = 0;
            if (!db->fetch())
                throw static_cast<ENUM_ERROR>(1);
            db->get_uint(0, nIssue);
            db->get_uint(1, lastTime);

            unsigned int now =
                static_cast<unsigned int>(GlobalData::s_systemTime_.getCurSec());
            time_t tLast = static_cast<time_t>(lastTime);
            struct tm tmLast;
            struct tm tmNow;
            localtime_r(&tLast, &tmLast);
            localtime_r(reinterpret_cast<time_t*>(&now), &tmNow);

            if (tmLast.tm_yday == tmNow.tm_yday)
            {
                ++nIssue;
                if (nIssue > 3)
                    throw static_cast<ENUM_ERROR>(0x78);
            }
            else
            {
                nIssue = 0;
            }

            db->set_query(
                "upDate member_security_card set phone='%s',cert_key='%s',"
                "server_key='%s',card='%s',fail_cnt=0,re_issue_cnt=%d,"
                "last_issue_time=now(),validity_time=0,apply_flag=0,"
                "q_pos='%s' where m_id=%s",
                sig->m_phone, sig->m_certKey, sig->m_serverKey, sig->m_card,
                nIssue, q, NumberToString(sig->m_id, 0));
            db->exec(true);
        }

        turnOffSecurityFlag(db, sig->m_id);
        _sendResult(slot, sig->m_serial, sig->m_serverKey, sig->m_card,
                    sig->m_qPos, ENUM_ERROR_NONE);
    }
    catch (ENUM_ERROR& e)
    {
        _sendResult(slot, 0, 0, 0, 0, e);
    }
    return 1;
}
// DB_GuildJoinList::dispatch —— ORIG @ 0x0843d87c（0x4ff）：
// GetDBHandle(8) + GetDBHandle(2) → Acquire("DBThread.cpp",0x9243) →
// guard << 0x21a << slot → *stream >> guildKey(uint) >> applicant(int) →
// db1 delete（server_group != applicant）→ db1 select charac_no/memo/occ_time
// → GetInBuffer<SIG_GUILD_JOIN_LIST> → 行数上限 0x32 → 循环 i：
//   get_uint(0,characNo) / get_str(1,memo,0x1a) / get_int(2,occ) →
//   remainTime = now - occ；循环 j：db2 按 characNo select charac_name/lev/
//   job/grow_type/sex，有行则 fetch 后 get_str(0,name,0x1e) / get_byte(1..4,
//   lev/job/grow_type/sex) → put(DISPATCH_Q) → 返 1。
// ============================================================================
bool DB_GuildJoinList::dispatch(int userIdx, int slot, Stream* stream)
{
    MySQL* db1 = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_8,
                                                   E_SERVER_GROUP_NONE);
    MySQL* db2 = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2,
                                                   E_SERVER_GROUP_NONE);
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x9243), true);
    guard.operator*()->operator<<(0x21a);
    guard.operator*()->operator<<(slot);

    unsigned int guildKey = 0;
    *stream >> guildKey;
    int applicant = 0;
    *stream >> applicant;

    db1->set_query(
        "delete from guild_join_list where guild_id=%d and server_group != %d",
        guildKey, applicant);
    if (!db1->exec(true))
        return 0;
    db1->set_query(
        "seLect charac_no,memo,unix_timestamp(occ_time) from guild_join_list "
        "where guild_id=%d",
        guildKey);
    if (!db1->exec(true))
        return 0;

    GuildJoinListView* reply = reinterpret_cast<GuildJoinListView*>(
        guard->GetInBuffer<SIG_GUILD_JOIN_LIST>());
    int occ = 0;                 // ORIG -0x38（声明于 time 前）
    unsigned int now = static_cast<unsigned int>(time(0));
    int nRows;
    nRows = (unsigned int)db1->get_n_rows() > 0x32
                ? 0x32
                : db1->get_n_rows();
    reply->m_count = nRows;

    for (int i = 0; i < nRows; ++i)
    {
        if (!db1->fetch())
            return 0;
        if (!db1->get_uint(0, reply->m_entry[i].m_characNo))
            return 0;
        if (!db1->get_str(1, reply->m_entry[i].m_memo, 0x1a))
            return 0;
        if (!db1->get_int(2, occ))
            return 0;
        reply->m_entry[i].m_remainTime = static_cast<int>(now) - occ;
    }

    for (int j = 0; j < nRows; ++j)
    {
        db2->set_query(
            "seLect charac_name,lev,job,grow_type,sex from charac_info "
            "where charac_no=%d",
            reply->m_entry[j].m_characNo);
        if (!db2->exec(true))
            return 0;
        if (db2->get_n_rows() == 0)
        {
            // ORIG：无行直接跳过（sete/jne 形态）
        }
        else
        {
            if (!db2->fetch())
                return 0;
            if (!db2->get_str(0, reply->m_entry[j].m_name, 0x1e))
                return 0;
            if (!db2->get_byte(1, reply->m_entry[j].m_lev))
                return 0;
            if (!db2->get_byte(2, reply->m_entry[j].m_job))
                return 0;
            if (!db2->get_byte(3, reply->m_entry[j].m_growType))
                return 0;
            if (!db2->get_byte(4, reply->m_entry[j].m_sex))
                return 0;
        }
    }

    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    return 1;
}
