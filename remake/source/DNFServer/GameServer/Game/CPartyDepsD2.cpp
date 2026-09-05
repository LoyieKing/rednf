// ============================================================================
// CPartyDepsD2.cpp —— CParty 第二层剩余方法独占恢复（2026-08-24，PartyDepsD2）
//
// 覆盖 26 个 ORIG 符号（除注明 objdump 转写者外，全部经
// docs/class_func_reports/CParty/ 对应报告 ## 汇编 + ## 反编译 C 完整读取后转写）：
//   CheckEnterDimensionDungeon(Rhc)   T 0x0859f36a（CheckEnterDimensionDungeon_0859f36a.md）
//   CheckHackAverageHitCount          T 0x085bb0ac（CheckHackAverageHitCount.md）
//   GetFirstValidMemberState          T 0x085ba574（GetFirstValidMemberState.md）
//   HasAllMemberSelection             T 0x085ba1c0（HasAllMemberSelection.md）
//   OnFailedDungeonClear              W 0x085bf7da（无报告：objdump 转写，转发 OnDungeonClear(user,false)；
//                                                已先读同类调用者 giveup_game/onBloodClear/
//                                                TournamentDungeonClear/dungeon_clear/ClearDungeon 报告）
//   OnSuccessedDungeonClear           W 0x085bf7fc（同上，转发 OnDungeonClear(user,true)）
//   OnStartMapFinishLoading           T 0x085b170a（OnStartMapFinishLoading.md）
//   OnUsedSkill                       T 0x085b5f8e（OnUsedSkill.md）
//   isCheckUseSkill                   T 0x085bb5b0（isCheckUseSkill.md）
//   SaveCoin                          T 0x085b3520（SaveCoin.md）
//   changeBattleDataOnMoveMap         T 0x085a8ee0（changeBattleDataOnMoveMap.md）
//   changePartyMemberInfo             T 0x085a8d1e（changePartyMemberInfo.md）
//   canFatigueUp                      T 0x085a8c88（canFatigueUp.md）
//   changePartyMemberInfoOnMoveMap    T 0x085a8e3c（changePartyMemberInfoOnMoveMap.md）
//   checkMoveMapHackCnt               T 0x085a83a2（checkMoveMapHackCnt.md）
//   check_allmember_loading           T 0x085b17a4（check_allmember_loading.md）
//   check_dungeon_start(CDungeon,Ri)  T 0x0859f7fa（check_dungeon_start.md）
//   getDungeonDropTestFlag            T 0x085bdc76（getDungeonDropTestFlag.md）
//   send_dungeon_fail_msg             T 0x085b35f4（send_dungeon_fail_msg.md）
//   send_to_party_cmd_error           T 0x0859d1aa（send_to_party_cmd_error.md）
//   setStandardDimensionLevel         T 0x0859f612（setStandardDimensionLevel.md）
//   sub_hellparty_value               T 0x085b690a（sub_hellparty_value.md）
//   send_hellParty_value              T 0x085b697c（send_hellParty_value.md；sub_hellparty_value
//                                                的被调方，原全工程无定义，一并闭环）
//   GetMapPlayingTime                 T 0x085b6768（GetMapPlayingTime.md）
//
// 待其他批次闭环的被调方（本 TU 仅按 CParty.h 既有声明调用）：
//   CParty::OnDungeonClear(CUser*,bool)   —— 报告 OnDungeonClear.md 已就绪
//   CParty::do_after_dungeon_start(...)   —— 报告 do_after_dungeon_start.md 已就绪
//
// 头文件类型注记（2026-09-03 已修正）：
//   CBattle_Field+0x1e8/+0x1ec/+0x1f4 = m_hellPartyRatio0/1/3 现为 int
//   （reset_hell_party_value 整数 mov + send/sub/cal/clear_reward fildl 实证）。
//   （原 +0x94 注记经 ORIG 反汇编复核为误标：check_allmember_loading 比较的是
//     CParty+0xcb8 = CBattle_Field+0x194（m_dungeonIndex），已改 cbfField94。）
// ============================================================================

#include <string.h>
#include "TowerOfDespairStatistic_Key.h"  // 权威 Key（CDungeon.cpp 定义 ctor）
#include <vector>

// ORIG T 0x08484a51（InitEnvHelpers.cpp 定义，无权威头声明）
bool IsCollectDungeonStatistic();

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
#define bd(p)         (*reinterpret_cast<BattleData*>(&(p)->m_padBattleData))
#define bf(p)         (*reinterpret_cast<CBattle_Field*>(&(p)->m_padBattleField1))
#define traceHack(p)  (*reinterpret_cast<CTraceMobDieHack*>(&(p)->m_pad210))
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

// ---------------------------------------------------------------------------
// CalcurateManageBonus —— ORIG T 0x085ada30
// 报告：docs/class_func_reports/CParty/CalcurateManageBonus.md
// 返回 st0 浮点；user 为空/角色为空/等级>=最大等级 → 0。
// type=0 → CharacManageScript::GetExpBonus(manageLevel)；
// type=1 → GetGoldBonus(manageLevel) * (double)(uint)param（fildll 无符号扩展）。
// ---------------------------------------------------------------------------
float CParty::CalcurateManageBonus(ManageBonusType::T type, CUser* user,
                                   unsigned int param)
{
    if (user == 0)
        return 0.0f;                                    // fldz
    if (user->getCurCharacR() == 0)
        return 0.0f;                                    // getCurCharacR==0

    // movzwl 0x27(%eax)：_Charac_info::m_level(+0x27, CUser.h)
    const short characLevel = user->getCurCharacR()->m_level;
    if (!(characLevel < user->GetUserMaxLevel()))
        return 0.0f;

    if (type == ManageBonusType::T_0)
    {
        const int manageLevel = user->GetManageLevel();
        CDataManager* dataMgr = G_CDataManager();
        // CDataManager+0x6394 = advancealtar::ConfigParameter+0x30
        //                     = m_altarConfig.m_characManageScript
        return dataMgr->m_altarConfig.m_characManageScript->GetExpBonus(manageLevel);
    }
    if (type == (ManageBonusType::T)1)
    {
        const int manageLevel = user->GetManageLevel();
        CDataManager* dataMgr = G_CDataManager();
        const float goldBonus =
            dataMgr->m_altarConfig.m_characManageScript->GetGoldBonus(manageLevel);
        return goldBonus * static_cast<float>(param);   // fmuls（fildll→fstps）
    }
    return 0.0f;
}

// ---------------------------------------------------------------------------
// CheckEnterDimensionDungeon(CDungeon const*, unsigned char&, bool) —— T 0x0859f1ce
// 报告：docs/class_func_reports/CParty/CheckEnterDimensionDungeon.md
// 错误码：0xab 人数不足且非 GM；0xac 等级不足（写出槽号）；0xad 维度入场券缺失
// （characR+0xeb8+possible 字节 <1 且 partyType!=1 且非 GM）；遍历完返回 1。
// ---------------------------------------------------------------------------
int CParty::CheckEnterDimensionDungeon(CDungeon const* dungeon, unsigned char& flag,
                                       bool checkMinParty)
{
    const int memberCount = get_member_count();
    const int dimPossible = dungeon->get_dimension_possible();

    for (int i = 0; i < 4; ++i)                          // cmpl $3/setle：i<=3
    {
        if (!_checkValidUser(i))
            continue;

        if (checkMinParty)
        {   // 人数下限：memberCount < min_partymem 且非 GM → 0xab
            if ((memberCount < dungeon->get_dimension_min_partymem()) &&
                !memberUser(this, i)->isGMUser())
                return 0xab;
        }

        // 等级下限 → 0xac（写出违规槽号）
        if (memberUser(this, i)->get_charac_level() < dungeon->get_min_level())
        {
            flag = (unsigned char)i;                     // mov %dl,(%eax)
            return 0xac;
        }

        // 维度入场券计数：*(char*)(characR + 0xeb8 + dimPossible) < 1
        // （单参重载 0x859f3fc 以 possible-1 + 0xeb9 寻址，同一字节）
        const unsigned char* characR =
            reinterpret_cast<const unsigned char*>(memberUser(this, i)->getCurCharacR());
        const signed char ticketCount =
            *reinterpret_cast<const signed char*>(
                reinterpret_cast<const char*>(characR) + 0xeb8 + dimPossible);
        if (ticketCount < 1 && m_partyType != 1 &&
            !memberUser(this, i)->isGMUser())
        {
            flag = (unsigned char)i;
            return 0xad;
        }
    }
    return 1;                                            // 全员通过
}

// ---------------------------------------------------------------------------
// CheckEnterDimensionDungeon(CDungeon const*, unsigned char&, char) —— T 0x0859f36a
// 报告：docs/class_func_reports/CParty/CheckEnterDimensionDungeon_0859f36a.md
// 以 (m_partyType, mode, -1) 构造 DimensionPartyCount（ctor W 0x085beabe）询问
// 可容纳入场；可容纳时要求队伍人数 > 计数字节，否则置 false 转发 bool 重载。
// ---------------------------------------------------------------------------
int CParty::CheckEnterDimensionDungeon(CDungeon const* dungeon, unsigned char& flag,
                                     char mode)
{
    bool allowed = true;                                 // movb $1,-0xd

    // ORIG 以 3 参构造 DimensionPartyCount((char)m_partyType, mode, -1)
    // （ctor W 0x085beabe），非默认构造+赋值
    DimensionPartyCount count((char)m_partyType, mode, (char)-1);

    if (dungeon->get_dimension_member_count(count))      // T 0x0834d220
    {   // (signed char)count.m_2 <= memberCount → 不可入场
        const int members = get_member_count();
        if (!((int)count.m_2 > members))
            allowed = false;
    }
    return CheckEnterDimensionDungeon(dungeon, flag, allowed);   // 尾调 bool 重载
}

