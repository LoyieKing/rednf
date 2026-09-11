// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 DBSecuLink/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
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

#include "DBSecuLink/SetControlData.inc"

#include "DBSecuLink/makeRequest.inc"


#include "DBSecuLink/makeRequest_1.inc"


#include "DBSecuLink/makeRequest_2.inc"


#include "DBSecuLink/makeRequest_3.inc"


#include "DBSecuLink/makeRequest_4.inc"


// ============================================================================
// SecurityCard 批自由函数（ORIG T 段；与 DB_SecurityCardIssue/Activate::dispatch
// 同属 DB_Secu 批，本 TU 独占）。
// ============================================================================

#include "DBSecuLink/saveSecurityCardHistory.inc"


#include "DBSecuLink/turnOnSecurityFlag.inc"


#include "DBSecuLink/turnOffSecurityFlag.inc"


#include "DBSecuLink/dispatch.inc"


#include "DBSecuLink/dispatch_1.inc"


#include "DBSecuLink/dispatch_2.inc"


#include "DBSecuLink/dispatch_3.inc"

#include "DBSecuLink/dispatch_4.inc"

