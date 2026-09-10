// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 PartyClosureC2/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// PartyClosureC2.cpp —— CParty 闭环剩余方法独占恢复（2026-08-24，PartyClosureC2）
//
// 覆盖 ORIG 符号（除注明 objdump 转写者外，全部完整读取
// docs/class_func_reports/ 对应 .md 的 ## 汇编 + ## 反编译 C 后转写）：
//
//   主目标（任务指派五函数）：
//   CParty::CheckQuestConnection(int, ENUM_MAZE_QUEST_TYPE)      T 0x085b6478
//   CParty::GetRoutingDiceMemberList(vector<pair<int,int>>&,CItem*) T 0x085ba294
//   CParty::OnDungeonClear(CUser*, bool)                         T 0x085b5518
//   CParty::do_after_dungeon_start(CDungeon const*, CUser*)      T 0x0859fcfc
//   CParty::increase_hell_party_ratio(char*)                     T 0x0859bba8
//
//   被调方闭环（本批次独占，全工程此前无定义）：
//   CParty::getPartyMemberAvgLevel()                             T 0x0859f582
//   CParty::set_basic_reward(BasicRewardType, Param, Result&)    T 0x0859b992
//   CParty::SendDungeonCharacInfo(CUser*, bool)                  T 0x085b5098
//   CParty::SendDungeonStatisticParty(bool, CUser*)              T 0x085b5158
//   CParty::SendDungeonStatisticPartyJob(bool, CUser*)           T 0x085b52a6
//   CParty::SendDungeonStatisticPartyCharac(bool, CUser*)        T 0x085b53aa
//   CParty::do_after_dungeon_start_checkndelete_item(...)        T 0x085a0756
//   CParty::enumPartyMember(bool (*)(CUser*))                    T 0x085b669a
//   CBattle_Field::onClearMap(bool)                              T 0x0830dd2c
//   pvp_assault::CAssaultMgr::NotifyGhostTime(CUser*,int,int)    T 0x082eff42
//   Timer_AssaultGhostTimer::RegisterNextTimer(CUser*,int,uint)  T 0x0863b242
//   CDungeonExpStatistic::IncreaseDungeonExp(int,int,int)        T 0x0860efca
//   CItem::IsPriorityJob(int,int)                                W 0x085beec8
//
// 死代码块注记（均经 ORIG ELF objdump 复核，按 Ghidra 活动路径转写；
// **两处死块已按 P337 用恒 0 volatile 守卫复现**，见各函数判词）：
//   - do_after_dungeon_start +0x654..+0x7c2（vilmark 检查/delete_item/cMyTrace
//     "UseAncientDungeonItems" 块）：唯一入口 0x85a034b 为 e9 无条件 jmp 直达
//     +0x7c7 包构造块，无任何跳转落入该区间。
//   - do_after_dungeon_start_checkndelete_item +0x2b..+0x126（GetInvenSlot/
//     delete_item/"isEventDungeon" 块）：入口 0x85a077c 同为 e9 jmp
//     直达 +0x12b 包构造块。两处均为编译器残留死代码。
//   P337 依据：`if(0)`/goto 恒被 4.4 -O0 DCE，而 volatile 守卫的读取不可省略
//   ⇒ 块体与全部 ORIG 调用点强制发射；运行期不可达（与 ORIG 的 e9 jmp 等价）。
//
// 统计包布局注记：ORIG PacketHeader(id,size) ctor（W 0x08110b70）写 [0..1]=id、
// [2..3]=size、[4..9]=0；各统计包负载字段偏移逐条对应报告汇编 mov 目标。
// Packet_Blood_dungeon_statistic 的 +0xa..+0xd 在 ORIG 中为未初始化栈值，
// 此处按工程惯例（Dispatcher_CompoundEmblem_impl.cpp）整体清零后写头部。
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
}

// ===========================================================================
// Timer_AssaultGhostTimer::RegisterNextTimer —— T 0x0863b242（无报告：objdump 转写）
// user 非空时：G_TimerQueue()->InsertTimer(OBJ_TYPE 0, user->GetUID(),
//   TIMER_MESSAGE 0xa0(调度槽 160), sec, 0, characNo)。
// ===========================================================================
#include "PartyClosureC2/RegisterNextTimer.inc"
}

// ===========================================================================
// pvp_assault::CAssaultMgr::NotifyGhostTime —— T 0x082eff42
// 报告：docs/class_func_reports/pvp_assault__CAssaultMgr/NotifyGhostTime.md
// 发 0x185 包（put_int(a)、put_int(ghostTime)）后注册幽灵计时器。
// ===========================================================================
namespace pvp_assault
{
#include "PartyClosureC2/NotifyGhostTime.inc"
}
}  // namespace pvp_assault

// ===========================================================================
// CItem::IsPriorityJob —— W 0x085beec8（无报告：objdump 转写）
// m_vecC0(+0xc0) 为 vector<pair<job,growType>>；growType 归一化：
//   {1,0x11}→1、{2,0x12}→2、{3,0x13}→3、{4,0x14}→4、其余→0；
// 空表返回 false；存在 pair.first==job 且 pair.second==归一化值 → true。
// ===========================================================================
#include "PartyClosureC2/IsPriorityJob.inc"
}

