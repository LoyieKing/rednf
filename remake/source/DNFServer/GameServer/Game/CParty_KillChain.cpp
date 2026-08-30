// ============================================================================
// CParty_KillChain.cpp —— CParty 域击杀/奖励链第三阶依赖（LinkKillChain 批次）。
//
// 覆盖 10 个 ORIG 真实符号（全部经 docs/class_func_reports/CParty/ 对应报告
// ## 汇编 + ## 反编译 C 完整读取后忠实转写，无桩）：
//   MemberRegisterKillMonster      T 0x085bae6e（MemberRegisterKillMonster.md）
//   _makeRewardItemInfo            T 0x085ad0be（_makeRewardItemInfo.md）
//   DungeonPermission              T 0x085b12f8（DungeonPermission.md）
//   calDeadUserDungeonPenalty      T 0x085ad9b4（calDeadUserDungeonPenalty.md）
//   existSamePCRoomUser            T 0x085b67ca（existSamePCRoomUser.md）
//   increase_result_card_gold      T 0x0859ba98（increase_result_card_gold.md）
//   hasPCRoomPlayer                T 0x085b4a70（hasPCRoomPlayer.md；链上依赖）
//   GetPartyMemberForceDropFlag    T 0x085bdc00（GetPartyMemberForceDropFlag.md；依赖）
//   addDungeonClear                T 0x085bddfc（addDungeonClear.md；DungeonPermission 依赖）
//   getDungeonTypeKind             T 0x085bdd2a（getDungeonTypeKind.md；依赖）
//
// 成员槽布局（ORIG 实证）：m_member[i] @ this+i*0x18+0x78，m_pUser 在槽首；
// m_field14(+0x8c) 为成员副本次数槽。每日入场次数记录数组位于 this+0x1808
// （每成员 8 字节：short uniqueId / int 计数，addDungeonClear 实证），处于
// CParty.h 占位区，按裸偏移访问。m_dungeonIdx 为 this+0xcb8（同上）。
// ============================================================================

#include <string.h>

#include <set>
#include <vector>

#include "QuickParty.h"        // 必须先于 CDungeon.h/CParty.h（守卫惯例）
#include "CParty.h"
#include "CDataManager.h"      // G_CDataManager/find_dungeon/getUltimateDungeonList
#include "CDungeon.h"          // get_min_level/get_index/get_dimension_possible/…
#include "CBattle_Field.h"     // bf 子对象视图
#include "CUser.h"             // CUser/CUserCharacInfo
#include "CInventory.h"        // tryInsertItemIntoInventory
#include "WarField.h"          // map_monster 完整定义
#include "Inven_Item.h"
#include "InterfacePacketBuf.h"
#include "LogManager.h"
#include "CQuickPartyRewardManager.h"  // QuickParty::RewardData_Param/Result

// CKillMonsterInfo 六谓词（定义见 CKillMonsterInfo_chain.cpp，ORIG mangled 符号；
// 本 TU 按 ORIG MemberRegisterKillMonster 调用形态引用）。
bool KmiCheckBossKillShim(void* self, unsigned int mobIdx, unsigned char diff)
    asm("_ZN16CKillMonsterInfo13CheckBossKillEjh");
bool KmiRegisterBossKillShim(void* self, unsigned int mobIdx, unsigned char diff)
    asm("_ZN16CKillMonsterInfo16RegisterBossKillEjh");
bool KmiCheckAPCBossKillShim(void* self, unsigned int mobIdx, unsigned char diff)
    asm("_ZN16CKillMonsterInfo16CheckAPCBossKillEjh");
bool KmiRegisterAPCBossKillShim(void* self, unsigned int mobIdx,
                                unsigned char diff)
    asm("_ZN16CKillMonsterInfo19RegisterAPCBossKillEjh");
bool KmiCheckNamedKillShim(void* self, unsigned int mobIdx, unsigned char diff)
    asm("_ZN16CKillMonsterInfo14CheckNamedKillEjh");
bool KmiRegisterNamedKillShim(void* self, unsigned int mobIdx,
                              unsigned char diff)
    asm("_ZN16CKillMonsterInfo17RegisterNamedKillEjh");

namespace
{

// bf 子对象视图（与 CPartyLink2.cpp 同惯例）。
inline CBattle_Field& kcbf(CParty* p)
{
    return *reinterpret_cast<CBattle_Field*>(&p->m_padBattleField1);
}

}  // namespace

