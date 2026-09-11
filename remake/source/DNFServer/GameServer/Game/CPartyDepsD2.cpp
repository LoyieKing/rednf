// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CPartyDepsD2/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include <string.h>
#include "TowerOfDespairStatistic_Key.h"  // 权威 Key（CDungeon.cpp 定义 ctor）
#include <vector>

#include "CPartyDepsD2/IsCollectDungeonStatistic.inc"

#include "QuickParty.h"       // 必须先于 CDungeon.h/CParty.h（守卫惯例，与 CParty.cpp 同）
#include "CParty.h"
#include "CGameManager.h"
#include "CUserProc.h"        // APSystem::CUserProc::ClearActionAndSendtoUser
#include "CSecretShop.h"      // secretshop::CSecretShop::LotteryItems
#include "SECRET_SHOP_DATA.h" // SECRET_SHOP_DATA::IsOpen
#include "CItemGenerator.h"   // ::CLuckPoint 完整类型（GetLuckPoint/AddUser）
#include "CStatisticServerProxy.h" // sendPremiumGoldCardStatistic
#include "Quest.h"               // Quest 完整类型（MakeQuestRewardPacket）
#include "GameResultSet.h"     // GameResultSet 完整类型（SetPlayResult）
#include "DisPatcherCore.h"    // DisPatcher_SetPlayResult::SendResult
#include "GoldCardEventStatistic.h"  // Packet_Goldcard_Event_Statistic_GTS m_stat 完整类型
#include "PacketGuard.h"
#include "TimerQueue.h"
#include "Secu_HackLogCheckByParty.h"
#include "PacketTypes.h"          // MSG_MOVE_MAP 完整类型
#include "CCommonStatisticsMgr.h"
#include "CDungeon.h"
#include "CDataManager.h"
#include "W9_EventSmall.h"   // CCharacBloodDungeon::GetEnterCount（check_dungeon_start）
#include "CBoosterGage.h"     // charac_expand::CData（ClearLayer alter）
#include "BestClearTime.h"    // makeBestClearTimePacket
#include "CServerProxyMgr.h"      // CServerProxyMgr<CStatisticServerProxy>
#include "TowerOfDespairMgr.h"    // TOD 域权威（TOD_Layer/APC_Mgr/TowerOfDespairMgr）
#include "DataManagerInitDepends.h" // TOD_UserState（ClearLayer）
#include "CInventory.h"
#include "CWorldMap.h"
#include "GameWorld.h"
#include "CGuildServerProxy.h"
#include "GlobalData.h"
#include "RDARScriptStringManager.h"
#include "CBattle_Field.h"
#include "BattleData.h"
#include "CharacManageScript.h"
#include "CPowerManager.h"
#include "CVillageMonsterMgr.h"
#include "WarField.h"             // MapInfo 完整类型
#include "LogManager.h"

namespace
{

// ---- 子对象视图（与 CParty.cpp 同惯例） ----
// 用函数式宏而非 inline 函数：-O0 下 inline 函数参数必分配栈槽，always_inline
// 会先 spill this 再 reload，比 ORIG 直接偏移算术更差（实测 0x826 -> 0x95c）。
// 宏文本替换为 &p->m_memberOffset，直接产生 ORIG 的偏移寻址（check_allmember_loading 报告）。
#define bd(p)         ((p)->m_battleData)   // ORIG DWARF CParty+0x32c 具名化
#define bf(p)         (*reinterpret_cast<CBattle_Field*>(&(p)->m_battleField))
// [b28d-solo-CParty2] CParty.h 字段化：pad 视图 cast → 具名成员。
#define traceHack(p)  ((p)->m_traceMobDieHack)
#define memberUser(p,i) ((p)->m_member[(i)].m_pUser)

// FISTP 取整惯例：ORIG 汇编为 fldcw 0xC（RC=11，truncate 向零截断）后 fistpl，
// 无 +0.5f 的 fadd（实测 checkHackAverageHitCount/sub_hellparty_value/
// send_hellParty_value 三处 ORIG 均为纯截断）。旧定义 +0.5f 系误解，修正。
#define D2_ROUND(x) ((int)(x))

// ---- CCharacBloodDungeon 只读视图（类未建模，同 CPartyDepsC2.cpp 惯例）----
// GetEnterCount W 0x085bfe88 = *(int*)(this+0x18)（loadData 报告证实计数区）。
struct BloodDungeonCountsViewD2
{
    char m_pad0[0x18];
    int m_enterCount;   // +0x18（GetEnterCount 读取）
};

}  // namespace

#include "CPartyDepsD2/CalcurateManageBonus.inc"


#include "CPartyDepsD2/CheckEnterDimensionDungeon.inc"


#include "CPartyDepsD2/CheckEnterDimensionDungeon_1.inc"


#include "CPartyDepsD2/CheckHackAverageHitCount.inc"


#include "CPartyDepsD2/GetFirstValidMemberState.inc"


#include "CPartyDepsD2/HasAllMemberSelection.inc"


#include "CPartyDepsD2/OnFailedDungeonClear.inc"


#include "CPartyDepsD2/OnSuccessedDungeonClear.inc"


#include "CPartyDepsD2/OnStartMapFinishLoading.inc"


#include "CPartyDepsD2/OnUsedSkill.inc"


#include "CPartyDepsD2/isCheckUseSkill.inc"


#include "CPartyDepsD2/SaveCoin.inc"


#include "CPartyDepsD2/changeBattleDataOnMoveMap.inc"


#include "CPartyDepsD2/canFatigueUp.inc"


#include "CPartyDepsD2/changePartyMemberInfo.inc"


