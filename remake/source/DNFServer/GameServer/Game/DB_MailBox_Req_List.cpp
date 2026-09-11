// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 DB_MailBox_Req_List/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include "DB_MailBox_Req_List.h"

#include "SigTypes.h"        // SIG_MAILBOX_LIST
#include "GlobalData.h"      // s_db_mgr / s_stream_pool / s_msgq_mgr
#include "CStreamGuard.h"
#include "MySQL.h"
#include "DBMgr.h"
#include "MsgQueueMgr.h"
#include "LogManager.h"
#include "GamePODTypes.h"    // stAvatarEmblemInfo_t
#include "CMailBoxHelper.h"  // SIG_LETTER_INFO（0x12b）
#include "DB_MailBox_Dispatch.h"  // getList2inQuery（ORIG 0x083ff968，已实现）

#include <map>
#include <vector>
#include <cstring>
#include <cstdio>
#include <algorithm>

// SIG_MAILBOX_LIST 响应区字段偏移（dispatch/SelectLetter/setCreatureData 报告）
#define SIG_I32(p, off)  (*(int*)((char*)(p) + (off)))
#define SIG_U32(p, off)  (*(unsigned int*)((char*)(p) + (off)))
#define SIG_PTR(p, off)  ((char*)(p) + (off))

enum
{
    SIG_OFF_POSTAL_COUNT   = 0x794,   // int 已填 postal 条数
    SIG_OFF_REMAIN_COUNT   = 0x798,   // int 剩余可拉条数
    SIG_OFF_MAX_POSTAL_ID  = 0x79c,   // uint 最大 postal_id / 增量游标
    SIG_OFF_USER_ITEM_CNT  = 0x7a0,   // int user_items 条数
    SIG_OFF_CREATURE_CNT   = 0xd30,   // int creature_items 条数
    SIG_OFF_LETTER_COUNT   = 0x1018,  // int 信件总条数
    SIG_OFF_LETTER_USED    = 0x3326,  // int 已用信件上限（= 请求 +0x08）
    SIG_OFF_LETTER_NEW     = 0x332a,  // int 本次新增信件条数（SelectLetter 写）
    SIG_OFF_LETTER_ADD     = 0x332e,  // int 增量计数（dispatch 第二循环）
    SIG_OFF_CHARAC_NO      = 0x3332,  // int 角色号（回包）
    SIG_OFF_FIELD3336      = 0x3336,  // int 请求 +0x10 透传
    SIG_OFF_EXTRA_IDS      = 0x333a,  // uint[20] map 键回写区
};

#include "DB_MailBox_Req_List/CompareLetterIndex.inc"
#include "DB_MailBox_Req_List/FindLastLetterIndexMail.inc"
#include "DB_MailBox_Req_List/RemoveMailByLatestLetter.inc"

#include "DB_MailBox_Req_List/IsContainedLetter.inc"


#include "DB_MailBox_Req_List/DoChangeMailList.inc"


#include "DB_MailBox_Req_List/CompareLetterIndex_1.inc"


#include "DB_MailBox_Req_List/FindLastLetterIndexMail_1.inc"


#include "DB_MailBox_Req_List/RemoveMailByLatestLetter_1.inc"


#include "DB_MailBox_Req_List/ArrangeMailAndLetterIndex.inc"



// ---- SIG_MAILBOX_LIST 请求头 / 明细区视图（pack(1)，偏移对照报告逐指令） ----
#pragma pack(push, 1)
struct MailBoxListReqView
{
    unsigned int m_characNo;          // +0x00
    unsigned int m_maxPostalId;       // +0x04（上次最大 postal_id，增量拉取）
    unsigned int m_maxLetter;         // +0x08（本次最大信件条数）
    unsigned int m_letterIdx;         // +0x0c（增量 letter_id）
    unsigned int m_field10;           // +0x10
    unsigned int m_postalExtra[20];   // +0x14（非 0 项 → extraMap / DoChangeMailList）
};
#pragma pack(pop)

// ext 源 union（ORIG 0x0841e7fb..0x0841e82c：源为 RandomOption 局部 -0x102 起的**直读**
// `mov -0x102(%ebp),%edx`；目标 ext 行同上）。同址双视角：写侧 m_ro（reset/get_binary），
// 读侧 m_ext（权威头行类型 SIG_MAILBOX_POSTAL_EXT，保 ORIG 的直读形）。
union MailExtSource
{
    RandomOption m_ro;
    SIG_MAILBOX_POSTAL_EXT m_ext;
};

// postal 明细项视图（stride 0x61）已**整体迁入权威头** SigTypes_D.h：
//   SIG_MAILBOX_POSTAL/SIG_MAILBOX_POSTAL_TAIL/SIG_MAILBOX_POSTAL_OCC/
//   SIG_MAILBOX_POSTAL_AMP/SIG_MAILBOX_POSTAL_EXT（§8.3 字段化，R2 已广播）。
// 旧 TU 局部 cast 视图（MailPostalItem/MailAmpView/MailExtView）已删除——
// cast 视图形恒「基址先载」，与 ORIG 4.1 的「下标先、基址后」装载序相反（P235），
// 且属「视图 cast 反模式」；现全部经参数类型自身成员具名访问。

// ============================================================================
// SelectLetter —— ORIG 0841d52c（普通成员，this 调用）：

#include "DB_MailBox_Req_List/SelectLetter.inc"


#include "DB_MailBox_Req_List/setCreatureData.inc"


#include "DB_MailBox_Req_List/dispatch.inc"
