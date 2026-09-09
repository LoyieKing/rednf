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
bool IsCollectDungeonStatistic();
// ORIG T 0x0830dc9d / 0x0830dce4（CBattle_Field_helpers.cpp 定义，无权威头声明；
// onClearMap 的 enumPartyMember 回调）
bool addUserHackCount149(CUser* user);
bool addUserHackCount150(CUser* user);

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
Packet_Dungeon_Statistic_Party::Packet_Dungeon_Statistic_Party()
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
Packet_Dungeon_Statistic_Party_Job::Packet_Dungeon_Statistic_Party_Job()
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
Packet_Dungeon_Statistic_Party_Charac::Packet_Dungeon_Statistic_Party_Charac()
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
Packet_Blood_dungeon_statistic::Packet_Blood_dungeon_statistic()
    : PacketHeader(0x1036, 0x14) {}


void CDungeonExpStatistic::IncreaseDungeonExp(int level, int exp, int count)
{
    // [b28d-solo-PartyClosureC2] ORIG 0x0860efca：
    //   P227 全表达式临时链（ORIG `mov 0xc(%ebp),%ebx` 在 ctor 前缓存 level）；
    //   P47/P222 极性交换（ORIG insert 路径直落、+= 路径在 else）；
    //   P0 具名引用缓存 `it->second`（ORIG `call operator->; add $4; mov %eax,-0xc`）。
    if (level < 1 || level > 200)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x581, 0)(
            "DUNGEON_EXP_STATISTIC wrong level(%d)", level);
        return;
    }
    std::map<int, stDungeonExp>::iterator it = m_map.find(level);
    if (m_map.end() == it)   // ORIG this=end 临时(-0x38)、arg=it(-0x4c)
    {
        stDungeonExp entry;
        entry.m_exp = exp;
        entry.m_count = count;
        entry.m_clearTimes = 1;
        m_map.insert(std::make_pair(level, entry));
    }
    else
    {
        stDungeonExp& entry = it->second;
        entry.m_exp += exp;
        entry.m_count += count;
        entry.m_clearTimes += 1;
    }
}

// ===========================================================================
// Timer_AssaultGhostTimer::RegisterNextTimer —— T 0x0863b242（无报告：objdump 转写）
// user 非空时：G_TimerQueue()->InsertTimer(OBJ_TYPE 0, user->GetUID(),
//   TIMER_MESSAGE 0xa0(调度槽 160), sec, 0, characNo)。
// ===========================================================================
void Timer_AssaultGhostTimer::RegisterNextTimer(CUser* user, int sec,
                                                unsigned int characNo)
{
    if (user == 0)
        return;
    G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)0, (int)user->GetUID(),
                                (TIMER_MESSAGE)0xa0, sec, (unsigned int)0,
                                characNo);
}

// ===========================================================================
// pvp_assault::CAssaultMgr::NotifyGhostTime —— T 0x082eff42
// 报告：docs/class_func_reports/pvp_assault__CAssaultMgr/NotifyGhostTime.md
// 发 0x185 包（put_int(a)、put_int(ghostTime)）后注册幽灵计时器。
// ===========================================================================
namespace pvp_assault
{
void CAssaultMgr::NotifyGhostTime(CUser* user, int a, int ghostTime)
{
    if (user == 0)
        return;

    PacketGuard packet;
    packet.clear();
    packet.put_header(0, 0x185);
    packet.put_int(a);
    packet.put_int(ghostTime);
    packet.finalize(true);
    user->Send(packet);

    Timer_AssaultGhostTimer::RegisterNextTimer(user, ghostTime,
                                               (unsigned int)user->getCurCharacNo());
}
}  // namespace pvp_assault

// ===========================================================================
// CItem::IsPriorityJob —— W 0x085beec8（无报告：objdump 转写）
// m_vecC0(+0xc0) 为 vector<pair<job,growType>>；growType 归一化：
//   {1,0x11}→1、{2,0x12}→2、{3,0x13}→3、{4,0x14}→4、其余→0；
// 空表返回 false；存在 pair.first==job 且 pair.second==归一化值 → true。
// ===========================================================================
bool CItem::IsPriorityJob(int job, int growType)
{
    if (m_vecC0.empty())
        return false;

    int normalizedGrowType = 0;
    if (growType == 1 || growType == 0x11)       // ORIG cmp/je+cmp/jne 链
        normalizedGrowType = 1;
    else if (growType == 2 || growType == 0x12)
        normalizedGrowType = 2;
    else if (growType == 3 || growType == 0x13)
        normalizedGrowType = 3;
    else if (growType == 4 || growType == 0x14)
        normalizedGrowType = 4;

    for (std::vector<std::pair<int, int> >::const_iterator it = m_vecC0.begin();
         it != m_vecC0.end(); it++)
    {
        const std::pair<int, int>& info = *it;
        if (info.first == job && info.second == normalizedGrowType)
            return true;
    }
    return false;
}