// ============================================================================
// hasPCRoomPlayer —— ORIG T 0x085b4a70。
// 任一有效成员持有 0xc 号 premium → true。
// ============================================================================
bool CParty::hasPCRoomPlayer()
{
    for (int i = 0; i <= 3; ++i)
    {
        if (_checkValidUser(i)
            && m_member[i].m_pUser->isAffectedPremium(
                static_cast<ENUM_PREMIUM_TYPE>(0xc)))
            return true;
    }
    return false;
}

// ============================================================================
// GetPartyMemberForceDropFlag —— ORIG T 0x085bdc00。
// 任一成员 get_user(i)!=0 且 isGMUser 且 getForceDropFlag → 1。
// ============================================================================
int CParty::GetPartyMemberForceDropFlag()
{
    for (int i = 0; i <= 3; ++i)
    {
        CUser* user = get_user(i);
        if (user != 0 && user->isGMUser() && user->getForceDropFlag())
            return 1;
    }
    return 0;
}

// ============================================================================
// existSamePCRoomUser —— ORIG T 0x085b67ca。
// 统计 PC 房号==pcbangNo 且非 0 的有效成员数，>=2 → true。
// ============================================================================
bool CParty::existSamePCRoomUser(unsigned int pcbangNo)
{
    int count = 0;
    for (int i = 0; i <= 3; ++i)
    {
        if (!_checkValidUser(i))
            continue;
        unsigned int roomNo =
            static_cast<unsigned int>(m_member[i].m_pUser->GetPCRoomNo());
        if (roomNo == pcbangNo && roomNo != 0)
        {
            count += 1;
            if (count > 1)
                return true;
        }
    }
    return false;
}

// ============================================================================
// calDeadUserDungeonPenalty —— ORIG T 0x085ad9b4。
// slot<0 → 0；memberCount(m_field9b8)>1 时 rate = float(记录[slot])/float(count-1)，
// ORIG 语义：rate == 0.3f（.rodata 0x08cbbbe8 = 0.3f）→ 归零（fucompp+test
// $0x45+sete 严格相等比较，非 <）。死亡计数表基址 this+0x10+(slot+0xe0)*4
// （ORIG 寻址实证），位于 BattleData 占位区，按裸偏移访问。
// ============================================================================
float CParty::calDeadUserDungeonPenalty(int slot)
{
    float rate = 0.0f;
    if (slot < 0)
        return rate;

    const int memberCount = *reinterpret_cast<int*>(
        reinterpret_cast<char*>(this) + 0x9b8);
    if (memberCount > 1)
    {
        const int deadCnt = *reinterpret_cast<int*>(
            reinterpret_cast<char*>(this) + 0x10 + (slot + 0xe0) * 4);
        rate =
            static_cast<float>(deadCnt) / static_cast<float>(memberCount - 1);
        if (rate == 0.3f)
            rate = 0.0f;
    }
    return rate;
}

// ============================================================================
// _makeRewardItemInfo —— ORIG T 0x085ad0be。
// item+0x3f（调用方结构扩展标志，越界字段按裸偏移访问）：-1 → 归零并清尾随
// addInfo；为 0 → 包内 put_byte(1)；否则尝试入包（tryInsertItemIntoInventory
// 槽位<0 视为失败回退 put_byte(1)，成功 put_byte(2)）。尾部统一
// put_int(0)/put_int(item.add_info())，标志非 0 时再补写标志与尾随 addInfo。
// 尾随 addInfo 宿主为 item+0x3d 处的 Inven_Item 视图（set_add_info 写其 +0x07）。
// ============================================================================
void CParty::_makeRewardItemInfo(CUser const* user, Inven_Item* item,
                                 PacketGuard& packet)
{
    char* itemBlob = reinterpret_cast<char*>(item);
    int& extFlag = *reinterpret_cast<int*>(itemBlob + 0x3f);
    Inven_Item* tailItem = reinterpret_cast<Inven_Item*>(itemBlob + 0x3d);

    if (extFlag == -1)
    {
        extFlag = 0;
        tailItem->set_add_info(0);
    }

    if (extFlag == 0)
    {
        packet.put_byte(1);
    }
    else
    {
        const CInventory* inven =
            const_cast<CUser*>(user)->getCurCharacInvenR();
        const int slot = inven->tryInsertItemIntoInventory(*item);
        if (slot < 0)
        {
            packet.put_byte(1);
            extFlag = 0;
            tailItem->set_add_info(0);
        }
        else
        {
            packet.put_byte(2);
        }
    }

    packet.put_int(0);
    packet.put_int(item->get_add_info());
    if (extFlag != 0)
    {
        packet.put_int(extFlag);
        packet.put_int(tailItem->get_add_info());
    }
}