// ---------------------------------------------------------------------------
// CheckHackAverageHitCount —— ORIG T 0x085bb0ac
// 报告：docs/class_func_reports/CParty/CheckHackAverageHitCount.md
// 常量实证（objdump rodata）：0x08cbbbfc=10.0f（乘）、0x08cbbc00=0.1f（加）。
// 三组命中样本带外统计过半 → 全体有效成员 addServerHackCnt 0xd9/0xda/0xdb。
// ---------------------------------------------------------------------------
void CParty::CheckHackAverageHitCount()
{
    if (GlobalData::s_villageMonsterMgr->OnEvent())      // 村庄袭击中跳过（0x82f0f44）
        return;
    if (!(m_mapHitCount < 0x2e))                         // >=46 直接返回
        return;

    // 血之副本 / 维度副本 / 样本<=1 → 返回（asm +0x67..+0x83 组合条件）
    bool skip;
    if (m_dungeon == 0)
        skip = false;
    else if (m_dungeon->m_89c == 0 && m_dungeon->get_dimension_possible() < 1)
        skip = false;
    else
        skip = true;
    if (!skip && !(1 < m_mapHitCount))
        skip = true;
    if (skip)
        return;

    // 三组平均值（asm fildl/fdivrp：整型转 float 相除）
    float avg1 = static_cast<float>(m_mapHitSum) / static_cast<float>(m_mapHitCount);
    float avg2 = static_cast<float>(m_mapHitSum2) / static_cast<float>(m_mapHitCount);
    float avg3 = static_cast<float>(m_mapHitSum3) / static_cast<float>(m_mapHitCount);

    int band1Upper = D2_ROUND(avg1 + avg1 * 10.0f + 0.1f);   // fistpl -0x54
    int band1Lower = D2_ROUND(avg1 - avg1 * 10.0f);          // fistpl -0x50
    int band2Upper = D2_ROUND(avg2 + avg2 * 10.0f + 0.1f);
    int band2Lower = D2_ROUND(avg2 - avg2 * 10.0f);
    int band3Upper = D2_ROUND(avg3 + avg3 * 10.0f + 0.1f);
    int band3Lower = D2_ROUND(avg3 - avg3 * 10.0f);

    int inBand1 = 0;                                     // -0x2c（list1 带内）
    int inBand2 = 0;                                     // -0x40（list2 带内）
    int zero2 = 0;                                       // -0x28（list2 为 0）
    int inBand3 = 0;                                     // -0x30（list3 带内）
    int zero3 = 0;                                       // -0x24（list3 为 0）

    for (int i = 0; i < m_mapHitCount; ++i)              // cmp [0x1874] setg
    {
        if (m_mapHitList1[i] <= band1Upper && band1Lower <= m_mapHitList1[i])
            ++inBand1;

        if (m_mapHitList2[i] == 0)
            ++zero2;
        else if (m_mapHitList2[i] <= band2Upper && band2Lower <= m_mapHitList2[i])
            ++inBand2;

        if (m_mapHitList3[i] == 0)
            ++zero3;
        else if (m_mapHitList3[i] <= band3Upper && band3Lower <= m_mapHitList3[i])
            ++inBand3;
    }

    // 第一组：带内数 >= 样本一半 → 0xd9
    if (!(m_mapHitCount / 2 > inBand1))
    {
        for (int i = 0; i < 4; ++i)
            if (_checkValidUser(i))
                memberUser(this, i)->getHackAnalyzer()->addServerHackCnt(
                    memberUser(this, i), (WongWork::ENUM_HACKTYPE)0xd9, 1, 0, 0);
    }

    // 第二组：非零样本中带内过半 → 0xda
    const int nonZero2 = m_mapHitCount - zero2;
    if (nonZero2 > 0 && !(nonZero2 / 2 > inBand2))
    {
        for (int i = 0; i < 4; ++i)
            if (_checkValidUser(i))
                memberUser(this, i)->getHackAnalyzer()->addServerHackCnt(
                    memberUser(this, i), (WongWork::ENUM_HACKTYPE)0xda, 1, 0, 0);
    }

    // 第三组：非零样本中带内过半 → 0xdb
    const int nonZero3 = m_mapHitCount - zero3;
    if (nonZero3 > 0 && !(nonZero3 / 2 > inBand3))
    {
        for (int i = 0; i < 4; ++i)
            if (_checkValidUser(i))
                memberUser(this, i)->getHackAnalyzer()->addServerHackCnt(
                    memberUser(this, i), (WongWork::ENUM_HACKTYPE)0xdb, 1, 0, 0);
    }
}

// ---------------------------------------------------------------------------
// GetFirstValidMemberState —— ORIG T 0x085ba574
// 报告：docs/class_func_reports/CParty/GetFirstValidMemberState.md
// 第一个有效成员的 CUser::get_state()；无有效成员返回 0。
// ---------------------------------------------------------------------------
char CParty::GetFirstValidMemberState()
{
    for (int i = 0; i < 4; ++i)
    {
        if (_checkValidUser(i))
            return (char)memberUser(this, i)->get_state();   // thunk 0x080da38c
    }
    return 0;
}

// ---------------------------------------------------------------------------
// HasAllMemberSelection —— ORIG T 0x085ba1c0
// 报告：docs/class_func_reports/CParty/HasAllMemberSelection.md
// 所有有效成员均完成物品路由选择（GetMemberRoutingState!=0）→ true。
// ---------------------------------------------------------------------------
bool CParty::HasAllMemberSelection()
{
    for (int i = 0; i < 4; ++i)
    {
        if (_checkValidUser(i) &&
            m_routingData.GetMemberRoutingState(i) == 0)     // thunk 0x085bf694
            return false;
    }
    return true;
}

// ---------------------------------------------------------------------------
// OnFailedDungeonClear —— ORIG W 0x085bf7da
// 无独立报告（objdump 转写，已读同类调用者 giveup_game/onBloodClear/
// TournamentDungeonClear/dungeon_clear/ClearDungeon 报告确认消费形态）：
// 3 条指令 thunk → OnDungeonClear(user, false)。
// ---------------------------------------------------------------------------
void CParty::OnFailedDungeonClear(CUser* user)
{
    OnDungeonClear(user, false);                         // movl $0x0,0x8(%esp)
}

// ---------------------------------------------------------------------------
// OnSuccessedDungeonClear —— ORIG W 0x085bf7fc
// 无独立报告（objdump 转写，依据同上）：thunk → OnDungeonClear(user, true)。
// ---------------------------------------------------------------------------
void CParty::OnSuccessedDungeonClear(CUser* user)
{
    OnDungeonClear(user, true);                          // movl $0x1,0x8(%esp)
}

// ---------------------------------------------------------------------------
// OnStartMapFinishLoading —— ORIG T 0x085b170a
// 报告：docs/class_func_reports/CParty/OnStartMapFinishLoading.md
// 有副本且 do_after_dungeon_start 成功处理 → giveup_game 并返回 false；
// 否则回发 0x28 cmd ok 包并返回 true。
// ---------------------------------------------------------------------------
bool CParty::OnStartMapFinishLoading(CUser* user)
{
    if (bf(this).GetDungeon() != 0)                      // thunk 0x080fdcfc ×2
    {
        // ORIG 实证双取形（无 dungeon 局部缓存），user 直传实参无 spill。
        if (do_after_dungeon_start(bf(this).GetDungeon(), user) != 0)
        {   // 入场善后失败：按放弃处理并返回 0
            giveup_game(user, false, false, false);      // B2 批次实现
            return false;
        }
    }
    user->SendCmdOkPacket((ENUM_CMDPACKET)0x28);         // T 0x0867bea0
    return true;
}

// ---------------------------------------------------------------------------
// OnUsedSkill —— ORIG T 0x085b5f8e
// 报告：docs/class_func_reports/CParty/OnUsedSkill.md
// 匹配成员槽累计 BattleData.m_arr7d0[slot]（this+(i+0x2bc)*4+0xc）技能使用
// 次数；isCheckUseSkill(job, skillIdx) 通过再累加 m_field1aa4[slot]。
// ---------------------------------------------------------------------------
bool CParty::OnUsedSkill(CUser* user, int skillIdx, int param)   // ORIG 0x085b5f8e 单出口 mov $0x1,%eax
{
    IsCollectDungeonStatistic();                         // 结果未消费（ORIG 形态）

    for (int i = 0; i < 4; ++i)
    {
        if (m_member[i].m_pUser == user)
        {
            ++bd(this).m_arr7d0[i];                      // this+(i+0x2bc)*4+0xc

            const int job = user->get_charac_job();      // thunk 0x080fdf20
            if (isCheckUseSkill(job, skillIdx))          // T 0x085bb5b0
                ++m_field1aa4[i];                        // this+(i+0x6a8)*4+4
        }
    }
    return true;
}

// ---------------------------------------------------------------------------
// isCheckUseSkill —— ORIG T 0x085bb5b0
// 报告：docs/class_func_reports/CParty/isCheckUseSkill.md
// 职业×技能黑名单：(0,0x2e)(1,5)(2,4)(3,0xc)(4,1)(5,4)(6,1)(7,5) 及
// skillIdx>0xa8 一律 false。
// ---------------------------------------------------------------------------
bool CParty::isCheckUseSkill(int job, int skillIdx)
{
    if (skillIdx > 0xa8)                    return false;
    if (job == 0 && skillIdx == 0x2e)       return false;
    if (job == 1 && skillIdx == 0x5)        return false;
    if (job == 2 && skillIdx == 0x4)        return false;
    if (job == 3 && skillIdx == 0xc)        return false;
    if (job == 4 && skillIdx == 0x1)        return false;
    if (job == 5 && skillIdx == 0x4)        return false;
    if (job == 6 && skillIdx == 0x1)        return false;
    if (job == 7 && skillIdx == 0x5)        return false;
    return true;
}

// ---------------------------------------------------------------------------
// SaveCoin —— ORIG T 0x085b3520
// 报告：docs/class_func_reports/CParty/SaveCoin.md
// eCoinSubReason 参数未消费（ORIG 形态）；全员 SaveMoneyCoin。
// ---------------------------------------------------------------------------
// [N-类] 剩 4：ORIG 4.1 `!_checkValidUser(i)` 产 xor$1;test;je 物化岛 +
// ++i 在 continue 路径中部；4.4.7 恒折 test/jne + ++i 归条件块（setcc-fold/
// 块布局跨版本，同 GetFirstValidMemberState 0x085ba574 剩 4 行）。
void CParty::SaveCoin(eCoinSubReason reason)
{
    (void)reason;
    for (int i = 0; i < 4; ++i)
        if (_checkValidUser(i))
            memberUser(this, i)->SaveMoneyCoin();        // T 0x086504d8
}

// ---------------------------------------------------------------------------
// changeBattleDataOnMoveMap —— ORIG T 0x085a8ee0
// 报告：docs/class_func_reports/CParty/changeBattleDataOnMoveMap.md
// （BD+0x18/0x1c ↔ CParty+0x344/0x348）；非 firstVisit 时累加 BD.m_field68c
// （CParty+0x9b8）。两次独立取时（ORIG 形态）。
// ---------------------------------------------------------------------------
void CParty::changeBattleDataOnMoveMap(bool firstVisit)
{
    BattleData& d = bd(this);

    unsigned int now = GlobalData::s_systemTime_.getCurTickCount();
    d.m_field34 = now;                                   // +0x360 低
    d.m_field38 = 0;                                     // +0x364 高

    now = GlobalData::s_systemTime_.getCurTickCount();   // 第二次取时
    d.m_field18 = now;                                   // +0x344 低
    d.m_field1c = 0;                                     // +0x348 高

    if (!firstVisit)                                     // movzbl xor $1 test je
        ++d.m_field68c;                                  // +0x9b8
}

