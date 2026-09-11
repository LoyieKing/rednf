// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 PartyClosureC2/<Func>.inc（不参与编译、内部不含 #include）。
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
#include <string.h>
#include <utility>
#include <vector>

// ORIG T 0x08484a51（InitEnvHelpers.cpp 定义，无权威头声明；同 CPartyDepsD2.cpp）
class CUser;
#include "PartyClosureC2/IsCollectDungeonStatistic.inc"
#include "PartyClosureC2/addUserHackCount149.inc"
#include "PartyClosureC2/addUserHackCount150.inc"

#include "QuickParty.h"           // 必须最先：QUICKPARTY_H 守卫需先于 CDungeon.h 放开，
                                  // 避免 CDungeon 本地最小 CQuickPartySystemManager 与权威定义冲突
#include "CAssaultMgr.h"
#include "BattleData.h"
#include "CBattle_Field.h"
#include "CParty.h"          // batch-15 dep-downgrade: was via CBattle_Field.h
#include "CDungeon.h"             // CDungeon/RequiredItem/MazeScript/ENUM_MAZE_QUEST_TYPE
#include "CGameManager.h"         // G_CGameManager/GetQuickPartyRewardManager
#include "CInventory.h"           // use_money/delete_item/MakeItemPacket + OS_API::snprintf
#include "CItem.h"
#include "CDataManager.h"         // G_CDataManager/m_serverParameter(ServerParameterScript)
#include "GlobalData.h"           // s_systemTime_/s_statisticMgr/s_statistic_proxy_mgr
#include "GameWorld.h"            // G_GameWorld/send_party_info_to_all
#include "PacketGuard.h"
#include "RDARScriptStringManager.h"  // findString/g_scriptStringManager_
#include "CStatisticServerProxy.h"    // SendPacket(char*, int)
#include "CDungeonExpStatistic.h"  // CStatisticMgr+0x18 子对象（局部视图用）
#include "TimerDispatcher.h"      // Timer_AssaultGhostTimer
#include "CServerProxyMgr.h"      // CServerProxyMgr<CStatisticServerProxy> 完整类型
#include "TimerQueue.h"           // G_TimerQueue
#include "W9_EventSmall.h"        // CCharacBloodDungeon（GetEnterCount/IncEnterCount/IncPoint/IncUseGold）

namespace
{

// ---- 子对象视图宏（-O0 下函数形式会产生额外 call，ORIG 为 add $0xb24 内联直读，
//      故用宏保持调用点内联展开） ----
#define bf(p) ((p)->m_battleField)   // batch-16 内嵌具名
#define bd(p) ((p)->m_battleData)   // ORIG DWARF CParty+0x32c 具名化（本批专改）

// CStatisticMgr +0x18 = CDungeonExpStatistic 子对象（本体即 map；
// CStatisticMgr.h 仅建模 +0x00 m_levelDungeonPlay，本视图补 +0x18 区）。
struct CStatisticMgrExpView
{
    char m_pad[0x18];
    CDungeonExpStatistic m_dungeonExp;   // +0x18
};

// 统计包头部写入（PacketHeader(id,size) W 0x08110b70 字节序：[0..1]id/[2..3]size/[4..9]=0）
inline void writeStatHeader(unsigned char* buf, unsigned short id, unsigned short size)
{
    buf[0] = static_cast<unsigned char>(id & 0xff);
    buf[1] = static_cast<unsigned char>((id >> 8) & 0xff);
    buf[2] = static_cast<unsigned char>(size & 0xff);
    buf[3] = static_cast<unsigned char>((size >> 8) & 0xff);
    *reinterpret_cast<unsigned short*>(buf + 4) = 0;
    *reinterpret_cast<unsigned int*>(buf + 6) = 0;
}

}  // namespace

// ===========================================================================
// CDungeonExpStatistic::IncreaseDungeonExp —— T 0x0860efca
// 声明/布局见 CDungeonExpStatistic.h（权威头，替换原本地建模）；本 TU 保留实现。
// ===========================================================================
// ===========================================================================
// 统计包结构（ORIG PacketHeader(id,size) W 0x08110b70 基类 + packed 字段；
// 各 ctor W：Party 0x085bede6 / Job 0x085bee0a / Charac 0x085bee2e /
// Blood 0x085bee52。字段偏移逐条对应报告汇编写入目标。）
// ===========================================================================
#include "PacketHeader.h"

