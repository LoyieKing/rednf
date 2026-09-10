// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 OrigDBInter20/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// df_game_r 还原 —— DB/Inter dispatch 非 W8LT 独占批（OrigDBInter20）
// ORIG 符号（nm 核对，df_game_r elf32-i386；均为仅ORIG——重建侧此前无定义）：
//   T _ZN25DB_SaveConditionEventInfo8dispatchEiiP6Stream      0x08437930
//   T _ZN30DB_SecurityCardUpdateCancelCnt8dispatchEiiP6Stream 0x0842c08e
//   T _ZN28DB_SecurityCardUpdateFailCnt8dispatchEiiP6Stream   0x0842bdda
//   T _ZN33DB_SecurityCardUpdateValidityTime8dispatchEiiP6Stream 0x0842bc64
//   T _ZN25DB_SecurityCardAddCertCnt8dispatchEiiP6Stream      0x0843630e
//   T _Z28generateSecurityCardQuestionPc                      0x086b216d（辅助，
//     UpdateFailCnt::dispatch 依赖，ORIG T 且 NEW 无，一并转写）
// 报告：docs/class_func_reports/{DB_SaveConditionEventInfo/dispatch,
//   DB_SecurityCardUpdateCancelCnt/dispatch, DB_SecurityCardUpdateFailCnt/dispatch,
//   DB_SecurityCardUpdateValidityTime/dispatch,
//   DB_SecurityCardAddCertCnt/dispatch}.md（逐指令）。
// 类壳与 W8LT_DBTask.cpp 注册表一致（继承 DBDispatcher、sz 0x4 仅 vptr）；
// makeRequest 已有权威定义（W8LT_DBTask.cpp / DB_AccountCargoSync.cpp /
// DBSecuLink.cpp），本 TU 仅定义 dispatch 虚函数与辅助函数，不构造对象、
// 不发射 vtable（key function 由本 TU dispatch 定义触发，与 W8LT 空壳类
// vtable 弱符号同址合并，DB_UpdateCeraShopPurchaseCount 先例验证）。
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

// get_rand_int 自由函数（ORIG T 0x086b1b87，DNFFunctionLib.cpp 实现）
#include "OrigDBInter20/get_rand_int.inc"
#include "OrigDBInter20/generateSecurityCardQuestion.inc"
class DB_SaveConditionEventInfo : public DBDispatcher
{
public:
    DB_SaveConditionEventInfo() {}
    virtual bool dispatch(int userIdx, int slot, Stream* stream);
};

#include "OrigDBInter20/dispatch.inc"
}

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
}

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
}

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
}

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
}

// ============================================================================
// 6. generateSecurityCardQuestion(char* out) —— ORIG 0x086b216d（UpdateFailCnt
//    dispatch 依赖，ORIG T / NEW 无；直接反汇编转写）：
//    栈槽（P28 声明序）：ORIG v1(-0x18)/a1(-0x14)/v2(-0x10)/a2(-0xc)。
// ============================================================================
#include "OrigDBInter20/generateSecurityCardQuestion_1.inc"
}