// ---------------------------------------------------------------------------
// canFatigueUp —— ORIG T 0x085a8c88
// 报告：docs/class_func_reports/CParty/canFatigueUp.md
// partyType==1 或无副本 → false；疲劳关闭(m_bloodDungeonType!=0)/血图(m_villageAttackEventFlag==1)/免疲劳副本
// → false；否则 true。flag 参数未消费（ORIG 形态）。
// ---------------------------------------------------------------------------
bool CParty::canFatigueUp(unsigned char flag)
{
    (void)flag;
    if (m_partyType == 1)
        return false;
    if (m_dungeon == 0)
        return false;
    if (m_dungeon->m_bloodDungeonType == 0 && !m_dungeon->m_villageAttackEventFlag &&
        !m_dungeon->isNoFatigueDungeon())
        return true;
    return false;
}

// ---------------------------------------------------------------------------
// changePartyMemberInfo —— ORIG T 0x085a8d1e
// 报告：docs/class_func_reports/CParty/changePartyMemberInfo.md
// 存活且未进图者：enterMapFlag(+0x384)/装载标志(+0x388) 任一置位则清
// enterMapFlag，否则累加 BattleData.m_arr64[slot]（this+(slot+0xe0)*4+0x10）；
// 清装载标志后按 msg.m_field17 判定 FatigueUp(0)。
// ---------------------------------------------------------------------------
void CParty::changePartyMemberInfo(MSG_MOVE_MAP& info, char slot)
{
    const int idx = (int)slot;
    if (!_checkValidUser(idx))
        return;

    if (m_liveFlag[idx] != 1)                            // +0x380+slot
    {
        if (m_enterMapFlag[idx] == 1 ||                  // +0x384+slot
            m_field388[idx] == 1)                        // +0x388+slot
        {
            m_enterMapFlag[idx] = 0;
        }
        else
        {
            ++bd(this).m_arr64[idx];                     // this+(slot+0xe0)*4+0x10
        }
    }
    m_field388[idx] = 0;

    if (canFatigueUp(info.m_field17))                    // movzbl 0x17(%eax)
        memberUser(this, idx)->FatigueUp(0);             // T 0x08655c60
}

// ---------------------------------------------------------------------------
// changePartyMemberInfoOnMoveMap —— ORIG T 0x085a8e3c
// 报告：docs/class_func_reports/CParty/changePartyMemberInfoOnMoveMap.md
// 非 firstVisit 时：逐张进入型副本仅刷新座位对应成员，否则逐成员刷新。
// ---------------------------------------------------------------------------
void CParty::changePartyMemberInfoOnMoveMap(MSG_MOVE_MAP& info, bool firstVisit)
{
    if (firstVisit)                                      // cmpb $0 jne 结束
        return;

    bool eachMap;
    if (m_dungeon == 0)
        eachMap = false;
    else
        eachMap = m_dungeon->IsEnterEachMap();           // T 0x08365278

    if (eachMap)
    {
        changePartyMemberInfo(info, (char)info.m_39);    // 座位号字节 +0x39
    }
    else
    {
        for (int i = 0; i < 4; ++i)                      // cmpl $3/setle
            changePartyMemberInfo(info, (char)i);
    }
}

// ---------------------------------------------------------------------------
// checkMoveMapHackCnt —— ORIG T 0x085a83a2
// 报告：docs/class_func_reports/CParty/checkMoveMapHackCnt.md
// 反汇编分支边界（+0x57d..+0x5fc）语义：
//   c==0 → 直接到任务豁免段；sec%5==0 → c>sec/5 记疑；sec%5!=0 → c>=sec/5 记疑。
// ---------------------------------------------------------------------------
bool CParty::checkMoveMapHackCnt(MSG_MOVE_MAP& info)
{
    const int prevAux = m_field1ab8;                     // -0x38（上一辅助计数）
    m_field1ab8 = 0;
    bd(this).m_field4c = 0;                              // +0x378 dword 清零

    // 地图游玩时长：now - enterTick(+0x344) - baseTick(+0x33c) → +0x334
    unsigned long long nowTick = GlobalData::s_systemTime_.getCurTickCount();
    unsigned long long enterTick =
        (unsigned long long)bd(this).m_field1c << 32 | bd(this).m_field18;
    unsigned long long baseTick =
        (unsigned long long)bd(this).m_field14 << 32 | bd(this).m_field10;
    unsigned long long playTime64 = (nowTick - enterTick) - baseTick;
    unsigned int playTime = (unsigned int)playTime64;    // +0x334（消费均为低 32 位）

    if (bf(this).IsVisitedCurrentMap())                  // W 0x085bf2e8
        return true;                                     // 已访问过 → 其余全部跳过

    int sumKill = 0;                                     // -0x34

    if (prevAux != 0)
    {   // 上一张图的第三/二组均值并入历史（idx=当前 m_mapHitCount）
        const int idx = m_mapHitCount;
        m_mapHitList3[idx] = m_mapHitList3[idx] / prevAux;
        m_mapHitSum3 += m_mapHitList3[idx];
        m_mapHitList2[idx] = m_mapHitList2[idx] / prevAux;
        m_mapHitSum2 += m_mapHitList2[idx];
    }

    for (int i = 0; i < 4; ++i)
    {
        const unsigned short kill = info.m_1b[i];        // +0x1b+2*i ushort
        if (kill == 0xffff)
            continue;
        if (!checkValidUser(i))                          // W 0x08145868
            continue;

        // 逐张进入型副本只统计座位匹配成员
        if (m_dungeon != 0 && m_dungeon->IsEnterEachMap() &&
            (int)info.m_39 != i)
            continue;

        MapInfo* lastMap = bf(this).GetLastMapInfo();    // T 0x0830eac8
        // cUserHistoryLog::MapClear(ushort,int,int) T 0x08684b22
        // 首参 = user+0x79700 日志门面；第 3 参 = *(int*)(lastMap+4)=m_mapIndex
        memberUser(this, i)->m_historyLog.MapClear(kill, (int)(playTime / 1000), lastMap->m_mapIndex);

        if (kill == 0)
        {   // 击杀数为 0 却换图 → 0x19b
            memberUser(this, i)->getHackAnalyzer()->addServerHackCnt(
                memberUser(this, i), (WongWork::ENUM_HACKTYPE)0x19b, 1, 0, 0);
        }

        // 时长/击杀比异常：quotient=kill/sec，>4 时上报 min(q-4,100) → 0x340
        const unsigned int sec = playTime / 1000;
        if (sec != 0)
        {
            const unsigned int quotient = (unsigned int)kill / sec;
            if (4 < quotient)
            {
                int over = (int)quotient - 4;            // -0x3c
                const int cap = 100;                     // -0x40
                if (over > cap)
                    over = cap;                          // std::min<unsigned>
                memberUser(this, i)->getHackAnalyzer()->addServerHackCnt(
                    memberUser(this, i), (WongWork::ENUM_HACKTYPE)0x340,
                    (unsigned int)over, 0, 0);
            }
        }

        sumKill += kill;

        // 维度刷图频率检测前置条件（asm +0x3be..+0x40f 组合）
        bool dimTrack = false;
        if (m_dungeon != 0 && m_dungeon->m_89c != 1 &&
            m_dungeon->get_dimension_possible() == 0 &&
            prevAux != 0 && m_field1aa0 != 1)
        {
            dimTrack = true;
        }
        if (dimTrack)
        {   // user+0x8eb48 计数超 9 归零并上报 0xdc
            ++memberUser(this, i)->m_field8eb48;
            if (9 < memberUser(this, i)->m_field8eb48)
            {
                memberUser(this, i)->m_field8eb48 = 0;
                memberUser(this, i)->getHackAnalyzer()->addServerHackCnt(
                    memberUser(this, i), (WongWork::ENUM_HACKTYPE)0xdc, 1, 0, 0);
            }
        }
        m_field1aa0 = 0;

        // 技能节奏记疑判定（边界语义见函数头注）：
        //   level<20 或未存活 或 c==0 → 不记疑；
        //   否则 q=playTime/5：rem==0 时 c>q 记疑；rem!=0 时 c>=q 记疑。
        bool suspectBoundary = false;
        {
            const int level = memberUser(this, i)->get_charac_level();
            const int c = m_field1aa4[i];
            if (level >= 0x14 && m_liveFlag[i] != 0 && c != 0)
            {
                const unsigned int q = playTime / 5;     // mul 0xcccccccd shr 2
                const unsigned int rem = playTime - q * 5;   // sec%5
                if (rem == 0)
                    suspectBoundary = ((unsigned int)c > q);
                else
                    suspectBoundary = ((unsigned int)c >= q);
            }
        }

        if (!suspectBoundary)
        {
            // 任务豁免：(0x10d3,idx 0xb,diff 2)/(0x10d4,0xc,2)/(0x10d8,0xf,diff>0)
            UserQuest* questR = memberUser(this, i)->getCurCharacQuestR();
            bool exempt = false;
            if (questR->IsDoingQuest(0x10d3) &&
                bf(this).get_dungeon_index() == 0xb &&
                bf(this).get_dungeon_diff() == 2)
                exempt = true;
            else if (questR->IsDoingQuest(0x10d4) &&
                     bf(this).get_dungeon_index() == 0xc &&
                     bf(this).get_dungeon_diff() == 2)
                exempt = true;
            else if (questR->IsDoingQuest(0x10d8) &&
                     bf(this).get_dungeon_index() == 0xf &&
                     bf(this).get_dungeon_diff() > 0)
                exempt = true;

            if (!exempt)
            {   // SetETC(0x84, c) 结果作为 addServerHackCnt 第 4 参 → 0x84
                const int etc =
                    memberUser(this, i)->SetETC(0x84, m_field1aa4[i]);
                memberUser(this, i)->getHackAnalyzer()->addServerHackCnt(
                    memberUser(this, i), (WongWork::ENUM_HACKTYPE)0x84, 1,
                    (unsigned int)etc, 0);
            }
        }
        m_field1aa4[i] = 0;

        // 单人模式换图检测：msg.dword@+0x35==0 且 IsSinglePlay → 0x1a0
        unsigned int dword35;
        memcpy(&dword35, reinterpret_cast<const char*>(&info) + 0x35, 4);
        if (dword35 == 0 && IsSinglePlay())
        {
            memberUser(this, i)->getHackAnalyzer()->addServerHackCnt(
                memberUser(this, i), (WongWork::ENUM_HACKTYPE)0x1a0, 1, 0, 0);
        }
    }

    // 本图第一组样本落盘并推进游标
    m_mapHitList1[m_mapHitCount] = sumKill;              // this+(n+0x61c)*4+0xc
    m_mapHitSum += sumKill;
    if (m_mapHitCount <= 0x2b)                           // jg 跳过递增
        m_mapHitCount = m_mapHitCount + 1;

    return true;
}