// ===========================================================================
// CBattle_Field::onClearMap —— T 0x0830dd2c
// 报告：docs/class_func_reports/CBattle_Field/onClearMap.md
// playInfo.m_clearTick=now；isClear 且 dungeonIndex==0x29 时按游玩 tick 与
// useItem 计数对全员 addUserHackCount149/150。
// ===========================================================================
void CBattle_Field::onClearMap(bool isClear)
{
    MapInfo* mi = GetCurrentMapInfo();
    mi->m_playInfo.m_clearTick = GlobalData::s_systemTime_.getCurTickCount();

    if (isClear)
    {
        if (m_dungeonIndex == 0x29)
        {
            // 限时阈值：单人 120000ms；多人 0x1d4c1（getPlayTick<=limit 判定）
            unsigned int limitTick = 120000;
            if (m_party->get_member_count() > 1)   // ORIG *(CParty**)this == m_party(+0x00)
                limitTick = 0x1d4c1;

            if (m_party->checkBossRoom())
            {
                if (mi->m_playInfo.getPlayTick() <= limitTick)
                    m_party->enumPartyMember(addUserHackCount149);
            }
            if (m_party->checkBossRoom())
            {
                if (mi->m_playInfo.m_useItemType2Count == 0)
                {
                    if (mi->m_playInfo.m_useItemOtherCount == 0)
                        m_party->enumPartyMember(addUserHackCount150);
                }
            }
        }
    }
}


// ===========================================================================
// CParty::enumPartyMember —— T 0x085b669a
// 报告缺失标记：无独立报告；objdump 逐指令转写（onClearMap 依赖闭环）。
// i=0..3：无效槽跳过；callback 返回 false 时提前结束遍历。
// ===========================================================================
void CParty::enumPartyMember(bool (*func)(CUser*))
{
    // 报告：docs/class_func_reports/CParty/enumPartyMember.md（T 0x085b669a）：
    // i=0..3，有效槽调用回调；回调返回 false 提前结束遍历。
    for (int i = 0; i <= 3; ++i)
    {
        if (_checkValidUser(i))
        {
            if (!func(m_member[i].m_pUser))   // xor $1/test/jne：false → ret
                return;
        }
    }
}

// ===========================================================================
// CParty::getPartyMemberAvgLevel —— T 0x0859f582
// 报告：getPartyMemberAvgLevel.md。有效成员等级和/人数；无人 → 0。
// ===========================================================================
int CParty::getPartyMemberAvgLevel()
{
    // 报告：docs/class_func_reports/CParty/getPartyMemberAvgLevel.md（T 0x0859f582）：
    // 有效成员等级和/人数；无人 → 0（-0x14 槽为 ORIG 保留死变量，保持帧布局一致）。
    int sum = 0;
    int level = 0;
    int validCount = 0;
    for (int i = 0; i <= 3; ++i)
    {
        if (!_checkValidUser(i))
            continue;
        sum += m_member[i].m_pUser->get_charac_level();
        ++validCount;
    }
    if (validCount == 0)
        return 0;
    return sum / validCount;
}

// ===========================================================================
// CParty::set_basic_reward —— T 0x0859b992
// 报告：set_basic_reward.md。仅快速队伍生效：
//   1) param.m_param4 已在奖励管理器副本列表中 → 忽略；
//   2) get_reward_type_object(BASIC, type) 命中 → vtable 槽1 process(param,result)。
// ===========================================================================
void CParty::set_basic_reward(QuickParty::BasicRewardType type,
                              QuickParty::RewardData_Param param,
                              QuickParty::RewardData_Result& result)
{
    // [b28d-solo-PartyClosureC2] ORIG 0x0859b992：p167 实证 ORIG 调
    // `vector<int>::begin() const` / `__normal_iterator<int const*,...>` ⇒
    // 先例形 `((const std::vector<int>&)...).begin()`（CBattle_Field.cpp:863 /
    // QuickParty.cpp:165）：const 重载直返 const_iterator，无转换构造 call，
    // sret 直接落 -0x10/-0x14 两槽（帧 0x24）。
    if (!is_quick_party())                      // xor $1 后 test：非快速队直接返回
        return;

    std::vector<int>::const_iterator it =
        ((const std::vector<int>&)
            G_CGameManager()->GetQuickPartyRewardManager()->get_dungeon_idx_list())
            .begin();
    std::vector<int>::const_iterator end =
        ((const std::vector<int>&)
            G_CGameManager()->GetQuickPartyRewardManager()->get_dungeon_idx_list())
            .end();
    for (; it != end; ++it)
    {
        if (param.m_param4 == *it)              // cmp 0x14(%ebp)（Param 第二字）
            return;
    }

    QuickParty::IRewardType* reward =
        G_CGameManager()->GetQuickPartyRewardManager()->get_reward_type_object(
            QuickParty::ENUM_REWARD_TYPE_BASIC, (unsigned int)type);
    if (reward != 0)
        reward->process(param, result);         // vtable 槽1
}

