#define _GLIBCXX_IOSTREAM 1
// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CompoundDeathLCB/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include "CCompoundAvatar.h"
#include "CDeathTowerRanking.h"
#include "CInventory.h"         // CInventory 完整类型 + OS_API::snprintf
#include "PacketGuard.h"        // PacketGuard 完整类型（CDeathTowerRanking.h 仅前向）
#include "CAvatarItemMgr.h"      // IsTempKey/GetRemainDate/GetExpireDate/UnRegistItem
#include "CUser.h"               // CUserCharacInfo getter/getCurCharacInven*
#include "CEnvironment.h"        // G_CEnvironment()/m_serverEnvir（_generateIPGNo 通道号）
#include "GlobalData.h"          // s_systemTime_/s_statistic_proxy_mgr/s_stream_pool/s_msgq_mgr
#include "CSystemTime.h"
#include "MsgQueueMgr.h"        // MsgQueueMgr::put / QUEUE_IDX
#include "CServerProxyMgr.h"     // CServerProxyMgr<CStatisticServerProxy> 完整模板
#include "CStatisticServerProxy.h"
#include "CStreamGuard.h"
#include "GameTypes.h"          // cMyTrace
#include "CDataManager.h"        // G_CDataManager()/find_item
#include "PvPTypes.h"            // CMTRand（m_rand 完整类型）
#include "GameRand.h"            // get_rand_int
#include "DB_UpdateAvatarHistory.h"
#include "Packet_Avater_Disjoint_Statistic.h"  // 统计包权威布局（本 TU 原重复定义已迁除）
#include <cstring>
#include <ctime>
#include <utility>
#include <vector>


#include "CompoundDeathLCB/getItemIndex.inc"

#include "CompoundDeathLCB/makeRequest.inc"


namespace WongWork {

// ---------------------------------------------------------------------------
// ORIG CItem 虚表槽直调 helper。
// ORIG CItem vtable(.rodata 0x08c96da0) 槽序实证：D1/D2、D0、纯虚区；
//   +0x0c=子类型索引（派生 _ZNK10CEquipItem12get_sub_typeEv）、
//   +0x10=IsAvatarItem(bool)、+0x24=getUsableItemType(int)(bool)。
// 当前 CItem.h 虚函数声明序与该权威槽序的审计差异尚未收敛（AuditCItemVtable
// 域负责）；为保证本 TU 与 ORIG 虚分派逐指令等价，经槽偏移显式调用，
// 全局虚表收敛后应替换回语义虚调用。
//
// 必须是**宏**而非 static helper：ORIG 的虚取址序列
//   mov (obj),%eax ; add $slot,%eax ; mov (%eax),%edx ; mov obj,%eax ;
//   mov %eax,(%esp) ; call *%edx
// （7 insn/处）位于调用者帧内；提成 helper 后调用者只剩 2 insn，本 TU
// _CheckCondition 实测 10 处虚调用 ⇒ ~50 insn 缺口（P183 同类物化口径，
// P167 差集 ORIG indirect=10 / ours=0 实证）。
// ---------------------------------------------------------------------------
#define CI_VCALL0(ret, slot, obj)                                        \
    reinterpret_cast<ret (*)(const void*)>(                              \
        *reinterpret_cast<const int*>(                                   \
            *reinterpret_cast<const void* const*>(obj) + (slot)))(obj)
#define CI_VCALL1(ret, slot, obj, a1)                                    \
    reinterpret_cast<ret (*)(const void*, int)>(                         \
        *reinterpret_cast<const int*>(                                   \
            *reinterpret_cast<const void* const*>(obj) + (slot)))(obj, (a1))

#define CI_SubType(item)       CI_VCALL0(int, 0x0c, item)
#define CI_IsAvatarItem(item)  CI_VCALL0(bool, 0x10, item)
#define CI_UsableType1(item)   CI_VCALL1(bool, 0x24, item, 1)

// ============================ _MakeCompoundInfo ============================

#include "CompoundDeathLCB/_MakeCompoundInfo.inc"


// ============================== _CheckCondition =============================

#include "CompoundDeathLCB/_CheckCondition.inc"


// ============================== _ProcCompoundCore ===========================

#include "CompoundDeathLCB/_ProcCompoundCore.inc"


// =========================== _SendDisjointAvatarStatistic ===================

#include "CompoundDeathLCB/_SendDisjointAvatarStatistic.inc"


// ================================ _generateIPGNo ============================

#include "CompoundDeathLCB/_generateIPGNo.inc"
                                      char* buf) const
{
    // [b28d-solo-CompoundDeathLCB] ORIG 0x08333df6 直取形参地址（无 time_t t
    // 局部槽）且把 localtime_r 返回值存具名指针槽 -0x1c 后经 p-> 读字段
    //（P43/P0）；五个 snprintf 实参全内联、右起求值（characNo→mday→mon→
    // year%2000→serverId），characNo 升格 %esi 跨 G_CEnvironment 保活
    //（P226/P227 形）。
    // 【定案】达成 **AE**：56/56 逐条全等（仅字符串立即数重定位标签伪差）。
    struct tm lt;
    struct tm* p = localtime_r(&curSec, &lt);
    // ORIG 0x08333e63 `mov 0x1b0(%eax),%eax`（4 字节读）= CEnvironment+0x198
    //（m_serverEnvir）+0x18 → Server_Envir::m_channelNo（通道号；同
    // CDataManager.cpp:1461 具名口径）。
    OS_API::snprintf(buf, 0x14, "C%02d%02d%02d%02d%010d",
                     G_CEnvironment()->m_serverEnvir.m_channelNo,
                     (p->tm_year + 1900) % 2000,
                     p->tm_mon + 1,
                     p->tm_mday,
                     user->getCurCharacNo());
    return buf;
}

}  // namespace WongWork

// ======================== CDeathTowerRanking packets ========================

namespace WongWork {

#include "CompoundDeathLCB/_makeRecordPacket.inc"


#include "CompoundDeathLCB/makeMyBestRecordPacket.inc"


#include "CompoundDeathLCB/makeTopRankerRecordPacket.inc"


}  // namespace WongWork