// ---------------------------------------------------------------------------
// check_allmember_loading —— ORIG T 0x085b17a4
// 报告：docs/class_func_reports/CParty/check_allmember_loading.md
// ---------------------------------------------------------------------------
void CParty::check_allmember_loading(bool flag)
{
    const int memberCount = get_member_count();
    if (memberCount == 0)
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x30e0, 0);
        trace("<IN_DUNGEON_NULL_PARTY> not_member PartyIndex(%d)", m_partyIndex);
        destroy();                                       // T 0x0859ad4c
        LogManager::logFormat(
            1, "party.cpp", __PRETTY_FUNCTION__, 0x30e3,
            "CParty::check_allmember_loading : 1 Using: %d, Member = 0\n",
            (unsigned int)(unsigned char)m_field68);
        return;
    }

    int loadedCount = 0;                                 // -0x2c
    for (int i = 0; i < 4; ++i)
    {
        if (_checkValidUser(i) && m_finishLoadingFlag[i])   // +0x37c+i byte
            ++loadedCount;
    }
    if (loadedCount != memberCount)
    {
        if (!flag)
            return;
        if (loadedCount < 1)
            return;
    }

    bf(this).ResetFirstEnterDungeon();                   // T 0x08307662

    {   // 广播 0x1e 包：put_int(BF+0x134)
        PacketGuard packet;
        packet.put_header(0, 0x1e);
        packet.put_int(bf(this).m_field134);
        packet.finalize(true);
        send_to_party(packet);
    }

    // 副本游玩时间累计：acc(+0x358/0x35c) += now - moveTick(+0x360/0x364)。
    // 按 ORIG 以 32 位低高半 + 进位链（报告 asm +0x19b..+0x1ee），而非 64 位临时
    // （-O0 会注入符号扩展/栈溢出链，且字段为 int，符号扩展与 ORIG 无符号语义不符）。
    {
        const unsigned int accLow  = (unsigned int)bd(this).m_field2c;   // +0x358
        const unsigned int accHigh = (unsigned int)bd(this).m_field30;   // +0x35c
        const unsigned int mtLow   = (unsigned int)bd(this).m_field34;   // +0x360
        const unsigned int mtHigh  = (unsigned int)bd(this).m_field38;   // +0x364
        const unsigned int now = GlobalData::s_systemTime_.getCurTickCount();  // 32-bit
        const unsigned int dLow  = now - mtLow;
        const unsigned int dHigh = 0u - mtHigh - (now < mtLow ? 1u : 0u);
        const unsigned int newLow  = dLow + accLow;
        const unsigned int newHigh = dHigh + accHigh + (newLow < dLow ? 1u : 0u);
        bd(this).m_field2c = (int)newLow;
        bd(this).m_field30 = (int)newHigh;
    }
    // 地图基准 tick：base(+0x33c/0x340) = now - enterTick(+0x344/0x348)。
    // 同上按 ORIG 32 位式（asm +0x1f4..+0x221）。
    {
        const unsigned int now = GlobalData::s_systemTime_.getCurTickCount();  // 32-bit
        const unsigned int etLow  = (unsigned int)bd(this).m_field18;   // +0x344
        const unsigned int etHigh = (unsigned int)bd(this).m_field1c;   // +0x348
        const unsigned int plLow  = now - etLow;
        const unsigned int plHigh = 0u - etHigh - (now < etLow ? 1u : 0u);
        bd(this).m_field10 = (int)plLow;
        bd(this).m_field14 = (int)plHigh;
    }

    // PCRoom 统计上下文：非自动建队且人数>1 时开启追踪
    const bool pcRoomTrace = !(IsAutoCreated() || get_member_count() == 1);
    traceHack(this).traceBegin(GlobalData::s_systemTime_.getCurSec(), pcRoomTrace);

    for (int i = 0; i < 4; ++i)
    {
        if (!_checkValidUser(i))
            continue;
        CUser* user = m_member[i].m_pUser;
        if (user->get_state() < 3 || user->getCurCharacR() == 0)   // thunk 0x080da38c
        {
            char* idStr = NumberToString(user->get_acc_id(), 0);   // T 0x0810904b
            cMyTrace trace(__PRETTY_FUNCTION__, 0x3123, 5);
            trace("PCROOM : Dispatcher_RequestPCRoomPlayerCount => User State : %d, "
                  "m_id : %s",
                  user->get_state(), idStr);
        }
        user->GetPCRoomNo();                             // 结果未消费（ORIG 形态）
    }

    // 开启反作弊采样
    for (int i = 0; i < 4; ++i)
    {
        if (_checkValidUser(i))
            memberUser(this, i)->getHackAnalyzer()->beginCollectHackInfo(
                m_member[i].m_pUser);
    }

    m_pad119 = 1;                                        // +0x119 装载检查标志
    bf(this).onStartMap();                               // T 0x0830dc74

    // 电梯副本收尾（BF+0x194==0x35 即 m_dungeonIndex==53 且 BF+0x13a 标志；
    // ORIG mov 0xcb8(%eax),%eax; cmp $0x35 / movzbl 0xc5e）
    if (bf(this).m_dungeonIndex == 0x35 && bf(this).m_elevatorFlag != 0)
    {
        bf(this).m_elevatorTimer = 0;                    // +0x139
        bf(this).m_elevatorState = 2;                    // +0x138
        bf(this).m_elevatorFlag = 0;                     // +0x13a
        const int key = gen_timer_key((TIMER_MESSAGE)0x28);
        G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)1, m_partyIndex,
                                    (TIMER_MESSAGE)0x28, 0xf, key, 0);
    }

    // 血票剩余时间提醒（BF+0x128=m_villageMonsterId 槽按 short 判非 0）
    if (bf(this).m_villageMonsterId != 0)
    {
        const int key = gen_timer_key((TIMER_MESSAGE)0x27);
        G_TimerQueue()->InsertTimer(
            (TimerEntry::OBJ_TYPE)1, m_partyIndex, (TIMER_MESSAGE)0x27,
            bf(this).m_field134, key,
            (unsigned long long)(unsigned short)bf(this).m_villageMonsterId);
    }

    // 任务 NPC 副本且有 boss 房：EPLP 状态机切换 + 0xfe 包
    if (bf(this).GetDungeon() != 0)
    {
        CDungeon* dungeon = bf(this).GetDungeon();
        if (dungeon->getQuestNpcDungeon() && checkBossRoom())
        {
            SetEPLPState(2);
            const int key = gen_timer_key((TIMER_MESSAGE)0x15);
            G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)1, GetPartyIndex(),
                                        (TIMER_MESSAGE)0x15, 600, key, 0);
            set_state(1);
            PacketGuard packet;
            packet.put_header(0, 0xfe);
            packet.finalize(true);
            send_to_party(packet);
        }
    }
}

// ---------------------------------------------------------------------------
// check_dungeon_start(CDungeon const*, int&) —— ORIG T 0x0859f7fa
// 报告：docs/class_func_reports/CParty/check_dungeon_start.md
// 返回错误码并写出违规成员槽：血图 10/0x5f/0xf5；m_bloodDungeonType==2 图 0xf4~0xf8；
// 其余 0xf3（人数超限）；通过返回 0。
// ---------------------------------------------------------------------------
int CParty::check_dungeon_start(CDungeon const* dungeon, int& param)
{
    if (dungeon->m_bloodDungeonType == 1)
    {   // 血之副本
        for (int i = 0; i < 4; ++i)
        {
            if (!_checkValidUser(i))
                continue;
            CUser* user = m_member[i].m_pUser;

            // 入场费 vs 现金（thunk 0x080da27e=getCurCharacInvenR）
            const int fee = get_blood_dungeon_admission_fee(user);
            if (user->getCurCharacInvenR()->get_money() < fee)
            {
                param = i;
                return 10;
            }
            // 等级下限
            if (user->get_charac_level() < dungeon->get_min_level())
            {
                param = i;
                return 0x5f;
            }
            // 进入次数上限（CCharacBloodDungeon::GetEnterCount，expand 类型 1）
            const int limit = dungeon->get_limit_Inout_count();
            BloodDungeonCountsViewD2* blood =
                reinterpret_cast<BloodDungeonCountsViewD2*>(
                    user->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)1));
            if (limit <= blood->m_enterCount)
                return 0xf5;
        }
        return 0;                                        // asm jmp 0x859fb0a
    }

    if (dungeon->m_bloodDungeonType == 2)
    {   // 特殊权限图（PowerWar/VillageAttack 等）
        (void)dungeon->get_index();                      // 结果未消费（ORIG 形态）
        if (!checkDugeonInoutTime(0))                    // T 0x0859f6e2
            return 0xf4;
        if (GlobalData::s_power_manager->IsPowerWarEventOn())
            return 0xf6;
        if (GlobalData::s_villageMonsterMgr->OnEvent())
            return 0xf7;

        const int members = get_member_count();
        if (dungeon->get_limit_party_count() < members)
            return 0xf3;

        for (int i = 0; i < 4; ++i)
        {
            if (!_checkValidUser(i))
                continue;
            CUser* user = m_member[i].m_pUser;
            if (user->get_charac_level() < dungeon->get_min_level())
            {
                param = i;
                return 0xf8;
            }
            // characR+0xec2 入场券计数
            const unsigned char* characR =
                reinterpret_cast<const unsigned char*>(user->getCurCharacR());
            const signed char ticket =
                *reinterpret_cast<const signed char*>(
                    reinterpret_cast<const char*>(characR) + 0xec2);
            if (ticket < 1 && m_partyType != 1)
            {
                param = i;
                return 0xf5;
            }
        }
        return 0;
    }

    // 普通副本：仅人数上限
    if (dungeon->get_limit_party_count() < get_member_count())
        return 0xf3;
    return 0;
}

// ---------------------------------------------------------------------------
// getDungeonDropTestFlag —— ORIG T 0x085bdc76
// 报告：docs/class_func_reports/CParty/getDungeonDropTestFlag.md
// 任一 GM 成员在存图模式 → true。
// ---------------------------------------------------------------------------
bool CParty::getDungeonDropTestFlag()
{
    for (int i = 0; i < 4; ++i)
    {
        CUser* user = get_user(i);                       // thunk 0x08145764
        if (user == 0)
            continue;
        if (user->isGMUser() && getDungeonMapSaving())
            return true;
    }
    return false;
}

// ---------------------------------------------------------------------------
// send_dungeon_fail_msg —— ORIG T 0x085b35f4
// 报告：docs/class_func_reports/CParty/send_dungeon_fail_msg.md
// 插入 TIMER_MESSAGE 0xe 定时器：+0xc4c（BF+0x128 槽）short 非 0 → 3 秒，
// 否则 10 秒。ORIG 直读 this+0xc4c（无 bf() 调用）。
// ---------------------------------------------------------------------------
void CParty::send_dungeon_fail_msg()
{
    int seconds = 10;
    if (*reinterpret_cast<unsigned short*>(
            reinterpret_cast<char*>(this) + 0xc4c) != 0)
        seconds = 3;

    const int key = gen_timer_key((TIMER_MESSAGE)0xe);
    G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)1, m_partyIndex,
                                (TIMER_MESSAGE)0xe, seconds, key, 0);
}

