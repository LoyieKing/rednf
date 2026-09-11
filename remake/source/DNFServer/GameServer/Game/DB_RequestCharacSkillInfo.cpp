// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 DB_RequestCharacSkillInfo/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include "DB_RequestCharacSkillInfo.h"

#include "GlobalData.h"     // s_stream_pool / s_msgq_mgr / s_db_mgr
#include "StreamPool.h"     // StreamPool::Acquire
#include "Stream.h"         // Stream::GetOutBuffer
#include "CStreamGuard.h"   // CStreamGuard(Stream*,bool)/operator<</operator->/GetInBuffer
#include "MsgQueueMgr.h"    // MsgQueueMgr::put + QUEUE_IDX
#include "DBMgr.h"          // DBMgr::GetDBHandle（完整类型）
#include "CEnvironment.h"   // ENUM_DB_HANDLE_IDX / ENUM_SERVER_GROUP
#include "LogManager.h"
#include "SigTypes_D.h"     // SIG_REQUEST_CHARAC_SKILL_INFO

#include <string.h>         // memcpy

// ---- 本 TU 转接声明（UNLINKED_CALLEE，源码暂无 owner；保持 ORIG 全局
// mangled 名以便链接器命中真实 T 符号）----
// ORIG T _Z19get_skill_slot_namec @0x083fbd93（report 已读：-1/0/其它→"skill_slot"，
//   1→"skill_slot_2nd"）。
const char* get_skill_slot_name(char slot)
{
    if (slot == (char)-1 || slot == 0)
        return "skill_slot";
    if (slot == 1)
        return "skill_slot_2nd";
    return "skill_slot";
}
#include "DB_RequestCharacSkillInfo/get_compressed_blob_data.inc"

#include "DB_RequestCharacSkillInfo/_GetSkillTreeIndex.inc"


#include "DB_RequestCharacSkillInfo/_GetSkillInfo.inc"


#include "DB_RequestCharacSkillInfo/dispatch.inc"


#include "DB_RequestCharacSkillInfo/sendResult.inc"

