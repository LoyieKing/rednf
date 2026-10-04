#define _GLIBCXX_IOSTREAM 1
// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 Arad_InternalDispatcher_INTER/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

// [M3S4 2026-09-11 · §6.5 串盲区 · 父泳道 M3FreeNext15B 批准] 文件作用域（全局命名空间）typedef：
//   ORIG prett `_ZZN…sendHelperBox…__PRETTY_FUNCTION__` @0x08ba2e40 size 0x5b =
//   "bool ARAD::DISPATCHER::Arad_INTER_kAradJumpingCharacter::sendHelperBox(CUser*, byte, byte)"。
// GCC `__PRETTY_FUNCTION__` 取**定义处**拼写并保留 typedef 名（探针 /tmp/m3s4_probe_prett.cpp 实证），
//   但置于 namespace 内会打印限定名 `ARAD::DISPATCHER::byte`（本 TU 实测）⇒ 必须全局作用域。
//   底层类型/mangled/sizeof/偏移/布局全不变（同 WarField.cpp:307 先例）。
typedef unsigned char byte;

#include "Arad_InternalDispatcher.h"

#include <time.h>
#include <string.h>
#include <string>

#include "GlobalData.h"
#include "CUser.h"              // CUser::eSendTarget / UserQuest / quest getter
#include "CUserCharacInfo.h"
#include "CInventory.h"         // CInventory / OS_API::snprintf
#include "Inven_Item.h"
#include "PacketGuard.h"
#include "LogManager.h"
#include "GameTypes.h"          // cMyTrace
#include "RDARScriptStringManager.h"   // g_scriptStringManager_ / findString
#include "CMailBoxHelper.h"     // WongWork::CMailBoxHelper::ReqDBSendNewSystemMail
#include "AvatarCoin.h"
#include "DataManagerInitDepends.h"    // AvatarRouletteServer / LevelUpSkill
#include "CGuildServerProxy.h"  // CEventBase / CEventManager::GetRepeatEvent
#include "CSystemTime.h"
#include "CEnvironment.h"
#include "Quest.h"
#include "QuestClear.h"                 // WongWork::CQuestClear
#include "CDataManager.h"               // G_CDataManager / find_quest / m_aradJumping
#include "Arad_DataManager.h"           // ARAD::Singleton<T> / findGameScript
#include "Arad_EventPeriodDataManager.h"
#include "AradServerStateMessage.h"
#include "Arad_ServerStateManager.h"    // insertState/deleteState/ENUM_GLOBAL_EFFECT_ARAD
#include "AvatarFixedHiddenOptionServer.h"
#include "AvatarRechargeServer.h"
#include "EmblemCompoundServer.h"
#include "AvatarConvertServer.h"
#include "MoreTypes.h"       // arad::SigAradEmblemCompound 权威定义（MoreTypes.h:257）

namespace
{

// ORIG INTER 包体字节视图（包字段布局归 OnlyOrig_ARAD 批的 Sig 结构化，
// 此处仅为指令级位移寻址形态保持：packet+offset 直读）。
#pragma pack(push, 1)
struct Packet_Arad_SaveServerState : PacketHeader
{
    char m_afterHdr[4];          // +0x0a..0x0d
    int m_effect;                // +0x0e ENUM_GLOBAL_EFFECT_ARAD
    unsigned int m_endSec;       // +0x12
};
#pragma pack(pop)

}  // namespace

#include "Arad_InternalDispatcher_INTER/PickedAvatarInfo.inc"
{
    m_pad0 = 0;              // +0x00
    m_itemId = 0;            // +0x04
    m_optionOrSkillIdx = 0;  // +0x08
    m_pad9 = 0;              // +0x09
    m_zoneType = 0;          // +0x0a
    m_gradeBand = 0;         // +0x0b
}

namespace ARAD
{

namespace DISPATCHER
{

#include "Arad_InternalDispatcher_INTER/dispatch_sig.inc"


#include "Arad_InternalDispatcher_INTER/dispatch_sig_1.inc"


#include "Arad_InternalDispatcher_INTER/dispatch_sig_2.inc"


#include "Arad_InternalDispatcher_INTER/dispatch_sig_3.inc"


#include "Arad_InternalDispatcher_INTER/clearEpicQuest.inc"


#include "Arad_InternalDispatcher_INTER/sendHelperBox.inc"


#include "Arad_InternalDispatcher_INTER/dispatch_sig_4.inc"


#include "Arad_InternalDispatcher_INTER/dispatch_sig_5.inc"


#include "Arad_InternalDispatcher_INTER/dispatch_sig_6.inc"


#include "Arad_InternalDispatcher_INTER/dispatch_sig_7.inc"


#include "Arad_InternalDispatcher_INTER/dispatch_sig_8.inc"



#include "Arad_InternalDispatcher_INTER/dispatch_sig_9.inc"


}  // namespace DISPATCHER

}  // namespace ARAD