class Packet_Dungeon_Statistic_Party : public PacketHeader
{
public:
    Packet_Dungeon_Statistic_Party();
    unsigned short m_channel;        // +0x0a
    int m_dungeonIndex;              // +0x0c
    unsigned char m_dungeonDiff;     // +0x10
    unsigned char m_battleFlag355;   // +0x11（party+0x355）
    unsigned char m_battleFlag356;   // +0x12
    unsigned char m_battleFlag357;   // +0x13
    bool m_flag;                     // +0x14
    unsigned char m_collect;         // +0x15（user+0x8e054）
    int m_playTime_f00;              // +0x16
    int m_f04;                       // +0x1a
    int m_f08;                       // +0x1e
    int m_f0c;                       // +0x22
    int m_f10;                       // +0x26
    int m_f14;                       // +0x2a
    int m_f18;                       // +0x2e
    int m_f1c;                       // +0x32
    int m_f34;                       // +0x36
    int m_f30;                       // +0x3a
    unsigned short m_level_f24;      // +0x3e
} __attribute__((packed));
#include "PartyClosureC2/Packet_Dungeon_Statistic_Party.inc"
    : PacketHeader(0xc35, 0x40) {}

class Packet_Dungeon_Statistic_Party_Job : public PacketHeader
{
public:
    Packet_Dungeon_Statistic_Party_Job();
    unsigned short m_channel;        // +0x0a
    int m_dungeonIndex;              // +0x0c
    unsigned char m_dungeonDiff;     // +0x10
    unsigned char m_battleFlag355;   // +0x11
    unsigned char m_battleFlag356;   // +0x12
    unsigned char m_battleFlag357;   // +0x13
    bool m_flag;                     // +0x14
    unsigned char m_collect;         // +0x15
    int m_job;                       // +0x16
    unsigned char m_growType;        // +0x1a
    unsigned int m_memberCount;      // +0x1b
} __attribute__((packed));
#include "PartyClosureC2/Packet_Dungeon_Statistic_Party_Job.inc"
    : PacketHeader(0xc37, 0x1f) {}

class Packet_Dungeon_Statistic_Party_Charac : public PacketHeader
{
public:
    Packet_Dungeon_Statistic_Party_Charac();
    unsigned short m_channel;        // +0x0a
    int m_dungeonIndex;              // +0x0c
    unsigned char m_dungeonDiff;     // +0x10
    unsigned char m_battleFlag354;   // +0x11（party+0x354）
    bool m_flag;                     // +0x12
    int m_job;                       // +0x13
    unsigned char m_growType;        // +0x17
    unsigned char m_collect;         // +0x18
    int m_playTime_f00;              // +0x19
    int m_f04;                       // +0x1d
    int m_f08;                       // +0x21
    int m_f0c;                       // +0x25
    int m_f10;                       // +0x29
    int m_f14;                       // +0x2d
    int m_f18;                       // +0x31
    int m_f1c;                       // +0x35
    int m_f34;                       // +0x39
    int m_f30;                       // +0x3d
    unsigned short m_level_f24;      // +0x41
    unsigned int m_memberCount;      // +0x43
} __attribute__((packed));
#include "PartyClosureC2/Packet_Dungeon_Statistic_Party_Charac.inc"
    : PacketHeader(0xc39, 0x47) {}

class Packet_Blood_dungeon_statistic : public PacketHeader
{
public:
    Packet_Blood_dungeon_statistic();
    char m_padA[4];                  // +0x0a（ORIG 栈上未初始化）
    int m_level;                     // +0x0e
    unsigned char m_one;             // +0x12（恒 1）
    unsigned char m_success;         // +0x13
} __attribute__((packed));
#include "PartyClosureC2/Packet_Blood_dungeon_statistic.inc"



#include "PartyClosureC2/IncreaseDungeonExp.inc"


#include "PartyClosureC2/RegisterNextTimer.inc"


// ===========================================================================
// pvp_assault::CAssaultMgr::NotifyGhostTime —— T 0x082eff42
// 报告：docs/class_func_reports/pvp_assault__CAssaultMgr/NotifyGhostTime.md
// 发 0x185 包（put_int(a)、put_int(ghostTime)）后注册幽灵计时器。
// ===========================================================================
namespace pvp_assault
{
#include "PartyClosureC2/NotifyGhostTime.inc"

}  // namespace pvp_assault

#include "PartyClosureC2/IsPriorityJob.inc"


#include "PartyClosureC2/onClearMap.inc"



#include "PartyClosureC2/enumPartyMember.inc"


#include "PartyClosureC2/getPartyMemberAvgLevel.inc"


#include "PartyClosureC2/set_basic_reward.inc"


#include "PartyClosureC2/increase_hell_party_ratio.inc"


#include "PartyClosureC2/CheckQuestConnection.inc"


#include "PartyClosureC2/GetRoutingDiceMemberList.inc"


#include "PartyClosureC2/SendDungeonStatisticParty.inc"


#include "PartyClosureC2/SendDungeonStatisticPartyJob.inc"


#include "PartyClosureC2/SendDungeonStatisticPartyCharac.inc"


#include "PartyClosureC2/SendDungeonCharacInfo.inc"


#include "PartyClosureC2/do_after_dungeon_start_checkndelete_item.inc"


#include "PartyClosureC2/do_after_dungeon_start.inc"


#include "PartyClosureC2/OnDungeonClear.inc"

