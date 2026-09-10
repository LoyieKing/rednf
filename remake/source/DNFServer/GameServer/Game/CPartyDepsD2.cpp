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

// ---------------------------------------------------------------------------
// CalcurateManageBonus —— ORIG T 0x085ada30
// 报告：docs/class_func_reports/CParty/CalcurateManageBonus.md
// 返回 st0 浮点；user 为空/角色为空/等级>=最大等级 → 0。
// type=0 → CharacManageScript::GetExpBonus(manageLevel)；
// type=1 → GetGoldBonus(manageLevel) * (double)(uint)param（fildll 无符号扩展）。
// ---------------------------------------------------------------------------
// [b28d-solo-CPartyDepsD2] AE（67=67）：P0/P200 去 characLevel 具名 short（内联
// movzwl+movswl %ax,%ebx）、P24/P52 去 manageLevel/dataMgr 缓存、P243 乘序改
// (float)param * GetGoldBonus（fildll+fstps 先落槽）⇒ 帧 0x24 逐条全等。
float CParty::CalcurateManageBonus(ManageBonusType::T type, CUser* user,
                                   unsigned int param)
{
    if (user == 0)
        return 0.0f;                                    // fldz
    if (user->getCurCharacR() == 0)
        return 0.0f;                                    // getCurCharacR==0

    // [b28d-solo-CPartyDepsD2] P0/P200：ORIG 无 characLevel 槽
    // （movzwl 0x27(%eax),%eax; movswl %ax,%ebx）⇒ 内联表达式直用。
    // movzwl 0x27(%eax)：_Charac_info::m_level(+0x27, CUser.h)
    if (!(user->getCurCharacR()->m_level < user->GetUserMaxLevel()))
        return 0.0f;

    if (type == ManageBonusType::T_0)
    {
        // CDataManager+0x6394 = advancealtar::ConfigParameter+0x30
        //                     = m_altarConfig.m_characManageScript
        return G_CDataManager()->m_altarConfig.m_characManageScript
            ->GetExpBonus(user->GetManageLevel());
    }
    if (type == (ManageBonusType::T)1)
    {
        // ORIG 先算左操作数 (float)param（fildll+fstps 落槽）再算右调用链，
        // 末条 fmuls 内存操作数 ⇒ 左为 param 转换（P243 书写序判据）。
        return static_cast<float>(param) *
               G_CDataManager()->m_altarConfig.m_characManageScript
                   ->GetGoldBonus(user->GetManageLevel());
    }
    return 0.0f;
}