// ===========================================================================
// CParty::increase_hell_party_ratio —— T 0x0859bba8
// 报告：increase_hell_party_ratio.md。平均等级 >59 时走 HellPartyDifficulty
// 基础奖励：str[1]/str[2] 作为 RewardData_Result 写入槽进出 process。
// ===========================================================================
void CParty::increase_hell_party_ratio(char* str)
{
    if (!(getPartyMemberAvgLevel() > 0x3b))     // setle+jne：<=59 直接返回
        return;

    QuickParty::RewardData_Param param(this);   // C1 W 0x08469976（头内联同型）
    // ORIG：ctor 置 m_param4=0 后，-0xc(%ebp) 立即被 get_dungeon_index 覆写，
    // 即 Param 第二字 = 当前副本 idx（供 set_basic_reward 副本去重）。
    param.m_param4 = bf(this).get_dungeon_index();
    QuickParty::RewardData_Result result;       // clear 后 +0x14=1.0f/+0x18=1
    result.m_value00 = str[1];                  // movsbl str[1]
    result.m_value04 = str[2];                  // movsbl str[2]

    set_basic_reward(QuickParty::BASIC_REWARD_HELL_DIFFICULTY /*0xc*/, param,
                     result);

    str[1] = static_cast<char>(result.m_value00);
    str[2] = static_cast<char>(result.m_value04);
}

// ===========================================================================
// CParty::CheckQuestConnection —— T 0x085b6478
// 报告：CheckQuestConnection.md：
// type==0 → 任一有效成员 IsDoingQuest；type==1 → isClearQuest；否则 false。
// ===========================================================================
bool CParty::CheckQuestConnection(int questIdx, ENUM_MAZE_QUEST_TYPE type)
{
    // [b28d-46 判词·N/P168a③b] ORIG 0x085b6478：64 vs 66，p167 CLEAR；残余唯一处
    // `call; test %al; je`（uchar callee 直判）vs 4.4 `setne %al; test` 归一
    // （跨版本 bool ABI 差）⇒ 封顶。
    for (int i = 0; i <= 3; ++i)
    {
        if (_checkValidUser(i))
        {
            switch ((int)type)
            {
                case 0:
                    if (m_member[i].m_pUser->getCurCharacQuestR()->IsDoingQuest(
                            questIdx))
                        return true;
                    break;
                case 1:
                    if (m_member[i].m_pUser->getCurCharacQuestR()->isClearQuest(
                            questIdx))
                        return true;
                    break;
            }
        }
    }
    return false;
}

// ===========================================================================
// CParty::GetRoutingDiceMemberList —— T 0x085ba294
// 报告：GetRoutingDiceMemberList.md。三段降级收集路由骰子参与者：
//   ① 路由物品且（状态1+优先职业匹配）② 结果为空且（状态1+职业可用）
//   ③ 结果仍为空且（状态1）。元素 pair<slot, curCharacNo>。
// ===========================================================================
void CParty::GetRoutingDiceMemberList(std::vector<std::pair<int, int> >& list,
                                      CItem* item)
{
    // [b28d-solo-PartyClosureC2] ORIG 0x085ba294：
    //   P226 语句表达式复现 && 短路物化岛（ORIG 每条件 je 共享 false 岛 +
    //   mov $1/mov $0/test/je，且零具名 bool 槽）；
    //   P61 具名 characNo 左值 ⇒ make_pair<int&,int&> 直构 sret（ORIG -0x24/-0x18/-0x0c
    //   三枚 per-loop 左值，无额外 pair 拷贝构造）；
    //   P183 去 user 缓存（ORIG 每处重算 m_member[i].m_pUser）。
    if (item->IsRoutingItem())                  // ORIG 直接解引用 item（无判空）
    {
        for (int i = 0; i <= 3; ++i)
        {
            if (({ _checkValidUser(i) &&
                   m_routingData.GetMemberRoutingState(i) == 1 &&
                   item->IsPriorityJob(
                       m_member[i].m_pUser->get_charac_job(),
                       (int)(signed char)m_member[i].m_pUser->getCurCharacGrowType()); }))
            {
                // P192：ORIG mangled 为 pair<int,int>::pair<int&,unsigned int>
                // （converting ctor）⇒ value_type(a,b) 直构，非 make_pair。
                list.push_back(std::pair<int, int>(
                    i, m_member[i].m_pUser->getCurCharacNo()));
            }
        }
    }

    if (list.empty())
    {
        for (int i = 0; i <= 3; ++i)
        {
            if (({ _checkValidUser(i) &&
                   m_routingData.GetMemberRoutingState(i) == 1 &&
                   item->check_job_type(
                       (char)m_member[i].m_pUser->get_charac_job()); }))
            {
                list.push_back(std::pair<int, int>(
                    i, m_member[i].m_pUser->getCurCharacNo()));
            }
        }
    }

    if (list.empty())
    {
        for (int i = 0; i <= 3; ++i)
        {
            if (({ _checkValidUser(i) &&
                   m_routingData.GetMemberRoutingState(i) == 1; }))
            {
                list.push_back(std::pair<int, int>(
                    i, m_member[i].m_pUser->getCurCharacNo()));
            }
        }
    }
}