#include "CPartyDepsD2/changePartyMemberInfoOnMoveMap.inc"


#include "CPartyDepsD2/checkMoveMapHackCnt.inc"


#include "CPartyDepsD2/check_allmember_loading.inc"


#include "CPartyDepsD2/check_dungeon_start.inc"


#include "CPartyDepsD2/getDungeonDropTestFlag.inc"


#include "CPartyDepsD2/send_dungeon_fail_msg.inc"


#include "CPartyDepsD2/send_to_party_cmd_error.inc"


#include "CPartyDepsD2/setStandardDimensionLevel.inc"


#include "CPartyDepsD2/sub_hellparty_value.inc"


#include "CPartyDepsD2/send_hellParty_value.inc"


// ---------------------------------------------------------------------------
// GetMapPlayingTime —— ORIG T 0x085b6768
// 报告：docs/class_func_reports/CParty/GetMapPlayingTime.md
// 64 位差值 EDX:EAX 返回；调用方（DisPatcher_MoveMap/CheckOneMapPlayTimeHackCnt、
// DisPatcher_SetPlayResult/dispatch_sig）均只消费低 32 位 /1000，
// 故按 CParty.h 既定 unsigned int 声明返回低 32 位。
// ---------------------------------------------------------------------------

#include "CPartyDepsD2/GetMapPlayingTime.inc"


// ============================================================================
// == 组队/战场链接缺口批（2026-08-25）——符号清单：
//   /tmp/df_batches/undef_link_syms_2026-08-25.txt 中 CParty 域 9 个 +
//   直接依赖（SetPlayResult 统计三辅助 / historyBloodRoundClear /
//   cal_hellparty_value / get_rate_level_difference / CLuckPoint::AddUser /
//   TOD ClearLayer / TowerOfDespairMgr::Get_APCNameByLayer）。
// 报告均在 docs/class_func_reports/CParty/ 与 objdump（无报告者标注地址）。
// ============================================================================

#include "CPartyDepsD2/AddUser.inc"


#include "CPartyDepsD2/GetLuckPoint.inc"


#include "CPartyDepsD2/InsertTournamentDungeonTimer.inc"


#include "CPartyDepsD2/GetMemberSlotNo.inc"


#include "CPartyDepsD2/VoteLackUser.inc"


#include "CPartyDepsD2/OnTimerWalkOutLackUser.inc"


#include "CPartyDepsD2/GetSetPlayResultCount.inc"


#include "CPartyDepsD2/GetNoSendSetPlayResultUser.inc"


#include "CPartyDepsD2/SetPlayResult.inc"


#include "CPartyDepsD2/MakeQuestRewardPacket.inc"


#include "CPartyDepsD2/OnTimeoutBossDie.inc"


// ---------------------------------------------------------------------------
// CParty::sendPremiumGoldCardStatistic —— ORIG T 0x085aca0a（报告
//   sendPremiumGoldCardStatistic.md）：金卡事件统计包（0x1f46/0x13）上报。
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// Packet_Goldcard_Event_Statistic_GTS —— ORIG ctor 0x085bee96 W：
//   PacketHeader(0x1f46,0x13)[10] + GoldCardEventStatistic(+0xa, size 0x9, 总 0x13)。
//   （与 Game/Packet_Goldcard_Event_Statistic_GTS.h 同布局；本 TU 自包含定义。）
// ---------------------------------------------------------------------------
struct Packet_Goldcard_Event_Statistic_GTS
{
    PacketHeader m_header;          // +0x00（packed 0xa）
    GoldCardEventStatistic m_stat;  // +0x0a（ORIG GoldCardEventStatisticC1 构造）

    Packet_Goldcard_Event_Statistic_GTS()
        : m_header(0x1f46, 0x13), m_stat()
    {
    }
} __attribute__((packed));

#include "CPartyDepsD2/sendPremiumGoldCardStatistic.inc"


#include "CPartyDepsD2/historyBloodRoundClear.inc"


#include "CPartyDepsD2/cal_hellparty_value.inc"


#include "CPartyDepsD2/get_rate_level_difference.inc"


#include "CPartyDepsD2/ClearLayer.inc"


#include "CPartyDepsD2/Get_APC_Name_byIndex.inc"


#include "CPartyDepsD2/Get_APCNameByLayer.inc"


#include "CPartyDepsD2/increaseLayer.inc"



#include "CPartyDepsD2/BloodRoundClear.inc"
                                      unsigned int playTime,
                                      const char* dungeonName,
                                      const char* memberNames)
{
    (void)playTime;
    (void)dungeonName;
    // ORIG 0x08684af0：无 null 检查，直接解引用。
    m_user->LogHistory("BloodRoundClear,%d,%u,%s", (int)round,
                       (unsigned int)clearTime, memberNames);
}

// 注：用 #pragma pack（而非 __attribute__((packed))）——GCC 4.4 的 attribute
// packed 对含非 POD 成员（Key 有 user ctor）的外层 struct 不生效。
#pragma pack(push, 1)
struct Packet_TowerOfDespair_Statistic_GTS
{
    PacketHeader m_header;   // +0x00（packed 0xa 字节）
    TowerOfDespairStatistic_Key m_key;  // +0x0a
    unsigned char m_flag;    // +0x12
    unsigned int m_accId;    // +0x13

    Packet_TowerOfDespair_Statistic_GTS()
        : m_header(0x271c, 0x17)
    {
        m_flag = 0;
        m_accId = 0;
    }
};
#pragma pack(pop)

#include "CPartyDepsD2/ClearDungeon.inc"


#include "CPartyDepsD2/makeBestClearTimePacket.inc"


#include "CPartyDepsD2/SendResult.inc"

