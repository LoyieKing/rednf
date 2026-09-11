#define _GLIBCXX_IOSTREAM 1
// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CPartyOnlyOrig/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
#include "CParty.h"

#include "CDungeon.h"              // m_entryRequiredItem/get_dimension_possible/
                                   //   getLimitCoinDiff/m_87c/m_898
#include "CBattle_Field.h"         // BF 视图转发（先例：CParty.cpp checkBossRoom）
#include "CDataManager.h"          // G_CDataManager()/find_dungeon/get_dimensionInout
#include "CInventory.h"            // check_item_exist/GetInvenSlot
#include "SECRET_SHOP_DATA.h"      // SECRET_SHOP_DATA::m_info（GetUserSecretShop 具名视图）
#include "PacketGuard.h"            // PacketGuard（InterfacePacketBuf 子类）
#include "TimerQueue.h"             // G_TimerQueue/InsertTimer/gen_timer_key 支持
#include "CGuildServerProxy.h"     // CEventBase/CEventManager::GetRepeatEvent→IsEventing
#include "Quest.h"                 // m_index/m_triggerType/m_triggerSubType
// HistoryLog::WriteUseCoin 以本 TU 最小视图接入（先例 WarRoom.cpp：权威
// HistoryLog.h 经 CGameManager.h 的 include 链与本 TU 冲突；mangled 一致）。
class HistoryLog
{
public:
    static void WriteUseCoin(_IO_FILE* file, const char* reason);
};
#include "cMyTraceNoop.h"
#include "DataManagerInitDepends.h" // TOD_UserState/TOD_Layer

namespace
{

// +0xb14 BattleData 逐成员已用硬币数组具名成员视图。
inline int* memberUsedCoin(CParty* party)
{
    return party->m_battleData.m_arr7e8;
}

// +0x1dc CPartyResultRecvFlag 接收标志区视图。ORIG 三处 receiver 取址
// （0x085b3f58 / 0x085b3f85 / 0x085a464）均为 `mov this,%eax;
// lea 0x1dc(%eax),%edx`；4.4 对「视图 cast + 具名成员」产同形 lea，对
// `reinterpret_cast<CPartyResultRecvFlag*>(&m_pad1dc)` 取址恒产 add（N107）
// ⇒ 以视图具名成员表达 receiver（探针 /tmp/b28d_s187/p2.cpp m1..m3）。
struct CPartyRecvFlagView
{
    unsigned char m_pad1dc[0x1dc];
    CPartyResultRecvFlag m_recvFlag;   // +0x1dc
};

}  // namespace

// ============================================================================
// P2P 连接管理区
// ============================================================================

#include "CPartyOnlyOrig/ConnectP2P.inc"


#include "CPartyOnlyOrig/_ClearConnectP2PAbsolute.inc"


#include "CPartyOnlyOrig/_ClearConnectP2PRelative.inc"


#include "CPartyOnlyOrig/_GetConnectP2PMaxMinPoint.inc"


#include "CPartyOnlyOrig/ClearBadP2PUser.inc"


#include "CPartyOnlyOrig/send_party_ipinfo.inc"


#include "CPartyOnlyOrig/send_invite_pvp.inc"


#include "CPartyOnlyOrig/send_invite_warroom.inc"


#include "CPartyOnlyOrig/send_to_alive_member.inc"


// ============================================================================
// 成员查询 / 访问器
// ============================================================================

#include "CPartyOnlyOrig/isMyMember.inc"


#include "CPartyOnlyOrig/GetCoinCount.inc"


#include "CPartyOnlyOrig/get_total_level.inc"


#include "CPartyOnlyOrig/getMemberMinLevel.inc"


#include "CPartyOnlyOrig/isSamePCRoomParty.inc"


#include "CPartyOnlyOrig/IsThereGhostInParty.inc"


#include "CPartyOnlyOrig/getMemberPassParty.inc"


#include "CPartyOnlyOrig/GetUserSecretShop.inc"


// ============================================================================
// 队伍状态 / 组队生命周期
// ============================================================================

#include "CPartyOnlyOrig/switchPostion.inc"


#include "CPartyOnlyOrig/join_user.inc"


#include "CPartyOnlyOrig/change_manager_for_quick_party.inc"


#include "CPartyOnlyOrig/delete_party_for_quick_party.inc"


#include "CPartyOnlyOrig/CheckMemberArea.inc"


#include "CPartyOnlyOrig/setMonsterMoveHack.inc"


// ============================================================================
// 结果接收 / 任务 / 副本入场券
// ============================================================================

#include "CPartyOnlyOrig/set_recv_party_packet_flag.inc"


#include "CPartyOnlyOrig/recv_party_packet_all.inc"


#include "CPartyOnlyOrig/CheckRecvFromAllMember.inc"


#include "CPartyOnlyOrig/TimeoutCompleteAfterAssault.inc"


#include "CPartyOnlyOrig/IsClearedQuest.inc"


#include "CPartyOnlyOrig/HasNoQuestGoWithApcInParty.inc"


#include "CPartyOnlyOrig/CheckEnterAncientDungeon.inc"


#include "CPartyOnlyOrig/checkLimitUsedCoinCondition.inc"


#include "CPartyOnlyOrig/checkUnLimitUsedCoinCondition.inc"


#include "CPartyOnlyOrig/CheckHellCoinLimint.inc"


#include "CPartyOnlyOrig/is_limit_minimum_age.inc"


#include "CPartyOnlyOrig/is_limit_minimum_level.inc"


#include "CPartyOnlyOrig/CheckMemberLastTryAssaultTime.inc"


#include "CPartyOnlyOrig/increase_dimension_inout_count.inc"


#include "CPartyOnlyOrig/GetExpPenaltyPercent.inc"


// ============================================================================
// 副本流程 / 结算
// ============================================================================

#include "CPartyOnlyOrig/RestartDungeon.inc"


#include "CPartyOnlyOrig/isMonsterDrop_item.inc"


// [N-类·帧布局] CParty::VoteLackUser（本 TU 声明对应定义在 CPartyDepsD2）
// 剩 31 全为槽文本差（4.4 帧底保留区致 i/temp/iterator 槽整体深 4B）+
// ebx/esi 角色互换（P78 禁追），语义已双证正确，勿再投预算。

#include "CPartyOnlyOrig/kill_blood_monster.inc"


#include "CPartyOnlyOrig/DestroyPassiveObject.inc"


#include "CPartyOnlyOrig/SetRidableObjectState.inc"


// ============================================================================
// TOD / 血轮复活
// ============================================================================

#include "CPartyOnlyOrig/OnTODRewardTimer.inc"


#include "CPartyOnlyOrig/SelectEPLPCommand.inc"


#include "CPartyOnlyOrig/_Send_TOD_ClearReward.inc"


#include "CPartyOnlyOrig/ReviveUser.inc"


#include "CPartyOnlyOrig/clear.inc"