// ---------------------------------------------------------------------------
// send_to_party_cmd_error —— ORIG T 0x0859d1aa
// 报告：docs/class_func_reports/CParty/send_to_party_cmd_error.md
// put_header(1, cmd) + byte 0 + byte error + finalize(true) 后全队发送。
// ---------------------------------------------------------------------------
void CParty::send_to_party_cmd_error(ENUM_CMDPACKET cmd, unsigned char error)
{
    PacketGuard packet;
    packet.put_header(1, cmd);
    packet.put_byte(0);
    packet.put_byte(error);
    packet.finalize(true);
    send_to_party(packet);
}

// ---------------------------------------------------------------------------
// setStandardDimensionLevel —— ORIG T 0x0859f612
// 报告：docs/class_func_reports/CParty/setStandardDimensionLevel.md
// 仅 GM 有效成员参与平均：ORIG 中 isGMUser() 结果为 0 时 je 跳到循环尾
// （不累加等级/计数）；成员数为 0 时 ORIG idivl 除零同样崩溃，忠实保留。
// ---------------------------------------------------------------------------
bool CParty::setStandardDimensionLevel(CDungeon const* dungeon)
{
    (void)dungeon;
    // ORIG 0x0859f612 + Ghidra 双证：char 死标志 -0x19（非 GM 时置 1，
    // 从不读取——isGMUser 结果**不是**跳过条件，等级照累加）；
    // _checkValidUser(i)=='\x01' 具值比较；sum/count 分槽 -0x18/-0x14。
    char nonGM = 0;                                      // -0x19
    int levelSum = 0;                                    // -0x18
    int validCount = 0;                                  // -0x14
    for (int i = 0; i < 4; ++i)                          // -0x1c
    {
        if (_checkValidUser(i))
        {
            if (!memberUser(this, i)->isGMUser())
            {
                nonGM = 1;
            }
            levelSum += memberUser(this, i)->get_charac_level();  // thunk 0x080da2b8
            ++validCount;
        }
    }
    (void)nonGM;
    m_standardDimensionLevel = levelSum / validCount;    // cdq/idivl
    return true;
}

// ---------------------------------------------------------------------------
// sub_hellparty_value —— ORIG T 0x085b690a
// 报告：docs/class_func_reports/CParty/sub_hellparty_value.md
// hellValue *= ratio4(CBF+0x1f8)；ROUND 后低于最小值(int@CBF+0x1ec)则钳制；
// 写回 BattleData.m_hellPartyValueTotal(+0xb0c) 后广播。
// ---------------------------------------------------------------------------
void CParty::sub_hellparty_value()
{
    // [N-类] 剩 15：ORIG 0x085b5a4e 为 4.1 位移折叠形（flds 0xb0c/0xd1c、
    // mov 0xd10 单段）；4.4.7 恒物化基址 `add $0x32c/$0xb24,%eax`+短位移
    // （地址算术折叠跨版本差，同 P112/memset-edx 类）。语义同址已验证
    // （0x32c+0x7e0=0xb0c、0xb24+0x1f8=0xd1c、0xb24+0x1ec=0xd10）。
    float value = bf(this).m_hellPartyRatio4 *
                  bd(this).m_hellPartyValueTotal;                        // flds×2 fmulp
    if (D2_ROUND(value) < bf(this).m_hellPartyRatio1)                    // fistpl/jge
    {
        value = static_cast<float>(bf(this).m_hellPartyRatio1);          // fildl
    }
    bd(this).m_hellPartyValueTotal = value;              // fstps 0xb0c
    send_hellParty_value();                              // T 0x085b697c
}

// ---------------------------------------------------------------------------
// send_hellParty_value —— ORIG T 0x085b697c
// 报告：docs/class_func_reports/CParty/send_hellParty_value.md
// put_header(0,0xcf) + put_int(ROUND(hellValue * K / int@CBF+0x1e8)) + finalize。
// 常量 _DAT_08cbbbcc（objdump rodata 8cbbbfc..8cc00）：0x42c80000 = 100.0f。
// ---------------------------------------------------------------------------
void CParty::send_hellParty_value()
{
    PacketGuard packet;
    packet.put_header(0, 0xcf);

    const float kScale = 100.0f;                         // flds 0x08cbbbcc
    // ORIG：flds 0xb0c（hellValue=bd.m_hellPartyValueTotal）* 100.0f /
    // fildl int m_hellPartyRatio0（=BF+0x1e8 = CParty+0xd0c）。
    const float denom =
        static_cast<float>(bf(this).m_hellPartyRatio0);
    const int out =
        D2_ROUND((kScale * bd(this).m_hellPartyValueTotal) / denom);
    packet.put_int(out);
    packet.finalize(true);
    send_to_party(packet);
}

// ---------------------------------------------------------------------------
// GetMapPlayingTime —— ORIG T 0x085b6768
// 报告：docs/class_func_reports/CParty/GetMapPlayingTime.md
// 64 位差值 EDX:EAX 返回；调用方（DisPatcher_MoveMap/CheckOneMapPlayTimeHackCnt、
// DisPatcher_SetPlayResult/dispatch_sig）均只消费低 32 位 /1000，
// 故按 CParty.h 既定 unsigned int 声明返回低 32 位。
// ---------------------------------------------------------------------------
unsigned int CParty::GetMapPlayingTime()
{
    // ORIG 0x085b6768：getCurTickCount 后连减 m_enterTick(+0x344/48)、
    // m_baseTick(+0x33c/40)（双 sub/sbb），结果写 m_playedTime(+0x334/338)，
    // 再读回 lo/hi（edx=0）。P180 union u64 别名使直减形编译为 sub/sbb。
    bd(this).m_playedTime =
        (unsigned long long)GlobalData::s_systemTime_.getCurTickCount() -
        bd(this).m_enterTick - bd(this).m_baseTick;
    return (unsigned int)bd(this).m_playedTime;
}

// ============================================================================
// == 组队/战场链接缺口批（2026-08-25）——符号清单：
//   /tmp/df_batches/undef_link_syms_2026-08-25.txt 中 CParty 域 9 个 +
//   直接依赖（SetPlayResult 统计三辅助 / historyBloodRoundClear /
//   cal_hellparty_value / get_rate_level_difference / CLuckPoint::AddUser /
//   TOD ClearLayer / TowerOfDespairMgr::Get_APCNameByLayer）。
// 报告均在 docs/class_func_reports/CParty/ 与 objdump（无报告者标注地址）。
// ============================================================================

// ---------------------------------------------------------------------------
// ::CLuckPoint::AddUser —— ORIG T 0x08550932：user 非空时 push_back 到 m_users。
// ---------------------------------------------------------------------------
void CLuckPoint::AddUser(CUser* user)
{
    if (user != 0)
        m_users.push_back(user);
}

// ---------------------------------------------------------------------------
// CParty::GetLuckPoint —— ORIG T 0x085b61be（报告 GetLuckPoint.md）：
//   有效成员按 flag（false=全部 / true=仅 m_liveFlag==1）收集到幸运点对象。
// ---------------------------------------------------------------------------
void CParty::GetLuckPoint(CLuckPoint* luckPoint, bool onlyAlive)
{
    if (luckPoint == 0)
        return;
    for (int i = 0; i < 4; ++i)
    {
        if (!_checkValidUser(i))
            continue;
        if (onlyAlive && m_liveFlag[i] != 1)
            continue;
        luckPoint->AddUser(m_member[i].m_pUser);
    }
}

// ---------------------------------------------------------------------------
// CParty::InsertTournamentDungeonTimer —— ORIG T 0x085bd6de（报告
//   InsertTournamentDungeonTimer.md）：0x2e→0x28ms、0x2f→4ms 注册轮转定时器。
// ---------------------------------------------------------------------------
void CParty::InsertTournamentDungeonTimer(TIMER_MESSAGE msg)
{
    if (msg == (TIMER_MESSAGE)0x2e)
    {
        unsigned int key = (unsigned int)gen_timer_key(msg);
        G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)1, m_partyIndex,
                                    msg, 0x28, (int)key, 0);
    }
    else if (msg == (TIMER_MESSAGE)0x2f)
    {
        unsigned int key = (unsigned int)gen_timer_key(msg);
        G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)1, m_partyIndex,
                                    msg, 4, (int)key, 0);
    }
}

// ---------------------------------------------------------------------------
// CParty::GetMemberSlotNo(unsigned int) —— ORIG T 0x0859acc2：按角色编号
//   （CUser::get_charac_no(-1)）返回成员 CUser*；无匹配返回 0。
// ---------------------------------------------------------------------------
CUser* CParty::GetMemberSlotNo(unsigned int characNo)
{
    for (int i = 0; i < 4; ++i)
    {
        if (!_checkValidUser(i))
            continue;
        if ((unsigned int)m_member[i].m_pUser->get_charac_no(-1) == characNo)
            return m_member[i].m_pUser;
    }
    return 0;
}

// ---------------------------------------------------------------------------
// CParty::VoteLackUser —— ORIG T 0x085b3c90（报告 VoteLackUser.md）：
//   缺人投票（cElection<int,4,4>），投票成功后 0xf 定时器 0x9c4ms。
// ---------------------------------------------------------------------------
int CParty::VoteLackUser(CUser* user, int victimIdx)
{
    if (victimIdx < 0 || victimIdx > 3)
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x3591, 5);
        trace("[%s][%d]", __PRETTY_FUNCTION__, 0x3591);
        return 0;
    }
    CUser* victim = get_user(victimIdx);
    if (user == victim || victim == 0)
        return 0;
    int voterSlot = (int)GetMemberSlotNo(user);
    if (voterSlot < 0 || voterSlot > 3)
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x35a1, 5);
        trace("[%s][%d]", __PRETTY_FUNCTION__, 0x35a1);
        return 0;
    }
    m_election.Vote(voterSlot, victimIdx);
    unsigned int key = (unsigned int)gen_timer_key((TIMER_MESSAGE)0xf);
    G_TimerQueue()->InsertTimerInMilisecond((TimerEntry::OBJ_TYPE)1, m_partyIndex,
                                            (TIMER_MESSAGE)0xf, (long long)0x9c4,
                                            (int)key, 0);
    return 1;
}