// ============================================================================
// increase_result_card_gold —— ORIG T 0x0859ba98。
// 遍历 result.m_dropItems 与成员槽双游标：checkValidUser(i) 且迭代器未尽时，
// 以 RewardCardGold(type=0xb) 结算该成员金卡加成并回写 item.m_addInfo2(+0x07)；
// 每次消费一个结果项（迭代器独立于成员循环推进）。
// ============================================================================
void CParty::increase_result_card_gold(WongWork::stGenerateResult_t& result)
{
    std::vector<Inven_Item>::iterator it = result.m_dropItems.begin();
    for (int i = 0; i <= 3; ++i)
    {
        if (!checkValidUser(i))
            continue;
        if (it == result.m_dropItems.end())
            continue;

        QuickParty::RewardData_Param param(this);
        param.m_param4 = kcbf(this).get_dungeon_index();
        QuickParty::RewardData_Result res;
        res.m_value00 = it->m_addInfo2;   // item+0x07 金卡金额
        set_basic_reward(QuickParty::BASIC_REWARD_CARD_GOLD, param, res);
        it->m_addInfo2 = res.m_value00;
        ++it;
    }
}

// ============================================================================
// MemberRegisterKillMonster —— ORIG T 0x085bae6e。
// 组队首杀同步：roleType==3（Boss）/8（APC Boss）/命名怪三分支，各遍历成员
// 1..3（跳过队长），对每成员 CKillMonsterInfo 先 Check 后 Register。
// ============================================================================
void CParty::MemberRegisterKillMonster(map_monster& monster, unsigned char flag)
{
    const unsigned int mobIdx = static_cast<unsigned int>(monster.m_mobIndex);

    if (monster.m_roleType == 3)
    {
        for (int i = 1; i <= 3; ++i)
        {
            if (!checkValidUser(i))
                continue;
            void* data = get_user(i)->GetCharacExpandData(
                static_cast<ENUM_CHARAC_EXPAND_TYPE>(0xc));
            if (!KmiCheckBossKillShim(data, mobIdx, flag))
                KmiRegisterBossKillShim(data, mobIdx, flag);
        }
    }
    else if (monster.m_roleType == 8)
    {
        for (int i = 1; i <= 3; ++i)
        {
            if (!checkValidUser(i))
                continue;
            void* data = get_user(i)->GetCharacExpandData(
                static_cast<ENUM_CHARAC_EXPAND_TYPE>(0xc));
            if (!KmiCheckAPCBossKillShim(data, mobIdx, flag))
                KmiRegisterAPCBossKillShim(data, mobIdx, flag);
        }
    }
    else if (monster.isNamedMonster())
    {
        for (int i = 1; i <= 3; ++i)
        {
            if (!checkValidUser(i))
                continue;
            void* data = get_user(i)->GetCharacExpandData(
                static_cast<ENUM_CHARAC_EXPAND_TYPE>(0xc));
            if (!KmiCheckNamedKillShim(data, mobIdx, flag))
                KmiRegisterNamedKillShim(data, mobIdx, flag);
        }
    }
}

// ============================================================================
// getDungeonTypeKind —— ORIG T 0x085bdd2a。
// 空=8；脚本 boss 图(dungeon+0x89c)!=0 → 1；维度可入 → 2；kind==1 → 3；
// 血轮 type1/type2 → 4/5；绝望塔 → 6；锦标赛 → 7；其余 0。
// ============================================================================
int CParty::getDungeonTypeKind(CDungeon const* dungeon)
{
    if (dungeon == 0)
        return 8;
    if (*reinterpret_cast<const signed char*>(
            reinterpret_cast<const char*>(dungeon) + 0x89c)
        != 0)
        return 1;
    if (dungeon->get_dimension_possible() >= 1)
        return 2;
    if (dungeon->getDungeonKind() == 1)
        return 3;
    if (dungeon->get_blood_dungeon_type() == 1)
        return 4;
    if (dungeon->get_blood_dungeon_type() == 2)
        return 5;
    if (dungeon->isTowerOfDespairDungeon())
        return 6;
    if (dungeon->isTournamentDungeon())
        return 7;
    return 0;
}