// ===========================================================================
// CParty::SendDungeonStatisticParty —— T 0x085b5158
// 报告：SendDungeonStatisticParty.md。0x40 字节统计包（header id 0xc35）。
// ===========================================================================
bool CParty::SendDungeonStatisticParty(bool flag, CUser* user)
{
    // [b28d-solo-PartyClosureC2] ORIG 0x085b5158：P52 单语句链式复现
    // `lea &pkt→%ebx` 跨 GetServerProxy 保活（具名 proxy 局部多槽 + 帧 0x78→0x64）。
    // 报告：docs/class_func_reports/CParty/SendDungeonStatisticParty.md（T 0x085b5158）：
    // 0x40 字节统计包（header id 0xc35），字段偏移逐条对应报告汇编。
    Packet_Dungeon_Statistic_Party pkt;
    pkt.m_channel = (unsigned short)G_CEnvironment()->get_channel_no();
    pkt.m_dungeonIndex = bf(this).get_dungeon_index();
    pkt.m_dungeonDiff = (unsigned char)bf(this).get_dungeon_diff();
    pkt.m_battleFlag355 = bd(this).m_flag28[1];   // party+0x355
    pkt.m_battleFlag356 = bd(this).m_flag28[2];   // party+0x356
    pkt.m_battleFlag357 = bd(this).m_flag28[3];   // party+0x357
    pkt.m_flag = flag;
    pkt.m_collect = (unsigned char)user->m_character.m_f20;   // +0x8e054
    pkt.m_playTime_f00 = user->m_character.m_f00;             // +0x8e034
    pkt.m_f04 = user->m_character.m_f04;
    pkt.m_f08 = user->m_character.m_f08;
    pkt.m_f0c = user->m_character.m_f0c;
    pkt.m_f10 = user->m_character.m_f10;
    pkt.m_f14 = user->m_character.m_f14;
    pkt.m_f18 = user->m_character.m_f18;
    pkt.m_f1c = user->m_character.m_f1c;
    pkt.m_f34 = user->m_character.m_f34;
    pkt.m_f30 = user->m_character.m_f30;
    pkt.m_level_f24 = (unsigned short)user->m_character.m_f24;

    GlobalData::s_statistic_proxy_mgr->GetServerProxy(user->GetServerGroup())
        ->SendPacket(reinterpret_cast<char*>(&pkt), 0x40);
    return true;
}

// ===========================================================================
// CParty::SendDungeonStatisticPartyJob —— T 0x085b52a6
// 报告：SendDungeonStatisticPartyJob.md。0x1f 字节统计包（header id 0xc37）。
// ===========================================================================
bool CParty::SendDungeonStatisticPartyJob(bool flag, CUser* user)
{
    // [b28d-solo-PartyClosureC2] ORIG 0x085b52a6：
    //   P0/P235 具名成员视图替 helper（m_resultView.m_resultField48@+0x183c
    //   复现 ORIG `movzwl 0x183c(%eax)`，删 EXTRA call）；
    //   P52 单语句链式 `GetServerProxy(...)->SendPacket(&pkt,0x1f)` 复现
    //   `lea &pkt→%ebx` 跨调用保活（具名 proxy 局部多 -0xc 槽 + 帧 0x58→0x44）。
    Packet_Dungeon_Statistic_Party_Job pkt;
    pkt.m_channel = (unsigned short)G_CEnvironment()->get_channel_no();
    pkt.m_dungeonIndex = bf(this).get_dungeon_index();
    pkt.m_dungeonDiff = (unsigned char)bf(this).get_dungeon_diff();
    pkt.m_battleFlag355 = bd(this).m_flag28[1];
    pkt.m_battleFlag356 = bd(this).m_flag28[2];
    pkt.m_battleFlag357 = bd(this).m_flag28[3];
    pkt.m_flag = flag;
    pkt.m_job = user->get_charac_job();
    pkt.m_growType = (unsigned char)user->getCurCharacGrowType();
    pkt.m_collect = (unsigned char)user->m_character.m_f20;
    if (flag)                                                   // party+0x183c
        pkt.m_memberCount = (unsigned int)m_resultView.m_resultField48;
    else
        pkt.m_memberCount = 0;

    GlobalData::s_statistic_proxy_mgr->GetServerProxy(user->GetServerGroup())
        ->SendPacket(reinterpret_cast<char*>(&pkt), 0x1f);
    return true;
}

// ===========================================================================
// CParty::SendDungeonStatisticPartyCharac —— T 0x085b53aa
// 报告：SendDungeonStatisticPartyCharac.md。0x47 字节统计包（header id 0xc39）。
// ===========================================================================
bool CParty::SendDungeonStatisticPartyCharac(bool flag, CUser* user)
{
    // [b28d-solo-PartyClosureC2] ORIG 0x085b53aa：P235 具名成员视图替 helper
    // （删 EXTRA call）＋ P52 单语句链式（帧 0x78→0x74、&pkt 入 %ebx）。
    Packet_Dungeon_Statistic_Party_Charac pkt;
    pkt.m_channel = (unsigned short)G_CEnvironment()->get_channel_no();
    pkt.m_dungeonIndex = bf(this).get_dungeon_index();
    pkt.m_dungeonDiff = (unsigned char)bf(this).get_dungeon_diff();
    pkt.m_battleFlag354 = bd(this).m_flag28[0];   // party+0x354
    pkt.m_flag = flag;
    pkt.m_job = user->get_charac_job();
    pkt.m_growType = (unsigned char)user->getCurCharacGrowType();
    pkt.m_collect = (unsigned char)user->m_character.m_f20;
    pkt.m_playTime_f00 = user->m_character.m_f00;
    pkt.m_f04 = user->m_character.m_f04;
    pkt.m_f08 = user->m_character.m_f08;
    pkt.m_f0c = user->m_character.m_f0c;
    pkt.m_f10 = user->m_character.m_f10;
    pkt.m_f14 = user->m_character.m_f14;
    pkt.m_f18 = user->m_character.m_f18;
    pkt.m_f1c = user->m_character.m_f1c;
    pkt.m_f34 = user->m_character.m_f34;
    pkt.m_f30 = user->m_character.m_f30;
    pkt.m_level_f24 = (unsigned short)user->m_character.m_f24;
    if (flag)
        pkt.m_memberCount = (unsigned int)m_resultView.m_resultField48;
    else
        pkt.m_memberCount = 0;

    GlobalData::s_statistic_proxy_mgr->GetServerProxy(user->GetServerGroup())
        ->SendPacket(reinterpret_cast<char*>(&pkt), 0x47);
    return true;
}

