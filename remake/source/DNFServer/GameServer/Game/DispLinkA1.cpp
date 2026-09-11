// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 DispLinkA1/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include "Dispatchers.h"

#include <utility>
#include <vector>

#include "Inven_Item.h"
#include "CItem.h"
#include "CStackableItem.h"         // stStackableBoosterItemInfo_t / getBoosterItem
#include "CDataManager.h"           // G_CDataManager / find_item / m_disJoint (+0x1c)
#include "DisJoint.h"               // DisJoint（m_disJoint 指向对象）
#include "CInventory.h"             // MakeItemPacket / GetInvenSlot
#include "CUser.h"                  // Send / get_state / GetParty
#include "CUserCharacInfo.h"        // getCurCharacInvenR / getCurCharacR / GetCurCharacExpertJob*
#include "PacketGuard.h"            // PacketGuard（含 PacketHeader 传递可见）
#include "CompoundEmblemDeps.h"     // LineFunc
#include "CStatisticServerProxy.h"  // SendPacket(char*, int)
#include "CServerProxyMgr.h"        // CServerProxyMgr<T>::GetServerProxy
#include "GlobalData.h"             // GlobalData::s_statistic_proxy_mgr
// [b28c-32] P104 合并：匿名 namespace 内的 Packet_Emblem_Create_Statistic /
// Packet_Avater_Disjoint_Statistic 镜像类删除，统一 include 权威头
// （Avater 局部版仅 0x17 字节 ⇒ 帧 0x34 vs ORIG 0x44、对象落 -0x23 vs -0x34）。
#include "Packet_Emblem_Create_Statistic.h"
#include "Packet_Avater_Disjoint_Statistic.h"

#include "DispLinkA1/read.inc"


#include "DispLinkA1/send.inc"


#include "DispLinkA1/send_packet_guildcreateright.inc"


#include "DispLinkA1/send_packet_guild_invite.inc"


#include "DispLinkA1/send_packet_guild_levelup.inc"


#include "DispLinkA1/check_error.inc"


#include "DispLinkA1/read_1.inc"


#include "DispLinkA1/check_error_1.inc"


#include "DispLinkA1/_read_packet.inc"


#include "DispLinkA1/check_error_2.inc"


#include "DispLinkA1/SendCreateEmblemInfo.inc"


#include "DispLinkA1/SendDisjointAvatarInfo.inc"


#include "DispLinkA1/_getResultItems.inc"