// ---------------------------------------------------------------------------
// CParty::OnTimerWalkOutLackUser —— ORIG T 0x085b3e12（报告
//   OnTimerWalkOutLackUser.md）：按选举结果循环踢出缺人成员。
// ---------------------------------------------------------------------------
bool CParty::OnTimerWalkOutLackUser()   // CParty.h:657 bool（PSM 落，定义同步）
{
    int victimIdx = m_election.GetElected();
    while (victimIdx >= 0)
    {
        m_election.CancelCandidate(victimIdx);
        m_election.CancelVoter(victimIdx);
        CUser* victim = m_member[victimIdx].m_pUser;
        if (victim == 0)
        {
            LogManager::logFormat(1, "party.cpp",
                                  __PRETTY_FUNCTION__, 0x35c6,
                                  "pVictim is null");
            return 0;
        }
        SendWalkout(victimIdx, (ENUM_WALKOUT_TYPE)1);
        walkout_member(m_manager, victimIdx);
        G_GameWorld()->out_from_dungeon(victim);
        victimIdx = m_election.GetElected();
    }
    return 1;
}

// ---------------------------------------------------------------------------
// CParty::IncreaseSetPlayResultCount —— ORIG T 0x085b22e2（报告
//   IncreaseSetPlayResultCount.md）：记录该成员已回传结算（m_field4=1）。
// ---------------------------------------------------------------------------
void CParty::IncreaseSetPlayResultCount(CUser* user)
{
    int slot = (int)GetMemberSlotNo(user);
    if (slot >= 0)
        m_member[slot].m_field4 = 1;
}

// ---------------------------------------------------------------------------
// CParty::GetSetPlayResultCount —— ORIG T 0x085b231e（报告
//   GetSetPlayResultCount.md）：已回传结算成员数。
// ---------------------------------------------------------------------------
int CParty::GetSetPlayResultCount()
{
    int count = 0;
    for (int i = 0; i < 4; ++i)
    {
        if (m_member[i].m_pUser != 0 && m_member[i].m_field4 != 0)
            ++count;
    }
    return count;
}

// ---------------------------------------------------------------------------
// CParty::GetNoSendSetPlayResultUser —— ORIG T 0x085b2384（报告
//   GetNoSendSetPlayResultUser.md）：未回传成员的昵称；无效返回 ""。
// ---------------------------------------------------------------------------
const char* CParty::GetNoSendSetPlayResultUser(int slot)
{
    // ORIG 0x085b2384 + Ghidra 双证：field4 **!=0** 且 pUser 非空且
    // getCurCharacR 非空才返回昵称；pUser 地址每次重算（无局部缓存 spill）。
    // [N-类·setcc-fold] 剩 5：getCurCharacR!=0 的 bool 物化 ORIG 4.1 出
    // test; setne%al; test; je，4.4.7 恒折 test;je+mov1/jmp/mov0 分支形
    // （范式库负例 setcc-fold，probe 全试无源码复现）。
    if (m_member[slot].m_pUser != 0 && m_member[slot].m_field4 != 0 &&
        reinterpret_cast<CUserCharacInfo*>(m_member[slot].m_pUser)
            ->getCurCharacR() != 0)
    {
        return reinterpret_cast<CUserCharacInfo*>(m_member[slot].m_pUser)
            ->getCurCharacName();
    }
    return "";
}

// ---------------------------------------------------------------------------
// CParty::SetPlayResult —— ORIG T 0x085b2412（报告 SetPlayResult.md）：
//   结算一个成员：写排行/最佳时间、发送结算、清奖励、统计缺口告警。
// ---------------------------------------------------------------------------
int CParty::SetPlayResult(CUser* user)
{
    if (get_dungeon_clear_state() != 1)
        return 1;
    int step = 0;
    if (get_dungeon_clear_state() == 1)
    {
        GameResultSet* resultSet = reinterpret_cast<GameResultSet*>(&m_padResult);
        step = (int)set_play_result(user, *resultSet);
        resultSet->m_fieldc = (int)bd(this).m_field0;      // 清图时长（+0xc）
        resultSet->m_field10 = (unsigned char)G_CDataManager()->getClearTimeBonusPoint(
            (int)G_CDataManager()->getDungeonClearAverageTime(
                (int)bf(this).get_dungeon_index(), getStartGamePartyCount(),
                (ENUM_DUNGEON_DIFFICULTY)bf(this).get_dungeon_diff()),
            (int)bd(this).m_field0);                       // 奖励分（+0x10）
        int rankPoint = G_CDataManager()->calRankPoint(
            (int)resultSet->m_field10, (int)resultSet->m_field9);
        resultSet->m_field42 = (unsigned short)G_CDataManager()->getRankIndex(rankPoint);
        if (!bf(this).isQuestMaze())
            setBestClearTime((int)bf(this).get_dungeon_index(),
                             bf(this).get_dungeon_diff(), (int)bd(this).m_field0);
        int sendOk = DisPatcher_SetPlayResult::SendResult(
            (CParty*)this, *resultSet, step);
        if (sendOk == 0)
            return 0;
        clear_reward((int)resultSet->m_field42);
        set_dungeon_clear_state(2);
        insert_time_clear_dungeon_reward_msg(0x1e);
        if (get_member_count() != GetSetPlayResultCount())
        {
            cMyTrace trace(__PRETTY_FUNCTION__, 0x3261, 0);
            trace("DUNGEON_CLEAR_BUG_CHECK total(%d),recv(%d), (%s,%s,%s,%s)",
                  get_member_count(), GetSetPlayResultCount(),
                  GetNoSendSetPlayResultUser(3), GetNoSendSetPlayResultUser(2),
                  GetNoSendSetPlayResultUser(1), GetNoSendSetPlayResultUser(0));
        }
    }
    return 1;
}

// ---------------------------------------------------------------------------
// CParty::MakeQuestRewardPacket —— ORIG T 0x085bd7ae（报告
//   MakeQuestRewardPacket.md）：任务清图奖励物品包（20 任务槽 × 清图物品表）。
// ---------------------------------------------------------------------------
void CParty::MakeQuestRewardPacket(PacketGuard* packet, CUser* user)
{
    int count = 0;
    int countIndex = packet->get_index();
    packet->put_byte(0);
    UserQuest* questRef = user->getCurCharacQuestR();
    for (int q = 0; q < 0x14; ++q)
    {
        if (questRef->m_quest[q] == 0 || questRef->m_trigger[q] == 0)
            continue;
        Quest* quest = G_CDataManager()->find_quest(questRef->m_quest[q]);
        if (quest == 0)
            continue;
        short dungeonItemIdx = *(short*)((char*)questRef + 0x75da);
        int dungeonIdx = bf(this).get_dungeon_index();
        if (!quest->check_clear_item(dungeonIdx, (int)dungeonItemIdx))
            continue;
        int needCount = questRef->m_quest[q];
        if (needCount <= 0)
            continue;
        std::vector<std::pair<int, int> > itemList =
            quest->get_clear_itemlist(dungeonIdx);
        for (size_t i = 0; i < itemList.size(); ++i)
        {
            Inven_Item item;
            item.reset();
            item.m_addInfo = itemList[i].first;
            G_CDataManager()->getItemList()->create_item(
                ENUM_ITEM_CREATE_TYPE_1, item, itemList[i].second);
            int slot = (int)user->getCurCharacInvenW()->insertItemIntoInventory(
                item, (eItemAddReason)0xb, true, true);
            if (slot < 0)
                continue;
            packet->put_short(slot);
            packet->put_int(item.m_addInfo);
            packet->put_int(item.get_add_info());
            packet->put_short(0);
            packet->put_byte((int)item.m_amp.getAbilityType());
            packet->put_short((int)item.m_amp.getAbilityValue());
            ++count;
            if ((item.m_addInfo >> 16) == 1)
            {
                CItem* card = G_CDataManager()->find_item(item.m_addInfo);
                if (card != 0)
                    CLuckPoint::UseLuckPoint(user, (int)(item.m_addInfo >> 16),
                                             card->get_rarity(),
                                             card->get_grade());
            }
        }
    }
    if (count != 0)
        packet->put_byte(countIndex, count);
}

// ---------------------------------------------------------------------------
// CParty::OnTimeoutBossDie —— ORIG T 0x085aa5d4（报告 OnTimeoutBossDie.md）：
//   超时击杀当前 Boss（BF.m_apcAllKilledSec）：遍历怪物→击杀→失败按队伍类型收尾。
// ---------------------------------------------------------------------------
int CParty::OnTimeoutBossDie(int monsterId, unsigned int characNo)
{
    if (bf(this).m_apcAllKilledSec != monsterId)
        return 0;
    CUser* user = GetMemberSlotNo(characNo);
    if (user == 0)
        return 0;
    if (m_dungeon != 0 && m_dungeon->IsEnterEachMap())
        bf(this).SetCurSeatNo(get_party_seatno(user));
    std::vector<int> mobIds;
    MapInfo* mapInfo = bf(this).GetCurrentMapInfo();
    if (mapInfo != 0)
    {
        std::map<int, map_monster>::iterator it = mapInfo->m_monsterMap.begin();
        for (; it != mapInfo->m_monsterMap.end(); ++it)
            mobIds.push_back(it->first);
    }
    char dieBuf[0xa3f];
    memset(dieBuf, 0, sizeof(dieBuf));
    const MSG_MONSTER_DIE& die = *reinterpret_cast<const MSG_MONSTER_DIE*>(dieBuf);
    for (size_t i = 0; i < mobIds.size(); ++i)
    {
        if (mapInfo != 0 && mapInfo->m_monsterMap.find(mobIds[i]) !=
                                mapInfo->m_monsterMap.end())
        {
        }
        if (kill_monster(user, mobIds[i], 0xffff, die, 0) != 1)
        {
            cMyTrace trace(__PRETTY_FUNCTION__,
                           0x22ce, 0);
            trace("KILLBOSS2 server - monster id(%d) failed", mobIds[i]);
            if (m_partyType == 1)
            {
                SetEPLPState(2);
                unsigned int key = (unsigned int)gen_timer_key((TIMER_MESSAGE)0x15);
                G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)1, m_partyIndex,
                                            (TIMER_MESSAGE)0x15, 0x3c, (int)key, 0);
                set_state(1);
                PacketGuard packet;
                packet.put_header(0, 0xfe);
                packet.finalize(true);
                send_to_party(packet);
            }
            else if (m_dungeon != 0 && m_dungeon->m_bloodDungeonType < 1)
            {
                PacketGuard packet;
                packet.put_header(0, 0x1f);
                packet.finalize(true);
                send_to_party(packet);
            }
            else if (m_dungeon != 0)
            {
                bf(this).m_bloodReward.onFinishBloodRound(
                    true, (short)bf(this).m_bloodRound.getBloodRoundOrder(),
                    this, m_dungeon);
                unsigned int key = (unsigned int)gen_timer_key((TIMER_MESSAGE)0x22);
                G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)1, m_partyIndex,
                                            (TIMER_MESSAGE)0x22, 5, (int)key, 0x22);
            }
            break;
        }
    }
    return 1;
}

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