// ---------------------------------------------------------------------------
// CheckEnterDimensionDungeon(CDungeon const*, unsigned char&, bool) —— T 0x0859f1ce
// 报告：docs/class_func_reports/CParty/CheckEnterDimensionDungeon.md
// 错误码：0xab 人数不足且非 GM；0xac 等级不足（写出槽号）；0xad 维度入场券缺失
// （characR+0xeb8+possible 字节 <1 且 partyType!=1 且非 GM）；遍历完返回 1。
// ---------------------------------------------------------------------------
// [b28d-solo-CPartyDepsD2] AE（135=135）：(signed char)get_dimension_possible()-1
// 与 (signed char)get_dimension_min_partymem（movsbl 两处）+ P311 具名成员数组
// m_demensionInout[dimIdx]（单条 scaled disp 0xeb9）⇒ 槽 -0x1c/-0x14/-0x10/-0xc 全等。
int CParty::CheckEnterDimensionDungeon(CDungeon const* dungeon, unsigned char& flag,
                                       bool checkMinParty)
{
    const int memberCount = get_member_count();
    // [b28d-solo-CPartyDepsD2] ORIG 0x0859f1f4 `movsbl %al,%eax; sub $0x1,%eax;
    // mov %eax,-0x10` ⇒ 源码为 (signed char)get_dimension_possible() - 1（同
    // CParty.cpp:1497 族）；券字节索引随之为 0xeb9+dimIdx。
    const int dimIdx = (signed char)dungeon->get_dimension_possible() - 1;

    for (int i = 0; i < 4; ++i)                          // cmpl $3/setle：i<=3
    {
        if (!_checkValidUser(i))
            continue;

        if (checkMinParty)
        {   // 人数下限：memberCount < min_partymem 且非 GM → 0xab
            // ORIG 0x0859f237 `movsbl %al,%eax` ⇒ (signed char) 语义（CParty.cpp:1509 同）。
            if ((memberCount < (signed char)dungeon->get_dimension_min_partymem()) &&
                !memberUser(this, i)->isGMUser())
                return 0xab;
        }

        // 等级下限 → 0xac（写出违规槽号）
        if (memberUser(this, i)->get_charac_level() < dungeon->get_min_level())
        {
            flag = (unsigned char)i;                     // mov %dl,(%eax)
            return 0xac;
        }

        // 维度入场券计数：*(char*)(characR + 0xeb9 + dimIdx) < 1
        // （单参重载 0x859f3fc 以 possible-1 + 0xeb9 寻址，同一字节）
        // [b28d-solo-CPartyDepsD2] P0/P89/P311：ORIG 0x0859f2e7 无 characR/
        // ticketCount 槽（`movzbl 0xeb9(%eax,%edx,1),%eax; test; jg`）⇒ 具名
        // 成员数组 m_demensionInout[dimIdx]（+0xeb9，CUserCharacInfo.h:484），
        // 与 CParty.cpp:1517 单参重载同形。
        if ((signed char)memberUser(this, i)->getCurCharacR()
                ->m_demensionInout[dimIdx] < '\x01' &&
            m_battleField.m_bloodState != 1 &&
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
// 以 (m_battleField.m_bloodState, mode, -1) 构造 DimensionPartyCount（ctor W 0x085beabe）询问
// 可容纳入场；可容纳时要求队伍人数 > 计数字节，否则置 false 转发 bool 重载。
// ---------------------------------------------------------------------------
int CParty::CheckEnterDimensionDungeon(CDungeon const* dungeon, unsigned char& flag,
                                     char mode)
{
    bool allowed = true;                                 // movb $1,-0xd

    // ORIG 以 3 参构造 DimensionPartyCount((char)m_battleField.m_bloodState, mode, -1)
    // （ctor W 0x085beabe），非默认构造+赋值
    DimensionPartyCount count((char)m_battleField.m_bloodState, mode, (char)-1);

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
// [b28d-solo-CPartyDepsD2] 判词 N82/N95（361=361）：已修 ①skip 改 ORIG 短路链
// 寄存器形 `A || !B`（A=pDungeon!=0&&(m_89c!=0||possible>0)，消 skip 槽）
// ②avg/band 三组交错 + float 预清两句式（ORIG -0x5c/-0x54/-0x50 族）。
// 残余：帧 0x94 vs 0x84、avg1 与 int 中转槽互换（ORIG avg1=-0x5c/int=-0x6c vs
// 我方 -0x58/-0x5c）——4.4 槽分配序差异（N82/N95 族）。语义/调用集齐。
void CParty::CheckHackAverageHitCount()
{
    if (GlobalData::s_villageMonsterMgr->OnEvent())      // 村庄袭击中跳过（0x82f0f44）
        return;
    if (!(m_mapHitCount < 0x2e))                         // >=46 直接返回
        return;

    // 血之副本 / 维度副本 / 样本<=1 → 返回。[b28d-solo-CPartyDepsD2] ORIG
    // 0x085bb0dd..0x085bb12d 为短路链 `A || !B` 寄存器形（无 skip 槽）：
    // A = pDungeon!=0 && (m_89c!=0 || possible>0)，B = m_mapHitCount>1。
    if ((m_battleField.m_pDungeon != 0 &&
         (m_battleField.m_pDungeon->m_89c != 0 ||
          m_battleField.m_pDungeon->get_dimension_possible() > 0)) ||
        !(m_mapHitCount > 1))
        return;

    // 三组平均值（asm fildl/fdivrp：整型转 float 相除）。ORIG 每组的
    // 「float avg=0 预清 → 除法 → 上下界」三件交错（avg1 -0x5c/-0x54/-0x50、
    // avg2 -0x4c/-0x48/-0x44、avg3 -0x3c/-0x38/-0x34），照抄顺序。
    float avg1 = 0.0f;
    avg1 = static_cast<float>(m_mapHitSum) / static_cast<float>(m_mapHitCount);
    int band1Upper = D2_ROUND(avg1 + avg1 * 10.0f + 0.1f);   // fistpl -0x54
    int band1Lower = D2_ROUND(avg1 - avg1 * 10.0f);          // fistpl -0x50
    float avg2 = 0.0f;
    avg2 = static_cast<float>(m_mapHitSum2) / static_cast<float>(m_mapHitCount);
    int band2Upper = D2_ROUND(avg2 + avg2 * 10.0f + 0.1f);
    int band2Lower = D2_ROUND(avg2 - avg2 * 10.0f);
    float avg3 = 0.0f;
    avg3 = static_cast<float>(m_mapHitSum3) / static_cast<float>(m_mapHitCount);
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
        // [b28d-51] P0/N96 清绑（原 `register uchar v asm("eax")`）；ORIG
        // 0x085ba574 O@9-12 `call; xor$1; test; je` 由裸 `!call` 形承载。
        if (!_checkValidUser(i))
        {
            continue;
        }
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
        // [b28d-51] P0/N96 清绑：删 `register uchar v asm("eax")` 与
        // `register bool s asm("al")`（全域禁绑定）；改裸 bool 形。
        // ORIG 0x085ba1c0 O@9-21 原形 `call; xor$1; test; jne` +
        // `call; test;sete%al; test; je`（4.4 清绑后物化路径见函数头判词）。
        if (!_checkValidUser(i))
        {
            continue;
        }
        if (m_routingData.GetMemberRoutingState(i) == 0)
        {
            return false;
        }
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

            // [P148 嵌套实参] ORIG 0x085b5f8e O@28：call 结果留 %eax 直调
            // outgoing arg，无 job 具名 spill。
            if (isCheckUseSkill(user->get_charac_job(), skillIdx))  // T 0x085bb5b0
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
    // [共享岛单链] ORIG 0x085bb5b0：9 条件合一 || 短路链，各 jcc 跳共享
    // return false 岛（9 条独立 if 语句 4.4 会各出 mov0;jmp 岛 +16 行）。
    if (skillIdx > 0xa8 ||
        (job == 0 && skillIdx == 0x2e) ||
        (job == 1 && skillIdx == 0x5) ||
        (job == 2 && skillIdx == 0x4) ||
        (job == 3 && skillIdx == 0xc) ||
        (job == 4 && skillIdx == 0x1) ||
        (job == 5 && skillIdx == 0x4) ||
        (job == 6 && skillIdx == 0x1) ||
        (job == 7 && skillIdx == 0x5))
    {
        return false;
    }
    return true;
}

// ---------------------------------------------------------------------------
// SaveCoin —— ORIG T 0x085b3520
// 报告：docs/class_func_reports/CParty/SaveCoin.md
// eCoinSubReason 参数未消费（ORIG 形态）；全员 SaveMoneyCoin。
// ---------------------------------------------------------------------------
// [X2·技巧22 精化形（Main 批准，ORIG 0x085b3520 O@9-12）] callee 结果
// xor-化：绑 %eax 具名 uchar + ^=1 → `call; xorl $1,%eax; testb; jne`。
void CParty::SaveCoin(eCoinSubReason reason)
{
    (void)reason;
    for (int i = 0; i < 4; ++i)
    {
        // [b28d-51] P0/N96 清绑（原 `register uchar v asm("eax")`）。
        if (!_checkValidUser(i))
        {
            continue;
        }
        memberUser(this, i)->SaveMoneyCoin();        // T 0x086504d8
    }
}

// ---------------------------------------------------------------------------
// changeBattleDataOnMoveMap —— ORIG T 0x085a8ee0
// 报告：docs/class_func_reports/CParty/changeBattleDataOnMoveMap.md
// （BD+0x18/0x1c ↔ CParty+0x344/0x348）；非 firstVisit 时累加 BD.m_field68c
// （CParty+0x9b8）。两次独立取时（ORIG 形态）。
// ---------------------------------------------------------------------------
void CParty::changeBattleDataOnMoveMap(bool firstVisit)
{
    // [b28d-51] P183 禁引用缓存 + P246 u64 别名：ORIG 0x085a8ef4/0x085a8f1c 为
    // 零扩展 64 位整体存（mov $0,%edx; mov %eax,0x360; mov %edx,0x364），
    // 且无具名 now 局部（call 结果 eax 直存）；两次独立取时。
    bd(this).m_moveTick = GlobalData::s_systemTime_.getCurTickCount();
    bd(this).m_enterTick = GlobalData::s_systemTime_.getCurTickCount();

    if (!firstVisit)                                     // movzbl xor $1 test je
        ++bd(this).m_field68c;                           // +0x9b8
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
    if (m_battleField.m_bloodState == 1)
        return false;
    if (m_battleField.m_pDungeon == 0)
        return false;
    // N96 清绑后普通形：三段守卫 && 汇聚（ORIG movzbl/test、xor$1/test、
    // call/xor$1/test 逐点 je→false 岛，尾 bool 归一 test;je;mov$1/mov$0）。
    if (m_battleField.m_pDungeon->m_bloodDungeonType == 0 &&
        m_battleField.m_pDungeon->m_villageAttackEventFlag == 0 &&
        m_battleField.m_pDungeon->isNoFatigueDungeon() == 0)
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
    // [b28d-51] 正向包裹（P222）：ORIG 0x085a8d38 为 `call; test %al,%al; je 尾部`
    // （4.4 对 bool callee 的 `!call` 恒加 xor$1 ⇒ 必须写正极 if 包体）。
    // 且 ORIG 每次用 slot 都重新 movsbl -0xc(%ebp)（无具名 int idx 局部，P0）。
    if (_checkValidUser(slot))
    {
        if (m_battleData.m_arr54[slot] != 1)                       // +0x380+slot
        {
            // [b28d-51] 反写（探针 z7）：ORIG 两条件 `xor$1;test;je <clear>` 同落
            // clear 岛、increment 落 fall-through ⇒ 源码须写 `==0 && ==0` 包 increment。
            if (m_battleData.m_arr58[slot] == 0 &&                 // +0x384+slot
                m_battleData.m_arr5c[slot] == 0)                   // +0x388+slot
            {
                ++bd(this).m_arr64[slot];                    // this+(slot+0xe0)*4+0x10
            }
            else
            {
                m_battleData.m_arr58[slot] = 0;
            }
        }
        m_battleData.m_arr5c[slot] = 0;

        if (canFatigueUp(info.m_field17))                    // movzbl 0x17(%eax)
            memberUser(this, slot)->FatigueUp(0);            // T 0x08655c60
    }
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

    // N96 清绑：ORIG 判空短路 + IsEnterEachMap 结果 mov$1/mov$0 物化岛
    // （%al 直落 test；4.4 恒折 test;jcc 无中转——P168a③b/N96 判词）。
    if (m_battleField.m_pDungeon != 0 &&
        m_battleField.m_pDungeon->IsEnterEachMap())
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
// §6.2 巨件核销：P167 33/33 [0缺0多]；gap 645→629（修前 611）；真修 1 处：
//   任务豁免三段各自独立 call getCurCharacQuestR（修前具名 UserQuest* 缓存
//   合并为 1 call → P167 MISSING 2，P48 反向实证）。
//   残差归因 N 类（详见 docs/asm_translation_patterns.md 负例表）：N82 全帧
//   槽复用差（0x6c vs 0x8c）、P183 反向 m_mapHitCount 重读 vs -0x50 缓存、
//   kill 的 -0x46 槽缓存 vs ORIG 逐次 movzwl 重读、N-88/setne 物化假象
//   （IsVisitedCurrentMap xor$1 vs setne）、P196 折向（info.m_39 movsbl vs
//   movzbl：槽位值域 0..3 无 ≥0x80 分叉，非真反）——语义与调用序列零缺口。
// ---------------------------------------------------------------------------
// [b28d-solo-CPartyDepsD2] gap 13→-1（645 vs 646），p167 33/33 CLEAR。本轮真修：
// ①[真 bug] 技能节奏阈值 q/rem 的基准错 1000 倍——旧码用原始 tick（playTime/5），
//   ORIG 0x085a88e0 读 -0x20 = m_playedTime/1000（0x085a8897 段写入）⇒ 改
//   `secQ = bd(this).m_field8/1000; q = secQ/5; rem = secQ - q*5`；边界比较
//   （rem==0 → c>q 记疑 / rem!=0 → c>=q）经汇编复核与原码一致，保留。
// ②P0/P329 去 lastMap/playTime 具名局部（MapClear 实参内联，ORIG 0x085a8591
//   `mov 0x4(%eax),%esi` 直解引用 + 每次重算 /1000）。③CDungeon::m_89c
//   uchar→bool（SweepCPartyDepsD2-2）后，本函数 0x085a876d 处 `m_89c != 1`
//   由 cmp 形转为 ORIG 的 `movzbl; xor$1; test; je`（指令 645→646）。残余：
//   帧 0x6c vs 0x8c（N82 槽复用差）+ 4.4 continue 中继岛（N-P223）。
//   语义/调用集齐，byte 层封顶。
bool CParty::checkMoveMapHackCnt(MSG_MOVE_MAP& info)
{
    // [b28d-solo-CPartyDepsD2] P171 死读照抄：ORIG 0x085a83ab 先 `mov 0x378` 到
    // -0x38，随即被 `mov 0x1ab8` 覆盖（同一槽）。
    int prevAux = bd(this).m_field4c;                    // -0x38（+0x378 死读）
    prevAux = m_field1ab8;                               // -0x38 覆盖
    m_field1ab8 = 0;
    bd(this).m_field4c = 0;                              // +0x378 dword 清零

    // 地图游玩时长：(u64)now - enterTick(+0x344/0x348) - baseTick(+0x33c/0x340)
    // → 存回 +0x334/+0x338（ORIG 读 getCurTickCount 后零扩 edx,eax 走双 sub/sbb）。
    bd(this).m_playedTime =
        (unsigned long long)GlobalData::s_systemTime_.getCurTickCount()
        - bd(this).m_enterTick - bd(this).m_baseTick;

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

        // 逐张进入型副本只统计座位匹配成员（ORIG 直接调 IsEnterEachMap，无空判）
        if (m_battleField.m_pDungeon->IsEnterEachMap() && (int)info.m_39 != i)
            continue;

        // cUserHistoryLog::MapClear(ushort,int,int) T 0x08684b22
        // 首参 = user+0x79700 日志门面；第 3 参 = *(int*)(lastMap+4)=m_mapIndex
        // [P0/P329] 无 lastMap/playTime 具名局部：ORIG 0x085a8591 直接
        // `mov 0x4(%eax),%esi`（GetLastMapInfo 返回值直解引用），0x085a8597
        // 每次重算 +0x334/1000（-0x4c 除法临时），均无槽往返。
        memberUser(this, i)->m_historyLog.MapClear(
            kill, (int)(bd(this).m_field8 / 1000),
            bf(this).GetLastMapInfo()->m_mapIndex);

        if (kill == 0)
        {   // 击杀数为 0 却换图 → 0x19b
            memberUser(this, i)->getHackAnalyzer()->addServerHackCnt(
                memberUser(this, i), (WongWork::ENUM_HACKTYPE)0x19b, 1, 0, 0);
        }

        // 时长/击杀比异常：quotient=kill/sec，>4 时上报 min(q-4,100) → 0x340
        const unsigned int sec = bd(this).m_field8 / 1000;
        if (sec != 0)
        {
            const unsigned int quotient = (unsigned int)kill / sec;
            if (4 < quotient)
            {
                unsigned int over = quotient - 4;        // -0x3c
                const unsigned int cap = 100;            // -0x40
                over = std::min<unsigned int>(over, cap);  // ORIG call _ZSt3minIjERKT_S2_S2_
                memberUser(this, i)->getHackAnalyzer()->addServerHackCnt(
                    memberUser(this, i), (WongWork::ENUM_HACKTYPE)0x340,
                    over, 0, 0);
            }
        }

        sumKill += kill;

        // 维度刷图频率检测前置条件（asm +0x3be..+0x40f 组合）
        // [b28d-round1-L24] P0/P183：ORIG 无 bool 槽（widthscan clr_b orig=0 vs
        // 旧码 3）⇒ 条件直接内联进 if，不设 dimTrack 具名局部（clr_b 3→2）。
        if (m_battleField.m_pDungeon != 0 && m_battleField.m_pDungeon->m_89c != 1 &&
            m_battleField.m_pDungeon->get_dimension_possible() == 0 &&
            prevAux != 0 && m_field1aa0 != 1)
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
            if (level >= 0x14 && m_battleData.m_arr54[i] != 0 && c != 0)
            {
                // [真 bug 修正] 基准是 m_playedTime/1000（ORIG 0x085a88e0 读 -0x20，
                // 由 0x085a8897 段 /1000 写入），不是原始 tick（旧码差 1000 倍）。
                const unsigned int secQ = bd(this).m_field8 / 1000;
                const unsigned int q = secQ / 5;         // mul 0xcccccccd shr 2
                const unsigned int rem = secQ - q * 5;   // sec%5
                if (rem == 0)
                    suspectBoundary = ((unsigned int)c > q);
                else
                    suspectBoundary = ((unsigned int)c >= q);
            }
        }

        if (!suspectBoundary)
        {
            // 任务豁免：(0x10d3,idx 0xb,diff 2)/(0x10d4,0xc,2)/(0x10d8,0xf,diff>0)
            bool exempt = false;
            // P48：ORIG 三段豁免各自独立 call getCurCharacQuestR（0 缓存），
            // 具名 UserQuest* 局部会合并为单次 call → P167 MISSING 2。
            if (memberUser(this, i)->getCurCharacQuestR()->IsDoingQuest(0x10d3) &&
                bf(this).get_dungeon_index() == 0xb &&
                bf(this).get_dungeon_diff() == 2)
                exempt = true;
            else if (memberUser(this, i)->getCurCharacQuestR()->IsDoingQuest(0x10d4) &&
                     bf(this).get_dungeon_index() == 0xc &&
                     bf(this).get_dungeon_diff() == 2)
                exempt = true;
            else if (memberUser(this, i)->getCurCharacQuestR()->IsDoingQuest(0x10d8) &&
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

        // 单人模式换图检测：msg.m_field35==0 且 IsSinglePlay → 0x1a0
        // （ORIG 0x85a8b8f `mov 0x35(%eax),%eax` 单 dword 直读）
        if (info.m_field35 == 0 && IsSinglePlay())
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
// [b28d-solo-CPartyDepsD2] gap 16→-7（454 vs 461），p167 真缺口归零（仅余
// C1/C2、D1/D2 同址别名 + 1 处 _Unwind_Resume，P93/N104）。本轮真修：
// ①[真缺口] trace 第 2 参由 `m_partyIndex` 直读改 out-of-line `GetPartyIndex()`
//   （ORIG 0x085b17d4 call，非 mov 0x64）；②[真缺口] GetDungeon 单次调用（旧码
//   `if(bf(this).GetDungeon()!=0){CDungeon* d=bf(this).GetDungeon();}` 调两次，
//   ORIG 0x085b1d04 单次存 -0x24）。形态（P0/P24/P227/P292）：首/次循环改
//   `if(!_checkValidUser) continue;` 否定早退、pcRoomTrace/user 具名局部删除、
//   两处 trace 改全表达式临时链（ORIG ctor 前求 varargs）、三处 key 内联实参、
//   acc-tick 表达式内联（LHS-first）、GetPCRoomNo 改空 if 体复现 `test;jmp`。
// 残余：帧 0x9c vs 0xac、flag 形参 4.1 cmpb 直比 vs 4.4 movzbl+xor（P209/N）、
// EH 尾块相对序（N104）、PCRoom trace 两 vararg 寄存器 esi↔ebx 互换（N7）。
void CParty::check_allmember_loading(bool flag)
{
    const int memberCount = get_member_count();
    if (memberCount == 0)
    {
        // ORIG 0x085b17d4 为 out-of-line `call CParty::GetPartyIndex()`（非 m_partyIndex 直读）。
        // P227 全表达式临时链：0x085b17d9 先 call GetPartyIndex 存 %ebx，再 ctor
        // （0x085b17f9），最后压参。
        cMyTrace(__PRETTY_FUNCTION__, 0x30e0, 0)(
            "<IN_DUNGEON_NULL_PARTY> not_member PartyIndex(%d)", GetPartyIndex());
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
        // P292/P133：ORIG 0x085b1881 `xor $1; test; jne`（bool callee 否定早退）；
        // 合并 `&&` 会多 setcc 物化。
        if (!_checkValidUser(i))
            continue;
        if (m_battleData.m_arr50[i])                     // +0x37c+i byte
            ++loadedCount;
    }
    if (loadedCount != memberCount)
    {
        // ORIG 0x085b18b8 `cmpb $0x0,-0x6c(%ebp); je`（4.1 形参 spill 后内存直比）；
        // 4.4 对 bool 形参恒 movzbl+xor 物化（P209/N，`==0` 写法实测同形）。
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
    // [b28d-solo-CPartyDepsD2] ORIG 0x085b195a..0x085b1ee 为 64 位
    // sub/sbb + add/adc 链（now 零扩展 64 位，具名 now/moveTick 槽 -0x80/-0x78）。
    // P0/P211：ORIG 0x085b1942 先读 acc(+0x358/0x35c) 再 call getCurTickCount
    // （LHS-first）⇒ 表达式内联，不设 now/moveTick 具名局部。
    bd(this).m_accTick += GlobalData::s_systemTime_.getCurTickCount()
                          - bd(this).m_moveTick;                 // +0x360/0x364
    // 地图基准 tick：base(+0x33c/0x340) = now - enterTick(+0x344/0x348)（无局部槽）。
    bd(this).m_baseTick = GlobalData::s_systemTime_.getCurTickCount()
                          - bd(this).m_enterTick;

    // PCRoom 统计上下文：非自动建队且人数>1 时开启追踪
    // P0：ORIG 0x085b19f9 直接 `movzbl %al,%ebx`（无 bool 槽）⇒ 内联实参。
    traceHack(this).traceBegin(GlobalData::s_systemTime_.getCurSec(),
                               !(IsAutoCreated() || get_member_count() == 1));

    for (int i = 0; i < 4; ++i)
    {
        if (!_checkValidUser(i))
            continue;
        // P0：ORIG 每点重算 m_member[i].m_pUser（无指针槽往返）。
        if (m_member[i].m_pUser->get_state() < 3 ||
            m_member[i].m_pUser->getCurCharacR() == 0)   // thunk 0x080da38c
        {
            // P227：ORIG 0x085b1ad4 NumberToString→%ebx、0x085b1af5 get_state→%esi
            // 均在 ctor（0x085b1b15）之前求值并跨 ctor 保活。
            cMyTrace(__PRETTY_FUNCTION__, 0x3123, 5)(
                "PCROOM : Dispatcher_RequestPCRoomPlayerCount => User State : %d, "
                "m_id : %s",
                m_member[i].m_pUser->get_state(),
                NumberToString(m_member[i].m_pUser->get_acc_id(), 0));  // T 0x0810904b
        }
        // ORIG 0x085b1b54 `call; test %eax,%eax; jmp`（4.1 空 if 体占位）。
        if (m_member[i].m_pUser->GetPCRoomNo())
        {
        }
    }

    // 开启反作弊采样
    for (int i = 0; i < 4; ++i)
    {
        // P292：ORIG 0x085b1b87 `xor $1; test; jne`（bool callee 否定早退）。
        if (!_checkValidUser(i))
            continue;
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
        // P24：ORIG 0x085b1c49/0x085b1c4e 把 gen_timer_key 结果留 %ebx、
        // m_partyIndex 留 %esi 跨 G_TimerQueue() call ⇒ 内联实参。
        G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)1, m_partyIndex,
                                    (TIMER_MESSAGE)0x28, 0xf,
                                    gen_timer_key((TIMER_MESSAGE)0x28), 0);
    }

    // 血票剩余时间提醒（BF+0x128=m_villageMonsterId 槽按 short 判非 0）
    if (bf(this).m_villageMonsterId != 0)
    {
        // P24：ORIG 0x085b1cb8/0x085b1cbd/0x085b1cc6 三值均跨 G_TimerQueue() 保活。
        G_TimerQueue()->InsertTimer(
            (TimerEntry::OBJ_TYPE)1, m_partyIndex, (TIMER_MESSAGE)0x27,
            bf(this).m_field134, gen_timer_key((TIMER_MESSAGE)0x27),
            (unsigned long long)(unsigned short)bf(this).m_villageMonsterId);
    }

    // 任务 NPC 副本且有 boss 房：EPLP 状态机切换 + 0xfe 包
    CDungeon* dungeon = bf(this).GetDungeon();           // ORIG 单次 call 存 -0x24
    if (dungeon != 0)
    {
        if (dungeon->getQuestNpcDungeon() && checkBossRoom())
        {
            SetEPLPState(2);
            // P24：ORIG 0x085b1d6e（key→%esi）/0x085b1d7b（index→%ebx）先于
            // G_TimerQueue() 求值。
            G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)1, GetPartyIndex(),
                                        (TIMER_MESSAGE)0x15, 600,
                                        gen_timer_key((TIMER_MESSAGE)0x15), 0);
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
// [b28d-solo-CPartyDepsD2] AE（242=242）：P183 去 user 指针缓存（逐点
// m_member[i].m_pUser 寻址）、CCharacBloodDungeon::GetEnterCount（include
// W9_EventSmall.h）、(signed char)limit/party_count、三路共享尾 return 0 ⇒
// 槽 -0x24/-0x20/-0x1c/-0x18/-0x14/-0x10/-0xc 与帧 0x34 全等。
int CParty::check_dungeon_start(CDungeon const* dungeon, int& param)
{
    if (dungeon->m_bloodDungeonType == 1)
    {   // 血之副本
        for (int i = 0; i < 4; ++i)
        {
            if (!_checkValidUser(i))
                continue;
            // [b28d-solo-CPartyDepsD2] P183：ORIG 逐点重算 m_member[i].m_pUser
            // 寻址（0x042/0x06b/0x0ae/0x106 四段完整链），无 user 指针槽。
            // 入场费 vs 现金（thunk 0x080da27e=getCurCharacInvenR）
            const int fee = get_blood_dungeon_admission_fee(m_member[i].m_pUser);
            if (m_member[i].m_pUser->getCurCharacInvenR()->get_money() < fee)
            {
                param = i;
                return 10;
            }
            // 等级下限
            if (m_member[i].m_pUser->get_charac_level() < dungeon->get_min_level())
            {
                param = i;
                return 0x5f;
            }
            // 进入次数上限（ORIG 0x0859f8f5 movsbl get_limit_Inout_count；
            // 0x0859f92a call CCharacBloodDungeon::GetEnterCount）
            const int limit = (signed char)dungeon->get_limit_Inout_count();
            const int enterCount = reinterpret_cast<CCharacBloodDungeon*>(
                m_member[i].m_pUser->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)1))
                ->GetEnterCount();
            if (limit <= enterCount)
                return 0xf5;
        }
    }
    else if (dungeon->m_bloodDungeonType == 2)
    {   // 特殊权限图（PowerWar/VillageAttack 等）
        // ORIG 0859f97a：get_index 结果作为实参传入 checkDugeonInoutTime
        if (!checkDugeonInoutTime(dungeon->get_index()))  // T 0x0859f6e2
            return 0xf4;
        if (GlobalData::s_power_manager->IsPowerWarEventOn())
            return 0xf6;
        if (GlobalData::s_villageMonsterMgr->OnEvent())
            return 0xf7;

        const int members = get_member_count();
        if ((signed char)dungeon->get_limit_party_count() < members)
            return 0xf3;

        for (int i = 0; i < 4; ++i)
        {
            if (!_checkValidUser(i))
                continue;
            if (m_member[i].m_pUser->get_charac_level() < dungeon->get_min_level())
            {
                param = i;
                return 0xf8;
            }
            // 入场券计数 = _Charac_info.m_ultimateInout[2]（+0xec2，CUserCharacInfo.h:441）
            // ORIG 0859fa8f `movzbl 0xec2(%eax); test %al,%al; jg` 无 ticket 槽
            // ⇒ 内联 (signed char) 表达式。
            if ((signed char)m_member[i].m_pUser->getCurCharacR()->m_ultimateInout[2] < 1 &&
                m_battleField.m_bloodState != 1)
            {
                param = i;
                return 0xf5;
            }
        }
    }
    else
    {
        // 普通副本：仅人数上限
        const int members = get_member_count();
        if ((signed char)dungeon->get_limit_party_count() < members)
            return 0xf3;
    }
    // ORIG 0x0859fb0a 共享尾 `mov $0x0,%eax`（血图/==2/else 三路汇入）
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
// 插入 TIMER_MESSAGE 0xe 定时器：m_battleField.m_villageMonsterId（CParty+0xc4c
// = BF+0x128）非 0 → 3 秒，否则 10 秒。ORIG 直读 this+0xc4c（无 bf() 调用）。
// ---------------------------------------------------------------------------
void CParty::send_dungeon_fail_msg()
{
    int seconds = 10;
    if (static_cast<unsigned short>(m_battleField.m_villageMonsterId) != 0)
        seconds = 3;

    // [b28d-51] P24：key 必须内联为实参（ORIG gen_timer_key 先于 m_partyIndex 装载、
    // 结果留 %ebx 跨 G_TimerQueue call），具名局部引入 -0xc 槽并致帧 0x34→0x30 反。
    G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)1, m_partyIndex,
                                (TIMER_MESSAGE)0xe, seconds,
                                gen_timer_key((TIMER_MESSAGE)0xe), 0);
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
// [b28d-solo-CPartyDepsD2] AE（62=62）：五槽照抄（char -0x19 / sum -0x18 /
// avg -0x14 / count -0x10 / i -0xc）+ P292 bool callee 裸 `if(!_checkValidUser)
// continue`（xor$1;test;jne）+ P223 岛形 + isGMUser 正极性（test;je 跳过 movb）
// + avg 具名 ⇒ 帧 0x38 全等。
bool CParty::setStandardDimensionLevel(CDungeon const* dungeon)
{
    (void)dungeon;
    // [b28d-solo-CPartyDepsD2] ORIG 0x0859f612 五槽照抄：char 死标志 -0x19
    // （isGMUser()!=0 时置 1，从不读取）；sum -0x18 / avg -0x14 / count -0x10 /
    // i -0x0c。ORIG `xor$1;test;jne 循环尾` ⇒ 源码为 `if(!_checkValidUser) continue;`
    // （P292 bool callee + P223 岛形）；isGMUser 正极性（test;je 跳过 movb）。
    char nonGM = 0;                                      // -0x19
    int levelSum = 0;                                    // -0x18
    int avg = 0;                                         // -0x14
    int validCount = 0;                                  // -0x10
    for (int i = 0; i < 4; ++i)                          // -0x0c
    {
        if (!_checkValidUser(i))
            continue;
        if (memberUser(this, i)->isGMUser())
            nonGM = 1;
        levelSum += memberUser(this, i)->get_charac_level();  // thunk 0x080da2b8
        ++validCount;
    }
    (void)nonGM;
    avg = levelSum / validCount;                         // cdq/idivl
    m_battleField.m_standardDimensionLevel = avg;
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
    // [待 CParty 具名化专改]（Main 裁决转 MonC9：m_padBattleData→BattleData
    // m_battleData 后 4.4 或折叠单段位移）。
    float value = bd(this).m_hellPartyValueTotal *
                  bf(this).m_hellPartyRatio4;              // flds×2 fmulp（交换律等价，复现 ORIG 压栈序）
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

    // [b28d-51] P0：ORIG 无具名 float 局部（flds 0xb0c; flds 0x8cbbbcc; fmulp;
    // fildl 0xd0c; fdivrp; fistpl 一气呵成）⇒ 表达式必须内联。
    const int out =
        D2_ROUND((bd(this).m_hellPartyValueTotal * 100.0f) /
                 static_cast<float>(bf(this).m_hellPartyRatio0));
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
//   有效成员按 flag（false=全部 / true=仅 m_battleData.m_arr54==1）收集到幸运点对象。
// ---------------------------------------------------------------------------
void CParty::GetLuckPoint(CLuckPoint* luckPoint, bool onlyAlive)
{
    if (luckPoint == 0)
        return;
    for (int i = 0; i < 4; ++i)
    {
        if (!_checkValidUser(i))
            continue;
        // [X2·技巧22 精化形] ORIG 0x085b61be O@17-22：`cmpb $0,onlyAlive;
        // je` 短路 + `movzbl liveFlag; xor$1,%eax; test; je` 真值取反判。
        // [b28d-51] P0/N96 清绑（原 `register uchar lf asm("eax")`）；arr54
        // 已 bool 化，`!arr54[i]` 即 ORIG `xor$1; test; je` 语义。
        if (onlyAlive && !m_battleData.m_arr54[i])
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
    // [switch+寄存器缓存] ORIG 0x085bd6de：`cmp $0x2e;je; cmp $0x2f;je; jmp`
    // 双 case 共享尾 + key 留 %ebx、m_partyIndex 留 %esi（call 前置装载）。
    switch (msg)
    {
    case (TIMER_MESSAGE)0x2e:
    {
        // [b28d-51] P24：key 内联实参（ORIG key 留 %ebx、m_partyIndex 留 %esi；
        // 具名 key 局部引入 -0x10/-0xc 双槽并使帧 0x20→0x30）。
        G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)1, m_partyIndex,
                                    msg, 0x28, (int)gen_timer_key(msg), 0);
        break;
    }
    case (TIMER_MESSAGE)0x2f:
    {
        G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)1, m_partyIndex,
                                    msg, 4, (int)gen_timer_key(msg), 0);
        break;
    }
    default:
        break;
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
// [b28d-solo-CPartyDepsD2] AE（91=91）：P221 全表达式临时链翻 sibling 块 trace 槽
// （ORIG -0x30/-0x20 vs 具名形 -0x24/-0x34）+ P24 gen_timer_key 内联实参（消 -0xc
// 槽，key 升格 %ebx 跨 G_TimerQueue call）。语义/调用集齐。
int CParty::VoteLackUser(CUser* user, int victimIdx)
{
    if (victimIdx < 0 || victimIdx > 3)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x3591, 5)("[%s][%d]", __PRETTY_FUNCTION__, 0x3591);
        return 0;
    }
    CUser* victim = get_user(victimIdx);
    if (user == victim || victim == 0)
        return 0;
    int voterSlot = (int)GetMemberSlotNo(user);
    if (voterSlot < 0 || voterSlot > 3)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x35a1, 5)("[%s][%d]", __PRETTY_FUNCTION__, 0x35a1);
        return 0;
    }
    m_election.Vote(voterSlot, victimIdx);
    G_TimerQueue()->InsertTimerInMilisecond(
        (TimerEntry::OBJ_TYPE)1, m_partyIndex, (TIMER_MESSAGE)0xf, (long long)0x9c4,
        (int)gen_timer_key((TIMER_MESSAGE)0xf), 0);
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
        // P184 死块照抄：ORIG +0x1e `cmpl $0; js → nop岛→return1`（4.1 物化
        // 头部冗余检查，4.4 -O0 同发射）。
        if (victimIdx < 0)
            break;
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
    // [b28d-51] ORIG 0x085b23e0 第三条件出 `test; setne%al; test; je` 物化，
    // 前两条为直跳 ⇒ 源码非单一 && 链：前两条件合并 if，第三条件独立嵌套 if
    // （probe g1 裸 if(call) 在 4.4 亦产 setne；&& 链 g5/g6/g7 恒折直跳）。
    if (m_member[slot].m_pUser != 0 && m_member[slot].m_field4 != 0)
    {
        if (reinterpret_cast<CUserCharacInfo*>(m_member[slot].m_pUser)
                ->getCurCharacR() != 0)
        {
            return reinterpret_cast<CUserCharacInfo*>(m_member[slot].m_pUser)
                ->getCurCharacName();
        }
    }
    return "";
}

// ---------------------------------------------------------------------------
// CParty::SetPlayResult —— ORIG T 0x085b2412（报告 SetPlayResult.md）：
//   结算一个成员：写排行/最佳时间、发送结算、清奖励、统计缺口告警。
// ---------------------------------------------------------------------------
// [b28d-solo-CPartyDepsD2] 判词 N（ORIG 0x085b2412，N80/N82 族）：
// 已修：①重复 get_dungeon_clear_state 删（ORIG 单次）；②m_field42→m_field48
// 真 bug（ORIG 0x085b251b 写 +0x48=0x183c，旧码写 +0x42 错位）；③resultSet
// 指针局部改 P235 具名视角（消 add $0x17f4）；④trace 改 P227 临时链；
// ⑤SendResult 返回 (unsigned char) 收窄复现 ORIG 0x085b2595 `movzbl %al,%eax`。
// 残余（193 vs 192，gap 1）：
//  ① ORIG 0x085b2457 `mov 0x330(%eax),%edx` 为**死读**（m_battleData.m_field4，
//     值随即被 0x085b245d 的 m_field0 装载覆盖、全函数无消费者）⇒ 4.1 对
//     dead local 保留 load 但不落槽；4.4 三形实测均不可造（探针
//     /tmp/probe_d2.cpp fA/fD 裸表达式+`(void)` 被 DCE；fB/fC 具名/volatile
//     局部多 store；/tmp/probe_d2b.cpp fK/fL volatile 取址多 `add $0x4`）⇒ 封 N。
//  ② SendResult 段 ORIG `lea 0x17f4(%eax),%edx` vs 4.4 `add $0x17f4,%eax`
//     （同址同指令数，union 内无 GameResultSet 完整类型视角可直访；P235 需改
//     CParty.h union 加非平凡成员，风险 > 收益）⇒ N。
// [b28d-SweepCPartyDepsD2-2] 真 bug 修复：trace 的 GetNoSend 实参序原写
// (3,2,1,0) ⇒ (%s,%s,%s,%s) 打印整体颠倒；ORIG 0x085b26a6 段证明 arg3..arg6
// = (0,1,2,3)（-0x64=(3)→arg6 / edi=(0)→arg3）。改源码为 (0,1,2,3) 后
// 4.4 右起求值产 call 3→2→1→0、槽 -0x64/-0x60/-0x5c/edi 与 ORIG 逐条一致
// （原判「varargs 求值序不可翻」作废——两侧同为 cdecl 右起，差异实为实参序）。
// 语义/调用集齐，byte 层封顶（仅余上述残余 ①②）。
// [b28d-round1-L24] **DIFF→AE**（193=193）：残余②的 dead 读真因 = 原码是 64 位
// union 成员的 int 截断 `(int)bd(this).m_clearPlayedTime`（探针 /tmp/probe_ll.cpp
// A 形：`mov 0x4(%eax),%edx; mov (%eax),%eax`，高半进 edx 后丢弃）；旧码
// `(int)bd(this).m_field0` 只读低半 ⇒ 少 1 条。改用 union 视角即逐条全等。
bool CParty::SetPlayResult(CUser* user)
{
    // [b28d-solo-CPartyDepsD2] ORIG 0x085b2412 单次 get_dungeon_clear_state +
    // `cmp$1;sete;test;je<共享尾>`（P222 正向包裹 + 尾 return 1）；resultSet
    // 无具名指针槽（P183：全点内联 this+0x17f4）；槽 -0x3c..-0x20 照抄。
    if (get_dungeon_clear_state() == 1)
    {
        int step = (int)set_play_result(user, m_gameResultSet);       // -0x3c
        // [b28d-solo-CPartyDepsD2] ORIG 0x085b2457 `mov 0x330(%eax),%edx;
        // mov 0x32c(%eax),%eax` = 64 位 union 整体装载后取低 32 位（P180 视角，
        // 探针 /tmp/probe_ll.cpp A 形）；写 m_field0 单半不会出该 edx 装载。
        const int clearTime = (int)bd(this).m_clearPlayedTime;        // -0x38
        const int dungeonIndex = (int)bf(this).get_dungeon_index();   // -0x34
        const int dungeonDiff = (int)bf(this).get_dungeon_diff();     // -0x30
        int avgTime = (int)G_CDataManager()->getDungeonClearAverageTime(
            dungeonIndex, getStartGamePartyCount(),
            (ENUM_DUNGEON_DIFFICULTY)dungeonDiff);                    // -0x2c
        const int prevField9 = (int)(signed char)m_gameResultSet.m_field9; // -0x28
        const int bonus = (int)G_CDataManager()->getClearTimeBonusPoint(
            avgTime, clearTime);                                      // -0x24
        const int rankPoint = G_CDataManager()->calRankPoint(
            bonus, prevField9);                                       // -0x20
        // [b28d-solo-CPartyDepsD2] 真 bug 修正：ORIG 0x085b251b 写 0x183c =
        // GameResultSet+0x48 = GameResultType::m_field48（旧 m_field42 落在
        // +0x42=0x1836，字段错位；clear_reward 读同一 +0x48）。P235 具名视角。
        m_gameResultSet.m_field48 =
            (unsigned short)G_CDataManager()->getRankIndex(rankPoint);
        m_gameResultSet.m_fieldc = (int)clearTime;
        m_gameResultSet.m_field10 = (unsigned char)bonus;
        if (!bf(this).isQuestMaze())
            setBestClearTime(dungeonIndex, dungeonDiff, clearTime);
        // ORIG 0x085b2595 `movzbl %al,%eax; mov %eax,-0x1c`（SendResult 单字节
        // 返回、int 接收）⇒ 调用点显式收窄（P3/探针 /tmp/p_movzbl.cpp 形 a）。
        int sendOk = (unsigned char)DisPatcher_SetPlayResult::SendResult(
            (CParty*)this, m_gameResultSet, step);
        if (sendOk == 0)
            return 0;
        clear_reward((int)m_gameResultSet.m_field48);
        set_dungeon_clear_state(2);
        insert_time_clear_dungeon_reward_msg(0x1e);
        if (get_member_count() != GetSetPlayResultCount())
        {
            // P227 全表达式临时链：ORIG 0x085b2612 求值序 GetNoSend(3/2/1/0)
            // 落槽 -0x64/-0x60/-0x5c/edi，压参 +0x1c/-0x18/+0x14/+0x10
            // = arg6/arg5/arg4/arg3 ⇒ 原码实参序为 (0),(1),(2),(3)
            // （4.1/4.4 同按 cdecl 右起求值）。旧码写 (3),(2),(1),(0) 是
            // **真 bug：(%s,%s,%s,%s) 打印顺序整体颠倒**（ORIG 0x085b26a6
            // 段：-0x64=(3)→arg6、edi=(0)→arg3）。
            cMyTrace(__PRETTY_FUNCTION__, 0x3261, 0)(
                "DUNGEON_CLEAR_BUG_CHECK total(%d),recv(%d), (%s,%s,%s,%s)",
                get_member_count(), GetSetPlayResultCount(),
                GetNoSendSetPlayResultUser(0), GetNoSendSetPlayResultUser(1),
                GetNoSendSetPlayResultUser(2), GetNoSendSetPlayResultUser(3));
        }
    }
    return 1;
}

// ---------------------------------------------------------------------------
// CParty::MakeQuestRewardPacket —— ORIG T 0x085bd7ae（报告
//   MakeQuestRewardPacket.md）：任务清图奖励物品包（20 任务槽 × 清图物品表）。
// ---------------------------------------------------------------------------
// [b28d-solo-CPartyDepsD2] gap 0→-1（295 vs 296），p167 40/40 CLEAR。本轮真修
// （三处 p167 真缺口）：①循环内取任务由 `getCurCharacQuestR`（18 字）改
//   `getCurCharacQuestRefR`（21 字，ORIG `_ZNK5CUser21getCurCharacQuestRefREv`）；
// ②去 dungeonIdx 缓存：ORIG 0x085bd878/0x085bd8e3 两次独立 call
//   get_dungeon_index（P183）；③`itemList[i]`→`itemList.at(i)`（ORIG 调
//   `_ZNSt6vectorISt4pairIiiESaIS1_EE2atEj`，P175；补发 at 实例，AE +2）。
// 既有修复保留：needCount 头部预清、itemList/pairValue 分步声明+拷贝赋值
// （N86 反向）、quest==0 死块照抄（P184）、m_itemList 成员直访。
// 残余：4.4 槽基恒 ebp-8 起 vs ORIG 大局部顶到 ebp ⇒ 全帧槽号 +8 平移
// （ORIG -0x89/-0x44/-0x4c vs 我方 -0x91/-0x4c/-0x54）；N95 定论（double
// framePad 反致下移、int pad/声明序不可上移）。语义/调用集齐，byte 层封顶。
void CParty::MakeQuestRewardPacket(PacketGuard* packet, CUser* user)
{
    int count = 0;
    int needCount = 0;   // ORIG 0x085bd7c0 头部预清（-0x24）
    int countIndex = packet->get_index();
    packet->put_byte(0);
    for (int q = 0; q < 0x14; ++q)
    {
        // [b28d-solo-CPartyDepsD2] P183：ORIG 0x085bd7f4 每轮重调
        // user->getCurCharacQuestR()（-0x1c 槽），无循环外缓存。
        // ORIG 0x085bd8xx 用 getCurCharacQuestRefR（21 字 mangled，与
        // checkMoveMapHackCnt 的 18 字 getCurCharacQuestR 不同重载）。
        UserQuest* questRef = user->getCurCharacQuestRefR();
        if (questRef->m_quest[q] == 0 || questRef->m_trigger[q] == 0)
            continue;
        Quest* quest = G_CDataManager()->find_quest(questRef->m_quest[q]);
        if (quest == 0)
            continue;
        // +0x75da = UserQuest.m_authenData(+0x75d8) + 2 = m_characClass（CUser.h:1220）
        short dungeonItemIdx = questRef->m_authenData.m_characClass;
        // P183：ORIG 0x085bd878/0x085bd8e3 两次独立 call get_dungeon_index
        //（无缓存槽，各自直传实参）。
        if (!quest->check_clear_item(bf(this).get_dungeon_index(),
                                     (int)dungeonItemIdx))
            continue;
        needCount = questRef->m_quest[q];
        if (needCount <= 0)
            continue;
        // ORIG 0x085bd8b8/-0x4c 循环外默认构造 + 拷贝赋值（N86 反向）；
        // 0x085bd8ce quest==0 死块照抄（P184）。
        std::vector<std::pair<int, int> > itemList;
        std::pair<int, int> pairValue;
        if (quest == 0)
            continue;
        itemList = quest->get_clear_itemlist(bf(this).get_dungeon_index());
        for (size_t i = 0; i < itemList.size(); ++i)
        {
            Inven_Item item;
            item.reset();
            // ORIG 0x085bd972 单次 vector::at（P175：ORIG 走 at 非 operator[]）
            // 后 pair 拷贝到 -0x4c/-0x48。
            pairValue = itemList.at(i);
            item.m_addInfo = pairValue.first;
            // ORIG 0x085bd98e `mov 0xc(%eax),%eax` 直接成员访问（无 getItemList call）
            G_CDataManager()->m_itemList->create_item(
                ENUM_ITEM_CREATE_TYPE_1, item, pairValue.second);
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
// [b28d-solo-CPartyDepsD2] gap -1→0（369=369），p167 真缺口归零（仅余 C1/C2、
// D1/D2 同址别名，P93）。本轮真修：①[真缺口] 两处 InsertTimer 的 index 实参由
// `m_partyIndex` 直读改 out-of-line `GetPartyIndex()`（ORIG 0x085aa90d/0x085aaa49
// call）；②[真缺口] 两循环增量 `++it`→`it++` 复现 ORIG ppEi（P28）。
// 形态（P24/P171/P227）：两处 key 内联实参、trace 改全表达式临时链、
// 删两处 `m_pDungeon != 0` 过度防御（ORIG 0x085aa9d7/0x085aaa9b 直接解引用
// 0xcac，无判空）。残余：map_monster 0x34c 临时 vs dieBuf 0xa3f 槽复用方向差
// （ORIG 帧 0xac0；4.4 不复用 ⇒ 0xe10，N82/N86）。语义/调用集齐，byte 层封顶。
// ---------------------------------------------------------------------------
int CParty::OnTimeoutBossDie(int monsterId, unsigned int characNo)
{
    if (bf(this).m_apcAllKilledSec != monsterId)
        return 0;
    CUser* user = GetMemberSlotNo(characNo);
    if (user == 0)
        return 0;
    if (m_battleField.m_pDungeon != 0 && m_battleField.m_pDungeon->IsEnterEachMap())
        bf(this).SetCurSeatNo(get_party_seatno(user));
    // [b28d-solo-CPartyDepsD2] ORIG 0x085aa5d4 语义照抄（Ghidra C 逐调用还原）：
    // ①第一循环：GetMonsterMap()->begin()/end()（每轮重调），map_monster 临时
    //   赋值 it->second，m_dropCnt(+0x2c)!=0 时 push_back(m_instId(+0x04))；
    // ②第二循环：vector 迭代器（非索引），每轮 memset(dieBuf,0,0xa3f)；
    //   空体 if 内真语义 = mit->second.m_active.getHpMax() 死写 -0x8e（P171）；
    // ③dieFlag(-0x9) 头部预清、成功后置 1（死变量照抄）。
    std::vector<int> mobIds;
    for (std::map<int, map_monster>::iterator it = bf(this).GetMonsterMap()->begin();
         it != bf(this).GetMonsterMap()->end(); it++)   // ORIG ppEi（P28 后置增量）
    {
        map_monster mob;
        mob = it->second;
        if (mob.m_dropCnt != 0)
            mobIds.push_back(mob.m_instId);
    }
    char dieBuf[0xa3f];
    char dieFlag = 0;
    for (std::vector<int>::iterator it = mobIds.begin(); it != mobIds.end(); it++)
    {
        memset(dieBuf, 0, sizeof(dieBuf));
        std::map<int, map_monster>::iterator mit = bf(this).GetMonsterMap()->find(*it);
        if (mit != bf(this).GetMonsterMap()->end())
        {
            const int hpMax = mit->second.m_active.getHpMax();
            (void)hpMax;
        }
        if (kill_monster(user, *it, 0xffff,
                         *reinterpret_cast<const MSG_MONSTER_DIE*>(dieBuf), 0) != 1)
        {
            // P227：ORIG 0x085aa886 先算 *it 存 %ebx，再 ctor（0x085aa8b1）。
            cMyTrace(__PRETTY_FUNCTION__, 0x22ce, 0)(
                "KILLBOSS2 server - monster id(%d) failed", *it);
            if (m_battleField.m_bloodState == 1)
            {
                SetEPLPState(2);
                // P24：ORIG 0x085aa905 key→%esi、0x085aa912 index→%ebx 跨 G_TimerQueue()。
                G_TimerQueue()->InsertTimer(
                    (TimerEntry::OBJ_TYPE)1, GetPartyIndex(), (TIMER_MESSAGE)0x15,
                    0x3c, (int)(unsigned int)gen_timer_key((TIMER_MESSAGE)0x15), 0);
                set_state(1);
                PacketGuard packet;
                packet.put_header(0, 0xfe);
                packet.finalize(true);
                send_to_party(packet);
            }
            else if (m_battleField.m_pDungeon->m_bloodDungeonType < 1)
            {
                PacketGuard packet;
                packet.put_header(0, 0x1f);
                packet.finalize(true);
                send_to_party(packet);
            }
            else
            {
                bf(this).m_bloodReward.onFinishBloodRound(
                    true, (short)bf(this).m_bloodRound.getBloodRoundOrder(),
                    this, m_battleField.m_pDungeon);
                G_TimerQueue()->InsertTimer(
                    (TimerEntry::OBJ_TYPE)1, GetPartyIndex(), (TIMER_MESSAGE)0x22,
                    5, (int)(unsigned int)gen_timer_key((TIMER_MESSAGE)0x22), 0x22);
            }
            break;
        }
        dieFlag = 1;
    }
    (void)dieFlag;
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
    // [b28d-51] P171 照抄：ORIG 无 proxy 判空（GetServerProxy 结果直接作 receiver，
    // 单表达式链 + &packet 留 %ebx 跨 call）；具名 proxy 局部引入 -0xc 槽、帧 +4。
    GlobalData::s_statistic_proxy_mgr->GetServerProxy((ENUM_SERVER_GROUP)0)
        ->SendPacket((char*)&packet, 0x13);
}

// ---------------------------------------------------------------------------
// CParty::historyBloodRoundClear —— ORIG T 0x085b7db6：逐有效成员写
//   cUserHistoryLog::BloodRoundClear（CUser+0x79700）。
// ---------------------------------------------------------------------------
// [b28d-solo-CPartyDepsD2] AE（72=72）：真 bug 修正 char[0x40]→char[0x100]
// （P116/P253：ORIG rep stos 的 %edx=0x40 是 dword 计数 = 0x100 字节，旧码少清
// 0xc0 字节且 getMemberNames 写 256B 溢出）+ P0 去 cUserHistoryLog 指针缓存
// （lea 0x79700(%eax),%edx 直压参）⇒ 帧 0x130 全等。
void CParty::historyBloodRoundClear(int round, unsigned int clearTime,
                                    unsigned int playTime, const char* dungeonName)
{
    // [b28d-solo-CPartyDepsD2] P116/P253：ORIG 0x085b7dce rep stos 的 %edx=0x40 是
    // dword 计数 ⇒ 缓冲实为 0x100 字节（旧 char[0x40] 少清 0xc0 字节且溢出）。
    // P0：cUserHistoryLog 指针 ORIG 无槽往返（lea 0x79700(%eax),%edx 直压参）
    // ⇒ 内联成员访问，删具名 log 局部。
    char memberNames[0x100] = { '\0' };
    getMemberNames(memberNames);
    for (int slot = 0; slot < 4; ++slot)
    {
        if (get_user(slot) == 0 || !checkValidUser(slot))
            continue;
        m_member[slot].m_pUser->m_historyLog.BloodRoundClear(
            round, clearTime, playTime, dungeonName, memberNames);
    }
}

// ---------------------------------------------------------------------------
// CParty::cal_hellparty_value —— ORIG T 0x085b58ac（报告 cal_hellparty_value.md）：
//   地狱值累计（上限钳制 + send_hellParty_value）。
//   +0xd0c..+0xd5b（m_padcee 未建模 hell 值表区）按报告偏移访问。
// ---------------------------------------------------------------------------
// [b28d-solo-CPartyDepsD2] 判词 NEAR（83=83，gap 1→0；p167 26/26 CLEAR）：
// 已修 P183 去 bf 引用缓存、P252 三数组具名 float 局部、P243 乘序、成员直存
// （fstps 0xb0c 无 value 槽）、比较反向。原「m_89c!=1 的 xor 形不可造」N 判词
// 已作废：CDungeon::m_89c uchar→bool（SweepCPartyDepsD2-2，值域 {0,1} 直证：
// STDungeonScript.cpp:1963 ScanInt((bool*)0)!=0 → CDungeon.cpp:457 拷贝）后，
// 源形 `!m_89c` 与 ORIG 0x085b5915 `movzbl; xor$1; test%al; je` 同形 ⇒
// DIFF→NEAR。残余：三处数组寻址常量拆分（ORIG `add $0x348; mov
// 0x14(%eax,%edx,4)` vs 4.4 `add $0x34c; mov 0x4(...)`），指令数相同，仅常量
// 折叠形差（N94/N4）。
void CParty::cal_hellparty_value(int dungeonDiff, int rateLevelDiff, int memberCount)
{
    if (rateLevelDiff == 0 || memberCount < 0 || dungeonDiff >= 5)
        return;
    // [b28d-solo-CPartyDepsD2] P183 去 base 缓存 + P252 具名 float 局部复现槽往返：
    // ORIG 0x085b58d0..0x085b593d 三数组值先落 -0x1c/-0x18/-0x14，base -0x10，
    // old -0xc；x87 序 `fildl ratio3; fmuls dungeonRate; flds mapRate; fld1;
    // faddp; fadds base; fadds itemRate; fmulp; fadds old` ⇒ 源形
    // `(float)ratio3 * dungeonRate * (mapRate + 1.0f + base + itemRate) + old`。
    // ORIG 偏移映射（报告 cal_hellparty_value.md，BF = this+0xb24）：
    //   +0xd58 = BF+0x234 m_hellPartyScriptRate（float）
    //   +0xd34+d*4 = BF+0x210 m_hellPartyMapRate[d]
    //   +0xd48+m*4 = BF+0x224 m_hellPartyItemRate[m]
    //   +0xd18 = BF+0x1f4 m_hellPartyRatio3（int）
    //   +0xd24+r*4 = BF+0x200 m_hellPartyDungeonRate[r]
    //   +0xd0c = BF+0x1e8 m_hellPartyRatio0（int 上限）
    //   +0xb0c = bd.m_hellPartyValueTotal
    float mapRate = bf(this).m_hellPartyMapRate[dungeonDiff];
    float dungeonRate = bf(this).m_hellPartyDungeonRate[rateLevelDiff];
    float itemRate = bf(this).m_hellPartyItemRate[memberCount];
    float baseValue = bf(this).m_hellPartyScriptRate;
    // [b28d-SweepCPartyDepsD2-2] ORIG 0x085b591e 为 `movzbl; xor$1; test%al; je`
    // ⇒ P248 档② bool 字段直证：CDungeon::m_89c 已改 bool（CDungeon.h），
    // 源形写 `!m_89c` 即复现（探针 /tmp/probe_d2.cpp fE/fF/fG；uchar 的 `!=1`
    // 与显式 `(x^1)!=0` 四形恒产 `cmp $1`）。
    if (!m_battleField.m_pDungeon->m_89c)
        baseValue = 0.0f;
    float old = bd(this).m_hellPartyValueTotal;
    // ORIG 0x085b5964 fstps 0xb0c(%eax) 直存（无 value 槽）；ROUND 段重读成员。
    bd(this).m_hellPartyValueTotal =
        (float)bf(this).m_hellPartyRatio3 * dungeonRate *
            (mapRate + 1.0f + baseValue + itemRate) +
        old;
    if ((int)bd(this).m_hellPartyValueTotal > bf(this).m_hellPartyRatio0)
        bd(this).m_hellPartyValueTotal = (float)bf(this).m_hellPartyRatio0;
    send_hellParty_value();
}

// ---------------------------------------------------------------------------
// CParty::get_rate_level_difference —— ORIG T 0x085b59ca（报告
//   get_rate_level_difference.md）：等级差档位（0/1/2/3）。
// ---------------------------------------------------------------------------
int CParty::get_rate_level_difference()
{
    if (m_battleField.m_pDungeon == 0)
        return 0;
    // [b28d-51] ORIG 0x085b5a07 用 `sar $0x1f/xor/sub` 位技巧取绝对值（4.1 对
    // __builtin_abs 的形）；`if (diff<0) diff=-diff;` 在 4.4 恒产 negl 分支形。
    const int diff = __builtin_abs(
        m_battleField.m_pDungeon->get_standard_level() - getMemberMaxLevel());
    // [b28d-51] 块序（N93附）反写：ORIG 0x085b5a14 段 11..19→2 落 fall-through、
    // 0..10→1 与 20+→3/0 各为岛 ⇒ 必须写成「11..19 显式区间 + 20+ 显式早返」形
    // （探针 /tmp/b28d_51_probe_abs3.cpp f7 逐条全等；f4/f5/f8/f9 均块序反）。
    if (diff < 0xb)
        return 1;
    if (diff > 0xa && diff <= 0x13)
        return 2;
    if (diff >= 0x14)
        return 3;
    return 0;
}

// ---------------------------------------------------------------------------
// TOD_UserState::ClearLayer() —— ORIG T 0x0864390e：层推进 + 脏标记 + 清层时间。
// ---------------------------------------------------------------------------
void TOD_UserState::ClearLayer()
{
    reinterpret_cast<charac_expand::CData*>(this)->alter();
    // [b23S3C] S23A.Sec TOD_UserState→内嵌 m_data（SIG_REQUEST_TOD_USER_STATE）
    // 迁移遗漏点修复（本 TU 编译红→绿；字段偏移经头 316-319 注释实证）。
    reinterpret_cast<TOD_Layer*>(&m_data.m_todayLayer)->increaseLayer();
    m_data.m_lastClearTime = GlobalData::s_systemTime_.getCurSec();
}

// ---------------------------------------------------------------------------
// TowerOfDespair_APC_Mgr::Get_APC_Name_byIndex —— ORIG T 0x085ff020：
//   映射命中返回 SendingAPC_Info::get_user_charac_name()，未命中返回 0。
// [b28d-solo-CPartyDepsD2] 判词 N93（N86 族，NEAR 封顶）：P50 复活先声明后赋值
// （`iterator it;` 独立 C1Ev）+ 非 const iterator（P96）后 39=39、助记符序列全等，
// 唯 it/e 双槽方向相反——ORIG 4.1 先声明深（it=-0x10/e=-0xc），4.4 先声明浅
// （it=-0xc/e=-0x10）；交换声明序槽翻正但 C1Ev/end 发射序反（DIFF），两难不可兼得
// ⇒ N93 类对象双槽方向封顶。p167 调用集齐（C1Ev/end/find/ne/ptEv/get_user_charac_name）。
// ---------------------------------------------------------------------------
const char* TowerOfDespair_APC_Mgr::Get_APC_Name_byIndex(const TOD_Layer& layer)
{
    std::map<TOD_Layer, SendingAPC_Info>::iterator it;   // P50 复活：独立 C1Ev
    std::map<TOD_Layer, SendingAPC_Info>::iterator e = m_APCInfoMap.end();
    it = m_APCInfoMap.find(layer);
    if (it != e)
        return it->second.get_user_charac_name();
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
// [b28d-SweepCPartyDepsD2-2] 真 bug 修复（虚调用槽取错，Main 全库排查判据命中）：
//   ORIG 0x085a9b6e `add $0x8,%edx` = vptr+0x08 = _ZTVN8WongWork14CItemGeneratorE
//   word[4] @0x08c9a798 = 0x0853a400 = CItemGenerator::generateRandomItem
//   （Ghidra C `(**(code **)(*piVar14 + 8))(piVar14, ref, result)`；同 vtable
//   word[5]=vptr+0x0c=0x0853a406=generateSpecificItem）。旧码调
//   generateSpecificItem ⇒ 运行时选错虚函数。改后本 TU 唯一 `call *` 槽号
//   与 ORIG 逐条一致（本 TU 全量 grep 仅此 1 处间接调用）。
// [b28d-solo-CPartyDepsD2] 判词 N82（1058 vs 1060，§6.2 三件套闭合）：已修
// P167 缺口 ①getManager() 由具名缓存改 4 处逐点重调（P183）②m_partyIndex 改
// GetPartyIndex() 3 处（0x2c/0x22/0x15 timer）③itemList[i].second 改
// item.m_addInfo2 字段直读（消 get_add_info call）④push_back 改传 m_todPos
// 引用（消 pair<int&,int&> 临时 + push_back(&&) 重载）。p167 149/149 CLEAR
// （0 缺 0 多）；残余：帧 0x170 vs 0x1d0 全帧槽平移 + 多分支槽复用方向差
// （N82 巨件族）。语义/调用集齐，byte 层封顶。
// [b28d-round1-L24] 本轮真修（1058→1055→1037，ORIG 1054；gap +4→+1→-17）：
// ①**块序反转**（P222/P272）：`if (m_bloodState == 1) { A; return; } B` —— ORIG
//   0x085a9356 `cmp $1; jne <B(+0x1b0)>` 且 A 尾 `jmp <epilogue>`（0x85a94bd）
//   ⇒ then=A 直落、B 在后；旧码写 `if (!= 1) { B }` 使 4.4 反向。
// ②**64 位 union 直算**（P180/P246）：ORIG 0x085a94e0..0x085a9525 无具名局部，
//   `sub/sbb` 直对 +0x20/+0x24 与 +0x2c/+0x30、双字直存 +0x32c/+0x330 ⇒ 改用
//   BattleData.h 新增 `m_clearPlayedTime`/`m_clearEnterTime` 视角（旧手写
//   `<<32|` 拼接产 40 条 sar/or 展开，净 -39）。
// ③**P183 去 CUser* 缓存**：ORIG 0x085a9589/0x85a95e0/... 六个调用点各自
//   重算 `m_member[slot].m_pUser`（9 条 stride 0x18 寻址）⇒ 净 +38。块序交换
//   （blood 分支 `>= 1` 前置）实测零 codegen 影响（4.4 if/else 两臂同形）。
// 残余（-17）：tower 大段（ORIG[318:782]）与我方发射位置差 ⇒ difflib 大段错位；
//   p167 149/149 CLEAR（0 缺 0 多）；帧 0x170 vs 0x1bc（我方多 push %edi + 槽集大）
//   ⇒ N82/N95 巨件族。语义/调用集齐。
void CParty::ClearDungeon()
{
    if (m_tournamentVictory != 0)
        return;
    if (m_battleField.m_bloodState == 1)
    {
        for (int slot = 0; slot < 4; ++slot)
        {
            if (!_checkValidUser(slot))
                continue;
            OnSuccessedDungeonClear(m_member[slot].m_pUser);
        }
        SetEPLPState(2);
        unsigned int key = (unsigned int)gen_timer_key((TIMER_MESSAGE)0x15);
        G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)1, GetPartyIndex(),
                                    (TIMER_MESSAGE)0x15, 0x3c, (int)key, 0);
        set_state(1);
        PacketGuard packet;
        packet.put_header(0, 0xfe);
        packet.finalize(true);
        send_to_party(packet);
        m_battleField.m_dungeonClearTracer.Trace("void CParty::ClearDungeon()");
        m_tournamentVictory = 1;
        return;
    }
    // [b28d-solo-CPartyDepsD2] ORIG 0x085a94e0..0x085a9525：无 now/enterTick/
    // baseTick/elapsed 具名局部（P0）——`sub/sbb` 直对 64 位字段运算、
    // 结果 `mov %eax,0x32c(%ecx); mov %edx,0x330(%ecx)` 双字直存。
    // 已改 BattleData.h 64 位 union 视角（探针 /tmp/probe_ll.cpp D 形）。
    bd(this).m_clearPlayedTime =
        (unsigned long long)GlobalData::s_systemTime_.getCurTickCount()
        - bd(this).m_clearEnterTime - bd(this).m_accTick;
    int memberCount = get_member_count();
    if (m_battleField.m_pDungeon != 0)
    {
        for (int slot = 0; slot < 4; ++slot)
        {
            if (!_checkValidUser(slot))
                continue;
            // [b28d-solo-CPartyDepsD2] P183：ORIG 0x085a9589/0x85a95e0/... 每处调用点
            // 各自重算 `m_member[slot].m_pUser`（9 条 stride 0x18 寻址），无 CUser* 缓存。
            if (m_battleField.m_pDungeon->m_villageAttackEventFlag == 0)
            {
                if (m_battleField.m_pDungeon->get_index() == 100)
                    APSystem::CUserProc::ClearActionAndSendtoUser(
                        m_member[slot].m_pUser, (APSystem::_ActionGroupIndex)0x19, 0,
                        (ENUM_PACKETCLASS)0);
                else if (m_battleField.m_pDungeon->m_89c == 0)
                {
                    if (m_battleField.m_pDungeon->get_dimension_possible() > 0)
                        APSystem::CUserProc::ClearActionAndSendtoUser(
                        m_member[slot].m_pUser, (APSystem::_ActionGroupIndex)0x1c, 0,
                        (ENUM_PACKETCLASS)0);
                }
                else
                    APSystem::CUserProc::ClearActionAndSendtoUser(
                        m_member[slot].m_pUser, (APSystem::_ActionGroupIndex)0x1b, 0,
                        (ENUM_PACKETCLASS)0);
            }
            else
                APSystem::CUserProc::ClearActionAndSendtoUser(
                        m_member[slot].m_pUser, (APSystem::_ActionGroupIndex)0x18, 0,
                        (ENUM_PACKETCLASS)0);
            if (!m_member[slot].m_pUser->IsUsedCoinInDungeon())
                APSystem::CUserProc::ClearActionAndSendtoUser(
                        m_member[slot].m_pUser, (APSystem::_ActionGroupIndex)0xf, 0,
                        (ENUM_PACKETCLASS)0);
        }
    }
    PacketGuard packet;
    if (m_battleField.m_pDungeon->m_bloodDungeonType >= 1)
    {
        bf(this).m_bloodReward.onFinishBloodRound(
            true, (short)bf(this).m_bloodRound.getBloodRoundOrder(), this, m_battleField.m_pDungeon);
        unsigned int key = (unsigned int)gen_timer_key((TIMER_MESSAGE)0x22);
        G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)1, GetPartyIndex(),
                                    (TIMER_MESSAGE)0x22, 5, (int)key, 0x22);
    }
    else
    {
        if (!m_battleField.m_pDungeon->isTowerOfDespairDungeon())
        {
            if (!m_battleField.m_pDungeon->isTournamentDungeon())
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
            // [b28d-solo-CPartyDepsD2] P183：ORIG 0x085a93xx.. 每次重调
            // getManager()（4 处 call），无具名 manager 缓存。
            TOD_UserState* todState = reinterpret_cast<TOD_UserState*>(
                getManager()->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)0xd));
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
                    const char* myName = getManager()->getCurCharacName();
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
                // [b28d-SweepCPartyDepsD2-2] 真 bug 修复：ORIG 0x085a9b6e
                // `add $0x8,%edx` = vptr+0x08 = _ZTVN8WongWork14CItemGeneratorE
                // word[4] @0x08c9a798 = 0x0853a400 =
                // CItemGenerator::generateRandomItem（Ghidra C
                // `(**(code **)(*piVar14 + 8))(piVar14, ref, result)`）；
                // 旧码调 generateSpecificItem（vptr+0x0c，word[5]）⇒ 运行时选错虚函数。
                genMgr->getGenerator((WongWork::CItemGeneratorMgr::eGenerateType_t)2)
                    ->generateRandomItem(ref, result);
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
                    // ORIG 1315 段直接读字段 +0x07（m_addInfo2），无 get_add_info call
                    itemList[i].second = item.m_addInfo2;
            }
            bf(this).m_todReward.m_todMap = itemList;
            if (bf(this).m_todReward.m_todPos.first != -1)
                itemList.push_back(bf(this).m_todReward.m_todPos);
            std::vector<int> slots;
            int putCount = (int)getManager()->getCurCharacInvenW()->insert_event_items(
                itemList, slots, (eMoneyAddReason)0x1f, (eItemAddReason)0x37, true,
                "game_server_msg_191", "game_server_msg_192");
            if (putCount >= 1 && slots.size() >= 3)
            {
                for (int i = 0; i < putCount; ++i)
                {
                    unsigned int slot = (unsigned int)slots[i * 3];
                    getManager()->SendUpdateItem((CUser::eSendTarget)1,
                                                (ENUM_ITEMSPACE)0, (int)slot);
                }
            }
            unsigned int key = (unsigned int)gen_timer_key((TIMER_MESSAGE)0x2c);
            G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)1, GetPartyIndex(),
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
        // [b28d-solo-CPartyDepsD2] ORIG 0x85aa027 段对 +0x32c/+0x330 直接做
        // 64 位 __divdi3（`mov 0x330(%eax),%edx` 单次高半装载，无 sar/or 拼接）
        // ⇒ 用 BattleData.h 的 64 位 union 视角（P180）。
        user->getCurCharacQuestW()->set_authen_data(
            (QUEST_CONDITION)2,
            (int)((long long)bd(this).m_clearPlayedTime / 1000),
            0);
        if (m_battleField.m_pDungeon->m_bloodDungeonType == 0 &&
            !m_battleField.m_pDungeon->isTowerOfDespairDungeon() &&
            !m_battleField.m_pDungeon->isTournamentDungeon())
            user->Send(packet);
        secretshop::SECRET_SHOP_DATA* shopData = &m_secretShop;
        if (shopData->IsOpen() != 0)
        {
            int level = m_battleField.m_pDungeon->get_standard_level();
            int dungeonIdx = m_battleField.m_pDungeon->get_index();
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
    m_battleField.m_dungeonClearTracer.Trace("void CParty::ClearDungeon()");
    m_tournamentVictory = 1;
    QuickParty::RewardData_Param rewardParam(this);
    QuickParty::RewardData_Result rewardResult;
    set_random_reward((QuickParty::RandomBuffType)10, rewardParam, rewardResult);
    rewardResult.clear();
    return;
}