// ===========================================================================
// CParty::SendDungeonCharacInfo —— T 0x085b5098
// 报告：SendDungeonCharacInfo.md。
// ===========================================================================
bool CParty::SendDungeonCharacInfo(CUser* user, bool flag)
{
    // 报告：docs/class_func_reports/CParty/SendDungeonCharacInfo.md（T 0x085b5098）：
    // 游玩时长/等级快照写入 +0x8e034/+0x8e058，收集开关开启时依次发三个统计包。
    user->m_character.m_f00 =
        GlobalData::s_systemTime_.getCurSec() - m_field185c;   // +0x8e034
    user->m_character.m_f24 = user->get_charac_level();        // +0x8e058

    if (user->m_character.m_f20 != 0)                          // +0x8e054 收集开关
        SendDungeonStatisticParty(flag, user);
    if (user->m_character.m_f20 != 0)
        SendDungeonStatisticPartyJob(flag, user);
    if (user->m_character.m_f20 != 0)
        SendDungeonStatisticPartyCharac(flag, user);
    return true;
}

// ===========================================================================
// CParty::do_after_dungeon_start_checkndelete_item —— T 0x085a0756
// 报告：do_after_dungeon_start_checkndelete_item.md。
// 入口 jmp 直达包构造块（中间 check/delete 块为死代码，见文件头注记）：
// put_header(0,0xe)/put_byte(0)/put_short(1)/MakeItemPacket(INVEN 1, slot=0)/
// finalize(true)/Send，恒返回 0。
// ===========================================================================
int CParty::do_after_dungeon_start_checkndelete_item(const RequiredItem& item,
                                                     CUser* user)
{
    // [b28d-solo-PartyClosureC2 判词·P337] ORIG 0x085a0756。
    // 旧判词 N111「死块不可造」被 P337 推翻：恒 0 **volatile** 守卫的
    // 读取不可省略，块体与 ORIG 7 个调用点强制发射（`if(0)`/goto 才会 DCE）。
    // 死块运行期不可达（与 ORIG 入口 jmp 等价），活动路径语义零变化。
    // 现读数：133 vs 134（gap −1）、p167 19 vs 19（余差仅 PacketGuard/cMyTrace
    // C1↔C2/D1↔D2 别名对）、帧 0x90 vs ORIG 0x80（N82 槽阶梯残余）。
    int slot = 0;                                         // ORIG -0xc（+0x008 movl）
    const CInventory* inv = user->getCurCharacInvenR();   // ORIG +0x1a（返回值仅死块用）

    volatile bool neverEnter = false;                     // P337 恒 0 守卫
    if (neverEnter)
    {
        // ORIG +0x2b：+0x1a 返回值直存 slot 槽再与 -1 比较（按指令转写）
        slot = (int)reinterpret_cast<size_t>(inv);
        const bool deadMissing = (slot == -1);                    // ORIG +0x2e
        if (deadMissing)
            return 0x11;
        const Inven_Item deadItem =
            user->getCurCharacInvenR()->GetInvenSlot(1, slot);    // ORIG +0x43..+0x6c
        if (deadItem.m_addInfo2 < item.m_itemCount)               // ORIG +0x6f..+0x7a
            return 0x11;
        if (!user->getCurCharacInvenW()->delete_item(             // ORIG +0x86..+0xc2
                (INVEN_TYPE)1, slot, item.m_itemCount, (eItemDelReason)0xe, true))
        {
            cMyTrace trace(__PRETTY_FUNCTION__, 0xdfa, 5);        // ORIG +0xdc..+0xfa
            trace("CParty::isEventDungeon, delete_item failed , User ch=%d , %d %d",
                  user->getCurCharacNo(), slot, item.m_itemCount); // ORIG +0xff..+0x11c
            return 0x11;
        }
    }

    PacketGuard packet;
    packet.put_header(0, 0xe);
    packet.put_byte(0);
    packet.put_short(1);
    user->getCurCharacInvenR()->MakeItemPacket((INVEN_TYPE)1, slot, packet);
    packet.finalize(true);
    user->Send(packet);
    return 0;
}