void CParty::sendPremiumGoldCardStatistic(int type, int value1, int value2)
{
    Packet_Goldcard_Event_Statistic_GTS packet;
    packet.m_stat.m_field0 = (unsigned char)type;
    packet.m_stat.m_field1 = value1;
    packet.m_stat.m_field5 = value2;
    CStatisticServerProxy* proxy =
        GlobalData::s_statistic_proxy_mgr->GetServerProxy((ENUM_SERVER_GROUP)0);
    if (proxy != 0)
        proxy->SendPacket((char*)&packet, 0x13);
}

// ---------------------------------------------------------------------------
// CParty::historyBloodRoundClear —— ORIG T 0x085b7db6：逐有效成员写
//   cUserHistoryLog::BloodRoundClear（CUser+0x79700）。
// ---------------------------------------------------------------------------
void CParty::historyBloodRoundClear(int round, unsigned int clearTime,
                                    unsigned int playTime, const char* dungeonName)
{
    char memberNames[0x40];
    memset(memberNames, 0, sizeof(memberNames));
    getMemberNames(memberNames);
    for (int slot = 0; slot < 4; ++slot)
    {
        if (get_user(slot) == 0 || !checkValidUser(slot))
            continue;
        cUserHistoryLog* log = &get_user(slot)->m_historyLog;
        log->BloodRoundClear(round, clearTime, playTime, dungeonName, memberNames);
    }
}

// ---------------------------------------------------------------------------
// CParty::cal_hellparty_value —— ORIG T 0x085b58ac（报告 cal_hellparty_value.md）：
//   地狱值累计（上限钳制 + send_hellParty_value）。
//   +0xd0c..+0xd5b（m_padcee 未建模 hell 值表区）按报告偏移访问。
// ---------------------------------------------------------------------------
void CParty::cal_hellparty_value(int dungeonDiff, int rateLevelDiff, int memberCount)
{
    if (rateLevelDiff == 0 || memberCount < 0 || dungeonDiff >= 5)
        return;
    // ORIG 偏移映射（报告 cal_hellparty_value.md，BF = this+0xb24）：
    //   +0xd58 = BF+0x234 m_hellPartyScriptRate（float）
    //   +0xd34+d*4 = BF+0x210 m_hellPartyMapRate[d]
    //   +0xd48+m*4 = BF+0x224 m_hellPartyItemRate[m]
    //   +0xd18 = BF+0x1f4 m_hellPartyRatio3（int）
    //   +0xd24+r*4 = BF+0x200 m_hellPartyDungeonRate[r]
    //   +0xd0c = BF+0x1e8 m_hellPartyRatio0（int 上限）
    //   +0xb0c = bd.m_hellPartyValueTotal
    CBattle_Field& field = bf(this);
    float baseValue = field.m_hellPartyScriptRate;
    if (m_dungeon->m_89c != 1)
        baseValue = 0.0f;
    // ORIG 乘序（cal_hellparty_value.md +0x0a0..+0x0b3）：ratio3(fildl) 先乘
    // dungeonRate（fmuls -0x18），再乘 (mapRate+1+base+itemRate) 和，最后 +old。
    float value = (float)field.m_hellPartyRatio3 *
                  field.m_hellPartyDungeonRate[rateLevelDiff] *
                  (field.m_hellPartyMapRate[dungeonDiff] + 1.0f + baseValue +
                   field.m_hellPartyItemRate[memberCount]) +
                  bd(this).m_hellPartyValueTotal;
    bd(this).m_hellPartyValueTotal = value;
    if (field.m_hellPartyRatio0 < (int)value)
        bd(this).m_hellPartyValueTotal = (float)field.m_hellPartyRatio0;
    send_hellParty_value();
}

// ---------------------------------------------------------------------------
// CParty::get_rate_level_difference —— ORIG T 0x085b59ca（报告
//   get_rate_level_difference.md）：等级差档位（0/1/2/3）。
// ---------------------------------------------------------------------------
int CParty::get_rate_level_difference()
{
    if (m_dungeon == 0)
        return 0;
    int diff = m_dungeon->get_standard_level() - getMemberMaxLevel();
    if (diff < 0)
        diff = -diff;
    if (diff < 0xb)
        return 1;
    if (diff < 0xb || 0x13 < diff)
    {
        if (diff < 0x14)
            return 0;
        return 3;
    }
    return 2;
}

// ---------------------------------------------------------------------------
// TOD_UserState::ClearLayer() —— ORIG T 0x0864390e：层推进 + 脏标记 + 清层时间。
// ---------------------------------------------------------------------------
void TOD_UserState::ClearLayer()
{
    reinterpret_cast<charac_expand::CData*>(this)->alter();
    reinterpret_cast<TOD_Layer*>(reinterpret_cast<char*>(this) + 0x14)
        ->increaseLayer();
    m_lastClearTime = GlobalData::s_systemTime_.getCurSec();
}

// ---------------------------------------------------------------------------
// TowerOfDespair_APC_Mgr::Get_APC_Name_byIndex —— ORIG T 0x085ff020：
//   映射命中返回 SendingAPC_Info::get_user_charac_name()，未命中返回 0。
// ---------------------------------------------------------------------------
const char* TowerOfDespair_APC_Mgr::Get_APC_Name_byIndex(const TOD_Layer& layer)
{
    std::map<TOD_Layer, SendingAPC_Info>::const_iterator it =
        m_APCInfoMap.find(layer);
    if (it != m_APCInfoMap.end())
        return const_cast<SendingAPC_Info&>(it->second).get_user_charac_name();
    return 0;
}

// ---------------------------------------------------------------------------
// TowerOfDespairMgr::Get_APCNameByLayer —— ORIG T 0x08643e4a：
//   m_apcMgr 非空时转 Get_APC_Name_byIndex。
// ---------------------------------------------------------------------------
const char* TowerOfDespairMgr::Get_APCNameByLayer(const TOD_Layer& layer)
{
    // ORIG 0x08643e4a：非空正分支 inline，null 冷岛在尾。
    if (m_apcMgr != 0)
    {
        return m_apcMgr->Get_APC_Name_byIndex(layer);
    }
    return 0;
}

// ---------------------------------------------------------------------------
// TOD_Layer::increaseLayer() —— ORIG T 0x085fe824：m_layer + 1；超过 100 时
//   resetLayer()（m_layer 置 1）。
// ---------------------------------------------------------------------------
void TOD_Layer::increaseLayer()
{
    ++m_layer;
    if (m_layer > 100)
        resetLayer();
}