// ===========================================================================
// CBattle_Field::onClearMap —— T 0x0830dd2c
// 报告：docs/class_func_reports/CBattle_Field/onClearMap.md
// playInfo.m_clearTick=now；isClear 且 dungeonIndex==0x29 时按游玩 tick 与
// useItem 计数对全员 addUserHackCount149/150。
// ===========================================================================
#include "PartyClosureC2/onClearMap.inc"
}


// ===========================================================================
// CParty::enumPartyMember —— T 0x085b669a
// 报告缺失标记：无独立报告；objdump 逐指令转写（onClearMap 依赖闭环）。
// i=0..3：无效槽跳过；callback 返回 false 时提前结束遍历。
// ===========================================================================
#include "PartyClosureC2/enumPartyMember.inc"
}

// ===========================================================================
// CParty::getPartyMemberAvgLevel —— T 0x0859f582
// 报告：getPartyMemberAvgLevel.md。有效成员等级和/人数；无人 → 0。
// ===========================================================================
#include "PartyClosureC2/getPartyMemberAvgLevel.inc"
}

// ===========================================================================
// CParty::set_basic_reward —— T 0x0859b992
// 报告：set_basic_reward.md。仅快速队伍生效：
//   1) param.m_param4 已在奖励管理器副本列表中 → 忽略；
//   2) get_reward_type_object(BASIC, type) 命中 → vtable 槽1 process(param,result)。
// ===========================================================================
#include "PartyClosureC2/set_basic_reward.inc"
}

// ===========================================================================
// CParty::increase_hell_party_ratio —— T 0x0859bba8
// 报告：increase_hell_party_ratio.md。平均等级 >59 时走 HellPartyDifficulty
// 基础奖励：str[1]/str[2] 作为 RewardData_Result 写入槽进出 process。
// ===========================================================================
#include "PartyClosureC2/increase_hell_party_ratio.inc"
}

// ===========================================================================
// CParty::CheckQuestConnection —— T 0x085b6478
// 报告：CheckQuestConnection.md：
// type==0 → 任一有效成员 IsDoingQuest；type==1 → isClearQuest；否则 false。
// ===========================================================================
#include "PartyClosureC2/CheckQuestConnection.inc"
}

// ===========================================================================
// CParty::GetRoutingDiceMemberList —— T 0x085ba294
// 报告：GetRoutingDiceMemberList.md。三段降级收集路由骰子参与者：
//   ① 路由物品且（状态1+优先职业匹配）② 结果为空且（状态1+职业可用）
//   ③ 结果仍为空且（状态1）。元素 pair<slot, curCharacNo>。
// ===========================================================================
#include "PartyClosureC2/GetRoutingDiceMemberList.inc"
}

// ===========================================================================
// CParty::SendDungeonStatisticParty —— T 0x085b5158
// 报告：SendDungeonStatisticParty.md。0x40 字节统计包（header id 0xc35）。
// ===========================================================================
#include "PartyClosureC2/SendDungeonStatisticParty.inc"
}

// ===========================================================================
// CParty::SendDungeonStatisticPartyJob —— T 0x085b52a6
// 报告：SendDungeonStatisticPartyJob.md。0x1f 字节统计包（header id 0xc37）。
// ===========================================================================
#include "PartyClosureC2/SendDungeonStatisticPartyJob.inc"
}

// ===========================================================================
// CParty::SendDungeonStatisticPartyCharac —— T 0x085b53aa
// 报告：SendDungeonStatisticPartyCharac.md。0x47 字节统计包（header id 0xc39）。
// ===========================================================================
#include "PartyClosureC2/SendDungeonStatisticPartyCharac.inc"
}

// ===========================================================================
// CParty::SendDungeonCharacInfo —— T 0x085b5098
// 报告：SendDungeonCharacInfo.md。
// ===========================================================================
#include "PartyClosureC2/SendDungeonCharacInfo.inc"
}

// ===========================================================================
// CParty::do_after_dungeon_start_checkndelete_item —— T 0x085a0756
// 报告：do_after_dungeon_start_checkndelete_item.md。
// 入口 jmp 直达包构造块（中间 check/delete 块为死代码，见文件头注记）：
// put_header(0,0xe)/put_byte(0)/put_short(1)/MakeItemPacket(INVEN 1, slot=0)/
// finalize(true)/Send，恒返回 0。
// ===========================================================================
#include "PartyClosureC2/do_after_dungeon_start_checkndelete_item.inc"
}

// ===========================================================================
// CParty::do_after_dungeon_start —— T 0x0859fcfc
// 报告：do_after_dungeon_start.md（活动路径；死代码块见文件头注记）。
// 错误码：0xad 虚检/维度券缺失、0x11 必需道具缺失或复仇票不足、0x15 非开放
// 定时副本；成功 0。血腥/维度/终极/锦标赛入场扣减与事件邀请删除均在主路径。
// ===========================================================================
#include "PartyClosureC2/do_after_dungeon_start.inc"
}

// ===========================================================================
// CParty::OnDungeonClear —— T 0x085b5518
// 报告：OnDungeonClear.md。统计上报、个人结算回调、历史日志、战场清场、
// 血腥统计包、村庄怪物战败幽灵惩罚。
// ===========================================================================
#include "PartyClosureC2/OnDungeonClear.inc"
}