// ===========================================================================
// CParty::do_after_dungeon_start —— T 0x0859fcfc
// 报告：do_after_dungeon_start.md（活动路径；死代码块见文件头注记）。
// 错误码：0xad 虚检/维度券缺失、0x11 必需道具缺失或复仇票不足、0x15 非开放
// 定时副本；成功 0。血腥/维度/终极/锦标赛入场扣减与事件邀请删除均在主路径。
// ===========================================================================
int CParty::do_after_dungeon_start(const CDungeon* dungeon, CUser* user)
{
    // [b28d-solo-PartyClosureC2] ORIG 0x0859fcfc：
    //   P168a bool callee 裸形（ORIG `call *%edx; xor $0x1,%eax; test; je`）；
    //   P214③/P312 `char msg[0x100] = {0}` 复现 4.1 内联 rep stos 慢路（N78）；
    //   P0 fmt/strlen 内联表达式（ORIG 无具名 fmt 槽）；
    //   P167 血腥段补真实调用族 GetCharacExpandData×3 + GetEnterCount/IncEnterCount/
    //   IncPoint/IncUseGold（原 BloodDungeonCountsView 裸字段视图缺 7 个调用点）；
    //   P133/P196 维度券三条件独立物化 + 必需道具段顺序 if + 提前 return（Ghidra C 实证）。
    //   P337 死块 +0x654..+0x7c2（vilmark/check_item/GetInvenSlot/delete_item/
    //   cMyTrace）已用恒 0 volatile 守卫复现：9 个调用点全部发射、运行期不可达。
    //   现读数：686 vs 687（gap −1）、p167 91 vs 91（余差仅 PacketGuard/cMyTrace
    //   C1↔C2/D1↔D2 别名对）、帧 0x2cc 与 ORIG 一致（msg 槽 -0x1b5 vs ORIG
    //   -0x1a9 属 N82 槽阶梯残余）；_do_after_dungeon_start 返回 int→bool 需
    //   CDungeon.h:665/849 + CDungeon.cpp:584/664 跨 TU 同步（已上报 Main）。
    // 虚表槽0：CDungeon::_do_after_dungeon_start(user)
    if (!dungeon->_do_after_dungeon_start(user))
        return 0xad;

    user->setCurCharacLastPlayDungeonIndex(dungeon->get_index());

    // ---- 血腥副本入场（dungeon->m_bloodDungeonType == 1） ----
    if (dungeon->m_bloodDungeonType == 1)
    {
        if (m_battleField.m_bloodState == 1)
        {
            // 提示串：findString(表4,"game_server_msg_110") 作为 snprintf 格式串，
            // 单人模式费用参数为 0
            char msg[0x100] = {0};
            OS_API::snprintf(
                msg, 0xff,
                g_scriptStringManager_.findString(4, "game_server_msg_110", 0), 0);
            PacketGuard packet;
            packet.put_header(0, 0xc);
            packet.put_byte(0);
            packet.put_short(0);
            packet.put_byte(0);
            packet.put_int((int)strlen(msg));
            packet.put_str(msg, (int)strlen(msg));
            packet.finalize(true);
            user->Send(packet);
            return 0;
        }

        const int fee = get_blood_dungeon_admission_fee(user);
        user->getCurCharacInvenW()->use_money(fee, (eMoneySubReason)0x22, true);
        user->SendUpdateItemList((CUser::eSendTarget)1, (ENUM_ITEMSPACE)0, 0);

        char msg[0x100] = {0};
        OS_API::snprintf(
            msg, 0xff,
            g_scriptStringManager_.findString(4, "game_server_msg_110", 0), fee);
        PacketGuard packet;
        packet.put_header(0, 0xc);
        packet.put_byte(0);
        packet.put_short(0);
        packet.put_byte(0);
        packet.put_int((int)strlen(msg));
        packet.put_str(msg, (int)strlen(msg));
        packet.finalize(true);
        user->Send(packet);

        // 血腥扩展数据计数（P167：ORIG 全走真实方法调用，各点独立
        // GetCharacExpandData(1) 重取，Inc* 内部自带 alter()）
        int enterCount = reinterpret_cast<CCharacBloodDungeon*>(
            const_cast<CExpandEquipslot*>(
                user->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)1)))
            ->GetEnterCount();
        const int lottePoint = G_CDataManager()
                                   ->m_serverParameter.GetLottePoint(enterCount);
        const int level = user->get_charac_level();
        const int avgPrice =
            G_CDataManager()->m_serverParameter.GetPriceAverage(level);
        reinterpret_cast<CCharacBloodDungeon*>(const_cast<CExpandEquipslot*>(
            user->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)1)))
            ->IncEnterCount();
        reinterpret_cast<CCharacBloodDungeon*>(const_cast<CExpandEquipslot*>(
            user->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)1)))
            ->IncPoint(lottePoint);
        reinterpret_cast<CCharacBloodDungeon*>(const_cast<CExpandEquipslot*>(
            user->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)1)))
            ->IncUseGold(fee - avgPrice);
    }

    // ---- 维度副本入场券（dimension possible>0） ----
    if ((signed char)dungeon->get_dimension_possible() > 0)
    {
        if (m_battleField.m_bloodState == 1)
            return 0;
        const int dimIdx = (signed char)dungeon->get_dimension_possible() - 1;
        // P311：具名成员数组 m_demensionInout[dimIdx] 复现单条
        // `movzbl 0xeb9(%eax,%edx,1)`（裸偏移恒 add 链）。
        const signed char ticketLeft =
            (signed char)user->getCurCharacR()->m_demensionInout[dimIdx];
        if (ticketLeft < 1 && m_battleField.m_bloodState != 1 && !user->isGMUser())
            return 0xad;
        if (m_battleField.m_bloodState != 1)
        {
            user->subDemensionInoutValue((char)dimIdx);
            G_GameWorld()->send_party_info_to_all(this, 1);
        }
    }

    // ---- 终极副本（m_bloodDungeonType==2） ----
    if (dungeon->m_bloodDungeonType == 2 && m_battleField.m_bloodState != 1)
        user->subUltimateInoutValue(2);

    // ---- 锦标赛入场包（0xe） ----
    if (dungeon->m_89c != 0 || dungeon->isTournamentDungeon())
    {
        if (m_battleField.m_bloodState == 1)
            return 0;
        if (dungeon->m_entryRequiredItem.m_itemIdx == -1)
            return 0;
        if (dungeon->m_entryRequiredItem.m_enableFlag != 1)
            return 0;

        int slot = 0;                                // ORIG -0x24（+0x642 movl $0x0）
        // [P337 改写] 旧判词「死块不可复现」被 P337 推翻：恒 0 **volatile** 守卫的
        // 读取不可省略 ⇒ 块体与 ORIG 9 个调用点强制发射（`if(0)`/goto 才会 DCE）。
        // ORIG +0x64f 为 e9 无条件 jmp 直达 +0x7c7，死块 +0x654..+0x7c2 无入口；
        // 运行期不可达（与本守卫等价），活动路径语义零变化。
        volatile bool neverEnter = false;
        if (neverEnter)
        {
            // ORIG +0x654 直接比较进入死块时的 eax（= dungeon 指针；32 位恒 ≠ 0x5dc）
            if ((int)reinterpret_cast<size_t>(dungeon) == 0x5dc)
            {
                slot = user->getCurCharacInvenR()->check_vilmark_dungeon_item_exist(
                    dungeon->m_entryRequiredItem.m_itemIdx);      // ORIG +0x660..+0x67b
                const bool deadMissing = (slot == -1);            // ORIG +0x683
                if (deadMissing)
                    return 0x11;
            }
            else
            {
                slot = user->getCurCharacInvenR()->check_item_exist(
                    dungeon->m_entryRequiredItem.m_itemIdx);      // ORIG +0x698..+0x6b3
                const bool deadMissing = (slot == -1);            // ORIG +0x6bb
                if (deadMissing)
                    return 0x11;
            }
            const Inven_Item deadItem =                           // ORIG +0x6d0..+0x6fc
                user->getCurCharacInvenR()->GetInvenSlot(1, slot);
            if (deadItem.m_addInfo2 < dungeon->m_entryRequiredItem.m_itemCount)
                return 0x11;                                      // ORIG +0x6ff..+0x717
            if (!user->getCurCharacInvenW()->delete_item(          // ORIG +0x71c..+0x760
                    (INVEN_TYPE)1, slot, dungeon->m_entryRequiredItem.m_itemCount,
                    (eItemDelReason)0xe, true))
            {
                cMyTrace trace(__PRETTY_FUNCTION__, 0xd66, 5);     // ORIG +0x762..+0x796
                trace("CParty::UseAncientDungeonItems, delete_item failed , User ch=%d , %d %d",
                      user->getCurCharacNo(), slot,
                      dungeon->m_entryRequiredItem.m_itemCount);   // ORIG +0x79b..+0x7b8
                return 0x11;
            }
        }
        PacketGuard packet;
        packet.put_header(0, 0xe);
        packet.put_byte(0);
        packet.put_short(1);
        user->getCurCharacInvenR()->MakeItemPacket((INVEN_TYPE)1, slot, packet);
        packet.finalize(true);
        user->Send(packet);
    }

    // ---- 村庄复仇 / 定时事件副本必需道具（ORIG 否定式早返 + 共享尾） ----
    if (dungeon->m_entryRestrictFlag == 0)
    {
        if (dungeon->m_villageAttackEventFlag != 0)
        {
            const unsigned char revengeCount = user->GetCurRevengeDungeonCount();
            if (revengeCount == 0 ||
                (unsigned char)dungeon->m_maxRevengeCount < revengeCount)
            {
                user->SetCurRevengeDungeonCount(0);
                return 0x11;
            }
            user->DecreaseRevengeDungeonTicket();
            user->VillageAttack_DBUpdate();
        }
        return 0;
    }
    if (!dungeon->IsOpenScheduleDungeon())
        return 0x15;
    if (user == 0)
        return 0;
    if (dungeon->m_entryRequiredItem2.m_itemIdx == -1 &&
        dungeon->m_entryRequiredItem.m_itemIdx == -1)
        return 0;
    if (dungeon->m_entryRequiredItem2.m_itemIdx != -1 &&
        dungeon->m_entryRequiredItem2.m_enableFlag != 1)
        return 0;
    if (dungeon->m_entryRequiredItem.m_itemIdx != -1 &&
        dungeon->m_entryRequiredItem.m_enableFlag != 1)
        return 0;

    // ORIG +0x93f..+0x9e3：req2 命中且 check 返回 0 时立即 SetInvitation 并
    // 提前返回；否则顺序进入 req1 检查（非 if/else 互斥）。
    int resultCode = 0x11;
    if (dungeon->m_entryRequiredItem2.m_itemIdx != -1)
    {
        resultCode = do_after_dungeon_start_checkndelete_item(
            dungeon->m_entryRequiredItem2, user);
        if (resultCode == 0)
        {
            user->SetDeleteEventDungeonInvitation(true);
            return resultCode;
        }
    }
    if (dungeon->m_entryRequiredItem.m_itemIdx != -1)
    {
        resultCode = do_after_dungeon_start_checkndelete_item(
            dungeon->m_entryRequiredItem, user);
        if (resultCode == 0)
            user->SetDeleteEventDungeonInvitation(true);
    }
    return resultCode;
}