// ============================================================================
// addDungeonClear —— ORIG T 0x085bddfc。
// 按 param(slot) 成员的 uniqueId 匹配记录槽（this+0x1808+i*8：short uniqueId /
// int 计数）；kind==1 上限 30、kind==2 上限 50、kind==0 上限 10（param 与
// dm+0x512e/0x512c 当日门槛双判定），且成员 m_field14(+0x8c)==0 时
// AddDungeonClear(this+0xcb8 dungeonIdx, 3)。其余 kind 或未命中返回 false。
// ============================================================================
bool CParty::addDungeonClear(const CDungeon* dungeon, int param, int slot)
{
    const short uniqueId =
        static_cast<short>(m_member[slot].m_pUser->get_unique_id());
    const int dungeonIdx = *reinterpret_cast<int*>(
        reinterpret_cast<char*>(this) + 0xcb8);

    for (int i = 0; i <= 3; ++i)
    {
        char* record = reinterpret_cast<char*>(this) + 0x1808 + i * 8;
        if (*reinterpret_cast<short*>(record) != uniqueId)
            continue;

        const int kind = getDungeonTypeKind(dungeon);
        const int usedCount = *reinterpret_cast<int*>(record + 4);
        if (kind == 1)
        {
            if (param < static_cast<int>(*reinterpret_cast<
                    unsigned short*>(reinterpret_cast<char*>(G_CDataManager())
                    + 0x512e))
                || usedCount > 0x1e || m_member[slot].m_field14 != 0)
                                continue;
        }
        else if (kind == 2)
        {
            if (param < static_cast<int>(*reinterpret_cast<
                    unsigned short*>(reinterpret_cast<char*>(G_CDataManager())
                    + 0x512e))
                || usedCount > 0x32 || m_member[slot].m_field14 != 0)
                                continue;
        }
        else if (kind == 0)
        {
            if (param < static_cast<int>(*reinterpret_cast<
                    unsigned short*>(reinterpret_cast<char*>(G_CDataManager())
                    + 0x512c))
                || usedCount > 10 || m_member[slot].m_field14 != 0)
                                continue;
        }
        else
        {
            return false;
        }

        return m_member[slot].m_pUser->AddDungeonClear(
            *reinterpret_cast<int*>(reinterpret_cast<char*>(this) + 0xcb8),
            3);
        (void)dungeonIdx;
    }
    return false;
}

// ============================================================================
// DungeonPermission —— ORIG T 0x085b12f8。
// 副本准入结算：逐成员校验等级下限后按当前难度分支记账（diff0→普通 1 次/
// diff1→dm+0x5130 门槛下 2 次/diff2→ultimate 清单内 addDungeonClear），
// 记账成功即发 0x05 准入通知包（short: 1 / dungeonIdx / diff+1）。
// ============================================================================
void CParty::DungeonPermission(int param)
{
    const int dungeonIdx = *reinterpret_cast<int*>(
        reinterpret_cast<char*>(this) + 0xcb8);
    CDungeon* dungeon = G_CDataManager()->find_dungeon(dungeonIdx);
    if (dungeon == 0)
        return;

    const int minLevel = dungeon->get_min_level();
    for (int i = 0; i <= 3; ++i)
    {
        bool granted = false;
        if (!_checkValidUser(i))
            continue;
        CUser* user = m_member[i].m_pUser;
        if (minLevel > static_cast<int>(user->get_charac_level()))
            continue;

        const int diff = kcbf(this).get_dungeon_diff();
        if (diff == 1)
        {
            const unsigned short limit = *reinterpret_cast<unsigned short*>(
                reinterpret_cast<char*>(G_CDataManager()) + 0x5130);
            if (static_cast<int>(limit) <= param)
                granted = user->AddDungeonClear(dungeonIdx, 2);
        }
        else if (diff == 2)
        {
            const std::set<int>* ultimate =
                G_CDataManager()->getUltimateDungeonList();
            if (ultimate->find(dungeon->get_index()) != ultimate->end())
                granted = addDungeonClear(dungeon, param, i);
        }
        else if (diff == 0)
        {
            granted = user->AddDungeonClear(dungeonIdx, 1);
        }

        if (granted)
        {
            PacketGuard packet;
            packet.put_header(0, 5);
            packet.put_short(1);
            packet.put_short(dungeonIdx);
            packet.put_short(kcbf(this).get_dungeon_diff() + 1);
            packet.finalize(true);
            user->Send(packet);
        }
    }
}