// ---------------------------------------------------------------------------
// CParty::makeBestClearTimePacket —— ORIG T 0x085be1f8：成员击杀榜时长包
//   （isQuestMaze / 成员数 / 每成员 uid+bestTime+newRecord）。
// ---------------------------------------------------------------------------
void CParty::makeBestClearTimePacket(PacketGuard& packet, int clearTime)
{
    packet.put_byte(bf(this).isQuestMaze() ? 1 : 0);
    packet.put_byte(get_member_count());
    int bestTime = 0;
    bool isNewRecord = false;
    for (int slot = 0; slot < 4; ++slot)
    {
        if (!checkValidUser(slot))
            continue;
        bestTime = reinterpret_cast<BestClearTime*>(
            m_member[slot].m_pUser->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)0x12))
                       ->getBestClearTime(bf(this).get_dungeon_index(),
                                          bf(this).get_dungeon_diff());
        isNewRecord = reinterpret_cast<BestClearTime*>(
            m_member[slot].m_pUser->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)0x12))
                          ->checkNewRecord();
        if (bestTime == 0)
            bestTime = clearTime;
        // ORIG +0x140: put_short(get_unique_id & 0xffff)，随后 put_int(bestTime) / put_byte(newRecord)
        packet.put_short((int)(m_member[slot].m_pUser->get_unique_id() & 0xffff));
        packet.put_int(bestTime);
        packet.put_byte(isNewRecord ? 1 : 0);
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