// ===========================================================================
// CParty::OnDungeonClear —— T 0x085b5518
// 报告：OnDungeonClear.md。统计上报、个人结算回调、历史日志、战场清场、
// 血腥统计包、村庄怪物战败幽灵惩罚。
// ===========================================================================
int CParty::OnDungeonClear(CUser* user, bool success)
{
    // [b28d-solo-PartyClosureC2] ORIG 0x085b5518：
    //   P225 推广嵌套 if 复现 `call; test; setne; test; je` 物化；
    //   (signed char)get_dimension_possible() 复现 ORIG `test %al,%al; jle`
    //   （头声明 unsigned char，同 CParty_KillChain 先例）；
    //   P66 左操作数先物化（std-5 <= level）＋ P52 单语句链式血腥包发送；
    //   penalty 非 const 防 4.4 折叠 penalty+600（ORIG 走 -0x10 槽计算）。
    // ---- 结算角色信息广播 ----
    if (IsCollectDungeonStatistic() && m_battleField.m_bloodState != 1)
    {
        if (user != 0)
        {
            if (user->getCurCharacR() != 0)
            {
                if (user->m_field8e074 != 0 &&
                    m_battleField.m_pDungeon != 0)   // 旧 m_character.m_f40（+0x40）——布局修正后为 CUser 直属 +0x8e074
                {
                    SendDungeonCharacInfo(user, success);
                }
            }
        }
    }

    // ---- 经验统计（维度票充足 或 等级在标准±窗口内） ----
    if (m_battleField.m_bloodState == 1)
    {
        // skip
    }
    else if (m_battleField.m_pDungeon != 0 &&
             (signed char)m_battleField.m_pDungeon->get_dimension_possible() > 0)
    {
        reinterpret_cast<CStatisticMgrExpView*>(GlobalData::s_statisticMgr)
            ->m_dungeonExp.IncreaseDungeonExp(user->get_charac_level(),
                                              user->m_character.m_f28,
                                              user->m_character.m_f30);
    }
    else if (m_battleField.m_pDungeon != 0)
    {
        if (m_battleField.m_pDungeon->get_standard_level() - 5 <= user->get_charac_level() &&
            m_battleField.m_pDungeon->get_standard_level() + 3 >= user->get_charac_level())
        {
            reinterpret_cast<CStatisticMgrExpView*>(GlobalData::s_statisticMgr)
                ->m_dungeonExp.IncreaseDungeonExp(user->get_charac_level(),
                                                  user->m_character.m_f28,
                                                  user->m_character.m_f30);
        }
    }

    // ---- 个人结算回调与历史日志 ----
    user->onDungeonClear(success);
    user->setDungeonClearHackCheck(success);
    user->m_historyLog.DungeonClearInfo(success ? 1 : 0,
                                        GlobalData::s_systemTime_.getCurSec() -
                                            m_field185c);

    bf(this).onClearMap(false);

    // ---- 血腥副本统计包（0x1036/0x14） ----
    // ---- 血腥副本统计包（Packet_Blood_dungeon_statistic ctor W 0x085bee52） ----
    if (user != 0 && m_battleField.m_pDungeon != 0)
    {
        if (user->getCurCharacR() != 0 &&
            static_cast<signed char>(m_battleField.m_pDungeon->m_bloodDungeonType) > 0)
        {
            Packet_Blood_dungeon_statistic stat;
            stat.m_level = user->get_charac_level();    // +0x0e
            stat.m_success = success ? 1 : 0;           // +0x13
            stat.m_one = 1;                             // +0x12

            GlobalData::s_statistic_proxy_mgr->GetServerProxy(
                user->GetServerGroup())
                ->SendPacket(reinterpret_cast<char*>(&stat), 0x14);
        }
    }

    // ---- 村庄怪物战败：混乱/幽灵惩罚 ----
    if (bf(this).m_villageMonsterId != 0 && !success)
    {
        int penalty = -0x23a;                             // -570（非 const：ORIG 走槽计算）
        const int now = GlobalData::s_systemTime_.getCurSec();
        user->SetCurCharacChaosDieTime(now);
        user->SetCurCharacChaosResponTime(now + 600 + penalty);
        // ORIG 以静态派发调用（函数体不使用 this，mangled 与普通成员一致）
        pvp_assault::CAssaultMgr::NotifyGhostTime(user, penalty + 600,
                                                  penalty + 600);
        user->SetCurCharacGhost(true);
        user->m_characterView.enableSaveCharacView();
    }
    return 1;                       // ORIG 尾 mov $0x1,%eax
}