// ---------------------------------------------------------------------------
// cUserHistoryLog::BloodRoundClear —— ORIG T 0x08684af0：
//   m_user 非空时 LogHistory("BloodRoundClear,%d,%u,%s", round, clearTime, names)。
// ---------------------------------------------------------------------------
void cUserHistoryLog::BloodRoundClear(int round, unsigned int clearTime,
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

// ---------------------------------------------------------------------------
// CParty::ClearDungeon —— ORIG T 0x085a9330（报告 ClearDungeon.md 逐段转写）：
//   清图流程：普通/绝望塔/巡回分支 → 动作点/统计/奖励 → 时间与地狱值结算。
// ---------------------------------------------------------------------------
void CParty::ClearDungeon()
{
    if (m_tournamentVictory != 0)
        return;
    if (m_partyType != 1)
    {
        unsigned int now = GlobalData::s_systemTime_.getCurTickCount();
        unsigned long long enterTick =
            (unsigned long long)bd(this).m_field24 << 32 | bd(this).m_field20;
        unsigned long long baseTick =
            (unsigned long long)bd(this).m_field30 << 32 | bd(this).m_field2c;
        unsigned long long elapsed = (unsigned long long)now - enterTick - baseTick;
        bd(this).m_field0 = (unsigned int)elapsed;
        bd(this).m_field4 = (unsigned int)(elapsed >> 32);
        int memberCount = get_member_count();
        if (m_dungeon != 0)
        {
            for (int slot = 0; slot < 4; ++slot)
            {
                if (!_checkValidUser(slot))
                    continue;
                CUser* user = m_member[slot].m_pUser;
                if (m_dungeon->m_villageAttackEventFlag == 0)
                {
                    if (m_dungeon->get_index() == 100)
                        APSystem::CUserProc::ClearActionAndSendtoUser(
                            user, (APSystem::_ActionGroupIndex)0x19, 0, (ENUM_PACKETCLASS)0);
                    else if (m_dungeon->m_89c == 0)
                    {
                        if (m_dungeon->get_dimension_possible() > 0)
                            APSystem::CUserProc::ClearActionAndSendtoUser(
                            user, (APSystem::_ActionGroupIndex)0x1c, 0, (ENUM_PACKETCLASS)0);
                    }
                    else
                        APSystem::CUserProc::ClearActionAndSendtoUser(
                            user, (APSystem::_ActionGroupIndex)0x1b, 0, (ENUM_PACKETCLASS)0);
                }
                else
                    APSystem::CUserProc::ClearActionAndSendtoUser(
                            user, (APSystem::_ActionGroupIndex)0x18, 0, (ENUM_PACKETCLASS)0);
                if (!user->IsUsedCoinInDungeon())
                    APSystem::CUserProc::ClearActionAndSendtoUser(
                            user, (APSystem::_ActionGroupIndex)0xf, 0, (ENUM_PACKETCLASS)0);
            }
        }
        PacketGuard packet;
        if (m_dungeon->m_bloodDungeonType < 1)
        {
            if (!m_dungeon->isTowerOfDespairDungeon())
            {
                if (!m_dungeon->isTournamentDungeon())
                {
                    packet.put_header(0, 0x1f);
                    packet.finalize(true);
                }
                else
                {
                    m_tournamentVictory = 1;
                    bf(this).m_tournamentReward.ResetReward();
                    MakeTournamentReward(bf(this).m_tournamentReward);
                }
            }
            else
            {
                m_tournamentVictory = 1;
                CUser* manager = getManager();
                TOD_UserState* todState = reinterpret_cast<TOD_UserState*>(
                    manager->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)0xd));
                todState->ClearLayer();
                int layerCount = 5;
                bf(this).m_todReward.m_todPos.first = -1;
                bf(this).m_todReward.m_todPos.second = 0;
                // ORIG 0x085a9330：每次取层都是 todState->getLastEnterLayer()
                //（sret 临时，无 TOD_Layer(ushort) 构造调用点，共 5 处）。
                int apcIdx = reinterpret_cast<TowerOfDespairMgr*>(
                    GlobalData::s_pTowerOfDespairMgr)->Get_APCIndexByLayer(
                        (unsigned int)todState->getLastEnterLayer().GetLayer() & 0xffff);
                if (apcIdx != -1)
                {
                    const char* apcName = reinterpret_cast<TowerOfDespairMgr*>(
                        GlobalData::s_pTowerOfDespairMgr)->Get_APCNameByLayer(
                            todState->getLastEnterLayer());
                    if (apcName != 0 && *apcName != '\0')
                    {
                        PacketGuard namePacket;
                        namePacket.put_header(0, 0x56);
                        namePacket.put_byte(8);
                        namePacket.put_short((int)todState->getLastEnterLayer().GetLayer());
                        namePacket.put_int((int)strlen(apcName));
                        namePacket.put_str(apcName, (int)strlen(apcName));
                        const char* myName = manager->getCurCharacName();
                        if (myName == 0)
                        {
                            namePacket.put_int(5);
                            namePacket.put_str("Neople", 6);
                        }
                        else
                        {
                            namePacket.put_int((int)strlen(myName));
                            namePacket.put_str(myName, (int)strlen(myName));
                        }
                        namePacket.finalize(true);
                        G_GameWorld()->send_all(namePacket);
                    }
                    layerCount = 9;
                    bf(this).m_todReward.m_todPos.first = 0x4e4;
                    bf(this).m_todReward.m_todPos.second = 1;
                }
                if (todState->getLastEnterLayer().GetLayer() == 100)
                {
                    bf(this).m_todReward.m_todPos.first = 0xcf2;
                    bf(this).m_todReward.m_todPos.second = 1;
                }
                WongWork::stGenerateRefData_t ref;
                WongWork::stGenerateResult_t result;
                std::vector<std::pair<int, int> > itemList;
                ref.m_field34 = 0;
                ref.m_field35 = 0;
                ref.m_rateSlot = 1;
                ref.m_field1c = 1.0f;
                ref.m_dropRate = 100;
                for (int i = 0; i < layerCount; ++i)
                {
                    WongWork::CItemGeneratorMgr* genMgr =
                        G_CDataManager()->getItemGenerator();
                    genMgr->getGenerator((WongWork::CItemGeneratorMgr::eGenerateType_t)2)
                        ->generateSpecificItem(ref, result);
                }
                itemList.resize(result.m_dropItems.size());
                for (size_t i = 0; i < result.m_dropItems.size(); ++i)
                {
                    const Inven_Item& item = result.m_dropItems[i];
                    int itemIdx = item.m_addInfo;
                    itemList[i].first = itemIdx;
                    CItem* card = G_CDataManager()->find_item(itemIdx);
                    if (card != 0 && !card->is_stackable())
                        itemList[i].second = 1;
                    else
                        itemList[i].second = item.get_add_info();
                }
                bf(this).m_todReward.m_todMap = itemList;
                if (bf(this).m_todReward.m_todPos.first != -1)
                    itemList.push_back(std::pair<int, int>(
                        bf(this).m_todReward.m_todPos.first,
                        bf(this).m_todReward.m_todPos.second));
                std::vector<int> slots;
                int putCount = (int)manager->getCurCharacInvenW()->insert_event_items(
                    itemList, slots, (eMoneyAddReason)0x1f, (eItemAddReason)0x37, true,
                    "game_server_msg_191", "game_server_msg_192");
                if (putCount >= 1 && slots.size() >= 3)
                {
                    for (int i = 0; i < putCount; ++i)
                    {
                        unsigned int slot = (unsigned int)slots[i * 3];
                        manager->SendUpdateItem((CUser::eSendTarget)1,
                                               (ENUM_ITEMSPACE)0, (int)slot);
                    }
                }
                unsigned int key = (unsigned int)gen_timer_key((TIMER_MESSAGE)0x2c);
                G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)1, m_partyIndex,
                                            (TIMER_MESSAGE)0x2c, 2, (int)key, 0x2c);
                Packet_TowerOfDespair_Statistic_GTS todStat;
                todStat.m_key.m_0 = (int)todState->getLastEnterLayer().GetLayer();
                todStat.m_key.m_4 = 0;
                CStatisticServerProxy* proxy =
                    GlobalData::s_statistic_proxy_mgr->GetServerProxy(
                        (ENUM_SERVER_GROUP)0);
                if (proxy != 0)
                    proxy->SendPacket((char*)&todStat, 0x17);
            }
        }
        else
        {
            bf(this).m_bloodReward.onFinishBloodRound(
                true, (short)bf(this).m_bloodRound.getBloodRoundOrder(), this, m_dungeon);
            unsigned int key = (unsigned int)gen_timer_key((TIMER_MESSAGE)0x22);
            G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)1, m_partyIndex,
                                        (TIMER_MESSAGE)0x22, 5, (int)key, 0x22);
        }
        for (int slot = 0; slot < 4; ++slot)
        {
            if (!_checkValidUser(slot))
                continue;
            CUser* user = m_member[slot].m_pUser;
            user->getCurCharacQuestW()->set_authen_data((QUEST_CONDITION)4,
                                                        memberCount, 0);
            // ORIG 0x85aa027..0x85aa057：64 位 elapsed（m_field0 低/ m_field4 高，
            // 即 this+0x32c/+0x330）经 __divdi3 除以 1000 后取低 32 位入第二个
            // set_authen_data（QUEST_CONDITION)2；ORIG 每个 set 前各调一次
            // getCurCharacQuestW（两次 call，互不复用）。
            user->getCurCharacQuestW()->set_authen_data(
                (QUEST_CONDITION)2,
                (int)(((long long)bd(this).m_field4 << 32 |
                       (unsigned int)bd(this).m_field0) /
                      1000),
                0);
            if (m_dungeon->m_bloodDungeonType == 0 &&
                !m_dungeon->isTowerOfDespairDungeon() &&
                !m_dungeon->isTournamentDungeon())
                user->Send(packet);
            secretshop::SECRET_SHOP_DATA* shopData =
                reinterpret_cast<secretshop::SECRET_SHOP_DATA*>(&m_padShop);
            if (shopData->IsOpen() != 0)
            {
                int level = m_dungeon->get_standard_level();
                int dungeonIdx = m_dungeon->get_index();
                // +0x2a8+slot*0x1c = SECRET_SHOP_DATA.m_info[slot].m_retailer.m_sales
                //（+0x2a4 SECRET_SHOP_DATA；m_info +0x04，SECRET_SHOP_INFO 0x1c）。
                std::vector<secretshop::SALE_INFO>& shopItems =
                    shopData->m_info[slot].m_retailer.m_sales;
                GlobalData::s_secret_shop->LotteryItems(
                    shopItems,
                    shopData->m_openCode, dungeonIdx, level);
                SendSecretShopItemList(user);
                APSystem::CUserProc::ClearActionAndSendtoUser(
                            user, (APSystem::_ActionGroupIndex)0xd, 0, (ENUM_PACKETCLASS)0);
            }
            if (user->getBreakAwayDungeonClearCnt() > 0)
                user->subBreakAwayDungeonClearCnt();
        }
        memberCount = get_member_count();
        if (memberCount == 1)
        {
            const float* hellValues = static_cast<const float*>(
                G_CDataManager()->get_hellparty_script_values());
            bd(this).SetHellPartyValueTotal((int)*(int*)((char*)hellValues + 8));
        }
        // ORIG 0x85aa27e..0x85aa2b0 比较 CParty+0xcee/0xcef/0xcdc（= CBattle_Field
        // +0x1ca m_killedAllHellGroups / +0x1cb m_field1cb / +0x1b8
        // m_hellPartyActive）三个字节均非 1 时 cal_hellparty_value；
        // 报告 ClearDungeon.md 反编译 C 末尾条件块。
        if (memberCount != 1 && bf(this).m_killedAllHellGroups != 1 &&
            bf(this).m_field1cb != 1 && bf(this).m_hellPartyActive != 1)
        {
            cal_hellparty_value(bf(this).get_dungeon_diff(),
                                get_rate_level_difference(), memberCount - 1);
        }
        m_dungeonClearTracer.Trace("void CParty::ClearDungeon()");
        m_tournamentVictory = 1;
        QuickParty::RewardData_Param rewardParam(this);
        QuickParty::RewardData_Result rewardResult;
        set_random_reward((QuickParty::RandomBuffType)10, rewardParam, rewardResult);
        rewardResult.clear();
        return;
    }
    for (int slot = 0; slot < 4; ++slot)
    {
        if (!_checkValidUser(slot))
            continue;
        OnSuccessedDungeonClear(m_member[slot].m_pUser);
    }
    SetEPLPState(2);
    unsigned int key = (unsigned int)gen_timer_key((TIMER_MESSAGE)0x15);
    G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)1, m_partyIndex,
                                (TIMER_MESSAGE)0x15, 0x3c, (int)key, 0);
    set_state(1);
    PacketGuard packet;
    packet.put_header(0, 0xfe);
    packet.finalize(true);
    send_to_party(packet);
    m_dungeonClearTracer.Trace("void CParty::ClearDungeon()");
    m_tournamentVictory = 1;
}

// ---------------------------------------------------------------------------
// CParty::makeBestClearTimePacket —— ORIG T 0x085be1f8：成员击杀榜时长包
//   （isQuestMaze / 成员数 / 每成员 uid+bestTime+newRecord）。
// ---------------------------------------------------------------------------
void CParty::makeBestClearTimePacket(PacketGuard& packet, int clearTime)
{
    packet.put_byte(bf(this).isQuestMaze() ? 1 : 0);
    packet.put_byte(get_member_count());
    for (int slot = 0; slot < 4; ++slot)
    {
        if (!checkValidUser(slot))
            continue;
        CUser* user = m_member[slot].m_pUser;
        int diff = bf(this).get_dungeon_diff();
        int dungeonIdx = bf(this).get_dungeon_index();
        BestClearTime* best = reinterpret_cast<BestClearTime*>(
            user->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)0x12));
        int bestTime = best != 0 ? best->getBestClearTime(dungeonIdx, diff) : 0;
        int newRecord = best != 0 ? (best->checkNewRecord() ? 1 : 0) : 0;
        if (bestTime == 0)
            bestTime = clearTime;
        unsigned int uid = (unsigned int)user->get_unique_id();
        packet.put_byte((int)(uid & 0xff));
        packet.put_short((int)(uid >> 8));
        packet.put_int(bestTime);
        packet.put_byte(newRecord);
    }
}

// ---------------------------------------------------------------------------
// DisPatcher_SetPlayResult::SendResult —— ORIG T 0x081f5db6：
//   header 0x22 结算广播（排名/时间/奖励分/最佳时间+新记录）。
// ---------------------------------------------------------------------------
int DisPatcher_SetPlayResult::SendResult(CParty* party, GameResultSet& result,
                                         int step)
{
    (void)step;
    PacketGuard packet;
    packet.put_header(0, 0x22);
    packet.put_byte((int)(unsigned short)result.m_field48);
    packet.put_int(result.m_fieldc);
    packet.put_byte((int)(char)result.m_field10);
    packet.put_byte((int)(char)result.m_field9);
    party->makeBestClearTimePacket(packet, result.m_fieldc);
    packet.finalize(true);
    party->send_to_party(packet);
    return 1;
}
