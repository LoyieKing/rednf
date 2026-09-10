// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 DBSecuLink/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
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

// 载荷视图已上收共享头（SecuLink 批次 2026-09-05 裸偏移清理，§4.4 唯一声明点）：
//   SIG_SECURITY_CARD_ACTIVATE / SIG_SECURITY_CARD_ISSUE → SigTypes_E.h
//   （makeRequest 0x0842bb5c/0x0842b57a + dispatch 0x0842b86c/0x0842af94 偏移
//    实证）；SIG_GUILD_JOIN_LIST 列表视图 → SigTypes_B.h（dispatch 0x0843d87c）；
//   SIG_CHECK_JOIN_GUILD::m_serverId（+0x18）→ SigTypes_A.h（dispatch
//   0x0843c98e `mov 0x18(%eax),%eax` → SQL server_id=%d）。

// ---- 跨 TU 引用（定义在其它权威 TU；本 TU 仅声明，U 合规）----

// ---- 跨 TU 引用（定义在其它权威 TU；本 TU 仅声明，U 合规）----
#include "DBSecuLink/generateSecurityCardQuestion.inc"
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
// [SweepDBSecuLink] ORIG 0x08286a30：250/250 AE（唯一残差 = 我方循环出口对齐 nop，
//   §6.1 豁免）。三处形态修正：
//   ① hackType/cnt/i unsigned→int：ORIG 0x08286a9e/0x08286aaf/0x08286ab9 为
//      jle/jg、0x08286b6d 尾测 setl（有符号）⇒ unsigned 生 jbe/ja/setb（P196/P2）；
//   ② trace 改全表达式临时链 cMyTrace(...)(fmt, NumberToString(...), ...)：
//      ORIG 0x08286ada 把 NumberToString 结果存 %ebx 跨 ctor 保活、并翻转四个
//      互斥 case 的 trace 对象槽方向（P227/P221；具名 mid 局部槽化且丢 push %ebx）；
//   ③ clamp 改单表达式 std::min(std::max((int)v1, 0), N)，getPoint 两次调用
//      内联、去 static_cast<int>：ORIG 0x08286cc3 为单段 shr/sub 无 regmove（P0/P41）。
#include "DBSecuLink/SetControlData.inc"
}
// ----------------------------------------------------------------------------
// DB_CheckJoinGuild::makeRequest(int uid, SIG_CHECK_JOIN_GUILD* sig)
// ORIG @ 0x0843cc06（报告 DB_CheckJoinGuild/makeRequest.md）：
// Acquire("DBThread.cpp", 0x911a) → guard(stream,true) → << 0x216（op<<(int)）|
// << uid（op<<(int)）→ GetInBuffer<SIG_CHECK_JOIN_GUILD>()（memcpy 0x1c 字节，
// 无 memset）→ put(DB_Q)。无 -1 槽。
// ----------------------------------------------------------------------------
#include "DBSecuLink/makeRequest.inc"
}

// ----------------------------------------------------------------------------
// DB_DenyJoinGuild::makeRequest(uint uid, uint guildKey, uint applicant)
// ORIG @ 0x0843df0e（报告 DB_DenyJoinGuild/makeRequest.md）：
// Acquire("DBThread.cpp", 0x92b4) → << 0x21c | << uid(int) | << guildKey(uint) |
// << applicant(uint) → put(DB_Q)。无 GetInBuffer。
// ----------------------------------------------------------------------------
#include "DBSecuLink/makeRequest_1.inc"
}

// ----------------------------------------------------------------------------
// DB_GuildJoinList::makeRequest(int uid, uint guildKey, ENUM_SERVER_GROUP group)
// ORIG @ 0x0843dd7c（报告 DB_GuildJoinList/makeRequest.md）：
// Acquire("DBThread.cpp", 0x9299) → << 0x21a | << uid(int) | << guildKey(uint) |
// << group（op<<(int)，参数槽按 int）→ put(DB_Q)。无 GetInBuffer。
// ----------------------------------------------------------------------------
#include "DBSecuLink/makeRequest_2.inc"
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
#include "DBSecuLink/makeRequest_3.inc"
}

// ----------------------------------------------------------------------------
// DB_SecurityCardActivate::makeRequest(uint uid, uint accId, certKey, activated)
// ORIG @ 0x0842bb5c（报告 DB_SecurityCardActivate/makeRequest.md）：
// Acquire("DBThread.cpp", 0x6e99) → << 0xff（op<<(int)）| << uid(int) →
// GetInBuffer<SIG_SECURITY_CARD_ACTIVATE>()（0xc 字节，无 memset）填：
//   +0x00 = accId（uint）；strncpy(+0x04, certKey, 6)；+0x0b(byte) = activated。
// → put(DB_Q)。
// ----------------------------------------------------------------------------
#include "DBSecuLink/makeRequest_4.inc"
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
#include "DBSecuLink/saveSecurityCardHistory.inc"
}

// ----------------------------------------------------------------------------
// turnOnSecurityFlag(MySQL*, uint m_id) —— ORIG @ 0x0842ae90：
// char buf[0x14] 清零后 memcpy 13 字节 "member_login"（含 \0）；set_query(
// "upDate %s set security_flag=1 where m_id=%s and security_flag=0", buf,
// NumberToString(m_id,0)) → exec(true)。void。
// ----------------------------------------------------------------------------
#include "DBSecuLink/turnOnSecurityFlag.inc"
}

// ----------------------------------------------------------------------------
// turnOffSecurityFlag(MySQL*, uint m_id) —— ORIG @ 0x0842af12：与 turnOn 同构，
// 仅 SQL 的 security_flag 目标值相反。
// ----------------------------------------------------------------------------
#include "DBSecuLink/turnOffSecurityFlag.inc"
}

// ============================================================================
// DB_DenyJoinGuild::dispatch —— ORIG @ 0x0843de6c（0xa1，最短）：
// GetDBHandle(DB_HANDLE_8, NONE) → *stream >> guildKey(uint) >>
// applicant(int) → set_query("deLete from guild_join_list where
// guild_id=%d and charac_no=%d", guildKey, applicant) → exec(true)。
// 失败返 0，成功返 1（无基类 dispatch 检查、无回包）。
// ============================================================================
#include "DBSecuLink/dispatch.inc"
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
#include "DBSecuLink/dispatch_1.inc"
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
#include "DBSecuLink/dispatch_2.inc"
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
// [b28d-54 修复] ORIG 0x0842af94：唯一残差为 localtime_r 两实参求值序
//   （ORIG arg2 先）；`unsigned int now` + reinterpret_cast 使 4.4 arg1 先。
//   改 `time_t now` + 纯 `&now` 后与 ORIG 同形（289=289，AE）。
#include "DBSecuLink/dispatch_3.inc"
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
// [b28d-54 修复] ORIG 0x0843d87c：唯一残差为 now/occ 两局部装载序
//   （ORIG `mov -0x38(occ),%edx; mov -0x18(now),%ecx`）；now 改 int 与 occ 同型后
//   4.4 复现同序（347=347，AE）。
#include "DBSecuLink/dispatch_4.inc"
}
