// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 OrigDBInter20/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include "SigTypes.h"       // SIG_SAVE_CONDITION_EVENT / SIG_SECURITY_CARD_UPDATE_FAIL_CNT /
                            // SIG_SECURITY_CARD_UPDATE_VALIDITYTIME / SIG_SECURITY_CARD_ADD_CERT_CNT
#include "GlobalData.h"     // s_db_mgr / s_stream_pool / s_msgq_mgr
#include "CStreamGuard.h"
#include "MsgQueueMgr.h"
#include "DBDispatcher.h"
#include "DBMgr.h"          // DBMgr::GetDBHandle
#include "MySQL.h"          // MySQL::set_query / exec / getAffectedRowCount
#include "CEnvironment.h"   // ENUM_DB_HANDLE_IDX / ENUM_SERVER_GROUP
// NumberToString(unsigned int, int)（ORIG T 0x0810904b，LinkFinalDeps.cpp 定义）
extern char* NumberToString(unsigned int value, int radix);

#include <string.h>         // strncpy

#include "OrigDBInter20/get_rand_int.inc"
#include "OrigDBInter20/generateSecurityCardQuestion.inc"

// ============================================================================
// 1. DB_SaveConditionEventInfo::dispatch —— ORIG 0x08437930（dispatch.md）：
//    GetDBHandle(2,0) → GetOutBuffer<SIG_SAVE_CONDITION_EVENT>（+0 uint charac_no /
//    +4 short current_step / +6 short reward_step）→ set_query(upDate ...) →
//    exec(true) 失败返 0；getAffectedRowCount()==0 时 set_query(inSert ...) →
//    exec(true) 失败返 0；返 1。字段访问经 SIG 直读（ORIG movzwl/movswl/cwtl
//    符号扩展形态，P9 语义）。
// ============================================================================
class DB_SaveConditionEventInfo : public DBDispatcher
{
public:
    DB_SaveConditionEventInfo() {}
    virtual bool dispatch(int userIdx, int slot, Stream* stream);
};

#include "OrigDBInter20/dispatch.inc"


class DB_SecurityCardUpdateCancelCnt : public DBDispatcher
{
public:
    DB_SecurityCardUpdateCancelCnt() {}
    virtual bool dispatch(int userIdx, int slot, Stream* stream);
};

// P1 组件视图：SIG_SECURITY_CARD_UPDATE_FAIL_CNT 的 +4 cancel_cnt 字段（ORIG
// 单指令 disp 折叠寻址 mov 0x4(%edx),%edx）。
struct SecurityCardCancelCntView
{
    unsigned int m_mId;      // +0x00
    unsigned int m_cancelCnt;// +0x04
};

#include "OrigDBInter20/dispatch_1.inc"


// ============================================================================
// 3. DB_SecurityCardUpdateFailCnt::dispatch —— ORIG 0x0842bdda（dispatch.md）：
//    GetDBHandle(6,0) → GetOutBuffer<SIG_SECURITY_CARD_UPDATE_FAIL_CNT>；
//    +4 failCnt==0 时 generateSecurityCardQuestion(buf[5]) 并带 q_pos='%s'，
//    否则仅 fail_cnt=%d；均 NumberToString(m_id,0)；exec(true) →
class DB_SecurityCardUpdateFailCnt : public DBDispatcher
{
public:
    DB_SecurityCardUpdateFailCnt() {}
    virtual bool dispatch(int userIdx, int slot, Stream* stream);
};

// P1 组件视图：SIG_SECURITY_CARD_UPDATE_FAIL_CNT 的 +4 fail_cnt 字段。
struct SecurityCardFailCntView
{
    unsigned int m_mId;   // +0x00
    unsigned int m_failCnt;// +0x04
};

#include "OrigDBInter20/dispatch_2.inc"


class DB_SecurityCardUpdateValidityTime : public DBDispatcher
{
public:
    DB_SecurityCardUpdateValidityTime() {}
    virtual bool dispatch(int userIdx, int slot, Stream* stream);
};

// P1 组件视图：SIG_SECURITY_CARD_UPDATE_VALIDITYTIME 的 +4 flag 字段。
struct SecurityCardValidityTimeView
{
    unsigned int m_mId;   // +0x00
    unsigned int m_flag;  // +0x04
};

#include "OrigDBInter20/dispatch_3.inc"


class DB_SecurityCardAddCertCnt : public DBDispatcher
{
public:
    DB_SecurityCardAddCertCnt() {}
    virtual bool dispatch(int userIdx, int slot, Stream* stream);
};

// P1 组件视图：SIG_SECURITY_CARD_ADD_CERT_CNT 的 +0 m_id 字段。
struct SecurityCardAddCertCntView
{
    unsigned int m_mId;   // +0x00
};

#include "OrigDBInter20/dispatch_4.inc"


#include "OrigDBInter20/generateSecurityCardQuestion_1.inc"

