// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CKillMonsterInfo_chain/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include <map>
#include "CKillMonsterInfo.h"
#include "../../../shared/packet/include/Packet_HellParty_Statistic_Item.h"
#include <string.h>

#include "CBoosterGage.h"          // charac_expand::CData（alter）
#include "CBattle_Field.h"
#include "CParty.h"          // batch-15 dep-downgrade: was via CBattle_Field.h         // 经 WarField.h 引入 CAICharacterList/CDataManager 宿主
#include "CDataManager.h"          // G_CDataManager / find_monster / m_aiCharacterList
#include "LogManager.h"            // LogManager::logFormat
#include "CServerProxyMgr.h"      // CServerProxyMgr<T>::GetServerProxy 完整模板
#include "PacketHeader.h"          // PacketHeader(ushort, ushort)
#include "InterfacePacketBuf.h"    // PacketGuard 发送域接口
#include "GlobalData.h"            // GlobalData::s_statistic_proxy_mgr
#include "CStatisticServerProxy.h" // CStatisticServerProxy::SendPacket
#include "CStreamGuard.h"         // CStreamGuard(Stream*,bool) / Stream
#include "MsgQueueMgr.h"          // MsgQueueMgr::put(DB_Q, CStreamGuard&)
#include "SigTypes_C.h"           // SIG_LOAD_KILL_MONSTER_INFO
#include "CHellPartyItemStatistic.h"  // 地狱派对统计（桥清零，权威头）

// ============================================================================
// CKillMonsterInfo 域 —— 桥清零：原 freestanding asm(_ZN…) shim 转为真实成员
// 函数（权威头 CKillMonsterInfo.h，返回型 void→bool 按 ORIG 报告；mangled 不含
// 返回型零 ABI 风险）。槽 map 经成员 m_bossKills/m_namedKills/m_apcBossKills
// 直接访问（+0x8/+0x20/+0x38），与 ORIG add $0x8/0x20/0x38 同形。
// ============================================================================

#include "CKillMonsterInfo_chain/CKillMonsterInfo.inc"


#include "CKillMonsterInfo_chain/GetDungeonDifficultyFlag.inc"


#include "CKillMonsterInfo_chain/CheckMonsterKill.inc"


#include "CKillMonsterInfo_chain/RegisterMonsterKill.inc"


#include "CKillMonsterInfo_chain/CheckBossKill.inc"


#include "CKillMonsterInfo_chain/RegisterBossKill.inc"


#include "CKillMonsterInfo_chain/CheckNamedKill.inc"


#include "CKillMonsterInfo_chain/RegisterNamedKill.inc"


#include "CKillMonsterInfo_chain/CheckAPCBossKill.inc"


#include "CKillMonsterInfo_chain/RegisterAPCBossKill.inc"


// ============================================================================
// CHellPartyItemStatistic 域
// ============================================================================

// ORIG 包类 Packet_HellParty_Statistic_Item（ctor 0x0860ff9e = PacketHeader
// (0xc31, 0x2a)）：权威定义见 shared/packet/include/Packet_HellParty_Statistic_Item.h
//（布局：+0xa bool type / +0xb int dungeonIndex / +0xf char diff /
//  +0x10 char partyCount / +0x11 char hellpartyDiff / +0x12 int data[6]，
//  与 ORIG mangled _ZN31Packet_HellParty_Statistic_ItemC1Ev 一致）。

#include "CKillMonsterInfo_chain/SendHellPartyItemStatistic.inc"


// ---- GetInstanceHellPartyItemStatistic —— ORIG T 0x0860fcb6 ---------------
// 函数级 static 单例（ORIG .bss instance 0x0947334c）。
// [R21scanPH §27.3] ORIG nm size = **1** @0x0947334c，下一符号
// `_ZZ29GetInstanceExpertJobStatisticvE8instance` @0x09473350 闭合（1 + 3 对齐）
// ⇒ 原码占位对象仅 1 字节（消费点只把 &instance 作 this 传给非虚
// CHellPartyItemStatistic::SendHellPartyItemStatistic，该函数体不访问 this 数据成员、
// 非虚调用无 vptr 需求）⇒ 原 0x24 与 ORIG 实测矛盾，改 1 字节对齐。
void* GetInstanceHellPartyItemStatistic()
{
    static unsigned char instance[1];
    return instance;
}

#include "CKillMonsterInfo_chain/dtor_CKillMonsterInfo.inc"


#include "CKillMonsterInfo_chain/_reset.inc"

// ============================================================================
// ORIG 数据序列化域 —— ConvertStructToMap / ConvertMapToStrcut / getData /
// loadData / _saveData（OrigKillMob 批次；逐报告 ## 汇编 + ## 反编译 C 转写）
// ============================================================================


#include "CKillMonsterInfo_chain/ConvertStructToMap.inc"


#include "CKillMonsterInfo_chain/ConvertMapToStrcut.inc"


#include "CKillMonsterInfo_chain/getData.inc"


#include "CKillMonsterInfo_chain/loadData.inc"


#include "CKillMonsterInfo_chain/_saveData.inc"

